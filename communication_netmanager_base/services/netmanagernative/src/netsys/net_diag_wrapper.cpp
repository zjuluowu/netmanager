/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "net_diag_wrapper.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"

#include <algorithm>
#include <iomanip>
#include <pthread.h>
#include <sstream>
#include <thread>

namespace OHOS {
namespace nmd {
namespace {
using namespace NetManagerStandard;
constexpr int32_t TIME_MS_TO_SEC = 1000;
constexpr int32_t PING_MAX_DURATION_TIME = 30;
constexpr int32_t PING_HEADER_MATCH_SIZE = 5;
constexpr int32_t PING_ICMP_SEQ_MATCH_SIZE = 6;
constexpr int32_t PING_STATISTICS_MATCH_SIZE = 4;
constexpr int32_t NETSTAT_NET_PROTOCOL_MATCH_SIZE = 10;
constexpr int32_t NETSTAT_UNIX_MATCH_SIZE = 8;
constexpr int32_t NETSTAT_ROUTE_TABLE_MATCH_SIZE = 9;
constexpr int32_t IFCONFIG_NAME_INFO_MATCH_SIZE = 6;
constexpr int32_t IFCONFIG_INET_INFO_MATCH_SIZE = 4;
constexpr int32_t IFCONFIG_INET6_INFO_MATCH_SIZE = 3;
constexpr int32_t IFCONFIG_MTU_MATCH_SIZE = 4;
constexpr int32_t IFCONFIG_TX_QUEUE_LEN_MATCH_SIZE = 2;
constexpr int32_t IFCONFIG_TRANS_BYTES_MATCH_SIZE = 3;

constexpr const char *PING_CMD_PATH = "/system/bin/ping";
constexpr const char *NETSTAT_CMD_PATH = "/system/bin/netstat";
constexpr const char *IFCONFIG_CMD_PATH = "/system/bin/ifconfig";
constexpr const char *PING_THREAD_NAME = "NetDiagPingThread";

constexpr const char *OPTION_SPACE = " ";

constexpr const char *PING_OPTION_IPV4 = "-4";
constexpr const char *PING_OPTION_IPV6 = "-6";
constexpr const char *PING_OPTION_SOURCE = "-I";
constexpr const char *PING_OPTION_INTERVAL = "-i";
constexpr const char *PING_OPTION_COUNT = "-c";
constexpr const char *PING_OPTION_SIZE = "-s";
constexpr const char *PING_OPTION_FLOOD = "-f";
constexpr const char *PING_OPTION_TTL = "-t";
constexpr const char *PING_OPTION_MARK = "-m";
constexpr const char *PING_OPTION_TIMEOUT = "-W";
constexpr const char *PING_OPTION_DURATION = "-w";

constexpr const char *NETSTAT_OPTION_ROUTE_TABLE = "-re";
constexpr const char *NETSTAT_OPTION_ALL_SOCKETS = "-ae";
constexpr const char *NETSTAT_OPTION_TCP_SOCKETS = "-atep";
constexpr const char *NETSTAT_OPTION_UDP_SOCKETS = "-auep";
constexpr const char *NETSTAT_OPTION_RAW_SOCKETS = "-arep";
constexpr const char *NETSTAT_OPTION_UNIX_SOCKETS = "-axe";

constexpr const char *IFCONFIG_OPTION_ALL_IFACE = "-a";
constexpr const char *IFCONFIG_OPTION_ADD_IPV6 = "add";
constexpr const char *IFCONFIG_OPTION_DEL_IPV6 = "del";
constexpr const char *IFCONFIG_OPTION_DEL_IPV4 = "default";
constexpr const char *IFCONFIG_OPTION_SET_IPV4_MASK = "netmask";
constexpr const char *IFCONFIG_OPTION_SET_IPV4_BCAST = "broadcast";
constexpr const char *IFCONFIG_OPTION_SET_MTU_LEN = "mtu";
constexpr const char *IFCONFIG_OPTION_SET_TX_QUEUE_LEN = "txqueuelen";
constexpr const char *IFCONFIG_OPTION_IFACE_UP = "up";
constexpr const char *IFCONFIG_OPTION_IFACE_DOWN = "down";

constexpr const char *PING_NAME_DOES_NOT_RESOLVED = "Name does not resolve";
constexpr const char *PING_NETWORK_UNREACHABLE = "Network unreachable";
} // namespace

NetDiagWrapper::NetDiagWrapper() {}

__attribute__((no_sanitize("cfi"))) NetDiagWrapper::~NetDiagWrapper() {}

int32_t NetDiagWrapper::PingHost(const NetDiagPingOption &pingOption, const sptr<INetDiagCallback> &callback)
{
    NETNATIVE_LOGI("Generate ping command: ");
    std::string command;
    int32_t ret = GeneratePingCommand(pingOption, command);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    auto wrapper = shared_from_this();
    std::thread pingThread([wrapper, command, callback]() {
        if (wrapper == nullptr) {
            NETNATIVE_LOGE("wrapper is nullptr");
            return;
        }
        std::string result;
        if (wrapper->ExecuteCommandForResult(command, result) != NETMANAGER_SUCCESS) {
            return;
        }
        if (result.empty()) {
            NETNATIVE_LOGE("Ping result is empty");
            return;
        }
        wrapper->ExtractPingResult(result, callback);
    });
    pthread_setname_np(pingThread.native_handle(), PING_THREAD_NAME);
    pingThread.detach();
    return NETMANAGER_SUCCESS;
}

int32_t NetDiagWrapper::GetRouteTable(std::list<NetDiagRouteTable> &routeTables)
{
    std::string command = std::string(NETSTAT_CMD_PATH) + OPTION_SPACE + NETSTAT_OPTION_ROUTE_TABLE;
    std::string result;
    int32_t ret = ExecuteCommandForResult(command, result);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    std::regex routeRegex(R"(([^\s]+)\s+([^\s]+)\s+([^\s]+)\s+([^\s]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+([^\s]+))");
    std::istringstream inStream(result);
    std::string line;
    while (std::getline(inStream, line)) {
        std::smatch match;
        if (!std::regex_search(line, match, routeRegex)) {
            continue;
        }
        ExtractRouteTableInfo(match, routeTables);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetDiagWrapper::GetSocketsInfo(NetDiagProtocolType socketType, NetDiagSocketsInfo &socketsInfo)
{
    std::string command = std::string(NETSTAT_CMD_PATH) + OPTION_SPACE;
    switch (socketType) {
        case PROTOCOL_TYPE_ALL:
            command = command + NETSTAT_OPTION_ALL_SOCKETS;
            break;
        case PROTOCOL_TYPE_TCP:
            command = command + NETSTAT_OPTION_TCP_SOCKETS;
            break;
        case PROTOCOL_TYPE_UDP:
            command = command + NETSTAT_OPTION_UDP_SOCKETS;
            break;
        case PROTOCOL_TYPE_UNIX:
            command = command + NETSTAT_OPTION_UNIX_SOCKETS;
            break;
        case PROTOCOL_TYPE_RAW:
            command = command + NETSTAT_OPTION_RAW_SOCKETS;
            break;
        default:
            NETNATIVE_LOGE("Unknown protocol type: %{public}d", socketType);
            return NETMANAGER_ERR_INTERNAL;
    }
    std::string result;
    int32_t ret = ExecuteCommandForResult(command, result);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    std::regex netProtoRegex(
        R"(([^\s]+)\s+(\d+)\s+(\d+)\s+([^\s]+)\s+([^\s]+)\s*([^\s]*)\s+([^\s]+)\s+(\d+)\s+([^\s]+))");
    std::regex unixRegex(R"(([^\s]+)\s+(\d+)\s+\[\s*([^\s]*)\s+\]\s+([^\s]+)\s*([^\s]*)\s+(\d+)\s*([^\s]*))");
    std::istringstream inStream(result);
    std::string line;
    while (std::getline(inStream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, netProtoRegex)) {
            ExtractNetProtoSocketsInfo(match, socketsInfo);
            continue;
        }

        if (std::regex_search(line, match, unixRegex)) {
            ExtractUnixSocketsInfo(match, socketsInfo);
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetDiagWrapper::GetInterfaceConfig(std::list<NetDiagIfaceConfig> &configs, const std::string &ifaceName)
{
    std::string command = std::string(IFCONFIG_CMD_PATH) + OPTION_SPACE;
    command = command + (ifaceName.empty() ? IFCONFIG_OPTION_ALL_IFACE : ifaceName);
    std::string result;
    int32_t ret = ExecuteCommandForResult(command, result);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    std::regex nameRegex(R"(([^\s]+)\s+Link encap:([^\s]+)\s+HWaddr\s+([^\s]+)|([^\s]+)\s+Link encap:(.*))");
    std::regex inetRegex(R"(inet addr:([^\s]+)\s+(?:Bcast:([^\s]+)\s+)?(?:Mask:([^\s]+))?)");
    std::regex inet6Regex(R"(inet6 addr:\s+([^\s]+)\s+Scope:\s+([^\s]+))");
    std::regex mtuRegex(R"((UP)?(.*?)MTU:(\d+))");
    std::regex txQueueLenRegex(R"(txqueuelen:(\d+))");
    std::regex bytesRegex(R"(RX bytes:(\d+)\s+TX bytes:(\d+))");
    NetDiagIfaceConfig config;
    std::istringstream inStream(result);
    for (std::string line; std::getline(inStream, line);) {
        std::smatch match;
        if (IsBlankLine(line)) {
            configs.push_back(config);
            config.Initialize();
            continue;
        }
        if (std::regex_search(line, match, nameRegex)) {
            ExtractIfaceName(match, config);
            continue;
        }
        if (std::regex_search(line, match, inetRegex)) {
            ExtractIfaceInet(match, config);
            continue;
        }
        if (std::regex_search(line, match, inet6Regex)) {
            ExtractIfaceInet6(match, config);
            continue;
        }
        if (std::regex_search(line, match, mtuRegex)) {
            ExtractIfaceMtu(match, config);
            continue;
        }
        if (std::regex_search(line, match, txQueueLenRegex)) {
            ExtractIfaceTxQueueLen(match, config);
            continue;
        }
        if (std::regex_search(line, match, bytesRegex)) {
            ExtractIfaceTransDataBytes(match, config);
            continue;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetDiagWrapper::UpdateInterfaceConfig(const NetDiagIfaceConfig &config, const std::string &ifaceName, bool add)
{
    std::string command = std::string(IFCONFIG_CMD_PATH) + OPTION_SPACE + ifaceName + OPTION_SPACE;
    if (add) {
        if (!config.ipv4Addr_.empty()) {
            command = command + config.ipv4Addr_ + OPTION_SPACE;
        }
        for (const auto &ipv6Addr : config.ipv6Addrs_) {
            if (ipv6Addr.first.empty()) {
                continue;
            }
            command = command + IFCONFIG_OPTION_ADD_IPV6 + ipv6Addr.first + OPTION_SPACE;
        }
        if (!config.ipv4Bcast_.empty()) {
            command = command + IFCONFIG_OPTION_SET_IPV4_BCAST + config.ipv4Bcast_ + OPTION_SPACE;
        }
        if (!config.ipv4Mask_.empty()) {
            command = command + IFCONFIG_OPTION_SET_IPV4_MASK + config.ipv4Mask_ + OPTION_SPACE;
        }
        if (config.mtu_) {
            command = command + IFCONFIG_OPTION_SET_MTU_LEN + std::to_string(config.mtu_) + OPTION_SPACE;
        }
        if (config.txQueueLen_) {
            command = command + IFCONFIG_OPTION_SET_TX_QUEUE_LEN + std::to_string(config.txQueueLen_) + OPTION_SPACE;
        }
    } else {
        if (!config.ipv4Addr_.empty()) {
            command = command + IFCONFIG_OPTION_DEL_IPV4 + OPTION_SPACE;
        }
        for (const auto &ipv6Addr : config.ipv6Addrs_) {
            if (ipv6Addr.first.empty()) {
                continue;
            }
            command = command + IFCONFIG_OPTION_DEL_IPV6 + ipv6Addr.first + OPTION_SPACE;
        }
    }
    std::string result;
    return ExecuteCommandForResult(command, result);
}

int32_t NetDiagWrapper::SetInterfaceActiveState(const std::string &ifaceName, bool up)
{
    std::string command = std::string(IFCONFIG_CMD_PATH) + OPTION_SPACE + ifaceName + OPTION_SPACE;
    command = command + (up ? IFCONFIG_OPTION_IFACE_UP : IFCONFIG_OPTION_IFACE_DOWN);
    std::string result;
    return ExecuteCommandForResult(command, result);
}

int32_t NetDiagWrapper::ExecuteCommandForResult(const std::string &command, std::string &result)
{
    if (command.empty()) {
        NETNATIVE_LOGE("ping command is empty.");
        return NETMANAGER_ERR_INTERNAL;
    }
    std::string().swap(result);
    if (CommonUtils::ForkExec(command, &result) == NETMANAGER_ERROR) {
        NETNATIVE_LOGE("Execute command:[%{public}s] failed", command.c_str());
        return NETMANAGER_ERR_INTERNAL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetDiagWrapper::GeneratePingCommand(const NetDiagPingOption &pingOption, std::string &command)
{
    if (pingOption.destination_.empty()) {
        NETNATIVE_LOGE("Ping destination is empty.");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    std::string().swap(command);
    command = command + PING_CMD_PATH + OPTION_SPACE;
    command = command + ((pingOption.forceType_ == FORCE_TYPE_IPV6) ? PING_OPTION_IPV6 : PING_OPTION_IPV4);
    command = command + OPTION_SPACE;
    if (!pingOption.source_.empty()) {
        command = command + PING_OPTION_SOURCE + OPTION_SPACE + pingOption.source_ + OPTION_SPACE;
    }
    if (pingOption.flood_) {
        command = command + PING_OPTION_FLOOD + OPTION_SPACE;
    }
    if (pingOption.count_) {
        command = command + PING_OPTION_COUNT + OPTION_SPACE + std::to_string(pingOption.count_) + OPTION_SPACE;
    }
    if (pingOption.interval_) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << (static_cast<float>(pingOption.interval_) / TIME_MS_TO_SEC);
        command = command + PING_OPTION_INTERVAL + OPTION_SPACE + oss.str() + OPTION_SPACE;
    }
    if (pingOption.mark_) {
        command = command + PING_OPTION_MARK + OPTION_SPACE + std::to_string(pingOption.mark_) + OPTION_SPACE;
    }
    if (pingOption.dataSize_) {
        command = command + PING_OPTION_SIZE + OPTION_SPACE + std::to_string(pingOption.dataSize_) + OPTION_SPACE;
    }
    if (pingOption.ttl_) {
        command = command + PING_OPTION_TTL + OPTION_SPACE + std::to_string(pingOption.ttl_) + OPTION_SPACE;
    }
    if (pingOption.timeOut_) {
        command = command + PING_OPTION_TIMEOUT + OPTION_SPACE + std::to_string(pingOption.timeOut_) + OPTION_SPACE;
    }

    uint32_t duration = (pingOption.duration_ != 0 && pingOption.duration_ < PING_MAX_DURATION_TIME)
                            ? pingOption.duration_
                            : PING_MAX_DURATION_TIME;
    command = command + PING_OPTION_DURATION + OPTION_SPACE + std::to_string(duration) + OPTION_SPACE;
    command = command + pingOption.destination_;
    return NETMANAGER_SUCCESS;
}

bool NetDiagWrapper::IsBlankLine(const std::string &line)
{
    std::string trimmed = line;
    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), [](unsigned char chr) { return std::isspace(chr); }),
                  trimmed.end());
    return trimmed.empty();
}

void NetDiagWrapper::ExtractPingResult(const std::string &result, const sptr<INetDiagCallback> &callback)
{
    if (callback == nullptr) {
        NETNATIVE_LOGE("PingHost callback is nullptr");
        return;
    }
    std::regex headerRegex(R"(Ping\s+([^(\s]+)\s+\(([^)]+)\):\s+(\d+)\((\d+)\)\s+bytes)");
    std::regex icmpSeqRegex(R"((\d+)\s+bytes\s+from\s+([^\s]+)\:\s+icmp_seq=(\d+)\s+ttl=(\d+)\s+time=(\d+)\s+ms)");
    std::regex statisticsRegex(R"((\d+)\s+packets\s+transmitted,\s+(\d+)\s+received,\s+(\d+)\%\s+packet loss)");

    NetDiagPingResult pingResult;
    std::istringstream inStream(result);
    std::string line;
    while (std::getline(inStream, line)) {
        if (line.find(PING_NAME_DOES_NOT_RESOLVED) != std::string::npos ||
            line.find(PING_NETWORK_UNREACHABLE) != std::string::npos) {
            break;
        }
        std::smatch match;
        if (std::regex_search(line, match, headerRegex)) {
            ExtractPingHeader(match, pingResult);
            continue;
        }

        if (std::regex_search(line, match, icmpSeqRegex)) {
            ExtractIcmpSeqInfo(match, pingResult);
            continue;
        }

        if (std::regex_search(line, match, statisticsRegex)) {
            ExtractPingStatistics(match, pingResult);
            break;
        }
    }

    int32_t ret = callback->OnNotifyPingResult(pingResult);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("Notify ping result failed.");
    }
}

void NetDiagWrapper::ExtractPingHeader(const std::smatch &match, NetDiagPingResult &pingResult)
{
    if (match.size() < PING_HEADER_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       PING_HEADER_MATCH_SIZE);
        return;
    }
    constexpr int32_t hostPos = 1;
    constexpr int32_t ipPos = 2;
    constexpr int32_t dataSizePos = 3;
    constexpr int32_t payloadSizePos = 4;

    pingResult.host_ = match[hostPos].str();
    pingResult.ipAddr_ = match[ipPos].str();
    pingResult.dateSize_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[dataSizePos].str()));
    pingResult.payloadSize_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[payloadSizePos].str()));
}

void NetDiagWrapper::ExtractIcmpSeqInfo(const std::smatch &match, NetDiagPingResult &pingResult)
{
    if (match.size() < PING_ICMP_SEQ_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       PING_ICMP_SEQ_MATCH_SIZE);
        return;
    }

    constexpr int32_t bytesPos = 1;
    constexpr int32_t fromPos = 2;
    constexpr int32_t icmpSeqPos = 3;
    constexpr int32_t ttlPos = 4;
    constexpr int32_t timePos = 5;

    PingIcmpResponseInfo icmpRespInfo;
    icmpRespInfo.bytes_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[bytesPos].str()));
    icmpRespInfo.from_ = match[fromPos].str();
    icmpRespInfo.icmpSeq_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[icmpSeqPos].str()));
    icmpRespInfo.ttl_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[ttlPos].str()));
    icmpRespInfo.costTime_ = CommonUtils::StrToUint(match[timePos].str());
    pingResult.icmpRespList_.push_back(icmpRespInfo);
}

void NetDiagWrapper::ExtractPingStatistics(const std::smatch &match, NetDiagPingResult &pingResult)
{
    if (match.size() < PING_STATISTICS_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       PING_STATISTICS_MATCH_SIZE);
        return;
    }
    constexpr int32_t transPos = 1;
    constexpr int32_t recvPos = 2;
    pingResult.transCount_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[transPos].str()));
    pingResult.recvCount_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[recvPos].str()));
}

void NetDiagWrapper::ExtractRouteTableInfo(const std::smatch &match, std::list<NetDiagRouteTable> &routeTables)
{
    if (match.size() < NETSTAT_ROUTE_TABLE_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       NETSTAT_ROUTE_TABLE_MATCH_SIZE);
        return;
    }
    constexpr int32_t dstPos = 1;
    constexpr int32_t gatewayPos = 2;
    constexpr int32_t maskPos = 3;
    constexpr int32_t flagsPos = 4;
    constexpr int32_t metricPos = 5;
    constexpr int32_t refPos = 6;
    constexpr int32_t usePos = 7;
    constexpr int32_t ifacePos = 8;

    NetDiagRouteTable routeTable;
    routeTable.destination_ = match[dstPos].str();
    routeTable.gateway_ = match[gatewayPos].str();
    routeTable.mask_ = match[maskPos].str();
    routeTable.flags_ = match[flagsPos].str();
    routeTable.metric_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[metricPos].str()));
    routeTable.ref_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[refPos].str()));
    routeTable.use_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[usePos].str()));
    routeTable.iface_ = match[ifacePos].str();
    routeTables.push_back(routeTable);
    return;
}

void NetDiagWrapper::ExtractNetProtoSocketsInfo(const std::smatch &match, NetDiagSocketsInfo &socketsInfo)
{
    if (match.size() < NETSTAT_NET_PROTOCOL_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       NETSTAT_NET_PROTOCOL_MATCH_SIZE);
        return;
    }
    constexpr int32_t protoPos = 1;
    constexpr int32_t recvPos = 2;
    constexpr int32_t sendPos = 3;
    constexpr int32_t localAddrPos = 4;
    constexpr int32_t foreignAddrPos = 5;
    constexpr int32_t statePos = 6;
    constexpr int32_t userPos = 7;
    constexpr int32_t iNodePos = 8;
    constexpr int32_t programePos = 9;

    NeyDiagNetProtoSocketInfo socketInfo;
    socketInfo.protocol_ = match[protoPos].str();
    socketInfo.recvQueue_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[recvPos].str()));
    socketInfo.sendQueue_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[sendPos].str()));
    socketInfo.localAddr_ = match[localAddrPos].str();
    socketInfo.foreignAddr_ = match[foreignAddrPos].str();
    socketInfo.state_ = match[statePos].str();
    socketInfo.user_ = match[userPos].str();
    socketInfo.inode_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[iNodePos].str()));
    socketInfo.programName_ = match[programePos].str();
    socketsInfo.netProtoSocketsInfo_.push_back(socketInfo);
}

void NetDiagWrapper::ExtractUnixSocketsInfo(const std::smatch &match, NetDiagSocketsInfo &socketsInfo)
{
    if (match.size() < NETSTAT_UNIX_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       NETSTAT_UNIX_MATCH_SIZE);
        return;
    }
    constexpr int32_t protoPos = 1;
    constexpr int32_t refCntPos = 2;
    constexpr int32_t flagsPos = 3;
    constexpr int32_t typePos = 4;
    constexpr int32_t statePos = 5;
    constexpr int32_t iNodePos = 6;
    constexpr int32_t pathPos = 7;

    NetDiagUnixSocketInfo socketInfo;
    socketInfo.protocol_ = match[protoPos].str();
    socketInfo.refCnt_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[refCntPos].str()));
    socketInfo.flags_ = match[flagsPos].str();
    socketInfo.type_ = match[typePos].str();
    socketInfo.state_ = match[statePos].str();
    socketInfo.inode_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[iNodePos].str()));
    socketInfo.path_ = match[pathPos].str();
    socketsInfo.unixSocketsInfo_.push_back(socketInfo);
}

void NetDiagWrapper::ExtractIfaceName(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo)
{
    if (match.size() < IFCONFIG_NAME_INFO_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       IFCONFIG_NAME_INFO_MATCH_SIZE);
        return;
    }
    constexpr int32_t iFaceNamePos = 1;
    constexpr int32_t linkEncapPos = 2;
    constexpr int32_t matAddrPos = 3;
    constexpr int32_t loIfacePos = 4;
    constexpr int32_t loLinkEncapPos = 5;
    if (!match[iFaceNamePos].str().empty()) {
        ifaceInfo.ifaceName_ = match[iFaceNamePos].str();
        ifaceInfo.linkEncap_ = match[linkEncapPos].str();
        ifaceInfo.macAddr_ = match[matAddrPos].str();
    } else if (!match[loIfacePos].str().empty()) {
        ifaceInfo.ifaceName_ = match[loIfacePos].str();
        ifaceInfo.linkEncap_ = match[loLinkEncapPos].str();
        ifaceInfo.macAddr_ = "";
    }
}

void NetDiagWrapper::ExtractIfaceInet(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo)
{
    if (match.size() < IFCONFIG_INET_INFO_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       IFCONFIG_INET_INFO_MATCH_SIZE);
        return;
    }

    constexpr int32_t ifaceAddrPos = 0;
    constexpr int32_t ifaceBcastPos = 1;
    constexpr int32_t ifaceMaskPos = 2;

    ifaceInfo.ipv4Addr_ = match[ifaceAddrPos].str();
    ifaceInfo.ipv4Bcast_ = match[ifaceBcastPos].str();
    if (!match[ifaceMaskPos].str().empty()) {
        ifaceInfo.ipv4Mask_ = match[ifaceMaskPos].str();
    }
}

void NetDiagWrapper::ExtractIfaceInet6(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo)
{
    if (match.size() < IFCONFIG_INET6_INFO_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       IFCONFIG_INET6_INFO_MATCH_SIZE);
        return;
    }
}

void NetDiagWrapper::ExtractIfaceMtu(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo)
{
    if (match.size() < IFCONFIG_MTU_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       IFCONFIG_MTU_MATCH_SIZE);
        return;
    }
    constexpr int32_t IFACE_MTU_POS = 0;
    ifaceInfo.mtu_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[IFACE_MTU_POS].str()));
}

void NetDiagWrapper::ExtractIfaceTxQueueLen(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo)
{
    if (match.size() < IFCONFIG_TX_QUEUE_LEN_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       IFCONFIG_TX_QUEUE_LEN_MATCH_SIZE);
        return;
    }

    constexpr int32_t ifaceQueenLenPos = 0;
    ifaceInfo.txQueueLen_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[ifaceQueenLenPos].str()));
}

void NetDiagWrapper::ExtractIfaceTransDataBytes(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo)
{
    if (match.size() < IFCONFIG_TRANS_BYTES_MATCH_SIZE) {
        NETNATIVE_LOGE("Regex match size:[%{public}d] is too small than %{public}d",
                       static_cast<uint32_t>(match.size()),
                       IFCONFIG_TRANS_BYTES_MATCH_SIZE);
        return;
    }
    constexpr int32_t ifaceRxPos = 0;
    constexpr int32_t ifaceTxPos = 1;
    ifaceInfo.rxBytes_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[ifaceRxPos].str()));
    ifaceInfo.txBytes_ = static_cast<uint16_t>(CommonUtils::StrToUint(match[ifaceTxPos].str()));
}
} // namespace nmd
} // namespace OHOS

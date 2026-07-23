/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <cinttypes>
#include <arpa/inet.h>
#include "netnative_log_wrapper.h"
#include "dns_config_client.h"
#include "dns_param_cache.h"
#include "init_socket.h"
#include "net_conn_client.h"
#include "net_handle.h"
#include "netsys_client.h"
#ifdef USE_SELINUX
#include "selinux/selinux.h"
#endif

#include "dns_quality_diag.h"
#include "dns_resolv_listen.h"
#include "epoller.h"
#include "fwmark_client.h"
#include "parameters.h"

namespace OHOS::nmd {
static constexpr const uint32_t MAX_LISTEN_NUM = 1024;
static constexpr const uint32_t DNS_QUERY_PRE_NUM = 4;
static constexpr const uint32_t DNS_QUERY_ABNORMAL_SIZE =
sizeof(int32_t) + sizeof(int32_t) + sizeof(DnsProcessInfoExt) + sizeof(int32_t) + sizeof(int8_t);
static constexpr const uint32_t DNS_POST_QUERY_PARAM_SIZE =
    sizeof(uint32_t) + sizeof(int32_t) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(QueryParam);
const std::string PUBLIC_DNS_SERVER = "persist.sys.netsysnative_dns_servers_backup";
using namespace NetManagerStandard;
static constexpr int32_t DNS_REPLACE_NUM = 2;

class DnsResolvListenInternal {
public:
    DnsResolvListenInternal() = default;
    ~DnsResolvListenInternal()
    {
        if (serverSockFd_ > 0) {
            close(serverSockFd_);
        }
    }

    void StartListen();

private:
    static void ProcGetConfigCommand(int clientSockFd, uint16_t netId, uint32_t uid);
    static void ProcGetConfigCommandExt(int clientSockFd, uint16_t netId, uint32_t uid);
#ifdef FEATURE_NET_FIREWALL_ENABLE
    static void ProcSetCacheCommand(const std::string &name, uint16_t netId, uint32_t callingUid,
                                    AddrInfoWithTtl addrInfo[MAX_RESULTS], uint32_t resNum);
    static void ProcGetCacheCommand(const std::string &name, int clientSockFd, uint16_t netId, uint32_t callingUid);
#endif
    static void ProcSetCacheCommand(const std::string &name, uint16_t netId, AddrInfoWithTtl addrInfo[MAX_RESULTS],
                                    uint32_t resNum);
    static void ProcGetCacheCommand(const std::string &name, int clientSockFd, uint16_t netId);
    static void ProcSetNodataCacheCommand(const std::string &name, uint16_t netId);
    static void ProcGetNodataCacheCommand(int clientSockFd, uint16_t netId, const std::string &name, uint32_t uid);
    static void ProcJudgeIpv6Command(int clientSockFd, uint16_t netId);
    static void ProcJudgeIpv4Command(int clientSockFd, uint16_t netId);
    static void ProcGetDefaultNetworkCommand(int clientSockFd);
    static void ProcBindSocketCommand(int32_t remoteFd, uint16_t netId);
    static void AddPublicDnsServers(ResolvConfig &sendData, size_t serverSize);
    static void AddPublicDnsServersExt(ResolvConfigExt &sendData, size_t serverSize);
    static bool IsUserDefinedServer(uint16_t netId, uint32_t uid);

    ReceiverRunner ProcCommand();
    ReceiverRunner ProcBindSocket(uint32_t netId);
    ReceiverRunner ProcGetKeyLengthForCache(CommandType command, uint16_t netId, uint32_t uid);
    ReceiverRunner ProcGetKeyForCache(CommandType command, uint16_t netId, uint32_t uid);
    ReceiverRunner ProcGetCacheSize(CommandType command, const std::string &name, uint16_t netId, uint32_t uid);
    ReceiverRunner ProcGetCacheContent(const std::string &name, uint16_t netId, uint32_t uid, uint32_t resNum);
    ReceiverRunner ProcGetTtlContent(const std::string &name, uint16_t netId, uint32_t uid,
        AddrInfo addrInfo[MAX_RESULTS], uint32_t resNum);
    ReceiverRunner ProcPostDnsThreadResult(uint16_t netId);
    ReceiverRunner ProcGetKeyLengthForCache(uint16_t netId, uint32_t uid, uint32_t pid);
    ReceiverRunner ProcGetKeyForCache(uint16_t netId, uint32_t uid, uint32_t pid);
    ReceiverRunner ProcGetPostParam(const std::string &name, uint16_t netId, uint32_t uid, uint32_t pid);

    ReceiverRunner ProcPostDnsThreadQueryResult(uint16_t netId);
    ReceiverRunner ProcGetKeyLengthForAllQueryResult(uint16_t netId, uint32_t uid,
        uint32_t pid, uint32_t size, uint32_t memSize);
    ReceiverRunner ProcPostDnsThreadAbnormal();
    ReceiverRunner ProcPostDnsThreadAbnormalExt(uint32_t eventfailcause, uint32_t uid,
        uint32_t pid, uint8_t addrSize, DnsProcessInfoExt processInfo);

    struct PostParam {
        uint32_t usedTime = 0;
        int32_t queryRet = 0;
        uint32_t aiSize = 0;
        uint32_t dnsServerNum = 0;
        QueryParam param{};
    };
    ReceiverRunner ProcPostDnsResult(const std::string &name, uint16_t netId, uint32_t uid, uint32_t pid,
                                     const PostParam &param);
    bool ProcGetKeyLengthForQueryAddr(uint8_t addrSize,
        PostDnsQueryParam &queryParam, const std::string &data, int index);

    int32_t serverSockFd_ = -1;
    std::shared_ptr<EpollServer> server_;
};

void DnsResolvListenInternal::AddPublicDnsServers(ResolvConfig &sendData, size_t serverSize)
{
    std::string publicDnsServer = OHOS::system::GetParameter(PUBLIC_DNS_SERVER, "");
    size_t i = 0;
    for (; i < serverSize; i++) {
        if (strcmp(sendData.nameservers[i], publicDnsServer.c_str()) == 0) {
            return;
        }
    }
    if (i >= MAX_SERVER_NUM) {
        NETNATIVE_LOGI("Invalid serverSize or mPublicDns already exists");
        return;
    }
    if (memcpy_s(sendData.nameservers[i], sizeof(sendData.nameservers[i]), publicDnsServer.c_str(),
                 publicDnsServer.length() + 1) != ERR_OK) {
        DNS_CONFIG_PRINT("mem copy failed");
        return;
    }
    DNS_CONFIG_PRINT("i = %{public}zu sendData.nameservers: %{public}s", i, sendData.nameservers[i]);
}

void DnsResolvListenInternal::AddPublicDnsServersExt(ResolvConfigExt &sendData, size_t serverSize)
{
    std::string publicDnsServer = OHOS::system::GetParameter(PUBLIC_DNS_SERVER, "");
    size_t i = 0;
    for (; i < serverSize; i++) {
        if (strcmp(sendData.nameservers[i], publicDnsServer.c_str()) == 0) {
            return;
        }
    }
    if (i >= MAX_SERVER_NUM_EXT) {
        NETNATIVE_LOGI("Invalid serverSize or mPublicDns already exists");
        return;
    }
    if (memcpy_s(sendData.nameservers[i], sizeof(sendData.nameservers[i]), publicDnsServer.c_str(),
                 publicDnsServer.length() + 1) != ERR_OK) {
        DNS_CONFIG_PRINT("mem copy failed");
        return;
    }
    DNS_CONFIG_PRINT("i = %{public}zu sendData.nameservers: %{public}s", i, sendData.nameservers[i]);
}

// LCOV_EXCL_START
static void ReplaceDnsServer(ResolvConfig &sendData, const std::vector<std::string> &servers)
{
    int32_t dnsNumTotal = static_cast<int32_t>(servers.size());
    if (dnsNumTotal <= MAX_SERVER_NUM - 1) {
        return;
    }
    for (int32_t k = 1; k <= DNS_REPLACE_NUM; k++) {
        int32_t index = MAX_SERVER_NUM - k - 1;
        if (memset_s(sendData.nameservers[index], sizeof(sendData.nameservers[index]),
            0, sizeof(sendData.nameservers[index])) != EOK) {
            DNS_CONFIG_PRINT("ReplaceDnsServer memset_s failed");
            continue;
        }

        if (memcpy_s(sendData.nameservers[index], sizeof(sendData.nameservers[index]),
            servers[dnsNumTotal - k].c_str(), servers[dnsNumTotal - k].length()) < 0) {
            DNS_CONFIG_PRINT("ReplaceDnsServer mem copy failed");
            continue;
        }
    }
}

void DnsResolvListenInternal::ProcGetConfigCommand(int clientSockFd, uint16_t netId, uint32_t uid)
{
    NETNATIVE_LOG_D("DnsResolvListenInternal::ProcGetConfigCommand uid = [%{public}u]", uid);
    ResolvConfig sendData = {0};
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    uint16_t baseTimeoutMsec = DEFAULT_TIMEOUT;
    uint8_t retryCount = DEFAULT_RETRY;
    bool isUserDefinedDnsServer = false;

#ifdef FEATURE_NET_FIREWALL_ENABLE
    DnsParamCache::GetInstance().SetCallingUid(uid);
#endif

    int status;
    if (DnsParamCache::GetInstance().IsVpnOpen() && netId == 0) {
        status = DnsParamCache::GetInstance().GetResolverConfig(static_cast<uint16_t>(netId), uid, servers, domains,
                                                                baseTimeoutMsec, retryCount);
    } else {
        status = DnsParamCache::GetInstance().GetResolverConfig(static_cast<uint16_t>(netId), servers, domains,
                                                                baseTimeoutMsec, retryCount);
    }
    DNS_CONFIG_PRINT("GetResolverConfig status: %{public}d", status);
    if (status < 0) {
        sendData.error = status;
    } else {
        sendData.retryCount = retryCount;
        sendData.timeoutMs = baseTimeoutMsec;
        size_t i = 0;
        for (; i < std::min<size_t>(MAX_SERVER_NUM - 1, servers.size()); i++) {
            if (memcpy_s(sendData.nameservers[i], sizeof(sendData.nameservers[i]), servers[i].c_str(),
                         servers[i].length()) < 0) {
                DNS_CONFIG_PRINT("mem copy failed");
                continue;
            }
            DNS_CONFIG_PRINT("i = %{public}d sendData.nameservers: %{public}s", i, sendData.nameservers[i]);
        }
        sendData.nonPublicNum = i;
        ReplaceDnsServer(sendData, servers);
        // the last one is for baidu DNS Server
#ifdef ENABLE_PUBLIC_DNS_SERVER
        AddPublicDnsServers(sendData, i);
#endif
    }
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&sendData), sizeof(ResolvConfig))) {
        DNS_CONFIG_PRINT("send failed");
    }
    DNS_CONFIG_PRINT("ProcGetConfigCommand end");
}

void DnsResolvListenInternal::ProcGetConfigCommandExt(int clientSockFd, uint16_t netId, uint32_t uid)
{
    NETNATIVE_LOG_D("DnsResolvListenInternal::ProcGetConfigCommandExt uid = [%{public}u]", uid);
    ResolvConfigExt sendData = {0};
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    uint16_t baseTimeoutMsec = DEFAULT_TIMEOUT;
    uint8_t retryCount = DEFAULT_RETRY;
    bool isUserDefinedDnsServer = false;

#ifdef FEATURE_NET_FIREWALL_ENABLE
    DnsParamCache::GetInstance().SetCallingUid(uid);
#endif

    int status;
    if (DnsParamCache::GetInstance().IsVpnOpen() && netId == 0) {
        status = DnsParamCache::GetInstance().GetResolverConfig(static_cast<uint16_t>(netId), uid, servers, domains,
                                                                baseTimeoutMsec, retryCount);
    } else {
        status = DnsParamCache::GetInstance().GetResolverConfig(static_cast<uint16_t>(netId), servers, domains,
                                                                baseTimeoutMsec, retryCount);
    }
    DNS_CONFIG_PRINT("GetResolverConfig status: %{public}d", status);
    if (status < 0) {
        sendData.error = status;
    } else {
        sendData.retryCount = retryCount;
        sendData.timeoutMs = baseTimeoutMsec;
        size_t i = 0;
        for (; i < std::min<size_t>(MAX_SERVER_NUM_EXT - 1, servers.size()); i++) {
            if (memcpy_s(sendData.nameservers[i], sizeof(sendData.nameservers[i]), servers[i].c_str(),
                         servers[i].length()) < 0) {
                DNS_CONFIG_PRINT("mem copy failed");
                continue;
            }
            DNS_CONFIG_PRINT("i = %{public}d sendData.nameservers: %{public}s", i, sendData.nameservers[i]);
        }
        sendData.nonPublicNum = i;
        // the last one is for baidu DNS Server
#ifdef ENABLE_PUBLIC_DNS_SERVER
        AddPublicDnsServersExt(sendData, i);
#endif
    }
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&sendData), sizeof(ResolvConfigExt))) {
        DNS_CONFIG_PRINT("send failed");
    }
    DNS_CONFIG_PRINT("ProcGetConfigCommand end");
}

void DnsResolvListenInternal::ProcGetCacheCommand(const std::string &name, int clientSockFd, uint16_t netId)
{
#ifdef FEATURE_NET_FIREWALL_ENABLE
    ProcGetCacheCommand(name, clientSockFd, netId, 0);
}

void DnsResolvListenInternal::ProcGetCacheCommand(const std::string &name, int clientSockFd, uint16_t netId,
                                                  uint32_t callingUid)
{
    DnsParamCache::GetInstance().SetCallingUid(callingUid);
#endif
    auto cacheRes = DnsParamCache::GetInstance().GetDnsCache(netId, name);

    uint32_t resNum = std::min<uint32_t>(MAX_RESULTS, static_cast<uint32_t>(cacheRes.size()));
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&resNum), sizeof(resNum))) {
        DNS_CONFIG_PRINT("send errno %{public}d", errno);
        return;
    }

    if (resNum == 0 || resNum > MAX_RESULTS) {
        return;
    }

    AddrInfo addrInfo[MAX_RESULTS] = {};
    for (uint32_t i = 0; i < resNum; i++) {
        if (memcpy_s(reinterpret_cast<char *>(&addrInfo[i]), sizeof(AddrInfo), reinterpret_cast<char *>(&cacheRes[i]),
                     sizeof(AddrInfo)) != 0) {
            return;
        }
    }
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(addrInfo), sizeof(AddrInfo) * resNum)) {
        DNS_CONFIG_PRINT("send errno %{public}d", errno);
        return;
    }
    DNS_CONFIG_PRINT("ProcGetCacheCommand end");
}

void DnsResolvListenInternal::ProcSetCacheCommand(const std::string &name, uint16_t netId,
                                                  AddrInfoWithTtl addrInfo[MAX_RESULTS], uint32_t resNum)
{
#ifdef FEATURE_NET_FIREWALL_ENABLE
    ProcSetCacheCommand(name, netId, 0, addrInfo, resNum);
}

void DnsResolvListenInternal::ProcSetCacheCommand(const std::string &name, uint16_t netId, uint32_t callingUid,
                                                  AddrInfoWithTtl addrInfo[MAX_RESULTS], uint32_t resNum)
{
#endif
#ifdef FEATURE_NET_FIREWALL_ENABLE
    DnsParamCache::GetInstance().SetCallingUid(callingUid);
#endif

    std::sort(addrInfo, addrInfo + resNum, [](const AddrInfoWithTtl &a, const AddrInfoWithTtl &b) {
        return a.ttl > b.ttl;
    });
    for (size_t i = 0; i < resNum; ++i) {
        DnsParamCache::GetInstance().SetDnsCache(netId, name, addrInfo[i]);
    }
    DnsParamCache::GetInstance().SetCacheDelayed(netId, name);
    DNS_CONFIG_PRINT("ProcSetCacheCommand end");
}

void DnsResolvListenInternal::ProcSetNodataCacheCommand(const std::string &name, uint16_t netId)
{
    DnsParamCache::GetInstance().SetNodataCache(netId, name);
}

void DnsResolvListenInternal::ProcGetNodataCacheCommand(int clientSockFd, uint16_t netId, const std::string &name,
    uint32_t uid)
{
    int isInNodataCache = DnsParamCache::GetInstance().IsInNodataCache(netId, name) ? 1 : 0;
    int isInIpv6UidBlackList = DnsParamCache::GetInstance().IsInIpv6UidBlackList(netId, uid) ? 1 : 0;
    int skipAAAA = (isInNodataCache != 0 || isInIpv6UidBlackList != 0) ? 1 : 0;
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&skipAAAA), sizeof(int))) {
        DNS_CONFIG_PRINT("send failed");
    }
    DNS_CONFIG_PRINT("ProcGetNodataCacheCommand end, netId: %{public}d, host: %{public}s, uid: %{public}d, "
        "skipAAAA: %{public}d", netId, name.c_str(), uid, skipAAAA);
}

void DnsResolvListenInternal::ProcJudgeIpv6Command(int clientSockFd, uint16_t netId)
{
    int enable = DnsParamCache::GetInstance().IsIpv6Enable(netId) ? 1 : 0;
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&enable), sizeof(int))) {
        DNS_CONFIG_PRINT("send failed");
    }
}

void DnsResolvListenInternal::ProcJudgeIpv4Command(int clientSockFd, uint16_t netId)
{
    int enable = DnsParamCache::GetInstance().IsIpv4Enable(netId) ? 1 : 0;
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&enable), sizeof(int))) {
        DNS_CONFIG_PRINT("send failed");
    }
}

void DnsResolvListenInternal::ProcGetDefaultNetworkCommand(int clientSockFd)
{
    NetHandle netHandle;
    int netId = DnsParamCache::GetInstance().GetDefaultNetwork();
    NETNATIVE_LOG_D("ProcGetDefaultNetworkCommand %{public}d", netId);
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&netId), sizeof(int))) {
        NETNATIVE_LOGE("send failed");
    }
}

void DnsResolvListenInternal::ProcBindSocketCommand(int32_t remoteFd, uint16_t netId)
{
    NETNATIVE_LOGE("ProcGetDefaultNetworkCommand %{public}d, %{public}d", netId, remoteFd);
    if (OHOS::nmd::FwmarkClient().BindSocket(remoteFd, netId) != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("BindSocket to netid failed");
    }
}

void DnsResolvListenInternal::StartListen()
{
    NETNATIVE_LOGE("Enter StartListen");

    serverSockFd_ = GetControlSocket(DNS_SOCKET_NAME);
    if (serverSockFd_ < 0) {
        NETNATIVE_LOGE("create socket failed %{public}d", errno);
        return;
    }

    // listen
    if (listen(serverSockFd_, MAX_LISTEN_NUM) < 0) {
        NETNATIVE_LOGE("listen errno %{public}d", errno);
        close(serverSockFd_);
        serverSockFd_ = -1;
        return;
    }

    if (!MakeNonBlock(serverSockFd_)) {
        close(serverSockFd_);
        serverSockFd_ = -1;
        return;
    }
    NETNATIVE_LOGE("begin listen");
    server_ = std::make_shared<EpollServer>(serverSockFd_, sizeof(RequestInfo), ProcCommand());
    server_->Run();
}

ReceiverRunner DnsResolvListenInternal::ProcCommand()
{
    // single thread, captrue <this> is safe
    return [this](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr || data.size() < sizeof(RequestInfo)) {
            return FixedLengthReceiverState::ONERROR;
        }

        RequestInfo requestInfo{};
        if (memcpy_s(&requestInfo, sizeof(RequestInfo), data.data(), sizeof(RequestInfo)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }

        auto info = &requestInfo;
        auto netId = info->netId;
        auto uid = info->uid;

        switch (info->command) {
            case GET_CONFIG:
                ProcGetConfigCommand(fd, netId, uid);
                return FixedLengthReceiverState::DATA_ENOUGH;
            case GET_CONFIG_EXT:
                ProcGetConfigCommandExt(fd, netId, uid);
                return FixedLengthReceiverState::DATA_ENOUGH;
            case GET_CACHE:
            case SET_CACHE:
                if (server_) {
                    server_->AddReceiver(fd, sizeof(uint32_t),
                                         ProcGetKeyLengthForCache(static_cast<CommandType>(info->command),
                                                                  static_cast<uint16_t>(info->netId), info->uid));
                }
                return FixedLengthReceiverState::CONTINUE;
            case SET_NODATA_CACHE:
            case GET_NODATA_CACHE:
                if (server_) {
                    server_->AddReceiver(fd, sizeof(uint32_t),
                                         ProcGetKeyLengthForCache(static_cast<CommandType>(info->command),
                                                                  static_cast<uint16_t>(info->netId), info->uid));
                }
                return FixedLengthReceiverState::CONTINUE;
            case POST_DNS_RESULT:
                server_->AddReceiver(fd, sizeof(uint32_t) + sizeof(uint32_t),
                                     ProcPostDnsThreadResult(static_cast<uint16_t>(info->netId)));
                return FixedLengthReceiverState::CONTINUE;
            case JUDGE_IPV6:
                ProcJudgeIpv6Command(fd, netId);
                return FixedLengthReceiverState::DATA_ENOUGH;
            case GET_DEFAULT_NETWORK:
                ProcGetDefaultNetworkCommand(fd);
                return FixedLengthReceiverState::DATA_ENOUGH;
            case BIND_SOCKET:
                server_->AddReceiver(fd, sizeof(int32_t), ProcBindSocket(netId));
                return FixedLengthReceiverState::CONTINUE;
            case POST_DNS_QUERY_RESULT:
                server_->AddReceiver(fd, sizeof(uint32_t) * DNS_QUERY_PRE_NUM,
                                     ProcPostDnsThreadQueryResult(static_cast<uint16_t>(info->netId)));
                return FixedLengthReceiverState::CONTINUE;
            case POST_DNS_ABNORMAL_RESULT:
                server_->AddReceiver(fd, DNS_QUERY_ABNORMAL_SIZE,
                                     ProcPostDnsThreadAbnormal());
                return FixedLengthReceiverState::CONTINUE;
            case JUDGE_IPV4:
                ProcJudgeIpv4Command(fd, netId);
                return FixedLengthReceiverState::DATA_ENOUGH;
            default:
                return FixedLengthReceiverState::ONERROR;
        }
    };
}

ReceiverRunner DnsResolvListenInternal::ProcBindSocket(uint32_t netId)
{
    return [this, netId](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        // fd is AF_UNIX fd
        // remoteFd is the TCP/UDP socket which is from app process
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < sizeof(int32_t)) {
            return FixedLengthReceiverState::ONERROR;
        }

        int32_t remoteFd;
        if (memcpy_s(&remoteFd, sizeof(int32_t), data.data(), sizeof(int32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        ProcBindSocketCommand(remoteFd, netId);
        return FixedLengthReceiverState::DATA_ENOUGH;
    };
}

ReceiverRunner DnsResolvListenInternal::ProcGetKeyLengthForCache(CommandType command, uint16_t netId, uint32_t uid)
{
    return [this, command, netId, uid](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < sizeof(uint32_t)) {
            return FixedLengthReceiverState::ONERROR;
        }

        uint32_t nameLen;
        if (memcpy_s(&nameLen, sizeof(uint32_t), data.data(), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (nameLen > MAX_HOST_NAME_LEN) {
            return FixedLengthReceiverState::ONERROR;
        }
        server_->AddReceiver(fd, nameLen, ProcGetKeyForCache(command, netId, uid));
        return FixedLengthReceiverState::CONTINUE;
    };
}

ReceiverRunner DnsResolvListenInternal::ProcGetKeyForCache(CommandType command, uint16_t netId, uint32_t uid)
{
    return [this, command, netId, uid](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.empty()) {
            return FixedLengthReceiverState::ONERROR;
        }

        switch (command) {
            case SET_CACHE:
                server_->AddReceiver(fd, sizeof(uint32_t), ProcGetCacheSize(command, data, netId, uid));
                return FixedLengthReceiverState::CONTINUE;
            case SET_NODATA_CACHE:
                ProcSetNodataCacheCommand(data, netId);
                return FixedLengthReceiverState::DATA_ENOUGH;
            case GET_NODATA_CACHE:
                ProcGetNodataCacheCommand(fd, netId, data, uid);
                return FixedLengthReceiverState::DATA_ENOUGH;
            case GET_CACHE:
#ifdef FEATURE_NET_FIREWALL_ENABLE
                ProcGetCacheCommand(data, fd, netId, uid);
#else
                ProcGetCacheCommand(data, fd, netId);
#endif
                return FixedLengthReceiverState::DATA_ENOUGH;
            default:
                return FixedLengthReceiverState::ONERROR;
        }
    };
}

ReceiverRunner DnsResolvListenInternal::ProcGetCacheSize(CommandType command,
    const std::string &name, uint16_t netId, uint32_t uid)
{
    return [this, command, name, netId, uid](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < sizeof(uint32_t)) {
            return FixedLengthReceiverState::ONERROR;
        }

        uint32_t resNum;
        if (memcpy_s(&resNum, sizeof(uint32_t), data.data(), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        resNum = std::min<uint32_t>(MAX_RESULTS, resNum);
        if (resNum == 0) {
            return FixedLengthReceiverState::ONERROR;
        }
        server_->AddReceiver(fd, sizeof(AddrInfoWithTtl) * resNum, ProcGetCacheContent(name, netId, uid, resNum));
        return FixedLengthReceiverState::CONTINUE;
    };
}

ReceiverRunner DnsResolvListenInternal::ProcGetCacheContent(const std::string &name, uint16_t netId, uint32_t uid,
                                                            uint32_t resNum)
{
    return [this, name, netId, uid, resNum](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < sizeof(AddrInfoWithTtl) * resNum) {
            return FixedLengthReceiverState::ONERROR;
        }

        auto size = std::min<uint32_t>(MAX_RESULTS, resNum);
        AddrInfoWithTtl addrInfo[MAX_RESULTS]{};
        if (memcpy_s(addrInfo, sizeof(AddrInfoWithTtl) * MAX_RESULTS,
                     data.data(), sizeof(AddrInfoWithTtl) * size) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }

#ifdef FEATURE_NET_FIREWALL_ENABLE
        ProcSetCacheCommand(name, netId, uid, addrInfo, size);
#else
        ProcSetCacheCommand(name, netId, addrInfo, size);
#endif
        return FixedLengthReceiverState::DATA_ENOUGH;
    };
}

ReceiverRunner DnsResolvListenInternal::ProcPostDnsThreadResult(uint16_t netId)
{
    return [this, netId](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < sizeof(uint32_t) + sizeof(uint32_t)) {
            return FixedLengthReceiverState::ONERROR;
        }

        uint32_t uid;
        uint32_t pid;
        if (memcpy_s(&uid, sizeof(uint32_t), data.data(), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memcpy_s(&pid, sizeof(uint32_t), data.data() + sizeof(uint32_t), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        server_->AddReceiver(fd, sizeof(uint32_t), ProcGetKeyLengthForCache(netId, uid, pid));
        return FixedLengthReceiverState::CONTINUE;
    };
}

ReceiverRunner DnsResolvListenInternal::ProcGetKeyLengthForCache(uint16_t netId, uint32_t uid, uint32_t pid)
{
    return [this, netId, uid, pid](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < sizeof(uint32_t)) {
            return FixedLengthReceiverState::ONERROR;
        }
        uint32_t nameLen;
        if (memcpy_s(&nameLen, sizeof(uint32_t), data.data(), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (nameLen > MAX_HOST_NAME_LEN) {
            return FixedLengthReceiverState::ONERROR;
        }
        server_->AddReceiver(fd, nameLen, ProcGetKeyForCache(netId, uid, pid));
        return FixedLengthReceiverState::CONTINUE;
    };
}

ReceiverRunner DnsResolvListenInternal::ProcGetKeyForCache(uint16_t netId, uint32_t uid, uint32_t pid)
{
    return [this, netId, uid, pid](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.empty()) {
            return FixedLengthReceiverState::ONERROR;
        }

        server_->AddReceiver(fd, DNS_POST_QUERY_PARAM_SIZE,
                             ProcGetPostParam(data, netId, uid, pid));
        return FixedLengthReceiverState::CONTINUE;
    };
}

ReceiverRunner DnsResolvListenInternal::ProcGetPostParam(const std::string &name, uint16_t netId, uint32_t uid,
                                                         uint32_t pid)
{
    return [this, name, netId, uid, pid](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < DNS_POST_QUERY_PARAM_SIZE) {
            return FixedLengthReceiverState::ONERROR;
        }

        PostParam param{};
        if (memcpy_s(&param.usedTime, sizeof(uint32_t), data.data(), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memcpy_s(&param.queryRet, sizeof(int32_t), data.data() + sizeof(uint32_t), sizeof(int32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memcpy_s(&param.aiSize, sizeof(uint32_t), data.data() + sizeof(uint32_t) + sizeof(int32_t),
                     sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memcpy_s(&param.param, sizeof(QueryParam),
                     data.data() + sizeof(uint32_t) + sizeof(int32_t) + sizeof(uint32_t), sizeof(QueryParam)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memcpy_s(&param.dnsServerNum, sizeof(uint32_t),
                     data.data() + sizeof(uint32_t) + sizeof(int32_t) + sizeof(uint32_t) + sizeof(QueryParam),
                     sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }

        if (param.queryRet == 0 && param.aiSize > 0) {
            auto size = std::min<uint32_t>(MAX_RESULTS, param.aiSize);
            param.aiSize = size;
            size_t dataLen = sizeof(AddrInfo) * size + param.dnsServerNum * sizeof(DnsServerInfo);
            server_->AddReceiver(fd, dataLen, ProcPostDnsResult(name, netId, uid, pid, param));
            return FixedLengthReceiverState::CONTINUE;
        } else {
            DnsQualityDiag::GetInstance().ReportDnsResult(netId, uid, pid, static_cast<int32_t>(param.usedTime),
                                                          const_cast<char *>(name.c_str()), 0, param.queryRet,
                                                          param.param, nullptr, 0, nullptr);
            return FixedLengthReceiverState::DATA_ENOUGH;
        }
    };
}

ReceiverRunner DnsResolvListenInternal::ProcPostDnsResult(const std::string &name, uint16_t netId, uint32_t uid,
                                                          uint32_t pid, const PostParam &param)
{
    return
        [this, name, netId, uid, pid, param](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
            if (server_ == nullptr) {
                return FixedLengthReceiverState::ONERROR;
            }
            size_t dataLen = sizeof(AddrInfo) * param.aiSize + sizeof(DnsServerInfo) * param.dnsServerNum;
            if (data.size() < dataLen) {
                return FixedLengthReceiverState::ONERROR;
            }
            auto size = std::min<uint32_t>(MAX_RESULTS, param.aiSize);
            AddrInfo addrInfo[MAX_RESULTS]{};
            if (memcpy_s(addrInfo, sizeof(AddrInfo) * MAX_RESULTS, data.data(), sizeof(AddrInfo) * size) != EOK) {
                return FixedLengthReceiverState::ONERROR;
            }
            DnsServerInfo dnsServerInfo[MAX_RESULTS]{};
            if (memcpy_s(dnsServerInfo, sizeof(DnsServerInfo) * MAX_RESULTS, data.data() + sizeof(AddrInfo) * size,
                sizeof(DnsServerInfo) * param.dnsServerNum) != EOK) {
                return FixedLengthReceiverState::ONERROR;
            }
            DnsQualityDiag::GetInstance().ReportDnsResult(netId, uid, pid, static_cast<int32_t>(param.usedTime),
                                                          const_cast<char *>(name.c_str()), size, param.queryRet,
                                                          param.param, addrInfo, param.dnsServerNum, dnsServerInfo);
            return FixedLengthReceiverState::DATA_ENOUGH;
        };
}

void DnsResolvListen::StartListen()
{
    (void)this;
    DnsResolvListenInternal dnsResolvListenInternal;
    dnsResolvListenInternal.StartListen();
}

bool DnsResolvListenInternal::IsUserDefinedServer(uint16_t netId, uint32_t uid)
{
    int status = 0;
    bool isUserDefinedDnsServer = false;
    if (DnsParamCache::GetInstance().IsVpnOpen() && netId == 0) {
        status = DnsParamCache::GetInstance().GetUserDefinedServerFlag(static_cast<uint16_t>(netId),
            isUserDefinedDnsServer, uid);
    } else {
        status = DnsParamCache::GetInstance().GetUserDefinedServerFlag(static_cast<uint16_t>(netId),
            isUserDefinedDnsServer);
    }
    if (status < 0) {
        isUserDefinedDnsServer = false;
    }
    return isUserDefinedDnsServer;
}

ReceiverRunner DnsResolvListenInternal::ProcPostDnsThreadAbnormalExt(
    uint32_t eventfailcause, uint32_t uid, uint32_t pid, uint8_t addrSize, DnsProcessInfoExt processInfo
)
{
    return [this, eventfailcause, uid, pid, addrSize, processInfo](FileDescriptor fd,
        const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < addrSize * sizeof(AddrInfo)) {
            return FixedLengthReceiverState::ONERROR;
        }
        AddrInfo addrInfo[MAX_RESULTS]{};
        if (memcpy_s(addrInfo, sizeof(AddrInfo) * MAX_RESULTS,  data.data(),
            sizeof(AddrInfo) * addrSize) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        PostDnsQueryParam queryParam;
        queryParam.netId = 0;
        queryParam.pid = pid;
        queryParam.uid = uid;
        queryParam.addrSize = addrSize;
        queryParam.processInfo = processInfo;
        DnsQualityDiag::GetInstance().ReportDnsQueryAbnormal(eventfailcause, queryParam, addrInfo);
        return FixedLengthReceiverState::DATA_ENOUGH;
    };
}

ReceiverRunner DnsResolvListenInternal::ProcPostDnsThreadAbnormal()
{
    return [this](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < DNS_QUERY_ABNORMAL_SIZE) {
            return FixedLengthReceiverState::ONERROR;
        }
        uint32_t uid;
        uint32_t pid;
        uint8_t addrSize;
        uint32_t eventfailcause;
        DnsProcessInfoExt processInfo;
        int index = 0;
        if (memcpy_s(&uid, sizeof(uint32_t), data.data(), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        index += sizeof(uint32_t);
        if (memcpy_s(&pid, sizeof(uint32_t), data.data() + index, sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        index += sizeof(uint32_t);
        if (memcpy_s(&eventfailcause, sizeof(uint32_t), data.data() + index, sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        index += sizeof(uint32_t);
        if (memcpy_s(&addrSize, sizeof(uint8_t), data.data() + index, sizeof(uint8_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        index += sizeof(int8_t);
        if (memcpy_s(&processInfo, sizeof(DnsProcessInfoExt),
            data.data() + index, sizeof(DnsProcessInfoExt)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        PostDnsQueryParam queryParam;
        queryParam.netId = 0;
        queryParam.uid = uid;
        queryParam.pid = pid;
        queryParam.addrSize = addrSize;
        queryParam.processInfo = processInfo;
        if (addrSize > 0) {
            server_->AddReceiver(fd, addrSize * sizeof(AddrInfo),
                ProcPostDnsThreadAbnormalExt(eventfailcause, uid, pid, addrSize, processInfo));
            return FixedLengthReceiverState::CONTINUE;
        } else {
            DnsQualityDiag::GetInstance().ReportDnsQueryAbnormal(eventfailcause, queryParam, nullptr);
            return FixedLengthReceiverState::DATA_ENOUGH;
        }
    };
}

ReceiverRunner DnsResolvListenInternal::ProcPostDnsThreadQueryResult(uint16_t netId)
{
    return [this, netId](FileDescriptor fd, const std::string &data) -> FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < sizeof(uint32_t) * DNS_QUERY_PRE_NUM) {
            return FixedLengthReceiverState::ONERROR;
        }
        uint32_t uid;
        uint32_t pid;
        uint32_t dnsCacheSize;
        uint32_t allCacheSize;
        if (memcpy_s(&uid, sizeof(uint32_t), data.data(), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memcpy_s(&pid, sizeof(uint32_t), data.data() + sizeof(uint32_t), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memcpy_s(&dnsCacheSize, sizeof(uint32_t), data.data() + sizeof(uint32_t) + sizeof(uint32_t),
            sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memcpy_s(&allCacheSize, sizeof(uint32_t),
            data.data() + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), sizeof(uint32_t)) != EOK) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (allCacheSize > MAX_ALL_CACHE_SIZE) {
            return FixedLengthReceiverState::ONERROR;
        }
        server_->AddReceiver(fd, allCacheSize,
            ProcGetKeyLengthForAllQueryResult(netId, uid, pid, dnsCacheSize, allCacheSize));
        return FixedLengthReceiverState::CONTINUE;
    };
}

bool DnsResolvListenInternal::ProcGetKeyLengthForQueryAddr(uint8_t addrSize,
    PostDnsQueryParam &queryParam, const std::string &data, int index)
{
    if (addrSize > 0) {
        uint32_t allSize = static_cast<uint32_t>(index + sizeof(AddrInfo) * queryParam.addrSize);
        if (allSize > data.size()) {
            return false;
        }
        auto size = std::min<uint8_t>(MAX_RESULTS, addrSize);
        queryParam.addrSize = size;
        AddrInfo addrInfo[MAX_RESULTS]{};
        if (memcpy_s(addrInfo, sizeof(AddrInfo) * MAX_RESULTS,  data.data() + index,
            sizeof(AddrInfo) * queryParam.addrSize) != EOK) {
            return false;
        }
        DnsQualityDiag::GetInstance().ReportDnsQueryResult(queryParam, addrInfo, size);
    } else {
        DnsQualityDiag::GetInstance().ReportDnsQueryResult(queryParam, nullptr, 0);
    }
    return true;
}

ReceiverRunner DnsResolvListenInternal::ProcGetKeyLengthForAllQueryResult(uint16_t netId,
    uint32_t uid, uint32_t pid, uint32_t size, uint32_t memSize)
{
    return [this, netId, uid, pid, size, memSize](FileDescriptor fd, const std::string &data) ->
        FixedLengthReceiverState {
        if (server_ == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (data.size() < memSize) {
            return FixedLengthReceiverState::ONERROR;
        }
        int index = 0;
        for (uint32_t i = 0; i < size; i++) {
            uint8_t addrSize = 0;
            PostDnsQueryParam queryParam;
            queryParam.netId = netId;
            queryParam.uid = uid;
            queryParam.pid = pid;
            if (static_cast<uint32_t>(index + sizeof(uint8_t)) > data.size()) {
                return FixedLengthReceiverState::ONERROR;
            }
            if (memcpy_s(&addrSize, sizeof(uint8_t), data.data() + index, sizeof(uint8_t)) != EOK) {
                return FixedLengthReceiverState::ONERROR;
            }
            index += sizeof(uint8_t);
            if (static_cast<uint32_t>(index + sizeof(DnsProcessInfoExt)) > data.size()) {
                return FixedLengthReceiverState::ONERROR;
            }
            if (memcpy_s(&queryParam.processInfo, sizeof(DnsProcessInfoExt),  data.data() + index,
                sizeof(DnsProcessInfoExt)) != EOK) {
                return FixedLengthReceiverState::ONERROR;
            }
            index += sizeof(DnsProcessInfoExt);
            if (!ProcGetKeyLengthForQueryAddr(addrSize, queryParam, data, index)) {
                return FixedLengthReceiverState::ONERROR;
            }
            index += (addrSize * sizeof(AddrInfo));
        }
        return FixedLengthReceiverState::DATA_ENOUGH;
    };
}
// LCOV_EXCL_STOP
} // namespace OHOS::nmd

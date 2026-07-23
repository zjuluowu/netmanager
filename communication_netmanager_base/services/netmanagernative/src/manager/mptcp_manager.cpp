/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "mptcp_manager.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {

namespace {
constexpr const char *IP_CMD_PATH = "/system/bin/ip";
constexpr const char *MPTCP_ENDPOINT_ADD = "mptcp endpoint add";
constexpr const char *MPTCP_ENDPOINT_DELETE = "mptcp endpoint delete id";
constexpr const char *MPTCP_ENDPOINT_SHOW = "mptcp endpoint show";
constexpr const char *MPTCP_LIMITS_SET = "mptcp limits set";
constexpr const char *MPTCP_SUBFLOW_FLAG = "subflow";
constexpr const char *MPTCP_DEV_FLAG = "dev";
constexpr const char *MPTCP_SUBFLOWS_PARAM = "subflows";
constexpr const char *MPTCP_ADD_ADDR_ACCEPTED_PARAM = "add_addr_accepted";

constexpr const char *OPTION_SPACE = " ";

const std::set<std::string> MONITORED_INTERFACES = {"wlan0", "rmnet0", "rmnet1"};

constexpr int32_t MPTCP_MAX_SUBFLOWS = 7;
constexpr int32_t MPTCP_MAX_ADD_ADDR_ACCEPTED = 7;
constexpr int32_t MPTCP_DISABLED = 0;
constexpr int32_t INVALID_ENDPOINT_ID = -1;
constexpr int32_t STRTOL_BASE = 10;
constexpr size_t ID_KEYWORD_LEN = 2;
constexpr int32_t MIN_ACTIVE_INTERFACE_COUNT = 1;
const std::string ENDPOINT_KEY_SEPARATOR = "_";
const std::string ID_KEYWORD = "id";
}

MptcpManager::MptcpManager() {}

MptcpManager::~MptcpManager() {}

int32_t MptcpManager::AddEndpoint(const std::string &ipAddr, const std::string &ifName)
{
    if (ipAddr.empty() || ifName.empty()) {
        NETNATIVE_LOGE("AddEndpoint: ipAddr or ifName is empty");
        return NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER;
    }

    std::string command = BuildEndpointAddCommand(ipAddr, ifName);
    if (command.empty()) {
        NETNATIVE_LOGE("AddEndpoint: build command failed");
        return NetManagerStandard::NETMANAGER_ERR_INTERNAL;
    }

    int32_t ret = ExecuteMptcpCommand(command);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("AddEndpoint: execute command failed, ret=%{public}d", ret);
        return ret;
    }

    int32_t endpointId = GetEndpointId(ipAddr, ifName);
    if (endpointId < 0) {
        NETNATIVE_LOGW("AddEndpoint: failed to get endpoint id for ip=%{public}s",
                       NetManagerStandard::CommonUtils::ToAnonymousIp(ipAddr).c_str());
    }

    std::string key = ipAddr + ENDPOINT_KEY_SEPARATOR + ifName;
    MptcpEndpointInfo info;
    info.ipAddr = ipAddr;
    info.ifName = ifName;
    info.endpointId = endpointId;
    endpoints_[key] = info;

    NETNATIVE_LOGI("AddEndpoint: success, ip=%{public}s, ifName=%{public}s, id=%{public}d",
                   NetManagerStandard::CommonUtils::ToAnonymousIp(ipAddr).c_str(), ifName.c_str(), endpointId);
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t MptcpManager::DeleteEndpoint(const std::string &ipAddr, const std::string &ifName)
{
    if (ipAddr.empty() || ifName.empty()) {
        NETNATIVE_LOGE("DeleteEndpoint: ipAddr or ifName is empty");
        return NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER;
    }

    std::string key = ipAddr + ENDPOINT_KEY_SEPARATOR + ifName;
    auto it = endpoints_.find(key);
    if (it == endpoints_.end()) {
        NETNATIVE_LOGW("DeleteEndpoint: endpoint not found, ip=%{public}s, ifName=%{public}s",
                       NetManagerStandard::CommonUtils::ToAnonymousIp(ipAddr).c_str(), ifName.c_str());
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

    int32_t endpointId = it->second.endpointId;
    if (endpointId < 0) {
        endpointId = GetEndpointId(ipAddr, ifName);
        if (endpointId < 0) {
            NETNATIVE_LOGE("DeleteEndpoint: failed to get endpoint id");
            endpoints_.erase(it);
            return NetManagerStandard::NETMANAGER_ERR_INTERNAL;
        }
    }

    std::string command = BuildEndpointDeleteCommand(endpointId);
    if (command.empty()) {
        NETNATIVE_LOGE("DeleteEndpoint: build command failed");
        return NetManagerStandard::NETMANAGER_ERR_INTERNAL;
    }

    int32_t ret = ExecuteMptcpCommand(command);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("DeleteEndpoint: execute command failed, ret=%{public}d", ret);
        return ret;
    }

    endpoints_.erase(it);

    NETNATIVE_LOGI("DeleteEndpoint: success, ip=%{public}s, ifName=%{public}s, id=%{public}d",
                   NetManagerStandard::CommonUtils::ToAnonymousIp(ipAddr).c_str(), ifName.c_str(), endpointId);
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t MptcpManager::SetLimits(int32_t subflows, int32_t addAddrAccepted)
{
    if (subflows < 0 || addAddrAccepted < 0) {
        NETNATIVE_LOGE("SetLimits: invalid parameters, subflows=%{public}d, addAddrAccepted=%{public}d",
                       subflows, addAddrAccepted);
        return NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER;
    }

    std::string command = BuildLimitsSetCommand(subflows, addAddrAccepted);
    if (command.empty()) {
        NETNATIVE_LOGE("SetLimits: build command failed");
        return NetManagerStandard::NETMANAGER_ERR_INTERNAL;
    }

    int32_t ret = ExecuteMptcpCommand(command);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("SetLimits: execute command failed, ret=%{public}d", ret);
        return ret;
    }

    currentSubflows_ = subflows;
    currentAddAddrAccepted_ = addAddrAccepted;

    NETNATIVE_LOGI("SetLimits: success, subflows=%{public}d, addAddrAccepted=%{public}d",
                   subflows, addAddrAccepted);
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

void MptcpManager::OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName)
{
    NETNATIVE_LOGI("OnInterfaceAddressUpdated: addr=%{public}s, ifName=%{public}s",
                   NetManagerStandard::CommonUtils::ToAnonymousIp(addr).c_str(), ifName.c_str());

    if (!IsMonitoredInterface(ifName)) {
        NETNATIVE_LOG_D("OnInterfaceAddressUpdated: interface %{public}s is not monitored", ifName.c_str());
        return;
    }

    std::lock_guard<std::mutex> lock(mptcpMutex_);
    auto it = ifaceToIpAddrs_.find(ifName);
    if (it == ifaceToIpAddrs_.end()) {
        ifaceToIpAddrs_[ifName] = std::vector<std::string>();
    }
    
    auto &ipAddrs = ifaceToIpAddrs_[ifName];
    if (std::find(ipAddrs.begin(), ipAddrs.end(), addr) == ipAddrs.end()) {
        ipAddrs.push_back(addr);
    }

    AddEndpoint(addr, ifName);
    UpdateMptcpLimits();
}

void MptcpManager::OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName)
{
    NETNATIVE_LOGI("OnInterfaceAddressRemoved: addr=%{public}s, ifName=%{public}s",
                   NetManagerStandard::CommonUtils::ToAnonymousIp(addr).c_str(), ifName.c_str());

    if (!IsMonitoredInterface(ifName)) {
        NETNATIVE_LOG_D("OnInterfaceAddressRemoved: interface %{public}s is not monitored", ifName.c_str());
        return;
    }

    std::lock_guard<std::mutex> lock(mptcpMutex_);
    auto it = ifaceToIpAddrs_.find(ifName);
    if (it != ifaceToIpAddrs_.end()) {
        auto &ipAddrs = it->second;
        auto addrIt = std::find(ipAddrs.begin(), ipAddrs.end(), addr);
        if (addrIt != ipAddrs.end()) {
            ipAddrs.erase(addrIt);
            DeleteEndpoint(addr, ifName);
            
            if (ipAddrs.empty()) {
                ifaceToIpAddrs_.erase(it);
            }
        }
    }
    UpdateMptcpLimits();
}

bool MptcpManager::IsMonitoredInterface(const std::string &ifName)
{
    return MONITORED_INTERFACES.find(ifName) != MONITORED_INTERFACES.end();
}

int32_t MptcpManager::ExecuteMptcpCommand(const std::string &command)
{
    std::string result;
    return ExecuteMptcpCommand(command, result);
}

int32_t MptcpManager::ExecuteMptcpCommand(const std::string &command, std::string &result)
{
    if (command.empty()) {
        NETNATIVE_LOGE("ExecuteMptcpCommand: command is empty");
        return NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER;
    }

    int32_t ret = NetManagerStandard::CommonUtils::ForkExec(command, &result);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("ExecuteMptcpCommand: execute failed, ret=%{public}d, result=%{public}s",
            ret, result.c_str());
        return NetManagerStandard::NETMANAGER_ERR_INTERNAL;
    }

    return NetManagerStandard::NETMANAGER_SUCCESS;
}

std::string MptcpManager::BuildEndpointAddCommand(const std::string &ipAddr, const std::string &ifName)
{
    std::string command = std::string(IP_CMD_PATH) + OPTION_SPACE + MPTCP_ENDPOINT_ADD + OPTION_SPACE;
    command += ipAddr + OPTION_SPACE;
    command += std::string(MPTCP_DEV_FLAG) + OPTION_SPACE + ifName + OPTION_SPACE + MPTCP_SUBFLOW_FLAG;
    return command;
}

std::string MptcpManager::BuildEndpointDeleteCommand(int32_t endpointId)
{
    if (endpointId < 0) {
        return "";
    }
    std::string command = std::string(IP_CMD_PATH) + OPTION_SPACE + MPTCP_ENDPOINT_DELETE + OPTION_SPACE;
    command += std::to_string(endpointId);
    return command;
}

std::string MptcpManager::BuildLimitsSetCommand(int32_t subflows, int32_t addAddrAccepted)
{
    std::string command = std::string(IP_CMD_PATH) + OPTION_SPACE + MPTCP_LIMITS_SET + OPTION_SPACE;
    command += std::string(MPTCP_SUBFLOWS_PARAM) + OPTION_SPACE + std::to_string(subflows) + OPTION_SPACE;
    command += std::string(MPTCP_ADD_ADDR_ACCEPTED_PARAM) + OPTION_SPACE + std::to_string(addAddrAccepted);
    return command;
}

std::string MptcpManager::BuildEndpointShowCommand()
{
    return std::string(IP_CMD_PATH) + OPTION_SPACE + MPTCP_ENDPOINT_SHOW;
}

int32_t MptcpManager::GetEndpointId(const std::string &ipAddr, const std::string &ifName)
{
    std::string command = BuildEndpointShowCommand();
    std::string result;
    int32_t ret = ExecuteMptcpCommand(command, result);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("GetEndpointId: execute show command failed");
        return INVALID_ENDPOINT_ID;
    }

    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.find(ipAddr) == std::string::npos || line.find(ifName) == std::string::npos) {
            continue;
        }

        size_t idPos = line.find(ID_KEYWORD);
        if (idPos == std::string::npos) {
            continue;
        }

        size_t idStart = idPos + ID_KEYWORD_LEN;
        while (idStart < line.length() && std::isspace(line[idStart])) {
            idStart++;
        }
        if (idStart >= line.length()) {
            continue;
        }

        size_t idEnd = idStart;
        while (idEnd < line.length() && std::isdigit(line[idEnd])) {
            idEnd++;
        }
        if (idEnd == idStart) {
            continue;
        }

        std::string idStr = line.substr(idStart, idEnd - idStart);
        int32_t id = static_cast<int32_t>(std::strtol(idStr.c_str(), nullptr, STRTOL_BASE));
        NETNATIVE_LOG_D("GetEndpointId: found id=%{public}d for ip=%{public}s, ifName=%{public}s",
                        id, NetManagerStandard::CommonUtils::ToAnonymousIp(ipAddr).c_str(), ifName.c_str());
        return id;
    }

    NETNATIVE_LOGW("GetEndpointId: endpoint not found for ip=%{public}s, ifName=%{public}s",
                   NetManagerStandard::CommonUtils::ToAnonymousIp(ipAddr).c_str(), ifName.c_str());
    return INVALID_ENDPOINT_ID;
}

void MptcpManager::UpdateMptcpLimits()
{
    int32_t activeCount = static_cast<int32_t>(ifaceToIpAddrs_.size());
    int32_t subflows = (activeCount > MIN_ACTIVE_INTERFACE_COUNT) ? MPTCP_MAX_SUBFLOWS : MPTCP_DISABLED;
    int32_t addAddrAccepted = (activeCount > MIN_ACTIVE_INTERFACE_COUNT) ? MPTCP_MAX_ADD_ADDR_ACCEPTED : MPTCP_DISABLED;

    if (subflows != currentSubflows_ || addAddrAccepted != currentAddAddrAccepted_) {
        SetLimits(subflows, addAddrAccepted);
    }
}

} // namespace nmd
} // namespace OHOS

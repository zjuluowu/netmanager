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

#ifndef INCLUDE_MPTCP_MANAGER_H
#define INCLUDE_MPTCP_MANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

namespace OHOS {
namespace nmd {

struct MptcpEndpointInfo {
    std::string ipAddr;
    std::string ifName;
    int32_t endpointId = -1;
};

class MptcpManager {
public:
    MptcpManager();
    ~MptcpManager();

    int32_t AddEndpoint(const std::string &ipAddr, const std::string &ifName);
    int32_t DeleteEndpoint(const std::string &ipAddr, const std::string &ifName);
    int32_t SetLimits(int32_t subflows, int32_t addAddrAccepted);

    void OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName);
    void OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName);

    bool IsMonitoredInterface(const std::string &ifName);

private:
    std::mutex mptcpMutex_;
    std::map<std::string, MptcpEndpointInfo> endpoints_;
    std::map<std::string, std::vector<std::string>> ifaceToIpAddrs_;
    int32_t currentSubflows_ = 0;
    int32_t currentAddAddrAccepted_ = 0;

    int32_t ExecuteMptcpCommand(const std::string &command);
    int32_t ExecuteMptcpCommand(const std::string &command, std::string &result);
    std::string BuildEndpointAddCommand(const std::string &ipAddr, const std::string &ifName);
    std::string BuildEndpointDeleteCommand(int32_t endpointId);
    std::string BuildLimitsSetCommand(int32_t subflows, int32_t addAddrAccepted);
    std::string BuildEndpointShowCommand();
    int32_t GetEndpointId(const std::string &ipAddr, const std::string &ifName);
    void UpdateMptcpLimits();
};

} // namespace nmd
} // namespace OHOS

#endif // INCLUDE_MPTCP_MANAGER_H

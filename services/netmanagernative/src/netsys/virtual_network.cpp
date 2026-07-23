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

#include "virtual_network.h"

#include <cinttypes>

#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "route_manager.h"
#include "vpn_manager.h"
#include "vnic_manager.h"
#ifdef SUPPORT_SYSVPN
#include "multi_vpn_manager.h"
#endif

namespace OHOS {
namespace nmd {

VirtualNetwork::VirtualNetwork(uint16_t netId, bool hasDns) : NetsysNetwork(netId), hasDns_(hasDns) {}

bool VirtualNetwork::GetHasDns() const
{
    return hasDns_;
}

int32_t VirtualNetwork::AddUids(const std::vector<UidRange> &uidVec)
{
    std::lock_guard<std::mutex> lock(mutex_);
    NETNATIVE_LOG_D("VirtualNetwork::AddUids update uidRanges_");
    auto middle = uidRanges_.insert(uidRanges_.end(), uidVec.begin(), uidVec.end());
    std::inplace_merge(uidRanges_.begin(), middle, uidRanges_.end()); // restart sort

    for (const auto &interface : interfaces_) {
        if (RouteManager::AddUsersToVirtualNetwork(netId_, interface, uidVec)) {
            NETNATIVE_LOGE("failed to add uids on interface %s of netId %u", interface.c_str(), netId_);
            return NETMANAGER_ERROR;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t VirtualNetwork::RemoveUids(const std::vector<UidRange> &uidVec)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto end =
        std::set_difference(uidRanges_.begin(), uidRanges_.end(), uidVec.begin(), uidVec.end(), uidRanges_.begin());
    uidRanges_.erase(end, uidRanges_.end());

    for (const auto &interface : interfaces_) {
        if (RouteManager::RemoveUsersFromVirtualNetwork(netId_, interface, uidVec)) {
            NETNATIVE_LOGE("failed to remove uids on interface %s of netId %u", interface.c_str(), netId_);
            return NETMANAGER_ERROR;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t VirtualNetwork::AddInterface(std::string &interfaceName)
{
    NETNATIVE_LOGI("Entry VirtualNetwork::AddInterface %{public}s", interfaceName.c_str());
    if (ExistInterface(interfaceName)) {
        NETNATIVE_LOGW("Failed to add interface %{public}s to netId_ %{public}u", interfaceName.c_str(), netId_);
        return NETMANAGER_ERROR;
    }
    int ret = NETMANAGER_SUCCESS;
#ifdef SUPPORT_SYSVPN
    if ((strncmp(interfaceName.c_str(), XFRM_CARD_NAME, strlen(XFRM_CARD_NAME)) == 0) ||
        (strncmp(interfaceName.c_str(), PPP_CARD_NAME, strlen(PPP_CARD_NAME)) == 0) ||
        (strncmp(interfaceName.c_str(), MULTI_TUN_CARD_NAME, strlen(MULTI_TUN_CARD_NAME)) == 0) ||
        (strncmp(interfaceName.c_str(), INNER_CHL_NAME, strlen(INNER_CHL_NAME)) == 0)) {
        ret = MultiVpnManager::GetInstance().CreateVpnInterface(interfaceName);
    } else {
        ret = VpnManager::GetInstance().CreateVpnInterface();
    }
#else
    ret = VpnManager::GetInstance().CreateVpnInterface();
#endif // SUPPORT_SYSVPN
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("create vpn interface error");
        return NETMANAGER_ERROR;
    }

    if (RouteManager::AddInterfaceToVirtualNetwork(netId_, interfaceName)) {
        NETNATIVE_LOGE("Failed to add interface %{public}s to netId_ %{public}u", interfaceName.c_str(), netId_);
        return NETMANAGER_ERROR;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    interfaces_.insert(interfaceName);
    return NETMANAGER_SUCCESS;
}

int32_t VirtualNetwork::RemoveInterface(std::string &interfaceName)
{
    NETNATIVE_LOGI("Entry VirtualNetwork::RemoveInterface %{public}s", interfaceName.c_str());
    if (!ExistInterface(interfaceName)) {
        NETNATIVE_LOGW("Failed to remove interface %{public}s to netId_ %{public}u", interfaceName.c_str(), netId_);
        return NETMANAGER_SUCCESS;
    }

    if (RouteManager::RemoveInterfaceFromVirtualNetwork(netId_, interfaceName)) {
        NETNATIVE_LOGE("Failed to remove interface %{public}s to netId_ %{public}u", interfaceName.c_str(), netId_);
        return NETMANAGER_ERROR;
    }
#ifdef SUPPORT_SYSVPN
    if ((strncmp(interfaceName.c_str(), XFRM_CARD_NAME, strlen(XFRM_CARD_NAME)) == 0) ||
        (strncmp(interfaceName.c_str(), PPP_CARD_NAME, strlen(PPP_CARD_NAME)) == 0) ||
        (strncmp(interfaceName.c_str(), MULTI_TUN_CARD_NAME, strlen(MULTI_TUN_CARD_NAME)) == 0) ||
        (strncmp(interfaceName.c_str(), INNER_CHL_NAME, strlen(INNER_CHL_NAME)) == 0)) {
        MultiVpnManager::GetInstance().DestroyVpnInterface(interfaceName);
    } else {
        VpnManager::GetInstance().DestroyVpnInterface();
    }
#else
    VpnManager::GetInstance().DestroyVpnInterface();
#endif // SUPPORT_SYSVPN
    std::lock_guard<std::mutex> lock(mutex_);
    interfaces_.erase(interfaceName);
    return NETMANAGER_SUCCESS;
}

#ifdef SUPPORT_SYSVPN
int32_t VirtualNetwork::UpdateVpnRules(const std::vector<std::string> &extMessages, bool add)
{
    std::lock_guard<std::mutex> lock(mutex_);
    NETNATIVE_LOGI("VirtualNetwork::UpdateVpnRules netId %{public}d", netId_);
    return RouteManager::UpdateVpnRules(netId_, *interfaces_.begin(), extMessages, add);
}
#endif // SUPPORT_SYSVPN
} // namespace nmd
} // namespace OHOS

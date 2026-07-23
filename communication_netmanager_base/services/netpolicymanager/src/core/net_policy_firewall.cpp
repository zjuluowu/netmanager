/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "net_policy_firewall.h"

#include "ipc_skeleton.h"

#include "firewall_rule.h"
#include "net_policy_core.h"
#include "net_policy_event_handler.h"
#include "net_settings.h"
#include <dlfcn.h>
#include "net_bundle.h"
#include "broadcast_manager.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr const char *NETWORK_POLICY_CHANGED_EVENT = "custom.event.NETWORK_POLICY_CHANGED";
constexpr const char *NETWORK_FIREWALL_POLICY_KEY = "FIREWALL_POLICY_INFO";
constexpr const int32_t HIVIEW_UID = 1201;
constexpr uint32_t MAX_LIST_SIZE = 1000;
void NetPolicyFirewall::Init()
{
    deviceIdleFirewallRule_ = FirewallRule::CreateFirewallRule(FIREWALL_CHAIN_DEVICE_IDLE);
    powerSaveFirewallRule_ = FirewallRule::CreateFirewallRule(FIREWALL_CHAIN_POWER_SAVE);
    idleDenyFirewallRule_ = FirewallRule::CreateFirewallRule(FIREWALL_CHAIN_IDLE_DENY);
    if (deviceIdleFirewallRule_ == nullptr || powerSaveFirewallRule_ == nullptr) {
        return;
    }
    
    std::unique_lock<std::shared_mutex> lock(listMutex_);
    GetFileInst()->ReadFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, deviceIdleAllowedList_, deviceIdleDeniedList_);
    GetFileInst()->ReadFirewallRules(FIREWALL_CHAIN_POWER_SAVE, powerSaveAllowedList_, powerSaveDeniedList_);

    deviceIdleFirewallRule_->SetAllowedList(deviceIdleAllowedList_);
    powerSaveFirewallRule_->SetAllowedList(powerSaveAllowedList_);
}

std::string NetPolicyFirewall::GetBundleNameForUid(uint32_t uid)
{
    std::string LIB_NET_BUNDLE_UTILS_PATH = "libnet_bundle_utils.z.so";
    void *handler = dlopen(LIB_NET_BUNDLE_UTILS_PATH.c_str(), RTLD_LAZY | RTLD_NODELETE);
    // LCOV_EXCL_START
    if (handler == nullptr) {
        return "";
    }
    using GetNetBundleClass = INetBundle *(*)();
    auto getNetBundle = (GetNetBundleClass)dlsym(handler, "GetNetBundle");
    if (getNetBundle == nullptr) {
        dlclose(handler);
        return "";
    }
    auto netBundle = getNetBundle();
    if (netBundle == nullptr) {
        dlclose(handler);
        return "";
    }
    std::optional<SampleBundleInfo> bundleInfo = netBundle->ObtainBundleInfoForUid(uid);
    dlclose(handler);
    if (!bundleInfo.has_value()) {
        return "";
    }
    return bundleInfo.value().bundleName_;
    // LCOV_EXCL_STOP
}
 
std::string NetPolicyFirewall::GetUidsBundleStr(const std::vector<uint32_t> &uids)
{
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < uids.size(); ++i) {
        ss << "\"" << uids[i] << "\"";
        if (i < uids.size() - 1) {
            ss << ",";
        }
    }
    ss << "]";
    return ss.str();
}
 
void NetPolicyFirewall::ReportFirewallPolicyChange(const std::string &callingFnc,
    uint32_t chainType, bool enable, std::shared_ptr<FirewallRule> &rule)
{
    if (rule == nullptr) {
        return;
    }
    uint32_t pid = static_cast<uint32_t>(IPCSkeleton::GetCallingPid());
    uint32_t uid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());
    std::string callingBundleName = GetBundleNameForUid(uid);
    uint64_t curTime = CommonUtils::GetCurrentMilliSecond();
    std::stringstream ss;
    ss << "{\"callingFnc\":\"" << callingFnc << "\",";
    ss << "\"callingBundle\":\"" << callingBundleName << "\",";
    ss << "\"timestamp\":" << curTime << ",";
    ss << "\"callingUid\":" << uid << ",";
    ss << "\"callingPid\":" << pid << ",";
    ss << "\"chainType\":" << chainType << ",";
    ss << "\"enable\":" << (enable ? 1 : 0) << ",";
    ss << "\"allowList\":" << GetUidsBundleStr(rule->GetAllowedList()) << ",";
    ss << "\"deniedList\":" << GetUidsBundleStr(rule->GetDeniedList()) << "}";
    BroadcastInfo info;
    info.action = NETWORK_POLICY_CHANGED_EVENT;
    info.subscriberUid = HIVIEW_UID;
    std::map<std::string, std::string> param = {{NETWORK_FIREWALL_POLICY_KEY, ss.str()}};
    BroadcastManager::GetInstance().SendBroadcast(info, param);
}

int32_t NetPolicyFirewall::SetDeviceIdleTrustlist(const std::vector<uint32_t> &uids, bool isAllowed)
{
    std::unique_lock<std::shared_mutex> lock(listMutex_);
    if (powerSaveAllowedList_.size() > MAX_LIST_SIZE) {
        NETMGR_LOG_E("Device idle allowed list's size is over the max size.");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    UpdateFirewallPolicyList(FIREWALL_CHAIN_DEVICE_IDLE, uids, isAllowed);
    GetFileInst()->WriteFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, deviceIdleAllowedList_, deviceIdleDeniedList_);
    deviceIdleFirewallRule_->SetAllowedList(uids, isAllowed ? FIREWALL_RULE_ALLOW : FIREWALL_RULE_DENY);
    ReportFirewallPolicyChange(__func__, FIREWALL_CHAIN_DEVICE_IDLE, deviceIdleMode_, deviceIdleFirewallRule_);
    std::shared_ptr<PolicyEvent> eventData = std::make_shared<PolicyEvent>();
    eventData->eventId = NetPolicyEventHandler::MSG_DEVICE_IDLE_LIST_UPDATED;
    eventData->deviceIdleList = deviceIdleAllowedList_;
    SendEvent(NetPolicyEventHandler::MSG_DEVICE_IDLE_LIST_UPDATED, eventData);
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyFirewall::SetPowerSaveTrustlist(const std::vector<uint32_t> &uids, bool isAllowed)
{
    std::unique_lock<std::shared_mutex> lock(listMutex_);
    if (powerSaveAllowedList_.size() > MAX_LIST_SIZE) {
        NETMGR_LOG_E("Power save allowed list's size is over the max size.");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    UpdateFirewallPolicyList(FIREWALL_CHAIN_POWER_SAVE, uids, isAllowed);
    GetFileInst()->WriteFirewallRules(FIREWALL_CHAIN_POWER_SAVE, powerSaveAllowedList_, powerSaveDeniedList_);
    if (powerSaveFirewallRule_ == nullptr) {
        NETMGR_LOG_E("powerSaveFirewallRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    powerSaveFirewallRule_->SetAllowedList(uids, isAllowed ? FIREWALL_RULE_ALLOW : FIREWALL_RULE_DENY);
    ReportFirewallPolicyChange(__func__, FIREWALL_CHAIN_POWER_SAVE, powerSaveMode_, powerSaveFirewallRule_);
    std::shared_ptr<PolicyEvent> eventData = std::make_shared<PolicyEvent>();
    eventData->eventId = NetPolicyEventHandler::MSG_POWER_SAVE_LIST_UPDATED;
    eventData->powerSaveList = powerSaveAllowedList_;
    SendEvent(NetPolicyEventHandler::MSG_POWER_SAVE_LIST_UPDATED, eventData);
    return NETMANAGER_SUCCESS;
}

void NetPolicyFirewall::UpdateFirewallPolicyList(uint32_t chainType, const std::vector<uint32_t> &uids, bool isAllowed)
{
    for (auto &uid : uids) {
        if (chainType == FIREWALL_CHAIN_DEVICE_IDLE) {
            if (isAllowed) {
                deviceIdleAllowedList_.emplace(uid);
            } else {
                deviceIdleAllowedList_.erase(uid);
            }
        }

        if (chainType == FIREWALL_CHAIN_POWER_SAVE) {
            if (isAllowed) {
                powerSaveAllowedList_.emplace(uid);
            } else {
                powerSaveAllowedList_.erase(uid);
            }
        }
    }
}

int32_t NetPolicyFirewall::GetDeviceIdleTrustlist(std::vector<uint32_t> &uids)
{
    if (deviceIdleFirewallRule_ == nullptr) {
        NETMGR_LOG_E("deviceIdleFirewallRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    uids = deviceIdleFirewallRule_->GetAllowedList();
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyFirewall::GetPowerSaveTrustlist(std::vector<uint32_t> &uids)
{
    if (powerSaveFirewallRule_ == nullptr) {
        NETMGR_LOG_E("deviceIdleFirewallRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    uids = powerSaveFirewallRule_->GetAllowedList();
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyFirewall::UpdateDeviceIdlePolicy(bool enable)
{
    if (deviceIdleMode_ == enable) {
        NETMGR_LOG_E("Same device idle policy.");
        return NETMANAGER_ERR_STATUS_EXIST;
    }
    if (deviceIdleFirewallRule_ == nullptr) {
        NETMGR_LOG_E("deviceIdleFirewallRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    if (enable) {
        deviceIdleFirewallRule_->SetAllowedList();
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Update device idle firewall status start");
    deviceIdleFirewallRule_->EnableFirewall(enable);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Update device idle firewall status end");
    deviceIdleMode_ = enable;
    ReportFirewallPolicyChange(__func__, FIREWALL_CHAIN_DEVICE_IDLE, enable, deviceIdleFirewallRule_);
    // notify to other core.
    auto policyEvent = std::make_shared<PolicyEvent>();
    policyEvent->deviceIdleMode = enable;
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Notify other policy class device idle status start");
    SendEvent(NetPolicyEventHandler::MSG_DEVICE_IDLE_MODE_CHANGED, policyEvent);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Notify other policy class device idle status end");
    NETMGR_LOG_I("NetPolicyFirewall::UpdateDeviceIdlePolicy End.");
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyFirewall::UpdatePowerSavePolicy(bool enable)
{
    if (powerSaveMode_ == enable) {
        NETMGR_LOG_E("Same power save policy.");
        return NETMANAGER_ERR_STATUS_EXIST;
    }
    if (powerSaveFirewallRule_ == nullptr) {
        NETMGR_LOG_E("powerSaveFirewallRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    if (enable) {
        powerSaveFirewallRule_->SetAllowedList();
    } else {
        powerSaveFirewallRule_->ClearFirewallAllRules();  // power save mode off, clear firewall all reules
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Update power save firewall status start");
    powerSaveFirewallRule_->EnableFirewall(enable);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Update power save firewall status end");
    powerSaveMode_ = enable;
    ReportFirewallPolicyChange(__func__, FIREWALL_CHAIN_POWER_SAVE, enable, powerSaveFirewallRule_);
    // notify to other core.
    auto policyEvent = std::make_shared<PolicyEvent>();
    policyEvent->powerSaveMode = enable;
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Notify other policy class power save status start");
    SendEvent(NetPolicyEventHandler::MSG_POWER_SAVE_MODE_CHANGED, policyEvent);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Notify other policy class power save status end");
    NETMGR_LOG_I("NetPolicyFirewall::UpdatePowerSavePolicy End");
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyFirewall::UpdateIdleDenyPolicy(bool enable)
{
    if (idleDenyMode_ == enable) {
        NETMGR_LOG_E("Same power save policy.");
        return NETMANAGER_ERR_STATUS_EXIST;
    }
    if (idleDenyFirewallRule_ == nullptr) {
        NETMGR_LOG_E("idleDenyFirewallRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Update idle deny firewall status start");
    idleDenyFirewallRule_->EnableFirewall(enable);
    ReportFirewallPolicyChange(__func__, FIREWALL_CHAIN_IDLE_DENY, enable, idleDenyFirewallRule_);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Update idle deny firewall status end");
    idleDenyMode_ = enable;
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyFirewall::SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd)
{
    if (uids.size() > MAX_LIST_SIZE) {
        NETMGR_LOG_E("idle deny list's size is over the max size.");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    if (idleDenyFirewallRule_ == nullptr) {
        NETMGR_LOG_E("idleDenyFirewallRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    idleDenyFirewallRule_->SetDeniedList(uids, isAdd ? FIREWALL_RULE_DENY : FIREWALL_RULE_ALLOW);
    ReportFirewallPolicyChange(__func__, FIREWALL_CHAIN_IDLE_DENY, idleDenyMode_, idleDenyFirewallRule_);
    return NETMANAGER_SUCCESS;
}

void NetPolicyFirewall::ResetPolicies()
{
    if (deviceIdleFirewallRule_ == nullptr) {
        NETMGR_LOG_E("deviceIdleFirewallRule_ is nullptr");
        return ;
    }
    deviceIdleFirewallRule_->ClearAllowedList();
    deviceIdleFirewallRule_->ClearDeniedList();

    if (powerSaveFirewallRule_ == nullptr) {
        NETMGR_LOG_E("powerSaveFirewallRule_ is nullptr");
        return ;
    }
    powerSaveFirewallRule_->ClearAllowedList();
    powerSaveFirewallRule_->ClearDeniedList();

    if (idleDenyFirewallRule_ == nullptr) {
        NETMGR_LOG_E("idleDenyFirewallRule_ is nullptr");
        return ;
    }
    idleDenyFirewallRule_->ClearDeniedList();

    std::unique_lock<std::shared_mutex> lock(listMutex_);
    deviceIdleAllowedList_.clear();
    deviceIdleDeniedList_.clear();
    GetFileInst()->WriteFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, deviceIdleAllowedList_, deviceIdleDeniedList_);

    powerSaveAllowedList_.clear();
    powerSaveDeniedList_.clear();
    GetFileInst()->WriteFirewallRules(FIREWALL_CHAIN_POWER_SAVE, powerSaveAllowedList_, powerSaveDeniedList_);

    UpdateDeviceIdlePolicy(false);
    UpdatePowerSavePolicy(false);
    UpdateIdleDenyPolicy(false);
}

void NetPolicyFirewall::DeleteUid(uint32_t uid)
{
    SetDeviceIdleTrustlist({uid}, false);
    SetPowerSaveTrustlist({uid}, false);

    deviceIdleFirewallRule_->RemoveFromAllowedList(uid);
    powerSaveFirewallRule_->RemoveFromAllowedList(uid);
    NETMGR_LOG_I("NetPolicyFirewall::DeleteUid End");
}

void NetPolicyFirewall::HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent)
{
    switch (eventId) {
        case NetPolicyEventHandler::MSG_UID_REMOVED:
            DeleteUid(policyEvent->deletedUid);
            break;
        case NetPolicyEventHandler::MSG_POWER_SAVE_MODE_CHANGED:
            UpdatePowerSavePolicy(policyEvent->powerSaveMode);
            break;
        default:
            break;
    }
}
} // namespace NetManagerStandard
} // namespace OHOS

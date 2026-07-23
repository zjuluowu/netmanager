/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "net_policy_rule.h"

#include "net_mgr_log_wrapper.h"
#include "iptables_type.h"

namespace OHOS {
namespace NetManagerStandard {
NetPolicyRule::NetPolicyRule() = default;

void NetPolicyRule::Init()
{
    // Init uid、policy and background allow status from file,and save uid、policy into uidPolicyRules_.
    NETMGR_LOG_I("Start init uid and policy.");
    const auto &uidsPolicies = GetFileInst()->ReadUidPolicies();
    backgroundAllow_ = GetFileInst()->ReadBackgroundPolicy();
    for (const auto &i : uidsPolicies) {
        auto uid = CommonUtils::StrToUint(i.uid.c_str());
        auto policy = CommonUtils::StrToUint(i.policy.c_str());
        TransPolicyToRule(uid, policy);
    }
}

void NetPolicyRule::TransPolicyToRule()
{
    // When system status is changed,traverse uidPolicyRules_ to calculate the rule and netsys.
    std::shared_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    for (auto &[uid, policy] : uidPolicyRules_) {
        TransConditionToRuleAndNetsys(uid, policy);
    }
}

void NetPolicyRule::TransPolicyToRule(uint32_t uid)
{
    std::shared_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    auto policyRule = uidPolicyRules_.find(uid);
    if (policyRule == uidPolicyRules_.end()) {
        return;
    }
    NETMGR_LOG_D("TransPolicyToRule only with uid value: uid[%{public}u] policy[%{public}u]", uid,
        policyRule->second.policy_);
    TransConditionToRuleAndNetsys(uid, policyRule->second);
}

int32_t NetPolicyRule::TransPolicyToRule(uint32_t uid, uint32_t policy)
{
    if (!IsValidNetPolicy(policy)) {
        NETMGR_LOG_E("policy[%{public}d] is invalid", policy);
        return POLICY_ERR_INVALID_POLICY;
    }
    std::unique_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    auto policyRule = uidPolicyRules_.find(uid);
    if (policyRule == uidPolicyRules_.end()) {
        NETMGR_LOG_D("Don't find this uid, need to add uid:[%{public}u] policy[%{public}u].", uid, policy);
        uidPolicyRules_[uid] = {.policy_ = policy};
        GetCbInst()->NotifyNetUidPolicyChange(uid, policy);
    } else {
        if (policyRule->second.policy_ != policy) {
            NETMGR_LOG_D("Update policy's value.uid:[%{public}u] policy[%{public}u]", uid, policy);
            policyRule->second.policy_ = policy;
            GetCbInst()->NotifyNetUidPolicyChange(uid, policy);
        }
    }

    TransConditionToRuleAndNetsys(uid, uidPolicyRules_[uid]);
    NETMGR_LOG_D("End TransPolicyToRule");
    return NETMANAGER_SUCCESS;
}

uint32_t NetPolicyRule::BuildTransCondition(uint32_t uid, uint32_t policy)
{
    // Integrate status values, the result of policyCondition will be use in TransConditionToRuleAndNetsys.
    uint32_t policyCondition = ChangePolicyToPolicyTransitionCondition(policy);

    if (IsIdleMode()) {
        policyCondition |= POLICY_TRANS_CONDITION_IDLE_MODE;
    }
    if (InIdleAllowedList(uid)) {
        policyCondition |= POLICY_TRANS_CONDITION_IDLE_ALLOWEDLIST;
    }
    if (IsLimitByAdmin()) {
        policyCondition |= POLICY_TRANS_CONDITION_ADMIN_RESTRICT;
    }
    if (IsPowerSave()) {
        policyCondition |= POLICY_TRANS_CONDITION_POWERSAVE_MODE;
    }
    if (InPowerSaveAllowedList(uid)) {
        policyCondition |= POLICY_TRANS_CONDITION_POWERSAVE_ALLOWEDLIST;
    }
    if (IsLimitedBackground()) {
        policyCondition |= POLICY_TRANS_CONDITION_BACKGROUND_RESTRICT;
    }
    if (IsForeground(uid)) {
        policyCondition |= POLICY_TRANS_CONDITION_FOREGROUND;
    }
    NETMGR_LOG_D("BuildTransCondition uid[%{public}u] policy[%{public}u]", uid, policy);
    return policyCondition;
}

void NetPolicyRule::TransConditionToRuleAndNetsys(uint32_t uid, UidPolicyRule& policyRule)
{
    NetmanagerHiTrace::NetmanagerStartSyncTrace("TransPolicyToRule start");
    auto policyCondition = BuildTransCondition(uid, policyRule.policy_);
    uint32_t conditionValue = GetMatchTransCondition(policyCondition);
    auto rule = MoveToRuleBit(conditionValue & POLICY_TRANS_RULE_MASK);
    NETMGR_LOG_D("NetPolicyRule->uid:[%{public}u] policy:[%{public}u] rule:[%{public}u] policyCondition[%{public}u]",
                 uid, policyRule.policy_, rule, policyCondition);
    auto netsys = conditionValue & POLICY_TRANS_NET_CTRL_MASK;

    if (policyRule.netsys_ != netsys) {
        NetsysCtrl(uid, netsys);
        policyRule.netsys_ = netsys;
    } else {
        NETMGR_LOG_I("Same netsys and uid ,don't need to do others.now netsys is: [%{public}u]", netsys);
    }

    GetFileInst()->WritePolicyByUid(uid, policyRule.policy_);

    if (policyRule.rule_ == rule) {
        NETMGR_LOG_D("Same rule and uid ,don't need to do others.uid is:[%{public}u] rule is:[%{public}u]", uid, rule);
    } else {
        policyRule.rule_ = rule;
        GetCbInst()->NotifyNetUidRuleChange(uid, rule);
    }
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("TransPolicyToRule end");
}

uint32_t NetPolicyRule::GetMatchTransCondition(uint32_t policyCondition)
{
    for (const auto &i : POLICY_TRANS_MAP) {
        auto condition = MoveToConditionBit(i & POLICY_TRANS_CONDITION_MASK);
        if ((policyCondition & condition) == condition) {
            NETMGR_LOG_D("GetMatchTransCondition condition: %{public}d.", i);
            return i;
        }
    }
    return POLICY_TRANS_MAP.back();
}

void NetPolicyRule::NetsysCtrl(uint32_t uid, uint32_t netsysCtrl)
{
    switch (netsysCtrl) {
        case POLICY_TRANS_CTRL_NONE:
            ProcessCtrlNone(uid);
            break;
        case POLICY_TRANS_CTRL_REMOVE_ALL:
            GetNetsysInst()->BandwidthRemoveAllowedList(uid);
            GetNetsysInst()->BandwidthRemoveDeniedList(uid);
            break;
        case POLICY_TRANS_CTRL_ADD_DENIEDLIST:
            GetNetsysInst()->BandwidthAddDeniedList(uid);
            GetNetsysInst()->BandwidthRemoveAllowedList(uid);
            break;
        case POLICY_TRANS_CTRL_ADD_ALLOWEDLIST:
            ProcessCtrlAddAllowedList(uid);
            break;
        default:
            NETMGR_LOG_E("Error netsysCtrl value, need to check");
            break;
    }
    NETMGR_LOG_D("uid:[%{public}u]   netsysCtrl: [%{public}u]", uid, netsysCtrl);
}

void NetPolicyRule::ProcessCtrlNone(uint32_t uid)
{
    if (IsPowerSave()) {
        GetNetsysInst()->PowerSaveUpdataAllowedList(uid, FirewallRule::RULE_DENY);
    } else {
        NETMGR_LOG_D("Don't need to do anything,keep now status.");
    }
}

void NetPolicyRule::ProcessCtrlAddAllowedList(uint32_t uid)
{
    GetNetsysInst()->BandwidthRemoveDeniedList(uid);
    GetNetsysInst()->BandwidthAddAllowedList(uid);
    if (IsPowerSave()) {
        GetNetsysInst()->PowerSaveUpdataAllowedList(uid, FirewallRule::RULE_ALLOW);
    }
}

uint32_t NetPolicyRule::MoveToConditionBit(uint32_t value)
{
    return value >> CONDITION_START_BIT;
}

uint32_t NetPolicyRule::MoveToRuleBit(uint32_t value)
{
    return value >> RULE_START_BIT;
}

uint32_t NetPolicyRule::ChangePolicyToPolicyTransitionCondition(uint32_t policy)
{
    if (policy == NET_POLICY_NONE) {
        return POLICY_TRANS_CONDITION_UID_POLICY_NONE;
    }
    return policy << 1;
}

int32_t NetPolicyRule::GetPolicyByUid(uint32_t uid, uint32_t &policy)
{
    std::shared_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    auto policyRule = uidPolicyRules_.find(uid);
    if (policyRule == uidPolicyRules_.end()) {
        NETMGR_LOG_D("Can't find uid:[%{public}u] and its policy, return default value.", uid);
        policy = NET_POLICY_NONE;
        return NETMANAGER_SUCCESS;
    }
    policy = policyRule->second.policy_;
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyRule::GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids)
{
    if (!IsValidNetPolicy(policy)) {
        return POLICY_ERR_INVALID_POLICY;
    }
    NETMGR_LOG_I("GetUidsByPolicy:policy:[%{public}u]", policy);
    std::shared_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    for (auto &iter : uidPolicyRules_) {
        if (iter.second.policy_ == policy) {
            uids.push_back(iter.first);
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyRule::IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed)
{
    NETMGR_LOG_D("IsUidNetAllowed:uid[%{public}u] metered:[%{public}d]", uid, metered);
    uint32_t rule = NetUidRule::NET_RULE_NONE;
    std::shared_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    auto iter = uidPolicyRules_.find(uid);
    if (iter != uidPolicyRules_.end()) {
        rule = iter->second.rule_;
    }
    NETMGR_LOG_I("IsUidNetAllowed:rule[%{public}u], backgroundAllow_[%{public}d]", rule, backgroundAllow_);
    if (rule == NetUidRule::NET_RULE_REJECT_ALL) {
        isAllowed = false;
        return NETMANAGER_SUCCESS;
    }

    if (!metered) {
        isAllowed = true;
        return NETMANAGER_SUCCESS;
    }

    if (rule == NetUidRule::NET_RULE_REJECT_METERED) {
        isAllowed = false;
        return NETMANAGER_SUCCESS;
    }

    if (rule == NetUidRule::NET_RULE_ALLOW_METERED) {
        isAllowed = true;
        return NETMANAGER_SUCCESS;
    }

    if (rule == NetUidRule::NET_RULE_ALLOW_METERED_FOREGROUND) {
        isAllowed = true;
        return NETMANAGER_SUCCESS;
    }

    if (!backgroundAllow_) {
        isAllowed = false;
        return NETMANAGER_SUCCESS;
    }

    isAllowed = true;
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyRule::SetBackgroundPolicy(bool allow)
{
    if (backgroundAllow_ != allow) {
        GetCbInst()->NotifyNetBackgroundPolicyChange(allow);
        backgroundAllow_ = allow;
        TransPolicyToRule();
        GetFileInst()->WriteBackgroundPolicy(allow);
        NetmanagerHiTrace::NetmanagerStartSyncTrace("SetBackgroundPolicy policy start");
        GetNetsysInst()->BandwidthEnableDataSaver(!allow);
        NetmanagerHiTrace::NetmanagerFinishSyncTrace("SetBackgroundPolicy policy end");
        return NETMANAGER_SUCCESS;
    }
    NETMGR_LOG_I("Same background policy,don't need to repeat set. now background policy is:[%{public}d]", allow);
    return NETMANAGER_ERR_PARAMETER_ERROR;
}

int32_t NetPolicyRule::GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid)
{
    uint32_t policy = 0;
    GetPolicyByUid(uid, policy);
    NETMGR_LOG_D("GetBackgroundPolicyByUid GetPolicyByUid uid: %{public}u policy: %{public}u.", uid, policy);
    if ((policy & NET_POLICY_REJECT_METERED_BACKGROUND) != 0) {
        backgroundPolicyOfUid = NET_BACKGROUND_POLICY_DISABLE;
        return NETMANAGER_SUCCESS;
    }

    if (backgroundAllow_) {
        backgroundPolicyOfUid = NET_BACKGROUND_POLICY_ENABLE;
        return NETMANAGER_SUCCESS;
    }

    if ((policy & NET_POLICY_ALLOW_METERED_BACKGROUND) != 0) {
        backgroundPolicyOfUid = NET_BACKGROUND_POLICY_TRUSTLIST;
        return NETMANAGER_SUCCESS;
    }
    backgroundPolicyOfUid = NET_BACKGROUND_POLICY_DISABLE;
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyRule::ResetPolicies()
{
    NETMGR_LOG_I("Reset uids-policies and backgroundpolicy");
    std::shared_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    auto uidPolicyRulesBak = uidPolicyRules_;
    lock.unlock();
    for (auto iter : uidPolicyRulesBak) {
        TransPolicyToRule(iter.first, NetUidPolicy::NET_POLICY_NONE);
    }
    return SetBackgroundPolicy(true);
}

int32_t NetPolicyRule::GetBackgroundPolicy(bool &backgroundPolicy)
{
    NETMGR_LOG_I("GetBackgroundPolicy:backgroundAllow_[%{public}d", backgroundAllow_);
    backgroundPolicy = backgroundAllow_;
    return NETMANAGER_SUCCESS;
}

bool NetPolicyRule::IsIdleMode()
{
    NETMGR_LOG_D("IsIdleMode:deviceIdleMode_[%{public}d", deviceIdleMode_);
    return deviceIdleMode_;
}

bool NetPolicyRule::InIdleAllowedList(uint32_t uid)
{
    std::shared_lock<std::shared_mutex> lock(deviceIdleAllowedListMutex_);
    if (std::find(deviceIdleAllowedList_.begin(), deviceIdleAllowedList_.end(), uid) != deviceIdleAllowedList_.end()) {
        return true;
    }
    return false;
}

bool NetPolicyRule::IsLimitByAdmin()
{
    // to judge if limit by admin.
    return false;
}

bool NetPolicyRule::IsForeground(uint32_t uid)
{
    std::lock_guard lock(foregroundUidListMutex_);
    return std::find(foregroundUidList_.begin(), foregroundUidList_.end(), uid) != foregroundUidList_.end();
}

bool NetPolicyRule::IsPowerSave()
{
    return powerSaveMode_;
}

bool NetPolicyRule::InPowerSaveAllowedList(uint32_t uid)
{
    return std::find(powerSaveAllowedList_.begin(), powerSaveAllowedList_.end(), uid) != powerSaveAllowedList_.end();
}

bool NetPolicyRule::IsLimitedBackground()
{
    return !backgroundAllow_;
}

void NetPolicyRule::DeleteUid(uint32_t uid)
{
    NETMGR_LOG_D("DeleteUid:uid[%{public}u]", uid);

    std::unique_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    const auto &it = std::find_if(uidPolicyRules_.begin(), uidPolicyRules_.end(),
                                  [&uid](const auto &pair) { return pair.first == uid; });
    if (it != uidPolicyRules_.end()) {
        uidPolicyRules_.erase(it);
    }
    lock.unlock();
    GetFileInst()->RemoveInexistentUid(uid);
    GetNetsysInst()->BandwidthRemoveDeniedList(uid);
    GetNetsysInst()->BandwidthRemoveAllowedList(uid);
}

void NetPolicyRule::HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent)
{
    switch (eventId) {
        case NetPolicyEventHandler::MSG_DEVICE_IDLE_LIST_UPDATED: {
            std::unique_lock<std::shared_mutex> lock(deviceIdleAllowedListMutex_);
            deviceIdleAllowedList_ = policyEvent->deviceIdleList;
            break;
        }
        case NetPolicyEventHandler::MSG_DEVICE_IDLE_MODE_CHANGED:
            deviceIdleMode_ = policyEvent->deviceIdleMode;
            TransPolicyToRule();
            break;
        case NetPolicyEventHandler::MSG_POWER_SAVE_MODE_CHANGED:
            powerSaveMode_ = policyEvent->powerSaveMode;
            TransPolicyToRule();
            break;
        case NetPolicyEventHandler::MSG_POWER_SAVE_LIST_UPDATED:
            powerSaveAllowedList_ = policyEvent->powerSaveList;
            break;
        case NetPolicyEventHandler::MSG_UID_REMOVED:
            DeleteUid(policyEvent->deletedUid);
            break;
        case NetPolicyEventHandler::MSG_UID_STATE_FOREGROUND:
            UpdateForegroundUidList(policyEvent->uid, true);
            TransPolicyToRule(policyEvent->uid);
            break;
        case NetPolicyEventHandler::MSG_UID_STATE_BACKGROUND:
            UpdateForegroundUidList(policyEvent->uid, false);
            TransPolicyToRule(policyEvent->uid);
            break;
        default:
            break;
    }
}

void NetPolicyRule::UpdateForegroundUidList(uint32_t uid, bool isForeground)
{
    std::lock_guard lock(foregroundUidListMutex_);
    if (isForeground) {
        foregroundUidList_.insert(uid);
        return;
    }
    foregroundUidList_.erase(uid);
}

bool NetPolicyRule::IsValidNetPolicy(uint32_t policy)
{
    switch (policy) {
        case NetUidPolicy::NET_POLICY_NONE:
        case NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND:
        case NetUidPolicy::NET_POLICY_REJECT_METERED_BACKGROUND: {
            return true;
        }
        default: {
            NETMGR_LOG_E("Invalid policy [%{public}d]", policy);
            return false;
        }
    }
}

void NetPolicyRule::GetDumpMessage(std::string &message)
{
    static const std::string TAB = "    ";
    message.append(TAB + "UidPolicy:\n");
    std::shared_lock<std::shared_mutex> lock(uidPolicyRuleMutex_);
    std::for_each(uidPolicyRules_.begin(), uidPolicyRules_.end(), [&message](const auto &pair) {
        message.append(TAB + TAB + "Uid: " + std::to_string(pair.first) + TAB + "Rule: " +
                       std::to_string(pair.second.rule_) + TAB + "Policy:" + std::to_string(pair.second.policy_) + TAB +
                       "NetSys: " + std::to_string(pair.second.netsys_) + "\n");
    });
    message.append(TAB + "DeviceIdleAllowedList: {");
    std::shared_lock<std::shared_mutex> deviceIdleAllowedListLock(deviceIdleAllowedListMutex_);
    std::for_each(deviceIdleAllowedList_.begin(), deviceIdleAllowedList_.end(),
                  [&message](const auto &item) { message.append(std::to_string(item) + ", "); });
    deviceIdleAllowedListLock.unlock();
    message.append("}\n");
    message.append(TAB + "DeviceIdleMode: " + std::to_string(deviceIdleMode_) + "\n");
    message.append(TAB + "PowerSaveAllowedList: {");
    std::for_each(powerSaveAllowedList_.begin(), powerSaveAllowedList_.end(),
                  [&message](const auto &item) { message.append(std::to_string(item) + ", "); });
    message.append(TAB + "PowerSaveMode: " + std::to_string(powerSaveMode_) + "\n");
    message.append(TAB + "BackgroundPolicy: " + std::to_string(backgroundAllow_) + "\n");
}

int32_t NetPolicyRule::SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag)
{
    return GetNetsysInst()->SetNetworkAccessPolicy(uid, policy, reconfirmFlag);
}

int32_t NetPolicyRule::DeleteNetworkAccessPolicy(uint32_t uid)
{
    return GetNetsysInst()->DeleteNetworkAccessPolicy(uid);
}

int32_t NetPolicyRule::PolicySetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status)
{
    return GetNetsysInst()->SetNicTrafficAllowed(ifaceNames, status);
}
} // namespace NetManagerStandard
} // namespace OHOS

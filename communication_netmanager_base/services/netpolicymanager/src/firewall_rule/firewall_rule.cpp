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

#include "firewall_rule.h"

#include "device_idle_firewall_rule.h"
#include "net_policy_inner_define.h"
#include "power_save_firewall_rule.h"
#include "idle_deny_firewall_rule.h"

namespace OHOS {
namespace NetManagerStandard {
std::shared_ptr<FirewallRule> FirewallRule::CreateFirewallRule(uint32_t chain)
{
    switch (chain) {
        case FIREWALL_CHAIN_DEVICE_IDLE:
            return DelayedSingleton<DeviceIdleFirewallRule>::GetInstance();
        case FIREWALL_CHAIN_POWER_SAVE:
            return DelayedSingleton<PowerSaveFirewallRule>::GetInstance();
        case FIREWALL_CHAIN_IDLE_DENY:
            return std::make_shared<IdleDenyFirewallRule>();
        default:
            break;
    }
    return nullptr;
}

FirewallRule::FirewallRule(uint32_t chainType)
{
    chainType_ = chainType;
    netsys_ = DelayedSingleton<NetsysPolicyWrapper>::GetInstance();
}

FirewallRule::~FirewallRule() = default;

std::vector<uint32_t> FirewallRule::GetAllowedList()
{
    std::shared_lock<std::shared_mutex> lock(allowedListMutex_);
    return allowedList_;
}

void FirewallRule::SetAllowedList(const std::vector<uint32_t> &uids, uint32_t rule)
{
    for (auto &uid : uids) {
        SetAllowedList(uid, rule);
    }
    netsys_->FirewallSetUidRule(chainType_, uids, rule);
}

void FirewallRule::SetAllowedList(uint32_t uid, uint32_t rule)
{
    std::unique_lock<std::shared_mutex> lock(allowedListMutex_);
    if (rule == FIREWALL_RULE_ALLOW) {
        if (std::find(allowedList_.begin(), allowedList_.end(), uid) == allowedList_.end()) {
            allowedList_.emplace_back(uid);
        }
    } else {
        for (auto iter = allowedList_.begin(); iter != allowedList_.end();) {
            if (uid == *iter) {
                allowedList_.erase(iter);
                break;
            } else {
                iter++;
            }
        }
    }
}

void FirewallRule::SetAllowedList(const std::set<uint32_t> &uids)
{
    {
        std::unique_lock<std::shared_mutex> lock(allowedListMutex_);
        for (const auto &it : uids) {
            if (std::find(allowedList_.begin(), allowedList_.end(), it) == allowedList_.end()) {
                allowedList_.push_back(it);
            }
        }
    }

    SetAllowedList();
}

void FirewallRule::SetAllowedList()
{
    std::shared_lock<std::shared_mutex> lock(allowedListMutex_);
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Set allowed list start");
    netsys_->FirewallSetUidsAllowedListChain(chainType_, allowedList_);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Set allowed list end");
}

void FirewallRule::ClearAllowedList()
{
    std::unique_lock<std::shared_mutex> lock(allowedListMutex_);
    allowedList_.clear();
    netsys_->FirewallSetUidsAllowedListChain(chainType_, allowedList_);
}

std::vector<uint32_t> FirewallRule::GetDeniedList()
{
    std::shared_lock<std::shared_mutex> lock(deniedListMutex_);
    return deniedList_;
}

void FirewallRule::SetDeniedList(uint32_t uid, uint32_t rule)
{
    std::unique_lock<std::shared_mutex> lock(deniedListMutex_);
    if (rule == FIREWALL_RULE_DENY) {
        if (std::find(deniedList_.begin(), deniedList_.end(), uid) == deniedList_.end()) {
            deniedList_.emplace_back(uid);
        }
    } else {
        for (auto iter = deniedList_.begin(); iter != deniedList_.end();) {
            if (uid == *iter) {
                iter = deniedList_.erase(iter);
            } else {
                iter++;
            }
        }
    }
    lock.unlock();
    netsys_->FirewallSetUidRule(chainType_, {uid}, rule);
}

void FirewallRule::SetDeniedList(const std::vector<uint32_t> &uids, uint32_t rule)
{
    {
        std::unique_lock<std::shared_mutex> lock(deniedListMutex_);
        for (const auto &it : uids) {
            if (rule == FIREWALL_RULE_DENY &&
                std::find(deniedList_.begin(), deniedList_.end(), it) == deniedList_.end()) {
                deniedList_.push_back(it);
            } else {
                deniedList_.erase(std::remove(deniedList_.begin(), deniedList_.end(), it), deniedList_.end());
            }
        }
    }

    SetDeniedList();
}

void FirewallRule::SetDeniedList()
{
    std::shared_lock<std::shared_mutex> lock(deniedListMutex_);
    netsys_->FirewallSetUidsDeniedListChain(chainType_, deniedList_);
}

void FirewallRule::ClearDeniedList()
{
    std::unique_lock<std::shared_mutex> lock(deniedListMutex_);
    deniedList_.clear();
    netsys_->FirewallSetUidsDeniedListChain(chainType_, deniedList_);
}

void FirewallRule::SetUidFirewallRule(uint uid, bool isAllowed)
{
    netsys_->FirewallSetUidRule(chainType_, {uid}, isAllowed ? FIREWALL_RULE_ALLOW : FIREWALL_RULE_DENY);
}

void FirewallRule::EnableFirewall(bool enable)
{
    netsys_->FirewallEnableChain(chainType_, enable);
}

void FirewallRule::RemoveFromAllowedList(uint32_t uid)
{
    std::unique_lock<std::shared_mutex> lock(allowedListMutex_);
    for (auto iter = allowedList_.begin(); iter != allowedList_.end(); ++iter) {
        if (*iter == uid) {
            allowedList_.erase(iter);
            break;
        }
    }
}

int32_t FirewallRule::ClearFirewallAllRules()
{
    return netsys_->ClearFirewallAllRules();
}

void FirewallRule::RemoveFromDeniedList(uint32_t uid)
{
    std::unique_lock<std::shared_mutex> lock(deniedListMutex_);
    for (auto iter = deniedList_.begin(); iter != deniedList_.end(); ++iter) {
        if (*iter == uid) {
            deniedList_.erase(iter);
            break;
        }
    }
    netsys_->FirewallSetUidsDeniedListChain(chainType_, deniedList_);
}
} // namespace NetManagerStandard
} // namespace OHOS

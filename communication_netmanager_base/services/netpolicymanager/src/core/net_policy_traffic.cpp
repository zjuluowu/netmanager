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

#include "net_policy_traffic.h"

#include "broadcast_manager.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_publish_info.h"
#include "common_event_support.h"
#include "system_ability_definition.h"

#include "net_manager_center.h"
#include "net_mgr_log_wrapper.h"
#include "net_policy_constants.h"
#include "net_policy_file.h"
#include "net_policy_inner_define.h"
#include "net_quota_policy.h"
#include "net_specifier.h"
#include "net_stats_info.h"
#include "netmanager_base_common_utils.h"
#include "netmanager_base_permission.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *BROADCAST_QUOTA_WARNING = "Net Policy Quota Warning";
constexpr const char *BROADCAST_QUOTA_LIMIT_REMIND = "Net Policy Quota Limit Remind";
constexpr const char *BROADCAST_QUOTA_LIMIT = "Net Policy Quota Limit";
} // namespace

void NetPolicyTraffic::Init()
{
    netsysCallback_ = new (std::nothrow)
        NetsysControllerCallbackImpl((std::static_pointer_cast<NetPolicyTraffic>(shared_from_this())));
    if (netsysCallback_ != nullptr) {
        GetNetsysInst()->RegisterNetsysCallback(netsysCallback_);
    }

    netConnCallback_ =
        new (std::nothrow) ConnCallBack((std::static_pointer_cast<NetPolicyTraffic>(shared_from_this())));
    if (netConnCallback_ != nullptr) {
        GetNetCenterInst().RegisterNetConnCallback(netConnCallback_);
    }
    ReadQuotaPolicies();
}

bool NetPolicyTraffic::IsValidQuotaPolicy(const NetQuotaPolicy &quotaPolicy)
{
    int32_t netType = quotaPolicy.networkmatchrule.netType;
    if (!IsValidNetType(netType)) {
        NETMGR_LOG_E("NetPolicyType is invalid policy[%{public}d]", netType);
        return false;
    }

    if (!IsValidPeriodDuration(quotaPolicy.quotapolicy.periodDuration)) {
        NETMGR_LOG_E("periodDuration [%{public}s] must Mx", quotaPolicy.quotapolicy.periodDuration.c_str());
        return false;
    }
    return true;
}

bool NetPolicyTraffic::IsValidNetType(int32_t netType)
{
    switch (netType) {
        case NetBearType::BEARER_CELLULAR:
        case NetBearType::BEARER_WIFI:
        case NetBearType::BEARER_BLUETOOTH:
        case NetBearType::BEARER_ETHERNET:
        case NetBearType::BEARER_VPN:
        case NetBearType::BEARER_WIFI_AWARE: {
            return true;
        }
        default: {
            NETMGR_LOG_E("Invalid netType [%{public}d]", netType);
            return false;
        }
    }
}

bool NetPolicyTraffic::IsValidNetRemindType(uint32_t remindType)
{
    switch (remindType) {
        case RemindType::REMIND_TYPE_WARNING:
        case RemindType::REMIND_TYPE_LIMIT: {
            return true;
        }
        default: {
            NETMGR_LOG_E("Invalid remindType [%{public}d]", remindType);
            return false;
        }
    }
}

int32_t NetPolicyTraffic::UpdateQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    if (quotaPolicies.empty()) {
        NETMGR_LOG_E("SetNetQuotaPolicies size is empty");
        return POLICY_ERR_INVALID_QUOTA_POLICY;
    }
    // formalize the quota policy
    NetmanagerHiTrace::NetmanagerStartSyncTrace("FormalizeQuotaPolicies quotaPolicies start");
    FormalizeQuotaPolicies(quotaPolicies);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("FormalizeQuotaPolicies quotaPolicies end");
    return UpdateQuotaPoliciesInner();
}

int32_t NetPolicyTraffic::UpdateQuotaPoliciesInner()
{
    // calculate the quota remain and get the metered ifaces
    NetmanagerHiTrace::NetmanagerStartSyncTrace("UpdateMeteredIfacesQuota start");
    auto meteredIfaces = UpdateMeteredIfacesQuota();
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("UpdateMeteredIfacesQuota end");

    // update the metered ifaces and notify the changes.
    NetmanagerHiTrace::NetmanagerStartSyncTrace("UpdateMeteredIfaces meteredIfaces start");
    UpdateMeteredIfaces(meteredIfaces);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("UpdateMeteredIfaces meteredIfaces end");

    // notify quota limit or warning.
    NetmanagerHiTrace::NetmanagerStartSyncTrace("UpdateQuotaNotify start");
    UpdateQuotaNotify();
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("UpdateQuotaNotify end");
    // write quota policies to file.
    if (!WriteQuotaPolicies()) {
        NETMGR_LOG_E("UpdateQuotaPolicies WriteFile failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    // notify the the quota policy change.
    std::shared_lock<std::shared_mutex> lock(quotaMutex_);
    GetCbInst()->NotifyNetQuotaPolicyChange(quotaPolicies_);
    NETMGR_LOG_I("End UpdateQuotaPoliciesInner.");
    return NETMANAGER_SUCCESS;
}

void NetPolicyTraffic::FormalizeQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    std::unique_lock<std::shared_mutex> lock(quotaMutex_);
    quotaPolicies_.clear();
    for (auto quotaPolicy : quotaPolicies) {
        if (!IsValidQuotaPolicy(quotaPolicy)) {
            NETMGR_LOG_E("UpdateQuotaPolicies invalid quota policy netType[%{public}d], periodDuration[%{public}s]",
                         quotaPolicy.networkmatchrule.netType, quotaPolicy.quotapolicy.periodDuration.c_str());
            continue;
        }
        if (quotaPolicy.quotapolicy.limitBytes == DATA_USAGE_UNKNOWN) {
            quotaPolicy.quotapolicy.limitAction = LIMIT_ACTION_ALERT_ONLY;
        } else if (quotaPolicy.quotapolicy.warningBytes == DATA_USAGE_UNKNOWN) {
            quotaPolicy.quotapolicy.warningBytes =
                quotaPolicy.quotapolicy.limitBytes * NINETY_PERCENTAGE / HUNDRED_PERCENTAGE;
        }
        if (quotaPolicy.quotapolicy.limitAction == LIMIT_ACTION_ALERT_ONLY) {
            quotaPolicy.quotapolicy.limitBytes = DATA_USAGE_UNLIMITED;
        }
        if (quotaPolicy.quotapolicy.warningBytes > quotaPolicy.quotapolicy.limitBytes) {
            quotaPolicy.quotapolicy.warningBytes = DATA_USAGE_UNLIMITED;
        }
        if (quotaPolicy.quotapolicy.limitBytes == DATA_USAGE_UNLIMITED) {
            quotaPolicy.quotapolicy.limitAction = LIMIT_ACTION_ALERT_ONLY;
        }
        quotaPolicies_.push_back(quotaPolicy);
    }
}

const std::vector<std::string> NetPolicyTraffic::UpdateMeteredIfacesQuota()
{
    std::vector<std::string> newMeteredIfaces;
    std::shared_lock<std::shared_mutex> lock(quotaMutex_);
    std::vector<NetQuotaPolicy> quotaTmp = quotaPolicies_;
    lock.unlock();
    for (auto &quotaPolicy : quotaTmp) {
        std::string iface = GetMatchIfaces(quotaPolicy);
        // set quota for metered iface.
        if (iface == UNKNOW_IFACE || !quotaPolicy.quotapolicy.metered) {
            continue;
        }
        newMeteredIfaces.push_back(iface);
        int64_t quotaRemain = GetQuotaRemain(quotaPolicy);
        if (quotaRemain >= 0) {
            GetNetsysInst()->BandwidthSetIfaceQuota(iface, quotaRemain);
        }
    }
    // remove the iface quota that not metered.
    std::shared_lock<std::shared_mutex> meteredLock(meteredMutex_);
    auto meteredIfacesBck = meteredIfaces_;
    meteredLock.unlock();
    for (uint32_t i = 0; i < meteredIfacesBck.size(); ++i) {
        if (!std::count(newMeteredIfaces.begin(), newMeteredIfaces.end(), meteredIfacesBck[i])) {
            GetNetsysInst()->BandwidthRemoveIfaceQuota(meteredIfacesBck[i]);
        }
    }
    return newMeteredIfaces;
}

void NetPolicyTraffic::UpdateMeteredIfaces(std::vector<std::string> &newMeteredIfaces)
{
    NETMGR_LOG_D("UpdateMeteredIfaces size[%{public}zu]", newMeteredIfaces.size());
    std::unique_lock<std::shared_mutex> lock(meteredMutex_);
    meteredIfaces_.clear();
    meteredIfaces_.reserve(newMeteredIfaces.size());
    for (auto &iface : newMeteredIfaces) {
        meteredIfaces_.push_back(iface);
    }
    // notify the callback of metered ifaces changed.
    GetCbInst()->NotifyNetMeteredIfacesChange(meteredIfaces_);
}

void NetPolicyTraffic::UpdateQuotaNotify()
{
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Traverse cellular network start");
    std::shared_lock<std::shared_mutex> lock(quotaMutex_);
    std::vector<NetQuotaPolicy> quotaPoliciesTmp = quotaPolicies_;
    lock.unlock();
    for (auto &quotaPolicy : quotaPoliciesTmp) {
        NetmanagerHiTrace::NetmanagerStartSyncTrace("Get the start time of the metering cycle start");
        int64_t start = quotaPolicy.GetPeriodStart();
        NetmanagerHiTrace::NetmanagerFinishSyncTrace("Get the start time of the metering cycle end");

        NetmanagerHiTrace::NetmanagerStartSyncTrace("Get the usage of traffic start");
        int64_t totalQuota = GetTotalQuota(quotaPolicy);
        NetmanagerHiTrace::NetmanagerFinishSyncTrace("Get the usage of traffic end");
        // check if the quota is over the limit
        if (quotaPolicy.IsOverLimit(totalQuota)) {
            if (quotaPolicy.quotapolicy.lastLimitRemind > start) {
                // notify the quota reach limit and has reminded before.
                NetmanagerHiTrace::NetmanagerStartSyncTrace("Notify quota limit reminded start");
                NotifyQuotaLimitReminded(totalQuota);
                NetmanagerHiTrace::NetmanagerFinishSyncTrace("Notify quota limit reminded end");
                continue;
            }
            NetmanagerHiTrace::NetmanagerStartSyncTrace("Update net enable status start");
            UpdateNetEnableStatus(quotaPolicy);
            NetmanagerHiTrace::NetmanagerFinishSyncTrace("Update net enable status end");
            // notify the quota reach limit
            NotifyQuotaLimit(totalQuota);
            continue;
        }
        // check if the quota is over the warning
        if (quotaPolicy.IsOverWarning(totalQuota) && quotaPolicy.quotapolicy.lastWarningRemind < start) {
            NetmanagerHiTrace::NetmanagerStartSyncTrace("Notify quota warning remind start");
            NotifyQuotaWarning(totalQuota);
            NetmanagerHiTrace::NetmanagerFinishSyncTrace("Notify quota warning remind end");
        }
    }
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Traverse cellular network end");
}

int64_t NetPolicyTraffic::GetQuotaRemain(NetQuotaPolicy &quotaPolicy)
{
    int64_t start = quotaPolicy.GetPeriodStart();
    int64_t totalQuota = GetTotalQuota(quotaPolicy);
    NETMGR_LOG_D("GetQuotaRemain totalQuota[%{public}s] limit[%{public}s] start[%{public}s]",
                 std::to_string(totalQuota).c_str(), std::to_string(quotaPolicy.quotapolicy.limitBytes).c_str(),
                 ctime(&start));
    // calculate the quota for each policy.
    bool hasLimit = quotaPolicy.quotapolicy.limitBytes != DATA_USAGE_UNKNOWN;
    int64_t quota = LONG_MAX;
    if (hasLimit || quotaPolicy.quotapolicy.metered) {
        if (hasLimit && quotaPolicy.quotapolicy.periodDuration != QUOTA_POLICY_NO_PERIOD) {
            if (quotaPolicy.quotapolicy.lastLimitRemind >= start) {
                return LONG_MAX;
            }
            quota = quotaPolicy.quotapolicy.limitBytes - totalQuota;
        }
    }
    return quota < 0 ? 0 : quota;
}

void NetPolicyTraffic::UpdateNetEnableStatus(const NetQuotaPolicy &quotaPolicy)
{
    NETMGR_LOG_D("UpdateNetEnableStatus metered[%{public}d] quotapolicy.limitAction[%{public}d]",
                 quotaPolicy.quotapolicy.metered, quotaPolicy.quotapolicy.limitAction);
    if (quotaPolicy.quotapolicy.metered || quotaPolicy.quotapolicy.limitAction == LIMIT_ACTION_ACCESS_DISABLED) {
        SetNetworkEnableStatus(quotaPolicy, false);
    }
}

int32_t NetPolicyTraffic::GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies)
{
    std::unique_lock<std::shared_mutex> lock(quotaMutex_);
    quotaPolicies.clear();
    quotaPolicies = quotaPolicies_;
    NETMGR_LOG_D("GetNetQuotaPolicies quotaPolicies end size[%{public}zu]", quotaPolicies.size());
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyTraffic::UpdateRemindPolicy(int32_t netType, const std::string &simId, uint32_t remindType)
{
    if (!IsValidNetType(netType)) {
        NETMGR_LOG_E("NetPolicyType is invalid policy[%{public}d]", netType);
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    if (!IsValidNetRemindType(remindType)) {
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    std::unique_lock<std::shared_mutex> lock(quotaMutex_);
    for (uint32_t i = 0; i < quotaPolicies_.size(); ++i) {
        NetQuotaPolicy &quotaPolicy = quotaPolicies_[i];
        int32_t netTypeTemp = quotaPolicy.networkmatchrule.netType;
        std::string iccidTemp = quotaPolicy.networkmatchrule.simId;
        if (netTypeTemp == netType && iccidTemp == simId) {
            switch (remindType) {
                case REMIND_TYPE_WARNING:
                    quotaPolicy.quotapolicy.lastWarningRemind = time(nullptr);
                    break;
                case REMIND_TYPE_LIMIT:
                    quotaPolicy.quotapolicy.lastLimitRemind = time(nullptr);
                    break;
                default:
                    return NETMANAGER_ERR_PARAMETER_ERROR;
            }
        }
    }
    lock.unlock();
    UpdateQuotaPoliciesInner();
    NETMGR_LOG_I("NetPolicyTraffic::UpdateRemindPolicy end.");
    return NETMANAGER_SUCCESS;
}

bool NetPolicyTraffic::IsMeteredIfaces(const std::string& ifaceName)
{
    std::shared_lock<std::shared_mutex> lock(meteredMutex_);
    if (std::find(meteredIfaces_.begin(), meteredIfaces_.end(), ifaceName) != meteredIfaces_.end()) {
        return true;
    }
    return false;
}

int32_t NetPolicyTraffic::ResetPolicies(const std::string &simId)
{
    std::shared_lock<std::shared_mutex> lock(quotaMutex_);
    for (auto &quotaPolicy : quotaPolicies_) {
        if (quotaPolicy.networkmatchrule.simId == simId) {
            quotaPolicy.Reset();
        }
    }
    lock.unlock();
    return UpdateQuotaPoliciesInner();
}

int32_t NetPolicyTraffic::ResetPolicies()
{
    std::shared_lock<std::shared_mutex> lock(quotaMutex_);
    for (auto &quotaPolicy : quotaPolicies_) {
        NETMGR_LOG_I("NetPolicyTraffic::ResetPolicies [%{public}s.", quotaPolicy.networkmatchrule.simId.c_str());
        quotaPolicy.Reset();
    }
    lock.unlock();
    return UpdateQuotaPoliciesInner();
}

void NetPolicyTraffic::ReachedLimit(const std::string &iface)
{
    NETMGR_LOG_D("ReachedLimit iface:%{public}s.", iface.c_str());
    if (IsMeteredIfaces(iface)) {
        UpdateQuotaPoliciesInner();
    }
}

void NetPolicyTraffic::UpdateNetPolicy()
{
    UpdateQuotaPoliciesInner();
}

int64_t NetPolicyTraffic::GetTotalQuota(NetQuotaPolicy &quotaPolicy)
{
    std::string iface = GetMatchIfaces(quotaPolicy);
    NetStatsInfo info;
    int64_t start = quotaPolicy.GetPeriodStart();
    int64_t end = static_cast<int64_t>(time(nullptr));
    if (end < 0) {
        return 0;
    }
    GetNetCenterInst().GetIfaceStatsDetail(iface, start, end, info);
    int64_t quota = static_cast<int64_t>(info.rxBytes_ + info.txBytes_);

    return quota < 0 ? 0 : quota;
}

void NetPolicyTraffic::ReadQuotaPolicies()
{
    std::unique_lock<std::shared_mutex> lock(quotaMutex_);
    GetFileInst()->ReadQuotaPolicies(quotaPolicies_);
    lock.unlock();
    UpdateQuotaPoliciesInner();
}

bool NetPolicyTraffic::WriteQuotaPolicies()
{
    std::shared_lock<std::shared_mutex> lock(quotaMutex_);
    return GetFileInst()->WriteQuotaPolicies(quotaPolicies_);
}

const std::string NetPolicyTraffic::GetMatchIfaces(const NetQuotaPolicy &quotaPolicy)
{
    std::string ident = "";
    if (quotaPolicy.networkmatchrule.netType == BEARER_CELLULAR) {
        ident = IDENT_PREFIX_CELLULAR + quotaPolicy.networkmatchrule.simId;
    } else if (quotaPolicy.networkmatchrule.netType == BEARER_WIFI) {
        ident = quotaPolicy.networkmatchrule.ident;
    } else if (quotaPolicy.networkmatchrule.netType == BEARER_ETHERNET) {
        ident = quotaPolicy.networkmatchrule.ident;
    }
    std::string iface;
    if (quotaPolicy.networkmatchrule.netType >= BEARER_DEFAULT) {
        return iface;
    }
    GetNetCenterInst().GetIfaceNameByType(static_cast<NetBearType>(quotaPolicy.networkmatchrule.netType), ident, iface);
    NETMGR_LOG_D("GetMatchIfaces netType: %{public}d ident: %{public}s iface: %{public}s.",
                 quotaPolicy.networkmatchrule.netType, ident.c_str(), iface.c_str());
    return iface;
}

void NetPolicyTraffic::SetNetworkEnableStatus(const NetQuotaPolicy &quotaPolicy, bool enable)
{
    NETMGR_LOG_D("SetNetworkEnableStatus enable: %{public}d ", enable);
}

void NetPolicyTraffic::NotifyQuotaWarning(int64_t totalQuota)
{
    PublishQuotaEvent(COMMON_EVENT_NET_QUOTA_WARNING, BROADCAST_QUOTA_WARNING, totalQuota);
}

void NetPolicyTraffic::NotifyQuotaLimitReminded(int64_t totalQuota)
{
    PublishQuotaEvent(COMMON_EVENT_NET_QUOTA_LIMIT_REMINDED, BROADCAST_QUOTA_LIMIT_REMIND, totalQuota);
}

void NetPolicyTraffic::NotifyQuotaLimit(int64_t totalQuota)
{
    PublishQuotaEvent(COMMON_EVENT_NET_QUOTA_LIMIT, BROADCAST_QUOTA_LIMIT, totalQuota);
}

void NetPolicyTraffic::PublishQuotaEvent(const std::string &action, const std::string &describe, int64_t quota)
{
    BroadcastInfo info;
    info.action = action;
    info.data = describe;
    info.permission = Permission::CONNECTIVITY_INTERNAL;
    std::map<std::string, int64_t> param = {{"totalQuota", quota}};
    BroadcastManager::GetInstance().SendBroadcast(info, param);
}

bool NetPolicyTraffic::IsValidPeriodDuration(const std::string &periodDuration)
{
    if (periodDuration.empty() || periodDuration.size() < PERIOD_DURATION_SIZE) {
        NETMGR_LOG_E("periodDuration is illegal");
        return false;
    }

    std::string cycle = periodDuration.substr(0, 1);
    NETMGR_LOG_D("PeriodDuration [%{public}s].", periodDuration.c_str());
    int32_t start = CommonUtils::StrToInt(periodDuration.substr(1, periodDuration.size()));

    if (cycle == PERIOD_DAY) {
        if (start >= PERIOD_START && start <= DAY_MAX) {
            return true;
        }
    }

    if (cycle == PERIOD_MONTH) {
        if (start >= PERIOD_START && start <= MONTH_MAX) {
            return true;
        }
    }

    if (cycle == PERIOD_YEAR) {
        if (start >= PERIOD_START && start <= YEAR_MAX) {
            return true;
        }
    }
    NETMGR_LOG_E("Invalid periodDuration start [%{public}d],Invalid periodDuration cycle [%{public}s]", start,
                 cycle.c_str());
    return false;
}

bool NetPolicyTraffic::IsQuotaPolicyExist(int32_t netType, const std::string &simId)
{
    std::vector<NetQuotaPolicy> quotaPolicies;
    GetFileInst()->ReadQuotaPolicies(quotaPolicies);

    if (quotaPolicies.empty()) {
        NETMGR_LOG_E("quotaPolicies is empty");
        return false;
    }

    for (uint32_t i = 0; i < quotaPolicies.size(); i++) {
        if (netType == quotaPolicies[i].networkmatchrule.netType && simId == quotaPolicies[i].networkmatchrule.simId) {
            return true;
        }
    }

    return false;
}

void NetPolicyTraffic::HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent)
{
    NETMGR_LOG_D("NetPolicyTraffic HandleEvent");
}

void NetPolicyTraffic::GetDumpMessage(std::string &message)
{
    static const std::string TAB = "    ";
    message.append(TAB + "MeteredIfaces: {");
    std::shared_lock<std::shared_mutex> meteredlock(meteredMutex_);
    std::for_each(meteredIfaces_.begin(), meteredIfaces_.end(),
                  [&message](const std::string &item) { message.append(item + ", "); });
    meteredlock.unlock();
    message.append("}\n");
    message.append(TAB + "QuotaPolicies:\n");
    std::shared_lock<std::shared_mutex> lock(quotaMutex_);
    std::vector<NetQuotaPolicy> quotaPoliciesTmp = quotaPolicies_;
    lock.unlock();
    std::for_each(quotaPoliciesTmp.begin(), quotaPoliciesTmp.end(), [&message](const auto &item) {
        message.append(TAB + TAB + "NetType: " + std::to_string(item.networkmatchrule.netType) + "\n" + TAB + TAB +
                       "simId: " + item.networkmatchrule.simId + "\n" + TAB + TAB +
                       "Ident: " + item.networkmatchrule.ident + "\n");
        message.append(TAB + TAB + "PeriodStartTime: " + std::to_string(item.quotapolicy.periodStartTime) + "\n");
        message.append(TAB + TAB + "PeriodDuration: " + item.quotapolicy.periodDuration + "\n");
        message.append(TAB + TAB + "Title: " + item.quotapolicy.title + "\n" + TAB + TAB +
                       "Summary: " + item.quotapolicy.summary + "\n");
        message.append(TAB + TAB + "quotapolicy.warningBytes: " + std::to_string(item.quotapolicy.warningBytes) + "\n");
        message.append(TAB + TAB + "quotapolicy.limitBytes: " + std::to_string(item.quotapolicy.limitBytes) + "\n");
        message.append(TAB + TAB +
                       "quotapolicy.lastWarningRemind: " + std::to_string(item.quotapolicy.lastWarningRemind) + "\n");
        message.append(TAB + TAB + "quotapolicy.lastLimitRemind: " + std::to_string(item.quotapolicy.lastLimitRemind) +
                       "\n");
        message.append(TAB + TAB + "Metered: " + std::to_string(item.quotapolicy.metered) + "\n" + TAB + TAB +
                       "Ident: " + item.networkmatchrule.ident + "\n");
        message.append(TAB + TAB + "quotapolicy.limitAction: " + std::to_string(item.quotapolicy.limitAction) + "\n" +
                       TAB + TAB + "UsedBytes: " + std::to_string(item.quotapolicy.usedBytes) + "\n");
        message.append(TAB + TAB + "UsedTimeDuration: " + std::to_string(item.quotapolicy.usedTimeDuration) + "\n");
        message.append(TAB + TAB + "Possessor: " + item.quotapolicy.possessor + "\n\n");
    });
}
} // namespace NetManagerStandard
} // namespace OHOS

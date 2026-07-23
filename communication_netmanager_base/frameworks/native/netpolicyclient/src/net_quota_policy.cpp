/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "net_quota_policy.h"

#include <ctime>

#include "parcel.h"

#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_POLICY_SIZE = 100;
static constexpr int32_t INVALID_VALUE = -1;

bool NetQuotaPolicy::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(networkmatchrule.netType)) {
        return false;
    }
    if (!parcel.WriteString(networkmatchrule.simId)) {
        return false;
    }
    if (!parcel.WriteInt64(quotapolicy.periodStartTime)) {
        return false;
    }
    if (!parcel.WriteString(quotapolicy.periodDuration)) {
        return false;
    }
    if (!parcel.WriteInt64(quotapolicy.warningBytes)) {
        return false;
    }
    if (!parcel.WriteInt64(quotapolicy.limitBytes)) {
        return false;
    }
    if (!parcel.WriteInt64(quotapolicy.lastLimitRemind)) {
        return false;
    }
    if (!parcel.WriteBool(quotapolicy.metered)) {
        return false;
    }
    if (!parcel.WriteInt32(quotapolicy.source)) {
        return false;
    }
    if (!parcel.WriteInt32(quotapolicy.limitAction)) {
        return false;
    }
    if (!parcel.WriteString(networkmatchrule.ident)) {
        return false;
    }

    return true;
}

bool NetQuotaPolicy::Marshalling(Parcel &parcel, const NetQuotaPolicy &quotaPolicy)
{
    quotaPolicy.Marshalling(parcel);
    return true;
}

bool NetQuotaPolicy::Marshalling(Parcel &parcel, const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    uint32_t vsize = static_cast<uint32_t>(quotaPolicies.size());
    if (!parcel.WriteUint32(vsize)) {
        return false;
    }

    for (uint32_t i = 0; i < vsize; ++i) {
        quotaPolicies[i].Marshalling(parcel);
    }

    return true;
}

bool NetQuotaPolicy::Unmarshalling(Parcel &parcel, NetQuotaPolicy &quotaPolicy)
{
    if (!parcel.ReadInt32(quotaPolicy.networkmatchrule.netType)) {
        return false;
    }
    if (!parcel.ReadString(quotaPolicy.networkmatchrule.simId)) {
        return false;
    }
    if (!parcel.ReadInt64(quotaPolicy.quotapolicy.periodStartTime)) {
        return false;
    }
    if (!parcel.ReadString(quotaPolicy.quotapolicy.periodDuration)) {
        return false;
    }
    if (!parcel.ReadInt64(quotaPolicy.quotapolicy.warningBytes)) {
        return false;
    }
    if (!parcel.ReadInt64(quotaPolicy.quotapolicy.limitBytes)) {
        return false;
    }
    if (!parcel.ReadInt64(quotaPolicy.quotapolicy.lastLimitRemind)) {
        return false;
    }
    if (!parcel.ReadBool(quotaPolicy.quotapolicy.metered)) {
        return false;
    }
    if (!parcel.ReadInt32(quotaPolicy.quotapolicy.source)) {
        return false;
    }
    if (!parcel.ReadInt32(quotaPolicy.quotapolicy.limitAction)) {
        return false;
    }
    if (!parcel.ReadString(quotaPolicy.networkmatchrule.ident)) {
        return false;
    }

    return true;
}

bool NetQuotaPolicy::Unmarshalling(Parcel &parcel, std::vector<NetQuotaPolicy> &quotaPolicies)
{
    uint32_t vSize = 0;
    if (!parcel.ReadUint32(vSize)) {
        return false;
    }
    vSize = vSize > MAX_POLICY_SIZE ? MAX_POLICY_SIZE : vSize;

    NetQuotaPolicy quotaPolicyTmp;
    for (uint32_t i = 0; i < vSize; i++) {
        if (!parcel.ReadInt32(quotaPolicyTmp.networkmatchrule.netType)) {
            return false;
        }
        if (!parcel.ReadString(quotaPolicyTmp.networkmatchrule.simId)) {
            return false;
        }
        if (!parcel.ReadInt64(quotaPolicyTmp.quotapolicy.periodStartTime)) {
            return false;
        }
        if (!parcel.ReadString(quotaPolicyTmp.quotapolicy.periodDuration)) {
            return false;
        }
        if (!parcel.ReadInt64(quotaPolicyTmp.quotapolicy.warningBytes)) {
            return false;
        }
        if (!parcel.ReadInt64(quotaPolicyTmp.quotapolicy.limitBytes)) {
            return false;
        }
        if (!parcel.ReadInt64(quotaPolicyTmp.quotapolicy.lastLimitRemind)) {
            return false;
        }
        if (!parcel.ReadBool(quotaPolicyTmp.quotapolicy.metered)) {
            return false;
        }
        if (!parcel.ReadInt32(quotaPolicyTmp.quotapolicy.source)) {
            return false;
        }
        if (!parcel.ReadInt32(quotaPolicyTmp.quotapolicy.limitAction)) {
            return false;
        }
        if (!parcel.ReadString(quotaPolicyTmp.networkmatchrule.ident)) {
            return false;
        }
        quotaPolicies.push_back(quotaPolicyTmp);
    }

    return true;
}

bool NetQuotaPolicy::IsOverWarning(int64_t totalQuota) const
{
    return totalQuota > quotapolicy.warningBytes;
}

bool NetQuotaPolicy::IsOverLimit(int64_t totalQuota) const
{
    return totalQuota > quotapolicy.limitBytes;
}

int64_t NetQuotaPolicy::GetPeriodStart()
{
    if (quotapolicy.periodDuration.size() < PERIOD_DURATION_SIZE) {
        quotapolicy.periodDuration = PERIOD_MONTH;
    }
    time_t timeNow;
    time_t now = time(&timeNow);
    if (now < 0) {
        return INVALID_VALUE;
    }
    struct tm tm;
    localtime_r(&timeNow, &tm);
    std::string cycle = quotapolicy.periodDuration.substr(0, 1);
    int32_t start = CommonUtils::StrToInt(quotapolicy.periodDuration.substr(1, quotapolicy.periodDuration.size()));

    if (cycle == PERIOD_DAY) {
        tm.tm_hour = start;
        tm.tm_min = 0;
        tm.tm_sec = 0;
    } else if (cycle == PERIOD_YEAR) {
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        tm.tm_yday = start - 1;
    } else {
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        tm.tm_mday = start;
    }
    time_t start_time = mktime(&tm);
    return start_time;
}

void NetQuotaPolicy::Reset()
{
    quotapolicy.periodDuration = PERIOD_MONTH + std::to_string(PERIOD_START);
    quotapolicy.warningBytes = DATA_USAGE_UNKNOWN;
    quotapolicy.limitBytes = DATA_USAGE_UNKNOWN;
    quotapolicy.lastWarningRemind = REMIND_NEVER;
    quotapolicy.lastLimitRemind = REMIND_NEVER;
    quotapolicy.metered = false;
    quotapolicy.limitAction = LimitAction::LIMIT_ACTION_NONE;
}
} // namespace NetManagerStandard
} // namespace OHOS

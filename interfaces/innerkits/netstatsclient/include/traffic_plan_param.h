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

#ifndef NET_STATS_TRAFFIC_PLAN_H
#define NET_STATS_TRAFFIC_PLAN_H

#include <cstdint>
#include <string>
#include <sstream>

namespace OHOS {
namespace NetManagerStandard {

// Error codes for traffic plan operations
constexpr int32_t TRAFFIC_PLAN_ERR_INVALID_PARAM = 2100001;
constexpr int32_t TRAFFIC_PLAN_ERR_SERVICE_FAILED = 2100002;
constexpr int32_t TRAFFIC_PLAN_ERR_DATABASE_FAILED = 2100003;

// Traffic plan parameters enumeration
enum class TrafficPlanParam : int32_t {
    DISPLAY_TRAFFIC_SWITCH = 1,  // Display traffic switch (0/1)
    UNLIMIT_TRAFFIC_SWITCH = 2,  // Unlimited traffic switch (0/1)
    TRAFFIC_LIMIT = 3,  // Traffic limit in bytes
    START_DATE = 4,  // Start date (1-31)
    OVER_LIMIT_BEHAVIOR = 5,  // Over limit behavior (1: disconnect, 0: popup)
    MONTHLY_LIMIT_PERCENTAGE = 6,  // Monthly limit percentage (0-100)
    DAILY_LIMIT_PERCENTAGE = 7  // Daily limit percentage (0-100)
};

// Traffic plan information structure
struct TrafficPlanInfo {
    int32_t slotId;
    std::string iccid;  // ICCID
    int32_t simId;  // SIM card ID
    int32_t displayTrafficSwitch;  // Display traffic switch (0/1)
    int32_t unlimitTrafficSwitch;  // Unlimited traffic switch (0/1)
    uint64_t trafficLimit;  // Traffic limit in bytes
    int32_t startDate;  // Start date (1-31)
    int32_t overLimitBehavior;  // Over limit behavior (1: disconnect, 2: popup)
    int32_t monthlyLimitPercentage;  // Monthly limit percentage (0-100)
    int32_t dailyLimitPercentage;  // Daily limit percentage (0-100)
    bool isCanNotifyMonthlyLimit = false;
    bool isCanNotifyMonthlyMark = false;
    bool isCanNotifyDailyMark = false;
    int32_t lastMonAlertTime = 0;
    int32_t lastMonNotifyTime = 0;
    int32_t lastDayNotifyTime = 0;

    TrafficPlanInfo()
        : slotId(-1),
          iccid(""),
          simId(-1),
          displayTrafficSwitch(0),
          unlimitTrafficSwitch(0),
          trafficLimit(UINT64_MAX),
          startDate(1),
          overLimitBehavior(1),
          monthlyLimitPercentage(80),   // 80%
          dailyLimitPercentage(10)   // 10%
    {}

    inline const std::string ToString() const
    {
        std::ostringstream oss;
        oss << iccid << "," << simId << "," << slotId << ","
            << displayTrafficSwitch << "," << unlimitTrafficSwitch << "," << trafficLimit << ","
            << startDate << "," << overLimitBehavior << "," << monthlyLimitPercentage << "," << dailyLimitPercentage;
        return oss.str();
    }
};

} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_STATS_TRAFFIC_PLAN_H

/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_UTIL_H
#define NET_STATS_UTIL_H
#include <stdint.h>
#include <string>

namespace OHOS {
namespace NetManagerStandard {

const int32_t DUAL_CARD = 2;

class NetStatsUtils final {
public:
    NetStatsUtils() = default;
    ~NetStatsUtils() = default;

    static int32_t GetStartTimestamp(int32_t startdate);
    static int32_t GetEndTimestamp(int32_t startdate);
    static int32_t GetTodayStartTimestamp();
    static int32_t GetNowTimestamp();
    static bool IsLeapYear(int32_t year);
    static int32_t GetDaysInMonth(int32_t year, int32_t month);
    static bool IsMobileDataEnabled();
    static int32_t IsDualCardEnabled();
    static int32_t GetPrimarySlotId();
    static bool ConvertToUint64(const std::string &str, uint64_t &value);
    static bool ConvertToInt32(const std::string &str, int32_t &value);
    static bool IsLessThanOneMonthAgoPrecise(time_t timestamp);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    static std::string GetIccIdBySlotId(int32_t slotId);
    static std::string GetIccIdBySimId(int32_t simId);
    static bool IsSimIdValid(int32_t simId);
    static bool IsSlotIdValid(int32_t slotId);
#endif
};
}
}
#endif

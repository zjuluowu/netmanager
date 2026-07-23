/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include <chrono>

#include "net_stats_utils.h"
#include "net_mgr_log_wrapper.h"
#ifdef SUPPORT_TRAFFIC_STATISTIC
#include "cellular_data_client.h"
#include "core_service_client.h"
#endif // SUPPORT_TRAFFIC_STATISTIC

namespace OHOS {
namespace NetManagerStandard {

const int32_t TM_YEAR_START = 1900;
const int32_t MONTH_NUM = 12;
static const int32_t DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
constexpr uint32_t ONE_MONTH_SECOND = 31 * 24 * 60 * 60;
constexpr int32_t SLOT_0 = 0;
constexpr int32_t SLOT_1 = 1;

int32_t NetStatsUtils::GetStartTimestamp(int32_t startdate)
{
    // 获取当前日期和时间
    auto now = std::chrono::system_clock::now();
    time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    tm* now_tm = std::localtime(&now_time_t);
    if (now_tm == nullptr) {
        return INT32_MAX;
    }
 
    // 获取当前年份和月份
    int current_year = now_tm->tm_year + TM_YEAR_START;
    int current_month = now_tm->tm_mon + 1; // tm_mon是0-11，所以需要加1
    int current_day = now_tm->tm_mday;
 
    // 计算上个月的年份和月份
    int previous_month = current_month == 1 ? MONTH_NUM : current_month - 1;
    int previous_year = current_month == 1 ? current_year - 1 : current_year;

    // 构建 tm 结构表示上个月
    tm last_month_xth_tm = {};
    last_month_xth_tm.tm_year = previous_year - TM_YEAR_START;
    last_month_xth_tm.tm_mon = previous_month - 1;
    last_month_xth_tm.tm_mday = startdate;

    int daysInCurrentMonth = NetStatsUtils::GetDaysInMonth(previous_year, previous_month);
    // 如果上个月没有起始日期这一天，就从上个月的月末一天开始算
    if (daysInCurrentMonth < startdate) {
        last_month_xth_tm.tm_year = previous_year - TM_YEAR_START;
        last_month_xth_tm.tm_mon = previous_month -1 ;
        last_month_xth_tm.tm_mday = daysInCurrentMonth;
    }
    // 如果当前天大于起始日期，则获取本月的时间
    if (startdate <= current_day) {
        last_month_xth_tm.tm_year = current_year - TM_YEAR_START;
        last_month_xth_tm.tm_mon = current_month - 1;
        last_month_xth_tm.tm_mday = startdate;
    }

    NETMGR_LOG_I("last year: %{public}d, month: %{public}d, day: %{public}d",
        last_month_xth_tm.tm_year + TM_YEAR_START, last_month_xth_tm.tm_mon + 1, last_month_xth_tm.tm_mday);
 
    // 转换为 time_t
    time_t last_month_xth_time_t = mktime(&last_month_xth_tm);
    auto last_month_xth = std::chrono::system_clock::from_time_t(last_month_xth_time_t);

    int32_t timestamp = static_cast<int32_t>(std::chrono::system_clock::to_time_t(last_month_xth));
    NETMGR_LOG_I("timestamp: %{public}d", timestamp);
 
    return timestamp;
}

int32_t NetStatsUtils::GetEndTimestamp(int32_t startdate)
{
    auto now = std::chrono::system_clock::now();
    time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    tm* now_tm = std::localtime(&now_time_t);
    if (now_tm == nullptr) {
        return INT32_MAX;
    }
 
    int current_year = now_tm->tm_year + TM_YEAR_START;
    int current_month = now_tm->tm_mon + 1;
    int current_day = now_tm->tm_mday;
 
    int next_month = current_month == MONTH_NUM ? 1 : current_month + 1;
    int next_year = current_month == MONTH_NUM ? current_year + 1 : current_year;

    tm next_month_xth_tm = {};
    next_month_xth_tm.tm_year = current_year - TM_YEAR_START;
    next_month_xth_tm.tm_mon = current_month - 1;
    next_month_xth_tm.tm_mday = startdate;
    next_month_xth_tm.tm_hour = 0;
    next_month_xth_tm.tm_min = 0;
    next_month_xth_tm.tm_sec = 0;

    if (startdate > current_day) {
        int daysInCurrentMonth = GetDaysInMonth(current_year, current_month);
        if (startdate > daysInCurrentMonth) {
            next_month_xth_tm.tm_year = current_year - TM_YEAR_START;
            next_month_xth_tm.tm_mon = current_month - 1 ;
            next_month_xth_tm.tm_mday = daysInCurrentMonth;
        } else {
            next_month_xth_tm.tm_year = current_year - TM_YEAR_START;
            next_month_xth_tm.tm_mon = current_month - 1;
            next_month_xth_tm.tm_mday = startdate;
        }
    }

    if (startdate <= current_day) {
        next_month_xth_tm.tm_year = next_year - TM_YEAR_START;
        next_month_xth_tm.tm_mon = next_month - 1;
        next_month_xth_tm.tm_mday = startdate;
    }

    time_t next_month_xth_time_t = mktime(&next_month_xth_tm);
    auto next_month_xth = std::chrono::system_clock::from_time_t(next_month_xth_time_t);
    int32_t timestamp = static_cast<int32_t>(std::chrono::system_clock::to_time_t(next_month_xth));
 
    return timestamp - 1;
}

int32_t NetStatsUtils::GetTodayStartTimestamp()
{
    auto now = std::chrono::system_clock::now();
    time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    tm* now_tm = std::localtime(&now_time_t);
    if (now_tm == nullptr) {
        return INT32_MAX;
    }

    tm last_month_xth_tm = {};
    last_month_xth_tm.tm_year = now_tm->tm_year;
    last_month_xth_tm.tm_mon = now_tm->tm_mon;
    last_month_xth_tm.tm_mday = now_tm->tm_mday;

    // 转换为 time_t
    time_t last_month_xth_time_t = mktime(&last_month_xth_tm);
 
    // 转换为系统时钟时间
    auto last_month_xth = std::chrono::system_clock::from_time_t(last_month_xth_time_t);
 
    // 转换为时间戳（通常用于网络传输的秒级时间戳）
    auto timestamp = std::chrono::system_clock::to_time_t(last_month_xth);

    return timestamp;
}

int32_t NetStatsUtils::GetNowTimestamp()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(now);
}

bool NetStatsUtils::IsLeapYear(int32_t year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); // 4: 100/400计算闰年
}
 
int32_t NetStatsUtils::GetDaysInMonth(int32_t year, int32_t month)
{
    if (year <= 0 || month <= 0 || month > static_cast<int32_t>(sizeof(DAYS_IN_MONTH) / sizeof(int32_t))) {
        return -1;
    }
 
    if (month == 2 && NetStatsUtils::IsLeapYear(year)) { // 如果是2月并且是闰年
        return 29;  // 则天数为29
    }

    return DAYS_IN_MONTH[month - 1];
}

bool NetStatsUtils::IsMobileDataEnabled()
{
    bool dataEnabled = false;
#ifdef SUPPORT_TRAFFIC_STATISTIC
    int32_t errorCode =
        DelayedRefSingleton<Telephony::CellularDataClient>::GetInstance().IsCellularDataEnabled(dataEnabled);
    NETMGR_LOG_I("errorCode: %{public}d, isEnabled: %{public}d", errorCode, dataEnabled);
#endif // SUPPORT_TRAFFIC_STATISTIC
    return dataEnabled;
}

int32_t NetStatsUtils::IsDualCardEnabled()
{
    int32_t actualSimNum = 0;
#ifdef SUPPORT_TRAFFIC_STATISTIC
    int32_t simNum = Telephony::CoreServiceClient::GetInstance().GetMaxSimCount();
    for (int32_t i = 0; i < simNum; ++i) {
        bool hasSimCard = false;
        Telephony::CoreServiceClient::GetInstance().HasSimCard(i, hasSimCard);
        if (hasSimCard) {
            actualSimNum++;
        }
    }
#endif // SUPPORT_TRAFFIC_STATISTIC
    NETMGR_LOG_I("actualSimNum == %{public}d.", actualSimNum);
    return actualSimNum;
}

int32_t NetStatsUtils::GetPrimarySlotId()
{
    int primarySlotId = -1;
#ifdef SUPPORT_TRAFFIC_STATISTIC
    int ret = Telephony::CoreServiceClient::GetInstance().GetPrimarySlotId(primarySlotId);
    if (primarySlotId == -1) {
        NETMGR_LOG_E("GetPrimarySlotId error, ret: %{public}d", ret);
    }
#endif // SUPPORT_TRAFFIC_STATISTIC
    return primarySlotId;
}

bool NetStatsUtils::ConvertToUint64(const std::string &str, uint64_t &value)
{
    char* end;
    errno = 0; // 清除 errno
    if (str.empty()) {
        NETMGR_LOG_E("string error. str: %{public}s", str.c_str());
        return false;
    }

    value = std::strtoull(str.c_str(), &end, 10);  // 10:十进制

    // 检查错误:
    // 1. 若没有数字被转换
    if (end == str.c_str()) {
        NETMGR_LOG_E("string error. str: %{public}s", str.c_str());
        return false;
    }
    // 2. 若存在范围错误（过大或过小）
    if (errno == ERANGE && (value == HUGE_VAL || value == HUGE_VALF || value == HUGE_VALL)) {
        NETMGR_LOG_E("string error. str: %{public}s", str.c_str());
        return false;
    }
    // 3. 若字符串包含非数字字符
    if (end != nullptr && *end != '\0') {
        NETMGR_LOG_E("string error. str: %{public}s", str.c_str());
        return false;
    }

    return true;
}

bool NetStatsUtils::ConvertToInt32(const std::string &str, int32_t &value)
{
    char* end;
    errno = 0; // 清除 errno

    if (str.empty()) {
        NETMGR_LOG_E("string error. str: %{public}s", str.c_str());
        return false;
    }

    value = std::strtod(str.c_str(), &end);

    // 检查错误:
    // 1. 若没有数字被转换
    if (end == str.c_str()) {
        NETMGR_LOG_E("string error. str: %{public}s", str.c_str());
        return false;
    }
    // 2. 若存在范围错误（过大或过小）
    if (errno == ERANGE && (value == HUGE_VAL || value == HUGE_VALF || value == HUGE_VALL)) {
        NETMGR_LOG_E("string error. str: %{public}s", str.c_str());
        return false;
    }
    // 3. 若字符串包含非数字字符
    if (*end != '\0') {
        NETMGR_LOG_E("string error. str: %{public}s", str.c_str());
        return false;
    }

    return true;
}

bool NetStatsUtils::IsLessThanOneMonthAgoPrecise(time_t timestamp)
{
    auto now = std::chrono::system_clock::now();
    time_t nowTimestamp = std::chrono::system_clock::to_time_t(now);
    if (nowTimestamp < static_cast<time_t>(ONE_MONTH_SECOND)) {
        return true;
    }

    time_t beforeOneMonthTimestamp = nowTimestamp - static_cast<time_t>(ONE_MONTH_SECOND);
    return timestamp < beforeOneMonthTimestamp;
}

#ifdef SUPPORT_TRAFFIC_STATISTIC
std::string NetStatsUtils::GetIccIdBySlotId(int32_t slotId)
{
    std::u16string u16Hplmn;
    Telephony::CoreServiceClient::GetInstance().GetSimIccId(slotId, u16Hplmn);
    return OHOS::Str16ToStr8(u16Hplmn);
}

std::string NetStatsUtils::GetIccIdBySimId(int32_t simId)
{
    if (!IsSimIdValid(simId)) {
        return "";
    }
    int32_t slotId = Telephony::CoreServiceClient::GetInstance().GetSlotId(simId);
    std::u16string u16Hplmn;
    int32_t ret = Telephony::CoreServiceClient::GetInstance().GetSimIccId(slotId, u16Hplmn);
    if (ret != 0) {
        NETMGR_LOG_E("GetSimIccId error. slot:%{public}d, ret:%{public}d", slotId, ret);
        return OHOS::Str16ToStr8(u16Hplmn);
    }
    return OHOS::Str16ToStr8(u16Hplmn);
}

bool NetStatsUtils::IsSimIdValid(int32_t simId)
{
    int32_t slotId = Telephony::CoreServiceClient::GetInstance().GetSlotId(simId);
    if (!IsSlotIdValid(slotId)) {
        NETMGR_LOG_E("simId:%{public}d, error", simId);
        return false;
    }
    return true;
}

bool NetStatsUtils::IsSlotIdValid(int32_t slotId)
{
    if (slotId != SLOT_0 && slotId != SLOT_1) {
        NETMGR_LOG_E("slotId:%{public}d, error", slotId);
        return false;
    }
    return true;
}
#endif
}
}

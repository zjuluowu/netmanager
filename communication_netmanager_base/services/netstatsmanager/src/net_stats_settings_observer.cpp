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
#include <cinttypes>
#include "net_stats_settings_observer.h"
#include "net_datashare_utils.h"
#include "net_mgr_log_wrapper.h"
#include "net_manager_constants.h"
#include "net_stats_service.h"
#include "net_stats_client.h"
#include "net_stats_utils.h"
namespace OHOS {
namespace NetManagerStandard {

static constexpr const char *CELLULAR_DATA_SETTING_DATA_ENABLE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=cellular_data_enable";

static constexpr const char *SETTING_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=";

const std::string UNLIMITED_TRAFFIC_ENABLE  = "unlimited_traffic_enable";
const std::string MONTHLY_LIMITED_TRAFFIC = "monthly_limited_traffic";
const std::string MONTHLY_BEGIN_DATE = "monthly_start_date";
const std::string MONTHLY_NOTIFY_TYPE = "monthly_notify_type";
const std::string OVER_MONTHLY_MARK = "over_monthly_mark";
const std::string OVER_DAILY_MARK = "over_daily_mark";
const std::string DISPLAY_TRAFFIC_SWITCH = "traffic_switch";
const std::string TAG_NAME = "net_stats_";

TrafficDataObserver::TrafficDataObserver(int32_t simId)
{
    NETMGR_LOG_I("TrafficDataObserver start. simId: %{public}d", simId);
    simId_ = simId;
}

void TrafficDataObserver::ReadTrafficDataSettings(TrafficPlanInfo &info)
{
    NETMGR_LOG_E("ReadTrafficDataSettings start.");
    auto dataShareHelperUtils = std::make_unique<NetDataShareHelperUtils>();
    // 读取流量设置
    Uri unLimitUri(SETTING_URI + TAG_NAME + std::to_string(simId_) + "_" + UNLIMITED_TRAFFIC_ENABLE);
    std::string value = "";
    dataShareHelperUtils->Query(unLimitUri, TAG_NAME + std::to_string(simId_) + "_" + UNLIMITED_TRAFFIC_ENABLE, value);
    info.unlimitTrafficSwitch = 0;
    int32_t enable = 0;
    if (!value.empty() && NetStatsUtils::ConvertToInt32(value, enable)) {
        info.unlimitTrafficSwitch = enable;
    }

    Uri mLimitUri(SETTING_URI + TAG_NAME + std::to_string(simId_) + "_" + MONTHLY_LIMITED_TRAFFIC);
    value = "";
    dataShareHelperUtils->Query(mLimitUri, TAG_NAME + std::to_string(simId_) + "_" + MONTHLY_LIMITED_TRAFFIC, value);
    info.trafficLimit = UINT64_MAX;
    uint64_t trafficInt = 0;
    if (!value.empty() && NetStatsUtils::ConvertToUint64(value, trafficInt)) {
        info.trafficLimit = trafficInt;
    }

    Uri beginTimeUri(SETTING_URI + TAG_NAME + std::to_string(simId_) + "_" + MONTHLY_BEGIN_DATE);
    value = "";
    dataShareHelperUtils->Query(beginTimeUri, TAG_NAME + std::to_string(simId_) + "_" + MONTHLY_BEGIN_DATE, value);
    info.startDate = 1;
    int32_t dateInt = 0;
    if (!value.empty() && NetStatsUtils::ConvertToInt32(value, dateInt)) {
        info.startDate = dateInt;
    }

    ReadTrafficDataSettingsPart2(info);
}

void TrafficDataObserver::ReadTrafficDataSettingsPart2(TrafficPlanInfo &info)
{
    NETMGR_LOG_E("ReadTrafficDataSettings part2 start.");
    auto dataShareHelperUtils = std::make_unique<NetDataShareHelperUtils>();
    Uri typeUri(SETTING_URI + TAG_NAME + std::to_string(simId_) + "_" + MONTHLY_NOTIFY_TYPE);
    std::string value = "";
    dataShareHelperUtils->Query(typeUri, TAG_NAME + std::to_string(simId_) + "_" + MONTHLY_NOTIFY_TYPE, value);
    info.overLimitBehavior = 1; // 1:默认断网
    int32_t type = 0;
    if (!value.empty() && NetStatsUtils::ConvertToInt32(value, type)) {
        info.overLimitBehavior = type;
    }

    Uri mMarkuri(SETTING_URI + TAG_NAME + std::to_string(simId_) + "_" + OVER_MONTHLY_MARK);
    value = "";
    dataShareHelperUtils->Query(mMarkuri, TAG_NAME + std::to_string(simId_) + "_" + OVER_MONTHLY_MARK, value);
    info.monthlyLimitPercentage = 80;  // 月限额比例默认80%
    int32_t mMark = 0;
    if (!value.empty() && NetStatsUtils::ConvertToInt32(value, mMark)) {
        info.monthlyLimitPercentage = mMark;
    }

    Uri dMarkuri(SETTING_URI + TAG_NAME + std::to_string(simId_) + "_" + OVER_DAILY_MARK);
    value = "";
    dataShareHelperUtils->Query(dMarkuri, TAG_NAME + std::to_string(simId_) + "_" + OVER_DAILY_MARK, value);
    info.dailyLimitPercentage = 10;  // 日限额比例默认10%
    int32_t dMark = 0;
    if (!value.empty() && NetStatsUtils::ConvertToInt32(value, dMark)) {
        info.dailyLimitPercentage = dMark;
    }

    Uri displayuri(SETTING_URI + TAG_NAME + std::to_string(simId_) + "_" + DISPLAY_TRAFFIC_SWITCH);
    value = "";
    dataShareHelperUtils->Query(displayuri, TAG_NAME + std::to_string(simId_) + "_" + DISPLAY_TRAFFIC_SWITCH, value);
    info.displayTrafficSwitch = 0;  // 显示流量 默认0
    int32_t display = 0;
    if (!value.empty() && NetStatsUtils::ConvertToInt32(value, display)) {
        info.displayTrafficSwitch = display;
    }
}
// LCOV_EXCL_STOP
} // namespace NetManagerStandard
} // namespace OHOS
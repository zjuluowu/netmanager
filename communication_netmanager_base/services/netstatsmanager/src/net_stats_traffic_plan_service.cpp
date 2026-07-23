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

#include "net_stats_traffic_plan_service.h"

#include <ctime>
#include <map>

#include "net_mgr_log_wrapper.h"
#include "net_manager_constants.h"
#include "net_stats_utils.h"
#include "net_stats_data_clone.h"
#include "net_stats_rdb.h"
#include "netmanager_base_common_utils.h"
#include "unique_fd.h"
#include "core_service_client.h"
#include "net_stats_constants.h"
#include "net_stats_settings_observer.h"


namespace OHOS {
namespace NetManagerStandard {

constexpr const char *EXTENSION_SUCCESS = "netstats extension success";
constexpr const char *EXTENSION_FAIL = "netstats extension fail";

void NetStatsTrafficPlanService::InitTrafficPlanInfo(int32_t simId)
{
    NETMGR_LOG_I("InitTrafficPlanInfo start, simId: %{public}d", simId);

    std::lock_guard<std::mutex> lock(mutex_);
    if (trafficPlanInfoMap_.find(simId) != trafficPlanInfoMap_.end()) {
        NETMGR_LOG_E("Map has simId: %{public}d", simId);
        return;
    }

    int32_t slotId = Telephony::CoreServiceClient::GetInstance().GetSlotId(simId);
    if (!NetStatsUtils::IsSlotIdValid(slotId)) {
        NETMGR_LOG_E("slotId invalid, value: %{public}d", slotId);
        return;
    }

    NetStatsRDB rdb;
    TrafficPlanInfo info;
    info.slotId = slotId;
    std::string iccid = NetStatsUtils::GetIccIdBySimId(simId);

    int32_t ret = rdb.QueryTrafficPlanInfoByIccid(iccid, info);
    if (ret == NETMANAGER_SUCCESS) {
        trafficPlanInfoMap_[simId] = std::make_shared<TrafficPlanInfo>(info);
        NETMGR_LOG_I("loaded traffic plan info: %{public}s", info.ToString().c_str());
        return;
    }

    TrafficPlanInfo infoNew;
    auto trafficDataObserver = std::make_shared<TrafficDataObserver>(simId);
    trafficDataObserver->ReadTrafficDataSettings(infoNew);
    NETMGR_LOG_E("Failed to query traffic plan info for simId: %{public}d from database", simId);

    infoNew.simId = simId;
    infoNew.iccid = NetStatsUtils::GetIccIdBySimId(simId);
    infoNew.slotId = slotId;

    trafficPlanInfoMap_[simId] = std::make_shared<TrafficPlanInfo>(infoNew);
    NETMGR_LOG_I("Need insert traffic info: %{public}s", infoNew.ToString().c_str());
    rdb.InsertOrUpdateTrafficPlanInfo(infoNew);
    return;
}

void NetStatsTrafficPlanService::DeleteTrafficPlanInfo(int32_t slotId)
{
    NETMGR_LOG_I("DeleteTrafficPlanInfo start, slotId: %{public}d, map size:%{public}lu",
        slotId, trafficPlanInfoMap_.size());

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = trafficPlanInfoMap_.begin(); iter != trafficPlanInfoMap_.end();) {
        if (iter->second->slotId == slotId) {
            iter = trafficPlanInfoMap_.erase(iter);
            break;
        } else {
            ++iter;
        }
    }
    NETMGR_LOG_I("DeleteTrafficPlanInfo success, slotId: %{public}d,  map size:%{public}lu",
        slotId, trafficPlanInfoMap_.size());
}

int32_t NetStatsTrafficPlanService::SetTrafficPlanInfo(int32_t simId, TrafficPlanParam param, int64_t value)
{
    NETMGR_LOG_I("SetTrafficPlanInfo start, simId: %{public}d, param: %{public}d, value: %{public}" PRId64,
                 simId, static_cast<int32_t>(param), value);

    if (!NetStatsUtils::IsSimIdValid(simId)) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    if (ValidateTrafficPlanParam(param, value) != NETMANAGER_SUCCESS) {
        return TRAFFIC_PLAN_ERR_INVALID_PARAM;
    }

    // Update cache
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = trafficPlanInfoMap_.find(simId);
    if (it != trafficPlanInfoMap_.end() && it->second != nullptr) {
        auto infoPtr = it->second;
        switch (param) {
            case TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH:
                infoPtr->displayTrafficSwitch = static_cast<int32_t>(value);
                break;
            case TrafficPlanParam::UNLIMIT_TRAFFIC_SWITCH:
                infoPtr->unlimitTrafficSwitch = static_cast<int32_t>(value);
                break;
            case TrafficPlanParam::TRAFFIC_LIMIT:
                infoPtr->trafficLimit = (value == -1) ? UINT64_MAX : static_cast<uint64_t>(value);
                break;
            case TrafficPlanParam::START_DATE:
                infoPtr->startDate = static_cast<int32_t>(value);
                break;
            case TrafficPlanParam::OVER_LIMIT_BEHAVIOR:
                infoPtr->overLimitBehavior = static_cast<int32_t>(value);
                break;
            case TrafficPlanParam::MONTHLY_LIMIT_PERCENTAGE:
                infoPtr->monthlyLimitPercentage = static_cast<int32_t>(value);
                infoPtr->isCanNotifyMonthlyMark = true;
                break;
            case TrafficPlanParam::DAILY_LIMIT_PERCENTAGE:
                infoPtr->dailyLimitPercentage = static_cast<int32_t>(value);
                infoPtr->isCanNotifyDailyMark = true;
                break;
            default:
                break;
        }

        NETMGR_LOG_I("Updated cache for simId: %{public}d, info:%{public}s", simId, infoPtr->ToString().c_str());
    }

    NetStatsRDB rdb;
    std::string iccid = NetStatsUtils::GetIccIdBySimId(simId);
    int32_t ret = rdb.UpdateTrafficPlanParam(iccid, param, value);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetStatsTrafficPlanService::GetTrafficPlanInfo(int32_t simId, TrafficPlanParam param, int64_t &value)
{
    NETMGR_LOG_I("GetTrafficPlanInfo start, simId: %{public}d, param: %{public}d",
                 simId, static_cast<int32_t>(param));

    if (!NetStatsUtils::IsSimIdValid(simId)) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    auto it = trafficPlanInfoMap_.find(simId);
    if (it != trafficPlanInfoMap_.end()) {
        TrafficPlanInfo info = *(it->second);
        value = GetFieldValueByParam(info, param);
        NETMGR_LOG_I("GetTrafficPlanInfo success from cache, value: %{public}" PRId64, value);
        return NETMANAGER_SUCCESS;
    }

    TrafficPlanInfo info;
    NetStatsRDB rdb;
    std::string iccid = NetStatsUtils::GetIccIdBySimId(simId);
    int32_t ret = rdb.QueryTrafficPlanInfoByIccid(iccid, info);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Failed to query traffic plan info from database");
        return ret;
    }

    value = GetFieldValueByParam(info, param);
    trafficPlanInfoMap_[simId] = std::make_shared<TrafficPlanInfo>(info);
    NETMGR_LOG_I("GetTrafficPlanInfo success from database, value: %{public}" PRId64, value);
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsTrafficPlanService::ValidateTrafficPlanParam(TrafficPlanParam param, int64_t value)
{
    switch (param) {
        case TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH:
        case TrafficPlanParam::UNLIMIT_TRAFFIC_SWITCH:
            if (value != 0 && value != 1) {   // 0：关闭  1：打开
                return TRAFFIC_PLAN_ERR_INVALID_PARAM;
            }
            break;
            
        case TrafficPlanParam::TRAFFIC_LIMIT:
            if (value < -1) {  // -1 ：未设置
                return TRAFFIC_PLAN_ERR_INVALID_PARAM;
            }
            break;
            
        case TrafficPlanParam::START_DATE:
            if (value < 1 || value > 31) {  // day 1~31
                return TRAFFIC_PLAN_ERR_INVALID_PARAM;
            }
            break;
            
        case TrafficPlanParam::OVER_LIMIT_BEHAVIOR:
            if (value != 1 && value != 0) {  // 0: 提醒  1：断网
                return TRAFFIC_PLAN_ERR_INVALID_PARAM;
            }
            break;
            
        case TrafficPlanParam::MONTHLY_LIMIT_PERCENTAGE:
        case TrafficPlanParam::DAILY_LIMIT_PERCENTAGE:
            if (value < 0 || value > 100) {  // percent 0~100
                return TRAFFIC_PLAN_ERR_INVALID_PARAM;
            }
            break;
            
        default:
            NETMGR_LOG_E("Unknown traffic plan param: %{public}d", static_cast<int32_t>(param));
            return TRAFFIC_PLAN_ERR_INVALID_PARAM;
    }
    
    return NETMANAGER_SUCCESS;
}

int64_t NetStatsTrafficPlanService::GetFieldValueByParam(const TrafficPlanInfo &info, TrafficPlanParam param)
{
    switch (param) {
        case TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH:
            return static_cast<int64_t>(info.displayTrafficSwitch);
        case TrafficPlanParam::UNLIMIT_TRAFFIC_SWITCH:
            return static_cast<int64_t>(info.unlimitTrafficSwitch);
        case TrafficPlanParam::TRAFFIC_LIMIT:
            return (info.trafficLimit >= INT64_MAX) ? -1 : static_cast<int64_t>(info.trafficLimit);
        case TrafficPlanParam::START_DATE:
            return static_cast<int64_t>(info.startDate);
        case TrafficPlanParam::OVER_LIMIT_BEHAVIOR:
            return static_cast<int64_t>(info.overLimitBehavior);
        case TrafficPlanParam::MONTHLY_LIMIT_PERCENTAGE:
            return static_cast<int64_t>(info.monthlyLimitPercentage);
        case TrafficPlanParam::DAILY_LIMIT_PERCENTAGE:
            return static_cast<int64_t>(info.dailyLimitPercentage);
        default:
            return -1;
    }
}

int32_t NetStatsTrafficPlanService::OnBackup(MessageParcel& data, MessageParcel& reply)
{
    NETMGR_LOG_I("OnBackup start");
    UniqueFd fd(-1);
    std::string replyCode = EXTENSION_SUCCESS;
    std::string tmp = "";
    int ret = NetStatsDataClone::GetInstance().OnBackup(fd, tmp);
    if (ret < 0) {
        NETMGR_LOG_E("OnBackup fail: backup data fail!");
        replyCode = EXTENSION_FAIL;
    }
    if (reply.WriteFileDescriptor(fd) == false || reply.WriteString(replyCode) == false) {
        close(fd.Release());
        CommonUtils::DeleteFile(NET_STATS_DATA_BACKUP_FILE);
        NETMGR_LOG_E("OnBackup fail: reply write fail!");
        return -1;
    }
    close(fd.Release());
    CommonUtils::DeleteFile(NET_STATS_DATA_BACKUP_FILE);
    return 0;
}

int32_t NetStatsTrafficPlanService::OnRestore(MessageParcel& data, MessageParcel& reply)
{
    UniqueFd fd(data.ReadFileDescriptor());

    std::string replyCode = EXTENSION_SUCCESS;
    std::string tmp = "";
    int ret = NetStatsDataClone::GetInstance().OnRestore(fd, tmp);
    if (ret < 0) {
        NETMGR_LOG_E("OnRestore fail: restore data fail! ret:%{public}d", ret);
        replyCode = EXTENSION_FAIL;
        return -1;
    }
    if (reply.WriteString(replyCode) == false) {
        close(fd.Release());
        CommonUtils::DeleteFile(NET_STATS_DATA_BACKUP_FILE);
        NETMGR_LOG_E("OnRestore fail: reply write fail!");
        return -1;
    }
    close(fd.Release());
    CommonUtils::DeleteFile(NET_STATS_DATA_BACKUP_FILE);
    return 0;
}

void NetStatsTrafficPlanService::ResetNotifyState(int32_t simId)
{
    if (!IsSimIdExistInMap(simId)) {
        NETMGR_LOG_E("UpdateTrafficLimitDate err. Not find simId:%{public}d", simId);
        return;
    }

    NETMGR_LOG_I("ResetNotifyState start");

    auto info = GetTrafficPlanInfoBySimId(simId);
    if (info == nullptr) {
        NETMGR_LOG_E("GetTrafficPlanInfoBySimId fail");
        return;
    }
    info->isCanNotifyMonthlyLimit = true;
    info->isCanNotifyDailyMark = true;
    info->isCanNotifyMonthlyMark = true;

    UpdateTrafficLimitDate(simId);
}

void NetStatsTrafficPlanService::UpdateTrafficLimitDate(int32_t simId)
{
    NETMGR_LOG_I("UpdateTrafficLimitDate start");
    NetStatsRDB netStats;
    NetStatsData statsData;

    auto trafficPlaninfoPtr = GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return;
    }

    statsData.simId = simId;
    statsData.monWarningDate = trafficPlaninfoPtr->lastMonAlertTime;
    statsData.dayNoticeDate = trafficPlaninfoPtr->lastDayNotifyTime;
    statsData.monNoticeDate = trafficPlaninfoPtr->lastMonNotifyTime;
    statsData.monWarningState = trafficPlaninfoPtr->isCanNotifyMonthlyLimit;
    statsData.dayNoticeState = trafficPlaninfoPtr->isCanNotifyDailyMark;
    statsData.monNoticeState = trafficPlaninfoPtr->isCanNotifyMonthlyMark;

    netStats.InsertData(statsData);
}

bool NetStatsTrafficPlanService::IsSimIdExistInMap(int32_t simId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (trafficPlanInfoMap_.find(simId) == trafficPlanInfoMap_.end()) {
        return false;
    }
    return true;
}

std::shared_ptr<TrafficPlanInfo> NetStatsTrafficPlanService::GetTrafficPlanInfoBySimId(int32_t simId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (trafficPlanInfoMap_.find(simId) == trafficPlanInfoMap_.end()) {
        return nullptr;
    }
    return trafficPlanInfoMap_[simId];
}

bool NetStatsTrafficPlanService::GetMonthlyLimitBySimId(int32_t simId, uint64_t &monthlyLimit)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (trafficPlanInfoMap_.find(simId) == trafficPlanInfoMap_.end()) {
        return false;
    }
    if (trafficPlanInfoMap_[simId] == nullptr) {
        trafficPlanInfoMap_.erase(simId);
        return false;
    }
    monthlyLimit = trafficPlanInfoMap_[simId]->trafficLimit;
    return true;
}

bool NetStatsTrafficPlanService::GetMonthlyMarkBySimId(int32_t simId, uint16_t &monthlyMark)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (trafficPlanInfoMap_.find(simId) == trafficPlanInfoMap_.end()) {
        return false;
    }
    monthlyMark = trafficPlanInfoMap_[simId]->monthlyLimitPercentage;
    return true;
}

bool NetStatsTrafficPlanService::GetDailyMarkBySimId(int32_t simId, uint16_t &dailyMark)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (trafficPlanInfoMap_.find(simId) == trafficPlanInfoMap_.end()) {
        return false;
    }
    dailyMark = trafficPlanInfoMap_[simId]->dailyLimitPercentage;
    return true;
}

void NetStatsTrafficPlanService::UpdateNetStatsToMapFromDB(int32_t simId)
{
    NETMGR_LOG_I("UpdateNetStatsToMapFromDB enter.");
    NetStatsRDB netStats;
    
    std::vector<NetStatsData> result = netStats.QueryAll();
    std::lock_guard<std::mutex> lock(mutex_);
    for (size_t i = 0; i < result.size(); i++) {
        int32_t curSumId = result[i].simId;
        if (simId == curSumId && trafficPlanInfoMap_.find(simId) != trafficPlanInfoMap_.end()) {
            trafficPlanInfoMap_[curSumId]->lastMonAlertTime = result[i].monWarningDate;
            trafficPlanInfoMap_[curSumId]->lastMonNotifyTime = result[i].dayNoticeDate;
            trafficPlanInfoMap_[curSumId]->lastDayNotifyTime = result[i].monNoticeDate;
            trafficPlanInfoMap_[curSumId]->isCanNotifyMonthlyLimit =
                static_cast<bool>(result[i].monWarningState);
            trafficPlanInfoMap_[curSumId]->isCanNotifyMonthlyMark = static_cast<bool>(result[i].monNoticeState);
            trafficPlanInfoMap_[curSumId]->isCanNotifyDailyMark = static_cast<bool>(result[i].dayNoticeState);
        }
    }
}

bool NetStatsTrafficPlanService::TrafficPlanParamToFlag(TrafficPlanParam param, uint8_t &flag)
{
    switch (param) {
        case TrafficPlanParam::UNLIMIT_TRAFFIC_SWITCH:
            flag = NET_STATS_NO_LIMIT_ENABLE;
            return true;
        case TrafficPlanParam::TRAFFIC_LIMIT:
            flag = NET_STATS_MONTHLY_LIMIT;
            return true;
        case TrafficPlanParam::START_DATE:
            flag =  NET_STATS_BEGIN_DATE;
            return true;
        case TrafficPlanParam::OVER_LIMIT_BEHAVIOR:
            flag =  NET_STATS_NOTIFY_TYPE;
            return true;
        case TrafficPlanParam::MONTHLY_LIMIT_PERCENTAGE:
            flag =  NET_STATS_MONTHLY_MARK;
            return true;
        case TrafficPlanParam::DAILY_LIMIT_PERCENTAGE:
            flag =  NET_STATS_DAILY_MARK;
            return true;
        default:
            return false;
    }
}

} // namespace NetManagerStandard
} // namespace OHOS

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

#include <cinttypes>
#include "net_stats_calibrate.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_data_handler.h"
#include "net_stats_utils.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {

void NetStatsCalibrate::InitChangeToIfaceTime()
{
    uint32_t time = UINT32_MAX;
    ReadChangeToIfaceTime(time);
    if (time != UINT32_MAX) {
        std::lock_guard<std::mutex> lock(timeMutex_);
        changeToIfaceTime_ = time;
        NETMGR_LOG_I("init changeToIfaceTime:%{public}d", changeToIfaceTime_);
    }
    uint32_t currentTime = CommonUtils::GetCurrentSecond();
    UpdateChangeToIfaceTime(currentTime);
}

uint32_t NetStatsCalibrate::GetChangeToIfaceTime()
{
    std::lock_guard<std::mutex> lock(timeMutex_);
    return changeToIfaceTime_;
}

void NetStatsCalibrate::UpdateChangeToIfaceTime(uint32_t startTime)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    NETMGR_LOG_I(" UpdateChangeToIfaceTime %{public}d", startTime);
    std::lock_guard<std::mutex> lock(timeMutex_);
    if (changeToIfaceTime_ != UINT32_MAX) {
        return;
    }
    changeToIfaceTime_ = startTime;
    auto handler = std::make_unique<NetStatsDataHandler>();
    handler->WriteChangeToIfaceTime(startTime);
#else
    NETMGR_LOG_I("UpdateChangeToIfaceTime error. curr device not support traffic calibrate");
#endif
}

bool NetStatsCalibrate::InitCalibrationInfo(uint32_t simId)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    if (calibrateInfo_.find(simId) != calibrateInfo_.end()) {
        NETMGR_LOG_I("has calicrationInfo, no need read DB. simId:%{public}d", simId);
        return true;
    }
    CalibrateInfo info;
    ReadCalibrationTrafficInfo(simId, info);
    if (info.startTime == 0 || info.endTime == 0) {
        NETMGR_LOG_E("no CalicrationInfo. simId:%{public}d", simId);
        return false;
    }

    std::lock_guard<std::mutex> lock(calibrateInfoMutex_);

    if (calibrateInfo_.find(simId) == calibrateInfo_.end()) {
        CalibrateInfo infoTmp;
        calibrateInfo_.insert({simId, infoTmp});
    }

    calibrateInfo_[simId].startTime = info.startTime;
    calibrateInfo_[simId].endTime = info.endTime;
    calibrateInfo_[simId].usedTraffic = info.usedTraffic;
    NETMGR_LOG_I("InitCalibrationInfo simId:%{public}d, start:%{public}d, end:%{public}d, data:%{public}" PRIu64,
        simId, calibrateInfo_[simId].startTime, calibrateInfo_[simId].endTime, calibrateInfo_[simId].usedTraffic);
    return true;
#else
    NETMGR_LOG_I("UpdateCalibrationInfo error. curr device not support traffic calibrate");
    return false;
#endif
}

bool NetStatsCalibrate::GetCalibrationInfo(uint32_t simId, CalibrateInfo &info)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    std::lock_guard<std::mutex> lock(calibrateInfoMutex_);
    if (calibrateInfo_.find(simId) == calibrateInfo_.end()) {
        return false;
    }
    info = calibrateInfo_[simId];
    return true;
#else
    NETMGR_LOG_I("UpdateCalibrationInfo error. curr device not support traffic calibrate");
    return false;
#endif
}

void NetStatsCalibrate::UpdateCalibrationInfo(uint32_t simId, uint64_t usedTraffic)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    NETMGR_LOG_I("UpdateCalibrationInfo. simId:%{public}d, usedTraffic:%{public}" PRIu64, simId, usedTraffic);
    std::lock_guard<std::mutex> lock(calibrateInfoMutex_);
    CalibrateInfo info;
    if (calibrateInfo_.find(simId) == calibrateInfo_.end()) {
        calibrateInfo_.insert({simId, info});
    }
    calibrateInfo_[simId].startTime = static_cast<uint64_t>(NetStatsUtils::GetStartTimestamp(1));  // 1: month begin day
    calibrateInfo_[simId].endTime = CommonUtils::GetCurrentSecond();
    calibrateInfo_[simId].usedTraffic = usedTraffic;
    auto handler = std::make_unique<NetStatsDataHandler>();
    handler->WriteCalibrationTrafficInfo(simId, calibrateInfo_[simId].startTime,
        calibrateInfo_[simId].endTime, calibrateInfo_[simId].usedTraffic);
#else
    NETMGR_LOG_I("UpdateCalibrationInfo error. curr device not support traffic calibrate");
#endif
}

void NetStatsCalibrate::ReadCalibrationTrafficInfo(uint32_t simId, CalibrateInfo &info)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    auto handler = std::make_unique<NetStatsDataHandler>();
    uint32_t startTime = 0;
    uint32_t endTime = 0;
    uint64_t usedTraffic = 0;
    int32_t ret = handler->ReadCalibrationTrafficInfo(simId, startTime, endTime, usedTraffic);
    info.startTime = startTime;
    info.endTime = endTime;
    info.usedTraffic = usedTraffic;
    NETMGR_LOG_I("ReadCalibrationTrafficInfo startTime:%{public}d, endTime:%{public}d, usedTraffic:%{public}" PRIu64,
        startTime, endTime, usedTraffic);
#else
    NETMGR_LOG_I("UpdateCalibrationInfo error. curr device not support traffic calibrate");
#endif
}

void NetStatsCalibrate::ReadChangeToIfaceTime(uint32_t &startTime)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    auto handler = std::make_unique<NetStatsDataHandler>();
    int32_t ret = handler->ReadChangeToIfaceTime(startTime);
    NETMGR_LOG_I("ReadChangeToIfaceTime ret: %{public}d", ret);
#else
    NETMGR_LOG_I("UpdateCalibrationInfo error. curr device not support traffic calibrate");
#endif
}

bool NetStatsCalibrate::IsExistCalibrationInfo(uint32_t simId)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    CalibrateInfo info;
    ReadCalibrationTrafficInfo(simId, info);
    if (info.startTime == 0 || info.endTime == 0) {
        NETMGR_LOG_I("IsExistCalibrationInfo false");
        return false;
    }
    NETMGR_LOG_I("IsExistCalibrationInfo true");
    return true;
#else
    NETMGR_LOG_I("IsExistCalibrationInfo error. curr device not support traffic calibrate");
    return false;
#endif
}

bool NetStatsCalibrate::DeleteCalibrationInfo(uint32_t simId)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    auto handler = std::make_unique<NetStatsDataHandler>();
    int32_t ret = handler->DeleteCalibrationTrafficInfo(simId);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("DeleteCalibrationTrafficInfo error: %{public}d", ret);
        return false;
    }
    return true;
#else
    NETMGR_LOG_I("IsExistCalibrationInfo error. curr device not support traffic calibrate");
    return false;
#endif
}
// #endif
}
}
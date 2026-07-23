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

#ifndef NET_STATS_CALIBRATE_H
#define NET_STATS_CALIBRATE_H
#include <map>
#include <mutex>

namespace OHOS {
namespace NetManagerStandard {
typedef struct {
    uint32_t startTime;
    uint32_t endTime;
    uint64_t usedTraffic;
} CalibrateInfo;

class NetStatsCalibrate {
public:
    void InitChangeToIfaceTime();
    uint32_t GetChangeToIfaceTime();
    void UpdateChangeToIfaceTime(uint32_t startTime);
    bool InitCalibrationInfo(uint32_t simId);
    bool GetCalibrationInfo(uint32_t simId, CalibrateInfo &info);
    void UpdateCalibrationInfo(uint32_t simId, uint64_t usedTraffic);
    void ReadCalibrationTrafficInfo(uint32_t simId, CalibrateInfo &info);
    void ReadChangeToIfaceTime(uint32_t &startTime);
    bool IsExistCalibrationInfo(uint32_t simId);
    bool DeleteCalibrationInfo(uint32_t simId);

private:
    std::map<uint32_t, CalibrateInfo> calibrateInfo_;
    uint32_t changeToIfaceTime_ = UINT32_MAX;
    std::mutex timeMutex_;
    std::mutex calibrateInfoMutex_;
};
}
}
#endif
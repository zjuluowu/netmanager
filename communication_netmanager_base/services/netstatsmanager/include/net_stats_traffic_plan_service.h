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

#ifndef NET_STATS_TRAFFIC_PLAN_SERVICE_H
#define NET_STATS_TRAFFIC_PLAN_SERVICE_H

#include <cstdint>
#include <mutex>
#include <string>
#include <map>
#include <message_parcel.h>
#include "traffic_plan_param.h"

namespace OHOS {
namespace NetManagerStandard {

class NetStatsTrafficPlanService {
public:
    int32_t SetTrafficPlanInfo(int32_t simId, TrafficPlanParam param, int64_t value);
    int32_t GetTrafficPlanInfo(int32_t simId, TrafficPlanParam param, int64_t &value);
    std::string GetIccidBySimId(int32_t simId);
    void InitTrafficPlanInfo(int32_t simId);
    void DeleteTrafficPlanInfo(int32_t simId);
    int32_t OnBackup(MessageParcel& data, MessageParcel& reply);
    int32_t OnRestore(MessageParcel& data, MessageParcel& reply);

public:
    void ResetNotifyState(int32_t simId);
    std::shared_ptr<TrafficPlanInfo> GetTrafficPlanInfoBySimId(int32_t simId);
    bool GetMonthlyLimitBySimId(int32_t simId, uint64_t &monthlyLimit);
    bool GetMonthlyMarkBySimId(int32_t simId, uint16_t &monthlyMark);
    bool GetDailyMarkBySimId(int32_t simId, uint16_t &dailyMark);
    void UpdateNetStatsToMapFromDB(int32_t simId);
    bool IsSimIdExistInMap(int32_t simId);
    void UpdateTrafficLimitDate(int32_t simId);
    bool TrafficPlanParamToFlag(TrafficPlanParam param, uint8_t &flag);
    int32_t ValidateTrafficPlanParam(TrafficPlanParam param, int64_t value);

private:
    int64_t GetFieldValueByParam(const TrafficPlanInfo &info, TrafficPlanParam param);
    std::string GetIccidFromSystem(int32_t simId);
    std::mutex mutex_;
    std::map<int32_t, std::shared_ptr<TrafficPlanInfo>> trafficPlanInfoMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_STATS_TRAFFIC_PLAN_SERVICE_H

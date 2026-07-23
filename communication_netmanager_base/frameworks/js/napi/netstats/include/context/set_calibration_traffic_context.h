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

#ifndef COMMUNICATION_NETMANAGER_BASE_SET_CALIBRATION_TRAFFIC_CONTEXT_H
#define COMMUNICATION_NETMANAGER_BASE_SET_CALIBRATION_TRAFFIC_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include <napi/native_api.h>

#include "base_context.h"

namespace OHOS {
namespace NetManagerStandard {
class SetCalibrationTrafficContext final : public BaseContext {
public:
    SetCalibrationTrafficContext() = delete;
    explicit SetCalibrationTrafficContext(napi_env env, std::shared_ptr<EventManager>& manager);

    uint64_t GetTotalMonthlyData() const;
    int64_t GetRemainingData() const;
    uint32_t GetSimId() const;

    void ParseParams(napi_value *params, size_t paramsCount);

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);

    bool CheckNetworkParams(napi_value *params, size_t paramsCount);

private:
    uint32_t simId_ = 0;
    int64_t remainingData_ = 0;
    uint64_t totalMonthlyData_ = UINT64_MAX;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_NETMANAGER_BASE_SET_CALIBRATION_TRAFFIC_CONTEXT_H
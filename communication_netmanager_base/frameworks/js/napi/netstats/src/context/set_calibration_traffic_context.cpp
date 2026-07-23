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

#include "set_calibration_traffic_context.h"

#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {

SetCalibrationTrafficContext::SetCalibrationTrafficContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager)
{
    SetReleaseVersion(API_VERSION_26);
}

void SetCalibrationTrafficContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("checkParamsType false");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    simId_ = NapiUtils::GetUint32FromValue(GetEnv(), params[ARG_INDEX_0]);
    remainingData_ = NapiUtils::GetInt64FromValue(GetEnv(), params[ARG_INDEX_1]);
    NETMANAGER_BASE_LOGD("get remainingData_ %{public}" PRId64, remainingData_);

    if (paramsCount == PARAM_TRIPLE_OPTIONS) {
        int64_t totalDataTmp = NapiUtils::GetInt64FromValue(GetEnv(), params[ARG_INDEX_2]);
        if (totalDataTmp < 0) {
            NETMANAGER_BASE_LOGE("totalDataTmp < 0, err");
            SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
            SetNeedThrowException(true);
            return;
        }
        totalMonthlyData_ = static_cast<uint64_t>(totalDataTmp);
        NETMANAGER_BASE_LOGD("get totalMonthlyData_: %{public}" PRIu64, totalMonthlyData_);
        if (totalMonthlyData_ == UINT64_MAX) {
            NETMANAGER_BASE_LOGE("get totalMonthlyData_ false");
            SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
            SetNeedThrowException(true);
            return;
        }
    }
    SetParseOK(true);
}

bool SetCalibrationTrafficContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_number;
    }
    if (paramsCount == PARAM_TRIPLE_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_2]) == napi_number;
    }
    return false;
}

uint64_t SetCalibrationTrafficContext::GetTotalMonthlyData() const
{
    return totalMonthlyData_;
}

int64_t SetCalibrationTrafficContext::GetRemainingData() const
{
    return remainingData_;
}

uint32_t SetCalibrationTrafficContext::GetSimId() const
{
    return simId_;
}

} // namespace NetManagerStandard
} // namespace OHOS

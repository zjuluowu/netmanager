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

#include "get_traffic_plan_info_context.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {

GetTrafficPlanInfoContext::GetTrafficPlanInfoContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) { SetReleaseVersion(API_VERSION_26); }

void GetTrafficPlanInfoContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }
    simId_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]);
    param_ = static_cast<TrafficPlanParam>(NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_1]));
    SetParseOK(true);
}

bool GetTrafficPlanInfoContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    return paramsCount == PARAM_DOUBLE_OPTIONS &&
           NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
           NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_number;
}

int32_t GetTrafficPlanInfoContext::GetSimId() const { return simId_; }
TrafficPlanParam GetTrafficPlanInfoContext::GetParam() const { return param_; }
int64_t GetTrafficPlanInfoContext::GetValue() const { return value_; }
void GetTrafficPlanInfoContext::SetValue(int64_t value) { value_ = value; }

} // namespace NetManagerStandard
} // namespace OHOS

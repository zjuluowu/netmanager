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

#include "set_traffic_plan_info_context.h"

#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {

SetTrafficPlanInfoContext::SetTrafficPlanInfoContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager)
{
    SetReleaseVersion(API_VERSION_26);
}

void SetTrafficPlanInfoContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("checkParamsType false");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    simId_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]);
    param_ = static_cast<TrafficPlanParam>(NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_1]));
    value_ = NapiUtils::GetInt64FromValue(GetEnv(), params[ARG_INDEX_2]);
    NETMANAGER_BASE_LOGI("SetTrafficPlanInfoContext: simId=%{public}d, param=%{public}d, value=%{public}" PRId64,
                         simId_, static_cast<int32_t>(param_), value_);
    
    SetParseOK(true);
}

bool SetTrafficPlanInfoContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount != PARAM_TRIPLE_OPTIONS) {
        return false;
    }
    return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
           NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_number &&
           NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_2]) == napi_number;
}

int32_t SetTrafficPlanInfoContext::GetSimId() const
{
    return simId_;
}

TrafficPlanParam SetTrafficPlanInfoContext::GetParam() const
{
    return param_;
}

int64_t SetTrafficPlanInfoContext::GetValue() const
{
    return value_;
}

} // namespace NetManagerStandard
} // namespace OHOS

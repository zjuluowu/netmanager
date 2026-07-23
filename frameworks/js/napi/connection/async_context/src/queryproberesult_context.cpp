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
 
#include "queryproberesult_context.h"

#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {

constexpr int32_t MAX_PING_DURATION = 1000;

QueryProbeResultContext::QueryProbeResultContext(napi_env env,
    std::shared_ptr<EventManager> &manager) : BaseContext(env, manager)
{
    dest_ = "";
    duration_ = 0;
    probeResultInfo_ = {0};
}

void QueryProbeResultContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }
    dest_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_0]);
    duration_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_1]);
    if (dest_ == "" || duration_ <= 0 || duration_ > MAX_PING_DURATION) {
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return;
    }
    SetParseOK(true);
}

bool QueryProbeResultContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_string &&
            NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_number;
    }
    return false;
}

 
} // namespace NetManagerStandard
} // namespace OHOS

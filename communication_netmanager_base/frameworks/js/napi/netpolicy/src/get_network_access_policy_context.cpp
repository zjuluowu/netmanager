/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "get_network_access_policy_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS::NetManagerStandard {

GetNetworkAccessPolicyContext::GetNetworkAccessPolicyContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}

void GetNetworkAccessPolicyContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetNeedThrowException(true);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }

    if (paramsCount == PARAM_JUST_OPTIONS) {
        policy_parmeter_.uid = NapiUtils::GetUint32FromValue(GetEnv(), params[ARG_INDEX_0]);
        policy_parmeter_.flag = true;
        SetParseOK(true);
        return;
    }

    policy_parmeter_.flag = false;
    SetParseOK(true);
}

bool GetNetworkAccessPolicyContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_NONE) {
        return true;
    }

    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number;
    }
    return false;
}

} // namespace OHOS::NetManagerStandard

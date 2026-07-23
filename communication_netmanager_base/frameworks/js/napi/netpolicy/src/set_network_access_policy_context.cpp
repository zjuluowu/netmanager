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

#include "set_network_access_policy_context.h"

#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
SetNetworkAccessPolicyContext::SetNetworkAccessPolicyContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager)
{
}

void SetNetworkAccessPolicyContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetNeedThrowException(true);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }

    uid_ = NapiUtils::GetUint32FromValue(GetEnv(), params[ARG_INDEX_0]);

    if (NapiUtils::HasNamedProperty(GetEnv(), params[ARG_INDEX_1], "allowWiFi")) {
        policy_.wifiAllow = NapiUtils::GetBooleanProperty(GetEnv(), params[ARG_INDEX_1], "allowWiFi");
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), params[ARG_INDEX_1], "allowCellular")) {
        policy_.cellularAllow = NapiUtils::GetBooleanProperty(GetEnv(), params[ARG_INDEX_1], "allowCellular");
    }

    if (paramsCount == PARAM_TRIPLE_OPTIONS) {
        isReconfirmFlag_ =  NapiUtils::GetBooleanValue(GetEnv(), params[ARG_INDEX_2]);
    }

    SetParseOK(true);
}

bool SetNetworkAccessPolicyContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        if (NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number) {
        }

        if (NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_object) {
        }

        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_object;
    }

    if (paramsCount == PARAM_TRIPLE_OPTIONS) {
        if (NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number) {
        }

        if (NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_object) {
        }

        if (NapiUtils::GetValueType(env, params[ARG_INDEX_2]) == napi_boolean) {
        }

        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_object &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_2]) == napi_boolean;
    }

    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS

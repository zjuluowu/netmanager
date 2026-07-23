/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "set_device_idle_allow_list_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "net_policy_constants.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
SetDeviceIdleTrustlistContext::SetDeviceIdleTrustlistContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager)
{
}

void SetDeviceIdleTrustlistContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("Check params failed");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    uint32_t arrayLength = NapiUtils::GetArrayLength(GetEnv(), params[ARG_INDEX_0]);
    arrayLength = arrayLength > ARRAY_LIMIT ? ARRAY_LIMIT : arrayLength;
    napi_value elementValue = nullptr;
    for (uint32_t i = 0; i < arrayLength; i++) {
        elementValue = NapiUtils::GetArrayElement(GetEnv(), params[ARG_INDEX_0], i);
        auto uid = NapiUtils::GetInt32FromValue(GetEnv(), elementValue);
        if (uid > 0) {
            uids_.push_back(static_cast<uint32_t>(uid));
        } else {
            NETMANAGER_BASE_LOGE("uid : %{public}d Setting error !!!", uid);
        }
    }

    isAllow_ = NapiUtils::GetBooleanValue(GetEnv(), params[ARG_INDEX_1]);
    if (paramsCount == PARAM_DOUBLE_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_2]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool SetDeviceIdleTrustlistContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_object
               && NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_boolean;
    }

    if (paramsCount == PARAM_DOUBLE_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_boolean &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_2]) == napi_function;
    }
    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS

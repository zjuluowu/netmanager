/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "registernetsupplier_context.h"

#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {

bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_TRIPLE_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_string &&
               NapiUtils::GetArrayLength(env, params[ARG_INDEX_2]) != 0;
    }

    if (paramsCount == PARAM_TRIPLE_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_string &&
               NapiUtils::GetArrayLength(env, params[ARG_INDEX_2]) != 0 &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_3]) == napi_function;
    }
    return false;
}
} // namespace

RegisterNetSupplierContext::RegisterNetSupplierContext(
    napi_env env, std::shared_ptr<EventManager>& manager) : BaseContext(env, manager) {}

void RegisterNetSupplierContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetNeedThrowException(true);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }

    bearerType_ = static_cast<NetBearType>(NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]));
    ident_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_1]);
    uint32_t arrLenth = NapiUtils::GetArrayLength(GetEnv(), params[ARG_INDEX_2]);
    for (uint32_t i = 0; i < arrLenth; i++) {
        napi_value element = NapiUtils::GetArrayElement(GetEnv(), params[ARG_INDEX_2], i);
        netCaps_.emplace(static_cast<NetCap>(NapiUtils::GetUint32FromValue(GetEnv(), element)));
    }
    if (paramsCount == PARAM_TRIPLE_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_3]) == napi_ok);
        return;
    }

    SetParseOK(true);
}
} // namespace NetManagerStandard
} // namespace OHOS

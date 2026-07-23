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

#include "addnetworkroute_context.h"

#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {

bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_object;
    }

    if (paramsCount == PARAM_DOUBLE_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_object &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_2]) == napi_function;
    }
    return false;
}
} // namespace

AddNetworkRouteContext::AddNetworkRouteContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}

void AddNetworkRouteContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetNeedThrowException(true);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }

    netId_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]);
    route_.iface_ = NapiUtils::GetStringPropertyUtf8(GetEnv(), params[1], "interface");
    napi_value destination = NapiUtils::GetNamedProperty(GetEnv(), params[1], "destination");
    napi_value destAddress = NapiUtils::GetNamedProperty(GetEnv(), destination, "address");
    route_.destination_.address_ = NapiUtils::GetStringPropertyUtf8(GetEnv(), destAddress, "address");
    route_.destination_.family_ = NapiUtils::GetInt32Property(GetEnv(), destAddress, "family");
    route_.destination_.port_ = NapiUtils::GetInt32Property(GetEnv(), destAddress, "port");
    route_.destination_.prefixlen_ = NapiUtils::GetInt32Property(GetEnv(), destination, "prefixLength");
    napi_value gateway = NapiUtils::GetNamedProperty(GetEnv(), params[1], "gateway");
    route_.gateway_.address_ = NapiUtils::GetStringPropertyUtf8(GetEnv(), gateway, "address");
    route_.gateway_.family_ = NapiUtils::GetInt32Property(GetEnv(), gateway, "family");
    route_.gateway_.port_ = NapiUtils::GetInt32Property(GetEnv(), gateway, "port");
    route_.hasGateway_ = NapiUtils::GetBooleanProperty(GetEnv(), params[1], "hasGateway");
    route_.isDefaultRoute_ = NapiUtils::GetBooleanProperty(GetEnv(), params[1], "isDefaultRoute");
    
    if (paramsCount == PARAM_DOUBLE_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_2]) == napi_ok);
        return;
    }

    SetParseOK(true);
}
} // namespace NetManagerStandard
} // namespace OHOS

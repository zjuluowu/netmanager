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
 
#include "netextattribute_context.h"

#include "netmanager_base_permission.h"
#include "net_manager_constants.h"
#include "napi_constant.h"
#include "constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"
 
namespace OHOS {
namespace NetManagerStandard {

static constexpr const char *SET_NET_EXT_ATTRIBUTE = "ohos.permission.SET_NET_EXT_ATTRIBUTE";
static constexpr const char *GET_NETWORK_INFO = "ohos.permission.GET_NETWORK_INFO";

SetNetExtAttributeContext::SetNetExtAttributeContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}
 
bool SetNetExtAttributeContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        if (NapiUtils::GetValueType(env, params[ARG_INDEX_0]) != napi_object) {
            return false;
        }
        auto value = NapiUtils::GetNamedProperty(env, params[ARG_INDEX_0], KEY_NET_ID);
        return NapiUtils::GetValueType(env, value) == napi_number &&
               NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_string;
    }

    return false;
}
 
void SetNetExtAttributeContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!NetManagerPermission::CheckPermission(SET_NET_EXT_ATTRIBUTE)) {
        SetErrorCode(NETMANAGER_ERR_PERMISSION_DENIED);
        return;
    }
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return;
    }

    int32_t netId_ = NapiUtils::GetInt32Property(GetEnv(), params[ARG_INDEX_0], KEY_NET_ID);
    netHandle_.SetNetId(netId_);
    netExtAttribute_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_1]);
    SetParseOK(true);
}
 
GetNetExtAttributeContext::GetNetExtAttributeContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}
 
void GetNetExtAttributeContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!NetManagerPermission::CheckPermission(GET_NETWORK_INFO)) {
        SetErrorCode(NETMANAGER_ERR_PERMISSION_DENIED);
        return;
    }
    if (paramsCount != PARAM_JUST_OPTIONS || NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) != napi_object) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return;
    }
    auto value = NapiUtils::GetNamedProperty(GetEnv(), params[ARG_INDEX_0], KEY_NET_ID);
    if (NapiUtils::GetValueType(GetEnv(), value) != napi_number) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return;
    }
    int32_t netId_ = NapiUtils::GetInt32Property(GetEnv(), params[ARG_INDEX_0], KEY_NET_ID);
    netHandle_.SetNetId(netId_);
    SetParseOK(true);
}
} // namespace NetManagerStandard
} // namespace OHOS

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
 
#include "pacurl_context.h"
 
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"
#include "net_all_capabilities.h"

namespace OHOS {
namespace NetManagerStandard {
ProxyModeContext::ProxyModeContext(napi_env env, std::shared_ptr<EventManager> &manager) : BaseContext(env, manager)
{
    mode_ = PROXY_MODE_OFF;
}

void ProxyModeContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }
    int32_t mode = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]);
    switch (mode) {
        case PROXY_MODE_OFF:
            mode_ = PROXY_MODE_OFF;
            break;
        case PROXY_MODE_AUTO:
            mode_ = PROXY_MODE_AUTO;
            break;
        default:
            SetParseOK(false);
            SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
            return;
    }
    SetParseOK(true);
}

bool ProxyModeContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_number;
    }
    return false;
}

FindPacFileUrlContext::FindPacFileUrlContext(napi_env env, std::shared_ptr<EventManager> &manager)
    : BaseContext(env, manager)
{
}

void FindPacFileUrlContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }
    url_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_0]);
    SetParseOK(true);
}

bool FindPacFileUrlContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_string;
    }
    return false;
}

GetPacFileUrlContext::GetPacFileUrlContext(napi_env env, std::shared_ptr<EventManager> &manager)
    : BaseContext(env, manager)
{
}

void GetPacFileUrlContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (paramsCount != PARAM_NONE) {
        SetParseOK(false);
        return;
    }
    SetParseOK(true);
}

SetPacUrlContext::SetPacUrlContext(napi_env env, std::shared_ptr<EventManager>& manager) : BaseContext(env, manager) {}

 
bool SetPacUrlContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_string;
    }
    return false;
}
 
void SetPacUrlContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }
    pacUrl_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_0]);
    SetParseOK(true);
}

SetPacFileUrlContext::SetPacFileUrlContext(napi_env env, std::shared_ptr<EventManager> &manager)
    : BaseContext(env, manager)
{
}

bool SetPacFileUrlContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_string;
    }
    return false;
}

void SetPacFileUrlContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }
    pacUrl_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_0]);
    SetParseOK(true);
}

 
GetPacUrlContext::GetPacUrlContext(napi_env env, std::shared_ptr<EventManager>& manager) : BaseContext(env, manager) {}
 
void GetPacUrlContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (paramsCount != PARAM_NONE) {
        SetParseOK(false);
        return;
    }
    SetParseOK(true);
}
 
} // namespace NetManagerStandard
} // namespace OHOS
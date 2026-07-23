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

#include "getdns_context.h"

#include "icu_helper.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "net_all_capabilities.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {

static constexpr const int MAX_HOST_LEN = 255;

GetDnsContext::GetDnsContext(napi_env env, std::shared_ptr<EventManager> &manager) : BaseContext(env, manager)
{
    conversionProcess_ = ConversionProcess::NO_CONFIGURATION;
    host_ = "";
}

void GetDnsContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("params is invalid");
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return;
    }
    if (paramsCount == PARAM_JUST_OPTIONS) {
        if (!ParseHost(params[ARG_INDEX_0])) {
            return;
        }
        conversionProcess_ = ConversionProcess::NO_CONFIGURATION;
        SetParseOK(true);
        return;
    }
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        if (!ParseHost(params[ARG_INDEX_0])) {
            return;
        }
        auto type = NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]);
        if (type == napi_undefined || type == napi_null) {
            conversionProcess_ = ConversionProcess::NO_CONFIGURATION;
            SetParseOK(true);
            return;
        }
        int32_t process = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_1]);
        if (process < static_cast<int32_t>(ConversionProcess::NO_CONFIGURATION) ||
            process > static_cast<int32_t>(ConversionProcess::USE_STD3_ASCII_RULES)) {
            SetParseOK(false);
            SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
            return;
        }
        conversionProcess_ = static_cast<ConversionProcess>(process);
        SetParseOK(true);
        return;
    }

    NETMANAGER_BASE_LOGE("check params type failed");
    SetParseOK(false);
    SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
}

bool GetDnsContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (params == nullptr) {
        return false;
    }
    auto env = GetEnv();
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_string;
    } else if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_string &&
               (NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_number ||
                NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_undefined ||
                NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_null);
    }
    return false;
}

bool GetDnsContext::ParseHost(napi_value jsHost)
{
    char str[MAX_HOST_LEN + 1] = {0};
    size_t length = 0;
    napi_status status = napi_get_value_string_utf8(GetEnv(), jsHost, str, MAX_HOST_LEN + 1, &length);
    if (status != napi_ok || length > MAX_HOST_LEN) {
        NETMANAGER_BASE_LOGE("host string too long or get length failed");
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return false;
    }

    host_.assign(str, length);
    return true;
}

} // namespace NetManagerStandard
} // namespace OHOS

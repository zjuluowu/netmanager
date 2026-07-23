/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "deletecustomdnsrule_context.h"

#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
DeleteCustomDNSRuleContext::DeleteCustomDNSRuleContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}

bool DeleteCustomDNSRuleContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_string;
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        if (NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_string &&
            NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_function) {
            return true;
        }
        if (NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_function) {
            SetCallback(params[paramsCount - 1]);
        }
    }
    return false;
}

void DeleteCustomDNSRuleContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetNeedThrowException(true);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }

    host_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_0]);

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_1]) == napi_ok);
        return;
    }

    SetParseOK(true);
}
} // namespace NetManagerStandard
} // namespace OHOS

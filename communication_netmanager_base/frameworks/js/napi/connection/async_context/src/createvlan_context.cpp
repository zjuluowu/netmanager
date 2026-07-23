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

#include "createvlan_context.h"
#include "napi_constant.h"
#include "netmanager_base_permission.h"

namespace OHOS {
namespace NetManagerStandard {

CreateVlanContext::CreateVlanContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}

void CreateVlanContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (params == nullptr) {
        return;
    }
    if (paramsCount != PARAM_DOUBLE_OPTIONS ||
        NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) != napi_string ||
        NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) != napi_number) {
        SetNeedThrowException(true);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }
    ifName_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_0]);
    vlanId_ = NapiUtils::GetUint32FromValue(GetEnv(), params[ARG_INDEX_1]);
    SetParseOK(true);
}
} // namespace NetManagerStandard
} // namespace OHOS
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

#include "getsystemnetportstates_context.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {

GetSystemNetPortStatesContext::GetSystemNetPortStatesContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}

void GetSystemNetPortStatesContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        return;
    }
    SetParseOK(true);
}
bool GetSystemNetPortStatesContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == NetManagerStandard::PARAM_NONE) {
        return true;
    }
    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS
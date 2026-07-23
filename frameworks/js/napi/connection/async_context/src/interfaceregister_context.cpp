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

#include "interfaceregister_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"
#include "net_interface_callback_observer.h"

namespace OHOS::NetManagerStandard {
IfaceRegisterContext::IfaceRegisterContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager)
{
    if (manager) {
        auto iface = static_cast<NetInterface *>(manager->GetData());
        if (iface) {
            callback_ = iface->GetObserver();
            iface_ = *iface;
        }
    }
}

wptr<NetInterfaceCallbackObserver> IfaceRegisterContext::GetNetInterfaceCallback()
{
    return callback_;
}

NetInterface IfaceRegisterContext::GetIface()
{
    return iface_;
}

void IfaceRegisterContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        return;
    }

    if (paramsCount == PARAM_JUST_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_0]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool IfaceRegisterContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_NONE) {
        return true;
    }

    if (paramsCount == PARAM_JUST_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_function;
    }
    return false;
}
} // namespace OHOS::NetManagerStandard

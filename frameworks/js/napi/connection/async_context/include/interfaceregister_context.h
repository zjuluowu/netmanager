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

#ifndef NETMANAGER_BASE_INTERFACE_REGISTER_CONTEXT_H
#define NETMANAGER_BASE_INTERFACE_REGISTER_CONTEXT_H

#include <cstddef>

#include <napi/native_api.h>

#include "base_context.h"
#include "netinterface.h"
#include "i_net_interface_callback.h"

namespace OHOS::NetManagerStandard {
class IfaceRegisterContext final : public BaseContext {
public:
    IfaceRegisterContext() = delete;

    explicit IfaceRegisterContext(napi_env env, std::shared_ptr<EventManager>& manager);

    void ParseParams(napi_value *params, size_t paramsCount);

    wptr<NetInterfaceCallbackObserver> GetNetInterfaceCallback();

    NetInterface GetIface();

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
    wptr<NetInterfaceCallbackObserver> callback_;
    NetInterface iface_;
};

typedef IfaceRegisterContext IfaceUnregisterContext;
} // namespace OHOS::NetManagerStandard

#endif /* NETMANAGER_BASE_INTERFACE_REGISTER_CONTEXT_H */

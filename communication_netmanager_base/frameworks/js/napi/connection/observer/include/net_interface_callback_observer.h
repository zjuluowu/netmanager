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

#ifndef NETMANAGER_BASE_NET_INTERFACE_CALLBACK_OBSERVER_H
#define NETMANAGER_BASE_NET_INTERFACE_CALLBACK_OBSERVER_H

#include "event_manager.h"
#include "napi_utils.h"
#include "net_interface_callback_stub.h"

namespace OHOS::NetManagerStandard {
class NetInterfaceCallbackObserver : public NetInterfaceStateCallbackStub {
public:
    int32_t OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName, int32_t flags,
                                      int32_t scope) override;
    int32_t OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName, int32_t flags,
                                      int32_t scope) override;
    int32_t OnInterfaceAdded(const std::string &ifName) override;
    int32_t OnInterfaceRemoved(const std::string &ifName) override;
    int32_t OnInterfaceChanged(const std::string &ifName, bool up) override;
    int32_t OnInterfaceLinkStateChanged(const std::string &ifName, bool up) override;
    int32_t OnRouteChanged(bool updated, const std::string &route, const std::string &gateway,
                           const std::string &ifName) override;

private:
    static napi_value CreateInterfaceAddressUpdateParam(napi_env env, const std::string &addr,
        const std::string &ifName, int32_t flags, int32_t scope);
    static napi_value CreateInterfaceUpdateParam(napi_env env,
        const std::string &ifName);
    static napi_value CreateInterfaceChangedParam(napi_env env,
        const std::string &ifName, bool up);
    static napi_value CreateRouteChangeParam(napi_env env, bool updated,
        const std::string &route, const std::string &gateway, const std::string &ifName);
};
} // namespace OHOS::NetManagerStandard

#endif /* NETMANAGER_BASE_NET_INTERFACE_CALLBACK_OBSERVER_H */

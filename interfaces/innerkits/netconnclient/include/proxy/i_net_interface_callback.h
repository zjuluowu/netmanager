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

#ifndef I_NET_INTERFACE_CALLBACK_H
#define I_NET_INTERFACE_CALLBACK_H

#include "conn_ipc_interface_code.h"
#include "iremote_broker.h"

namespace OHOS {
namespace NetManagerStandard {
class INetInterfaceStateCallback : public IRemoteBroker {
public:
    virtual ~INetInterfaceStateCallback() = default;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetInterfaceStateCallback");

public:
    virtual int32_t OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName, int32_t flags,
                                              int32_t scope) = 0;
    virtual int32_t OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName, int32_t flags,
                                              int32_t scope) = 0;
    virtual int32_t OnInterfaceAdded(const std::string &ifName) = 0;
    virtual int32_t OnInterfaceRemoved(const std::string &ifName) = 0;
    virtual int32_t OnInterfaceChanged(const std::string &ifName, bool up) = 0;
    virtual int32_t OnInterfaceLinkStateChanged(const std::string &ifName, bool up) = 0;
    virtual int32_t OnRouteChanged(bool updated, const std::string &route, const std::string &gateway,
                                   const std::string &ifName)
    {
        (void)updated;
        (void)route;
        (void)gateway;
        (void)ifName;
        return 0;
    };
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_NET_INTERFACE_CALLBACK_H

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

#ifndef NET_INTERFACE_CALLBACK_STUB_H
#define NET_INTERFACE_CALLBACK_STUB_H

#include "i_net_interface_callback.h"

#include <map>

#include "iremote_stub.h"

namespace OHOS {
namespace NetManagerStandard {
class NetInterfaceStateCallbackStub : public IRemoteStub<INetInterfaceStateCallback> {
public:
    NetInterfaceStateCallbackStub();
    virtual ~NetInterfaceStateCallbackStub() = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

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
    using NetInterfaceStateCallbackFunc = int32_t (NetInterfaceStateCallbackStub::*)(MessageParcel &, MessageParcel &);

private:
    int32_t CmdInterfaceAddressUpdated(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInterfaceAddressRemoved(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInterfaceAdded(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInterfaceRemoved(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInterfaceChanged(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInterfaceLinkStateChanged(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRouteChanged(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, NetInterfaceStateCallbackFunc> memberFuncMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_INTERFACE_CALLBACK_STUB_H

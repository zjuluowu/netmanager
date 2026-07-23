/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef NETFIREWALL_CALLBACK_STUB_H
#define NETFIREWALL_CALLBACK_STUB_H

#include <map>

#include "i_netfirewall_callback.h"
#include "iremote_stub.h"

namespace OHOS {
namespace NetsysNative {
class NetFirewallCallbackStub : public IRemoteStub<INetFirewallCallback> {
public:
    NetFirewallCallbackStub();
    virtual ~NetFirewallCallbackStub() = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    using FirewallCallbackFunc = int32_t (NetFirewallCallbackStub::*)(MessageParcel &, MessageParcel &);
    int32_t CmdOnIntercept(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, FirewallCallbackFunc> memberFuncMap_;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // NETFIREWALL_CALLBACK_STUB_H

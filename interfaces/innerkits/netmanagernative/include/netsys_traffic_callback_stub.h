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

#ifndef NETSYS_TRAFFIC_CALLBACK_STUB_H
#define NETSYS_TRAFFIC_CALLBACK_STUB_H

#include <map>

#include "i_netsys_traffic_callback.h"
#include "iremote_stub.h"
#include "netsys_net_dns_result_data.h"

namespace OHOS {
namespace NetsysNative {
class NetsysTrafficCallbackStub : public IRemoteStub<INetsysTrafficCallback> {
public:
    NetsysTrafficCallbackStub();
    virtual ~NetsysTrafficCallbackStub() = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    int32_t OnExceedTrafficLimits(int8_t &flag) override;

private:
    using NetsysTrafficCallbackFunc = int32_t (NetsysTrafficCallbackStub::*)(MessageParcel &, MessageParcel &);
    int32_t CmdOnExceedTrafficLimits(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, NetsysTrafficCallbackFunc> memberFuncMap_;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // NETSYS_TRAFFIC_CALLBACK_STUB_H

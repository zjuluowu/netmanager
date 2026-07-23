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
#ifndef  NETSYS_TRAFFIC_CALLBACK_PROXY_H
#define  NETSYS_TRAFFIC_CALLBACK_PROXY_H
#include "iremote_proxy.h"
#include "i_netsys_traffic_callback.h"

namespace OHOS {
namespace NetsysNative {
class NetsysTrafficCallbackProxy : public IRemoteProxy<INetsysTrafficCallback> {
public:
    explicit NetsysTrafficCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetsysTrafficCallbackProxy() = default;

    int32_t OnExceedTrafficLimits(int8_t &flag) override;

private:
    static inline BrokerDelegator<NetsysTrafficCallbackProxy> delegator_;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // NETSYS_TRAFFIC_CALLBACK_PROXY_H

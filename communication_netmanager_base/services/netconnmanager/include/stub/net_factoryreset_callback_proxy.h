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

#ifndef NET_FACTORYRESET_CALLBACK_PROXY_H
#define NET_FACTORYRESET_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "i_net_factoryreset_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class NetFactoryResetCallbackProxy : public IRemoteProxy<INetFactoryResetCallback> {
public:
    explicit NetFactoryResetCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetFactoryResetCallbackProxy();

public:
    int32_t OnNetFactoryReset() override;

private:
    bool WriteInterfaceToken(MessageParcel &data);

private:
    static inline BrokerDelegator<NetFactoryResetCallbackProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_DETECTION_CALLBACK_PROXY_H

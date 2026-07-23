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

#ifndef REFRESH_HTTP_PROXY_CALLBACK_PROXY_H
#define REFRESH_HTTP_PROXY_CALLBACK_PROXY_H

#include "iremote_proxy.h"
#include "i_refresh_http_proxy_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class RefreshHttpProxyCallbackProxy : public IRemoteProxy<IRefreshHttpProxyCallback> {
public:
    explicit RefreshHttpProxyCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~RefreshHttpProxyCallbackProxy() = default;

    int32_t OnRefreshHttpProxyResult(int32_t result, const HttpProxy &httpProxy) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);
    static inline BrokerDelegator<RefreshHttpProxyCallbackProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // REFRESH_HTTP_PROXY_CALLBACK_PROXY_H

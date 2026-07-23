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

#ifndef REFRESH_HTTP_PROXY_CALLBACK_STUB_H
#define REFRESH_HTTP_PROXY_CALLBACK_STUB_H

#include <map>
#include <functional>
#include "iremote_stub.h"
#include "i_refresh_http_proxy_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class RefreshHttpProxyCallbackStub : public IRemoteStub<IRefreshHttpProxyCallback> {
public:
    RefreshHttpProxyCallbackStub();
    virtual ~RefreshHttpProxyCallbackStub() = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                            MessageOption &option) override;

    int32_t OnRefreshHttpProxyResult(int32_t result, const HttpProxy &httpProxy) override;

    void SetRefreshCallback(std::function<void(int32_t, const HttpProxy &)> callback);

private:
    using RefreshCallbackFunc = int32_t (RefreshHttpProxyCallbackStub::*)(MessageParcel &, MessageParcel &);
    int32_t OnRefreshHttpProxyResultInner(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, RefreshCallbackFunc> memberFuncMap_;
    std::function<void(int32_t, const HttpProxy &)> refreshCallback_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // REFRESH_HTTP_PROXY_CALLBACK_STUB_H

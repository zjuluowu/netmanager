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

#ifndef I_REFRESH_HTTP_PROXY_CALLBACK_H
#define I_REFRESH_HTTP_PROXY_CALLBACK_H

#include "conn_ipc_interface_code.h"
#include "iremote_broker.h"
#include "http_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
class IRefreshHttpProxyCallback : public IRemoteBroker {
public:
    virtual ~IRefreshHttpProxyCallback() = default;
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.IRefreshHttpProxyCallback");
    virtual int32_t OnRefreshHttpProxyResult(int32_t result, const HttpProxy &httpProxy) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_REFRESH_HTTP_PROXY_CALLBACK_H

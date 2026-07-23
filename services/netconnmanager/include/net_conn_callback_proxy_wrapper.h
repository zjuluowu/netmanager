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

#ifndef NET_CONN_CALLBACK_PROXY_WRAPPER_H
#define NET_CONN_CALLBACK_PROXY_WRAPPER_H

#include "iremote_proxy.h"
#include "i_net_conn_callback.h"
#include "net_activate.h"

namespace OHOS {
namespace NetManagerStandard {
class NetConnCallbackProxyWrapper : public INetConnCallback  {
public:
    NetConnCallbackProxyWrapper(const sptr<INetConnCallback> &callback);
    ~NetConnCallbackProxyWrapper();

public:
    int32_t NetAvailable(sptr<NetHandle> &netHandle) override;
    int32_t NetCapabilitiesChange(sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap) override;
    int32_t NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info) override;
    int32_t NetLost(sptr<NetHandle> &netHandle) override;
    int32_t NetUnavailable() override;
    int32_t NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked) override;
    sptr<IRemoteObject> AsObject() override;
    void SetNetActivate(std::shared_ptr<NetActivate> netActivate);

private:
    bool IsAllowCallback(CallbackType callbackType);

private:
    sptr<INetConnCallback> netConnCallback_ = nullptr;
    std::weak_ptr<NetActivate> netActivate_;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CONN_CALLBACK_PROXY_H

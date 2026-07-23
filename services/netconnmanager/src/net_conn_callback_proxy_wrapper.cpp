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

#include "net_conn_callback_proxy_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetConnCallbackProxyWrapper::NetConnCallbackProxyWrapper(const sptr<INetConnCallback> &callback)
    : netConnCallback_(callback) {}

NetConnCallbackProxyWrapper::~NetConnCallbackProxyWrapper() {}

int32_t NetConnCallbackProxyWrapper::NetAvailable(sptr<NetHandle> &netHandle)
{
    if (IsAllowCallback(CALL_TYPE_AVAILABLE)) {
        return netConnCallback_->NetAvailable(netHandle);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackProxyWrapper::NetCapabilitiesChange(
    sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap)
{
    if (IsAllowCallback(CALL_TYPE_UPDATE_CAP)) {
        return netConnCallback_->NetCapabilitiesChange(netHandle, netAllCap);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackProxyWrapper::NetConnectionPropertiesChange
    (sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info)
{
    if (IsAllowCallback(CALL_TYPE_UPDATE_LINK)) {
        return netConnCallback_->NetConnectionPropertiesChange(netHandle, info);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackProxyWrapper::NetLost(sptr<NetHandle> &netHandle)
{
    if (IsAllowCallback(CALL_TYPE_LOST)) {
        return netConnCallback_->NetLost(netHandle);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackProxyWrapper::NetUnavailable()
{
    if (IsAllowCallback(CALL_TYPE_UNAVAILABLE)) {
        return netConnCallback_->NetUnavailable();
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackProxyWrapper::NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
{
    if (IsAllowCallback(CALL_TYPE_BLOCK_STATUS)) {
        return netConnCallback_->NetBlockStatusChange(netHandle, blocked);
    }
    return NETMANAGER_SUCCESS;
}

sptr<IRemoteObject> NetConnCallbackProxyWrapper::AsObject()
{
    if (netConnCallback_ == nullptr) {
        return nullptr;
    }
    return netConnCallback_->AsObject();
}

void NetConnCallbackProxyWrapper::SetNetActivate(std::shared_ptr<NetActivate> netActivate)
{
    netActivate_ = netActivate;
}

bool NetConnCallbackProxyWrapper::IsAllowCallback(CallbackType callback)
{
    if (netConnCallback_ == nullptr) {
        return false;
    }
    auto netActivate = netActivate_.lock();
    if (netActivate) {
        bool isAllow = netActivate->IsAllowCallback(callback);
        if (!isAllow) {
            return false;
        }
    }
    return true;
}

} // namespace NetManagerStandard
} // namespace OHOS

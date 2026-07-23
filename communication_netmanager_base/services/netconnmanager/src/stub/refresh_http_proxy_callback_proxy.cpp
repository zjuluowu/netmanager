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

#include "refresh_http_proxy_callback_proxy.h"
#include "net_conn_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
RefreshHttpProxyCallbackProxy::RefreshHttpProxyCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IRefreshHttpProxyCallback>(impl)
{}

bool RefreshHttpProxyCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(RefreshHttpProxyCallbackProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t RefreshHttpProxyCallbackProxy::OnRefreshHttpProxyResult(int32_t result, const HttpProxy &httpProxy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!httpProxy.Marshalling(data)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(RefreshHttpProxyCallbackInterfaceCode::ON_REFRESH_HTTP_PROXY_RESULT),
        data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_LOG_D("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return ret;
}
} // namespace NetManagerStandard
} // namespace OHOS

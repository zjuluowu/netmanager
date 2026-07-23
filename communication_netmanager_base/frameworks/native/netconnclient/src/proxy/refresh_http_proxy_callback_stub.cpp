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

#include "refresh_http_proxy_callback_stub.h"
#include "net_conn_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
RefreshHttpProxyCallbackStub::RefreshHttpProxyCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(RefreshHttpProxyCallbackInterfaceCode::ON_REFRESH_HTTP_PROXY_RESULT)] =
        &RefreshHttpProxyCallbackStub::OnRefreshHttpProxyResultInner;
}

int32_t RefreshHttpProxyCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                      MessageOption &option)
{
    std::u16string myDescriptor = RefreshHttpProxyCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        NETMGR_LOG_E("Descriptor checked failed");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t RefreshHttpProxyCallbackStub::OnRefreshHttpProxyResultInner(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_SUCCESS;
    // LCOV_EXCL_START
    if (!data.ReadInt32(result)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    HttpProxy httpProxy;
    if (!HttpProxy::Unmarshalling(data, httpProxy)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = OnRefreshHttpProxyResult(result, httpProxy);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    // LCOV_EXCL_END
    return NETMANAGER_SUCCESS;
}

int32_t RefreshHttpProxyCallbackStub::OnRefreshHttpProxyResult(int32_t result, const HttpProxy &httpProxy)
{
    if (refreshCallback_) {
        refreshCallback_(result, httpProxy);
    }
    return NETMANAGER_SUCCESS;
}

void RefreshHttpProxyCallbackStub::SetRefreshCallback(std::function<void(int32_t, const HttpProxy &)> callback)
{
    refreshCallback_ = std::move(callback);
}
} // namespace NetManagerStandard
} // namespace OHOS

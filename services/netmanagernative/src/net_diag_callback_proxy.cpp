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

#include "net_diag_callback_proxy.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_ipc_interface_code.h"

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace OHOS::NetManagerStandard;
} // namespace
NetDiagCallbackProxy::NetDiagCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<INetDiagCallback>(impl) {}

int32_t NetDiagCallbackProxy::OnNotifyPingResult(const NetDiagPingResult &pingResult)
{
    NETNATIVE_LOGI("Proxy OnNotifyPingResult");
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetDiagCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!pingResult.Marshalling(data)) {
        NETNATIVE_LOGE("NetDiagPingResult Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(NetDiagInterfaceCode::ON_NOTIFY_PING_RESULT), data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("proxy SendRequest failed, error: [%{public}d]", ret);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}
} // namespace NetsysNative
} // namespace OHOS

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

#include "net_factoryreset_callback_proxy.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetFactoryResetCallbackProxy::NetFactoryResetCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<INetFactoryResetCallback>(impl)
{}

NetFactoryResetCallbackProxy::~NetFactoryResetCallbackProxy() {}

int32_t NetFactoryResetCallbackProxy::OnNetFactoryReset()
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(static_cast<uint32_t>(FactoryResetCallbackInterfaceCode::NET_FACTORYRESET),
                                          data, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }

    return replyParcel.ReadInt32();
}

bool NetFactoryResetCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetFactoryResetCallbackProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

} // namespace NetManagerStandard
} // namespace OHOS

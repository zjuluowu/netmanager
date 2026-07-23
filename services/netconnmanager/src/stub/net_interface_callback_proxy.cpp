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

#include "net_interface_callback_proxy.h"

#include "net_conn_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetInterfaceStateCallbackProxy::NetInterfaceStateCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<INetInterfaceStateCallback>(impl)
{
}

bool NetInterfaceStateCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetInterfaceStateCallbackProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t NetInterfaceStateCallbackProxy::OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName,
                                                                  int32_t flags, int32_t scope)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteString(addr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteInt32(flags)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteInt32(scope)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDR_UPDATED),
        dataParcel, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetInterfaceStateCallbackProxy::OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName,
                                                                  int32_t flags, int32_t scope)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteString(addr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteInt32(flags)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteInt32(scope)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDR_REMOVED),
        dataParcel, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetInterfaceStateCallbackProxy::OnInterfaceAdded(const std::string &ifName)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDED),
                                          dataParcel, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetInterfaceStateCallbackProxy::OnInterfaceRemoved(const std::string &ifName)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_REMOVED),
                                          dataParcel, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetInterfaceStateCallbackProxy::OnInterfaceChanged(const std::string &ifName, bool up)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteBool(up)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_CHANGED),
                                          dataParcel, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetInterfaceStateCallbackProxy::OnInterfaceLinkStateChanged(const std::string &ifName, bool up)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteBool(up)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_LINK_STATE_CHANGED),
        dataParcel, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetInterfaceStateCallbackProxy::OnRouteChanged(bool updated, const std::string &route,
                                                       const std::string &gateway, const std::string &ifName)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!dataParcel.WriteBool(updated)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteString(route)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteString(gateway)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!dataParcel.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t retCode = remote->SendRequest(static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_ROUTE_CHANGED),
                                          dataParcel, replyParcel, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, retCode: [%{public}d]", retCode);
        return retCode;
    }
    return replyParcel.ReadInt32();
}
} // namespace NetManagerStandard
} // namespace OHOS

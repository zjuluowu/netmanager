/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "net_conn_service_proxy.h"

#include "net_conn_constants.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "ipc_skeleton.h"
#include "net_conn_service_pac_proxy_helper.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_IFACE_NUM = 16;
static constexpr uint32_t MAX_NET_CAP_NUM = 32;
static constexpr uint32_t MAX_IP_NEIGH_TABLE_SIZE = 1024;

NetConnServiceProxy::NetConnServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<INetConnService>(impl) {}

NetConnServiceProxy::~NetConnServiceProxy() {}

int32_t NetConnServiceProxy::SystemReady()
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SYSTEM_READY), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceProxy::SetInternetPermission(uint32_t uid, uint8_t allow)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    NETMGR_LOG_D("proxy SetInternetPermission [%{public}u %{public}hhu]", uid, allow);
    if (!data.WriteUint32(uid)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint8(allow)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERNET_PERMISSION),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::EnableVnicNetwork(const sptr<NetLinkInfo> &netLinkInfo, const std::set<int32_t> &uids)
{
    if (netLinkInfo == nullptr) {
        NETMGR_LOG_E("netLinkInfo is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(uids.size())) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    for (const auto &uid: uids) {
        if (!data.WriteInt32(uid)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
    }

    if (!netLinkInfo->Marshalling(data)) {
        NETMGR_LOG_E("proxy Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ENABLE_VNIC_NET_WORK), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::EnableDistributedClientNet(const std::string &virnicAddr,
    const std::string &virnicName, const std::string &iif)
{
    MessageParcel data;
    MessageParcel reply;

    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    // LCOV_EXCL_START
    if (!data.WriteString(virnicAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(virnicName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(iif)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP
    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ENABLE_DISTRIBUTE_CLIENT_NET), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    return ret;
}

int32_t NetConnServiceProxy::EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                                        const std::string &dstAddr, const std::string &gw)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    // LCOV_EXCL_START
    if (!data.WriteString(iif)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(devIface)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(dstAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(gw)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP

    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ENABLE_DISTRIBUTE_SERVER_NET), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::DisableDistributedNet(
    bool isServer, const std::string &virnicName, const std::string &dstAddr)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    // LCOV_EXCL_START
    if (!data.WriteBool(isServer)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(virnicName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(dstAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP
    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DISABLE_DISTRIBUTE_NET), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::DisableVnicNetwork()
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DISABLE_VNIC_NET_WORK), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                                 const std::set<NetCap> &netCaps, uint32_t &supplierId)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(bearerType))) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ident)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    uint32_t size = static_cast<uint32_t>(netCaps.size());
    if (!data.WriteUint32(size)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    for (auto netCap : netCaps) {
        if (!data.WriteUint32(static_cast<uint32_t>(netCap))) {
            return NETMANAGER_ERR_WRITE_DATA_FAIL;
        }
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REG_NET_SUPPLIER), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadUint32(supplierId)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::UnregisterNetSupplier(uint32_t supplierId)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    NETMGR_LOG_D("proxy supplierId[%{public}d]", supplierId);
    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREG_NETWORK), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::RegisterNetSupplierCallback(uint32_t supplierId,
                                                         const sptr<INetSupplierCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteUint32(supplierId);
    dataParcel.WriteRemoteObject(callback->AsObject());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_SUPPLIER_CALLBACK), dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    NETMGR_LOG_I("SendRequest retCode:[%{public}d]", retCode);
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::RegisterNetConnCallback(const sptr<INetConnCallback> callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_CONN_CALLBACK),
                                        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier,
                                                     const sptr<INetConnCallback> callback, const uint32_t &timeoutMS)
{
    if (netSpecifier == nullptr || callback == nullptr) {
        NETMGR_LOG_E("The parameter of netSpecifier or callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    netSpecifier->Marshalling(dataParcel);
    dataParcel.WriteUint32(timeoutMS);
    dataParcel.WriteRemoteObject(callback->AsObject());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_CONN_CALLBACK_BY_SPECIFIER),
        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::RequestNetConnection(const sptr<NetSpecifier> netSpecifier,
                                                  const sptr<INetConnCallback> callback, const uint32_t timeoutMS)
{
    if (netSpecifier == nullptr || callback == nullptr) {
        NETMGR_LOG_E("The parameter of netSpecifier or callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    netSpecifier->Marshalling(dataParcel);
    dataParcel.WriteUint32(timeoutMS);
    dataParcel.WriteRemoteObject(callback->AsObject());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REQUEST_NET_CONNECTION),
        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UnregisterNetConnCallback(const sptr<INetConnCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREGISTER_NET_CONN_CALLBACK),
                                        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UpdateNetCaps(const std::set<NetCap> &netCaps, const uint32_t supplierId)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t netCapsSize = static_cast<uint32_t>(netCaps.size());
    if (!data.WriteUint32(netCapsSize)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    for (const auto &cap : netCaps) {
        if (!data.WriteUint32(static_cast<uint32_t>(cap))) {
            return NETMANAGER_ERR_WRITE_DATA_FAIL;
        }
    }

    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t result = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UPDATE_NET_CAPS), data, reply);
    if (result != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("RemoteSendRequest failed");
        return result;
    }

    if (!reply.ReadInt32(result)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return result;
}

int32_t NetConnServiceProxy::UpdateNetStateForTest(const sptr<NetSpecifier> &netSpecifier, int32_t netState)
{
    NETMGR_LOG_I("Test NetConnServiceProxy::UpdateNetStateForTest(), begin");
    if (netSpecifier == nullptr) {
        NETMGR_LOG_E("The parameter of netSpecifier is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    netSpecifier->Marshalling(dataParcel);

    if (!dataParcel.WriteInt32(netState)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UPDATE_NET_STATE_FOR_TEST),
                                        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    NETMGR_LOG_I("NetConnServiceProxy::UpdateNetStateForTest(), SendRequest retCode:[%{public}d]", retCode);
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo)
{
    if (netSupplierInfo == nullptr) {
        NETMGR_LOG_E("netSupplierInfo is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    NETMGR_LOG_D("proxy supplierId[%{public}d]", supplierId);
    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    NETMGR_LOG_D("proxy supplierId[%{public}d] Marshalling success", supplierId);
    if (!netSupplierInfo->Marshalling(data)) {
        NETMGR_LOG_E("proxy Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    NETMGR_LOG_D("proxy Marshalling success");

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_NET_SUPPLIER_INFO),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    NETMGR_LOG_D("UpdateNetSupplierInfo out.");
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo)
{
    if (netLinkInfo == nullptr) {
        NETMGR_LOG_E("netLinkInfo is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!netLinkInfo->Marshalling(data)) {
        NETMGR_LOG_E("proxy Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_NET_LINK_INFO),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::RegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    int32_t error = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_DETECTION_RET_CALLBACK), dataParcel, replyParcel);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UnRegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    int32_t error = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREGISTER_NET_DETECTION_RET_CALLBACK),
        dataParcel, replyParcel);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::NetDetection(int32_t netId)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel replyParcel;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_NET_DETECTION),
                                      dataParcel, replyParcel);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::NetDetection(const std::string &rawUrl, PortalResponse &resp)
{
    MessageParcel dataParcel;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteString(rawUrl)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP
    MessageParcel replyParcel;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_NET_DETECTION_RESPONSE),
                                      dataParcel, replyParcel);
    // LCOV_EXCL_START
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    // LCOV_EXCL_STOP
    int32_t ret = NETMANAGER_SUCCESS;
    // LCOV_EXCL_START
    if (!replyParcel.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    // LCOV_EXCL_STOP
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    const void *rawData = replyParcel.ReadRawData(sizeof(PortalResponse));
    // LCOV_EXCL_START
    if (rawData == nullptr) {
        NETMGR_LOG_E("%{public}s, ReadRawData failed", __FUNCTION__);
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    // LCOV_EXCL_STOP
    resp = *static_cast<const PortalResponse *>(rawData);
    return ret;
}

int32_t NetConnServiceProxy::GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(bearerType)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_IFACE_NAMES),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = 0;
        if (!reply.ReadUint32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        for (uint32_t i = 0; i < size; ++i) {
            std::string value;
            if (!reply.ReadString(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            ifaceNames.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetIfaceNameByType(NetBearType bearerType, const std::string &ident,
                                                std::string &ifaceName)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (bearerType >= BEARER_DEFAULT) {
        return NETMANAGER_ERR_INTERNAL;
    }
    uint32_t netType = static_cast<NetBearType>(bearerType);
    if (!data.WriteUint32(netType)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ident)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_IFACENAME_BY_TYPE),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = 0;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadString(ifaceName)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetIfaceNameIdentMaps(NetBearType bearerType,
                                                   SafeMap<std::string, std::string> &ifaceNameIdentMaps)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (bearerType >= BEARER_DEFAULT) {
        return NETMANAGER_ERR_INTERNAL;
    }
    uint32_t netType = static_cast<NetBearType>(bearerType);
    if (!data.WriteUint32(netType)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    int32_t ret = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_GET_IFACENAME_IDENT_MAPS),
                                    data, reply);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    uint32_t size = 0;
    if (!reply.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
    for (uint32_t i = 0; i < size; ++i) {
        std::string key;
        std::string value;
        if (!reply.ReadString(key) || !reply.ReadString(value)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        ifaceNameIdentMaps.EnsureInsert(key, value);
    }
    return ret;
}

bool NetConnServiceProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetConnServiceProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t NetConnServiceProxy::GetDefaultNet(int32_t &netId)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel replyParcel;
    int32_t errCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GETDEFAULTNETWORK),
                                        dataParcel, replyParcel);
    if (errCode != NETMANAGER_SUCCESS) {
        return errCode;
    }
    NETMGR_LOG_D("SendRequest errcode:[%{public}d]", errCode);
    int32_t ret = 0;
    if (!replyParcel.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!replyParcel.ReadInt32(netId)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::HasDefaultNet(bool &flag)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_HASDEFAULTNET),
                                        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);

    int32_t ret = 0;
    if (!replyParcel.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!replyParcel.ReadBool(flag)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetSpecificNet(NetBearType bearerType, std::list<int32_t> &netIdList)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t type = static_cast<uint32_t>(bearerType);
    if (!data.WriteUint32(type)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_NET),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = 0;
        if (!reply.ReadUint32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        for (uint32_t i = 0; i < size; ++i) {
            uint32_t value;
            if (!reply.ReadUint32(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            netIdList.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetSpecificNetByIdent(
    NetBearType bearerType, const std::string &ident, std::list<int32_t> &netIdList)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("GetSpecificNetByIdent WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t type = static_cast<uint32_t>(bearerType);
    if (!data.WriteUint32(type)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(ident)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_NET_BY_IDENT),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = 0;
        if (!reply.ReadUint32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        for (uint32_t i = 0; i < size; ++i) {
            uint32_t value;
            if (!reply.ReadUint32(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            netIdList.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetAllNets(std::list<int32_t> &netIdList)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_ALL_NETS),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size;
        if (!reply.ReadUint32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        for (uint32_t i = 0; i < size; ++i) {
            uint32_t value;
            if (!reply.ReadUint32(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            netIdList.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetSpecificUidNet(int32_t uid, int32_t &netId)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(uid)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_UID_NET),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadInt32(netId)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetConnectionProperties(int32_t netId, NetLinkInfo &info)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_CONNECTION_PROPERTIES),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        sptr<NetLinkInfo> netLinkInfo_ptr = NetLinkInfo::Unmarshalling(reply);
        if (netLinkInfo_ptr != nullptr) {
            info = *netLinkInfo_ptr;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetNetCapabilities(int32_t netId, NetAllCapabilities &netAllCap)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_NET_CAPABILITIES),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return (ret == NETMANAGER_SUCCESS) ? GetNetCapData(reply, netAllCap) : ret;
}

int32_t NetConnServiceProxy::GetNetCapData(MessageParcel &reply, NetAllCapabilities &netAllCap)
{
    if (!reply.ReadUint32(netAllCap.linkUpBandwidthKbps_)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (!reply.ReadUint32(netAllCap.linkDownBandwidthKbps_)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    uint32_t size = 0;
    if (!reply.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
    uint32_t value = 0;
    for (uint32_t i = 0; i < size; ++i) {
        if (!reply.ReadUint32(value)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        if (value < NET_CAPABILITY_END) {
            netAllCap.netCaps_.insert(static_cast<NetCap>(value));
        }
    }
    if (!reply.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
    for (uint32_t i = 0; i < size; ++i) {
        if (!reply.ReadUint32(value)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        netAllCap.bearerTypes_.insert(static_cast<NetBearType>(value));
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceProxy::SetAirplaneMode(bool state)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteBool(state)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_AIRPLANE_MODE),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::IsDefaultNetMetered(bool &isMetered)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_IS_DEFAULT_NET_METERED),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadBool(isMetered)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::SetGlobalHttpProxy(const HttpProxy &httpProxy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!httpProxy.Marshalling(data)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_GLOBAL_HTTP_PROXY),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::GetGlobalHttpProxy(HttpProxy &httpProxy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(httpProxy.GetUserId())) {
        NETMGR_LOG_E("WriteUserId failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_GLOBAL_HTTP_PROXY),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        if (!HttpProxy::Unmarshalling(reply, httpProxy)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetDefaultHttpProxy(int32_t bindNetId, HttpProxy &httpProxy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(bindNetId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(httpProxy.GetUserId())) {
        NETMGR_LOG_E("WriteUserId failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_DEFAULT_HTTP_PROXY),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        if (!HttpProxy::Unmarshalling(reply, httpProxy)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::RefreshGlobalHttpProxy(const sptr<IRefreshHttpProxyCallback> &callback)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    // LCOV_EXCL_END
    if (callback == nullptr) {
        NETMGR_LOG_E("callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    // LCOV_EXCL_START
    if (!data.WriteRemoteObject(callback->AsObject())) {
        NETMGR_LOG_E("WriteRemoteObject failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REFRESH_GLOBAL_HTTP_PROXY),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    // LCOV_EXCL_END
    return ret;
}

int32_t NetConnServiceProxy::SetPacUrl(const std::string &pacUrl)
{
    auto fun = [&](uint32_t code, MessageParcel &data, MessageParcel &reply) {
        return RemoteSendRequest(code, data, reply);
    };
    return NetConnServicePacProxyHelper::GetInstance(fun)->SetPacUrl(pacUrl);
}

int32_t NetConnServiceProxy::GetPacUrl(std::string &pacUrl)
{
    auto fun = [&](uint32_t code, MessageParcel &data, MessageParcel &reply) {
        return RemoteSendRequest(code, data, reply);
    };
    return NetConnServicePacProxyHelper::GetInstance(fun)->GetPacUrl(pacUrl);
}

int32_t NetConnServiceProxy::QueryTraceRoute(const std::string &destination, int32_t maxJumpNumber,
    int32_t packetsType, std::string &traceRouteInfo, bool isCallerNative)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(destination)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(maxJumpNumber)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(packetsType)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    ConnInterfaceCode interfaceCode = isCallerNative ? ConnInterfaceCode::CMD_NM_QUERY_TRACEROUTE :
        ConnInterfaceCode::CMD_NM_QUERY_TRACEROUTE_JS;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(interfaceCode), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadString(traceRouteInfo)) {
            traceRouteInfo.clear();
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::SetProxyMode(const OHOS::NetManagerStandard::ProxyModeType mode)
{
    auto fun = [&](uint32_t code, MessageParcel &data, MessageParcel &reply) {
        return RemoteSendRequest(code, data, reply);
    };
    return NetConnServicePacProxyHelper::GetInstance(fun)->SetProxyMode(mode);
}

int32_t NetConnServiceProxy::GetProxyMode(OHOS::NetManagerStandard::ProxyModeType &mode)
{
    auto fun = [&](uint32_t code, MessageParcel &data, MessageParcel &reply) {
        return RemoteSendRequest(code, data, reply);
    };
    return NetConnServicePacProxyHelper::GetInstance(fun)->GetProxyMode(mode);
}

int32_t NetConnServiceProxy::SetPacFileUrl(const std::string &pacUrl)
{
    auto fun = [&](uint32_t code, MessageParcel &data, MessageParcel &reply) {
        return RemoteSendRequest(code, data, reply);
    };
    return NetConnServicePacProxyHelper::GetInstance(fun)->SetPacFileUrl(pacUrl);
}

int32_t NetConnServiceProxy::GetPacFileUrl(std::string &pacUrl)
{
    auto fun = [&](uint32_t code, MessageParcel &data, MessageParcel &reply) {
        return RemoteSendRequest(code, data, reply);
    };
    return NetConnServicePacProxyHelper::GetInstance(fun)->GetPacFileUrl(pacUrl);
}

int32_t NetConnServiceProxy::FindProxyForURL(const std::string &url, const std::string &host, std::string &proxy)
{
    auto fun = [&](uint32_t code, MessageParcel &data, MessageParcel &reply) {
        return RemoteSendRequest(code, data, reply);
    };
    return NetConnServicePacProxyHelper::GetInstance(fun)->FindProxyForURL(url, host, proxy);
}

int32_t NetConnServiceProxy::GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ident)) {
        NETMGR_LOG_E("Write string data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_NET_ID_BY_IDENTIFIER),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        NETMGR_LOG_E("Read return code failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = 0;
        if (!reply.ReadUint32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        int32_t value = 0;
        for (uint32_t i = 0; i < size; ++i) {
            if (!reply.ReadInt32(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            netIdList.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::SetAppNet(int32_t netId)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_APP_NET),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::RegisterNetInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_INTERFACE_CALLBACK),
        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UnregisterNetInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREGISTER_NET_INTERFACE_CALLBACK),
        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::GetNetInterfaceConfiguration(const std::string &iface, NetInterfaceConfiguration &config)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteString(iface)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_INTERFACE_CONFIGURATION),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!NetInterfaceConfiguration::Unmarshalling(reply, config)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::SetNetInterfaceIpAddress(const std::string &iface, const std::string &ipAddress)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteString(iface)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(ipAddress)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERFACE_IP_ADDRESS),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::SetInterfaceUp(const std::string &iface)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteString(iface)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERFACE_UP),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::SetInterfaceDown(const std::string &iface)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteString(iface)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERFACE_DOWN),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::RemoteSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageOption option;
    int32_t error = remote->SendRequest(code, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceProxy::AddNetworkRoute(int32_t netId, const std::string &ifName,
                                             const std::string &destination, const std::string &nextHop)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(destination)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(nextHop)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_NET_ROUTE),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::RemoveNetworkRoute(int32_t netId, const std::string &ifName,
                                                const std::string &destination, const std::string &nextHop)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(destination)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(nextHop)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REMOVE_NET_ROUTE),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                                 int32_t prefixLength)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ipAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteInt32(prefixLength)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_NET_ADDRESS),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                                 int32_t prefixLength)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ipAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteInt32(prefixLength)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REMOVE_NET_ADDRESS),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                          const std::string &ifName)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ipAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(macAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_STATIC_ARP),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                          const std::string &ifName)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ipAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(macAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DEL_STATIC_ARP),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::AddStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
    const std::string &ifName)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ipv6Addr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(macAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_STATIC_IPV6),
        data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::DelStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
    const std::string &ifName)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ipv6Addr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(macAddr)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DEL_STATIC_IPV6),
        data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::RegisterSlotType(uint32_t supplierId, int32_t type)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteInt32(type)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_SLOT_TYPE),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::GetSlotType(std::string &type)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SLOT_TYPE),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    int32_t ret = reply.ReadInt32();
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadString(type)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::FactoryResetNetwork()
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_FACTORYRESET_NETWORK), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_FACTORYRESET_CALLBACK), dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::IsPreferCellularUrl(const std::string& url, PreferCellularType& preferCellular)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteString(url)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    int32_t error = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_IS_PREFER_CELLULAR_URL),
                                      data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    int32_t ret = reply.ReadInt32();
    if (ret == NETMANAGER_SUCCESS) {
        int32_t preferCellularType = 0;
        if (!reply.ReadInt32(preferCellularType) ||
            preferCellularType < 0 || preferCellularType >= static_cast<int32_t>(PreferCellularType::END)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        preferCellular = static_cast<PreferCellularType>(preferCellularType);
    }
    return ret;
}

int32_t NetConnServiceProxy::RegisterPreAirplaneCallback(const sptr<IPreAirplaneCallback> callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_PREAIRPLANE_CALLBACK), dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UnregisterPreAirplaneCallback(const sptr<IPreAirplaneCallback> callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(
        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREGISTER_PREAIRPLANE_CALLBACK), dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UpdateSupplierScore(uint32_t supplierId, uint32_t detectionStatus)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(detectionStatus)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UPDATE_SUPPLIER_SCORE),
        data, reply);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::GetDefaultSupplierId(NetBearType bearerType, const std::string &ident,
    uint32_t& supplierId)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    uint32_t type = static_cast<uint32_t>(bearerType);
    if (!data.WriteUint32(type)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(ident)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_SUPPLIER_ID),
        data, reply);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadUint32(supplierId)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::CloseSocketsUid(int32_t netId, uint32_t uid)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(netId)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("WriteUint32 failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_CLOSE_SOCKETS_UID),
        data, reply);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::SetAppIsFrozened(uint32_t uid, bool isFrozened)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteBool(isFrozened)) {
        NETMGR_LOG_E("WriteBool failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_APP_IS_FROZENED),
        data, reply);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::IsDeadFlowResetTargetBundle(const std::string &bundleName, bool &flag)
{
    MessageParcel dataParcel;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteString(bundleName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP

    MessageParcel replyParcel;
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DEAD_FLOW_RESET_TARGET_BUNDLE),
                                        dataParcel, replyParcel);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);

    int32_t ret = 0;
    if (!replyParcel.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!replyParcel.ReadBool(flag)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::EnableAppFrozenedCallbackLimitation(bool flag)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteBool(flag)) {
        NETMGR_LOG_E("WriteBool failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    int32_t retCode = RemoteSendRequest(static_cast<uint32_t>(
        ConnInterfaceCode::CMD_NM_ENABLE_APP_FROZENED_CALLBACK_LIMITATION), data, reply);
    if (retCode != NETMANAGER_SUCCESS) {
        return retCode;
    }
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::SetReuseSupplierId(uint32_t supplierId, uint32_t reuseSupplierId, bool isReused)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(reuseSupplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteBool(isReused)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_REUSE_SUPPLIER_ID), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::GetNetExtAttribute(int32_t netId, std::string &netExtAttribute)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(netExtAttribute)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_NET_EXT_ATTRIBUTE),
        data, reply);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadString(netExtAttribute)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::SetNetExtAttribute(int32_t netId, const std::string &netExtAttribute)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(netExtAttribute)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_NET_EXT_ATTRIBUTE),
        data, reply);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_IP_NEIGH_TABLE), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size;
        if (!reply.ReadUint32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = (size > MAX_IP_NEIGH_TABLE_SIZE) ? MAX_IP_NEIGH_TABLE_SIZE : size;
        for (uint32_t i = 0; i <size; ++i) {
            sptr<NetIpMacInfo> infoPtr =  NetIpMacInfo::Unmarshalling(reply);
            if (infoPtr == nullptr) {
                ipMacInfo.clear();
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            ipMacInfo.push_back(*infoPtr);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::CreateVlan(const std::string &ifName, uint32_t vlanId)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(vlanId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_CREATE_VLAN),
        data, reply);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    // LCOV_EXCL_STOP
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::DestroyVlan(const std::string &ifName, uint32_t vlanId)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(vlanId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DESTROY_VLAN),
        data, reply);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    // LCOV_EXCL_STOP
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::AddVlanIp(const std::string &ifName, uint32_t vlanId,
                                       const std::string &ip, uint32_t mask)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(vlanId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(ip)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(mask)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_VLAN_IP),
        data, reply);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    // LCOV_EXCL_STOP
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::DeleteVlanIp(const std::string &ifName, uint32_t vlanId,
                                          const std::string &ip, uint32_t mask)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ifName)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(vlanId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteString(ip)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(mask)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DELETE_VLAN_IP),
        data, reply);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    // LCOV_EXCL_STOP
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::GetConnectOwnerUid(const NetConnInfo &netConnInfo, int32_t &ownerUid)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!netConnInfo.Marshalling(data)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_CONNECT_OWNER_UID), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadInt32(ownerUid)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

// LCOV_EXCL_START This will never happen.
int32_t NetConnServiceProxy::GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo)
{
    MessageParcel data;
    MessageParcel reply;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    int32_t error =
        RemoteSendRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SYSTEM_NET_PORT_STATES), data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        sptr<NetPortStatesInfo> infoPtr = NetPortStatesInfo::Unmarshalling(reply);
        if (infoPtr == nullptr) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        netPortStatesInfo = *infoPtr;
    }
    return ret;
}
// LCOV_EXCL_STOP
} // namespace NetManagerStandard
} // namespace OHOS

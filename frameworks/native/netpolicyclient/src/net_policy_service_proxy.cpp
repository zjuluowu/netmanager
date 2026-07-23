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

#include "net_policy_service_proxy.h"

#include "net_mgr_log_wrapper.h"
#include "net_policy_constants.h"
#include "ipc_skeleton.h"
#include "bundle_mgr_interface.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr uint32_t MAX_LIST_SIZE = 1000;
NetPolicyServiceProxy::NetPolicyServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<INetPolicyService>(impl) {}

NetPolicyServiceProxy::~NetPolicyServiceProxy() = default;

int32_t NetPolicyServiceProxy::SendRequest(sptr<IRemoteObject> &remote, uint32_t code, MessageParcel &data,
                                           MessageParcel &reply, MessageOption &option)
{
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    int32_t retCode = remote->SendRequest(code, data, reply, option);
    if (retCode != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    return ret;
}

int32_t NetPolicyServiceProxy::SetPolicyByUid(uint32_t uid, uint32_t policy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint32(policy)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POLICY_BY_UID), data, reply,
                       option);
}

int32_t NetPolicyServiceProxy::GetPolicyByUid(uint32_t uid, uint32_t &policy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode =
        SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_POLICY_BY_UID), data, reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadUint32(policy)) {
        NETMGR_LOG_E("Read uint32 reply failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    return retCode;
}

int32_t NetPolicyServiceProxy::GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(policy)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_UIDS_BY_POLICY), data,
                                  reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadUInt32Vector(&uids)) {
        NETMGR_LOG_E("Read uint32 vector reply failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return retCode;
}

int32_t NetPolicyServiceProxy::IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteBool(metered)) {
        NETMGR_LOG_E("Write Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_IS_NET_ALLOWED_BY_METERED),
                                  data, reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadBool(isAllowed)) {
        NETMGR_LOG_E("Read Bool reply failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return retCode;
}

int32_t NetPolicyServiceProxy::IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ifaceName)) {
        NETMGR_LOG_E("Write Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_IS_NET_ALLOWED_BY_IFACE),
                                  data, reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadBool(isAllowed)) {
        NETMGR_LOG_E("Read Bool reply failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return retCode;
}

int32_t NetPolicyServiceProxy::RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback)
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

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_REGISTER_NET_POLICY_CALLBACK),
                       dataParcel, replyParcel, option);
}

int32_t NetPolicyServiceProxy::UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback)
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

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_UNREGISTER_NET_POLICY_CALLBACK),
                       dataParcel, replyParcel, option);
}

int32_t NetPolicyServiceProxy::SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    MessageParcel data;
    if (quotaPolicies.empty()) {
        NETMGR_LOG_E("quotaPolicies is empty");
        return NetPolicyResultCode::POLICY_ERR_INVALID_QUOTA_POLICY;
    }

    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    if (!NetQuotaPolicy::Marshalling(data, quotaPolicies)) {
        NETMGR_LOG_E("Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NET_QUOTA_POLICIES), data, reply,
                       option);
}

int32_t NetPolicyServiceProxy::GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_NET_QUOTA_POLICIES),
                                  data, reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!NetQuotaPolicy::Unmarshalling(reply, quotaPolicies)) {
        NETMGR_LOG_E("Unmarshalling failed.");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return retCode;
}

int32_t NetPolicyServiceProxy::ResetPolicies(const std::string &simId)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(simId)) {
        NETMGR_LOG_E("Write String data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_RESET_POLICIES), data, reply, option);
}

int32_t NetPolicyServiceProxy::SetBackgroundPolicy(bool isBackgroundPolicyAllow)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteBool(isBackgroundPolicyAllow)) {
        NETMGR_LOG_E("Write Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_BACKGROUND_POLICY), data, reply,
                       option);
}

int32_t NetPolicyServiceProxy::GetBackgroundPolicy(bool &backgroundPolicy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_BACKGROUND_POLICY),
                                  data, reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadBool(backgroundPolicy)) {
        NETMGR_LOG_E("Read Bool reply failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return retCode;
}

int32_t NetPolicyServiceProxy::GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(
        remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_BACKGROUND_POLICY_BY_UID), data, reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadUint32(backgroundPolicyOfUid)) {
        NETMGR_LOG_E("Read uint32 reply failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return retCode;
}

int32_t NetPolicyServiceProxy::UpdateRemindPolicy(int32_t netType, const std::string &simId, uint32_t remindType)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    if (!data.WriteInt32(netType)) {
        NETMGR_LOG_E("Write int32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(simId)) {
        NETMGR_LOG_E("Write String data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteUint32(remindType)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_UPDATE_REMIND_POLICY), data, reply,
                       option);
}

int32_t NetPolicyServiceProxy::SetDeviceIdleTrustlist(const std::vector<uint32_t> &uids, bool isAllowed)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUInt32Vector(uids)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteBool(isAllowed)) {
        NETMGR_LOG_E("Write Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_TRUSTLIST), data, reply,
                       option);
}

int32_t NetPolicyServiceProxy::GetDeviceIdleTrustlist(std::vector<uint32_t> &uids)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_IDLE_TRUSTLIST), data,
                                  reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadUInt32Vector(&uids)) {
        NETMGR_LOG_E("Read reply uint32 Vector failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return retCode;
}

int32_t NetPolicyServiceProxy::SetDeviceIdlePolicy(bool enable)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    if (!data.WriteBool(enable)) {
        NETMGR_LOG_E("WriteBool Bool data failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_DEVICE_IDLE_POLICY), data, reply,
                       option);
}

int32_t NetPolicyServiceProxy::SetPowerSavePolicy(bool enable)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    if (!data.WriteBool(enable)) {
        NETMGR_LOG_E("Write Bool data failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POWER_SAVE_POLICY), data, reply,
                       option);
}

int32_t NetPolicyServiceProxy::GetPowerSaveTrustlist(std::vector<uint32_t> &uids)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_POWER_SAVE_TRUSTLIST),
                                  data, reply, option);
    if (retCode != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadUInt32Vector(&uids)) {
        NETMGR_LOG_E("proxy SendRequest Readuint32Vector failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return retCode;
}

int32_t NetPolicyServiceProxy::SetPowerSaveTrustlist(const std::vector<uint32_t> &uids, bool isAllowed)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    if (!data.WriteUInt32Vector(uids)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteBool(isAllowed)) {
        NETMGR_LOG_E("Write Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POWER_SAVE_TRUSTLIST), data,
                       reply, option);
}

bool NetPolicyServiceProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetPolicyServiceProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t NetPolicyServiceProxy::CheckPermission()
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    // LCOV_EXCL_STOP

    MessageParcel reply;
    MessageOption option;
    int ret = remote->SendRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_CHECK_PERMISSION),
        data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, ret code: [%{public}d]", ret);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    int32_t result = 0;
    if (!reply.ReadInt32(result)) {
        NETMGR_LOG_E("Read int32 reply failed.");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return result;
}

int32_t NetPolicyServiceProxy::FactoryResetPolicies()
{
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceProxy::SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint8(policy.wifiAllow)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint8(policy.cellularAllow)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteBool(reconfirmFlag)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NETWORK_ACCESS_POLICY),
                                        data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return reply.ReadInt32();
}

int32_t NetPolicyServiceProxy::AddNetworkAccessPolicy(const std::vector<std::string> &bundleNames)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteStringVector(bundleNames)) {
        NETMGR_LOG_E("WriteStringVector failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    // LCOV_EXCL_STOP

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote,
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_ADD_NETWORK_ACCESS_POLICY), data, reply, option);
    // LCOV_EXCL_START
    if (retCode != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }
    // LCOV_EXCL_STOP

    return retCode;
}

int32_t NetPolicyServiceProxy::RemoveNetworkAccessPolicy(const std::vector<std::string> &bundleNames)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteStringVector(bundleNames)) {
        NETMGR_LOG_E("WriteStringVector failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    // LCOV_EXCL_STOP

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote,
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_REMOVE_NETWORK_ACCESS_POLICY), data, reply, option);
    // LCOV_EXCL_START
    if (retCode != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }
    // LCOV_EXCL_STOP

    return retCode;
}

int32_t NetPolicyServiceProxy::GetNetworkAccessPolicy(AccessPolicyParameter parameter, AccessPolicySave &policy)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t callingUid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());
    if (!data.WriteBool(parameter.flag)) {
        return false;
    }
    if (!data.WriteInt32(parameter.uid)) {
        return false;
    }
    if (!data.WriteUint32(callingUid)) {
        return false;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    // LCOV_EXCL_STOP

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_NETWORK_ACCESS_POLICY),
                                  data, reply, option);
    if (retCode != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    retCode = NetworkAccessPolicy::Unmarshalling(reply, policy, parameter.flag);
    if (retCode != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("proxy SendRequest unmarshalling failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    return retCode;
}

int32_t NetPolicyServiceProxy::GetSelfNetworkAccessPolicy(NetAccessPolicy &policy)
{
    // LCOV_EXCL_START
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t retCode = SendRequest(remote,
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_SELF_NETWORK_ACCESS_POLICY), data, reply, option);
    if (retCode != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    if (!reply.ReadBool(policy.allowWiFi)) {
        NETMGR_LOG_E("Read bool wifiAllow failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (!reply.ReadBool(policy.allowCellular)) {
        NETMGR_LOG_E("Read bool cellularAllow failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    // LCOV_EXCL_STOP
    return retCode;
}

int32_t NetPolicyServiceProxy::NotifyNetAccessPolicyDiag(uint32_t uid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    MessageOption option;

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t retCode =
        SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_NOTIFY_NETWORK_ACCESS_POLICY_DIAG), data,
                    reply, option);
    if (retCode != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", retCode);
        return retCode;
    }

    return retCode;
}

int32_t NetPolicyServiceProxy::SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteBool(status)) {
        NETMGR_LOG_E("SetNicTrafficAllowed WriteBool func return error");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(ifaceNames.size())) {
        NETMGR_LOG_E("SetNicTrafficAllowed ifaceNames size return error");
        return ERR_FLATTEN_OBJECT;
    }
    // LCOV_EXCL_STOP
    for (const std::string& iter : ifaceNames) {
        if (!data.WriteString(iter)) {
            NETMGR_LOG_E("SetNicTrafficAllowed write name return error");
            return ERR_FLATTEN_OBJECT;
        }
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NIC_TRAFFIC_ALLOWED),
        data, reply, option);
}

int32_t NetPolicyServiceProxy::SetInternetAccessByIpForWifiShare(
    const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("SetInternetAccessByIpForWifiShare WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(ipAddr)) {
        NETMGR_LOG_E("SetInternetAccessByIpForWifiShare WriteString ipAddr failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint8(family)) {
        NETMGR_LOG_E("SetInternetAccessByIpForWifiShare WriteInt8 family failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteBool(accessInternet)) {
        NETMGR_LOG_E("SetInternetAccessByIpForWifiShare WriteBool accessInternet failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(clientNetIfName)) {
        NETMGR_LOG_E("SetInternetAccessByIpForWifiShare WriteString clientNetIfName failed");
        return ERR_FLATTEN_OBJECT;
    }
    
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    // LCOV_EXCL_STOP

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote,
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_INTERNET_ACCESS_BY_IP_FOR_WIFI_SHARE),
        data, reply, option);
}

int32_t NetPolicyServiceProxy::SetIdleDenyPolicy(bool enable)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    if (!data.WriteBool(enable)) {
        NETMGR_LOG_E("Write Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENY_POLICY), data, reply,
                       option);
}

int32_t NetPolicyServiceProxy::SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd)
{
    MessageParcel data;
    // LCOV_EXCL_START
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t uidsSize = uids.size();
    if (uidsSize < 0 || uidsSize > MAX_LIST_SIZE) {
        NETMGR_LOG_E("uids length is invalid: %{public}d", uidsSize);
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    if (!data.WriteInt32(static_cast<int32_t>(uidsSize))) {
        NETMGR_LOG_E("Write int32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    for (uint32_t i = 0; i < uidsSize; ++i) {
        if (!data.WriteUint32(uids[i])) {
            NETMGR_LOG_E("Write uint32 data failed");
            return NETMANAGER_ERR_WRITE_DATA_FAIL;
        }
    }
    if (!data.WriteBool(isAdd)) {
        NETMGR_LOG_E("Write Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    // LCOV_EXCL_STOP

    MessageParcel reply;
    MessageOption option;
    return SendRequest(remote, static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENYLIST), data,
                       reply, option);
}
} // namespace NetManagerStandard
} // namespace OHOS

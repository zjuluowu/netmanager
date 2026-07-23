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

#include "net_policy_service_stub.h"

#include "net_mgr_log_wrapper.h"
#include "net_policy_core.h"
#include "net_quota_policy.h"
#include "netmanager_base_permission.h"
#include "ipc_skeleton.h"
#include "broadcast_manager.h"
#include "cJSON.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
std::map<uint32_t, const char *> g_codeNPS = {
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POLICY_BY_UID), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_POLICY_BY_UID), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_UIDS_BY_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_IS_NET_ALLOWED_BY_METERED), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_IS_NET_ALLOWED_BY_IFACE), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_REGISTER_NET_POLICY_CALLBACK), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_UNREGISTER_NET_POLICY_CALLBACK),
     Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NET_QUOTA_POLICIES), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_NET_QUOTA_POLICIES), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_UPDATE_REMIND_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_TRUSTLIST), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_IDLE_TRUSTLIST), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_DEVICE_IDLE_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_RESET_POLICIES), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_BACKGROUND_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_BACKGROUND_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_BACKGROUND_POLICY_BY_UID), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POWER_SAVE_TRUSTLIST), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_POWER_SAVE_TRUSTLIST), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POWER_SAVE_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_CHECK_PERMISSION), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NETWORK_ACCESS_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_NETWORK_ACCESS_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_NOTIFY_NETWORK_ACCESS_POLICY_DIAG),
     Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NIC_TRAFFIC_ALLOWED), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_INTERNET_ACCESS_BY_IP_FOR_WIFI_SHARE),
     Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENY_POLICY), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENYLIST), Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_ADD_NETWORK_ACCESS_POLICY),
     Permission::MANAGE_NET_STRATEGY},
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_REMOVE_NETWORK_ACCESS_POLICY),
     Permission::MANAGE_NET_STRATEGY},
};
std::map<uint32_t, const char *> g_pub_codeNPS = {
    {static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_SELF_NETWORK_ACCESS_POLICY), ""},
};
constexpr uint32_t MAX_IFACENAMES_SIZE = 128;
constexpr int32_t MAX_LIST_SIZE = 1000;
constexpr int UID_EDM = 3057;
constexpr int UID_NET_MANAGER = 1099;
constexpr int UID_IOT_NET_MANAGER = 7211;
constexpr int UID_COLLABORATION = 5520;
constexpr int UID_RSS = 1096;
} // namespace

NetPolicyServiceStub::NetPolicyServiceStub() : ffrtQueue_(NET_POLICY_STUB_QUEUE)
{
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POLICY_BY_UID)] =
        &NetPolicyServiceStub::OnSetPolicyByUid;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_POLICY_BY_UID)] =
        &NetPolicyServiceStub::OnGetPolicyByUid;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_UIDS_BY_POLICY)] =
        &NetPolicyServiceStub::OnGetUidsByPolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_IS_NET_ALLOWED_BY_METERED)] =
        &NetPolicyServiceStub::OnIsUidNetAllowedMetered;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_IS_NET_ALLOWED_BY_IFACE)] =
        &NetPolicyServiceStub::OnIsUidNetAllowedIfaceName;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_REGISTER_NET_POLICY_CALLBACK)] =
        &NetPolicyServiceStub::OnRegisterNetPolicyCallback;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_UNREGISTER_NET_POLICY_CALLBACK)] =
        &NetPolicyServiceStub::OnUnregisterNetPolicyCallback;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NET_QUOTA_POLICIES)] =
        &NetPolicyServiceStub::OnSetNetQuotaPolicies;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_NET_QUOTA_POLICIES)] =
        &NetPolicyServiceStub::OnGetNetQuotaPolicies;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_RESET_POLICIES)] =
        &NetPolicyServiceStub::OnResetPolicies;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_UPDATE_REMIND_POLICY)] =
        &NetPolicyServiceStub::OnSnoozePolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_TRUSTLIST)] =
        &NetPolicyServiceStub::OnSetDeviceIdleTrustlist;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_IDLE_TRUSTLIST)] =
        &NetPolicyServiceStub::OnGetDeviceIdleTrustlist;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_DEVICE_IDLE_POLICY)] =
        &NetPolicyServiceStub::OnSetDeviceIdlePolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_POWER_SAVE_TRUSTLIST)] =
        &NetPolicyServiceStub::OnGetPowerSaveTrustlist;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POWER_SAVE_TRUSTLIST)] =
        &NetPolicyServiceStub::OnSetPowerSaveTrustlist;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_BACKGROUND_POLICY)] =
        &NetPolicyServiceStub::OnSetBackgroundPolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_BACKGROUND_POLICY)] =
        &NetPolicyServiceStub::OnGetBackgroundPolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_BACKGROUND_POLICY_BY_UID)] =
        &NetPolicyServiceStub::OnGetBackgroundPolicyByUid;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POWER_SAVE_POLICY)] =
        &NetPolicyServiceStub::OnSetPowerSavePolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_CHECK_PERMISSION)] =
        &NetPolicyServiceStub::OnCheckPermission;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_FACTORYRESET_POLICIES)] =
        &NetPolicyServiceStub::OnFactoryResetPolicies;
    ExtraNetPolicyServiceStub();
    InitEventHandler();
}

void NetPolicyServiceStub::ExtraNetPolicyServiceStub()
{
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NETWORK_ACCESS_POLICY)] =
        &NetPolicyServiceStub::OnSetNetworkAccessPolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_NETWORK_ACCESS_POLICY)] =
        &NetPolicyServiceStub::OnGetNetworkAccessPolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_SELF_NETWORK_ACCESS_POLICY)] =
        &NetPolicyServiceStub::OnGetSelfNetworkAccessPolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_NOTIFY_NETWORK_ACCESS_POLICY_DIAG)] =
        &NetPolicyServiceStub::OnNotifyNetAccessPolicyDiag;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NIC_TRAFFIC_ALLOWED)] =
        &NetPolicyServiceStub::OnSetNicTrafficAllowed;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_INTERNET_ACCESS_BY_IP_FOR_WIFI_SHARE)] =
        &NetPolicyServiceStub::OnSetInternetAccessByIpForWifiShare;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENY_POLICY)] =
        &NetPolicyServiceStub::OnSetIdleDenyPolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENYLIST)] =
        &NetPolicyServiceStub::OnSetUidsDeniedListChain;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_ADD_NETWORK_ACCESS_POLICY)] =
        &NetPolicyServiceStub::OnAddNetworkAccessPolicy;
    memberFuncMap_[static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_REMOVE_NETWORK_ACCESS_POLICY)] =
        &NetPolicyServiceStub::OnRemoveNetworkAccessPolicy;
    return;
}

NetPolicyServiceStub::~NetPolicyServiceStub() = default;

void NetPolicyServiceStub::InitEventHandler()
{
    std::call_once(onceFlag, [this]() {
        auto core = DelayedSingleton<NetPolicyCore>::GetInstance();
        handler_ = std::make_shared<NetPolicyEventHandler>(core, ffrtQueue_);
        core->Init(handler_);
    });
}

int32_t NetPolicyServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                              MessageOption &option)
{
    std::u16string myDescriptor = NetPolicyServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        NETMGR_LOG_E("descriptor checked fail");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }
    NETMGR_LOG_D("stub call start, code = [%{public}d]", code);
    if (handler_ == nullptr) {
        NETMGR_LOG_E("Net policy handler is null, recreate handler.");
        InitEventHandler();
        if (handler_ == nullptr) {
            NETMGR_LOG_E("recreate net policy handler failed.");
            return NETMANAGER_ERR_INTERNAL;
        }
    }
    auto res = CheckProcessPermission(code);
    if (res != NETMANAGER_SUCCESS) {
        return res;
    }
    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        int32_t checkPermissionResult = CheckPolicyPermission(code);
        if (checkPermissionResult != NETMANAGER_SUCCESS) {
            if (!reply.WriteInt32(checkPermissionResult)) {
                return IPC_STUB_WRITE_PARCEL_ERR;
            }
            return NETMANAGER_SUCCESS;
        }
        int32_t result = NETMANAGER_SUCCESS;
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            result = (this->*requestFunc)(data, reply);
            NETMGR_LOG_D("stub call end, code = [%{public}d], ret = [%{public}d]", code, result);
            return result;
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

bool NetPolicyServiceStub::SubCheckPermission(const std::string &permission, uint32_t funcCode)
{
    if (NetManagerPermission::CheckPermission(permission)) {
        return true;
    }
    NETMGR_LOG_E("Permission denied funcCode: %{public}d permission: %{public}s", funcCode, permission.c_str());
    return false;
}

int32_t NetPolicyServiceStub::CheckPolicyPermission(uint32_t code)
{
    if (g_codeNPS.find(code) != g_codeNPS.end()) {
        bool result = NetManagerPermission::IsSystemCaller();
        // LCOV_EXCL_START
        if (!result) {
            return NETMANAGER_ERR_NOT_SYSTEM_CALL;
        }
        // LCOV_EXCL_STOP
        result = SubCheckPermission(g_codeNPS[code], code);
        if (!result) {
            return NETMANAGER_ERR_PERMISSION_DENIED;
        }
        return NETMANAGER_SUCCESS;
    }
    if (g_pub_codeNPS.find(code) != g_pub_codeNPS.end()) {
        std::string permission = g_pub_codeNPS[code];
        if (permission.empty()) {
            return NETMANAGER_SUCCESS;
        }
        bool result = SubCheckPermission(permission, code);
        if (!result) {
            return NETMANAGER_ERR_PERMISSION_DENIED;
        }
        return NETMANAGER_SUCCESS;
    }
    NETMGR_LOG_E("Error funcCode, need check");
    return NETMANAGER_ERR_PERMISSION_DENIED;
}

int32_t NetPolicyServiceStub::CheckProcessPermission(uint32_t code)
{
    auto uid = IPCSkeleton::GetCallingUid();
    if (code == static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_INTERNET_ACCESS_BY_IP_FOR_WIFI_SHARE) &&
        uid != UID_COLLABORATION) {
        NETMGR_LOG_E("CheckUidPermission failed, code %{public}d, uid %{public}d", code, uid);
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    if ((code == static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENY_POLICY) ||
        code == static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENYLIST)) &&
        uid != UID_RSS) {
        NETMGR_LOG_E("CheckUidPermission failed, code %{public}d, uid %{public}d", code, uid);
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetPolicyByUid(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetPolicyByUid callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read Uint32 data failed.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t netPolicy = 0;
    if (!data.ReadUint32(netPolicy)) {
        NETMGR_LOG_E("Read Uint32 data failed.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetPolicyByUid(uid, netPolicy);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed.");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetPolicyByUid(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("GetPolicyByUid callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t policy = 0;
    int32_t result = GetPolicyByUid(uid, policy);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed.");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteInt32(policy)) {
            NETMGR_LOG_E("Write int32 reply failed.");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetUidsByPolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("GetUidsByPolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    uint32_t policy = 0;
    if (!data.ReadUint32(policy)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::vector<uint32_t> uids;
    int32_t result = GetUidsByPolicy(policy, uids);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteUInt32Vector(uids)) {
            NETMGR_LOG_E("Write uint32 vector reply failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnIsUidNetAllowedMetered(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("IsUidNetAllowedMetered callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    uint32_t uid = 0;
    bool metered = false;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (!data.ReadBool(metered)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    bool isAllowed = false;
    int32_t result = IsUidNetAllowed(uid, metered, isAllowed);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(isAllowed)) {
            NETMGR_LOG_E("Write Bool reply failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnIsUidNetAllowedIfaceName(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("IsUidNetAllowedIfaceName callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    uint32_t uid = 0;
    std::string ifaceName;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (!data.ReadString(ifaceName)) {
        NETMGR_LOG_E("Read String data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (ifaceName.size() > MAX_IFACENAMES_SIZE) {
        NETMGR_LOG_E("ifaceName too long");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    bool isAllowed = false;
    int32_t result = IsUidNetAllowed(uid, ifaceName, isAllowed);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(isAllowed)) {
            NETMGR_LOG_E("Write Bool reply failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnRegisterNetPolicyCallback(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("RegisterNetPolicyCallback callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetPolicyCallback> callback = iface_cast<INetPolicyCallback>(remote);
    int32_t result = RegisterNetPolicyCallback(callback);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnUnregisterNetPolicyCallback(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("UnregisterNetPolicyCallback callingUid/callingPid: %{public}d/%{public}d",
                 IPCSkeleton::GetCallingUid(), IPCSkeleton::GetCallingPid());
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("callback ptr is nullptr.");
        reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    sptr<INetPolicyCallback> callback = iface_cast<INetPolicyCallback>(remote);
    int32_t result = UnregisterNetPolicyCallback(callback);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetNetQuotaPolicies(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetNetQuotaPolicies callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    std::vector<NetQuotaPolicy> quotaPolicies;
    if (!NetQuotaPolicy::Unmarshalling(data, quotaPolicies)) {
        NETMGR_LOG_E("Unmarshalling failed.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetNetQuotaPolicies(quotaPolicies);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetNetQuotaPolicies(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("GetNetQuotaPolicies callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    std::vector<NetQuotaPolicy> quotaPolicies;

    int32_t result = GetNetQuotaPolicies(quotaPolicies);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!NetQuotaPolicy::Marshalling(reply, quotaPolicies)) {
            NETMGR_LOG_E("Marshalling failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnResetPolicies(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("ResetPolicies callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    std::string subscriberId;
    if (!data.ReadString(subscriberId)) {
        NETMGR_LOG_E("Read String data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = ResetPolicies(subscriberId);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetBackgroundPolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetBackgroundPolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    bool isBackgroundPolicyAllow = false;
    if (!data.ReadBool(isBackgroundPolicyAllow)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetBackgroundPolicy(isBackgroundPolicyAllow);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetBackgroundPolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("GetBackgroundPolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    bool backgroundPolicy = false;
    int32_t result = GetBackgroundPolicy(backgroundPolicy);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(backgroundPolicy)) {
            NETMGR_LOG_E("Write Bool reply failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetBackgroundPolicyByUid(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("GetBackgroundPolicyByUid callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t backgroundPolicyOfUid = 0;
    int32_t result = GetBackgroundPolicyByUid(uid, backgroundPolicyOfUid);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(backgroundPolicyOfUid)) {
            NETMGR_LOG_E("Write uint32 reply failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSnoozePolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SnoozePolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    int32_t netType = 0;
    if (!data.ReadInt32(netType)) {
        NETMGR_LOG_E("Read int32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string simId;
    if (!data.ReadString(simId)) {
        NETMGR_LOG_E("Read String data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t remindType = 0;
    if (!data.ReadUint32(remindType)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = UpdateRemindPolicy(netType, simId, remindType);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetDeviceIdleTrustlist(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetDeviceIdleTrustlist callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    std::vector<uint32_t> uids;
    if (!data.ReadUInt32Vector(&uids)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    bool isAllowed = false;
    if (!data.ReadBool(isAllowed)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetDeviceIdleTrustlist(uids, isAllowed);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetDeviceIdleTrustlist(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("GetDeviceIdleTrustlist callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    std::vector<uint32_t> uids;
    int32_t result = GetDeviceIdleTrustlist(uids);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteUInt32Vector(uids)) {
            NETMGR_LOG_E("Write uint32 vector reply failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetDeviceIdlePolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetDeviceIdlePolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    bool isAllowed = false;
    if (!data.ReadBool(isAllowed)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetDeviceIdlePolicy(isAllowed);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetPowerSaveTrustlist(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("GetPowerSaveTrustlist callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    std::vector<uint32_t> uids;
    int32_t result = GetPowerSaveTrustlist(uids);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteUInt32Vector(uids)) {
            NETMGR_LOG_E("Write uint32 Vector reply failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetPowerSaveTrustlist(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetPowerSaveTrustlist callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    std::vector<uint32_t> uids;
    if (!data.ReadUInt32Vector(&uids)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    bool isAllowed = false;
    if (!data.ReadBool(isAllowed)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetPowerSaveTrustlist(uids, isAllowed);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetPowerSavePolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetPowerSavePolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    bool isAllowed = false;
    if (!data.ReadBool(isAllowed)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetPowerSavePolicy(isAllowed);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnCheckPermission(MessageParcel &data, MessageParcel &reply)
{
    if (!reply.WriteInt32(NETMANAGER_SUCCESS)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnFactoryResetPolicies(MessageParcel &data, MessageParcel &reply)
{
    return NETMANAGER_SUCCESS;
}

void NetPolicyServiceStub::HandleReportNetworkPolicy()
{
#ifndef UNITTEST_FORBID_FFRT
    std::lock_guard<ffrt::mutex> lock(setNetworkPolicyMutex_);
#endif
    if (appNetworkPolicyMap_.empty()) {
        return;
    }
    BroadcastInfo info;
    info.action = NETWORK_POLICY_CHANGED_EVENT;
    info.subscriberUid = HIVIEW_UID;
    cJSON* networkPolicyJson = cJSON_CreateObject();
    for (auto &callingPolicy : appNetworkPolicyMap_) {
        cJSON* appPolicyJson = cJSON_CreateObject();
        for (auto &appPolicy : callingPolicy.second) {
            cJSON_AddNumberToObject(appPolicyJson, std::to_string(appPolicy.first).c_str(), appPolicy.second);
        }
        cJSON_AddItemToObject(networkPolicyJson, std::to_string(callingPolicy.first).c_str(), appPolicyJson);
    }
    char *pParamJson = cJSON_PrintUnformatted(networkPolicyJson);
    cJSON_Delete(networkPolicyJson);
    if (!pParamJson) {
        return;
    }
    std::string paramStr(pParamJson);
    NETMGR_LOG_I("HandleReportNetworkPolicy, %{public}s", paramStr.c_str());
    std::map<std::string, std::string> param = {{NETWORK_POLICY_INFO_KEY, paramStr}};
    BroadcastManager::GetInstance().SendBroadcast(info, param);
    cJSON_free(pParamJson);
    isPostDelaySetNetworkPolicy_ = false;
    appNetworkPolicyMap_.clear();
}
 
void NetPolicyServiceStub::HandleStoreNetworkPolicy(uint32_t uid, NetworkAccessPolicy &policy,
    uint32_t callingUid)
{
    std::lock_guard<ffrt::mutex> lock(setNetworkPolicyMutex_);
    if (appNetworkPolicyMap_.find(callingUid) == appNetworkPolicyMap_.end()) {
        std::map<uint32_t, uint32_t> policyMap;
        appNetworkPolicyMap_.emplace(callingUid, std::move(policyMap));
    }
    uint32_t policyInfo = 0;
    policyInfo |= policy.wifiAllow ? NET_POLICY_WIFI_ALLOW : 0;
    policyInfo |= policy.cellularAllow ? NET_POLICY_CELLULAR_ALLOW : 0;
    auto& allNetworkPolicy = appNetworkPolicyMap_.at(callingUid);
    allNetworkPolicy[uid] = policyInfo;
    if (!isPostDelaySetNetworkPolicy_) {
        isPostDelaySetNetworkPolicy_ = true;
#ifndef UNITTEST_FORBID_FFRT
        ffrtQueue_.submit([this]() {
#endif
            HandleReportNetworkPolicy();
#ifndef UNITTEST_FORBID_FFRT
            }, ffrt::task_attr().name("HandleReportNetworkPolicy").delay(NETWORK_POLICY_REPORT_DELAY));
#endif
    }
}

int32_t NetPolicyServiceStub::OnAddNetworkAccessPolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("OnAddNetworkAccessPolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    std::vector<std::string> bundleNames;

    // LCOV_EXCL_START
    if (!data.ReadStringVector(&bundleNames)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    // LCOV_EXCL_STOP

    int32_t ret = AddNetworkAccessPolicy(bundleNames);
    // LCOV_EXCL_START
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnRemoveNetworkAccessPolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("OnRemoveNetworkAccessPolicy callingUid/callingPid: %{public}d/%{public}d",
                 IPCSkeleton::GetCallingUid(), IPCSkeleton::GetCallingPid());
    std::vector<std::string> bundleNames;

    // LCOV_EXCL_START
    if (!data.ReadStringVector(&bundleNames)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    // LCOV_EXCL_STOP

    int32_t ret = RemoveNetworkAccessPolicy(bundleNames);
    // LCOV_EXCL_START
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetNetworkAccessPolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetNetworkAccessPolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    uint32_t uid;

    if (!data.ReadUint32(uid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint8_t wifi_allow;
    uint8_t cellular_allow;
    NetworkAccessPolicy policy;
    bool reconfirmFlag = true;

    if (!data.ReadUint8(wifi_allow)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (!data.ReadUint8(cellular_allow)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (!data.ReadBool(reconfirmFlag)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    policy.wifiAllow = wifi_allow;
    policy.cellularAllow = cellular_allow;
    int32_t ret = SetNetworkAccessPolicy(uid, policy, reconfirmFlag);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    HandleStoreNetworkPolicy(uid, policy, IPCSkeleton::GetCallingUid());
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetNetworkAccessPolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("GetNetworkAccessPolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    int32_t uid = 0;
    uint32_t callingUid = 1;
    bool flag = false;
    if (!data.ReadBool(flag)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (!data.ReadInt32(uid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (!data.ReadUint32(callingUid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    AccessPolicySave policies;
    AccessPolicyParameter parameters;
    parameters.flag = flag;
    parameters.uid = uid;
    parameters.callingUid = callingUid;

    int32_t ret = GetNetworkAccessPolicy(parameters, policies);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        ret = NetworkAccessPolicy::Marshalling(reply, policies, flag);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("GetNetworkAccessPolicy marshalling failed");
            return ret;
        }
    }

    return ret;
}

int32_t NetPolicyServiceStub::OnGetSelfNetworkAccessPolicy(MessageParcel &data, MessageParcel &reply)
{
    // LCOV_EXCL_START
    NetAccessPolicy policy;
    int32_t ret = GetSelfNetworkAccessPolicy(policy);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(policy.allowWiFi)) {
            NETMGR_LOG_E("Write bool wifiAllow failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        if (!reply.WriteBool(policy.allowCellular)) {
            NETMGR_LOG_E("Write bool cellularAllow failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    // LCOV_EXCL_STOP
    return ret;
}

int32_t NetPolicyServiceStub::OnNotifyNetAccessPolicyDiag(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("NotifyNetAccessPolicyDiag callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    uint32_t uid;

    if (!data.ReadUint32(uid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = NotifyNetAccessPolicyDiag(uid);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return ret;
}

int32_t NetPolicyServiceStub::OnSetNicTrafficAllowed(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETMGR_LOG_E("OnSetNicTrafficAllowed CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    auto uid = IPCSkeleton::GetCallingUid();
    if (uid != UID_EDM && uid != UID_NET_MANAGER && uid != UID_IOT_NET_MANAGER) {
        NETMGR_LOG_E("OnSetNicTrafficAllowed CheckUidPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    bool status = false;
    int32_t size = 0;
    if (!data.ReadBool(status) || !data.ReadInt32(size)) {
        NETMGR_LOG_E("OnSetNicTrafficAllowed read status or size failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (size > static_cast<int32_t>(MAX_IFACENAMES_SIZE)) {
        NETMGR_LOG_E("OnSetNicTrafficAllowed read data size too big");
        return ERR_FLATTEN_OBJECT;
    }
    std::vector<std::string> ifaceNames;
    std::string ifaceName;
    for (int32_t index = 0; index < size; index++) {
        data.ReadString(ifaceName);
        if (ifaceName.empty()) {
            NETMGR_LOG_E("OnSetNicTrafficAllowed ifaceName is empty, size mismatch");
            return ERR_FLATTEN_OBJECT;
        }
        ifaceNames.push_back(ifaceName);
    }
    int32_t result = SetNicTrafficAllowed(ifaceNames, status);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write OnSetNicTrafficAllowed result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetInternetAccessByIpForWifiShare(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETMGR_LOG_E("OnSetInternetAccessByIpForWifiShare CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    // LCOV_EXCL_START
    std::string ipAddr;
    if (!data.ReadString(ipAddr)) {
        NETMGR_LOG_E("OnSetInternetAccessByIpForWifiShare read ipAddr failed");
        return ERR_FLATTEN_OBJECT;
    }

    uint8_t family = 0;
    if (!data.ReadUint8(family)) {
        NETMGR_LOG_E("OnSetInternetAccessByIpForWifiShare read family failed");
        return ERR_FLATTEN_OBJECT;
    }

    bool accessInternet = false;
    if (!data.ReadBool(accessInternet)) {
        NETMGR_LOG_E("OnSetInternetAccessByIpForWifiShare read accessInternet failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string clientNetIfName;
    if (!data.ReadString(clientNetIfName)) {
        NETMGR_LOG_E("OnSetInternetAccessByIpForWifiShare read clientNetIfName failed");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t result = SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write OnSetInternetAccessByIpForWifiShare result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NETMANAGER_SUCCESS;
    // LCOV_EXCL_STOP
}

int32_t NetPolicyServiceStub::OnSetIdleDenyPolicy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetIdleDenyPolicy callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    // LCOV_EXCL_START
    bool isEnable = false;
    if (!data.ReadBool(isEnable)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetIdleDenyPolicy(isEnable);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
    // LCOV_EXCL_STOP
}

int32_t NetPolicyServiceStub::OnSetUidsDeniedListChain(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("SetUidsDeniedListChain callingUid/callingPid: %{public}d/%{public}d", IPCSkeleton::GetCallingUid(),
                 IPCSkeleton::GetCallingPid());
    // LCOV_EXCL_START
    int32_t uidsSize = -1;
    if (!data.ReadInt32(uidsSize)) {
        NETMGR_LOG_E("Read Int32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (uidsSize < 0 || uidsSize > MAX_LIST_SIZE) {
        NETMGR_LOG_E("uids length is invalid: %{public}d", uidsSize);
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    std::vector<uint32_t> uids;
    uint32_t uid;
    for (int32_t i = 0; i < uidsSize; ++i) {
        if (!data.ReadUint32(uid)) {
            NETMGR_LOG_E("Read uint32 data failed");
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        uids.push_back(uid);
    }
    bool isAdd = false;
    if (!data.ReadBool(isAdd)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetUidsDeniedListChain(uids, isAdd);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
    // LCOV_EXCL_STOP
}

} // namespace NetManagerStandard
} // namespace OHOS

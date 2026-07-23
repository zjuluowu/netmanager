/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "net_conn_service_stub.h"
#include "ipc_skeleton.h"
#include "net_conn_constants.h"
#include "net_conn_types.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_permission.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int32_t MAX_VNIC_UID_ARRAY_SIZE = 20;
constexpr size_t MAX_BUNDLE_NAME_LEN = 256;
constexpr uint32_t MAX_IFACE_NUM = 16;
constexpr uint32_t MAX_NET_CAP_NUM = 32;
constexpr uint32_t UID_FOUNDATION = 5523;
constexpr int32_t INVALID_UID = -1;
const std::vector<uint32_t> SYSTEM_CODE{static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_AIRPLANE_MODE),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_GLOBAL_HTTP_PROXY),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_GLOBAL_HTTP_PROXY),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_GET_IFACENAME_IDENT_MAPS),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_FACTORYRESET_NETWORK),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_PROXY_MODE),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_PROXY_MODE),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_CREATE_VLAN),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DESTROY_VLAN),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_VLAN_IP),
                                        static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DELETE_VLAN_IP)};
const std::vector<uint32_t> PERMISSION_NEED_CACHE_CODES{
    static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GETDEFAULTNETWORK),
    static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_HASDEFAULTNET)};
} // namespace

void NetConnServiceStub::InitNetSupplierFuncToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REG_NET_SUPPLIER)] = {
        &NetConnServiceStub::OnRegisterNetSupplier, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREG_NETWORK)] = {
        &NetConnServiceStub::OnUnregisterNetSupplier, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UPDATE_NET_CAPS)] = {
        &NetConnServiceStub::OnUpdateNetCaps, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_NET_SUPPLIER_INFO)] = {
        &NetConnServiceStub::OnUpdateNetSupplierInfo, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_NET_LINK_INFO)] = {
        &NetConnServiceStub::OnUpdateNetLinkInfo, {Permission::CONNECTIVITY_INTERNAL}};
}

NetConnServiceStub::NetConnServiceStub()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SYSTEM_READY)] = {
        &NetConnServiceStub::OnSystemReady, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_CONN_CALLBACK)] = {
        &NetConnServiceStub::OnRegisterNetConnCallback, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_CONN_CALLBACK_BY_SPECIFIER)] = {
        &NetConnServiceStub::OnRegisterNetConnCallbackBySpecifier, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REQUEST_NET_CONNECTION)] = {
        &NetConnServiceStub::OnRequestNetConnectionBySpecifier, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREGISTER_NET_CONN_CALLBACK)] = {
        &NetConnServiceStub::OnUnregisterNetConnCallback, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UPDATE_NET_STATE_FOR_TEST)] = {
        &NetConnServiceStub::OnUpdateNetStateForTest, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_DETECTION_RET_CALLBACK)] = {
        &NetConnServiceStub::OnRegisterNetDetectionCallback, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREGISTER_NET_DETECTION_RET_CALLBACK)] = {
        &NetConnServiceStub::OnUnRegisterNetDetectionCallback, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_NET_DETECTION)] = {
        &NetConnServiceStub::OnNetDetection, {Permission::GET_NETWORK_INFO, Permission::INTERNET}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_SUPPLIER_CALLBACK)] = {
        &NetConnServiceStub::OnRegisterNetSupplierCallback, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_NET_DETECTION_RESPONSE)] = {
        &NetConnServiceStub::OnNetDetectionResponse, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_AIRPLANE_MODE)] = {
        &NetConnServiceStub::OnSetAirplaneMode, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_GLOBAL_HTTP_PROXY)] = {
        &NetConnServiceStub::OnSetGlobalHttpProxy, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_APP_NET)] = {&NetConnServiceStub::OnSetAppNet,
                                                                                    {Permission::INTERNET}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERNET_PERMISSION)] = {
        &NetConnServiceStub::OnSetInternetPermission, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_INTERFACE_CALLBACK)] = {
        &NetConnServiceStub::OnRegisterNetInterfaceCallback, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREGISTER_NET_INTERFACE_CALLBACK)] = {
        &NetConnServiceStub::OnUnregisterNetInterfaceCallback, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_NET_ROUTE)] = {
        &NetConnServiceStub::OnAddNetworkRoute, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REMOVE_NET_ROUTE)] = {
        &NetConnServiceStub::OnRemoveNetworkRoute, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_IS_PREFER_CELLULAR_URL)] = {
        &NetConnServiceStub::OnIsPreferCellularUrl, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_REUSE_SUPPLIER_ID)] = {
        &NetConnServiceStub::OnSetReuseSupplierId, {Permission::CONNECTIVITY_INTERNAL}};
    InitNetSupplierFuncToInterfaceMap();
    InitAll();
}

void NetConnServiceStub::InitAll()
{
    InitInterfaceFuncToInterfaceMap();
    InitResetNetFuncToInterfaceMap();
    InitStaticArpToInterfaceMap();
    InitQueryFuncToInterfaceMap();
    InitQueryFuncToInterfaceMapExt();
    InitVnicFuncToInterfaceMap();
    InitVirnicFuncToInterfaceMap();
    InitStaticIpv6ToInterfaceMap();
}

void NetConnServiceStub::InitInterfaceFuncToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_NET_ADDRESS)] = {
        &NetConnServiceStub::OnAddInterfaceAddress, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REMOVE_NET_ADDRESS)] = {
        &NetConnServiceStub::OnDelInterfaceAddress, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_PREAIRPLANE_CALLBACK)] = {
        &NetConnServiceStub::OnRegisterPreAirplaneCallback, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UNREGISTER_PREAIRPLANE_CALLBACK)] = {
        &NetConnServiceStub::OnUnregisterPreAirplaneCallback, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_UPDATE_SUPPLIER_SCORE)] = {
        &NetConnServiceStub::OnUpdateSupplierScore, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_SUPPLIER_ID)] = {
        &NetConnServiceStub::OnGetDefaultSupplierId, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_CLOSE_SOCKETS_UID)] = {
        &NetConnServiceStub::OnCloseSocketsUid, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_APP_IS_FROZENED)] = {
        &NetConnServiceStub::OnSetAppIsFrozened, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ENABLE_APP_FROZENED_CALLBACK_LIMITATION)] = {
        &NetConnServiceStub::OnEnableAppFrozenedCallbackLimitation, {Permission::CONNECTIVITY_INTERNAL}};
}

void NetConnServiceStub::InitResetNetFuncToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_FACTORYRESET_NETWORK)] = {
        &NetConnServiceStub::OnFactoryResetNetwork, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_NET_FACTORYRESET_CALLBACK)] = {
        &NetConnServiceStub::OnRegisterNetFactoryResetCallback, {Permission::CONNECTIVITY_INTERNAL}};
}

void NetConnServiceStub::InitStaticArpToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_STATIC_ARP)] = {
        &NetConnServiceStub::OnAddStaticArp, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DEL_STATIC_ARP)] = {
        &NetConnServiceStub::OnDelStaticArp, {Permission::CONNECTIVITY_INTERNAL}};
}

void NetConnServiceStub::InitStaticIpv6ToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_STATIC_IPV6)] = {
        &NetConnServiceStub::OnAddStaticIpv6Addr, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DEL_STATIC_IPV6)] = {
        &NetConnServiceStub::OnDelStaticIpv6Addr, {Permission::CONNECTIVITY_INTERNAL}};
}

void NetConnServiceStub::InitProxyFuncToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_PROXY_MODE)] = {
        &NetConnServiceStub::OnSetProxyMode, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_PAC_FILE_URL)] = {
        &NetConnServiceStub::OnSetPacFileUrl, {Permission::SET_PAC_URL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_PAC_FILE_URL)] = {
        &NetConnServiceStub::OnGetPacFileUrl, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_PROXY_MODE)] = {
        &NetConnServiceStub::OnGetProxyMode, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_FIND_PAC_PROXY_FOR_URL)] = {
        &NetConnServiceStub::OnFindProxyForURL, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_PAC_URL)] = {&NetConnServiceStub::OnSetPacUrl,
                                                                                    {Permission::SET_PAC_URL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_PAC_URL)] = {&NetConnServiceStub::OnGetPacUrl,
                                                                                    {}};
}

void NetConnServiceStub::InitQueryFuncToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_IFACE_NAMES)] = {
        &NetConnServiceStub::OnGetIfaceNames, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_IFACENAME_BY_TYPE)] = {
        &NetConnServiceStub::OnGetIfaceNameByType, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_GET_IFACENAME_IDENT_MAPS)] = {
        &NetConnServiceStub::OnGetIfaceNameIdentMaps, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GETDEFAULTNETWORK)] = {
        &NetConnServiceStub::OnGetDefaultNet, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_HASDEFAULTNET)] = {
        &NetConnServiceStub::OnHasDefaultNet, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_NET)] = {
        &NetConnServiceStub::OnGetSpecificNet, {Permission::INTERNET}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_ALL_NETS)] = {&NetConnServiceStub::OnGetAllNets,
                                                                                     {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_UID_NET)] = {
        &NetConnServiceStub::OnGetSpecificUidNet, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_CONNECTION_PROPERTIES)] = {
        &NetConnServiceStub::OnGetConnectionProperties, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_NET_CAPABILITIES)] = {
        &NetConnServiceStub::OnGetNetCapabilities, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_IS_DEFAULT_NET_METERED)] = {
        &NetConnServiceStub::OnIsDefaultNetMetered, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_GLOBAL_HTTP_PROXY)] = {
        &NetConnServiceStub::OnGetGlobalHttpProxy, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_DEFAULT_HTTP_PROXY)] = {
        &NetConnServiceStub::OnGetDefaultHttpProxy, {}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REFRESH_GLOBAL_HTTP_PROXY)] = {
        &NetConnServiceStub::OnRefreshGlobalHttpProxy, {Permission::INTERNET}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_NET_ID_BY_IDENTIFIER)] = {
        &NetConnServiceStub::OnGetNetIdByIdentifier, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_INTERFACE_CONFIGURATION)] = {
        &NetConnServiceStub::OnGetNetInterfaceConfiguration, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERFACE_IP_ADDRESS)] = {
        &NetConnServiceStub::OnSetNetInterfaceIpAddress, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERFACE_UP)] = {
        &NetConnServiceStub::OnSetInterfaceUp, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERFACE_DOWN)] = {
        &NetConnServiceStub::OnSetInterfaceDown, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REGISTER_SLOT_TYPE)] = {
        &NetConnServiceStub::OnRegisterSlotType, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SLOT_TYPE)] = {
        &NetConnServiceStub::OnGetSlotType, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SYSTEM_NET_PORT_STATES)] = {
        &NetConnServiceStub::OnGetSystemNetPortStates, {Permission::GET_IP_MAC_INFO}};
    InitProxyFuncToInterfaceMap();
}

void NetConnServiceStub::InitQueryFuncToInterfaceMapExt()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_NET_BY_IDENT)] = {
        &NetConnServiceStub::OnGetSpecificNetByIdent, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_NET_EXT_ATTRIBUTE)] = {
        &NetConnServiceStub::OnSetNetExtAttribute, {Permission::SET_NET_EXT_ATTRIBUTE}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_NET_EXT_ATTRIBUTE)] = {
        &NetConnServiceStub::OnGetNetExtAttribute, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_QUERY_TRACEROUTE)] = {
        &NetConnServiceStub::OnQueryTraceRoute,
        {Permission::INTERNET, Permission::GET_NETWORK_LOCATION, Permission::ACCESS_NET_TRACE_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_QUERY_TRACEROUTE_JS)] = {
        &NetConnServiceStub::OnQueryTraceRoute, {Permission::INTERNET, Permission::GET_NETWORK_LOCATION,
        Permission::GET_NETWORK_APPROXIMATE_LOCATION, Permission::ACCESS_NET_TRACE_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_IP_NEIGH_TABLE)] = {
        &NetConnServiceStub::OnGetIpNeighTable, {Permission::GET_NETWORK_INFO, Permission::GET_IP_MAC_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_CREATE_VLAN)] = {
        &NetConnServiceStub::OnCreateVlan, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DESTROY_VLAN)] = {
        &NetConnServiceStub::OnDestroyVlan, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ADD_VLAN_IP)] = {
        &NetConnServiceStub::OnAddVlanIp, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DELETE_VLAN_IP)] = {
        &NetConnServiceStub::OnDeleteVlanIp, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_CONNECT_OWNER_UID)] = {
        &NetConnServiceStub::OnGetConnectOwnerUid, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DEAD_FLOW_RESET_TARGET_BUNDLE)] = {
        &NetConnServiceStub::OnIsDeadFlowResetTargetBundle, {Permission::INTERNET}};
}

void NetConnServiceStub::InitVnicFuncToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ENABLE_VNIC_NET_WORK)] = {
        &NetConnServiceStub::OnEnableVnicNetwork, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DISABLE_VNIC_NET_WORK)] = {
        &NetConnServiceStub::OnDisableVnicNetwork, {Permission::CONNECTIVITY_INTERNAL}};
}

void NetConnServiceStub::InitVirnicFuncToInterfaceMap()
{
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ENABLE_DISTRIBUTE_CLIENT_NET)] = {
        &NetConnServiceStub::OnEnableDistributedClientNet, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ENABLE_DISTRIBUTE_SERVER_NET)] = {
        &NetConnServiceStub::OnEnableDistributedServerNet, {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DISABLE_DISTRIBUTE_NET)] = {
        &NetConnServiceStub::OnDisableDistributedNet, {Permission::CONNECTIVITY_INTERNAL}};
}

NetConnServiceStub::~NetConnServiceStub() {}

int32_t NetConnServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                            MessageOption &option)
{
    NETMGR_LOG_D("stub call start, code = [%{public}d]", code);

    std::u16string myDescripter = NetConnServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        NETMGR_LOG_E("descriptor checked fail.");
        if (!reply.WriteInt32(NETMANAGER_ERR_DESCRIPTOR_MISMATCH)) {
            return IPC_STUB_WRITE_PARCEL_ERR;
        }
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc == memberFuncMap_.end()) {
        NETMGR_LOG_E("memberFuncMap not found this code! code: [%{public}d]", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    auto requestFunc = itFunc->second.first;
    if (requestFunc == nullptr) {
        NETMGR_LOG_E("requestFunc is nullptr. code:[%{public}d]", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    if (code == static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_INTERNET_PERMISSION)) {
        // get uid should be called in this function
        auto uid = IPCSkeleton::GetCallingUid();
        if (uid != UID_FOUNDATION && !CheckPermission({Permission::CONNECTIVITY_INTERNAL})) {
            if (!reply.WriteInt32(NETMANAGER_ERR_PERMISSION_DENIED)) {
                return IPC_STUB_WRITE_PARCEL_ERR;
            }
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }

    int32_t ret = OnRequestCheck(code, itFunc->second.second);
    if (ret == NETMANAGER_SUCCESS) {
        ret =(this->*requestFunc)(data, reply);
        NETMGR_LOG_D("stub call end, code = [%{public}d]", code);
        return ret;
    }
    if (!reply.WriteInt32(ret)) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    NETMGR_LOG_D("stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetConnServiceStub::OnRequestCheck(uint32_t code, const std::set<std::string> &permissions)
{
    if (std::find(SYSTEM_CODE.begin(), SYSTEM_CODE.end(), code) != SYSTEM_CODE.end()) {
        if (!NetManagerPermission::IsSystemCaller()) {
            NETMGR_LOG_E("Non-system applications use system APIs.");
            return NETMANAGER_ERR_NOT_SYSTEM_CALL;
        }
    }

    if (std::find(PERMISSION_NEED_CACHE_CODES.begin(), PERMISSION_NEED_CACHE_CODES.end(), code) !=
        PERMISSION_NEED_CACHE_CODES.end()) {
        if (CheckPermissionWithCache(permissions)) {
            return NETMANAGER_SUCCESS;
        }
    } else {
        if (CheckPermission(permissions)) {
            return NETMANAGER_SUCCESS;
        }
    }
    return NETMANAGER_ERR_PERMISSION_DENIED;
}

bool NetConnServiceStub::CheckPermission(const std::set<std::string> &permissions)
{
    for (const auto &permission : permissions) {
        if (!NetManagerPermission::CheckPermission(permission)) {
            return false;
        }
    }
    return true;
}

bool NetConnServiceStub::CheckPermissionWithCache(const std::set<std::string> &permissions)
{
    for (const auto &permission : permissions) {
        if (!NetManagerPermission::CheckPermissionWithCache(permission)) {
            return false;
        }
    }
    return true;
}

int32_t NetConnServiceStub::OnSystemReady(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = SystemReady();
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetInternetPermission(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid;
    if (!data.ReadUint32(uid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint8_t allow;
    if (!data.ReadUint8(allow)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = SetInternetPermission(uid, allow);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnEnableVnicNetwork(MessageParcel &data, MessageParcel &reply)
{
    std::set<int32_t> uids;
    int32_t size = 0;
    int32_t uid = 0;
    if (!data.ReadInt32(size)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (size < 0 || size > MAX_VNIC_UID_ARRAY_SIZE) {
        NETMGR_LOG_E("vnic uids size is invalid");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    for (int32_t index = 0; index < size; index++) {
        if (!data.ReadInt32(uid)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        uids.insert(uid);
    }

    sptr<NetLinkInfo> netLinkInfo = NetLinkInfo::Unmarshalling(data);
    if (netLinkInfo == nullptr) {
        NETMGR_LOG_E("netLinkInfo ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t ret = EnableVnicNetwork(netLinkInfo, uids);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

// LCOV_EXCL_START
int32_t NetConnServiceStub::OnDisableVnicNetwork(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = DisableVnicNetwork();
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
int32_t NetConnServiceStub::OnEnableDistributedClientNet(MessageParcel &data, MessageParcel &reply)
{
    std::string virnicAddr = "";
    if (!data.ReadString(virnicAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    std::string virnicName = "";
    if (!data.ReadString(virnicName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    std::string iif = "";
    if (!data.ReadString(iif)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = EnableDistributedClientNet(virnicAddr, virnicName, iif);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}
// LCOV_EXCL_STOP

int32_t NetConnServiceStub::OnEnableDistributedServerNet(MessageParcel &data, MessageParcel &reply)
{
    std::string iif = "";
    if (!data.ReadString(iif)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    std::string devIface = "";
    if (!data.ReadString(devIface)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    std::string dstAddr = "";
    if (!data.ReadString(dstAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    std::string gw = "";
    if (!data.ReadString(gw)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

// LCOV_EXCL_START
int32_t NetConnServiceStub::OnDisableDistributedNet(MessageParcel &data, MessageParcel &reply)
{
    bool isServer = false;
    if (!data.ReadBool(isServer)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string virnicName = "";
    if (!data.ReadString(virnicName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string dstAddr = "";
    if (!data.ReadString(dstAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = DisableDistributedNet(isServer, virnicName, dstAddr);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}
// LCOV_EXCL_STOP

int32_t NetConnServiceStub::OnRegisterNetSupplier(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub processing");
    NetBearType bearerType;
    std::string ident;
    std::set<NetCap> netCaps;

    uint32_t type = 0;
    if (!data.ReadUint32(type)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (type > static_cast<uint32_t>(NetBearType::BEARER_DEFAULT)) {
        return NETMANAGER_ERR_INTERNAL;
    }
    bearerType = static_cast<NetBearType>(type);

    if (!data.ReadString(ident)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    uint32_t size = 0;
    uint32_t value = 0;
    if (!data.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
    for (uint32_t i = 0; i < size; ++i) {
        if (!data.ReadUint32(value)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        if (value < NET_CAPABILITY_END) {
            netCaps.insert(static_cast<NetCap>(value));
        }
    }

    uint32_t supplierId = 0;
    int32_t ret = RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("supplierId[%{public}d].", supplierId);
        if (!reply.WriteUint32(supplierId)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUpdateNetCaps(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("On update net caps.");
    std::set<NetCap> netCaps;
    uint32_t netCapsSize = 0;
    uint32_t netCapVal = 0;

    if (!data.ReadUint32(netCapsSize)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (netCapsSize > MAX_NET_CAP_NUM) {
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    for (uint32_t netCapIndex = 0; netCapIndex < netCapsSize; ++netCapIndex) {
        if (!data.ReadUint32(netCapVal)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        if (netCapVal < NET_CAPABILITY_END) {
            netCaps.insert(static_cast<NetCap>(netCapVal));
        }
    }

    uint32_t supplierId = 0;
    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = UpdateNetCaps(netCaps, supplierId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUnregisterNetSupplier(MessageParcel &data, MessageParcel &reply)
{
    uint32_t supplierId = 0;
    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = UnregisterNetSupplier(supplierId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterNetSupplierCallback(MessageParcel &data, MessageParcel &reply)
{
    uint32_t supplierId = 0;
    data.ReadUint32(supplierId);
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetSupplierCallback> callback = iface_cast<INetSupplierCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = RegisterNetSupplierCallback(supplierId, callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterNetConnCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetConnCallback> callback = iface_cast<INetConnCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = RegisterNetConnCallback(callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterNetConnCallbackBySpecifier(MessageParcel &data, MessageParcel &reply)
{
    sptr<NetSpecifier> netSpecifier = NetSpecifier::Unmarshalling(data);
    uint32_t timeoutMS = data.ReadUint32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetConnCallback> callback = iface_cast<INetConnCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = RegisterNetConnCallback(netSpecifier, callback, timeoutMS);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRequestNetConnectionBySpecifier(MessageParcel &data, MessageParcel &reply)
{
    sptr<NetSpecifier> netSpecifier = NetSpecifier::Unmarshalling(data);
    uint32_t timeoutMS = data.ReadUint32();
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetConnCallback> callback = iface_cast<INetConnCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = RequestNetConnection(netSpecifier, callback, timeoutMS);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUnregisterNetConnCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetConnCallback> callback = iface_cast<INetConnCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = UnregisterNetConnCallback(callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUpdateNetStateForTest(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("Test NetConnServiceStub::OnUpdateNetStateForTest(), begin");
    sptr<NetSpecifier> netSpecifier = NetSpecifier::Unmarshalling(data);

    int32_t netState;
    if (!data.ReadInt32(netState)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_D("Test NetConnServiceStub::OnUpdateNetStateForTest(), netState[%{public}d]", netState);
    int32_t result = UpdateNetStateForTest(netSpecifier, netState);
    NETMGR_LOG_D("Test NetConnServiceStub::OnUpdateNetStateForTest(), result[%{public}d]", result);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUpdateNetSupplierInfo(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("OnUpdateNetSupplierInfo in.");
    uint32_t supplierId;
    if (!data.ReadUint32(supplierId)) {
        NETMGR_LOG_D("fail to get supplier id.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_D("OnUpdateNetSupplierInfo supplierId=[%{public}d].", supplierId);
    sptr<NetSupplierInfo> netSupplierInfo = NetSupplierInfo::Unmarshalling(data);
    int32_t ret = UpdateNetSupplierInfo(supplierId, netSupplierInfo);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_D("fail to update net supplier info.");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    NETMGR_LOG_D("OnUpdateNetSupplierInfo out.");

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUpdateNetLinkInfo(MessageParcel &data, MessageParcel &reply)
{
    uint32_t supplierId;

    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    sptr<NetLinkInfo> netLinkInfo = NetLinkInfo::Unmarshalling(data);

    int32_t ret = UpdateNetLinkInfo(supplierId, netLinkInfo);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterNetDetectionCallback(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_E("Execute ContainFileDescriptors failed");
    }
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetDetectionCallback> callback = iface_cast<INetDetectionCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = RegisterNetDetectionCallback(netId, callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUnRegisterNetDetectionCallback(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_E("Execute ContainFileDescriptors failed");
    }
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetDetectionCallback> callback = iface_cast<INetDetectionCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = UnRegisterNetDetectionCallback(netId, callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnNetDetection(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = NetDetection(netId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnNetDetectionResponse(MessageParcel &data, MessageParcel &reply)
{
    std::string rawUrl;
    // LCOV_EXCL_START
    if (!data.ReadString(rawUrl)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    // LCOV_EXCL_STOP
    PortalResponse resp;
    int32_t ret = NetDetection(rawUrl, resp);
    // LCOV_EXCL_START
    if (ret != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteRawData(static_cast<const void *>(&resp), sizeof(PortalResponse))) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetIfaceNames(MessageParcel &data, MessageParcel &reply)
{
    uint32_t netType = 0;
    if (!data.ReadUint32(netType)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (netType > static_cast<uint32_t>(NetBearType::BEARER_DEFAULT)) {
        return NETMANAGER_ERR_INTERNAL;
    }
    NetBearType bearerType = static_cast<NetBearType>(netType);
    std::list<std::string> ifaceNames;
    int32_t ret = GetIfaceNames(bearerType, ifaceNames);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(ifaceNames.size())) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }

        for (const auto &ifaceName : ifaceNames) {
            if (!reply.WriteString(ifaceName)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetIfaceNameByType(MessageParcel &data, MessageParcel &reply)
{
    uint32_t netType = 0;
    if (!data.ReadUint32(netType)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (netType > static_cast<uint32_t>(NetBearType::BEARER_DEFAULT)) {
        return NETMANAGER_ERR_INTERNAL;
    }
    NetBearType bearerType = static_cast<NetBearType>(netType);

    std::string ident;
    if (!data.ReadString(ident)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifaceName;
    int32_t ret = GetIfaceNameByType(bearerType, ident, ifaceName);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteString(ifaceName)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetIfaceNameIdentMaps(MessageParcel &data, MessageParcel &reply)
{
    uint32_t netType = 0;
    if (!data.ReadUint32(netType)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (netType > static_cast<uint32_t>(NetBearType::BEARER_DEFAULT)) {
        return NETMANAGER_ERR_INTERNAL;
    }
    NetBearType bearerType = static_cast<NetBearType>(netType);
    SafeMap<std::string, std::string> ifaceNameIdentMaps;
    int32_t ret = GetIfaceNameIdentMaps(bearerType, ifaceNameIdentMaps);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(ifaceNameIdentMaps.Size())) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        int32_t err = NETMANAGER_SUCCESS;
        ifaceNameIdentMaps.Iterate([&err, &reply](const std::string &k, const std::string &v) -> void {
            if (!reply.WriteString(k) || !reply.WriteString(v)) {
                err = NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        });
        if (err != NETMANAGER_SUCCESS) {
            return err;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetDefaultNet(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("OnGetDefaultNet Begin...");
    int32_t netId;
    int32_t result = GetDefaultNet(netId);
    NETMGR_LOG_D("GetDefaultNet result is: [%{public}d]", result);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(netId)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnHasDefaultNet(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("OnHasDefaultNet Begin...");
    bool flag = false;
    int32_t result = HasDefaultNet(flag);
    NETMGR_LOG_D("HasDefaultNet result is: [%{public}d]", result);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(flag)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetSpecificNet(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type;
    if (!data.ReadUint32(type)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (type > static_cast<uint32_t>(NetBearType::BEARER_DEFAULT)) {
        return NETMANAGER_ERR_INTERNAL;
    }

    NetBearType bearerType = static_cast<NetBearType>(type);

    NETMGR_LOG_D("stub execute GetSpecificNet");
    std::list<int32_t> netIdList;
    int32_t ret = GetSpecificNet(bearerType, netIdList);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = static_cast<uint32_t>(netIdList.size());
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }

        uint32_t index = 0;
        for (auto p = netIdList.begin(); p != netIdList.end(); ++p) {
            if (++index > MAX_IFACE_NUM) {
                break;
            }
            if (!reply.WriteInt32(*p)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetSpecificNetByIdent(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type;
    std::string ident = "";
    if (!data.ReadUint32(type)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (type > static_cast<uint32_t>(NetBearType::BEARER_DEFAULT)) {
        return NETMANAGER_ERR_INTERNAL;
    }

    NetBearType bearerType = static_cast<NetBearType>(type);
    if (!data.ReadString(ident)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_D("stub execute GetSpecificNetByIdent");
    std::list<int32_t> netIdList;
    int32_t ret = GetSpecificNetByIdent(bearerType, ident, netIdList);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = static_cast<uint32_t>(netIdList.size());
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }

        uint32_t index = 0;
        for (auto p = netIdList.begin(); p != netIdList.end(); ++p) {
            if (++index > MAX_IFACE_NUM) {
                break;
            }
            if (!reply.WriteInt32(*p)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetAllNets(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute GetAllNets");
    std::list<int32_t> netIdList;
    int32_t ret = GetAllNets(netIdList);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = static_cast<uint32_t>(netIdList.size());
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }

        for (auto p = netIdList.begin(); p != netIdList.end(); ++p) {
            if (!reply.WriteInt32(*p)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetSpecificUidNet(MessageParcel &data, MessageParcel &reply)
{
    int32_t uid = 0;
    if (!data.ReadInt32(uid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NETMGR_LOG_D("stub execute GetSpecificUidNet");

    int32_t netId = 0;
    int32_t ret = GetSpecificUidNet(uid, netId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteInt32(netId)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetConnectionProperties(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_D("stub execute GetConnectionProperties");
    NetLinkInfo info;
    int32_t ret = GetConnectionProperties(netId, info);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        sptr<NetLinkInfo> netLinkInfo_ptr = sptr<NetLinkInfo>::MakeSptr(info);
        if (!NetLinkInfo::Marshalling(reply, netLinkInfo_ptr)) {
            NETMGR_LOG_E("proxy Marshalling failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetNetCapabilities(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_D("stub execute GetNetCapabilities");

    NetAllCapabilities netAllCap;
    int32_t ret = GetNetCapabilities(netId, netAllCap);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(netAllCap.linkUpBandwidthKbps_) ||
            !reply.WriteUint32(netAllCap.linkDownBandwidthKbps_)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        uint32_t size = netAllCap.netCaps_.size();
        size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        uint32_t index = 0;
        for (auto netCap : netAllCap.netCaps_) {
            if (++index > MAX_NET_CAP_NUM) {
                break;
            }
            if (!reply.WriteUint32(static_cast<uint32_t>(netCap))) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }

        size = netAllCap.bearerTypes_.size();
        size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        index = 0;
        for (auto bearerType : netAllCap.bearerTypes_) {
            if (++index > MAX_NET_CAP_NUM) {
                break;
            }
            if (!reply.WriteUint32(static_cast<uint32_t>(bearerType))) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetAirplaneMode(MessageParcel &data, MessageParcel &reply)
{
    bool state = false;
    if (!data.ReadBool(state)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = SetAirplaneMode(state);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnIsDefaultNetMetered(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute IsDefaultNetMetered");
    bool flag = false;
    int32_t result = IsDefaultNetMetered(flag);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(flag)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetGlobalHttpProxy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute SetGlobalHttpProxy");

    HttpProxy httpProxy;
    if (!HttpProxy::Unmarshalling(data, httpProxy)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (httpProxy.GetUserId() == 0) {
        httpProxy.SetUserId(PRIMARY_USER_ID);
        NETMGR_LOG_I("SetGlobalHttpProxy change userId");
    }

    int32_t ret = SetGlobalHttpProxy(httpProxy);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetGlobalHttpProxy(MessageParcel &data, MessageParcel &reply)
{
    int32_t userId = -1;
    if (!data.ReadInt32(userId)) {
        NETMGR_LOG_E("ReadUserId failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    HttpProxy httpProxy;
    httpProxy.SetUserId(userId);
    int32_t result = GetGlobalHttpProxy(httpProxy);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (result != NETMANAGER_SUCCESS) {
        return result;
    }

    if (!httpProxy.Marshalling(reply)) {
        return ERR_FLATTEN_OBJECT;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetDefaultHttpProxy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnGetDefaultHttpProxy");
    int32_t bindNetId = 0;
    if (!data.ReadInt32(bindNetId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t userId = -1;
    if (!data.ReadInt32(userId)) {
        NETMGR_LOG_E("ReadUserId failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    HttpProxy httpProxy;
    httpProxy.SetUserId(userId);
    int32_t result = GetDefaultHttpProxy(bindNetId, httpProxy);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (result != NETMANAGER_SUCCESS) {
        return result;
    }

    if (!httpProxy.Marshalling(reply)) {
        return ERR_FLATTEN_OBJECT;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRefreshGlobalHttpProxy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnRefreshGlobalHttpProxy");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("OnRefreshGlobalHttpProxy remote is nullptr");
        // LCOV_EXCL_START
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        // LCOV_EXCL_END
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    sptr<IRefreshHttpProxyCallback> callback = iface_cast<IRefreshHttpProxyCallback>(remote);
    // LCOV_EXCL_START
    if (callback == nullptr) {
        NETMGR_LOG_E("OnRefreshGlobalHttpProxy callback is nullptr");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    int32_t result = RefreshGlobalHttpProxy(callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    // LCOV_EXCL_END
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnQueryTraceRoute(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnQueryTraceRoute");
    std::string destination = "";
    int32_t maxJumpNumber = -1;
    int32_t packetsType = -1;
    std::string traceRouteInfo = "";
    if (!data.ReadString(destination)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadInt32(maxJumpNumber)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadInt32(packetsType)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t result = QueryTraceRoute(destination, maxJumpNumber, packetsType, traceRouteInfo, true);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (result != NETMANAGER_SUCCESS) {
        return result;
    }
    if (!reply.WriteString(traceRouteInfo)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetPacUrl(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnGetPacUrl");
    std::string pacUrl = "";
    int32_t ret = GetPacUrl(pacUrl);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    if (!reply.WriteString(pacUrl)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetPacUrl(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnSetPacUrl");
    std::string pacUrl;
    if (!data.ReadString(pacUrl)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = SetPacUrl(pacUrl);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnFindProxyForURL(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnFindProxyForURL");
    std::string url = "";
    if (!data.ReadString(url)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    std::string host = "";
    if (!data.ReadString(host)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    std::string proxy;
    int32_t ret = FindProxyForURL(url, host, proxy);

    int32_t status = 0;
    if (ret != 0) {
        proxy.clear();
        status = NETMANAGER_SUCCESS;
    }
    if (!reply.WriteInt32(status)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteString(proxy)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetPacFileUrl(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnGetPacFileUrl");
    std::string pacUrl = "";
    int32_t ret = GetPacFileUrl(pacUrl);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    if (!reply.WriteString(pacUrl)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetProxyMode(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnGetProxyMode");
    OHOS::NetManagerStandard::ProxyModeType mode;
    int32_t ret = GetProxyMode(mode);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    if (!reply.WriteInt32(mode)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetProxyMode(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnSetProxyMode");
    int32_t mode = -1;
    if (!data.ReadInt32(mode)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    OHOS::NetManagerStandard::ProxyModeType proxyMode;
    switch (mode) {
        case PROXY_MODE_OFF:
            proxyMode = PROXY_MODE_OFF;
            break;
        case PROXY_MODE_AUTO:
            proxyMode = PROXY_MODE_AUTO;
            break;
        default:
            return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    int32_t ret = SetProxyMode(proxyMode);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetPacFileUrl(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnSetPacFileUrl");
    std::string pacUrl;
    if (!data.ReadString(pacUrl)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = SetPacFileUrl(pacUrl);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetNetIdByIdentifier(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnGetNetIdByIdentifier");
    std::string ident;
    if (!data.ReadString(ident)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::list<int32_t> netIdList;
    int32_t ret = GetNetIdByIdentifier(ident, netIdList);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = static_cast<uint32_t>(netIdList.size());
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        for (auto p = netIdList.begin(); p != netIdList.end(); ++p) {
            if (!reply.WriteInt32(*p)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetAppNet(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int ret = SetAppNet(netId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterNetInterfaceCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetInterfaceStateCallback> callback = iface_cast<INetInterfaceStateCallback>(remote);
    int32_t ret = RegisterNetInterfaceCallback(callback);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUnregisterNetInterfaceCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetInterfaceStateCallback> callback = iface_cast<INetInterfaceStateCallback>(remote);
    int32_t ret = UnregisterNetInterfaceCallback(callback);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetNetInterfaceConfiguration(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NetInterfaceConfiguration config;
    int32_t ret = GetNetInterfaceConfiguration(iface, config);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        if (!config.Marshalling(reply)) {
            return ERR_FLATTEN_OBJECT;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetNetInterfaceIpAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ipAddress;
    if (!data.ReadString(ipAddress)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = SetNetInterfaceIpAddress(iface, ipAddress);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetInterfaceUp(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = SetInterfaceUp(iface);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetInterfaceDown(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    if (!data.ReadString(iface)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = SetInterfaceDown(iface);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnAddNetworkRoute(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string destination = "";
    if (!data.ReadString(destination)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string nextHop = "";
    if (!data.ReadString(nextHop)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = AddNetworkRoute(netId, ifName, destination, nextHop);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRemoveNetworkRoute(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string destination = "";
    if (!data.ReadString(destination)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string nextHop = "";
    if (!data.ReadString(nextHop)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = RemoveNetworkRoute(netId, ifName, destination, nextHop);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnAddInterfaceAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t prefixLength = 0;
    if (!data.ReadInt32(prefixLength)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = AddInterfaceAddress(ifName, ipAddr, prefixLength);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnDelInterfaceAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t prefixLength = 0;
    if (!data.ReadInt32(prefixLength)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = DelInterfaceAddress(ifName, ipAddr, prefixLength);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnAddStaticArp(MessageParcel &data, MessageParcel &reply)
{
    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string macAddr = "";
    if (!data.ReadString(macAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = AddStaticArp(ipAddr, macAddr, ifName);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnDelStaticArp(MessageParcel &data, MessageParcel &reply)
{
    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string macAddr = "";
    if (!data.ReadString(macAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = DelStaticArp(ipAddr, macAddr, ifName);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnAddStaticIpv6Addr(MessageParcel &data, MessageParcel &reply)
{
    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string macAddr = "";
    if (!data.ReadString(macAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnDelStaticIpv6Addr(MessageParcel &data, MessageParcel &reply)
{
    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string macAddr = "";
    if (!data.ReadString(macAddr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterSlotType(MessageParcel &data, MessageParcel &reply)
{
    uint32_t supplierId = 0;
    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = RegisterSlotType(supplierId, type);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetSlotType(MessageParcel &data, MessageParcel &reply)
{
    std::string type = "";
    int32_t ret = GetSlotType(type);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteString(type)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnFactoryResetNetwork(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = FactoryResetNetwork();
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterNetFactoryResetCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetFactoryResetCallback> callback = iface_cast<INetFactoryResetCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = RegisterNetFactoryResetCallback(callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}
 
int32_t NetConnServiceStub::OnIsPreferCellularUrl(MessageParcel &data, MessageParcel &reply)
{
    std::string url;
    if (!data.ReadString(url)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    PreferCellularType preferCellular = PreferCellularType::NOT_PREFER;
    int32_t ret = IsPreferCellularUrl(url, preferCellular);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (!reply.WriteInt32(static_cast<int32_t>(preferCellular))) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
 
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetDefaultSupplierId(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type = 0;
    std::string ident = "";
    uint32_t supplierId;
    if (!data.ReadUint32(type)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (type > static_cast<uint32_t>(NetBearType::BEARER_DEFAULT)) {
        return NETMANAGER_ERR_INTERNAL;
    }
    if (!data.ReadString(ident)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NetBearType bearerType = static_cast<NetBearType>(type);
    int32_t ret = GetDefaultSupplierId(bearerType, ident, supplierId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("supplierId[%{public}d].", supplierId);
        if (!reply.WriteUint32(supplierId)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUpdateSupplierScore(MessageParcel &data, MessageParcel &reply)
{
    uint32_t supplierId;
    uint32_t detectionStatus = 0;
    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(detectionStatus)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = UpdateSupplierScore(supplierId, detectionStatus);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterPreAirplaneCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<IPreAirplaneCallback> callback = iface_cast<IPreAirplaneCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = RegisterPreAirplaneCallback(callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUnregisterPreAirplaneCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<IPreAirplaneCallback> callback = iface_cast<IPreAirplaneCallback>(remote);
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        if (!reply.WriteInt32(NETMANAGER_ERR_LOCAL_PTR_NULL)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    int32_t result = UnregisterPreAirplaneCallback(callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnCloseSocketsUid(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId;
    NETMGR_LOG_I("OnCloseSocketsUid");
    if (!data.ReadInt32(netId)) {
        NETMGR_LOG_E("ReadInt32 error.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    uint32_t uid;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("ReadUint32 error.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = CloseSocketsUid(netId, uid);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("reply.WriteInt32 error");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetAppIsFrozened(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("ReadUint32 error.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    bool isFrozened = false;
    if (!data.ReadBool(isFrozened)) {
        NETMGR_LOG_E("ReadBool error.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = SetAppIsFrozened(uid, isFrozened);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("reply.WriteInt32 error");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnEnableAppFrozenedCallbackLimitation(MessageParcel &data, MessageParcel &reply)
{
    bool flag = false;
    if (!data.ReadBool(flag)) {
        NETMGR_LOG_E("ReadBool error.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = EnableAppFrozenedCallbackLimitation(flag);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("reply.WriteInt32 error");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetReuseSupplierId(MessageParcel &data, MessageParcel &reply)
{
    uint32_t supplierId;
    uint32_t reuseSupplierId;
    bool isReused;
    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(reuseSupplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadBool(isReused)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = SetReuseSupplierId(supplierId, reuseSupplierId, isReused);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetNetExtAttribute(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("Enter OnGetNetExtAttribute");
    int32_t netId = 0;
    std::string netExtAttribute = "";
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadString(netExtAttribute)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = GetNetExtAttribute(netId, netExtAttribute);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("get netExtAttribute: [%{private}s]", netExtAttribute.c_str());
        if (!reply.WriteString(netExtAttribute)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetNetExtAttribute(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("Enter OnSetNetExtAttribute");
    int32_t netId = 0;
    std::string netExtAttribute = "";
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadString(netExtAttribute)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = SetNetExtAttribute(netId, netExtAttribute);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetIpNeighTable(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("Enter OnGetIpNeighTable");
    std::vector<NetIpMacInfo> ipMacInfo;
    int32_t ret = GetIpNeighTable(ipMacInfo);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = static_cast<uint32_t>(ipMacInfo.size());
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        uint32_t index = 0;
        for (auto p = ipMacInfo.begin(); p != ipMacInfo.end(); ++p) {
            sptr<NetIpMacInfo> info_ptr = sptr<NetIpMacInfo>::MakeSptr(*p);
            if (!NetIpMacInfo::Marshalling(reply, info_ptr)) {
                NETMGR_LOG_E("proxy Marshalling failed");
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnCreateVlan(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("Enter OnCreateVlan");
    std::string ifName = "";
    uint32_t vlanId = 0;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(vlanId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = CreateVlan(ifName, vlanId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnDestroyVlan(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("Enter OnDestroyVlan");
    std::string ifName = "";
    uint32_t vlanId = 0;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(vlanId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = DestroyVlan(ifName, vlanId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnAddVlanIp(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("Enter OnAddVlanIp");
    std::string ifName = "";
    uint32_t vlanId = 0;
    std::string ip = "";
    uint32_t mask = 0;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(vlanId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadString(ip)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(mask)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = AddVlanIp(ifName, vlanId, ip, mask);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnDeleteVlanIp(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("Enter OnDeleteVlanIp");
    std::string ifName = "";
    uint32_t vlanId = 0;
    std::string ip = "";
    uint32_t mask = 0;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(vlanId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadString(ip)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (!data.ReadUint32(mask)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = DeleteVlanIp(ifName, vlanId, ip, mask);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetConnectOwnerUid(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("Enter OnGetConnectOwnerUid");
    sptr<NetConnInfo> netConnInfo = NetConnInfo::Unmarshalling(data);
    if (netConnInfo == nullptr) {
        NETMGR_LOG_E("netConnInfo is nullptr");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ownerUid = -1;
    int32_t ret = GetConnectOwnerUid(*netConnInfo, ownerUid);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteInt32(ownerUid)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetSystemNetPortStates(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("Enter OnGetSystemNetPortStates");
    NetPortStatesInfo netPortStatesInfo;
    int32_t ret = GetSystemNetPortStates(netPortStatesInfo);
    // LCOV_EXCL_START This will never happen.
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        sptr<NetPortStatesInfo> info_ptr = sptr<NetPortStatesInfo>::MakeSptr(netPortStatesInfo);
        if (!NetPortStatesInfo::Marshalling(reply, info_ptr)) {
            NETMGR_LOG_E("proxy Marshalling failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnIsDeadFlowResetTargetBundle(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName;
    if (!data.ReadString(bundleName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (bundleName.size() > MAX_BUNDLE_NAME_LEN) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    bool flag = false;
    int32_t result = IsDeadFlowResetTargetBundle(bundleName, flag);
    // LCOV_EXCL_START
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(flag)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS

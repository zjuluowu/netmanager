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

#include "conn_manager.h"

#include <linux/if_ether.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <string>
#include "bpf_mapper.h"
#include "bpf_path.h"
#include "local_network.h"
#include "netlink_socket_diag.h"
#include "net_manager_constants.h"
#include "netlink_socket_diag.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "physical_network.h"
#include "virtual_network.h"
#include "securec.h"
#include "bpf_ring_buffer.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
namespace {
constexpr int32_t INTERFACE_UNSET = -1;
constexpr int32_t LOCAL_NET_ID = 99;
constexpr const char *VIRTUAL_IFACE_PREFIX = "tun";
} // namespace

ConnManager::ConnManager()
{
    networks_.EnsureInsert(LOCAL_NET_ID, std::make_shared<LocalNetwork>(LOCAL_NET_ID));
    defaultNetId_ = 0;
    needReinitRouteFlag_ = false;
}

ConnManager::~ConnManager()
{
    networks_.Clear();
}

int32_t ConnManager::SetInternetPermission(uint32_t uid, uint8_t allow, uint8_t isBroker)
{
    // 0 means root
    if (uid == 0) {
        return NETMANAGER_ERROR;
    }
    if (isBroker) {
        BpfMapper<sock_permission_key, sock_permission_value> permissionMap(BROKER_SOCKET_PERMISSION_MAP_PATH,
                                                                            BPF_F_WRONLY);
        if (!permissionMap.IsValid()) {
            return NETMANAGER_ERROR;
        }
        // 0 means no permission
        if (permissionMap.Write(uid, allow, 0) != 0) {
            return NETMANAGER_ERROR;
        }

        return NETMANAGER_SUCCESS;
    }
    BpfMapper<sock_permission_key, sock_permission_value> permissionMap(OH_SOCKET_PERMISSION_MAP_PATH, BPF_F_WRONLY);
    if (!permissionMap.IsValid()) {
        return NETMANAGER_ERROR;
    }
    // 0 means no permission
    if (permissionMap.Write(uid, allow, 0) != 0) {
        return NETMANAGER_ERROR;
    }

    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::CreatePhysicalNetwork(uint16_t netId, NetworkPermission permission)
{
    if (needReinitRouteFlag_) {
        std::set<int32_t> netIds;
        networks_.Iterate([&netIds](int32_t id, std::shared_ptr<NetsysNetwork> &NetsysNetworkPtr) {
            if (id == LOCAL_NET_ID || NetsysNetworkPtr == nullptr) {
                return;
            }
            netIds.insert(NetsysNetworkPtr->GetNetId());
        });

        for (auto netId : netIds) {
            std::string interfaceName;
            {
                std::lock_guard<std::mutex> lock(interfaceNameMutex_);
                interfaceName = physicalInterfaceName_[netId];
            }
            RemoveInterfaceFromNetwork(netId, interfaceName);
            DestroyNetwork(netId);
        }
        needReinitRouteFlag_ = false;
    }
    std::shared_ptr<NetsysNetwork> network = std::make_shared<PhysicalNetwork>(netId, permission);
    networks_.EnsureInsert(netId, network);
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::CreateVirtualNetwork(uint16_t netId, bool hasDns)
{
    networks_.EnsureInsert(netId, std::make_shared<VirtualNetwork>(netId, hasDns));
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::DestroyNetwork(int32_t netId)
{
    if (netId == LOCAL_NET_ID) {
        NETNATIVE_LOGE("Cannot destroy local network");
        return NETMANAGER_ERROR;
    }
    const auto &net = FindNetworkById(netId);
    if (std::get<0>(net)) {
        std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
        if (defaultNetId_ == netId) {
            if (nw->IsPhysical()) {
                static_cast<PhysicalNetwork *>(nw.get())->RemoveDefault();
            }
            defaultNetId_ = 0;
        }
        nw->ClearInterfaces();
    }
    networks_.Erase(netId);
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::SetDefaultNetwork(int32_t netId)
{
    if (defaultNetId_ == netId) {
        return NETMANAGER_SUCCESS;
    }

    // check if this network exists
    const auto &net = FindNetworkById(netId);
    if (std::get<0>(net)) {
        std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
        if (!nw->IsPhysical()) {
            NETNATIVE_LOGE("SetDefaultNetwork fail, network :%{public}d is not physical ", netId);
            return NETMANAGER_ERROR;
        }
        static_cast<PhysicalNetwork *>(nw.get())->AddDefault();
    }

    if (defaultNetId_ != 0) {
        const auto &defaultNet = FindNetworkById(defaultNetId_);
        if (std::get<0>(defaultNet)) {
            std::shared_ptr<NetsysNetwork> nw = std::get<1>(defaultNet);
            if (!nw->IsPhysical()) {
                NETNATIVE_LOGE("SetDefaultNetwork fail, defaultNetId_ :%{public}d is not physical", defaultNetId_);
                return NETMANAGER_ERROR;
            }
            static_cast<PhysicalNetwork *>(nw.get())->RemoveDefault();
        }
    }
    defaultNetId_ = netId;
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::ClearDefaultNetwork()
{
    if (defaultNetId_ != 0) {
        const auto &net = FindNetworkById(defaultNetId_);
        if (std::get<0>(net)) {
            std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
            if (!nw->IsPhysical()) {
                NETNATIVE_LOGE("ClearDefaultNetwork fail, defaultNetId_ :%{public}d is not physical", defaultNetId_);
                return NETMANAGER_ERROR;
            }
            static_cast<PhysicalNetwork *>(nw.get())->RemoveDefault();
        }
    }
    defaultNetId_ = 0;
    return NETMANAGER_SUCCESS;
}

std::tuple<bool, std::shared_ptr<NetsysNetwork>> ConnManager::FindNetworkById(int32_t netId)
{
    NETNATIVE_LOG_D("Entry ConnManager::FindNetworkById netId:%{public}d", netId);
    std::shared_ptr<NetsysNetwork> netsysNetworkPtr;
    bool ret = networks_.Find(netId, netsysNetworkPtr);
    if (ret) {
        return std::make_tuple(true, netsysNetworkPtr);
    }
    return std::make_tuple<bool, std::shared_ptr<NetsysNetwork>>(false, nullptr);
}

int32_t ConnManager::GetDefaultNetwork() const
{
    return defaultNetId_;
}

int32_t ConnManager::GetNetIdByInterface(const std::string &interfaceName)
{
    int32_t interfaceId = INTERFACE_UNSET;
    std::string ifaceName = interfaceName;
    networks_.Iterate([&interfaceId, &ifaceName](int32_t id, std::shared_ptr<NetsysNetwork> &netsysNetworkPtr) {
        if (interfaceId != INTERFACE_UNSET || netsysNetworkPtr == nullptr) {
            return;
        }
        if (netsysNetworkPtr->ExistInterface(ifaceName)) {
            interfaceId = id;
        }
    });
    if (interfaceId == INTERFACE_UNSET) {
        NETNATIVE_LOGE("netId not found, iface=%{public}s", interfaceName.c_str());
    } else {
        NETNATIVE_LOGI("iface=%{public}s, netId=%{public}d", interfaceName.c_str(), interfaceId);
    }
    return interfaceId;
}

int32_t ConnManager::GetNetworkForInterface(int32_t netId, std::string &interfaceName)
{
    NETNATIVE_LOG_D("Entry ConnManager::GetNetworkForInterface interfaceName:%{public}s", interfaceName.c_str());
    std::map<int32_t, std::shared_ptr<NetsysNetwork>>::iterator it;
    int32_t InterfaceId = INTERFACE_UNSET;
    bool isInternalNetId = IsInternalNetId(netId);
    networks_.Iterate([&InterfaceId, &interfaceName, isInternalNetId]
        (int32_t id, std::shared_ptr<NetsysNetwork> &NetsysNetworkPtr) {
        if (IsInternalNetId(id) != isInternalNetId) {
            return;
        }
        if (InterfaceId != INTERFACE_UNSET) {
            return;
        }
        if (NetsysNetworkPtr != nullptr) {
            if (NetsysNetworkPtr->ExistInterface(interfaceName)) {
                InterfaceId = id;
            }
        }
    });
    return InterfaceId;
}

net_interface_name_id ConnManager::GetInterfaceNameId(NetManagerStandard::NetBearType netBearerType)
{
    net_interface_name_id v = {0};
    if (netBearerType == BEARER_WIFI) {
        v = NETWORK_BEARER_TYPE_WIFI;
    } else if (netBearerType == BEARER_CELLULAR) {
        v = NETWORK_BEARER_TYPE_CELLULAR;
    } else {
        v = NETWORK_BEARER_TYPE_INITIAL;
    }
    return v;
}

void ConnManager::AddNetIdAndIfaceToMap(int32_t netId, net_interface_name_id nameId)
{
    // Create Map Table to establish the relationship betweet netId and the id about interfaceName.
    BpfMapper<net_index, net_interface_name_id> netIdAndIfaceMap(NET_INDEX_AND_IFACE_MAP_PATH, BPF_ANY);
    if (netIdAndIfaceMap.IsValid()) {
        if (netIdAndIfaceMap.Write(netId, nameId, 0) != 0) {
            NETNATIVE_LOGE("netIdAndIfaceMap add error: netId:%{public}d, nameId:%{public}d", netId, nameId);
        }
    }
}

void ConnManager::AddIfindexAndNetTypeToMap(const std::string &interfaceName, net_interface_name_id nameId)
{
    BpfMapper<if_index, net_interface_name_id> ifIndexAndNetTypeMap(IFINDEX_AND_NET_TYPE_MAP_PATH, BPF_ANY);
    if (ifIndexAndNetTypeMap.IsValid()) {
        uint32_t ifIndex = if_nametoindex(interfaceName.c_str());
        if (ifIndexAndNetTypeMap.Write(ifIndex, nameId, 0) != 0) {
            NETNATIVE_LOGE("ifIndexAndNetTypeMap add error: interfaceName:%{public}s, ifIndex:%{public}d",
                interfaceName.c_str(), ifIndex);
        }
    }
}

bool ConnManager::IsVirtualInterface(const std::string &interfaceName)
{
    return interfaceName.compare(0, strlen(VIRTUAL_IFACE_PREFIX), VIRTUAL_IFACE_PREFIX) == 0;
}

int32_t ConnManager::AddInterfaceToNetwork(int32_t netId, std::string &interfaceName,
                                           NetManagerStandard::NetBearType netBearerType)
{
    NETNATIVE_LOG_D(
        "Entry ConnManager::AddInterfaceToNetwork netId:%{public}d, interfaceName:%{public}s, netBearerType: "
        "%{public}u",
        netId, interfaceName.c_str(), netBearerType);
    int32_t alreadySetNetId = GetNetworkForInterface(netId, interfaceName);
    if ((alreadySetNetId != netId) && (alreadySetNetId != INTERFACE_UNSET)) {
        NETNATIVE_LOGE("AddInterfaceToNetwork failed alreadySetNetId:%{public}d", alreadySetNetId);
        return NETMANAGER_ERROR;
    }

    const auto &net = FindNetworkById(netId);
    if (std::get<0>(net)) {
        net_interface_name_id nameId = GetInterfaceNameId(netBearerType);
        AddNetIdAndIfaceToMap(netId, nameId);
        AddIfindexAndNetTypeToMap(interfaceName, nameId);
        std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
        if (nw->IsPhysical() && !IsVirtualInterface(interfaceName)) {
            std::lock_guard<std::mutex> lock(interfaceNameMutex_);
            physicalInterfaceName_[netId] = interfaceName;
        }
        return nw->AddInterface(interfaceName);
    }
    return NETMANAGER_ERROR;
}

int32_t ConnManager::RemoveInterfaceFromNetwork(int32_t netId, std::string &interfaceName)
{
    int32_t alreadySetNetId = GetNetworkForInterface(netId, interfaceName);
    if ((alreadySetNetId != netId) || (alreadySetNetId == INTERFACE_UNSET)) {
        return NETMANAGER_SUCCESS;
    } else if (alreadySetNetId == netId) {
        const auto &net = FindNetworkById(netId);
        if (std::get<0>(net)) {
            std::shared_ptr<NetsysNetwork> nw = std::get<1>(net);
            int32_t ret = nw->RemoveInterface(interfaceName);
            if (nw->IsPhysical() && !IsVirtualInterface(interfaceName)) {
                std::lock_guard<std::mutex> lock(interfaceNameMutex_);
                physicalInterfaceName_.erase(netId);
            }

            BpfMapper<net_index, net_interface_name_id> netIdAndIfaceMap(NET_INDEX_AND_IFACE_MAP_PATH, BPF_ANY);
            if (netIdAndIfaceMap.IsValid() && netIdAndIfaceMap.Delete(netId) != 0) {
                NETNATIVE_LOGE("netIdAndIfaceMap remove error: netId:%{public}d, interfaceName:%{public}s", netId,
                               interfaceName.c_str());
            }
            BpfMapper<if_index, net_interface_name_id> ifIndexAndNetTypeMap(IFINDEX_AND_NET_TYPE_MAP_PATH, BPF_ANY);
            uint32_t ifIndex = if_nametoindex(interfaceName.c_str());
            if (ifIndexAndNetTypeMap.IsValid() && ifIndexAndNetTypeMap.Delete(ifIndex) != 0) {
                NETNATIVE_LOGE("ifIndexAndNetTypeMap remove error: ifIndex:%{public}d, interfaceName:%{public}s",
                    ifIndex, interfaceName.c_str());
            }
            return ret;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::ReinitRoute()
{
    NETNATIVE_LOG_D("ConnManager::ReInitRoute");
    needReinitRouteFlag_ = true;
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::AddRoute(int32_t netId, NetworkRouteInfo networkRouteInfo, bool& routeRepeat)
{
    return RouteManager::AddRoute(GetTableType(netId), networkRouteInfo, routeRepeat);
}

int32_t ConnManager::AddRoutes(int32_t netId, const std::vector<NetworkRouteInfo> &infos)
{
    return RouteManager::AddRoutes(GetTableType(netId), infos);
}

int32_t ConnManager::RemoveRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop,
    bool isExcludedRoute)
{
    return RouteManager::RemoveRoute(GetTableType(netId), interfaceName, destination, nextHop, isExcludedRoute);
}

int32_t ConnManager::UpdateRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop)
{
    return RouteManager::UpdateRoute(GetTableType(netId), interfaceName, destination, nextHop);
}

RouteManager::TableType ConnManager::GetTableType(int32_t netId)
{
    if (netId == LOCAL_NET_ID) {
        return RouteManager::LOCAL_NETWORK;
    } else if (FindVirtualNetwork(netId) != nullptr) {
        return RouteManager::VPN_NETWORK;
    } else if (NetManagerStandard::IsInternalNetId(netId)) {
        return RouteManager::INTERNAL_DEFAULT;
    } else {
        return RouteManager::INTERFACE;
    }
}

int32_t ConnManager::GetFwmarkForNetwork(int32_t netId)
{
    return NETMANAGER_ERROR;
}

int32_t ConnManager::SetPermissionForNetwork(int32_t netId, NetworkPermission permission)
{
    return NETMANAGER_ERROR;
}

std::shared_ptr<NetsysNetwork> ConnManager::FindVirtualNetwork(int32_t netId)
{
    if (netId == LOCAL_NET_ID) {
        return nullptr;
    }
    std::shared_ptr<NetsysNetwork> netsysNetworkPtr = nullptr;
    auto ret = networks_.Find(netId, netsysNetworkPtr);
    if (!ret || netsysNetworkPtr == nullptr) {
        NETNATIVE_LOGE("invalid netId:%{public}d or nw is null.", netId);
        return nullptr;
    }
    if (netsysNetworkPtr->IsPhysical()) {
        return nullptr;
    }
    return netsysNetworkPtr;
}

int32_t ConnManager::AddUidsToNetwork(int32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges)
{
    auto netsysNetwork = FindVirtualNetwork(netId);
    if (netsysNetwork == nullptr) {
        NETNATIVE_LOGE("cannot add uids to non-virtual network with netId:%{public}d", netId);
        return NETMANAGER_ERROR;
    }
    return static_cast<VirtualNetwork *>(netsysNetwork.get())->AddUids(uidRanges);
}

int32_t ConnManager::RemoveUidsFromNetwork(int32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges)
{
    auto netsysNetwork = FindVirtualNetwork(netId);
    if (netsysNetwork == nullptr) {
        NETNATIVE_LOGE("cannot remove uids from non-virtual network with netId:%{public}d", netId);
        return NETMANAGER_ERROR;
    }
    return static_cast<VirtualNetwork *>(netsysNetwork.get())->RemoveUids(uidRanges);
}

void ConnManager::GetDumpInfos(std::string &infos)
{
    static const std::string TAB = "  ";
    infos.append("Netsys connect manager :\n");
    infos.append(TAB + "default NetId: " + std::to_string(defaultNetId_) + "\n");
    networks_.Iterate([&infos](int32_t id, std::shared_ptr<NetsysNetwork> &NetsysNetworkPtr) {
        infos.append(TAB + "NetId:" + std::to_string(id));
        std::string interfaces = TAB + "interfaces: {";
        for (const auto &interface : NetsysNetworkPtr->GetAllInterface()) {
            interfaces.append(interface + ", ");
        }
        infos.append(interfaces + "}\n");
    });
}

int32_t ConnManager::SetNetworkAccessPolicy(uint32_t uid, NetManagerStandard::NetworkAccessPolicy policy,
                                            bool reconfirmFlag)
{
    NETNATIVE_LOGI("SetNetworkAccessPolicy Enter");

    BpfMapper<app_uid_key, uid_access_policy_value> uidAccessPolicyMap(APP_UID_PERMISSION_MAP_PATH, BPF_ANY);
    if (!uidAccessPolicyMap.IsValid()) {
        NETNATIVE_LOGE("SetNetworkAccessPolicy uidAccessPolicyMap not exist.");
        return NETMANAGER_ERROR;
    }

    uid_access_policy_value v = {0};
    uid_access_policy_value v2 = {0};
    (void)uidAccessPolicyMap.Read(uid, v);

    v.configSetFromFlag = reconfirmFlag;
    v.diagAckFlag = 0;
    v.wifiPolicy = policy.wifiAllow;
    v.cellularPolicy = policy.cellularAllow;

    if (uidAccessPolicyMap.Write(uid, v, 0) != 0) {
        NETNATIVE_LOGE("SetNetworkAccessPolicy Write uidAccessPolicyMap err");
        return NETMANAGER_ERROR;
    }

    (void)uidAccessPolicyMap.Read(uid, v2);
    NETNATIVE_LOG_D(
        "SetNetworkAccessPolicy Read uid:%{public}u, wifi:%{public}u, cellular:%{public}u, reconfirmFlag:%{public}u",
        uid, v2.wifiPolicy, v2.cellularPolicy, v2.configSetFromFlag);
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::DeleteNetworkAccessPolicy(uint32_t uid)
{
    BpfMapper<app_uid_key, uid_access_policy_value> uidAccessPolicyMap(APP_UID_PERMISSION_MAP_PATH, BPF_ANY);
    if (!uidAccessPolicyMap.IsValid()) {
        NETNATIVE_LOGE("uidAccessPolicyMap not exist");
        return NETMANAGER_ERROR;
    }

    if (uidAccessPolicyMap.Delete(uid) != 0) {
        NETNATIVE_LOGE("DeleteNetworkAccessPolicy err");
        return NETMANAGER_ERROR;
    }

    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::NotifyNetBearerTypeChange(std::set<NetManagerStandard::NetBearType> bearerTypes)
{
    NETNATIVE_LOG_D("NotifyNetBearerTypeChange");
    BpfMapper<net_bear_id_key, net_bear_type_map_value> NetBearerTypeMap(NET_BEAR_TYPE_MAP_PATH, BPF_ANY);
    if (!NetBearerTypeMap.IsValid()) {
        NETNATIVE_LOGE("NetBearerTypeMap not exist");
        return NETMANAGER_ERROR;
    }

    // -1 means invalid
    int32_t netbearerType = -1;
    for (const auto& bearerType : bearerTypes) {
        if (bearerType == BEARER_CELLULAR) {
            netbearerType = NETWORK_BEARER_TYPE_CELLULAR;
        }
        if (bearerType == BEARER_WIFI) {
            netbearerType = NETWORK_BEARER_TYPE_WIFI;
        }
    }
    NETNATIVE_LOGI("NotifyNetBearerTypeChange Type: %{public}d", static_cast<int32_t>(netbearerType));

    net_bear_type_map_value v = 0;
    int32_t ret = NetBearerTypeMap.Read(0, v);

    net_bear_id_key key = DEFAULT_NETWORK_BEARER_MAP_KEY;
    // -1 means current bearer independent network access.
    if (netbearerType != -1 &&
        (((ret == NETSYS_SUCCESS) && (static_cast<int32_t>(v) != netbearerType)) || (ret != NETSYS_SUCCESS))) {
        v = netbearerType;
        if (NetBearerTypeMap.Write(key, v, 0) != 0) {
            NETNATIVE_LOGE("Could not update NetBearerTypeMap");
            return NETMANAGER_ERROR;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::GetSystemNetPortStates(NetManagerStandard::NetPortStatesInfo &netPortStatesInfo)
{
    NetLinkSocketDiag socketDiag;
    int32_t ret = socketDiag.GetSystemNetPortStates(netPortStatesInfo);
    return ret;
}

int ConnManager::CloseSocketsUid(const std::string &ipAddr, uint32_t uid)
{
    NetLinkSocketDiag socketDiag;
    socketDiag.DestroyLiveSocketsWithUid(ipAddr, uid);
    return NETMANAGER_SUCCESS;
}

int32_t ConnManager::GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo, int32_t &ownerUid)
{
    NetLinkSocketDiag socketDiag;
    uint8_t family = (netConnInfo.family_ == NetConnInfo::Family::IPv4) ? AF_INET : AF_INET6;
    return socketDiag.GetConnectOwnerUid(netConnInfo.protocolType_, family, netConnInfo.localAddress_,
                                         netConnInfo.localPort_, netConnInfo.remoteAddress_, netConnInfo.remotePort_,
                                         ownerUid);
}

#ifdef SUPPORT_SYSVPN
int32_t ConnManager::UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add)
{
    auto netsysNetwork = FindVirtualNetwork(netId);
    if (netsysNetwork == nullptr) {
        NETNATIVE_LOGE("cannot add uids to non-virtual network with netId:%{public}d", netId);
        return NETMANAGER_ERROR;
    }
    return static_cast<VirtualNetwork *>(netsysNetwork.get())->UpdateVpnRules(extMessages, add);
}
#endif  // SUPPORT_SYSVPN

} // namespace nmd
} // namespace OHOS

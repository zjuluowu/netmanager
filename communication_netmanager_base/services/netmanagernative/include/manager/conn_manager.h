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

#ifndef INCLUDE_CONN_MANAGER_H
#define INCLUDE_CONN_MANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sys/types.h>
#include <vector>
#include <thread>

#include "bpf_def.h"
#include "netsys_network.h"
#include "network_permission.h"
#include "route_manager.h"
#include "safe_map.h"
#include "netsys_access_policy.h"
#include "net_all_capabilities.h"
#include "net_conn_info.h"
#include "net_port_states_info.h"

namespace OHOS {
namespace nmd {
class ConnManager {
public:
    enum RouteAction {
        ROUTE_ADD,
        ROUTE_REMOVE,
        ROUTE_UPDATE,
    };

    ConnManager();
    ~ConnManager();

    /**
     * Disallow or allow a app to create AF_INET or AF_INET6 socket
     *
     * @param uid App's uid which need to be disallowed ot allowed to create AF_INET or AF_INET6 socket
     * @param allow 0 means disallow, 1 means allow
     * @return return 0 if OK, return error number if not OK
     */
    int32_t SetInternetPermission(uint32_t uid, uint8_t allow, uint8_t isBroker);

    /**
     * Creates a physical network
     *
     * @param netId The network Id to create
     * @param permission The permission necessary to use the network. Must be one of
     *        PERMISSION_NONE/PERMISSION_NETWORK/PERMISSION_SYSTEM
     *
     * @return Returns 0, successfully create the physical network, otherwise it will fail
     */
    int32_t CreatePhysicalNetwork(uint16_t netId, NetworkPermission permission);

    /**
     * Creates a virtual network
     *
     * @param netId The network Id to create
     * @param hasDns true if this network set dns
     * @param secure true if set bypass=false
     *
     * @return Returns 0, successfully create the physical network, otherwise it will fail
     */
    int32_t CreateVirtualNetwork(uint16_t netId, bool hasDns);

    /**
     * Destroy a network. Any interfaces added to the network are removed, and the network ceases
     *        to be the default network
     *
     * @param netId The network to destroy
     *
     * @return Returns 0, successfully destroy the network, otherwise it will fail
     */
    int32_t DestroyNetwork(int32_t netId);

    /**
     * Set network as default network
     *
     * @param netId The network to set as the default
     *
     * @return Returns 0, successfully Set default network, otherwise it will fail
     */
    int32_t SetDefaultNetwork(int32_t netId);

    /**
     * Clear default network
     *
     * @return Returns 0, successfully clear default network, otherwise it will fail
     */
    int32_t ClearDefaultNetwork();

    /**
     * Get default network
     *
     * @return NetId of default network
     */
    int32_t GetDefaultNetwork() const;

    int32_t GetNetIdByInterface(const std::string &interfaceName);

    /**
     * Add an interface to a network. The interface must not be assigned to any network, including
     *        the specified network
     *
     * @param netId The network to add the interface
     * @param interafceName The name of the interface to add
     *
     * @return Returns 0, successfully add an interface to a network, otherwise it will fail
     */
    int32_t AddInterfaceToNetwork(int32_t netId, std::string &interafceName,
                                  NetManagerStandard::NetBearType netBearerType = NetManagerStandard::BEARER_DEFAULT);

    /**
     * Remove an interface to a network. The interface must be assigned to the specified network
     *
     * @param netId The network to add the interface
     * @param interafceName The name of the interface to remove
     *
     * @return Returns 0, successfully remove an interface to a network, otherwise it will fail
     */
    int32_t RemoveInterfaceFromNetwork(int32_t netId, std::string &interafceName);

    /**
     * Reinit route when netmanager restart
     *
     * @param
     *
     * @return Returns 0, reinit route successfully, otherwise it will fail
     */
    int32_t ReinitRoute();

    /**
     * Add a route for specific network
     *
     * @param netId The network to add the route
     * @param interfaceName The name of interface of the route
     *                      This interface should be assigned to the netID
     * @param destination The destination of the route
     * @param nextHop The route's next hop address
     *
     * @return Returns 0, successfully add a route for specific network, otherwise it will fail
     */
    int32_t AddRoute(int32_t netId, NetworkRouteInfo networkRouteInfo, bool& routeRepeat);

    /**
     * Add multiple routes for specific network in batch
     *
     * @param netId The network to add the routes
     * @param infos List of route info (each element contains interface/destination/nextHop etc.)
     *              Each interface in route info should be assigned to the netID
     *
     * @return Returns 0, successfully add routes for specific network, otherwise it will fail
     */
    int32_t AddRoutes(int32_t netId, const std::vector<NetworkRouteInfo> &infos);

    /**
     * Remove a route for specific network
     *
     * @param netId The network to remove the route
     * @param interfaceName The name of interface of the route
     *                      This interface should be assigned to the netID
     * @param destination The destination of the route
     * @param nextHop The route's next hop address
     * @param isExcludedRoute Is excluded route
     *
     * @return Returns 0, successfully remove a route for specific network, otherwise it will fail
     */
    int32_t RemoveRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop,
        bool isExcludedRoute = false);

    /**
     * Update a route for specific network
     *
     * @param netId The network to update the route
     * @param interfaceName The name of interface of the route
     *                      This interface should be assigned to the netID
     * @param destination The destination of the route
     * @param nextHop The route's next hop address
     *
     * @return Returns 0, successfully update a route for specific network, otherwise it will fail
     */
    int32_t UpdateRoute(int32_t netId, std::string interfaceName, std::string destination, std::string nextHop);

    /**
     * Get the mark for the given network id
     *
     * @param netId The network to get the mark
     *
     * @return A Mark of the given network id.
     */
    int32_t GetFwmarkForNetwork(int32_t netId);

    /**
     * Set the permission required to access a specific network
     *
     * @param netId The network to set
     * @param permission Network permission to use
     *
     * @return Returns 0, successfully set the permission for specific network, otherwise it will fail
     */
    int32_t SetPermissionForNetwork(int32_t netId, NetworkPermission permission);

    /**
     * Find virtual network from netId
     *
     * @param netId The network id
     * @return Returns nullptr, the netId is not virtual network
     */
    std::shared_ptr<NetsysNetwork> FindVirtualNetwork(int32_t netId);

    /**
     * Add uids to virtual network
     *
     * @param netId The virtual network id
     * @param uidRanges App uids to set
     *
     * @return Returns 0, successfully set the uids for specific network, otherwise it will fail
     */
    int32_t AddUidsToNetwork(int32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges);

    /**
     * Remove uids from virtual network
     *
     * @param netId The virtual network id
     * @param uidRanges App uids to set
     *
     * @return Returns 0, successfully remove the uids for specific network, otherwise it will fail
     */
    int32_t RemoveUidsFromNetwork(int32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges);

    /**
     * Get the Dump Infos object
     *
     * @param infos The output message
     */
    void GetDumpInfos(std::string &infos);

    /**
     * Set the policy to access the network of the specified application.
     *
     * @param uid - The specified UID of application.
     * @param policy - the network access policy of application. For details, see {@link NetworkAccessPolicy}.
     * @return Returns 0, successfully set the network access policy for application, otherwise it will fail
     */
    int32_t SetNetworkAccessPolicy(uint32_t uid, NetManagerStandard::NetworkAccessPolicy policy, bool reconfirmFlag);
    int32_t DeleteNetworkAccessPolicy(uint32_t uid);
    int32_t NotifyNetBearerTypeChange(std::set<NetManagerStandard::NetBearType> bearerTypes);
    int32_t CloseSocketsUid(const std::string &ipAddr, uint32_t uid);
    int32_t GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo, int32_t &ownerUid);
    int32_t GetSystemNetPortStates(NetManagerStandard::NetPortStatesInfo &netPortStatesInfo);
#ifdef SUPPORT_SYSVPN
    /**
     * update vpn interface rules
     *
     * @param netId Network number
     * @param extMessages ext message
     * @param add true add, false remove
     * @return Returns 0, add network ip mark successfully, otherwise it will fail
     */
    int32_t UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add);
#endif // SUPPORT_SYSVPN
private:
    bool IsVirtualInterface(const std::string &interfaceName);
private:
    int32_t defaultNetId_;
    bool needReinitRouteFlag_;
    std::map<int32_t, std::string> physicalInterfaceName_;
    SafeMap<int32_t, std::shared_ptr<NetsysNetwork>> networks_;
    std::mutex interfaceNameMutex_;
    std::tuple<bool, std::shared_ptr<NetsysNetwork>> FindNetworkById(int32_t netId);
    int32_t GetNetworkForInterface(int32_t netId, std::string &interfaceName);
    RouteManager::TableType GetTableType(int32_t netId);
    net_interface_name_id GetInterfaceNameId(NetManagerStandard::NetBearType netBearerType);
    void AddNetIdAndIfaceToMap(int32_t netId, net_interface_name_id nameId);
    void AddIfindexAndNetTypeToMap(const std::string &interfaceName, net_interface_name_id nameId);
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_CONN_MANAGER_H

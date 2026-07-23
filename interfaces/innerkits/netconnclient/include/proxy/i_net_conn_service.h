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

#ifndef I_NET_CONN_SERVICE_H
#define I_NET_CONN_SERVICE_H

#include <string>

#include "iremote_broker.h"

#include "http_proxy.h"
#include "i_net_conn_callback.h"
#include "i_refresh_http_proxy_callback.h"
#include "i_net_detection_callback.h"
#include "i_net_interface_callback.h"
#include "i_net_supplier_callback.h"
#include "i_net_factoryreset_callback.h"

#include "net_conn_constants.h"
#include "net_conn_info.h"
#include "net_interface_config.h"
#include "net_link_info.h"
#include "net_specifier.h"
#include "net_supplier_info.h"
#include "conn_ipc_interface_code.h"
#include "safe_map.h"
#include "net_ip_mac_info.h"
#include "net_port_states_info.h"

namespace OHOS {
namespace NetManagerStandard {
class INetConnService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetConnService");

public:
    virtual int32_t SystemReady() = 0;
    virtual int32_t SetInternetPermission(uint32_t uid, uint8_t allow) = 0;
    virtual int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                        const std::set<NetCap> &netCaps, uint32_t &supplierId) = 0;
    virtual int32_t UnregisterNetSupplier(uint32_t supplierId) = 0;
    virtual int32_t RegisterNetSupplierCallback(uint32_t supplierId, const sptr<INetSupplierCallback> &callback) = 0;
    virtual int32_t RegisterNetConnCallback(const sptr<INetConnCallback> callback) = 0;
    virtual int32_t RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier,
                                            const sptr<INetConnCallback> callback, const uint32_t &timeoutMS) = 0;
    virtual int32_t RequestNetConnection(const sptr<NetSpecifier> netSpecifier,
                                         const sptr<INetConnCallback> callback, const uint32_t timeoutMS) = 0;
    virtual int32_t UnregisterNetConnCallback(const sptr<INetConnCallback> &callback) = 0;
    virtual int32_t UpdateNetCaps(const std::set<NetCap> &netCaps, const uint32_t supplierId) = 0;
    virtual int32_t UpdateNetStateForTest(const sptr<NetSpecifier> &netSpecifier, int32_t netState) = 0;
    virtual int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo) = 0;
    virtual int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo) = 0;
    virtual int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames) = 0;
    virtual int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName) = 0;
    virtual int32_t GetIfaceNameIdentMaps(NetBearType bearerType,
                                          SafeMap<std::string, std::string> &ifaceNameIdentMaps) = 0;
    virtual int32_t RegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback) = 0;
    virtual int32_t UnRegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback) = 0;
    virtual int32_t NetDetection(int32_t netId) = 0;
    virtual int32_t NetDetection(const std::string &rawUrl, PortalResponse &resp) = 0;
    virtual int32_t GetDefaultNet(int32_t &netId) = 0;
    virtual int32_t HasDefaultNet(bool &flag) = 0;
    virtual int32_t GetSpecificNet(NetBearType bearerType, std::list<int32_t> &netIdList) = 0;
    virtual int32_t GetSpecificNetByIdent(NetBearType bearerType, const std::string &ident,
                                          std::list<int32_t> &netIdList) = 0;
    virtual int32_t GetAllNets(std::list<int32_t> &netIdList) = 0;
    virtual int32_t GetSpecificUidNet(int32_t uid, int32_t &netId) = 0;
    virtual int32_t GetConnectionProperties(int32_t netId, NetLinkInfo &info) = 0;
    virtual int32_t GetNetCapabilities(int32_t netId, NetAllCapabilities &netAllCap) = 0;
    virtual int32_t SetAirplaneMode(bool state) = 0;
    virtual int32_t IsDefaultNetMetered(bool &isMetered) = 0;
    virtual int32_t SetGlobalHttpProxy(const HttpProxy &httpProxy) = 0;
    virtual int32_t GetGlobalHttpProxy(HttpProxy &httpProxy) = 0;
    virtual int32_t GetDefaultHttpProxy(int32_t bindNetId, HttpProxy &httpProxy) = 0;
    virtual int32_t RefreshGlobalHttpProxy(const sptr<IRefreshHttpProxyCallback> &callback) = 0;
    virtual int32_t GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList) = 0;
    virtual int32_t SetAppNet(int32_t netId) = 0;
    virtual int32_t RegisterNetInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback) = 0;
    virtual int32_t UnregisterNetInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback) = 0;
    virtual int32_t GetNetInterfaceConfiguration(const std::string &iface, NetInterfaceConfiguration &config) = 0;
    virtual int32_t SetNetInterfaceIpAddress(const std::string &iface, const std::string &ipAddress) = 0;
    virtual int32_t SetInterfaceUp(const std::string &iface) = 0;
    virtual int32_t SetInterfaceDown(const std::string &iface) = 0;
    virtual int32_t AddNetworkRoute(int32_t netId, const std::string &ifName,
                                    const std::string &destination, const std::string &nextHop) = 0;
    virtual int32_t RemoveNetworkRoute(int32_t netId, const std::string &ifName,
                                       const std::string &destination, const std::string &nextHop) = 0;
    virtual int32_t AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                        int32_t prefixLength) = 0;
    virtual int32_t DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                        int32_t prefixLength) = 0;
    virtual int32_t AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                 const std::string &ifName) = 0;
    virtual int32_t DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                 const std::string &ifName) = 0;
    virtual int32_t RegisterSlotType(uint32_t supplierId, int32_t type) = 0;
    virtual int32_t GetSlotType(std::string &type) = 0;

    virtual int32_t FactoryResetNetwork() = 0;
    virtual int32_t RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback) = 0;
    virtual int32_t IsPreferCellularUrl(const std::string& url, PreferCellularType& preferCellular) = 0;
    virtual int32_t RegisterPreAirplaneCallback(const sptr<IPreAirplaneCallback> callback) = 0;
    virtual int32_t UnregisterPreAirplaneCallback(const sptr<IPreAirplaneCallback> callback) = 0;
    virtual int32_t GetDefaultSupplierId(NetBearType bearerType, const std::string &ident,
        uint32_t& supplierId) = 0;
    virtual int32_t UpdateSupplierScore(uint32_t supplierId, uint32_t detectionStatus) = 0;
    virtual int32_t EnableVnicNetwork(const sptr<NetLinkInfo> &netLinkInfo, const std::set<int32_t> &uids) = 0;
    virtual int32_t DisableVnicNetwork() = 0;
    virtual int32_t EnableDistributedClientNet(const std::string &virnicAddr,
        const std::string &virnicName, const std::string &iif) = 0;
    virtual int32_t EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                               const std::string &dstAddr, const std::string &gw) = 0;
    virtual int32_t DisableDistributedNet(
        bool isServer, const std::string &virnicName, const std::string &dstAddr) = 0;
    virtual int32_t CloseSocketsUid(int32_t netId, uint32_t uid) = 0;
    virtual int32_t SetPacUrl(const std::string &pacUrl) = 0;
    virtual int32_t GetPacUrl(std::string &pacUrl) = 0;
    virtual int32_t SetPacFileUrl(const std::string &pacUrl) = 0;
    virtual int32_t SetProxyMode(const OHOS::NetManagerStandard::ProxyModeType mode) = 0;
    virtual int32_t GetProxyMode(OHOS::NetManagerStandard::ProxyModeType &mode) = 0;
    virtual int32_t GetPacFileUrl(std::string &pacUrl) = 0;
    virtual int32_t FindProxyForURL(const std::string &url, const std::string &host, std::string &proxy) = 0;
    virtual int32_t QueryTraceRoute(const std::string &destination,
        int32_t maxJumpNumber, int32_t packetsType, std::string &traceRouteInfo, bool isCallerNative) = 0;
    virtual int32_t SetAppIsFrozened(uint32_t uid, bool isFrozened) = 0;
    virtual int32_t IsDeadFlowResetTargetBundle(const std::string &bundleName, bool &flag) = 0;
    virtual int32_t EnableAppFrozenedCallbackLimitation(bool flag) = 0;
    virtual int32_t SetReuseSupplierId(uint32_t supplierId, uint32_t reuseSupplierId, bool isReused) = 0;
    virtual int32_t SetNetExtAttribute(int32_t netId, const std::string &netExtAttribute) = 0;
    virtual int32_t GetNetExtAttribute(int32_t netId, std::string &netExtAttribute) = 0;
    virtual int32_t AddStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName) = 0;
    virtual int32_t DelStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName) = 0;
    virtual int32_t GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo) = 0;
    virtual int32_t CreateVlan(const std::string &ifName, uint32_t vlanId) = 0;
    virtual int32_t DestroyVlan(const std::string &ifName, uint32_t vlanId) = 0;
    virtual int32_t AddVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask) = 0;
    virtual int32_t DeleteVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask) = 0;
    virtual int32_t GetConnectOwnerUid(const NetConnInfo &netConnInfo, int32_t &ownerUid) = 0;
    virtual int32_t GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_NET_CONN_SERVICE_H

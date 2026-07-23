/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef I_NETSYS_SERVICE_H
#define I_NETSYS_SERVICE_H

#include <netdb.h>
#include <string>
#include <set>
#include <unordered_map>

#include "dns_config_client.h"
#include "i_net_diag_callback.h"
#include "i_notify_callback.h"
#include "i_net_dns_result_callback.h"
#include "i_netsys_traffic_callback.h"
#include "interface_type.h"
#include "iremote_broker.h"
#include "net_conn_info.h"
#include "net_stats_info.h"
#include "network_sharing.h"
#include "netsys_ipc_interface_code.h"
#include "route_type.h"
#ifdef FEATURE_NET_FIREWALL_ENABLE
#include "i_netfirewall_callback.h"
#include "netfirewall_parcel.h"
#endif
#include "uid_range.h"
#include "netsys_access_policy.h"
#include "net_all_capabilities.h"
#include "net_ip_mac_info.h"
#include "net_port_states_info.h"

namespace OHOS {
namespace NetsysNative {
using namespace nmd;
using namespace OHOS::NetManagerStandard;
enum IptablesType {
    IPTYPE_NONE = 0,
    IPTYPE_IPV4 = 1,
    IPTYPE_IPV6 = 2,
    IPTYPE_IPV4V6 = 3,
};
enum SysVpnStageCode : int32_t {
    VPN_STAGE_RESTART = 0, // common stage. start charon
    VPN_STAGE_UP_HOME, // common stage. connect "home" configuration
    VPN_STAGE_DOWN_HOME, // common stage. disconnect "home" configuration
    VPN_STAGE_STOP, // common stage. stop charon
    VPN_STAGE_SWANCTL_LOAD, // ikev2 vpn. load ikev2 vpn config file
    VPN_STAGE_L2TP_LOAD, // l2tp vpn. load l2tp vpn config file
    VPN_STAGE_L2TP_CTL, // l2tp vpn. control pppd running
    VPN_STAGE_OPENVPN_RESTART, // openvpn. restart openvpn
    VPN_STAGE_OPENVPN_STOP, // openvpn. stop openvpn
    VPN_STAGE_L2TP_STOP, // close single l2tp connection
    VPN_STAGE_CREATE_PPP_FD, // create ppp fd
    VPN_STAGE_SET_XFRM_PHY_IFNAME, // set xfrm phy ifname
    VPN_STAGE_SET_VPN_CALL_MODE, // set vpn call mode
    VPN_STAGE_SET_VPN_REMOTE_ADDRESS, // set vpn remote ip address
    VPN_STAGE_SET_L2TP_CONF, // set l2tp config
};
class INetsysService : public IRemoteBroker {
public:
    virtual int32_t SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                      const std::vector<std::string> &servers,
                                      const std::vector<std::string> &domains) = 0;
    virtual int32_t GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                      std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                      uint8_t &retryCount) = 0;
    virtual int32_t CreateNetworkCache(uint16_t netId, bool isVpnNet = false) = 0;
    virtual int32_t DestroyNetworkCache(uint16_t netId, bool isVpnNet = false) = 0;
    virtual int32_t GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                                uint16_t netId, std::vector<AddrInfo> &res) = 0;
    virtual int32_t SetInterfaceMtu(const std::string &interfaceName, int mtu) = 0;
    virtual int32_t GetInterfaceMtu(const std::string &interfaceName) = 0;

    virtual int32_t SetTcpBufferSizes(const std::string &tcpBufferSizes) = 0;

    virtual int32_t RegisterNotifyCallback(sptr<INotifyCallback> &callback) = 0;
    virtual int32_t UnRegisterNotifyCallback(sptr<INotifyCallback> &callback) = 0;

    virtual int32_t NetworkAddRoute(int32_t netId, const std::string &interfaceName, const std::string &destination,
                                    const std::string &nextHop, bool isExcludedRoute = false) = 0;
    virtual int32_t NetworkRemoveRoute(int32_t netId, const std::string &interfaceName, const std::string &destination,
                                       const std::string &nextHop, bool isExcludedRoute = false) = 0;
    virtual int32_t NetworkAddRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo) = 0;
    virtual int32_t NetworkRemoveRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo) = 0;
    virtual int32_t NetworkSetDefault(int32_t netId) = 0;
    virtual int32_t NetworkGetDefault() = 0;
    virtual int32_t NetworkClearDefault() = 0;
    virtual int32_t GetProcSysNet(int32_t family, int32_t which, const std::string &ifname,
                                  const std::string &parameter, std::string &value) = 0;
    virtual int32_t SetProcSysNet(int32_t family, int32_t which, const std::string &ifname,
                                  const std::string &parameter, std::string &value) = 0;
    virtual int32_t SetInternetPermission(uint32_t uid, uint8_t allow, uint8_t isBroker) = 0;
    virtual int32_t NetworkCreatePhysical(int32_t netId, int32_t permission) = 0;
    virtual int32_t NetworkCreateVirtual(int32_t netId, bool hasDns) = 0;
    virtual int32_t NetworkAddUids(int32_t netId, const std::vector<UidRange> &uidRanges) = 0;
    virtual int32_t NetworkDelUids(int32_t netId, const std::vector<UidRange> &uidRanges) = 0;
    virtual int32_t AddInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                        int32_t prefixLength) = 0;
    virtual int32_t DelInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                        int32_t prefixLength) = 0;
    virtual int32_t DelInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                        int32_t prefixLength, int socketType) = 0;
    virtual int32_t InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress) = 0;
    virtual int32_t InterfaceSetIffUp(const std::string &ifaceName) = 0;
    virtual int32_t NetworkAddInterface(int32_t netId, const std::string &iface, NetBearType netBearerType) = 0;
    virtual int32_t NetworkRemoveInterface(int32_t netId, const std::string &iface) = 0;
    virtual int32_t NetworkDestroy(int32_t netId, bool isVpnNet = false) = 0;
    virtual int32_t CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix,
                               const std::set<int32_t> &uids) = 0;
    virtual int32_t DestroyVnic() = 0;
    virtual int32_t EnableDistributedClientNet(const std::string &virnicAddr,
        const std::string &virnicName, const std::string &iif) = 0;
    virtual int32_t EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                               const std::string &dstAddr, const std::string &gw) = 0;
    virtual int32_t DisableDistributedNet(
        bool isServer, const std::string &virnicName, const std::string &dstAddr) = 0;
    virtual int32_t GetFwmarkForNetwork(int32_t netId, MarkMaskParcel &markMaskParcel) = 0;
    virtual int32_t SetInterfaceConfig(const InterfaceConfigurationParcel &cfg) = 0;
    virtual int32_t GetInterfaceConfig(InterfaceConfigurationParcel &cfg) = 0;
    virtual int32_t InterfaceGetList(std::vector<std::string> &ifaces) = 0;
    virtual int32_t StartDhcpClient(const std::string &iface, bool bIpv6) = 0;
    virtual int32_t StopDhcpClient(const std::string &iface, bool bIpv6) = 0;
    virtual int32_t StartDhcpService(const std::string &iface, const std::string &ipv4addr) = 0;
    virtual int32_t StopDhcpService(const std::string &iface) = 0;
    virtual int32_t IpEnableForwarding(const std::string &requestor) = 0;
    virtual int32_t IpDisableForwarding(const std::string &requestor) = 0;
    virtual int32_t EnableNat(const std::string &downstreamIface, const std::string &upstreamIface) = 0;
    virtual int32_t DisableNat(const std::string &downstreamIface, const std::string &upstreamIface) = 0;
    virtual int32_t IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface) = 0;
    virtual int32_t IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface) = 0;
    virtual int32_t BandwidthAddAllowedList(uint32_t uid) = 0;
    virtual int32_t BandwidthRemoveAllowedList(uint32_t uid) = 0;
    virtual int32_t BandwidthEnableDataSaver(bool enable) = 0;
    virtual int32_t BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes) = 0;
    virtual int32_t BandwidthAddDeniedList(uint32_t uid) = 0;
    virtual int32_t BandwidthRemoveDeniedList(uint32_t uid) = 0;
    virtual int32_t BandwidthRemoveIfaceQuota(const std::string &ifName) = 0;
    virtual int32_t FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids) = 0;
    virtual int32_t FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids) = 0;
    virtual int32_t FirewallEnableChain(uint32_t chain, bool enable) = 0;
    virtual int32_t FirewallSetUidRule(uint32_t chain, const std::vector<uint32_t> &uids, uint32_t firewallRule) = 0;
    virtual int32_t ShareDnsSet(uint16_t netId) = 0;
    virtual int32_t StartDnsProxyListen() = 0;
    virtual int32_t StopDnsProxyListen() = 0;
    virtual int32_t GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                             NetworkSharingTraffic &traffic) = 0;
    virtual int32_t GetNetworkCellularSharingTraffic(nmd::NetworkSharingTraffic &traffic, std::string &ifaceName) = 0;
    virtual int32_t SetDpaCellularSharingTraffic(NetworkDpaTrafficReport &traffic) = 0;
    virtual int32_t GetTotalStats(uint64_t &stats, uint32_t type) = 0;
    virtual int32_t GetUidStats(uint64_t &stats, uint32_t type, uint32_t uid) = 0;
    virtual int32_t GetIfaceStats(uint64_t &stats, uint32_t type, const std::string &interfaceName) = 0;
    virtual int32_t GetAllStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats) = 0;
    virtual int32_t DeleteStatsInfo(uint32_t uid) = 0;
    virtual int32_t GetAllSimStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats) = 0;
    virtual int32_t DeleteSimStatsInfo(uint32_t uid) = 0;
    virtual int32_t SetNetStateTrafficMap(uint8_t flag, uint64_t availableTraffic) = 0;
    virtual int32_t GetNetStateTrafficMap(uint8_t flag, uint64_t &availableTraffic) = 0;
    virtual int32_t ClearIncreaseTrafficMap() = 0;
    virtual int32_t ClearSimStatsBpfMap() = 0;
    virtual int32_t DeleteIncreaseTrafficMap(uint64_t ifIndex) = 0;
    virtual int32_t UpdateIfIndexMap(int8_t key, uint64_t index) = 0;
    virtual int32_t SetNetStatusMap(uint8_t type, uint8_t value) = 0;
    virtual int32_t SetIptablesCommandForRes(const std::string &cmd, std::string &respond,
                                             IptablesType ipType = IPTYPE_IPV4) = 0;
    virtual int32_t SetIpCommandForRes(const std::string &cmd, std::string &respond) = 0;
    virtual int32_t NetDiagPingHost(const NetDiagPingOption &pingOption, const sptr<INetDiagCallback> &callback) = 0;
    virtual int32_t NetDiagGetRouteTable(std::list<NetDiagRouteTable> &routeTables) = 0;
    virtual int32_t NetDiagGetSocketsInfo(NetDiagProtocolType socketType, NetDiagSocketsInfo &socketsInfo) = 0;
    virtual int32_t NetDiagGetInterfaceConfig(std::list<NetDiagIfaceConfig> &configs, const std::string &ifaceName) = 0;
    virtual int32_t NetDiagUpdateInterfaceConfig(const NetDiagIfaceConfig &config, const std::string &ifaceName,
                                                 bool add) = 0;
    virtual int32_t NetDiagSetInterfaceActiveState(const std::string &ifaceName, bool up) = 0;
    virtual int32_t AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                 const std::string &ifName) = 0;
    virtual int32_t DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                 const std::string &ifName) = 0;
    virtual int32_t AddStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName) = 0;
    virtual int32_t DelStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName) = 0;
    virtual int32_t RegisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback, uint32_t delay) = 0;
    virtual int32_t UnregisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback) = 0;
    virtual int32_t GetCookieStats(uint64_t &stats, uint32_t type, uint64_t cookie) = 0;
    virtual int32_t GetNetworkSharingType(std::set<uint32_t>& sharingTypeIsOn) = 0;
    virtual int32_t UpdateNetworkSharingType(uint32_t type, bool isOpen) = 0;
#ifdef FEATURE_NET_FIREWALL_ENABLE
    virtual int32_t SetFirewallRules(NetFirewallRuleType type, const std::vector<sptr<NetFirewallBaseRule>> &ruleList,
                                     bool isFinish) = 0;
    virtual int32_t SetFirewallDefaultAction(int32_t userId, FirewallRuleAction inDefault,
                                             FirewallRuleAction outDefault) = 0;
    virtual int32_t SetFirewallCurrentUserId(int32_t userId) = 0;
    virtual int32_t ClearFirewallRules(NetFirewallRuleType type) = 0;
    virtual int32_t RegisterNetFirewallCallback(const sptr<INetFirewallCallback> &callback) = 0;
    virtual int32_t UnRegisterNetFirewallCallback(const sptr<INetFirewallCallback> &callback) = 0;
#endif
#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
    virtual int32_t EnableWearableDistributedNetForward(const int32_t tcpPortId, const int32_t udpPortId) = 0;
    virtual int32_t DisableWearableDistributedNetForward() = 0;
#endif
    virtual int32_t RegisterNetsysTrafficCallback(const sptr<INetsysTrafficCallback> &callback) = 0;
    virtual int32_t UnRegisterNetsysTrafficCallback(const sptr<INetsysTrafficCallback> &callback) = 0;
    virtual int32_t SetIpv6PrivacyExtensions(const std::string &interfaceName, const uint32_t on) = 0;
    virtual int32_t SetEnableIpv6(const std::string &interfaceName, const uint32_t on, bool needRestart) = 0;
    virtual int32_t SetIpv6AutoConf(const std::string &interfaceName, const uint32_t on) = 0;
    virtual int32_t SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag) = 0;
    virtual int32_t DeleteNetworkAccessPolicy(uint32_t uid) = 0;
    virtual int32_t NotifyNetBearerTypeChange(std::set<NetBearType> bearerTypes) = 0;
    virtual int32_t StartClat(const std::string &interfaceName, int32_t netId, const std::string &nat64PrefixStr) = 0;
    virtual int32_t StopClat(const std::string &interfaceName) = 0;
    virtual int32_t ClearFirewallAllRules() = 0;
    virtual int32_t SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status) = 0;
    virtual int32_t CloseSocketsUid(const std::string &ipAddr, uint32_t uid) = 0;
#ifdef SUPPORT_SYSVPN
    virtual int32_t ProcessVpnStage(NetsysNative::SysVpnStageCode stage, const std::string &message = "") = 0;
    virtual int32_t UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add) = 0;
#endif // SUPPORT_SYSVPN
    virtual int32_t SetBrokerUidAccessPolicyMap(const std::unordered_map<uint32_t, uint32_t> &uidMaps) = 0;
    virtual int32_t DelBrokerUidAccessPolicyMap(uint32_t uid) = 0;
    virtual int32_t SetUserDefinedServerFlag(uint16_t netId, bool flag) = 0;
    virtual int32_t FlushDnsCache(uint16_t netId) = 0;
    virtual int32_t SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo) = 0;
#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
    virtual int32_t UpdateEnterpriseRoute(const std::string &interfaceName, uint32_t uid, bool add) = 0;
#endif
    virtual int32_t GetIpNeighTable(std::vector<NetManagerStandard::NetIpMacInfo> &ipMacInfo) = 0;
    virtual int32_t CreateVlan(const std::string &ifName, uint32_t vlanId) = 0;
    virtual int32_t DestroyVlan(const std::string &ifName, uint32_t vlanId) = 0;
    virtual int32_t AddVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask) = 0;
    virtual int32_t SetInternetAccessByIpForWifiShare(
        const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName) = 0;
    virtual int32_t GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo,
                                       int32_t &ownerUid) = 0;
    virtual int32_t GetSystemNetPortStates(NetManagerStandard::NetPortStatesInfo &netPortStatesInfo) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetsysNative.INetsysService")
    virtual int32_t NetworkAddRoutes(int32_t netId, const std::vector<nmd::NetworkRouteInfo> &infos) = 0;
    virtual int32_t SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid) = 0;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // I_NETSYS_SERVICE_H

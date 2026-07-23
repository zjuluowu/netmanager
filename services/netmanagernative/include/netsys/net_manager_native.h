/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_NET_MANAGER_NATIVE_H
#define INCLUDE_NET_MANAGER_NATIVE_H

#include <memory>
#include <string>
#include <vector>

#include "ffrt.h"
#include "bandwidth_manager.h"
#include "conn_manager.h"
#include "dns_manager.h"
#include "firewall_manager.h"
#include "interface_manager.h"
#include "interface_type.h"
#include "mptcp_manager.h"
#include "net_conn_info.h"
#include "route_manager.h"
#include "vnic_manager.h"
#include "route_type.h"
#include "sharing_manager.h"
#include "uid_range.h"
#include "net_all_capabilities.h"
#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
#include "wearable_distributed_net_manager.h"
#endif

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;
using namespace OHOS::NetsysNative;
class NetManagerNative {
public:
    NetManagerNative();
    ~NetManagerNative() = default;

    static void GetOriginInterfaceIndex();
    static std::vector<uint32_t> GetCurrentInterfaceIndex();
    static void UpdateInterfaceIndex(uint32_t infIndex);

    void Init();

    int32_t NetworkReinitRoute();
    int32_t SetInternetPermission(uint32_t uid, uint8_t allow, uint8_t isBroker);
    int32_t NetworkCreatePhysical(int32_t netId, int32_t permission);
    int32_t NetworkCreateVirtual(int32_t netId, bool hasDns);
    int32_t NetworkDestroy(int32_t netId, bool isVpnNet = false);
    int32_t CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix, const std::set<int32_t> &uids);
    int32_t DestroyVnic();
    int32_t EnableDistributedClientNet(const std::string &virnicAddr,
        const std::string &virnicName, const std::string &iif);
    int32_t EnableDistributedServerNet(const std::string &iif, const std::string &devIface, const std::string &dstAddr,
                                       const std::string &gw);
    int32_t DisableDistributedNet(bool isServer, const std::string &virnicName, const std::string &dstAddr);
    int32_t NetworkAddUids(int32_t netId, const std::vector<UidRange> &uidRanges);
    int32_t NetworkDelUids(int32_t netId, const std::vector<UidRange> &uidRanges);
    int32_t NetworkAddInterface(int32_t netId, std::string iface, NetBearType netBearerType);
    int32_t NetworkRemoveInterface(int32_t netId, std::string iface);

    MarkMaskParcel GetFwmarkForNetwork(int32_t netId);
    int32_t NetworkAddRoute(int32_t netId, std::string ifName, std::string destination, std::string nextHop,
        bool isExcludedRoute = false);
    int32_t NetworkAddRoutes(int32_t netId, const std::vector<nmd::NetworkRouteInfo> &infos);

    int32_t NetworkRemoveRoute(int32_t netId, std::string ifName, std::string destination, std::string nextHop,
        bool isExcludedRoute = false);
    int32_t NetworkGetDefault();
    int32_t NetworkSetDefault(int32_t netId);
    int32_t NetworkClearDefault();
    int32_t NetworkSetPermissionForNetwork(int32_t netId, NetworkPermission permission);
    std::vector<std::string> InterfaceGetList();

    int32_t SetProcSysNet(int32_t family, int32_t which, const std::string ifname, const std::string parameter,
                          const std::string value);
    int32_t GetProcSysNet(int32_t family, int32_t which, const std::string ifname, const std::string parameter,
                          std::string *value);

    nmd::InterfaceConfigurationParcel GetInterfaceConfig(std::string ifName);
    void SetInterfaceConfig(InterfaceConfigurationParcel cfg);
    void ClearInterfaceAddrs(const std::string ifName);
    int32_t GetInterfaceMtu(std::string ifName);
    int32_t SetInterfaceMtu(std::string ifName, int32_t mtuValue);
    int32_t SetTcpBufferSizes(const std::string &tcpBufferSizes);
    int32_t AddInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength);
    int32_t DelInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength);
    int32_t DelInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength,
                                int socketType);
    int32_t InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress);
    int32_t InterfaceSetIffUp(std::string ifaceName);
    int32_t NetworkAddRouteParcel(int32_t netId, RouteInfoParcel routeInfo);
    int32_t NetworkRemoveRouteParcel(int32_t netId, RouteInfoParcel routeInfo);

    int64_t GetCellularRxBytes();
    int64_t GetCellularTxBytes();
    int64_t GetAllRxBytes();
    int64_t GetAllTxBytes();
    int64_t GetUidTxBytes(int32_t uid);
    int64_t GetUidRxBytes(int32_t uid);
    int64_t GetIfaceRxBytes(std::string interfaceName);
    int64_t GetIfaceTxBytes(std::string interfaceName);
    int32_t IpEnableForwarding(const std::string &requester);
    int32_t IpDisableForwarding(const std::string &requester);
    int32_t EnableNat(const std::string &downstreamIface, const std::string &upstreamIface);
    int32_t DisableNat(const std::string &downstreamIface, const std::string &upsteramIface);
    int32_t IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface);
    int32_t IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface);

    int32_t DnsSetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                 const std::vector<std::string> &servers, const std::vector<std::string> &domains);
    int32_t DnsGetResolverConfig(uint16_t netId, std::vector<std::string> &servers, std::vector<std::string> &domains,
                                 uint16_t &baseTimeoutMsec, uint8_t &retryCount);
    int32_t DnsCreateNetworkCache(uint16_t netid, bool isVpnNet = false);
    int32_t DnsDestroyNetworkCache(uint16_t netId, bool isVpnNet = false);
    int32_t BandwidthEnableDataSaver(bool enable);
    int32_t BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes);
    int32_t BandwidthRemoveIfaceQuota(const std::string &ifName);
    int32_t BandwidthAddDeniedList(uint32_t uid);
    int32_t BandwidthRemoveDeniedList(uint32_t uid);
    int32_t BandwidthAddAllowedList(uint32_t uid);
    int32_t BandwidthRemoveAllowedList(uint32_t uid);

    int32_t FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids);
    int32_t FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids);
    int32_t FirewallEnableChain(uint32_t chain, bool enable);
    int32_t FirewallSetUidRule(uint32_t chain, const std::vector<uint32_t> &uids, uint32_t firewallRule);
    void ShareDnsSet(uint16_t netId);
    void StartDnsProxyListen();
    void StopDnsProxyListen();
    void GetDumpInfo(std::string &infos);
    int32_t DnsGetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                           uint16_t netId, std::vector<AddrInfo> &res);
    int32_t AddStaticArp(const std::string &ipAddr, const std::string &macAddr, const std::string &ifName);
    int32_t DelStaticArp(const std::string &ipAddr, const std::string &macAddr, const std::string &ifName);
    int32_t AddStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr, const std::string &ifName);
    int32_t DelStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr, const std::string &ifName);
    int32_t RegisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback, uint32_t timeStep);
    int32_t UnregisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback);
    int32_t SetIpv6PrivacyExtensions(const std::string &interfaceName, const uint32_t on);
    int32_t SetEnableIpv6(const std::string &interfaceName, const uint32_t on, bool needRestart);
    int32_t SetIpv6AutoConf(const std::string &interfaceName, const uint32_t on);
#ifdef FEATURE_NET_FIREWALL_ENABLE
    int32_t SetFirewallDefaultAction(FirewallRuleAction inDefault, FirewallRuleAction outDefault);
    int32_t SetFirewallCurrentUserId(int32_t userId);
    int32_t SetFirewallRules(NetFirewallRuleType type, const std::vector<sptr<NetFirewallBaseRule>> &ruleList,
                             bool isFinish);
    int32_t ClearFirewallRules(NetFirewallRuleType type);
    int32_t RegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback);
    int32_t UnRegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback);
#endif
#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
    int32_t EnableWearableDistributedNetForward(const int32_t tcpPortId, const int32_t udpPortId);
    int32_t DisableWearableDistributedNetForward();
#endif
    int32_t SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag);
    int32_t DeleteNetworkAccessPolicy(uint32_t uid);
    int32_t NotifyNetBearerTypeChange(std::set<NetBearType> bearerTypes);
    int32_t ClearFirewallAllRules();
    int32_t CloseSocketsUid(const std::string &ipAddr, uint32_t uid);
    int32_t DnsSetUserDefinedServerFlag(uint16_t netId, bool flag);
    int32_t FlushDnsCache(uint16_t netId);
#ifdef SUPPORT_SYSVPN
    int32_t UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add);
#endif // SUPPORT_SYSVPN
    int32_t SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo);
#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
    int32_t UpdateEnterpriseRoute(const std::string &interfaceName, uint32_t uid, bool add);
#endif
    int32_t SetInternetAccessByIpForWifiShare(
        const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName);
    int32_t GetIpNeighTable(std::vector<NetManagerStandard::NetIpMacInfo> &ipMacInfo);
    int32_t CreateVlan(const std::string &ifName, uint32_t vlanId);
    int32_t DestroyVlan(const std::string &ifName, uint32_t vlanId);
    int32_t AddVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask);
    int32_t GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo, int32_t &ownerUid);
    int32_t GetSystemNetPortStates(NetManagerStandard::NetPortStatesInfo &netPortStatesInfo);
    void SetClatDnsEnableIpv4(int32_t netId, bool enable);
    int32_t SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid);
private:
    std::shared_ptr<BandwidthManager> bandwidthManager_ = nullptr;
    std::shared_ptr<ConnManager> connManager_ = nullptr;
    std::shared_ptr<FirewallManager> firewallManager_ = nullptr;
#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
    std::shared_ptr<WearableDistributedNet> wearableDistributedNet_ = nullptr;
#endif
    std::shared_ptr<RouteManager> routeManager_ = nullptr;
    std::shared_ptr<InterfaceManager> interfaceManager_ = nullptr;
    std::shared_ptr<SharingManager> sharingManager_ = nullptr;
    std::shared_ptr<DnsManager> dnsManager_ = nullptr;
    std::shared_ptr<MptcpManager> mptcpManager_ = nullptr;
    std::shared_ptr<ffrt::queue> mptcpFfrtQueue_ = nullptr;
    static inline std::vector<uint32_t> interfaceIdex_;
};
} // namespace nmd
} // namespace OHOS
#endif // !INCLUDE_NET_MANAGER_NATIVE_H

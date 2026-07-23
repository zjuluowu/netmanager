/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#ifndef COMMON_MOCK_NETSYS_SERVICE_H
#define COMMON_MOCK_NETSYS_SERVICE_H
#include <gmock/gmock.h>
#include "i_netsys_service.h"

namespace OHOS {
namespace NetsysNative {
class MockINetsysService : public INetsysService {
public:
    MockINetsysService() = default;
    ~MockINetsysService() = default;

    MOCK_METHOD(sptr<IRemoteObject>, AsObject, ());
    MOCK_METHOD(int32_t, SetResolverConfig, (uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
        const std::vector<std::string> &servers, const std::vector<std::string> &domains), (override));
    MOCK_METHOD(int32_t, GetResolverConfig, (uint16_t netId, std::vector<std::string> &servers,
        std::vector<std::string> &domains, uint16_t &baseTimeoutMsec, uint8_t &retryCount), (override));
    MOCK_METHOD(int32_t, CreateNetworkCache, (uint16_t netId, bool isVpnNet), (override));
    MOCK_METHOD(int32_t, DestroyNetworkCache, (uint16_t netId, bool isVpnNet), (override));
    MOCK_METHOD(int32_t, GetAddrInfo, (const std::string &hostName, const std::string &serverName,
        const AddrInfo &hints, uint16_t netId, std::vector<AddrInfo> &res), (override));
    MOCK_METHOD(int32_t, SetInterfaceMtu, (const std::string &interfaceName, int mtu), (override));
    MOCK_METHOD(int32_t, GetInterfaceMtu, (const std::string &interfaceName), (override));
    MOCK_METHOD(int32_t, SetTcpBufferSizes, (const std::string &tcpBufferSizes), (override));
    MOCK_METHOD(int32_t, RegisterNotifyCallback, (sptr<INotifyCallback> &callback), (override));
    MOCK_METHOD(int32_t, UnRegisterNotifyCallback, (sptr<INotifyCallback> &callback), (override));
    MOCK_METHOD(int32_t, NetworkAddRoute, (int32_t netId, const std::string &interfaceName,
        const std::string &destination, const std::string &nextHop, bool isExcludedRoute), (override));
    MOCK_METHOD(int32_t, NetworkAddRoutes, (int32_t netId,
        const std::vector<nmd::NetworkRouteInfo> &infos), (override));
    MOCK_METHOD(int32_t, NetworkRemoveRoute, (int32_t netId, const std::string &interfaceName,
        const std::string &destination, const std::string &nextHop, bool isExcludedRoute), (override));
    MOCK_METHOD(int32_t, NetworkAddRouteParcel, (int32_t netId, const RouteInfoParcel &routeInfo), (override));
    MOCK_METHOD(int32_t, NetworkRemoveRouteParcel, (int32_t netId, const RouteInfoParcel &routeInfo), (override));
    MOCK_METHOD(int32_t, NetworkSetDefault, (int32_t netId), (override));
    MOCK_METHOD(int32_t, NetworkGetDefault, (), (override));
    MOCK_METHOD(int32_t, NetworkClearDefault, (), (override));
    MOCK_METHOD(int32_t, GetProcSysNet, (int32_t family, int32_t which, const std::string &ifname,
        const std::string &parameter, std::string &value), (override));
    MOCK_METHOD(int32_t, SetProcSysNet, (int32_t family, int32_t which, const std::string &ifname,
        const std::string &parameter, std::string &value), (override));
    MOCK_METHOD(int32_t, SetInternetPermission, (uint32_t uid, uint8_t allow, uint8_t isBroker), (override));
    MOCK_METHOD(int32_t, NetworkCreatePhysical, (int32_t netId, int32_t permission), (override));
    MOCK_METHOD(int32_t, NetworkCreateVirtual, (int32_t netId, bool hasDns), (override));
    MOCK_METHOD(int32_t, NetworkAddUids, (int32_t netId, const std::vector<UidRange> &uidRanges), (override));
    MOCK_METHOD(int32_t, NetworkDelUids, (int32_t netId, const std::vector<UidRange> &uidRanges), (override));
    MOCK_METHOD(int32_t, AddInterfaceAddress, (const std::string &interfaceName, const std::string &addrString,
        int32_t prefixLength), (override));
    MOCK_METHOD(int32_t, DelInterfaceAddress, (const std::string &interfaceName, const std::string &addrString,
        int32_t prefixLength), (override));
    MOCK_METHOD(int32_t, DelInterfaceAddress, (const std::string &interfaceName, const std::string &addrString,
        int32_t prefixLength, int socketType), (override));
    MOCK_METHOD(int32_t, InterfaceSetIpAddress,
        (const std::string &ifaceName, const std::string &ipAddress), (override));
    MOCK_METHOD(int32_t, InterfaceSetIffUp, (const std::string &ifaceName), (override));
    MOCK_METHOD(int32_t, NetworkAddInterface,
        (int32_t netId, const std::string &iface, NetBearType netBearerType), (override));
    MOCK_METHOD(int32_t, NetworkRemoveInterface, (int32_t netId, const std::string &iface), (override));
    MOCK_METHOD(int32_t, NetworkDestroy, (int32_t netId, bool isVpnNet), (override));
    MOCK_METHOD(int32_t, CreateVnic, (uint16_t mtu, const std::string &tunAddr, int32_t prefix,
        const std::set<int32_t> &uids), (override));
    MOCK_METHOD(int32_t, DestroyVnic, (), (override));
    MOCK_METHOD(int32_t, EnableDistributedClientNet,
        (const std::string &virnicAddr, const std::string &virNicName, const std::string &iif), (override));
    MOCK_METHOD(int32_t, EnableDistributedServerNet, (const std::string &iif, const std::string &devIface,
        const std::string &dstAddr, const std::string &gw), (override));
    MOCK_METHOD(int32_t, DisableDistributedNet,
        (bool isServer, const std::string &virnicName, const std::string &dstAddr), (override));
    MOCK_METHOD(int32_t, GetFwmarkForNetwork, (int32_t netId, MarkMaskParcel &markMaskParcel), (override));
    MOCK_METHOD(int32_t, SetInterfaceConfig, (const InterfaceConfigurationParcel &cfg), (override));
    MOCK_METHOD(int32_t, GetInterfaceConfig, (InterfaceConfigurationParcel &cfg), (override));
    MOCK_METHOD(int32_t, InterfaceGetList, (std::vector<std::string> &ifaces), (override));
    MOCK_METHOD(int32_t, StartDhcpClient, (const std::string &iface, bool bIpv6), (override));
    MOCK_METHOD(int32_t, StopDhcpClient, (const std::string &iface, bool bIpv6), (override));
    MOCK_METHOD(int32_t, StartDhcpService, (const std::string &iface, const std::string &ipv4addr), (override));
    MOCK_METHOD(int32_t, StopDhcpService, (const std::string &iface), (override));
    MOCK_METHOD(int32_t, IpEnableForwarding, (const std::string &requestor), (override));
    MOCK_METHOD(int32_t, IpDisableForwarding, (const std::string &requestor), (override));
    MOCK_METHOD(int32_t, EnableNat, (const std::string &downstreamIface, const std::string &upstreamIface), (override));
    MOCK_METHOD(int32_t, DisableNat,
        (const std::string &downstreamIface, const std::string &upstreamIface), (override));
    MOCK_METHOD(int32_t, IpfwdAddInterfaceForward,
        (const std::string &fromIface, const std::string &toIface), (override));
    MOCK_METHOD(int32_t, IpfwdRemoveInterfaceForward,
        (const std::string &fromIface, const std::string &toIface), (override));
    MOCK_METHOD(int32_t, BandwidthAddAllowedList, (uint32_t uid), (override));
    MOCK_METHOD(int32_t, BandwidthRemoveAllowedList, (uint32_t uid), (override));
    MOCK_METHOD(int32_t, BandwidthEnableDataSaver, (bool enable), (override));
    MOCK_METHOD(int32_t, BandwidthSetIfaceQuota, (const std::string &ifName, int64_t bytes), (override));
    MOCK_METHOD(int32_t, BandwidthAddDeniedList, (uint32_t uid), (override));
    MOCK_METHOD(int32_t, BandwidthRemoveDeniedList, (uint32_t uid), (override));
    MOCK_METHOD(int32_t, BandwidthRemoveIfaceQuota, (const std::string &ifName), (override));
    MOCK_METHOD(int32_t, FirewallSetUidsAllowedListChain,
        (uint32_t chain, const std::vector<uint32_t> &uids), (override));
    MOCK_METHOD(int32_t, FirewallSetUidsDeniedListChain,
        (uint32_t chain, const std::vector<uint32_t> &uids), (override));
    MOCK_METHOD(int32_t, FirewallEnableChain, (uint32_t chain, bool enable), (override));
    MOCK_METHOD(int32_t, FirewallSetUidRule, (uint32_t chain, const std::vector<uint32_t> &uids,
        uint32_t firewallRule), (override));
    MOCK_METHOD(int32_t, ShareDnsSet, (uint16_t netId), (override));
    MOCK_METHOD(int32_t, StartDnsProxyListen, (), (override));
    MOCK_METHOD(int32_t, StopDnsProxyListen, (), (override));
    MOCK_METHOD(int32_t, GetNetworkSharingTraffic, (const std::string &downIface, const std::string &upIface,
        NetworkSharingTraffic &traffic), (override));
    MOCK_METHOD(int32_t, GetNetworkCellularSharingTraffic, (nmd::NetworkSharingTraffic &traffic,
        std::string &ifaceName), (override));
    MOCK_METHOD(int32_t, SetDpaCellularSharingTraffic, (nmd::NetworkDpaTrafficReport &traffic), (override));
    MOCK_METHOD(int32_t, GetTotalStats, (uint64_t &stats, uint32_t type), (override));
    MOCK_METHOD(int32_t, GetUidStats, (uint64_t &stats, uint32_t type, uint32_t uid), (override));
    MOCK_METHOD(int32_t, GetIfaceStats, (uint64_t &stats, uint32_t type, const std::string &interfaceName), (override));
    MOCK_METHOD(int32_t, GetAllStatsInfo, (std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats), (override));
    MOCK_METHOD(int32_t, DeleteStatsInfo, (uint32_t uid), (override));
    MOCK_METHOD(int32_t, GetAllSimStatsInfo, (std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats), (override));
    MOCK_METHOD(int32_t, DeleteSimStatsInfo, (uint32_t uid), (override));
    MOCK_METHOD(int32_t, SetNetStateTrafficMap, (uint8_t flag, uint64_t availableTraffic), (override));
    MOCK_METHOD(int32_t, GetNetStateTrafficMap, (uint8_t flag, uint64_t &availableTraffic), (override));
    MOCK_METHOD(int32_t, ClearIncreaseTrafficMap, (), (override));
    MOCK_METHOD(int32_t, ClearSimStatsBpfMap, (), (override));
    MOCK_METHOD(int32_t, DeleteIncreaseTrafficMap, (uint64_t ifIndex), (override));
    MOCK_METHOD(int32_t, UpdateIfIndexMap, (int8_t key, uint64_t index), (override));
    MOCK_METHOD(int32_t, SetNetStatusMap, (uint8_t type, uint8_t value), (override));
    MOCK_METHOD(int32_t, SetIptablesCommandForRes,
        (const std::string &cmd, std::string &respond, IptablesType ipType), (override));
    MOCK_METHOD(int32_t, SetIpCommandForRes, (const std::string &cmd, std::string &respond), (override));
    MOCK_METHOD(int32_t, NetDiagPingHost, (const NetDiagPingOption &pingOption,
        const sptr<INetDiagCallback> &callback), (override));
    MOCK_METHOD(int32_t, NetDiagGetRouteTable, (std::list<NetDiagRouteTable> &routeTables), (override));
    MOCK_METHOD(int32_t, NetDiagGetSocketsInfo,
        (NetDiagProtocolType socketType, NetDiagSocketsInfo &socketsInfo), (override));
    MOCK_METHOD(int32_t, NetDiagGetInterfaceConfig, (std::list<NetDiagIfaceConfig> &configs,
        const std::string &ifaceName), (override));
    MOCK_METHOD(int32_t, NetDiagUpdateInterfaceConfig, (const NetDiagIfaceConfig &config, const std::string &ifaceName,
        bool add), (override));
    MOCK_METHOD(int32_t, NetDiagSetInterfaceActiveState, (const std::string &ifaceName, bool up), (override));
    MOCK_METHOD(int32_t, AddStaticArp, (const std::string &ipAddr, const std::string &macAddr,
        const std::string &ifName), (override));
    MOCK_METHOD(int32_t, DelStaticArp, (const std::string &ipAddr, const std::string &macAddr,
        const std::string &ifName), (override));
    MOCK_METHOD(int32_t, AddStaticIpv6Addr, (const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName), (override));
    MOCK_METHOD(int32_t, DelStaticIpv6Addr, (const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName), (override));
    MOCK_METHOD(int32_t, RegisterDnsResultCallback,
        (const sptr<INetDnsResultCallback> &callback, uint32_t delay), (override));
    MOCK_METHOD(int32_t, UnregisterDnsResultCallback, (const sptr<INetDnsResultCallback> &callback), (override));
    MOCK_METHOD(int32_t, GetCookieStats, (uint64_t &stats, uint32_t type, uint64_t cookie), (override));
    MOCK_METHOD(int32_t, GetNetworkSharingType, (std::set<uint32_t>& sharingTypeIsOn), (override));
    MOCK_METHOD(int32_t, UpdateNetworkSharingType, (uint32_t type, bool isOpen), (override));
#ifdef FEATURE_NET_FIREWALL_ENABLE
    MOCK_METHOD(int32_t, SetFirewallRules, (NetFirewallRuleType type,
        const std::vector<sptr<NetFirewallBaseRule>> &ruleList, bool isFinish), (override));
    MOCK_METHOD(int32_t, SetFirewallDefaultAction, (int32_t userId, FirewallRuleAction inDefault,
        FirewallRuleAction outDefault), (override));
    MOCK_METHOD(int32_t, SetFirewallCurrentUserId, (int32_t userId), (override));
    MOCK_METHOD(int32_t, ClearFirewallRules, (NetFirewallRuleType type), (override));
    MOCK_METHOD(int32_t, RegisterNetFirewallCallback, (const sptr<INetFirewallCallback> &callback), (override));
    MOCK_METHOD(int32_t, UnRegisterNetFirewallCallback, (const sptr<INetFirewallCallback> &callback), (override));
#endif
#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
    MOCK_METHOD(int32_t, EnableWearableDistributedNetForward,
        (const int32_t tcpPortId, const int32_t udpPortId), (override));
    MOCK_METHOD(int32_t, DisableWearableDistributedNetForward, (), (override));
#endif
    MOCK_METHOD(int32_t, RegisterNetsysTrafficCallback, (const sptr<INetsysTrafficCallback> &callback), (override));
    MOCK_METHOD(int32_t, UnRegisterNetsysTrafficCallback, (const sptr<INetsysTrafficCallback> &callback), (override));
    MOCK_METHOD(int32_t, SetIpv6PrivacyExtensions, (const std::string &interfaceName, const uint32_t on), (override));
    MOCK_METHOD(int32_t, SetEnableIpv6, (const std::string &interfaceName, const uint32_t on,
        bool needRestart), (override));
    MOCK_METHOD(int32_t, SetIpv6AutoConf, (const std::string &interfaceName, const uint32_t on), (override));
    MOCK_METHOD(int32_t, SetNetworkAccessPolicy, (uint32_t uid,
        NetworkAccessPolicy policy, bool reconfirmFlag), (override));
    MOCK_METHOD(int32_t, DeleteNetworkAccessPolicy, (uint32_t uid), (override));
    MOCK_METHOD(int32_t, NotifyNetBearerTypeChange, (std::set<NetBearType> bearerTypes), (override));
    MOCK_METHOD(int32_t, StartClat, (const std::string &interfaceName, int32_t netId,
        const std::string &nat64PrefixStr), (override));
    MOCK_METHOD(int32_t, StopClat, (const std::string &interfaceName), (override));
    MOCK_METHOD(int32_t, ClearFirewallAllRules, (), (override));
    MOCK_METHOD(int32_t, SetNicTrafficAllowed, (const std::vector<std::string> &ifaceNames, bool status), (override));
    MOCK_METHOD(int32_t, CloseSocketsUid, (const std::string &ipAddr, uint32_t uid), (override));
#ifdef SUPPORT_SYSVPN
    MOCK_METHOD(int32_t, ProcessVpnStage,
        (NetsysNative::SysVpnStageCode stage, const std::string &message), (override));
    MOCK_METHOD(int32_t, UpdateVpnRules,
        (uint16_t netId, const std::vector<std::string> &extMessages, bool add), (override));
#endif // SUPPORT_SYSVPN
    MOCK_METHOD(int32_t, SetBrokerUidAccessPolicyMap,
        ((const std::unordered_map<uint32_t, uint32_t> &)uidMaps), (override));
    MOCK_METHOD(int32_t, DelBrokerUidAccessPolicyMap, (uint32_t uid), (override));
    MOCK_METHOD(int32_t, SetUserDefinedServerFlag, (uint16_t netId, bool flag), (override));
    MOCK_METHOD(int32_t, FlushDnsCache, (uint16_t netId), (override));
    MOCK_METHOD(int32_t, SetDnsCache,
        (uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo), (override));
#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
    MOCK_METHOD(int32_t, UpdateEnterpriseRoute, (const std::string &interfaceName, uint32_t uid, bool add), (override));
#endif
    MOCK_METHOD(int32_t, SetInternetAccessByIpForWifiShare,
        (const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName),
        (override));
    MOCK_METHOD(int32_t, SetIpv6UidBlackList, (std::vector<int32_t> &netIds, int32_t uid), (override));
    MOCK_METHOD(int32_t, GetIpNeighTable, (std::vector<NetIpMacInfo> &ipMacInfo));
    MOCK_METHOD(int32_t, CreateVlan, (const std::string &ifName, uint32_t vlanId));
    MOCK_METHOD(int32_t, DestroyVlan, (const std::string &ifName, uint32_t vlanId));
    MOCK_METHOD(int32_t, AddVlanIp, (const std::string &ifName, uint32_t vlanId,
        const std::string &ip, uint32_t mask));
    MOCK_METHOD(int32_t, GetConnectOwnerUid, (const NetConnInfo &netConnInfo, int32_t &ownerUid));
    MOCK_METHOD(int32_t, GetSystemNetPortStates, (NetPortStatesInfo &netPortStatesInfo));
};
} // NetsysNative
} // OHOS
#endif
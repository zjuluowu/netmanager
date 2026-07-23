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

#include <net/if.h>

#include "interface_manager.h"
#include "mptcp_manager.h"
#include "net_manager_constants.h"
#include "net_manager_native.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "network_permission.h"
#include "route_manager.h"
#include "traffic_manager.h"
#include "vpn_manager.h"
#include "vnic_manager.h"
#include "distributed_manager.h"
#ifdef SUPPORT_SYSVPN
#include "multi_vpn_manager.h"
#endif

using namespace OHOS::NetManagerStandard::CommonUtils;

namespace OHOS {
namespace nmd {
namespace {
constexpr const char *TUN_CARD_NAME = "vpn-tun";
constexpr const char *TCP_RMEM_PROC_FILE = "/proc/sys/net/ipv4/tcp_rmem";
constexpr int32_t NETID_UNSET = -1;
constexpr const char *TCP_WMEM_PROC_FILE = "/proc/sys/net/ipv4/tcp_wmem";
constexpr uint32_t TCP_BUFFER_SIZES_TYPE = 2;
constexpr uint32_t MAX_TCP_BUFFER_SIZES_COUNT = 6;
} // namespace

NetManagerNative::NetManagerNative()
    : bandwidthManager_(std::make_shared<BandwidthManager>()),
      connManager_(std::make_shared<ConnManager>()),
      firewallManager_(std::make_shared<FirewallManager>()),
#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
      wearableDistributedNet_(std::make_shared<WearableDistributedNet>()),
#endif
      routeManager_(std::make_shared<RouteManager>()),
      interfaceManager_(std::make_shared<InterfaceManager>()),
      sharingManager_(std::make_shared<SharingManager>()),
      dnsManager_(std::make_shared<DnsManager>()),
      mptcpManager_(std::make_shared<MptcpManager>()),
      mptcpFfrtQueue_(std::make_shared<ffrt::queue>("MptcpManager"))
{
}

void NetManagerNative::GetOriginInterfaceIndex()
{
    std::vector<std::string> ifNameList = InterfaceManager::GetInterfaceNames();
    interfaceIdex_.clear();
    for (auto iter = ifNameList.begin(); iter != ifNameList.end(); ++iter) {
        uint32_t infIndex = if_nametoindex((*iter).c_str());
        interfaceIdex_.push_back(infIndex);
    }
}

void NetManagerNative::UpdateInterfaceIndex(uint32_t infIndex)
{
    interfaceIdex_.push_back(infIndex);
}

std::vector<uint32_t> NetManagerNative::GetCurrentInterfaceIndex()
{
    return interfaceIdex_;
}

void NetManagerNative::Init()
{
    GetOriginInterfaceIndex();
}

int32_t NetManagerNative::NetworkReinitRoute()
{
    return connManager_->ReinitRoute();
}

int32_t NetManagerNative::SetInternetPermission(uint32_t uid, uint8_t allow, uint8_t isBroker)
{
    return connManager_->SetInternetPermission(uid, allow, isBroker);
}

int32_t NetManagerNative::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    return connManager_->CreatePhysicalNetwork(static_cast<uint16_t>(netId),
                                               static_cast<NetworkPermission>(permission));
}

int32_t NetManagerNative::NetworkCreateVirtual(int32_t netId, bool hasDns)
{
    return connManager_->CreateVirtualNetwork(netId, hasDns);
}

int32_t NetManagerNative::NetworkDestroy(int32_t netId, bool isVpnNet)
{
    auto ret = connManager_->DestroyNetwork(netId);
    dnsManager_->DestroyNetworkCache(netId, isVpnNet);
    return ret;
}

int32_t NetManagerNative::CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix,
                                     const std::set<int32_t> &uids)
{
    return VnicManager::GetInstance().CreateVnic(mtu, tunAddr, prefix, uids);
}

int32_t NetManagerNative::DestroyVnic()
{
    return VnicManager::GetInstance().DestroyVnic();
}

int32_t NetManagerNative::EnableDistributedClientNet(const std::string &virnicAddr,
    const std::string &virnicName, const std::string &iif)
{
    return routeManager_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
}

int32_t NetManagerNative::EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                                     const std::string &dstAddr, const std::string &gw)
{
    return routeManager_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
}

int32_t NetManagerNative::DisableDistributedNet(
    bool isServer, const std::string &virnicName, const std::string &dstAddr)
{
    return routeManager_->DisableDistributedNet(isServer, virnicName, dstAddr);
}

int32_t NetManagerNative::NetworkAddUids(int32_t netId, const std::vector<UidRange> &uidRanges)
{
    dnsManager_->AddUidRange(netId, uidRanges);
    return connManager_->AddUidsToNetwork(netId, uidRanges);
}

int32_t NetManagerNative::NetworkDelUids(int32_t netId, const std::vector<UidRange> &uidRanges)
{
    dnsManager_->DelUidRange(netId, uidRanges);
    return connManager_->RemoveUidsFromNetwork(netId, uidRanges);
}

int32_t NetManagerNative::NetworkAddInterface(int32_t netId, std::string interfaceName, NetBearType netBearerType)
{
    return connManager_->AddInterfaceToNetwork(netId, interfaceName, netBearerType);
}

int32_t NetManagerNative::NetworkRemoveInterface(int32_t netId, std::string interfaceName)
{
    return connManager_->RemoveInterfaceFromNetwork(netId, interfaceName);
}

int32_t NetManagerNative::AddInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength)
{
#ifdef SUPPORT_SYSVPN
    if ((strncmp(ifName.c_str(), XFRM_CARD_NAME, strlen(XFRM_CARD_NAME)) == 0) ||
        (strncmp(ifName.c_str(), PPP_CARD_NAME, strlen(PPP_CARD_NAME)) == 0) ||
        (strncmp(ifName.c_str(), MULTI_TUN_CARD_NAME, strlen(MULTI_TUN_CARD_NAME)) == 0) ||
        (strncmp(ifName.c_str(), INNER_CHL_NAME, strlen(INNER_CHL_NAME)) == 0)) {
        return MultiVpnManager::GetInstance().SetVpnAddress(ifName, addrString, prefixLength);
    }
#endif // SUPPORT_SYSVPN
    if (strncmp(ifName.c_str(), TUN_CARD_NAME, strlen(TUN_CARD_NAME)) != 0) {
        auto mptcpManager = mptcpManager_;
        std::string addr = addrString;
        std::string iface = ifName;
        if (mptcpFfrtQueue_ != nullptr && mptcpManager != nullptr) {
            mptcpFfrtQueue_->submit([mptcpManager, addr, iface]() {
                mptcpManager->OnInterfaceAddressUpdated(addr, iface);
            });
        }
        return interfaceManager_->AddAddress(ifName.c_str(), addrString.c_str(), prefixLength);
    }
    return VpnManager::GetInstance().SetVpnAddress(ifName, addrString, prefixLength);
}

int32_t NetManagerNative::DelInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength)
{
    auto mptcpManager = mptcpManager_;
    std::string addr = addrString;
    std::string iface = ifName;
    if (mptcpFfrtQueue_ != nullptr && mptcpManager != nullptr) {
        mptcpFfrtQueue_->submit([mptcpManager, addr, iface]() {
            mptcpManager->OnInterfaceAddressRemoved(addr, iface);
        });
    }
    return interfaceManager_->DelAddress(ifName.c_str(), addrString.c_str(), prefixLength);
}

int32_t NetManagerNative::DelInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength,
                                              int socketType)
{
    return interfaceManager_->DelAddress(ifName.c_str(), addrString.c_str(), prefixLength, socketType);
}

int32_t NetManagerNative::NetworkAddRoute(int32_t netId, std::string interfaceName, std::string destination,
                                          std::string nextHop, bool isExcludedRoute)
{
    bool routeRepeat = false;
    NetworkRouteInfo networkRouteInfo;
    networkRouteInfo.ifName = interfaceName;
    networkRouteInfo.destination = destination;
    networkRouteInfo.nextHop = nextHop;
    networkRouteInfo.isExcludedRoute = isExcludedRoute;
    auto ret = connManager_->AddRoute(netId, networkRouteInfo, routeRepeat);
    if (!ret || routeRepeat) {
        dnsManager_->EnableIpv6(netId, destination, nextHop);
        dnsManager_->EnableIpv4(netId, destination, nextHop);
    }
    return ret;
}

int32_t NetManagerNative::NetworkAddRoutes(int32_t netId, const std::vector<nmd::NetworkRouteInfo> &infos)
{
    return connManager_->AddRoutes(netId, infos);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
int32_t NetManagerNative::UpdateEnterpriseRoute(const std::string &interfaceName, uint32_t uid, bool add)
{
    return routeManager_->UpdateEnterpriseRoute(interfaceName, uid, add);
}
#endif

int32_t NetManagerNative::NetworkRemoveRoute(int32_t netId, std::string interfaceName, std::string destination,
                                             std::string nextHop, bool isExcludedRoute)
{
    return connManager_->RemoveRoute(netId, interfaceName, destination, nextHop, isExcludedRoute);
}

int32_t NetManagerNative::NetworkGetDefault()
{
    return connManager_->GetDefaultNetwork();
}

int32_t NetManagerNative::NetworkSetDefault(int32_t netId)
{
    dnsManager_->SetDefaultNetwork(netId);
    return connManager_->SetDefaultNetwork(netId);
}

int32_t NetManagerNative::NetworkClearDefault()
{
    return connManager_->ClearDefaultNetwork();
}

int32_t NetManagerNative::NetworkSetPermissionForNetwork(int32_t netId, NetworkPermission permission)
{
    return connManager_->SetPermissionForNetwork(netId, permission);
}

std::vector<std::string> NetManagerNative::InterfaceGetList()
{
    return InterfaceManager::GetInterfaceNames();
}

nmd::InterfaceConfigurationParcel NetManagerNative::GetInterfaceConfig(std::string interfaceName)
{
    return InterfaceManager::GetIfaceConfig(interfaceName.c_str());
}

void NetManagerNative::SetInterfaceConfig(nmd::InterfaceConfigurationParcel parcel)
{
    InterfaceManager::SetIfaceConfig(parcel);
}

void NetManagerNative::ClearInterfaceAddrs(const std::string ifName) {}

int32_t NetManagerNative::GetInterfaceMtu(std::string ifName)
{
    return InterfaceManager::GetMtu(ifName.c_str());
}

int32_t NetManagerNative::SetInterfaceMtu(std::string ifName, int32_t mtuValue)
{
#ifdef SUPPORT_SYSVPN
    if ((strncmp(ifName.c_str(), XFRM_CARD_NAME, strlen(XFRM_CARD_NAME)) == 0) ||
        (strncmp(ifName.c_str(), PPP_CARD_NAME, strlen(PPP_CARD_NAME)) == 0) ||
        (strncmp(ifName.c_str(), MULTI_TUN_CARD_NAME, strlen(MULTI_TUN_CARD_NAME)) == 0) ||
        (strncmp(ifName.c_str(), INNER_CHL_NAME, strlen(INNER_CHL_NAME)) == 0)) {
        return MultiVpnManager::GetInstance().SetVpnMtu(ifName, mtuValue);
    }
#endif
    if (strncmp(ifName.c_str(), TUN_CARD_NAME, strlen(TUN_CARD_NAME)) != 0) {
        return InterfaceManager::SetMtu(ifName.c_str(), std::to_string(mtuValue).c_str());
    }
    return VpnManager::GetInstance().SetVpnMtu(ifName, mtuValue);
}

int32_t NetManagerNative::SetTcpBufferSizes(const std::string &tcpBufferSizes)
{
    NETNATIVE_LOGI("tcpBufferSizes:%{public}s", tcpBufferSizes.c_str());
    const std::vector<std::string> vTcpBufferSizes = Split(tcpBufferSizes, ",");
    if (vTcpBufferSizes.size() != MAX_TCP_BUFFER_SIZES_COUNT) {
        NETNATIVE_LOGE("NetManagerNative::SetTcpBufferSizes size is not equals MAX_TCP_BUFFER_SIZES_COUNT");
        return -1;
    }
    std::string tcp_rwmem[TCP_BUFFER_SIZES_TYPE];
    for (size_t i = 0; i < TCP_BUFFER_SIZES_TYPE; i++) {
        for (size_t j = 0; j < MAX_TCP_BUFFER_SIZES_COUNT / TCP_BUFFER_SIZES_TYPE; j++) {
            tcp_rwmem[i] += Strip(vTcpBufferSizes[i * (MAX_TCP_BUFFER_SIZES_COUNT / TCP_BUFFER_SIZES_TYPE) + j]);
            tcp_rwmem[i] += ' ';
        }
    }
    if (!WriteFile(TCP_RMEM_PROC_FILE, tcp_rwmem[0]) || !WriteFile(TCP_WMEM_PROC_FILE, tcp_rwmem[1])) {
        NETNATIVE_LOGE("NetManagerNative::SetTcpBufferSizes sysctlbyname fail %{public}d", errno);
        return -1;
    }
    return 0;
}

int32_t NetManagerNative::InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    return InterfaceManager::SetIpAddress(ifaceName.c_str(), ipAddress.c_str());
}

int32_t NetManagerNative::InterfaceSetIffUp(std::string ifaceName)
{
    return InterfaceManager::SetIffUp(ifaceName.c_str());
}

nmd::MarkMaskParcel NetManagerNative::GetFwmarkForNetwork(int32_t netId)
{
    nmd::MarkMaskParcel mark;
    mark.mark = connManager_->GetFwmarkForNetwork(netId);
    mark.mask = 0XFFFF;
    return mark;
}

int32_t NetManagerNative::NetworkAddRouteParcel(int32_t netId, RouteInfoParcel parcel)
{
    bool routeRepeat = false;
    NetworkRouteInfo networkRouteInfo;
    networkRouteInfo.ifName = parcel.ifName;
    networkRouteInfo.destination = parcel.destination;
    networkRouteInfo.nextHop = parcel.nextHop;
    networkRouteInfo.isExcludedRoute = false;
    return connManager_->AddRoute(netId, networkRouteInfo, routeRepeat);
}

int32_t NetManagerNative::NetworkRemoveRouteParcel(int32_t netId, RouteInfoParcel parcel)
{
    return connManager_->RemoveRoute(netId, parcel.ifName, parcel.destination, parcel.nextHop);
}

int32_t NetManagerNative::SetProcSysNet(int32_t family, int32_t which, const std::string ifname,
                                        const std::string parameter, const std::string value)
{
    return 0;
}

int32_t NetManagerNative::GetProcSysNet(int32_t family, int32_t which, const std::string ifname,
                                        const std::string parameter, std::string *value)
{
    return 0;
}

int64_t NetManagerNative::GetCellularRxBytes()
{
    return 0;
}

int64_t NetManagerNative::GetCellularTxBytes()
{
    return 0;
}

int64_t NetManagerNative::GetAllRxBytes()
{
    return nmd::TrafficManager::GetAllRxTraffic();
}

int64_t NetManagerNative::GetAllTxBytes()
{
    return nmd::TrafficManager::GetAllTxTraffic();
}

int64_t NetManagerNative::GetUidTxBytes(int32_t uid)
{
    return 0;
}

int64_t NetManagerNative::GetUidRxBytes(int32_t uid)
{
    return 0;
}

int64_t NetManagerNative::GetIfaceRxBytes(std::string interfaceName)
{
    nmd::TrafficStatsParcel interfaceTraffic = nmd::TrafficManager::GetInterfaceTraffic(interfaceName);
    return interfaceTraffic.rxBytes;
}

int64_t NetManagerNative::GetIfaceTxBytes(std::string interfaceName)
{
    nmd::TrafficStatsParcel interfaceTraffic = nmd::TrafficManager::GetInterfaceTraffic(interfaceName);
    return interfaceTraffic.txBytes;
}

int32_t NetManagerNative::IpEnableForwarding(const std::string &requester)
{
    return sharingManager_->IpEnableForwarding(requester);
}

int32_t NetManagerNative::SetIpv6PrivacyExtensions(const std::string &interfaceName, const uint32_t on)
{
    return sharingManager_->SetIpv6PrivacyExtensions(interfaceName, on);
}

int32_t NetManagerNative::SetEnableIpv6(const std::string &interfaceName, const uint32_t on, bool needRestart)
{
    if (on == 0 && interfaceName.find("wlan") == 0) {
        NETNATIVE_LOGI("Disable ipv6 dns, iface=%{public}s", interfaceName.c_str());
        int32_t netId = connManager_->GetNetIdByInterface(interfaceName);
        if (netId != NETID_UNSET) {
            std::string destination;
            dnsManager_->EnableIpv6(static_cast<uint16_t>(netId), destination, std::string(), false);
        } else {
            NETNATIVE_LOGE("netId not found, iface=%{public}s", interfaceName.c_str());
        }
    }
    return sharingManager_->SetEnableIpv6(interfaceName, on, needRestart);
}

int32_t NetManagerNative::SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid)
{
    return dnsManager_->SetIpv6UidBlackList(netIds, uid);
}

int32_t NetManagerNative::SetIpv6AutoConf(const std::string &interfaceName, const uint32_t on)
{
    return InterfaceManager::SetIpv6AutoConf(interfaceName, on);
}

int32_t NetManagerNative::IpDisableForwarding(const std::string &requester)
{
    return sharingManager_->IpDisableForwarding(requester);
}

int32_t NetManagerNative::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    return sharingManager_->EnableNat(downstreamIface, upstreamIface);
}

int32_t NetManagerNative::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    return sharingManager_->DisableNat(downstreamIface, upstreamIface);
}

int32_t NetManagerNative::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    return sharingManager_->IpfwdAddInterfaceForward(fromIface, toIface);
}

int32_t NetManagerNative::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    return sharingManager_->IpfwdRemoveInterfaceForward(fromIface, toIface);
}

int32_t NetManagerNative::SetInternetAccessByIpForWifiShare(
    const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName)
{
    return sharingManager_->SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
}

int32_t NetManagerNative::DnsSetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                               const std::vector<std::string> &servers,
                                               const std::vector<std::string> &domains)
{
    return dnsManager_->SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
}

int32_t NetManagerNative::DnsGetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                               std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                               uint8_t &retryCount)
{
    return dnsManager_->GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
}

int32_t NetManagerNative::DnsCreateNetworkCache(uint16_t netId, bool isVpnNet)
{
    return dnsManager_->CreateNetworkCache(netId, isVpnNet);
}

int32_t NetManagerNative::DnsDestroyNetworkCache(uint16_t netId, bool isVpnNet)
{
    return dnsManager_->DestroyNetworkCache(netId, isVpnNet);
}

int32_t NetManagerNative::BandwidthEnableDataSaver(bool enable)
{
    return bandwidthManager_->EnableDataSaver(enable);
}

int32_t NetManagerNative::BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    return bandwidthManager_->SetIfaceQuota(ifName, bytes);
}

int32_t NetManagerNative::BandwidthRemoveIfaceQuota(const std::string &ifName)
{
    return bandwidthManager_->RemoveIfaceQuota(ifName);
}

int32_t NetManagerNative::BandwidthAddDeniedList(uint32_t uid)
{
    return bandwidthManager_->AddDeniedList(uid);
}

int32_t NetManagerNative::BandwidthRemoveDeniedList(uint32_t uid)
{
    return bandwidthManager_->RemoveDeniedList(uid);
}

int32_t NetManagerNative::BandwidthAddAllowedList(uint32_t uid)
{
    return bandwidthManager_->AddAllowedList(uid);
}

int32_t NetManagerNative::BandwidthRemoveAllowedList(uint32_t uid)
{
    return bandwidthManager_->RemoveAllowedList(uid);
}

int32_t NetManagerNative::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto chainType = static_cast<NetManagerStandard::ChainType>(chain);
    return firewallManager_->SetUidsAllowedListChain(chainType, uids);
}

int32_t NetManagerNative::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto chainType = static_cast<NetManagerStandard::ChainType>(chain);
    return firewallManager_->SetUidsDeniedListChain(chainType, uids);
}

int32_t NetManagerNative::FirewallEnableChain(uint32_t chain, bool enable)
{
    auto chainType = static_cast<NetManagerStandard::ChainType>(chain);
    return firewallManager_->EnableChain(chainType, enable);
}

int32_t NetManagerNative::FirewallSetUidRule(uint32_t chain, const std::vector<uint32_t> &uids, uint32_t firewallRule)
{
    auto chainType = static_cast<NetManagerStandard::ChainType>(chain);
    auto rule = static_cast<NetManagerStandard::FirewallRule>(firewallRule);
    for (auto &uid : uids) {
        auto ret = firewallManager_->SetUidRule(chainType, uid, rule);
        if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
            NETNATIVE_LOGE("SetUidRule fail, uid is %{public}u, ret is %{public}d", uid, ret);
        }
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
int32_t NetManagerNative::SetFirewallDefaultAction(FirewallRuleAction inDefault, FirewallRuleAction outDefault)
{
    NETNATIVE_LOG_D("NetManagerNative, SetFirewallDefaultAction");
    return dnsManager_->SetFirewallDefaultAction(inDefault, outDefault);
}

int32_t NetManagerNative::SetFirewallCurrentUserId(int32_t userId)
{
    NETNATIVE_LOG_D("NetManagerNative, SetFirewallCurrentUserId");
    return dnsManager_->SetFirewallCurrentUserId(userId);
}

int32_t NetManagerNative::SetFirewallRules(NetFirewallRuleType type,
                                           const std::vector<sptr<NetFirewallBaseRule>> &ruleList, bool isFinish)
{
    return dnsManager_->SetFirewallRules(type, ruleList, isFinish);
}

int32_t NetManagerNative::ClearFirewallRules(NetFirewallRuleType type)
{
    NETNATIVE_LOG_D("NetManagerNative, ClearFirewallRules");
    return dnsManager_->ClearFirewallRules(type);
}

int32_t NetManagerNative::RegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    NETNATIVE_LOG_D("NetManagerNative, RegisterNetFirewallCallback");
    return dnsManager_->RegisterNetFirewallCallback(callback);
}

int32_t NetManagerNative::UnRegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    NETNATIVE_LOG_D("NetManagerNative, UnRegisterNetFirewallCallback");
    return dnsManager_->UnRegisterNetFirewallCallback(callback);
}
#endif

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
int32_t NetManagerNative::EnableWearableDistributedNetForward(const int32_t tcpPortId, const int32_t udpPortId)
{
    NETNATIVE_LOG_D("Enabling wearable distributed net forward for TCP port and UDP port");
    return wearableDistributedNet_->EnableWearableDistributedNetForward(tcpPortId, udpPortId);
}

int32_t NetManagerNative::DisableWearableDistributedNetForward()
{
    NETNATIVE_LOG_D("NetManagerNative disable wearable distributed net forward");
    return wearableDistributedNet_->DisableWearableDistributedNetForward();
}
#endif

void NetManagerNative::ShareDnsSet(uint16_t netId)
{
    dnsManager_->ShareDnsSet(netId);
}

void NetManagerNative::StartDnsProxyListen()
{
    dnsManager_->StartDnsProxyListen();
}

void NetManagerNative::StopDnsProxyListen()
{
    dnsManager_->StopDnsProxyListen();
}

int32_t NetManagerNative::DnsGetAddrInfo(const std::string &hostName, const std::string &serverName,
                                         const AddrInfo &hints, uint16_t netId, std::vector<AddrInfo> &res)
{
    return dnsManager_->GetAddrInfo(hostName, serverName, hints, netId, res);
}

void NetManagerNative::GetDumpInfo(std::string &infos)
{
    connManager_->GetDumpInfos(infos);
    dnsManager_->GetDumpInfo(infos);
}

int32_t NetManagerNative::AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                       const std::string &ifName)
{
    if (interfaceManager_ == nullptr) {
        NETNATIVE_LOGE("interfaceManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return interfaceManager_->AddStaticArp(ipAddr, macAddr, ifName);
}

int32_t NetManagerNative::DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                       const std::string &ifName)
{
    if (interfaceManager_ == nullptr) {
        NETNATIVE_LOGE("interfaceManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return interfaceManager_->DelStaticArp(ipAddr, macAddr, ifName);
}

int32_t NetManagerNative::AddStaticIpv6Addr(const std::string &ipAddr, const std::string &macAddr,
    const std::string &ifName)
{
    NETNATIVE_LOGI("AddStaticIpv6Addr");
    if (interfaceManager_ == nullptr) {
        NETNATIVE_LOGE("interfaceManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return interfaceManager_->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
}

int32_t NetManagerNative::DelStaticIpv6Addr(const std::string &ipAddr, const std::string &macAddr,
    const std::string &ifName)
{
    NETNATIVE_LOGI("DelStaticIpv6Addr");
    if (interfaceManager_ == nullptr) {
        NETNATIVE_LOGE("interfaceManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return interfaceManager_->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
}

int32_t NetManagerNative::GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo)
{
    NETNATIVE_LOGI("GetIpNeighTable");
    if (interfaceManager_ == nullptr) {
        NETNATIVE_LOGE("interfaceManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return interfaceManager_->GetIpNeighTable(ipMacInfo);
}

int32_t NetManagerNative::GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo,
                                             int32_t &ownerUid)
{
    return connManager_->GetConnectOwnerUid(netConnInfo, ownerUid);
}

int32_t NetManagerNative::GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo)
{
    NETNATIVE_LOGI("GetSystemNetPortStates");
    if (connManager_ == nullptr) {
        NETNATIVE_LOGE("connManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return connManager_->GetSystemNetPortStates(netPortStatesInfo);
}

int32_t NetManagerNative::RegisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback, uint32_t timeStep)
{
    return dnsManager_->RegisterDnsResultCallback(callback, timeStep);
}

int32_t NetManagerNative::UnregisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback)
{
    return dnsManager_->UnregisterDnsResultCallback(callback);
}

int32_t NetManagerNative::SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag)
{
    return connManager_->SetNetworkAccessPolicy(uid, policy, reconfirmFlag);
}

int32_t NetManagerNative::DeleteNetworkAccessPolicy(uint32_t uid)
{
    return connManager_->DeleteNetworkAccessPolicy(uid);
}

int32_t NetManagerNative::NotifyNetBearerTypeChange(std::set<NetBearType> bearerTypes)
{
    return connManager_->NotifyNetBearerTypeChange(bearerTypes);
}

int32_t NetManagerNative::ClearFirewallAllRules()
{
    return firewallManager_->ClearAllRules();
}

int32_t NetManagerNative::CloseSocketsUid(const std::string &ipAddr, uint32_t uid)
{
    return connManager_->CloseSocketsUid(ipAddr, uid);
}

int32_t NetManagerNative::DnsSetUserDefinedServerFlag(uint16_t netId, bool flag)
{
    return dnsManager_->SetUserDefinedServerFlag(netId, flag);
}

int32_t NetManagerNative::FlushDnsCache(uint16_t netId)
{
    return dnsManager_->FlushDnsCache(netId);
}

#ifdef SUPPORT_SYSVPN
int32_t NetManagerNative::UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add)
{
    return connManager_->UpdateVpnRules(netId, extMessages, add);
}
#endif // SUPPORT_SYSVPN

int32_t NetManagerNative::CreateVlan(const std::string &ifName, uint32_t vlanId)
{
    NETNATIVE_LOGI("CreateVlan");
    if (interfaceManager_ == nullptr) {
        NETNATIVE_LOGE("interfaceManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return interfaceManager_->CreateVlan(ifName, vlanId);
}

int32_t NetManagerNative::DestroyVlan(const std::string &ifName, uint32_t vlanId)
{
    NETNATIVE_LOGI("DestroyVlan");
    if (interfaceManager_ == nullptr) {
        NETNATIVE_LOGE("interfaceManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return interfaceManager_->DestroyVlan(ifName, vlanId);
}

int32_t NetManagerNative::AddVlanIp(const std::string &ifName, uint32_t vlanId,
                                    const std::string &ip, uint32_t mask)
{
    NETNATIVE_LOGI("AddVlanIp");
    if (interfaceManager_ == nullptr) {
        NETNATIVE_LOGE("interfaceManager_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return interfaceManager_->AddVlanIp(ifName, vlanId, ip, mask);
}

int32_t NetManagerNative::SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo)
{
    return dnsManager_->SetDnsCache(netId, hostName, addrInfo);
}

void NetManagerNative::SetClatDnsEnableIpv4(int32_t netId, bool enable)
{
    dnsManager_->SetClatDnsEnableIpv4(netId, enable);
}
} // namespace nmd
} // namespace OHOS

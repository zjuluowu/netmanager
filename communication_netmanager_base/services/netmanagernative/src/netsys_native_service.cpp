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

#include <csignal>
#include <sys/types.h>
#include <regex>
#include <thread>
#include <unistd.h>

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "bpf_loader.h"
#include "bpf_path.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "netsys_native_service.h"
#ifdef SUPPORT_SYSVPN
#include "system_vpn_wrapper.h"
#endif // SUPPORT_SYSVPN
#include "bpf_ring_buffer.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
namespace OHOS {
namespace NetsysNative {
static constexpr const char *BFP_NAME_NETSYS_PATH = "/system/etc/bpf/netsys.o";
const std::regex REGEX_CMD_IPTABLES(std::string(R"(^-[\S]*[\s\S]*)"));

REGISTER_SYSTEM_ABILITY_BY_ID(NetsysNativeService, COMM_NETSYS_NATIVE_SYS_ABILITY_ID, true)

NetsysNativeService::NetsysNativeService()
    : SystemAbility(COMM_NETSYS_NATIVE_SYS_ABILITY_ID, true),
      netsysService_(nullptr),
      manager_(nullptr)
{
}

void NetsysNativeService::OnStart()
{
    NETNATIVE_LOGI("OnStart Begin");
    std::lock_guard<std::mutex> guard(instanceLock_);
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        return;
    }

    if (!Init()) {
        NETNATIVE_LOGE("NetsysNativeService init failed!");
        return;
    }
    bool res = SystemAbility::Publish(this);
    if (!res) {
        NETNATIVE_LOGE("publishing NetsysNativeService to sa manager failed!");
        return;
    }
    NETNATIVE_LOGI("Publish NetsysNativeService SUCCESS");
    state_ = ServiceRunningState::STATE_RUNNING;
    NETNATIVE_LOGI("start listener");
    manager_->StartListener();
#ifdef FEATURE_NET_FIREWALL_ENABLE
    bpfNetFirewall_->StartListener();
#endif
    NETNATIVE_LOGI("start listener end on start end");
}

void NetsysNativeService::OnStop()
{
    std::lock_guard<std::mutex> guard(instanceLock_);
    state_ = ServiceRunningState::STATE_STOPPED;
    NETNATIVE_LOGI("stop listener");
    manager_->StopListener();
#ifdef FEATURE_NET_FIREWALL_ENABLE
    bpfNetFirewall_->StopListener();
    auto ret = OHOS::NetManagerStandard::UnloadElf(BFP_NAME_NETSYS_PATH);
    NETNATIVE_LOGI("UnloadElf is %{public}d", ret);
    if (ret == ElfLoadError::ELF_LOAD_ERR_NONE) {
        bpfNetFirewall_->SetBpfLoaded(false);
    }
#endif
    NETNATIVE_LOGI("stop listener end on stop end");
#ifdef ENABLE_NETSYS_ACCESS_POLICY_DIAG_LISTEN
    NetsysBpfRingBuffer::ExistRingBufferPoll();
#endif
    NetsysBpfRingBuffer::ExistNetstatsRingBufferPoll();
}

int32_t NetsysNativeService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    NETNATIVE_LOG_D("Start Dump, fd: %{public}d", fd);
    std::string result;
    GetDumpMessage(result);
    int32_t ret = dprintf(fd, "%s\n", result.c_str());
    return ret < 0 ? SESSION_UNOPEN_ERR : ERR_NONE;
}

void NetsysNativeService::GetDumpMessage(std::string &message)
{
    netsysService_->GetDumpInfo(message);
}

void ExitHandler(int32_t signum)
{
    (void)signum;
    _Exit(1);
}

bool NetsysNativeService::Init()
{
    (void)signal(SIGTERM, ExitHandler);
    (void)signal(SIGABRT, ExitHandler);

    netsysService_ = std::make_unique<nmd::NetManagerNative>();
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is nullptr!");
        return false;
    }
    netsysService_->Init();

    manager_ = std::make_unique<OHOS::nmd::NetlinkManager>();
    if (manager_ == nullptr) {
        NETNATIVE_LOGE("manager_ is nullptr!");
        return false;
    }
    bpfStats_ = std::make_unique<OHOS::NetManagerStandard::NetsysBpfStats>();
    dhcpController_ = std::make_unique<OHOS::nmd::DhcpController>();
    fwmarkNetwork_ = std::make_unique<OHOS::nmd::FwmarkNetwork>();
    sharingManager_ = std::make_unique<SharingManager>();
    iptablesWrapper_ = IptablesWrapper::GetInstance();
    netDiagWrapper = NetDiagWrapper::GetInstance();
    clatManager_ = std::make_unique<OHOS::nmd::ClatManager>();

    auto ret = OHOS::NetManagerStandard::LoadElf(BFP_NAME_NETSYS_PATH);
    NETNATIVE_LOGI("LoadElf is %{public}d", ret);

#ifdef FEATURE_NET_FIREWALL_ENABLE
    bpfNetFirewall_ = NetsysBpfNetFirewall::GetInstance();
    if (ret == ElfLoadError::ELF_LOAD_ERR_NONE) {
        bpfNetFirewall_->SetBpfLoaded(true);
    }
    AddSystemAbilityListener(COMM_FIREWALL_MANAGER_SYS_ABILITY_ID);
    bpfNetFirewall_->LoadSystemAbility(COMM_FIREWALL_MANAGER_SYS_ABILITY_ID);
#endif

#ifdef ENABLE_NETSYS_ACCESS_POLICY_DIAG_LISTEN
    NetsysBpfRingBuffer::ListenNetworkAccessPolicyEvent();
#endif
    NetsysBpfRingBuffer::ListenNetworkStatsEvent();
    AddSystemAbilityListener(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID);
    return true;
}

void NetsysNativeService::OnNetManagerRestart()
{
    NETNATIVE_LOGI("OnNetManagerRestart");
    if (netsysService_ != nullptr) {
        netsysService_->NetworkReinitRoute();
    }
}

int32_t NetsysNativeService::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                               const std::vector<std::string> &servers,
                                               const std::vector<std::string> &domains)
{
    netsysService_->DnsSetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
    return 0;
}

int32_t NetsysNativeService::GetResolverConfig(uint16_t netid, std::vector<std::string> &servers,
                                               std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                               uint8_t &retryCount)
{
    NETNATIVE_LOG_D("GetResolverConfig netid = %{public}d", netid);
    netsysService_->DnsGetResolverConfig(netid, servers, domains, baseTimeoutMsec, retryCount);
    return 0;
}

int32_t NetsysNativeService::CreateNetworkCache(uint16_t netid, bool isVpnNet)
{
    NETNATIVE_LOG_D("CreateNetworkCache Begin");
    netsysService_->DnsCreateNetworkCache(netid, isVpnNet);

    return 0;
}

int32_t NetsysNativeService::DestroyNetworkCache(uint16_t netId, bool isVpnNet)
{
    NETNATIVE_LOG_D("DestroyNetworkCache");
    return netsysService_->DnsDestroyNetworkCache(netId, isVpnNet);
}

int32_t NetsysNativeService::GetAddrInfo(const std::string &hostName, const std::string &serverName,
                                         const AddrInfo &hints, uint16_t netId, std::vector<AddrInfo> &res)
{
    return netsysService_->DnsGetAddrInfo(hostName, serverName, hints, netId, res);
}

int32_t NetsysNativeService::SetInterfaceMtu(const std::string &interfaceName, int32_t mtu)
{
    NETNATIVE_LOG_D("SetInterfaceMtu  Begin");
    return netsysService_->SetInterfaceMtu(interfaceName, mtu);
}

int32_t NetsysNativeService::GetInterfaceMtu(const std::string &interfaceName)
{
    NETNATIVE_LOG_D("SetInterfaceMtu  Begin");
    return netsysService_->GetInterfaceMtu(interfaceName);
}

int32_t NetsysNativeService::SetTcpBufferSizes(const std::string &tcpBufferSizes)
{
    NETNATIVE_LOG_D("SetTcpBufferSizes  Begin");
    return netsysService_->SetTcpBufferSizes(tcpBufferSizes);
}

int32_t NetsysNativeService::RegisterNotifyCallback(sptr<INotifyCallback> &callback)
{
    NETNATIVE_LOG_D("RegisterNotifyCallback");
    dhcpController_->RegisterNotifyCallback(callback);
    manager_->RegisterNetlinkCallback(callback);
    return 0;
}

int32_t NetsysNativeService::UnRegisterNotifyCallback(sptr<INotifyCallback> &callback)
{
    NETNATIVE_LOGI("UnRegisterNotifyCallback");
    dhcpController_->UnregisterNotifyCallback(callback);
    manager_->UnregisterNetlinkCallback(callback);
    return 0;
}

int32_t NetsysNativeService::RegisterNetsysTrafficCallback(const sptr<INetsysTrafficCallback> &callback)
{
    NETNATIVE_LOGI("RegisterNetsysTrafficCallback");
    NetsysBpfRingBuffer::RegisterNetsysTrafficCallback(callback);
    return 0;
}

int32_t NetsysNativeService::UnRegisterNetsysTrafficCallback(const sptr<INetsysTrafficCallback> &callback)
{
    NETNATIVE_LOGI("UnRegisterNetsysTrafficCallback");
    NetsysBpfRingBuffer::UnRegisterNetsysTrafficCallback(callback);
    return 0;
}

int32_t NetsysNativeService::NetworkAddRoute(int32_t netId, const std::string &interfaceName,
    const std::string &destination, const std::string &nextHop, bool isExcludedRoute)
{
    NETNATIVE_LOG_D("NetworkAddRoute unpacket %{public}d %{public}s %{public}s %{public}s %{public}d", netId,
        interfaceName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str(), isExcludedRoute);
    int32_t result = netsysService_->NetworkAddRoute(netId, interfaceName, destination, nextHop, isExcludedRoute);
    NETNATIVE_LOG_D("NetworkAddRoute %{public}d", result);
    return result;
}

int32_t NetsysNativeService::NetworkAddRoutes(int32_t netId, const std::vector<nmd::NetworkRouteInfo> &infos)
{
    NETNATIVE_LOGI("NetworkAddRoutes: netId[%{public}d], infos[%{public}zu]", netId, infos.size());
    int32_t result = netsysService_->NetworkAddRoutes(netId, infos);
    NETNATIVE_LOG_D("NetworkAddRoutes %{public}d", result);
    return result;
}

int32_t NetsysNativeService::NetworkRemoveRoute(int32_t netId, const std::string &interfaceName,
    const std::string &destination, const std::string &nextHop, bool isExcludedRoute)
{
    int32_t result = netsysService_->NetworkRemoveRoute(netId, interfaceName, destination, nextHop, isExcludedRoute);
    NETNATIVE_LOG_D("NetworkRemoveRoute %{public}d", result);
    return result;
}

int32_t NetsysNativeService::NetworkAddRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo)
{
    int32_t result = netsysService_->NetworkAddRouteParcel(netId, routeInfo);
    NETNATIVE_LOG_D("NetworkAddRouteParcel %{public}d", result);
    return result;
}

int32_t NetsysNativeService::NetworkRemoveRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo)
{
    int32_t result = netsysService_->NetworkRemoveRouteParcel(netId, routeInfo);
    NETNATIVE_LOG_D("NetworkRemoveRouteParcel %{public}d", result);
    return result;
}

int32_t NetsysNativeService::NetworkSetDefault(int32_t netId)
{
    NETNATIVE_LOG_D("NetworkSetDefault in.");
    int32_t result = netsysService_->NetworkSetDefault(netId);
    NETNATIVE_LOG_D("NetworkSetDefault out.");
    return result;
}

int32_t NetsysNativeService::NetworkGetDefault()
{
    int32_t result = netsysService_->NetworkGetDefault();
    NETNATIVE_LOG_D("NetworkGetDefault");
    return result;
}

int32_t NetsysNativeService::NetworkClearDefault()
{
    int32_t result = netsysService_->NetworkClearDefault();
    NETNATIVE_LOG_D("NetworkClearDefault");
    return result;
}

int32_t NetsysNativeService::GetProcSysNet(int32_t family, int32_t which, const std::string &ifname,
                                           const std::string &parameter, std::string &value)
{
    int32_t result = netsysService_->GetProcSysNet(family, which, ifname, parameter, &value);
    NETNATIVE_LOG_D("GetProcSysNet");
    return result;
}

int32_t NetsysNativeService::SetProcSysNet(int32_t family, int32_t which, const std::string &ifname,
                                           const std::string &parameter, std::string &value)
{
    int32_t result = netsysService_->SetProcSysNet(family, which, ifname, parameter, value);
    NETNATIVE_LOG_D("SetProcSysNet");
    return result;
}

int32_t NetsysNativeService::SetInternetPermission(uint32_t uid, uint8_t allow, uint8_t isBroker)
{
    int32_t result = netsysService_->SetInternetPermission(uid, allow, isBroker);
    NETNATIVE_LOG_D("SetInternetPermission out.");
    return result;
}

int32_t NetsysNativeService::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    int32_t result = netsysService_->NetworkCreatePhysical(netId, permission);
    NETNATIVE_LOG_D("NetworkCreatePhysical out.");
    return result;
}

int32_t NetsysNativeService::NetworkCreateVirtual(int32_t netId, bool hasDns)
{
    int32_t result = netsysService_->NetworkCreateVirtual(netId, hasDns);
    NETNATIVE_LOG_D("NetworkCreateVirtual out.");
    return result;
}

int32_t NetsysNativeService::NetworkAddUids(int32_t netId, const std::vector<UidRange> &uidRanges)
{
    int32_t result = netsysService_->NetworkAddUids(netId, uidRanges);
    NETNATIVE_LOG_D("NetworkAddUids out.");
    return result;
}

int32_t NetsysNativeService::NetworkDelUids(int32_t netId, const std::vector<UidRange> &uidRanges)
{
    int32_t result = netsysService_->NetworkDelUids(netId, uidRanges);
    NETNATIVE_LOG_D("NetworkDelUids out.");
    return result;
}

int32_t NetsysNativeService::AddInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                                 int32_t prefixLength)
{
    int32_t result = netsysService_->AddInterfaceAddress(interfaceName, addrString, prefixLength);
    NETNATIVE_LOG_D("AddInterfaceAddress");
    return result;
}

int32_t NetsysNativeService::DelInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                                 int32_t prefixLength)
{
    int32_t result = netsysService_->DelInterfaceAddress(interfaceName, addrString, prefixLength);
    NETNATIVE_LOG_D("DelInterfaceAddress");
    return result;
}

int32_t NetsysNativeService::DelInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                                 int32_t prefixLength, int socketType)
{
    int32_t result = netsysService_->DelInterfaceAddress(interfaceName, addrString, prefixLength, socketType);
    NETNATIVE_LOG_D("DelInterfaceAddress");
    return result;
}

int32_t NetsysNativeService::InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    NETNATIVE_LOG_D("InterfaceSetIpAddress");
    return netsysService_->InterfaceSetIpAddress(ifaceName, ipAddress);
}

int32_t NetsysNativeService::InterfaceSetIffUp(const std::string &ifaceName)
{
    NETNATIVE_LOG_D("InterfaceSetIffUp");
    return netsysService_->InterfaceSetIffUp(ifaceName);
}

int32_t NetsysNativeService::NetworkAddInterface(int32_t netId, const std::string &iface, NetBearType netBearerType)
{
    NETNATIVE_LOG_D("NetworkAddInterface");
    int32_t result = netsysService_->NetworkAddInterface(netId, iface, netBearerType);
    return result;
}

int32_t NetsysNativeService::NetworkRemoveInterface(int32_t netId, const std::string &iface)
{
    int32_t result = netsysService_->NetworkRemoveInterface(netId, iface);
    NETNATIVE_LOG_D("NetworkRemoveInterface");
    return result;
}

int32_t NetsysNativeService::NetworkDestroy(int32_t netId, bool isVpnNet)
{
    int32_t result = netsysService_->NetworkDestroy(netId, isVpnNet);
    NETNATIVE_LOG_D("NetworkDestroy");
    return result;
}

int32_t NetsysNativeService::CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix,
                                        const std::set<int32_t> &uids)
{
    int32_t result = netsysService_->CreateVnic(mtu, tunAddr, prefix, uids);
    NETNATIVE_LOG_D("CreateVnic");
    return result;
}

int32_t NetsysNativeService::DestroyVnic()
{
    int32_t result = netsysService_->DestroyVnic();
    NETNATIVE_LOG_D("DestroyVnic");
    return result;
}

int32_t NetsysNativeService::EnableDistributedClientNet(const std::string &virnicAddr,
    const std::string &virnicName, const std::string &iif)
{
    if (virnicAddr.empty() || virnicName.empty() || iif.empty()) {
        NETNATIVE_LOGE("EnableDistributedClientNet param is empty.");
        return NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER;
    }
    int32_t result = netsysService_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    NETNATIVE_LOGI("EnableDistributedClientNet");
    return result;
}

int32_t NetsysNativeService::EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                                        const std::string &dstAddr, const std::string &gw)
{
    if (iif.empty() || dstAddr.empty()) {
        NETNATIVE_LOGE("EnableDistributedServerNet param is empty.");
        return NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER;
    }
    int32_t result = netsysService_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    NETNATIVE_LOGI("EnableDistributedServerNet");
    return result;
}

int32_t NetsysNativeService::DisableDistributedNet(
    bool isServer, const std::string &virnicName, const std::string &dstAddr)
{
    int32_t result = netsysService_->DisableDistributedNet(isServer, virnicName, dstAddr);
    NETNATIVE_LOGI("DisableDistributedNet");
    return result;
}

int32_t NetsysNativeService::GetFwmarkForNetwork(int32_t netId, MarkMaskParcel &markMaskParcel)
{
    markMaskParcel = netsysService_->GetFwmarkForNetwork(netId);
    NETNATIVE_LOG_D("GetFwmarkForNetwork");
    return ERR_NONE;
}

int32_t NetsysNativeService::SetInterfaceConfig(const InterfaceConfigurationParcel &cfg)
{
    NETNATIVE_LOG_D("SetInterfaceConfig");
    netsysService_->SetInterfaceConfig(cfg);
    return ERR_NONE;
}

int32_t NetsysNativeService::GetInterfaceConfig(InterfaceConfigurationParcel &cfg)
{
    NETNATIVE_LOG_D("GetInterfaceConfig");
    std::string ifName = cfg.ifName;
    cfg = netsysService_->GetInterfaceConfig(ifName);
    NETNATIVE_LOG_D("GetInterfaceConfig end");
    return ERR_NONE;
}

int32_t NetsysNativeService::InterfaceGetList(std::vector<std::string> &ifaces)
{
    NETNATIVE_LOG_D("InterfaceGetList");
    ifaces = netsysService_->InterfaceGetList();
    return ERR_NONE;
}

int32_t NetsysNativeService::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETNATIVE_LOG_D("StartDhcpClient");
    dhcpController_->StartClient(iface, bIpv6);
    return ERR_NONE;
}

int32_t NetsysNativeService::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETNATIVE_LOG_D("StopDhcpClient");
    dhcpController_->StopClient(iface, bIpv6);
    return ERR_NONE;
}

int32_t NetsysNativeService::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    NETNATIVE_LOG_D("StartDhcpService");
    dhcpController_->StartDhcpService(iface, ipv4addr);
    return ERR_NONE;
}

int32_t NetsysNativeService::StopDhcpService(const std::string &iface)
{
    NETNATIVE_LOG_D("StopDhcpService");
    dhcpController_->StopDhcpService(iface);
    return ERR_NONE;
}

int32_t NetsysNativeService::IpEnableForwarding(const std::string &requester)
{
    NETNATIVE_LOG_D("ipEnableForwarding");
    return netsysService_->IpEnableForwarding(requester);
}

int32_t NetsysNativeService::IpDisableForwarding(const std::string &requester)
{
    NETNATIVE_LOG_D("ipDisableForwarding");
    return netsysService_->IpDisableForwarding(requester);
}

int32_t NetsysNativeService::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETNATIVE_LOG_D("enableNat");
    return netsysService_->EnableNat(downstreamIface, upstreamIface);
}

int32_t NetsysNativeService::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETNATIVE_LOG_D("disableNat");
    return netsysService_->DisableNat(downstreamIface, upstreamIface);
}

int32_t NetsysNativeService::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    NETNATIVE_LOG_D("ipfwdAddInterfaceForward");
    return netsysService_->IpfwdAddInterfaceForward(fromIface, toIface);
}

int32_t NetsysNativeService::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    NETNATIVE_LOG_D("ipfwdRemoveInterfaceForward");
    return netsysService_->IpfwdRemoveInterfaceForward(fromIface, toIface);
}

int32_t NetsysNativeService::BandwidthEnableDataSaver(bool enable)
{
    NETNATIVE_LOG_D("bandwidthEnableDataSaver");
    return netsysService_->BandwidthEnableDataSaver(enable);
}

int32_t NetsysNativeService::BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    NETNATIVE_LOG_D("BandwidthSetIfaceQuota");
    return netsysService_->BandwidthSetIfaceQuota(ifName, bytes);
}

int32_t NetsysNativeService::BandwidthRemoveIfaceQuota(const std::string &ifName)
{
    NETNATIVE_LOG_D("BandwidthRemoveIfaceQuota");
    return netsysService_->BandwidthRemoveIfaceQuota(ifName);
}

int32_t NetsysNativeService::BandwidthAddDeniedList(uint32_t uid)
{
    NETNATIVE_LOG_D("BandwidthAddDeniedList");
    return netsysService_->BandwidthAddDeniedList(uid);
}

int32_t NetsysNativeService::BandwidthRemoveDeniedList(uint32_t uid)
{
    NETNATIVE_LOG_D("BandwidthRemoveDeniedList");
    return netsysService_->BandwidthRemoveDeniedList(uid);
}

int32_t NetsysNativeService::BandwidthAddAllowedList(uint32_t uid)
{
    NETNATIVE_LOG_D("BandwidthAddAllowedList");
    return netsysService_->BandwidthAddAllowedList(uid);
}

int32_t NetsysNativeService::BandwidthRemoveAllowedList(uint32_t uid)
{
    NETNATIVE_LOG_D("BandwidthRemoveAllowedList");
    return netsysService_->BandwidthRemoveAllowedList(uid);
}

int32_t NetsysNativeService::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    NETNATIVE_LOG_D("FirewallSetUidsAllowedListChain");
    return netsysService_->FirewallSetUidsAllowedListChain(chain, uids);
}

int32_t NetsysNativeService::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    NETNATIVE_LOG_D("FirewallSetUidsDeniedListChain");
    return netsysService_->FirewallSetUidsDeniedListChain(chain, uids);
}

int32_t NetsysNativeService::FirewallEnableChain(uint32_t chain, bool enable)
{
    NETNATIVE_LOG_D("FirewallEnableChain");
    return netsysService_->FirewallEnableChain(chain, enable);
}

int32_t NetsysNativeService::FirewallSetUidRule(uint32_t chain, const std::vector<uint32_t> &uids,
                                                uint32_t firewallRule)
{
    NETNATIVE_LOG_D("firewallSetUidRule");
    return netsysService_->FirewallSetUidRule(chain, uids, firewallRule);
}

int32_t NetsysNativeService::ShareDnsSet(uint16_t netid)
{
    NETNATIVE_LOG_D("NetsysNativeService ShareDnsSet");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return -1;
    }
    netsysService_->ShareDnsSet(netid);
    return ERR_NONE;
}

int32_t NetsysNativeService::StartDnsProxyListen()
{
    NETNATIVE_LOG_D("NetsysNativeService StartDnsProxyListen");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return -1;
    }
    netsysService_->StartDnsProxyListen();
    return ERR_NONE;
}

int32_t NetsysNativeService::StopDnsProxyListen()
{
    NETNATIVE_LOG_D("NetsysNativeService StopDnsProxyListen");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return -1;
    }
    netsysService_->StopDnsProxyListen();
    return ERR_NONE;
}

int32_t NetsysNativeService::GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                                      NetworkSharingTraffic &traffic)
{
    if (sharingManager_ == nullptr) {
        NETNATIVE_LOGE("manager is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    return sharingManager_->GetNetworkSharingTraffic(downIface, upIface, traffic);
}

void NetsysNativeService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    NETNATIVE_LOGI("OnAddSystemAbility systemAbilityId[%{public}d]", systemAbilityId);
    if (systemAbilityId == COMM_NET_CONN_MANAGER_SYS_ABILITY_ID) {
        if (!hasSARemoved_) {
            hasSARemoved_ = true;
            return;
        }
        OnNetManagerRestart();
    }
}

int32_t NetsysNativeService::GetNetworkCellularSharingTraffic(NetworkSharingTraffic &traffic, std::string &ifaceName)
{
    if (sharingManager_ == nullptr) {
        NETNATIVE_LOGE("manager is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    return sharingManager_->GetNetworkCellularSharingTraffic(traffic, ifaceName);
}

int32_t NetsysNativeService::SetDpaCellularSharingTraffic(NetworkDpaTrafficReport &traffic)
{
    if (sharingManager_ == nullptr) {
        NETNATIVE_LOGE("manager is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    return sharingManager_->SetDpaCellularSharingTraffic(traffic);
}

void NetsysNativeService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    NETNATIVE_LOGI("OnRemoveSystemAbility systemAbilityId[%{public}d]", systemAbilityId);
    if (systemAbilityId == COMM_NET_CONN_MANAGER_SYS_ABILITY_ID) {
        OnNetManagerRestart();
        hasSARemoved_ = true;
#ifdef FEATURE_NET_FIREWALL_ENABLE
    } else if (systemAbilityId == COMM_FIREWALL_MANAGER_SYS_ABILITY_ID) {
        bpfNetFirewall_->LoadSystemAbility(COMM_FIREWALL_MANAGER_SYS_ABILITY_ID);
#endif
    }
}

int32_t NetsysNativeService::GetTotalStats(uint64_t &stats, uint32_t type)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->GetTotalStats(stats, static_cast<OHOS::NetManagerStandard::StatsType>(type));
}

int32_t NetsysNativeService::GetUidStats(uint64_t &stats, uint32_t type, uint32_t uid)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->GetUidStats(stats, static_cast<OHOS::NetManagerStandard::StatsType>(type), uid);
}

int32_t NetsysNativeService::GetIfaceStats(uint64_t &stats, uint32_t type, const std::string &interfaceName)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->GetIfaceStats(stats, static_cast<OHOS::NetManagerStandard::StatsType>(type), interfaceName);
}

int32_t NetsysNativeService::SetNetStateTrafficMap(uint8_t flag, uint64_t availableTraffic)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->SetNetStateTrafficMap(flag, availableTraffic);
}

int32_t NetsysNativeService::GetNetStateTrafficMap(uint8_t flag, uint64_t &availableTraffic)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->GetNetStateTrafficMap(flag, availableTraffic);
}

int32_t NetsysNativeService::UpdateIfIndexMap(int8_t key, uint64_t index)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->UpdateIfIndexMap(key, index);
}

int32_t NetsysNativeService::ClearIncreaseTrafficMap()
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->ClearIncreaseTrafficMap();
}

int32_t NetsysNativeService::SetNetStatusMap(uint8_t type, uint8_t value)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->SetNetStatusMap(type, value);
}

int32_t NetsysNativeService::ClearSimStatsBpfMap()
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
 
    return bpfStats_->ClearSimStatsBpfMap();
}

int32_t NetsysNativeService::DeleteIncreaseTrafficMap(uint64_t ifIndex)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
 
    return bpfStats_->DeleteIncreaseTrafficMap(ifIndex);
}

int32_t NetsysNativeService::GetAllSimStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    return bpfStats_->GetAllSimStatsInfo(stats);
}

int32_t NetsysNativeService::DeleteSimStatsInfo(uint32_t uid)
{
    NETNATIVE_LOGI("DeleteSimStatsInfo uid[%{public}u]", uid);
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    return bpfStats_->DeleteStatsInfo(APP_UID_SIM_STATS_MAP_PATH, uid);
}

int32_t NetsysNativeService::GetAllStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->GetAllStatsInfo(stats);
}

int32_t NetsysNativeService::DeleteStatsInfo(uint32_t uid)
{
    NETNATIVE_LOGI("DeleteStatsInfo uid[%{public}u]", uid);
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    return bpfStats_->DeleteStatsInfo(APP_UID_IF_STATS_MAP_PATH, uid);
}

int32_t NetsysNativeService::SetIptablesCommandForRes(const std::string &cmd, std::string &respond, IptablesType ipType)
{
    if (!regex_match(cmd, REGEX_CMD_IPTABLES)) {
        NETNATIVE_LOGE("IptablesWrapper command format is invalid");
        return NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER;
    }
    if (iptablesWrapper_ == nullptr) {
        NETNATIVE_LOGE("SetIptablesCommandForRes iptablesWrapper_ is null");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    switch (ipType) {
        case IptablesType::IPTYPE_IPV4:
            respond = iptablesWrapper_->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV4, cmd);
            break;
        case IptablesType::IPTYPE_IPV6:
            respond = iptablesWrapper_->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV6, cmd);
            break;
        case IptablesType::IPTYPE_IPV4V6:
            respond = iptablesWrapper_->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV4V6, cmd);
            break;
        default:
            NETNATIVE_LOGE("IptablesWrapper ipputType is invalid");
            return NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeService::SetIpCommandForRes(const std::string &cmd, std::string &respond)
{
    if (netDiagWrapper == nullptr) {
        NETNATIVE_LOGE("SetIpCommandForRes netDiagWrapper is null");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    netDiagWrapper->ExecuteCommandForResult(cmd, respond);
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeService::NetDiagPingHost(const NetDiagPingOption &pingOption,
                                             const sptr<INetDiagCallback> &callback)
{
    if (netDiagWrapper == nullptr) {
        NETNATIVE_LOGE("netDiagWrapper is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netDiagWrapper->PingHost(pingOption, callback);
}

int32_t NetsysNativeService::NetDiagGetRouteTable(std::list<NetDiagRouteTable> &routeTables)
{
    if (netDiagWrapper == nullptr) {
        NETNATIVE_LOGE("netDiagWrapper is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netDiagWrapper->GetRouteTable(routeTables);
}

int32_t NetsysNativeService::NetDiagGetSocketsInfo(NetDiagProtocolType socketType, NetDiagSocketsInfo &socketsInfo)
{
    if (netDiagWrapper == nullptr) {
        NETNATIVE_LOGE("netDiagWrapper is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netDiagWrapper->GetSocketsInfo(socketType, socketsInfo);
}

int32_t NetsysNativeService::NetDiagGetInterfaceConfig(std::list<NetDiagIfaceConfig> &configs,
                                                       const std::string &ifaceName)
{
    if (netDiagWrapper == nullptr) {
        NETNATIVE_LOGE("netDiagWrapper is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netDiagWrapper->GetInterfaceConfig(configs, ifaceName);
}

int32_t NetsysNativeService::NetDiagUpdateInterfaceConfig(const NetDiagIfaceConfig &config,
                                                          const std::string &ifaceName, bool add)
{
    if (netDiagWrapper == nullptr) {
        NETNATIVE_LOGE("netDiagWrapper is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netDiagWrapper->UpdateInterfaceConfig(config, ifaceName, add);
}

int32_t NetsysNativeService::NetDiagSetInterfaceActiveState(const std::string &ifaceName, bool up)
{
    if (netDiagWrapper == nullptr) {
        NETNATIVE_LOGE("netDiagWrapper is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netDiagWrapper->SetInterfaceActiveState(ifaceName, up);
}

int32_t NetsysNativeService::AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                          const std::string &ifName)
{
    NETNATIVE_LOG_D("AddStaticArp");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->AddStaticArp(ipAddr, macAddr, ifName);
}

int32_t NetsysNativeService::DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                          const std::string &ifName)
{
    NETNATIVE_LOG_D("DelStaticArp");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->DelStaticArp(ipAddr, macAddr, ifName);
}

int32_t NetsysNativeService::AddStaticIpv6Addr(const std::string &ipAddr, const std::string &macAddr,
    const std::string &ifName)
{
    NETNATIVE_LOG_D("AddStaticIpv6Addr");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
}

int32_t NetsysNativeService::DelStaticIpv6Addr(const std::string &ipAddr, const std::string &macAddr,
    const std::string &ifName)
{
    NETNATIVE_LOG_D("DelStaticIpv6Addr");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
}

int32_t NetsysNativeService::GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo)
{
    NETNATIVE_LOG_D("NetsysNativeService GetIpNeighTable");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->GetIpNeighTable(ipMacInfo);
}

int32_t NetsysNativeService::GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo,
                                                int32_t &ownerUid)
{
    NETNATIVE_LOG_D("NetsysNativeService GetConnectOwnerUid");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->GetConnectOwnerUid(netConnInfo, ownerUid);
}

int32_t NetsysNativeService::GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo)
{
    NETNATIVE_LOG_D("NetsysNativeService GetSystemNetPortStates");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->GetSystemNetPortStates(netPortStatesInfo);
}

int32_t NetsysNativeService::RegisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback, uint32_t timeStep)
{
    return netsysService_->RegisterDnsResultCallback(callback, timeStep);
}

int32_t NetsysNativeService::UnregisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback)
{
    return netsysService_->UnregisterDnsResultCallback(callback);
}

int32_t NetsysNativeService::SetIpv6PrivacyExtensions(const std::string &interfaceName, const uint32_t on)
{
    int32_t result = netsysService_->SetIpv6PrivacyExtensions(interfaceName, on);
    NETNATIVE_LOG_D("SetIpv6PrivacyExtensions");
    return result;
}
int32_t NetsysNativeService::SetEnableIpv6(const std::string &interfaceName, const uint32_t on, bool needRestart)
{
    int32_t result = netsysService_->SetEnableIpv6(interfaceName, on, needRestart);
    NETNATIVE_LOG_D("SetEnableIpv6");
    return result;
}

int32_t NetsysNativeService::SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid)
{
    int32_t result = netsysService_->SetIpv6UidBlackList(netIds, uid);
    NETNATIVE_LOG_D("SetIpv6UidBlackList result=%{public}d", result);
    return result;
}

int32_t NetsysNativeService::SetIpv6AutoConf(const std::string &interfaceName, const uint32_t on)
{
    int32_t result = netsysService_->SetIpv6AutoConf(interfaceName, on);
    NETNATIVE_LOG_D("SetIpv6AutoConf");
    return result;
}

int32_t NetsysNativeService::GetCookieStats(uint64_t &stats, uint32_t type, uint64_t cookie)
{
    if (bpfStats_ == nullptr) {
        NETNATIVE_LOGE("bpfStats is null.");
        return NetManagerStandard::NETMANAGER_ERROR;
    }

    return bpfStats_->GetCookieStats(stats, static_cast<OHOS::NetManagerStandard::StatsType>(type), cookie);
}

int32_t NetsysNativeService::GetNetworkSharingType(std::set<uint32_t>& sharingTypeIsOn)
{
    NETNATIVE_LOGI("GetNetworkSharingType");
    std::lock_guard<std::mutex> guard(instanceLock_);
    sharingTypeIsOn = sharingTypeIsOn_;
    return NETSYS_SUCCESS;
}

int32_t NetsysNativeService::UpdateNetworkSharingType(uint32_t type, bool isOpen)
{
    NETNATIVE_LOGI("UpdateNetworkSharingType");
    std::lock_guard<std::mutex> guard(instanceLock_);
    if (isOpen) {
        sharingTypeIsOn_.insert(type);
    } else {
        sharingTypeIsOn_.erase(type);
    }
    return NETSYS_SUCCESS;
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
int32_t NetsysNativeService::SetFirewallRules(NetFirewallRuleType type,
                                              const std::vector<sptr<NetFirewallBaseRule>> &ruleList, bool isFinish)
{
    NETNATIVE_LOGI("NetsysNativeService::SetFirewallRules: size=%{public}zu isFinish=%{public}" PRId32, ruleList.size(),
                   isFinish);
    int32_t ret = bpfNetFirewall_->SetFirewallRules(type, ruleList, isFinish);
    if (ret != NETSYS_SUCCESS) {
        NETNATIVE_LOGI("NetsysNativeService::SetFirewallRules: error ret:%{public}d", ret);
        return ret;
    }
    ret = netsysService_->SetFirewallRules(type, ruleList, isFinish);
    return ret;
}

int32_t NetsysNativeService::SetFirewallDefaultAction(int32_t userId, FirewallRuleAction inDefault,
    FirewallRuleAction outDefault)
{
    NETNATIVE_LOGI("NetsysNativeService::SetFirewallDefaultAction");
    int32_t ret = netsysService_->SetFirewallDefaultAction(inDefault, outDefault);
    ret += bpfNetFirewall_->SetFirewallDefaultAction(userId, inDefault, outDefault);
    return ret;
}

int32_t NetsysNativeService::SetFirewallCurrentUserId(int32_t userId)
{
    NETNATIVE_LOGI("NetsysNativeService::SetFirewallCurrentUserId");
    int32_t ret = netsysService_->SetFirewallCurrentUserId(userId);
    ret += bpfNetFirewall_->SetFirewallCurrentUserId(userId);
    return ret;
}

int32_t NetsysNativeService::ClearFirewallRules(NetFirewallRuleType type)
{
    NETNATIVE_LOGI("NetsysNativeService::ClearFirewallRules");
    int32_t ret = NETSYS_SUCCESS;
    switch (type) {
        case NetFirewallRuleType::RULE_IP:
        case NetFirewallRuleType::RULE_DOMAIN:
            ret = bpfNetFirewall_->ClearFirewallRules(type);
            break;
        case NetFirewallRuleType::RULE_DNS:
            ret = netsysService_->ClearFirewallRules(type);
            break;
        case NetFirewallRuleType::RULE_ALL:
            ret = bpfNetFirewall_->ClearFirewallRules(NetFirewallRuleType::RULE_ALL);
            ret += netsysService_->ClearFirewallRules(NetFirewallRuleType::RULE_ALL);
            break;
        default:
            break;
    }
    return ret;
}

int32_t NetsysNativeService::RegisterNetFirewallCallback(const sptr<INetFirewallCallback> &callback)
{
    NETNATIVE_LOGI("NetsysNativeService::RegisterNetFirewallCallback");
    int32_t ret = netsysService_->RegisterNetFirewallCallback(callback);
    ret += bpfNetFirewall_->RegisterCallback(callback);
    return ret;
}

int32_t NetsysNativeService::UnRegisterNetFirewallCallback(const sptr<INetFirewallCallback> &callback)
{
    NETNATIVE_LOGI("NetsysNativeService::UnRegisterNetFirewallCallback");
    int32_t ret = netsysService_->UnRegisterNetFirewallCallback(callback);
    ret += bpfNetFirewall_->UnregisterCallback(callback);
    return ret;
}
#endif

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
int32_t NetsysNativeService::EnableWearableDistributedNetForward(const int32_t tcpPortId, const int32_t udpPortId)
{
    NETNATIVE_LOGI("Enabling wearable distributed net forward for TCP port and UDP port");
    return netsysService_->EnableWearableDistributedNetForward(tcpPortId, udpPortId);
}

int32_t NetsysNativeService::DisableWearableDistributedNetForward()
{
    NETNATIVE_LOGI("NetsysNativeService Disable Wearable Distributed NetForward");
    return netsysService_->DisableWearableDistributedNetForward();
}
#endif

int32_t NetsysNativeService::SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag)
{
    NETNATIVE_LOGI("SetNetworkAccessPolicy");

    return netsysService_->SetNetworkAccessPolicy(uid, policy, reconfirmFlag);
}

int32_t NetsysNativeService::DeleteNetworkAccessPolicy(uint32_t uid)
{
    NETNATIVE_LOGI("DeleteNetworkAccessPolicy");
    return netsysService_->DeleteNetworkAccessPolicy(uid);
}

int32_t NetsysNativeService::NotifyNetBearerTypeChange(std::set<NetBearType> bearerTypes)
{
    NETNATIVE_LOG_D("NotifyNetBearerTypeChange");
    return netsysService_->NotifyNetBearerTypeChange(bearerTypes);
}

int32_t NetsysNativeService::StartClat(const std::string &interfaceName, int32_t netId,
                                       const std::string &nat64PrefixStr)
{
    int32_t result = clatManager_->ClatStart(interfaceName, netId, nat64PrefixStr, netsysService_.get());
    NETNATIVE_LOG_D("StartClat");
    return result;
}

int32_t NetsysNativeService::StopClat(const std::string &interfaceName)
{
    int32_t result = clatManager_->ClatStop(interfaceName, netsysService_.get());
    NETNATIVE_LOG_D("StartClat");
    return result;
}

int32_t NetsysNativeService::ClearFirewallAllRules()
{
    NETNATIVE_LOG_D("ClearFirewallAllRules");
    return netsysService_->ClearFirewallAllRules();
}

int32_t NetsysNativeService::SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool allowed)
{
    if (iptablesWrapper_ == nullptr) {
        NETNATIVE_LOGE("SetNicTrafficAllowed iptablesWrapper_ is null");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    bool ret = false;
    std::vector<std::string> cmds;
    for (const std::string& ifaceName : ifaceNames) {
        if (allowed) {
            NETNATIVE_LOG_D("SetNicTrafficAllowed %{public}s allowed", ifaceName.c_str());
            cmds.push_back("-t raw -D OUTPUT -o " + ifaceName + " -j DROP");
            cmds.push_back("-t raw -D PREROUTING -i " + ifaceName + " -j DROP");
        } else {
            NETNATIVE_LOG_D("SetNicTrafficAllowed %{public}s disallowed", ifaceName.c_str());
            cmds.push_back("-t raw -I OUTPUT -o " + ifaceName + " -j DROP");
            cmds.push_back("-t raw -I PREROUTING -i " + ifaceName + " -j DROP");
        }
    }
    ret = IptablesWrapper::GetInstance()->RunMutipleCommands(OHOS::nmd::IpType::IPTYPE_IPV4V6, cmds);
    if (ret) {
        NETNATIVE_LOGE("SetNicTrafficAllowed iptablesWrapper_ apply failed");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    NETNATIVE_LOG_D("SetNicTrafficAllowed iptablesWrapper_ apply success");
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

#ifdef SUPPORT_SYSVPN
int32_t NetsysNativeService::ProcessVpnStage(NetsysNative::SysVpnStageCode stage, const std::string &message)
{
    NETNATIVE_LOGI("ProcessVpnStage stage %{public}d", stage);
    if (SystemVpnWrapper::GetInstance() == nullptr) {
        NETNATIVE_LOGE("ProcessVpnStage SystemVpnWrapper is null");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    int32_t ret = SystemVpnWrapper::GetInstance()->Update(stage, message);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("ProcessVpnStage failed");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeService::UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add)
{
    NETNATIVE_LOGI("UpdateVpnRules netId %{public}d", netId);
    int32_t result = netsysService_->UpdateVpnRules(netId, extMessages, add);
    NETNATIVE_LOG_D("UpdateVpnRules out.");
    return result;
}
#endif // SUPPORT_SYSVPN

int32_t NetsysNativeService::CloseSocketsUid(const std::string &ipAddr, uint32_t uid)
{
    NETNATIVE_LOGI("CloseSocketsUid uid[%{public}d]", uid);
    return netsysService_->CloseSocketsUid(ipAddr, uid);
}

int32_t NetsysNativeService::SetBrokerUidAccessPolicyMap(const std::unordered_map<uint32_t, uint32_t> &uidMaps)
{
    NETNATIVE_LOGI("SetBrokerUidAccessPolicyMap Enter");
    if (uidMaps.size() == 0) {
        return NetManagerStandard::NETSYS_SUCCESS;
    }
    BpfMapper<app_uid_key, app_uid_key> brokerUidAccessPolicyMap(BROKER_UID_ACCESS_POLICY_MAP_PATH, BPF_ANY);
    if (!brokerUidAccessPolicyMap.IsValid()) {
        NETNATIVE_LOGE("invalid map");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    for (auto iter = uidMaps.begin(); iter != uidMaps.end(); ++iter) {
        app_uid_key k = {0};
        k = iter->first;
        app_uid_key v = {0};
        v = iter->second;
        auto ret = brokerUidAccessPolicyMap.Write(k, v, BPF_ANY);
        if (ret < 0) {
            NETNATIVE_LOGE("Write map err. ret[%{public}d], item[%{public}u, %{public}u]", ret, k, v);
        }
    }
    return NetManagerStandard::NETSYS_SUCCESS;
}

int32_t NetsysNativeService::DelBrokerUidAccessPolicyMap(uint32_t uid)
{
    NETNATIVE_LOGI("DelBrokerUidAccessPolicyMap Enter");
    BpfMapper<app_uid_key, app_uid_key> brokerUidAccessPolicyMap(BROKER_UID_ACCESS_POLICY_MAP_PATH, BPF_F_WRONLY);
    if (!brokerUidAccessPolicyMap.IsValid()) {
        NETNATIVE_LOGE("invalid map");
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    auto ret = brokerUidAccessPolicyMap.Delete(uid);
    if (ret != 0) {
        NETNATIVE_LOGE("Delete map err. ret[%{public}d]", ret);
        return NetManagerStandard::NETMANAGER_ERROR;
    }
    return NetManagerStandard::NETSYS_SUCCESS;
}

int32_t NetsysNativeService::SetUserDefinedServerFlag(uint16_t netId, bool flag)
{
    netsysService_->DnsSetUserDefinedServerFlag(netId, flag);
    return NetManagerStandard::NETSYS_SUCCESS;
}

int32_t NetsysNativeService::FlushDnsCache(uint16_t netId)
{
    NETNATIVE_LOG_D("FlushDnsCache");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->FlushDnsCache(netId);
}

int32_t NetsysNativeService::SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo)
{
    NETNATIVE_LOG_D("SetDnsCache");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->SetDnsCache(netId, hostName, addrInfo);
}
#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
int32_t NetsysNativeService::UpdateEnterpriseRoute(const std::string &interfaceName, uint32_t uid, bool add)
{
    NETNATIVE_LOG_D("UpdateEnterpriseRoute");
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->UpdateEnterpriseRoute(interfaceName, uid, add);
}
#endif

int32_t NetsysNativeService::CreateVlan(const std::string &ifName, uint32_t vlanId)
{
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->CreateVlan(ifName, vlanId);
}

int32_t NetsysNativeService::DestroyVlan(const std::string &ifName, uint32_t vlanId)
{
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->DestroyVlan(ifName, vlanId);
}

int32_t NetsysNativeService::AddVlanIp(const std::string &ifName, uint32_t vlanId,
                                       const std::string &ip, uint32_t mask)
{
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->AddVlanIp(ifName, vlanId, ip, mask);
}

int32_t NetsysNativeService::SetInternetAccessByIpForWifiShare(
    const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName)
{
    NETNATIVE_LOGI("SetInternetAccessByIpForWifiShare addr[%{public}s], access[%{public}d], IfName[%{public}s]",
        NetManagerStandard::CommonUtils::ToAnonymousIp(ipAddr, true).c_str(), accessInternet, clientNetIfName.c_str());
    if (netsysService_ == nullptr) {
        NETNATIVE_LOGE("netsysService_ is null");
        return NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netsysService_->SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
}
} // namespace NetsysNative
} // namespace OHOS

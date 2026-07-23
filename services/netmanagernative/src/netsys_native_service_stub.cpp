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

#include <cstdlib>
#include <net/route.h>
#include <netdb.h>
#include <unistd.h>

#include "ipc_skeleton.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netmanager_base_permission.h"
#include "netnative_log_wrapper.h"
#include "netsys_native_service_stub.h"
#include "securec.h"
#include "i_net_dns_result_callback.h"
#include "netsys_traffic_callback_proxy.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
namespace OHOS {
namespace NetsysNative {
namespace {
constexpr int32_t MAX_VNIC_UID_ARRAY_SIZE = 20;
constexpr int32_t MAX_FLAG_NUM = 64;
constexpr int32_t MAX_DNS_CONFIG_SIZE = 7;
constexpr int32_t NETMANAGER_ERR_PERMISSION_DENIED = 201;
constexpr uint32_t UIDS_LIST_MAX_SIZE = 1024;
constexpr uint32_t MAX_UID_ARRAY_SIZE = 1024;
constexpr uint32_t MAX_CONFIG_LIST_SIZE = 1024;
constexpr uint32_t MAX_ROUTE_TABLE_SIZE = 128;
constexpr uint32_t MAX_ROUTES_ARRAY_SIZE = 1024;
constexpr uint32_t MAX_IFACENAMES_SIZE = 128;
constexpr uint32_t MAX_SHARING_TYPE_SIZE = 32;
constexpr int32_t INVALID_UID = -1;
} // namespace

NetsysNativeServiceStub::NetsysNativeServiceStub()
{
    InitNetInfoOpToInterfaceMap();
    InitBandwidthOpToInterfaceMap();
    InitBandwidthOpToInterfaceMapExt();
    InitFirewallOpToInterfaceMap();
    InitOpToInterfaceMapExt();
    InitVlanInterfaceMap();
    InitNetDiagOpToInterfaceMap();
    InitNetDnsDiagOpToInterfaceMap();
    InitStaticArpToInterfaceMap();
    InitNetVnicInterfaceMap();
    InitNetVirnicInterfaceMap();
    InitNetStatsInterfaceMap();
    InitStaticIpv6ToInterfaceMap();
#ifdef SUPPORT_SYSVPN
    InitVpnOpToInterfaceMap();
#endif // SUPPORT_SYSVPN
    InitDnsServerOpToInterfaceMap();
#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
    InitEnterpriseMap();
#endif
    uids_ = {UID_ROOT, UID_HIVIEW, UID_SHELL, UID_NET_MANAGER, UID_WIFI, UID_RADIO, UID_HIDUMPER_SERVICE,
        UID_SAMGR, UID_PARAM_WATCHER, UID_EDM, UID_SECURITY_GUARD, UID_IOT_NET_MANAGER, UID_IOT_TV_NET_MANAGER};
}

void NetsysNativeServiceStub::InitNetInfoOpToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_RESOLVER_CONFIG)] =
        &NetsysNativeServiceStub::CmdSetResolverConfig;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_RESOLVER_CONFIG)] =
        &NetsysNativeServiceStub::CmdGetResolverConfig;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_CREATE_NETWORK_CACHE)] =
        &NetsysNativeServiceStub::CmdCreateNetworkCache;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DESTROY_NETWORK_CACHE)] =
        &NetsysNativeServiceStub::CmdDestroyNetworkCache;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_ADDR_INFO)] =
        &NetsysNativeServiceStub::CmdGetAddrInfo;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_SET_MTU)] =
        &NetsysNativeServiceStub::CmdSetInterfaceMtu;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_GET_MTU)] =
        &NetsysNativeServiceStub::CmdGetInterfaceMtu;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_TCP_BUFFER_SIZES)] =
        &NetsysNativeServiceStub::CmdSetTcpBufferSizes;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_REGISTER_NOTIFY_CALLBACK)] =
        &NetsysNativeServiceStub::CmdRegisterNotifyCallback;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_UNREGISTER_NOTIFY_CALLBACK)] =
        &NetsysNativeServiceStub::CmdUnRegisterNotifyCallback;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_ADD_ROUTE)] =
        &NetsysNativeServiceStub::CmdNetworkAddRoute;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_ADD_ROUTES)] =
        &NetsysNativeServiceStub::CmdNetworkAddRoutes;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_REMOVE_ROUTE)] =
        &NetsysNativeServiceStub::CmdNetworkRemoveRoute;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_ADD_ROUTE_PARCEL)] =
        &NetsysNativeServiceStub::CmdNetworkAddRouteParcel;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_REMOVE_ROUTE_PARCEL)] =
        &NetsysNativeServiceStub::CmdNetworkRemoveRouteParcel;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_SET_DEFAULT)] =
        &NetsysNativeServiceStub::CmdNetworkSetDefault;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_GET_DEFAULT)] =
        &NetsysNativeServiceStub::CmdNetworkGetDefault;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_CLEAR_DEFAULT)] =
        &NetsysNativeServiceStub::CmdNetworkClearDefault;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_PROC_SYS_NET)] =
        &NetsysNativeServiceStub::CmdGetProcSysNet;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_PROC_SYS_NET)] =
        &NetsysNativeServiceStub::CmdSetProcSysNet;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_CREATE_PHYSICAL)] =
        &NetsysNativeServiceStub::CmdNetworkCreatePhysical;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_ADD_ADDRESS)] =
        &NetsysNativeServiceStub::CmdAddInterfaceAddress;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_DEL_ADDRESS)] =
        &NetsysNativeServiceStub::CmdDelInterfaceAddress;
#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_ENABLE_WEARABLE_DISTRIBUTED_NET_FORWARD)] =
        &NetsysNativeServiceStub::CmdEnableWearableDistributedNetForward;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DISABLE_WEARABLE_DISTRIBUTED_NET_FORWARD)] =
        &NetsysNativeServiceStub::CmdDisableWearableDistributedNetForward;
#endif
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_SET_IPV6_PRIVCAY_EXTENSION)] =
        &NetsysNativeServiceStub::CmdSetIpv6PrivacyExtensions;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_ENABLE_IPV6)] =
        &NetsysNativeServiceStub::CmdSetIpv6Enable;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_START_CLAT)] =
        &NetsysNativeServiceStub::CmdStartClat;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_STOP_CLAT)] =
        &NetsysNativeServiceStub::CmdStopClat;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_SET_IPV6_AUTO_CONF)] =
        &NetsysNativeServiceStub::CmdSetIpv6AutoConf;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_CONNECT_OWNER_UID)] =
        &NetsysNativeServiceStub::CmdGetConnectOwnerUid;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_SET_IPV6_UID_BLACK_LIST)] =
        &NetsysNativeServiceStub::CmdSetIpv6UidBlackList;
}

void NetsysNativeServiceStub::InitBandwidthOpToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_SHARING_NETWORK_TRAFFIC)] =
        &NetsysNativeServiceStub::CmdGetNetworkSharingTraffic;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_CELLULAR_SHARING_NETWORK_TRAFFIC)] =
        &NetsysNativeServiceStub::CmdGetNetworkCellularSharingTraffic;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_TOTAL_STATS)] =
        &NetsysNativeServiceStub::CmdGetTotalStats;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_UID_STATS)] =
        &NetsysNativeServiceStub::CmdGetUidStats;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_IFACE_STATS)] =
        &NetsysNativeServiceStub::CmdGetIfaceStats;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_ALL_SIM_STATS_INFO)] =
        &NetsysNativeServiceStub::CmdGetAllSimStatsInfo;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DELETE_SIM_STATS_INFO)] =
        &NetsysNativeServiceStub::CmdDeleteSimStatsInfo;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_ALL_STATS_INFO)] =
        &NetsysNativeServiceStub::CmdGetAllStatsInfo;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DELETE_STATS_INFO)] =
        &NetsysNativeServiceStub::CmdDeleteStatsInfo;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_COOKIE_STATS)] =
        &NetsysNativeServiceStub::CmdGetCookieStats;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_CREATE_VIRTUAL)] =
        &NetsysNativeServiceStub::CmdNetworkCreateVirtual;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_ADD_UIDS)] =
        &NetsysNativeServiceStub::CmdNetworkAddUids;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_DEL_UIDS)] =
        &NetsysNativeServiceStub::CmdNetworkDelUids;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_BANDWIDTH_ENABLE_DATA_SAVER)] =
        &NetsysNativeServiceStub::CmdBandwidthEnableDataSaver;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_BANDWIDTH_SET_IFACE_QUOTA)] =
        &NetsysNativeServiceStub::CmdBandwidthSetIfaceQuota;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_BANDWIDTH_REMOVE_IFACE_QUOTA)] =
        &NetsysNativeServiceStub::CmdBandwidthRemoveIfaceQuota;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_BANDWIDTH_ADD_DENIED_LIST)] =
        &NetsysNativeServiceStub::CmdBandwidthAddDeniedList;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_BANDWIDTH_REMOVE_DENIED_LIST)] =
        &NetsysNativeServiceStub::CmdBandwidthRemoveDeniedList;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_BANDWIDTH_ADD_ALLOWED_LIST)] =
        &NetsysNativeServiceStub::CmdBandwidthAddAllowedList;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_BANDWIDTH_REMOVE_ALLOWED_LIST)] =
        &NetsysNativeServiceStub::CmdBandwidthRemoveAllowedList;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_INTERNET_PERMISSION)] =
        &NetsysNativeServiceStub::CmdSetInternetPermission;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_NETWORK_ACCESS_POLICY)] =
        &NetsysNativeServiceStub::CmdSetNetworkAccessPolicy;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DEL_NETWORK_ACCESS_POLICY)] =
        &NetsysNativeServiceStub::CmdDelNetworkAccessPolicy;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NOTIFY_NETWORK_BEARER_TYPE_CHANGE)] =
        &NetsysNativeServiceStub::CmdNotifyNetBearerTypeChange;
}

void NetsysNativeServiceStub::InitBandwidthOpToInterfaceMapExt()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_DPA_SHARING_NETWORK_TRAFFIC)] =
        &NetsysNativeServiceStub::CmdSetDpaCellularSharingTraffic;
}

void NetsysNativeServiceStub::InitFirewallOpToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_FIREWALL_SET_UID_ALLOWED_LIST_CHAIN)] =
        &NetsysNativeServiceStub::CmdFirewallSetUidsAllowedListChain;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_FIREWALL_SET_UID_DENIED_LIST_CHAIN)] =
        &NetsysNativeServiceStub::CmdFirewallSetUidsDeniedListChain;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_FIREWALL_ENABLE_CHAIN)] =
        &NetsysNativeServiceStub::CmdFirewallEnableChain;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_FIREWALL_SET_UID_RULE)] =
        &NetsysNativeServiceStub::CmdFirewallSetUidRule;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_NETWORK_SHARING_TYPE)] =
        &NetsysNativeServiceStub::CmdGetNetworkSharingType;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_UPDATE_NETWORK_SHARING_TYPE)] =
        &NetsysNativeServiceStub::CmdUpdateNetworkSharingType;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_CLEAR_FIREWALL_RULE)] =
        &NetsysNativeServiceStub::CmdClearFirewallAllRules;
#ifdef FEATURE_NET_FIREWALL_ENABLE
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES)] =
        &NetsysNativeServiceStub::CmdSetFirewallRules;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_DEFAULT_ACTION)] =
        &NetsysNativeServiceStub::CmdSetFirewallDefaultAction;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_USER_ID)] =
        &NetsysNativeServiceStub::CmdSetFirewallCurrentUserId;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NET_FIREWALL_CLEAR_RULES)] =
        &NetsysNativeServiceStub::CmdClearFirewallRules;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NET_FIREWALL_REGISTER)] =
        &NetsysNativeServiceStub::CmdRegisterNetFirewallCallback;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NET_FIREWALL_UNREGISTER)] =
        &NetsysNativeServiceStub::CmdUnRegisterNetFirewallCallback;
#endif
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_INTERNET_ACCESS_BY_IP_FOR_NET_SHARE)] =
        &NetsysNativeServiceStub::CmdSetInternetAccessByIpForWifiShare;
}

#ifdef SUPPORT_SYSVPN
void NetsysNativeServiceStub::InitVpnOpToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_PROCESS_VPN_STAGE)] =
        &NetsysNativeServiceStub::CmdProcessVpnStage;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_UPDATE_VPN_RULES)] =
        &NetsysNativeServiceStub::CmdUpdateVpnRules;
}
#endif

void NetsysNativeServiceStub::InitVlanInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_CREATE_VLAN)] =
        &NetsysNativeServiceStub::CmdCreateVlan;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DESTROY_VLAN)] =
        &NetsysNativeServiceStub::CmdDestroyVlan;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_ADD_VLAN_IP)] =
        &NetsysNativeServiceStub::CmdAddVlanIp;
}

void NetsysNativeServiceStub::InitOpToInterfaceMapExt()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_SET_IP_ADDRESS)] =
        &NetsysNativeServiceStub::CmdInterfaceSetIpAddress;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_SET_IFF_UP)] =
        &NetsysNativeServiceStub::CmdInterfaceSetIffUp;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_ADD_INTERFACE)] =
        &NetsysNativeServiceStub::CmdNetworkAddInterface;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_REMOVE_INTERFACE)] =
        &NetsysNativeServiceStub::CmdNetworkRemoveInterface;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETWORK_DESTROY)] =
        &NetsysNativeServiceStub::CmdNetworkDestroy;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_FWMARK_FOR_NETWORK)] =
        &NetsysNativeServiceStub::CmdGetFwmarkForNetwork;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_SET_CONFIG)] =
        &NetsysNativeServiceStub::CmdSetInterfaceConfig;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_GET_CONFIG)] =
        &NetsysNativeServiceStub::CmdGetInterfaceConfig;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_INTERFACE_GET_LIST)] =
        &NetsysNativeServiceStub::CmdInterfaceGetList;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_START_DHCP_CLIENT)] =
        &NetsysNativeServiceStub::CmdStartDhcpClient;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_STOP_DHCP_CLIENT)] =
        &NetsysNativeServiceStub::CmdStopDhcpClient;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_START_DHCP_SERVICE)] =
        &NetsysNativeServiceStub::CmdStartDhcpService;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_STOP_DHCP_SERVICE)] =
        &NetsysNativeServiceStub::CmdStopDhcpService;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_IPENABLE_FORWARDING)] =
        &NetsysNativeServiceStub::CmdIpEnableForwarding;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_IPDISABLE_FORWARDING)] =
        &NetsysNativeServiceStub::CmdIpDisableForwarding;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_ENABLE_NAT)] =
        &NetsysNativeServiceStub::CmdEnableNat;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DISABLE_NAT)] =
        &NetsysNativeServiceStub::CmdDisableNat;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_IPFWD_ADD_INTERFACE_FORWARD)] =
        &NetsysNativeServiceStub::CmdIpfwdAddInterfaceForward;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_IPFWD_REMOVE_INTERFACE_FORWARD)] =
        &NetsysNativeServiceStub::CmdIpfwdRemoveInterfaceForward;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_IPTABLES_CMD_FOR_RES)] =
        &NetsysNativeServiceStub::CmdSetIptablesCommandForRes;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_TETHER_DNS_SET)] =
        &NetsysNativeServiceStub::CmdShareDnsSet;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_START_DNS_PROXY_LISTEN)] =
        &NetsysNativeServiceStub::CmdStartDnsProxyListen;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_STOP_DNS_PROXY_LISTEN)] =
        &NetsysNativeServiceStub::CmdStopDnsProxyListen;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_NIC_TRAFFIC_ALLOWED)] =
        &NetsysNativeServiceStub::CmdSetNicTrafficAllowed;
}

void NetsysNativeServiceStub::InitDnsServerOpToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_USER_DEFINED_SERVER_FLAG)] =
        &NetsysNativeServiceStub::CmdSetUserDefinedServerFlag;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_FLUSH_DNS_CACHE)] =
        &NetsysNativeServiceStub::CmdFlushDnsCache;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_DNS_CACHE)] =
        &NetsysNativeServiceStub::CmdSetDnsCache;
}

void NetsysNativeServiceStub::InitNetDiagOpToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETDIAG_PING_HOST)] =
        &NetsysNativeServiceStub::CmdNetDiagPingHost;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETDIAG_GET_ROUTE_TABLE)] =
        &NetsysNativeServiceStub::CmdNetDiagGetRouteTable;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETDIAG_GET_SOCKETS_INFO)] =
        &NetsysNativeServiceStub::CmdNetDiagGetSocketsInfo;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETDIAG_GET_IFACE_CONFIG)] =
        &NetsysNativeServiceStub::CmdNetDiagGetInterfaceConfig;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETDIAG_UPDATE_IFACE_CONFIG)] =
        &NetsysNativeServiceStub::CmdNetDiagUpdateInterfaceConfig;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NETDIAG_SET_IFACE_ACTIVE_STATE)] =
        &NetsysNativeServiceStub::CmdNetDiagSetInterfaceActiveState;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_IPCMD_FOR_RES)] =
        &NetsysNativeServiceStub::CmdSetIpCommandForRes;
}

void NetsysNativeServiceStub::InitStaticArpToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_ADD_STATIC_ARP)] =
        &NetsysNativeServiceStub::CmdAddStaticArp;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DEL_STATIC_ARP)] =
        &NetsysNativeServiceStub::CmdDelStaticArp;
}

void NetsysNativeServiceStub::InitStaticIpv6ToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_ADD_STATIC_IPV6)] =
        &NetsysNativeServiceStub::CmdAddStaticIpv6Addr;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DEL_STATIC_IPV6)] =
        &NetsysNativeServiceStub::CmdDelStaticIpv6Addr;
}

void NetsysNativeServiceStub::InitNetDnsDiagOpToInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_REGISTER_DNS_RESULT_LISTENER)] =
        &NetsysNativeServiceStub::CmdRegisterDnsResultListener;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_UNREGISTER_DNS_RESULT_LISTENER)] =
        &NetsysNativeServiceStub::CmdUnregisterDnsResultListener;
}

void NetsysNativeServiceStub::InitNetVnicInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_VNIC_CREATE)] =
        &NetsysNativeServiceStub::CmdCreateVnic;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_VNIC_DESTROY)] =
        &NetsysNativeServiceStub::CmdDestroyVnic;
}

void NetsysNativeServiceStub::InitNetVirnicInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_ENABLE_DISTRIBUTE_CLIENT_NET)] =
        &NetsysNativeServiceStub::CmdEnableDistributedClientNet;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_ENABLE_DISTRIBUTE_SERVER_NET)] =
        &NetsysNativeServiceStub::CmdEnableDistributedServerNet;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DISABLE_DISTRIBUTE_NET)] =
        &NetsysNativeServiceStub::CmdDisableDistributedNet;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_CLOSE_SOCKETS_UID)] =
        &NetsysNativeServiceStub::CmdCloseSocketsUid;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DEL_BROKER_UID_NETWORK_POLICY)] =
        &NetsysNativeServiceStub::CmdDelBrokerUidAccessPolicyMap;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_BROKER_UID_NETWORK_POLICY)] =
        &NetsysNativeServiceStub::CmdSetBrokerUidAccessPolicyMap;
}

void NetsysNativeServiceStub::InitNetStatsInterfaceMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_TRAFFIC_REGISTER)] =
        &NetsysNativeServiceStub::CmdRegisterNetsysTrafficCallback;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_TRAFFIC_UNREGISTER)] =
        &NetsysNativeServiceStub::CmdUnRegisterNetsysTrafficCallback;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_TRAFFIC_AVAILABLE_MAP)] =
        &NetsysNativeServiceStub::CmdSetNetStateTrafficMap;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_TRAFFIC_AVAILABLE_MAP)] =
        &NetsysNativeServiceStub::CmdGetNetStateTrafficMap;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_CLEAR_INCRE_TRAFFIC_MAP)] =
        &NetsysNativeServiceStub::CmdClearIncreaseTrafficMap;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_DELETE_INCRE_TRAFFIC_MAP)] =
        &NetsysNativeServiceStub::CmdDeleteIncreaseTrafficMap;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_CLEAR_SIM_STATS_MAP)] =
        &NetsysNativeServiceStub::CmdClearSimStatsBpfMap;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_UPDATE_IFINDEX_MAP)] =
        &NetsysNativeServiceStub::CmdUpdateIfIndexMap;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_NET_STATUS_MAP)] =
        &NetsysNativeServiceStub::CmdSetNetStatusMap;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_IP_NEIGH_TABLE)] =
        &NetsysNativeServiceStub::CmdGetIpNeighTable;
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_GET_SYSTEM_NET_PORT_STATES)] =
        &NetsysNativeServiceStub::CmdGetSystemNetPortStates;
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
void NetsysNativeServiceStub::InitEnterpriseMap()
{
    opToInterfaceMap_[static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_UPDATE_ENTERPRISE_ROUTE)] =
        &NetsysNativeServiceStub::CmdUpdateEnterpriseRoute;
}
#endif

int32_t NetsysNativeServiceStub::CmdRegisterNetsysTrafficCallback(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOGI("CmdRegisterNetsysTrafficCallback start.");
    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        result = IPC_STUB_ERR;
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetsysTrafficCallback> callback = iface_cast<INetsysTrafficCallback>(remote);
    if (callback == nullptr) {
        NETNATIVE_LOGE("CmdRegisterNetsysTrafficCallback err.");
        result = ERR_FLATTEN_OBJECT;
        reply.WriteInt32(result);
        return result;
    }

    result = RegisterNetsysTrafficCallback(callback);
    reply.WriteInt32(result);
    NETNATIVE_LOGI("CmdRegisterNetsysTrafficCallback end. result:%{public}d", result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdUnRegisterNetsysTrafficCallback(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        result = IPC_STUB_ERR;
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetsysTrafficCallback> callback = iface_cast<INetsysTrafficCallback>(remote);
    if (callback == nullptr) {
        result = ERR_FLATTEN_OBJECT;
        reply.WriteInt32(result);
        return result;
    }

    result = UnRegisterNetsysTrafficCallback(callback);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                 MessageOption &option)
{
    NETNATIVE_LOG_D("Begin to call procedure with code %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        NETNATIVE_LOGE("Cannot response request %d: unknown tranction", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    auto uid = IPCSkeleton::GetCallingUid();
    if (std::find(uids_.begin(), uids_.end(), uid) == uids_.end()) {
        NETNATIVE_LOGE("This uid connot use netsys %{public}d", uid);
        if (!reply.WriteInt32(NETMANAGER_ERR_PERMISSION_DENIED)) {
            return IPC_STUB_WRITE_PARCEL_ERR;
        }
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    if ((code == static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_IPTABLES_CMD_FOR_RES) ||
         code == static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_SET_IPCMD_FOR_RES)) && uid != UID_EDM &&
        uid != UID_NET_MANAGER && uid != UID_IOT_NET_MANAGER && uid != UID_IOT_TV_NET_MANAGER) {
        if (!reply.WriteInt32(NETMANAGER_ERR_PERMISSION_DENIED)) {
            return IPC_STUB_WRITE_PARCEL_ERR;
        }
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    const std::u16string descriptor = NetsysNativeServiceStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        NETNATIVE_LOGE("Check remote descriptor failed");
        return IPC_STUB_INVALID_DATA_ERR;
    }

#ifdef FEATURE_NET_FIREWALL_ENABLE
    if (code >= static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_DEFAULT_ACTION) &&
        code <= static_cast<uint32_t>(NetsysInterfaceCode::NETSYS_NET_FIREWALL_UNREGISTER) &&
        !NetManagerPermission::CheckPermission(Permission::NETSYS_INTERNAL)) {
        if (!reply.WriteInt32(NETMANAGER_ERR_PERMISSION_DENIED)) {
            return IPC_STUB_WRITE_PARCEL_ERR;
        }
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
#endif

    return (this->*(interfaceIndex->second))(data, reply);
}

int32_t NetsysNativeServiceStub::CmdSetResolverConfig(MessageParcel &data, MessageParcel &reply)
{
    uint16_t netId = 0;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    if (!data.ReadUint16(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.ReadUint16(baseTimeoutMsec)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.ReadUint8(retryCount)) {
        return ERR_FLATTEN_OBJECT;
    }
    int32_t vServerSize;
    if (!data.ReadInt32(vServerSize)) {
        return ERR_FLATTEN_OBJECT;
    }
    vServerSize = (vServerSize > MAX_DNS_CONFIG_SIZE) ? MAX_DNS_CONFIG_SIZE : vServerSize;
    std::string s;
    for (int32_t i = 0; i < vServerSize; ++i) {
        std::string().swap(s);
        if (!data.ReadString(s)) {
            return ERR_FLATTEN_OBJECT;
        }
        servers.push_back(s);
    }
    int32_t vDomainSize;
    if (!data.ReadInt32(vDomainSize)) {
        return ERR_FLATTEN_OBJECT;
    }
    vDomainSize = (vDomainSize > MAX_DNS_CONFIG_SIZE) ? MAX_DNS_CONFIG_SIZE : vDomainSize;
    for (int32_t i = 0; i < vDomainSize; ++i) {
        std::string().swap(s);
        if (!data.ReadString(s)) {
            return ERR_FLATTEN_OBJECT;
        }
        domains.push_back(s);
    }

    int32_t result = SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetResolverConfig has received result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdGetResolverConfig(MessageParcel &data, MessageParcel &reply)
{
    uint16_t baseTimeoutMsec;
    uint8_t retryCount;
    uint16_t netId = 0;
    std::vector<std::string> servers;
    std::vector<std::string> domains;

    data.ReadUint16(netId);
    int32_t result = GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
    reply.WriteInt32(result);
    reply.WriteUint16(baseTimeoutMsec);
    reply.WriteUint8(retryCount);
    auto vServerSize = static_cast<int32_t>(servers.size());
    vServerSize = (vServerSize > MAX_DNS_CONFIG_SIZE) ? MAX_DNS_CONFIG_SIZE : vServerSize;
    reply.WriteInt32(vServerSize);
    int32_t index = 0;
    for (auto &server : servers) {
        if (++index > MAX_DNS_CONFIG_SIZE) {
            break;
        }
        reply.WriteString(server);
    }
    auto vDomainsSize = static_cast<int32_t>(domains.size());
    vDomainsSize = (vDomainsSize > MAX_DNS_CONFIG_SIZE) ? MAX_DNS_CONFIG_SIZE : vDomainsSize;
    reply.WriteInt32(vDomainsSize);
    std::vector<std::string>::iterator iterDomains;
    index = 0;
    for (iterDomains = domains.begin(); iterDomains != domains.end(); ++iterDomains) {
        if (++index > MAX_DNS_CONFIG_SIZE) {
            break;
        }
        reply.WriteString(*iterDomains);
    }
    NETNATIVE_LOG_D("GetResolverConfig has recved result %{public}d", result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdCreateNetworkCache(MessageParcel &data, MessageParcel &reply)
{
    uint16_t netid = data.ReadUint16();
    bool isVpnNet = data.ReadBool();
    NETNATIVE_LOGI("CreateNetworkCache  netid %{public}d, isVpnNet %{public}d", netid, isVpnNet);
    int32_t result = CreateNetworkCache(netid, isVpnNet);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("CreateNetworkCache has recved result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdDestroyNetworkCache(MessageParcel &data, MessageParcel &reply)
{
    uint16_t netId = data.ReadUint16();
    bool isVpnNet = data.ReadBool();
    NETNATIVE_LOGI("DestroyNetworkCache  netId %{public}d, isVpnNet %{public}d", netId, isVpnNet);
    int32_t result = DestroyNetworkCache(netId, isVpnNet);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("DestroyNetworkCache has recved result %{public}d", result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::NetsysFreeAddrinfo(struct addrinfo *aihead)
{
    struct addrinfo *ai;
    struct addrinfo *ainext;
    for (ai = aihead; ai != nullptr; ai = ainext) {
        if (ai->ai_addr != nullptr)
            free(ai->ai_addr);
        if (ai->ai_canonname != nullptr)
            free(ai->ai_canonname);
        ainext = ai->ai_next;
        free(ai);
    }
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdGetAddrInfo(MessageParcel &data, MessageParcel &reply)
{
    std::string hostName;
    std::string serverName;
    AddrInfo hints = {};
    uint16_t netId;
    if (!data.ReadString(hostName)) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    if (!data.ReadString(serverName)) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    auto p = data.ReadRawData(sizeof(AddrInfo));
    if (p == nullptr) {
        return IPC_STUB_INVALID_DATA_ERR;
    }
    if (memcpy_s(&hints, sizeof(AddrInfo), p, sizeof(AddrInfo)) != EOK) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    if (!data.ReadUint16(netId)) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    std::vector<AddrInfo> retInfo;
    auto ret = GetAddrInfo(hostName, serverName, hints, netId, retInfo);
    if (retInfo.size() > MAX_RESULTS) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    if (!reply.WriteInt32(ret)) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    if (ret != ERR_NONE) {
        return ERR_NONE;
    }

    if (!reply.WriteUint32(static_cast<uint32_t>(retInfo.size()))) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    for (const auto &info : retInfo) {
        if (!reply.WriteRawData(&info, sizeof(AddrInfo))) {
            return IPC_STUB_WRITE_PARCEL_ERR;
        }
    }
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdSetInterfaceMtu(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName = data.ReadString();
    int32_t mtu = data.ReadInt32();
    int32_t result = SetInterfaceMtu(ifName, mtu);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetInterfaceMtu has recved result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdGetInterfaceMtu(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName = data.ReadString();
    int32_t result = GetInterfaceMtu(ifName);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("GetInterfaceMtu has recved result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdSetTcpBufferSizes(MessageParcel &data, MessageParcel &reply)
{
    std::string tcpBufferSizes = data.ReadString();
    int32_t result = SetTcpBufferSizes(tcpBufferSizes);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetTcpBufferSizes has recved result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdRegisterNotifyCallback(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd RegisterNotifyCallback");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        return -1;
    }

    sptr<INotifyCallback> callback = iface_cast<INotifyCallback>(remote);
    int32_t result = RegisterNotifyCallback(callback);
    reply.WriteInt32(result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdUnRegisterNotifyCallback(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd UnRegisterNotifyCallback");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        return -1;
    }

    sptr<INotifyCallback> callback = iface_cast<INotifyCallback>(remote);
    int32_t result = UnRegisterNotifyCallback(callback);
    reply.WriteInt32(result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdNetworkAddRoute(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    std::string ifName = data.ReadString();
    std::string destination = data.ReadString();
    std::string nextHop = data.ReadString();
    bool isExcludedRoute = data.ReadBool();

    NETNATIVE_LOGI("netId[%{public}d}, ifName[%{public}s], destination[%{public}s}, nextHop[%{public}s], \
        isExcludedRoute[%{public}d]", netId, ifName.c_str(), ToAnonymousIp(destination).c_str(),
        ToAnonymousIp(nextHop).c_str(), isExcludedRoute);
    int32_t result = NetworkAddRoute(netId, ifName, destination, nextHop, isExcludedRoute);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkAddRoute has recved result %{public}d", result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkAddRoutes(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    int32_t size = 0;
    if (!data.ReadInt32(netId) || !data.ReadInt32(size)) {
        NETNATIVE_LOGE("read net id or size failed");
        return IPC_STUB_ERR;
    }
    size = (size > static_cast<int32_t>(MAX_ROUTES_ARRAY_SIZE)) ? static_cast<int32_t>(MAX_ROUTES_ARRAY_SIZE) : size;
    sptr<nmd::NetworkRouteInfo> r;
    std::vector<nmd::NetworkRouteInfo> infos;
    for (int32_t index = 0; index < size; index++) {
        r = nmd::NetworkRouteInfo::Unmarshalling(data);
        if (r == nullptr) {
            NETNATIVE_LOGE("SysRoute::Unmarshalling(parcel) is null");
            return IPC_STUB_ERR;
        }
        infos.push_back(*r);
    }

    int32_t result = NetworkAddRoutes(netId, infos);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkAddRoutes has recvd result %{public}d", result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkRemoveRoute(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    std::string interfaceName = data.ReadString();
    std::string destination = data.ReadString();
    std::string nextHop = data.ReadString();
    bool isExcludedRoute = data.ReadBool();

    NETNATIVE_LOGI("netId[%{public}d}, ifName[%{public}s], destination[%{public}s}, nextHop[%{public}s],"
        "isExcludedRoute[%{public}d]", netId, interfaceName.c_str(), ToAnonymousIp(destination).c_str(),
        ToAnonymousIp(nextHop).c_str(), isExcludedRoute);
    int32_t result = NetworkRemoveRoute(netId, interfaceName, destination, nextHop, isExcludedRoute);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkRemoveRoute has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkAddRouteParcel(MessageParcel &data, MessageParcel &reply)
{
    RouteInfoParcel routeInfo = {};
    int32_t netId = data.ReadInt32();
    routeInfo.ifName = data.ReadString();
    routeInfo.destination = data.ReadString();
    routeInfo.nextHop = data.ReadString();
    int32_t result = NetworkAddRouteParcel(netId, routeInfo);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkAddRouteParcel has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkRemoveRouteParcel(MessageParcel &data, MessageParcel &reply)
{
    RouteInfoParcel routeInfo = {};
    int32_t netId = data.ReadInt32();
    routeInfo.ifName = data.ReadString();
    routeInfo.destination = data.ReadString();
    routeInfo.nextHop = data.ReadString();

    int32_t result = NetworkRemoveRouteParcel(netId, routeInfo);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkRemoveRouteParcel has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkSetDefault(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();

    int32_t result = NetworkSetDefault(netId);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkSetDefault has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkGetDefault(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NetworkGetDefault();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkGetDefault has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkClearDefault(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NetworkClearDefault();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkClearDefault has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetProcSysNet(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd GetProcSysNet");
    int32_t family = data.ReadInt32();
    int32_t which = data.ReadInt32();
    std::string ifname = data.ReadString();
    std::string parameter = data.ReadString();
    std::string value;
    int32_t result = GetProcSysNet(family, which, ifname, parameter, value);
    reply.WriteInt32(result);
    std::string valueRsl = value;
    reply.WriteString(valueRsl);
    return result;
}

int32_t NetsysNativeServiceStub::CmdSetProcSysNet(MessageParcel &data, MessageParcel &reply)
{
    int32_t family = data.ReadInt32();
    int32_t which = data.ReadInt32();
    std::string ifname = data.ReadString();
    std::string parameter = data.ReadString();
    std::string value = data.ReadString();
    int32_t result = SetProcSysNet(family, which, ifname, parameter, value);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetProcSysNet has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdSetInternetPermission(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = data.ReadUint32();
    uint8_t allow = data.ReadUint8();
    uint8_t isBroker = data.ReadUint8();
    int32_t result = SetInternetPermission(uid, allow, isBroker);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetInternetPermission has recved result %{public}d", result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkCreatePhysical(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    int32_t permission = data.ReadInt32();

    int32_t result = NetworkCreatePhysical(netId, permission);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkCreatePhysical has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkCreateVirtual(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    bool hasDns = false;
    if (!data.ReadInt32(netId) || !data.ReadBool(hasDns)) {
        NETNATIVE_LOGE("read net id or hasDns failed");
        return IPC_STUB_ERR;
    }

    int32_t result = NetworkCreateVirtual(netId, hasDns);
    if (!reply.WriteInt32(result)) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    NETNATIVE_LOG_D("NetworkCreateVirtual has recved result %{public}d", result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdNetworkAddUids(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    int32_t size = 0;
    if (!data.ReadInt32(netId) || !data.ReadInt32(size)) {
        NETNATIVE_LOGE("read net id or size failed");
        return IPC_STUB_ERR;
    }
    size = (size > static_cast<int32_t>(MAX_UID_ARRAY_SIZE)) ? static_cast<int32_t>(MAX_UID_ARRAY_SIZE) : size;

    sptr<UidRange> uid;
    std::vector<UidRange> uidRanges;
    for (int32_t index = 0; index < size; index++) {
        uid = UidRange::Unmarshalling(data);
        if (uid == nullptr) {
            NETNATIVE_LOGE("UidRange::Unmarshalling(parcel) is null");
            return IPC_STUB_ERR;
        }
        uidRanges.push_back(*uid);
    }
    int32_t result = NetworkAddUids(netId, uidRanges);
    if (!reply.WriteInt32(result)) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    NETNATIVE_LOG_D("NetworkAddUids has recved result %{public}d", result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdNetworkDelUids(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    int32_t size = 0;
    if (!data.ReadInt32(netId) || !data.ReadInt32(size)) {
        NETNATIVE_LOGE("read net id or size failed");
        return IPC_STUB_ERR;
    }

    size = (size > static_cast<int32_t>(MAX_UID_ARRAY_SIZE)) ? static_cast<int32_t>(MAX_UID_ARRAY_SIZE) : size;

    sptr<UidRange> uid;
    std::vector<UidRange> uidRanges;
    for (int32_t index = 0; index < size; index++) {
        uid = UidRange::Unmarshalling(data);
        if (uid == nullptr) {
            NETNATIVE_LOGE("UidRange::Unmarshalling(parcel) is null");
            return IPC_STUB_ERR;
        }
        uidRanges.push_back(*uid);
    }
    int32_t result = NetworkDelUids(netId, uidRanges);
    if (!reply.WriteInt32(result)) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }
    NETNATIVE_LOG_D("NetworkDelUids has recved result %{public}d", result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdAddInterfaceAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string interfaceName = data.ReadString();
    std::string ipAddr = data.ReadString();
    int32_t prefixLength = data.ReadInt32();

    int32_t result = AddInterfaceAddress(interfaceName, ipAddr, prefixLength);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("AddInterfaceAddress has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdDelInterfaceAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string interfaceName = data.ReadString();
    std::string ipAddr = data.ReadString();
    int32_t prefixLength = data.ReadInt32();
    int socketType = 2;
    int32_t result = 0;
    // LCOV_EXCL_START
    if (!data.ReadInt32(socketType)) {
        NETNATIVE_LOG_D("DelInterfaceAddress");
        result = DelInterfaceAddress(interfaceName, ipAddr, prefixLength);
    } else {
        NETNATIVE_LOG_D("DelInterfaceAddress with netCapabilities %{public}d", socketType);
        result = DelInterfaceAddress(interfaceName, ipAddr, prefixLength, socketType);
    }
    // LCOV_EXCL_STOP
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("DelInterfaceAddress has recved result %{public}d", result);

    return result;
}

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
int32_t NetsysNativeServiceStub::CmdEnableWearableDistributedNetForward(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOGI("NetsysNativeServiceStub enable wearable distributed net forward");
    int32_t tcpPort = data.ReadInt32();
    int32_t udpPort = data.ReadInt32();
    int32_t result = EnableWearableDistributedNetForward(tcpPort, udpPort);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    return result;
}

int32_t NetsysNativeServiceStub::CmdDisableWearableDistributedNetForward(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOGI("NetsysNativeServiceStub disable wearable distributed net forward");
    int32_t result = DisableWearableDistributedNetForward();
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    return result;
}
#endif

int32_t NetsysNativeServiceStub::CmdInterfaceSetIpAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string ifaceName = data.ReadString();
    std::string ipAddress = data.ReadString();

    int32_t result = InterfaceSetIpAddress(ifaceName, ipAddress);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("InterfaceSetIpAddress has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdInterfaceSetIffUp(MessageParcel &data, MessageParcel &reply)
{
    std::string ifaceName = data.ReadString();

    int32_t result = InterfaceSetIffUp(ifaceName);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("InterfaceSetIffUp has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkAddInterface(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    std::string iface = data.ReadString();
    NetBearType netBearerType = static_cast<NetBearType>(data.ReadUint8());

    int32_t result = NetworkAddInterface(netId, iface, netBearerType);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkAddInterface has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkRemoveInterface(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    std::string iface = data.ReadString();
    int32_t result = NetworkRemoveInterface(netId, iface);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkRemoveInterface has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkDestroy(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    bool isVpnNet = data.ReadBool();
    int32_t result = NetworkDestroy(netId, isVpnNet);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkDestroy has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdCreateVnic(MessageParcel &data, MessageParcel &reply)
{
    uint16_t mtu = data.ReadUint16();
    std::string tunAddr = data.ReadString();
    int32_t prefix = data.ReadInt32();
    std::set<int32_t> uids;
    int32_t size = 0;
    int32_t uid = 0;
    if (!data.ReadInt32(size)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (size < 0 || size > MAX_VNIC_UID_ARRAY_SIZE) {
        NETNATIVE_LOGE("vnic uids size is invalid");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    for (int32_t index = 0; index < size; index++) {
        if (!data.ReadInt32(uid)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        uids.insert(uid);
    }
    int32_t result = CreateVnic(mtu, tunAddr, prefix, uids);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("VnciCreate has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdDestroyVnic(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = DestroyVnic();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("VnicDestroy has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdEnableDistributedClientNet(MessageParcel &data, MessageParcel &reply)
{
    std::string virnicAddr = data.ReadString();
    std::string virnicName = data.ReadString();
    std::string iif = data.ReadString();

    int32_t result = EnableDistributedClientNet(virnicAddr, virnicName, iif);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("CmdEnableDistributedClientNet has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdEnableDistributedServerNet(MessageParcel &data, MessageParcel &reply)
{
    std::string iif = data.ReadString();
    std::string devIface = data.ReadString();
    std::string dstAddr = data.ReadString();
    std::string gw = data.ReadString();

    int32_t result = EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("CmdEnableDistributedServerNet has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdDisableDistributedNet(MessageParcel &data, MessageParcel &reply)
{
    bool isServer = data.ReadBool();
    std::string virnicName = data.ReadString();
    std::string dstAddr = data.ReadString();

    int32_t result = DisableDistributedNet(isServer, virnicName, dstAddr);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("CmdDisableDistributedNet has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetFwmarkForNetwork(MessageParcel &data, MessageParcel &reply)
{
    MarkMaskParcel markMaskParcel = {};
    int32_t netId = data.ReadInt32();
    markMaskParcel.mark = data.ReadInt32();
    markMaskParcel.mask = data.ReadInt32();
    int32_t result = GetFwmarkForNetwork(netId, markMaskParcel);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("GetFwmarkForNetwork has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdSetInterfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    InterfaceConfigurationParcel cfg = {};
    cfg.ifName = data.ReadString();
    cfg.hwAddr = data.ReadString();
    cfg.ipv4Addr = data.ReadString();
    cfg.prefixLength = data.ReadInt32();
    int32_t vSize = data.ReadInt32();
    vSize = (vSize > MAX_FLAG_NUM) ? MAX_FLAG_NUM : vSize;
    std::vector<std::string> vFlags;
    for (int i = 0; i < vSize; i++) {
        vFlags.emplace_back(data.ReadString());
    }
    cfg.flags.assign(vFlags.begin(), vFlags.end());
    int32_t result = SetInterfaceConfig(cfg);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetInterfaceConfig has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetInterfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd GetInterfaceConfig");
    InterfaceConfigurationParcel cfg = {};
    cfg.ifName = data.ReadString();
    int32_t result = GetInterfaceConfig(cfg);
    reply.WriteInt32(result);
    reply.WriteString(cfg.ifName);
    reply.WriteString(cfg.hwAddr);
    reply.WriteString(cfg.ipv4Addr);
    reply.WriteInt32(cfg.prefixLength);
    int32_t vsize = static_cast<int32_t>(cfg.flags.size());
    vsize = vsize > MAX_DNS_CONFIG_SIZE ? MAX_DNS_CONFIG_SIZE : vsize;
    reply.WriteInt32(vsize);
    std::vector<std::string>::iterator iter;
    int32_t index = 0;
    for (iter = cfg.flags.begin(); iter != cfg.flags.end(); ++iter) {
        if (++index > MAX_DNS_CONFIG_SIZE) {
            break;
        }
        reply.WriteString(*iter);
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdInterfaceGetList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd InterfaceGetList");
    std::vector<std::string> ifaces;
    int32_t result = InterfaceGetList(ifaces);
    reply.WriteInt32(result);
    auto vsize = static_cast<int32_t>(ifaces.size());
    reply.WriteInt32(vsize);
    std::vector<std::string>::iterator iter;
    for (iter = ifaces.begin(); iter != ifaces.end(); ++iter) {
        reply.WriteString(*iter);
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdStartDhcpClient(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdStartDhcpClient");
    std::string iface = data.ReadString();
    bool bIpv6 = data.ReadBool();
    int32_t result = StartDhcpClient(iface, bIpv6);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdStopDhcpClient(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdStopDhcpClient");
    std::string iface = data.ReadString();
    bool bIpv6 = data.ReadBool();
    int32_t result = StopDhcpClient(iface, bIpv6);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdStartDhcpService(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdStartDhcpService");
    std::string iface = data.ReadString();
    std::string ipv4addr = data.ReadString();
    int32_t result = StartDhcpService(iface, ipv4addr);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdStopDhcpService(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdStopDhcpService");
    std::string iface = data.ReadString();
    int32_t result = StopDhcpService(iface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdIpEnableForwarding(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdIpEnableForwarding");
    const auto &requester = data.ReadString();
    int32_t result = IpEnableForwarding(requester);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdIpDisableForwarding(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdIpDisableForwarding");
    const auto &requester = data.ReadString();
    int32_t result = IpDisableForwarding(requester);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdEnableNat(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdEnableNat");
    const auto &downstreamIface = data.ReadString();
    const auto &upstreamIface = data.ReadString();
    int32_t result = EnableNat(downstreamIface, upstreamIface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdDisableNat(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdDisableNat");
    const auto &downstreamIface = data.ReadString();
    const auto &upstreamIface = data.ReadString();
    int32_t result = DisableNat(downstreamIface, upstreamIface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdIpfwdAddInterfaceForward(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdIpfwdAddInterfaceForward");
    std::string fromIface = data.ReadString();
    std::string toIface = data.ReadString();
    int32_t result = IpfwdAddInterfaceForward(fromIface, toIface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdIpfwdRemoveInterfaceForward(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdIpfwdRemoveInterfaceForward");
    const auto &fromIface = data.ReadString();
    const auto &toIface = data.ReadString();
    int32_t result = IpfwdRemoveInterfaceForward(fromIface, toIface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthEnableDataSaver(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthEnableDataSaver");
    bool enable = data.ReadBool();
    int32_t result = BandwidthEnableDataSaver(enable);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthSetIfaceQuota(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthSetIfaceQuota");
    std::string ifName = data.ReadString();
    int64_t bytes = data.ReadInt64();
    int32_t result = BandwidthSetIfaceQuota(ifName, bytes);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthRemoveIfaceQuota(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthRemoveIfaceQuota");
    std::string ifName = data.ReadString();
    int32_t result = BandwidthRemoveIfaceQuota(ifName);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthAddDeniedList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthAddDeniedList");
    uint32_t uid = data.ReadUint32();
    int32_t result = BandwidthAddDeniedList(uid);
    reply.WriteInt32(result);
    return result;
}
int32_t NetsysNativeServiceStub::CmdBandwidthRemoveDeniedList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthRemoveDeniedList");
    uint32_t uid = data.ReadUint32();
    int32_t result = BandwidthRemoveDeniedList(uid);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthAddAllowedList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthAddAllowedList");
    uint32_t uid = data.ReadUint32();
    int32_t result = BandwidthAddAllowedList(uid);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthRemoveAllowedList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthRemoveAllowedList");
    uint32_t uid = data.ReadUint32();
    int32_t result = BandwidthRemoveAllowedList(uid);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdFirewallSetUidsAllowedListChain(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdFirewallSetUidsAllowedListChain");
    uint32_t chain = data.ReadUint32();
    std::vector<uint32_t> uids;
    uint32_t uidSize = data.ReadUint32();
    uidSize = (uidSize > UIDS_LIST_MAX_SIZE) ? UIDS_LIST_MAX_SIZE : uidSize;
    for (uint32_t i = 0; i < uidSize; i++) {
        uint32_t uid = data.ReadUint32();
        uids.push_back(uid);
    }
    int32_t result = FirewallSetUidsAllowedListChain(chain, uids);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdFirewallSetUidsDeniedListChain(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdFirewallSetUidsDeniedListChain");
    uint32_t chain = data.ReadUint32();
    std::vector<uint32_t> uids;
    uint32_t uidSize = data.ReadUint32();
    uidSize = (uidSize > UIDS_LIST_MAX_SIZE) ? UIDS_LIST_MAX_SIZE : uidSize;
    for (uint32_t i = 0; i < uidSize; i++) {
        uint32_t uid = data.ReadUint32();
        uids.push_back(uid);
    }
    int32_t result = FirewallSetUidsDeniedListChain(chain, uids);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdFirewallEnableChain(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdFirewallEnableChain");
    uint32_t chain = data.ReadUint32();
    bool enable = data.ReadBool();
    int32_t result = FirewallEnableChain(chain, enable);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdFirewallSetUidRule(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdFirewallSetUidRule");
    uint32_t chain = (unsigned)data.ReadUint32();
    std::vector<uint32_t> uids;
    data.ReadUInt32Vector(&uids);
    uint32_t firewallRule = (unsigned)data.ReadInt32();
    int32_t result = FirewallSetUidRule(chain, uids, firewallRule);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdShareDnsSet(MessageParcel &data, MessageParcel &reply)
{
    uint16_t netId = 0;
    data.ReadUint16(netId);
    int32_t result = ShareDnsSet(netId);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("ShareDnsSet has received result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdStartDnsProxyListen(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = StartDnsProxyListen();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("StartDnsProxyListen has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdStopDnsProxyListen(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = StopDnsProxyListen();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("StopDnsProxyListen has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetNetworkSharingTraffic(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd GetNetworkSharingTraffic");
    std::string downIface = data.ReadString();
    std::string upIface = data.ReadString();
    NetworkSharingTraffic traffic;
    int32_t result = GetNetworkSharingTraffic(downIface, upIface, traffic);
    reply.WriteInt32(result);
    reply.WriteInt64(traffic.receive);
    reply.WriteInt64(traffic.send);
    reply.WriteInt64(traffic.all);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetNetworkCellularSharingTraffic(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd GetNetworkSharingTraffic");
    std::string ifaceName;
    NetworkSharingTraffic traffic;
    int32_t result = GetNetworkCellularSharingTraffic(traffic, ifaceName);
    reply.WriteInt32(result);
    reply.WriteInt64(traffic.receive);
    reply.WriteInt64(traffic.send);
    reply.WriteInt64(traffic.all);
    reply.WriteString(ifaceName);

    return result;
}

int32_t NetsysNativeServiceStub::CmdSetDpaCellularSharingTraffic(MessageParcel &data, MessageParcel &reply)
{
    NetworkDpaTrafficReport traffic;
    traffic.src_if_index = data.ReadUint32();
    traffic.dst_if_index = data.ReadUint32();
    traffic.pkts = data.ReadUint32();
    traffic.bytes = data.ReadUint32();
    return SetDpaCellularSharingTraffic(traffic);
}

int32_t NetsysNativeServiceStub::CmdGetTotalStats(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type = data.ReadUint32();
    uint64_t stats = 0;
    int32_t result = GetTotalStats(stats, type);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!reply.WriteUint64(stats)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdGetUidStats(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type = data.ReadUint32();
    uint32_t uId = data.ReadUint32();
    uint64_t stats = 0;
    int32_t result = GetUidStats(stats, type, uId);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!reply.WriteUint64(stats)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdGetIfaceStats(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type = data.ReadUint32();
    std::string interfaceName = data.ReadString();
    uint64_t stats = 0;
    int32_t result = GetIfaceStats(stats, type, interfaceName);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!reply.WriteUint64(stats)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdGetAllSimStatsInfo(MessageParcel &data, MessageParcel &reply)
{
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> stats;
    int32_t result = GetAllSimStatsInfo(stats);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!OHOS::NetManagerStandard::NetStatsInfo::Marshalling(reply, stats)) {
        NETNATIVE_LOGE("Read stats info failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdDeleteSimStatsInfo(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = data.ReadUint32();
    int32_t ret = DeleteSimStatsInfo(uid);
    NETNATIVE_LOG_D("DeleteSimStatsInfo uid[%{public}d] ret[%{public}d]", uid, ret);
    if (!reply.WriteInt32(ret)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdGetAllStatsInfo(MessageParcel &data, MessageParcel &reply)
{
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> stats;
    int32_t result = GetAllStatsInfo(stats);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!OHOS::NetManagerStandard::NetStatsInfo::Marshalling(reply, stats)) {
        NETNATIVE_LOGE("Read stats info failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdDeleteStatsInfo(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = data.ReadUint32();
    int32_t ret = DeleteStatsInfo(uid);
    NETNATIVE_LOG_D("DeleteStatsInfo uid[%{public}d] ret[%{public}d]", uid, ret);
    if (!reply.WriteInt32(ret)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdSetNetStateTrafficMap(MessageParcel &data, MessageParcel &reply)
{
    uint8_t flag = 0;
    uint64_t availableTraffic = 0;
    if (!data.ReadUint8(flag)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.ReadUint64(availableTraffic)) {
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = SetNetStateTrafficMap(flag, availableTraffic);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdGetNetStateTrafficMap(MessageParcel &data, MessageParcel &reply)
{
    uint8_t flag = 0;
    if (!data.ReadUint8(flag)) {
        return ERR_FLATTEN_OBJECT;
    }
    uint64_t availableTraffic = 0;
    int32_t result = GetNetStateTrafficMap(flag, availableTraffic);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!reply.WriteUint64(availableTraffic)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdClearIncreaseTrafficMap(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = ClearIncreaseTrafficMap();
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdClearSimStatsBpfMap(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = ClearSimStatsBpfMap();
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdDeleteIncreaseTrafficMap(MessageParcel &data, MessageParcel &reply)
{
    uint64_t ifIndex = 0;
    if (!data.ReadUint64(ifIndex)) {
        return ERR_FLATTEN_OBJECT;
    }
    int32_t result = DeleteIncreaseTrafficMap(ifIndex);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdUpdateIfIndexMap(MessageParcel &data, MessageParcel &reply)
{
    int8_t key = 0;
    uint64_t index = 0;
    if (!data.ReadInt8(key)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.ReadUint64(index)) {
        return ERR_FLATTEN_OBJECT;
    }
    int32_t result = UpdateIfIndexMap(key, index);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdSetNetStatusMap(MessageParcel &data, MessageParcel &reply)
{
    uint8_t type = 0;
    uint8_t value = 0;
    if (!data.ReadUint8(type)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.ReadUint8(value)) {
        return ERR_FLATTEN_OBJECT;
    }
    int32_t result = SetNetStatusMap(type, value);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdSetIptablesCommandForRes(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdSetIptablesCommandForRes CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    std::string cmd = data.ReadString();
    IptablesType ipType = static_cast<IptablesType>(data.ReadUint32());
    std::string respond;
    int32_t result = SetIptablesCommandForRes(cmd, respond, ipType);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdSetIptablesCommandForRes result failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!reply.WriteString(respond)) {
        NETNATIVE_LOGE("Write CmdSetIptablesCommandForRes respond failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdSetIpCommandForRes(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdSetIpCommandForRes CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    std::string cmd = data.ReadString();
    std::string respond;
    int32_t result = SetIpCommandForRes(cmd, respond);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdSetIpCommandForRes result failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!reply.WriteString(respond)) {
        NETNATIVE_LOGE("Write CmdSetIpCommandForRes respond failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdNetDiagPingHost(MessageParcel &data, MessageParcel &reply)
{
    NetDiagPingOption pingOption;
    if (!NetDiagPingOption::Unmarshalling(data, pingOption)) {
        NETNATIVE_LOGE("Unmarshalling failed.");
        return IPC_STUB_ERR;
    }

    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("remote is nullptr.");
        return IPC_STUB_ERR;
    }

    sptr<INetDiagCallback> callback = iface_cast<INetDiagCallback>(remote);
    int32_t result = NetDiagPingHost(pingOption, callback);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetDiagGetRouteTable(MessageParcel &data, MessageParcel &reply)
{
    std::list<NetDiagRouteTable> routeTables;
    int32_t result = NetDiagGetRouteTable(routeTables);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (result == NetManagerStandard::NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(
            static_cast<uint32_t>(std::min(MAX_ROUTE_TABLE_SIZE, static_cast<uint32_t>(routeTables.size()))))) {
            NETNATIVE_LOGE("Write uint32 failed");
            return ERR_FLATTEN_OBJECT;
        }
        uint32_t count = 0;
        for (const auto &routeTable : routeTables) {
            if (!routeTable.Marshalling(reply)) {
                NETNATIVE_LOGE("NetDiagRouteTable marshalling failed");
                return ERR_FLATTEN_OBJECT;
            }
            ++count;
            if (count >= MAX_ROUTE_TABLE_SIZE) {
                break;
            }
        }
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetDiagGetSocketsInfo(MessageParcel &data, MessageParcel &reply)
{
    uint8_t socketType = 0;
    if (!data.ReadUint8(socketType)) {
        NETNATIVE_LOGE("Read uint8 failed");
        return ERR_FLATTEN_OBJECT;
    }
    NetDiagSocketsInfo socketsInfo;
    int32_t result = NetDiagGetSocketsInfo(static_cast<NetDiagProtocolType>(socketType), socketsInfo);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (result == NetManagerStandard::NETMANAGER_SUCCESS) {
        if (!socketsInfo.Marshalling(reply)) {
            NETNATIVE_LOGE("NetDiagSocketsInfo marshalling failed.");
            return ERR_FLATTEN_OBJECT;
        }
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetDiagGetInterfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    std::string ifaceName;
    if (!data.ReadString(ifaceName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }
    std::list<NetDiagIfaceConfig> configList;
    int32_t result = NetDiagGetInterfaceConfig(configList, ifaceName);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (result == NetManagerStandard::NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(
            static_cast<uint32_t>(std::min(MAX_CONFIG_LIST_SIZE, static_cast<uint32_t>(configList.size()))))) {
            NETNATIVE_LOGE("Write uint32 failed");
            return ERR_FLATTEN_OBJECT;
        }
        uint32_t count = 0;
        for (const auto &config : configList) {
            if (!config.Marshalling(reply)) {
                NETNATIVE_LOGE("NetDiagIfaceConfig marshalling failed");
                return ERR_FLATTEN_OBJECT;
            }
            ++count;
            if (count >= MAX_CONFIG_LIST_SIZE) {
                break;
            }
        }
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetDiagUpdateInterfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    NetDiagIfaceConfig config;
    if (!NetDiagIfaceConfig::Unmarshalling(data, config)) {
        NETNATIVE_LOGE("NetDiagIfaceConfig unmarshalling failed.");
        return IPC_STUB_ERR;
    }

    std::string ifaceName;
    if (!data.ReadString(ifaceName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    bool add = false;
    if (!data.ReadBool(add)) {
        NETNATIVE_LOGE("Read bool failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = NetDiagUpdateInterfaceConfig(config, ifaceName, add);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetDiagSetInterfaceActiveState(MessageParcel &data, MessageParcel &reply)
{
    std::string ifaceName;
    if (!data.ReadString(ifaceName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    bool up = false;
    if (!data.ReadBool(up)) {
        NETNATIVE_LOGE("Read bool failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = NetDiagSetInterfaceActiveState(ifaceName, up);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdAddStaticArp(MessageParcel &data, MessageParcel &reply)
{
    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string macAddr = "";
    if (!data.ReadString(macAddr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = AddStaticArp(ipAddr, macAddr, ifName);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    NETNATIVE_LOG_D("CmdAddStaticArp has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdDelStaticArp(MessageParcel &data, MessageParcel &reply)
{
    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string macAddr = "";
    if (!data.ReadString(macAddr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = DelStaticArp(ipAddr, macAddr, ifName);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    NETNATIVE_LOG_D("CmdDelStaticArp has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdAddStaticIpv6Addr(MessageParcel &data, MessageParcel &reply)
{
    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string macAddr = "";
    if (!data.ReadString(macAddr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    NETNATIVE_LOG_D("CmdAddStaticIpv6Addr has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdDelStaticIpv6Addr(MessageParcel &data, MessageParcel &reply)
{
    std::string ipAddr = "";
    if (!data.ReadString(ipAddr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string macAddr = "";
    if (!data.ReadString(macAddr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string ifName = "";
    if (!data.ReadString(ifName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    NETNATIVE_LOG_D("CmdDelStaticIpv6Addr has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetIpNeighTable(MessageParcel &data, MessageParcel &reply)
{
    std::vector<OHOS::NetManagerStandard::NetIpMacInfo> ipMacInfo;
    int32_t result = GetIpNeighTable(ipMacInfo);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (result == NETMANAGER_SUCCESS) {
        uint32_t size = static_cast<uint32_t>(ipMacInfo.size());
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        for (auto p = ipMacInfo.begin(); p != ipMacInfo.end(); ++p) {
            sptr<NetIpMacInfo> info_ptr = sptr<NetIpMacInfo>::MakeSptr(*p);
            if (!NetIpMacInfo::Marshalling(reply, info_ptr)) {
                NETNATIVE_LOGE("proxy Marshalling failed");
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdCreateVlan(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdCreateVlan CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    std::string ifName = data.ReadString();
    uint32_t vlanId = data.ReadUint32();
    int32_t result = CreateVlan(ifName, vlanId);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdCreateVlan result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdDestroyVlan(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdDestroyVlan CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    std::string ifName = data.ReadString();
    uint32_t vlanId = data.ReadUint32();
    int32_t result = DestroyVlan(ifName, vlanId);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdDestroyVlan result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdAddVlanIp(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdAddVlanIp CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    std::string ifName = data.ReadString();
    uint32_t vlanId = data.ReadUint32();
    std::string ip = data.ReadString();
    uint32_t mask = data.ReadUint32();
    int32_t result = AddVlanIp(ifName, vlanId, ip, mask);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdAddVlanIp result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdGetConnectOwnerUid(MessageParcel &data, MessageParcel &reply)
{
    sptr<NetConnInfo> netConnInfo = NetConnInfo::Unmarshalling(data);
    if (netConnInfo == nullptr) {
        NETNATIVE_LOGE("netConnInfo is nullptr");
        return IPC_STUB_ERR;
    }
    int32_t ownerUid = INVALID_UID;
    int32_t ret = GetConnectOwnerUid(*netConnInfo, ownerUid);
    if (!reply.WriteInt32(ret)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteInt32(ownerUid)) {
            return ERR_FLATTEN_OBJECT;
        }
    }
    return ret;
}

int32_t NetsysNativeServiceStub::CmdGetSystemNetPortStates(MessageParcel &data, MessageParcel &reply)
{
    NetPortStatesInfo netPortStatesInfo;
    int32_t result = GetSystemNetPortStates(netPortStatesInfo);
    // LCOV_EXCL_START This will never happen.
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (result == NETMANAGER_SUCCESS) {
        sptr<NetPortStatesInfo> info_ptr = sptr<NetPortStatesInfo>::MakeSptr(netPortStatesInfo);
        if (!NetPortStatesInfo::Marshalling(reply, info_ptr)) {
            NETNATIVE_LOGE("proxy Marshalling failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    // LCOV_EXCL_STOP
    return result;
}

int32_t NetsysNativeServiceStub::CmdRegisterDnsResultListener(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        result = IPC_STUB_ERR;
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetDnsResultCallback> callback = iface_cast<INetDnsResultCallback>(remote);
    if (callback == nullptr) {
        result = ERR_FLATTEN_OBJECT;
        reply.WriteInt32(result);
        return result;
    }

    uint32_t delay;
    if (!data.ReadUint32(delay)) {
        NETNATIVE_LOGE("Read uint32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    result = RegisterDnsResultCallback(callback, delay);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdUnregisterDnsResultListener(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        result = IPC_STUB_ERR;
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetDnsResultCallback> callback = iface_cast<INetDnsResultCallback>(remote);
    if (callback == nullptr) {
        result = ERR_FLATTEN_OBJECT;
        reply.WriteInt32(result);
        return result;
    }

    result = UnregisterDnsResultCallback(callback);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdGetCookieStats(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type = 0;
    if (!data.ReadUint32(type)) {
        NETNATIVE_LOGE("Read uint32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    uint64_t cookie = 0;
    if (!data.ReadUint64(cookie)) {
        NETNATIVE_LOGE("Read uint64 failed");
        return ERR_FLATTEN_OBJECT;
    }

    uint64_t stats = 0;
    int32_t result = GetCookieStats(stats, type, cookie);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!reply.WriteUint64(stats)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdGetNetworkSharingType(MessageParcel &data, MessageParcel &reply)
{
    std::set<uint32_t> sharingTypeIsOn;
    int32_t ret = GetNetworkSharingType(sharingTypeIsOn);
    if (!reply.WriteInt32(ret)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (sharingTypeIsOn.size() > MAX_SHARING_TYPE_SIZE) {
        NETNATIVE_LOGE("Write size error");
        return ERR_FLATTEN_OBJECT;
    }
    if (!reply.WriteUint32(sharingTypeIsOn.size())) {
            NETNATIVE_LOGE("Write parcel failed");
            return ERR_FLATTEN_OBJECT;
    }
    for (auto mem : sharingTypeIsOn) {
        if (!reply.WriteUint32(mem)) {
            NETNATIVE_LOGE("Write parcel failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    
    return ret;
}

int32_t NetsysNativeServiceStub::CmdUpdateNetworkSharingType(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type = ERR_NONE;
    if (!data.ReadUint32(type)) {
        NETNATIVE_LOGE("Read uint32 failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (type < ERR_NONE) {
        NETNATIVE_LOGE("type parameter invalid");
        return ERR_INVALID_DATA;
    }

    bool isOpen = false;
    if (!data.ReadBool(isOpen)) {
        NETNATIVE_LOGE("Read bool failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t ret = UpdateNetworkSharingType(type, isOpen);
    if (!reply.WriteInt32(ret)) {
        NETNATIVE_LOGE("Write parcel failed");
        return ERR_FLATTEN_OBJECT;
    }

    return ret;
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
int32_t NetsysNativeServiceStub::CmdSetFirewallRules(MessageParcel &data, MessageParcel &reply)
{
    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        NETNATIVE_LOGE("Read rule type failed");
        return ERR_FLATTEN_OBJECT;
    }
    NetFirewallRuleType ruleType = static_cast<NetFirewallRuleType>(type);
    uint32_t size = 0;
    if (!data.ReadUint32(size)) {
        NETNATIVE_LOGE("Read size failed");
        return ERR_FLATTEN_OBJECT;
    }
    NETNATIVE_LOGI("NetsysNativeServiceStub::CmdSetFirewallRules ruleType=%{public}d, size=%{public}d", ruleType, size);
    uint32_t maxSize =
        ruleType == NetFirewallRuleType::RULE_IP ? FIREWALL_IPC_IP_RULE_PAGE_SIZE : FIREWALL_RULE_SIZE_MAX;
    if (size > maxSize) {
        return FIREWALL_ERR_EXCEED_MAX_IP;
    }
    bool isFinish = false;
    if (!data.ReadBool(isFinish)) {
        NETNATIVE_LOGE("Read isFinish failed");
        return ERR_FLATTEN_OBJECT;
    }
    std::vector<sptr<NetFirewallBaseRule>> ruleList;
    for (uint32_t i = 0; i < size; i++) {
        sptr<NetFirewallBaseRule> rule = nullptr;
        if (ruleType == NetFirewallRuleType::RULE_IP) {
            rule = NetFirewallIpRule::Unmarshalling(data);
        } else if (ruleType == NetFirewallRuleType::RULE_DOMAIN) {
            rule = NetFirewallDomainRule::Unmarshalling(data);
        } else if (ruleType == NetFirewallRuleType::RULE_DNS) {
            rule = NetFirewallDnsRule::Unmarshalling(data);
        }
        if (rule != nullptr) {
            ruleList.emplace_back(std::move(rule));
        }
    }
    return SetFirewallRules(ruleType, ruleList, isFinish);
}

int32_t NetsysNativeServiceStub::CmdSetFirewallDefaultAction(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOGI("NetsysNativeServiceStub::CmdSetFirewallDefaultAction");
    int32_t userId = 0;
    if (!data.ReadInt32(userId)) {
        NETNATIVE_LOGE("Read userId failed");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t inDefault = 0;
    if (!data.ReadInt32(inDefault)) {
        NETNATIVE_LOGE("Read inDefault failed");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t outDefault = 0;
    if (!data.ReadInt32(outDefault)) {
        NETNATIVE_LOGE("Read outDefault failed");
        return ERR_FLATTEN_OBJECT;
    }
    return SetFirewallDefaultAction(userId, static_cast<FirewallRuleAction>(inDefault),
                                    static_cast<FirewallRuleAction>(outDefault));
}

int32_t NetsysNativeServiceStub::CmdSetFirewallCurrentUserId(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOGI("NetsysNativeServiceStub::CmdSetFirewallCurrentUserId");
    int32_t userId = 0;
    if (!data.ReadInt32(userId)) {
        NETNATIVE_LOGE("Read userId failed");
        return ERR_FLATTEN_OBJECT;
    }
    return SetFirewallCurrentUserId(userId);
}

int32_t NetsysNativeServiceStub::CmdClearFirewallRules(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOGI("NetsysNativeServiceStub::CmdClearFirewallRules");
    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        NETNATIVE_LOGE("Read clear type failed");
        return ERR_FLATTEN_OBJECT;
    }

    NetFirewallRuleType clearType = static_cast<NetFirewallRuleType>(type);
    return ClearFirewallRules(clearType);
}

int32_t NetsysNativeServiceStub::CmdRegisterNetFirewallCallback(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        result = IPC_STUB_ERR;
        return result;
    }

    sptr<INetFirewallCallback> callback = iface_cast<INetFirewallCallback>(remote);
    if (callback == nullptr) {
        result = ERR_FLATTEN_OBJECT;
        return result;
    }

    return RegisterNetFirewallCallback(callback);
}

int32_t NetsysNativeServiceStub::CmdUnRegisterNetFirewallCallback(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        result = IPC_STUB_ERR;
        return result;
    }

    sptr<INetFirewallCallback> callback = iface_cast<INetFirewallCallback>(remote);
    if (callback == nullptr) {
        result = ERR_FLATTEN_OBJECT;
        return result;
    }

    return UnRegisterNetFirewallCallback(callback);
}
#endif

int32_t NetsysNativeServiceStub::CmdSetIpv6PrivacyExtensions(MessageParcel &data, MessageParcel &reply)
{
    std::string interfaceName = data.ReadString();
    int32_t on = data.ReadInt32();

    int32_t result = SetIpv6PrivacyExtensions(interfaceName, on);
    reply.WriteInt32(result);
    NETNATIVE_LOGI("SetIpv6PrivacyExtensions has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdSetIpv6Enable(MessageParcel &data, MessageParcel &reply)
{
    std::string interfaceName = data.ReadString();
    int32_t on = data.ReadInt32();
    bool needRestart = data.ReadBool();

    int32_t result = SetEnableIpv6(interfaceName, on, needRestart);
    reply.WriteInt32(result);
    NETNATIVE_LOGI("SetIpv6Enable has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdSetIpv6UidBlackList(MessageParcel &data, MessageParcel &reply)
{
    int32_t size = 0;
    if (!data.ReadInt32(size)) {
        NETNATIVE_LOGE("CmdSetIpv6UidBlackList read size failed");
        return ERR_FLATTEN_OBJECT;
    }

    if (size < 0) {
        return ERR_FLATTEN_OBJECT;
    }
    
    std::vector<int32_t> netIds;
    int32_t netId;
    for (int32_t index = 0; index < size; index++) {
        if (!data.ReadInt32(netId)) {
            NETNATIVE_LOGE("CmdSetIpv6UidBlackList read netId failed");
            return ERR_FLATTEN_OBJECT;
        }
        if (netId == 0) {
            NETNATIVE_LOGE("CmdSetIpv6UidBlackList netId is zero");
            return ERR_FLATTEN_OBJECT;
        }
        netIds.emplace_back(netId);
    }
 
    int32_t uid = 0;
    if (!data.ReadInt32(uid)) {
        NETNATIVE_LOGE("CmdSetIpv6UidBlackList read uid failed");
        return ERR_FLATTEN_OBJECT;
    }
 
    int32_t result = SetIpv6UidBlackList(netIds, uid);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdSetIpv6UidBlackList result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdSetIpv6AutoConf(MessageParcel &data, MessageParcel &reply)
{
    std::string interfaceName = data.ReadString();
    int32_t on = data.ReadInt32();

    int32_t result = SetIpv6AutoConf(interfaceName, on);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetIpv6AutoConf has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdSetNetworkAccessPolicy(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETNATIVE_LOGE("Read uint32 failed");
        return ERR_FLATTEN_OBJECT;
    }
    uint8_t wifi_allow = 0;
    if (!data.ReadUint8(wifi_allow)) {
        NETNATIVE_LOGE("Read uint8 failed");
        return ERR_FLATTEN_OBJECT;
    }
    uint8_t cellular_allow = 0;
    if (!data.ReadUint8(cellular_allow)) {
        NETNATIVE_LOGE("Read uint8 failed");
        return ERR_FLATTEN_OBJECT;
    }
    bool reconfirmFlag = true;
    if (!data.ReadBool(reconfirmFlag)) {
        NETNATIVE_LOGE("Read bool failed");
        return ERR_FLATTEN_OBJECT;
    }

    NetworkAccessPolicy policy;
    policy.wifiAllow = wifi_allow;
    policy.cellularAllow = cellular_allow;
    int32_t result = SetNetworkAccessPolicy(uid, policy, reconfirmFlag);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdDelNetworkAccessPolicy(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETNATIVE_LOGE("Read uint32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = DeleteNetworkAccessPolicy(uid);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdNotifyNetBearerTypeChange(MessageParcel &data, MessageParcel &reply)
{
    std::set<NetBearType> bearerTypes;

    uint32_t size = 0;
    uint32_t value = 0;
    if (!data.ReadUint32(size)) {
        return ERR_FLATTEN_OBJECT;
    }
    size = (size > BEARER_DEFAULT) ? BEARER_DEFAULT : size;
    for (uint32_t i = 0; i < size; i++) {
        if (!data.ReadUint32(value)) {
            return ERR_FLATTEN_OBJECT;
        }
        if (value >= BEARER_DEFAULT) {
            return ERR_FLATTEN_OBJECT;
        }
        bearerTypes.insert(static_cast<NetBearType>(value));
    }
    int32_t result = NotifyNetBearerTypeChange(bearerTypes);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdStartClat(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdStartClat CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    std::string interfaceName;
    if (!data.ReadString(interfaceName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        NETNATIVE_LOGE("Read int32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string nat64PrefixStr;
    if (!data.ReadString(nat64PrefixStr)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = StartClat(interfaceName, netId, nat64PrefixStr);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdStopClat(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdStopClat CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    std::string interfaceName;
    if (!data.ReadString(interfaceName)) {
        NETNATIVE_LOGE("Read string failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = StopClat(interfaceName);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdClearFirewallAllRules(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdClearFirewallAllRules");
    int32_t result = ClearFirewallAllRules();
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdSetNicTrafficAllowed(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdSetNicTrafficAllowed CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    bool status = false;
    int32_t size = 0;
    if (!data.ReadBool(status) || !data.ReadInt32(size)) {
        NETNATIVE_LOGE("CmdSetNicTrafficAllowed read status or size failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (size > static_cast<int32_t>(MAX_IFACENAMES_SIZE)) {
        NETNATIVE_LOGE("CmdSetNicTrafficAllowed read data size too big");
        return ERR_FLATTEN_OBJECT;
    }
    std::vector<std::string> ifaceNames;
    std::string ifaceName;
    for (int32_t index = 0; index < size; index++) {
        data.ReadString(ifaceName);
        if (ifaceName.empty()) {
            NETNATIVE_LOGE("CmdSetNicTrafficAllowed ifaceName is empty, size mismatch");
            return ERR_FLATTEN_OBJECT;
        }
        ifaceNames.push_back(ifaceName);
    }
    int32_t result = SetNicTrafficAllowed(ifaceNames, status);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdSetNicTrafficAllowed result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

#ifdef SUPPORT_SYSVPN
int32_t NetsysNativeServiceStub::CmdProcessVpnStage(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdProcessVpnStage CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    int32_t stage = 0;
    if (!data.ReadInt32(stage)) {
        return ERR_FLATTEN_OBJECT;
    }

    std::string message;
    if (!data.ReadString(message)) {
        return ERR_FLATTEN_OBJECT;
    }
    int32_t result = ProcessVpnStage(static_cast<NetsysNative::SysVpnStageCode>(stage), message);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdProcessVpnStage result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdUpdateVpnRules(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdUpdateVpnRules CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    uint16_t netId = 0;
    if (!data.ReadUint16(netId)) {
        NETNATIVE_LOGE("CmdUpdateVpnRules read netId failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t size = 0;
    if (!data.ReadInt32(size)) {
        NETNATIVE_LOGE("CmdUpdateVpnRules read size failed");
        return ERR_FLATTEN_OBJECT;
    }

    if (size < 0 || size > MAX_VNIC_UID_ARRAY_SIZE) {
        NETNATIVE_LOGE("CmdUpdateVpnRules size is invalid");
        return ERR_FLATTEN_OBJECT;
    }

    std::vector<std::string> extMessages;
    std::string extMessage;
    for (int32_t index = 0; index < size; index++) {
        if (!data.ReadString(extMessage)) {
            return ERR_FLATTEN_OBJECT;
        }
        if (extMessage.empty()) {
            NETNATIVE_LOGE("CmdUpdateVpnRules extMessage is empty, size mismatch");
            return ERR_FLATTEN_OBJECT;
        }
        extMessages.push_back(extMessage);
    }

    bool add = false;
    if (!data.ReadBool(add)) {
        NETNATIVE_LOGE("CmdUpdateVpnRules read flag failed");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t result = UpdateVpnRules(netId, extMessages, add);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdUpdateVpnRules result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}
#endif // SUPPORT_SYSVPN

int32_t NetsysNativeServiceStub::CmdCloseSocketsUid(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to CmdCloseSocketsUid");
    std::string ipAddr = data.ReadString();
    uint32_t uid = data.ReadUint32();
    int32_t result = CloseSocketsUid(ipAddr, uid);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdSetBrokerUidAccessPolicyMap(MessageParcel &data, MessageParcel &reply)
{
    std::unordered_map<uint32_t, uint32_t> params;
    uint32_t count = 0;
    if (!data.ReadUint32(count)) {
        NETNATIVE_LOGE("Read count failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (count > UINT16_MAX) {
        NETNATIVE_LOGE("count too big");
        return ERR_FLATTEN_OBJECT;
    }
    uint32_t key = 0;
    uint32_t value = 0;
    for (uint32_t i = 0; i < count; i++) {
        if (!data.ReadUint32(key) || !data.ReadUint32(value)) {
            NETNATIVE_LOGE("Read param failed.");
            return ERR_FLATTEN_OBJECT;
        }
        params.emplace(key, value);
    }
    int32_t result = SetBrokerUidAccessPolicyMap(params);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NETSYS_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdDelBrokerUidAccessPolicyMap(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETNATIVE_LOGE("Read uid failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = DelBrokerUidAccessPolicyMap(uid);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NETSYS_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdSetUserDefinedServerFlag(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdSetUserDefinedServerFlag CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    bool flag = false;
    uint16_t netId = 0;
    if (!data.ReadUint16(netId)) {
        NETNATIVE_LOGE("CmdSetUserDefinedServerFlag read netId failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.ReadBool(flag)) {
        NETNATIVE_LOGE("CmdSetUserDefinedServerFlag read flag failed");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t result = SetUserDefinedServerFlag(netId, flag);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdSetUserDefinedServerFlag result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdFlushDnsCache(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdFlushDnsCache CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    uint16_t netId = 0;
    if (!data.ReadUint16(netId)) {
        NETNATIVE_LOGE("CmdFlushDnsCache read netId failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = FlushDnsCache(netId);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdFlushDnsCache result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysNativeServiceStub::CmdSetDnsCache(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdSetDnsCache CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    uint16_t netId = 0;
    if (!data.ReadUint16(netId)) {
        NETNATIVE_LOGE("CmdSetDnsCache read netId failed");
        return ERR_FLATTEN_OBJECT;
    }
    
    std::string hostName;
    if (!data.ReadString(hostName)) {
        NETNATIVE_LOGE("CmdSetDnsCache read hostName failed");
        return ERR_FLATTEN_OBJECT;
    }

    AddrInfo addrInfo = {};
    auto p = data.ReadRawData(sizeof(AddrInfo));
    if (p == nullptr) {
        NETNATIVE_LOGE("CmdSetDnsCache read addrInfo failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (memcpy_s(&addrInfo, sizeof(AddrInfo), p, sizeof(AddrInfo)) != EOK) {
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = SetDnsCache(netId, hostName, addrInfo);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdSetDnsCache result failed");
        return ERR_FLATTEN_OBJECT;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
int32_t NetsysNativeServiceStub::CmdUpdateEnterpriseRoute(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdUpdateEnterpriseRoute CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    
    std::string interfaceName;
    if (!data.ReadString(interfaceName)) {
        NETNATIVE_LOGE("CmdUpdateEnterpriseRoute read interfaceName failed");
        return ERR_FLATTEN_OBJECT;
    }
 
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETNATIVE_LOGE("CmdUpdateEnterpriseRoute read uid failed");
        return ERR_FLATTEN_OBJECT;
    }
 
    bool add = 0;
    if (!data.ReadBool(add)) {
        NETNATIVE_LOGE("CmdUpdateEnterpriseRoute read add failed");
        return ERR_FLATTEN_OBJECT;
    }
 
    int32_t result = UpdateEnterpriseRoute(interfaceName, uid, add);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdUpdateEnterpriseRoute result failed");
        return ERR_FLATTEN_OBJECT;
    }
 
    return NetManagerStandard::NETMANAGER_SUCCESS;
}
#endif

int32_t NetsysNativeServiceStub::CmdSetInternetAccessByIpForWifiShare(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerStandard::NetManagerPermission::CheckNetSysInternalPermission(
        NetManagerStandard::Permission::NETSYS_INTERNAL)) {
        NETNATIVE_LOGE("CmdSetInternetAccessByIpForWifiShare CheckNetSysInternalPermission failed");
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }

    std::string ipAddr;
    if (!data.ReadString(ipAddr)) {
        NETNATIVE_LOGE("CmdSetInternetAccessByIpForWifiShare read ipAddr failed");
        return ERR_FLATTEN_OBJECT;
    }

    uint8_t family = 0;
    if (!data.ReadUint8(family)) {
        NETNATIVE_LOGE("CmdSetInternetAccessByIpForWifiShare read family failed");
        return ERR_FLATTEN_OBJECT;
    }

    bool accessInternet = 0;
    if (!data.ReadBool(accessInternet)) {
        NETNATIVE_LOGE("CmdSetInternetAccessByIpForWifiShare read accessInternet failed");
        return ERR_FLATTEN_OBJECT;
    }

    std::string clientNetIfName;
    if (!data.ReadString(clientNetIfName)) {
        NETNATIVE_LOGE("CmdSetInternetAccessByIpForWifiShare read clientNetIfName failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t result = SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
    if (!reply.WriteInt32(result)) {
        NETNATIVE_LOGE("Write CmdSetInternetAccessByIpForWifiShare result failed");
        return ERR_FLATTEN_OBJECT;
    }
 
    return NetManagerStandard::NETMANAGER_SUCCESS;
}
} // namespace NetsysNative
} // namespace OHOS

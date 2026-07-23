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

#ifndef COMMUNICATIONNETMANAGERBASE_CONNECTION_EXEC_H
#define COMMUNICATIONNETMANAGERBASE_CONNECTION_EXEC_H

#include <netdb.h>

#include "bindsocket_context.h"
#include "getaddressbyname_context.h"
#include "getappnet_context.h"
#include "getdefaultnet_context.h"
#include "gethttpproxy_context.h"
#include "napi/native_api.h"
#include "none_params_context.h"
#include "parse_nethandle_context.h"
#include "register_context.h"
#include "setappnet_context.h"
#include "setglobalhttpproxy_context.h"
#include "setcustomdnsrule_context.h"
#include "deletecustomdnsrule_context.h"
#include "deletecustomdnsrules_context.h"
#include "factoryresetnetwork_context.h"
#include "setapphttpproxy_context.h"
#include "pacurl_context.h"
#include "setinterfaceup_context.h"
#include "setinterfaceipaddr_context.h"
#include "addnetworkroute_context.h"
#include "interfaceregister_context.h"
#include "getinterfaceconfig_context.h"
#include "registernetsupplier_context.h"
#include "unregisternetsupplier_context.h"
#include "netextattribute_context.h"
#include "getconnectowneruid_context.h"
#include "getipneightable_context.h"
#include "createvlan_context.h"
#include "destroyvlan_context.h"
#include "addvlanip_context.h"
#include "deletevlanip_context.h"
#include "getdns_context.h"
#include "getsystemnetportstates_context.h"
#include "querytraceroute_context.h"
#include "queryproberesult_context.h"

#define DECLARE_WRITABLE_NAPI_FUNCTION(name, func)                                  \
    {                                                                               \
        (name), nullptr, (func), nullptr, nullptr, nullptr, napi_writable, nullptr  \
    }
namespace OHOS::NetManagerStandard {
static constexpr size_t MAX_ARRAY_LENGTH = 64;
static constexpr size_t MAX_ROUTE_LENGTH = 1024;
static constexpr size_t MAX_IPV4_STR_LEN = 16;
static constexpr size_t MAX_IPV6_STR_LEN = 64;
class ConnectionExec final {
public:
    ConnectionExec() = delete;

    ~ConnectionExec() = delete;

    static napi_value CreateNetHandle(napi_env env, NetHandle *handle);

    static napi_value CreateNetCapabilities(napi_env env, NetAllCapabilities *capabilities);

    static napi_value CreateConnectionProperties(napi_env env, NetLinkInfo *linkInfo);

    static napi_value CreateIpNeighTable(napi_env env, const NetIpMacInfo &ipNeighTable);

    static napi_value CreateNetPortStatesInfo(napi_env env, const NetPortStatesInfo &netPortStatesInfo);

    static bool ExecGetDefaultNet(GetDefaultNetContext *context);

    static napi_value GetDefaultNetCallback(GetDefaultNetContext *context);

    static bool ExecHasDefaultNet(HasDefaultNetContext *context);

    static napi_value HasDefaultNetCallback(HasDefaultNetContext *context);

    static bool ExecIsDefaultNetMetered(IsDefaultNetMeteredContext *context);

    static napi_value IsDefaultNetMeteredCallback(IsDefaultNetMeteredContext *context);

    static bool ExecGetNetCapabilities(GetNetCapabilitiesContext *context);

    static napi_value GetNetCapabilitiesCallback(GetNetCapabilitiesContext *context);

    static bool ExecGetConnectionProperties(GetConnectionPropertiesContext *context);

    static napi_value GetConnectionPropertiesCallback(GetConnectionPropertiesContext *context);

    static bool ExecGetAddressByName(GetAddressByNameContext *context);

    static napi_value GetAddressByNameCallback(GetAddressByNameContext *context);

    static bool ExecGetAddressesByNameWithOptions(GetAddressByNameWithOptionsContext *context);

    static napi_value GetAddressesByNameWithOptionsCallback(GetAddressByNameWithOptionsContext *context);

    static bool ExecGetAllNets(GetAllNetsContext *context);

    static napi_value GetAllNetsCallback(GetAllNetsContext *context);

    static bool ExecEnableAirplaneMode(EnableAirplaneModeContext *context);

    static napi_value EnableAirplaneModeCallback(EnableAirplaneModeContext *context);

    static bool ExecDisableAirplaneMode(DisableAirplaneModeContext *context);

    static napi_value DisableAirplaneModeCallback(DisableAirplaneModeContext *context);

    static bool ExecReportNetConnected(ReportNetConnectedContext *context);

    static napi_value ReportNetConnectedCallback(ReportNetConnectedContext *context);

    static bool ExecReportNetDisconnected(ReportNetDisconnectedContext *context);

    static napi_value ReportNetDisconnectedCallback(ReportNetDisconnectedContext *context);

    static bool ExecGetDefaultHttpProxy(GetHttpProxyContext *context);

    static napi_value GetDefaultHttpProxyCallback(GetHttpProxyContext *context);

    static bool ExecGetGlobalHttpProxy(GetHttpProxyContext *context);

    static napi_value GetGlobalHttpProxyCallback(GetHttpProxyContext *context);

    static bool ExecSetGlobalHttpProxy(SetGlobalHttpProxyContext *context);

    static napi_value SetGlobalHttpProxyCallback(SetGlobalHttpProxyContext *context);

    static bool ExecRefreshGlobalHttpProxy(GetHttpProxyContext *context);

    static napi_value RefreshGlobalHttpProxyCallback(GetHttpProxyContext *context);

    static bool ExecSetAppHttpProxy(SetAppHttpProxyContext *context);

    static napi_value SetAppHttpProxyCallback(SetAppHttpProxyContext *context);

    static bool ExecGetAppNet(GetAppNetContext *context);

    static napi_value GetAppNetCallback(GetAppNetContext *context);

    static bool ExecSetAppNet(SetAppNetContext *context);

    static napi_value SetAppNetCallback(SetAppNetContext *context);

    static bool ExecSetInterfaceUp(SetInterfaceUpContext *context);

    static napi_value SetInterfaceUpCallback(SetInterfaceUpContext *context);

    static bool ExecSetInterfaceIpAddr(SetInterfaceIpAddrContext *context);

    static napi_value SetInterfaceIpAddrCallback(SetInterfaceIpAddrContext *context);

    static bool ExecAddNetworkRoute(AddNetworkRouteContext *context);

    static napi_value AddNetworkRouteCallback(AddNetworkRouteContext *context);

    static bool ExecGetNetInterfaceConfiguration(GetNetInterfaceConfigurationContext *context);

    static napi_value GetNetInterfaceConfigurationCallback(GetNetInterfaceConfigurationContext *context);

    static bool ExecRegisterNetSupplier(RegisterNetSupplierContext *context);

    static napi_value RegisterNetSupplierCallback(RegisterNetSupplierContext *context);

    static bool ExecUnregisterNetSupplier(UnregisterNetSupplierContext *context);

    static napi_value UnregisterNetSupplierCallback(UnregisterNetSupplierContext *context);

    static bool ExecSetCustomDNSRule(SetCustomDNSRuleContext *context);

    static napi_value SetCustomDNSRuleCallback(SetCustomDNSRuleContext *context);

    static bool ExecDeleteCustomDNSRule(DeleteCustomDNSRuleContext *context);

    static napi_value DeleteCustomDNSRuleCallback(DeleteCustomDNSRuleContext *context);

    static bool ExecDeleteCustomDNSRules(DeleteCustomDNSRulesContext *context);

    static napi_value DeleteCustomDNSRulesCallback(DeleteCustomDNSRulesContext *context);

    static bool ExecFactoryResetNetwork(FactoryResetNetworkContext *context);

    static napi_value FactoryResetNetworkCallback(FactoryResetNetworkContext *context);

    static bool ExecSetPacUrl(SetPacUrlContext *context);

    static napi_value SetPacUrlCallback(SetPacUrlContext *context);

    static bool ExecGetPacUrl(GetPacUrlContext *context);

    static napi_value GetPacUrlCallback(GetPacUrlContext *context);

    static bool ExecGetNetExtAttribute(GetNetExtAttributeContext *context);

    static napi_value GetNetExtAttributeCallback(GetNetExtAttributeContext *context);

    static bool ExecSetNetExtAttribute(SetNetExtAttributeContext *context);

    static napi_value SetNetExtAttributeCallback(SetNetExtAttributeContext *context);

    static bool ExecSetPacFileUrl(SetPacFileUrlContext *context);

    static bool ExecSetProxyMode(ProxyModeContext *context);

    static bool ExecGetProxyMode(ProxyModeContext *context);

    static napi_value SetProxyModeCallback(ProxyModeContext *context);

    static napi_value GetProxyModeCallback(ProxyModeContext *context);

    static napi_value SetPacFileUrlCallback(SetPacFileUrlContext *context);

    static bool ExecGetPacFileUrl(GetPacFileUrlContext *context);

    static napi_value GetPacFileUrlCallback(GetPacFileUrlContext *context);

    static bool ExecFindProxyForUrl(FindPacFileUrlContext *context);

    static napi_value FindProxyForUrlCallback(FindPacFileUrlContext *context);

    static bool ExecGetIpNeighTable(GetIpNeighTableContext *context);

    static napi_value GetIpNeighTableCallback(GetIpNeighTableContext *context);

    static bool ExecCreateVlan(CreateVlanContext *context);

    static napi_value CreateVlanCallback(CreateVlanContext *context);

    static bool ExecDestroyVlan(DestroyVlanContext *context);

    static napi_value DestroyVlanCallback(DestroyVlanContext *context);

    static bool ExecAddVlanIp(AddVlanIpContext *context);

    static napi_value AddVlanIpCallback(AddVlanIpContext *context);

    static bool ExecDeleteVlanIp(DeleteVlanIpContext *context);

    static napi_value DeleteVlanIpCallback(DeleteVlanIpContext *context);

    static bool ExecGetConnectOwnerUid(GetConnectOwnerUidContext *context);

    static napi_value GetConnectOwnerUidCallback(GetConnectOwnerUidContext *context);

    static bool ExecGetDnsASCII(GetDnsContext *context);

    static bool ExecGetDnsUnicode(GetDnsContext *context);

    static napi_value GetDnsCallback(GetDnsContext *context);
 
    static bool ExecGetSystemNetPortStates(GetSystemNetPortStatesContext *context);

    static napi_value GetSystemNetPortStatesCallback(GetSystemNetPortStatesContext *context);

    static bool ExecQueryProbeResult(QueryProbeResultContext *context);

    static napi_value QueryProbeResultCallback(QueryProbeResultContext *context);

    static bool ExecQueryTraceRoute(QueryTraceRouteContext *context);

    static napi_value QueryTraceRouteCallback(QueryTraceRouteContext *context);

    static napi_value CreateTraceRouteInfo(napi_env env, const TraceRouteInfo &traceRouteInfo);

    class NetHandleExec final {
    public:
        NetHandleExec() = delete;

        ~NetHandleExec() = delete;

        static bool ExecGetAddressByName(GetAddressByNameContext *context);

        static napi_value GetAddressByNameCallback(GetAddressByNameContext *context);

        static bool ExecGetAddressesByName(GetAddressByNameContext *context);

        static napi_value GetAddressesByNameCallback(GetAddressByNameContext *context);
        
        static bool ExecGetAddressesByNameWithOptions(GetAddressByNameWithOptionsContext *context);

        static napi_value GetAddressesByNameWithOptionsCallback(GetAddressByNameWithOptionsContext *context);

        static bool ExecBindSocket(BindSocketContext *context);

        static napi_value BindSocketCallback(BindSocketContext *context);

    private:
        static napi_value MakeNetAddressJsValue(napi_env env, const NetAddress &address);

        static void SetAddressInfo(const char *host, addrinfo *info, NetAddress &address);
    };

    class NetConnectionExec final {
    public:
        NetConnectionExec() = delete;

        ~NetConnectionExec() = delete;

        static bool ExecRegister(RegisterContext *context);

        static napi_value RegisterCallback(RegisterContext *context);

        static bool ExecUnregister(UnregisterContext *context);

        static napi_value UnregisterCallback(UnregisterContext *context);
    };

    class NetInterfaceExec final {
    public:
        NetInterfaceExec() = delete;

        ~NetInterfaceExec() = delete;

        static bool ExecIfaceRegister(IfaceRegisterContext *context);

        static napi_value IfaceRegisterCallback(IfaceRegisterContext *context);

        static bool ExecIfaceUnregister(IfaceUnregisterContext *context);

        static napi_value IfaceUnregisterCallback(IfaceUnregisterContext *context);
    };

private:
    static void FillLinkAddress(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo);

    static void FillRouoteList(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo);

    static void FillDns(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo);

    static void FillNetAddressInfo(napi_env env, napi_value jsNetAddress, const NetIpMacInfo &ipNeighTable);

    static int ConvertToAiFamily(GetAddressByNameWithOptionsContext::Family family);
};

extern std::mutex g_predefinedHostMtx;
} // namespace OHOS::NetManagerStandard

#endif /* COMMUNICATIONNETMANAGERBASE_CONNECTION_EXEC_H */

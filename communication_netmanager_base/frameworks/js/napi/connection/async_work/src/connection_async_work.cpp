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

#include "connection_async_work.h"

#include "connection_exec.h"
#include "base_async_work.h"
#include "getappnet_context.h"
#include "gethttpproxy_context.h"
#include "none_params_context.h"
#include "parse_nethandle_context.h"
#include "setappnet_context.h"
#include "setglobalhttpproxy_context.h"
#include "setcustomdnsrule_context.h"
#include "deletecustomdnsrule_context.h"
#include "deletecustomdnsrules_context.h"
#include "setinterfaceup_context.h"
#include "setinterfaceipaddr_context.h"
#include "addnetworkroute_context.h"
#include "interfaceregister_context.h"
#include "getconnectowneruid_context.h"
#include "getinterfaceconfig_context.h"
#include "registernetsupplier_context.h"
#include "unregisternetsupplier_context.h"

namespace OHOS::NetManagerStandard {
void ConnectionAsyncWork::ExecGetAddressesByName(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAddressByNameContext, ConnectionExec::ExecGetAddressByName>(env, data);
}

void ConnectionAsyncWork::GetAddressesByNameCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAddressByNameContext, ConnectionExec::GetAddressByNameCallback>(env, status,
                                                                                                        data);
}

void ConnectionAsyncWork::ExecGetAddressesByNameWithOptions(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAddressByNameWithOptionsContext, ConnectionExec::ExecGetAddressesByNameWithOptions>(
        env, data);
}

void ConnectionAsyncWork::GetAddressesByNameWithOptionsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAddressByNameWithOptionsContext,
                                        ConnectionExec::GetAddressesByNameWithOptionsCallback>(
        env, status, data);
}
void ConnectionAsyncWork::ExecHasDefaultNet(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<HasDefaultNetContext, ConnectionExec::ExecHasDefaultNet>(env, data);
}

void ConnectionAsyncWork::HasDefaultNetCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<HasDefaultNetContext, ConnectionExec::HasDefaultNetCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecIsDefaultNetMetered(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<IsDefaultNetMeteredContext, ConnectionExec::ExecIsDefaultNetMetered>(env, data);
}

void ConnectionAsyncWork::IsDefaultNetMeteredCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<IsDefaultNetMeteredContext, ConnectionExec::IsDefaultNetMeteredCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecSetProxyMode(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ProxyModeContext, ConnectionExec::ExecSetProxyMode>(env, data);
}

void ConnectionAsyncWork::ExecGetProxyMode(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ProxyModeContext, ConnectionExec::ExecGetProxyMode>(env, data);
}

void ConnectionAsyncWork::ExecGetNetCapabilities(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetNetCapabilitiesContext, ConnectionExec::ExecGetNetCapabilities>(env, data);
}

void ConnectionAsyncWork::GetProxyModeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ProxyModeContext, ConnectionExec::GetProxyModeCallback>(env, status, data);
}

void ConnectionAsyncWork::SetProxyModeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ProxyModeContext, ConnectionExec::SetProxyModeCallback>(env, status, data);
}

void ConnectionAsyncWork::GetNetCapabilitiesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetNetCapabilitiesContext, ConnectionExec::GetNetCapabilitiesCallback>(env, status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecGetConnectionProperties(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetConnectionPropertiesContext, ConnectionExec::ExecGetConnectionProperties>(env,
                                                                                                              data);
}

void ConnectionAsyncWork::GetConnectionPropertiesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetConnectionPropertiesContext, ConnectionExec::GetConnectionPropertiesCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecGetDefaultNet(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetDefaultNetContext, ConnectionExec::ExecGetDefaultNet>(env, data);
}

void ConnectionAsyncWork::GetDefaultNetCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetDefaultNetContext, ConnectionExec::GetDefaultNetCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecGetAllNets(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAllNetsContext, ConnectionExec::ExecGetAllNets>(env, data);
}

void ConnectionAsyncWork::GetAllNetsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAllNetsContext, ConnectionExec::GetAllNetsCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecEnableAirplaneMode(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<EnableAirplaneModeContext, ConnectionExec::ExecEnableAirplaneMode>(env, data);
}

void ConnectionAsyncWork::EnableAirplaneModeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<EnableAirplaneModeContext, ConnectionExec::EnableAirplaneModeCallback>(env, status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecDisableAirplaneMode(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<DisableAirplaneModeContext, ConnectionExec::ExecDisableAirplaneMode>(env, data);
}

void ConnectionAsyncWork::DisableAirplaneModeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<DisableAirplaneModeContext, ConnectionExec::DisableAirplaneModeCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecReportNetConnected(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ReportNetConnectedContext, ConnectionExec::ExecReportNetConnected>(env, data);
}

void ConnectionAsyncWork::ReportNetConnectedCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ReportNetConnectedContext, ConnectionExec::ReportNetConnectedCallback>(env, status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecReportNetDisconnected(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ReportNetDisconnectedContext, ConnectionExec::ExecReportNetDisconnected>(env, data);
}

void ConnectionAsyncWork::ReportNetDisconnectedCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ReportNetDisconnectedContext, ConnectionExec::ReportNetDisconnectedCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecGetDefaultHttpProxy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetHttpProxyContext, ConnectionExec::ExecGetDefaultHttpProxy>(env, data);
}

void ConnectionAsyncWork::GetDefaultHttpProxyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetHttpProxyContext, ConnectionExec::GetDefaultHttpProxyCallback>(env, status,
                                                                                                       data);
}

void ConnectionAsyncWork::ExecGetGlobalHttpProxy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetHttpProxyContext, ConnectionExec::ExecGetGlobalHttpProxy>(env, data);
}

void ConnectionAsyncWork::GetGlobalHttpProxyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetHttpProxyContext, ConnectionExec::GetGlobalHttpProxyCallback>(env, status,
                                                                                                      data);
}

void ConnectionAsyncWork::ExecSetGlobalHttpProxy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetGlobalHttpProxyContext, ConnectionExec::ExecSetGlobalHttpProxy>(env, data);
}

void ConnectionAsyncWork::SetGlobalHttpProxyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetGlobalHttpProxyContext, ConnectionExec::SetGlobalHttpProxyCallback>(env, status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecRefreshGlobalHttpProxy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetHttpProxyContext, ConnectionExec::ExecRefreshGlobalHttpProxy>(env, data);
}

void ConnectionAsyncWork::RefreshGlobalHttpProxyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetHttpProxyContext, ConnectionExec::RefreshGlobalHttpProxyCallback>(env, status,
                                                                                                          data);
}

void ConnectionAsyncWork::ExecGetAppNet(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAppNetContext, ConnectionExec::ExecGetAppNet>(env, data);
}

void ConnectionAsyncWork::GetAppNetCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAppNetContext, ConnectionExec::GetAppNetCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecSetAppNet(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetAppNetContext, ConnectionExec::ExecSetAppNet>(env, data);
}

void ConnectionAsyncWork::SetAppNetCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetAppNetContext, ConnectionExec::SetAppNetCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecSetInterfaceUp(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetInterfaceUpContext, ConnectionExec::ExecSetInterfaceUp>(env, data);
}

void ConnectionAsyncWork::SetInterfaceUpCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetInterfaceUpContext, ConnectionExec::SetInterfaceUpCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecSetInterfaceIpAddr(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetInterfaceIpAddrContext, ConnectionExec::ExecSetInterfaceIpAddr>(env, data);
}

void ConnectionAsyncWork::SetInterfaceIpAddrCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetInterfaceIpAddrContext, ConnectionExec::SetInterfaceIpAddrCallback>(env,
                                                                                                            status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecGetIpNeighTable(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetIpNeighTableContext, ConnectionExec::ExecGetIpNeighTable>(env, data);
}

void ConnectionAsyncWork::GetIpNeighTableCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetIpNeighTableContext,
        ConnectionExec::GetIpNeighTableCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecCreateVlan(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<CreateVlanContext, ConnectionExec::ExecCreateVlan>(env, data);
}

void ConnectionAsyncWork::CreateVlanCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<CreateVlanContext,
        ConnectionExec::CreateVlanCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecDestroyVlan(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<DestroyVlanContext, ConnectionExec::ExecDestroyVlan>(env, data);
}

void ConnectionAsyncWork::DestroyVlanCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<DestroyVlanContext,
        ConnectionExec::DestroyVlanCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecAddVlanIp(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<AddVlanIpContext, ConnectionExec::ExecAddVlanIp>(env, data);
}

void ConnectionAsyncWork::AddVlanIpCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<AddVlanIpContext,
        ConnectionExec::AddVlanIpCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecDeleteVlanIp(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<DeleteVlanIpContext, ConnectionExec::ExecDeleteVlanIp>(env, data);
}

void ConnectionAsyncWork::DeleteVlanIpCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<DeleteVlanIpContext,
        ConnectionExec::DeleteVlanIpCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecGetConnectOwnerUid(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetConnectOwnerUidContext, ConnectionExec::ExecGetConnectOwnerUid>(env, data);
}

void ConnectionAsyncWork::GetConnectOwnerUidCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetConnectOwnerUidContext, ConnectionExec::GetConnectOwnerUidCallback>(env, status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecGetSystemNetPortStates(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetSystemNetPortStatesContext, ConnectionExec::ExecGetSystemNetPortStates>(env, data);
}

void ConnectionAsyncWork::GetSystemNetPortStatesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetSystemNetPortStatesContext,
        ConnectionExec::GetSystemNetPortStatesCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecAddNetworkRoute(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<AddNetworkRouteContext, ConnectionExec::ExecAddNetworkRoute>(env, data);
}

void ConnectionAsyncWork::AddNetworkRouteCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<AddNetworkRouteContext, ConnectionExec::AddNetworkRouteCallback>(env, status,
                                                                                                      data);
}

void ConnectionAsyncWork::ExecGetNetInterfaceConfiguration(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetNetInterfaceConfigurationContext,
        ConnectionExec::ExecGetNetInterfaceConfiguration>(env, data);
}

void ConnectionAsyncWork::GetNetInterfaceConfigurationCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetNetInterfaceConfigurationContext,
        ConnectionExec::GetNetInterfaceConfigurationCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecRegisterNetSupplier(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<RegisterNetSupplierContext,
        ConnectionExec::ExecRegisterNetSupplier>(env, data);
}

void ConnectionAsyncWork::RegisterNetSupplierCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<RegisterNetSupplierContext,
        ConnectionExec::RegisterNetSupplierCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecUnregisterNetSupplier(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UnregisterNetSupplierContext,
        ConnectionExec::ExecUnregisterNetSupplier>(env, data);
}

void ConnectionAsyncWork::UnregisterNetSupplierCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<UnregisterNetSupplierContext,
        ConnectionExec::UnregisterNetSupplierCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecSetCustomDNSRule(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetCustomDNSRuleContext, ConnectionExec::ExecSetCustomDNSRule>(env, data);
}

void ConnectionAsyncWork::SetCustomDNSRuleCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetCustomDNSRuleContext, ConnectionExec::SetCustomDNSRuleCallback>(env, status,
                                                                                                        data);
}

void ConnectionAsyncWork::ExecDeleteCustomDNSRule(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<DeleteCustomDNSRuleContext, ConnectionExec::ExecDeleteCustomDNSRule>(env, data);
}

void ConnectionAsyncWork::DeleteCustomDNSRuleCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<DeleteCustomDNSRuleContext, ConnectionExec::DeleteCustomDNSRuleCallback>(env,
        status, data);
}

void ConnectionAsyncWork::ExecDeleteCustomDNSRules(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<DeleteCustomDNSRulesContext, ConnectionExec::ExecDeleteCustomDNSRules>(env, data);
}

void ConnectionAsyncWork::DeleteCustomDNSRulesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<DeleteCustomDNSRulesContext, ConnectionExec::DeleteCustomDNSRulesCallback>(env,
        status, data);
}

void ConnectionAsyncWork::ExecFactoryResetNetwork(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<FactoryResetNetworkContext, ConnectionExec::ExecFactoryResetNetwork>(env, data);
}

void ConnectionAsyncWork::FactoryResetNetworkCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<FactoryResetNetworkContext, ConnectionExec::FactoryResetNetworkCallback>(env,
        status, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::ExecGetAddressesByName(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAddressByNameContext, ConnectionExec::NetHandleExec::ExecGetAddressesByName>(env,
                                                                                                                 data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::GetAddressesByNameCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAddressByNameContext,
                                     ConnectionExec::NetHandleExec::GetAddressesByNameCallback>(env, status, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::ExecGetAddressByName(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAddressByNameContext, ConnectionExec::NetHandleExec::ExecGetAddressByName>(env,
                                                                                                               data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::GetAddressByNameCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAddressByNameContext, ConnectionExec::NetHandleExec::GetAddressByNameCallback>(
        env, status, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::ExecGetAddressesByNameWithOptions(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAddressByNameWithOptionsContext,
                                    ConnectionExec::NetHandleExec::ExecGetAddressesByNameWithOptions>(
        env, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::GetAddressesByNameWithOptionsCallback(napi_env env,
                                                                                    napi_status status,
                                                                                    void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAddressByNameWithOptionsContext,
                                        ConnectionExec::NetHandleExec::GetAddressesByNameWithOptionsCallback>(
        env, status, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::ExecBindSocket(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<BindSocketContext, ConnectionExec::NetHandleExec::ExecBindSocket>(env, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::BindSocketCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<BindSocketContext, ConnectionExec::NetHandleExec::BindSocketCallback>(env, status,
                                                                                                           data);
}

void ConnectionAsyncWork::NetConnectionAsyncWork::ExecRegister(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<RegisterContext, ConnectionExec::NetConnectionExec::ExecRegister>(env, data);
}

void ConnectionAsyncWork::NetConnectionAsyncWork::RegisterCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<RegisterContext, ConnectionExec::NetConnectionExec::RegisterCallback>(env, status,
                                                                                                           data);
}

void ConnectionAsyncWork::NetConnectionAsyncWork::ExecUnregister(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UnregisterContext, ConnectionExec::NetConnectionExec::ExecUnregister>(env, data);
}

void ConnectionAsyncWork::NetConnectionAsyncWork::UnregisterCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<UnregisterContext, ConnectionExec::NetConnectionExec::UnregisterCallback>(
        env, status, data);
}

void ConnectionAsyncWork::NetInterfaceAsyncWork::ExecIfaceRegister(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<IfaceRegisterContext, ConnectionExec::NetInterfaceExec::ExecIfaceRegister>(env, data);
}

void ConnectionAsyncWork::NetInterfaceAsyncWork::IfaceRegisterCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<IfaceRegisterContext,
        ConnectionExec::NetInterfaceExec::IfaceRegisterCallback>(env, status, data);
}

void ConnectionAsyncWork::NetInterfaceAsyncWork::ExecIfaceUnregister(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<IfaceUnregisterContext,
        ConnectionExec::NetInterfaceExec::ExecIfaceUnregister>(env, data);
}

void ConnectionAsyncWork::NetInterfaceAsyncWork::IfaceUnregisterCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<IfaceUnregisterContext,
        ConnectionExec::NetInterfaceExec::IfaceUnregisterCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecGetNetExtAttribute(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetNetExtAttributeContext,
        ConnectionExec::ExecGetNetExtAttribute>(env, data);
}

void ConnectionAsyncWork::GetNetExtAttributeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetNetExtAttributeContext, ConnectionExec::GetNetExtAttributeCallback>(env,
        status, data);
}

void ConnectionAsyncWork::ExecSetNetExtAttribute(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetNetExtAttributeContext,
        ConnectionExec::ExecSetNetExtAttribute>(env, data);
}

void ConnectionAsyncWork::SetNetExtAttributeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetNetExtAttributeContext, ConnectionExec::SetNetExtAttributeCallback>(env,
        status, data);
}

void ConnectionAsyncWork::ExecQueryTraceRoute(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<QueryTraceRouteContext, ConnectionExec::ExecQueryTraceRoute>(env, data);
}

void ConnectionAsyncWork::QueryTraceRouteCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<QueryTraceRouteContext, ConnectionExec::QueryTraceRouteCallback>(env,
        status, data);
}

void ConnectionAsyncWork::ExecQueryProbeResult(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<QueryProbeResultContext, ConnectionExec::ExecQueryProbeResult>(env, data);
}

void ConnectionAsyncWork::QueryProbeResultCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<QueryProbeResultContext, ConnectionExec::QueryProbeResultCallback>(env,
        status, data);
}
} // namespace OHOS::NetManagerStandard

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

#ifndef COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H
#define COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H

#include <napi/native_api.h>
#include <napi/native_node_api.h>

namespace OHOS::NetManagerStandard {
class ConnectionAsyncWork final {
public:
    ConnectionAsyncWork() = delete;

    ~ConnectionAsyncWork() = delete;

    static void ExecGetDefaultNet(napi_env env, void *data);

    static void GetDefaultNetCallback(napi_env env, napi_status status, void *data);

    static void ExecHasDefaultNet(napi_env env, void *data);

    static void HasDefaultNetCallback(napi_env env, napi_status status, void *data);

    static void ExecIsDefaultNetMetered(napi_env env, void *data);

    static void IsDefaultNetMeteredCallback(napi_env env, napi_status status, void *data);

    static void ExecGetNetCapabilities(napi_env env, void *data);

    static void ExecGetProxyMode(napi_env env, void *data);

    static void ExecSetProxyMode(napi_env env, void *data);

    static void GetNetCapabilitiesCallback(napi_env env, napi_status status, void *data);

    static void GetProxyModeCallback(napi_env env, napi_status status, void *data);

    static void SetProxyModeCallback(napi_env env, napi_status status, void *data);

    static void ExecGetConnectionProperties(napi_env env, void *data);

    static void GetConnectionPropertiesCallback(napi_env env, napi_status status, void *data);

    static void ExecGetAddressesByName(napi_env env, void *data);

    static void GetAddressesByNameCallback(napi_env env, napi_status status, void *data);

    static void ExecGetAddressesByNameWithOptions(napi_env env, void *data);

    static void GetAddressesByNameWithOptionsCallback(napi_env env, napi_status status, void *data);

    static void ExecGetAllNets(napi_env env, void *data);

    static void GetAllNetsCallback(napi_env env, napi_status status, void *data);

    static void ExecEnableAirplaneMode(napi_env env, void *data);

    static void EnableAirplaneModeCallback(napi_env env, napi_status status, void *data);

    static void ExecDisableAirplaneMode(napi_env env, void *data);

    static void DisableAirplaneModeCallback(napi_env env, napi_status status, void *data);

    static void ExecReportNetConnected(napi_env env, void *data);

    static void ReportNetConnectedCallback(napi_env env, napi_status status, void *data);

    static void ExecReportNetDisconnected(napi_env env, void *data);

    static void ReportNetDisconnectedCallback(napi_env env, napi_status status, void *data);

    static void ExecGetDefaultHttpProxy(napi_env env, void *data);

    static void GetDefaultHttpProxyCallback(napi_env env, napi_status status, void *data);

    static void ExecGetGlobalHttpProxy(napi_env env, void *data);

    static void GetGlobalHttpProxyCallback(napi_env env, napi_status status, void *data);

    static void ExecSetGlobalHttpProxy(napi_env env, void *data);

    static void SetGlobalHttpProxyCallback(napi_env env, napi_status status, void *data);

    static void ExecRefreshGlobalHttpProxy(napi_env env, void *data);

    static void RefreshGlobalHttpProxyCallback(napi_env env, napi_status status, void *data);

    static void ExecGetAppNet(napi_env env, void *data);

    static void GetAppNetCallback(napi_env env, napi_status status, void *data);

    static void ExecSetAppNet(napi_env env, void *data);

    static void SetAppNetCallback(napi_env env, napi_status status, void *data);

    static void ExecSetInterfaceUp(napi_env env, void *data);

    static void SetInterfaceUpCallback(napi_env env, napi_status status, void *data);

    static void ExecSetInterfaceIpAddr(napi_env env, void *data);

    static void SetInterfaceIpAddrCallback(napi_env env, napi_status status, void *data);

    static void ExecAddNetworkRoute(napi_env env, void *data);

    static void AddNetworkRouteCallback(napi_env env, napi_status status, void *data);

    static void ExecGetNetInterfaceConfiguration(napi_env env, void *data);

    static void GetNetInterfaceConfigurationCallback(napi_env env, napi_status status, void *data);

    static void ExecRegisterNetSupplier(napi_env env, void *data);

    static void RegisterNetSupplierCallback(napi_env env, napi_status status, void *data);

    static void ExecUnregisterNetSupplier(napi_env env, void *data);

    static void UnregisterNetSupplierCallback(napi_env env, napi_status status, void *data);

    static void ExecSetCustomDNSRule(napi_env env, void *data);

    static void SetCustomDNSRuleCallback(napi_env env, napi_status status, void *data);

    static void ExecDeleteCustomDNSRule(napi_env env, void *data);

    static void DeleteCustomDNSRuleCallback(napi_env env, napi_status status, void *data);

    static void ExecDeleteCustomDNSRules(napi_env env, void *data);

    static void DeleteCustomDNSRulesCallback(napi_env env, napi_status status, void *data);

    static void ExecFactoryResetNetwork(napi_env env, void *data);

    static void FactoryResetNetworkCallback(napi_env env, napi_status status, void *data);

    static void ExecGetNetExtAttribute(napi_env env, void *data);

    static void GetNetExtAttributeCallback(napi_env env, napi_status status, void *data);

    static void ExecSetNetExtAttribute(napi_env env, void *data);

    static void SetNetExtAttributeCallback(napi_env env, napi_status status, void *data);

    static void ExecGetIpNeighTable(napi_env env, void *data);

    static void GetIpNeighTableCallback(napi_env env, napi_status status, void *data);

    static void ExecCreateVlan(napi_env env, void *data);

    static void CreateVlanCallback(napi_env env, napi_status status, void *data);

    static void ExecDestroyVlan(napi_env env, void *data);

    static void DestroyVlanCallback(napi_env env, napi_status status, void *data);

    static void ExecAddVlanIp(napi_env env, void *data);

    static void AddVlanIpCallback(napi_env env, napi_status status, void *data);

    static void ExecDeleteVlanIp(napi_env env, void *data);

    static void DeleteVlanIpCallback(napi_env env, napi_status status, void *data);

    static void ExecGetConnectOwnerUid(napi_env env, void *data);

    static void GetConnectOwnerUidCallback(napi_env env, napi_status status, void *data);

    static void ExecGetSystemNetPortStates(napi_env env, void *data);

    static void GetSystemNetPortStatesCallback(napi_env env, napi_status status, void *data);

    static void ExecQueryTraceRoute(napi_env env, void *data);

    static void QueryTraceRouteCallback(napi_env env, napi_status status, void *data);

    static void ExecQueryProbeResult(napi_env env, void *data);

    static void QueryProbeResultCallback(napi_env env, napi_status status, void *data);

    class NetHandleAsyncWork final {
    public:
        NetHandleAsyncWork() = delete;

        ~NetHandleAsyncWork() = delete;

        static void ExecGetAddressByName(napi_env env, void *data);

        static void GetAddressByNameCallback(napi_env env, napi_status status, void *data);

        static void ExecGetAddressesByName(napi_env env, void *data);

        static void GetAddressesByNameCallback(napi_env env, napi_status status, void *data);
        
        static void ExecGetAddressesByNameWithOptions(napi_env env, void *data);

        static void GetAddressesByNameWithOptionsCallback(napi_env env, napi_status status, void *data);

        static void ExecBindSocket(napi_env env, void *data);

        static void BindSocketCallback(napi_env env, napi_status status, void *data);
    };

    class NetConnectionAsyncWork final {
    public:
        NetConnectionAsyncWork() = delete;

        ~NetConnectionAsyncWork() = delete;

        static void ExecRegister(napi_env env, void *data);

        static void RegisterCallback(napi_env env, napi_status status, void *data);

        static void ExecUnregister(napi_env env, void *data);

        static void UnregisterCallback(napi_env env, napi_status status, void *data);
    };

    class NetInterfaceAsyncWork final {
    public:
        NetInterfaceAsyncWork() = delete;

        ~NetInterfaceAsyncWork() = delete;

        static void ExecIfaceRegister(napi_env env, void *data);

        static void IfaceRegisterCallback(napi_env env, napi_status status, void *data);

        static void ExecIfaceUnregister(napi_env env, void *data);

        static void IfaceUnregisterCallback(napi_env env, napi_status status, void *data);
    };
};
} // namespace OHOS::NetManagerStandard

#endif /* COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H */

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

#ifndef COMMUNICATIONNETMANAGERBASE_CONNECTION_MODULE_H
#define COMMUNICATIONNETMANAGERBASE_CONNECTION_MODULE_H

#include <napi/native_api.h>
#include <initializer_list>

namespace OHOS::NetManagerStandard {
enum NetConnectionType {
    PARAMETER_ZERO = 0,
    PARAMETER_SPECIFIER,
    PARAMETER_TIMEOUT,
    PARAMETER_ERROR,
};

enum Socks5DnsStrategyType {
    SYSTEM_MODE = 0,
    PROXY_MODE = 1,
};

class ConnectionModule final {
public:
    static constexpr const char *FUNCTION_GET_DEFAULT_NET = "getDefaultNet";
    static constexpr const char *FUNCTION_GET_DEFAULT_NET_SYNC = "getDefaultNetSync";
    static constexpr const char *FUNCTION_HAS_DEFAULT_NET = "hasDefaultNet";
    static constexpr const char *FUNCTION_HAS_DEFAULT_NET_SYNC = "hasDefaultNetSync";
    static constexpr const char *FUNCTION_IS_DEFAULT_NET_METERED = "isDefaultNetMetered";
    static constexpr const char *FUNCTION_IS_DEFAULT_NET_METERED_SYNC = "isDefaultNetMeteredSync";
    static constexpr const char *FUNCTION_GET_NET_CAPABILITIES = "getNetCapabilities";
    static constexpr const char *FUNCTION_GET_NET_CAPABILITIES_SYNC = "getNetCapabilitiesSync";
    static constexpr const char *FUNCTION_GET_CONNECTION_PROPERTIES = "getConnectionProperties";
    static constexpr const char *FUNCTION_GET_CONNECTION_PROPERTIES_SYNC = "getConnectionPropertiesSync";
    static constexpr const char *FUNCTION_CREATE_NET_CONNECTION = "createNetConnection";
    static constexpr const char *FUNCTION_GET_ADDRESSES_BY_NAME = "getAddressesByName";
    static constexpr const char *FUNCTION_GET_ALL_NETS = "getAllNets";
    static constexpr const char *FUNCTION_GET_ALL_NETS_SYNC = "getAllNetsSync";
    static constexpr const char *FUNCTION_ENABLE_AIRPLANE_MODE = "enableAirplaneMode";
    static constexpr const char *FUNCTION_DISABLE_AIRPLANE_MODE = "disableAirplaneMode";
    static constexpr const char *FUNCTION_REPORT_NET_CONNECTED = "reportNetConnected";
    static constexpr const char *FUNCTION_REPORT_NET_DISCONNECTED = "reportNetDisconnected";
    static constexpr const char *FUNCTION_GET_DEFAULT_HTTP_PROXY = "getDefaultHttpProxy";
    static constexpr const char *FUNCTION_GET_GLOBAL_HTTP_PROXY = "getGlobalHttpProxy";
    static constexpr const char *FUNCTION_SET_GLOBAL_HTTP_PROXY = "setGlobalHttpProxy";
    static constexpr const char *FUNCTION_REFRESH_GLOBAL_HTTP_PROXY = "refreshGlobalHttpProxy";
    static constexpr const char *FUNCTION_SET_CUSTOM_DNS_RULE = "addCustomDnsRule";
    static constexpr const char *FUNCTION_DELETE_CUSTOM_DNS_RULE = "removeCustomDnsRule";
    static constexpr const char *FUNCTION_DELETE_CUSTOM_DNS_RULES = "clearCustomDnsRules";
    static constexpr const char *FUNCTION_SET_APP_HTTP_PROXY = "setAppHttpProxy";
    static constexpr const char *FUNCTION_GET_APP_NET = "getAppNet";
    static constexpr const char *FUNCTION_GET_APP_NET_SYNC = "getAppNetSync";
    static constexpr const char *FUNCTION_SET_APP_NET = "setAppNet";
    static constexpr const char *INTERFACE_NET_CONNECTION = "NetConnection";
    static constexpr const char *INTERFACE_NET_CAP = "NetCap";
    static constexpr const char *INTERFACE_NET_BEAR_TYPE = "NetBearType";
    static constexpr const char *INTERFACE_TCP_STATE = "TcpState";
    static constexpr const char *INTERFACE_PROXY_MODE_TYPE = "ProxyMode";
    static constexpr const char *INTERFACE_CONVERSION_PROCESS = "ConversionProcess";
    static constexpr const char *FUNCTION_FACTORY_RESET_NETWORK = "factoryReset";
    static constexpr const char *FUNCTION_FACTORY_RESET_NETWORK_SYNC = "factoryResetNetworkSync";
    static constexpr const char *FUNCTION_SET_PAC_URL = "setPacUrl";
    static constexpr const char *FUNCTION_GET_PAC_URL = "getPacUrl";
    static constexpr const char *FUNCTION_SET_FILE_PAC_URL = "setPacFileUrl";
    static constexpr const char *FUNCTION_GET_FILE_PAC_URL = "getPacFileUrl";
    static constexpr const char *FUNCTION_GET_PROXY_MODE = "getProxyMode";
    static constexpr const char *FUNCTION_SET_PROXY_MODE = "setProxyMode";
    static constexpr const char *FUNCTION_FIND_PROXY_FOR_URL = "findProxyForUrl";
    static constexpr const char *FUNCTION_SET_INTERFACE_UP = "setInterfaceUp";
    static constexpr const char *FUNCTION_SET_INTERFACE_IP_ADDRESS = "setNetInterfaceIpAddress";
    static constexpr const char *FUNCTION_ADD_NETWORK_ROUTE = "addNetworkRoute";
    static constexpr const char *FUNCTION_CREATE_NET_INTERFACE = "createNetInterface";
    static constexpr const char *INTERFACE_NET_INTERFACE = "NetInterface";
    static constexpr const char *FUNCTION_GET_INTERFACE_CONFIG = "getNetInterfaceConfiguration";
    static constexpr const char *FUNCTION_REGISTER_NET_SUPPLIER = "registerNetSupplier";
    static constexpr const char *FUNCTION_UNREGISTER_NET_SUPPLIER = "unregisterNetSupplier";
    static constexpr const char *FUNCTION_SET_NET_EXT_ATTRIBUTE = "setNetExtAttribute";
    static constexpr const char *FUNCTION_GET_NET_EXT_ATTRIBUTE = "getNetExtAttribute";
    static constexpr const char *FUNCTION_SET_NET_EXT_ATTRIBUTE_SYNC = "setNetExtAttributeSync";
    static constexpr const char *FUNCTION_GET_NET_EXT_ATTRIBUTE_SYNC = "getNetExtAttributeSync";
    static constexpr const char *FUNCTION_GET_IP_NEIGH_TABLE = "getIpNeighTable";
    static constexpr const char *INTERFACE_FAMILY_TYPE = "FamilyType";
    static constexpr const char *FUNCTION_GET_ADDRESSES_BY_NAME_WITH_OPTION = "getAddressesByNameWithOptions";
    static constexpr const char *FUNCTION_GET_CONNECT_OWNER_UID = "getConnectOwnerUid";
    static constexpr const char *FUNCTION_GET_CONNECT_OWNER_UID_SYNC = "getConnectOwnerUidSync";
    static constexpr const char *INTERFACE_PROTOCOL_TYPE = "ProtocolType";
    static constexpr const char *FUNCTION_GET_DNS_ASCII = "getDnsAscii";
    static constexpr const char *FUNCTION_GET_DNS_UNICODE = "getDnsUnicode";
    static constexpr const char *FUNCTION_CREATE_VLAN = "createVlanInterface";
    static constexpr const char *FUNCTION_DESTROY_VLAN = "destroyVlanInterface";
    static constexpr const char *FUNCTION_ADD_VLAN_IP = "addVlanIp";
    static constexpr const char *FUNCTION_DELETE_VLAN_IP = "deleteVlanIp";
    static constexpr const char *FUNCTION_GET_SYSTEM_NET_PORT_STATES = "getSystemNetPortStates";
    static constexpr const char *FUNCTION_QUERY_PROBE_RESULT = "queryProbeResult";
    static constexpr const char *FUNCTION_QUERY_TRACE_ROUTE = "queryTraceRoute";
    static constexpr const char *INTERFACE_PACKETS_TYPE = "PacketsType";
    static constexpr const char *INTERFACE_SOCKS5_DNS_STRATEGY = "Socks5DnsStrategy";

    static napi_value InitConnectionModule(napi_env env, napi_value exports);
    static std::initializer_list<napi_property_descriptor> createPropertyList();
    static std::initializer_list<napi_property_descriptor> createWritablePropertyList();

    class NetConnectionInterface final {
    public:
        static constexpr const char *FUNCTION_ON = "on";
        static constexpr const char *FUNCTION_REGISTER = "register";
        static constexpr const char *FUNCTION_UNREGISTER = "unregister";

        static napi_value On(napi_env env, napi_callback_info info);
        static napi_value Register(napi_env env, napi_callback_info info);
        static napi_value Unregister(napi_env env, napi_callback_info info);
    };

    class NetInterfaceInterface final {
    public:
        static constexpr const char *FUNCTION_ON = "on";
        static constexpr const char *FUNCTION_REGISTER = "register";
        static constexpr const char *FUNCTION_UNREGISTER = "unregister";

        static napi_value On(napi_env env, napi_callback_info info);
        static napi_value Register(napi_env env, napi_callback_info info);
        static napi_value Unregister(napi_env env, napi_callback_info info);
    };

private:
    static void InitClasses(napi_env env, napi_value exports);
    static void InitProperties(napi_env env, napi_value exports);
    static void InitTcpStates(napi_env env, napi_value exports);
    static void InitProtocolTypeProperties(napi_env env, napi_value exports);
    static void InitFamilyTypes(napi_env env, napi_value exports);
    static void InitSocks5DnsStrategyProperties(napi_env env, napi_value exports);

    static napi_value GetDefaultNet(napi_env env, napi_callback_info info);
    static napi_value GetDefaultNetSync(napi_env env, napi_callback_info info);
    static napi_value CreateNetConnection(napi_env env, napi_callback_info info);
    static napi_value GetAddressesByName(napi_env env, napi_callback_info info);
    static napi_value HasDefaultNet(napi_env env, napi_callback_info info);
    static napi_value HasDefaultNetSync(napi_env env, napi_callback_info info);
    static napi_value IsDefaultNetMetered(napi_env env, napi_callback_info info);
    static napi_value IsDefaultNetMeteredSync(napi_env env, napi_callback_info info);
    static napi_value GetNetCapabilities(napi_env env, napi_callback_info info);
    static napi_value GetProxyMode(napi_env env, napi_callback_info info);
    static napi_value SetProxyMode(napi_env env, napi_callback_info info);
    static napi_value GetNetCapabilitiesSync(napi_env env, napi_callback_info info);
    static napi_value GetConnectionProperties(napi_env env, napi_callback_info info);
    static napi_value GetConnectionPropertiesSync(napi_env env, napi_callback_info info);
    static napi_value GetAllNets(napi_env env, napi_callback_info info);
    static napi_value GetAllNetsSync(napi_env env, napi_callback_info info);
    static napi_value EnableAirplaneMode(napi_env env, napi_callback_info info);
    static napi_value DisableAirplaneMode(napi_env env, napi_callback_info info);
    static napi_value ReportNetConnected(napi_env env, napi_callback_info info);
    static napi_value ReportNetDisconnected(napi_env env, napi_callback_info info);
    static napi_value GetDefaultHttpProxy(napi_env env, napi_callback_info info);
    static napi_value GetGlobalHttpProxy(napi_env env, napi_callback_info info);
    static napi_value SetGlobalHttpProxy(napi_env env, napi_callback_info info);
    static napi_value RefreshGlobalHttpProxy(napi_env env, napi_callback_info info);
    static napi_value AddCustomDnsRule(napi_env env, napi_callback_info info);
    static napi_value RemoveCustomDnsRule(napi_env env, napi_callback_info info);
    static napi_value ClearCustomDnsRules(napi_env env, napi_callback_info info);
    static napi_value SetAppHttpProxy(napi_env env, napi_callback_info info);
    static napi_value GetAppNet(napi_env env, napi_callback_info info);
    static napi_value GetAppNetSync(napi_env env, napi_callback_info info);
    static napi_value SetAppNet(napi_env env, napi_callback_info info);
    static napi_value FactoryResetNetwork(napi_env env, napi_callback_info info);
    static napi_value FactoryResetNetworkSync(napi_env env, napi_callback_info info);
    static napi_value SetPacUrl(napi_env env, napi_callback_info info);
    static napi_value SetPacFileUrl(napi_env env, napi_callback_info info);
    static napi_value GetPacUrl(napi_env env, napi_callback_info info);
    static napi_value GetPacFileUrl(napi_env env, napi_callback_info info);
    static napi_value FindProxyForUrl(napi_env env, napi_callback_info info);
    static napi_value SetInterfaceUp(napi_env env, napi_callback_info info);
    static napi_value SetNetInterfaceIpAddress(napi_env env, napi_callback_info info);
    static napi_value AddNetworkRoute(napi_env env, napi_callback_info info);
    static napi_value CreateNetInterface(napi_env env, napi_callback_info info);
    static napi_value GetNetInterfaceConfiguration(napi_env env, napi_callback_info info);
    static napi_value RegisterNetSupplier(napi_env env, napi_callback_info info);
    static napi_value UnregisterNetSupplier(napi_env env, napi_callback_info info);
    static napi_value GetNetExtAttribute(napi_env env, napi_callback_info info);
    static napi_value SetNetExtAttribute(napi_env env, napi_callback_info info);
    static napi_value GetNetExtAttributeSync(napi_env env, napi_callback_info info);
    static napi_value SetNetExtAttributeSync(napi_env env, napi_callback_info info);
    static napi_value GetIpNeighTable(napi_env env, napi_callback_info info);
    static napi_value GetAddressesByNameWithOptions(napi_env env, napi_callback_info info);
    static napi_value GetConnectOwnerUid(napi_env env, napi_callback_info info);
    static napi_value GetConnectOwnerUidSync(napi_env env, napi_callback_info info);
    static napi_value GetDnsASCII(napi_env env, napi_callback_info info);
    static napi_value GetDnsUnicode(napi_env env, napi_callback_info info);
    static napi_value CreateVlan(napi_env env, napi_callback_info info);
    static napi_value DestroyVlan(napi_env env, napi_callback_info info);
    static napi_value AddVlanIp(napi_env env, napi_callback_info info);
    static napi_value DeleteVlanIp(napi_env env, napi_callback_info info);
    static napi_value GetSystemNetPortStates(napi_env env, napi_callback_info info);
    static napi_value QueryTraceRoute(napi_env env, napi_callback_info info);
    static napi_value QueryProbeResult(napi_env env, napi_callback_info info);
};
} // namespace OHOS::NetManagerStandard

#endif /* COMMUNICATIONNETMANAGERBASE_CONNECTION_MODULE_H */

// Copyright (C) 2026 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#![allow(missing_docs, clippy::not_unsafe_ptr_arg_deref)]

mod bridge;
mod connection;
mod error_code;
mod log;
pub mod wrapper;

use ani_rs::ani_constructor;

ani_constructor!(
    namespace "@ohos.net.connection.connection"
    [
        "createNetConnection": connection::create_net_connection,
        "getDefaultNetSync" : connection::get_default_net,
        "getAllNetsSync" : connection::get_all_nets,
        "hasDefaultNetSync" : connection::has_default_net,
        "getNetCapabilitiesSync": connection::get_net_capabilities,
        "getDefaultHttpProxySync": connection::get_default_http_proxy,
        "getGlobalHttpProxySync": connection::get_global_http_proxy,
        "setGlobalHttpProxySync": connection::set_global_http_proxy,
        "setAppHttpProxy": connection::set_app_http_proxy,
        "enableAirplaneModeSync": connection::enable_airplane_mode,
        "disableAirplaneModeSync": connection::disable_airplane_mode,
        "getAppNetSync": connection::get_app_net,
        "setAppNetSync": connection::set_app_net,
        "getPacUrl" : connection::get_pac_url,
        "setPacUrl" : connection::set_pac_url,
        "factoryResetSync" : connection::factory_reset_network,
        "isDefaultNetMeteredSync" : connection::is_default_net_metered,
        "getConnectionPropertiesSync" : connection::get_connection_properties,
        "getAddressesByNameSync" : connection::get_addresses_by_name,
        "reportNetConnectedSync" : connection::net_detection,
        "reportNetDisconnectedSync" : connection::net_detection,
        "clearCustomDnsRulesSync" : connection::clear_custom_dns_rules,
        "addCustomDnsRuleSync" : connection::set_custom_dns_rule,
        "removeCustomDnsRuleSync" : connection::remove_custom_dns_rule,
        "refreshGlobalHttpProxySync" : connection::refresh_global_http_proxy,
        "setPacFileUrlSync" : connection::set_pac_file_url,
        "findProxyForUrlSync" : connection::find_proxy_for_url,
        "getAddressesByNameWithOptionsSync" : connection::get_addresses_by_name_with_options,
        "createVlanInterfaceSync" : connection::create_vlan_interface,
        "destroyVlanInterfaceSync" : connection::destroy_vlan_interface,
        "addVlanIpSync" : connection::add_vlan_ip,
        "deleteVlanIpSync" : connection::delete_vlan_ip,
        "getSystemNetPortStatesSync" : connection::get_system_net_port_states,
        "getIpNeighTableSync" : connection::get_ip_neigh_table,
        "getConnectOwnerUidSyncImpl" : connection::get_connect_owner_uid,
        "getDnsUnicodeSync" : connection::get_dns_unicode,
        "getDnsAsciiSync" : connection::get_dns_ascii,
        "setInterfaceUpSync" : connection::set_interface_up,
        "queryProbeResultSync" : connection::query_probe_result,
        "queryTraceRouteSync" : connection::query_trace_route,
        "getProxyModeSync" : connection::get_proxy_mode,
        "setProxyModeSync" : connection::set_proxy_mode,
        "getPacFileUrlSync" : connection::get_pac_file_url,
        "setNetExtAttributeSync" : connection::set_net_ext_attribute,
        "getNetExtAttributeSync" : connection::get_net_ext_attribute,
    ]
    class "@ohos.net.connection.connection.NetHandleInner"
    [
        "getAddressByNameSyncWithHandle" : connection::get_address_by_name_with_handle,
        "getAddressesByNameSyncWithHandle": connection::get_addresses_by_name_with_handle,
        "getAddressesByNameWithOptionsSyncWithHandle": connection::get_addresses_by_name_with_options_with_handle,
    ]
    class "@ohos.net.connection.connection.NetConnectionInner"
    [
        "onNetAvailableInner": connection::on_net_available,
        "onNetBlockStatusChangeInner": connection::on_net_block_status_change,
        "onNetCapabilitiesChangeInner": connection::on_net_capabilities_change,
        "onNetConnectionPropertiesChangeInner": connection::on_net_connection_properties_change,
        "onNetLostInner": connection::on_net_lost,
        "onNetUnavailableInner": connection::on_net_unavailable,
        "registerSync" : connection::register_network_change,
        "unregisterSync" : connection::unregister_network_change,
    ]
    class "@ohos.net.connection.connection.Cleaner"
    [
        "clean" : connection::connection_clean,
    ]
);

const LOG_LABEL: hilog_rust::HiLogLabel = hilog_rust::HiLogLabel {
    log_type: hilog_rust::LogType::LogCore,
    domain: 0xD0015B0,
    tag: "NetMgrSubSystem",
};

#[used]
#[link_section = ".init_array"]
static G_CONNECTION_PANIC_HOOK: extern "C" fn() = {
    #[link_section = ".text.startup"]
    extern "C" fn init() {
        std::panic::set_hook(Box::new(|info| {
            connection_error!("Panic occurred: {:?}", info);
        }));
    }
    init
};

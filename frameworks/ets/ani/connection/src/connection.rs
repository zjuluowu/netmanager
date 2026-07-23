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

use std::{ffi::CStr, mem, net::IpAddr};

use ani_rs::{
    business_error::BusinessError,
    objects::{AniFnObject, AniRef, GlobalRefCallback},
    AniEnv,
};

use crate::{
    bridge::{
        Cleaner, ConnectionProperties, HttpProxy, NetAddress, NetBlockStatusInfo, NetCapabilities,
        NetCapabilityInfo, NetConnection, NetConnectionPropertyInfo, NetHandle, NetSpecifier,
    },
    connection_error,
    error_code::convert_to_business_error,
    wrapper::{check_permission, ConnUnregisterHandle, NetConnClient},
};

const INTERNET_PERMISSION: &str = "ohos.permission.INTERNET";

#[ani_rs::native]
pub(crate) fn get_default_net() -> Result<NetHandle, BusinessError> {
    NetConnClient::get_default_net_handle().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_all_nets() -> Result<Vec<NetHandle>, BusinessError> {
    NetConnClient::get_all_nets().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn has_default_net() -> Result<bool, BusinessError> {
    NetConnClient::has_default_net().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_net_capabilities(
    net_handle: NetHandle,
) -> Result<NetCapabilities, BusinessError> {
    NetConnClient::get_net_capabilities(net_handle).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_default_http_proxy() -> Result<HttpProxy, BusinessError> {
    NetConnClient::get_default_http_proxy().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_global_http_proxy() -> Result<HttpProxy, BusinessError> {
    NetConnClient::get_global_http_proxy().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn enable_airplane_mode() -> Result<(), BusinessError> {
    NetConnClient::set_airplane_mode(true).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn disable_airplane_mode() -> Result<(), BusinessError> {
    NetConnClient::set_airplane_mode(false).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_app_net() -> Result<NetHandle, BusinessError> {
    NetConnClient::get_app_net()
        .map(|net_id| NetHandle { net_id })
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn set_app_net(net_handle: NetHandle) -> Result<(), BusinessError> {
    NetConnClient::set_app_net(net_handle.net_id).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_pac_url() -> Result<String, BusinessError> {
    NetConnClient::get_pac_url().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn set_pac_url(pac_url: String) -> Result<(), BusinessError> {
    validate_pac_file_url(&pac_url)?;
    NetConnClient::set_pac_url(&pac_url).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn factory_reset_network() -> Result<(), BusinessError> {
    NetConnClient::factory_reset_network().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn is_default_net_metered() -> Result<bool, BusinessError> {
    NetConnClient::is_default_net_metered().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_connection_properties(
    net_handle: NetHandle,
) -> Result<ConnectionProperties, BusinessError> {
    NetConnClient::get_connection_properties(net_handle.net_id).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_addresses_by_name(host: String) -> Result<Vec<NetAddress>, BusinessError> {
    NetConnClient::get_addresses_by_name(&host, 0).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_address_by_name_with_handle(
    this: NetHandle,
    host: String,
) -> Result<NetAddress, BusinessError> {
    NetConnClient::get_address_by_name(&host, this.net_id).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_addresses_by_name_with_handle(
    this: NetHandle,
    host: String,
) -> Result<Vec<NetAddress>, BusinessError> {
    NetConnClient::get_addresses_by_name(&host, this.net_id).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn set_global_http_proxy(proxy: HttpProxy) -> Result<(), BusinessError> {
    NetConnClient::set_global_http_proxy(proxy).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn set_app_http_proxy(proxy: HttpProxy) -> Result<(), BusinessError> {
    NetConnClient::set_app_http_proxy(proxy).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn bind_socket(this: NetHandle, socket: i32) -> Result<(), BusinessError> {
    NetConnClient::bind_socket(socket, this.net_id).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn net_detection(net_handle: NetHandle) -> Result<(), BusinessError> {
    NetConnClient::net_detection(net_handle.net_id).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn clear_custom_dns_rules() -> Result<(), BusinessError> {
    if !check_permission(INTERNET_PERMISSION) {
        return Err(BusinessError::PERMISSION);
    }
    NetConnClient::clear_custom_dns_rules().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn set_custom_dns_rule(host: String, ips: Vec<String>) -> Result<(), BusinessError> {
    if !check_permission(INTERNET_PERMISSION) {
        return Err(BusinessError::PERMISSION);
    }
    NetConnClient::set_custom_dns_rules(host, ips).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn remove_custom_dns_rule(host: String) -> Result<(), BusinessError> {
    if !check_permission(INTERNET_PERMISSION) {
        return Err(BusinessError::PERMISSION);
    }
    NetConnClient::remove_custom_dns_rule(host).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn refresh_global_http_proxy() -> Result<HttpProxy, BusinessError> {
    NetConnClient::refresh_global_http_proxy().map_err(convert_to_business_error)
}

fn validate_pac_file_url(pac_url: &str) -> Result<(), BusinessError> {
    if pac_url.is_empty() {
        return Err(BusinessError::new(401, "pac_url must not be empty".to_string()));
    }
    if let Some(scheme_end) = pac_url.find("://") {
        let original_scheme = &pac_url[..scheme_end];
        let scheme = original_scheme.to_lowercase();
        let valid_schemes = ["http", "https", "ftp", "file", "data"];
        if scheme.is_empty() || !valid_schemes.contains(&scheme.as_str()) {
            return Err(BusinessError::new(401,
                format!("pac_url has unsupported scheme '{}'. Supported: http, https, ftp, file, data", original_scheme)));
        }
        if pac_url.len() <= scheme_end + 3 {
            return Err(BusinessError::new(401, "pac_url is missing host part after scheme://".to_string()));
        }
        Ok(())
    } else {
        Err(BusinessError::new(401, "pac_url must be a valid URL with scheme:// (e.g. http://example.com)".to_string()))
    }
}

#[ani_rs::native]
pub(crate) fn set_pac_file_url(pac_url: String) -> Result<(), BusinessError> {
    validate_pac_file_url(&pac_url)?;
    NetConnClient::set_pac_file_url(&pac_url).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn find_proxy_for_url(url: String) -> Result<String, BusinessError> {
    NetConnClient::find_proxy_for_url(&url).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_addresses_by_name_with_options(
    host: String,
    option: crate::bridge::QueryOptions,
) -> Result<Vec<NetAddress>, BusinessError> {
    let family = option.family.map(|f| f as i32)
        .unwrap_or(crate::bridge::FamilyType::FAMILY_TYPE_ALL as i32);
    NetConnClient::get_addresses_by_name_with_options(&host, 0, family).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_addresses_by_name_with_options_with_handle(
    this: NetHandle,
    host: String,
    option: crate::bridge::QueryOptions,
) -> Result<Vec<NetAddress>, BusinessError> {
    let family = option.family.map(|f| f as i32)
        .unwrap_or(crate::bridge::FamilyType::FAMILY_TYPE_ALL as i32);
    NetConnClient::get_addresses_by_name_with_options(&host, this.net_id, family).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn create_vlan_interface(if_name: String, vlan_id: u32) -> Result<(), BusinessError> {
    NetConnClient::create_vlan_interface(&if_name, vlan_id).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn destroy_vlan_interface(if_name: String, vlan_id: u32) -> Result<(), BusinessError> {
    NetConnClient::destroy_vlan_interface(&if_name, vlan_id).map_err(convert_to_business_error)
}

const MAX_IPV4_PREFIX_LENGTH: u32 = 32;
const MAX_IPV6_PREFIX_LENGTH: u32 = 128;

fn validate_vlan_mask(mask: u32, ip: &str) -> Result<(), BusinessError> {
    let addr = ip.parse::<IpAddr>()
        .map_err(|_| BusinessError::new(401, format!("Invalid IP address '{}'", ip)))?;
    let max_mask = if addr.is_ipv6() { MAX_IPV6_PREFIX_LENGTH } else { MAX_IPV4_PREFIX_LENGTH };
    if mask > max_mask {
        return Err(BusinessError::new(401,
            format!("Invalid mask value {} for this address type. IPv4 allows 0-32, IPv6 allows 0-128", mask)));
    }
    Ok(())
}

#[ani_rs::native]
pub(crate) fn add_vlan_ip(if_name: String, vlan_id: u32, ip: String, mask: u32) -> Result<(), BusinessError> {
    ip.parse::<IpAddr>()
        .map_err(|_| BusinessError::new(401, format!("Invalid IP address '{}'", ip)))?;
    validate_vlan_mask(mask, &ip)?;
    NetConnClient::add_vlan_ip(&if_name, vlan_id, &ip, mask).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn delete_vlan_ip(if_name: String, vlan_id: u32, ip: String, mask: u32) -> Result<(), BusinessError> {
    ip.parse::<IpAddr>()
        .map_err(|_| BusinessError::new(401, format!("Invalid IP address '{}'", ip)))?;
    validate_vlan_mask(mask, &ip)?;
    NetConnClient::delete_vlan_ip(&if_name, vlan_id, &ip, mask).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_system_net_port_states() -> Result<crate::bridge::NetPortStatesInfo, BusinessError> {
    NetConnClient::get_system_net_port_states().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_ip_neigh_table() -> Result<Vec<crate::bridge::NetIpMacInfo>, BusinessError> {
    NetConnClient::get_ip_neigh_table().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_connect_owner_uid(
    param: crate::bridge::NetConnInfoParam,
) -> Result<i32, BusinessError> {
    NetConnClient::get_connect_owner_uid(
        param.protocol_type as i32,
        param.family as i32,
        &param.local_address,
        param.local_port as u16,
        &param.remote_address,
        param.remote_port as u16,
    ).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_dns_unicode(
    host: String,
    conversion_process: crate::bridge::ConversionProcess,
) -> Result<String, BusinessError> {
    NetConnClient::get_dns_unicode(&host, conversion_process as i32).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_dns_ascii(
    host: String,
    conversion_process: crate::bridge::ConversionProcess,
) -> Result<String, BusinessError> {
    NetConnClient::get_dns_ascii(&host, conversion_process as i32).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn set_interface_up(iface: String) -> Result<(), BusinessError> {
    NetConnClient::set_interface_up(&iface).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn query_probe_result(
    dest: String,
    duration: i32,
) -> Result<crate::bridge::ProbeResultInfo, BusinessError> {
    NetConnClient::query_probe_result(&dest, duration).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn query_trace_route(
    destination: String,
    max_jump_number: i32,
    packets_type: crate::bridge::PacketsType,
) -> Result<Vec<crate::bridge::TraceRouteInfo>, BusinessError> {
    NetConnClient::query_trace_route(&destination, max_jump_number, packets_type as i32)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_proxy_mode() -> Result<crate::bridge::ProxyMode, BusinessError> {
    NetConnClient::get_proxy_mode().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn set_proxy_mode(mode: crate::bridge::ProxyMode) -> Result<(), BusinessError> {
    NetConnClient::set_proxy_mode(mode).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_pac_file_url() -> Result<String, BusinessError> {
    NetConnClient::get_pac_file_url().map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn set_net_ext_attribute(net_handle: crate::bridge::NetHandle, net_ext_attribute: String) -> Result<(), BusinessError> {
    NetConnClient::set_net_ext_attribute(net_handle.net_id, &net_ext_attribute).map_err(convert_to_business_error)
}

#[ani_rs::native]
pub(crate) fn get_net_ext_attribute(net_handle: crate::bridge::NetHandle) -> Result<String, BusinessError> {
    NetConnClient::get_net_ext_attribute(net_handle.net_id).map_err(convert_to_business_error)
}

pub struct Connection {
    pub net_specifier: Option<NetSpecifier>,
    pub timeout: Option<i32>,
    pub callback: ConnCallback,
    pub unregister: Option<ConnUnregisterHandle>,
}

pub struct ConnCallback {
    pub on_net_available: Option<GlobalRefCallback<(NetHandle,)>>,
    pub on_net_block_status_change: Option<GlobalRefCallback<(NetBlockStatusInfo,)>>,
    pub on_net_capabilities_change: Option<GlobalRefCallback<(NetCapabilityInfo,)>>,
    pub on_net_connection_properties_change:
        Option<GlobalRefCallback<(NetConnectionPropertyInfo,)>>,
    pub on_net_lost: Option<GlobalRefCallback<(NetHandle,)>>,
    pub on_net_unavailable: Option<GlobalRefCallback<()>>,
}

impl ConnCallback {
    pub fn new() -> Self {
        Self {
            on_net_available: None,
            on_net_block_status_change: None,
            on_net_capabilities_change: None,
            on_net_connection_properties_change: None,
            on_net_lost: None,
            on_net_unavailable: None,
        }
    }
}

impl Connection {
    pub fn new(net_specifier: Option<NetSpecifier>, timeout: Option<i32>) -> Self {
        Self {
            net_specifier,
            timeout,
            callback: ConnCallback::new(),
            unregister: None,
        }
    }
}

#[ani_rs::native]
pub(crate) fn create_net_connection<'local>(
    env: &AniEnv<'local>,
    net_specifier: Option<NetSpecifier>,
    timeout: Option<i32>,
) -> Result<AniRef<'local>, BusinessError> {
    static CONNECTION_CLASS: &CStr = unsafe {
        CStr::from_bytes_with_nul_unchecked(
            b"@ohos.net.connection.connection.NetConnectionInner\0",
        )
    };
    static CTOR_SIGNATURE: &CStr = unsafe { CStr::from_bytes_with_nul_unchecked(b"l:\0") };

    let connection = Box::new(Connection::new(net_specifier, timeout));

    let ptr = Box::into_raw(connection);
    let class = env.find_class(CONNECTION_CLASS).unwrap();
    let obj = env
        .new_object_with_signature(&class, CTOR_SIGNATURE, (ptr as i64,))
        .unwrap();
    Ok(obj.into())
}

#[ani_rs::native]
pub(crate) fn on_net_available(
    env: &AniEnv,
    this: NetConnection,
    callback: AniFnObject,
) -> Result<(), BusinessError> {
    let connection = unsafe { &mut *(this.native_ptr as *mut Connection) };
    connection.callback.on_net_available = Some(callback.into_global_callback(env).unwrap());
    Ok(())
}

#[ani_rs::native]
pub(crate) fn on_net_block_status_change(
    env: &AniEnv,
    this: NetConnection,
    callback: AniFnObject,
) -> Result<(), BusinessError> {
    let connection = unsafe { &mut *(this.native_ptr as *mut Connection) };
    connection.callback.on_net_block_status_change =
        Some(callback.into_global_callback(env).unwrap());
    Ok(())
}

#[ani_rs::native]
pub(crate) fn on_net_capabilities_change(
    env: &AniEnv,
    this: NetConnection,
    callback: AniFnObject,
) -> Result<(), BusinessError> {
    let connection = unsafe { &mut *(this.native_ptr as *mut Connection) };
    connection.callback.on_net_capabilities_change =
        Some(callback.into_global_callback(env).unwrap());
    Ok(())
}

#[ani_rs::native]
pub(crate) fn on_net_connection_properties_change(
    env: &AniEnv,
    this: NetConnection,
    callback: AniFnObject,
) -> Result<(), BusinessError> {
    let connection = unsafe { &mut *(this.native_ptr as *mut Connection) };
    connection.callback.on_net_connection_properties_change =
        Some(callback.into_global_callback(env).unwrap());
    Ok(())
}

#[ani_rs::native]
pub(crate) fn on_net_lost(
    env: &AniEnv,
    this: NetConnection,
    callback: AniFnObject,
) -> Result<(), BusinessError> {
    let connection = unsafe { &mut *(this.native_ptr as *mut Connection) };
    connection.callback.on_net_lost = Some(callback.into_global_callback(env).unwrap());
    Ok(())
}

#[ani_rs::native]
pub(crate) fn on_net_unavailable(
    env: &AniEnv,
    this: NetConnection,
    callback: AniFnObject,
) -> Result<(), BusinessError> {
    let connection = unsafe { &mut *(this.native_ptr as *mut Connection) };
    connection.callback.on_net_unavailable = Some(callback.into_global_callback(env).unwrap());
    Ok(())
}

#[ani_rs::native]
pub(crate) fn register_network_change(this: NetConnection) -> Result<(), BusinessError> {
    let connection = unsafe { &mut *(this.native_ptr as *mut Connection) };

    let mut callback = Box::new(ConnCallback::new());
    mem::swap(&mut connection.callback, &mut callback);

    let unregister = NetConnClient::register_net_conn_callback(callback).map_err(|e| {
        BusinessError::new(e, format!("Failed to register network change callback"))
    })?;
    connection.unregister = Some(unregister);

    Ok(())
}

#[ani_rs::native]
pub(crate) fn unregister_network_change(this: NetConnection) -> Result<(), BusinessError> {
    let connection = unsafe { &mut *(this.native_ptr as *mut Connection) };

    if let Some(unregister) = connection.unregister.as_mut() {
        if let Err(e) = unregister.unregister() {
            return Err(BusinessError::new(
                e,
                format!("Failed to unregister network change callback"),
            ));
        }
        Ok(())
    } else {
        Err(BusinessError::new(
            2101007,
            format!("No network change callback to unregister"),
        ))
    }
}

#[ani_rs::native]
pub(crate) fn connection_clean(this: Cleaner) -> Result<(), BusinessError> {
    let mut connection = unsafe { Box::from_raw(this.ptr as *mut Connection) };
    if let Some(mut unregister) = connection.unregister.take() {
        let _ = unregister.unregister();
    }
    drop(connection);
    Ok(())
}

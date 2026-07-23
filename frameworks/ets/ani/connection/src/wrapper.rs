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

use std::ffi::CString;

use crate::connection::ConnCallback;
use cxx::{let_cxx_string, UniquePtr};
use ffi::{NetBlockStatusInfo, NetCapabilityInfo, NetConnectionPropertyInfo, NetHandle};

use crate::bridge;

pub struct NetConnClient;

pub fn check_permission(permission: &str) -> bool {
    let token_id = ipc::Skeleton::calling_full_token_id();
    ffi::CheckPermission(token_id, permission)
}

impl NetConnClient {
    pub fn get_default_net_handle() -> Result<bridge::NetHandle, i32> {
        let mut ret = 0;
        let net_handle = ffi::GetDefaultNetHandle(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(net_handle.into())
    }

    pub fn get_all_nets() -> Result<Vec<bridge::NetHandle>, i32> {
        let mut ret = 0;
        let net_handles = ffi::GetAllNets(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(net_handles.into_iter().map(Into::into).collect())
    }

    pub fn has_default_net() -> Result<bool, i32> {
        let mut ret = 0;
        let has_default_net = ffi::HasDefaultNet(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(has_default_net)
    }

    pub fn get_net_capabilities(handle: bridge::NetHandle) -> Result<bridge::NetCapabilities, i32> {
        let mut ret = 0;
        let net_capabilities = ffi::GetNetCapabilities(&handle.into(), &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(net_capabilities.into())
    }

    pub fn get_default_http_proxy() -> Result<bridge::HttpProxy, i32> {
        let mut ret = 0;
        let default_http_proxy = ffi::GetDefaultHttpProxy(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(default_http_proxy.into())
    }

    pub fn get_global_http_proxy() -> Result<bridge::HttpProxy, i32> {
        let mut ret = 0;
        let global_http_proxy = ffi::GetGlobalHttpProxy(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(global_http_proxy.into())
    }

    pub fn set_global_http_proxy(proxy: bridge::HttpProxy) -> Result<(), i32> {
        let ret = ffi::SetGlobalHttpProxy(&proxy.into());
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn set_app_http_proxy(proxy: bridge::HttpProxy) -> Result<(), i32> {
        let ret = ffi::SetAppHttpProxy(&proxy.into());
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn get_app_net() -> Result<i32, i32> {
        let net_conn_client = ffi::GetNetConnClient(&mut 0);

        let mut net_id = 0;
        let ret = net_conn_client.GetAppNet(&mut net_id);
        if ret != 0 {
            return Err(ret);
        }
        Ok(net_id)
    }

    pub fn set_app_net(net_id: i32) -> Result<(), i32> {
        let net_conn_client = ffi::GetNetConnClient(&mut 0);

        let ret = net_conn_client.SetAppNet(net_id);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn set_airplane_mode(enable: bool) -> Result<(), i32> {
        let net_conn_client = ffi::GetNetConnClient(&mut 0);

        let ret = net_conn_client.SetAirplaneMode(enable);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn set_pac_url(pac_url: &str) -> Result<(), i32> {
        let net_conn_client = ffi::GetNetConnClient(&mut 0);

        let_cxx_string!(pac_url = pac_url);
        let ret = net_conn_client.SetPacUrl(&pac_url);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn get_pac_url() -> Result<String, i32> {
        let net_conn_client = ffi::GetNetConnClient(&mut 0);

        let_cxx_string!(pac_url = "");
        let ret = net_conn_client.GetPacUrl(pac_url.as_mut());
        if ret != 0 {
            return Err(ret);
        }
        Ok(pac_url.to_string())
    }

    pub fn factory_reset_network() -> Result<(), i32> {
        let net_conn_client = ffi::GetNetConnClient(&mut 0);

        let ret = net_conn_client.FactoryResetNetwork();
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn refresh_global_http_proxy() -> Result<bridge::HttpProxy, i32> {
        let mut ret = 0;
        let http_proxy = ffi::RefreshGlobalHttpProxySync(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(http_proxy.into())
    }

    pub fn set_pac_file_url(pac_url: &str) -> Result<(), i32> {
        let_cxx_string!(pac_url = pac_url);
        let ret = ffi::SetPacFileUrl(&pac_url);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn find_proxy_for_url(url: &str) -> Result<String, i32> {
        let_cxx_string!(url = url);
        let mut ret = 0;
        let proxy = ffi::FindProxyForURL(&url, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(proxy)
    }

    pub fn get_addresses_by_name_with_options(
        host: &str,
        net_id: i32,
        family: i32,
    ) -> Result<Vec<bridge::NetAddress>, i32> {
        let_cxx_string!(host = host);
        let mut ret = 0;
        let addresses = ffi::GetAddressesByNameWithOptions(&host, net_id, family, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(addresses.into_iter().map(Into::into).collect())
    }

    pub fn create_vlan_interface(if_name: &str, vlan_id: u32) -> Result<(), i32> {
        let_cxx_string!(if_name = if_name);
        let ret = ffi::CreateVlanInterface(&if_name, vlan_id);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn destroy_vlan_interface(if_name: &str, vlan_id: u32) -> Result<(), i32> {
        let_cxx_string!(if_name = if_name);
        let ret = ffi::DestroyVlanInterface(&if_name, vlan_id);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn add_vlan_ip(if_name: &str, vlan_id: u32, ip: &str, mask: u32) -> Result<(), i32> {
        let_cxx_string!(if_name = if_name);
        let_cxx_string!(ip = ip);
        let ret = ffi::AddVlanIp(&if_name, vlan_id, &ip, mask);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn delete_vlan_ip(if_name: &str, vlan_id: u32, ip: &str, mask: u32) -> Result<(), i32> {
        let_cxx_string!(if_name = if_name);
        let_cxx_string!(ip = ip);
        let ret = ffi::DeleteVlanIp(&if_name, vlan_id, &ip, mask);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn get_system_net_port_states() -> Result<bridge::NetPortStatesInfo, i32> {
        let mut ret = 0;
        let info = ffi::GetSystemNetPortStates(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(info.into())
    }

    pub fn get_ip_neigh_table() -> Result<Vec<bridge::NetIpMacInfo>, i32> {
        let mut ret = 0;
        let list = ffi::GetIpNeighTable(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(list.into_iter().map(Into::into).collect())
    }

    pub fn get_connect_owner_uid(
        protocol_type: i32,
        family: i32,
        local_address: &str,
        local_port: u16,
        remote_address: &str,
        remote_port: u16,
    ) -> Result<i32, i32> {
        let param = ffi::NetConnInfoParam {
            protocol_type,
            family,
            local_address: local_address.to_string(),
            local_port,
            remote_address: remote_address.to_string(),
            remote_port,
        };
        let mut ret: i32 = 0;
        let uid = ffi::GetConnectOwnerUid(&param, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(uid)
    }

    pub fn get_dns_unicode(host: &str, conversion_process: i32) -> Result<String, i32> {
        let_cxx_string!(host = host);
        let mut ret = 0;
        let unicode = ffi::GetDnsUnicode(&host, conversion_process, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(unicode)
    }

    pub fn get_dns_ascii(host: &str, conversion_process: i32) -> Result<String, i32> {
        let_cxx_string!(host = host);
        let mut ret = 0;
        let ascii = ffi::GetDnsAscii(&host, conversion_process, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(ascii)
    }

    pub fn set_interface_up(iface: &str) -> Result<(), i32> {
        let_cxx_string!(iface = iface);
        let ret = ffi::SetInterfaceUp(&iface);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn query_probe_result(dest: &str, duration: i32) -> Result<bridge::ProbeResultInfo, i32> {
        let_cxx_string!(dest = dest);
        let mut ret = 0;
        let info = ffi::QueryProbeResult(&dest, duration, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(info.into())
    }

    pub fn query_trace_route(
        destination: &str,
        max_jump_number: i32,
        packets_type: i32,
    ) -> Result<Vec<bridge::TraceRouteInfo>, i32> {
        let_cxx_string!(destination = destination);
        let mut ret = 0;
        let list = ffi::QueryTraceRoute(&destination, max_jump_number, packets_type, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(list.into_iter().map(Into::into).collect())
    }

    pub fn get_proxy_mode() -> Result<bridge::ProxyMode, i32> {
        let mut mode: i32 = 0;
        let ret = ffi::GetProxyMode(&mut mode);
        if ret != 0 {
            return Err(ret);
        }
        match mode {
            0 => Ok(bridge::ProxyMode::PROXY_MODE_OFF),
            1 => Ok(bridge::ProxyMode::PROXY_MODE_AUTO),
            _ => Err(-1),
        }
    }

    pub fn set_proxy_mode(mode: crate::bridge::ProxyMode) -> Result<(), i32> {
        let ret = ffi::SetProxyMode(mode as i32);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn get_pac_file_url() -> Result<String, i32> {
        let mut ret = 0;
        let url = ffi::GetPacFileUrl(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(url)
    }

    pub fn set_net_ext_attribute(net_id: i32, net_ext_attribute: &str) -> Result<(), i32> {
        let_cxx_string!(net_ext_attribute = net_ext_attribute);
        let ret = ffi::SetNetExtAttribute(net_id, &net_ext_attribute);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn get_net_ext_attribute(net_id: i32) -> Result<String, i32> {
        let mut ret = 0;
        let attr = ffi::GetNetExtAttribute(net_id, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(attr)
    }

    pub fn is_default_net_metered() -> Result<bool, i32> {
        let mut is_metered = false;
        let ret = ffi::IsDefaultNetMetered(&mut is_metered);
        if ret != 0 {
            return Err(ret);
        }
        Ok(is_metered)
    }

    pub fn get_connection_properties(net_id: i32) -> Result<bridge::ConnectionProperties, i32> {
        let mut ret = 0;
        let connection_properties = ffi::GetConnectionProperties(net_id, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(connection_properties.into())
    }

    pub fn get_addresses_by_name(host: &str, net_id: i32) -> Result<Vec<bridge::NetAddress>, i32> {
        let_cxx_string!(host = host);

        let mut ret = 0;
        let addresses = ffi::GetAddressesByName(&host, net_id, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(addresses.into_iter().map(Into::into).collect())
    }

    pub fn get_address_by_name(host: &str, net_id: i32) -> Result<bridge::NetAddress, i32> {
        let_cxx_string!(host = host);

        let mut ret = 0;
        let address = ffi::GetAddressByName(&host, net_id, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(address.into())
    }

    pub fn bind_socket(fd: i32, net_id: i32) -> Result<(), i32> {
        let net_conn_client = ffi::GetNetConnClient(&mut 0);

        let ret = net_conn_client.BindSocket(fd, net_id);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn net_detection(net_id: i32) -> Result<(), i32> {
        let mut ret = 0;
        ffi::NetDetection(net_id, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn clear_custom_dns_rules() -> Result<(), i32> {
        let ret = unsafe { predefined_host_clear_all_hosts() };
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn remove_custom_dns_rule(host: String) -> Result<(), i32> {
        let Ok(c_host) = CString::new(host) else {
            return Err(-1);
        };
        let ret = unsafe { predefined_host_remove_host(c_host.as_ptr()) };
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn set_custom_dns_rules(mut host: String, ips: Vec<String>) -> Result<(), i32> {
        host.push(',');
        for i in 0..ips.len() {
            host.push_str(&ips[i]);
            if i < ips.len() - 1 {
                host.push(',');
            }
        }
        let Ok(c_host) = CString::new(host) else {
            return Err(-1);
        };

        let ret = unsafe { predefined_host_set_hosts(c_host.as_ptr()) };
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }

    pub fn register_net_conn_callback(
        callback: Box<ConnCallback>,
    ) -> Result<ConnUnregisterHandle, i32> {
        let mut ret = 0;
        let unregister = ffi::RegisterNetConnCallback(callback, &mut ret);
        if ret != 0 {
            return Err(ret);
        }
        Ok(ConnUnregisterHandle { inner: unregister })
    }
}

pub struct ConnUnregisterHandle {
    inner: UniquePtr<ffi::UnregisterHandle>,
}

impl ConnUnregisterHandle {
    pub fn unregister(&mut self) -> Result<(), i32> {
        let ret = self.inner.pin_mut().Unregister();
        if ret != 0 {
            return Err(ret);
        }
        Ok(())
    }
}

impl ConnCallback {
    fn on_net_available(&self, handle: NetHandle) -> i32 {
        if let Some(callback) = self.on_net_available.as_ref() {
            callback.execute((handle.into(),));
        }
        0
    }

    fn on_net_block_status_change(&self, status: NetBlockStatusInfo) -> i32 {
        if let Some(callback) = self.on_net_block_status_change.as_ref() {
            callback.execute((status.into(),));
        }
        0
    }

    fn on_net_capabilities_change(&self, cap: NetCapabilityInfo) -> i32 {
        if let Some(callback) = self.on_net_capabilities_change.as_ref() {
            callback.execute((cap.into(),));
        }
        0
    }

    fn on_net_connection_properties_change(&self, properties: NetConnectionPropertyInfo) -> i32 {
        if let Some(callback) = self.on_net_connection_properties_change.as_ref() {
            callback.execute((properties.into(),));
        }
        0
    }

    fn on_net_lost(&self, handle: NetHandle) -> i32 {
        if let Some(callback) = self.on_net_lost.as_ref() {
            callback.execute((handle.into(),));
        }
        0
    }

    fn on_net_unavailable(&self) -> i32 {
        if let Some(callback) = self.on_net_unavailable.as_ref() {
            callback.execute(());
        }
        0
    }
}

impl From<NetHandle> for bridge::NetHandle {
    fn from(handle: NetHandle) -> Self {
        bridge::NetHandle {
            net_id: handle.net_id,
        }
    }
}

impl From<bridge::NetHandle> for NetHandle {
    fn from(handle: bridge::NetHandle) -> Self {
        NetHandle {
            net_id: handle.net_id,
        }
    }
}

impl From<ffi::NetCap> for bridge::NetCap {
    fn from(value: ffi::NetCap) -> Self {
        match value {
            ffi::NetCap::NET_CAPABILITY_MMS => bridge::NetCap::NetCapabilityMms,
            ffi::NetCap::NET_CAPABILITY_NOT_METERED => bridge::NetCap::NetCapabilityNotMetered,
            ffi::NetCap::NET_CAPABILITY_INTERNET => bridge::NetCap::NetCapabilityInternet,
            ffi::NetCap::NET_CAPABILITY_NOT_VPN => bridge::NetCap::NetCapabilityNotVpn,
            ffi::NetCap::NET_CAPABILITY_VALIDATED => bridge::NetCap::NetCapabilityValidated,
            ffi::NetCap::NET_CAPABILITY_PORTAL => bridge::NetCap::NetCapabilityPortal,
            ffi::NetCap::NET_CAPABILITY_CHECKING_CONNECTIVITY => {
                bridge::NetCap::NetCapabilityCheckingConnectivity
            }
            _ => unimplemented!(),
        }
    }
}

impl From<ffi::NetBearType> for bridge::NetBearType {
    fn from(value: ffi::NetBearType) -> Self {
        match value {
            ffi::NetBearType::BEARER_CELLULAR => bridge::NetBearType::BearerCellular,
            ffi::NetBearType::BEARER_WIFI => bridge::NetBearType::BearerWifi,
            ffi::NetBearType::BEARER_BLUETOOTH => bridge::NetBearType::BearerBluetooth,
            ffi::NetBearType::BEARER_ETHERNET => bridge::NetBearType::BearerEthernet,
            ffi::NetBearType::BEARER_VPN => bridge::NetBearType::BearerVpn,
            _ => unimplemented!(),
        }
    }
}

impl From<ffi::NetCapabilities> for bridge::NetCapabilities {
    fn from(value: ffi::NetCapabilities) -> Self {
        bridge::NetCapabilities {
            link_up_bandwidth_kbps: Some(value.linkUpBandwidthKbps),
            link_down_bandwidth_kbps: Some(value.linkDownBandwidthKbps),
            network_cap: Some(value.networkCap.into_iter().map(Into::into).collect()),
            bearer_types: value.bearerTypes.into_iter().map(Into::into).collect(),
        }
    }
}

impl From<ffi::HttpProxy> for bridge::HttpProxy {
    fn from(value: ffi::HttpProxy) -> Self {
        bridge::HttpProxy {
            host: value.host,
            port: value.port,
            username: Some(value.username),
            password: Some(value.password),
            exclusion_list: value.exclusionList,
        }
    }
}

impl From<ffi::LinkAddress> for bridge::LinkAddress {
    fn from(value: ffi::LinkAddress) -> Self {
        bridge::LinkAddress {
            address: bridge::NetAddress {
                address: value.address.address,
                family: Some(value.address.family),
                port: Some(value.address.port),
            },
            prefix_length: value.prefix_length,
        }
    }
}

impl From<ffi::NetAddress> for bridge::NetAddress {
    fn from(value: ffi::NetAddress) -> Self {
        bridge::NetAddress {
            address: value.address,
            family: Some(value.family),
            port: Some(value.port),
        }
    }
}

impl From<ffi::RouteInfo> for bridge::RouteInfo {
    fn from(value: ffi::RouteInfo) -> Self {
        bridge::RouteInfo {
            iface: value.interface,
            destination: value.destination.into(),
            gateway: value.gateway.into(),
            has_gateway: value.has_gateway,
            is_default_route: value.is_default_route,
        }
    }
}

impl From<ffi::ConnectionProperties> for bridge::ConnectionProperties {
    fn from(value: ffi::ConnectionProperties) -> Self {
        bridge::ConnectionProperties {
            interface_name: value.interface_name,
            domains: value.domains,
            link_addresses: value.link_addresses.into_iter().map(Into::into).collect(),
            dnses: value.dnses.into_iter().map(Into::into).collect(),
            routes: value.routes.into_iter().map(Into::into).collect(),
            mtu: value.mtu,
        }
    }
}

impl From<bridge::HttpProxy> for ffi::HttpProxy {
    fn from(value: bridge::HttpProxy) -> Self {
        ffi::HttpProxy {
            host: value.host,
            port: value.port,
            username: value.username.unwrap_or_default(),
            password: value.password.unwrap_or_default(),
            exclusionList: value.exclusion_list,
        }
    }
}

impl From<ffi::NetBlockStatusInfo> for bridge::NetBlockStatusInfo {
    fn from(value: ffi::NetBlockStatusInfo) -> Self {
        bridge::NetBlockStatusInfo {
            net_handle: value.net_handle.into(),
            blocked: value.blocked,
        }
    }
}

impl From<ffi::NetCapabilityInfo> for bridge::NetCapabilityInfo {
    fn from(value: ffi::NetCapabilityInfo) -> Self {
        bridge::NetCapabilityInfo {
            net_handle: value.net_handle.into(),
            net_cap: value.net_cap.into(),
        }
    }
}

impl From<ffi::NetConnectionPropertyInfo> for bridge::NetConnectionPropertyInfo {
    fn from(value: ffi::NetConnectionPropertyInfo) -> Self {
        bridge::NetConnectionPropertyInfo {
            net_handle: value.net_handle.into(),
            connection_properties: value.connection_properties.into(),
        }
    }
}

impl From<ffi::AniTcpNetPortStatesInfo> for bridge::TcpNetPortStatesInfo {
    fn from(value: ffi::AniTcpNetPortStatesInfo) -> Self {
        bridge::TcpNetPortStatesInfo {
            tcp_local_ip: value.tcpLocalIp,
            tcp_local_port: value.tcpLocalPort as i32,
            tcp_remote_ip: value.tcpRemoteIp,
            tcp_remote_port: value.tcpRemotePort as i32,
            tcp_uid: value.tcpUid,
            tcp_pid: value.tcpPid,
            tcp_state: match value.tcpState {
                1 => bridge::TcpState::TCP_ESTABLISHED,
                2 => bridge::TcpState::TCP_SYN_SENT,
                3 => bridge::TcpState::TCP_SYN_RECV,
                4 => bridge::TcpState::TCP_FIN_WAIT1,
                5 => bridge::TcpState::TCP_FIN_WAIT2,
                6 => bridge::TcpState::TCP_TIME_WAIT,
                7 => bridge::TcpState::TCP_CLOSE,
                8 => bridge::TcpState::TCP_CLOSE_WAIT,
                9 => bridge::TcpState::TCP_LAST_ACK,
                10 => bridge::TcpState::TCP_LISTEN,
                11 => bridge::TcpState::TCP_CLOSING,
                _ => bridge::TcpState::TCP_CLOSE,
            },
        }
    }
}

impl From<ffi::AniUdpNetPortStatesInfo> for bridge::UdpNetPortStatesInfo {
    fn from(value: ffi::AniUdpNetPortStatesInfo) -> Self {
        bridge::UdpNetPortStatesInfo {
            udp_local_ip: value.udpLocalIp,
            udp_local_port: value.udpLocalPort as i32,
            udp_uid: value.udpUid,
            udp_pid: value.udpPid,
        }
    }
}

impl From<ffi::AniNetPortStatesInfo> for bridge::NetPortStatesInfo {
    fn from(value: ffi::AniNetPortStatesInfo) -> Self {
        bridge::NetPortStatesInfo {
            tcp_port_states_info: Some(value.tcpPortStatesInfo.into_iter().map(Into::into).collect()),
            udp_port_states_info: Some(value.udpPortStatesInfo.into_iter().map(Into::into).collect()),
        }
    }
}

impl From<ffi::AniNetIpMacInfo> for bridge::NetIpMacInfo {
    fn from(value: ffi::AniNetIpMacInfo) -> Self {
        bridge::NetIpMacInfo {
            ip_address: bridge::NetAddress {
                address: value.ipAddress,
                family: Some(value.family),
                port: Some(0),
            },
            mac_address: value.macAddress,
            iface: value.iface,
        }
    }
}

impl From<ffi::AniProbeResultInfo> for bridge::ProbeResultInfo {
    fn from(value: ffi::AniProbeResultInfo) -> Self {
        bridge::ProbeResultInfo {
            loss_rate: value.lossRate,
            rtt: value.rtt,
        }
    }
}

impl From<ffi::AniTraceRouteInfo> for bridge::TraceRouteInfo {
    fn from(value: ffi::AniTraceRouteInfo) -> Self {
        bridge::TraceRouteInfo {
            jump_no: value.jumpNo,
            address: value.address,
            rtt: value.rtt,
        }
    }
}

#[cxx::bridge(namespace = "OHOS::NetManagerAni")]
pub mod ffi {

    #[namespace = "OHOS::NetManagerStandard"]
    #[repr(i32)]
    pub enum NetCap {
        NET_CAPABILITY_MMS = 0,
        NET_CAPABILITY_NOT_METERED = 11,
        NET_CAPABILITY_INTERNET = 12,
        NET_CAPABILITY_NOT_VPN = 15,
        NET_CAPABILITY_VALIDATED = 16,
        NET_CAPABILITY_PORTAL = 17,
        NET_CAPABILITY_CHECKING_CONNECTIVITY = 31,
    }

    #[namespace = "OHOS::NetManagerStandard"]
    #[repr(i32)]
    pub enum NetBearType {
        BEARER_CELLULAR = 0,
        BEARER_WIFI = 1,
        BEARER_BLUETOOTH = 2,
        BEARER_ETHERNET = 3,
        BEARER_VPN = 4,
    }

    pub struct NetHandle {
        pub net_id: i32,
    }

    pub struct NetBlockStatusInfo {
        pub net_handle: NetHandle,
        pub blocked: bool,
    }

    pub struct NetCapabilities {
        linkUpBandwidthKbps: i32,
        linkDownBandwidthKbps: i32,
        networkCap: Vec<NetCap>,
        bearerTypes: Vec<NetBearType>,
    }

    pub struct NetCapabilityInfo {
        pub net_handle: NetHandle,
        pub net_cap: NetCapabilities,
    }

    pub struct NetConnectionPropertyInfo {
        pub net_handle: NetHandle,
        pub connection_properties: ConnectionProperties,
    }

    pub struct HttpProxy {
        host: String,
        port: i32,
        username: String,
        password: String,
        exclusionList: Vec<String>,
    }

    pub struct LinkAddress {
        pub address: NetAddress,
        pub prefix_length: i32,
    }

    pub struct NetAddress {
        pub address: String,
        pub family: i32,
        pub port: i32,
    }

    pub struct RouteInfo {
        pub interface: String,

        pub destination: LinkAddress,

        pub gateway: NetAddress,

        pub has_gateway: bool,

        pub is_default_route: bool,

        pub is_excluded_route: bool,
    }

    pub struct ConnectionProperties {
        pub interface_name: String,
        pub domains: String,
        pub link_addresses: Vec<LinkAddress>,

        pub dnses: Vec<NetAddress>,

        pub routes: Vec<RouteInfo>,

        pub mtu: i32,

        pub is_ipv6_link_valid: bool,

        pub is_ipv4_link_valid: bool,
    }

    pub struct NetConnInfoParam {
        pub protocol_type: i32,
        pub family: i32,
        pub local_address: String,
        pub local_port: u16,
        pub remote_address: String,
        pub remote_port: u16,
    }

    extern "Rust" {
        type ConnCallback;

        fn on_net_available(&self, handle: NetHandle) -> i32;
        fn on_net_block_status_change(&self, status: NetBlockStatusInfo) -> i32;
        fn on_net_capabilities_change(&self, cap: NetCapabilityInfo) -> i32;
        fn on_net_connection_properties_change(&self, properties: NetConnectionPropertyInfo)
            -> i32;
        fn on_net_lost(&self, handle: NetHandle) -> i32;
        fn on_net_unavailable(&self) -> i32;
    }

    unsafe extern "C++" {
        include!("connection_ani.h");
        include!("net_all_capabilities.h");

        #[namespace = "OHOS::NetManagerStandard"]
        type NetCap;
        #[namespace = "OHOS::NetManagerStandard"]
        type NetBearType;
        #[namespace = "OHOS::NetManagerStandard"]
        type NetConnClient;

        type UnregisterHandle;

        fn Unregister(self: Pin<&mut UnregisterHandle>) -> i32;

        fn GetDefaultNetHandle(ret: &mut i32) -> NetHandle;
        fn GetAllNets(ret: &mut i32) -> Vec<NetHandle>;
        fn HasDefaultNet(ret: &mut i32) -> bool;
        fn GetNetCapabilities(handle: &NetHandle, ret: &mut i32) -> NetCapabilities;

        fn GetDefaultHttpProxy(ret: &mut i32) -> HttpProxy;
        fn GetGlobalHttpProxy(ret: &mut i32) -> HttpProxy;
        fn SetGlobalHttpProxy(proxy: &HttpProxy) -> i32;
        fn SetAppHttpProxy(proxy: &HttpProxy) -> i32;

        fn GetNetConnClient(_: &mut i32) -> Pin<&'static mut NetConnClient>;

        fn SetAppNet(self: Pin<&mut NetConnClient>, net_id: i32) -> i32;

        fn GetAppNet(self: Pin<&mut NetConnClient>, net_id: &mut i32) -> i32;

        fn SetAirplaneMode(self: Pin<&mut NetConnClient>, enable: bool) -> i32;

        fn IsDefaultNetMetered(is_metered: &mut bool) -> i32;

        fn GetPacUrl(self: Pin<&mut NetConnClient>, pac_url: Pin<&mut CxxString>) -> i32;

        fn SetPacUrl(self: Pin<&mut NetConnClient>, pac_url: &CxxString) -> i32;

        fn FactoryResetNetwork(self: Pin<&mut NetConnClient>) -> i32;

        fn GetConnectionProperties(net_id: i32, ret: &mut i32) -> ConnectionProperties;

        fn GetAddressesByName(host: &CxxString, net_id: i32, ret: &mut i32) -> Vec<NetAddress>;

        fn GetAddressByName(host: &CxxString, net_id: i32, ret: &mut i32) -> NetAddress;

        fn BindSocket(self: Pin<&mut NetConnClient>, fd: i32, net_id: i32) -> i32;

        fn NetDetection(net_id: i32, ret: &mut i32);

        fn CheckPermission(token_id: u64, permission: &str) -> bool;

        fn RegisterNetConnCallback(
            connection: Box<ConnCallback>,
            ret: &mut i32,
        ) -> UniquePtr<UnregisterHandle>;

        fn GetErrorCodeAndMessage(error_code: &mut i32) -> String;

        fn RefreshGlobalHttpProxySync(ret: &mut i32) -> HttpProxy;

        fn SetPacFileUrl(pac_url: &CxxString) -> i32;

        fn FindProxyForURL(url: &CxxString, ret: &mut i32) -> String;

        fn GetAddressesByNameWithOptions(host: &CxxString, net_id: i32, family: i32, ret: &mut i32) -> Vec<NetAddress>;

        fn CreateVlanInterface(if_name: &CxxString, vlan_id: u32) -> i32;

        fn DestroyVlanInterface(if_name: &CxxString, vlan_id: u32) -> i32;

        fn AddVlanIp(if_name: &CxxString, vlan_id: u32, ip: &CxxString, mask: u32) -> i32;

        fn DeleteVlanIp(if_name: &CxxString, vlan_id: u32, ip: &CxxString, mask: u32) -> i32;

        fn GetSystemNetPortStates(ret: &mut i32) -> AniNetPortStatesInfo;

        fn GetIpNeighTable(ret: &mut i32) -> Vec<AniNetIpMacInfo>;

        type NetConnInfoParam;

        fn GetConnectOwnerUid(param: &NetConnInfoParam, ret: &mut i32) -> i32;

        fn GetDnsUnicode(host: &CxxString, conversion_process: i32, ret: &mut i32) -> String;

        fn GetDnsAscii(host: &CxxString, conversion_process: i32, ret: &mut i32) -> String;

        fn SetInterfaceUp(iface: &CxxString) -> i32;

        fn QueryProbeResult(dest: &CxxString, duration: i32, ret: &mut i32) -> AniProbeResultInfo;

        fn QueryTraceRoute(destination: &CxxString, max_jump_number: i32, packets_type: i32,
                           ret: &mut i32) -> Vec<AniTraceRouteInfo>;

        fn GetProxyMode(mode: &mut i32) -> i32;

        fn SetProxyMode(mode: i32) -> i32;

        fn GetPacFileUrl(ret: &mut i32) -> String;

        fn SetNetExtAttribute(net_id: i32, net_ext_attribute: &CxxString) -> i32;

        fn GetNetExtAttribute(net_id: i32, ret: &mut i32) -> String;
    }

    pub struct AniTcpNetPortStatesInfo {
        pub tcpLocalIp: String,
        pub tcpLocalPort: u16,
        pub tcpRemoteIp: String,
        pub tcpRemotePort: u16,
        pub tcpUid: i32,
        pub tcpPid: i32,
        pub tcpState: i32,
    }

    pub struct AniUdpNetPortStatesInfo {
        pub udpLocalIp: String,
        pub udpLocalPort: u16,
        pub udpUid: i32,
        pub udpPid: i32,
    }

    pub struct AniNetPortStatesInfo {
        pub tcpPortStatesInfo: Vec<AniTcpNetPortStatesInfo>,
        pub udpPortStatesInfo: Vec<AniUdpNetPortStatesInfo>,
    }

    pub struct AniNetIpMacInfo {
        pub ipAddress: String,
        pub family: i32,
        pub macAddress: String,
        pub iface: String,
    }

    pub struct AniProbeResultInfo {
        pub lossRate: i32,
        pub rtt: Vec<i32>,
    }

    pub struct AniTraceRouteInfo {
        pub jumpNo: i32,
        pub address: String,
        pub rtt: Vec<f64>,
    }
}

extern "C" {
    fn predefined_host_clear_all_hosts() -> i32;
    fn predefined_host_remove_host(host: *const ::std::os::raw::c_char) -> i32;
    fn predefined_host_set_hosts(host_ips: *const ::std::os::raw::c_char) -> i32;
}

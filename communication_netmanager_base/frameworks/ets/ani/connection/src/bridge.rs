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

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetConnectionInner")]
pub struct NetConnection {
    pub native_ptr: i64,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.Cleaner")]
pub struct Cleaner {
    pub ptr: i64,
}

#[ani_rs::ani]
pub struct NetSpecifier {
    pub net_capabilities: NetCapabilities,
    pub bearer_private_identifier: Option<String>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetCapabilityInfoInner")]
pub struct NetCapabilityInfo {
    pub net_handle: NetHandle,
    pub net_cap: NetCapabilities,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetHandleInner")]
pub struct NetHandle {
    pub net_id: i32,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetCapabilitiesInner")]
pub struct NetCapabilities {
    pub link_up_bandwidth_kbps: Option<i32>,
    pub link_down_bandwidth_kbps: Option<i32>,
    pub network_cap: Option<Vec<NetCap>>,
    pub bearer_types: Vec<NetBearType>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetConnectionPropertyInfoInner")]
pub struct NetConnectionPropertyInfo {
    pub net_handle: NetHandle,
    pub connection_properties: ConnectionProperties,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetBlockStatusInfoInner")]
pub struct NetBlockStatusInfo {
    pub net_handle: NetHandle,
    pub blocked: bool,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetCap")]
pub enum NetCap {
    NetCapabilityMms = 0,
    NetCapabilityNotMetered = 11,
    NetCapabilityInternet = 12,
    NetCapabilityNotVpn = 15,
    NetCapabilityValidated = 16,
    NetCapabilityPortal = 17,
    NetCapabilityCheckingConnectivity = 31,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetBearType")]
pub enum NetBearType {
    BearerCellular = 0,
    BearerWifi = 1,
    BearerBluetooth = 2,
    BearerEthernet = 3,
    BearerVpn = 4,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.ConnectionPropertiesInner")]
pub struct ConnectionProperties {
    pub interface_name: String,
    pub domains: String,
    pub link_addresses: Vec<LinkAddress>,
    pub dnses: Vec<NetAddress>,
    pub routes: Vec<RouteInfo>,
    pub mtu: i32,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.RouteInfoInner")]
pub struct RouteInfo {
    pub iface: String,
    pub destination: LinkAddress,
    pub gateway: NetAddress,
    pub has_gateway: bool,
    pub is_default_route: bool,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.LinkAddressInner")]
pub struct LinkAddress {
    pub address: NetAddress,
    pub prefix_length: i32,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetAddressInner")]
pub struct NetAddress {
    pub address: String,
    pub family: Option<i32>,
    pub port: Option<i32>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.Socks5DnsStrategy")]
pub enum Socks5DnsStrategy {
    SYSTEM_MODE = 0,
    PROXY_MODE = 1,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.Socks5ProxyInner")]
pub struct Socks5Proxy {
    pub host: String,
    pub port: i32,
    pub username: Option<String>,
    pub password: Option<String>,
    pub dns_strategy: Option<Socks5DnsStrategy>,
    pub exclusion_list: Option<Vec<String>>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.HttpProxyInner")]
pub struct HttpProxy {
    pub host: String,
    pub port: i32,
    pub username: Option<String>,
    pub password: Option<String>,
    pub exclusion_list: Vec<String>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.FamilyType")]
pub enum FamilyType {
    FAMILY_TYPE_ALL = 0,
    FAMILY_TYPE_IPV4 = 1,
    FAMILY_TYPE_IPV6 = 2,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.ConversionProcess")]
pub enum ConversionProcess {
    NO_CONFIGURATION = 0,
    ALLOW_UNASSIGNED = 1,
    USE_STD3_ASCII_RULES = 2,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.TcpState")]
pub enum TcpState {
    TCP_ESTABLISHED = 1,
    TCP_SYN_SENT = 2,
    TCP_SYN_RECV = 3,
    TCP_FIN_WAIT1 = 4,
    TCP_FIN_WAIT2 = 5,
    TCP_TIME_WAIT = 6,
    TCP_CLOSE = 7,
    TCP_CLOSE_WAIT = 8,
    TCP_LAST_ACK = 9,
    TCP_LISTEN = 10,
    TCP_CLOSING = 11,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.ProtocolType")]
pub enum ProtocolType {
    PROTO_TYPE_TCP = 6,
    PROTO_TYPE_UDP = 17,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.PacketsType")]
pub enum PacketsType {
    NETCONN_PACKETS_ICMP = 0,
    NETCONN_PACKETS_UDP = 1,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.ProxyMode")]
pub enum ProxyMode {
    PROXY_MODE_OFF = 0,
    PROXY_MODE_AUTO = 1,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.TcpNetPortStatesInfoInner")]
pub struct TcpNetPortStatesInfo {
    pub tcp_local_ip: String,
    pub tcp_local_port: i32,
    pub tcp_remote_ip: String,
    pub tcp_remote_port: i32,
    pub tcp_uid: i32,
    pub tcp_pid: i32,
    pub tcp_state: TcpState,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.UdpNetPortStatesInfoInner")]
pub struct UdpNetPortStatesInfo {
    pub udp_local_ip: String,
    pub udp_local_port: i32,
    pub udp_uid: i32,
    pub udp_pid: i32,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetPortStatesInfoInner")]
pub struct NetPortStatesInfo {
    pub tcp_port_states_info: Option<Vec<TcpNetPortStatesInfo>>,
    pub udp_port_states_info: Option<Vec<UdpNetPortStatesInfo>>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetIpMacInfoInner")]
pub struct NetIpMacInfo {
    pub ip_address: NetAddress,
    pub mac_address: String,
    pub iface: String,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.QueryOptionsInner")]
pub struct QueryOptions {
    pub family: Option<FamilyType>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.ProbeResultInfoInner")]
pub struct ProbeResultInfo {
    pub loss_rate: i32,
    pub rtt: Vec<i32>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.TraceRouteInfoInner")]
pub struct TraceRouteInfo {
    pub jump_no: i32,
    pub address: String,
    pub rtt: Vec<f64>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.TraceRouteOptionsInner")]
pub struct TraceRouteOptions {
    pub max_jump_number: Option<i32>,
    pub packets_type: Option<PacketsType>,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.UDPSocket")]
pub struct UDPSocket {
    pub socket_fd: i32,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.TCPSocket")]
pub struct TCPSocket {
    pub socket_fd: i32,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetConnInfoParamInner")]
pub struct NetConnInfoParam {
    pub protocol_type: ProtocolType,
    pub family: FamilyType,
    pub local_address: String,
    pub local_port: i32,
    pub remote_address: String,
    pub remote_port: i32,
}

#[ani_rs::ani(path = "@ohos.net.connection.connection.HttpRequest")]
pub struct HttpRequest {
    pub url: String,
    pub method: String,
    pub headers: Vec<String>,
    pub body: Option<String>,
}

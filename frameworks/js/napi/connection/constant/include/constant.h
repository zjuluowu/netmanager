/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETMANAGERBASE_CONSTANT_H
#define COMMUNICATIONNETMANAGERBASE_CONSTANT_H

static constexpr const char *KEY_ADDRESS = "address";
static constexpr const char *KEY_FAMILY = "family";
static constexpr const char *KEY_PORT = "port";

static constexpr const int PARAM_NONE = 0;
static constexpr const int PARAM_JUST_OPTIONS = 1;
static constexpr const int PARAM_JUST_CALLBACK = 1;
static constexpr const int PARAM_OPTIONS_AND_CALLBACK = 2;
static constexpr const int PARAM_JUST_OPTIONS_TWO = 2;
static constexpr const int PARAM_JUST_OPTIONS_THREE = 3;

static constexpr const char *EVENT_NET_AVAILABLE = "netAvailable";
static constexpr const char *EVENT_NET_BLOCK_STATUS_CHANGE = "netBlockStatusChange";
static constexpr const char *EVENT_NET_CAPABILITIES_CHANGE = "netCapabilitiesChange";
static constexpr const char *EVENT_NET_CONNECTION_PROPERTIES_CHANGE = "netConnectionPropertiesChange";
static constexpr const char *EVENT_NET_LOST = "netLost";
static constexpr const char *EVENT_NET_UNAVAILABLE = "netUnavailable";

static constexpr const char *EVENT_IFACE_ADDRESS_UPDATED = "interfaceAddressUpdated";
static constexpr const char *EVENT_IFACE_ADDRESS_REMOVED = "interfaceAddressRemoved";
static constexpr const char *EVENT_IFACE_ADDED = "interfaceAdded";
static constexpr const char *EVENT_IFACE_REMOVED = "interfaceRemoved";
static constexpr const char *EVENT_IFACE_CHANGED = "interfaceChanged";
static constexpr const char *EVENT_IFACE_LINK_STATE_CHANGED = "interfaceLinkStateChanged";
static constexpr const char *EVENT_IFACE_ROUTE_CHANGED = "routeChanged";

static constexpr const char *KEY_LINK_UP_BAND_WIDTH_KPS = "linkUpBandwidthKbps";
static constexpr const char *KEY_LINK_DOWN_BAND_WIDTH_KPS = "linkDownBandwidthKbps";
static constexpr const char *KEY_NET_CAPABILITIES = "netCapabilities";
static constexpr const char *KEY_BEARER_PRIVATE_IDENTIFIER = "bearerPrivateIdentifier";
static constexpr const char *KEY_NETWORK_CAP = "networkCap";
static constexpr const char *KEY_BEARER_TYPE = "bearerTypes";

static constexpr const char *KEY_INTERFACE_NAME = "interfaceName";
static constexpr const char *KEY_IS_USE_PRIVATE_DNS = "isUsePrivateDns";
static constexpr const char *KEY_PRIVATE_DNS_SERVER_NAME = "privateDnsServerName";
static constexpr const char *KEY_DOMAINS = "domains";
static constexpr const char *KEY_HTTP_PROXY = "httpProxy";
static constexpr const char *KEY_LINK_ADDRESSES = "linkAddresses";
static constexpr const char *KEY_DNSES = "dnses";
static constexpr const char *KEY_ROUTES = "routes";
static constexpr const char *KEY_MTU = "mtu";
static constexpr const char *KEY_IS_IPV4_LINK_VALID = "isIPv4LinkValid";
static constexpr const char *KEY_IS_IPV6_LINK_VALID = "isIPv6LinkValid";
static constexpr const char *KEY_PREFIX_LENGTH = "prefixLength";
static constexpr const char *KEY_INTERFACE = "interface";
static constexpr const char *KEY_DESTINATION = "destination";
static constexpr const char *KEY_GATE_WAY = "gateway";
static constexpr const char *KEY_HAS_GET_WAY = "hasGateway";
static constexpr const char *KEY_IS_DEFAULT_ROUE = "isDefaultRoute";
static constexpr const char *KEY_IS_EXCLUDED_ROUTE = "isExcludedRoute";
static constexpr const char *KEY_HOST = "host";
static constexpr const char *KEY_PARSED_EXCLUSION_LIST = "parsedExclusionList";
static constexpr const char *KEY_NET_HANDLE = "netHandle";
static constexpr const char *KEY_BLOCKED = "blocked";
static constexpr const char *KEY_NET_CAP = "netCap";
static constexpr const char *KEY_CONNECTION_PROPERTIES = "connectionProperties";
static constexpr const char *KEY_NET_ID = "netId";

static constexpr const char *KEY_SOCKET_FD = "socketFd";
static constexpr const char *KEY_FLAGS = "flags";
static constexpr const char *KEY_SCOPE = "scope";
static constexpr const char *KEY_UP = "up";
static constexpr const char *KEY_ROUTE = "route";
static constexpr const char *KEY_UPDATED = "updated";

static constexpr const char *KEY_IP_ADDRESS = "ipAddress";
static constexpr const char *KEY_IFACE_NAME = "iface";
static constexpr const char *KEY_MAC_ADDRESS = "macAddress";

static constexpr const char *KEY_TCP_PORT_STATES_INFO = "tcpPortStatesInfo";
static constexpr const char *KEY_TCP_LOCAL_IP = "tcpLocalIp";
static constexpr const char *KEY_TCP_LOCAL_PORT = "tcpLocalPort";
static constexpr const char *KEY_TCP_REOMTE_IP = "tcpRemoteIp";
static constexpr const char *KEY_TCP_REMOTE_PORT = "tcpRemotePort";
static constexpr const char *KEY_TCP_UID = "tcpUid";
static constexpr const char *KEY_TCP_PID = "tcpPid";
static constexpr const char *KEY_TCP_STATE = "tcpState";

static constexpr const char *KEY_UDP_PORT_STATES_INFO = "udpPortStatesInfo";
static constexpr const char *KEY_UDP_LOCAL_IP = "udpLocalIp";
static constexpr const char *KEY_UDP_LOCAL_PORT = "udpLocalPort";
static constexpr const char *KEY_UDP_UID = "udpUid";
static constexpr const char *KEY_UDP_PID = "udpPid";

static constexpr const char *KEY_JUMP_NO = "jumpNo";
static constexpr const char *KEY_RTT = "rtt";
static constexpr const char *KEY_LOSS_RATE = "lossRate";

enum {
    ARG_NUM_0,
    ARG_NUM_1,
    ARG_NUM_2,
};

#endif /* COMMUNICATIONNETMANAGERBASE_CONSTANT_H */

/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NET_FIREWALL_MAP_DEF_H
#define NET_FIREWALL_MAP_DEF_H

#ifndef SEC
#define SEC(NAME) __attribute__((section(NAME), used))
#endif // SEC

#define MAP_MAX_ENTRIES 5000
#define MAP_MAX_PORT_ENTRIES 65535

#define INGRESS_SADDR_MAP in_saddr_map
#define INGRESS_SADDR6_MAP in_saddr6_map
#define INGRESS_DADDR_MAP in_daddr_map
#define INGRESS_DADDR6_MAP in_daddr6_map
#define INGRESS_SPORT_MAP in_sport_map
#define INGRESS_DPORT_MAP in_dport_map
#define INGRESS_PROTO_MAP in_proto_map
#define INGRESS_APPUID_MAP in_appuid_map
#define INGRESS_UID_MAP in_uid_map
#define INGRESS_ACTION_MAP in_action_map
#define INGRESS_INTERFACE_MAP in_iface_map

#define EGRESS_SADDR_MAP out_saddr_map
#define EGRESS_SADDR6_MAP out_saddr6_map
#define EGRESS_DADDR_MAP out_daddr_map
#define EGRESS_DADDR6_MAP out_daddr6_map
#define EGRESS_SPORT_MAP out_sport_map
#define EGRESS_DPORT_MAP out_dport_map
#define EGRESS_PROTO_MAP out_proto_map
#define EGRESS_APPUID_MAP out_appuid_map
#define EGRESS_UID_MAP out_uid_map
#define EGRESS_ACTION_MAP out_action_map
#define EGRESS_INTERFACE_MAP out_iface_map

#define EVENT_MAP event_map
#define DEFAULT_ACTION_MAP def_act_map
#define CT_MAP ct_map
#define CURRENT_UID_MAP current_uid_map
#define DOMAIN_IPV4_MAP domain_ipv4_map
#define DOMAIN_IPV6_MAP domain_ipv6_map
#define DOMAIN_PASS_MAP domain_pass_map
#define DOMAIN_DENY_MAP domain_deny_map
#define LOOP_BACK_IPV4_MAP loop_ipv4_map
#define LOOP_BACK_IPV6_MAP loop_ipv6_map
#define DOMAIN_DATA_KEY_MAP domain_data_key_map

#define MAPS_DIR() "/sys/fs/bpf/netsys/maps/"
#define STR(x) #x
#define MAP_NAME(x) STR(x)
#define MAP_PATH(name) MAPS_DIR() MAP_NAME(name)
#define GET_MAP_PATH(ingress, name) ((ingress) ? MAPS_DIR() "in_" #name "_map" : MAPS_DIR() "out_" #name "_map")
#define GET_MAP(ingress, name) ((ingress) ? &in_##name##_map : &out_##name##_map)

#endif // NET_FIREWALL_MAP_DEF_H
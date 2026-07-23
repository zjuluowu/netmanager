/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NETMANAGER_BASE_BPF_PATH_H
#define NETMANAGER_BASE_BPF_PATH_H
namespace OHOS::NetManagerStandard {
static constexpr const char *IFACE_STATS_MAP_PATH = "/sys/fs/bpf/netsys/maps/iface_stats_map";
static constexpr const char *APP_UID_STATS_MAP_PATH = "/sys/fs/bpf/netsys/maps/app_uid_stats_map";
static constexpr const char *APP_UID_IF_STATS_MAP_PATH = "/sys/fs/bpf/netsys/maps/app_uid_if_stats_map";
static constexpr const char *APP_UID_SIM_STATS_MAP_PATH = "/sys/fs/bpf/netsys/maps/app_uid_sim_stats_map";
static constexpr const char *SOCK_NETNS_MAP_PATH = "/sys/fs/bpf/netsys/maps/sock_netns_map";
static constexpr const char *OH_SOCKET_PERMISSION_MAP_PATH = "/sys/fs/bpf/netsys/maps/oh_sock_permission_map";
static constexpr const char *BROKER_SOCKET_PERMISSION_MAP_PATH =
    "/sys/fs/bpf/netsys/maps/broker_sock_permission_map";
static constexpr const char *APP_COOKIE_STATS_MAP_PATH = "/sys/fs/bpf/netsys/maps/app_cookie_stats_map";
static constexpr const char *APP_UID_PERMISSION_MAP_PATH = "/sys/fs/bpf/netsys/maps/app_uid_access_policy_map";
static constexpr const char *RING_BUFFER_MAP_PATH = "/sys/fs/bpf/netsys/maps/ringbuf_map";
static constexpr const char *NET_STATS_RING_BUFFER_MAP_PATH = "/sys/fs/bpf/netsys/maps/net_stats_ringbuf_map";
static constexpr const char *NET_BEAR_TYPE_MAP_PATH = "/sys/fs/bpf/netsys/maps/net_bear_type_map";
static constexpr const char *BROKER_UID_ACCESS_POLICY_MAP_PATH = "/sys/fs/bpf/netsys/maps/broker_uid_access_policy_map";
static constexpr const char *NET_INDEX_AND_IFACE_MAP_PATH = "/sys/fs/bpf/netsys/maps/net_index_and_iface_map";
static constexpr const char *LIMITS_STATS_MAP_PATH = "/sys/fs/bpf/netsys/maps/limits_stats_map";
static constexpr const char *INCREMENT_STATS_MAP_PATH = "/sys/fs/bpf/netsys/maps/increment_stats_map";
static constexpr const char *IFINDEX_MAP_PATH = "/sys/fs/bpf/netsys/maps/ifindex_map";
static constexpr const char *NET_STATUS_MAP_PATH = "/sys/fs/bpf/netsys/maps/net_status_map";
static constexpr const char *NET_WLAN1_MAP_PATH = "/sys/fs/bpf/netsys/maps/net_wlan1_map";
static constexpr const char *IFINDEX_AND_NET_TYPE_MAP_PATH = "/sys/fs/bpf/netsys/maps/ifindex_and_net_type_map";
} // namespace OHOS::NetManagerStandard
#endif /* NETMANAGER_BASE_BPF_PATH_H */

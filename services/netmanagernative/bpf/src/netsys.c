/*
* Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.

* The netsys.c is dual licensed: you can use it either under the terms of
* the GPL V2, or the 3-Clause BSD license, at your option.
* See the LICENSE file in the root of this repository for complete details.
*/

#include <linux/bpf.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/string.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "bpf_helpers.h"
#include "bpf_def.h"

#ifdef FEATURE_NET_FIREWALL_ENABLE
#include "netfirewall/netfirewall.h"
#endif //FEATURE_NET_FIREWALL_ENABLE

#define SEC(NAME) __attribute__((section(NAME), used))

#ifdef SUPPORT_EBPF_MEM_MIN
static const int32_t APP_STATS_MAP_SIZE = 200;
static const int32_t APP_STATS_MAP_SIZE_MIN = 1;
static const int32_t IFACE_STATS_MAP_SIZE = 64;
static const int32_t IFACE_NAME_MAP_SIZE = 200;
static const int32_t IFACE_NAME_MAP_SIZE_MIN = 1;
static const int32_t OH_SOCK_PERMISSION_MAP_SIZE = 200;
static const int32_t BROKER_SOCK_PERMISSION_MAP_SIZE = 1;
static const int32_t UID_ACCESS_POLICY_ARRAY_SIZE = 1000;
static const int32_t NET_NS_MAP_SIZE = 2000;
#else
static const int32_t APP_STATS_MAP_SIZE = 5000;
static const int32_t APP_STATS_MAP_SIZE_MIN = 5000;
static const int32_t IFACE_STATS_MAP_SIZE = 1000;
static const int32_t IFACE_NAME_MAP_SIZE = 1000;
static const int32_t IFACE_NAME_MAP_SIZE_MIN = 1000;
static const int32_t OH_SOCK_PERMISSION_MAP_SIZE = 1000;
static const int32_t BROKER_SOCK_PERMISSION_MAP_SIZE = 1000;
static const int32_t UID_ACCESS_POLICY_ARRAY_SIZE = 65535;
static const int32_t NET_NS_MAP_SIZE = 5000;
#endif
static const int32_t TRAFFIC_INCREASE_MAP_SIZE = 9;
static const uint32_t TRAFFIC_NOTIFY_TYPE = 3;

// network stats begin
bpf_map_def SEC("maps") iface_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint64_t),
    .value_size = sizeof(iface_stats_value),
    .max_entries = IFACE_STATS_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") app_uid_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint64_t),
    .value_size = sizeof(app_uid_stats_value),
    .max_entries = APP_STATS_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") sock_netns_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(sock_netns_key),
    .value_size = sizeof(sock_netns_value),
    .max_entries = NET_NS_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") app_uid_sim_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(app_uid_sim_stats_key),
    .value_size = sizeof(app_uid_sim_stats_value),
    .max_entries = APP_STATS_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") app_uid_if_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(app_uid_if_stats_key),
    .value_size = sizeof(app_uid_if_stats_value),
    .max_entries = IFACE_NAME_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") app_cookie_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(socket_cookie_stats_key),
    .value_size = sizeof(app_cookie_stats_value),
    .max_entries = IFACE_NAME_MAP_SIZE_MIN,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

// 1. 建立一个可用限额map
bpf_map_def SEC("maps") limits_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(traffic_notify_flag),
    .value_size = sizeof(traffic_value),
    .max_entries = TRAFFIC_INCREASE_MAP_SIZE, // slot0:0-monthly limit 1-monthly mark 2-daily mark;slot1: 3 4 5
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

// 2. 建立一个增量map
bpf_map_def SEC("maps") increment_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint64_t),
    .value_size = sizeof(traffic_value),
    .max_entries = TRAFFIC_INCREASE_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

// 3. 网卡index
bpf_map_def SEC("maps") ifindex_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint8_t),
    .value_size = sizeof(uint64_t),
    .max_entries = 20,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") net_stats_ringbuf_map = {
    .type = BPF_MAP_TYPE_RINGBUF,
    .max_entries = 256 * 1024 /* 256 KB */,
};

bpf_map_def SEC("maps") net_status_map = {
    .type =BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint8_t),
    .value_size = sizeof(uint8_t),
    .max_entries = 3,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") net_wlan1_map = {
    .type =BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint8_t),
    .value_size = sizeof(uint64_t),
    .max_entries = 1,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

static inline __u8 socket_ringbuf_net_stats_event_submit(__u8 flag)
{
    uint8_t *e;
    e = bpf_ringbuf_reserve(&net_stats_ringbuf_map, sizeof(uint8_t), 0);
    if (e) {
        *e = flag;
        bpf_ringbuf_submit(e, 0);
        return 1;
    }
    return 0;
}

static inline __u32 get_data_len(struct __sk_buff *skb)
{
    __u32 length = skb->len;
    __u32 packages = skb->gso_segs;
    if (packages < 1) {
        packages = 1;
    }

    if (skb->vlan_present == 1) {
        length += VLAN_HEADER_LENGTH;
    }
    if (skb->family == AF_INET) {
        length += (IPV4_HEADER_LENGTH + TRANSFER_HEADER_LENGTH) * (packages - 1);
    }
    if (skb->family == AF_INET6) {
        length += (IPV6_HEADER_LENGTH + TRANSFER_HEADER_LENGTH) * (packages - 1);
    }
    return length;
}

static traffic_value update_new_incre_value(uint64_t ifindex, __u32 len)
{
    traffic_value *old_value = bpf_map_lookup_elem(&increment_stats_map, &ifindex);
    if (old_value == NULL) {
        traffic_value newValue = 0;
        bpf_map_update_elem(&increment_stats_map, &ifindex, &newValue, BPF_NOEXIST);
        old_value = bpf_map_lookup_elem(&increment_stats_map, &ifindex);
    }

    if (old_value != NULL) {
        *old_value = (*old_value) + len;
        bpf_map_update_elem(&increment_stats_map, &ifindex, old_value, BPF_ANY);
        return *old_value;
    }
    return 0;
}

static traffic_notify_flag is_exceed_daily_mark(uint32_t slotId, traffic_value used_value)
{
    traffic_notify_flag daily_mark = slotId * TRAFFIC_NOTIFY_TYPE + 2; // 2: DAILY_MARK
    traffic_value *day_mark_avai = bpf_map_lookup_elem(&limits_stats_map, &daily_mark);

    traffic_value max_value = UINT64_MAX;
    if (day_mark_avai == NULL) {
        bpf_map_update_elem(&limits_stats_map, &daily_mark, &max_value, BPF_NOEXIST);
        day_mark_avai = bpf_map_lookup_elem(&limits_stats_map, &daily_mark);
    }
    if (day_mark_avai != NULL && used_value > *day_mark_avai) {
        bpf_map_update_elem(&limits_stats_map, &daily_mark, &max_value, BPF_ANY);
        return daily_mark;
    }
    return UINT8_MAX;
}

static traffic_notify_flag is_exceed_mothly_mark(uint32_t slotId, traffic_value used_value)
{
    traffic_notify_flag monthly_mark = slotId * TRAFFIC_NOTIFY_TYPE + 1; // 1: MONTHLY_MARK
    traffic_value *monthly_mark_avai = bpf_map_lookup_elem(&limits_stats_map, &monthly_mark);

    traffic_value max_value = UINT64_MAX;
    if (monthly_mark_avai == NULL) {
        bpf_map_update_elem(&limits_stats_map, &monthly_mark, &max_value, BPF_NOEXIST);
        monthly_mark_avai = bpf_map_lookup_elem(&limits_stats_map, &monthly_mark);
    }
    if (monthly_mark_avai != NULL && used_value > *monthly_mark_avai) {
        bpf_map_update_elem(&limits_stats_map, &monthly_mark, &max_value, BPF_ANY);
        return monthly_mark;
    }
    return UINT8_MAX;
}

static traffic_notify_flag is_exceed_mothly_limit(uint32_t slotId, traffic_value used_value)
{
    traffic_notify_flag monthly_limit = slotId * TRAFFIC_NOTIFY_TYPE + 0; // 0: MONTHLY_LIMIT
    traffic_value *monthly_limit_avai = bpf_map_lookup_elem(&limits_stats_map, &monthly_limit);

    traffic_value max_value = UINT64_MAX;
    if (monthly_limit_avai == NULL) {
        bpf_map_update_elem(&limits_stats_map, &monthly_limit, &max_value, BPF_NOEXIST);
        monthly_limit_avai = bpf_map_lookup_elem(&limits_stats_map, &monthly_limit);
    }
    if (monthly_limit_avai != NULL && used_value > *monthly_limit_avai) {
        bpf_map_update_elem(&limits_stats_map, &monthly_limit, &max_value, BPF_ANY);
        return monthly_limit;
    }
    return UINT8_MAX;
}

static void process_traffic_notify(struct __sk_buff *skb, uint64_t ifindex)
{
    uint8_t slot0 = 0;
    uint8_t slot1 = 1;
    uint8_t curSlotId = UINT8_MAX;
    uint64_t *cur_rmnet_ifindex = bpf_map_lookup_elem(&ifindex_map, &slot0);
    if (cur_rmnet_ifindex == NULL || *cur_rmnet_ifindex != ifindex) {
        cur_rmnet_ifindex = bpf_map_lookup_elem(&ifindex_map, &slot1);
        if (cur_rmnet_ifindex == NULL || *cur_rmnet_ifindex != ifindex) {
            return;
        } else {
            curSlotId = slot1;
        }
    } else {
        curSlotId = slot0;
    }
 
    traffic_value used_value = update_new_incre_value(ifindex, skb->len);
    if (used_value == 0) {
        return;
    }
    traffic_notify_flag flag = is_exceed_mothly_limit(curSlotId, used_value);
    if (flag == UINT8_MAX) {
        flag = is_exceed_mothly_mark(curSlotId, used_value);
        if (flag == UINT8_MAX) {
            flag = is_exceed_daily_mark(curSlotId, used_value);
        }
    }
    if (flag != UINT8_MAX) {
        socket_ringbuf_net_stats_event_submit(flag);
    }
}

SEC("socket/iface/stats")
int socket_iface_stats(struct __sk_buff *skb)
{
    if (skb == NULL) {
        return 1;
    }

    if (skb->pkt_type == PACKET_LOOPBACK) {
        return 1;
    }

    uint64_t ifindex = skb->ifindex;
    iface_stats_value *value_if = bpf_map_lookup_elem(&iface_stats_map, &ifindex);
    if (value_if == NULL) {
        iface_stats_value newValue = {};
        bpf_map_update_elem(&iface_stats_map, &ifindex, &newValue, BPF_NOEXIST);
        value_if = bpf_map_lookup_elem(&iface_stats_map, &ifindex);
    }

    if (skb->pkt_type == PACKET_OUTGOING) {
        if (value_if != NULL) {
            __sync_fetch_and_add(&value_if->txPackets, 1);
            __sync_fetch_and_add(&value_if->txBytes, skb->len);
        }
    } else {
        if (value_if != NULL) {
            __sync_fetch_and_add(&value_if->rxPackets, 1);
            __sync_fetch_and_add(&value_if->rxBytes, skb->len);
        }
    }

    process_traffic_notify(skb, ifindex);
    return 1;
}

bpf_map_def SEC("maps") app_uid_access_policy_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(app_uid_key),
    .value_size = sizeof(uid_access_policy_value),
    .max_entries = UID_ACCESS_POLICY_ARRAY_SIZE,
    .map_flags = BPF_F_NO_PREALLOC,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") broker_uid_access_policy_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(app_uid_key),
    .value_size = sizeof(app_uid_key),
    .max_entries = APP_STATS_MAP_SIZE_MIN,
    .map_flags = BPF_F_NO_PREALLOC,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") net_index_and_iface_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(net_index),
    .value_size = sizeof(net_interface_name_id),
    .max_entries = 5,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") ifindex_and_net_type_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(if_index),
    .value_size = sizeof(net_interface_name_id),
    .max_entries = 5,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

static inline net_bear_type_map_value check_socket_fwmark(__u32 mark)
{
    __u8 explicitlySelected = (mark >> 16) & (0x1);
    net_bear_type_map_value net_bear_mark_type = NETWORK_BEARER_TYPE_INITIAL;
    // explicitlySelected == 1 means the socket fwmark is set
    if (explicitlySelected == 1) {
        void *ifaceC_map_ptr = &net_index_and_iface_map;
        __u16 TmpnetId = mark & (0x0000FFFF);
        net_interface_name_id *ifaceC = bpf_map_lookup_elem(ifaceC_map_ptr, &TmpnetId);
        // ifaceC == NULL, default bear type (*net_bear_type) is used.
        if (ifaceC != NULL) {
            net_bear_mark_type = *ifaceC;
        }
    }
    return net_bear_mark_type;
}

static inline net_bear_type_map_value check_socket_bound_dev_if(__u32 ifIndex)
{
    net_bear_type_map_value net_bear_mark_type = NETWORK_BEARER_TYPE_INITIAL;
    void *ifnet_map_ptr = &ifindex_and_net_type_map;
    net_interface_name_id *ifaceC = bpf_map_lookup_elem(ifnet_map_ptr, &ifIndex);
    if (ifaceC != NULL) {
        net_bear_mark_type = *ifaceC;
    }
    return net_bear_mark_type;
}

static inline net_bear_type_map_value check_socket_select_net(struct bpf_sock *sk)
{
    net_bear_type_map_value net_bear_mark_type = NETWORK_BEARER_TYPE_INITIAL;
    if (sk == NULL) {
        return net_bear_mark_type;
    }
    net_bear_mark_type = check_socket_fwmark(sk->mark);
    net_bear_type_map_value net_bear_bound_type = check_socket_bound_dev_if(sk->bound_dev_if);
    if (net_bear_bound_type != NETWORK_BEARER_TYPE_INITIAL) {
        net_bear_mark_type = net_bear_bound_type;
    }
    return net_bear_mark_type;
}

bpf_map_def SEC("maps") net_bear_type_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(net_bear_id_key),
    .value_size = sizeof(net_bear_type_map_value),
    .max_entries = IFACE_NAME_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

static inline __u8 check_network_policy(net_bear_type_map_value net_bear_mark_type,
                                        uid_access_policy_value *netAccessPolicyValue)
{
    if (((net_bear_mark_type == NETWORK_BEARER_TYPE_WIFI) && (!netAccessPolicyValue->wifiPolicy)) ||
        ((net_bear_mark_type == NETWORK_BEARER_TYPE_CELLULAR) && (!netAccessPolicyValue->cellularPolicy))) {
        return 0;
    } else if (net_bear_mark_type != NETWORK_BEARER_TYPE_INITIAL) {
        return 1;
    }
    if (((netAccessPolicyValue->netIfIndex == NETWORK_BEARER_TYPE_WIFI) && (!netAccessPolicyValue->wifiPolicy)) ||
        ((netAccessPolicyValue->netIfIndex == NETWORK_BEARER_TYPE_CELLULAR) &&
        (!netAccessPolicyValue->cellularPolicy))) {
        return 0;
    }
    if (netAccessPolicyValue->netIfIndex == NETWORK_BEARER_TYPE_INITIAL) {
        void *net_bear_map_ptr = &net_bear_type_map;
        net_bear_id_key net_bear_id = DEFAULT_NETWORK_BEARER_MAP_KEY;
        net_bear_type_map_value *net_bear_type = bpf_map_lookup_elem(net_bear_map_ptr, &net_bear_id);
        if (net_bear_type == NULL) {
            return 1;
        }

        if (((*net_bear_type == NETWORK_BEARER_TYPE_CELLULAR)) && (!netAccessPolicyValue->cellularPolicy)) {
            return 0;
        }
        if (((*net_bear_type == NETWORK_BEARER_TYPE_WIFI)) && (!netAccessPolicyValue->wifiPolicy)) {
            return 0;
        }
    }
    return 1;
}

static inline __u64 check_broker_policy(uint64_t uid)
{
    if (uid < SIM_UID_MIN) {
        return uid;
    }
    uint64_t network_access_uid = uid;
    void *broker_map_ptr = &broker_uid_access_policy_map;
    app_uid_key *broker_uid_key = (app_uid_key *)&uid;
    app_uid_key *broker_uid_value = bpf_map_lookup_elem(broker_map_ptr, broker_uid_key);
    if (broker_uid_value != NULL) {
        network_access_uid = *broker_uid_value;
        return network_access_uid;
    } else {
        __u32 broker_default_uid = DEFAULT_BROKER_UID_KEY;
        app_uid_key *broker_default_value = bpf_map_lookup_elem(broker_map_ptr, &broker_default_uid);
        if (broker_default_value != NULL && uid > SIM_UID_MIN && uid < SIM_UID_MAX) {
            network_access_uid = *broker_default_value;
            bpf_map_update_elem(broker_map_ptr, broker_uid_key, broker_default_value, BPF_NOEXIST);
        }
    }
    return network_access_uid;
}

static inline __u32 filter_sim_stats(__u32 ipv4)
{
    if (IS_MATCHED_IP(ipv4, WLAN_IPv4) || IS_MATCHED_IP(ipv4, CELLULAR_IPv4) || IS_MATCHED_IP(ipv4, CELLULAR_IPv42)) {
        return 1;
    }
    return 0;
}

static inline __u32 get_iface_type(__u32 ipv4)
{
    if (IS_MATCHED_IP(ipv4, WLAN_IPv4)) {
        return IFACE_TYPE_WIFI;
    }
    if (IS_MATCHED_IP(ipv4, CELLULAR_IPv4) || IS_MATCHED_IP(ipv4, CELLULAR_IPv42)) {
        return IFACE_TYPE_CELLULAR;
    }
    return 0;
}

static __u32 rgm_stats_rx_count(struct __sk_buff *skb, __u64 sock_uid)
{
    __u32 ip = 0;
    if (skb->family == AF_INET && filter_sim_stats(skb->local_ip4) == 1) {
        ip = skb->local_ip4;
    } else if (skb->family == AF_INET6 && filter_sim_stats(skb->local_ip6[3]) == 1) { // 3: the last 32 bit
        ip = skb->local_ip6[3]; // 3: the last 32 bit
    }
    if (ip != 0) {
        app_uid_sim_stats_key key_sim = {.uId = sock_uid, .ifIndex = skb->ifindex,
                                         .ifType = get_iface_type(ip)};
        app_uid_sim_stats_value *value_uid_sim = bpf_map_lookup_elem(&app_uid_sim_stats_map, &key_sim);
        if (value_uid_sim == NULL) {
            app_uid_sim_stats_value newValue = {};
            bpf_map_update_elem(&app_uid_sim_stats_map, &key_sim, &newValue, BPF_NOEXIST);
            value_uid_sim = bpf_map_lookup_elem(&app_uid_sim_stats_map, &key_sim);
        }
        if (value_uid_sim != NULL) {
            __sync_fetch_and_add(&value_uid_sim->rxPackets, 1);
            __sync_fetch_and_add(&value_uid_sim->rxBytes, get_data_len(skb));
        }
    }
    return 0;
}

static __u32 rgm_stats_tx_count(struct __sk_buff *skb, __u64 sock_uid)
{
    __u32 ip = 0;
    if (skb->family == AF_INET && filter_sim_stats(skb->local_ip4) == 1) {
        ip = skb->local_ip4;
    } else if (skb->family == AF_INET6 && filter_sim_stats(skb->local_ip6[3]) == 1) { // 3: the last 32 bit
        ip = skb->local_ip6[3]; // 3: the last 32 bit
    }
    if (ip != 0) {
        app_uid_sim_stats_key key_sim = {.uId = sock_uid, .ifIndex = skb->ifindex,
                                         .ifType = get_iface_type(ip)};
        app_uid_sim_stats_value *value_uid_sim = bpf_map_lookup_elem(&app_uid_sim_stats_map, &key_sim);
        if (value_uid_sim == NULL) {
            app_uid_sim_stats_value newValue = {};
            bpf_map_update_elem(&app_uid_sim_stats_map, &key_sim, &newValue, BPF_NOEXIST);
            value_uid_sim = bpf_map_lookup_elem(&app_uid_sim_stats_map, &key_sim);
        }
        if (value_uid_sim != NULL) {
            __sync_fetch_and_add(&value_uid_sim->txPackets, 1);
            __sync_fetch_and_add(&value_uid_sim->txBytes, get_data_len(skb));
        }
    }
    return 0;
}

SEC("cgroup_skb/uid/ingress")
int bpf_cgroup_skb_uid_ingress(struct __sk_buff *skb)
{
#ifdef FEATURE_NET_FIREWALL_ENABLE
    if (skb == NULL) {
        return 1;
    }
    if (netfirewall_policy_ingress(skb) != SK_PASS) {
        return SK_DROP;
    }
    if (skb->pkt_type == PACKET_LOOPBACK) {
        return 1;
    }
#else
    if (skb == NULL || skb->pkt_type == PACKET_LOOPBACK) {
        return 1;
    }
#endif
    uint64_t sock_uid = bpf_get_socket_uid(skb);
    sock_netns_key key_sock_netns1 = sock_uid;
    sock_netns_value *value_sock_netns1 = bpf_map_lookup_elem(&sock_netns_map, &key_sock_netns1);
    sock_netns_key key_sock_netns2 = SOCK_COOKIE_ID_NULL;
    sock_netns_value *value_sock_netns2 = bpf_map_lookup_elem(&sock_netns_map, &key_sock_netns2);
    uint64_t network_access_uid = sock_uid;
    if (value_sock_netns1 != NULL && value_sock_netns2 != NULL && *value_sock_netns1 != *value_sock_netns2) {
        network_access_uid = check_broker_policy(sock_uid);
    }

    uid_access_policy_value *netAccessPolicyValue =
        bpf_map_lookup_elem(&app_uid_access_policy_map, &network_access_uid);
    if (netAccessPolicyValue != NULL) {
        net_bear_type_map_value net_bear_mark_type = check_socket_fwmark(skb->mark);
        net_bear_type_map_value net_bear_bound_type = check_socket_bound_dev_if(skb->ifindex);
        if (net_bear_bound_type != NETWORK_BEARER_TYPE_INITIAL) {
            net_bear_mark_type = net_bear_bound_type;
        }
        if (check_network_policy(net_bear_mark_type, netAccessPolicyValue) == 0) {
            return 0;
        }
    }
    app_uid_stats_value *value = bpf_map_lookup_elem(&app_uid_stats_map, &sock_uid);
    if (value == NULL) {
        app_uid_stats_value newValue = {};
        bpf_map_update_elem(&app_uid_stats_map, &sock_uid, &newValue, BPF_NOEXIST);
        value = bpf_map_lookup_elem(&app_uid_stats_map, &sock_uid);
    }
    if (value != NULL) {
        __sync_fetch_and_add(&value->rxPackets, 1);
        __sync_fetch_and_add(&value->rxBytes, skb->len);
    }
    socket_cookie_stats_key sock_cookie = bpf_get_socket_cookie(skb);
    app_cookie_stats_value *value_cookie = bpf_map_lookup_elem(&app_cookie_stats_map, &sock_cookie);
    if (value_cookie == NULL) {
        app_cookie_stats_value newValue = {};
        bpf_map_update_elem(&app_cookie_stats_map, &sock_cookie, &newValue, BPF_NOEXIST);
        value_cookie = bpf_map_lookup_elem(&app_cookie_stats_map, &sock_cookie);
    }
    if (value_cookie != NULL) {
        __sync_fetch_and_add(&value_cookie->rxPackets, 1);
        __sync_fetch_and_add(&value_cookie->rxBytes, skb->len);
    }

    if (value_sock_netns1 != NULL && value_sock_netns2 != NULL && *value_sock_netns1 == *value_sock_netns2) {
        app_uid_if_stats_key key = {.uId = sock_uid, .ifIndex = skb->ifindex};
        app_uid_if_stats_value *value_uid_if = bpf_map_lookup_elem(&app_uid_if_stats_map, &key);
        if (value_uid_if == NULL) {
            app_uid_if_stats_value newValue = {};
            bpf_map_update_elem(&app_uid_if_stats_map, &key, &newValue, BPF_NOEXIST);
            value_uid_if = bpf_map_lookup_elem(&app_uid_if_stats_map, &key);
        }
        if (value_uid_if != NULL) {
            __sync_fetch_and_add(&value_uid_if->rxPackets, 1);
            __sync_fetch_and_add(&value_uid_if->rxBytes, get_data_len(skb));
        }
    } else {
        rgm_stats_rx_count(skb, sock_uid);
    }
    return 1;
}

SEC("cgroup_skb/uid/egress")
int bpf_cgroup_skb_uid_egress(struct __sk_buff *skb)
{
#ifdef FEATURE_NET_FIREWALL_ENABLE
    if (skb == NULL) {
        return 1;
    }
    if (netfirewall_policy_egress(skb) != SK_PASS) {
        return SK_DROP;
    }
    if (skb->pkt_type == PACKET_LOOPBACK) {
        return 1;
    }
#else
    if (skb == NULL || skb->pkt_type == PACKET_LOOPBACK) {
        return 1;
    }
#endif

    uint64_t sock_uid = bpf_get_socket_uid(skb);
    sock_netns_key key_sock_netns1 = sock_uid;
    sock_netns_value *value_sock_netns1 = bpf_map_lookup_elem(&sock_netns_map, &key_sock_netns1);
    sock_netns_key key_sock_netns2 = SOCK_COOKIE_ID_NULL;
    sock_netns_value *value_sock_netns2 = bpf_map_lookup_elem(&sock_netns_map, &key_sock_netns2);
    uint64_t network_access_uid = sock_uid;
    if (value_sock_netns1 != NULL && value_sock_netns2 != NULL && *value_sock_netns1 != *value_sock_netns2) {
        network_access_uid = check_broker_policy(sock_uid);
    }
    uid_access_policy_value *netAccessPolicyValue =
        bpf_map_lookup_elem(&app_uid_access_policy_map, &network_access_uid);
    if (netAccessPolicyValue != NULL) {
        net_bear_type_map_value net_bear_mark_type = check_socket_fwmark(skb->mark);
        net_bear_type_map_value net_bear_bound_type = check_socket_bound_dev_if(skb->ifindex);
        if (net_bear_bound_type != NETWORK_BEARER_TYPE_INITIAL) {
            net_bear_mark_type = net_bear_bound_type;
        }
        if (check_network_policy(net_bear_mark_type, netAccessPolicyValue) == 0) {
            return 0;
        }
    }

    app_uid_stats_value *value = bpf_map_lookup_elem(&app_uid_stats_map, &sock_uid);
    if (value == NULL) {
        app_uid_stats_value newValue = {};
        bpf_map_update_elem(&app_uid_stats_map, &sock_uid, &newValue, BPF_NOEXIST);
        value = bpf_map_lookup_elem(&app_uid_stats_map, &sock_uid);
    }
    if (value != NULL) {
        __sync_fetch_and_add(&value->txPackets, 1);
        __sync_fetch_and_add(&value->txBytes, skb->len);
    }
    socket_cookie_stats_key sock_cookie = bpf_get_socket_cookie(skb);
    app_cookie_stats_value *value_cookie = bpf_map_lookup_elem(&app_cookie_stats_map, &sock_cookie);
    if (value_cookie == NULL) {
        app_cookie_stats_value newValue = {};
        bpf_map_update_elem(&app_cookie_stats_map, &sock_cookie, &newValue, BPF_NOEXIST);
        value_cookie = bpf_map_lookup_elem(&app_cookie_stats_map, &sock_cookie);
    }
    if (value_cookie != NULL) {
        __sync_fetch_and_add(&value_cookie->txPackets, 1);
        __sync_fetch_and_add(&value_cookie->txBytes, skb->len);
    }

    if (value_sock_netns1 != NULL && value_sock_netns2 != NULL && *value_sock_netns1 == *value_sock_netns2) {
        app_uid_if_stats_key key = {.uId = sock_uid, .ifIndex = skb->ifindex};
        app_uid_if_stats_value *value_uid_if = bpf_map_lookup_elem(&app_uid_if_stats_map, &key);
        if (value_uid_if == NULL) {
            app_uid_if_stats_value newValue = {};
            bpf_map_update_elem(&app_uid_if_stats_map, &key, &newValue, BPF_NOEXIST);
            value_uid_if = bpf_map_lookup_elem(&app_uid_if_stats_map, &key);
        }
        if (value_uid_if != NULL) {
            __sync_fetch_and_add(&value_uid_if->txPackets, 1);
            __sync_fetch_and_add(&value_uid_if->txBytes, get_data_len(skb));
        }
    } else {
        rgm_stats_tx_count(skb, sock_uid);
    }
    return 1;
}
// network stats end

// internet permission begin
bpf_map_def SEC("maps") oh_sock_permission_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(sock_permission_key),
    .value_size = sizeof(sock_permission_value),
    .max_entries = OH_SOCK_PERMISSION_MAP_SIZE,
};

bpf_map_def SEC("maps") broker_sock_permission_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(sock_permission_key),
    .value_size = sizeof(sock_permission_value),
    .max_entries = BROKER_SOCK_PERMISSION_MAP_SIZE,
};

SEC("cgroup_sock/inet_create_socket")
int inet_create_socket(struct bpf_sock *sk)
{
    __u64 uid_gid = bpf_get_current_uid_gid();
    sock_netns_key key_sock_netns1 = uid_gid & 0x00000000FFFFFFFF;
    sock_netns_value value_sock_netns1 = bpf_get_netns_cookie(sk);
    bpf_map_update_elem(&sock_netns_map, &key_sock_netns1, &value_sock_netns1, BPF_NOEXIST);
    sock_netns_key key_sock_netns2 = SOCK_COOKIE_ID_NULL;
    sock_netns_value value_sock_netns2 = bpf_get_netns_cookie(NULL);
    bpf_map_update_elem(&sock_netns_map, &key_sock_netns2, &value_sock_netns2, BPF_NOEXIST);

    void *map_ptr = &oh_sock_permission_map;
    if (bpf_get_netns_cookie(sk) != bpf_get_netns_cookie(NULL)) {
        map_ptr = &broker_sock_permission_map;
    }

    __u32 uid = (__u32)(uid_gid & 0x00000000FFFFFFFF);
    sock_permission_value *value = bpf_map_lookup_elem(map_ptr, &uid);
    // value == NULL means that the process attached to this uid is not a hap process which started by appspawn
    // it is a native process, native process should have this permission
    if (value == NULL) {
        return 1;
    }
    // *value == 0 means no permission
    if (*value == 0) {
        return 0;
    }
    return 1;
}

SEC("cgroup_sock/inet_release_socket")
int inet_release_socket(struct bpf_sock *sk)
{
    sock_netns_key key_sock_netns = bpf_get_socket_cookie(sk);
    bpf_map_delete_elem(&sock_netns_map, &key_sock_netns);

    socket_cookie_stats_key key_sock_cookie = bpf_get_socket_cookie(sk);
    bpf_map_delete_elem(&app_cookie_stats_map, &key_sock_cookie);
    return 1;
}
// internet permission end

bpf_map_def SEC("maps") ringbuf_map = {
    .type = BPF_MAP_TYPE_RINGBUF,
    .max_entries = 256 * 1024 /* 256 KB */,
};

static inline __u8 socket_check_network_policy(net_bear_type_map_value net_bear_mark_type,
    net_bear_type_map_value *net_bear_type, uid_access_policy_value *value)
{
    if (((net_bear_mark_type == NETWORK_BEARER_TYPE_WIFI) && (!value->wifiPolicy)) ||
        ((net_bear_mark_type == NETWORK_BEARER_TYPE_CELLULAR) && (!value->cellularPolicy))) {
        return 0;
    } else if (net_bear_mark_type != NETWORK_BEARER_TYPE_INITIAL) {
        return 1;
    }
    if (((*net_bear_type == NETWORK_BEARER_TYPE_WIFI) && (!value->wifiPolicy)) ||
        ((*net_bear_type == NETWORK_BEARER_TYPE_CELLULAR) && (!value->cellularPolicy))) {
        return 0;
    }
    return 1;
}

static inline __u8 socket_ringbuf_event_submit(__u32 uid)
{
    uint32_t *e;
    e = bpf_ringbuf_reserve(&ringbuf_map, sizeof(*e), 0);
    if (e) {
        *e = uid;
        bpf_ringbuf_submit(e, 0);
        return 1;
    }
    return 0;
}

SEC("cgroup_addr/bind4")
static int inet_check_bind4(struct bpf_sock_addr *ctx)
{
    void *map_ptr = &app_uid_access_policy_map;
    __u64 uid_gid = bpf_get_current_uid_gid();
    __u32 uid = (__u32)(uid_gid & 0x00000000FFFFFFFF);
    if (bpf_get_netns_cookie(ctx) != bpf_get_netns_cookie(NULL)) {
        uid = check_broker_policy(uid);
    }

    uid_access_policy_value *value = bpf_map_lookup_elem(map_ptr, &uid);
    // value == NULL means that the process attached to this uid is not a hap process which has a default configuration
    if (value == NULL) {
        return 1;
    }

    void *net_bear_map_ptr = &net_bear_type_map;
    net_bear_id_key net_bear_id = DEFAULT_NETWORK_BEARER_MAP_KEY;
    net_bear_type_map_value *net_bear_type = bpf_map_lookup_elem(net_bear_map_ptr, &net_bear_id);
    if (net_bear_type == NULL) {
        return 1;
    }

    struct bpf_sock *sk = ctx->sk;
    net_bear_type_map_value net_bear_mark_type = check_socket_select_net(sk);
    if (socket_check_network_policy(net_bear_mark_type, net_bear_type, value) == 0) {
        if (value->diagAckFlag) {
            return 0;
        }

        // the policy configuration needs to be reconfirmed or the network bearer changes
        if (value->configSetFromFlag) {
            if (socket_ringbuf_event_submit(uid) != 0) {
                value->diagAckFlag = 1;
            }
        }
        value->netIfIndex = *net_bear_type;
        bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
        return 0;
    }

    value->netIfIndex = *net_bear_type;
    bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
    return 1;
}

SEC("cgroup_addr/bind6")
static int inet_check_bind6(struct bpf_sock_addr *ctx)
{
    void *map_ptr = &app_uid_access_policy_map;
    __u64 uid_gid = bpf_get_current_uid_gid();
    __u32 uid = (__u32)(uid_gid & 0x00000000FFFFFFFF);
    if (bpf_get_netns_cookie(ctx) != bpf_get_netns_cookie(NULL)) {
        uid = check_broker_policy(uid);
    }

    uid_access_policy_value *value = bpf_map_lookup_elem(map_ptr, &uid);
    // value == NULL means that the process attached to this uid is not a hap process which has a default configuration
    if (value == NULL) {
        return 1;
    }

    void *net_bear_map_ptr = &net_bear_type_map;
    net_bear_id_key net_bear_id = DEFAULT_NETWORK_BEARER_MAP_KEY;
    net_bear_type_map_value *net_bear_type = bpf_map_lookup_elem(net_bear_map_ptr, &net_bear_id);
    if (net_bear_type == NULL) {
        return 1;
    }

    struct bpf_sock *sk = ctx->sk;
    net_bear_type_map_value net_bear_mark_type = check_socket_select_net(sk);
    if (socket_check_network_policy(net_bear_mark_type, net_bear_type, value) == 0) {
        if (value->diagAckFlag) {
            return 0;
        }

        // the policy configuration needs to be reconfirmed or the network bearer changes
        if (value->configSetFromFlag) {
            if (socket_ringbuf_event_submit(uid) != 0) {
                value->diagAckFlag = 1;
            }
        }
        value->netIfIndex = *net_bear_type;
        bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
        return 0;
    }

    value->netIfIndex = *net_bear_type;
    bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
    return 1;
}

SEC("cgroup_addr/connect4")
static int inet_check_connect4(struct bpf_sock_addr *ctx)
{
    void *map_ptr = &app_uid_access_policy_map;
    __u64 uid_gid = bpf_get_current_uid_gid();
    __u32 uid = (__u32)(uid_gid & 0x00000000FFFFFFFF);
    if (bpf_get_netns_cookie(ctx) != bpf_get_netns_cookie(NULL)) {
        uid = check_broker_policy(uid);
    }

    uid_access_policy_value *value = bpf_map_lookup_elem(map_ptr, &uid);
    // value == NULL means that the process attached to this uid is not a hap process which has a default configuration
    if (value == NULL) {
        return 1;
    }

    void *net_bear_map_ptr = &net_bear_type_map;
    net_bear_id_key net_bear_id = DEFAULT_NETWORK_BEARER_MAP_KEY;
    net_bear_type_map_value *net_bear_type = bpf_map_lookup_elem(net_bear_map_ptr, &net_bear_id);
    if (net_bear_type == NULL) {
        return 1;
    }

    struct bpf_sock *sk = ctx->sk;
    net_bear_type_map_value net_bear_mark_type = check_socket_select_net(sk);
    if (socket_check_network_policy(net_bear_mark_type, net_bear_type, value) == 0) {
        if (value->diagAckFlag) {
            return 0;
        }

        // the policy configuration needs to be reconfirmed or the network bearer changes
        if (value->configSetFromFlag) {
            if (socket_ringbuf_event_submit(uid) != 0) {
                value->diagAckFlag = 1;
            }
        }
        value->netIfIndex = *net_bear_type;
        bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
        return 0;
    }

    value->netIfIndex = *net_bear_type;
    bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
    return 1;
}


SEC("cgroup_addr/connect6")
static int inet_check_connect6(struct bpf_sock_addr *ctx)
{
    void *map_ptr = &app_uid_access_policy_map;
    __u64 uid_gid = bpf_get_current_uid_gid();
    __u32 uid = (__u32)(uid_gid & 0x00000000FFFFFFFF);
    if (bpf_get_netns_cookie(ctx) != bpf_get_netns_cookie(NULL)) {
        uid = check_broker_policy(uid);
    }

    uid_access_policy_value *value = bpf_map_lookup_elem(map_ptr, &uid);
    // value == NULL means that the process attached to this uid is not a hap process which has a default configuration
    if (value == NULL) {
        return 1;
    }

    void *net_bear_map_ptr = &net_bear_type_map;
    net_bear_id_key net_bear_id = DEFAULT_NETWORK_BEARER_MAP_KEY;
    net_bear_type_map_value *net_bear_type = bpf_map_lookup_elem(net_bear_map_ptr, &net_bear_id);
    if (net_bear_type == NULL) {
        return 1;
    }

    struct bpf_sock *sk = ctx->sk;
    net_bear_type_map_value net_bear_mark_type = check_socket_select_net(sk);
    if (socket_check_network_policy(net_bear_mark_type, net_bear_type, value) == 0) {
        if (value->diagAckFlag) {
            return 0;
        }

        // the policy configuration needs to be reconfirmed or the network bearer changes
        if (value->configSetFromFlag) {
            if (socket_ringbuf_event_submit(uid) != 0) {
                value->diagAckFlag = 1;
            }
        }
        value->netIfIndex = *net_bear_type;
        bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
        return 0;
    }

    value->netIfIndex = *net_bear_type;
    bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
    return 1;
}

SEC("cgroup_addr/sendmsg4")
static int inet_check_sendmsg4(struct bpf_sock_addr *ctx)
{
    void *map_ptr = &app_uid_access_policy_map;
    __u64 uid_gid = bpf_get_current_uid_gid();
    __u32 uid = (__u32)(uid_gid & 0x00000000FFFFFFFF);
    if (bpf_get_netns_cookie(ctx) != bpf_get_netns_cookie(NULL)) {
        uid = check_broker_policy(uid);
    }

    uid_access_policy_value *value = bpf_map_lookup_elem(map_ptr, &uid);
    // value == NULL means that the process attached to this uid is not a hap process which has a default configuration
    if (value == NULL) {
        return 1;
    }

    void *net_bear_map_ptr = &net_bear_type_map;
    net_bear_id_key net_bear_id = DEFAULT_NETWORK_BEARER_MAP_KEY;
    net_bear_type_map_value *net_bear_type = bpf_map_lookup_elem(net_bear_map_ptr, &net_bear_id);
    if (net_bear_type == NULL) {
        return 1;
    }

    struct bpf_sock *sk = ctx->sk;
    net_bear_type_map_value net_bear_mark_type = check_socket_select_net(sk);
    if (socket_check_network_policy(net_bear_mark_type, net_bear_type, value) == 0) {
        if (value->diagAckFlag) {
            return 0;
        }

        // the policy configuration needs to be reconfirmed or the network bearer changes
        if (value->configSetFromFlag) {
            if (socket_ringbuf_event_submit(uid) != 0) {
                value->diagAckFlag = 1;
            }
        }
        value->netIfIndex = *net_bear_type;
        bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
        return 0;
    }

    value->netIfIndex = *net_bear_type;
    bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
    return 1;
}

SEC("cgroup_addr/sendmsg6")
static int inet_check_sendmsg6(struct bpf_sock_addr *ctx)
{
    void *map_ptr = &app_uid_access_policy_map;
    __u64 uid_gid = bpf_get_current_uid_gid();
    __u32 uid = (__u32)(uid_gid & 0x00000000FFFFFFFF);
    if (bpf_get_netns_cookie(ctx) != bpf_get_netns_cookie(NULL)) {
        uid = check_broker_policy(uid);
    }

    uid_access_policy_value *value = bpf_map_lookup_elem(map_ptr, &uid);
    // value == NULL means that the process attached to this uid is not a hap process which has a default configuration
    if (value == NULL) {
        return 1;
    }

    void *net_bear_map_ptr = &net_bear_type_map;
    net_bear_id_key net_bear_id = DEFAULT_NETWORK_BEARER_MAP_KEY;
    net_bear_type_map_value *net_bear_type = bpf_map_lookup_elem(net_bear_map_ptr, &net_bear_id);
    if (net_bear_type == NULL) {
        return 1;
    }

    struct bpf_sock *sk = ctx->sk;
    net_bear_type_map_value net_bear_mark_type = check_socket_select_net(sk);
    if (socket_check_network_policy(net_bear_mark_type, net_bear_type, value) == 0) {
        if (value->diagAckFlag) {
            return 0;
        }

        // the policy configuration needs to be reconfirmed or the network bearer changes
        if (value->configSetFromFlag) {
            if (socket_ringbuf_event_submit(uid) != 0) {
                value->diagAckFlag = 1;
            }
        }
        value->netIfIndex = *net_bear_type;
        bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
        return 0;
    }

    value->netIfIndex = *net_bear_type;
    bpf_map_update_elem(map_ptr, &uid, value, BPF_NOEXIST);
    return 1;
}

char g_license[] SEC("license") = "GPL";

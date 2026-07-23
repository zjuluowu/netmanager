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
#ifndef NET_FIREWALL_H
#define NET_FIREWALL_H

#include <linux/bpf.h>

#include "netfirewall_ct.h"
#include "netfirewall_def.h"
#include "netfirewall_domain.h"
#include "netfirewall_event.h"
#include "netfirewall_match.h"

#define FIREWALL_DNS_QUERY_PORT         53
#define FIREWALL_DNS_OVER_QUERY_PORT    853

/**
 * @brief if tcp socket was intercepted, need send reset packet to peer
 *
 * @param tuple match tuple of skb meta data
 * @param skb struct __sk_buff
 * @param dir enum stream_dir
 * @return 0 if no error, -1 if an error occurred
 */
static __always_inline int send_sock_tcp_reset(struct match_tuple *tuple, struct __sk_buff *skb, enum stream_dir dir)
{
    if (!skb || !tuple) {
        return -1;
    }
    if (tuple->protocol == IPPROTO_TCP) {
        if (dir == INGRESS) {
            bpf_sock_tcp_send_reset(skb);
        } else if (dir == EGRESS) {
            bpf_skb_sock_destroy(skb);
        }
        return 0;
    }
    return -1;
}

/**
 * @brief Get the packet rst on tuple
 *
 * @param tuple struct match_tuple
 * @return true if success or false if an error occurred
 */
static __always_inline bool get_packet_rst_flag(struct match_tuple *tuple)
{
    if (!tuple) {
        return false;
    }

    if (tuple->rst == 1) {
        return true;
    }

    return false;
}

/**
 * @brief Get the ct tuple from match tuple
 *
 * @param match_tpl struct match_tuple
 * @param ct_tpl struct ct_tuple
 * @return true if success or false if an error occurred
 */
static __always_inline bool get_ct_tuple(struct match_tuple *match_tpl, struct ct_tuple *ct_tpl)
{
    if (!match_tpl || !ct_tpl) {
        return false;
    }

    ct_tpl->uid = match_tpl->uid;
    ct_tpl->family = match_tpl->family;
    ct_tpl->protocol = match_tpl->protocol;
    ct_tpl->sport = match_tpl->sport;
    ct_tpl->dport = match_tpl->dport;

    if (match_tpl->family == AF_INET) {
        ct_tpl->ipv4.saddr = match_tpl->ipv4.saddr;
        ct_tpl->ipv4.daddr = match_tpl->ipv4.daddr;
    } else {
        ct_tpl->ipv6.saddr = match_tpl->ipv6.saddr;
        ct_tpl->ipv6.daddr = match_tpl->ipv6.daddr;
    }

    return true;
}

/**
 * @brief Determine ingress packet drop or not
 *
 * @param skb struct __sk_buff
 * @return SK_DROP if intercepted or SK_PASS if not
 */
static __always_inline enum sk_action netfirewall_policy_ingress(struct __sk_buff *skb)
{
    if (match_dns_query(skb) == SK_DROP) {
        struct match_tuple tuple = { 0 };
        if (!get_match_tuple(skb, &tuple, INGRESS)) {
            return SK_DROP;
        }
        log_intercept_event(&tuple, skb);
        return SK_DROP;
    }

    struct match_tuple tuple = { 0 };
    if (!get_match_tuple(skb, &tuple, INGRESS)) {
        return SK_PASS;
    }

    log_tuple(&tuple);

    struct ct_tuple ct_tpl = {};
    if (!get_ct_tuple(&tuple, &ct_tpl)) {
        return SK_PASS;
    }

    enum ct_status status = ct_map_lookup_entry(skb, &ct_tpl, CT_INGRESS, match_loopback(tuple));
    log_dbg(DBG_CT_LOOKUP, INGRESS, status);
    if (status != CT_NEW) {
        return SK_PASS;
    }

    if (get_packet_rst_flag(&tuple)) {
        return SK_PASS;
    }

    struct bitmap key = { 0 };
    if (!match_action_key(skb, &tuple, &key)) {
        return SK_PASS;
    }

    if (match_action(&tuple, &key) != SK_PASS) {
        log_intercept(&tuple);
        send_sock_tcp_reset(&tuple, skb, INGRESS);
        return SK_DROP;
    }

    if (status == CT_NEW) {
        ct_create_entry(&ct_tpl, skb, CT_INGRESS);
    }

    return SK_PASS;
}

static __always_inline bool MatchDnsQuery(const struct match_tuple *tuple)
{
    __be16 port = bpf_htons(tuple->sport);
    if (port == FIREWALL_DNS_QUERY_PORT || port == FIREWALL_DNS_OVER_QUERY_PORT) {
        struct defalut_action_value *default_value = bpf_map_lookup_elem(&DEFAULT_ACTION_MAP, &tuple->uid);
        return default_value && default_value->outaction != SK_PASS;
    }
    return false;
}

/**
 * @brief Determine egress packet drop or not
 *
 * @param skb struct __sk_buff
 * @return SK_DROP if intercepted or SK_PASS if not
 */
static __always_inline enum sk_action netfirewall_policy_egress(struct __sk_buff *skb)
{
    if (match_dns_query(skb) == SK_DROP) {
        struct match_tuple tuple = { 0 };
        if (!get_match_tuple(skb, &tuple, EGRESS)) {
            return SK_DROP;
        }
        log_intercept_event(&tuple, skb);
        return SK_DROP;
    }

    struct match_tuple tuple = { 0 };
    if (!get_match_tuple(skb, &tuple, EGRESS)) {
        return SK_PASS;
    }

    log_tuple(&tuple);

    if (get_packet_rst_flag(&tuple)) {
        return SK_PASS;
    }

    struct ct_tuple ct_tpl = {};
    if (!get_ct_tuple(&tuple, &ct_tpl)) {
        return SK_PASS;
    }

    enum ct_status status = ct_map_lookup_entry(skb, &ct_tpl, CT_EGRESS, match_loopback(tuple));
    log_dbg(DBG_CT_LOOKUP, EGRESS, status);
    if (status != CT_NEW) {
        return SK_PASS;
    }

    if (get_packet_rst_flag(&tuple)) {
        return SK_PASS;
    }

    struct bitmap key = { 0 };
    if (!match_action_key(skb, &tuple, &key)) {
        return SK_PASS;
    }
    // Outbound DNS queries need to be released
    if (!MatchDnsQuery(&tuple) && match_action(&tuple, &key) != SK_PASS) {
        log_intercept(&tuple);
        send_sock_tcp_reset(&tuple, skb, EGRESS);
        return SK_DROP;
    }

    if (status == CT_NEW) {
        ct_create_entry(&ct_tpl, skb, CT_EGRESS);
    }

    return SK_PASS;
}

#endif // NET_FIREWALL_H
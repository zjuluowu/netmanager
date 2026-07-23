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
#ifndef NET_FIREWALL_CT_DEF_H
#define NET_FIREWALL_CT_DEF_H

#include <linux/types.h>

#define TCP_CONN_TIMEOUT_SEC 21600
#define NONTCP_CONN_TIMEOUT_SEC 60
#define TCP_SYN_TIMEOUT_SEC 60
#define CONN_COLSE_TIMEOUT_SEC 10
#define REPORT_INTERVAL_SEC 5
#define REPORT_FLAGS 0xff

#define NS_PER_SEC (1000ULL * 1000ULL * 1000UL)

enum ct_action {
    CT_ACTION_UNSPEC,
    CT_ACTION_CREATE,
    CT_ACTION_CLOSE,
};

enum ct_dir {
    CT_EGRESS,
    CT_INGRESS,
};

enum ct_status {
    CT_NEW,
    CT_ESTABLISHED,
    CT_REOPENED,
    CT_RELATED,
};

struct ct_tuple {
    __u32 uid;
    __u32 family;
    __u8 protocol;
    union {
        struct {
            __be32 saddr;
            __be32 daddr;
        } ipv4;
        struct {
            struct in6_addr saddr;
            struct in6_addr daddr;
        } ipv6;
    };

    __be16 sport;
    __be16 dport;
};

struct ct_entry {
    __u32 lifetime;

    // clang-format off
    __u8 rx_closing_flag : 1,
         tx_closing_flag : 1,
         seen_non_syn : 1,
         reserved : 5;
    // clang-format on

    __u8 tx_seen_flag;
    __u8 rx_seen_flag;

    __u32 last_tx_report;
    __u32 last_rx_report;
};

#endif // NET_FIREWALL_CT_DEF_H
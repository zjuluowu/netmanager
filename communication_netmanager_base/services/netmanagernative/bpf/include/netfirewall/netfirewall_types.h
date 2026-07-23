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
#ifndef NET_FIREWALL_TYPES_H
#define NET_FIREWALL_TYPES_H

#include <linux/types.h>
#include <linux/bpf.h>
#include <sys/socket.h>

#ifdef __cplusplus
#include <netinet/in.h>
#else
#include <linux/in6.h>
#endif

#define USER_ID_DIVIDOR 200000
#define DEFAULT_USER_ID 100
#define BITMAP_LEN 63
#define IPV4_MAX_PREFIXLEN 32
#define IPV6_MAX_PREFIXLEN 128
#define PORT_MAX_MASK 16
#define DNS_PORT 53
#define DNS_QR_DEFALUT_MASK 15
#define DNS_QRS_IPV4_TYPE 1
#define DNS_QRS_IPV4_LEN 4
#define DNS_QRS_IPV6_TYPE 28
#define DNS_QRS_IPV6_LEN 16
#define DNS_DOMAIN_LEN_MIN 1
#define BITS_PER_BYTE 8
#define DNS_DOMAIN_LEN 128
#define DNS_ANSWER_CNT 32
#define PROTOCOL_SAT_EXPAK 64
#define INTERFACE_NAME_MAX_LEN 16

struct bitmap {
    __u32 val[BITMAP_LEN];
};

typedef __u32 *bitmap_ptr;
typedef __u32 bitmap_t[BITMAP_LEN];
#define BITMAP_BITS (BITMAP_LEN * 32)

enum stream_dir {
    INVALID = -1,
    INGRESS = 1,
    EGRESS,
};

enum event_type {
    EVENT_INTERCEPT = 1,
    EVENT_DEBUG,
    EVENT_TUPLE_DEBUG,
};

enum debug_type {
    DBG_GENERIC, /* Generic, no message, useful to dump random integers */
    DBG_MATCH_SPORT,
    DBG_MATCH_DPORT,
    DBG_MATCH_PROTO,
    DBG_MATCH_APPUID,
    DBG_MATCH_UID,
    DBG_ACTION_KEY,
    DBG_MATCH_ACTION,
    DBG_CT_LOOKUP,
    DBG_MATCH_DOMAIN,
    DBG_MATCH_DOMAIN_ACTION,
    DBG_MATCH_INTERFACE,
};

struct domain_hash_key {
    __u32 prefixlen;
    __u32 uid;
    __u32 appuid;
    __u8 data[DNS_DOMAIN_LEN];
};

struct defalut_action_value {
    enum sk_action inaction;
    enum sk_action outaction;
};

struct domain_value {
    __u32 appuid;
    __u32 uid;
};

struct debug_event {
    enum debug_type type;
    enum stream_dir dir;
    __u32 arg1;
    __u32 arg2;
    __u32 arg3;
    __u32 arg4;
    __u32 arg5;
};

struct intercept_event {
    enum stream_dir dir;
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
    __u32 appuid;
    struct domain_hash_key domainData;
};

struct match_tuple {
    enum stream_dir dir;
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
    __u32 appuid;
    __u32 uid;
    __u16 rst;
    __u32 ifindex;
};

struct event {
    enum event_type type;
    union {
        struct debug_event debug;
        struct intercept_event intercept;
        struct match_tuple tuple;
    };
    __u32 len;
};

typedef __u8 loop_back_val;
typedef __be32 ip4_key;
typedef struct in6_addr ip6_key;
typedef __u8 action_key;
typedef struct bitmap action_val;
typedef __be16 port_key_val;
typedef __u8 proto_key;
typedef __u32 appuid_key;
typedef __u32 uid_key;
typedef struct {
    char name[INTERFACE_NAME_MAX_LEN];
} interface_key;

typedef enum {
    CURRENT_USER_ID_KEY = 1,
} current_user_id_key;

typedef enum {
    DEFAULT_ACT_IN_KEY = 1,
    DEFAULT_ACT_OUT_KEY = 2,
} default_action_key;

struct ipv4_lpm_key {
        __u32 prefixlen;
        ip4_key data;
};

struct ipv6_lpm_key {
        __u32 prefixlen;
        ip6_key data;
};

struct port_key {
        __u32 prefixlen;
        __be16 data;
};

struct dnshdr {
    __be16 id;
    __be16 flag;
    __be16 qdcount;
    __be16 ancount;
    __be16 nscount;
    __be16 arcount;
};

#endif // NET_FIREWALL_TYPES_H

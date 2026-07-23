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
#ifndef NET_FIREWALL_EVENT_H
#define NET_FIREWALL_EVENT_H

#include "netfirewall_types.h"
#include "netfirewall_event_map.h"

#include "netfirewall_event.h"
#include "netfirewall_map.h"
#include "netfirewall_utils.h"
#include "netfirewall_domain.h"

/**
 * @brief output event to ring buffer for user polling
 *
 * @param type enum event_type
 * @param data event data
 * @param len  event data len
 */
static __always_inline void output_to_user(enum event_type type, void *data, __u32 len)
{
    if (data && len > 0) {
        struct event *e = bpf_ringbuf_reserve(&EVENT_MAP, sizeof(struct event), 0);
        if (!e) {
            return;
        }

        e->type = type;
        e->len = len;

        switch (type) {
            case EVENT_DEBUG:
                memcpy(&(e->debug), data, len);
                break;
            case EVENT_INTERCEPT:
                memcpy(&(e->intercept), data, len);
                break;
            case EVENT_TUPLE_DEBUG:
                memcpy(&(e->tuple), data, len);
                break;
            default:
                break;
        }

        bpf_ringbuf_submit(e, 0);
    }
}

/**
 * @brief send intercept info to ring buffer for user polling
 *
 * @param tuple struct match_tuple
 */
static __always_inline void log_intercept(struct match_tuple *tuple)
{
    if (!tuple) {
        return;
    }

    struct intercept_event ev = {
        .dir = tuple->dir,
        .family = tuple->family,
        .protocol = tuple->protocol,
        .sport = tuple->sport,
        .dport = tuple->dport,
        .appuid = tuple->appuid,
    };

    if (AF_INET == tuple->family) {
        ev.ipv4.saddr = tuple->ipv4.saddr;
        ev.ipv4.daddr = tuple->ipv4.daddr;
    } else if (AF_INET6 == tuple->family) {
        ev.ipv6.saddr = tuple->ipv6.saddr;
        ev.ipv6.daddr = tuple->ipv6.daddr;
    }
    output_to_user(EVENT_INTERCEPT, &ev, sizeof(struct intercept_event));
}

static __always_inline void log_intercept_event(struct match_tuple *tuple, struct __sk_buff *skb)
{
    if (!tuple || !skb) {
        return;
    }

    __u64 skbAddr = (__u64)(unsigned long)skb;
    struct domain_hash_key *domainData = bpf_map_lookup_elem(&DOMAIN_DATA_KEY_MAP, &skbAddr);

    struct event *e = bpf_ringbuf_reserve(&EVENT_MAP, sizeof(struct event), 0);
    if (!e) {
        return;
    }
    e->type = EVENT_INTERCEPT;
    e->len = sizeof(struct intercept_event);
    e->intercept.dir = tuple->dir;
    e->intercept.family = tuple->family;
    e->intercept.protocol = tuple->protocol;
    e->intercept.sport = tuple->sport;
    e->intercept.dport = tuple->dport;
    e->intercept.appuid = tuple->appuid;
    if (domainData) {
        memcpy(&(e->intercept.domainData), domainData, sizeof(struct domain_hash_key));
    } else {
        e->intercept.domainData.prefixlen = 0;
        memset(&(e->intercept.domainData.data), 0, sizeof(e->intercept.domainData.data));
    }
    if (AF_INET == tuple->family) {
        e->intercept.ipv4.saddr = tuple->ipv4.saddr;
        e->intercept.ipv4.daddr = tuple->ipv4.daddr;
    } else if (AF_INET6 == tuple->family) {
        e->intercept.ipv6.saddr = tuple->ipv6.saddr;
        e->intercept.ipv6.daddr = tuple->ipv6.daddr;
    }
    bpf_map_delete_elem(&DOMAIN_DATA_KEY_MAP, &skbAddr);
    bpf_ringbuf_submit(e, 0);
}

#if NET_FIREWALL_DEBUG_TUPLE
/**
 * @brief send match tuple to ring buffer for user polling
 *
 * @param tuple struct match_tuple
 */
static __always_inline void log_tuple(struct match_tuple *tuple)
{
    if (!tuple) {
        return;
    }

    output_to_user(EVENT_TUPLE_DEBUG, tuple, sizeof(struct match_tuple));
}

#else // NET_FIREWALL_DEBUG_TUPLE

#define log_tuple(tuple)

#endif // NET_FIREWALL_DEBUG_TUPLE

#if NET_FIREWALL_DEBUG
static __always_inline void log_dbg_any(enum debug_type type, __u32 arg1)
{
    struct debug_event ev = {
        .type = type,
        .arg1 = arg1,
    };

    output_to_user(EVENT_DEBUG, &ev, sizeof(struct debug_event));
}

static __always_inline void log_dbg(enum debug_type type, enum stream_dir dir, __u32 arg1)
{
    struct debug_event ev = {
        .type = type,
        .dir = dir,
        .arg1 = arg1,
    };

    output_to_user(EVENT_DEBUG, &ev, sizeof(struct debug_event));
}

static __always_inline void log_dbg2(enum debug_type type, enum stream_dir dir, __u32 arg1, __u32 arg2)
{
    struct debug_event ev = {
        .type = type,
        .dir = dir,
        .arg1 = arg1,
        .arg2 = arg2,
    };

    output_to_user(EVENT_DEBUG, &ev, sizeof(struct debug_event));
}

static __always_inline void log_dbg3(enum debug_type type, enum stream_dir dir, __u32 arg1, __u32 arg2, __u32 arg3)
{
    struct debug_event ev = {
        .type = type,
        .dir = dir,
        .arg1 = arg1,
        .arg2 = arg2,
        .arg3 = arg3,
    };

    output_to_user(EVENT_DEBUG, &ev, sizeof(struct debug_event));
}

static __always_inline void log_dbg4(enum debug_type type, enum stream_dir dir, __u32 arg1, __u32 arg2, __u32 arg3,
    __u32 arg4)
{
    struct debug_event ev = {
        .type = type,
        .dir = dir,
        .arg1 = arg1,
        .arg2 = arg2,
        .arg3 = arg3,
        .arg4 = arg4,
    };

    output_to_user(EVENT_DEBUG, &ev, sizeof(struct debug_event));
}

static __always_inline void log_dbg5(enum debug_type type, enum stream_dir dir, __u32 arg1, __u32 arg2, __u32 arg3,
    __u32 arg4, __u32 arg5)
{
    struct debug_event ev = {
        .type = type,
        .dir = dir,
        .arg1 = arg1,
        .arg2 = arg2,
        .arg3 = arg3,
        .arg4 = arg4,
        .arg5 = arg5,
    };

    output_to_user(EVENT_DEBUG, &ev, sizeof(struct debug_event));
}

#else // NET_FIREWALL_DEBUG

#define log_dbg_any(type, arg1)
#define log_dbg(type, dir, arg1)
#define log_dbg2(type, dir, arg1, arg2)
#define log_dbg3(type, dir, arg1, arg2, arg3)
#define log_dbg4(type, dir, arg1, arg2, arg3, arg4)
#define log_dbg5(type, dir, arg1, arg2, arg3, arg4, arg5)

#endif // NET_FIREWALL_DEBUG
#endif // NET_FIREWALL_EVENT_H
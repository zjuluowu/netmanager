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
#ifndef NET_FIREWALL_CT_MAP_H
#define NET_FIREWALL_CT_MAP_H

#include <linux/bpf.h>
#include "netfirewall_ct_def.h"
#include "netfirewall_map_def.h"

bpf_map_def SEC("maps") CT_MAP = {
    .type = BPF_MAP_TYPE_LRU_HASH,
    .key_size = sizeof(struct ct_tuple),
    .value_size = sizeof(struct ct_entry),
    .max_entries = MAP_MAX_ENTRIES,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

#endif // NET_FIREWALL_CT_MAP_H
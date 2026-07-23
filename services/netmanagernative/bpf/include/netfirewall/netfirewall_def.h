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

#ifndef NET_FIREWALL_DEF_H
#define NET_FIREWALL_DEF_H

#include "netfirewall_types.h"
#include "netfirewall_map_def.h"
#include "netfirewall_ct_def.h"

static const ip4_key OTHER_IP4_KEY = (ip4_key)0xffffffff;
static const port_key_val OTHER_PORT_KEY = (port_key_val)0xffffffff;
static const proto_key OTHER_PROTO_KEY = (proto_key)0xffffffff;
static const appuid_key OTHER_APPUID_KEY = (appuid_key)0xffffffff;
static const appuid_key OTHER_UID_KEY = (uid_key)0xffffffff;
static const interface_key OTHER_INTERFACE_KEY = {};

#endif // NET_FIREWALL_DEF_H
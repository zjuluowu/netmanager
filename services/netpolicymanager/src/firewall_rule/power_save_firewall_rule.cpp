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
 * WI   THOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "power_save_firewall_rule.h"

#include "net_policy_inner_define.h"
namespace OHOS {
namespace NetManagerStandard {
PowerSaveFirewallRule::PowerSaveFirewallRule() : FirewallRule(FIREWALL_CHAIN_POWER_SAVE) {}

PowerSaveFirewallRule::~PowerSaveFirewallRule() = default;
} // namespace NetManagerStandard
} // namespace OHOS
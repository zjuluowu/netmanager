// Copyright (C) 2026 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use ani_rs::ani;
use serde::Deserialize;

#[ani_rs::ani(path = "@ohos.net.policy.policy.NetAccessPolicyInner")]
#[derive(Clone, Debug)]
pub struct NetAccessPolicyInner {
    pub allow_wiFi: bool,
    pub allow_cellular: bool,
}

#[ani_rs::ani(path = "@ohos.net.policy.policy.UidPolicyChangeInfo")]
#[derive(Clone, Debug)]
pub struct NetUidPolicyInfo {
    pub uid: i32,
    pub policy: i32,
}

#[ani_rs::ani(path = "@ohos.net.policy.policy.UidRuleChangeInfo")]
#[derive(Clone, Debug)]
pub struct NetUidRuleInfo {
    pub uid: i32,
    pub rule: i32,
}

/// Typed input for setNetQuotaPolicies – deserialized from ETS by the ANI framework.
/// Nested structure matching the external API: networkMatchRule + quotaPolicy.
#[derive(Clone, Debug, Deserialize)]
#[allow(non_snake_case)]
pub struct NetworkMatchRuleInput {
    pub netType: i32,
    pub identity: String,
    pub simId: String,
}

#[derive(Clone, Debug, Deserialize)]
#[allow(non_snake_case)]
pub struct QuotaPolicyInput {
    pub periodDuration: String,
    pub warningBytes: i64,
    pub limitBytes: i64,
    pub metered: bool,
    pub limitAction: i32,
    #[serde(default)]
    pub lastWarningRemind: i64,
    #[serde(default)]
    pub lastLimitRemind: i64,
}

#[derive(Clone, Debug, Deserialize)]
#[allow(non_snake_case)]
pub struct NetQuotaPolicyInput {
    pub networkMatchRule: NetworkMatchRuleInput,
    pub quotaPolicy: QuotaPolicyInput,
}

/// Typed input for setNetworkAccessPolicy – deserialized from ETS by the ANI framework.
/// Field names follow the external API NetworkAccessPolicy interface.
#[derive(Clone, Debug, Deserialize)]
#[allow(non_snake_case)]
pub struct NetworkAccessPolicyInput {
    #[serde(default)]
    pub allowWiFi: bool,
    #[serde(default)]
    pub allowCellular: bool,
    #[serde(default)]
    pub alwaysAllowWiFi: bool,
    #[serde(default)]
    pub alwaysAllowCellular: bool,
}

/// Typed output for getNetworkAccessPolicy(uid) – serialized to ETS by the ANI framework.
#[ani_rs::ani(path = "@ohos.net.policy.policy.NetworkAccessPolicy")]
#[derive(Clone, Debug)]
pub struct NetworkAccessPolicyOutput {
    pub allowWiFi: bool,
    pub allowCellular: bool,
    pub alwaysAllowWiFi: bool,
    pub alwaysAllowCellular: bool,
}

/// Typed output item for getNetworkAccessPolicy() – one uid-policy pair.
#[ani_rs::ani(path = "@ohos.net.policy.policy.UidNetworkPolicyItem")]
#[derive(Clone, Debug)]
pub struct UidNetworkPolicyItem {
    pub uid: i32,
    pub allowWiFi: bool,
    pub allowCellular: bool,
    pub alwaysAllowWiFi: bool,
    pub alwaysAllowCellular: bool,
}

/// Typed output for getNetQuotaPolicies – serialized to ETS by the ANI framework.
/// Nested structure matching the external API: networkMatchRule + quotaPolicy.
#[ani_rs::ani(path = "@ohos.net.policy.policy.NetworkMatchRule")]
#[derive(Clone, Debug)]
pub struct NetworkMatchRuleOutput {
    pub netType: i32,
    pub identity: String,
    pub simId: String,
}

#[ani_rs::ani(path = "@ohos.net.policy.policy.QuotaPolicy")]
#[derive(Clone, Debug)]
pub struct QuotaPolicyOutput {
    pub periodDuration: String,
    pub warningBytes: i64,
    pub limitBytes: i64,
    pub metered: bool,
    pub limitAction: i32,
    pub lastWarningRemind: i64,
    pub lastLimitRemind: i64,
}

#[ani_rs::ani(path = "@ohos.net.policy.policy.NetQuotaPolicy")]
#[derive(Clone, Debug)]
pub struct NetQuotaPolicyOutput {
    pub networkMatchRule: NetworkMatchRuleOutput,
    pub quotaPolicy: QuotaPolicyOutput,
}


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

use crate::bridge;
use ffi::NetAccessPolicyInner;

// Re-export execute callbacks implemented in `register.rs` so cxxbridge's
// generated extern "Rust" references (which expect symbols in this module)
// can resolve them.
pub use crate::register::execute_net_quota_policy_changed;
pub use crate::register::execute_net_background_policy_changed;
pub use crate::register::execute_net_metered_ifaces_changed;
pub use crate::register::execute_net_uid_rule_changed;
pub use crate::register::execute_net_uid_policy_changed;

pub struct NetPolicyClient;

#[repr(transparent)]
pub struct AniEnv {
    pub inner: ani_rs::AniEnv<'static>,
}

#[repr(transparent)]
pub struct AniObject {
    pub inner: ani_rs::objects::AniObject<'static>,
}

impl NetPolicyClient {
    pub fn get_self_network_access_policy() -> Result<bridge::NetAccessPolicyInner, i32> {
        let mut ret = 0;
        let policy = ffi::GetSelfNetworkAccessPolicy(&mut ret);
        if ret != 0 {
            return Err(ret);
        }
        let result = policy.into();
        Ok(result)
    }

    pub fn register_policy_observer() -> Result<(), i32> {
        let res = ffi::RegisterNetQuotaPolicyChangeCallback();
        if res != 0 {
            return Err(res);
        }
        Ok(())
    }

    pub fn unregister_policy_observer() -> Result<(), i32> {
        let res = ffi::UnregisterNetQuotaPolicyChangeCallback();
        if res != 0 {
            return Err(res);
        }
        Ok(())
    }

    pub fn show_app_net_policy_settings(native_context: i64) -> Result<(), i32> {
        crate::policy_info!("wrapper::show_app_net_policy_settings enter, native_context={}", native_context);
        let ret = ffi::ShowAppNetPolicySettings(native_context);
        if ret != 0 {
            crate::policy_error!("wrapper::show_app_net_policy_settings failed, ret={}", ret);
            return Err(ret);
        }
        crate::policy_info!("wrapper::show_app_net_policy_settings success");
        Ok(())
    }
}

impl From<ffi::NetAccessPolicyInner> for bridge::NetAccessPolicyInner {
    fn from(policy: ffi::NetAccessPolicyInner) -> Self {
        bridge::NetAccessPolicyInner {
            allow_wiFi: policy.allowWiFi,
            allow_cellular: policy.allowCellular,
        }
    }
}

impl From<ffi::NetUidRuleInfo> for bridge::NetUidRuleInfo {
    fn from(info: ffi::NetUidRuleInfo) -> Self {
        bridge::NetUidRuleInfo { uid: info.uid, rule: info.rule }
    }
}

impl From<ffi::NetUidPolicyInfo> for bridge::NetUidPolicyInfo {
    fn from(info: ffi::NetUidPolicyInfo) -> Self {
        bridge::NetUidPolicyInfo { uid: info.uid, policy: info.policy }
    }
}

impl From<bridge::NetQuotaPolicyInput> for ffi::NetQuotaPolicyAni {
    fn from(input: bridge::NetQuotaPolicyInput) -> Self {
        ffi::NetQuotaPolicyAni {
            network_match_rule: ffi::NetworkMatchRuleAni {
                net_type: input.networkMatchRule.netType,
                identity: input.networkMatchRule.identity,
                sim_id: input.networkMatchRule.simId,
            },
            quota_policy: ffi::QuotaPolicyAni {
                period_duration: input.quotaPolicy.periodDuration,
                warning_bytes: input.quotaPolicy.warningBytes,
                limit_bytes: input.quotaPolicy.limitBytes,
                metered: input.quotaPolicy.metered,
                limit_action: input.quotaPolicy.limitAction,
                last_warning_remind: input.quotaPolicy.lastWarningRemind,
                last_limit_remind: input.quotaPolicy.lastLimitRemind,
            },
        }
    }
}

impl From<bridge::NetworkAccessPolicyInput> for ffi::NetworkAccessPolicyAni {
    fn from(input: bridge::NetworkAccessPolicyInput) -> Self {
        ffi::NetworkAccessPolicyAni {
            wifiAllow: input.allowWiFi,
            cellularAllow: input.allowCellular,
            wifiSwitchDisable: input.alwaysAllowWiFi,
            cellularSwitchDisable: input.alwaysAllowCellular,
        }
    }
}

impl From<ffi::NetworkAccessPolicyAni> for bridge::NetworkAccessPolicyOutput {
    fn from(policy: ffi::NetworkAccessPolicyAni) -> Self {
        bridge::NetworkAccessPolicyOutput {
            allowWiFi: policy.wifiAllow,
            allowCellular: policy.cellularAllow,
            alwaysAllowWiFi: policy.wifiSwitchDisable,
            alwaysAllowCellular: policy.cellularSwitchDisable,
        }
    }
}

impl From<ffi::UidNetworkPolicyAni> for bridge::UidNetworkPolicyItem {
    fn from(item: ffi::UidNetworkPolicyAni) -> Self {
        bridge::UidNetworkPolicyItem {
            uid: item.uid,
            allowWiFi: item.policy.wifiAllow,
            allowCellular: item.policy.cellularAllow,
            alwaysAllowWiFi: item.policy.wifiSwitchDisable,
            alwaysAllowCellular: item.policy.cellularSwitchDisable,
        }
    }
}

impl From<ffi::NetQuotaPolicyAni> for bridge::NetQuotaPolicyOutput {
    fn from(p: ffi::NetQuotaPolicyAni) -> Self {
        bridge::NetQuotaPolicyOutput {
            networkMatchRule: bridge::NetworkMatchRuleOutput {
                netType: p.network_match_rule.net_type,
                identity: p.network_match_rule.identity,
                simId: p.network_match_rule.sim_id,
            },
            quotaPolicy: bridge::QuotaPolicyOutput {
                periodDuration: p.quota_policy.period_duration,
                warningBytes: p.quota_policy.warning_bytes,
                limitBytes: p.quota_policy.limit_bytes,
                metered: p.quota_policy.metered,
                limitAction: p.quota_policy.limit_action,
                lastWarningRemind: p.quota_policy.last_warning_remind,
                lastLimitRemind: p.quota_policy.last_limit_remind,
            },
        }
    }
}

#[cxx::bridge(namespace = "OHOS::NetManagerAni")]
pub mod ffi {
    pub struct NetAccessPolicyInner {
        pub allowWiFi: bool,
        pub allowCellular: bool,
    }
    extern "Rust" {
        pub fn execute_net_quota_policy_changed(policies: Vec<NetQuotaPolicyAni>);
        pub fn execute_net_background_policy_changed(allow: bool);
        pub fn execute_net_metered_ifaces_changed(info: MeteredIfaces);
        pub fn execute_net_uid_rule_changed(info: NetUidRuleInfo);
        pub fn execute_net_uid_policy_changed(info: NetUidPolicyInfo);
    }

    pub struct NetUidRuleInfo {
        pub uid: i32,
        pub rule: i32,
    }

    pub struct NetUidPolicyInfo {
        pub uid: i32,
        pub policy: i32,
    }

    pub struct MeteredIfaces {
        pub ifaces: Vec<String>,
    }

    // Typed structs mirroring native NetworkAccessPolicy and quota policy
    pub struct NetworkAccessPolicyAni {
        pub wifiAllow: bool,
        pub cellularAllow: bool,
        pub wifiSwitchDisable: bool,
        pub cellularSwitchDisable: bool,
    }

    pub struct UidNetworkPolicyAni {
        pub uid: i32,
        pub policy: NetworkAccessPolicyAni,
    }

    pub struct AccessPolicySaveAni {
        pub policy: NetworkAccessPolicyAni,
        pub uidPolicies: Vec<UidNetworkPolicyAni>,
    }

    pub struct NetworkMatchRuleAni {
        pub net_type: i32,
        pub identity: String,
        pub sim_id: String,
    }

    pub struct QuotaPolicyAni {
        pub period_duration: String,
        pub warning_bytes: i64,
        pub limit_bytes: i64,
        pub metered: bool,
        pub limit_action: i32,
        pub last_warning_remind: i64,
        pub last_limit_remind: i64,
    }

    pub struct NetQuotaPolicyAni {
        pub network_match_rule: NetworkMatchRuleAni,
        pub quota_policy: QuotaPolicyAni,
    }

    unsafe extern "C++" {
        include!("policy_ani.h");

        fn GetSelfNetworkAccessPolicy(ret: &mut i32) -> NetAccessPolicyInner;
        fn GetErrorCodeAndMessage(error_code: &mut i32) -> String;
        fn RegisterNetQuotaPolicyChangeCallback() -> i32;
        fn UnregisterNetQuotaPolicyChangeCallback() -> i32;
        fn ShowAppNetPolicySettings(context: i64) -> i32;
        fn SetBackgroundAllowed(allowed: bool) -> i32;
        fn RestoreAllPolicies(iccid: String) -> i32;
        fn SetPowerTrustlist(uids: Vec<u32>, isAllowed: bool) -> i32;
        fn UpdateRemindPolicy(netType: i32, iccid: String, remindType: u32) -> i32;
        fn GetPolicyByUid(uid: u32, policy: &mut u32) -> i32;
        fn GetBackgroundPolicy(backgroundPolicy: &mut bool) -> i32;
        fn GetPowerSaveTrustlist(result: &mut Vec<u32>) -> i32;
        fn GetDeviceIdleTrustlist(result: &mut Vec<u32>) -> i32;
        fn IsUidNetAllowed(uid: u32, metered: bool, isAllowed: &mut bool) -> i32;
        fn IsUidNetAllowedByIface(uid: u32, iface: String, isAllowed: &mut bool) -> i32;
        fn SetPolicyByUid(uid: u32, policy: u32) -> i32;
        fn GetBackgroundPolicyByUid(uid: u32, backgroundPolicyOfUid: &mut u32) -> i32;
        fn SetDeviceIdleTrustlist(uids: Vec<u32>, isAllowed: bool) -> i32;
        fn GetUidsByPolicy(policy: u32, result: &mut Vec<u32>) -> i32;
        fn GetNetworkAccessPolicyByUid(uid: u32, result: &mut NetworkAccessPolicyAni) -> i32;
        fn GetAllNetworkAccessPolicies(result: &mut Vec<UidNetworkPolicyAni>) -> i32;
        fn GetNetQuotaPoliciesTyped(result: &mut Vec<NetQuotaPolicyAni>) -> i32;
        fn SetNetworkAccessPolicyTyped(uid: u32, policy: NetworkAccessPolicyAni,
            reconfirmFlag: bool) -> i32;
        fn SetNetQuotaPolicies(policies: Vec<NetQuotaPolicyAni>) -> i32;
    }

    extern "Rust" {
        type AniEnv;

        type AniObject;
    }
}

impl NetPolicyClient {
    pub fn set_background_allowed(allowed: bool) -> Result<(), i32> {
        let res = ffi::SetBackgroundAllowed(allowed);
        if res != 0 { return Err(res); }
        Ok(())
    }

    pub fn restore_all_policies(iccid: &str) -> Result<(), i32> {
        let res = ffi::RestoreAllPolicies(iccid.to_owned());
        if res != 0 { return Err(res); }
        Ok(())
    }

    pub fn set_power_save_trustlist(uids: Vec<u32>, is_allowed: bool) -> Result<(), i32> {
        let res = ffi::SetPowerTrustlist(uids, is_allowed);
        if res != 0 { return Err(res); }
        Ok(())
    }

    pub fn update_remind_policy(net_type: i32, iccid: &str, remind_type: u32) -> Result<(), i32> {
        let res = ffi::UpdateRemindPolicy(net_type, iccid.to_owned(), remind_type);
        if res != 0 { return Err(res); }
        Ok(())
    }

    pub fn get_policy_by_uid(uid: u32) -> Result<u32, i32> {
        let mut out: u32 = 0;
        let res = ffi::GetPolicyByUid(uid, &mut out);
        if res != 0 { return Err(res); }
        Ok(out)
    }

    pub fn get_background_policy() -> Result<bool, i32> {
        let mut out: bool = false;
        let res = ffi::GetBackgroundPolicy(&mut out);
        if res != 0 { return Err(res); }
        Ok(out)
    }

    pub fn reset_policies(iccid: &str) -> Result<(), i32> {
        let res = ffi::RestoreAllPolicies(iccid.to_owned());
        if res != 0 { return Err(res); }
        Ok(())
    }

    pub fn get_power_save_trustlist() -> Result<Vec<u32>, i32> {
        let mut result = Vec::new();
        let res = ffi::GetPowerSaveTrustlist(&mut result);
        if res != 0 { return Err(res); }
        Ok(result)
    }

    pub fn get_device_idle_trustlist() -> Result<Vec<u32>, i32> {
        let mut result = Vec::new();
        let res = ffi::GetDeviceIdleTrustlist(&mut result);
        if res != 0 { return Err(res); }
        Ok(result)
    }

    pub fn is_uid_net_allowed(uid: u32, metered: bool) -> Result<bool, i32> {
        let mut out: bool = false;
        let res = ffi::IsUidNetAllowed(uid, metered, &mut out);
        if res != 0 { return Err(res); }
        Ok(out)
    }

    pub fn is_uid_net_allowed_by_iface(uid: u32, iface: &str) -> Result<bool, i32> {
        let mut out: bool = false;
        let res = ffi::IsUidNetAllowedByIface(uid, iface.to_owned(), &mut out);
        if res != 0 { return Err(res); }
        Ok(out)
    }

    pub fn set_policy_by_uid(uid: u32, policy: u32) -> Result<(), i32> {
        let res = ffi::SetPolicyByUid(uid, policy);
        if res != 0 { return Err(res); }
        Ok(())
    }

    pub fn get_background_policy_by_uid(uid: u32) -> Result<u32, i32> {
        let mut out: u32 = 0;
        let res = ffi::GetBackgroundPolicyByUid(uid, &mut out);
        if res != 0 { return Err(res); }
        Ok(out)
    }

    pub fn set_device_idle_trustlist(uids: Vec<u32>, is_allowed: bool) -> Result<(), i32> {
        let res = ffi::SetDeviceIdleTrustlist(uids, is_allowed);
        if res != 0 { return Err(res); }
        Ok(())
    }

    pub fn get_uids_by_policy(policy: u32) -> Result<Vec<u32>, i32> {
        let mut result = Vec::new();
        let res = ffi::GetUidsByPolicy(policy, &mut result);
        if res != 0 { return Err(res); }
        Ok(result)
    }

    pub fn get_network_access_policy(uid: u32) -> Result<bridge::NetworkAccessPolicyOutput, i32> {
        let mut policy = ffi::NetworkAccessPolicyAni {
            wifiAllow: false,
            cellularAllow: false,
            wifiSwitchDisable: false,
            cellularSwitchDisable: false,
        };
        let res = ffi::GetNetworkAccessPolicyByUid(uid, &mut policy);
        if res != 0 { return Err(res); }
        Ok(policy.into())
    }

    pub fn get_all_network_access_policies() -> Result<Vec<bridge::UidNetworkPolicyItem>, i32> {
        let mut items = Vec::new();
        let res = ffi::GetAllNetworkAccessPolicies(&mut items);
        if res != 0 { return Err(res); }
        Ok(items.into_iter().map(|item| item.into()).collect())
    }

    pub fn get_net_quota_policies() -> Result<Vec<bridge::NetQuotaPolicyOutput>, i32> {
        let mut policies = Vec::new();
        let res = ffi::GetNetQuotaPoliciesTyped(&mut policies);
        if res != 0 { return Err(res); }
        Ok(policies.into_iter().map(|p| p.into()).collect())
    }

    pub fn set_network_access_policy(uid: u32,
        policy: bridge::NetworkAccessPolicyInput, reconfirm_flag: bool) -> Result<(), i32> {
        let ani_policy: ffi::NetworkAccessPolicyAni = policy.into();
        let res = ffi::SetNetworkAccessPolicyTyped(uid, ani_policy, reconfirm_flag);
        if res != 0 { return Err(res); }
        Ok(())
    }

    pub fn set_net_quota_policies(policies: Vec<bridge::NetQuotaPolicyInput>) -> Result<(), i32> {
        let ani_policies: Vec<ffi::NetQuotaPolicyAni> =
            policies.into_iter().map(|p| p.into()).collect();
        let res = ffi::SetNetQuotaPolicies(ani_policies);
        if res != 0 { return Err(res); }
        Ok(())
    }
}

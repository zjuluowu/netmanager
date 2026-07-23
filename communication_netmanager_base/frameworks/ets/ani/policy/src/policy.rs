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

use crate::{bridge, error_code::convert_to_business_error, policy_error, policy_info, wrapper::NetPolicyClient};
use ani_rs::business_error::BusinessError;
use ani_rs::objects::AniObject;
use std::ffi::CStr;

/// Safe conversion: u32 -> i32, clamping to i32::MAX and logging on overflow.
/// Policy values and UIDs are well within i32 range under normal conditions,
/// but this guards against unexpected overflow.
fn to_i32_saturating(v: u32) -> i32 {
    match i32::try_from(v) {
        Ok(i) => i,
        Err(_) => {
            policy_error!("u32 to i32 overflow: {} > i32::MAX, clamping", v);
            i32::MAX
        }
    }
}

fn get_stage_mode_context(env: &ani_rs::AniEnv, context: &AniObject) -> Result<i64, BusinessError> {
    let native_context_str = CStr::from_bytes_with_nul(b"nativeContext\0")
        .expect("nativeContext\0 literal is always null-terminated");
    let native_context = env.get_field::<i64>(context, native_context_str)
        .map_err(|e| {
            policy_error!("get_stage_mode_context failed: {}", e.to_string());
            BusinessError::new(401,
                format!("Parameter error: failed to read nativeContext, {}", e.to_string()))
        })?;
    if native_context == 0 {
        policy_error!("get_stage_mode_context: nativeContext is null");
        return Err(BusinessError::new(401,
            "Parameter error: nativeContext is null".to_string()));
    }
    Ok(native_context)
}

#[ani_rs::native]
pub fn get_net_access_policy() -> Result<bridge::NetAccessPolicyInner, BusinessError> {
    let raw_result = NetPolicyClient::get_self_network_access_policy();
    raw_result
        .map(|v| { v })
        .map_err(|e| {
            convert_to_business_error(e)
        })
}

#[ani_rs::native]
pub fn show_app_net_policy_settings(env: &ani_rs::AniEnv, context: AniObject) -> Result<i32, BusinessError> {
    policy_info!("show_app_net_policy_settings enter");
    let native_context = get_stage_mode_context(env, &context)?;
    policy_info!("show_app_net_policy_settings native_context={}", native_context);
    let result = NetPolicyClient::show_app_net_policy_settings(native_context)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("show_app_net_policy_settings success"),
        Err(e) => policy_error!("show_app_net_policy_settings failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn set_background_allowed(is_allowed: bool) -> Result<i32, BusinessError> {
    policy_info!("set_background_allowed enter, is_allowed={}", is_allowed);
    let result = NetPolicyClient::set_background_allowed(is_allowed)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("set_background_allowed success"),
        Err(e) => policy_error!("set_background_allowed failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn restore_all_policies(iccid: String) -> Result<i32, BusinessError> {
    policy_info!("restore_all_policies enter");
    let result = NetPolicyClient::restore_all_policies(&iccid)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("restore_all_policies success"),
        Err(e) => policy_error!("restore_all_policies failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn set_power_save_trustlist(uids: Vec<i32>, is_allowed: bool) -> Result<i32, BusinessError> {
    policy_info!("set_power_save_trustlist enter, count={}, is_allowed={}", uids.len(), is_allowed);
    if uids.iter().any(|&v| v < 0) {
        return Err(BusinessError::new(401, ("Parameter error: uids must be non-negative").to_string()));
    }
    let uids_u32: Vec<u32> = uids.into_iter().map(|v| v as u32).collect();
    let result = NetPolicyClient::set_power_save_trustlist(uids_u32, is_allowed)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("set_power_save_trustlist success"),
        Err(e) => policy_error!("set_power_save_trustlist failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn update_remind_policy(net_type: i32, iccid: String, remind_type: i32) -> Result<i32, BusinessError> {
    policy_info!("update_remind_policy enter, net_type={}, remind_type={}", net_type, remind_type);
    if net_type < 0 {
        return Err(BusinessError::new(401, ("Parameter error: net_type must be non-negative").to_string()));
    }
    if remind_type < 0 {
        return Err(BusinessError::new(401, ("Parameter error: remind_type must be non-negative").to_string()));
    }
    let result = NetPolicyClient::update_remind_policy(net_type, &iccid, remind_type as u32)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("update_remind_policy success"),
        Err(e) => policy_error!("update_remind_policy failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn get_policy_by_uid(uid: i32) -> Result<i32, BusinessError> {
    policy_info!("get_policy_by_uid enter, uid={}", uid);
    if uid < 0 {
        return Err(BusinessError::new(401, ("Parameter error: uid must be non-negative").to_string()));
    }
    let result = NetPolicyClient::get_policy_by_uid(uid as u32)
        .map(|v| to_i32_saturating(v))
        .map_err(convert_to_business_error);
    match &result {
        Ok(v) => policy_info!("get_policy_by_uid success, policy={}", v),
        Err(e) => policy_error!("get_policy_by_uid failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn is_background_allowed() -> Result<bool, BusinessError> {
    policy_info!("is_background_allowed enter");
    let result = NetPolicyClient::get_background_policy()
        .map_err(convert_to_business_error);
    match &result {
        Ok(v) => policy_info!("is_background_allowed success, result={}", v),
        Err(e) => policy_error!("is_background_allowed failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn reset_policies(iccid: String) -> Result<i32, BusinessError> {
    policy_info!("reset_policies enter");
    let result = NetPolicyClient::reset_policies(&iccid)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("reset_policies success"),
        Err(e) => policy_error!("reset_policies failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn get_power_save_trustlist() -> Result<Vec<i32>, BusinessError> {
    policy_info!("get_power_save_trustlist enter");
    let result = NetPolicyClient::get_power_save_trustlist()
        .map(|v| v.into_iter().map(|x| to_i32_saturating(x)).collect::<Vec<_>>())
        .map_err(convert_to_business_error);
    match &result {
        Ok(v) => policy_info!("get_power_save_trustlist success, count={}", v.len()),
        Err(e) => policy_error!("get_power_save_trustlist failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn get_device_idle_trustlist() -> Result<Vec<i32>, BusinessError> {
    policy_info!("get_device_idle_trustlist enter");
    let result = NetPolicyClient::get_device_idle_trustlist()
        .map(|v| v.into_iter().map(|x| to_i32_saturating(x)).collect::<Vec<_>>())
        .map_err(convert_to_business_error);
    match &result {
        Ok(v) => policy_info!("get_device_idle_trustlist success, count={}", v.len()),
        Err(e) => policy_error!("get_device_idle_trustlist failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn is_uid_net_allowed(uid: i32, is_metered: bool) -> Result<bool, BusinessError> {
    policy_info!("is_uid_net_allowed enter, uid={}, is_metered={}", uid, is_metered);
    if uid < 0 {
        return Err(BusinessError::new(401, ("Parameter error: uid must be non-negative").to_string()));
    }
    let result = NetPolicyClient::is_uid_net_allowed(uid as u32, is_metered)
        .map_err(convert_to_business_error);
    match &result {
        Ok(v) => policy_info!("is_uid_net_allowed success, result={}", v),
        Err(e) => policy_error!("is_uid_net_allowed failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn is_uid_net_allowed_by_iface(uid: i32, iface: String) -> Result<bool, BusinessError> {
    policy_info!("is_uid_net_allowed_by_iface enter, uid={}, iface={}", uid, iface);
    if uid < 0 {
        return Err(BusinessError::new(401, ("Parameter error: uid must be non-negative").to_string()));
    }
    let result = NetPolicyClient::is_uid_net_allowed_by_iface(uid as u32, &iface)
        .map_err(convert_to_business_error);
    match &result {
        Ok(v) => policy_info!("is_uid_net_allowed_by_iface success, result={}", v),
        Err(e) => policy_error!("is_uid_net_allowed_by_iface failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn set_policy_by_uid(uid: i32, policy: i32) -> Result<i32, BusinessError> {
    policy_info!("set_policy_by_uid enter, uid={}, policy={}", uid, policy);
    if uid < 0 {
        return Err(BusinessError::new(401, ("Parameter error: uid must be non-negative").to_string()));
    }
    if policy < 0 {
        return Err(BusinessError::new(401, ("Parameter error: policy must be non-negative").to_string()));
    }
    let result = NetPolicyClient::set_policy_by_uid(uid as u32, policy as u32)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("set_policy_by_uid success"),
        Err(e) => policy_error!("set_policy_by_uid failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn get_background_policy_by_uid(uid: i32) -> Result<i32, BusinessError> {
    policy_info!("get_background_policy_by_uid enter, uid={}", uid);
    if uid < 0 {
        return Err(BusinessError::new(401, ("Parameter error: uid must be non-negative").to_string()));
    }
    let result = NetPolicyClient::get_background_policy_by_uid(uid as u32)
        .map(|v| to_i32_saturating(v))
        .map_err(convert_to_business_error);
    match &result {
        Ok(v) => policy_info!("get_background_policy_by_uid success, policy={}", v),
        Err(e) => policy_error!("get_background_policy_by_uid failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn set_device_idle_trustlist(uids: Vec<i32>, is_allowed: bool) -> Result<i32, BusinessError> {
    policy_info!("set_device_idle_trustlist enter, count={}, is_allowed={}", uids.len(), is_allowed);
    if uids.iter().any(|&v| v < 0) {
        return Err(BusinessError::new(401, ("Parameter error: uids must be non-negative").to_string()));
    }
    let uids_u32: Vec<u32> = uids.into_iter().map(|v| v as u32).collect();
    let result = NetPolicyClient::set_device_idle_trustlist(uids_u32, is_allowed)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("set_device_idle_trustlist success"),
        Err(e) => policy_error!("set_device_idle_trustlist failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn get_uids_by_policy(policy: i32) -> Result<Vec<i32>, BusinessError> {
    policy_info!("get_uids_by_policy enter, policy={}", policy);
    if policy < 0 {
        return Err(BusinessError::new(401, ("Parameter error: policy must be non-negative").to_string()));
    }
    let result = NetPolicyClient::get_uids_by_policy(policy as u32)
        .map(|v| v.into_iter().map(|x| to_i32_saturating(x)).collect::<Vec<_>>())
        .map_err(convert_to_business_error);
    match &result {
        Ok(v) => policy_info!("get_uids_by_policy success, count={}", v.len()),
        Err(e) => policy_error!("get_uids_by_policy failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn get_network_access_policy_by_uid(uid: i32) -> Result<bridge::NetworkAccessPolicyOutput, BusinessError> {
    policy_info!("get_network_access_policy_by_uid enter, uid={}", uid);
    if uid < 0 {
        return Err(BusinessError::new(401, ("Parameter error: uid must be non-negative").to_string()));
    }
    let result = NetPolicyClient::get_network_access_policy(uid as u32)
        .map_err(convert_to_business_error);
    match &result {
        Ok(ref v) => policy_info!("get_network_access_policy_by_uid success, wifiAllow={}, cellularAllow={}",
            v.allowWiFi, v.allowCellular),
        Err(e) => policy_error!("get_network_access_policy_by_uid failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn get_all_network_access_policies() -> Result<Vec<bridge::UidNetworkPolicyItem>, BusinessError> {
    policy_info!("get_all_network_access_policies enter");
    let result = NetPolicyClient::get_all_network_access_policies()
        .map_err(convert_to_business_error);
    match &result {
        Ok(ref v) => policy_info!("get_all_network_access_policies success, count={}", v.len()),
        Err(e) => policy_error!("get_all_network_access_policies failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn get_net_quota_policies() -> Result<Vec<bridge::NetQuotaPolicyOutput>, BusinessError> {
    policy_info!("get_net_quota_policies enter");
    let result = NetPolicyClient::get_net_quota_policies()
        .map_err(convert_to_business_error);
    match &result {
        Ok(ref v) => policy_info!("get_net_quota_policies success, count={}", v.len()),
        Err(e) => policy_error!("get_net_quota_policies failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn set_network_access_policy(uid: i32, policy: bridge::NetworkAccessPolicyInput,
    reconfirm_flag: bool) -> Result<i32, BusinessError> {
    policy_info!("set_network_access_policy enter, uid={}, allowWiFi={:?}, allowCellular={:?}, reconfirm={}",
        uid, policy.allowWiFi, policy.allowCellular, reconfirm_flag);
    if uid < 0 {
        return Err(BusinessError::new(401, ("Parameter error: uid must be non-negative").to_string()));
    }
    let result = NetPolicyClient::set_network_access_policy(uid as u32, policy, reconfirm_flag)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("set_network_access_policy success"),
        Err(e) => policy_error!("set_network_access_policy failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn set_net_quota_policies(policies: Vec<bridge::NetQuotaPolicyInput>) -> Result<i32, BusinessError> {
    policy_info!("set_net_quota_policies enter, count={}", policies.len());
    let result = NetPolicyClient::set_net_quota_policies(policies)
        .map(|_| 0)
        .map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("set_net_quota_policies success"),
        Err(e) => policy_error!("set_net_quota_policies failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

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

use std::{ops::Deref, sync::{Mutex, OnceLock}};

use ani_rs::{
    business_error::BusinessError,
    objects::{AniFnObject, GlobalRefCallback},
    AniEnv,
};

use crate::{
    bridge,
    error_code::convert_to_business_error,
    policy_error,
    policy_info,
    wrapper::{self, NetPolicyClient},
};

fn callback_error(e: impl std::fmt::Display) -> BusinessError {
    policy_error!("callback creation failed: {}", e);
    BusinessError::new(401, format!("Parameter error: {}", e))
}

struct Registar {
    inner: Mutex<Vec<CallbackFlavor>>,
}

impl Registar {
    fn new() -> Self {
        Self { inner: Mutex::new(Vec::new()) }
    }

    pub fn get_instance() -> &'static Self {
        static INSTANCE: OnceLock<Registar> = OnceLock::new();
        INSTANCE.get_or_init(Registar::new)
    }

    pub fn register(&self, callback: CallbackFlavor) -> Result<(), i32> {
        let mut inner = self.inner.lock().unwrap_or_else(|e| e.into_inner());
        let was_empty = inner.is_empty();
        if was_empty {
            NetPolicyClient::register_policy_observer()?;
        }

        inner.retain(|c| *c != callback);
        inner.push(callback);
        Ok(())
    }

    pub fn unregister<F>(&self, callback_ref: Option<CallbackFlavor>, is_same_variant: F) -> Result<(), i32>
    where
        F: Fn(&CallbackFlavor) -> bool,
    {
        let mut inner = self.inner.lock().unwrap_or_else(|e| e.into_inner());
        if let Some(callback) = callback_ref {
            inner.retain(|c| *c != callback);
        } else {
            inner.retain(|c| !is_same_variant(c));
        }

        if inner.is_empty() {
            NetPolicyClient::unregister_policy_observer()?;
        }

        Ok(())
    }

    pub fn on_net_quota_policy_changed(&self, policies: Vec<bridge::NetQuotaPolicyOutput>) {
        let inner = self.inner.lock().unwrap_or_else(|e| e.into_inner());
        for listen in inner.deref() {
            if let CallbackFlavor::NetQuotaPolicyChange(callback) = listen {
                callback.execute((policies.clone(),));
            }
        }
    }

    pub fn on_net_background_policy_changed(&self, allow: bool) {
        let inner = self.inner.lock().unwrap_or_else(|e| e.into_inner());
        for listen in inner.deref() {
            if let CallbackFlavor::NetBackgroundPolicyChange(callback) = listen {
                callback.execute((allow,));
            }
        }
    }

    pub fn on_net_metered_ifaces_changed(&self, info: Vec<String>) {
        let inner = self.inner.lock().unwrap_or_else(|e| e.into_inner());
        for listen in inner.deref() {
            if let CallbackFlavor::NetMeteredIfacesChange(callback) = listen {
                callback.execute((info.clone(),));
            }
        }
    }

    pub fn on_net_uid_rule_changed(&self, info: bridge::NetUidRuleInfo) {
        let inner = self.inner.lock().unwrap_or_else(|e| e.into_inner());
        for listen in inner.deref() {
            if let CallbackFlavor::NetUidRuleChange(callback) = listen {
                callback.execute((info.clone(),));
            }
        }
    }

    pub fn on_net_uid_policy_changed(&self, info: bridge::NetUidPolicyInfo) {
        let inner = self.inner.lock().unwrap_or_else(|e| e.into_inner());
        for listen in inner.deref() {
            if let CallbackFlavor::NetUidPolicyChange(callback) = listen {
                callback.execute((info.clone(),));
            }
        }
    }
}

#[ani_rs::native]
pub fn on_net_quota_policy_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("on_net_quota_policy_change enter");
    let callback = callback.into_global_callback(env)
        .map_err(callback_error)?;
    let flavor = CallbackFlavor::NetQuotaPolicyChange(callback);
    let result = Registar::get_instance().register(flavor).map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("on_net_quota_policy_change success"),
        Err(e) => policy_error!("on_net_quota_policy_change failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn off_net_quota_policy_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("off_net_quota_policy_change enter");
    let callback_flavor = if env.is_undefined(&callback)
        .map_err(callback_error)?
    {
        None
    } else {
        let callback_global = callback.into_global_callback(env)
            .map_err(callback_error)?;
        Some(CallbackFlavor::NetQuotaPolicyChange(callback_global))
    };

    Registar::get_instance()
        .unregister(callback_flavor, |c| matches!(c, CallbackFlavor::NetQuotaPolicyChange(_)))
        .map_err(convert_to_business_error)?;

    Ok(())
}

#[ani_rs::native]
pub fn on_net_background_policy_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("on_net_background_policy_change enter");
    let callback = callback.into_global_callback(env)
        .map_err(callback_error)?;
    let flavor = CallbackFlavor::NetBackgroundPolicyChange(callback);
    let result = Registar::get_instance().register(flavor).map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("on_net_background_policy_change success"),
        Err(e) => policy_error!("on_net_background_policy_change failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn off_net_background_policy_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("off_net_background_policy_change enter");
    let callback_flavor = if env.is_undefined(&callback)
        .map_err(callback_error)?
    {
        None
    } else {
        let callback_global = callback.into_global_callback(env)
            .map_err(callback_error)?;
        Some(CallbackFlavor::NetBackgroundPolicyChange(callback_global))
    };

    Registar::get_instance()
        .unregister(callback_flavor, |c| matches!(c, CallbackFlavor::NetBackgroundPolicyChange(_)))
        .map_err(convert_to_business_error)?;

    Ok(())
}

#[ani_rs::native]
pub fn on_net_metered_ifaces_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("on_net_metered_ifaces_change enter");
    let callback = callback.into_global_callback(env)
        .map_err(callback_error)?;
    let flavor = CallbackFlavor::NetMeteredIfacesChange(callback);
    let result = Registar::get_instance().register(flavor).map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("on_net_metered_ifaces_change success"),
        Err(e) => policy_error!("on_net_metered_ifaces_change failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn off_net_metered_ifaces_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("off_net_metered_ifaces_change enter");
    let callback_flavor = if env.is_undefined(&callback)
        .map_err(callback_error)?
    {
        None
    } else {
        let callback_global = callback.into_global_callback(env)
            .map_err(callback_error)?;
        Some(CallbackFlavor::NetMeteredIfacesChange(callback_global))
    };

    Registar::get_instance()
        .unregister(callback_flavor, |c| matches!(c, CallbackFlavor::NetMeteredIfacesChange(_)))
        .map_err(convert_to_business_error)?;

    Ok(())
}

#[ani_rs::native]
pub fn on_net_uid_rule_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("on_net_uid_rule_change enter");
    let callback = callback.into_global_callback(env)
        .map_err(callback_error)?;
    let flavor = CallbackFlavor::NetUidRuleChange(callback);
    let result = Registar::get_instance().register(flavor).map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("on_net_uid_rule_change success"),
        Err(e) => policy_error!("on_net_uid_rule_change failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn off_net_uid_rule_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("off_net_uid_rule_change enter");
    let callback_flavor = if env.is_undefined(&callback)
        .map_err(callback_error)?
    {
        None
    } else {
        let callback_global = callback.into_global_callback(env)
            .map_err(callback_error)?;
        Some(CallbackFlavor::NetUidRuleChange(callback_global))
    };

    Registar::get_instance()
        .unregister(callback_flavor, |c| matches!(c, CallbackFlavor::NetUidRuleChange(_)))
        .map_err(convert_to_business_error)?;

    Ok(())
}

#[ani_rs::native]
pub fn on_net_uid_policy_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("on_net_uid_policy_change enter");
    let callback = callback.into_global_callback(env)
        .map_err(callback_error)?;
    let flavor = CallbackFlavor::NetUidPolicyChange(callback);
    let result = Registar::get_instance().register(flavor).map_err(convert_to_business_error);
    match &result {
        Ok(_) => policy_info!("on_net_uid_policy_change success"),
        Err(e) => policy_error!("on_net_uid_policy_change failed, code={}, msg={}", e.code(), e.message()),
    }
    result
}

#[ani_rs::native]
pub fn off_net_uid_policy_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    policy_info!("off_net_uid_policy_change enter");
    let callback_flavor = if env.is_undefined(&callback)
        .map_err(callback_error)?
    {
        None
    } else {
        let callback_global = callback.into_global_callback(env)
            .map_err(callback_error)?;
        Some(CallbackFlavor::NetUidPolicyChange(callback_global))
    };

    Registar::get_instance()
        .unregister(callback_flavor, |c| matches!(c, CallbackFlavor::NetUidPolicyChange(_)))
        .map_err(convert_to_business_error)?;

    Ok(())
}

#[derive(PartialEq, Eq)]
pub enum CallbackFlavor {
    NetQuotaPolicyChange(GlobalRefCallback<(Vec<bridge::NetQuotaPolicyOutput>,)>),
    NetBackgroundPolicyChange(GlobalRefCallback<(bool,)>),
    NetMeteredIfacesChange(GlobalRefCallback<(Vec<String>,)>),
    NetUidRuleChange(GlobalRefCallback<(bridge::NetUidRuleInfo,)>),
    NetUidPolicyChange(GlobalRefCallback<(bridge::NetUidPolicyInfo,)>),
}

pub fn execute_net_quota_policy_changed(policies: Vec<crate::wrapper::ffi::NetQuotaPolicyAni>) {
    policy_info!("execute_net_quota_policy_changed enter, count={}", policies.len());
    let bridge_policies: Vec<bridge::NetQuotaPolicyOutput> =
        policies.into_iter().map(|p| p.into()).collect();
    Registar::get_instance().on_net_quota_policy_changed(bridge_policies);
}

pub fn execute_net_background_policy_changed(allow: bool) {
    policy_info!("execute_net_background_policy_changed enter, allow={}", allow);
    Registar::get_instance().on_net_background_policy_changed(allow);
}

pub fn execute_net_metered_ifaces_changed(info: crate::wrapper::ffi::MeteredIfaces) {
    policy_info!("execute_net_metered_ifaces_changed enter, iface_count={}", info.ifaces.len());
    Registar::get_instance().on_net_metered_ifaces_changed(info.ifaces);
}

pub fn execute_net_uid_rule_changed(info: crate::wrapper::ffi::NetUidRuleInfo) {
    policy_info!("execute_net_uid_rule_changed enter, uid={}, rule={}", info.uid, info.rule);
    let bridge_info = bridge::NetUidRuleInfo { uid: info.uid, rule: info.rule };
    Registar::get_instance().on_net_uid_rule_changed(bridge_info);
}

pub fn execute_net_uid_policy_changed(info: crate::wrapper::ffi::NetUidPolicyInfo) {
    policy_info!("execute_net_uid_policy_changed enter, uid={}, policy={}", info.uid, info.policy);
    let bridge_info = bridge::NetUidPolicyInfo { uid: info.uid, policy: info.policy };
    Registar::get_instance().on_net_uid_policy_changed(bridge_info);
}

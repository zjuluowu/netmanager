// Copyright (C) 2025 Huawei Device Co., Ltd.
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

use std::{
    mem,
    ops::Deref,
    sync::{Mutex, OnceLock},
};

use ani_rs::{
    business_error::BusinessError,
    objects::{AniFnObject, GlobalRefCallback},
    AniEnv,
};

use crate::{
    bridge,
    error_code::convert_to_business_error,
    wrapper::{ffi, NetStatsClient},
};

struct Registar {
    inner: Mutex<Vec<CallbackFlavor>>,
}

impl Registar {
    fn new() -> Self {
        Self {
            inner: Mutex::new(Vec::new()),
        }
    }

    pub fn get_instance() -> &'static Self {
        static INSTANCE: OnceLock<Registar> = OnceLock::new();
        INSTANCE.get_or_init(Registar::new)
    }

    pub fn register(&self, callback: CallbackFlavor) -> Result<(), i32> {
        let mut inner = self.inner.lock().unwrap();
        NetStatsClient::register_net_statis_observer()?;

        inner.retain(|c| {
            if std::mem::discriminant(&callback) == std::mem::discriminant(c) {
                false
            } else {
                true
            }
        });
        inner.push(callback);
        Ok(())
    }

    pub fn unregister(&self, callback_ref: Option<CallbackFlavor>) -> Result<(), i32> {
        let mut inner = self.inner.lock().unwrap();
        if let Some(callback) = callback_ref {
            inner.retain(|c| *c != callback)
        } else {
            inner.retain(|c| {
                if let CallbackFlavor::NetStatesChange(_) = c {
                    false
                } else {
                    true
                }
            })
        }

        if (inner.is_empty()) {
            NetStatsClient::unregister_net_statis_observer()?;
        }

        Ok(())
    }

    pub fn on_net_iface_stats_changed(&self, info: ffi::NetStatsChangeInfo) {
        let inner = self.inner.lock().unwrap();
        let mut param = bridge::NetStatsChangeInfo::from(info);
        param.uid = None;
        for listen in inner.deref() {
            if let CallbackFlavor::NetStatesChange(callback) = listen {
                callback.execute((param.clone(),));
            }
        }
    }

    pub fn on_net_uid_stats_changed(&self, info: ffi::NetStatsChangeInfo) {
        let inner = self.inner.lock().unwrap();
        let param = bridge::NetStatsChangeInfo::from(info);
        for listen in inner.deref() {
            if let CallbackFlavor::NetStatesChange(callback) = listen {
                callback.execute((param.clone(),));
            }
        }
    }
}

#[ani_rs::native]
pub fn on_net_states_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let callback = callback.into_global_callback(env).unwrap();
    let flavor = CallbackFlavor::NetStatesChange(callback);
    Registar::get_instance()
        .register(flavor)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn off_net_states_change(env: &AniEnv, callback: AniFnObject) -> Result<(), BusinessError> {
    let callback_flavor = if env.is_undefined(&callback).unwrap() {
        None
    } else {
        let callback_global = callback.into_global_callback(env).unwrap();
        Some(CallbackFlavor::NetStatesChange(callback_global))
    };

    Registar::get_instance()
        .unregister(callback_flavor)
        .map_err(convert_to_business_error)
}

#[derive(PartialEq, Eq)]
pub enum CallbackFlavor {
    NetStatesChange(GlobalRefCallback<(bridge::NetStatsChangeInfo,)>),
}

pub fn execute_net_iface_stats_changed(info: ffi::NetStatsChangeInfo) {
    Registar::get_instance().on_net_iface_stats_changed(info);
}
pub fn execute_net_uid_stats_changed(info: ffi::NetStatsChangeInfo) {
    Registar::get_instance().on_net_uid_stats_changed(info);
}
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

mod bridge;
mod error_code;
// #[macro_use]
mod log;
mod policy;
mod register;
mod wrapper;

use ani_rs::ani_constructor;
// use log::policy_error;

ani_constructor! {
    namespace "@ohos.net.policy.policy"
    [
        "getNetAccessPolicySync" : policy::get_net_access_policy,
        "onNetQuotaPolicyChangeSync": register::on_net_quota_policy_change,
        "offNetQuotaPolicyChangeSync": register::off_net_quota_policy_change,
        "onNetBackgroundPolicyChangeSync": register::on_net_background_policy_change,
        "offNetBackgroundPolicyChangeSync": register::off_net_background_policy_change,
        "onNetMeteredIfacesChangeSync": register::on_net_metered_ifaces_change,
        "offNetMeteredIfacesChangeSync": register::off_net_metered_ifaces_change,
        "onNetUidRuleChangeSync": register::on_net_uid_rule_change,
        "offNetUidRuleChangeSync": register::off_net_uid_rule_change,
        "onNetUidPolicyChangeSync": register::on_net_uid_policy_change,
        "offNetUidPolicyChangeSync": register::off_net_uid_policy_change,
        "showAppNetPolicySettingsSync": policy::show_app_net_policy_settings,
        "setBackgroundAllowedSync": policy::set_background_allowed,
        "restoreAllPoliciesSync": policy::restore_all_policies,
        "setPowerSaveTrustlistSync": policy::set_power_save_trustlist,
        "updateRemindPolicySync": policy::update_remind_policy,
        "getPolicyByUidSync": policy::get_policy_by_uid,
        "isBackgroundAllowedSync": policy::is_background_allowed,
        "resetPoliciesSync": policy::reset_policies,
        "getPowerSaveTrustlistSync": policy::get_power_save_trustlist,
        "getDeviceIdleTrustlistSync": policy::get_device_idle_trustlist,
        "isUidNetAllowedSync": policy::is_uid_net_allowed,
        "isUidNetAllowedByIfaceSync": policy::is_uid_net_allowed_by_iface,
        "setPolicyByUidSync": policy::set_policy_by_uid,
        "getBackgroundPolicyByUidSync": policy::get_background_policy_by_uid,
        "getUidsByPolicySync": policy::get_uids_by_policy,
        "setDeviceIdleTrustlistSync": policy::set_device_idle_trustlist,
        "getNetworkAccessPolicyByUidSync": policy::get_network_access_policy_by_uid,
        "getAllNetworkAccessPoliciesSync": policy::get_all_network_access_policies,
        "getNetQuotaPoliciesSync": policy::get_net_quota_policies,
        "setNetworkAccessPolicySync": policy::set_network_access_policy,
        "setNetQuotaPoliciesSync": policy::set_net_quota_policies,
    ]
}

const LOG_LABEL: hilog_rust::HiLogLabel = hilog_rust::HiLogLabel {
    log_type: hilog_rust::LogType::LogCore,
    domain: 0xD0015B0,
    tag: "NetMgrSubSystem",
};

#[used]
#[link_section = ".init_array"]
static G_POLICY_PANIC_HOOK: extern "C" fn() = {
    #[link_section = ".text.startup"]
    extern "C" fn init() {
        std::panic::set_hook(Box::new(|info| {
            policy_error!("Panic occurred: {:?}", info);
        }));
    }
    init
};

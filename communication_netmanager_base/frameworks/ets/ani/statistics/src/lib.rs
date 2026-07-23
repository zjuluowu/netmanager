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

mod bridge;
mod error_code;
mod log;
mod register;
mod statistics;
mod wrapper;

use ani_rs::ani_constructor;

ani_constructor! {
    namespace "@ohos.net.statistics.statistics"
    [
        "getAllRxBytesSync" : statistics::get_all_rx_bytes,
        "getAllTxBytesSync" : statistics::get_all_tx_bytes,
        "getCellularRxBytesSync" : statistics::get_cellular_rx_bytes,
        "getCellularTxBytesSync" : statistics::get_cellular_tx_bytes,
        "getIfaceRxBytesSync" : statistics::get_iface_rx_bytes,
        "getIfaceTxBytesSync" : statistics::get_iface_tx_bytes,
        "getUidRxBytesSync" : statistics::get_uid_rx_bytes,
        "getUidTxBytesSync" : statistics::get_uid_tx_bytes,
        "getSockfdRxBytesSync" : statistics::get_sockfd_rx_bytes,
        "getSockfdTxBytesSync" : statistics::get_sockfd_tx_bytes,
        "onNetStatsChangeSync": register::on_net_states_change,
        "offNetStatsChangeSync": register::off_net_states_change,
        "getTrafficStatsByIfaceSync": statistics::get_traffic_stats_by_iface,
        "getTrafficStatsByUidSync": statistics::get_traffic_stats_by_uid,
        "getTrafficStatsByNetworkSync" : statistics::get_traffic_stats_by_network,
        "getTrafficStatsByUidNetworkSync" : statistics::get_traffic_stats_by_uid_network,
        "updateStatsDataSync": statistics::update_stats_data,
        "getSelfTrafficStatsSync": statistics::get_self_traffic_stats,
        "updateIfacesStatsSync": statistics::update_ifaces_stats,
        "setCalibrationTrafficSync": statistics::set_calibration_traffic,
        "getTrafficPlanInfoSync": statistics::get_traffic_plan_info,
        "setTrafficPlanInfoSync": statistics::set_traffic_plan_info,
    ]
}

const LOG_LABEL: hilog_rust::HiLogLabel = hilog_rust::HiLogLabel {
    log_type: hilog_rust::LogType::LogCore,
    domain: 0xD0015B0,
    tag: "NetMgrSubSystem",
};

#[used]
#[link_section = ".init_array"]
static G_STATISTICS_PANIC_HOOK: extern "C" fn() = {
    #[link_section = ".text.startup"]
    extern "C" fn init() {
        std::panic::set_hook(Box::new(|info| {
            statistics_error!("Panic occurred: {:?}", info);
        }));
    }
    init
};

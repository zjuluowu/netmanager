// Copyright (c) 2025 Huawei Device Co., Ltd.
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
use crate::wrapper::ffi;
use ani_rs::ani;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

#[ani_rs::ani(path = "@ohos.net.connection.connection.NetBearType")]
#[derive(Debug, Copy, Clone)]
pub enum NetBearType {
    BearerCellular = 0,
    BearerWifi = 1,
    BearerBluetooth = 2,
    BearerEthernet = 3,
    BearerVpn = 4,
    BearerWifiAware = 5,
    BearerDefault,
}

#[ani_rs::ani(path = "@ohos.net.statistics.statistics.NetworkInfo")]
pub struct AniNetworkInfo {
    pub type_: NetBearType,
    pub start_time: i32,
    pub end_time: i32,
    pub sim_id: Option<i32>,
}

#[ani_rs::ani(path = "@ohos.net.statistics.statistics.UidNetStatsInfo")]
pub struct AniUidNetStatsInfoPair {
    pub uid: i32,
    pub net_stats_info: NetStatsInfo,
}

#[ani_rs::ani(path = "@ohos.net.statistics.statistics.NetStatsInfoSequenceItemInner")]
pub struct AniNetStatsInfoSequenceItem {
    pub start_time: i32,
    pub end_time: i32,
    pub info: NetStatsInfo,
}

#[ani_rs::ani(path = "@ohos.net.statistics.statistics.NetStatsChangeInfo")]
#[derive(Clone)]
pub struct NetStatsChangeInfo {
    pub iface: String,
    pub uid: Option<i32>,
}

#[ani_rs::ani(path = "@ohos.net.statistics.statistics.IfaceInfo")]
pub struct IfaceInfo {
    pub iface: String,
    pub start_time: i32,
    pub end_time: i32,
}

#[ani_rs::ani(path = "@ohos.net.statistics.statistics.NetStatsInfoInner")]
pub struct NetStatsInfo {
    pub rx_bytes: i64,
    pub tx_bytes: i64,
    pub rx_packets: i64,
    pub tx_packets: i64,
}

#[ani_rs::ani(path = "@ohos.net.statistics.statistics.UidInfo")]
pub struct UidInfo {
    pub iface_info: IfaceInfo,
    pub uid: i32,
}

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

use crate::{bridge, error_code::{convert_to_business_error, NETMANAGER_ERR_PARAMETER_ERROR}, wrapper::NetStatsClient};
use ani_rs::business_error::BusinessError;
use std::collections::HashMap;

#[ani_rs::native]
pub fn get_all_rx_bytes() -> Result<i64, BusinessError> {
    NetStatsClient::get_all_rx_bytes()
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_all_tx_bytes() -> Result<i64, BusinessError> {
    NetStatsClient::get_all_tx_bytes()
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_cellular_rx_bytes() -> Result<i64, BusinessError> {
    NetStatsClient::get_cellular_rx_bytes()
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_cellular_tx_bytes() -> Result<i64, BusinessError> {
    NetStatsClient::get_cellular_tx_bytes()
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_iface_rx_bytes(iface: String) -> Result<i64, BusinessError> {
    NetStatsClient::get_iface_rx_bytes(&iface)
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_iface_tx_bytes(iface: String) -> Result<i64, BusinessError> {
    NetStatsClient::get_iface_tx_bytes(&iface)
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_uid_rx_bytes(uid: i64) -> Result<i64, BusinessError> {
    NetStatsClient::get_uid_rx_bytes(uid as u32)
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_uid_tx_bytes(uid: i64) -> Result<i64, BusinessError> {
    NetStatsClient::get_uid_tx_bytes(uid as u32)
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_sockfd_rx_bytes(sockfd: i32) -> Result<i64, BusinessError> {
    NetStatsClient::get_sockfd_rx_bytes(sockfd)
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_sockfd_tx_bytes(sockfd: i32) -> Result<i64, BusinessError> {
    NetStatsClient::get_sockfd_tx_bytes(sockfd)
        .map(|v| v as i64)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_traffic_stats_by_iface(
    ifaceInfo: bridge::IfaceInfo,
) -> Result<bridge::NetStatsInfo, BusinessError> {
    NetStatsClient::get_traffic_stats_by_iface(ifaceInfo.into())
        .map(|v| v as bridge::NetStatsInfo)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_traffic_stats_by_uid(
    uidInfo: bridge::UidInfo,
) -> Result<bridge::NetStatsInfo, BusinessError> {
    NetStatsClient::get_traffic_stats_by_uid(uidInfo.into())
        .map(|v| v as bridge::NetStatsInfo)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_traffic_stats_by_network(
    networkInfo: bridge::AniNetworkInfo,
) -> Result<HashMap<i32, bridge::NetStatsInfo>, BusinessError> {
    NetStatsClient::get_traffic_stats_by_network(networkInfo.into())
        .map(|v: Vec<bridge::AniUidNetStatsInfoPair>| {
            v.into_iter()
                .map(|item| (item.uid, item.net_stats_info))
                .collect()
        })
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_traffic_stats_by_uid_network(
    uid: i32,
    networkInfo: bridge::AniNetworkInfo,
) -> Result<Vec<bridge::AniNetStatsInfoSequenceItem>, BusinessError> {
    NetStatsClient::get_traffic_stats_by_uid_network(uid, networkInfo.into())
        .map(|v| v as Vec<bridge::AniNetStatsInfoSequenceItem>)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn update_stats_data() -> Result<(), BusinessError> {
    NetStatsClient::update_stats_data()
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_self_traffic_stats(networkInfo: bridge::AniNetworkInfo)
    -> Result<bridge::NetStatsInfo, BusinessError> {
    NetStatsClient::get_self_traffic_stats(networkInfo)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn update_ifaces_stats(iface: String, start: i32, end: i32,
    stats: bridge::NetStatsInfo) -> Result<(), BusinessError> {
    if start < 0 || end < 0 {
        return Err(BusinessError::new(NETMANAGER_ERR_PARAMETER_ERROR, ("Parameter error: start and end must be non-negative").to_string()));
    }
    NetStatsClient::update_ifaces_stats(&iface, start as u64, end as u64, stats)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn set_calibration_traffic(sim_id: i32, remain_traffic: i64,
    total_traffic: i64) -> Result<(), BusinessError> {
    let sim_id: u32 = sim_id.try_into().map_err(|_| {
        BusinessError::new(NETMANAGER_ERR_PARAMETER_ERROR, ("Parameter error: sim_id must be non-negative").to_string())
    })?;
    if (remain_traffic < 0) {
        return Err(BusinessError::new(NETMANAGER_ERR_PARAMETER_ERROR, ("Parameter error: remain_traffic must be non-negative").to_string()));
    }
    let total_traffic: u64 = total_traffic.try_into().map_err(|_| {
        BusinessError::new(NETMANAGER_ERR_PARAMETER_ERROR, ("Parameter error: total_traffic must be non-negative").to_string())
    })?;
    NetStatsClient::set_calibration_traffic(sim_id, remain_traffic, total_traffic)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn get_traffic_plan_info(sim_id: i32, param: i32) -> Result<i64, BusinessError> {
    NetStatsClient::get_traffic_plan_info(sim_id, param)
        .map_err(convert_to_business_error)
}

#[ani_rs::native]
pub fn set_traffic_plan_info(sim_id: i32, param: i32, value: i64) -> Result<(), BusinessError> {
    NetStatsClient::set_traffic_plan_info(sim_id, param, value)
        .map_err(convert_to_business_error)
}

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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STATS_IPC_INTERFACE_CODE_H
#define STATS_IPC_INTERFACE_CODE_H

/* SAID: 1153 */
namespace OHOS {
namespace NetManagerStandard {
enum class StatsCallBackInterfaceCode {
    NET_STATS_IFACE_CHANGED = 0,
    NET_STATS_UID_CHANGED = 1,
};

enum class StatsInterfaceCode {
    CMD_START = 0,
    CMD_SYSTEM_READY,
    CMD_GET_IFACE_STATS_DETAIL,
    CMD_GET_UID_STATS_DETAIL,
    CMD_UPDATE_IFACES_STATS,
    CMD_UPDATE_STATS_DATA,
    CMD_NSM_REGISTER_NET_STATS_CALLBACK,
    CMD_NSM_UNREGISTER_NET_STATS_CALLBACK,
    CMD_NSM_RESET_FACTORY,
    CMD_GET_IFACE_RXBYTES,
    CMD_GET_IFACE_TXBYTES,
    CMD_GET_CELLULAR_RXBYTES,
    CMD_GET_CELLULAR_TXBYTES,
    CMD_GET_ALL_RXBYTES,
    CMD_GET_ALL_TXBYTES,
    CMD_GET_UID_RXBYTES,
    CMD_GET_UID_TXBYTES,
    CMD_GET_ALL_STATS_INFO,
    CMD_GET_COOKIE_RXBYTES,
    CMD_GET_COOKIE_TXBYTES,
    CMD_GET_ALL_SIM_STATS_INFO,
    CMD_GET_TRAFFIC_STATS_BY_NETWORK,
    CMD_GET_TRAFFIC_STATS_BY_UID_NETWORK,
    CMD_SET_APP_STATS,
    CMD_SET_SHARING_TRAFFIC_BEFORE_STOP,
    CMD_SET_CALIBRATION_TRAFFIC,
    CMD_SET_TRAFFIC_PLAN_INFO,
    CMD_GET_TRAFFIC_PLAN_INFO,
    CMD_GET_MONTH_TRAFFIC_STATS_BY_NETWORK,
    CMD_SET_DPA_APP_STATS,
    CMD_END = 100,
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // STATS_IPC_INTERFACE_CODE_H
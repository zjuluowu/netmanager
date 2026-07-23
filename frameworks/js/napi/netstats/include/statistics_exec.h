/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETMANAGERBASE_STATISTICS_EXEC_H
#define COMMUNICATIONNETMANAGERBASE_STATISTICS_EXEC_H

#include <cstdint>
#include <string>

#include <napi/native_api.h>

#include "get_cellular_rxbytes_context.h"
#include "get_iface_rxbytes_context.h"
#include "get_iface_stats_context.h"
#include "get_iface_uid_stats_context.h"
#include "get_traffic_stats_by_network_context.h"
#include "get_self_traffic_stats_context.h"
#include "get_traffic_stats_by_uid_network_context.h"
#include "get_month_traffic_stats_context_by_network.h"
#include "get_uid_rxbytes_context.h"
#include "update_iface_stats_context.h"
#include "get_sockfd_rxbytes_context.h"
#include "set_calibration_traffic_context.h"
#include "set_traffic_plan_info_context.h"
#include "get_traffic_plan_info_context.h"

namespace OHOS {
namespace NetManagerStandard {
class StatisticsExec final {
public:
    StatisticsExec() = delete;
    ~StatisticsExec() = delete;

    static bool ExecGetCellularRxBytes(GetCellularRxBytesContext *context);
    static bool ExecGetCellularTxBytes(GetCellularTxBytesContext *context);
    static bool ExecGetAllRxBytes(GetAllRxBytesContext *context);
    static bool ExecGetAllTxBytes(GetAllTxBytesContext *context);
    static bool ExecGetUidRxBytes(GetUidRxBytesContext *context);
    static bool ExecGetUidTxBytes(GetUidTxBytesContext *context);
    static bool ExecGetIfaceRxBytes(GetIfaceRxBytesContext *context);
    static bool ExecGetIfaceTxBytes(GetIfaceTxBytesContext *context);
    static bool ExecGetIfaceStats(GetIfaceStatsContext *context);
    static bool ExecGetIfaceUidStats(GetIfaceUidStatsContext *context);
    static bool ExecUpdateIfacesStats(UpdateIfacesStatsContext *context);
    static bool ExecUpdateStatsData(UpdateStatsDataContext *context);
    static bool ExecGetSockfdRxBytes(GetSockfdRxBytesContext *context);
    static bool ExecGetSockfdTxBytes(GetSockfdTxBytesContext *context);
    static bool ExecGetTrafficStatsByNetwork(GetTrafficStatsByNetworkContext *context);
    static bool ExecGetTrafficStatsByUidNetwork(GetTrafficStatsByUidNetworkContext *context);
    static bool ExecGetSelfTrafficStats(GetSelfTrafficStatsContext *context);
    static bool ExecGetMonthTrafficStatsByNetwork(GetMonthTrafficStatsByNetworkContext *context);
    static bool ExecSetCalibrationTraffic(SetCalibrationTrafficContext *context);
    static bool ExecSetTrafficPlanInfo(SetTrafficPlanInfoContext *context);
    static bool ExecGetTrafficPlanInfo(GetTrafficPlanInfoContext *context);

    static napi_value GetCellularRxBytesCallback(GetCellularRxBytesContext *context);
    static napi_value GetCellularTxBytesCallback(GetCellularTxBytesContext *context);
    static napi_value GetAllRxBytesCallback(GetAllRxBytesContext *context);
    static napi_value GetAllTxBytesCallback(GetAllTxBytesContext *context);
    static napi_value GetUidRxBytesCallback(GetUidRxBytesContext *context);
    static napi_value GetUidTxBytesCallback(GetUidTxBytesContext *context);
    static napi_value GetIfaceRxBytesCallback(GetIfaceRxBytesContext *context);
    static napi_value GetIfaceTxBytesCallback(GetIfaceTxBytesContext *context);
    static napi_value GetIfaceStatsCallback(GetIfaceStatsContext *context);
    static napi_value GetIfaceUidStatsCallback(GetIfaceUidStatsContext *context);
    static napi_value UpdateIfacesStatsCallback(UpdateIfacesStatsContext *context);
    static napi_value UpdateStatsDataCallback(UpdateStatsDataContext *context);
    static napi_value GetSockfdRxBytesCallback(GetSockfdRxBytesContext *context);
    static napi_value GetSockfdTxBytesCallback(GetSockfdTxBytesContext *context);
    static napi_value GetGetTrafficStatsByNetworkCallback(GetTrafficStatsByNetworkContext *context);
    static napi_value GetGetTrafficStatsByUidNetworkCallback(GetTrafficStatsByUidNetworkContext *context);
    static napi_value GetSelfTrafficStatsCallback(GetSelfTrafficStatsContext *context);
    static napi_value GetMonthTrafficStatsByNetworkCallback(GetMonthTrafficStatsByNetworkContext *context);
    static napi_value SetCalibrationTrafficCallback(SetCalibrationTrafficContext *context);
    static napi_value SetTrafficPlanInfoCallback(SetTrafficPlanInfoContext *context);
    static napi_value GetTrafficPlanInfoCallback(GetTrafficPlanInfoContext *context);

    static napi_value CreateCodeMessage(napi_env env, const std::string &msg, int32_t code);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGERBASE_STATISTICS_EXEC_H

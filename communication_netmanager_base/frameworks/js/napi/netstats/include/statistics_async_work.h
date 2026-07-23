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

#ifndef COMMUNICATIONNETMANAGERBASE_STATISTICS_ASYNC_WORK_H
#define COMMUNICATIONNETMANAGERBASE_STATISTICS_ASYNC_WORK_H

#include <napi/native_api.h>
#include <napi/native_node_api.h>

namespace OHOS {
namespace NetManagerStandard {
class StatisticsAsyncWork final {
public:
    StatisticsAsyncWork() = delete;
    ~StatisticsAsyncWork() = delete;

    static void ExecGetCellularRxBytes(napi_env env, void *data);
    static void ExecGetCellularTxBytes(napi_env env, void *data);
    static void ExecGetAllRxBytes(napi_env env, void *data);
    static void ExecGetAllTxBytes(napi_env env, void *data);
    static void ExecGetUidRxBytes(napi_env env, void *data);
    static void ExecGetUidTxBytes(napi_env env, void *data);
    static void ExecGetIfaceRxBytes(napi_env env, void *data);
    static void ExecGetIfaceTxBytes(napi_env env, void *data);
    static void ExecGetIfaceStats(napi_env env, void *data);
    static void ExecGetIfaceUidStats(napi_env env, void *data);
    static void ExecUpdateIfacesStats(napi_env env, void *data);
    static void ExecUpdateStatsData(napi_env env, void *data);
    static void ExecGetSockfdRxBytes(napi_env env, void *data);
    static void ExecGetSockfdTxBytes(napi_env env, void *data);
    static void ExecGetTrafficStatsByNetwork(napi_env env, void *data);
    static void ExecGetTrafficStatsByUidNetwork(napi_env env, void *data);
    static void ExecGetSelfTrafficStats(napi_env env, void *data);
    static void ExecGetMonthTrafficStatsByNetwork(napi_env env, void *data);
    static void ExecSetCalibrationTraffic(napi_env env, void *data);

    static void GetCellularRxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetCellularTxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetAllRxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetAllTxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetUidRxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetUidTxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetIfaceRxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetIfaceTxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetIfaceStatsCallback(napi_env env, napi_status status, void *data);
    static void GetIfaceUidStatsCallback(napi_env env, napi_status status, void *data);
    static void UpdateIfacesStatsCallback(napi_env env, napi_status status, void *data);
    static void UpdateStatsDataCallback(napi_env env, napi_status status, void *data);
    static void GetSockfdRxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetSockfdTxBytesCallback(napi_env env, napi_status status, void *data);
    static void GetTrafficStatsByNetworkCallback(napi_env env, napi_status status, void *data);
    static void GetTrafficStatsByUidNetworkCallback(napi_env env, napi_status status, void *data);
    static void GetSelfTrafficStatsCallback(napi_env env, napi_status status, void *data);
    static void GetMonthTrafficStatsByNetworkCallback(napi_env env, napi_status status, void *data);
    static void SetCalibrationTrafficCallback(napi_env env, napi_status status, void *data);
    static void ExecSetTrafficPlanInfo(napi_env env, void *data);
    static void ExecGetTrafficPlanInfo(napi_env env, void *data);
    static void SetTrafficPlanInfoCallback(napi_env env, napi_status status, void *data);
    static void GetTrafficPlanInfoCallback(napi_env env, napi_status status, void *data);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGERBASE_STATISTICS_ASYNC_WORK_H

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

#include "statistics_async_work.h"

#include "base_async_work.h"
#include "get_cellular_rxbytes_context.h"
#include "get_iface_rxbytes_context.h"
#include "get_iface_stats_context.h"
#include "get_iface_uid_stats_context.h"
#include "get_traffic_stats_by_network_context.h"
#include "get_traffic_stats_by_uid_network_context.h"
#include "get_month_traffic_stats_context_by_network.h"
#include "get_uid_rxbytes_context.h"
#include "statistics_exec.h"
#include "update_iface_stats_context.h"
#include "set_calibration_traffic_context.h"
#include "set_traffic_plan_info_context.h"
#include "get_traffic_plan_info_context.h"

namespace OHOS {
namespace NetManagerStandard {
void StatisticsAsyncWork::ExecGetCellularRxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetCellularRxBytesContext, StatisticsExec::ExecGetCellularRxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetCellularTxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetCellularTxBytesContext, StatisticsExec::ExecGetCellularTxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetAllRxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAllRxBytesContext, StatisticsExec::ExecGetAllRxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetAllTxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAllTxBytesContext, StatisticsExec::ExecGetAllTxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetUidRxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetUidRxBytesContext, StatisticsExec::ExecGetUidRxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetUidTxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetUidTxBytesContext, StatisticsExec::ExecGetUidTxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetIfaceRxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetIfaceRxBytesContext, StatisticsExec::ExecGetIfaceRxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetIfaceTxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetIfaceTxBytesContext, StatisticsExec::ExecGetIfaceTxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetIfaceStats(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetIfaceStatsContext, StatisticsExec::ExecGetIfaceStats>(env, data);
}

void StatisticsAsyncWork::ExecGetIfaceUidStats(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetIfaceUidStatsContext, StatisticsExec::ExecGetIfaceUidStats>(env, data);
}

void StatisticsAsyncWork::ExecUpdateIfacesStats(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UpdateIfacesStatsContext, StatisticsExec::ExecUpdateIfacesStats>(env, data);
}

void StatisticsAsyncWork::ExecUpdateStatsData(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UpdateStatsDataContext, StatisticsExec::ExecUpdateStatsData>(env, data);
}

void StatisticsAsyncWork::ExecGetSockfdRxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetSockfdRxBytesContext, StatisticsExec::ExecGetSockfdRxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetSockfdTxBytes(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetSockfdTxBytesContext, StatisticsExec::ExecGetSockfdTxBytes>(env, data);
}

void StatisticsAsyncWork::ExecGetTrafficStatsByNetwork(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetTrafficStatsByNetworkContext, StatisticsExec::ExecGetTrafficStatsByNetwork>(env,
                                                                                                                data);
}

void StatisticsAsyncWork::ExecGetTrafficStatsByUidNetwork(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetTrafficStatsByUidNetworkContext, StatisticsExec::ExecGetTrafficStatsByUidNetwork>(
        env, data);
}

void StatisticsAsyncWork::ExecGetSelfTrafficStats(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetSelfTrafficStatsContext, StatisticsExec::ExecGetSelfTrafficStats>(
        env, data);
}

void StatisticsAsyncWork::ExecGetMonthTrafficStatsByNetwork(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetMonthTrafficStatsByNetworkContext,
        StatisticsExec::ExecGetMonthTrafficStatsByNetwork>(env, data);
}

void StatisticsAsyncWork::GetCellularRxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetCellularRxBytesContext, StatisticsExec::GetCellularRxBytesCallback>(
        env, status, data);
}

void StatisticsAsyncWork::GetCellularTxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetCellularTxBytesContext, StatisticsExec::GetCellularTxBytesCallback>(
        env, status, data);
}

void StatisticsAsyncWork::GetAllRxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAllRxBytesContext, StatisticsExec::GetAllRxBytesCallback>(env, status, data);
}

void StatisticsAsyncWork::GetAllTxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAllTxBytesContext, StatisticsExec::GetAllTxBytesCallback>(env, status, data);
}

void StatisticsAsyncWork::GetUidRxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetUidRxBytesContext, StatisticsExec::GetUidRxBytesCallback>(env, status, data);
}

void StatisticsAsyncWork::GetUidTxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetUidTxBytesContext, StatisticsExec::GetUidTxBytesCallback>(env, status, data);
}
void StatisticsAsyncWork::GetIfaceRxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetIfaceRxBytesContext, StatisticsExec::GetIfaceRxBytesCallback>(env, status,
                                                                                                      data);
}

void StatisticsAsyncWork::GetIfaceTxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetIfaceTxBytesContext, StatisticsExec::GetIfaceTxBytesCallback>(env, status,
                                                                                                      data);
}

void StatisticsAsyncWork::GetIfaceStatsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetIfaceStatsContext, StatisticsExec::GetIfaceStatsCallback>(env, status, data);
}

void StatisticsAsyncWork::GetIfaceUidStatsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetIfaceUidStatsContext, StatisticsExec::GetIfaceUidStatsCallback>(env, status,
                                                                                                        data);
}

void StatisticsAsyncWork::UpdateIfacesStatsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<UpdateIfacesStatsContext, StatisticsExec::UpdateIfacesStatsCallback>(env, status,
                                                                                                          data);
}

void StatisticsAsyncWork::UpdateStatsDataCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<UpdateStatsDataContext, StatisticsExec::UpdateStatsDataCallback>(env, status,
                                                                                                      data);
}

void StatisticsAsyncWork::GetSockfdRxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetSockfdRxBytesContext, StatisticsExec::GetSockfdRxBytesCallback>(env, status,
                                                                                                        data);
}

void StatisticsAsyncWork::GetSockfdTxBytesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetSockfdTxBytesContext, StatisticsExec::GetSockfdTxBytesCallback>(env, status,
                                                                                                        data);
}

void StatisticsAsyncWork::GetTrafficStatsByNetworkCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetTrafficStatsByNetworkContext,
                                     StatisticsExec::GetGetTrafficStatsByNetworkCallback>(env, status, data);
}

void StatisticsAsyncWork::GetMonthTrafficStatsByNetworkCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetMonthTrafficStatsByNetworkContext,
                                     StatisticsExec::GetMonthTrafficStatsByNetworkCallback>(env, status, data);
}

void StatisticsAsyncWork::GetTrafficStatsByUidNetworkCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetTrafficStatsByUidNetworkContext,
                                     StatisticsExec::GetGetTrafficStatsByUidNetworkCallback>(env, status, data);
}

void StatisticsAsyncWork::GetSelfTrafficStatsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetSelfTrafficStatsContext,
                                     StatisticsExec::GetSelfTrafficStatsCallback>(env, status, data);
}

void StatisticsAsyncWork::ExecSetCalibrationTraffic(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetCalibrationTrafficContext, StatisticsExec::ExecSetCalibrationTraffic>(env, data);
}

void StatisticsAsyncWork::SetCalibrationTrafficCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetCalibrationTrafficContext,
                                     StatisticsExec::SetCalibrationTrafficCallback>(env, status, data);
}
void StatisticsAsyncWork::ExecSetTrafficPlanInfo(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetTrafficPlanInfoContext, StatisticsExec::ExecSetTrafficPlanInfo>(env, data);
}

void StatisticsAsyncWork::ExecGetTrafficPlanInfo(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetTrafficPlanInfoContext, StatisticsExec::ExecGetTrafficPlanInfo>(env, data);
}

void StatisticsAsyncWork::SetTrafficPlanInfoCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetTrafficPlanInfoContext,
                                     StatisticsExec::SetTrafficPlanInfoCallback>(env, status, data);
}

void StatisticsAsyncWork::GetTrafficPlanInfoCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetTrafficPlanInfoContext,
                                     StatisticsExec::GetTrafficPlanInfoCallback>(env, status, data);
}

}
} // namespace NetManagerStandard

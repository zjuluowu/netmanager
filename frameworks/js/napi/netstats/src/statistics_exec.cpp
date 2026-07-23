/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "statistics_exec.h"

#include "errorcode_convertor.h"
#include "napi_utils.h"
#include "net_stats_client.h"
#include "net_stats_constants.h"
#include "net_stats_network.h"
#include "netmanager_base_log.h"
#include "statistics_observer_wrapper.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
const std::string RX_BYTES = "rxBytes";
const std::string TX_BYTES = "txBytes";
const std::string RX_PACKETS = "rxPackets";
const std::string TX_PACKETS = "txPackets";
const std::string START_TIME = "startTime";
const std::string END_TIME = "endTime";
const std::string NET_STATS_INFO = "info";
} // namespace

bool StatisticsExec::ExecGetCellularRxBytes(GetCellularRxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetCellularRxBytes(context->bytes64_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetCellularTxBytes(GetCellularTxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetCellularTxBytes(context->bytes64_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetAllRxBytes(GetAllRxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetAllRxBytes(context->bytes64_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetAllTxBytes(GetAllTxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetAllTxBytes(context->bytes64_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetUidRxBytes(GetUidRxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetUidRxBytes(context->bytes64_, context->uid_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetUidTxBytes(GetUidTxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetUidTxBytes(context->bytes64_, context->uid_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetIfaceRxBytes(GetIfaceRxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetIfaceRxBytes(context->bytes64_, context->interfaceName_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetIfaceTxBytes(GetIfaceTxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetIfaceTxBytes(context->bytes64_, context->interfaceName_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetIfaceStats(GetIfaceStatsContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetIfaceStatsDetail(context->GetInterfaceName(), context->GetStart(),
                                                                       context->GetEnd(), context->GetStatsInfo());
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetIfaceUidStats(GetIfaceUidStatsContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetUidStatsDetail(context->GetInterfaceName(), context->GetUid(),
                                                                     context->GetStart(), context->GetEnd(),
                                                                     context->GetStatsInfo());
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecUpdateIfacesStats(UpdateIfacesStatsContext *context)
{
    int32_t result = NetStatsClient::GetInstance().UpdateIfacesStats(context->GetInterfaceName(), context->GetStart(),
                                                                     context->GetEnd(), context->GetStatsInfo());
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecUpdateStatsData(UpdateStatsDataContext *context)
{
    int32_t result = NetStatsClient::GetInstance().UpdateStatsData();
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetSockfdRxBytes(GetSockfdRxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetSockfdRxBytes(context->bytes64_, context->sockfd_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetSockfdTxBytes(GetSockfdTxBytesContext *context)
{
    int32_t result = NetStatsClient::GetInstance().GetSockfdTxBytes(context->bytes64_, context->sockfd_);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetTrafficStatsByNetwork(GetTrafficStatsByNetworkContext *context)
{
    sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork();
    if (network == nullptr) {
        NETMANAGER_BASE_LOGE("the network of param to get traffic stats is null");
        return false;
    }
    network->simId_ = context->GetSimId();
    network->startTime_ = context->GetStartTime();
    network->endTime_ = context->GetEndTime();
    network->type_ = context->GetNetBearType();
    int32_t result = NetStatsClient::GetInstance().GetTrafficStatsByNetwork(context->GetNetStatsInfo(), network);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetMonthTrafficStatsByNetwork(GetMonthTrafficStatsByNetworkContext *context)
{
    uint32_t simId = context->GetSimId();
    int32_t result = NetStatsClient::GetInstance().GetMonthTrafficStatsByNetwork(simId,
        context->GetMonthTrafficData());
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetTrafficStatsByUidNetwork(GetTrafficStatsByUidNetworkContext *context)
{
    sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork();
    if (network == nullptr) {
        NETMANAGER_BASE_LOGE("the network of param to get traffic stats is null");
        return false;
    }
    network->simId_ = context->GetSimId();
    network->startTime_ = context->GetStartTime();
    network->endTime_ = context->GetEndTime();
    network->type_ = context->GetNetBearType();
    int32_t result = NetStatsClient::GetInstance().GetTrafficStatsByUidNetwork(context->GetNetStatsInfoSequence(),
                                                                               context->GetUid(), network);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetSelfTrafficStats(GetSelfTrafficStatsContext *context)
{
    sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork();
    if (network == nullptr) {
        NETMANAGER_BASE_LOGE("the network of param to get traffic stats is null");
        return false;
    }
    network->simId_ = context->GetSimId();
    network->startTime_ = context->GetStartTime();
    network->endTime_ = context->GetEndTime();
    network->type_ = context->GetNetBearType();
    int32_t uid = IPCSkeleton::GetCallingUid();
    int32_t result = NetStatsClient::GetInstance().GetTrafficStatsByUidNetwork(context->GetNetStatsInfoSequence(),
                                                                               uid, network);
    NETMANAGER_BASE_LOGE("ExecGetSelfTrafficStats callingUid:%{public}d, result:%{public}d", uid, result);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecSetCalibrationTraffic(SetCalibrationTrafficContext *context)
{
    if (context == nullptr) {
        return false;
    }
    uint32_t simId = context->GetSimId();
    int64_t remainingData = context->GetRemainingData();
    uint64_t totalMonthlyData = context->GetTotalMonthlyData();
    int32_t result = NetStatsClient::GetInstance().SetCalibrationTraffic(simId, remainingData, totalMonthlyData);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecSetTrafficPlanInfo(SetTrafficPlanInfoContext *context)
{
    if (context == nullptr) {
        return false;
    }
    int32_t result = NetStatsClient::GetInstance().SetTrafficPlanInfo(
        context->GetSimId(), static_cast<int32_t>(context->GetParam()), context->GetValue());
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

bool StatisticsExec::ExecGetTrafficPlanInfo(GetTrafficPlanInfoContext *context)
{
    if (context == nullptr) {
        return false;
    }
    int64_t value = 0;
    int32_t result = NetStatsClient::GetInstance().GetTrafficPlanInfo(
        context->GetSimId(), static_cast<int32_t>(context->GetParam()), value);
    context->SetValue(value);
    context->SetErrorCode(result);
    return result == NETMANAGER_SUCCESS;
}

napi_value StatisticsExec::GetCellularRxBytesCallback(GetCellularRxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetCellularTxBytesCallback(GetCellularTxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetAllRxBytesCallback(GetAllRxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetAllTxBytesCallback(GetAllTxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetUidRxBytesCallback(GetUidRxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetUidTxBytesCallback(GetUidTxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetIfaceRxBytesCallback(GetIfaceRxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetIfaceTxBytesCallback(GetIfaceTxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetIfaceStatsCallback(GetIfaceStatsContext *context)
{
    napi_value netStatsInfo = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, RX_BYTES, context->GetStatsInfo().rxBytes_);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, TX_BYTES, context->GetStatsInfo().txBytes_);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, RX_PACKETS, context->GetStatsInfo().rxPackets_);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, TX_PACKETS, context->GetStatsInfo().txPackets_);
    return netStatsInfo;
}

napi_value StatisticsExec::GetIfaceUidStatsCallback(GetIfaceUidStatsContext *context)
{
    napi_value netStatsInfo = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, RX_BYTES, context->GetStatsInfo().rxBytes_);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, TX_BYTES, context->GetStatsInfo().txBytes_);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, RX_PACKETS, context->GetStatsInfo().rxPackets_);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, TX_PACKETS, context->GetStatsInfo().txPackets_);
    return netStatsInfo;
}

napi_value StatisticsExec::GetGetTrafficStatsByNetworkCallback(GetTrafficStatsByNetworkContext *context)
{
    napi_value infos = NapiUtils::CreateObject(context->GetEnv());
    auto data = context->GetNetStatsInfo();
    for (const auto &item : data) {
        napi_value tmp = NapiUtils::CreateObject(context->GetEnv());
        NapiUtils::SetInt64Property(context->GetEnv(), tmp, RX_BYTES, item.second.rxBytes_);
        NapiUtils::SetInt64Property(context->GetEnv(), tmp, TX_BYTES, item.second.txBytes_);
        NapiUtils::SetInt64Property(context->GetEnv(), tmp, RX_PACKETS, item.second.rxPackets_);
        NapiUtils::SetInt64Property(context->GetEnv(), tmp, TX_PACKETS, item.second.txPackets_);
        std::string key = std::to_string(item.first);
        NapiUtils::SetNamedProperty(context->GetEnv(), infos, key, tmp);
    }
    return infos;
}

napi_value StatisticsExec::GetGetTrafficStatsByUidNetworkCallback(GetTrafficStatsByUidNetworkContext *context)
{
    auto list = context->GetNetStatsInfoSequence();
    napi_value stats = NapiUtils::CreateArray(context->GetEnv(), list.size());
    size_t index = 0;
    for (const auto &item : list) {
        napi_value info = NapiUtils::CreateObject(context->GetEnv());
        NapiUtils::SetInt64Property(context->GetEnv(), info, RX_BYTES, item.info_.rxBytes_);
        NapiUtils::SetInt64Property(context->GetEnv(), info, TX_BYTES, item.info_.txBytes_);
        NapiUtils::SetInt64Property(context->GetEnv(), info, RX_PACKETS, item.info_.rxPackets_);
        NapiUtils::SetInt64Property(context->GetEnv(), info, TX_PACKETS, item.info_.txPackets_);

        napi_value tmp = NapiUtils::CreateObject(context->GetEnv());
        NapiUtils::SetInt64Property(context->GetEnv(), tmp, START_TIME, item.startTime_);
        NapiUtils::SetInt64Property(context->GetEnv(), tmp, END_TIME, item.endTime_);
        NapiUtils::SetNamedProperty(context->GetEnv(), tmp, NET_STATS_INFO, info);

        NapiUtils::SetArrayElement(context->GetEnv(), stats, index++, tmp);
    }
    return stats;
}

napi_value StatisticsExec::GetSelfTrafficStatsCallback(GetSelfTrafficStatsContext *context)
{
    auto list = context->GetNetStatsInfoSequence();
    int64_t rxByte = 0;
    int64_t txByte = 0;
    int64_t rxPackets = 0;
    int64_t txPackets = 0;
    for (const auto &item : list) {
        rxByte += item.info_.rxBytes_;
        txByte += item.info_.txBytes_;
        rxPackets += item.info_.rxPackets_;
        txPackets += item.info_.txPackets_;
    }

    napi_value netStatsInfo = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, RX_BYTES, rxByte);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, TX_BYTES, txByte);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, RX_PACKETS, rxPackets);
    NapiUtils::SetInt64Property(context->GetEnv(), netStatsInfo, TX_PACKETS, txPackets);
    return netStatsInfo;
}

napi_value StatisticsExec::SetCalibrationTrafficCallback(SetCalibrationTrafficContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value StatisticsExec::GetMonthTrafficStatsByNetworkCallback(GetMonthTrafficStatsByNetworkContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->GetMonthTrafficData());
}

napi_value StatisticsExec::UpdateIfacesStatsCallback(UpdateIfacesStatsContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value StatisticsExec::UpdateStatsDataCallback(UpdateStatsDataContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value StatisticsExec::GetSockfdRxBytesCallback(GetSockfdRxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::GetSockfdTxBytesCallback(GetSockfdTxBytesContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->bytes64_);
}

napi_value StatisticsExec::SetTrafficPlanInfoCallback(SetTrafficPlanInfoContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value StatisticsExec::GetTrafficPlanInfoCallback(GetTrafficPlanInfoContext *context)
{
    return NapiUtils::CreateInt64(context->GetEnv(), context->GetValue());
}

} // namespace NetManagerStandard
} // namespace OHOS
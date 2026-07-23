/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "net_stats_service_proxy.h"

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetStatsServiceProxy::NetStatsServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<INetStatsService>(impl) {}

NetStatsServiceProxy::~NetStatsServiceProxy() = default;
int32_t NetStatsServiceProxy::SendRequest(uint32_t code, MessageParcel &data,
                                          MessageParcel &reply)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    MessageOption option;
    int32_t retCode = remote->SendRequest(code, data, reply, option);
    if (retCode != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    NETMGR_LOG_D("SendRequest ret = [%{public}d]", ret);
    return ret;
}

bool NetStatsServiceProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetStatsServiceProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t NetStatsServiceProxy::RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    return SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_NSM_REGISTER_NET_STATS_CALLBACK),
                       dataParcel, replyParcel);
}

int32_t NetStatsServiceProxy::UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel replyParcel;
    return SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_NSM_UNREGISTER_NET_STATS_CALLBACK),
                       dataParcel, replyParcel);
}

int32_t NetStatsServiceProxy::GetIfaceRxBytes(uint64_t &stats, const std::string &interfaceName)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteString(interfaceName)) {
        NETMGR_LOG_E("WriteString failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_IFACE_RXBYTES), data, reply);
    if (error != 0) {
        if (error != STATS_ERR_READ_BPF_FAIL) {
            NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        }
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetIfaceTxBytes(uint64_t &stats, const std::string &interfaceName)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteString(interfaceName)) {
        NETMGR_LOG_E("WriteString failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_IFACE_TXBYTES), data, reply);
    if (error != 0) {
        if (error != STATS_ERR_READ_BPF_FAIL) {
            NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        }
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetCellularRxBytes(uint64_t &stats)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_CELLULAR_RXBYTES), data, reply);
    if (error != 0) {
        if (error != STATS_ERR_GET_IFACE_NAME_FAILED) {
            NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        }
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetCellularTxBytes(uint64_t &stats)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_CELLULAR_TXBYTES), data, reply);
    if (error != 0) {
        if (error != STATS_ERR_GET_IFACE_NAME_FAILED) {
            NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        }
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetAllRxBytes(uint64_t &stats)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_ALL_RXBYTES), data, reply);
    if (error != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetAllTxBytes(uint64_t &stats)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_ALL_TXBYTES), data, reply);
    if (error != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetUidRxBytes(uint64_t &stats, uint32_t uid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("proxy uid%{public}d", uid);
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_UID_RXBYTES), data, reply);
    if (error != 0) {
        if (error != STATS_ERR_READ_BPF_FAIL) {
            NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        }
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetUidTxBytes(uint64_t &stats, uint32_t uid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("proxy uid%{public}d", uid);
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_UID_TXBYTES), data, reply);
    if (error != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                                  NetStatsInfo &statsInfo)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!(data.WriteString(iface) && data.WriteUint64(start) && data.WriteUint64(end))) {
        NETMGR_LOG_E("Write data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t sendResult =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_IFACE_STATS_DETAIL), data, reply);
    if (sendResult != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", sendResult);
        return sendResult;
    }
    if (!NetStatsInfo::Unmarshalling(reply, statsInfo)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return sendResult;
}

int32_t NetStatsServiceProxy::GetUidStatsDetail(const std::string &iface, uint32_t uid, uint64_t start, uint64_t end,
                                                NetStatsInfo &statsInfo)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!(data.WriteString(iface) && data.WriteUint32(uid) && data.WriteUint64(start) && data.WriteUint64(end))) {
        NETMGR_LOG_E("Write data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t sendResult =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_UID_STATS_DETAIL), data, reply);
    if (sendResult != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", sendResult);
        return sendResult;
    }
    if (!NetStatsInfo::Unmarshalling(reply, statsInfo)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return sendResult;
}

int32_t NetStatsServiceProxy::UpdateIfacesStats(const std::string &iface, uint64_t start, uint64_t end,
                                                const NetStatsInfo &stats)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!(data.WriteString(iface) && data.WriteUint64(start) && data.WriteUint64(end))) {
        NETMGR_LOG_E("Write data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!stats.Marshalling(data)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    return SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_UPDATE_IFACES_STATS), data, reply);
}

int32_t NetStatsServiceProxy::UpdateStatsData()
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    return SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_UPDATE_STATS_DATA), data, reply);
}

int32_t NetStatsServiceProxy::ResetFactory()
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    return SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_NSM_RESET_FACTORY), data, reply);
}

int32_t NetStatsServiceProxy::GetAllStatsInfo(std::vector<NetStatsInfo> &infos)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t result =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_ALL_STATS_INFO), data, reply);
    if (result != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", result);
        return result;
    }
    if (!NetStatsInfo::Unmarshalling(reply, infos)) {
        NETMGR_LOG_E("Read stats info failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return result;
}

int32_t NetStatsServiceProxy::GetAllSimStatsInfo(std::vector<NetStatsInfo> &infos)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t result =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_ALL_SIM_STATS_INFO), data, reply);
    if (result != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", result);
        return result;
    }
    if (!NetStatsInfo::Unmarshalling(reply, infos)) {
        NETMGR_LOG_E("Read stats info failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return result;
}

int32_t NetStatsServiceProxy::GetTrafficStatsByNetwork(std::unordered_map<uint32_t, NetStatsInfo> &infos,
                                                       const sptr<NetStatsNetwork> &network)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (network == nullptr) {
        NETMGR_LOG_E("network is nullptr");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    if (!network->Marshalling(data)) {
        NETMGR_LOG_E("proxy Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    NETMGR_LOG_D("proxy sptr<NetStatsNetwork> Marshalling success");
    MessageParcel reply;
    int32_t result =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_TRAFFIC_STATS_BY_NETWORK), data, reply);
    if (result != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", result);
        return result;
    }
    if (!NetStatsInfo::Unmarshalling(reply, infos)) {
        NETMGR_LOG_E("Read stats info failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return result;
}

int32_t NetStatsServiceProxy::GetMonthTrafficStatsByNetwork(uint32_t simId, uint64_t &monthData)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(simId)) {
        NETMGR_LOG_E("WriteUint32 simId failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t result =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_MONTH_TRAFFIC_STATS_BY_NETWORK), data, reply);
    if (result != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", result);
        return result;
    }
    if (!reply.ReadUint64(monthData)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return result;
}

int32_t NetStatsServiceProxy::GetTrafficStatsByUidNetwork(std::vector<NetStatsInfoSequence> &infos, uint32_t uid,
                                                          const sptr<NetStatsNetwork> &network)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (network == nullptr) {
        NETMGR_LOG_E("network is nullptr");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("WriteUint32 uid failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!network->Marshalling(data)) {
        NETMGR_LOG_E("sptr<NetStatsNetwork> Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    NETMGR_LOG_D("proxy sptr<NetStatsNetwork> Marshalling success");
    MessageParcel reply;
    int32_t ret =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_TRAFFIC_STATS_BY_UID_NETWORK), data, reply);
    if (ret != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", ret);
        return ret;
    }
    if (!NetStatsInfoSequence::Unmarshalling(reply, infos)) {
        NETMGR_LOG_E("Read stats info failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetStatsServiceProxy::SetAppStats(const PushStatsInfo &info)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!info.Marshalling(data)) {
        NETMGR_LOG_E("pushStatsInfo marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    NETMGR_LOG_D("PushStatsInfo Marshalling success");
    MessageParcel reply;
    int32_t ret = SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_SET_APP_STATS), data, reply);
    if (ret != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", ret);
        return ret;
    }
    return ret;
}

int32_t NetStatsServiceProxy::SetDpaAppStats(const NetStatsInfo &info)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!info.Marshalling(data)) {
        NETMGR_LOG_E("pushStatsInfo marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    NETMGR_LOG_D("DpaAppStats Marshalling success");
    MessageParcel reply;
    int32_t ret = SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_SET_DPA_APP_STATS), data, reply);
    if (ret != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", ret);
        return ret;
    }
    return ret;
}

int32_t NetStatsServiceProxy::SaveSharingTraffic(const NetStatsInfo &infos)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!infos.Marshalling(data)) {
        NETMGR_LOG_E("SaveSharingTraffic NetStatsInfo marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    MessageParcel reply;
    int32_t ret = SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_SET_SHARING_TRAFFIC_BEFORE_STOP),
        data, reply);
    if (ret != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", ret);
        return ret;
    }
    return ret;
}

int32_t NetStatsServiceProxy::GetCookieRxBytes(uint64_t &stats, uint64_t cookie)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteUint64(cookie)) {
        NETMGR_LOG_E("proxy cookie write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_COOKIE_RXBYTES), data, reply);
    if (error != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::GetCookieTxBytes(uint64_t &stats, uint64_t cookie)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteUint64(cookie)) {
        NETMGR_LOG_E("proxy cookie write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_COOKIE_TXBYTES), data, reply);
    if (error != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return error;
    }
    if (!reply.ReadUint64(stats)) {
        NETMGR_LOG_E("ReadUint64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return error;
}

int32_t NetStatsServiceProxy::SetCalibrationTraffic(uint32_t simId, int64_t remainingData, uint64_t totalMonthlyData)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(simId)) {
        NETMGR_LOG_E("proxy simId write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt64(remainingData)) {
        NETMGR_LOG_E("proxy remainingData write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteUint64(totalMonthlyData)) {
        NETMGR_LOG_E("proxy totalMonthlyData write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_SET_CALIBRATION_TRAFFIC), data, reply);
    if (ret != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, ret: [%{public}d]", ret);
    }
    return ret;
}

int32_t NetStatsServiceProxy::SetTrafficPlanInfo(int32_t simId, int32_t param, int64_t value)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(simId)) {
        NETMGR_LOG_E("proxy simId write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(param)) {
        NETMGR_LOG_E("proxy TrafficPlanParam write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt64(value)) {
        NETMGR_LOG_E("proxy value write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_SET_TRAFFIC_PLAN_INFO), data, reply);
    if (ret != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, ret: [%{public}d]", ret);
        return ret;
    }
    return ret;
}

int32_t NetStatsServiceProxy::GetTrafficPlanInfo(int32_t simId, int32_t param, int64_t &value)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(simId)) {
        NETMGR_LOG_E("proxy simId write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(param)) {
        NETMGR_LOG_E("proxy TrafficPlanParam write failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret =
        SendRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_TRAFFIC_PLAN_INFO), data, reply);
    if (ret != 0) {
        NETMGR_LOG_E("proxy SendRequest failed, ret: [%{public}d]", ret);
        return ret;
    }
    if (!reply.ReadInt64(value)) {
        NETMGR_LOG_E("ReadInt64 failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}
} // namespace NetManagerStandard
} // namespace OHOS

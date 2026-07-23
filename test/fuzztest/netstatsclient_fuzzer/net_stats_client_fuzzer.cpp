/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <ctime>
#include <thread>
#include <vector>

#include <securec.h>

#include "data_flow_statistics.h"
#include "inet_stats_service.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_client.h"
#include "net_stats_constants.h"
#define private public
#include "net_stats_service.h"
#include "net_stats_service_stub.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
const uint8_t *g_baseFuzzData = nullptr;
size_t g_baseFuzzSize = 0;
size_t g_baseFuzzPos;
constexpr size_t STR_LEN = 10;
} // namespace

template <class T> T NetStatsGetData()
{
    T object{};
    size_t netStatsSize = sizeof(object);
    if (g_baseFuzzData == nullptr || netStatsSize > g_baseFuzzSize - g_baseFuzzPos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, netStatsSize, g_baseFuzzData + g_baseFuzzPos, netStatsSize);
    if (ret != EOK) {
        return {};
    }
    g_baseFuzzPos += netStatsSize;
    return object;
}

std::string NetStatsGetString(int strlen)
{
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        cstr[i] = NetStatsGetData<char>();
    }
    std::string str(cstr);
    return str;
}

class INetStatsCallbackTest : public IRemoteStub<INetStatsCallback> {
public:
    int32_t NetIfaceStatsChanged(const std::string &iface)
    {
        return 0;
    }

    int32_t NetUidStatsChanged(const std::string &iface, uint32_t uid)
    {
        return 0;
    }
};

static bool g_isInited = false;

void Init()
{
    if (!g_isInited) {
        NetStatsService::GetInstance()->Init();
        g_isInited = true;
    } else {
        g_isInited = false;
    }
}

__attribute__((no_sanitize("cfi"))) int32_t OnRemoteRequest(uint32_t code, MessageParcel &data)
{
    if (!g_isInited) {
        Init();
    }

    MessageParcel reply;
    MessageOption option;

    int32_t ret = NetStatsService::GetInstance()->OnRemoteRequest(code, data, reply, option);
    return ret;
}

bool WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetStatsServiceStub::GetDescriptor())) {
        return false;
    }
    return true;
}

void CheckParamVaild(MessageParcel &dataParcel, const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    g_baseFuzzData = data;
    g_baseFuzzSize = size;
    g_baseFuzzPos = 0;

    if (!WriteInterfaceToken(dataParcel)) {
        return;
    }

    return;
}

void RegisterNetStatsCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    sptr<INetStatsCallbackTest> callback = new (std::nothrow) INetStatsCallbackTest();
    if (callback == nullptr) {
        return;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        return;
    }
    CheckParamVaild(dataParcel, data, size);
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_NSM_REGISTER_NET_STATS_CALLBACK), dataParcel);
}

void UnregisterNetStatsCallbackFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    sptr<INetStatsCallbackTest> callback = new (std::nothrow) INetStatsCallbackTest();
    if (callback == nullptr) {
        return;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        return;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());
    CheckParamVaild(dataParcel, data, size);

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_NSM_UNREGISTER_NET_STATS_CALLBACK), dataParcel);
}

__attribute__((no_sanitize("cfi"))) void GetIfaceRxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    std::string interfaceName = NetStatsGetString(STR_LEN);
    dataParcel.WriteString(interfaceName);

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_IFACE_RXBYTES), dataParcel);
}

void GetIfaceTxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    std::string interfaceName = NetStatsGetString(STR_LEN);
    dataParcel.WriteString(interfaceName);

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_IFACE_TXBYTES), dataParcel);
}

void GetUidRxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint32_t uid = NetStatsGetData<uint32_t>();
    dataParcel.WriteUint32(uid);

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_UID_RXBYTES), dataParcel);
}

void GetUidTxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint32_t uid = NetStatsGetData<uint32_t>();
    dataParcel.WriteUint32(uid);

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_UID_TXBYTES), dataParcel);
}

void GetCellularRxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_CELLULAR_RXBYTES), dataParcel);
}

void GetCellularTxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_CELLULAR_TXBYTES), dataParcel);
}

void GetAllRxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_ALL_RXBYTES), dataParcel);
}

void GetAllTxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_ALL_TXBYTES), dataParcel);
}

void GetIfaceStatsDetailFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    dataParcel.WriteString(NetStatsGetString(STR_LEN));
    dataParcel.WriteUint64(NetStatsGetData<uint64_t>());
    dataParcel.WriteUint64(NetStatsGetData<uint64_t>());

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_IFACE_STATS_DETAIL), dataParcel);
}

void GetUidStatsDetailFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    dataParcel.WriteString(NetStatsGetString(STR_LEN));
    dataParcel.WriteUint64(NetStatsGetData<uint32_t>());
    dataParcel.WriteUint64(NetStatsGetData<uint64_t>());
    dataParcel.WriteUint64(NetStatsGetData<uint64_t>());

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_UID_STATS_DETAIL), dataParcel);
}

void UpdateIfacesStatsFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    dataParcel.WriteString(NetStatsGetString(STR_LEN));
    dataParcel.WriteUint64(NetStatsGetData<uint64_t>());
    dataParcel.WriteUint64(NetStatsGetData<uint64_t>());
    NetStatsInfo stats;
    stats.iface_ = NetStatsGetString(STR_LEN);
    stats.uid_ = NetStatsGetData<uint32_t>();
    stats.date_ = NetStatsGetData<uint64_t>();
    stats.rxBytes_ = NetStatsGetData<uint64_t>();
    stats.txBytes_ = NetStatsGetData<uint64_t>();
    stats.rxPackets_ = NetStatsGetData<uint64_t>();
    stats.txPackets_ = NetStatsGetData<uint64_t>();
    stats.Marshalling(dataParcel);

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_UPDATE_IFACES_STATS), dataParcel);
}

void UpdateStatsDataFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_UPDATE_STATS_DATA), dataParcel);
}

void ResetFactoryFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_NSM_RESET_FACTORY), dataParcel);
}

void GetCookieRxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint64_t cookie = NetStatsGetData<uint64_t>();
    dataParcel.WriteUint32(cookie);

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_COOKIE_RXBYTES), dataParcel);
}

void GetCookieTxBytesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint64_t cookie = NetStatsGetData<uint64_t>();
    dataParcel.WriteUint32(cookie);

    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_COOKIE_TXBYTES), dataParcel);
}

void SetAppStatsFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint32_t uid = NetStatsGetData<uint32_t>();
    std::string iface = NetStatsGetString(STR_LEN);
    dataParcel.WriteUint32(uid);
    dataParcel.WriteString(iface);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_SET_APP_STATS), dataParcel);
}

void GetTrafficStatsByNetworkFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint32_t type = NetStatsGetData<uint32_t>();
    uint32_t startTime = NetStatsGetData<uint64_t>();
    uint32_t endTime = NetStatsGetData<uint64_t>();
    dataParcel.WriteUint32(type);
    dataParcel.WriteUint64(startTime);
    dataParcel.WriteUint64(endTime);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_TRAFFIC_STATS_BY_NETWORK), dataParcel);
}

void GetTrafficStatsByUidNetworkFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint32_t uid = NetStatsGetData<uint32_t>();
    uint32_t startTime = NetStatsGetData<uint64_t>();
    uint32_t endTime = NetStatsGetData<uint64_t>();
    dataParcel.WriteUint32(uid);
    dataParcel.WriteUint64(startTime);
    dataParcel.WriteUint64(endTime);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_TRAFFIC_STATS_BY_UID_NETWORK), dataParcel);
}

void GetMonthTrafficStatsByNetworkFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint32_t simId = NetStatsGetData<uint32_t>();
    uint32_t usedData = NetStatsGetData<uint64_t>();
    dataParcel.WriteUint32(simId);
    dataParcel.WriteUint64(usedData);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_MONTH_TRAFFIC_STATS_BY_NETWORK), dataParcel);
}

void SetCalibrationTrafficFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    uint32_t simId = NetStatsGetData<uint32_t>();
    int64_t remainData = NetStatsGetData<int64_t>();
    uint64_t tatalData = NetStatsGetData<uint64_t>();
    dataParcel.WriteUint32(simId);
    dataParcel.WriteInt64(remainData);
    dataParcel.WriteUint64(tatalData);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_SET_CALIBRATION_TRAFFIC), dataParcel);
}

void SetTrafficPlanInfoFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    int32_t simId = NetStatsGetData<int32_t>();
    int32_t param = NetStatsGetData<int32_t>();
    int64_t value = NetStatsGetData<int64_t>();
    dataParcel.WriteInt32(simId);
    dataParcel.WriteInt32(param);
    dataParcel.WriteInt64(value);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_SET_TRAFFIC_PLAN_INFO), dataParcel);
}

void GetTrafficPlanInfoFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    CheckParamVaild(dataParcel, data, size);
    int32_t simId = NetStatsGetData<int32_t>();
    int32_t param = NetStatsGetData<int32_t>();
    dataParcel.WriteInt32(simId);
    dataParcel.WriteInt32(param);
    OnRemoteRequest(static_cast<uint32_t>(StatsInterfaceCode::CMD_GET_TRAFFIC_PLAN_INFO), dataParcel);
}
} // namespace NetManagerStandard
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::NetManagerStandard::RegisterNetStatsCallbackFuzzTest(data, size);
    OHOS::NetManagerStandard::UnregisterNetStatsCallbackFuzzTest(data, size);
    OHOS::NetManagerStandard::GetIfaceRxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetIfaceTxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetUidRxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetUidTxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetCellularRxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetCellularTxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetAllRxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetAllTxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetIfaceStatsDetailFuzzTest(data, size);
    OHOS::NetManagerStandard::GetUidStatsDetailFuzzTest(data, size);
    OHOS::NetManagerStandard::UpdateIfacesStatsFuzzTest(data, size);
    OHOS::NetManagerStandard::ResetFactoryFuzzTest(data, size);
    OHOS::NetManagerStandard::GetCookieRxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetCookieTxBytesFuzzTest(data, size);
    OHOS::NetManagerStandard::SetAppStatsFuzzTest(data, size);
    OHOS::NetManagerStandard::GetTrafficStatsByNetworkFuzzTest(data, size);
    OHOS::NetManagerStandard::GetTrafficStatsByUidNetworkFuzzTest(data, size);
    OHOS::NetManagerStandard::GetMonthTrafficStatsByNetworkFuzzTest(data, size);
    OHOS::NetManagerStandard::SetCalibrationTrafficFuzzTest(data, size);
    OHOS::NetManagerStandard::SetTrafficPlanInfoFuzzTest(data, size);
    OHOS::NetManagerStandard::GetTrafficPlanInfoFuzzTest(data, size);
    return 0;
}

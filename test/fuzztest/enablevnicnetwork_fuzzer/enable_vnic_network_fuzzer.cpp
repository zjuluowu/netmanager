/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "enable_vnic_network_fuzzer.h"
#include "netmanager_base_test_security.h"
#define private public
#include "net_conn_service.h"
#include "net_link_info.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
const uint8_t *g_baseFuzzData = nullptr;
size_t g_baseFuzzSize = 0;
size_t g_baseFuzzPos;
constexpr size_t STR_LEN = 10;
}

template <class T> T GetData()
{
    T object{};
    size_t objectSize = sizeof(object);
    if (g_baseFuzzData == nullptr || objectSize > g_baseFuzzSize - g_baseFuzzPos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_baseFuzzData + g_baseFuzzPos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_baseFuzzPos += objectSize;
    return object;
}


bool WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetConnServiceStub::GetDescriptor())) {
        NETMGR_LOG_D("Write token failed.");
        return false;
    }
    return true;
}

static bool g_isInited = false;
void Init()
{
    if (!g_isInited) {
        if (!DelayedSingleton<NetConnService>::GetInstance()->Init()) {
            g_isInited = false;
        } else {
            g_isInited = true;
        }
    }
}

int32_t OnRemoteRequest(uint32_t code, MessageParcel &data)
{
    if (!g_isInited) {
        NETMGR_LOG_D("Net conn client fuzz test g_isInited is false.");
        Init();
    }

    MessageParcel reply;
    MessageOption option;

    int32_t ret = DelayedSingleton<NetConnService>::GetInstance()->OnRemoteRequest(code, data, reply, option);
    return ret;
}


bool IsConnClientDataAndSizeValid(const uint8_t *data, size_t size, MessageParcel &dataParcel)
{
    g_baseFuzzData = data;
    g_baseFuzzSize = size;
    g_baseFuzzPos = 0;

    if (!WriteInterfaceToken(dataParcel)) {
        return false;
    }
    return true;
}

void EnableVnicNetworkFuzzTest(const uint8_t *data, size_t size)
{
    NetManagerBaseAccessToken token;
    MessageParcel dataParcel;
    if (!IsConnClientDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t uidSize = GetData<int32_t>() % 23;
    dataParcel.WriteInt32(uidSize);
    for (uint32_t i = 0; i < uidSize; i++) {
        int32_t uid = GetData<int32_t>();
        dataParcel.WriteInt32(uid);
    }
    sptr<NetLinkInfo> netLinkInfo = sptr<NetLinkInfo>::MakeSptr();
    netLinkInfo->Marshalling(dataParcel);
    OnRemoteRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_ENABLE_VNIC_NET_WORK), dataParcel);
}
} // namespace NetManagerStandard
} // namespace OHOS
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::NetManagerStandard::EnableVnicNetworkFuzzTest(data, size);
    return 0;
}
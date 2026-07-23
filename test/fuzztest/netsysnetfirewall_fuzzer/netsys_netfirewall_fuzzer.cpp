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

#include <securec.h>
#include <thread>

#include "netsys_netfirewall_fuzzer.h"
#include "notify_callback_stub.h"
#include "singleton.h"
#include "system_ability_definition.h"
#include "netsys_native_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
const uint8_t *g_baseFuzzData = nullptr;
size_t g_baseFuzzSize = 0;
size_t g_baseFuzzPos;
constexpr size_t STR_LEN = 10;
constexpr uint32_t MAX_RULES = 15;
constexpr int32_t NUMBER_TWO = 2;
constexpr int32_t NUMBER_ONE = 1;
bool g_isWaitAsync = false;
} // namespace

template <class T> T NetFireWallGetData()
{
    T object{};
    size_t netFireWallSize = sizeof(object);
    if (g_baseFuzzData == nullptr || netFireWallSize > g_baseFuzzSize - g_baseFuzzPos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, netFireWallSize, g_baseFuzzData + g_baseFuzzPos, netFireWallSize);
    if (ret != EOK) {
        return {};
    }
    g_baseFuzzPos += netFireWallSize;
    return object;
}

std::string NetFireWallGetString(int strlen)
{
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        cstr[i] = NetFireWallGetData<char>();
    }
    std::string str(cstr);
    return str;
}

static bool g_isInited = false;
__attribute__((no_sanitize("cfi"))) void Init()
{
    nmd::IptablesWrapper::GetInstance();
    g_isInited = DelayedSingleton<NetsysNative::NetsysNativeService>::GetInstance()->Init();
}

__attribute__((no_sanitize("cfi"))) int32_t OnRemoteRequest(uint32_t code, MessageParcel &data)
{
    if (!g_isInited) {
        Init();
    }

    MessageParcel reply;
    MessageOption option;

    return DelayedSingleton<NetsysNative::NetsysNativeService>::GetInstance()->OnRemoteRequest(code, data, reply,
                                                                                               option);
}

bool WriteInterfaceToken(MessageParcel &data)
{
    return data.WriteInterfaceToken(NetsysNative::NetsysNativeServiceStub::GetDescriptor());
}

bool WriteInterfaceTokenCallback(MessageParcel &data)
{
    return data.WriteInterfaceToken(NetsysNative::NotifyCallbackStub::GetDescriptor());
}

bool IsDataAndSizeValid(const uint8_t *data, size_t size, MessageParcel &dataParcel)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    g_baseFuzzData = data;
    g_baseFuzzSize = size;
    g_baseFuzzPos = 0;

    return WriteInterfaceToken(dataParcel);
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
void InterfaceBitmapBuild001FuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("lo");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void InterfaceBitmapBuild002FuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void InterfaceBitmapBuild003FuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 2;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    for (int i = 0; i < numRules; i++) {
        dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
        dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
        dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
        dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
        dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
        dataParcel.WriteUint32(0);
        dataParcel.WriteUint32(0);
        dataParcel.WriteUint32(0);
        dataParcel.WriteUint32(0);
        dataParcel.WriteString("lo");
    }

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void InterfaceBitmapBuild004FuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 2;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("wlan0");

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("eth0");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void InterfaceBitmapBuild005FuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 2;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("wlan0");

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("eth0");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void InterfaceBitmapBuild006FuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 2;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_OUT));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("wlan0");

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_OUT));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void InterfaceBitmapBuild007FuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("lo");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);

    MessageParcel dataParcel2;
    if (!WriteInterfaceToken(dataParcel2)) {
        return;
    }
    int32_t clearType = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    dataParcel2.WriteInt32(clearType);
    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_CLEAR_RULES),
        dataParcel2);
}

void NetsysNetFirewallTest007FuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("lo");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void WriteInterfaceBpfMapEmptyFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 0;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void WriteInterfaceBpfMapLongNameFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("abcdefghijklmnopq");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void WriteInterfaceBpfMapEgressFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_OUT));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("eth0");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void InterfaceRebuildFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = false;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);

    MessageParcel dataParcel2;
    if (!WriteInterfaceToken(dataParcel2)) {
        return;
    }
    type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    numRules = 1;
    isFinish = true;

    dataParcel2.WriteInt32(type);
    dataParcel2.WriteUint32(numRules);
    dataParcel2.WriteBool(isFinish);
    dataParcel2.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel2.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel2.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel2.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel2.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel2.WriteUint32(0);
    dataParcel2.WriteUint32(0);
    dataParcel2.WriteUint32(0);
    dataParcel2.WriteUint32(0);
    dataParcel2.WriteString("lo");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel2);
}

void WriteInterfaceBpfMapIngressFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 2;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("lo");

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("eth0");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void BuildMarkBitmapAllowWithInterfaceFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_ALLOW));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("wlan0");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void BuildNoMarkBitmapMixedInterfaceFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 3;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_OUT));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("eth0");

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_OUT));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("");

    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_OUT));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("wlan0");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}

void HandleDebugEventFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    DebugEvent debugEv = {};
    debugEv.type = (enum debug_type)NetFireWallGetData<uint32_t>();
    debugEv.dir = (enum stream_dir)NetFireWallGetData<uint32_t>();
    debugEv.arg1 = NetFireWallGetData<uint32_t>();
    debugEv.arg2 = NetFireWallGetData<uint32_t>();
    NetsysBpfNetFirewall::HandleDebugEvent(&debugEv);
}

void ClearBpfFirewallRulesFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    dataParcel.WriteInt32(type);

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_CLEAR_RULES),
        dataParcel);
}

void WriteInterfaceBpfMapSuccessFuzzTest(const uint8_t *data, size_t size)
{
    MessageParcel dataParcel;
    if (!IsDataAndSizeValid(data, size, dataParcel)) {
        return;
    }

    int32_t type = static_cast<int32_t>(NetFirewallRuleType::RULE_IP);
    uint32_t numRules = 1;
    bool isFinish = true;

    dataParcel.WriteInt32(type);
    dataParcel.WriteUint32(numRules);
    dataParcel.WriteBool(isFinish);
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(NetFireWallGetData<int32_t>());
    dataParcel.WriteInt32(static_cast<int32_t>(NetFirewallRuleDirection::RULE_IN));
    dataParcel.WriteInt32(static_cast<int32_t>(FirewallRuleAction::RULE_DENY));
    dataParcel.WriteInt32(static_cast<int32_t>(NetworkProtocol::TCP));
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteUint32(0);
    dataParcel.WriteString("lo");

    OnRemoteRequest(
        static_cast<uint32_t>(NetsysNative::NetsysInterfaceCode::NETSYS_NET_FIREWALL_SET_RULES),
        dataParcel);
}
#endif // FEATURE_NET_FIREWALL_ENABLE
} // namespace NetManagerStandard
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
#ifdef FEATURE_NET_FIREWALL_ENABLE
    OHOS::NetManagerStandard::InterfaceBitmapBuild001FuzzTest(data, size);
    OHOS::NetManagerStandard::InterfaceBitmapBuild002FuzzTest(data, size);
    OHOS::NetManagerStandard::InterfaceBitmapBuild003FuzzTest(data, size);
    OHOS::NetManagerStandard::InterfaceBitmapBuild004FuzzTest(data, size);
    OHOS::NetManagerStandard::InterfaceBitmapBuild005FuzzTest(data, size);
    OHOS::NetManagerStandard::InterfaceBitmapBuild006FuzzTest(data, size);
    OHOS::NetManagerStandard::InterfaceBitmapBuild007FuzzTest(data, size);
    OHOS::NetManagerStandard::NetsysNetFirewallTest007FuzzTest(data, size);
    OHOS::NetManagerStandard::WriteInterfaceBpfMapEmptyFuzzTest(data, size);
    OHOS::NetManagerStandard::WriteInterfaceBpfMapLongNameFuzzTest(data, size);
    OHOS::NetManagerStandard::WriteInterfaceBpfMapEgressFuzzTest(data, size);
    OHOS::NetManagerStandard::InterfaceRebuildFuzzTest(data, size);
    OHOS::NetManagerStandard::WriteInterfaceBpfMapIngressFuzzTest(data, size);
    OHOS::NetManagerStandard::BuildMarkBitmapAllowWithInterfaceFuzzTest(data, size);
    OHOS::NetManagerStandard::BuildNoMarkBitmapMixedInterfaceFuzzTest(data, size);
    OHOS::NetManagerStandard::HandleDebugEventFuzzTest(data, size);
    OHOS::NetManagerStandard::ClearBpfFirewallRulesFuzzTest(data, size);
    OHOS::NetManagerStandard::WriteInterfaceBpfMapSuccessFuzzTest(data, size);
#endif
    return 0;
}
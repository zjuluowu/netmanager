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

#include "is_dead_flow_reset_target_bundle_fuzzer.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <string>

#include "securec.h"

#include "message_parcel.h"
#include "net_conn_service_proxy.h"
#include "net_conn_service_stub.h"
#include "net_conn_service_stub_test.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr size_t MAX_BUNDLE_NAME_LEN = 256;
constexpr uint8_t MODE_COUNT = 4;
constexpr uint8_t MODE_SKIP_TOKEN = 0;
constexpr uint8_t MODE_WRITE_EMPTY_BUNDLE = 2;
constexpr uint8_t MODE_WRITE_FUZZ_BUNDLE = 3;
const uint8_t *g_baseFuzzData = nullptr;
size_t g_baseFuzzSize = 0;
size_t g_baseFuzzPos = 0;

class DeadFlowResetTargetBundleFuzzStub : public MockNetConnServiceStub {
public:
    int32_t IsDeadFlowResetTargetBundle(const std::string &bundleName, bool &flag) override
    {
        flag = !bundleName.empty();
        return NETMANAGER_SUCCESS;
    }
};

bool InitFuzzData(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }
    g_baseFuzzData = data;
    g_baseFuzzSize = size;
    g_baseFuzzPos = 0;
    return true;
}

template <typename T>
T GetData()
{
    T value {};
    if (g_baseFuzzData == nullptr || g_baseFuzzPos >= g_baseFuzzSize ||
        sizeof(T) > g_baseFuzzSize - g_baseFuzzPos) {
        return value;
    }
    if (memcpy_s(&value, sizeof(T), g_baseFuzzData + g_baseFuzzPos, sizeof(T)) != EOK) {
        return T {};
    }
    g_baseFuzzPos += sizeof(T);
    return value;
}

std::string GetStringFromData()
{
    if (g_baseFuzzData == nullptr || g_baseFuzzPos >= g_baseFuzzSize) {
        return {};
    }
    size_t remainSize = g_baseFuzzSize - g_baseFuzzPos;
    size_t stringSize = std::min(static_cast<size_t>(GetData<uint8_t>()), remainSize);
    if (stringSize == 0 || g_baseFuzzPos + stringSize > g_baseFuzzSize) {
        return {};
    }
    std::string bundleName(reinterpret_cast<const char *>(g_baseFuzzData + g_baseFuzzPos), stringSize);
    g_baseFuzzPos += stringSize;
    if (bundleName.size() > MAX_BUNDLE_NAME_LEN) {
        bundleName.resize(MAX_BUNDLE_NAME_LEN);
    }
    return bundleName;
}

void ProxyIsDeadFlowResetTargetBundleFuzzTest(const uint8_t *data, size_t size)
{
    if (!InitFuzzData(data, size)) {
        return;
    }
    sptr<DeadFlowResetTargetBundleFuzzStub> stub = new (std::nothrow) DeadFlowResetTargetBundleFuzzStub();
    if (stub == nullptr) {
        return;
    }
    NetConnServiceProxy proxy(stub->AsObject());
    std::string bundleName = GetStringFromData();
    bool flag = GetData<bool>();
    (void)proxy.IsDeadFlowResetTargetBundle(bundleName, flag);
}

void StubOnRemoteRequestFuzzTest(const uint8_t *data, size_t size)
{
    if (!InitFuzzData(data, size)) {
        return;
    }
    sptr<DeadFlowResetTargetBundleFuzzStub> stub = new (std::nothrow) DeadFlowResetTargetBundleFuzzStub();
    if (stub == nullptr) {
        return;
    }

    MessageParcel request;
    MessageParcel reply;
    MessageOption option;
    uint8_t mode = GetData<uint8_t>() % MODE_COUNT;
    if (mode != MODE_SKIP_TOKEN) {
        (void)request.WriteInterfaceToken(NetConnServiceStub::GetDescriptor());
    }
    if (mode == MODE_WRITE_EMPTY_BUNDLE) {
        (void)request.WriteString("");
    }
    if (mode == MODE_WRITE_FUZZ_BUNDLE) {
        (void)request.WriteString(GetStringFromData());
    }

    (void)stub->OnRemoteRequest(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DEAD_FLOW_RESET_TARGET_BUNDLE),
        request, reply, option);
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    ProxyIsDeadFlowResetTargetBundleFuzzTest(data, size);
    StubOnRemoteRequestFuzzTest(data, size);
    return 0;
}
} // namespace NetManagerStandard
} // namespace OHOS
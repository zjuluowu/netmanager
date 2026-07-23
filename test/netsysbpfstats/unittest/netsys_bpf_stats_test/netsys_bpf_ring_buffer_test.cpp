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
#include <net/if.h>
#include <thread>
#include <vector>

#include <gtest/gtest.h>
#include <sys/resource.h>
#include <unistd.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "bpf_loader.h"
#include "bpf_mapper.h"
#include "bpf_path.h"
#include "bpf_ring_buffer.h"

#include "net_stats_constants.h"

namespace OHOS {
namespace NetManagerStandard {

using namespace testing::ext;

class NetsysBpfRingBufferTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();

protected:
    NetsysBpfRingBufferTest() = default;
};
void NetsysBpfRingBufferTest::SetUpTestCase() {}

void NetsysBpfRingBufferTest::TearDownTestCase() {}

void NetsysBpfRingBufferTest::SetUp() {}

void NetsysBpfRingBufferTest::TearDown() {}

HWTEST_F(NetsysBpfRingBufferTest, HandleNetworkPolicyEventCallbackTest001, TestSize.Level1)
{
    void *ctx = malloc(100);
    void *data = malloc(100);
    size_t dataSize = 1;

    std::unique_ptr<NetsysBpfRingBuffer> bpfringbuffer = std::make_unique<NetsysBpfRingBuffer>();
    EXPECT_LE(bpfringbuffer->HandleNetworkPolicyEventCallback(ctx, data, dataSize), 1);
    data = nullptr;
    EXPECT_EQ(bpfringbuffer->HandleNetworkPolicyEventCallback(ctx, data, dataSize), 1);
}

HWTEST_F(NetsysBpfRingBufferTest, RegisterNetsysTrafficCallbackTest001, TestSize.Level1)
{
    sptr<NetsysNative::INetsysTrafficCallback> callback;
    sptr<NetsysNative::INetsysTrafficCallback> callback1 = nullptr;

    std::unique_ptr<NetsysBpfRingBuffer> bpfringbuffer = std::make_unique<NetsysBpfRingBuffer>();
    EXPECT_EQ(bpfringbuffer->RegisterNetsysTrafficCallback(callback), 0);
    EXPECT_EQ(bpfringbuffer->RegisterNetsysTrafficCallback(callback1), 0);
    bpfringbuffer->callbacks_ = {callback, callback, callback, callback, callback, callback, callback,
                                 callback, callback, callback, callback, callback, callback, callback,
                                 callback, callback, callback, callback, callback, callback, callback};
    EXPECT_EQ(bpfringbuffer->RegisterNetsysTrafficCallback(callback), 0);
}

HWTEST_F(NetsysBpfRingBufferTest, UnRegisterNetsysTrafficCallbackTest001, TestSize.Level1)
{
    sptr<NetsysNative::INetsysTrafficCallback> callback;
    sptr<NetsysNative::INetsysTrafficCallback> callback1 = nullptr;

    std::unique_ptr<NetsysBpfRingBuffer> bpfringbuffer = std::make_unique<NetsysBpfRingBuffer>();
    bpfringbuffer->callbacks_ = {callback, callback1};
    EXPECT_EQ(bpfringbuffer->UnRegisterNetsysTrafficCallback(callback), 0);
}

HWTEST_F(NetsysBpfRingBufferTest, HandleNetStatsEventCallbackTest001, TestSize.Level1)
{
    void *ctx = malloc(100);
    void *data = malloc(100);
    size_t dataSize = 1;

    std::unique_ptr<NetsysBpfRingBuffer> bpfringbuffer = std::make_unique<NetsysBpfRingBuffer>();
    EXPECT_EQ(bpfringbuffer->HandleNetStatsEventCallback(ctx, data, dataSize), 0);
    dataSize = 0;
    EXPECT_EQ(bpfringbuffer->HandleNetStatsEventCallback(ctx, data, dataSize), 1);
    data = nullptr;
    dataSize = 1;
    EXPECT_EQ(bpfringbuffer->HandleNetStatsEventCallback(ctx, data, dataSize), 1);
    dataSize = 0;
    EXPECT_EQ(bpfringbuffer->HandleNetStatsEventCallback(ctx, data, dataSize), 1);
}

HWTEST_F(NetsysBpfRingBufferTest, ListenNetStatsRingBufferThreadTest001, TestSize.Level1)
{
    std::unique_ptr<NetsysBpfRingBuffer> bpfringbuffer = std::make_unique<NetsysBpfRingBuffer>();
    bpfringbuffer->existNetStatsThread_ = false;
    bpfringbuffer->ListenNetStatsRingBufferThread();
    EXPECT_NE(bpfringbuffer, nullptr);
}
} // namespace NetManagerStandard
} // namespace OHOS
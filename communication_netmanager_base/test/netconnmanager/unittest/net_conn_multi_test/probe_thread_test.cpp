/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <netdb.h>
#include <gtest/gtest.h>
#include <sys/socket.h>

#include "i_net_monitor_callback.h"
#include "net_manager_constants.h"
#define private public
#include "probe_thread.h"
#undef private

namespace OHOS {
namespace NetManagerStandard {

using namespace testing::ext;
constexpr uint32_t TEST_NETID = 999;
std::shared_ptr<TinyCountDownLatch> latch = std::make_shared<TinyCountDownLatch>(2);
std::shared_ptr<TinyCountDownLatch> latchAll = std::make_shared<TinyCountDownLatch>(4);
class ProbeThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<ProbeThread> instance_ =
        std::make_shared<ProbeThread>(TEST_NETID, BEARER_DEFAULT, NetLinkInfo(),
        latch, latchAll, PROBE_HTTP, "http://test/", "https://test/");
};

void ProbeThreadTest::SetUpTestCase() {}

void ProbeThreadTest::TearDownTestCase() {}

void ProbeThreadTest::SetUp() {}

void ProbeThreadTest::TearDown() {}

HWTEST_F(ProbeThreadTest, SendHttpProbe001, TestSize.Level1)
{
    instance_->SendHttpProbe(PROBE_HTTP_HTTPS);

    auto httpProbeInstance = std::move(instance_->httpProbe_);
    instance_->httpProbe_ = nullptr;
    instance_->SetXReqId("xReqId", 10);
    instance_->SendHttpProbe(PROBE_HTTP_HTTPS);
    instance_->httpProbe_ = std::move(httpProbeInstance);
    EXPECT_TRUE(instance_->httpProbe_ != nullptr);
}

} // NetManagerStandard
} // OHOS
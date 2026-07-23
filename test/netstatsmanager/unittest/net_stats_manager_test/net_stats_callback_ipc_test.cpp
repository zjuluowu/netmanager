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

#include <gtest/gtest.h>

#include "net_manager_constants.h"
#include "net_stats_callback_test.h"
#include "net_stats_callback.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *TEST_IFACE = "TEST_IFACE";
constexpr uint32_t TEST_UID = 4454;
} // namespace

class NetStatsCallbackIpcTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetStatsCallback> instance_ = std::make_shared<NetStatsCallback>();
    static inline sptr<INetStatsCallback> callback_ = nullptr;
};

void NetStatsCallbackIpcTest::SetUpTestCase()
{
    callback_ = new (std::nothrow) NetStatsCallbackTest();
}

void NetStatsCallbackIpcTest::TearDownTestCase() {}

void NetStatsCallbackIpcTest::SetUp() {}

void NetStatsCallbackIpcTest::TearDown() {}

HWTEST_F(NetStatsCallbackIpcTest, NotifyNetIfaceStatsChangedTest001, TestSize.Level1)
{
    instance_->RegisterNetStatsCallback(callback_);
    instance_->RegisterNetStatsCallback(callback_);
    instance_->RegisterNetStatsCallback(nullptr);
    instance_->UnregisterNetStatsCallback(callback_);
    instance_->UnregisterNetStatsCallback(callback_);
    instance_->UnregisterNetStatsCallback(nullptr);
    for (int16_t i = 0; i < LIMIT_STATS_CALLBACK_NUM; i++) {
        sptr<INetStatsCallback> callback = new (std::nothrow) NetStatsCallbackTest();
        instance_->RegisterNetStatsCallback(callback);
    }
    instance_->RegisterNetStatsCallback(callback_);

    int32_t ret = instance_->NotifyNetIfaceStatsChanged(TEST_IFACE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsCallbackIpcTest, NotifyNetUidStatsChangedTest001, TestSize.Level1)
{
    int32_t ret = instance_->NotifyNetUidStatsChanged(TEST_IFACE, TEST_UID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS
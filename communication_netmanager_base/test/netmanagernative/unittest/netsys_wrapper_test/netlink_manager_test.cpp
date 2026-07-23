/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "netlink_define.h"
#include "netlink_manager.h"
#include "notify_callback_stub.h"
#include "common_notify_callback_test.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
} // namespace

class NetlinkManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::unique_ptr<NetlinkManager> manager_ = nullptr;
};

void NetlinkManagerTest::SetUpTestCase()
{
    manager_ = std::make_unique<NetlinkManager>();
}

void NetlinkManagerTest::TearDownTestCase() {}

void NetlinkManagerTest::SetUp() {}

void NetlinkManagerTest::TearDown() {}

HWTEST_F(NetlinkManagerTest, StartListenerTest001, TestSize.Level1)
{
    int32_t ret = manager_->StartListener();
    EXPECT_EQ(ret, NetlinkResult::OK);
}

HWTEST_F(NetlinkManagerTest, RegisterNetlinkCallbackTest002, TestSize.Level1)
{
    sptr<NetsysNative::INotifyCallback> callback = nullptr;
    int32_t ret = manager_->RegisterNetlinkCallback(callback);
    EXPECT_NE(ret, NetlinkResult::OK);
    ret = manager_->UnregisterNetlinkCallback(callback);
    EXPECT_NE(ret, NetlinkResult::OK);

    callback = new (std::nothrow) NetsysNative::NotifyCallbackTest();
    ret = manager_->RegisterNetlinkCallback(callback);
    EXPECT_EQ(ret, NetlinkResult::OK);
    ret = manager_->UnregisterNetlinkCallback(callback);
    EXPECT_EQ(ret, NetlinkResult::OK);
}

HWTEST_F(NetlinkManagerTest, StopListenerTest003, TestSize.Level1)
{
    int32_t ret = manager_->StopListener();
    EXPECT_EQ(ret, NetlinkResult::OK);
}
} // namespace nmd
} // namespace OHOS

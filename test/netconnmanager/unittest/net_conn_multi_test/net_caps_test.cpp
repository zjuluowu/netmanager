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

#include "net_activate.h"
#include "net_conn_callback_stub.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr uint32_t INVALID_NET_CAP = 1000;
} // namespace

class NetCapsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::unique_ptr<NetCaps> instance_ = nullptr;
};

void NetCapsTest::SetUpTestCase()
{
    std::set<NetCap> caps = {NetCap::NET_CAPABILITY_MMS,
                             NetCap::NET_CAPABILITY_NOT_METERED,
                             NetCap::NET_CAPABILITY_INTERNET,
                             NetCap::NET_CAPABILITY_VALIDATED,
                             NetCap::NET_CAPABILITY_PORTAL,
                             NetCap::NET_CAPABILITY_INTERNAL_DEFAULT,
                             NetCap::NET_CAPABILITY_END};
    instance_ = std::make_unique<NetCaps>(caps);
}

void NetCapsTest::TearDownTestCase() {}

void NetCapsTest::SetUp() {}

void NetCapsTest::TearDown() {}

HWTEST_F(NetCapsTest, ConstructorTest001, TestSize.Level1)
{
    std::unique_ptr<NetCaps> caps = std::make_unique<NetCaps>();
    EXPECT_NE(caps, nullptr);
}

HWTEST_F(NetCapsTest, ConstructorTest002, TestSize.Level1)
{
    std::set<NetCap> caps = {NetCap::NET_CAPABILITY_MMS,
                             NetCap::NET_CAPABILITY_NOT_METERED,
                             NetCap::NET_CAPABILITY_INTERNET,
                             NetCap::NET_CAPABILITY_NOT_VPN,
                             NetCap::NET_CAPABILITY_VALIDATED,
                             NetCap::NET_CAPABILITY_PORTAL,
                             NetCap::NET_CAPABILITY_INTERNAL_DEFAULT,
                             NetCap::NET_CAPABILITY_END};
    std::unique_ptr<NetCaps> netCaps = std::make_unique<NetCaps>(caps);
    EXPECT_NE(netCaps, nullptr);
}

HWTEST_F(NetCapsTest, operatorTest001, TestSize.Level1)
{
    NetCaps lCaps;
    NetCaps rCaps;
    EXPECT_TRUE(lCaps == rCaps);
}

HWTEST_F(NetCapsTest, IsValidNetCapTest001, TestSize.Level1)
{
    auto result = instance_->IsValidNetCap(NetCap::NET_CAPABILITY_NOT_VPN);
    EXPECT_TRUE(result);
}

HWTEST_F(NetCapsTest, IsValidNetCapTest002, TestSize.Level1)
{
    auto result = instance_->IsValidNetCap(static_cast<NetCap>(INVALID_NET_CAP));
    EXPECT_FALSE(result);
}

HWTEST_F(NetCapsTest, IsValidNetCapTest003, TestSize.Level1)
{
    auto result = instance_->IsValidNetCap(NetCap::NET_CAPABILITY_INTERNAL_DEFAULT);
    EXPECT_TRUE(result);
}

HWTEST_F(NetCapsTest, InsertNetCapTest001, TestSize.Level1)
{
    instance_->InsertNetCap(NetCap::NET_CAPABILITY_VALIDATED);
    auto result = instance_->HasNetCap(NetCap::NET_CAPABILITY_VALIDATED);
    EXPECT_TRUE(result);
}

HWTEST_F(NetCapsTest, InsertNetCapTest002, TestSize.Level1)
{
    instance_->RemoveNetCap(NetCap::NET_CAPABILITY_VALIDATED);
    auto result = instance_->HasNetCap(NetCap::NET_CAPABILITY_VALIDATED);
    EXPECT_TRUE(!result);
}

HWTEST_F(NetCapsTest, HasNetCapTest001, TestSize.Level1)
{
    auto result = instance_->HasNetCap(NetCap::NET_CAPABILITY_VALIDATED);
    EXPECT_TRUE(!result);
}

HWTEST_F(NetCapsTest, HasNetCapTest002, TestSize.Level1)
{
    auto result = instance_->HasNetCap(NetCap::NET_CAPABILITY_NOT_VPN);
    EXPECT_FALSE(result);
}

HWTEST_F(NetCapsTest, HasNetCapsTest001, TestSize.Level1)
{
    std::set<NetCap> caps;
    auto result = instance_->HasNetCaps(caps);
    EXPECT_TRUE(result);
}

HWTEST_F(NetCapsTest, HasNetCapsTest002, TestSize.Level1)
{
    std::set<NetCap> caps = {NetCap::NET_CAPABILITY_NOT_VPN};
    auto result = instance_->HasNetCaps(caps);
    EXPECT_FALSE(result);
}

HWTEST_F(NetCapsTest, HasNetCapsTest003, TestSize.Level1)
{
    std::set<NetCap> caps = {NetCap::NET_CAPABILITY_INTERNAL_DEFAULT};
    auto result = instance_->HasNetCaps(caps);
    EXPECT_TRUE(result);
}

HWTEST_F(NetCapsTest, ToSetTest001, TestSize.Level1)
{
    auto result = instance_->ToSet();
    EXPECT_FALSE(result.empty());
}

} // namespace NetManagerStandard
} // namespace OHOS
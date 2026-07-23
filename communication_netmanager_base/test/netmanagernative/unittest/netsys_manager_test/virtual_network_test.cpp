/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "virtual_network.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
constexpr int32_t TEST_NET_ID = 1001;
} // namespace
class VirtualNetWorkTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<VirtualNetwork> instance_ = nullptr;
};

void VirtualNetWorkTest::SetUpTestCase()
{
    instance_ = std::make_shared<VirtualNetwork>(TEST_NET_ID, false);
}

void VirtualNetWorkTest::TearDownTestCase() {}

void VirtualNetWorkTest::SetUp() {}

void VirtualNetWorkTest::TearDown() {}

HWTEST_F(VirtualNetWorkTest, GetHasDns001, TestSize.Level1)
{
    auto result = instance_->GetHasDns();
    EXPECT_FALSE(result);
}

HWTEST_F(VirtualNetWorkTest, AddUids001, TestSize.Level1)
{
    std::vector<UidRange> uidVec;
    auto result = instance_->AddUids(uidVec);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VirtualNetWorkTest, RemoveUids001, TestSize.Level1)
{
    std::vector<UidRange> uidVec;
    auto result = instance_->RemoveUids(uidVec);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VirtualNetWorkTest, AddInterface001, TestSize.Level1)
{
    std::string iface = "wlan0";
    auto result = instance_->AddInterface(iface);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VirtualNetWorkTest, RemoveInterface001, TestSize.Level1)
{
    std::string iface = "wlan0";
    auto result = instance_->RemoveInterface(iface);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

#ifdef SUPPORT_SYSVPN
HWTEST_F(VirtualNetWorkTest, UpdateVpnRules001, TestSize.Level1)
{
    const std::string ipAddr = "172.0.0.1";
    std::vector<std::string> extMessages;
    extMessages.emplace_back(ipAddr);
    int32_t result = instance_->UpdateVpnRules(extMessages, true);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    result = instance_->UpdateVpnRules(extMessages, false);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VirtualNetWorkTest, UpdateVpnRules002, TestSize.Level1)
{
    const std::string ipAddr = "172.0.1";
    std::vector<std::string> extMessages;
    extMessages.emplace_back(ipAddr);
    int32_t result = instance_->UpdateVpnRules(extMessages, true);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    result = instance_->UpdateVpnRules(extMessages, true);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}
#endif // SUPPORT_SYSVPN
} // namespace nmd
} // namespace OHOS
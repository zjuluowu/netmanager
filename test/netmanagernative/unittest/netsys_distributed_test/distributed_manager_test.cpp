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

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "distributed_manager.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *DISTRIBUTED_TUN_CARD_NAME = "virnic";
} // namespace

class DistributedManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DistributedManagerTest::SetUpTestCase() {}

void DistributedManagerTest::TearDownTestCase() {}

void DistributedManagerTest::SetUp() {}

void DistributedManagerTest::TearDown() {}

HWTEST_F(DistributedManagerTest, ConfigVirnicAndVeth001, TestSize.Level1)
{
    std::string virNicAddr = "";
    std::string virnicName = "";
    std::string virnicVethName = "1";
    auto result = DistributedManager::GetInstance().ConfigVirnicAndVeth(virNicAddr, virnicName, virnicVethName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    virnicName = "1";
    virnicVethName = "";
    result = DistributedManager::GetInstance().ConfigVirnicAndVeth(virNicAddr, virnicName, virnicVethName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    virnicName = "";
    result = DistributedManager::GetInstance().ConfigVirnicAndVeth(virNicAddr, virnicName, virnicVethName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    virnicName = "1";
    virnicVethName = "1";
    result = DistributedManager::GetInstance().ConfigVirnicAndVeth(virNicAddr, virnicName, virnicVethName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    virNicAddr = "192.168.1.1";
    virnicName = "virnic0";
    virnicVethName = "virnic1";
    result = DistributedManager::GetInstance().ConfigVirnicAndVeth(virNicAddr, virnicName, virnicVethName);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <random>
#include <thread>
#include <unistd.h>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_policy_file.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr uint32_t MAX_LIST_SIZE = 10;
constexpr uint32_t SLEEP_SECOND_TIME = 5;
} // namespace
std::shared_ptr<NetPolicyFile> netPolicyFile_ = nullptr;

using namespace testing::ext;
class UtNetPolicyFile : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::set<uint32_t> white_;
    std::set<uint32_t> black_;
};

void UtNetPolicyFile::SetUpTestCase()
{
    netPolicyFile_ = DelayedSingleton<NetPolicyFile>::GetInstance();
    ASSERT_TRUE(DelayedSingleton<NetPolicyFile>::GetInstance());
    netPolicyFile_->InitPolicy();
}

void UtNetPolicyFile::TearDownTestCase()
{
    sleep(SLEEP_SECOND_TIME);
    netPolicyFile_.reset();
}

void UtNetPolicyFile::SetUp()
{
    netPolicyFile_->ReadFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, white_, black_);
}

void UtNetPolicyFile::TearDown()
{
    netPolicyFile_->WriteFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, white_, black_);
}

/**
 * @tc.name: NetPolicyFile001
 * @tc.desc: Test NetPolicyFile ReadFirewallRules.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFile, NetPolicyFile001, TestSize.Level1)
{
    std::set<uint32_t> allowedList;
    std::set<uint32_t> deniedList;
    for (uint32_t i = 0; i <= MAX_LIST_SIZE; i++) {
        allowedList.insert(i);
        deniedList.insert(i);
    }

    netPolicyFile_->WriteFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, allowedList, deniedList);
    std::set<uint32_t> allowedList1;
    std::set<uint32_t> deniedList1;
    netPolicyFile_->ReadFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, allowedList1, deniedList1);
    ASSERT_TRUE(allowedList == allowedList1);
    ASSERT_TRUE(deniedList == deniedList1);
}

/**
 * @tc.name: NetPolicyFile002
 * @tc.desc: Test NetPolicyFile WriteFirewallRules.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFile, NetPolicyFile002, TestSize.Level1)
{
    std::set<uint32_t> allowedList;
    std::set<uint32_t> deniedList;
    for (uint32_t i = 0; i <= MAX_LIST_SIZE; i++) {
        allowedList.insert(i);
        deniedList.insert(i);
    }
    netPolicyFile_->WriteFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, allowedList, deniedList);
    sleep(SLEEP_SECOND_TIME);
    netPolicyFile_->InitPolicy();
    std::set<uint32_t> allowedList1;
    std::set<uint32_t> deniedList1;
    netPolicyFile_->ReadFirewallRules(FIREWALL_CHAIN_DEVICE_IDLE, allowedList1, deniedList1);
    ASSERT_TRUE(deniedList == deniedList1);
}

/**
 * @tc.name: Json2Obj001
 * @tc.desc: Test Json2Obj WriteFirewallRules.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFile, Json2Obj001, TestSize.Level1)
{
    std::string content = "";
    NetPolicy netPolicy;
    auto result = netPolicyFile_->Json2Obj(content, netPolicy);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: AddBackgroundPolicy001
 * @tc.desc: Test AddBackgroundPolicy WriteFirewallRules.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFile, AddBackgroundPolicy001, TestSize.Level1)
{
    netPolicyFile_->netPolicy_.backgroundPolicyStatus = "";
    netPolicyFile_->AddBackgroundPolicy(nullptr);
    EXPECT_NE(netPolicyFile_->netPolicy_.backgroundPolicyStatus, "");
}

HWTEST_F(UtNetPolicyFile, GetHandler001, TestSize.Level1)
{
    auto netPolicyFileInstance = std::make_shared<NetPolicyFile>();
    auto handler = netPolicyFileInstance->GetHandler();
    ASSERT_NE(handler, nullptr);
    EXPECT_TRUE(handler->Write());
}
} // namespace NetManagerStandard
} // namespace OHOS

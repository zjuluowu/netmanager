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

#include <thread>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_mgr_log_wrapper.h"
#include "net_policy_callback_test.h"
#include "net_policy_client.h"
#include "net_policy_constants.h"
#include "net_policy_firewall.h"
#include "net_policy_inner_define.h"

namespace OHOS {
namespace NetManagerStandard {
const std::string TEST_STRING_PERIODDURATION = "M1";
const std::string ICCID_1 = "sim_abcdefg_1";
const std::string ICCID_2 = "sim_abcdefg_2";

static std::shared_ptr<NetPolicyFirewall> netPolicyFirewall_ = nullptr;

using namespace testing::ext;
class UtNetPolicyFirewall : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<NetPolicyCallbackTest> GetINetPolicyCallbackSample() const;
};

void UtNetPolicyFirewall::SetUpTestCase()
{
    netPolicyFirewall_ = std::make_shared<NetPolicyFirewall>();
    netPolicyFirewall_->Init();
}

void UtNetPolicyFirewall::TearDownTestCase()
{
    netPolicyFirewall_.reset();
}

void UtNetPolicyFirewall::SetUp() {}

void UtNetPolicyFirewall::TearDown() {}

/**
 * @tc.name: NetPolicyFirewall001
 * @tc.desc: Test NetPolicyFirewall SetDeviceIdleTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, NetPolicyFirewall001, TestSize.Level1)
{
    const uint32_t uid = 123;
    netPolicyFirewall_->SetDeviceIdleTrustlist({uid}, false);
    std::vector<uint32_t> allowedList;
    netPolicyFirewall_->GetDeviceIdleTrustlist(allowedList);
    ASSERT_TRUE(std::find(allowedList.begin(), allowedList.end(), uid) == allowedList.end());
}

/**
 * @tc.name: NetPolicyFirewall002
 * @tc.desc: Test NetPolicyFirewall GetDeviceIdleTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, NetPolicyFirewall002, TestSize.Level1)
{
    const uint32_t uid = 456;
    netPolicyFirewall_->SetDeviceIdleTrustlist({uid}, true);
    std::vector<uint32_t> allowedList;
    netPolicyFirewall_->GetDeviceIdleTrustlist(allowedList);
    ASSERT_TRUE(std::find(allowedList.begin(), allowedList.end(), uid) != allowedList.end());
}

/**
 * @tc.name: NetPolicyFirewall003
 * @tc.desc: Test NetPolicyFirewall UpdateDeviceIdlePolicy
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, NetPolicyFirewall003, TestSize.Level1)
{
    const uint32_t uid = 789;
    netPolicyFirewall_->SetDeviceIdleTrustlist({uid}, true);
    netPolicyFirewall_->UpdateDeviceIdlePolicy(true);
    std::vector<uint32_t> allowedList;
    netPolicyFirewall_->GetDeviceIdleTrustlist(allowedList);
    ASSERT_TRUE(std::find(allowedList.begin(), allowedList.end(), uid) != allowedList.end());
}

/**
 * @tc.name: NetPolicyFirewall004
 * @tc.desc: Test NetPolicyFirewall ResetPolicies
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, NetPolicyFirewall004, TestSize.Level1)
{
    netPolicyFirewall_->ResetPolicies();
    std::vector<uint32_t> allowedList;
    netPolicyFirewall_->GetDeviceIdleTrustlist(allowedList);
    ASSERT_TRUE(allowedList.size() == 0);
}

/**
 * @tc.name: NetPolicyFirewall005
 * @tc.desc: Test NetPolicyFirewall HandleEvent.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, NetPolicyFirewall005, TestSize.Level1)
{
    const uint32_t uid = 101;
    netPolicyFirewall_->SetDeviceIdleTrustlist({uid}, true);
    std::vector<uint32_t> allowedList;
    netPolicyFirewall_->GetDeviceIdleTrustlist(allowedList);
    ASSERT_TRUE(std::find(allowedList.begin(), allowedList.end(), uid) != allowedList.end());
    auto policyEvent = std::make_shared<PolicyEvent>();
    policyEvent->deletedUid = uid;
    netPolicyFirewall_->HandleEvent(NetPolicyEventHandler::MSG_UID_REMOVED, policyEvent);
    netPolicyFirewall_->GetDeviceIdleTrustlist(allowedList);
    ASSERT_TRUE(std::find(allowedList.begin(), allowedList.end(), uid) == allowedList.end());
}

/**
 * @tc.name: NetPolicyFirewall006
 * @tc.desc: Test NetPolicyFirewall UpdateDeviceIdlePolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, NetPolicyFirewall006, TestSize.Level1)
{
    netPolicyFirewall_->UpdateDeviceIdlePolicy(false);
    netPolicyFirewall_->UpdateDeviceIdlePolicy(true);
    int32_t ret = netPolicyFirewall_->UpdateDeviceIdlePolicy(true);
    EXPECT_EQ(ret, NETMANAGER_ERR_STATUS_EXIST);
}

/**
 * @tc.name: SetDeviceIdleTrustlist001
 * @tc.desc: Test SetDeviceIdleTrustlist Func.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, SetDeviceIdleTrustlist001, TestSize.Level1)
{
    const uint32_t uid = 101;
    std::vector<uint32_t> initialUids(1001);
    for (size_t i = 0; i < initialUids.size(); ++i) {
        initialUids[i] = i;
    }
    for (uint32_t testuid : initialUids) {
        netPolicyFirewall_->powerSaveAllowedList_.insert(testuid);
    }
    auto result = netPolicyFirewall_->SetDeviceIdleTrustlist({uid}, true);
    EXPECT_EQ(result, NETMANAGER_ERR_PARAMETER_ERROR);
}

/**
 * @tc.name: UpdateFirewallPolicyList001
 * @tc.desc: Test UpdateFirewallPolicyList Func.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, UpdateFirewallPolicyList001, TestSize.Level1)
{
    uint32_t chainType = 17;
    std::vector<uint32_t> uids = {1, 2, 3};
    bool isAllowed = true;
    netPolicyFirewall_->powerSaveAllowedList_.clear();
    netPolicyFirewall_->UpdateFirewallPolicyList(chainType, uids, isAllowed);
    EXPECT_EQ(netPolicyFirewall_->powerSaveAllowedList_.size(), 3);
}

/**
 * @tc.name: UpdatePowerSavePolicy001
 * @tc.desc: Test UpdatePowerSavePolicy Func.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, UpdatePowerSavePolicy001, TestSize.Level1)
{
    bool enable = false;
    netPolicyFirewall_->powerSaveMode_ = true;
    auto result = netPolicyFirewall_->UpdatePowerSavePolicy(enable);
    EXPECT_NE(result, NETMANAGER_ERR_STATUS_EXIST);
    EXPECT_NE(result, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.name: ResetPolicies001
 * @tc.desc: Test ResetPolicies ResetPolicies Func.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, ResetPolicies001, TestSize.Level1)
{
    EXPECT_NE(netPolicyFirewall_->powerSaveFirewallRule_, nullptr);
    netPolicyFirewall_->powerSaveFirewallRule_ = nullptr;
    netPolicyFirewall_->ResetPolicies();
}

/**
 * @tc.name: HandleEvent001
 * @tc.desc: Test NetPolicyFirewall HandleEvent Func.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, HandleEvent001, TestSize.Level1)
{
    const uint32_t uid = 101;
    netPolicyFirewall_->SetDeviceIdleTrustlist({uid}, true);
    std::vector<uint32_t> allowedList;
    netPolicyFirewall_->GetDeviceIdleTrustlist(allowedList);
    ASSERT_TRUE(std::find(allowedList.begin(), allowedList.end(), uid) != allowedList.end());
    auto policyEvent = std::make_shared<PolicyEvent>();
    netPolicyFirewall_->HandleEvent(NetPolicyEventHandler::MSG_POWER_SAVE_MODE_CHANGED, policyEvent);
    netPolicyFirewall_->HandleEvent(NetPolicyEventHandler::MSG_DEVICE_IDLE_MODE_CHANGED, policyEvent);
    netPolicyFirewall_->HandleEvent(NetPolicyEventHandler::MSG_DEVICE_IDLE_LIST_UPDATED, policyEvent);
    ASSERT_FALSE(std::find(allowedList.begin(), allowedList.end(), uid) == allowedList.end());
}

/**
 * @tc.name: UpdateIdleDenyPolicy001
 * @tc.desc: Test NetPolicyFirewall UpdateIdleDenyPolicy Func.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, UpdateIdleDenyPolicy001, TestSize.Level1)
{
    netPolicyFirewall_->UpdateIdleDenyPolicy(false);
    netPolicyFirewall_->UpdateIdleDenyPolicy(true);
    int32_t ret = netPolicyFirewall_->UpdateIdleDenyPolicy(true);
    EXPECT_EQ(ret, NETMANAGER_ERR_STATUS_EXIST);
}

/**
 * @tc.name: UpdateIdleDenyPolicy001
 * @tc.desc: Test NetPolicyFirewall SetUidsDeniedListChain Func.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyFirewall, SetUidsDeniedListChain001, TestSize.Level1)
{
    std::vector<uint32_t> uids(1001, 1);
    int32_t ret = netPolicyFirewall_->SetUidsDeniedListChain(uids, true);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    std::vector<uint32_t> uid2 {1};
    ret = netPolicyFirewall_->SetUidsDeniedListChain(uid2, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = netPolicyFirewall_->SetUidsDeniedListChain(uid2, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyFirewall, SetPowerSaveTrustlist001, TestSize.Level1)
{
    std::vector<uint32_t> uids(1001);
    bool isAllowed = true;
    for (uint32_t i = 0; i < 1001; ++i) {
        netPolicyFirewall_->powerSaveAllowedList_.insert(i);
    }
    int32_t ret = netPolicyFirewall_->SetPowerSaveTrustlist(uids, isAllowed);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    netPolicyFirewall_->powerSaveAllowedList_.clear();
}
} // namespace NetManagerStandard
} // namespace OHOS

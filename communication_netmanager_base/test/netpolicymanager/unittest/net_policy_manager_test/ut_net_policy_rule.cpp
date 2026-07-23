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

#include <thread>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif
#include "net_policy_callback_test.h"
#include "net_policy_client.h"
#include "net_policy_firewall.h"
#include "net_policy_rule.h"
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr int32_t WAIT_TIME_SECOND_LONG = 10;
constexpr int32_t WAIT_TIME_THIRTY_SECOND_LONG = 30;
constexpr int32_t INVALID_VALUE = 100;
constexpr uint32_t TEST_UID1 = 200;
constexpr uint32_t TEST_UID2 = 13000;
std::shared_ptr<NetPolicyRule> g_netPolicyRule = nullptr;
std::shared_ptr<NetPolicyFirewall> g_netPolicyFirewallR = nullptr;
} // namespace

class UtNetPolicyRule : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<NetPolicyCallbackTest> GetINetPolicyCallbackSample() const;
};

void UtNetPolicyRule::SetUpTestCase()
{
    g_netPolicyRule = std::make_shared<NetPolicyRule>();
    g_netPolicyFirewallR = std::make_shared<NetPolicyFirewall>();
    g_netPolicyRule->Init();
    g_netPolicyRule->ResetPolicies();
}

void UtNetPolicyRule::TearDownTestCase()
{
    g_netPolicyRule->TransPolicyToRule(TEST_UID2, NetUidPolicy::NET_POLICY_NONE);
    g_netPolicyRule->TransPolicyToRule(TEST_UID1, NetUidPolicy::NET_POLICY_NONE);
    g_netPolicyRule.reset();
}

void UtNetPolicyRule::SetUp() {}

void UtNetPolicyRule::TearDown() {}

sptr<NetPolicyCallbackTest> UtNetPolicyRule::GetINetPolicyCallbackSample() const
{
    sptr<NetPolicyCallbackTest> callbackR = new (std::nothrow) NetPolicyCallbackTest();
    return callbackR;
}

/**
 * @tc.name: NetPolicyRule001
 * @tc.desc: Test NetPolicyRule TransPolicyToRule.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRule001, TestSize.Level1)
{
    int32_t result = g_netPolicyRule->TransPolicyToRule(10000, 1);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
    result = g_netPolicyRule->TransPolicyToRule(10000, 1);  // set same policy
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetPolicyRule002
 * @tc.desc: Test NetPolicyRule IsUidNetAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRule002, TestSize.Level1)
{
    int32_t result = g_netPolicyRule->TransPolicyToRule(15000, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
    bool isAllowed = false;
    g_netPolicyRule->IsUidNetAllowed(15000, false, isAllowed);
    ASSERT_TRUE(isAllowed);
}

/**
 * @tc.name: NetPolicyRule003
 * @tc.desc: Test NetPolicyRule GetPolicyByUid.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRule003, TestSize.Level1)
{
    int32_t result = g_netPolicyRule->TransPolicyToRule(16000, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
    uint32_t policy = 0;
    g_netPolicyRule->GetPolicyByUid(16000, policy);
    ASSERT_EQ(policy, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
}

/**
 * @tc.name: NetPolicyRule004
 * @tc.desc: Test NetPolicyRule GetUidsByPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRule004, TestSize.Level1)
{
    int32_t result = g_netPolicyRule->TransPolicyToRule(16000, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
    int32_t result2 = g_netPolicyRule->TransPolicyToRule(17000, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
    ASSERT_EQ(result2, NETMANAGER_SUCCESS);
    int32_t result3 = g_netPolicyRule->TransPolicyToRule(18000, NetUidPolicy::NET_POLICY_REJECT_METERED_BACKGROUND);
    ASSERT_EQ(result3, NETMANAGER_SUCCESS);
    int32_t result4 = g_netPolicyRule->TransPolicyToRule(19000, NetUidPolicy::NET_POLICY_REJECT_METERED_BACKGROUND);
    ASSERT_EQ(result4, NETMANAGER_SUCCESS);

    std::vector<uint32_t> uids;
    g_netPolicyRule->GetUidsByPolicy(NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND, uids);

    bool result5 = false;
    bool result6 = false;
    for (const auto &i : uids) {
        if (i == 16000) {
            result5 = true;
        }
    }

    for (const auto &i : uids) {
        if (i == 17000) {
            result6 = true;
        }
    }
    ASSERT_TRUE(result5 && result6);
    result5 = false;
    result6 = false;
    g_netPolicyRule->GetUidsByPolicy(NetUidPolicy::NET_POLICY_REJECT_METERED_BACKGROUND, uids);
    for (const auto &i : uids) {
        if (i == 18000) {
            result5 = true;
        }
    }

    for (const auto &i : uids) {
        if (i == 19000) {
            result6 = true;
        }
    }
    ASSERT_TRUE(result5 && result6);
}

HWTEST_F(UtNetPolicyRule, TransPolicyToRule001, TestSize.Level1)
{
    auto netPolicyRule = std::make_shared<NetPolicyRule>();
    UidPolicyRule uidPolicyRule;
    netPolicyRule->uidPolicyRules_.emplace(2000, uidPolicyRule);
    netPolicyRule->TransPolicyToRule(2000);
    EXPECT_NE(netPolicyRule->uidPolicyRules_.find(2000), netPolicyRule->uidPolicyRules_.end());
}

/**
 * @tc.name: NetPolicyRule005
 * @tc.desc: Test NetPolicyRule ResetPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRule005, TestSize.Level1)
{
    int32_t result = g_netPolicyRule->TransPolicyToRule(TEST_UID2, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);

    int32_t result2 = g_netPolicyRule->SetBackgroundPolicy(false);
    ASSERT_EQ(result2, NETMANAGER_SUCCESS);

    int32_t result3 = g_netPolicyRule->ResetPolicies();
    ASSERT_EQ(result3, NETMANAGER_SUCCESS);

    uint32_t policy = 0;
    g_netPolicyRule->GetPolicyByUid(TEST_UID2, policy);
    ASSERT_EQ(policy, NET_POLICY_NONE);
    bool backgroundPolicy = false;
    g_netPolicyRule->GetBackgroundPolicy(backgroundPolicy);
    ASSERT_TRUE(backgroundPolicy);
}

/**
 * @tc.name: NetPolicyRule006
 * @tc.desc: Test NetPolicyRule SetBackgroundPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRule006, TestSize.Level1)
{
    int32_t result = g_netPolicyRule->TransPolicyToRule(TEST_UID2, NetUidPolicy::NET_POLICY_REJECT_METERED_BACKGROUND);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
    uint32_t backgroundPolicyOfUid = 0;
    g_netPolicyRule->GetBackgroundPolicyByUid(TEST_UID2, backgroundPolicyOfUid);
    ASSERT_EQ(backgroundPolicyOfUid, NetBackgroundPolicy::NET_BACKGROUND_POLICY_DISABLE);
}

/**
 * @tc.name: NetPolicyRule007
 * @tc.desc: Test NetPolicyRule GetBackgroundPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRule007, TestSize.Level1)
{
    g_netPolicyRule->SetBackgroundPolicy(false);
    int32_t result = g_netPolicyRule->SetBackgroundPolicy(true);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
    bool backgroundPolicy;
    g_netPolicyRule->GetBackgroundPolicy(backgroundPolicy);
    ASSERT_TRUE(backgroundPolicy);
}

void SetPolicyUid()
{
    NetManagerBaseAccessToken token;
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetPolicyByUid(
        TEST_UID1, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

void SendMessage()
{
    NetManagerBaseAccessToken token;
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetDeviceIdlePolicy(true);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
    int32_t result2 = DelayedSingleton<NetPolicyClient>::GetInstance()->SetDeviceIdleTrustlist({TEST_UID1}, true);
    ASSERT_EQ(result2, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetPolicyRule008
 * @tc.desc: Test NetPolicyRule HandleEvent.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRule008, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetDeviceIdlePolicy(false);
    sptr<NetPolicyCallbackTest> callback = GetINetPolicyCallbackSample();
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->RegisterNetPolicyCallback(callback);
    uint32_t rule = 0;
    uint32_t rule2 = 0;
    if (result == NETMANAGER_SUCCESS) {
        std::thread setPolicy(SetPolicyUid);
        callback->WaitFor(WAIT_TIME_SECOND_LONG);
        setPolicy.join();
        rule = callback->GetRule();
        std::cout << "rule:" << rule << std::endl;
    } else {
        std::cout << "RegisterNetPolicyCallback failed!" << std::endl;
    }
    NetManagerBaseAccessToken token1;
    int32_t result2 = DelayedSingleton<NetPolicyClient>::GetInstance()->UnregisterNetPolicyCallback(callback);
    ASSERT_EQ(result2, NETMANAGER_SUCCESS);

    NetManagerBaseAccessToken token2;
    sptr<NetPolicyCallbackTest> callbackR = GetINetPolicyCallbackSample();
    int32_t result3 = DelayedSingleton<NetPolicyClient>::GetInstance()->RegisterNetPolicyCallback(callbackR);
    if (result3 == NETMANAGER_SUCCESS) {
        std::thread sendMessage(SendMessage);
        callbackR->WaitFor(WAIT_TIME_THIRTY_SECOND_LONG);
        sendMessage.join();
        rule2 = callbackR->GetRule();
        std::cout << "rule2:" << rule2 << std::endl;
    } else {
        std::cout << "RegisterNetPolicyCallbackR failed!" << std::endl;
    }
    NetManagerBaseAccessToken token3;
    int32_t result4 = DelayedSingleton<NetPolicyClient>::GetInstance()->UnregisterNetPolicyCallback(callbackR);
    ASSERT_EQ(result4, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetPolicyRuleBranchTest001
 * @tc.desc: Test NetPolicyRule Branch.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetPolicyRuleBranchTest001, TestSize.Level1)
{
    g_netPolicyRule->DeleteUid(TEST_UID2);

    auto policyEvent = std::make_shared<PolicyEvent>();
    int32_t eventId = static_cast<int32_t>(NetPolicyEventHandler::MSG_UID_REMOVED);
    g_netPolicyRule->HandleEvent(eventId, policyEvent);

    eventId = static_cast<int32_t>(NetPolicyEventHandler::MSG_UID_STATE_FOREGROUND);
    g_netPolicyRule->HandleEvent(eventId, policyEvent);

    eventId = static_cast<int32_t>(NetPolicyEventHandler::MSG_UID_STATE_BACKGROUND);
    g_netPolicyRule->HandleEvent(eventId, policyEvent);

    eventId = INVALID_VALUE;
    g_netPolicyRule->HandleEvent(eventId, policyEvent);

    g_netPolicyRule->UpdateForegroundUidList(TEST_UID2, false);
    g_netPolicyRule->UpdateForegroundUidList(TEST_UID2, true);

    std::string message = "";
    g_netPolicyRule->GetDumpMessage(message);

    bool ret = g_netPolicyRule->IsValidNetPolicy(INVALID_VALUE);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: BuildTransCondition001
 * @tc.desc: Test NetPolicyRule BuildTransCondition.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, BuildTransCondition001, TestSize.Level1)
{
    uint32_t uid = 2;
    uint32_t policy = 2;
    NetPolicyRule netpolicyrule;
    netpolicyrule.deviceIdleMode_ = true;
    netpolicyrule.deviceIdleAllowedList_ = {1, 2, 3};
    netpolicyrule.powerSaveMode_ = true;
    netpolicyrule.powerSaveAllowedList_ = {1, 2, 3};
    auto ret = netpolicyrule.BuildTransCondition(uid, policy);
    EXPECT_NE(ret, 0);
}

/**
 * @tc.name: NetsysCtrl001
 * @tc.desc: Test NetPolicyRule NetsysCtrl.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetsysCtrl001, TestSize.Level1)
{
    uint32_t uid = 2;
    uint32_t netsysCtrl = POLICY_TRANS_CTRL_NONE;
    NetPolicyRule netpolicyrule;
    netpolicyrule.NetsysCtrl(uid, netsysCtrl);
    EXPECT_EQ(netpolicyrule.powerSaveMode_, false);
    netsysCtrl = 0;
    netpolicyrule.NetsysCtrl(uid, netsysCtrl);
    EXPECT_EQ(netpolicyrule.powerSaveMode_, false);
}

/**
 * @tc.name: ProcessCtrlNone001
 * @tc.desc: Test NetPolicyRule ProcessCtrlNone.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, ProcessCtrlNone001, TestSize.Level1)
{
    uint32_t uid = 2;
    NetPolicyRule netpolicyrule;
    netpolicyrule.ProcessCtrlNone(uid);
    netpolicyrule.powerSaveMode_ = true;
    netpolicyrule.ProcessCtrlNone(uid);
    EXPECT_EQ(netpolicyrule.powerSaveMode_, true);
}

/**
 * @tc.name: ProcessCtrlAddAllowedList001
 * @tc.desc: Test NetPolicyRule ProcessCtrlAddAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, ProcessCtrlAddAllowedList001, TestSize.Level1)
{
    uint32_t uid = 2;
    NetPolicyRule netpolicyrule;
    netpolicyrule.powerSaveMode_ = true;
    netpolicyrule.ProcessCtrlAddAllowedList(uid);
    EXPECT_EQ(netpolicyrule.powerSaveMode_, true);
}

/**
 * @tc.name: GetUidsByPolicy001
 * @tc.desc: Test NetPolicyRule GetUidsByPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, GetUidsByPolicy001, TestSize.Level1)
{
    uint32_t policy = 3;
    std::vector<uint32_t> uids = {1, 2, 3};
    NetPolicyRule netpolicyrule;
    EXPECT_EQ(netpolicyrule.GetUidsByPolicy(policy, uids), POLICY_ERR_INVALID_POLICY);
}

/**
 * @tc.name: IsUidNetAllowed001
 * @tc.desc: Test NetPolicyRule IsUidNetAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, IsUidNetAllowed001, TestSize.Level1)
{
    uint32_t uid = 1;
    bool metered = true;
    bool isAllowed = true;
    UidPolicyRule x1, x2, x3, x4, x5;
    x1.rule_ = NET_RULE_REJECT_ALL;
    x2.rule_ = NET_RULE_REJECT_METERED;
    x3.rule_ = NET_RULE_ALLOW_METERED;
    x4.rule_ = NET_RULE_ALLOW_METERED_FOREGROUND;
    NetPolicyRule netpolicyrule;
    netpolicyrule.uidPolicyRules_ = {
        {1, {x1}},
        {2, {x2}},
        {3, {x3}},
        {4, {x4}},
        {5, {x5}}
        };
    netpolicyrule.IsUidNetAllowed(uid, metered, isAllowed);
    EXPECT_EQ(isAllowed, false);
    uid = 2;
    netpolicyrule.IsUidNetAllowed(uid, metered, isAllowed);
    EXPECT_EQ(isAllowed, false);
    uid = 3;
    netpolicyrule.IsUidNetAllowed(uid, metered, isAllowed);
    EXPECT_EQ(isAllowed, true);
    uid = 4;
    netpolicyrule.IsUidNetAllowed(uid, metered, isAllowed);
    EXPECT_EQ(isAllowed, true);
    uid = 5;
    netpolicyrule.backgroundAllow_ = false;
    netpolicyrule.IsUidNetAllowed(uid, metered, isAllowed);
    EXPECT_EQ(isAllowed, false);
}

/**
 * @tc.name: GetBackgroundPolicyByUid001
 * @tc.desc: Test NetPolicyRule GetBackgroundPolicyByUid.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, GetBackgroundPolicyByUid001, TestSize.Level1)
{
    uint32_t uid = 1;
    uint32_t backgroundPolicyOfUid = 0;
    NetPolicyRule netpolicyrule;
    netpolicyrule.backgroundAllow_ = false;
    netpolicyrule.GetBackgroundPolicyByUid(uid, backgroundPolicyOfUid);
    EXPECT_EQ(backgroundPolicyOfUid, NET_BACKGROUND_POLICY_DISABLE);
}

/**
 * @tc.name: InIdleAllowedList001
 * @tc.desc: Test NetPolicyRule InIdleAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, InIdleAllowedList001, TestSize.Level1)
{
    uint32_t uid = 1;
    NetPolicyRule netpolicyrule;
    netpolicyrule.deviceIdleAllowedList_ = {1, 2, 3};
    EXPECT_TRUE(netpolicyrule.InIdleAllowedList(uid));
}

/**
 * @tc.name: HandleEvent001
 * @tc.desc: Test NetPolicyRule HandleEvent.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, HandleEvent001, TestSize.Level1)
{
    int32_t eventId = 1;
    std::shared_ptr<PolicyEvent> policyEvent = std::make_shared<PolicyEvent>();
    std::set<uint32_t> devicelist = {1, 2, 3};
    policyEvent->deviceIdleList = devicelist;
    std::set<uint32_t> powersavelist = {1, 2, 3};
    policyEvent->powerSaveList = powersavelist;
    policyEvent->deviceIdleMode = true;
    policyEvent->powerSaveMode = true;
    NetPolicyRule netpolicyrule;
    netpolicyrule.HandleEvent(eventId, policyEvent);
    eventId = 2;
    netpolicyrule.HandleEvent(eventId, policyEvent);
    eventId = 3;
    netpolicyrule.HandleEvent(eventId, policyEvent);
    eventId = 5;
    netpolicyrule.HandleEvent(eventId, policyEvent);
    EXPECT_TRUE(netpolicyrule.deviceIdleMode_);
}

/**
 * @tc.name: NetsysCtrl002
 * @tc.desc: Test NetPolicyRule NetsysCtrl.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyRule, NetsysCtrl002, TestSize.Level1)
{
    uint32_t uid = 2;
    uint32_t netsysCtrl = 3;
    NetPolicyRule netpolicyrule;
    netpolicyrule.NetsysCtrl(uid, netsysCtrl);
    EXPECT_EQ(netpolicyrule.powerSaveMode_, false);
}
} // namespace NetManagerStandard
} // namespace OHOS

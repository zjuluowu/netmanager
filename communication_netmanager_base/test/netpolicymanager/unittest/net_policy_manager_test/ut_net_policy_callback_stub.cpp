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
#include <iostream>

#include "net_manager_constants.h"
#include "net_policy_callback_stub.h"
#include "net_policy_inner_define.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr uint32_t TEST_UID = 4454;
constexpr uint32_t TEST_POLICY = 2121;
constexpr uint32_t TEST_RULE = 441;
class NetPolicyCbStubTest : public NetPolicyCallbackStub {
public:
    NetPolicyCbStubTest() = default;
    ~NetPolicyCbStubTest() override {}
    int32_t NetUidPolicyChange(uint32_t uid, uint32_t policy) override
    {
        return 0;
    }

    int32_t NetUidRuleChange(uint32_t uid, uint32_t rule) override
    {
        std::cout << "licheng--Stub NetUidRuleChange" << std::endl;
        return 0;
    }

    int32_t NetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies) override
    {
        return 0;
    }

    int32_t NetStrategySwitch(const std::string &simId, bool enable) override
    {
        return 0;
    }

    int32_t NetMeteredIfacesChange(std::vector<std::string> &ifaces) override
    {
        return 0;
    }

    int32_t NetBackgroundPolicyChange(bool isBackgroundPolicyAllow) override
    {
        return 0;
    }
};
} // namespace

class UtNetPolicyCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetPolicyCbStubTest> instance_ = std::make_shared<NetPolicyCbStubTest>();
};

void UtNetPolicyCallbackStubTest::SetUpTestCase() {}

void UtNetPolicyCallbackStubTest::TearDownTestCase() {}

void UtNetPolicyCallbackStubTest::SetUp() {}

void UtNetPolicyCallbackStubTest::TearDown() {}

HWTEST_F(UtNetPolicyCallbackStubTest, OnNetUidPolicyChangeTest001, TestSize.Level1)
{
    uint32_t uid = TEST_UID;
    uint32_t policy = TEST_POLICY;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteUint32(uid), false);
    ASSERT_NE(data.WriteUint32(policy), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(100, data, reply, option);
    EXPECT_NE(ret, 0);

    ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyCallbackInterfaceCode::NOTIFY_NET_UID_POLICY_CHANGE),
                                     data, reply, option);
    EXPECT_NE(ret, 0);

    MessageParcel dataOk;
    ASSERT_NE(dataOk.WriteInterfaceToken(NetPolicyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(dataOk.WriteUint32(uid), false);
    ASSERT_NE(dataOk.WriteUint32(policy), false);
    ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyCallbackInterfaceCode::NOTIFY_NET_UID_POLICY_CHANGE),
                                     dataOk, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyCallbackStubTest, OnNetUidRuleChangeTest001, TestSize.Level1)
{
    uint32_t uid = TEST_UID;
    uint32_t rule = TEST_RULE;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteUint32(uid), false);
    ASSERT_NE(data.WriteUint32(rule), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyCallbackInterfaceCode::NOTIFY_NET_UID_RULE_CHANGE), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyCallbackStubTest, OnNetQuotaPolicyChangeTest001, TestSize.Level1)
{
    std::vector<NetQuotaPolicy> cellularPolicies;
    NetQuotaPolicy policy;
    policy.networkmatchrule.simId = "testIccid";
    policy.networkmatchrule.ident = "testIdent";
    policy.quotapolicy.title = "testTitle";
    cellularPolicies.push_back(policy);
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyCallbackStub::GetDescriptor()), false);
    if (!NetQuotaPolicy::Marshalling(data, cellularPolicies)) {
        return;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyCallbackInterfaceCode::NOTIFY_NET_QUOTA_POLICY_CHANGE), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyCallbackStubTest, OnNetMeteredIfacesChangeTest001, TestSize.Level1)
{
    std::vector<std::string> ifNames;
    ifNames.push_back("test0");
    ifNames.push_back("test1");
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyCallbackStub::GetDescriptor()), false);
    if (!data.WriteUint32(ifNames.size())) {
        return;
    }
    for (int32_t idx = 0; idx < ifNames.size(); idx++) {
        if (!data.WriteString(ifNames[idx])) {
            return;
        }
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyCallbackInterfaceCode::NOTIFY_NET_METERED_IFACES_CHANGE), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyCallbackStubTest, NotifyNetQuotaPolicyChangeTest002, TestSize.Level1)
{
    bool isBackgroundPolicyAllow = false;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyCallbackStub::GetDescriptor()), false);
    if (!data.WriteBool(isBackgroundPolicyAllow)) {
        return;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyCallbackInterfaceCode::NOTIFY_BACKGROUND_POLICY_CHANGE), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

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

#include <gtest/gtest.h>

#include "net_manager_constants.h"
#include "net_policy_callback.h"
#include "net_policy_callback_test.h"
#include "net_policy_inner_define.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr uint32_t TEST_UID = 4454;
constexpr uint32_t TEST_POLICY = 2121;
constexpr uint32_t TEST_RULE = 441;
std::map<int32_t, sptr<INetPolicyCallback>> g_callbackMap;
} // namespace

class UtNetPolicyCallbackIpcTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetPolicyCallback> instance_ = DelayedSingleton<NetPolicyCallback>::GetInstance();
    static inline sptr<INetPolicyCallback> callback_ = nullptr;
};

void UtNetPolicyCallbackIpcTest::SetUpTestCase()
{
    callback_ = new (std::nothrow) NetPolicyCallbackTest();
    for (int32_t i = 0; i < LIMIT_CALLBACK_NUM; i++) {
        g_callbackMap[i] = new (std::nothrow) NetPolicyCallbackTest();
    }
}

void UtNetPolicyCallbackIpcTest::TearDownTestCase()
{
    instance_->UnregisterNetPolicyCallback(callback_);
}

void UtNetPolicyCallbackIpcTest::SetUp() {}

void UtNetPolicyCallbackIpcTest::TearDown() {}

HWTEST_F(UtNetPolicyCallbackIpcTest, RegisterNetStatsCallbackTest001, TestSize.Level1)
{
    int32_t ret = instance_->RegisterNetPolicyCallback(callback_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->RegisterNetPolicyCallback(callback_);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    ret = instance_->RegisterNetPolicyCallback(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    ret = instance_->UnregisterNetPolicyCallback(callback_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->UnregisterNetPolicyCallback(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    std::for_each(g_callbackMap.begin(), g_callbackMap.end(),
                  [this](const auto &pair) { instance_->RegisterNetPolicyCallback(pair.second); });
    ret = instance_->RegisterNetPolicyCallback(callback_);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    std::for_each(g_callbackMap.begin(), g_callbackMap.end(),
                  [this](const auto &pair) { instance_->UnregisterNetPolicyCallback(pair.second); });
    instance_->RegisterNetPolicyCallback(callback_);
}

HWTEST_F(UtNetPolicyCallbackIpcTest, NotifyNetUidPolicyChangeTest001, TestSize.Level1)
{
    int32_t ret = instance_->NotifyNetUidPolicyChange(TEST_UID, TEST_POLICY);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyCallbackIpcTest, NotifyNetUidRuleChangeTest001, TestSize.Level1)
{
    int32_t ret = instance_->NotifyNetUidRuleChange(TEST_UID, TEST_RULE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyCallbackIpcTest, NotifyNetQuotaPolicyChangeTest001, TestSize.Level1)
{
    std::vector<NetQuotaPolicy> quotaPolicies;
    int32_t ret = instance_->NotifyNetQuotaPolicyChange(quotaPolicies);
    EXPECT_EQ(ret, POLICY_ERR_QUOTA_POLICY_NOT_EXIST);
}

HWTEST_F(UtNetPolicyCallbackIpcTest, NotifyNetQuotaPolicyChangeTest002, TestSize.Level1)
{
    NetQuotaPolicy quota;
    std::vector<NetQuotaPolicy> quotaPolicies;
    quotaPolicies.push_back(quota);
    int32_t ret = instance_->NotifyNetQuotaPolicyChange(quotaPolicies);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyCallbackIpcTest, NotifyNetMeteredIfacesChangeTest001, TestSize.Level1)
{
    std::vector<std::string> ifaces = {};
    int32_t ret = instance_->NotifyNetMeteredIfacesChange(ifaces);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyCallbackIpcTest, NotifyNetBackgroundPolicyChangeTest001, TestSize.Level1)
{
    int32_t ret = instance_->NotifyNetBackgroundPolicyChange(false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

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
#include "netsys_policy_wrapper.h"
#include "iptables_type.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *POLICY_FILE_NAME = "/data/service/el1/public/netmanager/net_policy.json";
using namespace testing::ext;
} // namespace

class NetsysPolicyWrapperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline auto instance_ = DelayedSingleton<NetsysPolicyWrapper>::GetInstance();
};

void NetsysPolicyWrapperTest::SetUpTestCase() {}

void NetsysPolicyWrapperTest::TearDownTestCase() {}

void NetsysPolicyWrapperTest::SetUp() {}

void NetsysPolicyWrapperTest::TearDown() {}

HWTEST_F(NetsysPolicyWrapperTest, RegisterNetsysCallbackTest001, TestSize.Level1)
{
    sptr<NetsysControllerCallback> callback = nullptr;
    auto ret = instance_->RegisterNetsysCallback(callback);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, BandwidthEnableDataSaverTest001, TestSize.Level1)
{
    auto ret = instance_->BandwidthEnableDataSaver(false);
    EXPECT_LE(ret, 0);
}

HWTEST_F(NetsysPolicyWrapperTest, BandwidthSetIfaceQuotaTest001, TestSize.Level1)
{
    std::string iface = "testIface";
    int64_t bytes = 666;
    auto ret = instance_->BandwidthSetIfaceQuota(iface, bytes);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, BandwidthRemoveIfaceQuotaTest001, TestSize.Level1)
{
    std::string iface = "testIface";
    auto ret = instance_->BandwidthRemoveIfaceQuota(iface);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, BandwidthAddDeniedListTest001, TestSize.Level1)
{
    uint32_t uid = 666;
    auto ret = instance_->BandwidthAddDeniedList(uid);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, BandwidthRemoveDeniedListTest001, TestSize.Level1)
{
    uint32_t uid = 666;
    auto ret = instance_->BandwidthRemoveDeniedList(uid);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, BandwidthAddAllowedListTest001, TestSize.Level1)
{
    uint32_t uid = 666;
    auto ret = instance_->BandwidthAddAllowedList(uid);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, BandwidthRemoveAllowedListTest001, TestSize.Level1)
{
    uint32_t uid = 666;
    auto ret = instance_->BandwidthRemoveAllowedList(uid);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, PowerSaveUpdataAllowedListTest001, TestSize.Level1)
{
    uint32_t uid = 666;
    auto ret = instance_->PowerSaveUpdataAllowedList(uid, FirewallRule::RULE_ALLOW);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
    ret = instance_->PowerSaveUpdataAllowedList(uid, FirewallRule::RULE_DENY);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, FirewallSetUidsAllowedListChainTest001, TestSize.Level1)
{
    uint32_t chain = 2;
    std::vector<uint32_t> uids;
    auto ret = instance_->FirewallSetUidsAllowedListChain(chain, uids);
    EXPECT_LE(ret, 0);
}

HWTEST_F(NetsysPolicyWrapperTest, FirewallSetUidsDeniedListChainTest001, TestSize.Level1)
{
    uint32_t chain = 2;
    std::vector<uint32_t> uids;
    auto ret = instance_->FirewallSetUidsDeniedListChain(chain, uids);
    EXPECT_LE(ret, 0);
}

HWTEST_F(NetsysPolicyWrapperTest, FirewallSetUidRuleTest001, TestSize.Level1)
{
    uint32_t chain = 2;
    uint32_t uid = 666;
    uint32_t firewallRule = 2;
    auto ret = instance_->FirewallSetUidRule(chain, {uid}, firewallRule);
    EXPECT_LE(ret, 0);
}

HWTEST_F(NetsysPolicyWrapperTest, FirewallEnableChainTest001, TestSize.Level1)
{
    uint32_t chain = 2;
    auto ret = instance_->FirewallEnableChain(chain, false);
    EXPECT_LE(ret, 0);
    std::remove(POLICY_FILE_NAME);
}

HWTEST_F(NetsysPolicyWrapperTest, SetNetworkAccessPolicyTest001, TestSize.Level1)
{
    uint32_t uid = 666;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    auto ret = instance_->SetNetworkAccessPolicy(uid, netAccessPolicy, reconfirmFlag);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysPolicyWrapperTest, DeleteNetworkAccessPolicyTest001, TestSize.Level1)
{
    uint32_t uid = 666;
    auto ret = instance_->DeleteNetworkAccessPolicy(uid);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS
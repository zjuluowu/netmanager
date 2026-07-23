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

#define private public
#include "bandwidth_manager.h"
#undef private
#include "iptables_type.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_controller.h"
#include "netsys_native_service_proxy.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace NetManagerStandard;
using namespace nmd;
std::shared_ptr<BandwidthManager> g_BandwidthManager = nullptr;

class BandwidthManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BandwidthManagerTest::SetUpTestCase()
{
    g_BandwidthManager = std::make_shared<BandwidthManager>();
}

void BandwidthManagerTest::TearDownTestCase()
{
    g_BandwidthManager.reset();
}

void BandwidthManagerTest::SetUp() {}

void BandwidthManagerTest::TearDown() {}

/**
 * @tc.name: BandwidthInnerFuctionTest
 * @tc.desc: Test BandwidthManager BandwidthInnerFuctionTest.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthInnerFuctionTest, TestSize.Level1)
{
    if (!g_BandwidthManager->chainInitFlag_) {
        int32_t ret = g_BandwidthManager->InitChain();
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
        ret = g_BandwidthManager->InitDefaultRules();
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    }
    ChainType chain = ChainType::CHAIN_OHBW_INPUT;
    std::string resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "ohbw_INPUT"), true);

    chain = ChainType::CHAIN_OHBW_OUTPUT;
    resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "ohbw_OUTPUT"), true);

    chain = ChainType::CHAIN_OHBW_FORWARD;
    resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "ohbw_FORWARD"), true);

    chain = ChainType::CHAIN_OHBW_DENIED_LIST_BOX;
    resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "ohbw_denied_list_box"), true);

    chain = ChainType::CHAIN_OHBW_ALLOWED_LIST_BOX;
    resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "ohbw_allowed_list_box"), true);

    chain = ChainType::CHAIN_OHBW_GLOBAL_ALERT;
    resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "ohbw_global_alert"), true);

    chain = ChainType::CHAIN_OHBW_COSTLY_SHARED;
    resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "ohbw_costly_shared"), true);

    chain = ChainType::CHAIN_OHBW_DATA_SAVER;
    resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "ohbw_data_saver"), true);

    chain = ChainType::CHAIN_OHFW_UNDOZABLE;
    resultStr = g_BandwidthManager->FetchChainName(chain);
    EXPECT_EQ((resultStr == "oh_unusable"), true);
    g_BandwidthManager->DeInitChain();
}

/**
 * @tc.name: IptablesNewChain000
 * @tc.desc: Test BandwidthManager IptablesNewChain.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, IptablesNewChain000, TestSize.Level1)
{
    std::string chainName = g_BandwidthManager->FetchChainName(ChainType::CHAIN_OHFW_INPUT);
    int32_t ret = g_BandwidthManager->IptablesNewChain(chainName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: IptablesDeleteChain000
 * @tc.desc: Test BandwidthManager IptablesDeleteChain.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, IptablesDeleteChain000, TestSize.Level1)
{
    std::string chainName = g_BandwidthManager->FetchChainName(ChainType::CHAIN_OHFW_INPUT);
    int32_t ret = g_BandwidthManager->IptablesDeleteChain(chainName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetGlobalAlert000
 * @tc.desc: Test BandwidthManager SetGlobalAlert.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetGlobalAlert000, TestSize.Level1)
{
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetGlobalAlert(BandwidthManager::OP_SET, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetGlobalAlert001
 * @tc.desc: Test BandwidthManager SetGlobalAlert.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetGlobalAlert001, TestSize.Level1)
{
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetGlobalAlert(BandwidthManager::Operate::OP_UNSET, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetGlobalAlert002
 * @tc.desc: Test BandwidthManager SetGlobalAlert.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetGlobalAlert002, TestSize.Level1)
{
    int64_t bytes = 1;
    int32_t ret = g_BandwidthManager->SetGlobalAlert(BandwidthManager::Operate::OP_SET, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetGlobalAlert003
 * @tc.desc: Test BandwidthManager SetGlobalAlert.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetGlobalAlert003, TestSize.Level1)
{
    int64_t bytes = 1;
    int32_t ret = g_BandwidthManager->SetGlobalAlert(BandwidthManager::Operate::OP_UNSET, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetCostlyAlert000
 * @tc.desc: Test BandwidthManager SetCostlyAlert.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetCostlyAlert000, TestSize.Level1)
{
    std::string iface = "wlan0";
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetCostlyAlert(BandwidthManager::Operate::OP_SET, iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetCostlyAlert001
 * @tc.desc: Test BandwidthManager SetCostlyAlert.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetCostlyAlert001, TestSize.Level1)
{
    std::string iface = "wlan0";
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetCostlyAlert(BandwidthManager::Operate::OP_SET, iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = g_BandwidthManager->SetCostlyAlert(BandwidthManager::Operate::OP_UNSET, iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetCostlyAlert002
 * @tc.desc: Test BandwidthManager SetCostlyAlert.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetCostlyAlert002, TestSize.Level1)
{
    std::string iface = "wlan0";
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetCostlyAlert(BandwidthManager::Operate::OP_SET, iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    bytes = 100;
    ret = g_BandwidthManager->SetCostlyAlert(BandwidthManager::Operate::OP_UNSET, iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: EnableDataSaver000
 * @tc.desc: Test BandwidthManager EnableDataSaver.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, EnableDataSaver000, TestSize.Level1)
{
    int32_t ret = g_BandwidthManager->EnableDataSaver(false);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: EnableDataSaver001
 * @tc.desc: Test BandwidthManager EnableDataSaver.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, EnableDataSaver001, TestSize.Level1)
{
    int32_t ret = g_BandwidthManager->EnableDataSaver(true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = g_BandwidthManager->EnableDataSaver(false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetIfaceQuota000
 * @tc.desc: Test BandwidthManager SetIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetIfaceQuota000, TestSize.Level1)
{
    std::string iface = "wlan0";
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetIfaceQuota(iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetIfaceQuota001
 * @tc.desc: Test BandwidthManager SetIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetIfaceQuota001, TestSize.Level1)
{
    std::string iface = "// /.";
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetIfaceQuota(iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: SetIfaceQuota002
 * @tc.desc: Test BandwidthManager SetIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, SetIfaceQuota002, TestSize.Level1)
{
    std::string iface = "wlan0";
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetIfaceQuota(iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    bytes = 1;
    ret = g_BandwidthManager->SetIfaceQuota(iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveIfaceQuota000
 * @tc.desc: Test BandwidthManager RemoveIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveIfaceQuota000, TestSize.Level1)
{
    std::string iface = "// /.";
    int32_t ret = g_BandwidthManager->RemoveIfaceQuota(iface);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: RemoveIfaceQuota001
 * @tc.desc: Test BandwidthManager RemoveIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveIfaceQuota001, TestSize.Level1)
{
    std::string iface = "wlan0";
    int32_t ret = g_BandwidthManager->RemoveIfaceQuota(iface);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveIfaceQuota002
 * @tc.desc: Test BandwidthManager RemoveIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveIfaceQuota002, TestSize.Level1)
{
    std::string iface = "wlan0";
    int64_t bytes = 0;
    int32_t ret = g_BandwidthManager->SetIfaceQuota(iface, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = g_BandwidthManager->RemoveIfaceQuota(iface);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: AddDeniedList000
 * @tc.desc: Test BandwidthManager AddDeniedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, AddDeniedList000, TestSize.Level1)
{
    uint32_t uid = 150000;
    int32_t ret = g_BandwidthManager->AddDeniedList(uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = g_BandwidthManager->AddDeniedList(uid);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: RemoveDeniedList000
 * @tc.desc: Test BandwidthManager RemoveDeniedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveDeniedList000, TestSize.Level1)
{
    uint32_t uid = 150000;
    int32_t ret = g_BandwidthManager->RemoveDeniedList(uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveDeniedList001
 * @tc.desc: Test BandwidthManager RemoveDeniedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveDeniedList001, TestSize.Level1)
{
    uint32_t uid = 150000;
    int32_t ret = g_BandwidthManager->AddDeniedList(uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = g_BandwidthManager->RemoveDeniedList(uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: AddAllowedList000
 * @tc.desc: Test BandwidthManager AddAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, AddAllowedList000, TestSize.Level1)
{
    uint32_t uid = 150000;
    int32_t ret = g_BandwidthManager->AddAllowedList(uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = g_BandwidthManager->AddAllowedList(uid);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: RemoveAllowedList000
 * @tc.desc: Test BandwidthManager RemoveAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveAllowedList000, TestSize.Level1)
{
    uint32_t uid = 150000;
    int32_t ret = g_BandwidthManager->RemoveAllowedList(uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveAllowedList001
 * @tc.desc: Test BandwidthManager RemoveAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveAllowedList001, TestSize.Level1)
{
    uint32_t uid = 150000;
    int32_t ret = g_BandwidthManager->AddDeniedList(uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = g_BandwidthManager->RemoveAllowedList(uid);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: BandwidthManagerBranchTest001
 * @tc.desc: Test BandwidthManager Branch.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthManagerBranchTest001, TestSize.Level1)
{
    int32_t ret = g_BandwidthManager->InitDefaultBwChainRules();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = g_BandwidthManager->InitDefaultListBoxChainRules();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = g_BandwidthManager->InitDefaultAlertChainRules();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = g_BandwidthManager->InitDefaultRules();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = g_BandwidthManager->IptablesNewChain(ChainType::CHAIN_FORWARD);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = g_BandwidthManager->IptablesDeleteChain(ChainType::CHAIN_FORWARD);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string ifName = "wlan0";
    int64_t bytes = 0;
    ret = g_BandwidthManager->SetIfaceQuotaDetail(ifName, bytes);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveIfaceQuota003
 * @tc.desc: Test BandwidthManager RemoveIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveIfaceQuota003, TestSize.Level1)
{
    std::string iface = "abc";
    int32_t ret = g_BandwidthManager->RemoveIfaceQuota(iface);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: RemoveDeniedList002
 * @tc.desc: Test BandwidthManager RemoveDeniedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, RemoveDeniedList002, TestSize.Level1)
{
    uint32_t uid = -1;
    int32_t ret = g_BandwidthManager->RemoveDeniedList(uid);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}
} // namespace NetsysNative
} // namespace OHOS
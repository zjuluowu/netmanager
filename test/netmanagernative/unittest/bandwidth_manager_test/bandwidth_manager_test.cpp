/*
 * Copyright (c) 2022 - 2024 Huawei Device Co., Ltd.
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
#include <chrono>
#include <thread>

#include "netsys_controller.h"

#include "system_ability_definition.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace NetManagerStandard;
using namespace nmd;

constexpr uint32_t TEST_UID = 11256;
class BandwidthManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BandwidthManagerTest::SetUpTestCase()
{
    NetsysController::GetInstance().BandwidthEnableDataSaver(false);
}

void BandwidthManagerTest::TearDownTestCase()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void BandwidthManagerTest::SetUp() {}

void BandwidthManagerTest::TearDown() {}

/**
 * @tc.name: BandwidthEnableDataSaverTest001
 * @tc.desc: Test BandwidthManager BandwidthEnableDataSaver.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthEnableDataSaverTest001, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthEnableDataSaver(true);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthEnableDataSaverTest002
 * @tc.desc: Test BandwidthManager BandwidthEnableDataSaver.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthEnableDataSaverTest002, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthEnableDataSaver(false);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthEnableDataSaverTest003
 * @tc.desc: Test BandwidthManager BandwidthEnableDataSaver.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthEnableDataSaverTest003, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthEnableDataSaver(true);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthEnableDataSaver(true);
    EXPECT_TRUE(ret == -1 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthEnableDataSaverTest004
 * @tc.desc: Test BandwidthManager BandwidthEnableDataSaver.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthEnableDataSaverTest004, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthEnableDataSaver(false);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthEnableDataSaver(false);
    EXPECT_TRUE(ret == -1 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthSetIfaceQuotaTest001
 * @tc.desc: Test BandwidthManager BandwidthSetIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthSetIfaceQuotaTest000, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthSetIfaceQuota("_0iface", 2097152);
    EXPECT_TRUE(ret == -1 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthSetIfaceQuotaTest001
 * @tc.desc: Test BandwidthManager BandwidthSetIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthSetIfaceQuotaTest001, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthSetIfaceQuota("iface0", 2097152);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthSetIfaceQuotaTest002
 * @tc.desc: Test BandwidthManager BandwidthSetIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthSetIfaceQuotaTest002, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthSetIfaceQuota("wlan0", 2097152);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthSetIfaceQuotaTest003
 * @tc.desc: Test BandwidthManager BandwidthSetIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthSetIfaceQuotaTest003, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthSetIfaceQuota("wlan0", 5000000000000000);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthSetIfaceQuotaTest004
 * @tc.desc: Test BandwidthManager BandwidthSetIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthSetIfaceQuotaTest004, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthSetIfaceQuota("wlan0", 5000000000000);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthSetIfaceQuota("wlan0", 2097152);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthRemoveIfaceQuotaTest000
 * @tc.desc: Test BandwidthManager BandwidthRemoveIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthRemoveIfaceQuotaTest000, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthRemoveIfaceQuota("_iface0");
    EXPECT_TRUE(ret == -1 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthRemoveIfaceQuotaTest001
 * @tc.desc: Test BandwidthManager BandwidthRemoveIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthRemoveIfaceQuotaTest001, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthRemoveIfaceQuota("iface0");
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthRemoveIfaceQuotaTest002
 * @tc.desc: Test BandwidthManager BandwidthRemoveIfaceQuota.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthRemoveIfaceQuotaTest002, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthRemoveIfaceQuota("wlan0");
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthAddDeniedListTest001
 * @tc.desc: Test BandwidthManager BandwidthAddDeniedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthAddDeniedListTest001, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthAddDeniedList(TEST_UID);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthRemoveDeniedList(TEST_UID);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthAddDeniedListTest002
 * @tc.desc: Test BandwidthManager BandwidthAddDeniedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthAddDeniedListTest002, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthAddDeniedList(TEST_UID);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthAddDeniedList(TEST_UID);
    EXPECT_TRUE(ret == -1 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthRemoveDeniedListTest001
 * @tc.desc: Test BandwidthManager BandwidthRemoveDeniedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthRemoveDeniedListTest001, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthRemoveDeniedList(TEST_UID);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthRemoveDeniedListTest002
 * @tc.desc: Test BandwidthManager BandwidthRemoveDeniedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthRemoveDeniedListTest002, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthAddDeniedList(TEST_UID);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthRemoveDeniedList(TEST_UID);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthRemoveDeniedList(TEST_UID);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthAddAllowedListTest001
 * @tc.desc: Test BandwidthManager BandwidthAddAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthAddAllowedListTest001, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthAddAllowedList(TEST_UID);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthAddAllowedListTest002
 * @tc.desc: Test BandwidthManager BandwidthAddAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthAddAllowedListTest002, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthAddAllowedList(TEST_UID);
    EXPECT_TRUE(ret == -1 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthRemoveAllowedListTest001
 * @tc.desc: Test BandwidthManager BandwidthRemoveAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthRemoveAllowedListTest001, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthRemoveAllowedList(TEST_UID);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
}

/**
 * @tc.name: BandwidthRemoveAllowedListTest002
 * @tc.desc: Test BandwidthManager BandwidthRemoveAllowedList.
 * @tc.type: FUNC
 */
HWTEST_F(BandwidthManagerTest, BandwidthRemoveAllowedListTest002, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().BandwidthAddAllowedList(TEST_UID);
    EXPECT_TRUE(ret == 0 || ret == NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthRemoveAllowedList(TEST_UID);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().BandwidthRemoveAllowedList(TEST_UID);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
}
} // namespace NetsysNative
} // namespace OHOS

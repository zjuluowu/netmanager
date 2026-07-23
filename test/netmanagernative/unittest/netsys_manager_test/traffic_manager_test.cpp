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

#include "traffic_manager.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
} // namespace

class TrafficManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TrafficManagerTest::SetUpTestCase() {}

void TrafficManagerTest::TearDownTestCase() {}

void TrafficManagerTest::SetUp() {}

void TrafficManagerTest::TearDown() {}

HWTEST_F(TrafficManagerTest, GetInterfaceTraffic001, TestSize.Level1)
{
    std::string ifName = "test0";
    TrafficStatsParcel tmpData = TrafficManager::GetInterfaceTraffic(ifName);
    EXPECT_EQ(tmpData.rxBytes, 0);
}

HWTEST_F(TrafficManagerTest, GetInterfaceTraffic002, TestSize.Level1)
{
    std::string ifName = "eth0";
    TrafficStatsParcel tmpData = TrafficManager::GetInterfaceTraffic(ifName);
    EXPECT_GE(tmpData.rxBytes, 0);
}

HWTEST_F(TrafficManagerTest, GetAllTxTraffic001, TestSize.Level1)
{
    long allTxBytes = TrafficManager::GetAllTxTraffic();
    EXPECT_GE(allTxBytes, -1);
}

HWTEST_F(TrafficManagerTest, GetAllTxTraffic002, TestSize.Level1)
{
    long allTxBytes = TrafficManager::GetAllTxTraffic();
    EXPECT_GE(allTxBytes, 0);
}

HWTEST_F(TrafficManagerTest, GetAllRxTraffic001, TestSize.Level1)
{
    long allRxBytes = TrafficManager::GetAllRxTraffic();
    EXPECT_GE(allRxBytes, -1);
}

HWTEST_F(TrafficManagerTest, GetAllRxTraffic002, TestSize.Level1)
{
    long allRxBytes = TrafficManager::GetAllRxTraffic();
    EXPECT_GE(allRxBytes, 0);
}
} // namespace nmd
} // namespace OHOS
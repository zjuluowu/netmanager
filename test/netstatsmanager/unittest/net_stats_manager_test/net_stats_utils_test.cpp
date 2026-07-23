/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <ctime>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_stats_utils.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
class NetStatsUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetStatsUtilsTest::SetUpTestCase() {}

void NetStatsUtilsTest::TearDownTestCase() {}

void NetStatsUtilsTest::SetUp() {}

void NetStatsUtilsTest::TearDown() {}

HWTEST_F(NetStatsUtilsTest, IsDualCardEnabledTest001, TestSize.Level1)
{
    int32_t ret = NetStatsUtils::IsDualCardEnabled();
    EXPECT_TRUE(ret >= 0);
}

HWTEST_F(NetStatsUtilsTest, GetPrimarySlotIdTest001, TestSize.Level1)
{
    int32_t ret = NetStatsUtils::GetPrimarySlotId();
    EXPECT_TRUE(ret == -1 || ret == 0 || ret == 1);
}

HWTEST_F(NetStatsUtilsTest, IsLessThanOneMonthAgoPreciseTest001, TestSize.Level1)
{
    time_t timastamp = 123;
    int32_t ret = NetStatsUtils::IsLessThanOneMonthAgoPrecise(timastamp);
    EXPECT_EQ(ret, true);
}
}
}
}
/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "net_stats_calibrate.h"

namespace OHOS {
namespace NetManagerStandard {

using namespace testing::ext;
class NetStatsCalibrateTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    uint32_t GetTestTime();
};

void NetStatsCalibrateTest::SetUpTestCase() {}

void NetStatsCalibrateTest::TearDownTestCase() {}

void NetStatsCalibrateTest::SetUp() {}

void NetStatsCalibrateTest::TearDown() {}


HWTEST_F(NetStatsCalibrateTest, InitChangeToIfaceTimeTest001, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    
    netStatsCalibrate.InitChangeToIfaceTime();
    EXPECT_NE(netStatsCalibrate.changeToIfaceTime_, UINT32_MAX);

    netStatsCalibrate.InitChangeToIfaceTime();
    EXPECT_NE(netStatsCalibrate.changeToIfaceTime_, UINT32_MAX);
}

HWTEST_F(NetStatsCalibrateTest, GetChangeToIfaceTimeTest001, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    netStatsCalibrate.InitChangeToIfaceTime();
    uint32_t time = netStatsCalibrate.GetChangeToIfaceTime();
    EXPECT_NE(time, UINT32_MAX);
}

HWTEST_F(NetStatsCalibrateTest, UpdateChangeToIfaceTimeTest001, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    uint32_t startTime = UINT32_MAX;
    netStatsCalibrate.UpdateChangeToIfaceTime(startTime);
    EXPECT_EQ(netStatsCalibrate.changeToIfaceTime_, UINT32_MAX);
}

HWTEST_F(NetStatsCalibrateTest, GetCalicrationInfoTest001, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    uint32_t simId = 100;
    CalibrateInfo info;
    bool ret = netStatsCalibrate.GetCalibrationInfo(simId, info);
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsCalibrateTest, IsExistCalibrationInfoTest001, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    uint32_t simId = 100;
    bool ret = netStatsCalibrate.IsExistCalibrationInfo(simId);
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsCalibrateTest, InitCalibrationInfoTest001, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    uint32_t simId = 100;
    bool ret = netStatsCalibrate.InitCalibrationInfo(simId);
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsCalibrateTest, UpdateCalibrationInfoTest001, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    uint32_t simId = 1;
    uint64_t usedTraffic = 100*1024*1024;
    netStatsCalibrate.UpdateCalibrationInfo(simId, usedTraffic);
    auto iter = netStatsCalibrate.calibrateInfo_.find(simId);
    EXPECT_NE(iter, netStatsCalibrate.calibrateInfo_.end());
}

HWTEST_F(NetStatsCalibrateTest, UpdateCalibrationInfoTest002, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    uint32_t simId = 1;
    uint64_t usedTraffic = 100*1024*2048;
    netStatsCalibrate.UpdateCalibrationInfo(simId, usedTraffic);
    netStatsCalibrate.UpdateCalibrationInfo(simId, usedTraffic);
    auto iter = netStatsCalibrate.calibrateInfo_.find(simId);
    EXPECT_NE(iter, netStatsCalibrate.calibrateInfo_.end());

    bool ret = netStatsCalibrate.InitCalibrationInfo(simId);
    EXPECT_EQ(ret, true);
    ret = netStatsCalibrate.InitCalibrationInfo(simId);
    ret = netStatsCalibrate.InitCalibrationInfo(simId + 1);

    CalibrateInfo info;
    ret = netStatsCalibrate.GetCalibrationInfo(simId, info);
    EXPECT_EQ(ret, true);
}

HWTEST_F(NetStatsCalibrateTest, IsExistCalibrationInfoTest002, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    uint32_t simId = 1;
    bool ret = netStatsCalibrate.IsExistCalibrationInfo(simId);
    EXPECT_EQ(ret, true);
}

HWTEST_F(NetStatsCalibrateTest, DeleteCalibrationInfoTest001, TestSize.Level1)
{
    NetStatsCalibrate netStatsCalibrate;
    bool ret = netStatsCalibrate.DeleteCalibrationInfo(100);
    uint32_t simId = 1;
    ret = netStatsCalibrate.DeleteCalibrationInfo(simId);
    EXPECT_EQ(ret, true);
}
} // namespace NetManagerStandard
} // namespace OHOS

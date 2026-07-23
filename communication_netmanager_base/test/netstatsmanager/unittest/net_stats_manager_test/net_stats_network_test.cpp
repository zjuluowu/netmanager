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

#include "net_stats_network.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int32_t TEST_TYPE = 0;
constexpr int32_t TEST_SIM_ID = 1;
constexpr int64_t TEST_START_TIME = 100;
constexpr int64_t TEST_END_TIME = 200;
NetStatsNetwork GetNetworkData()
{
    NetStatsNetwork network;
    network.type_ = TEST_TYPE;
    network.startTime_ = TEST_START_TIME;
    network.endTime_ = TEST_END_TIME;
    network.simId_ = TEST_SIM_ID;
    return network;
}
sptr<NetStatsNetwork> GetSptrNetworkData()
{
    sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork();
    network->type_ = TEST_TYPE;
    network->startTime_ = TEST_START_TIME;
    network->endTime_ = TEST_END_TIME;
    network->simId_ = TEST_SIM_ID;
    return network;
}
} // namespace

using namespace testing::ext;
class NetStatsNetworkTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    uint32_t GetTestTime();
};

void NetStatsNetworkTest::SetUpTestCase() {}

void NetStatsNetworkTest::TearDownTestCase() {}

void NetStatsNetworkTest::SetUp() {}

void NetStatsNetworkTest::TearDown() {}

HWTEST_F(NetStatsNetworkTest, MarshallingAndUnmarshallingTest001, TestSize.Level1)
{
    Parcel parcel;
    NetStatsNetwork info = GetNetworkData();
    EXPECT_TRUE(info.Marshalling(parcel));
    sptr<NetStatsNetwork> result = NetStatsNetwork::Unmarshalling(parcel);
    EXPECT_EQ(result->type_, info.type_);
    EXPECT_EQ(result->startTime_, info.startTime_);
    EXPECT_EQ(result->endTime_, info.endTime_);
    EXPECT_EQ(result->simId_, info.simId_);
}

HWTEST_F(NetStatsNetworkTest, MarshallingAndUnmarshallingTest002, TestSize.Level1)
{
    Parcel parcel;
    sptr<NetStatsNetwork> info = GetSptrNetworkData();
    EXPECT_TRUE(NetStatsNetwork::Marshalling(parcel, info));
    sptr<NetStatsNetwork> result = NetStatsNetwork::Unmarshalling(parcel);
    EXPECT_EQ(result->type_, info->type_);
    EXPECT_EQ(result->startTime_, info->startTime_);
    EXPECT_EQ(result->endTime_, info->endTime_);
    EXPECT_EQ(result->simId_, info->simId_);
}

} // namespace NetManagerStandard
} // namespace OHOS
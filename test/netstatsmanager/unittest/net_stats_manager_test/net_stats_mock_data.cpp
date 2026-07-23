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

#include <memory>
#include <random>

#include <gtest/gtest.h>

#include "net_manager_constants.h"
#include "net_stats_database_helper.h"
#include "net_stats_data_handler.h"
#include "net_stats_database_defines.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;

using namespace testing::ext;
namespace {
const std::string CCH = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr uint32_t UID_MAX_TEST = 200;
constexpr uint32_t MOCK_DATA_SIZE = 1000;
const std::vector<std::string> MOCK_IFACE = {"wlan0", "eth0", "eth1", "usb0", "wlan1", "usb1"};
std::random_device g_rd;
std::mt19937 g_regn(g_rd());
uint32_t GetUint32()
{
    return static_cast<uint32_t>(g_regn()) % UID_MAX_TEST;
}

uint64_t GetUint64()
{
    return static_cast<uint64_t>(g_regn());
}

uint64_t GetMockDate()
{
    return static_cast<uint64_t>(g_regn() % LONG_MAX);
}

std::string GetMockIface()
{
    return MOCK_IFACE.at(g_regn() % MOCK_IFACE.size());
}
} // namespace
class NetStatsMockData : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetStatsMockData::SetUpTestCase() {}

void NetStatsMockData::TearDownTestCase() {}

void NetStatsMockData::SetUp() {}

void NetStatsMockData::TearDown() {}

HWTEST_F(NetStatsMockData, CreateTableTest001, TestSize.Level1)
{
    auto handler = std::make_unique<NetStatsDataHandler>();
    std::vector<NetStatsInfo> statsData;
    for (uint32_t i = 0; i < MOCK_DATA_SIZE; i++) {
        NetStatsInfo info;
        info.uid_ = GetUint32();
        info.date_ = GetMockDate();
        info.iface_ = GetMockIface();
        info.rxBytes_ = GetUint64();
        info.rxPackets_ = GetUint64();
        info.txBytes_ = GetUint64();
        info.txPackets_ = GetUint64();
        statsData.push_back(info);
    }
    int32_t ret = handler->WriteStatsData(statsData, UID_TABLE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = handler->WriteStatsData(statsData, IFACE_TABLE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS
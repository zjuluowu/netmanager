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

#include <ctime>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_manager_center.h"
#include "net_stats_callback_test.h"
#include "net_stats_constants.h"
#include "net_stats_service.h"
#include "common/net_stats_service_common.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
#define DTEST_LOG std::cout << __func__ << ":" << __LINE__ << ":"
} // namespace

using namespace testing::ext;
class NetStatsServiceCommonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetStatsServiceCommon> instance_ = nullptr;
};

void NetStatsServiceCommonTest::SetUpTestCase()
{
    instance_ = std::make_shared<NetStatsServiceCommon>();
}

void NetStatsServiceCommonTest::TearDownTestCase()
{
    instance_ = nullptr;
}

void NetStatsServiceCommonTest::SetUp() {}

void NetStatsServiceCommonTest::TearDown() {}

HWTEST_F(NetStatsServiceCommonTest, GetDumpMessageTest001, TestSize.Level1)
{
    std::string iface = "wlan0";
    uint64_t start = 1000;
    uint64_t end = 2000;
    NetStatsInfo info;
    auto res = instance_->GetIfaceStatsDetail(iface, start, end, info);
    EXPECT_NE(res, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsServiceCommonTest, GetIfaceStatsDetailTest002, TestSize.Level1)
{
    std::string iface = "eth0";
    uint64_t start = 1000;
    uint64_t end = 2000;
    NetStatsInfo info;
    auto res = instance_->GetIfaceStatsDetail(iface, start, end, info);
    EXPECT_NE(res, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsServiceCommonTest, GetIfaceStatsDetailTest003, TestSize.Level1)
{
    std::string iface = "usb0";
    uint64_t start = 1000;
    uint64_t end = 2000;
    NetStatsInfo info;
    auto res = instance_->GetIfaceStatsDetail(iface, start, end, info);
    EXPECT_NE(res, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsServiceCommonTest, ResetStatsFactoryTest001, TestSize.Level1)
{
    auto result = instance_->ResetStatsFactory();
    EXPECT_EQ(result, 0);
}
} // namespace NetManagerStandard
} // namespace OHOS

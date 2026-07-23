/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "gmock/gmock.h"
#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "mock_core_service_manager.h"
#include "net_manager_center.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_cached.h"
#include "net_stats_callback_test.h"
#include "net_stats_constants.h"
#include "net_stats_database_defines.h"
#include "net_stats_service.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;
constexpr uint32_t DAY_SECONDS = 2 * 24 * 60 * 60;
using namespace testing;
using namespace testing::ext;
class NetStatsServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetStatsServiceTest::SetUpTestCase() {}

void NetStatsServiceTest::TearDownTestCase() {}

void NetStatsServiceTest::SetUp() {}

void NetStatsServiceTest::TearDown() {}

HWTEST_F(NetStatsServiceTest, GetTrafficStatsByUidNetworkTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::vector<NetStatsInfoSequence> infos = {};
    uint32_t uid = 1;
    const sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork();
    network->type_ = 1;
    network->startTime_ = 1;
    network->endTime_ = 2;
    int32_t ret = netStatsService.GetTrafficStatsByUidNetwork(infos, uid, *network);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsServiceTest, GetTrafficStatsByUidNetworkTest002, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::vector<NetStatsInfoSequence> infos = {};
    uint32_t uid = 1;
    const sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork();
    network->type_ = 1;
    network->startTime_ = 1857600534;
    network->endTime_ = 1867600534;
    int32_t ret = netStatsService.GetTrafficStatsByUidNetwork(infos, uid, *network);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsServiceTest, GetTrafficStatsByUidNetworkTest003, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::vector<NetStatsInfoSequence> infos = {};
    uint32_t uid = 1;
    const sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork();
    network->type_ = 1;
    network->startTime_ = 1757600034;
    network->endTime_ = 1867600534;
    int32_t ret = netStatsService.GetTrafficStatsByUidNetwork(infos, uid, *network);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsServiceTest, ProcessOsAccountChangedTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    AccountSA::OsAccountState state = AccountSA::OsAccountState::CREATED;
    int32_t userId = 101;
    int32_t ret = netStatsService.ProcessOsAccountChanged(userId, state);
    EXPECT_EQ(ret, 0);
    state = AccountSA::OsAccountState::STOPPING;
    ret = netStatsService.ProcessOsAccountChanged(888, state);
    EXPECT_EQ(ret, 0);
    netStatsService.netStatsCached_->SetCurPrivateUserId(101);
    EXPECT_EQ(netStatsService.netStatsCached_->GetCurPrivateUserId(), 101);
    state = AccountSA::OsAccountState::INVALID_TYPE;
    ret = netStatsService.ProcessOsAccountChanged(888, state);
}

HWTEST_F(NetStatsServiceTest, ProcessOsAccountChangedTest002, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t userId = 101;
    netStatsService.netStatsCached_->SetCurPrivateUserId(userId);
    AccountSA::OsAccountState state = AccountSA::OsAccountState::STOPPED;
    int32_t ret = netStatsService.ProcessOsAccountChanged(userId, state);

    int32_t curPrivateUserId = netStatsService.netStatsCached_->GetCurPrivateUserId();
    EXPECT_EQ(curPrivateUserId, -1);
}

HWTEST_F(NetStatsServiceTest, ProcessOsAccountChangedTest003, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t userId = 101;
    netStatsService.netStatsCached_->SetCurPrivateUserId(userId);
    AccountSA::OsAccountState state = AccountSA::OsAccountState::SWITCHED;
    int32_t ret = netStatsService.ProcessOsAccountChanged(userId, state);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetStatsServiceTest, ProcessOsAccountChangedTest004, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t userId = 101;
    netStatsService.netStatsCached_->SetCurPrivateUserId(userId);
    AccountSA::OsAccountState state = AccountSA::OsAccountState::LOCKED;
    int32_t ret = netStatsService.ProcessOsAccountChanged(userId, state);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetStatsServiceTest, ProcessOsAccountChangedTest005, TestSize.Level1)
{
    auto netStatsServicePtr = std::make_shared<NetStatsService>();
    int32_t userId = 101;
    netStatsServicePtr->netStatsCached_->SetCurPrivateUserId(userId);
    AccountSA::OsAccountState state = AccountSA::OsAccountState::SWITCHED;
    int32_t ret = netStatsServicePtr->ProcessOsAccountChanged(userId + 1, state);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetStatsServiceTest, MergeTrafficStatsByAccountTest001, TestSize.Level1)
{
    NetStatsService netStatsService;

    int32_t curUserId = -1;
    int32_t ret1 = AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(curUserId);
    int32_t defaultUserId = -1;
    int32_t ret2 = AccountSA::OsAccountManager::GetDefaultActivatedOsAccount(defaultUserId);
    ASSERT_EQ(ret1, 0);
    ASSERT_EQ(ret2, 0);

    std::vector<NetStatsInfo> infos;
    NetStatsInfo info1;
    info1.userId_ = curUserId;
    NetStatsInfo info2;
    info2.userId_ = 101;
    infos.push_back(info1);
    infos.push_back(info2);
    netStatsService.netStatsCached_->SetCurPrivateUserId(101);

    if (curUserId == defaultUserId) {
        netStatsService.MergeTrafficStatsByAccount(infos);
        EXPECT_EQ(infos[1].userId_, 101);
    }
}

HWTEST_F(NetStatsServiceTest, MergeTrafficStatsByAccountTest002, TestSize.Level1)
{
    NetStatsService netStatsService;

    int32_t curUserId = -1;
    int32_t ret1 = AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(curUserId);
    int32_t defaultUserId = -1;
    int32_t ret2 = AccountSA::OsAccountManager::GetDefaultActivatedOsAccount(defaultUserId);
    ASSERT_EQ(ret1, 0);
    ASSERT_EQ(ret2, 0);
    NETMGR_LOG_E("curUserId:%{public}d, defaultUserId:%{public}d", curUserId, defaultUserId);
    std::vector<NetStatsInfo> infos;
    NetStatsInfo info1;
    info1.userId_ = curUserId;
    NetStatsInfo info2;
    info2.userId_ = 101;
    NetStatsInfo info3;
    info3.userId_ = SIM_PRIVATE_USERID;
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    netStatsService.netStatsCached_->SetCurDefaultUserId(109);
    netStatsService.netStatsCached_->SetCurPrivateUserId(curUserId);
    netStatsService.MergeTrafficStatsByAccount(infos);
    if (curUserId == defaultUserId) {
        EXPECT_EQ(infos[1].uid_, DEFAULT_ACCOUNT_UID);
    }
}

HWTEST_F(NetStatsServiceTest, MergeTrafficStatsByAccountTest003, TestSize.Level1)
{
    NetStatsService netStatsService;

    int32_t curUserId = -1;
    int32_t ret1 = AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(curUserId);
    int32_t defaultUserId = -1;
    int32_t ret2 = AccountSA::OsAccountManager::GetDefaultActivatedOsAccount(defaultUserId);
    ASSERT_EQ(ret1, 0);
    ASSERT_EQ(ret2, 0);
    std::vector<NetStatsInfo> infos;
    NetStatsInfo info1;
    info1.userId_ = curUserId;
    NetStatsInfo info2;
    info2.userId_ = 101;
    NetStatsInfo info3;
    info3.userId_ = SIM_PRIVATE_USERID;
    NetStatsInfo info4;
    info4.userId_ = 108;
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.netStatsCached_->SetCurDefaultUserId(curUserId);
    netStatsService.netStatsCached_->SetCurPrivateUserId(108);
    netStatsService.MergeTrafficStatsByAccount(infos);

    if (curUserId == defaultUserId) {
        EXPECT_EQ(infos[3].uid_, OTHER_ACCOUNT_UID);
    }
}

HWTEST_F(NetStatsServiceTest, DeleteTrafficStatsByAccountTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t defaultUserId = -1;
    AccountSA::OsAccountManager::GetDefaultActivatedOsAccount(defaultUserId);
    std::vector<NetStatsInfoSequence> infos;
    NetStatsInfoSequence info1;
    info1.info_.userId_ = defaultUserId;
    NetStatsInfoSequence info2;
    info2.info_.userId_ = defaultUserId + 1;
    NetStatsInfoSequence info3;
    info3.info_.userId_ = 12345612;
    NetStatsInfoSequence info4;
    info3.info_.userId_ = 0;
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, DEFAULT_ACCOUNT_UID);
    EXPECT_EQ(infos.size(), 2);
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, OTHER_ACCOUNT_UID);
    EXPECT_EQ(infos.size(), 2);
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, 888);
    EXPECT_EQ(infos.size(), 6);
}

HWTEST_F(NetStatsServiceTest, DeleteTrafficStatsByAccountTest003, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t defaultUserId = -1;
    AccountSA::OsAccountManager::GetDefaultActivatedOsAccount(defaultUserId);
    netStatsService.netStatsCached_->SetCurDefaultUserId(defaultUserId);
    std::vector<NetStatsInfoSequence> infos;
    NetStatsInfoSequence info1;
    info1.info_.userId_ = defaultUserId;
    NetStatsInfoSequence info2;
    info2.info_.userId_ = defaultUserId + 1;
    NetStatsInfoSequence info3;
    info3.info_.userId_ = 12345612;
    NetStatsInfoSequence info4;
    info3.info_.userId_ = 0;
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, Sim_UID);
    EXPECT_EQ(infos.size(), 2);
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, SIM2_UID);
    EXPECT_EQ(infos.size(), 4);
}

HWTEST_F(NetStatsServiceTest, DeleteTrafficStatsByAccountTest004, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t defaultUserId = -1;
    AccountSA::OsAccountManager::GetDefaultActivatedOsAccount(defaultUserId);
    netStatsService.netStatsCached_->SetCurDefaultUserId(defaultUserId);
    std::vector<NetStatsInfoSequence> infos;
    NetStatsInfoSequence info1;
    info1.info_.userId_ = defaultUserId;
    NetStatsInfoSequence info2;
    info2.info_.userId_ = defaultUserId + 1;
    NetStatsInfoSequence info3;
    info3.info_.userId_ = 12345612;
    NetStatsInfoSequence info4;
    info3.info_.userId_ = 0;
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, DEFAULT_ACCOUNT_UID);
    EXPECT_EQ(infos.size(), 3);
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, OTHER_ACCOUNT_UID);
    EXPECT_EQ(infos.size(), 1);
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, 888);
    EXPECT_EQ(infos.size(), 5);
}

HWTEST_F(NetStatsServiceTest, DeleteTrafficStatsByAccountTest002, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t defaultUserId = -1;
    AccountSA::OsAccountManager::GetDefaultActivatedOsAccount(defaultUserId);
    std::vector<NetStatsInfoSequence> infos;
    NetStatsInfoSequence info1;
    info1.info_.userId_ = defaultUserId;
    NetStatsInfoSequence info2;
    info2.info_.userId_ = defaultUserId + 1;
    NetStatsInfoSequence info3;
    info3.info_.userId_ = 12345612;
    NetStatsInfoSequence info4;
    info3.info_.userId_ = 0;
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, Sim_UID);
    EXPECT_EQ(infos.size(), 0);
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.DeleteTrafficStatsByAccount(infos, SIM2_UID);
    EXPECT_EQ(infos.size(), 0);
}

HWTEST_F(NetStatsServiceTest, EraseNetStatsInfoByUserIdTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    int defaultUserId = 100;
    std::vector<NetStatsInfoSequence> infos;
    NetStatsInfoSequence info1;
    info1.info_.userId_ = defaultUserId;
    NetStatsInfoSequence info2;
    info2.info_.userId_ = defaultUserId + 1;
    NetStatsInfoSequence info3;
    info3.info_.userId_ = 12345612;
    NetStatsInfoSequence info4;
    info3.info_.userId_ = 0;
    infos.push_back(info1);
    infos.push_back(info2);
    infos.push_back(info3);
    infos.push_back(info4);
    netStatsService.EraseNetStatsInfoByUserId(infos, 0);
    EXPECT_EQ(infos.size(), 2);
}

HWTEST_F(NetStatsServiceTest, AddUidStatsFlagTest001, TestSize.Level1)
{
    auto netStatsServicePtr = std::make_shared<NetStatsService>();
    netStatsServicePtr->AddUidStatsFlag(0);
    sleep(1);
    EXPECT_EQ(netStatsServicePtr->isUpdate_, true);
}

#ifdef SUPPORT_TRAFFIC_STATISTIC
HWTEST_F(NetStatsServiceTest, MergeTrafficStatsTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::vector<NetStatsInfoSequence> statsInfoSequences;
    NetStatsInfoSequence infoSque;
    infoSque.startTime_ = CommonUtils::GetCurrentSecond() - 100;
    infoSque.endTime_ = CommonUtils::GetCurrentSecond() - 100;
    statsInfoSequences.push_back(infoSque);
    NetStatsInfo info;
    info.date_ = CommonUtils::GetCurrentSecond();
    uint32_t endTimestamp = CommonUtils::GetCurrentSecond();
    bool isNeedMerge = true;
    netStatsService.MergeTrafficStats(statsInfoSequences, info, endTimestamp);
    EXPECT_EQ(statsInfoSequences.size(), 2);

    std::vector<NetStatsInfoSequence> statsInfoSequences2;
    statsInfoSequences2.push_back(infoSque);
    netStatsService.MergeTrafficStats(statsInfoSequences2, info, endTimestamp, isNeedMerge);
    EXPECT_EQ(statsInfoSequences2.size(), 1);
}

HWTEST_F(NetStatsServiceTest, SetCalibrationTrafficTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    uint32_t simId = 20;
    int64_t remainingData = 100;
    uint64_t totalMonthlyData = 500;
    netStatsService.SetCalibrationTraffic(1, remainingData, totalMonthlyData);
    netStatsService.SetCalibrationTraffic(1, remainingData + 500, totalMonthlyData);
    netStatsService.SetCalibrationTraffic(1, remainingData + 500, UINT64_MAX);
    int32_t ret = netStatsService.SetCalibrationTraffic(simId, remainingData, totalMonthlyData);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetStatsServiceTest, SetCalibrationTrafficTest002, TestSize.Level1)
{
    NetStatsService netStatsService;
    uint32_t simId = 20;
    int64_t remainingData = -100;
    uint64_t totalMonthlyData = 500;
    netStatsService.SetCalibrationTraffic(1, remainingData, totalMonthlyData);
    netStatsService.SetCalibrationTraffic(1, remainingData, totalMonthlyData);
    int32_t ret = netStatsService.SetCalibrationTraffic(simId, remainingData, totalMonthlyData);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetStatsServiceTest, ResetNotifyStateTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t simId = 0;
    auto info = std::make_shared<TrafficPlanInfo>();
    netStatsService.trafficPlanService_->trafficPlanInfoMap_.insert(std::make_pair(simId, info));
    netStatsService.trafficPlanService_->ResetNotifyState(0);
    netStatsService.trafficPlanService_->ResetNotifyState(1);
    EXPECT_NE(netStatsService.netStatsCached_, nullptr);
}
#endif

HWTEST_F(NetStatsServiceTest, GetHistoryDataTest001, TestSize.Level1)
{
    auto netStatsService = NetStatsService::GetInstance();
    std::vector<NetStatsInfo> infos;
    uint32_t start = 1745847234;
    uint32_t end = 1745847823;
    netStatsService->GetHistoryData(infos, "111", 12345612, 0, UINT32_MAX);
    EXPECT_EQ(infos.size(), 0);
    netStatsService->GetHistoryData(infos, "111", DEFAULT_ACCOUNT_UID, 0, UINT32_MAX);
    netStatsService->netStatsCached_->SetCurPrivateUserId(101);

    int32_t ret = netStatsService->GetHistoryData(infos, "111", OTHER_ACCOUNT_UID, 0, UINT32_MAX);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    netStatsService->netStatsCached_->SetCurPrivateUserId(-1);
    ret = netStatsService->GetHistoryData(infos, "111", OTHER_ACCOUNT_UID, 0, UINT32_MAX);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

#ifdef SUPPORT_TRAFFIC_STATISTIC
HWTEST_F(NetStatsServiceTest, UpdateBpfMapTimerTaskTest001, TestSize.Level1)
{
    auto netStatsService = NetStatsService::GetInstance();
    netStatsService->trafficPlanFfrtQueue_ = nullptr;
    netStatsService->UpdateBpfMapTimerTask();
    EXPECT_EQ(netStatsService->trafficPlanService_->trafficPlanInfoMap_.size(), 0);
}

HWTEST_F(NetStatsServiceTest, NotifyTrafficAlertFfrtTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t simId = 1;
    uint8_t flag = 2;
    netStatsService.trafficPlanFfrtQueue_ = std::make_shared<ffrt::queue>("TrafficPlanStatistic");
    bool ret = netStatsService.NotifyTrafficAlertFfrt(simId, flag);
    EXPECT_EQ(ret, 0);

    NetStatsService netStatsService2;
    netStatsService2.trafficPlanFfrtQueue_ = nullptr;
    ret = netStatsService2.NotifyTrafficAlertFfrt(simId, flag);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetStatsServiceTest, UpdateCurActiviteSimChangedTest001, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t simId = 10;
    uint64_t index = 12;
    netStatsService.trafficPlanFfrtQueue_ = std::make_shared<ffrt::queue>("TrafficPlanStatistic");
    netStatsService.UpdateCurActiviteSimChanged(simId, index);
    bool ret = netStatsService.trafficPlanService_->trafficPlanInfoMap_.find(simId) ==
                       netStatsService.trafficPlanService_->trafficPlanInfoMap_.end()
                   ? true : false;
    EXPECT_EQ(ret, false);

    int32_t simId2 = 11;
    uint64_t index2 = 13;
    netStatsService.simIdToIfIndexMap_[simId2] = index2;
    netStatsService.UpdateCurActiviteSimChanged(simId2, index2);
    ret = netStatsService.trafficPlanService_->trafficPlanInfoMap_.find(simId) ==
                  netStatsService.trafficPlanService_->trafficPlanInfoMap_.end()
              ? true : false;
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsServiceTest, UpdateCurActiviteSimChangedTest002, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t simId = 0;
    auto info = std::make_shared<TrafficPlanInfo>();
    netStatsService.trafficPlanService_->trafficPlanInfoMap_.insert(std::make_pair(simId, info));

    netStatsService.trafficPlanFfrtQueue_ = std::make_shared<ffrt::queue>("TrafficPlanStatistic");
    netStatsService.UpdateCurActiviteSimChanged(simId, 10);
    bool ret = netStatsService.trafficPlanService_->trafficPlanInfoMap_.find(simId) ==
                       netStatsService.trafficPlanService_->trafficPlanInfoMap_.end()
                   ? true : false;
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsServiceTest, UpdateCurActiviteSimChangedTest003, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t simId = 0;
    auto info = std::make_shared<TrafficPlanInfo>();
    info->trafficLimit = 132465789;
    info->unlimitTrafficSwitch = 1;
    netStatsService.trafficPlanService_->trafficPlanInfoMap_.insert(std::make_pair(simId, info));

    netStatsService.trafficPlanFfrtQueue_ = std::make_shared<ffrt::queue>("TrafficPlanStatistic");
    netStatsService.UpdateCurActiviteSimChanged(simId, 10);
    bool ret = netStatsService.trafficPlanService_->trafficPlanInfoMap_.find(simId) ==
                       netStatsService.trafficPlanService_->trafficPlanInfoMap_.end()
                   ? true : false;
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsServiceTest, UpdateCurActiviteSimChangedTest004, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t simId = 0;
    auto info = std::make_shared<TrafficPlanInfo>();
    info->trafficLimit = UINT64_MAX;
    info->unlimitTrafficSwitch = 1;
    netStatsService.trafficPlanService_->trafficPlanInfoMap_.insert(std::make_pair(simId, info));

    netStatsService.trafficPlanFfrtQueue_ = std::make_shared<ffrt::queue>("TrafficPlanStatistic");
    netStatsService.UpdateCurActiviteSimChanged(simId, 10);
    bool ret = netStatsService.trafficPlanService_->trafficPlanInfoMap_.find(simId) ==
                       netStatsService.trafficPlanService_->trafficPlanInfoMap_.end()
                   ? true : false;
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsServiceTest, UpdateCurActiviteSimChangedTest005, TestSize.Level1)
{
    NetStatsService netStatsService;
    int32_t simId = 0;
    auto info = std::make_shared<TrafficPlanInfo>();
    info->trafficLimit = 132465789;
    info->unlimitTrafficSwitch = 0;
    netStatsService.trafficPlanService_->trafficPlanInfoMap_.insert(std::make_pair(simId, info));

    netStatsService.trafficPlanFfrtQueue_ = std::make_shared<ffrt::queue>("TrafficPlanStatistic");
    netStatsService.UpdateCurActiviteSimChanged(simId, 10);
    bool ret = netStatsService.trafficPlanService_->trafficPlanInfoMap_.find(simId) ==
                       netStatsService.trafficPlanService_->trafficPlanInfoMap_.end()
                   ? true : false;
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsServiceTest, OnExtensionTest, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    NetStatsService netStatsService;
    int32_t ret = netStatsService.OnExtension("backup", data, reply);
    EXPECT_NE(ret, -1);
}

HWTEST_F(NetStatsServiceTest, OnExtensionTest2, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    NetStatsService netStatsService;
    int32_t ret = netStatsService.OnExtension("restore", data, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetStatsServiceTest, SetTrafficPlanInfoTest01, TestSize.Level1)
{
    NetStatsService netStatsService;
    netStatsService.trafficPlanService_ = nullptr;
    int32_t ret = netStatsService.SetTrafficPlanInfo(1, 1, 1);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetStatsServiceTest, SetTrafficPlanInfoTest02, TestSize.Level1)
{
    NetStatsService netStatsService;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(_)).WillRepeatedly(Return(-1));
    int32_t ret = netStatsService.SetTrafficPlanInfo(1, 1, 1);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetStatsServiceTest, SetTrafficPlanInfoTest03, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::u16string iccid = {u"1234564654651"};
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimIccId(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(iccid), Return(0)));
    int32_t ret = netStatsService.SetTrafficPlanInfo(1, 1, 1);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsServiceTest, SetTrafficPlanInfoTest04, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::u16string iccid = {u"1234564654651"};
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimIccId(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(iccid), Return(0)));
    int32_t ret = netStatsService.SetTrafficPlanInfo(1, 99, 1);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetStatsServiceTest, SetTrafficPlanInfoTest05, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::u16string iccid = {u"1234564654651"};
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimIccId(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(iccid), Return(0)));
    int32_t ret = netStatsService.SetTrafficPlanInfo(1, -19, 1);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetStatsServiceTest, GetTrafficPlanInfoTest01, TestSize.Level1)
{
    NetStatsService netStatsService;
    netStatsService.trafficPlanService_ = nullptr;
    int64_t value = 0;
    int32_t ret = netStatsService.GetTrafficPlanInfo(1, 1, value);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetStatsServiceTest, GetTrafficPlanInfoTest02, TestSize.Level1)
{
    NetStatsService netStatsService;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(_)).WillRepeatedly(Return(-1));
    int64_t value = 0;
    int32_t ret = netStatsService.GetTrafficPlanInfo(1, 1, value);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetStatsServiceTest, GetTrafficPlanInfoTest03, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::u16string iccid = {u"1234564654651123"};
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimIccId(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(iccid), Return(0)));
    int64_t value = 0;
    int32_t ret = netStatsService.GetTrafficPlanInfo(1, 1, value);
    EXPECT_EQ(ret, TRAFFIC_PLAN_ERR_DATABASE_FAILED);
}

HWTEST_F(NetStatsServiceTest, GetTrafficPlanInfoTest04, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::u16string iccid = {u"1234564654651123"};
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimIccId(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(iccid), Return(0)));
    int64_t value = 0;
    int32_t ret = netStatsService.GetTrafficPlanInfo(1, -1, value);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetStatsServiceTest, GetTrafficPlanInfoTest05, TestSize.Level1)
{
    NetStatsService netStatsService;
    std::u16string iccid = {u"1234564654651"};
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSlotId(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimIccId(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(iccid), Return(0)));
    int64_t value = 0;
    int32_t ret = netStatsService.GetTrafficPlanInfo(1, 99, value);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetStatsServiceTest, CellularDataStateChangedFfrtTest01, TestSize.Level1)
{
    NetStatsService netStatsService;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimId(_)).WillRepeatedly(Return(1));
    int32_t dataState = 0;
    int32_t slotId = -1;
    int32_t ret = netStatsService.CellularDataStateChangedFfrt(slotId, dataState);
    EXPECT_EQ(ret, false);
}

HWTEST_F(NetStatsServiceTest, CellularDataStateChangedFfrtTest02, TestSize.Level1)
{
    NetStatsService netStatsService;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimId(_)).WillRepeatedly(Return(1));
    int32_t dataState = 0;
    int32_t slotId = 0;
    int32_t ret = netStatsService.CellularDataStateChangedFfrt(slotId, dataState);
    EXPECT_EQ(ret, true);
}

HWTEST_F(NetStatsServiceTest, CellularDataStateChangedFfrtTest03, TestSize.Level1)
{
    NetStatsService netStatsService;
    EXPECT_CALL(MockCoreServiceManager::GetInstance(), GetSimId(_)).WillRepeatedly(Return(1));
    int32_t dataState = 0;
    int32_t slotId = 0;
    netStatsService.trafficPlanService_->trafficPlanInfoMap_[1] = std::make_shared<TrafficPlanInfo>();
    int32_t ret = netStatsService.CellularDataStateChangedFfrt(slotId, dataState);
    EXPECT_EQ(ret, true);
}
#endif
} // namespace NetManagerStandard
} // namespace OHOS

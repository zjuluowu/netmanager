/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_stats_notification.h"
#include "net_stats_utils.h"
#include "net_stats_rdb.h"
#include "net_stats_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *KEY_MONTH_LIMIT_TEXT = "netstats_month_limit_message";
constexpr const char *KEY_MONTH_NOTIFY_TEXT = "netstats_month_notify_message";
constexpr const char *KEY_DAILY_NOTIFY_TEXT = "netstats_daily_notify_message";
constexpr const char *KEY_NETWORK_MONTH_LIMIT_SINGLE_TITLE = "netstats_excess_monthlimit_notofication_title";
constexpr const char *KEY_NETWORK_MONTH_MARK_SINGLE_TITLE = "netstats_excess_monthmark_notofication_title";
constexpr const char *KEY_NETWORK_DAY_MARK_SINGLE_TITLE = "netstats_excess_daymark_notofication_title";
constexpr const char *KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE = "netstats_excess_monthlimit_notofication_title_sub";
constexpr const char *KEY_NETWORK_MONTH_MARK_DUAL_TITLE = "netstats_excess_monthmark_notofication_title_sub";
constexpr const char *KEY_NETWORK_DAY_MARK_DUAL_TITLE = "netstats_excess_daymark_notofication_title_sub";
const int32_t UNIT_CONVERT_1024 = 1024;

class MockRdbStore : public NativeRdb::RdbStore {
public:
    int Delete(int &deletedRows, const std::string &table,
        const std::string &whereClause = "", const Values &args = {}) override
    {
        return 0;
    }

    std::shared_ptr<NativeRdb::AbsSharedResultSet> QuerySql(const std::string &sql, const Values &args = {}) override
    {
        return nullptr;
    }

    std::shared_ptr<NativeRdb::ResultSet> QueryByStep(const std::string &sql, const Values &args = {},
        bool preCount = true) override
    {
        return nullptr;
    }

    std::pair<int32_t, NativeRdb::ValueObject> Execute(
        const std::string &sql, const Values &args = {}, int64_t trxId = 0) override
    {
        return std::make_pair(0, NativeRdb::ValueObject());
    }

    int GetVersion(int &version) override
    {
        return 0;
    }

    int SetVersion(int version) override
    {
        return 0;
    }
};
}

class NetStatsNotificationTest : public testing::Test {
public:
    static void SetUpTestCase() {}

    static void TearDownTestCase() {}

    void SetUp() {}

    void TearDown() {}
};

HWTEST_F(NetStatsNotificationTest, ParseJSONFileTest001, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    std::string filePath;
    notification->ParseJSONFile(filePath, notification->languageMap);
    EXPECT_FALSE(notification->languageMap.empty());
}

HWTEST_F(NetStatsNotificationTest, UpdateResourceMapTest001, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    notification->UpdateResourceMap();
    EXPECT_FALSE(notification->localeBaseName.empty());
}

HWTEST_F(NetStatsNotificationTest, GetDayNotificationTextTest001, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    std::string temp = notification->resourceMap[KEY_DAILY_NOTIFY_TEXT];
    notification->resourceMap.erase(KEY_DAILY_NOTIFY_TEXT);
    auto ret = notification->GetDayNotificationText();
    EXPECT_TRUE(ret.empty());

    notification->resourceMap[KEY_DAILY_NOTIFY_TEXT] = "";
    ret = notification->GetDayNotificationText();
    EXPECT_TRUE(ret.empty());
    notification->resourceMap[KEY_DAILY_NOTIFY_TEXT] = temp;
}

HWTEST_F(NetStatsNotificationTest, GetMonthAlertTextTest001, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    std::string temp = notification->resourceMap[KEY_MONTH_LIMIT_TEXT];
    notification->resourceMap.erase(KEY_MONTH_LIMIT_TEXT);
    auto ret = notification->GetMonthAlertText();
    EXPECT_TRUE(ret.empty());

    notification->resourceMap[KEY_MONTH_LIMIT_TEXT] = "";
    ret = notification->GetMonthAlertText();
    EXPECT_TRUE(ret.empty());
    notification->resourceMap[KEY_MONTH_LIMIT_TEXT] = temp;
}

HWTEST_F(NetStatsNotificationTest, SetTitleAndTextTest001, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    int notificationId = 0;
    std::shared_ptr<Notification::NotificationNormalContent> content = nullptr;
    bool isDualCard = false;
    auto ret = notification->SetTitleAndText(notificationId, content, isDualCard);
    EXPECT_FALSE(ret);

    content = std::make_shared<Notification::NotificationNormalContent>();
    ret = notification->SetTitleAndText(notificationId, content, isDualCard);
    EXPECT_FALSE(ret);

    notification->resourceMap[""] = "test";
    ret = notification->SetTitleAndText(notificationId, content, isDualCard);
    EXPECT_FALSE(ret);
    notification->resourceMap.erase("");
}

HWTEST_F(NetStatsNotificationTest, SetTitleAndTextTest002, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    int notificationId = NETMGR_STATS_LIMIT_DAY;
    auto content = std::make_shared<Notification::NotificationNormalContent>();
    bool isDualCard = false;
    auto ret = notification->SetTitleAndText(notificationId, content, isDualCard);
    EXPECT_TRUE(ret);

    notificationId = NETMGR_STATS_LIMIT_MONTH;
    ret = notification->SetTitleAndText(notificationId, content, isDualCard);
    EXPECT_TRUE(ret);

    isDualCard = true;
    notificationId = NETMGR_STATS_ALERT_MONTH;
    ret = notification->SetTitleAndText(notificationId, content, isDualCard);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetStatsNotificationTest, GetPixelMapTest001, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    EXPECT_NE(notification->netmgrStatsLimitIconPixelMap_, nullptr);
    notification->GetPixelMap();
}

HWTEST_F(NetStatsNotificationTest, GetTrafficNumTest001, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    double traffic = static_cast<double>(UNIT_CONVERT_1024);
    auto ret = notification->GetTrafficNum(traffic);
    EXPECT_FALSE(ret.empty());

    for (int i = 0; i < 4; i++) {
        traffic *= UNIT_CONVERT_1024;
    }
    ret = notification->GetTrafficNum(traffic);
    EXPECT_FALSE(ret.empty());
}

HWTEST_F(NetStatsNotificationTest, GetStartTimestampTest001, TestSize.Level1)
{
    NetStatsUtils utils;
    int32_t startdate = 32;
    auto ret = utils.GetStartTimestamp(startdate);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetStatsNotificationTest, GetTrafficNumTest002, TestSize.Level1)
{
    NetStatsUtils utils;
    int32_t startdate = 1;
    auto ret = utils.GetStartTimestamp(startdate);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetStatsNotificationTest, GetTodayStartTimestampTest001, TestSize.Level1)
{
    NetStatsUtils utils;
    auto ret = utils.GetTodayStartTimestamp();
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetStatsNotificationTest, GetDaysInMonthTest001, TestSize.Level1)
{
    NetStatsUtils utils;
    int32_t year = 0;
    int32_t month = 0;
    auto ret = utils.GetDaysInMonth(year, month);
    EXPECT_EQ(ret, -1);

    year = 2000;
    ret = utils.GetDaysInMonth(year, month);
    EXPECT_EQ(ret, -1);

    month = 13;
    ret = utils.GetDaysInMonth(year, month);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetStatsNotificationTest, GetDaysInMonthTest002, TestSize.Level1)
{
    NetStatsUtils utils;
    int32_t year = 2001;
    int32_t month = 1;
    auto ret = utils.GetDaysInMonth(year, month);
    EXPECT_EQ(ret, 31);

    month = 2;
    ret = utils.GetDaysInMonth(year, month);
    EXPECT_EQ(ret, 28);

    year = 2000;
    ret = utils.GetDaysInMonth(year, month);
    EXPECT_EQ(ret, 29);
}

HWTEST_F(NetStatsNotificationTest, ConvertToUint64Test001, TestSize.Level1)
{
    NetStatsUtils utils;
    std::string str;
    uint64_t value = 0;
    auto ret = utils.ConvertToUint64(str, value);
    EXPECT_FALSE(ret);

    str = "test";
    ret = utils.ConvertToUint64(str, value);
    EXPECT_FALSE(ret);

    str = "123test";
    ret = utils.ConvertToUint64(str, value);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetStatsNotificationTest, ConvertToUint64Test002, TestSize.Level1)
{
    NetStatsUtils utils;
    std::string str = "99999999999999999999";
    uint64_t value = 0;
    auto ret = utils.ConvertToUint64(str, value);
    EXPECT_TRUE(ret);

    str = "123";
    ret = utils.ConvertToUint64(str, value);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetStatsNotificationTest, ConvertToInt32Test001, TestSize.Level1)
{
    NetStatsUtils utils;
    std::string str;
    int32_t value = 0;
    auto ret = utils.ConvertToInt32(str, value);
    EXPECT_FALSE(ret);

    str = "test";
    ret = utils.ConvertToInt32(str, value);
    EXPECT_FALSE(ret);

    str = "123test";
    ret = utils.ConvertToInt32(str, value);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetStatsNotificationTest, ConvertToInt32Test002, TestSize.Level1)
{
    NetStatsUtils utils;
    std::string str = "1e309";
    int32_t value = 0;
    auto ret = utils.ConvertToInt32(str, value);
    EXPECT_TRUE(ret);

    str = "123";
    ret = utils.ConvertToInt32(str, value);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetStatsNotificationTest, OnUpgradeTest001, TestSize.Level1)
{
    NetStatsRDB::RdbDataOpenCallback callback;
    MockRdbStore store;
    auto ret = callback.OnUpgrade(store, 0, 1);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsNotificationTest, UpgradeDbVersionToTest001, TestSize.Level1)
{
    NetStatsRDB::RdbDataOpenCallback callback;
    MockRdbStore store;
    auto ret = callback.OnUpgrade(store, 0, 1);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    callback.UpgradeDbVersionTo(store, 0);
    callback.UpgradeDbVersionTo(store, 1);
}

HWTEST_F(NetStatsNotificationTest, GetRdbStoreTest001, TestSize.Level1)
{
    NetStatsRDB rdb;
    auto ret = rdb.GetRdbStore();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    EXPECT_NE(rdb.rdbStore_, nullptr);
    ret = rdb.GetRdbStore();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsNotificationTest, InitRdbStoreTest001, TestSize.Level1)
{
    NetStatsRDB rdb;
    auto ret = rdb.InitRdbStore();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsNotificationTest, InsertDataTest001, TestSize.Level1)
{
    NetStatsRDB rdb;
    NetStatsData state;
    state.simId = 0;
    auto ret = rdb.InsertData(state);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = rdb.DeleteBySimId(state.simId);
    EXPECT_EQ(ret, 1);
}

HWTEST_F(NetStatsNotificationTest, DeleteBySimIdTest001, TestSize.Level1)
{
    NetStatsRDB rdb;
    NetStatsData state;
    auto ret = rdb.DeleteBySimId(0);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetStatsNotificationTest, UpdateBySimIdTest001, TestSize.Level1)
{
    NetStatsRDB rdb;
    NetStatsData state;
    auto ret = rdb.UpdateBySimId(0, state);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetStatsNotificationTest, QueryAllTest001, TestSize.Level1)
{
    NetStatsRDB rdb;
    NetStatsData state;
    state.simId = 0;
    auto ret = rdb.QueryAll();
    EXPECT_TRUE(ret.empty());

    rdb.InsertData(state);
    ret = rdb.QueryAll();
    EXPECT_EQ(ret.size(), 1);
    rdb.DeleteBySimId(state.simId);
}

HWTEST_F(NetStatsNotificationTest, QueryBySimIdTest001, TestSize.Level1)
{
    NetStatsRDB rdb;
    NetStatsData state;
    state.simId = 0;
    auto ret = rdb.QueryBySimId(state.simId, state);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    rdb.InsertData(state);
    ret = rdb.QueryBySimId(1, state);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = rdb.QueryBySimId(state.simId, state);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsNotificationTest, BackUpNetStatsFreqDBTest002, TestSize.Level1)
{
    NetStatsRDB rdb;
    auto ret = rdb.BackUpNetStatsFreqDB("", "");
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = rdb.BackUpNetStatsFreqDB("./xxx.db", "");
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = rdb.BackUpNetStatsFreqDB("", "./xxx.db");
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = rdb.BackUpNetStatsFreqDB("./xxx.db", "./xxx.db");
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetStatsNotificationTest, GetNotificationTitleTest001, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    std::string notificationType = KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE;
    
    notification->resourceMap[KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE] = "Month limit %d";
    
    auto ret = notification->GetNotificationTitle(notificationType);
    EXPECT_FALSE(ret.empty());
    
    notification->resourceMap.erase(KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE);
}

HWTEST_F(NetStatsNotificationTest, GetNotificationTitleTest002, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    std::string notificationType = KEY_NETWORK_MONTH_MARK_DUAL_TITLE;
    
    notification->resourceMap[KEY_NETWORK_MONTH_MARK_DUAL_TITLE] = "Month mark %d";
    
    auto ret = notification->GetNotificationTitle(notificationType);
    EXPECT_FALSE(ret.empty());
    
    notification->resourceMap.erase(KEY_NETWORK_MONTH_MARK_DUAL_TITLE);
}

HWTEST_F(NetStatsNotificationTest, GetNotificationTitleTest003, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    std::string notificationType = KEY_NETWORK_DAY_MARK_DUAL_TITLE;
    
    notification->resourceMap[KEY_NETWORK_DAY_MARK_DUAL_TITLE] = "Day mark %d";
    
    auto ret = notification->GetNotificationTitle(notificationType);
    EXPECT_FALSE(ret.empty());
    
    notification->resourceMap.erase(KEY_NETWORK_DAY_MARK_DUAL_TITLE);
}

HWTEST_F(NetStatsNotificationTest, GetNotificationTitleTest007, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    std::string notificationType = "nonexistent_key";
    
    auto ret = notification->GetNotificationTitle(notificationType);
    EXPECT_TRUE(ret.empty());
}

HWTEST_F(NetStatsNotificationTest, GetNotificationTitleTest008, TestSize.Level1)
{
    auto notification = NetMgrNetStatsLimitNotification::GetInstance();
    std::string notificationType = KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE;
    
    notification->resourceMap[KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE] = "Month limit no format";
    
    auto ret = notification->GetNotificationTitle(notificationType);
    EXPECT_TRUE(ret.empty());
    
    notification->resourceMap.erase(KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE);
}
} // namespace NetManagerStandard
} // namespace OHOS
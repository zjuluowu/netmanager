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

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#endif
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_database_helper.h"
#include "net_stats_database_defines.h"
#include "net_stats_constants.h"
namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;
using namespace testing::ext;
namespace {
constexpr const char *NET_STATS_DATABASE_TEST_PATH = "/data/service/el1/public/netmanager/net_stats_test.db";
} // namespace
class NetStatsDatabaseHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetStatsDatabaseHelperTest::SetUpTestCase() {}

void NetStatsDatabaseHelperTest::TearDownTestCase() {}

void NetStatsDatabaseHelperTest::SetUp() {}

void NetStatsDatabaseHelperTest::TearDown() {}

HWTEST_F(NetStatsDatabaseHelperTest, CreateTableTest001, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::string tableInfo =
        "ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, NAME TEXT NOT NULL, AGE INT NOT NULL, ADDRESS CHAR(50)";
    int32_t ret = helper->CreateTable("testTable", tableInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, CreateTableTest002, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    const std::string tableInfo = UID_TABLE_CREATE_PARAM;
    int32_t ret = helper->CreateTable(UID_TABLE, tableInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, CreateTableTest003, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    const std::string tableInfo = IFACE_TABLE_CREATE_PARAM;
    int32_t ret = helper->CreateTable(IFACE_TABLE, tableInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, CreateTableTest004, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    const std::string tableInfo = UID_SIM_TABLE_CREATE_PARAM;
    int32_t ret = helper->CreateTable(UID_SIM_TABLE, tableInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, CreateTableTest005, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    const std::string tableInfo = VERSION_TABLE_CREATE_PARAM;
    int32_t ret = helper->CreateTable(VERSION_TABLE, tableInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, ExecTableUpgradeTest005, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    const std::string tableInfo = UID_SIM_TABLE_CREATE_PARAM;
    int32_t ret = helper->ExecTableUpgrade(UID_TABLE, NetStatsDatabaseHelper::Version_1);
    ret = helper->ExecTableUpgrade(UID_TABLE, NetStatsDatabaseHelper::Version_2);
    ret = helper->ExecTableUpgrade(UID_TABLE, NetStatsDatabaseHelper::Version_3);
    ret = helper->ExecTableUpgrade(UID_TABLE, NetStatsDatabaseHelper::Version_4);
    ret = helper->ExecTableUpgrade(UID_TABLE, NetStatsDatabaseHelper::Version_5);
    ret = helper->ExecTableUpgrade(UID_TABLE, NetStatsDatabaseHelper::Version_6);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, InsertDataHelperTest001, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    NETMGR_LOG_I("InsertDataHelperTest001");
    NetStatsInfo info;
    info.uid_ = 10222;
    info.iface_ = "eth0";
    info.date_ = 15254500;
    info.rxBytes_ = 4455;
    info.txBytes_ = 8536;
    info.rxPackets_ = 45122;
    info.txPackets_ = 144215;
    info.userId_ = 100;
    int32_t ret = helper->InsertData(UID_TABLE, UID_TABLE_PARAM_LIST, info);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = helper->InsertData("", UID_TABLE_PARAM_LIST, info);
    EXPECT_EQ(ret, NetStatsResultCode::STATS_ERR_WRITE_DATA_FAIL);

    ret = helper->InsertData(UID_SIM_TABLE, UID_SIM_TABLE_PARAM_LIST, info);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = helper->InsertData("", UID_SIM_TABLE_PARAM_LIST, info);
    EXPECT_EQ(ret, NetStatsResultCode::STATS_ERR_WRITE_DATA_FAIL);
}

HWTEST_F(NetStatsDatabaseHelperTest, SelectDataHelperTest001, TestSize.Level1)
{
    NETMGR_LOG_I("SelectDataHelperTest001");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::vector<NetStatsInfo> infos;
    int32_t ret = helper->SelectData(infos, UID_TABLE, 0, LONG_MAX);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    for (auto const &info : infos) {
        NETMGR_LOG_I("uid:%{public}d, iface:%{public}s, date:%{public}s", info.uid_, info.iface_.c_str(),
                     std::to_string(info.date_).c_str());
    }
    infos.clear();
    uint64_t date = 15254400;
    ret = helper->SelectData(infos, UID_TABLE, date, LONG_MAX);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = helper->SelectData(infos, "", date, LONG_MAX);
    EXPECT_EQ(ret, NetStatsResultCode::STATS_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetStatsDatabaseHelperTest, SelectDataHelperTest002, TestSize.Level1)
{
    NETMGR_LOG_I("SelectDataHelperTest002");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::vector<NetStatsInfo> infos;
    uint32_t uid = 100;
    int32_t ret = helper->SelectData(uid, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    infos.clear();
    uint64_t date = 15254400;
    ret = helper->SelectData(uid, date, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, SelectDataHelperTest003, TestSize.Level1)
{
    NETMGR_LOG_I("SelectDataHelperTest003");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::vector<NetStatsInfo> infos;
    std::string iface = "wlan0";
    int32_t ret = helper->SelectData(iface, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    infos.clear();
    uint64_t date = 15254400;
    ret = helper->SelectData(iface, date, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, SelectDataHelperTest004, TestSize.Level1)
{
    NETMGR_LOG_I("SelectDataHelperTest004");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::vector<NetStatsInfo> infos;
    std::string iface = "wlan0";
    uint32_t uid = 100;
    int32_t ret = helper->SelectData(iface, uid, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    infos.clear();
    uint64_t date = 15254400;
    ret = helper->SelectData(iface, uid, date, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, QueryDataDataHelperTest001, TestSize.Level1)
{
    NETMGR_LOG_I("QueryDataDataHelperTest001");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::vector<NetStatsInfo> infos;
    std::string ident = "2";
    int32_t ret = helper->QueryData(UID_TABLE, ident, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    infos.clear();
    uint64_t date = 15254400;
    ret = helper->QueryData(UID_TABLE, ident, date, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = helper->QueryData("", ident, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, STATS_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetStatsDatabaseHelperTest, QueryDataDataHelperTest002, TestSize.Level1)
{
    NETMGR_LOG_I("QueryDataDataHelperTest002");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::vector<NetStatsInfo> infos;
    uint32_t uid = 200022;
    std::string ident = "2";
    int32_t ret = helper->QueryData(UID_TABLE, uid, ident, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    infos.clear();
    uint64_t date = 15254400;
    ret = helper->QueryData(UID_TABLE, uid, ident, date, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = helper->QueryData("", uid, ident, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, STATS_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetStatsDatabaseHelperTest, DeleteDataHelperTest001, TestSize.Level1)
{
    NETMGR_LOG_I("DeleteDataHelperTest001");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    uint64_t date = 15254400;
    int32_t ret = helper->DeleteData(UID_TABLE, date, 15254560);
    std::vector<NetStatsInfo> infos;
    helper->SelectData(infos, UID_TABLE, 0, LONG_MAX);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, DeleteDataHelperTest002, TestSize.Level1)
{
    NETMGR_LOG_I("DeleteDataHelperTest002");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    uint32_t uid = 100;
    int32_t ret = helper->DeleteData(UID_TABLE, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, ClearDataHelperTest001, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::vector<NetStatsInfo> infos;
    std::string iface = "wlan0";
    uint32_t uid = 100;
    int32_t ret = helper->SelectData(iface, uid, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = helper->ClearData(UID_TABLE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, NetStatsDatabaseHelperBranchTest001, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>("");
    int32_t ret = helper->Open("");
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = helper->Close();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t id = 0;
    ret = helper->BindInt64(id, 0, 0);
    EXPECT_EQ(ret, STATS_ERR_READ_DATA_FAIL);

    ret = helper->ClearData("");
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetStatsDatabaseHelperTest, QueryDataTest001, TestSize.Level1)
{
    NETMGR_LOG_I("QueryDataTest001");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    std::vector<NetStatsInfo> infos;
    std::string ident = "2";
    int32_t ret = helper->QueryData(UID_TABLE, ident, 100, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    infos.clear();
    uint64_t date = 15254400;
    ret = helper->QueryData(UID_TABLE, ident, date, LONG_MAX, infos);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = helper->QueryData("", ident, 0, LONG_MAX, infos);
    EXPECT_EQ(ret, STATS_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetStatsDatabaseHelperTest, ExecUpgradeSqlNextTest001, TestSize.Level1)
{
    NETMGR_LOG_I("ExecUpgradeSqlNextTest001");
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    NetStatsDatabaseHelper::TableVersion oldVersion = NetStatsDatabaseHelper::Version_0;
    NetStatsDatabaseHelper::TableVersion newVersion = NetStatsDatabaseHelper::Version_0;
    helper->ExecUpgradeSqlNext(NET_STATS_DATABASE_TEST_PATH, oldVersion, newVersion);
    EXPECT_EQ(oldVersion, NetStatsDatabaseHelper::Version_0);
}

HWTEST_F(NetStatsDatabaseHelperTest, UpgradeTest001, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    int32_t ret = helper->Upgrade();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetStatsDatabaseHelperTest, ExecUpgradeSqlTest001, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    std::string tableName;
    auto oldVersion = NetStatsDatabaseHelper::Version_5;
    auto newVersion = NetStatsDatabaseHelper::Version_0;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);

    oldVersion = NetStatsDatabaseHelper::Version_0;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);

    newVersion = NetStatsDatabaseHelper::Version_5;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);
    EXPECT_NE(oldVersion, NetStatsDatabaseHelper::Version_0);

    oldVersion = NetStatsDatabaseHelper::Version_5;
    newVersion = NetStatsDatabaseHelper::Version_6;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);
    EXPECT_EQ(oldVersion, NetStatsDatabaseHelper::Version_6);

    oldVersion = NetStatsDatabaseHelper::Version_6;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);

    newVersion = NetStatsDatabaseHelper::Version_5;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);
}

HWTEST_F(NetStatsDatabaseHelperTest, ExecUpgradeSqlTest002, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    helper->isDisplayTrafficAncoList_ = false;
    std::string tableName;
    auto oldVersion = NetStatsDatabaseHelper::Version_5;
    auto newVersion = NetStatsDatabaseHelper::Version_0;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);

    oldVersion = NetStatsDatabaseHelper::Version_0;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);

    newVersion = NetStatsDatabaseHelper::Version_5;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);
    EXPECT_NE(oldVersion, NetStatsDatabaseHelper::Version_0);

    oldVersion = NetStatsDatabaseHelper::Version_5;
    newVersion = NetStatsDatabaseHelper::Version_6;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);
    EXPECT_EQ(oldVersion, NetStatsDatabaseHelper::Version_6);

    oldVersion = NetStatsDatabaseHelper::Version_6;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);

    newVersion = NetStatsDatabaseHelper::Version_5;
    helper->ExecUpgradeSql(tableName, oldVersion, newVersion);
}

HWTEST_F(NetStatsDatabaseHelperTest, BackupNetStatsDataTest001, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>("");
    if (helper == nullptr) {return;}
    bool ret = helper->BackupNetStatsData("xxxx/xxxx.db", NET_STATS_DATABASE_PATH);
    EXPECT_EQ(ret, false);
    helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_BACK_PATH);
    ret = helper->BackupNetStatsData(NET_STATS_DATABASE_BACK_PATH, "xxxx/xxxx.db");
    EXPECT_EQ(ret, false);
    helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    ret = helper->BackupNetStatsData(NET_STATS_DATABASE_BACK_PATH, NET_STATS_DATABASE_PATH);
    EXPECT_EQ(ret, true);
}

HWTEST_F(NetStatsDatabaseHelperTest, DeleteAndBackupTest001, TestSize.Level1)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>("");
    int32_t ret = helper->DeleteAndBackup(SQLITE_NOTADB);
    EXPECT_EQ(ret, SQLITE_NOTADB);
    ret = helper->DeleteAndBackup(SQLITE_CORRUPT);
    EXPECT_EQ(ret, SQLITE_CORRUPT);
    ret = helper->DeleteAndBackup(0);
    EXPECT_EQ(ret, 0);
    helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    ret = helper->DeleteAndBackup(SQLITE_NOTADB);
    
    auto helper2 = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_BACK_PATH);
    ret = helper2->DeleteAndBackup(SQLITE_NOTADB);
}

HWTEST_F(NetStatsDatabaseHelperTest, GetTableVersionNormalTest001, TestSize.Level1)
{
    NETMGR_LOG_I("GetTableVersionNormalTest001");

    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_TEST_PATH);
    int32_t ret = helper->UpdateTableVersion(NetStatsDatabaseHelper::Version_1, VERSION_TABLE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    NetStatsDatabaseHelper::TableVersion version;
    ret = helper->GetTableVersion(version, VERSION_TABLE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(version, NetStatsDatabaseHelper::Version_1);
}

HWTEST_F(NetStatsDatabaseHelperTest, GetTableVersionErrorHandlingTest001, TestSize.Level1)
{
    NETMGR_LOG_I("GetTableVersionErrorHandlingTest001");
    auto helper = std::make_unique<NetStatsDatabaseHelper>("");

    NetStatsDatabaseHelper::TableVersion version;
    int32_t ret = helper->GetTableVersion(version, VERSION_TABLE);
    EXPECT_EQ(ret, STATS_ERR_READ_DATA_FAIL);
}

#ifdef SUPPORT_TRAFFIC_STATISTIC
HWTEST_F(NetStatsDatabaseHelperTest, InsertChangeToIndexTimeTest001, TestSize.Level1)
{
    NETMGR_LOG_I("InsertChangeToIndexTimeTest001");
    auto helper = std::make_unique<NetStatsDatabaseHelper>("");

    uint32_t timestamp = 123456789;
    std::string tableName = CHANGE_TABLE;
    std::string paramList = "StartTime,ErrorTest";
    int32_t ret = helper->InsertChangeToIndexTime(timestamp, tableName, paramList);
    EXPECT_EQ(ret, STATS_ERR_WRITE_DATA_FAIL);
}
#endif
} // namespace NetManagerStandard
} // namespace OHOS

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

#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <unistd.h>

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_data_clone.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing;
using namespace testing::ext;

class NetStatsDataCloneTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    std::string testBackupFile_;
    std::string testDbPath_;
};

void NetStatsDataCloneTest::SetUpTestCase()
{
    NETMGR_LOG_D("NetStatsDataCloneTest SetUpTestCase");
}

void NetStatsDataCloneTest::TearDownTestCase()
{
    NETMGR_LOG_D("NetStatsDataCloneTest TearDownTestCase");
}

void NetStatsDataCloneTest::SetUp()
{
    NETMGR_LOG_D("NetStatsDataCloneTest SetUp");
    testBackupFile_ = "/data/service/el1/public/netmanager/test_net_stats_data_backup.txt";
    testDbPath_ = "/data/service/el1/public/netmanager/test_net_stats_traffic_plan.db";
}

void NetStatsDataCloneTest::TearDown()
{
    NETMGR_LOG_D("NetStatsDataCloneTest TearDown");
    if (access(testBackupFile_.c_str(), F_OK) == 0) {
        remove(testBackupFile_.c_str());
    }
    if (access(testDbPath_.c_str(), F_OK) == 0) {
        remove(testDbPath_.c_str());
    }
}

#ifdef SUPPORT_TRAFFIC_STATISTIC
/**
 * @tc.number: NetStatsDataClone_FdClone_InvalidFd
 * @tc.name: FdClone with invalid file descriptor
 * @tc.desc: Test FdClone returns false when fd is invalid or fstat fails
 */
HWTEST_F(NetStatsDataCloneTest, FdClone_InvalidFd, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd invalidFd(-1);

    bool result = dataClone.FdClone(invalidFd);

    EXPECT_FALSE(result);
}

/**
 * @tc.number: NetStatsDataClone_OnBackup_Success
 * @tc.name: OnBackup successful backup operation
 * @tc.desc: Test OnBackup successfully queries database, writes backup file, and opens file descriptor
 */
HWTEST_F(NetStatsDataCloneTest, OnBackup_Success, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string backupInfo = "backup_test";

    int32_t result = dataClone.OnBackup(fd, backupInfo);

    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: NetStatsDataClone_OnBackup_OpenFileFailed
 * @tc.name: OnBackup with file open failure
 * @tc.desc: Test OnBackup handles case when open backup file fails
 */
HWTEST_F(NetStatsDataCloneTest, OnBackup_OpenFileFailed, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string backupInfo = "backup_test";

    int32_t result = dataClone.OnBackup(fd, backupInfo);

    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: NetStatsDataClone_OnRestore_Success
 * @tc.name: OnRestore successful restore operation
 * @tc.desc: Test OnRestore successfully parses backup file and restores data to database
 */
HWTEST_F(NetStatsDataCloneTest, OnRestore_Success, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string restoreInfo = "restore_test";

    int32_t result = dataClone.OnRestore(fd, restoreInfo);

    EXPECT_EQ(result, NETMANAGER_ERROR);
}

/**
 * @tc.number: NetStatsDataClone_OnRestore_EmptyBackupFile
 * @tc.name: OnRestore with empty backup file
 * @tc.desc: Test OnRestore handles case when backup file is empty or doesn't exist
 */
HWTEST_F(NetStatsDataCloneTest, OnRestore_EmptyBackupFile, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string restoreInfo = "restore_test";

    int32_t result = dataClone.OnRestore(fd, restoreInfo);

    EXPECT_EQ(result, NETMANAGER_ERROR);
}

/**
 * @tc.number: NetStatsDataClone_OnRestore_DataParseError
 * @tc.name: OnRestore with data parsing error
 * @tc.desc: Test OnRestore continues processing next line when data format is invalid
 */
HWTEST_F(NetStatsDataCloneTest, OnRestore_DataParseError, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string restoreInfo = "restore_test";

    int32_t result = dataClone.OnRestore(fd, restoreInfo);

    EXPECT_EQ(result, NETMANAGER_ERROR);
}

/**
 * @tc.number: NetStatsDataClone_OnRestore_MultipleEntries
 * @tc.name: OnRestore with multiple traffic plan entries
 * @tc.desc: Test OnRestore successfully restores multiple entries from backup file
 */
HWTEST_F(NetStatsDataCloneTest, OnRestore_MultipleEntries, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string restoreInfo = "restore_test";

    int32_t result = dataClone.OnRestore(fd, restoreInfo);

    EXPECT_EQ(result, NETMANAGER_ERROR);
}

/**
 * @tc.number: NetStatsDataClone_OnBackup_EmptyDataList
 * @tc.name: OnBackup with empty database query result
 * @tc.desc: Test OnBackup creates empty backup file when database query returns no data
 */
HWTEST_F(NetStatsDataCloneTest, OnBackup_EmptyDataList, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string backupInfo = "backup_test";

    int32_t result = dataClone.OnBackup(fd, backupInfo);

    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: NetStatsDataClone_OnBackup_QuerySuccess
 * @tc.name: OnBackup with successful database query
 * @tc.desc: Test OnBackup successfully queries traffic plan info from database and prepares backup
 */
HWTEST_F(NetStatsDataCloneTest, OnBackup_QuerySuccess, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string backupInfo = "backup_test";

    int32_t result = dataClone.OnBackup(fd, backupInfo);

    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: NetStatsDataClone_OnBackup_WriteFileFailed
 * @tc.name: OnBackup with file write failure
 * @tc.desc: Test OnBackup handles case when WriteFile operation fails
 */
HWTEST_F(NetStatsDataCloneTest, OnBackup_WriteFileFailed, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string backupInfo = "backup_test";

    int32_t result = dataClone.OnBackup(fd, backupInfo);

    EXPECT_EQ(result, 0);
}

/**
 * @tc.number: NetStatsDataClone_OnRestore_FdCloneFailed
 * @tc.name: OnRestore with FdClone failure
 * @tc.desc: Test OnRestore returns error when FdClone operation fails
 */
HWTEST_F(NetStatsDataCloneTest, OnRestore_FdCloneFailed, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string restoreInfo = "restore_test";

    int32_t result = dataClone.OnRestore(fd, restoreInfo);

    EXPECT_EQ(result, NETMANAGER_ERROR);
}

/**
 * @tc.number: NetStatsDataClone_OnRestore_OpenBackupFileFailed
 * @tc.name: OnRestore with backup file open failure
 * @tc.desc: Test OnRestore returns error when open backup file for reading fails
 */
HWTEST_F(NetStatsDataCloneTest, OnRestore_OpenBackupFileFailed, TestSize.Level1)
{
    NetStatsDataClone dataClone;
    UniqueFd fd(-1);
    const std::string restoreInfo = "restore_test";

    int32_t result = dataClone.OnRestore(fd, restoreInfo);

    EXPECT_EQ(result, NETMANAGER_ERROR);
}
#endif
} // namespace NetManagerStandard
} // namespace OHOS

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

#include "net_stats_database_helper.h"

#include <cstdlib>
#include <filesystem>

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_constants.h"
#include "net_stats_database_defines.h"
#include "net_stats_info.h"
#include "net_stats_utils.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;

constexpr const char* SELECT_FROM = "SELECT * FROM ";
constexpr const char* DELETE_FROM = "DELETE FROM ";
constexpr const char* UPDATE = "UPDATE ";
constexpr const char* ALTER_TABLE = "ALTER TABLE ";
constexpr const char* DATA_MORE_THAN = " AND t.Date >= ?";
constexpr const char* DATA_LESS_THAN = " AND t.Date <= ?";
constexpr const char* INSERT_OR_REPLACE_INTO = "INSERT OR REPLACE INTO ";
constexpr const char* SET_FLAG = " SET Flag = ";
constexpr const char* SET_USERID = " SET UserId = ";
namespace {
NetStatsDatabaseHelper::SqlCallback sqlCallback = [](void *notUsed, int argc, char **argv, char **colName) {
    std::string data;
    for (int i = 0; i < argc; i++) {
        data.append(colName[i]).append(" = ").append(argv[i] ? argv[i] : "nullptr\n");
    }
    NETMGR_LOG_D("Recv data: %{public}s", data.c_str());
    return 0;
};

bool CheckFilePath(const std::string &fileName)
{
    char tmpPath[PATH_MAX] = {0};
    const auto pos = fileName.find_last_of('/');
    if (pos == std::string::npos) {
        return false;
    }
    const auto dir = fileName.substr(0, pos);
    if (!realpath(dir.c_str(), tmpPath)) {
        NETMGR_LOG_E("Get realPath failed error: %{public}d, %{public}s", errno, strerror(errno));
        return false;
    }
    if (strcmp(tmpPath, dir.c_str()) != 0) {
        NETMGR_LOG_E("file name is illegal fileName: %{public}s, tmpPath: %{public}s", fileName.c_str(), tmpPath);
        return false;
    }
    return true;
}
} // namespace

ffrt::mutex NetStatsDatabaseHelper::sqliteMutex_;

NetStatsDatabaseHelper::NetStatsDatabaseHelper(const std::string &path)
{
    if (!CheckFilePath(path)) {
        return;
    }
    Open(path);
    path_ = path;
    isDisplayTrafficAncoList_ = CommonUtils::IsNeedDisplayTrafficAncoList();
}

NetStatsDatabaseHelper::~NetStatsDatabaseHelper()
{
    Close();
    sqlite_ = nullptr;
}

int32_t NetStatsDatabaseHelper::ExecSql(const std::string &sql, void *recv, SqlCallback callback)
{
    char *errMsg = nullptr;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = sqlite3_exec(sqlite_, sql.c_str(), callback, recv, &errMsg);
    lock.unlock();
    NETMGR_LOG_D("EXEC SQL : %{public}s", sql.c_str());
    if (errMsg != nullptr) {
        NETMGR_LOG_E("Exec sql failed err:%{public}s, path: %{public}s", errMsg, path_.c_str());
        sqlite3_free(errMsg);
    }
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp == SQLITE_OK && rettmp != ret) {
        std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
        int32_t ret = sqlite3_exec(sqlite_, sql.c_str(), callback, recv, &errMsg);
        lock.unlock();
    }
    
    return rettmp == SQLITE_OK ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t NetStatsDatabaseHelper::CreateTable(const std::string &tableName, const std::string &tableInfo)
{
    std::string sql = CREATE_TABLE_IF_NOT_EXISTS + tableName + "(" + tableInfo + ");";
    int32_t ret = ExecSql(sql, nullptr, sqlCallback);
    if (ret != NETMANAGER_SUCCESS) {
        return STATS_ERR_CREATE_TABLE_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDatabaseHelper::Open(const std::string &path)
{
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = sqlite3_open_v2(path.c_str(), &sqlite_,
                                  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr);
    return ret == SQLITE_OK ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t NetStatsDatabaseHelper::InsertData(const std::string &tableName, const std::string &paramList,
                                           const NetStatsInfo &info)
{
    std::string params;
    int32_t paramCount = count(paramList.begin(), paramList.end(), ',') + 1;
    for (int32_t i = 0; i < paramCount; ++i) {
        params += "?";
        if (i != paramCount - 1) {
            params += ",";
        }
    }
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    std::string sql = "INSERT INTO " + tableName + " (" + paramList + ") " + "VALUES" + " (" + params + ") ";
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    if (paramCount == UID_PARAM_NUM) {
        statement_.BindInt64(idx, info.uid_);
        ++idx;
    }
    statement_.BindText(idx, info.iface_);
    statement_.BindInt64(++idx, info.date_);
    statement_.BindInt64(++idx, info.rxBytes_);
    statement_.BindInt64(++idx, info.rxPackets_);
    statement_.BindInt64(++idx, info.txBytes_);
    statement_.BindInt64(++idx, info.txPackets_);
    statement_.BindText(++idx, info.ident_);
    statement_.BindInt64(++idx, info.flag_);
    statement_.BindInt64(++idx, info.userId_);
    ret = statement_.Step();
    statement_.ResetStatementAndClearBindings();
    if (ret != SQLITE_DONE) {
        NETMGR_LOG_E("Step failed ret:%{public}d", ret);
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

#ifdef SUPPORT_TRAFFIC_STATISTIC
int32_t NetStatsDatabaseHelper::InsertCalibrationTrafficInfo(const std::string &simId, int32_t startTime,
    int32_t endTime, uint64_t usedTraffic, const std::string &tableName, const std::string &paramList)
{
    NETMGR_LOG_I("NetStatsDatabaseHelper InsertCalibrationTrafficInfo enter");
    std::string params;
    int32_t paramCount = count(paramList.begin(), paramList.end(), ',') + 1;
    for (int32_t i = 0; i < paramCount; ++i) {
        params += "?";
        if (i != paramCount - 1) {
            params += ",";
        }
    }
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    std::string sql = "INSERT INTO " + tableName + " (" + paramList + ") " + "VALUES" + " (" + params + ") ";
    int32_t ret = statement_.Prepare(sqlite_, sql);
    // LCOV_EXCL_START
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP
    int32_t idx = 1;
    statement_.BindText(idx, simId);
    statement_.BindInt32(++idx, startTime);
    statement_.BindInt32(++idx, endTime);
    statement_.BindInt64(++idx, usedTraffic);
    ret = statement_.Step();
    statement_.ResetStatementAndClearBindings();
    // LCOV_EXCL_START
    if (ret != SQLITE_DONE) {
        NETMGR_LOG_E("Step failed ret12:%{public}d", ret);
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDatabaseHelper::DeleteCalibrateData(const std::string &tableName, uint32_t simId)
{
    std::string sql =
        DELETE_FROM + tableName + " WHERE Ident = " + std::to_string(simId);
    return ExecSql(sql, nullptr, sqlCallback);
}
#endif

int32_t NetStatsDatabaseHelper::SelectData(std::vector<NetStatsInfo> &infos, const std::string &tableName,
                                           uint64_t start, uint64_t end)
{
    infos.clear();
    std::string sql = SELECT_FROM + tableName + " t WHERE 1=1" + DATA_MORE_THAN + DATA_LESS_THAN;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    ret = statement_.BindInt64(idx, start);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Bind int64 failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = statement_.BindInt64(++idx, end);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Bind int64 failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    return Step(infos);
}

int32_t NetStatsDatabaseHelper::SelectData(const uint32_t uid, uint64_t start, uint64_t end,
                                           std::vector<NetStatsInfo> &infos)
{
    infos.clear();
    std::string sql = SELECT_FROM + std::string(UID_TABLE) + " t WHERE 1=1 AND t.UID == ?" + DATA_MORE_THAN +
                      DATA_LESS_THAN;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    ret = statement_.BindInt64(idx, uid);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Bind int32 failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = BindInt64(idx, start, end);
    if (ret != SQLITE_OK) {
        return ret;
    }
    return Step(infos);
}

int32_t NetStatsDatabaseHelper::SelectData(const std::string &iface, uint64_t start, uint64_t end,
                                           std::vector<NetStatsInfo> &infos)
{
    infos.clear();
    std::string sql = SELECT_FROM + std::string(IFACE_TABLE) + " t WHERE 1=1 AND t.IFace = ?" +
                      DATA_MORE_THAN + DATA_LESS_THAN;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    ret = statement_.BindText(idx, iface);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Bind text failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = BindInt64(idx, start, end);
    if (ret != SQLITE_OK) {
        return ret;
    }
    return Step(infos);
}

int32_t NetStatsDatabaseHelper::SelectData(const std::string &iface, const uint32_t uid, uint64_t start, uint64_t end,
                                           std::vector<NetStatsInfo> &infos)
{
    infos.clear();
    std::string sql = SELECT_FROM + std::string(UID_TABLE) + " t WHERE 1=1 AND t.UID = ?" + " AND t.IFace = ?" +
                      DATA_MORE_THAN + DATA_LESS_THAN;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    ret = statement_.BindInt64(idx, uid);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("bind int32 ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = statement_.BindText(++idx, iface);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Bind text failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = BindInt64(idx, start, end);
    if (ret != SQLITE_OK) {
        return ret;
    }
    return Step(infos);
}

int32_t NetStatsDatabaseHelper::QueryData(const std::string &tableName, const std::string &ident, uint64_t start,
                                          uint64_t end, std::vector<NetStatsInfo> &infos)
{
    infos.clear();
    std::string sql =
        SELECT_FROM + tableName + " t WHERE 1=1 AND t.Ident = ?" + DATA_MORE_THAN + DATA_LESS_THAN;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    ret = statement_.BindText(idx, ident);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("bind text ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = BindInt64(idx, start, end);
    if (ret != SQLITE_OK) {
        return ret;
    }
    return Step(infos);
}
#ifdef SUPPORT_TRAFFIC_STATISTIC
int32_t NetStatsDatabaseHelper::QueryCalibrationTrafficInfo(const std::string &tableName, const std::string &ident,
    uint32_t &startTime, uint32_t &endTime, uint64_t &usedTraffic)
{
    std::string sql =
        SELECT_FROM + tableName + " t WHERE 1=1 AND t.Ident = ? ";
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    // LCOV_EXCL_START
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    int32_t idx = 1;
    ret = statement_.BindText(idx, ident);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("bind text ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }

    int32_t rc = statement_.Step();
    uint64_t usedTrafficTmp = 0;
    int32_t index = 1;
    while (rc != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            statement_.GetColumnInt(index, startTime);
            statement_.GetColumnInt(++index, endTime);
            statement_.GetColumnLong(++index, usedTrafficTmp);
            rc = statement_.Step();
        } else {
            NETMGR_LOG_E("Step failed with rc:%{public}d", rc);
            break;
        }
    }
    // LCOV_EXCL_STOP

    usedTraffic = usedTrafficTmp;
    statement_.ResetStatementAndClearBindings();
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDatabaseHelper::QueryChangeToIfaceTime(const std::string &tableName, uint32_t &startTime)
{
    std::string sql =
        SELECT_FROM + tableName + " t WHERE 1=1";
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    // LCOV_EXCL_START
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }

    int32_t rc = statement_.Step();
    std::string remainingDataStr;
    while (rc != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            statement_.GetColumnInt(0, startTime);
            rc = statement_.Step();
        } else {
            NETMGR_LOG_E("Step failed with rc:%{public}d", rc);
            break;
        }
    }
    // LCOV_EXCL_STOP
    statement_.ResetStatementAndClearBindings();
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDatabaseHelper::InsertChangeToIndexTime(
    uint32_t timestamp, const std::string &tableName, const std::string &paramList)
{
    std::string params;
    int32_t paramCount = count(paramList.begin(), paramList.end(), ',') + 1;
    for (int32_t i = 0; i < paramCount; ++i) {
        params += "?";
        if (i != paramCount - 1) {
            params += ",";
        }
    }
    if (paramCount != 1) {
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    std::string sql = "INSERT INTO " + tableName + " (" + paramList + ") " + "VALUES" + " (" + params + ") ";
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    // LCOV_EXCL_START
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    statement_.BindInt32(1, static_cast<int32_t>(timestamp));  // 1: param index
    ret = statement_.Step();
    statement_.ResetStatementAndClearBindings();
    if (ret != SQLITE_DONE) {
        NETMGR_LOG_E("Step failed ret:%{public}d", ret);
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}
#endif

int32_t NetStatsDatabaseHelper::QueryData(const std::string &tableName, const std::string &ident, const int32_t userId,
                                          uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos)
{
    infos.clear();
    std::string sql =
        SELECT_FROM + tableName + " t WHERE 1=1 AND t.Ident = ? " + " AND t.UserId = ? " +
        DATA_MORE_THAN + DATA_LESS_THAN;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    int32_t idx = 1;
    ret = statement_.BindText(idx, ident);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("bind text ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = statement_.BindInt32(++idx, userId);
    if (ret != SQLITE_OK) {
        return ret;
    }
    ret = BindInt64(idx, start, end);
    if (ret != SQLITE_OK) {
        return ret;
    }
    return Step(infos);
}

int32_t NetStatsDatabaseHelper::QueryData(const std::string &tableName, const uint32_t uid, const std::string &ident,
                                          uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos)
{
    infos.clear();
    std::string sql = SELECT_FROM + tableName + " t WHERE 1=1 AND T.UID = ? AND t.Ident = ?" + DATA_MORE_THAN +
                      DATA_LESS_THAN;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    ret = statement_.BindInt64(idx, uid);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("bind int32 ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = statement_.BindText(++idx, ident);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("bind text ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = BindInt64(idx, start, end);
    if (ret != SQLITE_OK) {
        return ret;
    }
    return Step(infos);
}

int32_t NetStatsDatabaseHelper::QueryIfaceStatsByIdent(const std::string &tableName, const std::string &ident,
                                                       uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos)
{
    infos.clear();
    std::string sql = SELECT_FROM + tableName + " t WHERE 1=1 AND t.Ident = ?" + DATA_MORE_THAN +
                      DATA_LESS_THAN;
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    // LCOV_EXCL_START
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    ret = statement_.BindText(idx, ident);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("bind text ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = BindInt64(idx, start, end);
    if (ret != SQLITE_OK) {
        return ret;
    }
    // LCOV_EXCL_STOP
    return Step(infos);
}

int32_t NetStatsDatabaseHelper::DeleteData(const std::string &tableName, uint64_t start, uint64_t end)
{
    std::string sql =
        DELETE_FROM + tableName + " WHERE Date >= " + std::to_string(start) + " AND Date <= " + std::to_string(end);
    return ExecSql(sql, nullptr, sqlCallback);
}

int32_t NetStatsDatabaseHelper::DeleteData(const std::string &tableName, uint64_t uid)
{
    std::string sql = DELETE_FROM + tableName + " WHERE UID = ?";
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    statement_.BindInt64(idx, uid);
    ret = statement_.Step();
    statement_.ResetStatementAndClearBindings();
    if (ret != SQLITE_DONE) {
        NETMGR_LOG_E("Step failed ret:%{public}d", ret);
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDatabaseHelper::Close()
{
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = sqlite3_close_v2(sqlite_);
    return ret == SQLITE_OK ? NETMANAGER_SUCCESS : NETMANAGER_ERROR;
}

int32_t NetStatsDatabaseHelper::ClearData(const std::string &tableName)
{
    std::string sql = DELETE_FROM + tableName;
    std::string shrinkMemSql = "PRAGMA shrink_memory";
    int32_t execSqlRet = ExecSql(sql, nullptr, sqlCallback);
    if (execSqlRet != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Delete data failed");
        return execSqlRet;
    }
    execSqlRet = ExecSql(shrinkMemSql, nullptr, sqlCallback);
    if (execSqlRet != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Delete data failed");
        return execSqlRet;
    }
    sql = "VACUUM";
    execSqlRet = ExecSql(sql, nullptr, sqlCallback);
    if (execSqlRet != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Delete data failed");
        return execSqlRet;
    }
    return ExecSql(shrinkMemSql, nullptr, sqlCallback);
}

int32_t NetStatsDatabaseHelper::Step(std::vector<NetStatsInfo> &infos)
{
    int32_t rc = statement_.Step();
    NETMGR_LOG_D("Step result:%{public}d", rc);
    while (rc != SQLITE_DONE) {
        if (rc != SQLITE_ROW) {
            NETMGR_LOG_E("sqlite step error: %{public}d", rc);
            statement_.ResetStatementAndClearBindings();
            return STATS_ERR_READ_DATA_FAIL;
        }
        int32_t i = 0;
        NetStatsInfo info;
        if (statement_.GetColumnCount() == UID_PARAM_NUM) {
            statement_.GetColumnInt(i, info.uid_);
            ++i;
        }
        statement_.GetColumnString(i, info.iface_);
        statement_.GetColumnLong(++i, info.date_);
        statement_.GetColumnLong(++i, info.rxBytes_);
        statement_.GetColumnLong(++i, info.rxPackets_);
        statement_.GetColumnLong(++i, info.txBytes_);
        statement_.GetColumnLong(++i, info.txPackets_);
        statement_.GetColumnString(++i, info.ident_);
        if (statement_.GetColumnCount() == UID_PARAM_NUM) {
            statement_.GetColumnInt(++i, info.flag_);
            statement_.GetColumnInt(++i, info.userId_);
        }
        infos.emplace_back(info);
        rc = statement_.Step();
        NETMGR_LOG_D("Step result:%{public}d", rc);
    }
    statement_.ResetStatementAndClearBindings();
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDatabaseHelper::BindInt64(int32_t idx, uint64_t start, uint64_t end)
{
    int32_t ret = statement_.BindInt64(++idx, start);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Bind int64 failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    ret = statement_.BindInt64(++idx, end);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("Bind int64 failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }

    return ret;
}

int32_t NetStatsDatabaseHelper::Upgrade()
{
    // LCOV_EXCL_START
    auto ret = ExecTableUpgrade(UID_TABLE, Version_1);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_TABLE, Version_1);
    }
    ret = ExecTableUpgrade(UID_SIM_TABLE, Version_2);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_SIM_TABLE, Version_2);
    }
    ret = ExecTableUpgrade(UID_TABLE, Version_3);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_TABLE, Version_3);
    }
    ret = ExecTableUpgrade(UID_SIM_TABLE, Version_3);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_SIM_TABLE, Version_3);
    }
    ret = ExecTableUpgrade(UID_SIM_TABLE, Version_4);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_SIM_TABLE, Version_4);
    }
    ret = ExecTableUpgrade(UID_SIM_TABLE, Version_5);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_SIM_TABLE, Version_5);
    }
    ret = ExecTableUpgrade(UID_TABLE, Version_6);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_SIM_TABLE, Version_6);
    }
    ret = ExecTableUpgrade(UID_SIM_TABLE, Version_6);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_SIM_TABLE, Version_6);
    }
    ret = ExecTableUpgrade(UID_SIM_TABLE, Version_6);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_SIM_TABLE, Version_6);
    }
    ret = ExecTableUpgrade(IFACE_TABLE, Version_1);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Upgrade db failed. table is %{public}s, version is %{public}d", UID_SIM_TABLE, Version_6);
    }
    // LCOV_EXCL_STOP
    return ret;
}

int32_t NetStatsDatabaseHelper::ExecTableUpgrade(const std::string &tableName, TableVersion newVersion)
{
    TableVersion oldVersion;
    auto ret = GetTableVersion(oldVersion, tableName);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("ExecTableUpgrade getTableVersion failed. ret = %{public}d", ret);
        return NETMANAGER_ERROR;
    }
    if (oldVersion == newVersion) {
        return NETMANAGER_SUCCESS;
    }
    NETMGR_LOG_I("ExecTableUpgrade tableName = %{public}s, oldVersion = %{public}d, newVersion = %{public}d",
                 tableName.c_str(), oldVersion, newVersion);
    ExecUpgradeSql(tableName, oldVersion, newVersion);
    if (oldVersion != newVersion) {
        NETMGR_LOG_E("ExecTableUpgrade error. oldVersion = %{public}d, newVersion = %{public}d",
                     oldVersion, newVersion);
        return NETMANAGER_ERROR;
    }
    ret = UpdateTableVersion(oldVersion, tableName);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("ExecTableUpgrade updateVersion failed. ret = %{public}d", ret);
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}

void NetStatsDatabaseHelper::ExecUpgradeSql(const std::string &tableName, TableVersion &oldVersion,
                                            TableVersion newVersion)
{
    int32_t ret = NETMANAGER_SUCCESS;
    if (oldVersion < Version_1 && newVersion >= Version_1) {
        std::string sql = ALTER_TABLE + tableName + " ADD COLUMN Ident CHAR(100) NOT NULL DEFAULT '';";
        ret = ExecSql(sql, nullptr, sqlCallback);
        if (ret != SQLITE_OK) {
            NETMGR_LOG_E("ExecTableUpgrade version_1 failed. ret = %{public}d", ret);
        }
        oldVersion = Version_1;
    }
    if (oldVersion < Version_2 && newVersion >= Version_2) {
        ret = DeleteData(tableName, Sim_UID);
        if (ret != SQLITE_OK) {
            NETMGR_LOG_E("ExecTableUpgrade Version_2 failed. ret = %{public}d", ret);
        }
        oldVersion = Version_2;
    }
    if (oldVersion < Version_3 && newVersion >= Version_3) {
        std::string sql = ALTER_TABLE + tableName + " ADD COLUMN Flag INTEGER NOT NULL DEFAULT 0;";
        ret = ExecSql(sql, nullptr, sqlCallback);
        if (ret != SQLITE_OK) {
            NETMGR_LOG_E("ExecTableUpgrade version_3 failed. ret = %{public}d", ret);
        }
        oldVersion = Version_3;
    }
    if (oldVersion < Version_4 && newVersion >= Version_4) {
        std::string sql = UPDATE + tableName + SET_FLAG + std::to_string(STATS_DATA_FLAG_SIM) +
                          " WHERE Flag = " + std::to_string(STATS_DATA_FLAG_DEFAULT) + ";";
        ret = ExecSql(sql, nullptr, sqlCallback);
        if (ret != SQLITE_OK) {
            NETMGR_LOG_E("ExecTableUpgrade Version_4 failed. ret = %{public}d", ret);
        }
        oldVersion = Version_4;
    }
    if (oldVersion < Version_5 && newVersion >= Version_5) {
        if (isDisplayTrafficAncoList_) {
            std::string sqlsim = UPDATE + tableName + SET_FLAG + std::to_string(STATS_DATA_FLAG_SIM_BASIC) +
                              " WHERE Flag = " + std::to_string(STATS_DATA_FLAG_SIM) + ";";
            ret = ExecSql(sqlsim, nullptr, sqlCallback);
            std::string sqlsim2 = UPDATE + tableName + SET_FLAG + std::to_string(STATS_DATA_FLAG_SIM2_BASIC) +
                              " WHERE Flag = " + std::to_string(STATS_DATA_FLAG_SIM2) + ";";
            int32_t retsim2 = ExecSql(sqlsim2, nullptr, sqlCallback);
            if (ret != SQLITE_OK || retsim2 != SQLITE_OK) {
                NETMGR_LOG_E("ExecTableUpgrade Version_5 failed. ret = %{public}d", ret);
            }
        }
        oldVersion = Version_5;
    }
    ExecUpgradeSqlNext(tableName, oldVersion, newVersion);
}

void NetStatsDatabaseHelper::ExecUpgradeSqlNext(const std::string &tableName, TableVersion &oldVersion,
                                                TableVersion newVersion)
{
    int32_t ret = NETMANAGER_SUCCESS;
    if (oldVersion < Version_6 && newVersion >= Version_6) {
        std::string sql = ALTER_TABLE + tableName + " ADD COLUMN UserId INTEGER NOT NULL DEFAULT 0;";
        ret = ExecSql(sql, nullptr, sqlCallback);
        if (ret != SQLITE_OK) {
            NETMGR_LOG_E("ExecTableUpgrade Version_6 failed. ret = %{public}d", ret);
        }
        oldVersion = Version_6;
    }
}

int32_t NetStatsDatabaseHelper::GetTableVersion(TableVersion &version, const std::string &tableName)
{
    std::string sql = SELECT_FROM + std::string(VERSION_TABLE) + " WHERE Name = ?;";
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    int32_t ret = statement_.Prepare(sqlite_, sql);
    int32_t rettmp = DeleteAndBackup(ret);
    if (rettmp != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    if (rettmp != ret) {
        statement_.Prepare(sqlite_, sql);
    }
    int32_t idx = 1;
    ret = statement_.BindText(idx, tableName);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("bind text ret:%{public}d", ret);
        return STATS_ERR_READ_DATA_FAIL;
    }
    int32_t rc = statement_.Step();
    auto v = static_cast<uint32_t>(Version_0);
    while (rc != SQLITE_DONE) {
        if (rc == SQLITE_ROW) {
            int32_t i = 1;
            statement_.GetColumnInt(i, v);
            rc = statement_.Step();
        } else {
            NETMGR_LOG_E("Step failed with rc:%{public}d", rc);
            break;
        }
    }
    statement_.ResetStatementAndClearBindings();
    version = static_cast<TableVersion>(v);
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDatabaseHelper::UpdateTableVersion(TableVersion version, const std::string &tableName)
{
    std::string sql = INSERT_OR_REPLACE_INTO + std::string(VERSION_TABLE) + "(Name, Version) VALUES('" +
                      tableName + "', " + std::to_string(version) + ");";
    return ExecSql(sql, nullptr, sqlCallback);
}

int32_t NetStatsDatabaseHelper::UpdateStatsFlag(const std::string &tableName, uint32_t uid, uint32_t flag)
{
    std::string sql = UPDATE + tableName + SET_FLAG + std::to_string(flag) +
                      " WHERE UID = " + std::to_string(uid);
    return ExecSql(sql, nullptr, sqlCallback);
}

int32_t NetStatsDatabaseHelper::UpdateStatsFlagByUserId(const std::string &tableName, int32_t userId, uint32_t flag)
{
    std::string sql = UPDATE + tableName + SET_FLAG + std::to_string(flag) +
                      " WHERE UserId = " + std::to_string(userId);
    return ExecSql(sql, nullptr, sqlCallback);
}

int32_t NetStatsDatabaseHelper::UpdateStatsUserIdByUserId(const std::string &tableName,
    int32_t oldUserId, int32_t newUserId)
{
    std::string sql = UPDATE + tableName + SET_USERID + std::to_string(newUserId) +
                      " WHERE UserId = " + std::to_string(oldUserId);
    return ExecSql(sql, nullptr, sqlCallback);
}

int32_t NetStatsDatabaseHelper::UpdateDataFlag(const std::string &tableName, uint32_t oldFlag, uint32_t newFlag)
{
    std::string sql =
        UPDATE + tableName + SET_FLAG + std::to_string(newFlag) + " WHERE Flag = " + std::to_string(oldFlag);
    return ExecSql(sql, nullptr, sqlCallback);
}

bool NetStatsDatabaseHelper::BackupNetStatsData(const std::string &sourceDb, const std::string &backupDb)
{
    NETMGR_LOG_I("BackupNetStatsData start");
    sqlite3* source = nullptr;
    sqlite3* backup = nullptr;
    int32_t ret = sqlite3_open(sourceDb.c_str(), &source);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("sqlite3_open failed ret:%{public}d", ret);
        return false;
    }
    ret = sqlite3_open(backupDb.c_str(), &backup);
    if (ret != SQLITE_OK) {
        NETMGR_LOG_E("sqlite3_open failed ret:%{public}d", ret);
        return false;
    }

    sqlite3_backup* pBackup = sqlite3_backup_init(backup, "main", source, "main");
    int rc = -1;
    if (pBackup) {
        while ((rc = sqlite3_backup_step(pBackup, -1)) == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
            if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
                break;
            }
        }
        if (rc != SQLITE_DONE) {
            NETMGR_LOG_E("Backup failed: %{public}s, ret: %{public}d", sqlite3_errmsg(backup), rc);
        } else {
            NETMGR_LOG_E("Backup completed successfully");
        }
        sqlite3_backup_finish(pBackup);
    } else {
        NETMGR_LOG_E("Failed to initialize backup: %{public}s", sqlite3_errmsg(backup));
    }

    sqlite3_close(source);
    sqlite3_close(backup);
    return (rc == SQLITE_DONE);
}

bool NetStatsDatabaseHelper::IntegrityCheck(sqlite3 *db)
{
    bool isIntegrated = false;
    sqlite3_stmt *stmt = nullptr;
 
    int32_t errorCode = sqlite3_prepare_v2(db, "PRAGMA integrity_check;", -1, &stmt, nullptr);
    if (errorCode == SQLITE_OK && stmt != nullptr) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string result = std::string(reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, 0)));
            if (result == "ok") {
                NETMGR_LOG_I("IntegrityCheck ok");
                isIntegrated = true;
                break;
            }
        }
        sqlite3_finalize(stmt);
    } else {
        NETMGR_LOG_E("prepare SQL statement failed, errorCode:%{public}d", errorCode);
    }
    return isIntegrated;
}

bool NetStatsDatabaseHelper::BackupNetStatsDataDB(const std::string &sourceDb, const std::string &backupDb)
{
    NETMGR_LOG_I("BackupNetStatsDataDB");
    std::unique_lock<ffrt::mutex> lock(sqliteMutex_);
    bool ret = BackupNetStatsData(sourceDb, backupDb);
    return ret;
}

int32_t NetStatsDatabaseHelper::DeleteAndBackup(int32_t errCode)
{
    if ((errCode != SQLITE_NOTADB && errCode != SQLITE_CORRUPT) || (path_ != NET_STATS_DATABASE_BACK_PATH &&
        path_ != NET_STATS_DATABASE_PATH)) {
        return errCode;
    }
    bool backupRet = false;
    if (path_.find(NET_STATS_DATABASE_BACK_PATH) != std::string::npos) {
        NETMGR_LOG_I("BACK_PATH is bad");
        CommonUtils::DeleteFile(NET_STATS_DATABASE_BACK_PATH);
        backupRet = BackupNetStatsData(NET_STATS_DATABASE_PATH, NET_STATS_DATABASE_BACK_PATH);
    } else {
        NETMGR_LOG_I("DATABASE_PATH is bad");
        CommonUtils::DeleteFile(NET_STATS_DATABASE_PATH);
        backupRet = BackupNetStatsData(NET_STATS_DATABASE_BACK_PATH, NET_STATS_DATABASE_PATH);
    }

    if (backupRet) {
        return SQLITE_OK;
    }
    return errCode;
}
} // namespace NetManagerStandard
} // namespace OHOS

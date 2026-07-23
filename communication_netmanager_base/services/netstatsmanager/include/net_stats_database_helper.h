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

#ifndef NET_STATS_DATABASE_HELPER_H
#define NET_STATS_DATABASE_HELPER_H

#include <climits>
#include <functional>
#include <string>

#ifndef USE_SQLITE_SYMBOLS
#include "sqlite3.h"
#else
#include "sqlite3sym.h"
#endif

#include "net_stats_sqlite_statement.h"
#include "net_stats_info.h"
#include "ffrt_inner.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsDatabaseHelper {
public:
    using SqlCallback = sqlite3_callback;
    explicit NetStatsDatabaseHelper(const std::string &path);
    NetStatsDatabaseHelper() = delete;
    ~NetStatsDatabaseHelper();

    int32_t CreateTable(const std::string &tableName, const std::string &tableInfo);
    int32_t InsertData(const std::string &tableName, const std::string &paramList,
                       const NetStatsInfo &info);
    int32_t SelectData(std::vector<NetStatsInfo> &infos, const std::string &tableName, uint64_t start, uint64_t end);
    int32_t SelectData(const uint32_t uid, uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos);
    int32_t SelectData(const std::string &iface, uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos);
    int32_t SelectData(const std::string &iface, const uint32_t uid, uint64_t start, uint64_t end,
                       std::vector<NetStatsInfo> &infos);
    int32_t QueryData(const std::string &tableName, const std::string &ident, uint64_t start, uint64_t end,
                      std::vector<NetStatsInfo> &infos);
    int32_t QueryData(const std::string &tableName, const uint32_t uid, const std::string &ident, uint64_t start,
                      uint64_t end, std::vector<NetStatsInfo> &infos);
    int32_t QueryData(const std::string &tableName, const std::string &ident, const int32_t userId,
                      uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos);
    int32_t DeleteData(const std::string &tableName, uint64_t start, uint64_t end);
    int32_t DeleteData(const std::string &tableName, uint64_t uid);
    int32_t ClearData(const std::string &tableName);
    int32_t Step(std::vector<NetStatsInfo> &infos);
    int32_t ExecSql(const std::string &sql, void *recv, SqlCallback callback);
    int32_t Upgrade();
    int32_t UpdateStatsFlag(const std::string &tableName, uint32_t uid, uint32_t flag);
    int32_t UpdateDataFlag(const std::string &tableName, uint32_t oldFlag, uint32_t newFlag);
    bool BackupNetStatsDataDB(const std::string &sourceDb, const std::string &backupDb);
    int32_t DeleteAndBackup(int32_t errCode);
    bool IntegrityCheck(sqlite3 *db);
    int32_t UpdateStatsFlagByUserId(const std::string &tableName, int32_t userId, uint32_t flag);
    int32_t UpdateStatsUserIdByUserId(const std::string &tableName, int32_t oldUserId, int32_t newUserId);
    int32_t QueryIfaceStatsByIdent(const std::string &tableName, const std::string &ident,
                                   uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    int32_t InsertCalibrationTrafficInfo(const std::string &simId, int32_t startTime, int32_t endTime,
        uint64_t usedTraffic, const std::string &tableName, const std::string &paramList);
    int32_t QueryCalibrationTrafficInfo(const std::string &tableName, const std::string &ident,
        uint32_t &startTime, uint32_t &endTime, uint64_t &usedTraffic);
    int32_t QueryChangeToIfaceTime(const std::string &tableName, uint32_t &startTime);
    int32_t InsertChangeToIndexTime(uint32_t timestamp, const std::string &tableName, const std::string &paramList);
    int32_t DeleteCalibrateData(const std::string &tableName, uint32_t simId);
#endif

private:
    enum TableVersion : int32_t {
        Version_0 = 0,
        Version_1,
        Version_2,
        Version_3,
        Version_4,
        Version_5,
        Version_6, // private space
    };

private:
    int32_t Open(const std::string &path);
    int32_t Close();
    int32_t BindInt64(int32_t idx, uint64_t start, uint64_t end);
    int32_t GetTableVersion(TableVersion &version, const std::string &tableName);
    int32_t UpdateTableVersion(TableVersion version, const std::string &tableName);
    int32_t ExecTableUpgrade(const std::string &tableName, TableVersion newVersion);
    void ExecUpgradeSql(const std::string &tableName, TableVersion &oldVersion, TableVersion newVersion);
    bool BackupNetStatsData(const std::string &sourceDb, const std::string &backupDb);
    void ExecUpgradeSqlNext(const std::string &tableName, TableVersion &oldVersion, TableVersion newVersion);
    sqlite3 *sqlite_ = nullptr;
    NetStatsSqliteStatement statement_;
    static ffrt::mutex sqliteMutex_;
    std::mutex mutex_;
    std::atomic<bool> isNeedUpdate_ = false;
    std::string path_ = "";
    bool isDisplayTrafficAncoList_ = false;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_STATS_DATABASE_HELPER_H

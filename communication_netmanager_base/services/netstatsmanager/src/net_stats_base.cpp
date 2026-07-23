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
#include "net_stats_rdb.h"

#include "traffic_plan_param.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;
namespace {
[[maybe_unused]] const int32_t RDB_VERSION_0 = 0;
const int32_t RDB_VERSION_1 = 1;
const int32_t RDB_VERSION_2 = 2;
const int32_t RDB_VERSION_3 = 3;
const std::string NETMANAGER_DB_SIMID_STATS_TABLE = "uid_stats_infos";
const std::string SQL_TABLE_COLUMS = std::string(
    "simId INTEGER NOT NULL PRIMARY KEY, "
    "monthWarningdate INTEGER NOT NULL, dayNontificationdate INTEGER NOT NULL, monthNontificationdate "
    "INTEGER NOT NULL, monthWarningState INTEGER NOT NULL, dayNontificationState "
    "INTEGER NOT NULL, monNontificationState INTEGER NOT NULL");

constexpr const char *FILED_ICCID = "ICCID";
constexpr const char *FILED_SIMID = "simId";
constexpr const char *FILED_DISPLAY_TRAFFIC_SWITCH = "DisplayTrafficSwitch";
constexpr const char *FILED_UNLIMIT_TRAFFIC_SWITCH = "UnlimitTrafficSwitch";
constexpr const char *FILED_TRAFFIC_LIMIT = "TrafficLimit";
constexpr const char *FILED_START_DATE = "StartDate";
constexpr const char *FILED_OVER_LIMIT_BEHAVIOR = "OverLimitBehavior";
constexpr const char *FILED_MONTHLY_LIMIT_PERCENTAGE = "MonthlyLimitPercentage";
constexpr const char *FILED_DAILY_LIMIT_PERCENTAGE = "DailyLimitPercentage";
} // namespace

int NetStatsRDB::RdbDataOpenCallback::OnCreate(NativeRdb::RdbStore &rdbStore)
{
    return NETMANAGER_SUCCESS;
}

int NetStatsRDB::RdbDataOpenCallback::OnUpgrade(NativeRdb::RdbStore &store, int oldVersion, int newVersion)
{
    NETMGR_LOG_I("OnUpgrade, oldVersion: %{public}d, newVersion: %{public}d", oldVersion, newVersion);
    while (oldVersion < newVersion) {
        UpgradeDbVersionTo(store, ++oldVersion);
    }
    return NETMANAGER_SUCCESS;
}

/* 这里删除了AddIsBreaker */
void NetStatsRDB::RdbDataOpenCallback::UpgradeDbVersionTo(NativeRdb::RdbStore &store, int newVersion)
{
    switch (newVersion) {
        case RDB_VERSION_1:
        // When upgrading the rdb version to 1, the is_broker field was added, but some users failed the upgrade.
        case RDB_VERSION_2:
        case RDB_VERSION_3: {
            // Upgrade to version 3: create T_traffic_plan table
            std::string createTable = std::string(CREATE_TABLE_IF_NOT_EXISTS) + TRAFFIC_PLAN_TABLE + " (" +
                                      TRAFFIC_PLAN_TABLE_CREATE_PARAM + ")";
            int ret = store.ExecuteSql(createTable);
            if (ret != NativeRdb::E_OK) {
                NETMGR_LOG_E("Failed to create T_traffic_plan table during upgrade, ret: %{public}d", ret);
            } else {
                NETMGR_LOG_I("Successfully created T_traffic_plan table during upgrade to version 3");
            }
            break;
        }
        default:
            NETMGR_LOG_E("no such newVersion: %{public}d", newVersion);
    }
}

int32_t NetStatsRDB::GetRdbStore()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (rdbStore_ != nullptr) {
        return NETMANAGER_SUCCESS;
    }

    int errCode = NETMANAGER_SUCCESS;
    NativeRdb::RdbStoreConfig config(NOTICE_DATABASE_NAME);
    NetStatsRDB::RdbDataOpenCallback helper;
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_3, helper, errCode);
    if (rdbStore_ != nullptr) {
        NETMGR_LOG_E("RDB GetRdbStore success");
        return NETMANAGER_SUCCESS;
    }
    NETMGR_LOG_E("RDB create failed, errCode: %{public}d", errCode);
    if (errCode == NativeRdb::E_SQLITE_CORRUPT) {
        int rettmp = NativeRdb::RdbHelper::DeleteRdbStore(config);
        NETMGR_LOG_E("rdbStore_ DeleteRdbStore ret: %{public}d", rettmp);
        rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_3, helper, errCode);
        if (rdbStore_ != nullptr) {
            int restorRet = rdbStore_->Restore(NOTICE_DATABASE_BACK_NAME);
            NETMGR_LOG_E("RDB Restore restorRet: %{public}d", restorRet);
            if (restorRet == 0) {
                NETMGR_LOG_E("RDB rdbStore success");
                return NETMANAGER_SUCCESS;
            }
        }
    }
    return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
}

std::shared_ptr<NativeRdb::RdbStore> NetStatsRDB::GetRdbStorePtr()
{
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("NetStatsRDB GetRdbStore error");
        return nullptr;
    }

    return rdbStore_;
}
int32_t NetStatsRDB::BackUpNetStatsFreqDB(const std::string &sourceDB, const std::string &targetDB)
{
    NETMGR_LOG_E("RDB rdbStore BackUpNetStatsFreqDB start");
    if (sourceDB.empty() || targetDB.empty()) {
        NETMGR_LOG_E("sourceDB or targetDB is empty");
        return NETMANAGER_ERROR;
    }
    int errCode = NETMANAGER_SUCCESS;
    NativeRdb::RdbStoreConfig config(targetDB);
    NetStatsRDB::RdbDataOpenCallback helper;
    auto rdbStore = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_3, helper, errCode);
    if (rdbStore == nullptr) {
        NETMGR_LOG_E("RDB GetRdbStore failed, errCode: %{public}d", errCode);
        return NETMANAGER_ERROR;
    }
    int ret = rdbStore->Restore(sourceDB);
    NETMGR_LOG_E("RDB rdbStore BackUpNetStatsFreqDB Restore ret: %{public}d", ret);
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsRDB::InitRdbStore()
{
    InitRdbStoreBackupDB();
    NETMGR_LOG_I("NetStatsRDB InitRdbStore start");
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("NetStatsRDB GetRdbStore error");
        return ret;
    }

    std::string createTable =
        std::string(CREATE_TABLE_IF_NOT_EXISTS) + NETMANAGER_DB_SIMID_STATS_TABLE + " (" + SQL_TABLE_COLUMS + ")";
    int ret0 = rdbStore_->ExecuteSql(createTable);
    NETMGR_LOG_I("createTable uid_stats_infos ret = %{public}d", ret0);

    std::string createPlanTable =
        std::string(CREATE_TABLE_IF_NOT_EXISTS) + TRAFFIC_PLAN_TABLE + " (" + TRAFFIC_PLAN_TABLE_CREATE_PARAM + ")";
    int ret1 = rdbStore_->ExecuteSql(createPlanTable);
    NETMGR_LOG_I("createTable T_traffic_plan ret = %{public}d", ret1);
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsRDB::InitRdbStoreBackupDB()
{
    NETMGR_LOG_I("InitRdbStoreBackupDB start");
    int errCode = NETMANAGER_SUCCESS;
    NativeRdb::RdbStoreConfig config(NOTICE_DATABASE_BACK_NAME);
    NetStatsRDB::RdbDataOpenCallback helper;
    std::shared_ptr<NativeRdb::RdbStore> rdbStore =
        NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_3, helper, errCode);
    if (rdbStore == nullptr) {
        NETMGR_LOG_E("RDB create failed, errCode: %{public}d", errCode);
        if (errCode == NativeRdb::E_SQLITE_CORRUPT) {
            NETMGR_LOG_E("RDB create retry");
            NativeRdb::RdbHelper::DeleteRdbStore(config);
            rdbStore = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_3, helper, errCode);
            if (rdbStore == nullptr) {
                return NETMANAGER_ERROR;
            }
        } else {
            return NETMANAGER_ERROR;
        }
    }

    std::string createTable =
        std::string(CREATE_TABLE_IF_NOT_EXISTS) + NETMANAGER_DB_SIMID_STATS_TABLE + " (" + SQL_TABLE_COLUMS + ")";
    int ret = rdbStore->ExecuteSql(createTable);
    NETMGR_LOG_I("InitRdbStore ret = %{public}d", ret);

    std::string createPlanTable =
        std::string(CREATE_TABLE_IF_NOT_EXISTS) + TRAFFIC_PLAN_TABLE + " (" + TRAFFIC_PLAN_TABLE_CREATE_PARAM + ")";
    int ret2 = rdbStore->ExecuteSql(createPlanTable);
    NETMGR_LOG_I("InitRdbStore ret = %{public}d", ret2);
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsRDB::InsertData(NetStatsData state)
{
    NETMGR_LOG_I("InsertData");
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return NETMANAGER_ERROR;
    }

    NativeRdb::ValuesBucket statsValues;
    /*simIDd*/
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_SIMID, state.simId);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_MON_W, state.monWarningDate);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_DAY_N, state.dayNoticeDate);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_MON_N, state.monNoticeDate);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_MON_W_S, state.monWarningState);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_DAY_N_S, state.dayNoticeState);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_MON_N_S, state.monNoticeState);
    int64_t id = 0;
    ret = rdbStore_->Insert(id, NETMANAGER_DB_SIMID_STATS_TABLE, statsValues);
    if (ret != NativeRdb::E_OK) {
        ret = UpdateBySimId(state.simId, state);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("Update operation failed, result is %{public}d", ret);
            return NETMANAGER_ERROR;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetStatsRDB::DeleteBySimId(int32_t simId)
{
    NETMGR_LOG_I("DeleteBySimId");
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return NETMANAGER_ERROR;
    }

    int32_t deletedRows = -1;
    std::vector<std::string> whereArgs;
    NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_SIMID_STATS_TABLE};
    rdbPredicate.EqualTo(NetStatsRdbFiledConst::FILED_SIMID, std::to_string(simId));
    int32_t result = rdbStore_->Delete(deletedRows, rdbPredicate);
    if (result != NativeRdb::E_OK) {
        NETMGR_LOG_E("delete operation failed, result is %{public}d", result);
        return result;
    }

    return deletedRows;
}

int32_t NetStatsRDB::UpdateBySimId(int32_t simId, NetStatsData state)
{
    NETMGR_LOG_I("UpdateBySimId, simId:%{public}d", simId);
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return NETMANAGER_ERROR;
    }

    NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_SIMID_STATS_TABLE};
    NETMGR_LOG_I("begin EqualTo");
    rdbPredicate.EqualTo(NetStatsRdbFiledConst::FILED_SIMID, std::to_string(simId));
    NETMGR_LOG_I("end EqualTo");

    NativeRdb::ValuesBucket statsValues;
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_SIMID, simId);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_MON_W, state.monWarningDate);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_DAY_N, state.dayNoticeDate);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_MON_N, state.monNoticeDate);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_MON_W_S, state.monWarningState);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_DAY_N_S, state.dayNoticeState);
    statsValues.PutInt(NetStatsRdbFiledConst::FILED_MON_N_S, state.monNoticeState);

    int32_t rowId = -1;
    int32_t result = rdbStore_->Update(rowId, statsValues, rdbPredicate);
    if (result != NativeRdb::E_OK) {
        NETMGR_LOG_E("Update operation failed. Result %{public}d", result);
        return result;
    }

    return NETMANAGER_SUCCESS;
}

std::vector<NetStatsData> NetStatsRDB::QueryAll()
{
    NETMGR_LOG_I("QueryAll");
    std::vector<NetStatsData> result;
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return result;
    }

    NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_SIMID_STATS_TABLE};
    std::vector<std::string> whereArgs;
    auto queryResultSet = rdbStore_->Query(rdbPredicate, whereArgs);
    if (queryResultSet == nullptr) {
        return result;
    }

    bool isAtLastRow = false;
    queryResultSet->IsAtLastRow(isAtLastRow);
    while (!queryResultSet->GoToNextRow()) {
        NetStatsData stats;
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_ZERO, stats.simId);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_ONE, stats.monWarningDate);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_TWO, stats.dayNoticeDate);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_THR, stats.monNoticeDate);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_FUR, stats.monWarningState);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_FW, stats.dayNoticeState);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_SIX, stats.monNoticeState);
        result.emplace_back(stats);
    }
    queryResultSet->Close();
    return result;
}

int32_t NetStatsRDB::QueryBySimId(int simId, NetStatsData &simStats)
{
    NETMGR_LOG_I("QueryBySimId simId:%{public}d", simId);
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return NETMANAGER_ERROR;
    }

    NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_SIMID_STATS_TABLE};
    rdbPredicate.EqualTo(NetStatsRdbFiledConst::FILED_SIMID, simId);

    std::vector<std::string> whereArgs;
    auto queryResultSet = rdbStore_->Query(rdbPredicate, whereArgs);
    if (queryResultSet == nullptr) {
        NETMGR_LOG_E("QueryBySimId error");
        return NETMANAGER_ERROR;
    }

    int32_t rowCount = 0;
    ret = queryResultSet->GetRowCount(rowCount);
    if (ret != OHOS::NativeRdb::E_OK) {
        NETMGR_LOG_E("query setting failed, get row count failed, name:%{public}d, ret:%{public}d", simId, ret);
        queryResultSet->Close();
        return ret;
    }
    if (rowCount == 0) {
        NETMGR_LOG_D("query setting name:%{public}d, num is 0", simId);
        queryResultSet->Close();
        return NETMANAGER_ERROR;
    }

    while (!queryResultSet->GoToNextRow()) {
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_ONE, simStats.monWarningDate);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_TWO, simStats.dayNoticeDate);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_THR, simStats.monNoticeDate);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_FUR, simStats.monWarningState);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_FW, simStats.dayNoticeState);
        queryResultSet->GetInt(NetStatsRdbFiledConst::FILED_COLUMN_INDEX_SIX, simStats.monNoticeState);
        if (simStats.simId == simId) {
            queryResultSet->Close();
            return NETMANAGER_SUCCESS;
        }
    }

    queryResultSet->Close();
    return NETMANAGER_ERROR;
}


int32_t NetStatsRDB::InsertOrUpdateTrafficPlanInfo(const TrafficPlanInfo &info)
{
    NETMGR_LOG_I("InsertOrUpdateTrafficPlanInfo start, %{public}s",
                 info.ToString().c_str());

    NativeRdb::ValuesBucket values = BuildValuesBucket(info);
    int32_t ret = InsertTrafficPlanInfo(values);
    if (ret == NativeRdb::E_OK) {
        NETMGR_LOG_I("InsertOrUpdateTrafficPlanInfo success");
        return NETMANAGER_SUCCESS;
    }

    return UpdatePlanInfoByIccid(info);
}

NativeRdb::ValuesBucket NetStatsRDB::BuildValuesBucket(const TrafficPlanInfo &info)
{
    NativeRdb::ValuesBucket values;
    values.PutString(FILED_ICCID, info.iccid);
    values.PutInt(FILED_SIMID, info.simId);
    values.PutInt(FILED_DISPLAY_TRAFFIC_SWITCH, info.displayTrafficSwitch);
    values.PutInt(FILED_UNLIMIT_TRAFFIC_SWITCH, info.unlimitTrafficSwitch);
    values.PutLong(FILED_TRAFFIC_LIMIT, static_cast<int64_t>(info.trafficLimit));
    values.PutInt(FILED_START_DATE, info.startDate);
    values.PutInt(FILED_OVER_LIMIT_BEHAVIOR, info.overLimitBehavior);
    values.PutInt(FILED_MONTHLY_LIMIT_PERCENTAGE, info.monthlyLimitPercentage);
    values.PutInt(FILED_DAILY_LIMIT_PERCENTAGE, info.dailyLimitPercentage);
    return values;
}

int32_t NetStatsRDB::InsertTrafficPlanInfo(const NativeRdb::ValuesBucket &values)
{
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Failed to get RDB store");
        return ret;
    }
    
    if (rdbStore_ == nullptr) {
        NETMGR_LOG_E("rdbStore_ nullptr");
        return ret;
    }

    int64_t id = 0;
    int32_t ret2 = rdbStore_->Insert(id, TRAFFIC_PLAN_TABLE, values);

    NETMGR_LOG_I("InsertTrafficPlanInfo ret:%{public}d", ret2);
    return ret2;
}

int32_t NetStatsRDB::QueryTrafficPlanInfoByIccid(const std::string &iccid, TrafficPlanInfo &info)
{
    NETMGR_LOG_I("QueryTrafficPlanInfoByIccid start");

    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Failed to get RDB store");
        return ret;
    }

    NativeRdb::RdbPredicates rdbPredicate(TRAFFIC_PLAN_TABLE);
    rdbPredicate.EqualTo(FILED_ICCID, iccid);
    std::vector<std::string> whereArgs;
    
    auto queryResultSet = rdbStore_->Query(rdbPredicate, whereArgs);
    if (queryResultSet == nullptr) {
        NETMGR_LOG_E("QueryTrafficPlanInfoByIccid: queryResultSet is null");
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED;
    }

    int32_t rowCount = 0;
    ret = queryResultSet->GetRowCount(rowCount);
    if (ret != NativeRdb::E_OK) {
        NETMGR_LOG_E("Failed to get row count, ret: %{public}d", ret);
        queryResultSet->Close();
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED;
    }
    
    if (rowCount == 0) {
        NETMGR_LOG_E("QueryTrafficPlanInfoByIccid: no record found");
        queryResultSet->Close();
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED;
    }

    ret = queryResultSet->GoToFirstRow();
    if (ret != NativeRdb::E_OK) {
        NETMGR_LOG_E("Failed to go to first row, ret: %{public}d", ret);
        queryResultSet->Close();
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED;
    }

    int32_t idx = 0;
    queryResultSet->GetString(idx++, info.iccid);
    queryResultSet->GetInt(idx++, info.simId);
    queryResultSet->GetInt(idx++, info.displayTrafficSwitch);
    queryResultSet->GetInt(idx++, info.unlimitTrafficSwitch);
    int64_t trafficLimitTemp = 0;
    queryResultSet->GetLong(idx++, trafficLimitTemp);
    info.trafficLimit = static_cast<uint64_t>(trafficLimitTemp);
    queryResultSet->GetInt(idx++, info.startDate);
    queryResultSet->GetInt(idx++, info.overLimitBehavior);
    queryResultSet->GetInt(idx++, info.monthlyLimitPercentage);
    queryResultSet->GetInt(idx++, info.dailyLimitPercentage);

    queryResultSet->Close();
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsRDB::QueryAllTrafficPlanInfo(std::vector<TrafficPlanInfo> &infoList)
{
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Failed to get RDB store");
        return ret;
    }

    NativeRdb::RdbPredicates rdbPredicate(TRAFFIC_PLAN_TABLE);
    std::vector<std::string> whereArgs;
    
    auto queryResultSet = rdbStore_->Query(rdbPredicate, whereArgs);
    if (queryResultSet == nullptr) {
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED + 1;
    }

    int32_t rowCount = 0;
    ret = queryResultSet->GetRowCount(rowCount);
    if (ret != NativeRdb::E_OK) {
        NETMGR_LOG_E("Failed to get row count, ret: %{public}d", ret);
        queryResultSet->Close();
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED;
    }
    
    if (rowCount == 0) {
        NETMGR_LOG_D("QueryAllTrafficPlanInfo: no records found");
        queryResultSet->Close();
        return NETMANAGER_SUCCESS;
    }

    ret = queryResultSet->GoToFirstRow();
    if (ret != NativeRdb::E_OK) {
        NETMGR_LOG_E("Failed to go to first row, ret: %{public}d", ret);
        queryResultSet->Close();
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED;
    }

    do {
        TrafficPlanInfo info;
        int32_t idx = 0;
        queryResultSet->GetString(idx++, info.iccid);
        queryResultSet->GetInt(idx++, info.simId);
        queryResultSet->GetInt(idx++, info.displayTrafficSwitch);
        queryResultSet->GetInt(idx++, info.unlimitTrafficSwitch);
        int64_t trafficLimitTemp = 0;
        queryResultSet->GetLong(idx++, trafficLimitTemp);
        info.trafficLimit = static_cast<uint64_t>(trafficLimitTemp);
        queryResultSet->GetInt(idx++, info.startDate);
        queryResultSet->GetInt(idx++, info.overLimitBehavior);
        queryResultSet->GetInt(idx++, info.monthlyLimitPercentage);
        queryResultSet->GetInt(idx++, info.dailyLimitPercentage);
        
        infoList.push_back(info);
    } while (queryResultSet->GoToNextRow() == NativeRdb::E_OK);

    queryResultSet->Close();
    
    NETMGR_LOG_I("QueryAllTrafficPlanInfo success, found %{public}zu records", infoList.size());
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsRDB::UpdateTrafficPlanParam(const std::string &iccid, TrafficPlanParam param, int64_t value)
{
    NETMGR_LOG_I("UpdateTrafficPlanParam start, param: %{public}d, value: %{public}" PRId64,
                 static_cast<int32_t>(param), value);
    if (iccid.empty()) {
        return TRAFFIC_PLAN_ERR_INVALID_PARAM;
    }
    std::string fieldName = GetFieldNameByParam(param);
    if (fieldName.empty()) {
        NETMGR_LOG_E("Invalid traffic plan param: %{public}d", static_cast<int32_t>(param));
        return TRAFFIC_PLAN_ERR_INVALID_PARAM;
    }
    
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Failed to get RDB store");
        return ret;
    }

    NativeRdb::RdbPredicates rdbPredicate(TRAFFIC_PLAN_TABLE);
    rdbPredicate.EqualTo(FILED_ICCID, iccid);

    NativeRdb::ValuesBucket values;
    values.PutLong(fieldName, value);

    int32_t rows;
    ret = rdbStore_->Update(rows, values, rdbPredicate);
    if (ret != NativeRdb::E_OK) {
        NETMGR_LOG_E("Update operation failed. Result %{public}d", ret);
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED;
    }

    NETMGR_LOG_I("UpdateTrafficPlanParam success, updated %{public}d rows", rows);
    return NETMANAGER_SUCCESS;
}

std::string NetStatsRDB::GetFieldNameByParam(TrafficPlanParam param)
{
    switch (param) {
        case TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH:
            return FILED_DISPLAY_TRAFFIC_SWITCH;
        case TrafficPlanParam::UNLIMIT_TRAFFIC_SWITCH:
            return FILED_UNLIMIT_TRAFFIC_SWITCH;
        case TrafficPlanParam::TRAFFIC_LIMIT:
            return FILED_TRAFFIC_LIMIT;
        case TrafficPlanParam::START_DATE:
            return FILED_START_DATE;
        case TrafficPlanParam::OVER_LIMIT_BEHAVIOR:
            return FILED_OVER_LIMIT_BEHAVIOR;
        case TrafficPlanParam::MONTHLY_LIMIT_PERCENTAGE:
            return FILED_MONTHLY_LIMIT_PERCENTAGE;
        case TrafficPlanParam::DAILY_LIMIT_PERCENTAGE:
            return FILED_DAILY_LIMIT_PERCENTAGE;
        default:
            return "";
    }
}

// 根据iccid更新一整条数据
int32_t NetStatsRDB::UpdatePlanInfoByIccid(const TrafficPlanInfo &info)
{
    NETMGR_LOG_I("UpdatePlanInfoByIccid start, info:%{public}s", info.ToString().c_str());
    if (info.iccid.empty()) {
        return TRAFFIC_PLAN_ERR_INVALID_PARAM;
    }
    
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Failed to get RDB store");
        return ret;
    }

    NativeRdb::RdbPredicates rdbPredicate(TRAFFIC_PLAN_TABLE);
    rdbPredicate.EqualTo(FILED_ICCID, info.iccid);

    NativeRdb::ValuesBucket values;
    values.PutString(FILED_ICCID, info.iccid);
    values.PutInt(FILED_SIMID, info.simId);
    values.PutInt(FILED_DISPLAY_TRAFFIC_SWITCH, info.displayTrafficSwitch);
    values.PutInt(FILED_UNLIMIT_TRAFFIC_SWITCH, info.unlimitTrafficSwitch);
    values.PutLong(FILED_TRAFFIC_LIMIT, info.trafficLimit);
    values.PutInt(FILED_START_DATE, info.startDate);
    values.PutInt(FILED_OVER_LIMIT_BEHAVIOR, info.overLimitBehavior);
    values.PutInt(FILED_MONTHLY_LIMIT_PERCENTAGE, info.monthlyLimitPercentage);
    values.PutInt(FILED_DAILY_LIMIT_PERCENTAGE, info.dailyLimitPercentage);

    int32_t rows;
    ret = rdbStore_->Update(rows, values, rdbPredicate);
    if (ret != NativeRdb::E_OK) {
        NETMGR_LOG_E("Update operation failed. Result %{public}d", ret);
        return TRAFFIC_PLAN_ERR_DATABASE_FAILED;
    }

    NETMGR_LOG_I("UpdatePlanInfoByIccid success, updated %{public}d rows", rows);
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS

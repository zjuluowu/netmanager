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

#include "net_access_policy_rdb.h"
#include <unistd.h>

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "rdb_errno.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
[[maybe_unused]] const int32_t RDB_VERSION_0 = 0;
const int32_t RDB_VERSION_1 = 1;
const int32_t RDB_VERSION_2 = 2;
const std::string NETMANAGER_DB_UID_ACCESS_POLICY_TABLE = "uid_access_policy_infos";
const std::string SQL_TABLE_COLUMS = std::string(
    "uid INTEGER NOT NULL PRIMARY KEY, "
    "wifiPolicy INTEGER NOT NULL, cellularPolicy INTEGER NOT NULL, setFromConfigFlag INTEGER NOT NULL, isBroker "
    "INTEGER NOT NULL");
} // namespace

int NetAccessPolicyRDB::RdbDataOpenCallback::OnCreate(NativeRdb::RdbStore &rdbStore)
{
    return NETMANAGER_SUCCESS;
}

int NetAccessPolicyRDB::RdbDataOpenCallback::OnUpgrade(NativeRdb::RdbStore &store, int oldVersion, int newVersion)
{
    NETMGR_LOG_I("OnUpgrade, oldVersion: %{public}d, newVersion: %{public}d", oldVersion, newVersion);
    while (oldVersion < newVersion) {
        UpgradeDbVersionTo(store, ++oldVersion);
    }
    return NETMANAGER_SUCCESS;
}

void NetAccessPolicyRDB::RdbDataOpenCallback::UpgradeDbVersionTo(NativeRdb::RdbStore &store, int newVersion)
{
    switch (newVersion) {
        case RDB_VERSION_1:
        // When upgrading the rdb version to 1, the is_broker field was added, but some users failed the upgrade.
        case RDB_VERSION_2:
            AddIsBroker(store, newVersion);
            break;
        default:
            NETMGR_LOG_E("no such newVersion: %{public}d", newVersion);
    }
}

void NetAccessPolicyRDB::RdbDataOpenCallback::AddIsBroker(NativeRdb::RdbStore &store, int newVersion)
{
    std::string NewVersionModify = "ALTER TABLE uid_access_policy_infos ADD COLUMN isBroker INTEGER";
    int ret = store.ExecuteSql(NewVersionModify);
    if (ret != 0) {
        NETMGR_LOG_E("OnUpgrade failed, ret: %{public}d, newVersion: %{public}d", ret, newVersion);
    }
}

int32_t NetAccessPolicyRDB::GetRdbStore()
{
    if (rdbStore_ != nullptr) {
        return NETMANAGER_SUCCESS;
    }

    int errCode = NETMANAGER_SUCCESS;
    NativeRdb::RdbStoreConfig config(DATABASE_NAME);
    NetAccessPolicyRDB::RdbDataOpenCallback helper;
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_2, helper, errCode);
    if (rdbStore_ != nullptr) {
        return NETMANAGER_SUCCESS;
    }
    NETMGR_LOG_E("RDB create failed, errCode: %{public}d", errCode);
    if (errCode == NativeRdb::E_SQLITE_CORRUPT) {
        int ret = NativeRdb::RdbHelper::DeleteRdbStore(config);
        NETMGR_LOG_E("RDB DeleteRdbStore ret: %{public}d", ret);
        rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_2, helper, errCode);
        if (rdbStore_ != nullptr) {
            ret = rdbStore_->Restore(DATABASE_BACK_NAME);
            NETMGR_LOG_E("RDB Restore ret: %{public}d", ret);
            if (ret == 0) {
                return NETMANAGER_SUCCESS;
            }
        }
    }
    return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t NetAccessPolicyRDB::GetBackUpRdbStore()
{
    if (backupRdbStore_ != nullptr) {
        return NETMANAGER_SUCCESS;
    }

    int errCode = NETMANAGER_SUCCESS;
    NativeRdb::RdbStoreConfig config(DATABASE_BACK_NAME);
    NetAccessPolicyRDB::RdbDataOpenCallback helper;
    backupRdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_2, helper, errCode);
    if (backupRdbStore_ != nullptr) {
        return NETMANAGER_SUCCESS;
    }
    NETMGR_LOG_E("backup RDB create failed, errCode: %{public}d", errCode);
    if (errCode == NativeRdb::E_SQLITE_CORRUPT) {
        int ret = NativeRdb::RdbHelper::DeleteRdbStore(config);
        NETMGR_LOG_E("RDB DeleteRdbStore ret: %{public}d", ret);
        backupRdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_2, helper, errCode);
        if (backupRdbStore_ != nullptr) {
            return NETMANAGER_SUCCESS;
        }
    }
    return NETMANAGER_ERROR;
}

int32_t NetAccessPolicyRDB::InitRdbStore()
{
    InitRdbStoreBackupDB();
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    std::string createTable =
        CREATE_TABLE_IF_NOT_EXISTS + NETMANAGER_DB_UID_ACCESS_POLICY_TABLE + " (" + SQL_TABLE_COLUMS + ")";
    rdbStore_->ExecuteSql(createTable);

    NETMGR_LOG_D("InitRdbStore");
    return NETMANAGER_SUCCESS;
}

int32_t NetAccessPolicyRDB::InitRdbStoreBackupDB()
{
    NETMGR_LOG_I("InitRdbStoreBackupDB start");
    int32_t ret = GetBackUpRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    std::string createTable =
        CREATE_TABLE_IF_NOT_EXISTS + NETMANAGER_DB_UID_ACCESS_POLICY_TABLE + " (" + SQL_TABLE_COLUMS + ")";
    backupRdbStore_->ExecuteSql(createTable);

    NETMGR_LOG_I("InitRdbStoreBackupDB end");
    return NETMANAGER_SUCCESS;
}

int32_t NetAccessPolicyRDB::InsertData(NetAccessPolicyData policy)
{
    NETMGR_LOG_D("InsertData");
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return NETMANAGER_ERROR;
    }

    NativeRdb::ValuesBucket policyValues;
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_UID, policy.uid);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_WIFI_POLICY, policy.wifiPolicy);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_CELLULAR_POLICY, policy.cellularPolicy);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_SET_FROM_CONFIG_FLAG, policy.setFromConfigFlag);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_IS_BROKER, 0);

    int64_t id = 0;
    ret = rdbStore_->Insert(id, NETMANAGER_DB_UID_ACCESS_POLICY_TABLE, policyValues);
    if (ret != NativeRdb::E_OK) {
        ret = UpdateByUid(policy.uid, policy);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("Update operation failed, result is %{public}d", ret);
            return NETMANAGER_ERROR;
        }
    }
    InsertDataToBackDB(policy);
    return NETMANAGER_SUCCESS;
}

int32_t NetAccessPolicyRDB::InsertDataToBackDB(NetAccessPolicyData policy)
{
    NETMGR_LOG_D("InsertDataToBackDB");
    int32_t ret = GetBackUpRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    int64_t id = 0;
    NativeRdb::ValuesBucket policyValues;
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_UID, policy.uid);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_WIFI_POLICY, policy.wifiPolicy);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_CELLULAR_POLICY, policy.cellularPolicy);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_SET_FROM_CONFIG_FLAG, policy.setFromConfigFlag);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_IS_BROKER, 0);
    int insertRet = backupRdbStore_->Insert(id, NETMANAGER_DB_UID_ACCESS_POLICY_TABLE, policyValues);
    if (ret != NativeRdb::E_OK) {
        ret = UpdateByUidToBackDB(policy.uid, policy);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("Update operation failed, result is %{public}d", ret);
            return NETMANAGER_ERROR;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetAccessPolicyRDB::BackUpPolicyDB(const std::string &sourceDB, const std::string &targetDB)
{
    NETMGR_LOG_I("rdbStore BackUpPolicyDB start");
    if (sourceDB.empty() || targetDB.empty()) {
        NETMGR_LOG_E("sourceDB or targetDB is empty");
        return NETMANAGER_ERROR;
    }
    int errCode = NETMANAGER_SUCCESS;
    NativeRdb::RdbStoreConfig config(targetDB);
    NetAccessPolicyRDB::RdbDataOpenCallback helper;
    auto rdbStore = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION_2, helper, errCode);
    if (rdbStore == nullptr) {
        NETMGR_LOG_E("RDB GetRdbStore failed, errCode: %{public}d", errCode);
        return NETMANAGER_ERROR;
    }
    int ret = rdbStore->Restore(sourceDB);
    NETMGR_LOG_I("rdbStore BackUpPolicyDB Restore ret: %{public}d", ret);
    return NETMANAGER_SUCCESS;
}

int32_t NetAccessPolicyRDB::DeleteByUid(const int32_t uid)
{
    NETMGR_LOG_I("DeleteByUid");
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return NETMANAGER_ERROR;
    }

    int32_t deletedRows = -1;
    std::vector<std::string> whereArgs;
    OHOS::NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_UID_ACCESS_POLICY_TABLE};
    rdbPredicate.EqualTo(NetAccessPolicyRdbFiledConst::FILED_UID, std::to_string(uid));
    int32_t result = rdbStore_->Delete(deletedRows, rdbPredicate);
    if (result != NativeRdb::E_OK) {
        NETMGR_LOG_E("delete operation failed, result is %{public}d", result);
        return result;
    }
    DeleteByUidToBackDB(uid);
    return deletedRows;
}

int32_t NetAccessPolicyRDB::DeleteByUidToBackDB(const int32_t uid)
{
    NETMGR_LOG_I("DeleteByUidToBackDB");
    int32_t ret = GetBackUpRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    int32_t deletedRows = -1;
    std::vector<std::string> whereArgs;
    OHOS::NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_UID_ACCESS_POLICY_TABLE};
    rdbPredicate.EqualTo(NetAccessPolicyRdbFiledConst::FILED_UID, std::to_string(uid));
    int32_t result = backupRdbStore_->Delete(deletedRows, rdbPredicate);
    if (result != NativeRdb::E_OK) {
        NETMGR_LOG_E("delete operation failed, result is %{public}d", result);
        return result;
    }

    return deletedRows;
}

int32_t NetAccessPolicyRDB::UpdateByUid(int32_t uid, NetAccessPolicyData policy)
{
    NETMGR_LOG_D("UpdateByUid");
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return NETMANAGER_ERROR;
    }

    OHOS::NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_UID_ACCESS_POLICY_TABLE};
    rdbPredicate.EqualTo(NetAccessPolicyRdbFiledConst::FILED_UID, std::to_string(uid));

    NativeRdb::ValuesBucket policyValues;
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_UID, uid);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_WIFI_POLICY, policy.wifiPolicy);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_CELLULAR_POLICY, policy.cellularPolicy);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_SET_FROM_CONFIG_FLAG, policy.setFromConfigFlag);

    int32_t rowId = -1;
    int32_t result = rdbStore_->Update(rowId, policyValues, rdbPredicate);
    if (result != NativeRdb::E_OK) {
        NETMGR_LOG_E("Update operation failed. Result %{public}d", result);
        return result;
    }
    UpdateByUidToBackDB(uid, policy);
    return NETMANAGER_SUCCESS;
}

int32_t NetAccessPolicyRDB::UpdateByUidToBackDB(int32_t uid, NetAccessPolicyData policy)
{
    NETMGR_LOG_D("UpdateByUidToBackDB");
    int32_t ret = GetBackUpRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    OHOS::NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_UID_ACCESS_POLICY_TABLE};
    rdbPredicate.EqualTo(NetAccessPolicyRdbFiledConst::FILED_UID, std::to_string(uid));

    NativeRdb::ValuesBucket policyValues;
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_UID, uid);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_WIFI_POLICY, policy.wifiPolicy);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_CELLULAR_POLICY, policy.cellularPolicy);
    policyValues.PutInt(NetAccessPolicyRdbFiledConst::FILED_SET_FROM_CONFIG_FLAG, policy.setFromConfigFlag);

    int32_t rowId = -1;
    int32_t result = backupRdbStore_->Update(rowId, policyValues, rdbPredicate);
    if (result != NativeRdb::E_OK) {
        NETMGR_LOG_E("Update operation failed. Result %{public}d", result);
        return result;
    }

    return NETMANAGER_SUCCESS;
}

std::vector<NetAccessPolicyData> NetAccessPolicyRDB::QueryAll()
{
    NETMGR_LOG_D("QueryAll");
    std::vector<NetAccessPolicyData> result;
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return result;
    }

    OHOS::NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_UID_ACCESS_POLICY_TABLE};
    std::vector<std::string> whereArgs;
    auto queryResultSet = rdbStore_->Query(rdbPredicate, whereArgs);
    if (queryResultSet == nullptr) {
        return result;
    }

    bool isAtLastRow = false;
    queryResultSet->IsAtLastRow(isAtLastRow);
    while (!queryResultSet->GoToNextRow()) {
        NetAccessPolicyData policy;
        queryResultSet->GetInt(NetAccessPolicyRdbFiledConst::FILED_COLUMN_INDEX_ZERO, policy.uid);
        queryResultSet->GetInt(NetAccessPolicyRdbFiledConst::FILED_COLUMN_INDEX_ONE, policy.wifiPolicy);
        queryResultSet->GetInt(NetAccessPolicyRdbFiledConst::FILED_COLUMN_INDEX_TWO, policy.cellularPolicy);
        queryResultSet->GetInt(NetAccessPolicyRdbFiledConst::FILED_COLUMN_INDEX_THREE, policy.setFromConfigFlag);
        result.emplace_back(policy);
    }
    queryResultSet->Close();
    return result;
}

int32_t NetAccessPolicyRDB::QueryByUid(int uid, NetAccessPolicyData& uidPolicy)
{
    NETMGR_LOG_D("QueryByUid uid:%{public}d", uid);
    int32_t ret = GetRdbStore();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("error: rdbStore_ is nullptr");
        return NETMANAGER_ERROR;
    }

    OHOS::NativeRdb::RdbPredicates rdbPredicate{NETMANAGER_DB_UID_ACCESS_POLICY_TABLE};
    rdbPredicate.EqualTo(NetAccessPolicyRdbFiledConst::FILED_UID, uid);

    std::vector<std::string> whereArgs;
    auto queryResultSet = rdbStore_->Query(rdbPredicate, whereArgs);
    if (queryResultSet == nullptr) {
        NETMGR_LOG_E("QueryByUid error");
        return NETMANAGER_ERROR;
    }

    int32_t rowCount = 0;
    ret = queryResultSet->GetRowCount(rowCount);
    if (ret != OHOS::NativeRdb::E_OK) {
        NETMGR_LOG_E("query setting failed, get row count failed, name:%{public}d, ret:%{public}d", uid, ret);
        queryResultSet->Close();
        return ret;
    }
    if (rowCount == 0) {
        NETMGR_LOG_D("query setting name:%{public}d, num is 0", uid);
        queryResultSet->Close();
        return NETMANAGER_ERROR;
    }

    while (!queryResultSet->GoToNextRow()) {
        queryResultSet->GetInt(NetAccessPolicyRdbFiledConst::FILED_COLUMN_INDEX_ZERO, uidPolicy.uid);
        queryResultSet->GetInt(NetAccessPolicyRdbFiledConst::FILED_COLUMN_INDEX_ONE, uidPolicy.wifiPolicy);
        queryResultSet->GetInt(NetAccessPolicyRdbFiledConst::FILED_COLUMN_INDEX_TWO, uidPolicy.cellularPolicy);
        queryResultSet->GetInt(NetAccessPolicyRdbFiledConst::FILED_COLUMN_INDEX_THREE, uidPolicy.setFromConfigFlag);
        if (uidPolicy.uid == uid) {
            queryResultSet->Close();
            return NETMANAGER_SUCCESS;
        }
    }

    queryResultSet->Close();
    return NETMANAGER_ERROR;
}

} // namespace NetManagerStandard
} // namespace OHOS

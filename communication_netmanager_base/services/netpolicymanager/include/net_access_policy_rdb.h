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

#ifndef NET_ACCESS_POLICY_RDB_H
#define NET_ACCESS_POLICY_RDB_H

#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store.h"
#include "result_set.h"
#include "rdb_sql_utils.h"
#include "net_access_policy.h"
#include "rdb_predicates.h"

namespace OHOS {
namespace NetManagerStandard {
namespace NetAccessPolicyRdbFiledConst {
    const std::string FILED_UID = "uid";
    const std::string FILED_WIFI_POLICY = "wifiPolicy";
    const std::string FILED_CELLULAR_POLICY = "cellularPolicy";
    const std::string FILED_SET_FROM_CONFIG_FLAG = "setFromConfigFlag";
    const std::string FILED_IS_BROKER = "isBroker";

    constexpr int32_t FILED_COLUMN_INDEX_ZERO = 0;
    constexpr int32_t FILED_COLUMN_INDEX_ONE = 1;
    constexpr int32_t FILED_COLUMN_INDEX_TWO = 2;
    constexpr int32_t FILED_COLUMN_INDEX_THREE = 3;
}

constexpr const char* DATABASE_NAME = "/data/service/el1/public/netmanager/net_uid_access_policy.db";
constexpr const char* DATABASE_BACK_NAME = "/data/service/el1/public/netmanager/net_uid_access_policy_back.db";
constexpr const char* POLICY_DATABASE_BACKUP_FILE =
    "/data/service/el1/public/netmanager/net_uid_access_policy_backup.txt";

typedef struct NetAccessPolicyData {
    int32_t uid;
    int32_t wifiPolicy;
    int32_t cellularPolicy;
    int32_t setFromConfigFlag;
}NetAccessPolicyData;

class NetAccessPolicyRDB {
public:
    static NetAccessPolicyRDB& GetInstance();
    NetAccessPolicyRDB() = default;
    ~NetAccessPolicyRDB() = default;
    int32_t InitRdbStore();
    int32_t GetRdbStore();
    int32_t GetBackUpRdbStore();
    int32_t InsertData(NetAccessPolicyData policy);
    int32_t DeleteByUid(const int32_t uid);
    int32_t UpdateByUid(int32_t uid, NetAccessPolicyData policy);
    int32_t BackUpPolicyDB(const std::string &sourceDB, const std::string &targetDB);
    int32_t InitRdbStoreBackupDB();
    int32_t InsertDataToBackDB(NetAccessPolicyData policy);
    int32_t DeleteByUidToBackDB(const int32_t uid);
    int32_t UpdateByUidToBackDB(int32_t uid, NetAccessPolicyData policy);

    bool IsRdbNull()
    {
        return rdbStore_ == nullptr;
    }
    std::vector<NetAccessPolicyData> QueryAll();
    int32_t QueryByUid(int uid, NetAccessPolicyData &uidPolicy);

    class RdbDataOpenCallback : public NativeRdb::RdbOpenCallback {
    public:
        int32_t OnCreate(NativeRdb::RdbStore &rdbStore) override;
        int32_t OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t oldVersion, int32_t newVersion) override;
    private:
        void UpgradeDbVersionTo(NativeRdb::RdbStore &store, int newVersion);
        void AddIsBroker(NativeRdb::RdbStore &store, int newVersion);
    };
    friend class RdbDataOpenCallback;

private:
    std::shared_ptr<NativeRdb::RdbStore> rdbStore_{nullptr};
    std::shared_ptr<NativeRdb::RdbStore> backupRdbStore_{nullptr};
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_ACCESS_POLICY_RDB_H

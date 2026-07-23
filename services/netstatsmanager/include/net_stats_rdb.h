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

#ifndef NET_STATS_RDB_H
#define NET_STATS_RDB_H

#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store.h"
#include "result_set.h"
#include "rdb_sql_utils.h"
#include "traffic_plan_param.h"
#include "rdb_predicates.h"

namespace OHOS {
namespace NetManagerStandard {
namespace NetStatsRdbFiledConst {
    const std::string FILED_SIMID = "simId";
    const std::string FILED_MON_W = "monthWarningdate";
    const std::string FILED_DAY_N = "dayNontificationdate";
    const std::string FILED_MON_N = "monthNontificationdate";
    const std::string FILED_MON_W_S = "monthWarningState";
    const std::string FILED_DAY_N_S = "dayNontificationState";
    const std::string FILED_MON_N_S = "monNontificationState";
    
    constexpr int32_t FILED_COLUMN_INDEX_ZERO = 0;
    constexpr int32_t FILED_COLUMN_INDEX_ONE = 1;
    constexpr int32_t FILED_COLUMN_INDEX_TWO = 2;
    constexpr int32_t FILED_COLUMN_INDEX_THR = 3;
    constexpr int32_t FILED_COLUMN_INDEX_FUR = 4;
    constexpr int32_t FILED_COLUMN_INDEX_FW = 5;
    constexpr int32_t FILED_COLUMN_INDEX_SIX = 6;
}

constexpr const char* NOTICE_DATABASE_NAME = "/data/service/el1/public/netmanager/net_stats_notice_freq.db";
constexpr const char* NOTICE_DATABASE_BACK_NAME = "/data/service/el1/public/netmanager/net_stats_notice_freq_back.db";

typedef struct NetStatsData {
    int32_t simId;
    int monWarningDate;
    int dayNoticeDate;
    int monNoticeDate;
    int monWarningState;
    int dayNoticeState;
    int monNoticeState;
} NetStatsData;

class NetStatsRDB {
public:
    NetStatsRDB() = default;
    ~NetStatsRDB() = default;
    int32_t InitRdbStore();
    int32_t GetRdbStore();
    int32_t InsertData(NetStatsData state);
    int32_t DeleteBySimId(int32_t simId);
    int32_t UpdateBySimId(int32_t simId, NetStatsData state);

    std::vector<NetStatsData> QueryAll();
    int32_t QueryBySimId(int simId, NetStatsData& simStats);
    int32_t BackUpNetStatsFreqDB(const std::string &sourceDB, const std::string &targetDB);
    int32_t InitRdbStoreBackupDB();

    NativeRdb::ValuesBucket BuildValuesBucket(const TrafficPlanInfo &info);
    
    int32_t InsertTrafficPlanInfo(const NativeRdb::ValuesBucket &values);

    std::string GetFieldNameByParam(TrafficPlanParam param);

    int32_t InsertOrUpdateTrafficPlanInfo(const TrafficPlanInfo &info);

    int32_t QueryTrafficPlanInfoByIccid(const std::string &iccid, TrafficPlanInfo &info);
    int32_t QueryAllTrafficPlanInfo(std::vector<TrafficPlanInfo> &infoList);
    int32_t UpdateTrafficPlanParam(const std::string &iccid, TrafficPlanParam param, int64_t value);
    int32_t UpdatePlanInfoByIccid(const TrafficPlanInfo &info);

    std::shared_ptr<NativeRdb::RdbStore> GetRdbStorePtr();
    class RdbDataOpenCallback : public NativeRdb::RdbOpenCallback {
    public:
        int32_t OnCreate(NativeRdb::RdbStore &rdbStore) override;
        int32_t OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t oldVersion, int32_t newVersion) override;
    private:
        void UpgradeDbVersionTo(NativeRdb::RdbStore &store, int newVersion);
    };
    friend class RdbDataOpenCallback;

private:
    std::shared_ptr<NativeRdb::RdbStore> rdbStore_{nullptr};
    std::mutex mutex_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_ACCESS_POLICY_RDB_H

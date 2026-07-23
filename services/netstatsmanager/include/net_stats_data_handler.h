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

#ifndef NET_STATS_DATA_HANDLER_H
#define NET_STATS_DATA_HANDLER_H

#include "net_stats_info.h"
#include "net_stats_database_defines.h"

namespace OHOS {
namespace NetManagerStandard {

class NetStatsDataHandler {
public:
    NetStatsDataHandler();
    ~NetStatsDataHandler() = default;
    int32_t WriteStatsData(const std::vector<NetStatsInfo> &infos, const std::string &tableName);
    int32_t WriteIptablesData(const NetStatsInfo &infos, const std::string &tableName);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, uint64_t start, uint64_t end);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, uint64_t uid, uint64_t start, uint64_t end);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, const std::string &iface, uint64_t start, uint64_t end);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, const std::string &iface, const uint32_t uid,
                          uint64_t start, uint64_t end);
    int32_t ReadStatsDataByIdent(std::vector<NetStatsInfo> &infos, const std::string &ident, uint64_t start,
                                 uint64_t end);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, uint32_t uid, const std::string &ident, uint64_t start,
                          uint64_t end);
    int32_t ReadStatsDataByIdentAndUserId(std::vector<NetStatsInfo> &infos, const std::string &ident,
                                          const int32_t userId, uint64_t start, uint64_t end);
    int32_t ReadIfaceTableHistoryByIdent(std::vector<NetStatsInfo> &recv, const std::string &ident,
                                         uint64_t start, uint64_t end);
    int32_t ReadIfaceStatsByIdent(std::vector<NetStatsInfo> &recv, const std::string &ident,
                                                   uint64_t start, uint64_t end);
    int32_t DeleteByUid(uint64_t uid);
    int32_t DeleteSimStatsByUid(uint64_t uid);
    int32_t DeleteByDate(const std::string &tableName, uint64_t start, uint64_t end);
    int32_t UpdateStatsFlag(uint32_t uid, uint32_t flag);
    int32_t UpdateSimStatsFlag(uint32_t uid, uint32_t flag);
    int32_t UpdateStatsFlagByUserId(int32_t userId, uint32_t flag);
    int32_t UpdateSimStatsFlagByUserId(int32_t userId, uint32_t flag);
    int32_t UpdateStatsUserIdByUserId(int32_t userId, int32_t newUserId);
    int32_t UpdateSimStatsUserIdByUserId(int32_t userId, int32_t newUserId);
    int32_t UpdateSimDataFlag(uint32_t oldFlag, uint32_t newFlag);
    int32_t ClearData();
    int32_t BackupNetStatsData(const std::string &sourceDb, const std::string &backupDb);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    int32_t WriteCalibrationTrafficInfo(uint32_t simId, uint32_t startTime, uint32_t endTime, uint64_t usedTraffic);
    int32_t ReadCalibrationTrafficInfo(uint32_t simId, uint32_t &startTime, uint32_t &endTime, uint64_t &usedTraffic);
    int32_t DeleteCalibrationTrafficInfo(uint32_t simId);
    int32_t WriteChangeToIfaceTime(uint32_t startTime);
    int32_t ReadChangeToIfaceTime(uint32_t &startTime);
#endif

private:
    bool isDisplayTrafficAncoList = false;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_DATA_HANDLER_H

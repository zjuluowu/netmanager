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

#include "net_stats_data_handler.h"

#include "net_mgr_log_wrapper.h"
#include "net_stats_database_defines.h"
#include "net_stats_database_helper.h"
#include "net_stats_constants.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;

NetStatsDataHandler::NetStatsDataHandler()
{
    isDisplayTrafficAncoList = CommonUtils::IsNeedDisplayTrafficAncoList();
}

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->SelectData(infos, UID_TABLE, start, end);
}

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, uint64_t uid, uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->SelectData(uid, start, end, infos);
}

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, const std::string &iface, uint64_t start,
                                           uint64_t end)
{
    if (iface.empty()) {
        NETMGR_LOG_E("Param is invalid");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->SelectData(iface, start, end, infos);
}

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, const std::string &iface,
                                           const uint32_t uid, uint64_t start, uint64_t end)
{
    if (iface.empty()) {
        NETMGR_LOG_E("Param is invalid");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->SelectData(iface, uid, start, end, infos);
}

int32_t NetStatsDataHandler::ReadStatsDataByIdent(std::vector<NetStatsInfo> &infos, const std::string &ident,
                                                  uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret1;
    int32_t ret2;
    std::vector<NetStatsInfo> uidSimTableInfos;
    ret1 = helper->QueryData(UID_TABLE, ident, start, end, infos);
    ret2 = helper->QueryData(UID_SIM_TABLE, ident, start, end, uidSimTableInfos);
    uidSimTableInfos.erase(std::remove_if(uidSimTableInfos.begin(), uidSimTableInfos.end(), [](const auto &item) {
                               return item.flag_ <= STATS_DATA_FLAG_DEFAULT || item.flag_ >= STATS_DATA_FLAG_LIMIT;
                           }),
                           uidSimTableInfos.end());
    std::for_each(uidSimTableInfos.begin(), uidSimTableInfos.end(), [this](NetStatsInfo &info) {
        if (!isDisplayTrafficAncoList) {
            if (info.flag_ == STATS_DATA_FLAG_SIM2) {
                info.uid_ = SIM2_UID;
            } else if (info.flag_ == STATS_DATA_FLAG_SIM) {
                info.uid_ = Sim_UID;
            }
        } else {
            if (info.flag_ == STATS_DATA_FLAG_SIM_BASIC) {
                info.uid_ = Sim_UID;
            } else if (info.flag_ == STATS_DATA_FLAG_SIM2_BASIC) {
                info.uid_ = SIM2_UID;
            }
        }
    });
    if (ret1 != NETMANAGER_SUCCESS || ret2 != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("QueryData wrong, ret1=%{public}d, ret2=%{public}d", ret1, ret2);
        return ret1 != NETMANAGER_SUCCESS ? ret1 : ret2;
    }
    infos.insert(infos.end(), uidSimTableInfos.begin(), uidSimTableInfos.end());
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDataHandler::ReadIfaceTableHistoryByIdent(std::vector<NetStatsInfo> &recv, const std::string &ident,
                                                          uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    // LCOV_EXCL_START
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret1;
    int32_t ret2;
    int32_t ret = helper->QueryData(IFACE_TABLE, ident, start, end, recv);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("QueryData wrong, ret=%{public}d", ret);
        return ret;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, uint32_t uid, const std::string &ident,
                                           uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret1;
    int32_t ret2;
    std::vector<NetStatsInfo> uidSimTableInfos;
    ret1 = helper->QueryData(UID_TABLE, uid, ident, start, end, infos);
    if (uid == Sim_UID || uid == SIM2_UID) {
        ret2 = helper->QueryData(UID_SIM_TABLE, ident, start, end, uidSimTableInfos);
        uidSimTableInfos.erase(std::remove_if(uidSimTableInfos.begin(), uidSimTableInfos.end(), [](const auto &item) {
                                   return item.flag_ <= STATS_DATA_FLAG_DEFAULT || item.flag_ >= STATS_DATA_FLAG_LIMIT;
                               }),
                               uidSimTableInfos.end());
        std::for_each(uidSimTableInfos.begin(), uidSimTableInfos.end(), [this](NetStatsInfo &info) {
            if (!isDisplayTrafficAncoList) {
                if (info.flag_ == STATS_DATA_FLAG_SIM2) {
                    info.uid_ = SIM2_UID;
                } else if (info.flag_ == STATS_DATA_FLAG_SIM) {
                    info.uid_ = Sim_UID;
                }
            } else {
                if (info.flag_ == STATS_DATA_FLAG_SIM_BASIC) {
                    info.uid_ = Sim_UID;
                } else if (info.flag_ == STATS_DATA_FLAG_SIM2_BASIC) {
                    info.uid_ = SIM2_UID;
                }
            }
        });
    } else {
        ret2 = helper->QueryData(UID_SIM_TABLE, uid, ident, start, end, uidSimTableInfos);
        if (!uidSimTableInfos.empty() && isDisplayTrafficAncoList) {
            uidSimTableInfos.erase(std::remove_if(uidSimTableInfos.begin(), uidSimTableInfos.end(),
                [](const auto &item) {
                    return item.flag_ != STATS_DATA_FLAG_SIM && item.flag_ != STATS_DATA_FLAG_SIM2;
                }),
                uidSimTableInfos.end());
        }
    }
    if (ret1 != NETMANAGER_SUCCESS || ret2 != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("QueryData wrong, ret1=%{public}d, ret2=%{public}d", ret1, ret2);
        return ret1 != NETMANAGER_SUCCESS ? ret1 : ret2;
    }
    infos.insert(infos.end(), uidSimTableInfos.begin(), uidSimTableInfos.end());
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDataHandler::ReadStatsDataByIdentAndUserId(std::vector<NetStatsInfo> &infos,
    const std::string &ident, const int32_t userId, uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret1;
    int32_t ret2;
    std::vector<NetStatsInfo> uidSimTableInfos;
    ret1 = helper->QueryData(UID_TABLE, ident, userId, start, end, infos);
    ret2 = helper->QueryData(UID_SIM_TABLE, ident, userId, start, end, uidSimTableInfos);
    uidSimTableInfos.erase(std::remove_if(uidSimTableInfos.begin(), uidSimTableInfos.end(), [](const auto &item) {
                               return item.flag_ <= STATS_DATA_FLAG_DEFAULT || item.flag_ >= STATS_DATA_FLAG_LIMIT;
                           }),
                           uidSimTableInfos.end());
    std::for_each(uidSimTableInfos.begin(), uidSimTableInfos.end(), [this](NetStatsInfo &info) {
        if (!isDisplayTrafficAncoList) {
            if (info.flag_ == STATS_DATA_FLAG_SIM2) {
                info.uid_ = SIM2_UID;
            } else if (info.flag_ == STATS_DATA_FLAG_SIM) {
                info.uid_ = Sim_UID;
            }
        } else {
            if (info.flag_ == STATS_DATA_FLAG_SIM_BASIC) {
                info.uid_ = Sim_UID;
            } else if (info.flag_ == STATS_DATA_FLAG_SIM2_BASIC) {
                info.uid_ = SIM2_UID;
            }
        }
    });
    if (ret1 != NETMANAGER_SUCCESS || ret2 != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("QueryData wrong, ret1=%{public}d, ret2=%{public}d", ret1, ret2);
        return ret1 != NETMANAGER_SUCCESS ? ret1 : ret2;
    }
    infos.insert(infos.end(), uidSimTableInfos.begin(), uidSimTableInfos.end());
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDataHandler::WriteStatsData(const std::vector<NetStatsInfo> &infos, const std::string &tableName)
{
    NETMGR_LOG_I("WriteStatsData enter tableName:%{public}s", tableName.c_str());
    if (infos.empty() || tableName.empty()) {
        NETMGR_LOG_I("Param wrong, info: %{public}zu, tableName: %{public}zu", infos.size(), tableName.size());
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    if (tableName == UID_TABLE) {
        std::for_each(infos.begin(), infos.end(),
                      [&helper](const auto &info) { helper->InsertData(UID_TABLE, UID_TABLE_PARAM_LIST, info); });
        return NETMANAGER_SUCCESS;
    }
    if (tableName == IFACE_TABLE) {
        std::for_each(infos.begin(), infos.end(),
                      [&helper](const auto &info) { helper->InsertData(IFACE_TABLE, IFACE_TABLE_PARAM_LIST, info); });
        return NETMANAGER_SUCCESS;
    }
    if (tableName == UID_SIM_TABLE) {
        std::for_each(infos.begin(), infos.end(), [&helper](const auto &info) {
            helper->InsertData(UID_SIM_TABLE, UID_SIM_TABLE_PARAM_LIST, info);
        });
        return NETMANAGER_SUCCESS;
    }
    return NETMANAGER_ERR_PARAMETER_ERROR;
}

#ifdef SUPPORT_TRAFFIC_STATISTIC
int32_t NetStatsDataHandler::WriteCalibrationTrafficInfo(uint32_t simId, uint32_t startTime, uint32_t endTime,
                                                         uint64_t usedTraffic)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    // LCOV_EXCL_START
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    uint32_t startTimeTemp = 0;
    uint32_t endTimeTemp = 0;
    uint64_t usedDataTemp = 0;
    ReadCalibrationTrafficInfo(simId, startTimeTemp, endTimeTemp, usedDataTemp);
    if (startTimeTemp != 0) {
        int32_t ret = DeleteCalibrationTrafficInfo(simId);
        if (ret != NETMANAGER_SUCCESS) {
            return NETMANAGER_ERR_INTERNAL;
        }
    }
    // LCOV_EXCL_STOP
    return helper->InsertCalibrationTrafficInfo(std::to_string(simId), startTime, endTime, usedTraffic,
        CALIBRATION_TABLE, CALIBRATION_TABLE_PARAM_LIST);
}

int32_t NetStatsDataHandler::DeleteCalibrationTrafficInfo(uint32_t simId)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    // LCOV_EXCL_START
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret = helper->DeleteCalibrateData(CALIBRATION_TABLE, simId);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("DeleteCalibrateData error: %{public}d", ret);
    }
    // LCOV_EXCL_STOP
    return ret;
}

int32_t NetStatsDataHandler::ReadCalibrationTrafficInfo(uint32_t simId, uint32_t &startTime, uint32_t &endTime,
    uint64_t &usedTraffic)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    // LCOV_EXCL_START
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    // LCOV_EXCL_STOP
    return helper->QueryCalibrationTrafficInfo(CALIBRATION_TABLE, std::to_string(simId),
        startTime, endTime, usedTraffic);
}

int32_t NetStatsDataHandler::WriteChangeToIfaceTime(uint32_t startTime)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    // LCOV_EXCL_START
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    // LCOV_EXCL_STOP
    return helper->InsertChangeToIndexTime(startTime, CHANGE_TABLE, CHANGE_TABLE_PARAM_LIST);
}

int32_t NetStatsDataHandler::ReadChangeToIfaceTime(uint32_t &startTime)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    // LCOV_EXCL_START
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    // LCOV_EXCL_STOP
    return helper->QueryChangeToIfaceTime(CHANGE_TABLE, startTime);
}

int32_t NetStatsDataHandler::ReadIfaceStatsByIdent(std::vector<NetStatsInfo> &recv, const std::string &ident,
                                                   uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    // LCOV_EXCL_START
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    // LCOV_EXCL_STOP
    int32_t ret = helper->QueryIfaceStatsByIdent(CALIBRATION_TABLE, ident, start, end, recv);
    return ret;
}
#endif

int32_t NetStatsDataHandler::DeleteByUid(uint64_t uid)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->DeleteData(UID_TABLE, uid);
}

int32_t NetStatsDataHandler::DeleteSimStatsByUid(uint64_t uid)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->DeleteData(UID_SIM_TABLE, uid);
}

int32_t NetStatsDataHandler::DeleteByDate(const std::string &tableName, uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->DeleteData(tableName, start, end);
}

int32_t NetStatsDataHandler::UpdateStatsFlag(uint32_t uid, uint32_t flag)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->UpdateStatsFlag(UID_TABLE, uid, flag);
}

int32_t NetStatsDataHandler::UpdateStatsFlagByUserId(int32_t userId, uint32_t flag)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret = helper->UpdateStatsFlagByUserId(UID_TABLE, userId, flag);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("UpdateStatsFlagByUserId failed. userId:%{public}d, flag:%{public}d", userId, flag);
    }
    return ret;
}

int32_t NetStatsDataHandler::UpdateStatsUserIdByUserId(int32_t userId, int32_t newUserId)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->UpdateStatsUserIdByUserId(UID_TABLE, userId, newUserId);
}

int32_t NetStatsDataHandler::UpdateSimStatsUserIdByUserId(int32_t userId, int32_t newUserId)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->UpdateStatsUserIdByUserId(UID_TABLE, userId, newUserId);
}

int32_t NetStatsDataHandler::UpdateSimStatsFlagByUserId(int32_t userId, uint32_t flag)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret = helper->UpdateStatsFlagByUserId(UID_SIM_TABLE, userId, flag);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("UpdateSimStatsFlagByUserId failed. userId:%{public}d, flag:%{public}d", userId, flag);
    }
    return ret;
}

int32_t NetStatsDataHandler::UpdateSimStatsFlag(uint32_t uid, uint32_t flag)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->UpdateStatsFlag(UID_SIM_TABLE, uid, flag);
}

int32_t NetStatsDataHandler::UpdateSimDataFlag(uint32_t oldFlag, uint32_t newFlag)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    return helper->UpdateDataFlag(UID_SIM_TABLE, oldFlag, newFlag);
}

int32_t NetStatsDataHandler::ClearData()
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ifaceDataRet = helper->ClearData(IFACE_TABLE);
    int32_t uidDataRet = helper->ClearData(UID_TABLE);
    int32_t uidSimDataRet = helper->ClearData(UID_SIM_TABLE);
    if (ifaceDataRet != NETMANAGER_SUCCESS || uidDataRet != NETMANAGER_SUCCESS || uidSimDataRet != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsDataHandler::BackupNetStatsData(const std::string &sourceDb, const std::string &backupDb)
{
    sqlite3* backup = nullptr;
    bool ret_back = true;
    int32_t ret_file = sqlite3_open(backupDb.c_str(), &backup);
    if (ret_file != SQLITE_OK) {
        NETMGR_LOG_E("Failed to open backup database: %s", sqlite3_errstr(ret_file));
        return NETMANAGER_ERR_INTERNAL;
    }
    auto helper_back = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_BACK_PATH);
    if (backup != nullptr) {
        ret_back = helper_back->IntegrityCheck(backup);
    }
    if (!ret_back) {
        CommonUtils::DeleteFile(NET_STATS_DATABASE_BACK_PATH);
    }
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (helper == nullptr) {
        NETMGR_LOG_E("db helper instance is nullptr");
        sqlite3_close(backup);
        return NETMANAGER_ERR_INTERNAL;
    }
    bool ret = helper->BackupNetStatsDataDB(sourceDb, backupDb);
    if (!ret) {
        sqlite3_close(backup);
        return NETMANAGER_ERROR;
    }
    sqlite3_close(backup);
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS

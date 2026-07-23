/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
#include "net_stats_data_clone.h"
#include <sstream>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include "iservice_registry.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "system_ability_definition.h"
#include "net_mgr_log_wrapper.h"
#include "traffic_plan_param.h"
#include "net_stats_rdb.h"

namespace OHOS {
namespace NetManagerStandard {

int32_t NetStatsDataClone::OnBackup(UniqueFd &fd, const std::string &backupInfo)
{
    NetStatsRDB rdb;
    std::vector<TrafficPlanInfo> infoList;
    int32_t ret = rdb.QueryAllTrafficPlanInfo(infoList);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("QueryAllTrafficPlanInfo error: %{public}d", ret);
        return -1;
    }

    std::ostringstream ss;

    for (size_t i = 0; i < infoList.size(); i++) {
        ss << infoList[i].iccid << " " << infoList[i].simId << " " << infoList[i].displayTrafficSwitch <<
        " " <<infoList[i].unlimitTrafficSwitch << " " << infoList[i].trafficLimit << " " << infoList[i].startDate <<
        " " <<infoList[i].overLimitBehavior << " " << infoList[i].monthlyLimitPercentage << " " <<
        " " <<infoList[i].dailyLimitPercentage << std::endl;
        NETMGR_LOG_E("backup info : %{public}s", infoList[i].ToString().c_str());
    }

    std::string content = ss.str();
    bool writeRet = CommonUtils::WriteFile(NET_STATS_DATA_BACKUP_FILE, content);
    if (!writeRet) {
        return -1;
    }

    fd = UniqueFd(open(NET_STATS_DATA_BACKUP_FILE, O_RDONLY));
    if (fd.Get() < 0) {
        NETMGR_LOG_E("OnBackup open fail.");
        return -1;
    }
    NETMGR_LOG_I("OnBackup end. fd: %{public}d.", fd.Get());
    return 0;
}

int32_t NetStatsDataClone::OnRestore(UniqueFd &fd, const std::string &backupInfo)
{
    if (!FdClone(fd)) {
        return NETMANAGER_ERROR;
    }

    std::ifstream file;
    file.open(NET_STATS_DATA_BACKUP_FILE);
    if (!file.is_open()) {
        NETMGR_LOG_E("Failed to open backup file");
        return NETMANAGER_ERROR;
    }

    std::string line;
    std::vector<TrafficPlanInfo> infoList;
    std::lock_guard<ffrt::mutex> lock(mutex_);
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        TrafficPlanInfo info;
        if (!(iss >> info.iccid >> info.simId >> info.displayTrafficSwitch >>
            info.unlimitTrafficSwitch >> info.trafficLimit >> info.startDate >>
            info.overLimitBehavior >> info.monthlyLimitPercentage >> info.dailyLimitPercentage)) {
            NETMGR_LOG_E("istringstream error");
            continue;
        }
        infoList.push_back(info);
    }

    NetStatsRDB rdb;
    for (auto info : infoList) {
        rdb.InsertOrUpdateTrafficPlanInfo(info);
    }

    file.close();
    return NETMANAGER_SUCCESS;
}

NetStatsDataClone &NetStatsDataClone::GetInstance()
{
    static NetStatsDataClone gNetStatsDataClone;
    return gNetStatsDataClone;
}

bool NetStatsDataClone::FdClone(UniqueFd &fd)
{
    struct stat statBuf;
    if (fd.Get() < 0 || fstat(fd.Get(), &statBuf) < 0) {
        NETMGR_LOG_E("OnRestore fstat fd fail.");
        return false;
    }

    int destFd = open(NET_STATS_DATA_BACKUP_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (destFd < 0) {
        NETMGR_LOG_E("OnRestore open file fail.");
        return false;
    }
    if (sendfile(destFd, fd.Get(), nullptr, statBuf.st_size) < 0) {
        NETMGR_LOG_E("OnRestore fd sendfile(size: %{public}d) to destFd fail.",
            static_cast<int>(statBuf.st_size));
        close(destFd);
        return false;
    }
    close(destFd);
    return true;
}
}
}

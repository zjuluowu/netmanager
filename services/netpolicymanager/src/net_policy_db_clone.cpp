/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "net_policy_db_clone.h"
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
#include "net_access_policy_rdb.h"
#include "net_policy_core.h"
#include "net_mgr_log_wrapper.h"
#include "net_policy_rule.h"
#include "net_bundle_impl.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int32_t MAIN_USER_ID = 100;
constexpr int32_t NET_ACCESS_POLICY_ALLOW_VALUE = 1;
constexpr uint64_t DAY_MICROSECONDS =  24ULL * 60ULL * 60ULL * 1000ULL * 1000ULL;
sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        NETMGR_LOG_E("fail to get system ability mgr.");
        return nullptr;
    }

    auto remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        NETMGR_LOG_E("fail to get bundle manager proxy.");
        return nullptr;
    }

    sptr<AppExecFwk::IBundleMgr> iBundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (iBundleMgr == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return nullptr;
    }
    return iBundleMgr;
}
}

NetPolicyDBClone &NetPolicyDBClone::GetInstance()
{
    static NetPolicyDBClone gNetPolicyDBClone;
    return gNetPolicyDBClone;
}

int32_t NetPolicyDBClone::OnBackup(UniqueFd &fd, const std::string &backupInfo)
{
    NetAccessPolicyRDB netAccessPolicyRdb;
    std::vector<NetAccessPolicyData> result = netAccessPolicyRdb.QueryAll();
    NETMGR_LOG_I("OnBackup size: %{public}zu", result.size());

    std::string content;
    std::ostringstream ss;
    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return NETMANAGER_ERR_INTERNAL;
    }
    for (size_t i = 0; i < result.size(); i++) {
        if (result[i].wifiPolicy && result[i].cellularPolicy) {
            continue;
        }
        std::string uidBundleName;
        if (bundleMgrProxy->GetNameForUid(result[i].uid, uidBundleName) != ERR_OK) {
            NETMGR_LOG_E("GetNameForUid error. uid:%{public}d", result[i].uid);
            continue;
        }
        ss << uidBundleName << " " << result[i].wifiPolicy << " " << result[i].cellularPolicy << std::endl;
    }
    content = ss.str();
    bool writeRet = CommonUtils::WriteFile(POLICY_DATABASE_BACKUP_FILE, content);
    if (!writeRet) {
        return -1;
    }

    fd = UniqueFd(open(POLICY_DATABASE_BACKUP_FILE, O_RDONLY));
    if (fd.Get() < 0) {
        NETMGR_LOG_E("OnBackup open fail.");
        return -1;
    }
    NETMGR_LOG_I("OnBackup end. fd: %{public}d.", fd.Get());
    return 0;
}

int32_t NetPolicyDBClone::OnRestore(UniqueFd &fd, const std::string &backupInfo)
{
    if (!FdClone(fd)) {
        return NETMANAGER_ERROR;
    }

    std::ifstream file;
    file.open(POLICY_DATABASE_BACKUP_FILE);
    if (!file.is_open()) {
        NETMGR_LOG_E("Failed to open backup file");
        return NETMANAGER_ERROR;
    }

    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        return NETMANAGER_ERR_INTERNAL;
    }
    std::shared_ptr<NetPolicyRule> netPolicyRule =
        DelayedSingleton<NetPolicyCore>::GetInstance()->CreateCore<NetPolicyRule>();
    if (netPolicyRule == nullptr) {
        return NETMANAGER_ERROR;
    }

    std::string line;
    NetAccessPolicyRDB netAccessPolicyRdb;
    std::lock_guard<ffrt::mutex> lock(mutex_);
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string bundleName;
        NetAccessPolicyData policyData;
        if (!(iss >> bundleName >> policyData.wifiPolicy >> policyData.cellularPolicy)) {
            NETMGR_LOG_E("istringstream error");
            continue;
        }
        policyData.uid = bundleMgrProxy->GetUidByBundleName(bundleName, MAIN_USER_ID);
        if (policyData.uid == -1) {
            NETMGR_LOG_E("Failed to get uid from bundleName. [%{public}s]", bundleName.c_str());
            unInstallApps_[bundleName] = policyData;
            continue;
        }
        policyData.setFromConfigFlag = 1;
        int32_t insertRet = netAccessPolicyRdb.InsertData(policyData);
        if (insertRet != NETMANAGER_SUCCESS) {
            continue;
        }
        NetworkAccessPolicy policy;
        policy.wifiAllow = policyData.wifiPolicy == NET_ACCESS_POLICY_ALLOW_VALUE ? true : false;
        policy.cellularAllow = policyData.cellularPolicy == NET_ACCESS_POLICY_ALLOW_VALUE ? true : false;
        (void)netPolicyRule->SetNetworkAccessPolicy(policyData.uid, policy, true);
    }

    file.close();
    ClearBackupInfo();
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyDBClone::OnRestoreSingleApp(const std::string &bundleNameFromListen)
{
    NETMGR_LOG_I("Get OnRestoreSingleApp bundleName. [%{public}s]", bundleNameFromListen.c_str());
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return NETMANAGER_ERR_INTERNAL;
    }
    std::shared_ptr<NetPolicyRule> netPolicyRule =
        DelayedSingleton<NetPolicyCore>::GetInstance()->CreateCore<NetPolicyRule>();
    if (netPolicyRule == nullptr) {
        return NETMANAGER_ERROR;
    }

    NetAccessPolicyRDB netAccessPolicyRdb;
    std::string bundleName;
    NetAccessPolicyData policyData;
    auto it = unInstallApps_.find(bundleNameFromListen);
    if (it == unInstallApps_.end()) {
        return NETMANAGER_ERROR;
    }
    bundleName = it->first;
    policyData = it->second;
    policyData.uid = bundleMgrProxy->GetUidByBundleName(bundleName, MAIN_USER_ID);
    if (policyData.uid == -1) {
        NETMGR_LOG_I("policyData.uid = -1");
        return NETMANAGER_ERROR;
    }
    NETMGR_LOG_I("Get policyData. [%{public}d, %{public}d]", policyData.wifiPolicy, policyData.cellularPolicy);
    policyData.setFromConfigFlag = 1;
    int32_t insertRet = netAccessPolicyRdb.InsertData(policyData);
    if (insertRet != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("insert error");
        return NETMANAGER_ERROR;
    }
    NetworkAccessPolicy policy;
    policy.wifiAllow = policyData.wifiPolicy == NET_ACCESS_POLICY_ALLOW_VALUE ? true : false;
    policy.cellularAllow = policyData.cellularPolicy == NET_ACCESS_POLICY_ALLOW_VALUE ? true : false;
    (void)netPolicyRule->SetNetworkAccessPolicy(policyData.uid, policy, true);
    return NETMANAGER_SUCCESS;
}

bool NetPolicyDBClone::FdClone(UniqueFd &fd)
{
    struct stat statBuf;
    if (fd.Get() < 0 || fstat(fd.Get(), &statBuf) < 0) {
        NETMGR_LOG_E("OnRestore fstat fd fail.");
        return false;
    }

    int destFd = open(POLICY_DATABASE_BACKUP_FILE, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
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

void NetPolicyDBClone::ClearBackupInfo()
{
    NETMGR_LOG_I("start timer: clearBackupInfo");
    std::function<void()> ClearApps = [this]() {
        std::lock_guard<ffrt::mutex> lock(mutex_);
        this->unInstallApps_.clear();
        NETMGR_LOG_I("ClearBackupApps");
    };

    ffrt::submit(std::move(ClearApps), {}, {},
        ffrt::task_attr().delay(static_cast<uint64_t>(DAY_MICROSECONDS)).name("FfrtTimerClearBackupInfo"));
}
}
}

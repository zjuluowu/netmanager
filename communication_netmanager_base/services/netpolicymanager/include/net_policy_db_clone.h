/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#ifndef NET_POLICY_DB_CLONE_H
#define NET_POLICY_DB_CLONE_H

#include <mutex>
#include <string>
#include "unique_fd.h"
#include "net_access_policy_rdb.h"
#include "ffrt.h"
namespace OHOS {
namespace NetManagerStandard {
class NetPolicyDBClone {
public:
    static NetPolicyDBClone &GetInstance();
    int32_t OnBackup(UniqueFd &fd, const std::string &backupInfo);
    int32_t OnRestore(UniqueFd &fd, const std::string &restoreInfo);
    int32_t OnRestoreSingleApp(const std::string &bundleName);
    void ClearBackupInfo();
    bool FdClone(UniqueFd &fd);
    std::map<std::string, NetAccessPolicyData> unInstallApps_;

private:
    ffrt::mutex mutex_;
};
}
}

#endif
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

#ifndef OHOS_MOCK_NETPOLICY_DB_CLONE_H
#define OHOS_MOCK_NETPOLICY_DB_CLONE_H

#include <gmock/gmock.h>

#include <string>
#include "unique_fd.h"

namespace OHOS {
namespace NetManagerStandard {

class MockNetPolicyDBClone {
public:
    virtual ~MockNetPolicyDBClone() = default;
    virtual int32_t OnBackup(UniqueFd &fd, const std::string &backupInfo) = 0;
    virtual int32_t OnRestore(UniqueFd &fd, const std::string &restoreInfo) = 0;
    virtual bool FdClone(UniqueFd &fd) = 0;
    virtual int32_t OnRestoreSingleApp(const std::string &bundleNameFromListen) = 0;
};

class NetPolicyDBClone : public MockNetPolicyDBClone {
public:
    static NetPolicyDBClone &GetInstance(void);
    MOCK_METHOD2(OnBackup, int32_t(UniqueFd &fd, const std::string &backupInfo));
    MOCK_METHOD2(OnRestore, int32_t(UniqueFd &fd, const std::string &backupInfo));
    MOCK_METHOD1(FdClone, bool(UniqueFd &fd));
    MOCK_METHOD1(OnRestoreSingleApp, int32_t(const std::string &bundleNameFromListen));
};
}  // namespace NetManagerStandard
}  // namespace OHOS
#endif
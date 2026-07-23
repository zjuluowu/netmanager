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

#ifndef NET_DISTRIBUTED_MANAGER_H
#define NET_DISTRIBUTED_MANAGER_H

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <linux/if.h>
#include "uid_range.h"

namespace OHOS {
namespace NetManagerStandard {

class DistributedManager {
public:
    static DistributedManager &GetInstance()
    {
        static DistributedManager instance;
        return instance;
    }

public:
    void SetServerNicInfo(const std::string &iif, const std::string &devIface);
    std::string GetServerIifNic();
    std::string GetServerDevIfaceNic();
    int32_t ConfigVirnicAndVeth(const std::string &virNicAddr, const std::string &virnicName,
        const std::string &virnicVethName);
    void DisableVirnic(const std::string &virnicName);

private:
    DistributedManager() = default;
    ~DistributedManager() = default;
    DistributedManager(const DistributedManager &) = delete;
    DistributedManager &operator=(const DistributedManager &) = delete;

private:
    std::string serverIif_;
    std::string serverDevIface_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_DISTRIBUTED_MANAGER_H

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

#ifndef NET_VNIC_MANAGER_H
#define NET_VNIC_MANAGER_H

#include "uid_range.h"
#include <cstdint>
#include <linux/if.h>
#include <map>
#include <set>
#include <string>
#include <mutex>

namespace OHOS {
namespace NetManagerStandard {

class VnicManager {
private:
    VnicManager() = default;
    ~VnicManager() = default;
    VnicManager(const VnicManager &) = delete;
    VnicManager &operator=(const VnicManager &) = delete;

public:
    static VnicManager &GetInstance()
    {
        static VnicManager instance;
        return instance;
    }

public:
    int32_t CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix, const std::set<int32_t> &uids);
    int32_t DestroyVnic();
    int32_t CreateVnicInterface();
    void DestroyVnicInterface();
    int32_t SetVnicMtu(const std::string &ifName, int32_t mtu);
    int32_t SetVnicAddress(const std::string &ifName, const std::string &tunAddr, int32_t prefix);

private:
    std::atomic_int& GetNetSock(bool ipv4);
    int32_t SetVnicUp();
    int32_t SetVnicDown();
    int32_t AddDefaultRoute();
    int32_t DelDefaultRoute();
    int32_t InitIfreq(ifreq &ifr, const std::string &cardName);
    int32_t SetVnicResult(std::atomic_int &fd, unsigned long cmd, ifreq &ifr);

private:
    std::atomic_int tunFd_ = 0;
    std::atomic_int net4Sock_ = 0;
    std::atomic_int net6Sock_ = 0;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    std::mutex vnicMutex_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_VNIC_MANAGER_H

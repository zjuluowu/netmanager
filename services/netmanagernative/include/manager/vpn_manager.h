/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NET_VPN_MANAGER_H
#define NET_VPN_MANAGER_H

#include <cstdint>
#include <net/if.h>
#include <string>

namespace OHOS {
namespace NetManagerStandard {
class VpnManager : public std::enable_shared_from_this<VpnManager> {
private:
    VpnManager(const VpnManager &) = delete;
    VpnManager &operator=(const VpnManager &) = delete;

public:
    VpnManager() = default;
    ~VpnManager() = default;
    static VpnManager &GetInstance()
    {
        static std::shared_ptr<VpnManager> instance = std::make_shared<VpnManager>();
        return *instance;
    }

public:
    int32_t CreateVpnInterface();
    void DestroyVpnInterface();
    int32_t SetVpnMtu(const std::string &ifName, int32_t mtu);
    int32_t SetVpnAddress(const std::string &ifName, const std::string &tunAddr, int32_t prefix);

private:
    int32_t SetVpnUp();
    int32_t SetVpnDown();
    int32_t InitIfreq(ifreq &ifr, const std::string &cardName);
    int32_t SetVpnResult(std::atomic_int &fd, unsigned long cmd, ifreq &ifr);

    int32_t SendNetlinkAddress(int ifindex, int family, const char* addrbuf, int prefix);
    int32_t SendVpnInterfaceFdToClient(int32_t clientFd, int32_t tunFd);
    void StartUnixSocketListen();
    void StartVpnInterfaceFdListen();

private:
    std::atomic_int tunFd_ = 0;
    std::atomic_int net4Sock_ = 0;
    std::atomic_int net6Sock_ = 0;
    std::atomic_bool listeningFlag_ = false;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_VPN_MANAGER_H

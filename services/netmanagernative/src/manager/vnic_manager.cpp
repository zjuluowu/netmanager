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

#include "vnic_manager.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/ipv6.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

#include "init_socket.h"
#include "net_manager_constants.h"
#include "netlink_socket_diag.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "route_manager.h"
#include "securec.h"

#include "uid_range.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {
constexpr const char *VNIC_TUN_CARD_NAME = "vnic-tun";
constexpr const char *VNIC_TUN_DEVICE_PATH = "/dev/tun";
constexpr int32_t NET_MASK_MAX_LENGTH = 32;
constexpr int32_t NET6_MASK_MAX_LENGTH = 128;
constexpr uint32_t MAX_VNIC_UID_ARRAY_SIZE = 20;
} // namespace


std::atomic_int& VnicManager::GetNetSock(bool ipv4)
{
    if (ipv4) {
        // LCOV_EXCL_START
        if (net4Sock_ < 0) {
            net4Sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        }
        // LCOV_EXCL_STOP
        return net4Sock_;
    } else {
        // LCOV_EXCL_START
        if (net6Sock_ < 0) {
            net6Sock_ = socket(AF_INET6, SOCK_DGRAM, 0);
        }
        // LCOV_EXCL_STOP
        return net6Sock_;
    }
}

int32_t VnicManager::CreateVnicInterface()
{
    // LCOV_EXCL_START
    if (tunFd_ != 0) {
        return NETMANAGER_SUCCESS;
    }
    // LCOV_EXCL_STOP

    ifreq ifr{};
    if (InitIfreq(ifr, VNIC_TUN_CARD_NAME) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    int32_t tunfd = open(VNIC_TUN_DEVICE_PATH, O_RDWR | O_NONBLOCK);
    // LCOV_EXCL_START
    if (tunfd <= 0) {
        NETNATIVE_LOGE("open virtual device failed: %{public}d", errno);
        return NETMANAGER_ERROR;
    }

    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    if (ioctl(tunfd, TUNSETIFF, &ifr) < 0) {
        close(tunfd);
        NETNATIVE_LOGE("tun set iff error: %{public}d", errno);
        return NETMANAGER_ERROR;
    }

    net4Sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (net4Sock_ < 0) {
        NETNATIVE_LOGE("create SOCK_DGRAM ipv4 failed: %{public}d", errno);
    }
    net6Sock_ = socket(AF_INET6, SOCK_DGRAM, 0);
    if (net6Sock_ < 0) {
        NETNATIVE_LOGE("create SOCK_DGRAM ipv6 failed: %{public}d", errno);
    }

    if (net4Sock_ < 0 && net6Sock_ < 0) {
        close(tunfd);
        NETNATIVE_LOGE("create SOCK_DGRAM ip failed");
        DestroyVnicInterface();
        return NETMANAGER_ERROR;
    }
    // LCOV_EXCL_STOP

    NETNATIVE_LOGI("open virtual device successfully, [%{public}d]", tunfd);
    tunFd_ = tunfd;
    SetVnicUp();
    return NETMANAGER_SUCCESS;
}

void VnicManager::DestroyVnicInterface()
{
    SetVnicDown();
    if (net4Sock_ != 0) {
        close(net4Sock_);
        net4Sock_ = 0;
    }
    if (net6Sock_ != 0) {
        close(net6Sock_);
        net6Sock_ = 0;
    }
    if (tunFd_ != 0) {
        close(tunFd_);
        tunFd_ = 0;
    }
}

int32_t VnicManager::SetVnicResult(std::atomic_int &fd, unsigned long cmd, ifreq &ifr)
{
    if (fd > 0) {
        // LCOV_EXCL_START
        if (ioctl(fd, cmd, &ifr) < 0) {
            NETNATIVE_LOGE("set vnic error, errno:%{public}d", errno);
            return NETMANAGER_ERROR;
        }
        // LCOV_EXCL_STOP
    }
    return NETMANAGER_SUCCESS;
}

int32_t VnicManager::SetVnicMtu(const std::string &ifName, int32_t mtu)
{
    if (mtu <= 0) {
        NETNATIVE_LOGE("invalid mtu value");
        return NETMANAGER_ERROR;
    }

    ifreq ifr;
    if (InitIfreq(ifr, ifName) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    ifr.ifr_mtu = mtu;
    int32_t ret4 = SetVnicResult(GetNetSock(true), SIOCSIFMTU, ifr);
    int32_t ret6 = SetVnicResult(GetNetSock(false), SIOCSIFMTU, ifr);
    if (ret4 == NETMANAGER_ERROR || ret6 == NETMANAGER_ERROR || (GetNetSock(true) < 0 && GetNetSock(false) < 0)) {
        NETNATIVE_LOGI("set MTU failed");
        return NETMANAGER_ERROR;
    } else {
        NETNATIVE_LOGI("set MTU success");
        return NETMANAGER_SUCCESS;
    }
}

int32_t VnicManager::SetVnicAddress(const std::string &ifName, const std::string &tunAddr, int32_t prefix)
{
    ifreq ifr{};
    if (InitIfreq(ifr, ifName) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    bool isIpv6 = CommonUtils::IsValidIPV6(tunAddr);
    if (isIpv6) {
        struct in6_ifreq ifr6 = {};
        if (ioctl(GetNetSock(false), SIOCGIFINDEX, &ifr) <0) {
            NETNATIVE_LOGE(" get network interface ipv6 failed: %{public}d", errno);
            return NETMANAGER_ERROR;
        }
        if (inet_pton(AF_INET6, tunAddr.c_str(), &ifr6.ifr6_addr) == 0) {
            NETNATIVE_LOGE("inet_pton ipv6 address failed: %{public}d", errno);
        }
        if (prefix <= 0 || prefix >= NET6_MASK_MAX_LENGTH) {
            NETNATIVE_LOGE("ipv6 prefix: %{public}d error", prefix);
            return NETMANAGER_ERROR;
        }
        ifr6.ifr6_prefixlen = static_cast<uint32_t>(prefix);
        ifr6.ifr6_ifindex = ifr.ifr_ifindex;
        if (ioctl(GetNetSock(false), SIOCSIFADDR, &ifr6) < 0) {
            NETNATIVE_LOGE("ioctl set ipv6 address failed: %{public}d", errno);
            return NETMANAGER_ERROR;
        }
    } else {
        in_addr ipv4Addr = {};
        if (inet_aton(tunAddr.c_str(), &ipv4Addr) == 0) {
            NETNATIVE_LOGE("addr inet_aton error");
            return NETMANAGER_ERROR;
        }

        auto sin = reinterpret_cast<sockaddr_in *>(&ifr.ifr_addr);
        sin->sin_family = AF_INET;
        sin->sin_addr = ipv4Addr;
        if (ioctl(GetNetSock(true), SIOCSIFADDR, &ifr) < 0) {
            NETNATIVE_LOGE("ioctl set ipv4 address failed: %{public}d", errno);
            return NETMANAGER_ERROR;
        }

        if (prefix <= 0 || prefix >= NET_MASK_MAX_LENGTH) {
            NETNATIVE_LOGE("prefix: %{public}d error", prefix);
            return NETMANAGER_ERROR;
        }
        in_addr_t mask = prefix ? (0xFFFFFFFF << (NET_MASK_MAX_LENGTH - prefix)) : 0;
        sin = reinterpret_cast<sockaddr_in *>(&ifr.ifr_netmask);
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = htonl(mask);
        if (ioctl(GetNetSock(true), SIOCSIFNETMASK, &ifr) < 0) {
            NETNATIVE_LOGE("ioctl set ip mask failed: %{public}d", errno);
            return NETMANAGER_ERROR;
        }
    }

    NETNATIVE_LOGI("set ip address success");
    return NETMANAGER_SUCCESS;
}

int32_t VnicManager::SetVnicUp()
{
    ifreq ifr{};
    if (InitIfreq(ifr, VNIC_TUN_CARD_NAME) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    ifr.ifr_flags = IFF_UP;
    int32_t ret4 = SetVnicResult(GetNetSock(true), SIOCSIFFLAGS, ifr);
    int32_t ret6 = SetVnicResult(GetNetSock(false), SIOCSIFFLAGS, ifr);
    if (ret4 == NETMANAGER_ERROR || ret6 == NETMANAGER_ERROR || (GetNetSock(true) < 0 && GetNetSock(false) < 0)) {
        NETNATIVE_LOGI("set iff up failed");
        return NETMANAGER_ERROR;
    } else {
        NETNATIVE_LOGI("set iff up success");
        return NETMANAGER_SUCCESS;
    }
}

int32_t VnicManager::SetVnicDown()
{
    ifreq ifr{};
    if (InitIfreq(ifr, VNIC_TUN_CARD_NAME) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    ifr.ifr_flags = (uint16_t)ifr.ifr_flags & ~IFF_UP;
    int32_t ret4 = SetVnicResult(GetNetSock(true), SIOCSIFFLAGS, ifr);
    int32_t ret6 = SetVnicResult(GetNetSock(false), SIOCSIFFLAGS, ifr);
    if (ret4 == NETMANAGER_ERROR || ret6 == NETMANAGER_ERROR || (GetNetSock(true) < 0 && GetNetSock(false) < 0)) {
        NETNATIVE_LOGI("set iff down failed");
        return NETMANAGER_ERROR;
    } else {
        NETNATIVE_LOGI("set iff down success");
        return NETMANAGER_SUCCESS;
    }
}

int32_t VnicManager::AddDefaultRoute()
{
    const std::string interface = VNIC_TUN_CARD_NAME;
    const std::string destinationName = "0.0.0.0/0";
    const std::string nextHop = "0.0.0.0";
    return nmd::RouteManager::UpdateVnicRoute(interface, destinationName, nextHop, true);
}

int32_t VnicManager::DelDefaultRoute()
{
    const std::string interface = VNIC_TUN_CARD_NAME;
    const std::string destinationName = "0.0.0.0/0";
    const std::string nextHop = "0.0.0.0";
    return nmd::RouteManager::UpdateVnicRoute(interface, destinationName, nextHop, false);
}

int32_t VnicManager::InitIfreq(ifreq &ifr, const std::string &cardName)
{
    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        NETNATIVE_LOGE("memset_s ifr failed!");
        return NETMANAGER_ERROR;
    }
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, cardName.c_str(), strlen(cardName.c_str())) != EOK) {
        NETNATIVE_LOGE("strcpy_s ifr name fail");
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}

int32_t VnicManager::CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix,
                                const std::set<int32_t> &uids)
{
    std::unique_lock<std::mutex> lock(vnicMutex_);
    if (uids.size() > MAX_VNIC_UID_ARRAY_SIZE) {
        NETNATIVE_LOGE("vnic uids's size is over the max size.");
        return NETMANAGER_ERROR;
    }
    
    uidRanges.clear();
    for (const auto &uid: uids) {
        uidRanges.push_back({uid, uid, 0, 0});
    }

    if (CreateVnicInterface() != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }
    if (SetVnicMtu(VNIC_TUN_CARD_NAME, mtu) != NETMANAGER_SUCCESS ||
        SetVnicAddress(VNIC_TUN_CARD_NAME, tunAddr, prefix) != NETMANAGER_SUCCESS ||
        AddDefaultRoute() != NETMANAGER_SUCCESS) {
        DestroyVnicInterface();
        return NETMANAGER_ERROR;
    }

    if (!uidRanges.empty() &&
        nmd::RouteManager::UpdateVnicUidRangesRule(uidRanges, true) != NETMANAGER_SUCCESS) {
        uidRanges.clear();
        DelDefaultRoute();
        DestroyVnicInterface();
        return NETMANAGER_ERROR;
    }

    nmd::NetLinkSocketDiag socketDiag;
    for (auto const &uid : uidRanges) {
        NETNATIVE_LOG_D("CreateVnic uid %{public}d", (uint32_t)uid.begin_);
        socketDiag.DestroyLiveSocketsWithUid("", (uint32_t)uid.begin_);
    }

    return NETMANAGER_SUCCESS;
}

int32_t VnicManager::DestroyVnic()
{
    std::unique_lock<std::mutex> lock(vnicMutex_);
    nmd::NetLinkSocketDiag socketDiag;
    nmd::RouteManager::UpdateVnicUidRangesRule(uidRanges, false);
    DelDefaultRoute();
    DestroyVnicInterface();
    for (auto const &uid : uidRanges) {
        NETNATIVE_LOG_D("DestroyVnic uid %{public}d", (uint32_t)uid.begin_);
        socketDiag.DestroyLiveSocketsWithUid("", (uint32_t)uid.begin_);
    }
    uidRanges.clear();
    return NETMANAGER_SUCCESS;
}

} // namespace NetManagerStandard
} // namespace OHOS

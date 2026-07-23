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

#include "vpn_manager.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/ipv6.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <cstring>
#include <string>

#include "netlink_msg.h"
#include "netlink_socket.h"
#include "init_socket.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "securec.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {
constexpr const char *TUN_CARD_NAME = "vpn-tun";
constexpr const char *TUN_DEVICE_PATH = "/dev/tun";
constexpr int32_t NET_MASK_MAX_LENGTH = 32;
constexpr int32_t MAX_UNIX_SOCKET_CLIENT = 5;
constexpr uint8_t IPV4_LEN = 32;
constexpr uint8_t IPV6_LEN = 128;
} // namespace

int32_t VpnManager::CreateVpnInterface()
{
    if (tunFd_ != 0) {
        StartVpnInterfaceFdListen();
        return NETMANAGER_SUCCESS;
    }

    ifreq ifr = {};
    if (InitIfreq(ifr, TUN_CARD_NAME) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    int32_t tunfd = open(TUN_DEVICE_PATH, O_RDWR | O_NONBLOCK | O_CLOEXEC);
    if (tunfd <= 0) {
        NETNATIVE_LOGE("open virtual device failed: %{public}d", errno);
        DestroyVpnInterface();
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
        DestroyVpnInterface();
        return NETMANAGER_ERROR;
    }

    NETNATIVE_LOGI("open virtual device successfully, [%{public}d]", tunfd);
    tunFd_ = tunfd;
    SetVpnUp();
    StartVpnInterfaceFdListen();
    return NETMANAGER_SUCCESS;
}

void VpnManager::DestroyVpnInterface()
{
    SetVpnDown();
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

int32_t VpnManager::SetVpnResult(std::atomic_int &fd, unsigned long cmd, ifreq &ifr)
{
    if (fd > 0) {
        if (ioctl(fd, cmd, &ifr) < 0) {
            NETNATIVE_LOGE("set vpn error, errno:%{public}d", errno);
            return NETMANAGER_ERROR;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t VpnManager::SetVpnMtu(const std::string &ifName, int32_t mtu)
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
    int32_t ret4 = SetVpnResult(net4Sock_, SIOCSIFMTU, ifr);
    int32_t ret6 = SetVpnResult(net6Sock_, SIOCSIFMTU, ifr);
    if (ret4 == NETMANAGER_ERROR || ret6 == NETMANAGER_ERROR || (net4Sock_ < 0 && net6Sock_ < 0)) {
        NETNATIVE_LOGI("set MTU failed");
        return NETMANAGER_ERROR;
    } else {
        NETNATIVE_LOGI("set MTU success");
        return NETMANAGER_SUCCESS;
    }
}

int32_t VpnManager::SetVpnAddress(const std::string &ifName, const std::string &tunAddr, int32_t prefix)
{
    char addrbuf[sizeof(in6_addr)] = {0};
    int family = -1;

    if (inet_pton(AF_INET, tunAddr.c_str(), addrbuf) == 1) {
        family = AF_INET;
    } else if (inet_pton(AF_INET6, tunAddr.c_str(), addrbuf) == 1) {
        family = AF_INET6;
    } else {
        NETNATIVE_LOGE("invalid IP address: %{public}s", tunAddr.c_str());
        return NETMANAGER_ERROR;
    }

    uint32_t ifindex = if_nametoindex(ifName.c_str());
    if (ifindex == 0) {
        NETNATIVE_LOGE("if_nametoindex failed: %{public}d", errno);
        return NETMANAGER_ERROR;
    }

    return SendNetlinkAddress(ifindex, family, addrbuf, prefix);
}

int32_t VpnManager::SendNetlinkAddress(int ifindex, int family, const char* addrbuf, int prefix)
{
    if ((family == AF_INET && (prefix < 0 || prefix > IPV4_LEN)) ||
        (family == AF_INET6 && (prefix < 0 || prefix > IPV6_LEN))) {
        NETNATIVE_LOGE("Invalid prefix length: %{public}d", prefix);
        return NETMANAGER_ERROR;
    }

    constexpr size_t kMaxMsgLen = 4096;
    nmd::NetlinkMsg netMsg(NLM_F_CREATE, kMaxMsgLen, getpid());

    ifaddrmsg ifa {};
    ifa.ifa_family = static_cast<uint8_t>(family);
    ifa.ifa_prefixlen = static_cast<uint8_t>(prefix);
    ifa.ifa_flags = IFA_F_PERMANENT;
    ifa.ifa_scope = 0;
    ifa.ifa_index = static_cast<uint32_t>(ifindex);

    netMsg.AddAddress(RTM_NEWADDR, ifa);

    int addrLen = (family == AF_INET) ? 4 : 16;
    int32_t resultLocal = netMsg.AddAttr(IFA_LOCAL, const_cast<char*>(addrbuf), addrLen);
    int32_t resultAddress = netMsg.AddAttr(IFA_ADDRESS, const_cast<char*>(addrbuf), addrLen);
    if (resultLocal < 0 || resultAddress < 0) {
        NETNATIVE_LOGE("AddAttr failed");
        return NETMANAGER_ERROR;
    }

    return nmd::SendNetlinkMsgToKernel(netMsg.GetNetLinkMessage(), 0);
}

int32_t VpnManager::SetVpnUp()
{
    ifreq ifr = {};
    if (InitIfreq(ifr, TUN_CARD_NAME) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    ifr.ifr_flags = IFF_UP;
    int32_t ret4 = SetVpnResult(net4Sock_, SIOCSIFFLAGS, ifr);
    int32_t ret6 = SetVpnResult(net6Sock_, SIOCSIFFLAGS, ifr);
    if (ret4 == NETMANAGER_ERROR || ret6 == NETMANAGER_ERROR || (net4Sock_ < 0 && net6Sock_ < 0)) {
        NETNATIVE_LOGI("set iff up failed");
        return NETMANAGER_ERROR;
    } else {
        NETNATIVE_LOGI("set iff up success");
        return NETMANAGER_SUCCESS;
    }
}

int32_t VpnManager::SetVpnDown()
{
    ifreq ifr = {};
    if (InitIfreq(ifr, TUN_CARD_NAME) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    ifr.ifr_flags &= ~IFF_UP;
    int32_t ret4 = SetVpnResult(net4Sock_, SIOCSIFFLAGS, ifr);
    int32_t ret6 = SetVpnResult(net6Sock_, SIOCSIFFLAGS, ifr);
    if (ret4 == NETMANAGER_ERROR || ret6 == NETMANAGER_ERROR || (net4Sock_ < 0 && net6Sock_ < 0)) {
        NETNATIVE_LOGI("set iff down failed");
        return NETMANAGER_ERROR;
    } else {
        NETNATIVE_LOGI("set iff down success");
        return NETMANAGER_SUCCESS;
    }
}

int32_t VpnManager::InitIfreq(ifreq &ifr, const std::string &cardName)
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

int32_t VpnManager::SendVpnInterfaceFdToClient(int32_t clientFd, int32_t tunFd)
{
    char buf[1] = {0};
    iovec iov;
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    union {
        cmsghdr align;
        char cmsg[CMSG_SPACE(sizeof(int32_t))];
    } cmsgu;
    if (memset_s(cmsgu.cmsg, sizeof(cmsgu.cmsg), 0, sizeof(cmsgu.cmsg)) != EOK) {
        NETNATIVE_LOGE("memset_s cmsgu.cmsg failed!");
        return NETMANAGER_ERROR;
    }
    msghdr message;
    if (memset_s(&message, sizeof(message), 0, sizeof(message)) != EOK) {
        NETNATIVE_LOGE("memset_s message failed!");
        return NETMANAGER_ERROR;
    }

    message.msg_iov = &iov;
    message.msg_iovlen = 1;
    message.msg_control = cmsgu.cmsg;
    message.msg_controllen = sizeof(cmsgu.cmsg);
    cmsghdr *cmsgh = CMSG_FIRSTHDR(&message);
    if (cmsgh == nullptr) {
        NETNATIVE_LOGE("CMSG_FIRSTHDR return nullptr!");
        return NETMANAGER_ERROR;
    }
    cmsgh->cmsg_len = CMSG_LEN(sizeof(tunFd));
    cmsgh->cmsg_level = SOL_SOCKET;
    cmsgh->cmsg_type = SCM_RIGHTS;
    if (memcpy_s(CMSG_DATA(cmsgh), sizeof(tunFd), &tunFd, sizeof(tunFd)) != EOK) {
        NETNATIVE_LOGE("memcpy_s cmsgu failed!");
        return NETMANAGER_ERROR;
    }
    if (sendmsg(clientFd, &message, 0) < 0) {
        NETNATIVE_LOGE("sendmsg error: %{public}d, clientfd[%{public}d], tunfd[%{public}d]", errno, clientFd, tunFd);
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}

void VpnManager::StartUnixSocketListen()
{
    NETNATIVE_LOGI("StartUnixSocketListen...");
    int32_t serverfd = GetControlSocket("tunfd");
    if (serverfd < 0) {
        NETNATIVE_LOGE("get control socket error: %{public}d", errno);
        return;
    }
    if (listen(serverfd, MAX_UNIX_SOCKET_CLIENT) < 0) {
        close(serverfd);
        NETNATIVE_LOGE("listen socket error: %{public}d", errno);
        return;
    }

    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    while (true) {
        int32_t clientFd = accept(serverfd, reinterpret_cast<sockaddr *>(&clientAddr), &len);
        if (clientFd < 0) {
            NETNATIVE_LOGE("accept socket error: %{public}d", errno);
            continue;
        }

        SendVpnInterfaceFdToClient(clientFd, tunFd_);
        close(clientFd);
    }

    close(serverfd);
    listeningFlag_ = false;
}

void VpnManager::StartVpnInterfaceFdListen()
{
    if (listeningFlag_) {
        NETNATIVE_LOGI("VpnInterface fd is listening...");
        return;
    }

    NETNATIVE_LOGI("StartVpnInterfaceFdListen...");
    std::thread t([sp = shared_from_this()]() { sp->StartUnixSocketListen(); });
    t.detach();
    pthread_setname_np(t.native_handle(), "unix_socket_tunfd");
    listeningFlag_ = true;
}

} // namespace NetManagerStandard
} // namespace OHOS

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

#include <arpa/inet.h>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <linux/filter.h>
#include <linux/if_packet.h>
#include <linux/if_tun.h>
#include <linux/ipv6.h>
#include <list>
#include <mutex>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "clat_utils.h"
#include "ffrt.h"
#include "netmanager_base_common_utils.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;
ffrt::mutex g_tunV4AddrMutex;
std::list<in_addr_t> g_tunV4AddrInUse;

bool IsIpv4AddressFree(const in_addr_t v4Addr)
{
    std::lock_guard<ffrt::mutex> lock(g_tunV4AddrMutex);
    if (std::find(g_tunV4AddrInUse.begin(), g_tunV4AddrInUse.end(), v4Addr) != g_tunV4AddrInUse.end()) {
        return false;
    }
    int s = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (s == -1) {
        return false;
    }

    // check if the address is available by trying to connect to it
    sockaddr_in sin = {
        .sin_family = AF_INET,
        .sin_port = htons(WKN_DNS_PORT),
        .sin_addr = {v4Addr},
    };
    socklen_t len = sizeof(sin);
    const bool inuse = !connect(s, reinterpret_cast<sockaddr *>(&sin), sizeof(sin)) &&
                       !getsockname(s, reinterpret_cast<sockaddr *>(&sin), &len) &&
                       len == static_cast<socklen_t>(sizeof(sin)) && sin.sin_addr.s_addr == v4Addr;

    close(s);
    // LCOV_EXCL_START
    if (!inuse) {
        g_tunV4AddrInUse.emplace_back(v4Addr);
    }
    // LCOV_EXCL_STOP
    return !inuse;
}

void FreeTunV4Addr(const std::string &v4AddrStr)
{
    std::lock_guard<ffrt::mutex> lock(g_tunV4AddrMutex);
    in_addr v4Addr;
    if (inet_pton(AF_INET, v4AddrStr.c_str(), &v4Addr) != 1) {
        NETNATIVE_LOGW("fail to free tun v4 address, tun address invalid");
        return;
    }
    g_tunV4AddrInUse.remove(v4Addr.s_addr);
}

in_addr_t GetAvailableIpv4Address(const in_addr initV4Addr, const int16_t prefixLen)
{
    if (prefixLen < 0 || prefixLen > V4ADDR_BIT_LEN) {
        return INADDR_NONE;
    }
    const uint32_t mask =
        (prefixLen == 0) ? 0 : (0xffffffffu >> (V4ADDR_BIT_LEN - prefixLen) << (V4ADDR_BIT_LEN - prefixLen));
    uint32_t v4Num = ntohl(initV4Addr.s_addr);
    const uint32_t initV4Num = v4Num;
    const uint32_t prefix = v4Num & mask;

    do {
        if (IsIpv4AddressFree(htonl(v4Num))) {
            return htonl(v4Num);
        }
        v4Num = prefix | ((v4Num + 1) & ~mask);
    } while (v4Num != initV4Num);

    return INADDR_NONE;
}

int32_t SelectIpv4Address(const std::string &initV4AddrStr, int prefixLen, std::string &v4AddrStr)
{
    in_addr initV4Addr;
    if (inet_pton(AF_INET, initV4AddrStr.c_str(), &initV4Addr) != 1) {
        NETNATIVE_LOGW("fail to select ipv4 address for tun, init address invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    in_addr v4Addr = {GetAvailableIpv4Address(initV4Addr, prefixLen)};
    if (v4Addr.s_addr == INADDR_NONE) {
        NETNATIVE_LOGW("No free IPv4 address in %{public}s/%{public}d",
            CommonUtils::ToAnonymousIp(initV4AddrStr).c_str(), prefixLen);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    char addrstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, reinterpret_cast<void *>(&v4Addr), addrstr, sizeof(addrstr));
    v4AddrStr = addrstr;
    return NETMANAGER_SUCCESS;
}

uint16_t Checksum32To16(uint32_t sum32)
{
    while (sum32 >> (sizeof(uint16_t) * CHAR_BIT)) {
        sum32 = (sum32 & 0xffff) + (sum32 >> (sizeof(uint16_t) * CHAR_BIT));
    }
    return sum32;
}

uint16_t AdjustChecksum(uint16_t oldSum16, uint32_t oldSumHdr, uint32_t newSumHdr)
{
    // More details in RFC 1624.
    oldSum16 = ~oldSum16;
    uint16_t sumFolded = Checksum32To16(newSumHdr + oldSum16);
    uint16_t oldFolded = Checksum32To16(oldSumHdr);
    if (sumFolded > oldFolded) {
        return ~(sumFolded - oldFolded);
    }
    return ~(sumFolded - oldFolded - 1);
}

uint32_t AddChecksum(uint32_t sum, const void *data, int len)
{
    const uint16_t *single16 = reinterpret_cast<const uint16_t *>(data);
    int multiplier = sizeof(uint32_t) / sizeof(uint16_t);
    while (len >= multiplier) {
        sum += *single16;
        single16++;
        len -= multiplier;
    }
    if (len) {
        sum += *reinterpret_cast<const uint8_t *>(single16);
    }

    return sum;
}

void MakeChecksumNeutral(in6_addr &v6Addr, const in_addr &v4Addr, const in6_addr &nat64Prefix)
{
    arc4random_buf(&v6Addr.s6_addr[CLAT_V6ADDR_RANDOMIZE_OFFSET], CLAT_V6ADDR_RANDOMIZE_BIT_LENGTH);

    size_t adjustOffset = CLAT_V6ADDR_RANDOMIZE_OFFSET + CLAT_V6ADDR_NEUTRALIZE_OFFSET;
    uint16_t middleBytes = (v6Addr.s6_addr[adjustOffset] << CHAR_BIT) + v6Addr.s6_addr[adjustOffset + 1];

    uint32_t v4Checksum = AddChecksum(0, &v4Addr, sizeof(v4Addr));
    uint32_t v6Checksum = AddChecksum(0, &nat64Prefix, sizeof(nat64Prefix)) + AddChecksum(0, &v6Addr, sizeof(v6Addr));

    uint16_t delta = AdjustChecksum(middleBytes, v4Checksum, v6Checksum);
    v6Addr.s6_addr[adjustOffset] = delta >> CHAR_BIT;
    v6Addr.s6_addr[adjustOffset + 1] = delta & 0xff;
}

int32_t GetSuitableIpv6Address(const std::string &v6IfaceStr, const in_addr v4Addr, const in6_addr &nat64Prefix,
                               in6_addr &v6Addr, const uint32_t mark)
{
    int s = socket(AF_INET6, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (s == -1)
        return NETMANAGER_ERR_OPERATION_FAILED;

    if (setsockopt(s, SOL_SOCKET, SO_MARK, &mark, sizeof(mark))) {
        auto err = errno;
        NETNATIVE_LOGW("setsockopt(SOL_SOCKET, SO_MARK) failed: %{public}s", strerror(err));
        close(s);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    if (setsockopt(s, SOL_SOCKET, SO_BINDTODEVICE, v6IfaceStr.c_str(), v6IfaceStr.length())) {
        auto err = errno;
        NETNATIVE_LOGW("setsockopt(SOL_SOCKET, SO_BINDTODEVICE, '%{public}s') failed: %{public}s", v6IfaceStr.c_str(),
                       strerror(err));
        close(s);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    sockaddr_in6 sin6 = {0};
    sin6.sin6_family = AF_INET6;
    sin6.sin6_addr = nat64Prefix;
    if (connect(s, reinterpret_cast<sockaddr *>(&sin6), sizeof(sin6))) {
        close(s);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    socklen_t len = sizeof(sin6);
    if (getsockname(s, reinterpret_cast<sockaddr *>(&sin6), &len)) {
        close(s);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    v6Addr = sin6.sin6_addr;

    MakeChecksumNeutral(v6Addr, v4Addr, nat64Prefix);
    close(s);

    return 0;
}

int32_t GenerateIpv6Address(const std::string &v6IfaceStr, const std::string &v4AddrStr, const std::string &prefix64Str,
                            uint32_t mark, std::string &v6AddrStr)
{
    if (v6IfaceStr.empty()) {
        NETNATIVE_LOGW("fail to generate ipv6 address, ipv6 interface name null");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    in_addr v4Addr;
    if (inet_pton(AF_INET, v4AddrStr.c_str(), &v4Addr) != 1) {
        NETNATIVE_LOGW("fail to generate ipv6 address, ipv4 address invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    in6_addr prefix64;
    if (inet_pton(AF_INET6, prefix64Str.c_str(), &prefix64) != 1) {
        NETNATIVE_LOGW("fail to generate ipv6 address, prefix invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    in6_addr v6Addr;
    int32_t ret = GetSuitableIpv6Address(v6IfaceStr, v4Addr, prefix64, v6Addr, mark);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("Unable to find global source address on %{public}s for %{public}s", v6IfaceStr.c_str(),
                       prefix64Str.c_str());
        return ret;
    }

    char addrstr[INET6_ADDRSTRLEN];
    if (!inet_ntop(AF_INET6, reinterpret_cast<void *>(&v6Addr), addrstr, sizeof(addrstr))) {
        NETNATIVE_LOGW("fail to generate ipv6 address, ipv6 address invalid");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    v6AddrStr = addrstr;
    return NETMANAGER_SUCCESS;
}

uint16_t CalChecksum(const void *data, int len)
{
    uint32_t tempSum = AddChecksum(0xffff, data, len);
    return ~Checksum32To16(tempSum);
}

int32_t CreateTunInterface(const std::string &tunIface, int &fd)
{
    fd = open("/dev/tun", O_RDWR | O_NONBLOCK | O_CLOEXEC);
    if (fd == -1) {
        NETNATIVE_LOGW("open tun device failed, errno: %{public}d", errno);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    struct ifreq ifr = {};
    ifr.ifr_flags = static_cast<short>(IFF_TUN | IFF_TUN_EXCL);

    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, tunIface.c_str(), tunIface.length()) != EOK) {
        close(fd);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    if (ioctl(fd, TUNSETIFF, &ifr, sizeof(ifr))) {
        close(fd);
        NETNATIVE_LOGW("ioctl(TUNSETIFF) failed");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return NETMANAGER_SUCCESS;
}

int32_t OpenPacketSocket(int &readSock6)
{
    readSock6 = socket(AF_PACKET, SOCK_RAW | SOCK_CLOEXEC, 0);
    if (readSock6 < 0) {
        NETNATIVE_LOGW("packet socket failed");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    const int on = 1;

    if (setsockopt(readSock6, SOL_PACKET, PACKET_AUXDATA, &on, sizeof(on))) {
        NETNATIVE_LOGW("packet socket auxdata enablement failed");
        close(readSock6);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    if (setsockopt(readSock6, SOL_PACKET, PACKET_VNET_HDR, &on, sizeof(on))) {
        NETNATIVE_LOGW("packet socket vnet_hdr enablement failed");
        close(readSock6);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}

int32_t OpenRawSocket6(const uint32_t mark, int &writeSock6)
{
    writeSock6 = socket(AF_INET6, SOCK_RAW | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_RAW);
    if (writeSock6 < 0) {
        NETNATIVE_LOGW("raw socket failed");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    if (setsockopt(writeSock6, SOL_SOCKET, SO_MARK, &mark, sizeof(mark)) < 0) {
        NETNATIVE_LOGW("could not set mark on raw socket");
        close(writeSock6);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return NETMANAGER_SUCCESS;
}

int32_t ConfigureWriteSocket(int sockFd, const std::string &v6Iface)
{
    if (sockFd < 0) {
        NETNATIVE_LOGW("Invalid file descriptor");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    int ret = setsockopt(sockFd, SOL_SOCKET, SO_BINDTODEVICE, v6Iface.c_str(),
                         static_cast<socklen_t>(v6Iface.length() + 1));
    if (ret) {
        NETNATIVE_LOGW("setsockopt SO_BINDTODEVICE failed: %{public}s", strerror(errno));
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}

int AddFilterAndBindPacketSocket(const int sock, const in6_addr *const addr, const int ifIndex)
{
    sock_filter filter[] = {
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS, static_cast<__u32>(SKF_NET_OFF) + offsetof(ipv6hdr, daddr.s6_addr32[0])),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ntohl(addr->s6_addr32[0]), 1, 0),
        BPF_STMT(BPF_RET | BPF_K, 0),
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS, static_cast<__u32>(SKF_NET_OFF) + offsetof(ipv6hdr, daddr.s6_addr32[1])),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ntohl(addr->s6_addr32[1]), 1, 0),
        BPF_STMT(BPF_RET | BPF_K, 0),
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS, static_cast<__u32>(SKF_NET_OFF) + offsetof(ipv6hdr, daddr.s6_addr32[2])),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ntohl(addr->s6_addr32[2]), 1, 0),
        BPF_STMT(BPF_RET | BPF_K, 0),
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS, static_cast<__u32>(SKF_NET_OFF) + offsetof(ipv6hdr, daddr.s6_addr32[3])),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ntohl(addr->s6_addr32[3]), 1, 0),
        BPF_STMT(BPF_RET | BPF_K, 0),
        BPF_STMT(BPF_RET | BPF_K, 0xFFFFFFFF),
    };
    sock_fprog filterProg = {sizeof(filter) / sizeof(filter[0]), filter};

    if (setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &filterProg, sizeof(filterProg))) {
        auto err = errno;
        NETNATIVE_LOGW("attach packet filter failed: %{public}s", strerror(err));
        return -err;
    }

    sockaddr_ll sll = {
        .sll_family = AF_PACKET,
        .sll_protocol = htons(ETH_P_IPV6),
        .sll_ifindex = ifIndex,
        .sll_pkttype = PACKET_OTHERHOST,
    };
    if (bind(sock, reinterpret_cast<sockaddr *>(&sll), sizeof(sll))) {
        auto err = errno;
        NETNATIVE_LOGW("binding packet socket failed: %{public}s", strerror(err));
        return -err;
    }
    return 0;
}

int32_t ConfigureReadSocket(int sockFd, const std::string &addrStr, int ifIndex)
{
    if (sockFd < 0) {
        NETNATIVE_LOGW("Invalid file descriptor");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    in6_addr addr;
    if (inet_pton(AF_INET6, addrStr.c_str(), &addr) != 1) {
        NETNATIVE_LOGW("Invalid IPv6 address %{public}s", CommonUtils::ToAnonymousIp(addrStr).c_str());
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    int ret = AddFilterAndBindPacketSocket(sockFd, &addr, ifIndex);
    if (ret < 0) {
        NETNATIVE_LOGW("configure packet socket failed");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}

int32_t SetTunInterfaceAddress(const std::string &ifName, const std::string &tunAddr, int32_t prefix)
{
    ifreq ifr = {};
    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        NETNATIVE_LOGE("memset_s ifr failed!");
        return NETMANAGER_ERROR;
    }
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, ifName.c_str(), strlen(ifName.c_str())) != EOK) {
        NETNATIVE_LOGE("strcpy_s ifr name fail");
        return NETMANAGER_ERROR;
    }

    in_addr ipv4Addr = {};
    if (inet_aton(tunAddr.c_str(), &ipv4Addr) == 0) {
        NETNATIVE_LOGE("addr inet_aton error");
        return NETMANAGER_ERROR;
    }

    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    // LCOV_EXCL_START
    if (socketfd < 0) {
        NETNATIVE_LOGE("socket fd is invalid, errno: %{public}d", errno);
        return NETMANAGER_ERROR;
    }
    // LCOV_EXCL_STOP
    auto sin = reinterpret_cast<sockaddr_in *>(&ifr.ifr_addr);
    sin->sin_family = AF_INET;
    sin->sin_addr = ipv4Addr;
    if (ioctl(socketfd, SIOCSIFADDR, &ifr) < 0) {
        NETNATIVE_LOGE("ioctl set ipv4 address failed: %{public}d", errno);
        close(socketfd);
        return NETMANAGER_ERROR;
    }

    if (prefix <= 0 || prefix > V4ADDR_BIT_LEN) {
        NETNATIVE_LOGE("prefix: %{public}d error", prefix);
        close(socketfd);
        return NETMANAGER_ERROR;
    }
    in_addr_t mask = prefix ? (~0 << (V4ADDR_BIT_LEN - prefix)) : 0;
    sin = reinterpret_cast<sockaddr_in *>(&ifr.ifr_netmask);
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = htonl(mask);
    if (ioctl(socketfd, SIOCSIFNETMASK, &ifr) < 0) {
        NETNATIVE_LOGE("ioctl set ip mask failed: %{public}d", errno);
        close(socketfd);
        return NETMANAGER_ERROR;
    }
    close(socketfd);
    return NETMANAGER_SUCCESS;
}

} // namespace nmd
} // namespace OHOS
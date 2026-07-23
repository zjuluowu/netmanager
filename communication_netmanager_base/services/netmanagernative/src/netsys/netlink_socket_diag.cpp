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

#include "netlink_socket_diag.h"

#include <arpa/inet.h>
#include <cstring>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <unistd.h>
#include <linux/rtnetlink.h>

#include "fwmark.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;

namespace {
constexpr uint32_t KERNEL_BUFFER_SIZE = 8192U;
constexpr uint8_t ADDR_POSITION = 3U;
constexpr int32_t DOMAIN_IP_ADDR_MAX_LEN = 128;
constexpr uint32_t LOCKBACK_MASK = 0xff000000;
constexpr uint32_t LOCKBACK_DEFINE = 0x7f000000;
constexpr uid_t PUSH_UID = 7023;
constexpr uint32_t INET_DIAG_REQ_V2_STATES_ALL = 0xffffffff;
constexpr int32_t INVALID_OWNER_UID = -1;
constexpr int32_t IDIAG_ARRAY_LEN = 4;
constexpr int32_t TCP_STATES = (1 << TCP_ESTABLISHED) | (1 << TCP_SYN_SENT) | (1 << TCP_SYN_RECV) |
    (1 << TCP_FIN_WAIT1) | (1 << TCP_FIN_WAIT2) | (1 << TCP_TIME_WAIT) | (1 << TCP_CLOSE) | (1 << TCP_CLOSE_WAIT) |
    (1 << TCP_LAST_ACK) | (1 << TCP_LISTEN) | (1 << TCP_CLOSING);
constexpr int32_t INET_DIAG_SK_PID = 23;
constexpr const char *V4_MAPPED_PREFIX = "::ffff:";
} // namespace

NetLinkSocketDiag::~NetLinkSocketDiag()
{
    CloseNetlinkSocket();
}

std::string NetLinkSocketDiag::StripV4MappedPrefix(const std::string &addr)
{
    if (addr.find(V4_MAPPED_PREFIX) != 0) {
        return addr;
    }
    std::string stripped = addr.substr(strlen(V4_MAPPED_PREFIX));
    if (CommonUtils::IsValidIPV4(stripped)) {
        return stripped;
    }
    return addr;
}

// LCOV_EXCL_START
bool NetLinkSocketDiag::InLookBack(uint32_t a)
{
    return (a & LOCKBACK_MASK) == LOCKBACK_DEFINE;
}
// LCOV_EXCL_STOP

bool NetLinkSocketDiag::CreateNetlinkSocket()
{
    dumpSock_ = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_INET_DIAG);
    if (dumpSock_ < 0) {
        NETNATIVE_LOGE("Create netlink socket for dump failed, error[%{public}d]: %{public}s", errno, strerror(errno));
        return false;
    }

    destroySock_ = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_INET_DIAG);
    if (destroySock_ < 0) {
        NETNATIVE_LOGE("Create netlink socket for destroy failed, error[%{public}d]: %{public}s", errno,
                       strerror(errno));
        close(dumpSock_);
        dumpSock_ = -1;
        return false;
    }

    sockaddr_nl nl = {.nl_family = AF_NETLINK};
    // LCOV_EXCL_START
    if ((connect(dumpSock_, reinterpret_cast<sockaddr *>(&nl), sizeof(nl)) < 0) ||
        (connect(destroySock_, reinterpret_cast<sockaddr *>(&nl), sizeof(nl)) < 0)) {
        NETNATIVE_LOGE("Connect to netlink socket failed, error[%{public}d]: %{public}s", errno, strerror(errno));
        CloseNetlinkSocket();
        return false;
    }
    // LCOV_EXCL_STOP
    return true;
}

void NetLinkSocketDiag::CloseNetlinkSocket()
{
    // avoid double close
    if (dumpSock_ >= 0) {
        close(dumpSock_);
    }
    if (destroySock_ >= 0) {
        close(destroySock_);
    }
    if (queryUidSock_ >= 0) {
        close(queryUidSock_);
    }
    dumpSock_ = -1;
    destroySock_ = -1;
    queryUidSock_ = -1;
}

// LCOV_EXCL_START
int32_t NetLinkSocketDiag::ExecuteDestroySocket(uint8_t proto, const inet_diag_msg *msg)
{
    if (msg == nullptr) {
        NETNATIVE_LOGE("inet_diag_msg is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    SockDiagRequest request;
    request.nlh_.nlmsg_type = SOCK_DESTROY;
    request.nlh_.nlmsg_flags = NLM_F_REQUEST;
    request.nlh_.nlmsg_len = sizeof(request);

    request.req_ = {.sdiag_family = msg->idiag_family,
                    .sdiag_protocol = proto,
                    .idiag_states = static_cast<uint32_t>(1 << msg->idiag_state),
                    .id = msg->id};
    ssize_t writeLen = write(destroySock_, &request, sizeof(request));
    if (writeLen < static_cast<ssize_t>(sizeof(request))) {
        NETNATIVE_LOGE("Write destroy request to socket failed errno[%{public}d]: strerror:%{public}s", errno,
                       strerror(errno));
        return NETMANAGER_ERR_INTERNAL;
    }

    int32_t ret = GetErrorFromKernel(destroySock_);
    if (ret == NETMANAGER_SUCCESS) {
        NETNATIVE_LOGI("destroy socket, sport: %{public}d, uid: %{public}u",
            msg->id.idiag_sport, msg->idiag_uid);
        socketsDestroyed_++;
    }
    return ret;
}
// LCOV_EXCL_STOP

int32_t NetLinkSocketDiag::GetErrorFromKernel(int32_t fd)
{
    int32_t kernelError = 0;
    return GetErrorFromKernel(fd, kernelError);
}

int32_t NetLinkSocketDiag::GetErrorFromKernel(int32_t fd, int32_t &kernelError)
{
    Ack ack;
    ssize_t bytesread = recv(fd, &ack, sizeof(ack), MSG_DONTWAIT | MSG_PEEK);
    if (bytesread < 0) {
        return (errno == EAGAIN) ? NETMANAGER_SUCCESS : -errno;
    }
    if (bytesread == static_cast<ssize_t>(sizeof(ack)) && ack.hdr_.nlmsg_type == NLMSG_ERROR) {
        recv(fd, &ack, sizeof(ack), 0);
        NETNATIVE_LOGE("Receive NLMSG_ERROR:[%{public}d] from kernel", ack.err_.error);
        kernelError = ack.err_.error;
        return NETMANAGER_ERR_INTERNAL;
    }
    return NETMANAGER_SUCCESS;
}

// LCOV_EXCL_START
bool NetLinkSocketDiag::IsLoopbackSocket(const inet_diag_msg *msg)
{
    if (msg->idiag_family == AF_INET) {
        return InLookBack(htonl(msg->id.idiag_src[0])) || InLookBack(htonl(msg->id.idiag_dst[0]));
    }

    if (msg->idiag_family == AF_INET6) {
        const struct in6_addr *src = (const struct in6_addr *)&msg->id.idiag_src;
        const struct in6_addr *dst = (const struct in6_addr *)&msg->id.idiag_dst;
        return (IN6_IS_ADDR_V4MAPPED(src) && InLookBack(src->s6_addr32[ADDR_POSITION])) ||
               (IN6_IS_ADDR_V4MAPPED(dst) && InLookBack(dst->s6_addr32[ADDR_POSITION])) || IN6_IS_ADDR_LOOPBACK(src) ||
               IN6_IS_ADDR_LOOPBACK(dst);
    }
    return false;
}
// LCOV_EXCL_STOP

bool NetLinkSocketDiag::IsMatchNetwork(const inet_diag_msg *msg, const std::string &ipAddr)
{
    if (msg->idiag_family == AF_INET) {
        // LCOV_EXCL_START
        if (CommonUtils::GetAddrFamily(ipAddr) != AF_INET) {
            return false;
        }

        in_addr_t addr = inet_addr(ipAddr.c_str());
        if (addr == msg->id.idiag_src[0] || addr == msg->id.idiag_dst[0]) {
            return true;
        }
        // LCOV_EXCL_STOP
    }

    if (msg->idiag_family == AF_INET6) {
        // deal with V4-mapped V6 address, such as "::ffff:192.168.1.1"
        if (CommonUtils::GetAddrFamily(ipAddr) == AF_INET) {
            in_addr_t addr = inet_addr(ipAddr.c_str());
            uint32_t mapped[4] = { 0, 0, htonl(0xffff), addr };
            bool isV4MappedAddr = true;
            for (int32_t i = 0; i < IDIAG_ARRAY_LEN; i++) {
                isV4MappedAddr = isV4MappedAddr & (mapped[i] == msg->id.idiag_src[i]);
            }
            if (isV4MappedAddr) {
                NETNATIVE_LOGI("destroy socket on V4-mapped V6address, sport: %{public}d, uid: %{public}u",
                    msg->id.idiag_sport, msg->idiag_uid);
                return true;
            }
        }

        if (CommonUtils::GetAddrFamily(ipAddr) != AF_INET6) {
            return false;
        }

        char src[DOMAIN_IP_ADDR_MAX_LEN] = {0};
        char dst[DOMAIN_IP_ADDR_MAX_LEN] = {0};
        inet_ntop(AF_INET6, msg->id.idiag_src, src, sizeof(src));
        inet_ntop(AF_INET6, msg->id.idiag_dst, dst, sizeof(dst));
        // LCOV_EXCL_START
        if (src == ipAddr || dst == ipAddr) {
            return true;
        }
        // LCOV_EXCL_STOP
    }
    return false;
}

int32_t NetLinkSocketDiag::ProcessSockDiagDumpResponse(uint8_t proto, const std::string &ipAddr, bool excludeLoopback)
{
    char buf[KERNEL_BUFFER_SIZE] = {0};
    ssize_t readBytes = read(dumpSock_, buf, sizeof(buf));
    if (readBytes < 0) {
        NETNATIVE_LOGE("Failed to read socket, errno:%{public}d, strerror:%{public}s", errno, strerror(errno));
        return NETMANAGER_ERR_INTERNAL;
    }
    while (readBytes > 0) {
        uint32_t len = static_cast<uint32_t>(readBytes);
        for (nlmsghdr *nlh = reinterpret_cast<nlmsghdr *>(buf); NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            // LCOV_EXCL_START
            if (nlh->nlmsg_type == NLMSG_ERROR) {
                nlmsgerr *err = reinterpret_cast<nlmsgerr *>(NLMSG_DATA(nlh));
                NETNATIVE_LOGE("Error netlink msg, errno:%{public}d, strerror:%{public}s", -err->error,
                               strerror(-err->error));
                return err->error;
            } else if (nlh->nlmsg_type == NLMSG_DONE) {
                return NETMANAGER_SUCCESS;
            } else {
                const auto *msg = reinterpret_cast<inet_diag_msg *>(NLMSG_DATA(nlh));
                SockDiagDumpCallback(proto, msg, ipAddr, excludeLoopback);
            }
            // LCOV_EXCL_STOP
        }
        readBytes = read(dumpSock_, buf, sizeof(buf));
        // LCOV_EXCL_START
        if (readBytes < 0) {
            return -errno;
        }
        // LCOV_EXCL_STOP
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetLinkSocketDiag::SendSockDiagDumpRequest(uint8_t proto, uint8_t family, uint32_t states)
{
    SockDiagRequest request;
    size_t len = sizeof(request);
    iovec iov;
    iov.iov_base = &request;
    iov.iov_len = len;
    request.nlh_.nlmsg_type = SOCK_DIAG_BY_FAMILY;
    request.nlh_.nlmsg_flags = (NLM_F_REQUEST | NLM_F_DUMP);
    request.nlh_.nlmsg_len = len;

    request.req_ = {.sdiag_family = family, .sdiag_protocol = proto, .idiag_states = states};

    ssize_t writeLen = writev(dumpSock_, &iov, (sizeof(iov) / sizeof(iovec)));
    // LCOV_EXCL_START
    if (writeLen != static_cast<ssize_t>(len)) {
        NETNATIVE_LOGE("Write dump request failed errno:%{public}d, strerror:%{public}s", errno, strerror(errno));
        return NETMANAGER_ERR_INTERNAL;
    }
    // LCOV_EXCL_STOP

    return GetErrorFromKernel(dumpSock_);
}

// LCOV_EXCL_START
void NetLinkSocketDiag::SockDiagDumpCallback(uint8_t proto, const inet_diag_msg *msg, const std::string &ipAddr,
                                             bool excludeLoopback)
{
    if (msg == nullptr) {
        NETNATIVE_LOGE("msg is nullptr");
        return;
    }

    if (socketDestroyType_ == SocketDestroyType::DESTROY_SPECIAL_CELLULAR && msg->idiag_uid != PUSH_UID) {
        return;
    }

    if (socketDestroyType_ == SocketDestroyType::DESTROY_DEFAULT_CELLULAR && msg->idiag_uid == PUSH_UID) {
        return;
    }

    if (excludeLoopback && IsLoopbackSocket(msg)) {
        NETNATIVE_LOG_D("Loop back socket, no need to close.");
        return;
    }

    if (!IsMatchNetwork(msg, ipAddr)) {
        NETNATIVE_LOG_D("Socket is not associated with the network");
        return;
    }

    ExecuteDestroySocket(proto, msg);
}
// LCOV_EXCL_STOP

void NetLinkSocketDiag::DestroyLiveSockets(const char *ipAddr, bool excludeLoopback)
{
    NETNATIVE_LOG_D("DestroySocketsLackingNetwork in");
    if (ipAddr == nullptr) {
        NETNATIVE_LOGE("Ip address is nullptr.");
        return;
    }

    // LCOV_EXCL_START
    if (!CreateNetlinkSocket()) {
        NETNATIVE_LOGE("Create netlink diag socket failed.");
        return;
    }
    // LCOV_EXCL_STOP

    const int32_t proto = IPPROTO_TCP;
    const uint32_t states = (1 << TCP_ESTABLISHED) | (1 << TCP_SYN_SENT) | (1 << TCP_SYN_RECV);

    for (const int family : {AF_INET, AF_INET6}) {
        int32_t ret = SendSockDiagDumpRequest(proto, family, states);
        // LCOV_EXCL_START
        if (ret != NETMANAGER_SUCCESS) {
            NETNATIVE_LOGE("Failed to dump %{public}s sockets", family == AF_INET ? "IPv4" : "IPv6");
            break;
        }
        ret = ProcessSockDiagDumpResponse(proto, ipAddr, excludeLoopback);
        if (ret != NETMANAGER_SUCCESS) {
            NETNATIVE_LOGE("Failed to destroy %{public}s sockets", family == AF_INET ? "IPv4" : "IPv6");
            break;
        }
        // LCOV_EXCL_STOP
    }

    NETNATIVE_LOGI("Destroyed %{public}d sockets", socketsDestroyed_);
}

bool NetLinkSocketDiag::GetTcpNetPortStatesInfo(const inet_diag_msg* msg,
    NetPortStatesInfo& netPortStatesInfo, const uint32_t pid)
{
    if (msg == nullptr) {
        return false;
    }
    NetManagerStandard::TcpNetPortStatesInfo tcpInfo;
    char localAddr[DOMAIN_IP_ADDR_MAX_LEN] = {0};
    char remoteAddr[DOMAIN_IP_ADDR_MAX_LEN] = {0};
    if (msg->idiag_family == AF_INET) {
        in_addr aSrc{.s_addr = msg->id.idiag_src[0]};
        in_addr aDst{.s_addr = msg->id.idiag_dst[0]};
        inet_ntop(AF_INET, &aSrc, localAddr, sizeof(localAddr));
        inet_ntop(AF_INET, &aDst, remoteAddr, sizeof(remoteAddr));
        tcpInfo.tcpLocalIp_ = localAddr;
        tcpInfo.tcpRemoteIp_ = remoteAddr;
    } else if (msg->idiag_family == AF_INET6) {
        inet_ntop(AF_INET6, msg->id.idiag_src, localAddr, sizeof(localAddr));
        inet_ntop(AF_INET6, msg->id.idiag_dst, remoteAddr, sizeof(remoteAddr));
        tcpInfo.tcpLocalIp_ = StripV4MappedPrefix(localAddr);
        tcpInfo.tcpRemoteIp_ = StripV4MappedPrefix(remoteAddr);
    }
    if ((!CommonUtils::IsValidIPV4(tcpInfo.tcpLocalIp_) && !CommonUtils::IsValidIPV6(tcpInfo.tcpLocalIp_)) ||
        (!CommonUtils::IsValidIPV4(tcpInfo.tcpRemoteIp_) && !CommonUtils::IsValidIPV6(tcpInfo.tcpRemoteIp_))) {
        return false;
    }
    tcpInfo.tcpLocalPort_ = ntohs(msg->id.idiag_sport);
    tcpInfo.tcpRemotePort_ = ntohs(msg->id.idiag_dport);
    tcpInfo.tcpUid_ = msg->idiag_uid;
    tcpInfo.tcpPid_ = pid;
    tcpInfo.tcpState_ = msg->idiag_state;
    netPortStatesInfo.tcpNetPortStatesInfo_.push_back(tcpInfo);
    return true;
}

bool NetLinkSocketDiag::GetUdpNetPortStatesInfo(const inet_diag_msg* msg,
    NetPortStatesInfo& netPortStatesInfo, const uint32_t pid)
{
    if (msg == nullptr) {
        return false;
    }
    NetManagerStandard::UdpNetPortStatesInfo udpInfo;
    char localAddr[DOMAIN_IP_ADDR_MAX_LEN] = {0};
    if (msg->idiag_family == AF_INET) {
        in_addr aSrc { .s_addr = msg->id.idiag_src[0] };
        inet_ntop(AF_INET, &aSrc, localAddr, sizeof(localAddr));
        udpInfo.udpLocalIp_ = localAddr;
    } else if (msg->idiag_family == AF_INET6) {
        inet_ntop(AF_INET6, msg->id.idiag_src, localAddr, sizeof(localAddr));
        udpInfo.udpLocalIp_ = StripV4MappedPrefix(localAddr);
    }
    if (!CommonUtils::IsValidIPV4(udpInfo.udpLocalIp_) && !CommonUtils::IsValidIPV6(udpInfo.udpLocalIp_)) {
        return false;
    }
    udpInfo.udpLocalPort_ = ntohs(msg->id.idiag_sport);
    udpInfo.udpUid_ = msg->idiag_uid;
    udpInfo.udpPid_ = pid;
    netPortStatesInfo.udpNetPortStatesInfo_.push_back(udpInfo);
    return true;
}

bool NetLinkSocketDiag::ProcessGetNetPortStatesInfo(const uint8_t proto, NetPortStatesInfo& netPortStatesInfo,
    const nlmsghdr *nlh)
{
    inet_diag_msg *msg = reinterpret_cast<inet_diag_msg *>(NLMSG_DATA(nlh));
    uint32_t skPid = 0;
    int32_t attrLen = nlh->nlmsg_len - static_cast<int32_t>(NLMSG_LENGTH(sizeof(*msg)));
    struct rtattr *attr = reinterpret_cast<struct rtattr*>(msg + 1);
    auto rtaret = RTA_OK(attr, attrLen);
    while (rtaret) {
        // LCOV_EXCL_START
        if (attr->rta_type == INET_DIAG_SK_PID) {
            skPid = *reinterpret_cast<uint32_t*>(RTA_DATA(attr));
            break;
        }
        // LCOV_EXCL_STOP
        attr = RTA_NEXT(attr, attrLen);
        rtaret = RTA_OK(attr, attrLen);
    }
    if (proto == IPPROTO_TCP) {
        if (!GetTcpNetPortStatesInfo(msg, netPortStatesInfo, skPid)) {
            return false;
        }
    } else if (proto == IPPROTO_UDP) {
        if (!GetUdpNetPortStatesInfo(msg, netPortStatesInfo, skPid)) {
            return false;
        }
    }
    return true;
}

int32_t NetLinkSocketDiag::ProcessSockDiagDumpInfo(uint8_t proto,
                                                   NetManagerStandard::NetPortStatesInfo &netPortStatesInfo)
{
    char buf[KERNEL_BUFFER_SIZE] = {0};
    ssize_t readBytes = read(dumpSock_, buf, sizeof(buf));
    if (readBytes < 0) {
        NETNATIVE_LOGE("Read netlink dump failed errno:%{public}d %{public}s", errno, strerror(errno));
        return NETMANAGER_ERR_INTERNAL;
    }
    while (readBytes > 0) {
        int len = readBytes;
        for (nlmsghdr *nlh = reinterpret_cast<nlmsghdr *>(buf); NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_type == NLMSG_ERROR) {
                return NETMANAGER_ERR_INTERNAL;
            } else if (nlh->nlmsg_type == NLMSG_DONE) {
                return NETMANAGER_SUCCESS;
            }
            if (nlh->nlmsg_len < NLMSG_LENGTH(sizeof(inet_diag_msg))) {
                continue;
            }
            if (!ProcessGetNetPortStatesInfo(proto, netPortStatesInfo, nlh)) {
                continue;
            }
        }
        readBytes = read(dumpSock_, buf, sizeof(buf));
        if (readBytes < 0) {
            NETNATIVE_LOGE("Read netlink dump failed errno:%{public}d %{public}s", errno, strerror(errno));
            return NETMANAGER_ERR_INTERNAL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetLinkSocketDiag::GetSystemNetPortStates(NetManagerStandard::NetPortStatesInfo &netPortStatesInfo)
{
    if (!CreateNetlinkSocket()) {
        NETNATIVE_LOGE("Create netlink diag socket failed.");
        return NETMANAGER_ERR_INTERNAL;
    }

    for (int32_t proto : {IPPROTO_TCP, IPPROTO_UDP}) {
        for (const int family : {AF_INET, AF_INET6}) {
            uint32_t states = 0;
            if (proto == IPPROTO_TCP) {
                states = TCP_STATES;
            } else if (proto == IPPROTO_UDP) {
                states = -1;
            }
            int32_t ret = SendSockDiagDumpRequest(proto, family, states);
            if (ret != NETMANAGER_SUCCESS) {
                NETNATIVE_LOGE("Failed to dump %{public}s %{public}s sockets", family == AF_INET ? "IPv4" : "IPv6",
                               proto == IPPROTO_TCP ? "TCP" : "UDP");
                continue;
            }
            ret = ProcessSockDiagDumpInfo(proto, netPortStatesInfo);
            if (ret != NETMANAGER_SUCCESS) {
                NETNATIVE_LOGE("Failed to process %{public}s %{public}s sockets", family == AF_INET ? "IPv4" : "IPv6",
                               proto == IPPROTO_TCP ? "TCP" : "UDP");
                continue;
            }
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetLinkSocketDiag::SetSocketDestroyType(int socketType)
{
    if (socketType >= static_cast<int>(SocketDestroyType::DESTROY_DEFAULT)) {
        socketDestroyType_ = SocketDestroyType::DESTROY_DEFAULT;
    } else {
        socketDestroyType_ = static_cast<SocketDestroyType>(socketType);
    }
    return 0;
}

// LCOV_EXCL_START
void NetLinkSocketDiag::SockDiagUidDumpCallback(uint8_t proto, const inet_diag_msg *msg,
    const NetLinkSocketDiag::DestroyFilter& needDestroy)
{
    NETNATIVE_LOG_D(" SockDiagUidDumpCallback");
    if (!needDestroy(msg)) {
        return;
    }

    ExecuteDestroySocket(proto, msg);
}
// LCOV_EXCL_STOP

int32_t NetLinkSocketDiag::ProcessSockDiagUidDumpResponse(uint8_t proto,
    const NetLinkSocketDiag::DestroyFilter& needDestroy)
{
    NETNATIVE_LOG_D("ProcessSockDiagUidDumpResponse");
    char buf[KERNEL_BUFFER_SIZE] = {0};
    ssize_t readBytes = read(dumpSock_, buf, sizeof(buf));
    if (readBytes < 0) {
        return NETMANAGER_ERR_INTERNAL;
    }
    while (readBytes > 0) {
        int len = readBytes;
        for (nlmsghdr *nlh = reinterpret_cast<nlmsghdr *>(buf); NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_type == NLMSG_ERROR) {
                nlmsgerr *err = reinterpret_cast<nlmsgerr *>(NLMSG_DATA(nlh));
                NETNATIVE_LOGE("Error netlink msg, errno:%{public}d, strerror:%{public}s", -err->error,
                    strerror(-err->error));
                return err->error;
            } else if (nlh->nlmsg_type == NLMSG_DONE) {
                NETNATIVE_LOGE("ProcessSockDiagUidDumpResponse nlh->nlmsg_type == NLMSG_DONE");
                return NETMANAGER_SUCCESS;
            } else {
                const auto *msg = reinterpret_cast<inet_diag_msg *>(NLMSG_DATA(nlh));
                SockDiagUidDumpCallback(proto, msg, needDestroy);
            }
        }
        readBytes = read(dumpSock_, buf, sizeof(buf));
        // LCOV_EXCL_START
        if (readBytes < 0) {
            NETNATIVE_LOGE("ProcessSockDiagUidDumpResponse readBytes < 0");
            return -errno;
        }
        // LCOV_EXCL_STOP
    }
    return NETMANAGER_SUCCESS;
}

void NetLinkSocketDiag::DestroyLiveSocketsWithUid(const std::string &ipAddr, uint32_t uid)
{
    NETNATIVE_LOG_D("TCP-RST DestroyLiveSocketsWithUid, uid:%{public}d", uid);
    // LCOV_EXCL_START
    if (!CreateNetlinkSocket()) {
        NETNATIVE_LOGE("Create netlink diag socket failed.");
        return;
    }
    auto needDestroy = [&] (const inet_diag_msg *msg) {
        bool isMatchNetwork = true;
        if (ipAddr != "") {
            isMatchNetwork = IsMatchNetwork(msg, ipAddr);
        }
        return msg != nullptr && uid == msg->idiag_uid && isMatchNetwork && !IsLoopbackSocket(msg);
    };
    const int32_t proto = IPPROTO_TCP;
    const uint32_t states = (1 << TCP_ESTABLISHED) | (1 << TCP_SYN_SENT) | (1 << TCP_SYN_RECV) | (1 << TCP_CLOSE_WAIT)
             | (1 << TCP_FIN_WAIT1) | (1 << TCP_FIN_WAIT2) | (1 << TCP_TIME_WAIT) | (1 << TCP_LAST_ACK);
    for (const int family : {AF_INET, AF_INET6}) {
        int32_t ret = SendSockDiagDumpRequest(proto, family, states);
        if (ret != NETMANAGER_SUCCESS) {
            NETNATIVE_LOGE("Failed to dump %{public}s sockets", family == AF_INET ? "IPv4" : "IPv6");
            break;
        }
        ret = ProcessSockDiagUidDumpResponse(proto, needDestroy);
        if (ret != NETMANAGER_SUCCESS) {
            NETNATIVE_LOGE("Failed to destroy %{public}s sockets", family == AF_INET ? "IPv4" : "IPv6");
            break;
        }
    }
    // LCOV_EXCL_STOP

    NETNATIVE_LOGI("TCP-RST Destroyed %{public}d sockets for uid:%{public}d", socketsDestroyed_, uid);
}

bool NetLinkSocketDiag::CreateNetlinkSocketForQueryUid()
{
    queryUidSock_ = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_INET_DIAG);
    if (queryUidSock_ < 0) {
        NETNATIVE_LOGE("Create netlink socket for query uid failed, error[%{public}d]: %{public}s", errno,
                       strerror(errno));
        return false;
    }

    sockaddr_nl nl = {.nl_family = AF_NETLINK};
    if (connect(queryUidSock_, reinterpret_cast<sockaddr *>(&nl), sizeof(nl)) < 0) {
        NETNATIVE_LOGE("Connect to netlink socket failed, error[%{public}d]: %{public}s", errno, strerror(errno));
        CloseNetlinkSocket();
        return false;
    }
    return true;
}

int32_t NetLinkSocketDiag::MakeQueryUidRequestInfo(uint8_t proto, uint8_t family, const std::string &localAddress,
                                                   uint32_t localPort, const std::string &remoteAddress,
                                                   uint32_t remotePort, inet_diag_req_v2 &request)
{
    memset_s(&request, sizeof(inet_diag_req_v2), 0, sizeof(inet_diag_req_v2));
    request = {
        .sdiag_family = family,
        .sdiag_protocol = proto,
        .idiag_ext = 1,
        .pad = 0,
        .idiag_states = INET_DIAG_REQ_V2_STATES_ALL,
        .id = {
            .idiag_sport = htons(localPort),
            .idiag_dport = htons(remotePort),
            .idiag_if = 0,
            .idiag_cookie = {INET_DIAG_NOCOOKIE, INET_DIAG_NOCOOKIE},
        }
    };
    if (family == AF_INET) {
        if ((inet_pton(AF_INET, localAddress.c_str(), &request.id.idiag_src[0]) != 1) ||
            (inet_pton(AF_INET, remoteAddress.c_str(), &request.id.idiag_dst[0]) != 1)) {
            NETNATIVE_LOGE("Convert IP address failed.");
            return NETMANAGER_ERR_INTERNAL;
        }
    } else {
        if ((inet_pton(AF_INET6, localAddress.c_str(), request.id.idiag_src) != 1) ||
            (inet_pton(AF_INET6, remoteAddress.c_str(), request.id.idiag_dst) != 1)) {
            NETNATIVE_LOGE("Convert IP address failed.");
            return NETMANAGER_ERR_INTERNAL;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetLinkSocketDiag::ProcessQueryUidResponse(int32_t &uid)
{
    uid = INVALID_OWNER_UID;
    char buf[KERNEL_BUFFER_SIZE] = {0};
    ssize_t readBytes = read(queryUidSock_, buf, sizeof(buf));
    if (readBytes < 0) {
        NETNATIVE_LOGE("Failed to read socket, errno:%{public}d, strerror:%{public}s", errno, strerror(errno));
        return NETMANAGER_ERR_INTERNAL;
    }

    // if readBytes is 0, it is equivalent to no matching uid being found.
    while (readBytes > 0) {
        int len = readBytes;
        for (nlmsghdr *nlh = reinterpret_cast<nlmsghdr *>(buf); NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            if (nlh->nlmsg_type == NLMSG_ERROR) {
                nlmsgerr *err = reinterpret_cast<nlmsgerr *>(NLMSG_DATA(nlh));
                NETNATIVE_LOGE("Error netlink msg, errno:%{public}d, strerror:%{public}s", -err->error,
                               strerror(-err->error));
                return NETMANAGER_ERR_INTERNAL;
            } else if (nlh->nlmsg_type == NLMSG_DONE) {
                NETNATIVE_LOGE("ProcessQueryUidResponse nlh->nlmsg_type == NLMSG_DONE");
                return NETMANAGER_SUCCESS;
            } else {
                const auto *msg = reinterpret_cast<inet_diag_msg *>(NLMSG_DATA(nlh));
                uid = static_cast<int32_t>(msg->idiag_uid);
                return NETMANAGER_SUCCESS;
            }
        }
        readBytes = read(queryUidSock_, buf, sizeof(buf));
        if (readBytes < 0) {
            NETNATIVE_LOGE("Failed to read socket, errno:%{public}d, strerror:%{public}s", errno, strerror(errno));
            return NETMANAGER_ERR_INTERNAL;
        }
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetLinkSocketDiag::GetConnectOwnerUid(uint8_t proto, uint8_t family, const std::string &localAddress,
                                              uint32_t localPort, const std::string &remoteAddress, uint32_t remotePort,
                                              int32_t &uid)
{
    uid = INVALID_OWNER_UID;
    if (!CreateNetlinkSocketForQueryUid()) {
        return NETMANAGER_ERR_INTERNAL;
    }

    int32_t ret = QueryConnectOwnerUid(proto, family, localAddress, localPort, remoteAddress, remotePort, uid);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    /**
     * If five-tuple match fails, retry with local address + port only.
     * Reason: Unconnected UDP sockets don't store remote endpoint info.
     */
    if (uid == INVALID_OWNER_UID && proto == IPPROTO_UDP) {
        std::string wildcardIp = (family == AF_INET) ? "0.0.0.0" : "::";
        ret = QueryConnectOwnerUid(proto, family, localAddress, localPort, wildcardIp, 0, uid);
    }

    return ret;
}

int32_t NetLinkSocketDiag::QueryConnectOwnerUid(uint8_t proto, uint8_t family, const std::string &localAddress,
                                                uint32_t localPort, const std::string &remoteAddress,
                                                uint32_t remotePort, int32_t &uid)
{
    SockDiagRequest request;
    memset_s(&request, sizeof(SockDiagRequest), 0, sizeof(SockDiagRequest));
    size_t requestLen = sizeof(request);
    iovec iov;
    iov.iov_base = &request;
    iov.iov_len = requestLen;
    request.nlh_.nlmsg_type = SOCK_DIAG_BY_FAMILY;
    request.nlh_.nlmsg_flags = (NLM_F_REQUEST | NLM_F_DUMP);
    request.nlh_.nlmsg_len = requestLen;

    int32_t ret =
        MakeQueryUidRequestInfo(proto, family, localAddress, localPort, remoteAddress, remotePort, request.req_);
    // LCOV_EXCL_START
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("Failed to query uid err = %{public}d", ret);
        return NETMANAGER_ERR_INTERNAL;
    }

    ssize_t writeLen = writev(queryUidSock_, &iov, (sizeof(iov) / sizeof(iovec)));
    if (writeLen != static_cast<ssize_t>(requestLen)) {
        NETNATIVE_LOGE("Write dump request failed errno:%{public}d, strerror:%{public}s", errno, strerror(errno));
        return NETMANAGER_ERR_INTERNAL;
    }

    int32_t kernelError = 0;
    ret = GetErrorFromKernel(queryUidSock_, kernelError);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("GetErrorFromKernel failed ret = %{public}d, kernelError = %{public}d", ret, kernelError);
        return (kernelError == -ENOENT) ? NETMANAGER_SUCCESS : NETMANAGER_ERR_INTERNAL;
    }
    // LCOV_EXCL_STOP

    ret = ProcessQueryUidResponse(uid);
    return ret;
}
} // namespace nmd
} // namespace OHOS
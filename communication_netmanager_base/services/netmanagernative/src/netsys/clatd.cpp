/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "clatd.h"

#include <arpa/inet.h>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/icmp6.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <poll.h>
#include <string>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "clat_constants.h"
#include "clat_utils.h"
#include "clatd_packet_converter.h"
#include "ffrt.h"
#include "ffrt_inner.h"
#include "ffrt_timer.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;
Clatd::Clatd(int tunFd, int readSock6, int writeSock6, const std::string &v6Iface, const std::string &prefixAddrStr,
             const std::string &v4AddrStr, const std::string &v6AddrStr)
    : tunFd_(tunFd), readSock6_(readSock6), writeSock6_(writeSock6), v6Iface_(v6Iface)
{
    stopFd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    tunIface_ = std::string(CLAT_PREFIX) + v6Iface;
    inet_pton(AF_INET6, v6AddrStr.c_str(), &v6Addr_);
    inet_pton(AF_INET, v4AddrStr.c_str(), &v4Addr_.s_addr);
    inet_pton(AF_INET6, prefixAddrStr.c_str(), &prefixAddr_);
    isSocketClosed_ = false;
    stopStatus_ = true;
}

Clatd::~Clatd()
{
    close(stopFd_);
}

// LCOV_EXCL_START
void Clatd::Start()
{
    bool expectedStop = true;
    if (!stopStatus_.compare_exchange_strong(expectedStop, false)) {
        NETNATIVE_LOGW("fail to start clatd, clatd for %{public}s is already running", v6Iface_.c_str());
        return;
    }
    SendDadPacket();
    std::thread([this]() { RunLoop(); }).detach();
}

void Clatd::Stop()
{
    if (stopStatus_) {
        NETNATIVE_LOGW("fail to stop clatd, clatd for %{public}s is not running", v6Iface_.c_str());
        return;
    }
    uint64_t one = 1;
    write(stopFd_, &one, sizeof(one));

    std::unique_lock<ffrt::mutex> lck(mutex_);
    cv_.wait(lck, [this] { return stopStatus_ == true; });
};

void Clatd::SendDadPacket()
{
    ClatdDadPacket dadPacket;

    dadPacket.v6Header.ip6_vfc = IPV6_VERSION_FLAG;
    dadPacket.v6Header.ip6_plen = htons(sizeof(ClatdDadPacket) - sizeof(ip6_hdr));
    dadPacket.v6Header.ip6_nxt = IPPROTO_ICMPV6;
    dadPacket.v6Header.ip6_hlim = 0xff;
    inet_pton(AF_INET6, "::", &dadPacket.v6Header.ip6_src);
    inet_pton(AF_INET6, SOLICITED_NODE_PREFIX, &dadPacket.v6Header.ip6_dst);
    size_t v6AddrByteLen = V6ADDR_BIT_LEN / CHAR_BIT;
    for (size_t i = SOLICITED_NODE_SUFFIX_OFFSET; i < v6AddrByteLen; i++) {
        dadPacket.v6Header.ip6_dst.s6_addr[i] = v6Addr_.s6_addr[i];
    }

    dadPacket.ns.nd_ns_type = ND_NEIGHBOR_SOLICIT;
    dadPacket.ns.nd_ns_code = 0;
    dadPacket.ns.nd_ns_reserved = 0;
    dadPacket.ns.nd_ns_target = v6Addr_;
    uint32_t checkSum = dadPacket.v6Header.ip6_plen + htons(dadPacket.v6Header.ip6_nxt);
    checkSum = AddChecksum(checkSum, &dadPacket.v6Header.ip6_src, sizeof(dadPacket) - IPV6_SRC_OFFSET);
    dadPacket.ns.nd_ns_cksum = ~Checksum32To16(checkSum);

    dadPacket.nonceOptType = NDP_NOUNCE_OPT;
    dadPacket.nonceOptLen = 1;
    arc4random_buf(&dadPacket.nonce, sizeof(dadPacket.nonce));

    sockaddr_in6 dstAddr;
    dstAddr.sin6_family = AF_INET6;
    dstAddr.sin6_addr = dadPacket.v6Header.ip6_dst;
    dstAddr.sin6_scope_id = if_nametoindex(v6Iface_.c_str());

    sendto(writeSock6_, &dadPacket, sizeof(dadPacket), 0, reinterpret_cast<const sockaddr *>(&dstAddr),
           sizeof(dstAddr));
}

void Clatd::RunLoop()
{
    pollfd fds[] = {
        {stopFd_, POLLIN, 0},
        {readSock6_, POLLIN, 0},
        {tunFd_, POLLIN, 0},
    };
    enum clatdFds {
        EVENT_STOP,
        READ_V6,
        READ_V4,
    };
    FfrtTimer timerClatdRunning;
    timerClatdRunning.Start(CLATD_TIMER_CYCLE_MS, []() { NETNATIVE_LOGI("Clatd is running loop"); });
    while (!isSocketClosed_) {
        if (poll(fds, sizeof(fds) / sizeof((fds)[0]), -1) == -1) {
            if (errno != EINTR) {
                NETNATIVE_LOGW("event_loop/poll returned an error, errno: %{public}d", errno);
            }
        } else {
            if (fds[EVENT_STOP].revents) {
                uint64_t one = 1;
                read(stopFd_, &one, sizeof one);
                std::unique_lock<ffrt::mutex> lck(mutex_);
                stopStatus_ = true;
                cv_.notify_one();
                break;
            }
            if (fds[READ_V6].revents) {
                ProcessV6Packet();
            }
            if (fds[READ_V4].revents) {
                ProcessV4Packet();
            }
        }
    }
    timerClatdRunning.Stop();
}

int32_t Clatd::MaybeCalculateL4Checksum(int packetLen, ClatdReadV6Buf &readBuf)
{
    const int csumStart = readBuf.vnet.csumStart;
    const int csumOffset = csumStart + readBuf.vnet.csumOffset;
    if (csumOffset > packetLen - 1) {
        NETNATIVE_LOGW("csum offset %{public}d larger than packet length %{public}d", csumOffset, packetLen);
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    uint16_t csum = CalChecksum(readBuf.payload, packetLen); // L4 checksum calculation required
    if (csum == 0) {
        csum = 0xFFFF;
    }
    readBuf.payload[csumOffset] = csum & 0xFF;
    readBuf.payload[csumOffset + 1] = csum >> CHAR_BIT;
    return NETMANAGER_SUCCESS;
}

void Clatd::ProcessV6Packet()
{
    ClatdReadV6Buf readBuf;
    iovec iov;
    iov.iov_base = &readBuf;
    iov.iov_len = sizeof(readBuf);

    char cmsgBuf[CMSG_SPACE(sizeof(tpacket_auxdata))];
    msghdr msgHdr;
    msgHdr.msg_iov = &iov;
    msgHdr.msg_iovlen = 1;
    msgHdr.msg_control = cmsgBuf;
    msgHdr.msg_controllen = sizeof(cmsgBuf);

    ssize_t readLen;
    if (ReadV6Packet(msgHdr, readLen) != NETMANAGER_SUCCESS) {
        return;
    }

    uint32_t tpStatus = 0;
    uint16_t tpNet = 0;
    for (cmsghdr *cmsgHdr = CMSG_FIRSTHDR(&msgHdr); cmsgHdr != NULL; cmsgHdr = CMSG_NXTHDR(&msgHdr, cmsgHdr)) {
        if (cmsgHdr->cmsg_level == SOL_PACKET && cmsgHdr->cmsg_type == PACKET_AUXDATA) {
            tpacket_auxdata *auxData = reinterpret_cast<tpacket_auxdata *>(CMSG_DATA(cmsgHdr));
            tpStatus = auxData->tp_status;
            tpNet = auxData->tp_net;
            break;
        }
    }

    if (static_cast<size_t>(readLen) < offsetof(ClatdReadV6Buf, payload) + tpNet) {
        NETNATIVE_LOGW("%{public}zd read packet len shorter than %{public}u L2 header", readLen, tpNet);
        return;
    }

    int packetLen = readLen - offsetof(ClatdReadV6Buf, payload);
    bool skip_csum = false;
    if ((tpStatus & TP_STATUS_CSUMNOTREADY) || (tpStatus & TP_STATUS_CSUM_VALID)) {
        NETNATIVE_LOGW("skip csum for packet which length is %{public}zd", readLen);
        skip_csum = true;
    }
    if (tpNet >= CLAT_DATA_LINK_HDR_LEN + CLAT_MAX_MTU) {
        return;
    }
    ClatdPacketConverter converter = ClatdPacketConverter(readBuf.payload + tpNet, packetLen - tpNet,
                                                          CONVERT_FROM_V6_TO_V4, v4Addr_, v6Addr_, prefixAddr_);
    if (converter.ConvertPacket(skip_csum) != NETMANAGER_SUCCESS) {
        return;
    }
    std::vector<iovec> iovPackets(CLATD_MAX);
    int effectivePos = 0;
    converter.GetConvertedPacket(iovPackets, effectivePos);
    if (effectivePos > 0) {
        writev(tunFd_, &iovPackets[0], effectivePos);
    }
}

void Clatd::ProcessV4Packet()
{
    ClatdReadTunBuf readBuf;
    ssize_t readLen;
    if (ReadV4Packet(readBuf, readLen) != NETMANAGER_SUCCESS) {
        return;
    }

    const int payloadOffset = offsetof(ClatdReadTunBuf, payload);
    if (readLen < payloadOffset) {
        NETNATIVE_LOGW("%{public}zd read packet len shorter than %{public}d payload offset", readLen, payloadOffset);
        return;
    }

    const int packetLen = readLen - payloadOffset;

    uint16_t tunProtocol = ntohs(readBuf.tunProtocolInfo.proto);
    if (tunProtocol != ETH_P_IP) {
        NETNATIVE_LOGW("unknown packet type = 0x%{public}x", tunProtocol);
        return;
    }

    if (readBuf.tunProtocolInfo.flags != 0) {
        NETNATIVE_LOGW("unexpected flags = %{public}d", readBuf.tunProtocolInfo.flags);
    }

    ClatdPacketConverter converter =
        ClatdPacketConverter(readBuf.payload, packetLen, CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    bool skip_csum = false;
    if (converter.ConvertPacket(skip_csum) != NETMANAGER_SUCCESS) {
        return;
    }
    std::vector<iovec> iovPackets(CLATD_MAX);
    int effectivePos = 0;
    converter.GetConvertedPacket(iovPackets, effectivePos);
    if (effectivePos > 0) {
        SendV6OnRawSocket(writeSock6_, iovPackets, effectivePos);
    }
}

int32_t Clatd::ReadV6Packet(msghdr &msgHdr, ssize_t &readLen)
{
    readLen = recvmsg(readSock6_, &msgHdr, 0);
    if (readLen < 0) {
        if (errno != EAGAIN) {
            NETNATIVE_LOGW("recvmsg failed: %{public}s", strerror(errno));
        }
        return NETMANAGER_ERR_OPERATION_FAILED;
    } else if (readLen == 0) {
        NETNATIVE_LOGW("recvmsg failed: socket closed");
        isSocketClosed_ = true;
        return NETMANAGER_ERR_OPERATION_FAILED;
    } else if (static_cast<size_t>(readLen) >= sizeof(ClatdReadV6Buf)) {
        NETNATIVE_LOGW("recvmsg failed: packet oversize, readLen: %{public}zu, sizeof(ClatdReadV6Buf): %{public}zu",
            static_cast<size_t>(readLen), sizeof(ClatdReadV6Buf));
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}

int32_t Clatd::ReadV4Packet(ClatdReadTunBuf &readBuf, ssize_t &readLen)
{
    readLen = read(tunFd_, &readBuf, sizeof(readBuf));
    if (readLen < 0) {
        NETNATIVE_LOGW("read failed: %{public}s", strerror(errno));
        return NETMANAGER_ERR_OPERATION_FAILED;
    } else if (readLen == 0) {
        NETNATIVE_LOGW("read failed: socket closed");
        isSocketClosed_ = true;
        return NETMANAGER_ERR_OPERATION_FAILED;
    } else if (static_cast<size_t>(readLen) >= sizeof(readBuf)) {
        NETNATIVE_LOGW("read failed: packet oversize");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}

void Clatd::SendV6OnRawSocket(int fd, std::vector<iovec> &iovPackets, int effectivePos)
{
    if (effectivePos <= static_cast<int>(CLATD_TPHDR)) {
        NETNATIVE_LOGW("SendV6OnRawSocket: effectivePos %{public}d is invalid", effectivePos);
        return;
    }

    sockaddr_in6 sin6 = {AF_INET6, 0, 0, {{{0, 0, 0, 0}}}, 0};
    msghdr msgHeader;
    msgHeader.msg_name = &sin6;
    msgHeader.msg_namelen = sizeof(sin6);

    msgHeader.msg_iov = &iovPackets[0];
    msgHeader.msg_iovlen = effectivePos;
    sin6.sin6_addr = reinterpret_cast<struct ip6_hdr *>(iovPackets[CLATD_TPHDR].iov_base)->ip6_dst;
    
    ssize_t sendLen = sendmsg(fd, &msgHeader, 0);
    if (sendLen < 0) {
        NETNATIVE_LOGW("SendV6OnRawSocket: sendmsg failed: %{public}s", strerror(errno));
    }
}
// LCOV_EXCL_STOP
} // namespace nmd
} // namespace OHOS
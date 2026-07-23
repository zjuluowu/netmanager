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
#ifndef NETSYS_CLATD_PACKET_CONVERTER_H
#define NETSYS_CLATD_PACKET_CONVERTER_H

#include <linux/if_tun.h>
#include <netinet/icmp6.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <string>
#include <sys/uio.h>
#include <vector>

#include "clat_constants.h"
#include "clat_utils.h"
#include "inet_addr.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;
class ClatdPacketConverter {
public:
    ClatdPacketConverter(const uint8_t *inputPacket, size_t inputPacketSize, ClatdConvertType convertType,
                         const in_addr &v4Addr, const in6_addr &v6Addr, const in6_addr &prefixAddr);

    int32_t ConvertPacket(bool skip_csum);

    void GetConvertedPacket(std::vector<iovec> &iovPackets, int &effectivePos);

private:
    int32_t ConvertV4Packet(int pos, const uint8_t *inputPacket, size_t inputPacketSize);
    int32_t ConvertV4TpPacket(int pos, const iphdr *ipHeader, ip6_hdr *ip6Header, size_t tpLen, uint8_t v6TpProtocol);
    int32_t ConvertV6Packet(int pos, const uint8_t *inputPacket, size_t inputPacketSize);
    int32_t ConvertV6TpPacket(int pos, const ip6_hdr *ip6Header, iphdr *ipHeader, size_t tpLen, uint8_t v4TpProtocol);
    bool IsV4PacketValid(const iphdr *ipHeader, size_t packetSize);
    bool IsV6PacketValid(const ip6_hdr *ip6Header, size_t packetSize);
    void WriteIpv6Header(ip6_hdr *ip6Header, uint8_t tpProtocol, const iphdr *ipHeader);
    void WriteIpv4Header(iphdr *ipHeader, uint8_t tpProtocol, const ip6_hdr *ip6Header);
    void ConvertV4Address(uint32_t v4Addr, in6_addr &v6Addr);
    void ConvertV6Address(const in6_addr &v6Addr, uint32_t &v4Addr);
    size_t WriteFragHeader(ip6_frag *ip6FragHeader, ip6_hdr *ip6Header, const iphdr *ipHeader);
    void ProcessFragHeader(const ip6_frag *ip6FragHeader, iphdr *ipHeader, uint8_t &v6TpProtocol,
                           uint8_t &v4TpProtocol);
    uint32_t CalV4PseudoHeaderChecksum(const iphdr *ipHeader, uint16_t tpLen, uint8_t tpProtocol);
    uint32_t CalV6PseudoHeaderChecksum(const ip6_hdr *ip6Header, uint32_t tpLen, uint8_t tpProtocol);
    uint16_t GetIovPacketLength(int pos);
    int32_t ConvertIcmpPacket(int pos, const icmphdr *icmpHeader, uint32_t checksum, size_t tpLen);
    int32_t ConvertIcmpv6Packet(int pos, const icmp6_hdr *icmp6Header, size_t tpLen);
    void ConvertIcmpTypeAndCode(const uint8_t &icmpType, const uint8_t &icmpCode, uint8_t &icmp6Type,
                                uint8_t &icmp6Code);
    void ConvertIcmpV6TypeAndCode(const uint8_t &icmp6Type, const uint8_t &icmp6Code, uint8_t &icmpType,
                                  uint8_t &icmpCode);
    uint16_t CalIovPacketChecksum(uint32_t sum, int pos);
    int32_t ConvertTcpPacket(int pos, const tcphdr *tcpHeader, uint32_t oldChecksum, uint32_t newChecksum,
                             size_t tpLen);
    bool IsTcpPacketValid(const tcphdr *packet, size_t packetSize);
    int32_t ConvertUdpPacket(int pos, const udphdr *udpHeader, uint32_t oldChecksum, uint32_t newChecksum,
                             size_t tpLen);
    void WritePayload(int pos, const uint8_t *tpHeader, size_t tpLen);
    void WriteTunHeader(bool skip_csum);

    const uint8_t *inputPacket_;
    size_t inputPacketSize_;
    ClatdConvertType convertType_;
    in_addr localV4Addr_;
    in6_addr localV6Addr_;
    in6_addr prefixAddr_;

    std::vector<std::string> iovBufs_;
    std::vector<size_t> iovBufLens_;
    int effectivePos_{0};
};
} // namespace nmd
} // namespace OHOS
#endif
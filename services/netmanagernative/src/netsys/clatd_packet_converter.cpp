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
#include "clatd_packet_converter.h"

#include <algorithm>
#include <climits>
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
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;

ClatdPacketConverter::ClatdPacketConverter(const uint8_t *inputPacket, size_t inputPacketSize,
                                           ClatdConvertType convertType, const in_addr &v4Addr, const in6_addr &v6Addr,
                                           const in6_addr &prefixAddr)
    : inputPacket_(inputPacket),
      inputPacketSize_(inputPacketSize),
      convertType_(convertType),
      localV4Addr_(v4Addr),
      localV6Addr_(v6Addr),
      prefixAddr_(prefixAddr),
      iovBufs_(CLATD_MAX),
      iovBufLens_(CLATD_MAX)
{
}

int32_t ClatdPacketConverter::ConvertPacket(bool skip_csum)
{
    int32_t ret;
    if (convertType_ == CONVERT_FROM_V4_TO_V6) {
        ret = ConvertV4Packet(CLATD_IPHDR, inputPacket_, inputPacketSize_);
        if (ret != NETMANAGER_SUCCESS) {
            NETNATIVE_LOGW("fail to convert ipv4 packet");
        }
    } else if (convertType_ == CONVERT_FROM_V6_TO_V4) {
        ret = ConvertV6Packet(CLATD_IPHDR, inputPacket_, inputPacketSize_);
        if (ret != NETMANAGER_SUCCESS) {
            NETNATIVE_LOGW("fail to convert ipv6 packet");
        } else if (effectivePos_ > 0) {
            WriteTunHeader(skip_csum);
        }
    } else {
        NETNATIVE_LOGW("invalid convert type");
        ret = NETMANAGER_ERR_INVALID_PARAMETER;
    }
    return ret;
}

void ClatdPacketConverter::GetConvertedPacket(std::vector<iovec> &iovPackets, int &effectivePos)
{
    for (size_t i = CLATD_TUNHDR; i < CLATD_MAX; i++) {
        iovPackets[i].iov_base = iovBufs_[i].data();
        iovPackets[i].iov_len = iovBufLens_[i];
    }
    effectivePos = effectivePos_;
}

int32_t ClatdPacketConverter::ConvertV4Packet(int pos, const uint8_t *inputPacket, size_t inputPacketSize)
{
    const iphdr *ipHeader = reinterpret_cast<const iphdr *>(inputPacket);
    if (!IsV4PacketValid(ipHeader, inputPacketSize)) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    // details about how to convert ip/icmp in RFC 6145
    uint8_t v4TpProtocol = ipHeader->protocol;
    uint8_t v6TpProtocol = v4TpProtocol;
    if (v4TpProtocol == IPPROTO_ICMP) {
        v6TpProtocol = IPPROTO_ICMPV6;
    }

    ip6_hdr ip6Header;
    WriteIpv6Header(&ip6Header, v6TpProtocol, ipHeader);
    iovBufLens_[pos] = sizeof(ip6_hdr);

    ip6_frag ip6FragHeader;
    size_t ip6FragHeaderLen = WriteFragHeader(&ip6FragHeader, &ip6Header, ipHeader);
    iovBufLens_[pos + 1] = ip6FragHeaderLen;
    iovBufs_[pos + 1].assign(reinterpret_cast<const char *>(&ip6FragHeader), iovBufLens_[pos + 1]);

    size_t tpLen = inputPacketSize - ipHeader->ihl * WORD_32BIT_IN_BYTE_UNIT;
    const uint8_t *tpHeader = inputPacket + ipHeader->ihl * WORD_32BIT_IN_BYTE_UNIT;
    if (ip6FragHeaderLen > 0 && (ip6FragHeader.ip6f_offlg & IP6F_OFF_MASK)) {
        WritePayload(pos, tpHeader, tpLen);
        ip6Header.ip6_plen = htons(GetIovPacketLength(pos));
        iovBufs_[pos].assign(reinterpret_cast<const char *>(&ip6Header), iovBufLens_[pos]);
        return NETMANAGER_SUCCESS;
    }
    return ConvertV4TpPacket(pos, ipHeader, &ip6Header, tpLen, v6TpProtocol);
}

int32_t ClatdPacketConverter::ConvertV4TpPacket(int pos, const iphdr *ipHeader, ip6_hdr *ip6Header, size_t tpLen,
                                                uint8_t v6TpProtocol)
{
    uint8_t v4TpProtocol = ipHeader->protocol;
    uint32_t oldChecksum = CalV4PseudoHeaderChecksum(ipHeader, tpLen, v4TpProtocol);
    uint32_t newChecksum = CalV6PseudoHeaderChecksum(ip6Header, tpLen, v6TpProtocol);
    const uint8_t *tpHeader = reinterpret_cast<const uint8_t *>(ipHeader) + ipHeader->ihl * WORD_32BIT_IN_BYTE_UNIT;
    int32_t ret;
    switch (v6TpProtocol) {
        case IPPROTO_ICMPV6:
            ret = ConvertIcmpPacket(pos + IP_TP_PACKET_POSITION_DELTA, reinterpret_cast<const icmphdr *>(tpHeader),
                                    newChecksum, tpLen);
            break;
        case IPPROTO_TCP:
            ret = ConvertTcpPacket(pos + IP_TP_PACKET_POSITION_DELTA, reinterpret_cast<const tcphdr *>(tpHeader),
                                   oldChecksum, newChecksum, tpLen);
            break;
        case IPPROTO_UDP:
            ret = ConvertUdpPacket(pos + IP_TP_PACKET_POSITION_DELTA, reinterpret_cast<const udphdr *>(tpHeader),
                                   oldChecksum, newChecksum, tpLen);
            break;
        case IPPROTO_GRE:
        case IPPROTO_ESP:
            WritePayload(pos, tpHeader, tpLen);
            ret = NETMANAGER_SUCCESS;
            break;
        default:
            NETNATIVE_LOGW("unknown transport protocol");
            ret = NETMANAGER_ERR_INVALID_PARAMETER;
    }
    ip6Header->ip6_plen = htons(GetIovPacketLength(pos));
    iovBufs_[pos].assign(reinterpret_cast<const char *>(ip6Header), iovBufLens_[pos]);
    return ret;
}

int32_t ClatdPacketConverter::ConvertV6Packet(int pos, const uint8_t *inputPacket, size_t inputPacketSize)
{
    const ip6_hdr *ip6Header = reinterpret_cast<const ip6_hdr *>(inputPacket);
    if (!IsV6PacketValid(ip6Header, inputPacketSize)) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    // details about how to convert ip/icmp in RFC 6145
    uint8_t v6TpProtocol = ip6Header->ip6_nxt;
    uint8_t v4TpProtocol = v6TpProtocol == IPPROTO_ICMPV6 ? IPPROTO_ICMP : v6TpProtocol;

    iphdr ipHeader;
    WriteIpv4Header(&ipHeader, v4TpProtocol, ip6Header);
    iovBufLens_[pos] = sizeof(iphdr);

    size_t tpLen = inputPacketSize - sizeof(ip6_hdr);
    const uint8_t *tpHeader = inputPacket + sizeof(ip6_hdr);

    const ip6_frag *ip6FragHeader = nullptr;
    if (v6TpProtocol == IPPROTO_FRAGMENT) {
        ip6FragHeader = reinterpret_cast<const ip6_frag *>(tpHeader);
        // LCOV_EXCL_START
        if (tpLen < sizeof(*ip6FragHeader)) {
            NETNATIVE_LOGW("fail to convert ipv6 packet, fragment packet size is too small");
            effectivePos_ = 0;
            return NETMANAGER_ERR_INVALID_PARAMETER;
        }
        // LCOV_EXCL_STOP
        tpHeader += sizeof(*ip6FragHeader);
        tpLen -= sizeof(*ip6FragHeader);

        ProcessFragHeader(ip6FragHeader, &ipHeader, v6TpProtocol, v4TpProtocol);
    }

    if (ip6FragHeader != nullptr && (ip6FragHeader->ip6f_offlg & IP6F_OFF_MASK)) {
        WritePayload(pos, tpHeader, tpLen);
        ipHeader.tot_len = htons(ntohs(ipHeader.tot_len) + GetIovPacketLength(pos));
        ipHeader.check = CalChecksum(&ipHeader, sizeof(iphdr));
        iovBufs_[pos].assign(reinterpret_cast<const char *>(&ipHeader), iovBufLens_[pos]);
        return NETMANAGER_SUCCESS;
    }

    return ConvertV6TpPacket(pos, ip6Header, &ipHeader, tpLen, v4TpProtocol);
}

int32_t ClatdPacketConverter::ConvertV6TpPacket(int pos, const ip6_hdr *ip6Header, iphdr *ipHeader, size_t tpLen,
                                                uint8_t v4TpProtocol)
{
    uint8_t v6TpProtocol = ip6Header->ip6_nxt;
    uint32_t oldChecksum = CalV6PseudoHeaderChecksum(ip6Header, tpLen, v6TpProtocol);
    uint32_t newChecksum = CalV4PseudoHeaderChecksum(ipHeader, tpLen, v4TpProtocol);
    const uint8_t *tpHeader = reinterpret_cast<const uint8_t *>(ip6Header) + sizeof(ip6_hdr);
    int32_t ret;
    switch (v4TpProtocol) {
        case IPPROTO_ICMP:
            ret = ConvertIcmpv6Packet(pos + IP_TP_PACKET_POSITION_DELTA, reinterpret_cast<const icmp6_hdr *>(tpHeader),
                                      tpLen);
            break;
        case IPPROTO_TCP:
            ret = ConvertTcpPacket(pos + IP_TP_PACKET_POSITION_DELTA, reinterpret_cast<const tcphdr *>(tpHeader),
                                   oldChecksum, newChecksum, tpLen);
            break;
        case IPPROTO_UDP:
            ret = ConvertUdpPacket(pos + IP_TP_PACKET_POSITION_DELTA, reinterpret_cast<const udphdr *>(tpHeader),
                                   oldChecksum, newChecksum, tpLen);
            break;
        case IPPROTO_GRE:
        case IPPROTO_ESP:
            WritePayload(pos, tpHeader, tpLen);
            ret = NETMANAGER_SUCCESS;
            break;
        default:
            NETNATIVE_LOGW("unknown transport protocol");
            ret = NETMANAGER_ERR_INVALID_PARAMETER;
    }
    ipHeader->tot_len = htons(ntohs(ipHeader->tot_len) + GetIovPacketLength(pos));
    ipHeader->check = CalChecksum(ipHeader, sizeof(iphdr));
    iovBufs_[pos].assign(reinterpret_cast<const char *>(ipHeader), iovBufLens_[pos]);
    return ret;
}

bool ClatdPacketConverter::IsV4PacketValid(const iphdr *ipHeader, size_t packetSize)
{
    if (packetSize < sizeof(iphdr)) {
        NETNATIVE_LOGW("Invalid ipv4 packet, input packet size too small");
        return false;
    }
    if (ipHeader->ihl * WORD_32BIT_IN_BYTE_UNIT < IPV4_HDR_MIN_LEN) {
        NETNATIVE_LOGW("Invalid ipv4 packet, ip header length %{public}u smaller than 5", ipHeader->ihl);
        return false;
    }
    if (static_cast<size_t>(ipHeader->ihl * WORD_32BIT_IN_BYTE_UNIT) > packetSize) {
        NETNATIVE_LOGW("Invalid ipv4 packet, ip header length %{public}u larger than entire packet", ipHeader->ihl);
        return false;
    }
    if (ipHeader->version != IPVERSION) {
        NETNATIVE_LOGW("Invalid ipv4 packet, version %{public}u not 4", ipHeader->version);
        return false;
    }
    return true;
}

bool ClatdPacketConverter::IsV6PacketValid(const ip6_hdr *ip6Header, size_t packetSize)
{
    if (packetSize < sizeof(ip6_hdr)) {
        NETNATIVE_LOGW("Invalid ipv6 packet, input packet size too small");
        return false;
    }

    if (IN6_IS_ADDR_MULTICAST(&ip6Header->ip6_dst)) {
        NETNATIVE_LOGW("Invalid ipv6 packet, destination address is multicast");
        return false;
    }

    if (!(std::equal(prefixAddr_.s6_addr, prefixAddr_.s6_addr + CLAT_PREFIX_BYTE_LEN, ip6Header->ip6_src.s6_addr) &&
          IN6_ARE_ADDR_EQUAL(&ip6Header->ip6_dst, &localV6Addr_)) &&
        !(std::equal(prefixAddr_.s6_addr, prefixAddr_.s6_addr + CLAT_PREFIX_BYTE_LEN, ip6Header->ip6_dst.s6_addr) &&
          IN6_ARE_ADDR_EQUAL(&ip6Header->ip6_src, &localV6Addr_)) &&
        ip6Header->ip6_nxt != IPPROTO_ICMPV6) {
        NETNATIVE_LOGW("Invalid ipv6 packet, unknown source/destination address");
        return false;
    }

    return true;
}

void ClatdPacketConverter::WriteIpv6Header(ip6_hdr *ip6Header, uint8_t tpProtocol, const iphdr *ipHeader)
{
    ip6Header->ip6_vfc = IPV6_VERSION_FLAG;
    ip6Header->ip6_plen = 0;
    ip6Header->ip6_nxt = tpProtocol;
    ip6Header->ip6_hlim = ipHeader->ttl;

    ConvertV4Address(ipHeader->saddr, ip6Header->ip6_src);
    ConvertV4Address(ipHeader->daddr, ip6Header->ip6_dst);
}

void ClatdPacketConverter::WriteIpv4Header(iphdr *ipHeader, uint8_t tpProtocol, const ip6_hdr *ip6Header)
{
    ipHeader->ihl = (IPV4_HDR_MIN_LEN / WORD_32BIT_IN_BYTE_UNIT);
    ipHeader->version = IPVERSION;
    ipHeader->tos = 0;
    ipHeader->tot_len = htons(sizeof(iphdr));
    ipHeader->id = 0;
    ipHeader->frag_off = htons(IP_DF);
    ipHeader->ttl = ip6Header->ip6_hlim;
    ipHeader->protocol = tpProtocol;
    ipHeader->check = 0;

    ConvertV6Address(ip6Header->ip6_src, ipHeader->saddr);
    ConvertV6Address(ip6Header->ip6_dst, ipHeader->daddr);

    if (static_cast<uint32_t>(ipHeader->saddr) == INADDR_NONE) {
        ipHeader->saddr = htonl((0xffffff << CHAR_BIT) + ip6Header->ip6_hlim);
    }
}

void ClatdPacketConverter::ConvertV4Address(uint32_t v4Addr, in6_addr &v6Addr)
{
    if (v4Addr == localV4Addr_.s_addr) {
        v6Addr = localV6Addr_;
    } else {
        v6Addr = prefixAddr_;
        v6Addr.s6_addr32[CLAT_SUFFIX_OFFSET_IN_32] = v4Addr;
    }
}

void ClatdPacketConverter::ConvertV6Address(const in6_addr &v6Addr, uint32_t &v4Addr)
{
    if (std::equal(prefixAddr_.s6_addr, prefixAddr_.s6_addr + CLAT_PREFIX_BYTE_LEN, v6Addr.s6_addr)) {
        v4Addr = v6Addr.s6_addr32[CLAT_SUFFIX_OFFSET_IN_32];
    } else if (IN6_ARE_ADDR_EQUAL(&v6Addr, &localV6Addr_)) {
        v4Addr = localV4Addr_.s_addr;
    } else {
        v4Addr = INADDR_NONE;
    }
}

size_t ClatdPacketConverter::WriteFragHeader(ip6_frag *ip6FragHeader, ip6_hdr *ip6Header, const iphdr *ipHeader)
{
    uint16_t fragValue = ntohs(ipHeader->frag_off);
    uint16_t fragOffset = fragValue & IP_OFFMASK;
    if (fragOffset == 0 && (fragValue & IP_MF) == 0) {
        return 0;
    }

    ip6FragHeader->ip6f_nxt = ip6Header->ip6_nxt;
    ip6FragHeader->ip6f_reserved = 0;
    ip6FragHeader->ip6f_offlg = htons(fragOffset << IPV6_FRAG_OFFSET_BIT_SUPPLEMENTARY);
    if (fragValue & IP_MF) {
        ip6FragHeader->ip6f_offlg |= IP6F_MORE_FRAG;
    }
    ip6FragHeader->ip6f_ident = htonl(ntohs(ipHeader->id));
    ip6Header->ip6_nxt = IPPROTO_FRAGMENT;

    return sizeof(*ip6FragHeader);
}

void ClatdPacketConverter::ProcessFragHeader(const ip6_frag *ip6FragHeader, iphdr *ipHeader, uint8_t &v6TpProtocol,
                                             uint8_t &v4TpProtocol)
{
    uint16_t fragOffset = ntohs(ip6FragHeader->ip6f_offlg & IP6F_OFF_MASK) >> IPV6_FRAG_OFFSET_BIT_SUPPLEMENTARY;
    if (ip6FragHeader->ip6f_offlg & IP6F_MORE_FRAG) {
        fragOffset |= IP_MF;
    }
    ipHeader->frag_off = htons(fragOffset);
    ipHeader->id = htons(ntohl(ip6FragHeader->ip6f_ident) & 0xffff);

    v6TpProtocol = ip6FragHeader->ip6f_nxt;
    v4TpProtocol = v6TpProtocol == IPPROTO_ICMPV6 ? IPPROTO_ICMP : v6TpProtocol;
    ipHeader->protocol = v4TpProtocol;
}

uint32_t ClatdPacketConverter::CalV4PseudoHeaderChecksum(const iphdr *ipHeader, uint16_t tpLen, uint8_t tpProtocol)
{
    uint16_t len = htons(tpLen);
    uint16_t protocol = htons(tpProtocol);
    uint32_t sum = 0;

    sum = AddChecksum(sum, &(ipHeader->saddr), sizeof(uint32_t));
    sum = AddChecksum(sum, &(ipHeader->daddr), sizeof(uint32_t));
    sum = AddChecksum(sum, &len, sizeof(uint16_t));
    sum = AddChecksum(sum, &protocol, sizeof(uint16_t));
    return sum;
}

uint32_t ClatdPacketConverter::CalV6PseudoHeaderChecksum(const ip6_hdr *ip6Header, uint32_t tpLen, uint8_t tpProtocol)
{
    uint32_t len = htonl(tpLen);
    uint32_t protocol = htonl(tpProtocol);
    uint32_t sum = 0;

    sum = AddChecksum(sum, &(ip6Header->ip6_src), sizeof(in6_addr));
    sum = AddChecksum(sum, &(ip6Header->ip6_dst), sizeof(in6_addr));
    sum = AddChecksum(sum, &len, sizeof(uint32_t));
    sum = AddChecksum(sum, &protocol, sizeof(uint32_t));
    return sum;
}

uint16_t ClatdPacketConverter::GetIovPacketLength(int pos)
{
    size_t sum = 0;
    for (size_t i = pos + 1; i < static_cast<int>(CLATD_MAX); i++) {
        sum += iovBufLens_[i];
    }
    return sum;
}

int32_t ClatdPacketConverter::ConvertIcmpPacket(int pos, const icmphdr *icmpHeader, uint32_t checksum, size_t tpLen)
{
    if (tpLen < sizeof(icmphdr)) {
        NETNATIVE_LOGW("fail to convert icmp packet, packet length is too small");
        effectivePos_ = 0;
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    icmp6_hdr icmp6Header;

    ConvertIcmpTypeAndCode(icmpHeader->type, icmpHeader->code, icmp6Header.icmp6_type, icmp6Header.icmp6_code);

    iovBufLens_[pos] = sizeof(icmp6_hdr);

    const uint8_t *payload = reinterpret_cast<const uint8_t *>(icmpHeader + 1);
    size_t payloadLen = tpLen - sizeof(icmphdr);

    int32_t ret;
    if (pos == static_cast<int>(CLATD_TPHDR) &&
        (icmp6Header.icmp6_type == ICMP6_DST_UNREACH || icmp6Header.icmp6_type == ICMP6_TIME_EXCEEDED)) {
        ret = ConvertV4Packet(pos + 1, payload, payloadLen);

        checksum = checksum + htons(IPV6_HDR_LEN - IPV4_HDR_MIN_LEN);
    } else if (icmp6Header.icmp6_type == ICMP6_ECHO_REQUEST || icmp6Header.icmp6_type == ICMP6_ECHO_REPLY) {
        // Ping packet.
        icmp6Header.icmp6_id = icmpHeader->un.echo.id;
        icmp6Header.icmp6_seq = icmpHeader->un.echo.sequence;
        iovBufs_[CLATD_PAYLOAD].assign(reinterpret_cast<const char *>(payload), payloadLen);
        iovBufLens_[CLATD_PAYLOAD] = payloadLen;
        effectivePos_ = CLATD_PAYLOAD + 1;
        ret = NETMANAGER_SUCCESS;
    } else {
        effectivePos_ = 0;
        ret = NETMANAGER_ERR_INVALID_PARAMETER;
    }

    icmp6Header.icmp6_cksum = 0;
    iovBufs_[pos].assign(reinterpret_cast<const char *>(&icmp6Header), iovBufLens_[pos]);
    icmp6Header.icmp6_cksum = CalIovPacketChecksum(checksum, pos);
    iovBufs_[pos].assign(reinterpret_cast<const char *>(&icmp6Header), iovBufLens_[pos]);
    return ret;
}

void ClatdPacketConverter::ConvertIcmpTypeAndCode(const uint8_t &icmpType, const uint8_t &icmpCode, uint8_t &icmp6Type,
                                                  uint8_t &icmp6Code)
{
    switch (icmpType) {
        case ICMP_ECHO:
            icmp6Type = ICMP6_ECHO_REQUEST;
            icmp6Code = icmpCode;
            break;
        case ICMP_ECHOREPLY:
            icmp6Type = ICMP6_ECHO_REPLY;
            icmp6Code = icmpCode;
            break;
        case ICMP_TIME_EXCEEDED:
            icmp6Type = ICMP6_TIME_EXCEEDED;
            icmp6Code = icmpCode;
            break;
        case ICMP_DEST_UNREACH:
            switch (icmpCode) {
                case ICMP_UNREACH_NET:
                case ICMP_UNREACH_HOST:
                case ICMP_UNREACH_SRCFAIL:
                case ICMP_UNREACH_NET_UNKNOWN:
                case ICMP_UNREACH_HOST_UNKNOWN:
                case ICMP_UNREACH_ISOLATED:
                case ICMP_UNREACH_TOSNET:
                case ICMP_UNREACH_TOSHOST:
                    icmp6Type = ICMP6_DST_UNREACH;
                    icmp6Code = ICMP6_DST_UNREACH_NOROUTE;
                    break;
                case ICMP_UNREACH_PORT:
                    icmp6Type = ICMP6_DST_UNREACH;
                    icmp6Code = ICMP6_DST_UNREACH_NOPORT;
                    break;
                case ICMP_UNREACH_NET_PROHIB:
                case ICMP_UNREACH_HOST_PROHIB:
                case ICMP_UNREACH_FILTER_PROHIB:
                case ICMP_UNREACH_PRECEDENCE_CUTOFF:
                    icmp6Type = ICMP6_DST_UNREACH;
                    icmp6Code = ICMP6_DST_UNREACH_ADMIN;
                    break;
                default:
                    icmp6Type = ICMP6_PARAM_PROB;
                    NETNATIVE_LOGW("fail to convert icmp packet type %{public}d code %{public}d", icmpType, icmpCode);
                    break;
            }
            break;
        default:
            NETNATIVE_LOGW("fail to convert icmp packet type %{public}d", icmpType);
            icmp6Type = ICMP6_PARAM_PROB;
    }
}

int32_t ClatdPacketConverter::ConvertIcmpv6Packet(int pos, const icmp6_hdr *icmp6Header, size_t tpLen)
{
    if (tpLen < sizeof(icmp6_hdr)) {
        NETNATIVE_LOGW("fail to convert icmp6 packet, packet length is too small");
        effectivePos_ = 0;
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    icmphdr icmpHeader;
    ConvertIcmpV6TypeAndCode(icmp6Header->icmp6_type, icmp6Header->icmp6_code, icmpHeader.type, icmpHeader.code);
    iovBufLens_[pos] = sizeof(icmphdr);

    const uint8_t *payload = reinterpret_cast<const uint8_t *>(icmp6Header + 1);
    size_t payloadLen = tpLen - sizeof(icmp6_hdr);
    int32_t ret;
    if (pos == CLATD_TPHDR && icmp6Header->icmp6_type < ICMP6_ECHO_REQUEST && icmpHeader.type != ICMP_PARAMETERPROB) {
        ret = ConvertV6Packet(pos + 1, payload, payloadLen);
    } else if (icmpHeader.type == ICMP_ECHO || icmpHeader.type == ICMP_ECHOREPLY) {
        // Ping packet.
        icmpHeader.un.echo.id = icmp6Header->icmp6_id;
        icmpHeader.un.echo.sequence = icmp6Header->icmp6_seq;
        iovBufs_[CLATD_PAYLOAD].assign(reinterpret_cast<const char *>(payload), payloadLen);
        iovBufLens_[CLATD_PAYLOAD] = payloadLen;
        effectivePos_ = CLATD_PAYLOAD + 1;
        ret = NETMANAGER_SUCCESS;
    } else {
        effectivePos_ = 0;
        ret = NETMANAGER_ERR_INVALID_PARAMETER;
    }

    icmpHeader.checksum = 0;
    iovBufs_[pos].assign(reinterpret_cast<const char *>(&icmpHeader), iovBufLens_[pos]);
    icmpHeader.checksum = CalIovPacketChecksum(0, pos);
    iovBufs_[pos].assign(reinterpret_cast<const char *>(&icmpHeader), iovBufLens_[pos]);

    return ret;
}

void ClatdPacketConverter::ConvertIcmpV6TypeAndCode(const uint8_t &icmp6Type, const uint8_t &icmp6Code,
                                                    uint8_t &icmpType, uint8_t &icmpCode)
{
    switch (icmp6Type) {
        case ICMP6_ECHO_REQUEST:
            icmpType = ICMP_ECHO;
            icmpCode = icmp6Code;
            break;
        case ICMP6_ECHO_REPLY:
            icmpType = ICMP_ECHOREPLY;
            icmpCode = icmp6Code;
            break;
        case ICMP6_TIME_EXCEEDED:
            icmpType = ICMP_TIME_EXCEEDED;
            icmpCode = icmp6Code;
            break;
        case ICMP6_DST_UNREACH:
            switch (icmp6Code) {
                case ICMP6_DST_UNREACH_NOROUTE:
                case ICMP6_DST_UNREACH_BEYONDSCOPE:
                case ICMP6_DST_UNREACH_ADDR:
                    icmpType = ICMP_DEST_UNREACH;
                    icmpCode = ICMP_UNREACH_HOST;
                    break;

                case ICMP6_DST_UNREACH_ADMIN:
                    icmpType = ICMP_DEST_UNREACH;
                    icmpCode = ICMP_UNREACH_HOST_PROHIB;
                    break;

                case ICMP6_DST_UNREACH_NOPORT:
                    icmpType = ICMP_DEST_UNREACH;
                    icmpCode = ICMP_UNREACH_PORT;
                    break;
                default:
                    NETNATIVE_LOGW("fail to convert icmpv6 packet type %{public}d", icmp6Type);
                    icmpType = ICMP_PARAMETERPROB;
            }
            break;
        default:
            NETNATIVE_LOGW("fail to convert icmpv6 packet type %{public}d", icmp6Type);
            icmpType = ICMP_PARAMETERPROB;
    }
}

uint16_t ClatdPacketConverter::CalIovPacketChecksum(uint32_t sum, int pos)
{
    for (size_t i = pos; i < CLATD_MAX; i++) {
        if (iovBufLens_[i] > 0) {
            sum = AddChecksum(sum, iovBufs_[i].data(), iovBufLens_[i]);
        }
    }
    return ~Checksum32To16(sum);
}

int32_t ClatdPacketConverter::ConvertTcpPacket(int pos, const tcphdr *tcpHeader, uint32_t oldChecksum,
                                               uint32_t newChecksum, size_t tpLen)
{
    if (!IsTcpPacketValid(tcpHeader, tpLen)) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    size_t tcpHdrLen = tcpHeader->doff * WORD_32BIT_IN_BYTE_UNIT;
    iovBufLens_[pos] = tcpHdrLen;

    char tcpHdrBuf[TCP_HDR_MAX_LEN];
    // LCOV_EXCL_START
    if (memcpy_s(tcpHdrBuf, TCP_HDR_MAX_LEN, tcpHeader, tcpHdrLen) != EOK) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    // LCOV_EXCL_STOP
    tcphdr *tcpHeaderOut = reinterpret_cast<tcphdr *>(tcpHdrBuf);

    iovBufs_[CLATD_PAYLOAD].assign(reinterpret_cast<const char *>(tcpHeader) + tcpHdrLen, tpLen - tcpHdrLen);
    iovBufLens_[CLATD_PAYLOAD] = tpLen - tcpHdrLen;
    tcpHeaderOut->check = AdjustChecksum(tcpHeader->check, oldChecksum, newChecksum);
    iovBufs_[pos].assign(reinterpret_cast<const char *>(tcpHdrBuf), tcpHdrLen);
    effectivePos_ = CLATD_PAYLOAD + 1;
    return NETMANAGER_SUCCESS;
}

bool ClatdPacketConverter::IsTcpPacketValid(const tcphdr *tcpHeader, size_t packetSize)
{
    if (packetSize < sizeof(tcphdr)) {
        NETNATIVE_LOGW("Invalid tcp packet, packet length is too small");
        effectivePos_ = 0;
        return false;
    }

    if (tcpHeader->doff * WORD_32BIT_IN_BYTE_UNIT < TCP_HDR_MIN_LEN) {
        NETNATIVE_LOGW("Invalid tcp packet, tcp header length %{public}u smaller than 5", tcpHeader->doff);
        effectivePos_ = 0;
        return false;
    }

    if (static_cast<size_t>(tcpHeader->doff * WORD_32BIT_IN_BYTE_UNIT) > packetSize) {
        NETNATIVE_LOGW("Invalid tcp packet, tcp header length %{public}u larger than entire packet", tcpHeader->doff);
        effectivePos_ = 0;
        return false;
    }
    // LCOV_EXCL_START
    if (tcpHeader->doff * WORD_32BIT_IN_BYTE_UNIT > TCP_HDR_MAX_LEN) {
        NETNATIVE_LOGW("Invalid tcp packet, tcp header length %{public}u larger than MAX_TCP_HDR", tcpHeader->doff);
        effectivePos_ = 0;
        return false;
    }
    // LCOV_EXCL_STOP
    return true;
}

int32_t ClatdPacketConverter::ConvertUdpPacket(int pos, const udphdr *udpHeader, uint32_t oldChecksum,
                                               uint32_t newChecksum, size_t tpLen)
{
    if (tpLen < sizeof(udphdr)) {
        NETNATIVE_LOGW("Invalid udp packet, packet length is too small");
        effectivePos_ = 0;
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    iovBufLens_[pos] = sizeof(udphdr);

    iovBufLens_[CLATD_PAYLOAD] = tpLen - sizeof(udphdr);
    iovBufs_[CLATD_PAYLOAD].assign(reinterpret_cast<const char *>(udpHeader + 1), tpLen - sizeof(udphdr));

    udphdr udpHeaderOut = *udpHeader;
    // details about zero checksum in RFC 768
    if (udpHeaderOut.check == 0) {
        iovBufs_[pos].assign(reinterpret_cast<const char *>(&udpHeaderOut), sizeof(udphdr));
        udpHeaderOut.check = CalIovPacketChecksum(newChecksum, pos);
    } else {
        udpHeaderOut.check = AdjustChecksum(udpHeader->check, oldChecksum, newChecksum);
    }

    // LCOV_EXCL_START
    if (udpHeaderOut.check == 0) {
        udpHeaderOut.check = 0xffff;
    }
    // LCOV_EXCL_STOP
    iovBufs_[pos].assign(reinterpret_cast<const char *>(&udpHeaderOut), sizeof(udphdr));
    effectivePos_ = CLATD_PAYLOAD + 1;
    return NETMANAGER_SUCCESS;
}

void ClatdPacketConverter::WritePayload(int pos, const uint8_t *tpHeader, size_t tpLen)
{
    iovBufLens_[pos + IP_TP_PACKET_POSITION_DELTA] = 0;
    iovBufs_[CLATD_PAYLOAD].assign(reinterpret_cast<const char *>(tpHeader), tpLen);
    iovBufLens_[CLATD_PAYLOAD] = tpLen;
    effectivePos_ = CLATD_MAX;
}

void ClatdPacketConverter::WriteTunHeader(bool skip_csum)
{
    tun_pi tunProtocolInfo;
    if (skip_csum) {
        tunProtocolInfo.flags = htons(TP_CSUM_UNNECESSARY);
    } else {
        tunProtocolInfo.flags = 0;
    }
    tunProtocolInfo.proto = htons(ETH_P_IP);
    iovBufLens_[CLATD_TUNHDR] = sizeof(tun_pi);
    iovBufs_[CLATD_TUNHDR].assign(reinterpret_cast<const char *>(&tunProtocolInfo), sizeof(tun_pi));
}

} // namespace nmd
} // namespace OHOS
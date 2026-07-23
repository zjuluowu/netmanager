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
#ifndef NETSYS_CLAT_UTILS_H
#define NETSYS_CLAT_UTILS_H

#include <linux/if_tun.h>
#include <netinet/icmp6.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <string>
#include <sys/uio.h>

#include "clat_constants.h"
#include "inet_addr.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;
typedef struct {
    std::string v6Iface;
    std::string tunIface;
    INetAddr v4Addr;
    INetAddr v6Addr;
    std::string nat64PrefixStr;
    int tunFd;
    int readSock6;
    int writeSock6;
    int netId;
} ClatdTracker;

typedef struct {
    ip6_hdr v6Header;
    nd_neighbor_solicit ns;
    uint8_t nonceOptType;
    uint8_t nonceOptLen;
    uint8_t nonce[6];
} ClatdDadPacket;

typedef struct {
    __u8 flags;
    __u8 gsoType;
    __u16 hdrLen;     /* Ethernet + IP + tcp/udp hdrs */
    __u16 gsoSize;    /* Bytes to append to hdr_len per frame */
    __u16 csumStart;  /* Position to start checksumming from */
    __u16 csumOffset; /* Offset after that to place checksum */
} virtioNetHdr;

typedef struct {
    virtioNetHdr vnet;
    uint8_t payload[CLAT_DATA_LINK_HDR_LEN + CLAT_MAX_MTU];
    char pad; // +1 to make packet truncation obvious
} ClatdReadV6Buf;

typedef struct {
    tun_pi tunProtocolInfo;
    uint8_t payload[CLAT_MAX_MTU];
    char pad; // +1 to make packet truncation obvious
} ClatdReadTunBuf;

typedef iovec ClatdPacket[CLATD_MAX];

void FreeTunV4Addr(const std::string &v4AddrStr);
int32_t SelectIpv4Address(const std::string &initV4AddrStr, int prefixLen, std::string &v4AddrStr);
int32_t GenerateIpv6Address(const std::string &v6IfaceStr, const std::string &v4AddrStr, const std::string &prefix64Str,
                            uint32_t mark, std::string &v6AddrStr);
uint16_t CalChecksum(const void *data, int len);
uint32_t AddChecksum(uint32_t sum, const void *data, int len);
uint16_t Checksum32To16(uint32_t sum32);
uint16_t AdjustChecksum(uint16_t oldSum16, uint32_t oldSumHdr, uint32_t newSumHdr);
int32_t CreateTunInterface(const std::string &tunIface, int &fd);
int32_t OpenPacketSocket(int &readSock6);
int32_t OpenRawSocket6(const uint32_t mark, int &writeSock6);
int32_t ConfigureWriteSocket(const int sockFd, const std::string &v6Iface);
int32_t ConfigureReadSocket(const int sockFd, const std::string &addrStr, int ifIndex);
int32_t SetTunInterfaceAddress(const std::string &ifName, const std::string &tunAddr, int32_t prefix);

} // namespace nmd
} // namespace OHOS
#endif
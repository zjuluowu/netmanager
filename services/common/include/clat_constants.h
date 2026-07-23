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
#ifndef CLAT_CONSTANTS_H
#define CLAT_CONSTANTS_H

#include <string>

namespace OHOS {
namespace NetManagerStandard {
enum Nat464UpdateFlag {
    NAT464_SERVICE_CONTINUE,
    NAT464_SERVICE_STOP,
};

enum Nat464ServiceState {
    NAT464_SERVICE_STATE_IDLE,
    NAT464_SERVICE_STATE_DISCOVERING,
    NAT464_SERVICE_STATE_RUNNING,
};

enum ClatdConvertType {
    CONVERT_FROM_V6_TO_V4,
    CONVERT_FROM_V4_TO_V6,
};

enum ClatdPacketPosition {
    CLATD_TUNHDR,
    CLATD_IPHDR,
    CLATD_FRAGHDR,
    CLATD_TPHDR,
    CLATD_ICMP_IPHDR,
    CLATD_ICMP_FRAGHDR,
    CLATD_ICMP_TPHDR,
    CLATD_PAYLOAD,
    CLATD_MAX
};

static constexpr size_t IP_TP_PACKET_POSITION_DELTA = 2; // packet position delta between CLATD_IPHDR and CLATD_TPHDR

static constexpr size_t TCP_HDR_MIN_LEN = 20;
static constexpr size_t TCP_HDR_MAX_LEN = 60;
static constexpr int IPV4_HDR_MIN_LEN = 20;
static constexpr int IPV6_HDR_LEN = 40;
static constexpr int FRAG_HDR_LEN = 8;
static constexpr int MTU_DELTA = IPV6_HDR_LEN - IPV4_HDR_MIN_LEN + FRAG_HDR_LEN;
static constexpr int CLAT_MAX_MTU = 65536 + MTU_DELTA;
static constexpr int CLAT_DATA_LINK_HDR_LEN = 22; // 14 bytes ethernet header + at most 8 bytes VLAN Tag

static constexpr int CLAT_IPV6_MIN_MTU = 1280;
static constexpr uint16_t TP_CSUM_UNNECESSARY = 1;

static constexpr const char *CLAT_PREFIX = "tunv4-";
static constexpr int CLAT_PREFIX_BYTE_LEN = 12;
static constexpr int CLAT_SUFFIX_OFFSET_IN_32 = 3; // 96-bit prefix + 32-bit suffix, the suffix is the 4th 32-bit word

static constexpr int WORD_32BIT_IN_BYTE_UNIT = 4; // 4-bytes unit for tcphdr->doff and iphdr->ihl

static constexpr uint8_t IPV6_VERSION_FLAG = 0x60;
static constexpr int V4ADDR_BIT_LEN = 32;
static constexpr int V6ADDR_BIT_LEN = 128;
static constexpr const char *INIT_V4ADDR_STRING = "192.0.0.0"; // Reserved ip addr in RFC 5736
static constexpr int INIT_V4ADDR_PREFIX_BIT_LEN = 29;

static constexpr size_t CLAT_V6ADDR_RANDOMIZE_OFFSET = 8;
static constexpr size_t CLAT_V6ADDR_RANDOMIZE_BIT_LENGTH = 8;
static constexpr size_t CLAT_V6ADDR_NEUTRALIZE_OFFSET = 3;

static constexpr uint16_t WKN_DNS_PORT = 53;
static constexpr const char *DEFAULT_V4_ADDR = "0.0.0.0/0";

static constexpr const char *SOLICITED_NODE_PREFIX = "ff02::1:ff00:0000"; // Solicited-Node Address in RFC 4291
static constexpr size_t SOLICITED_NODE_SUFFIX_OFFSET = 13;

static constexpr uint16_t IPV6_FRAG_OFFSET_BIT_SUPPLEMENTARY = 3; // offset field in ipv6 fragment header is top 13 bit

static constexpr int INVALID_IFINDEX = 0;

static constexpr const char *IFACE_LINK_UP = "up";

static constexpr size_t IPV6_SRC_OFFSET = 8;
static constexpr uint8_t NDP_NOUNCE_OPT = 14;

static constexpr const char *IPV4_ONLY_HOST = "ipv4only.arpa.";

static constexpr uint32_t INITIAL_DISCOVERY_CYCLE_MS = 100;

static constexpr uint32_t MAX_DISCOVERY_CYCLE_MS = 100000;

static constexpr uint32_t DISCOVERY_CYCLE_MULTIPLIER = 2;

static constexpr uint32_t CLATD_TIMER_CYCLE_MS = 5000;

} // namespace NetManagerStandard
} // namespace OHOS
#endif
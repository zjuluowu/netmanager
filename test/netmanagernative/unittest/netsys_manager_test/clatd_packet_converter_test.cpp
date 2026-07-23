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
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <netinet/in.h>
#include "securec.h"

#define private public
#include "clatd_packet_converter.h"
#include "net_manager_constants.h"

using namespace testing;

namespace OHOS {
namespace nmd {
using namespace testing::ext;

static constexpr const char *V4ADDR = "192.0.0.0";
static constexpr const char *V6ADDR_UDP_ICMP = "2408:8456:3242:b272:28fb:90b4:fdc6:ce53";
static constexpr const char *V6ADDR_TCP = "2408:8456:3226:d7a4:a265:ca6:72b2:3ef6";
static constexpr const char *PREFIXADDR = "2407:c080:7ef:ffff::";

// clang-format off
static const uint8_t V4_UDP_PACKET_TX[] = {
    0x45, 0x00, 0x00, 0x20, 0x68, 0x69, 0x40, 0x00, 0x40, 0x11, 0x5d, 0xa5, 0xc0, 0x00, 0x00, 0x00,
    0x8b, 0x09, 0x29, 0xb5, 0x95, 0xd4, 0x14, 0x51, 0x00, 0x0c, 0x70, 0x1d, 0x15, 0xcd, 0x5b, 0x07,
};

static const uint8_t V6_UDP_PACKET_TX[] = {
    0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x11, 0x40, 0x24, 0x08, 0x84, 0x56, 0x32, 0x42, 0xb2, 0x72,
    0x28, 0xfb, 0x90, 0xb4, 0xfd, 0xc6, 0xce, 0x53, 0x24, 0x07, 0xc0, 0x80, 0x07, 0xef, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x8b, 0x09, 0x29, 0xb5, 0x95, 0xd4, 0x14, 0x51, 0x00, 0x0c, 0x30, 0xc9,
    0x15, 0xcd, 0x5b, 0x07
};

static const uint8_t V6_INVALID_PROTOCOL_PACKET_TX[] = {
    0x60, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x99, 0x40, 0x24, 0x08, 0x84, 0x56, 0x32, 0x42, 0xb2, 0x72,
    0x28, 0xfb, 0x90, 0xb4, 0xfd, 0xc6, 0xce, 0x53, 0x24, 0x07, 0xc0, 0x80, 0x07, 0xef, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x8b, 0x09, 0x29, 0xb5, 0x95, 0xd4, 0x14, 0x51, 0x00, 0x0c, 0x30, 0xc9,
    0x15, 0xcd, 0x5b, 0x07
};

static const uint8_t V6_UDP_PACKET_RX[] = {
    0x69, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x11, 0x29, 0x24, 0x07, 0xc0, 0x80, 0x07, 0xef, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x8b, 0x09, 0x29, 0xb5, 0x24, 0x08, 0x84, 0x56, 0x32, 0x42, 0xb2, 0x72,
    0x28, 0xfb, 0x90, 0xb4, 0xfd, 0xc6, 0xce, 0x53, 0x14, 0x51, 0x95, 0xd4, 0x00, 0x0c, 0x33, 0x2d,
    0x36, 0x37, 0x38, 0x39
};

static const uint8_t V4_UDP_PACKET_RX[] = {
    0x45, 0x00, 0x00, 0x20, 0x00, 0x00, 0x40, 0x00, 0x29, 0x11, 0xdd, 0x0e, 0x8b, 0x09, 0x29, 0xb5,
    0xc0, 0x00, 0x00, 0x00, 0x14, 0x51, 0x95, 0xd4, 0x00, 0x0c, 0x72, 0x81, 0x36, 0x37, 0x38, 0x39,
};

static const uint8_t V4_TCP_PACKET_TX[] = {
    0x45, 0x00, 0x00, 0x3c, 0x7a, 0x91, 0x40, 0x00, 0x40, 0x06, 0x4b, 0x6c, 0xc0, 0x00, 0x00, 0x00,
    0x8b, 0x09, 0x29, 0xb5, 0xca, 0x66, 0x1f, 0x90, 0xd5, 0xd3, 0x06, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0xa0, 0x02, 0xff, 0xff, 0x37, 0x8b, 0x00, 0x00, 0x02, 0x04, 0x05, 0x34, 0x04, 0x02, 0x08, 0x0a,
    0x11, 0xa2, 0xc4, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x08
};

static const uint8_t V4_TCP_PACKET_FRAG_TX[] = {
    0x45, 0x00, 0x00, 0x3c, 0x7a, 0x91, 0x1f, 0xff, 0x40, 0x06, 0x4b, 0x6c, 0xc0, 0x00, 0x00, 0x00,
    0x8b, 0x09, 0x29, 0xb5, 0xca, 0x66, 0x1f, 0x90, 0xd5, 0xd3, 0x06, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0xa0, 0x02, 0xff, 0xff, 0x37, 0x8b, 0x00, 0x00, 0x02, 0x04, 0x05, 0x34, 0x04, 0x02, 0x08, 0x0a,
    0x11, 0xa2, 0xc4, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x08
};

static const uint8_t V4_TCP_INVALID_1[] = {0x45};

static const uint8_t V4_TCP_INVALID_2[] = {
    0x11, 0x00, 0x00, 0x3c, 0x7a, 0x91, 0x40, 0x00, 0x40, 0x06, 0x4b, 0x6c, 0xc0, 0x00, 0x00, 0x00,
    0x8b, 0x09, 0x29, 0xb5, 0xca, 0x66, 0x1f, 0x90, 0xd5, 0xd3, 0x06, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0xa0, 0x02, 0xff, 0xff, 0x37, 0x8b, 0x00, 0x00, 0x02, 0x04, 0x05, 0x34, 0x04, 0x02, 0x08, 0x0a,
    0x11, 0xa2, 0xc4, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x08
};

static const uint8_t V4_TCP_INVALID_3[] = {
    0xFF, 0x00, 0x00, 0x3c, 0x7a, 0x91, 0x40, 0x00, 0x40, 0x06, 0x4b, 0x6c, 0xc0, 0x00, 0x00, 0x00,
    0x8b, 0x09, 0x29, 0xb5, 0xca, 0x66, 0x1f, 0x90, 0xd5, 0xd3, 0x06, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0xa0, 0x02, 0xff, 0xff, 0x37, 0x8b, 0x00, 0x00, 0x02, 0x04, 0x05, 0x34, 0x04, 0x02, 0x08, 0x0a,
    0x11, 0xa2, 0xc4, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03
};

static const uint8_t V4_TCP_INVALID_4[] = {
    0x59, 0x00, 0x00, 0x3c, 0x7a, 0x91, 0x40, 0x00, 0x40, 0x06, 0x4b, 0x6c, 0xc0, 0x00, 0x00, 0x00,
    0x8b, 0x09, 0x29, 0xb5, 0xca, 0x66, 0x1f, 0x90, 0xd5, 0xd3, 0x06, 0xc0, 0x00, 0x00, 0x00, 0x00,
    0xa0, 0x02, 0xff, 0xff, 0x37, 0x8b, 0x00, 0x00, 0x02, 0x04, 0x05, 0x34, 0x04, 0x02, 0x08, 0x0a,
    0x11, 0xa2, 0xc4, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x08
};

static const uint8_t V6_TCP_PACKET_TX[] = {
    0x60, 0x00, 0x00, 0x00, 0x00, 0x28, 0x06, 0x40, 0x24, 0x08, 0x84, 0x56, 0x32, 0x26, 0xd7, 0xa4,
    0xa2, 0x65, 0x0c, 0xa6, 0x72, 0xb2, 0x3e, 0xf6, 0x24, 0x07, 0xc0, 0x80, 0x07, 0xef, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x8b, 0x09, 0x29, 0xb5, 0xca, 0x66, 0x1f, 0x90, 0xd5, 0xd3, 0x06, 0xc0,
    0x00, 0x00, 0x00, 0x00, 0xa0, 0x02, 0xff, 0xff, 0xf8, 0x36, 0x00, 0x00, 0x02, 0x04, 0x05, 0x34,
    0x04, 0x02, 0x08, 0x0a, 0x11, 0xa2, 0xc4, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x08,
};

static const uint8_t V6_TCP_PACKET_RX[] = {
    0x69, 0x00, 0x00, 0x00, 0x00, 0x28, 0x06, 0x2a, 0x24, 0x07, 0xc0, 0x80, 0x07, 0xef, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x8b, 0x09, 0x29, 0xb5, 0x24, 0x08, 0x84, 0x56, 0x32, 0x26, 0xd7, 0xa4,
    0xa2, 0x65, 0x0c, 0xa6, 0x72, 0xb2, 0x3e, 0xf6, 0x1f, 0x90, 0xca, 0x66, 0x97, 0x37, 0x91, 0xdc,
    0xd5, 0xd3, 0x06, 0xc1, 0xa0, 0x12, 0xfe, 0x88, 0x15, 0x25, 0x00, 0x00, 0x02, 0x04, 0x04, 0xb0,
    0x04, 0x02, 0x08, 0x0a, 0x50, 0x73, 0x6b, 0x75, 0x11, 0xa2, 0xc4, 0x08, 0x01, 0x03, 0x03, 0x07
};

static const uint8_t V4_TCP_PACKET_RX[] = {
    0x45, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x40, 0x00, 0x2a, 0x06, 0xdb, 0xfd, 0x8b, 0x09, 0x29, 0xb5,
    0xc0, 0x00, 0x00, 0x00, 0x1f, 0x90, 0xca, 0x66, 0x97, 0x37, 0x91, 0xdc, 0xd5, 0xd3, 0x06, 0xc1,
    0xa0, 0x12, 0xfe, 0x88, 0x54, 0x79, 0x00, 0x00, 0x02, 0x04, 0x04, 0xb0, 0x04, 0x02, 0x08, 0x0a,
    0x50, 0x73, 0x6b, 0x75, 0x11, 0xa2, 0xc4, 0x08, 0x01, 0x03, 0x03, 0x07
};

static const uint8_t V4_ICMP_PACKET_TX[] = {
    0x45, 0x00, 0x00, 0x54, 0xec, 0x22, 0x40, 0x00, 0x40, 0x01, 0xd9, 0xc7, 0xc0, 0x00, 0x00, 0x00,
    0x8b, 0x09, 0x29, 0xb5, 0x08, 0x00, 0x85, 0xc1, 0x00, 0x01, 0x01, 0x00, 0x62, 0x3d, 0x0f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

static const uint8_t V6_ICMP_PACKET_TX[] = {
    0x60, 0x00, 0x00, 0x00, 0x00, 0x40, 0x3a, 0x40, 0x24, 0x08, 0x84, 0x56, 0x32, 0x42, 0xb2, 0x72,
    0x28, 0xfb, 0x90, 0xb4, 0xfd, 0xc6, 0xce, 0x53, 0x24, 0x07, 0xc0, 0x80, 0x07, 0xef, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x8b, 0x09, 0x29, 0xb5, 0x80, 0x00, 0x59, 0x33, 0x00, 0x01, 0x01, 0x00,
    0x62, 0x3d, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t V6_ICMP_PACKET_RX[] = {
    0x69, 0x00, 0x00, 0x00, 0x00, 0x40, 0x3a, 0x2a, 0x24, 0x07, 0xc0, 0x80, 0x07, 0xef, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x8b, 0x09, 0x29, 0xb5, 0x24, 0x08, 0x84, 0x56, 0x32, 0x42, 0xb2, 0x72,
    0x28, 0xfb, 0x90, 0xb4, 0xfd, 0xc6, 0xce, 0x53, 0x81, 0x00, 0x58, 0x33, 0x00, 0x01, 0x01, 0x00,
    0x62, 0x3d, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t V6_ICMP_PACKET_FRAGMENT[] = {
    0x69, 0x00, 0x00, 0x00, 0x00, 0x40, 0x2c, 0x2a, 0x24, 0x07, 0xc0, 0x80, 0x07, 0xef, 0xff,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x8b, 0x09, 0x29, 0xb5, 0x24, 0x08, 0x84, 0x56, 0x32, 0x42,
    0xb2, 0x72, 0x28, 0xfb, 0x90, 0xb4, 0xfd, 0xc6, 0xce, 0x53, 0x81, 0x00, 0x58, 0x33, 0x00,
    0x01, 0x01, 0x00, 0x62, 0x3d, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static const uint8_t V4_ICMP_PACKET_RX[] = {
    0x45, 0x00, 0x00, 0x54, 0x00, 0x00, 0x40, 0x00, 0x2a, 0x01, 0xdb, 0xea, 0x8b, 0x09, 0x29, 0xb5,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8d, 0xc1, 0x00, 0x01, 0x01, 0x00, 0x62, 0x3d, 0x0f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

class MockClatdPacketConverter : public ClatdPacketConverter {
public:
    MockClatdPacketConverter(const uint8_t *inputPacket, size_t inputPacketSize, ClatdConvertType convertType,
                             const in_addr &v4Addr, const in6_addr &v6Addr, const in6_addr &prefixAddr)
        : ClatdPacketConverter(inputPacket, inputPacketSize, convertType, v4Addr, v6Addr, prefixAddr)
    {
    }
    MOCK_METHOD3(WriteFragHeader, size_t(ip6_frag *ip6FragHeader, ip6_hdr *ip6Header, const iphdr *ipHeader));
};

// clang-format on
class ClatdPacketConverterTest : public ::testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    bool IsTranslatedPacketCorrect(std::vector<iovec> iovPackets, const uint8_t *packet);

    in_addr v4Addr_{};
    in6_addr v6Addr_{};
    in6_addr prefixAddr_{};
};

void ClatdPacketConverterTest::SetUp()
{
    inet_pton(AF_INET, V4ADDR, &v4Addr_.s_addr);
    inet_pton(AF_INET6, PREFIXADDR, &prefixAddr_);
}

void ClatdPacketConverterTest::TearDown() {}

bool ClatdPacketConverterTest::IsTranslatedPacketCorrect(std::vector<iovec> iovPackets, const uint8_t *packet)
{
    if (memcmp(iovPackets[CLATD_IPHDR].iov_base, packet, iovPackets[CLATD_IPHDR].iov_len) != 0) {
        return false;
    }
    packet += iovPackets[CLATD_IPHDR].iov_len;
    if (memcmp(iovPackets[CLATD_TPHDR].iov_base, packet, iovPackets[CLATD_TPHDR].iov_len) != 0) {
        return false;
    }
    packet += iovPackets[CLATD_TPHDR].iov_len;
    if (memcmp(iovPackets[CLATD_PAYLOAD].iov_base, packet, iovPackets[CLATD_PAYLOAD].iov_len) != 0) {
        return false;
    }
    return true;
}

/**
 * @tc.name: ClatdPacketConverterTranslateUdpTest001
 * @tc.desc: Test ClatdPacketConverter translate ipv4 udp packet.
 * @tc.type: FUNC
 */
HWTEST_F(ClatdPacketConverterTest, ClatdPacketConverterTranslateUdpTest001, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_UDP_ICMP, &v6Addr_);
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V4_UDP_PACKET_TX, sizeof(V4_UDP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_SUCCESS);

    std::vector<iovec> iovPackets(CLATD_MAX);
    int effectivePos = 0;
    clatdPacketConverter->GetConvertedPacket(iovPackets, effectivePos);

    EXPECT_EQ(IsTranslatedPacketCorrect(iovPackets, V6_UDP_PACKET_TX), true);
}

/**
 * @tc.name: ClatdPacketConverterTranslateUdpTest002
 * @tc.desc: Test ClatdPacketConverter translate ipv6 udp packet.
 * @tc.type: FUNC
 */
HWTEST_F(ClatdPacketConverterTest, ClatdPacketConverterTranslateUdpTest002, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_UDP_ICMP, &v6Addr_);
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_UDP_PACKET_RX, sizeof(V6_UDP_PACKET_RX), CONVERT_FROM_V6_TO_V4, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_SUCCESS);

    std::vector<iovec> iovPackets(CLATD_MAX);
    int effectivePos = 0;
    clatdPacketConverter->GetConvertedPacket(iovPackets, effectivePos);

    EXPECT_EQ(IsTranslatedPacketCorrect(iovPackets, V4_UDP_PACKET_RX), true);
}

/**
 * @tc.name: ClatdPacketConverterTranslateTcpTest001
 * @tc.desc: Test ClatdPacketConverter translate ipv4 tcp packet.
 * @tc.type: FUNC
 */
HWTEST_F(ClatdPacketConverterTest, ClatdPacketConverterTranslateTcpTest001, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_TCP, &v6Addr_);
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V4_TCP_PACKET_TX, sizeof(V4_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_SUCCESS);

    std::vector<iovec> iovPackets(CLATD_MAX);
    int effectivePos = 0;
    clatdPacketConverter->GetConvertedPacket(iovPackets, effectivePos);

    EXPECT_EQ(IsTranslatedPacketCorrect(iovPackets, V6_TCP_PACKET_TX), true);
}

/**
 * @tc.name: ClatdPacketConverterTranslateTcpTest002
 * @tc.desc: Test ClatdPacketConverter translate ipv6 tcp packet.
 * @tc.type: FUNC
 */
HWTEST_F(ClatdPacketConverterTest, ClatdPacketConverterTranslateTcpTest002, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_TCP, &v6Addr_);
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_TCP_PACKET_RX, sizeof(V6_TCP_PACKET_RX), CONVERT_FROM_V6_TO_V4, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_SUCCESS);

    std::vector<iovec> iovPackets(CLATD_MAX);
    int effectivePos = 0;
    clatdPacketConverter->GetConvertedPacket(iovPackets, effectivePos);

    EXPECT_EQ(IsTranslatedPacketCorrect(iovPackets, V4_TCP_PACKET_RX), true);
}

/**
 * @tc.name: ClatdPacketConverterTranslateIcmpTest001
 * @tc.desc: Test ClatdPacketConverter translate ipv4 icmp packet.
 * @tc.type: FUNC
 */
HWTEST_F(ClatdPacketConverterTest, ClatdPacketConverterTranslateIcmpTest001, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_UDP_ICMP, &v6Addr_);
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V4_ICMP_PACKET_TX, sizeof(V4_ICMP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_SUCCESS);

    std::vector<iovec> iovPackets(CLATD_MAX);
    int effectivePos = 0;
    clatdPacketConverter->GetConvertedPacket(iovPackets, effectivePos);

    EXPECT_EQ(IsTranslatedPacketCorrect(iovPackets, V6_ICMP_PACKET_TX), true);
}

/**
 * @tc.name: ClatdPacketConverterTranslateIcmpTest002
 * @tc.desc: Test ClatdPacketConverter translate ipv6 icmp packet.
 * @tc.type: FUNC
 */
HWTEST_F(ClatdPacketConverterTest, ClatdPacketConverterTranslateIcmpTest002, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_UDP_ICMP, &v6Addr_);
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_ICMP_PACKET_RX, sizeof(V6_ICMP_PACKET_RX), CONVERT_FROM_V6_TO_V4, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_SUCCESS);

    std::vector<iovec> iovPackets(CLATD_MAX);
    int effectivePos = 0;
    clatdPacketConverter->GetConvertedPacket(iovPackets, effectivePos);

    EXPECT_EQ(IsTranslatedPacketCorrect(iovPackets, V4_ICMP_PACKET_RX), true);
}

HWTEST_F(ClatdPacketConverterTest, ConvertPacketErr, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_UDP_ICMP, &v6Addr_);
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_ICMP_PACKET_RX, sizeof(V6_ICMP_PACKET_RX), CONVERT_FROM_V6_TO_V4, v4Addr_, v6Addr_, prefixAddr_);
    clatdPacketConverter->convertType_ = static_cast<ClatdConvertType>(3);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(ClatdPacketConverterTest, ConvertPacketInvalidV6Packet, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_UDP_ICMP, &v6Addr_);
    auto clatdPacketConverter =
        std::make_unique<ClatdPacketConverter>(V6_INVALID_PROTOCOL_PACKET_TX, sizeof(V6_INVALID_PROTOCOL_PACKET_TX),
                                               CONVERT_FROM_V6_TO_V4, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_ERR_INVALID_PARAMETER);
    auto clatdPacketConverter2 =
        std::make_unique<ClatdPacketConverter>(V6_INVALID_PROTOCOL_PACKET_TX, sizeof(ip6_hdr) - 1,
                                               CONVERT_FROM_V6_TO_V4, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter2->ConvertPacket(false), NETMANAGER_ERR_INVALID_PARAMETER);
    auto clatdPacketConverter3 = std::make_unique<ClatdPacketConverter>(
        V6_ICMP_PACKET_FRAGMENT, sizeof(V6_ICMP_PACKET_FRAGMENT), CONVERT_FROM_V6_TO_V4, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter3->ConvertPacket(false), NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdPacketConverterTest, ConvertPacketInvalidV4Packet, TestSize.Level1)
{
    inet_pton(AF_INET6, V6ADDR_TCP, &v6Addr_);
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V4_TCP_INVALID_1, sizeof(V4_TCP_INVALID_1), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter->ConvertPacket(false), NETMANAGER_ERR_INVALID_PARAMETER);
    auto clatdPacketConverter2 = std::make_unique<ClatdPacketConverter>(
        V4_TCP_INVALID_2, sizeof(V4_TCP_INVALID_2), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter2->ConvertPacket(false), NETMANAGER_ERR_INVALID_PARAMETER);
    auto clatdPacketConverter3 = std::make_unique<ClatdPacketConverter>(
        V4_TCP_INVALID_3, sizeof(V4_TCP_INVALID_3), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter3->ConvertPacket(false), NETMANAGER_ERR_INVALID_PARAMETER);
    auto clatdPacketConverter4 = std::make_unique<ClatdPacketConverter>(
        V4_TCP_INVALID_4, sizeof(V4_TCP_INVALID_4), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter4->ConvertPacket(false), NETMANAGER_ERR_INVALID_PARAMETER);
    auto clatdPacketConverter5 = std::make_unique<ClatdPacketConverter>(
        V4_TCP_PACKET_FRAG_TX, sizeof(V4_TCP_PACKET_FRAG_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    EXPECT_EQ(clatdPacketConverter4->ConvertPacket(false), NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(ClatdPacketConverterTest, IsV6PacketValidTest, TestSize.Level1)
{
    ip6_hdr ip6Header;
    size_t packetSize;
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_TCP_PACKET_TX, sizeof(V6_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    packetSize = sizeof(ip6_hdr) - 1;
    EXPECT_FALSE(clatdPacketConverter->IsV6PacketValid(&ip6Header, packetSize));
    packetSize = sizeof(ip6_hdr);
    in6_addr multicastAddr;
    inet_pton(AF_INET6, "ff00::8", &multicastAddr);
    memcpy_s(&ip6Header.ip6_dst, sizeof(ip6Header.ip6_dst), &multicastAddr, sizeof(multicastAddr));
    EXPECT_FALSE(clatdPacketConverter->IsV6PacketValid(&ip6Header, packetSize));
    memset_s(&ip6Header.ip6_src, sizeof(ip6Header.ip6_src), 0, sizeof(ip6Header.ip6_src));
    memset_s(&ip6Header.ip6_dst, sizeof(ip6Header.ip6_dst), 0, sizeof(ip6Header.ip6_dst));
    ip6Header.ip6_nxt = IPPROTO_TCP;
    EXPECT_FALSE(clatdPacketConverter->IsV6PacketValid(&ip6Header, packetSize));
    in6_addr loopbackAddr;
    inet_pton(AF_INET6, "::1", &loopbackAddr);
    memcpy_s(&ip6Header.ip6_src, sizeof(ip6Header.ip6_src), &loopbackAddr, sizeof(loopbackAddr));
    memcpy_s(&ip6Header.ip6_dst, sizeof(ip6Header.ip6_dst), &loopbackAddr, sizeof(loopbackAddr));
    ip6Header.ip6_nxt = IPPROTO_UDP;
    EXPECT_FALSE(clatdPacketConverter->IsV6PacketValid(&ip6Header, packetSize));
}

HWTEST_F(ClatdPacketConverterTest, ConvertV4TpPacket_ShouldReturnSuccess_WhenGreProtocol, TestSize.Level1)
{
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V4_TCP_PACKET_TX, sizeof(V4_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    int32_t pos = 0;
    iphdr ipHeader{};
    ip6_hdr ip6Header{};
    auto v6TpProtocol = IPPROTO_GRE;
    auto tpLen = 100;
    int32_t ret = clatdPacketConverter->ConvertV4TpPacket(pos, &ipHeader, &ip6Header, tpLen, v6TpProtocol);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdPacketConverterTest, ConvertV6TpPacket_ShouldReturnSuccess_WhenGreProtocol, TestSize.Level1)
{
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V4_TCP_PACKET_TX, sizeof(V4_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    int32_t pos = 0;
    iphdr ipHeader{};
    ip6_hdr ip6Header{};
    auto v6TpProtocol = IPPROTO_GRE;
    auto tpLen = 100;
    int32_t ret = clatdPacketConverter->ConvertV6TpPacket(pos, &ip6Header, &ipHeader, tpLen, v6TpProtocol);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdPacketConverterTest, ConvertV4TpPacket_ShouldReturnSuccess_WhenEspProtocol, TestSize.Level1)
{
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V4_TCP_PACKET_TX, sizeof(V4_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    int32_t pos = 0;
    iphdr ipHeader{};
    ip6_hdr ip6Header{};
    auto v6TpProtocol = IPPROTO_ESP;
    auto tpLen = 100;
    int32_t ret = clatdPacketConverter->ConvertV4TpPacket(pos, &ipHeader, &ip6Header, tpLen, v6TpProtocol);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdPacketConverterTest, ConvertV4TpPacket_ShouldReturnError_WhenUnknownProtocol, TestSize.Level0)
{
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V4_TCP_PACKET_TX, sizeof(V4_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    int32_t pos = 0;
    iphdr ipHeader{};
    ip6_hdr ip6Header{};
    auto v6TpProtocol = 255;
    auto tpLen = 100;
    int32_t ret = clatdPacketConverter->ConvertV4TpPacket(pos, &ipHeader, &ip6Header, tpLen, v6TpProtocol);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(ClatdPacketConverterTest, ConvertV4Packet_FragHeaderGreaterThanZero, TestSize.Level0) {
    auto converter = std::make_unique<MockClatdPacketConverter>(
        V4_TCP_PACKET_TX, sizeof(V4_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    int32_t expectedResult = NETMANAGER_SUCCESS;
    int pos = 0;

    ON_CALL(*converter, WriteFragHeader).WillByDefault(testing::Return(50));

    int32_t result = converter->ConvertV4Packet(pos, V4_TCP_PACKET_TX, sizeof(V4_TCP_PACKET_TX));
    EXPECT_EQ(result, expectedResult);
}

HWTEST_F(ClatdPacketConverterTest, ConvertIcmpTypeAndCodeTest, TestSize.Level0) {
    auto clatdPacketConverter = std::make_unique<MockClatdPacketConverter>(
        V4_TCP_PACKET_TX, sizeof(V4_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    uint8_t icmpType = ICMP_ECHO;
    uint8_t icmpCode = 10;
    uint8_t icmp6Type;
    uint8_t icmp6Code;
    clatdPacketConverter->ConvertIcmpTypeAndCode(icmpType, icmpCode, icmp6Type, icmp6Code);
    EXPECT_EQ(icmp6Type, ICMP6_ECHO_REQUEST);
    EXPECT_EQ(icmp6Code, icmpCode);
    icmpType = ICMP_ECHOREPLY;
    icmpCode = 10;
    clatdPacketConverter->ConvertIcmpTypeAndCode(icmpType, icmpCode, icmp6Type, icmp6Code);
    EXPECT_EQ(icmp6Type, ICMP6_ECHO_REPLY);
    EXPECT_EQ(icmp6Code, icmpCode);
    icmpType = ICMP_TIME_EXCEEDED;
    icmpCode = 10;
    clatdPacketConverter->ConvertIcmpTypeAndCode(icmpType, icmpCode, icmp6Type, icmp6Code);
    EXPECT_EQ(icmp6Type, ICMP6_TIME_EXCEEDED);
    EXPECT_EQ(icmp6Code, icmpCode);
    icmpType = ICMP_DEST_UNREACH;
    icmpCode = ICMP_UNREACH_NET;
    clatdPacketConverter->ConvertIcmpTypeAndCode(icmpType, icmpCode, icmp6Type, icmp6Code);
    EXPECT_EQ(icmp6Type, ICMP6_DST_UNREACH);
    EXPECT_EQ(icmp6Code, ICMP6_DST_UNREACH_NOROUTE);
    icmpType = ICMP_DEST_UNREACH;
    icmpCode = ICMP_UNREACH_PORT;
    clatdPacketConverter->ConvertIcmpTypeAndCode(icmpType, icmpCode, icmp6Type, icmp6Code);
    EXPECT_EQ(icmp6Type, ICMP6_DST_UNREACH);
    EXPECT_EQ(icmp6Code, ICMP6_DST_UNREACH_NOPORT);
    icmpType = ICMP_DEST_UNREACH;
    icmpCode = 10;
    clatdPacketConverter->ConvertIcmpTypeAndCode(icmpType, icmpCode, icmp6Type, icmp6Code);
    EXPECT_EQ(icmp6Type, ICMP6_DST_UNREACH);
    icmpType = ICMP_DEST_UNREACH;
    icmpCode = 99;
    clatdPacketConverter->ConvertIcmpTypeAndCode(icmpType, icmpCode, icmp6Type, icmp6Code);
    EXPECT_EQ(icmp6Type, ICMP6_PARAM_PROB);
    icmpType = 10;
    icmpCode = 10;
    clatdPacketConverter->ConvertIcmpTypeAndCode(icmpType, icmpCode, icmp6Type, icmp6Code);
    EXPECT_EQ(icmp6Type, ICMP6_PARAM_PROB);
}

HWTEST_F(ClatdPacketConverterTest, ConvertIcmpV6TypeAndCodeTest, TestSize.Level0) {
    auto clatdPacketConverter = std::make_unique<MockClatdPacketConverter>(
        V6_TCP_PACKET_TX, sizeof(V6_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    uint8_t icmp6Type = ICMP6_ECHO_REQUEST;
    uint8_t icmp6Code = 0;
    uint8_t icmpType = 0;
    uint8_t icmpCode = 0;
    clatdPacketConverter->ConvertIcmpV6TypeAndCode(icmp6Type, icmp6Code, icmpType, icmpCode);
    EXPECT_EQ(icmpType, ICMP_ECHO);
    EXPECT_EQ(icmpCode, 0);
    icmp6Type = ICMP6_ECHO_REPLY;
    icmp6Code = 0;
    clatdPacketConverter->ConvertIcmpV6TypeAndCode(icmp6Type, icmp6Code, icmpType, icmpCode);
    EXPECT_EQ(icmpType, ICMP_ECHOREPLY);
    EXPECT_EQ(icmpCode, 0);
    icmp6Type = ICMP6_TIME_EXCEEDED;
    icmp6Code = 0;
    clatdPacketConverter->ConvertIcmpV6TypeAndCode(icmp6Type, icmp6Code, icmpType, icmpCode);
    EXPECT_EQ(icmpType, ICMP_TIME_EXCEEDED);
    EXPECT_EQ(icmpCode, 0);
    icmp6Type = ICMP6_DST_UNREACH;
    icmp6Code = ICMP6_DST_UNREACH_NOROUTE;
    clatdPacketConverter->ConvertIcmpV6TypeAndCode(icmp6Type, icmp6Code, icmpType, icmpCode);
    EXPECT_EQ(icmpType, ICMP_DEST_UNREACH);
    EXPECT_EQ(icmpCode, ICMP_UNREACH_HOST);
    icmp6Type = ICMP6_DST_UNREACH;
    icmp6Code = ICMP6_DST_UNREACH_ADMIN;
    clatdPacketConverter->ConvertIcmpV6TypeAndCode(icmp6Type, icmp6Code, icmpType, icmpCode);
    EXPECT_EQ(icmpType, ICMP_DEST_UNREACH);
    EXPECT_EQ(icmpCode, ICMP_UNREACH_HOST_PROHIB);
    icmp6Type = ICMP6_DST_UNREACH;
    icmp6Code = ICMP6_DST_UNREACH_NOPORT;
    clatdPacketConverter->ConvertIcmpV6TypeAndCode(icmp6Type, icmp6Code, icmpType, icmpCode);
    EXPECT_EQ(icmpType, ICMP_DEST_UNREACH);
    EXPECT_EQ(icmpCode, ICMP_UNREACH_PORT);
    icmp6Type = 99;
    icmp6Code = 0;
    clatdPacketConverter->ConvertIcmpV6TypeAndCode(icmp6Type, icmp6Code, icmpType, icmpCode);
    EXPECT_EQ(icmpType, ICMP_PARAMETERPROB);
    icmp6Type = ICMP6_DST_UNREACH;
    icmp6Code = 99;
    clatdPacketConverter->ConvertIcmpV6TypeAndCode(icmp6Type, icmp6Code, icmpType, icmpCode);
    EXPECT_EQ(icmpType, ICMP_PARAMETERPROB);
}

HWTEST_F(ClatdPacketConverterTest, WriteFragHeaderTest, TestSize.Level0)
{
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_TCP_PACKET_TX, sizeof(V6_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    ip6_frag ip6FragHeader;
    ip6_hdr ip6Header;
    iphdr ipHeader;
    ipHeader.frag_off = 0;
    EXPECT_EQ(clatdPacketConverter->WriteFragHeader(&ip6FragHeader, &ip6Header, &ipHeader), 0);
    ipHeader.frag_off = htons(1);
    EXPECT_EQ(clatdPacketConverter->WriteFragHeader(&ip6FragHeader, &ip6Header, &ipHeader), sizeof(ip6FragHeader));
    ipHeader.frag_off = htons(IP_MF);
    clatdPacketConverter->WriteFragHeader(&ip6FragHeader, &ip6Header, &ipHeader);
    EXPECT_EQ(ip6FragHeader.ip6f_offlg & IP6F_MORE_FRAG, IP6F_MORE_FRAG);
}

HWTEST_F(ClatdPacketConverterTest, IsTcpPacketValidTest, TestSize.Level0)
{
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_TCP_PACKET_TX, sizeof(V6_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    tcphdr tcpHeader{};
    size_t packetSize = 0;
    EXPECT_FALSE(clatdPacketConverter->IsTcpPacketValid(&tcpHeader, packetSize));
    packetSize = sizeof(tcphdr);
    tcpHeader.doff = htons(0);
    EXPECT_FALSE(clatdPacketConverter->IsTcpPacketValid(&tcpHeader, packetSize));
    tcpHeader.doff = packetSize / WORD_32BIT_IN_BYTE_UNIT + 1;
    EXPECT_FALSE(clatdPacketConverter->IsTcpPacketValid(&tcpHeader, packetSize));
    EXPECT_EQ(clatdPacketConverter->ConvertTcpPacket(0, &tcpHeader, 0, 0, packetSize),
              NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(ClatdPacketConverterTest, ConvertUdpPacketTest, TestSize.Level0)
{
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_TCP_PACKET_TX, sizeof(V6_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);
    udphdr hdr{};
    size_t tpLen = 0;
    EXPECT_EQ(clatdPacketConverter->ConvertUdpPacket(0, &hdr, 0, 0, tpLen), NETMANAGER_ERR_INVALID_PARAMETER);
    tpLen = sizeof(udphdr);
    EXPECT_EQ(clatdPacketConverter->ConvertUdpPacket(0, &hdr, 0, 0, tpLen), NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdPacketConverterTest, ConvertIcmpv6PacketTest, TestSize.Level0)
{
    auto clatdPacketConverter = std::make_unique<ClatdPacketConverter>(
        V6_TCP_PACKET_TX, sizeof(V6_TCP_PACKET_TX), CONVERT_FROM_V4_TO_V6, v4Addr_, v6Addr_, prefixAddr_);

    size_t tpLen = 0;
    icmp6_hdr hdr{};
    EXPECT_EQ(clatdPacketConverter->ConvertIcmpv6Packet(0, &hdr, tpLen), NETMANAGER_ERR_INVALID_PARAMETER);
    int pos = CLATD_TPHDR;
    tpLen = sizeof(icmp6_hdr);
    hdr.icmp6_type = ICMP6_TIME_EXCEEDED;
    EXPECT_EQ(clatdPacketConverter->ConvertIcmpv6Packet(pos, &hdr, tpLen), NETMANAGER_ERR_INVALID_PARAMETER);
    hdr.icmp6_type = ICMP6_ECHO_REQUEST - 1;
    EXPECT_EQ(clatdPacketConverter->ConvertIcmpv6Packet(pos, &hdr, tpLen), NETMANAGER_ERR_INVALID_PARAMETER);
}
} // namespace nmd
} // namespace OHOS
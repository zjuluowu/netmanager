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

#include <gtest/gtest.h>

#include "netsys_udp_transfer.h"

namespace OHOS {
namespace nmd {
namespace PollUdpDataTransfer {
namespace {
using namespace testing::ext;
constexpr const uint32_t MAX_REQUESTDATA_LEN = 512;
} // namespace

class PollUdpDataTransferTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PollUdpDataTransferTest::SetUpTestCase() {}

void PollUdpDataTransferTest::TearDownTestCase() {}

void PollUdpDataTransferTest::SetUp() {}

void PollUdpDataTransferTest::TearDown() {}

HWTEST_F(PollUdpDataTransferTest, PollUdpSendData001, TestSize.Level1)
{
    int32_t sock = 34343;
    char data[] = "testdnsproxydata";
    size_t size = 17;
    AlignedSockAddr addr = {};
    addr.sin.sin_family = AF_INET;
    addr.sin.sin_port = htons(1212);
    socklen_t addrLen = sizeof(sockaddr_in);
    int32_t ret = PollUdpDataTransfer::PollUdpSendData(sock, data, size, addr, addrLen);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(PollUdpDataTransferTest, PollUdpSendData002, TestSize.Level1)
{
    int32_t sock = 34343;
    char data[] = "testdnsproxydata";
    size_t size = 17;
    AlignedSockAddr addr = {};
    addr.sin6.sin6_family = AF_INET6;
    addr.sin6.sin6_port = htons(1212);
    socklen_t addrLen = sizeof(sockaddr_in6);
    int32_t ret = PollUdpDataTransfer::PollUdpSendData(sock, data, size, addr, addrLen);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(PollUdpDataTransferTest, PollUdpRecvData001, TestSize.Level1)
{
    int32_t sock = 34343;
    char requesData[MAX_REQUESTDATA_LEN] = {0};
    AlignedSockAddr addr = {};
    addr.sin.sin_family = AF_INET;
    addr.sin.sin_port = htons(1212);
    socklen_t addrLen = sizeof(sockaddr_in);
    int32_t ret = PollUdpDataTransfer::PollUdpRecvData(sock, requesData, MAX_REQUESTDATA_LEN, addr, addrLen);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(PollUdpDataTransferTest, PollUdpRecvData002, TestSize.Level1)
{
    int32_t sock = 34343;
    char requesData[MAX_REQUESTDATA_LEN] = {0};
    AlignedSockAddr addr = {};
    addr.sin6.sin6_family = AF_INET6;
    addr.sin6.sin6_port = htons(1212);
    socklen_t addrLen = sizeof(sockaddr_in6);
    int32_t ret = PollUdpDataTransfer::PollUdpRecvData(sock, requesData, MAX_REQUESTDATA_LEN, addr, addrLen);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(PollUdpDataTransferTest, MakeUdpNonBlock001, TestSize.Level1)
{
    int32_t sock = -1;
    bool ret = PollUdpDataTransfer::MakeUdpNonBlock(sock);
    EXPECT_EQ(ret, false);
}

HWTEST_F(PollUdpDataTransferTest, MakeUdpNonBlock002, TestSize.Level1)
{
    int32_t sock = 34343;
    bool ret = PollUdpDataTransfer::MakeUdpNonBlock(sock);
    EXPECT_EQ(ret, false);
}
} // namespace PollUdpDataTransfer
} // namespace nmd
} // namespace OHOS

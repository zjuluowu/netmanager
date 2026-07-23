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

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_manager_constants.h"
#include "netlink_socket_diag.cpp"
#include "netlink_socket_diag.h"
#include <net/if.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>
#include <cstring>
#include <sys/uio.h>
#include <unistd.h>

#include "fwmark.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "securec.h"
namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
}
class NetlinkSocketDiagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetlinkSocketDiagTest::SetUpTestCase() {}

void NetlinkSocketDiagTest::TearDownTestCase() {}

void NetlinkSocketDiagTest::SetUp() {}

void NetlinkSocketDiagTest::TearDown() {}

HWTEST_F(NetlinkSocketDiagTest, InLookBack001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    uint32_t a = 0x7f000000;
    bool result = netLinkSocketDiag.InLookBack(a);
    EXPECT_EQ(result, true);
}

HWTEST_F(NetlinkSocketDiagTest, InLookBack002, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    uint32_t a = 0x6f000000;
    bool result = netLinkSocketDiag.InLookBack(a);
    EXPECT_EQ(result, false);
}

HWTEST_F(NetlinkSocketDiagTest, ExecuteDestroySocket001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    uint8_t proto = IPPROTO_TCP;
    const inet_diag_msg *msg = nullptr;
    int32_t result = netLinkSocketDiag.ExecuteDestroySocket(proto, msg);
    EXPECT_EQ(result, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetlinkSocketDiagTest, ExecuteDestroySocket002, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    uint8_t proto = IPPROTO_TCP;
    inet_diag_msg temp_msg;
    temp_msg.idiag_family = AF_INET;
    temp_msg.idiag_state = TCP_ESTABLISHED;
    const inet_diag_msg *msg = &temp_msg;
    int32_t result = netLinkSocketDiag.ExecuteDestroySocket(proto, msg);
    EXPECT_EQ(result, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetlinkSocketDiagTest, ExecuteDestroySocket003, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    uint8_t proto = IPPROTO_TCP;
    inet_diag_msg temp_msg;
    temp_msg.idiag_family = AF_INET;
    temp_msg.idiag_state = TCP_ESTABLISHED;
    const inet_diag_msg *msg = &temp_msg;
    NetLinkSocketDiag::SockDiagRequest request;
    request.nlh_.nlmsg_type = SOCK_DESTROY;
    request.nlh_.nlmsg_flags = NLM_F_REQUEST;
    request.nlh_.nlmsg_len = 10;

    write(-1, &request, sizeof(request));
    int32_t result = netLinkSocketDiag.ExecuteDestroySocket(proto, msg);
    EXPECT_EQ(result, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetlinkSocketDiagTest, GetErrorFromKernel001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    int32_t fd = 1;
    errno = EAGAIN;
    NetLinkSocketDiag::Ack ack;
    recv(fd, &ack, sizeof(ack), MSG_DONTWAIT | MSG_PEEK);
    int32_t result = netLinkSocketDiag.GetErrorFromKernel(fd);
    EXPECT_NE(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetlinkSocketDiagTest, IsMatchNetwork001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET;
    msg.id.idiag_src[0] = inet_addr("192.168.1.10");
    msg.id.idiag_dst[0] = inet_addr("192.168.1.20");

    EXPECT_TRUE(netLinkSocketDiag.IsMatchNetwork(&msg, "192.168.1.10"));
    EXPECT_TRUE(netLinkSocketDiag.IsMatchNetwork(&msg, "192.168.1.20"));
    EXPECT_FALSE(netLinkSocketDiag.IsMatchNetwork(&msg, "192.168.1.30"));
}

HWTEST_F(NetlinkSocketDiagTest, IsMatchNetwork002, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET;
    msg.id.idiag_src[0] = inet_addr("10.0.0.1");
    msg.id.idiag_dst[0] = inet_addr("10.0.0.2");

    EXPECT_FALSE(netLinkSocketDiag.IsMatchNetwork(&msg, "192.168.1.10"));
}

HWTEST_F(NetlinkSocketDiagTest, IsMatchNetwork003, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET6;
    inet_pton(AF_INET6, "2001:db8::1", &msg.id.idiag_src);
    inet_pton(AF_INET6, "2001:db8::2", &msg.id.idiag_dst);

    EXPECT_TRUE(netLinkSocketDiag.IsMatchNetwork(&msg, "2001:db8::1"));
    EXPECT_TRUE(netLinkSocketDiag.IsMatchNetwork(&msg, "2001:db8::2"));
    EXPECT_FALSE(netLinkSocketDiag.IsMatchNetwork(&msg, "2001:db8::3"));
}

HWTEST_F(NetlinkSocketDiagTest, IsMatchNetwork004, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET6;
    inet_pton(AF_INET6, "::1", &msg.id.idiag_src);
    inet_pton(AF_INET6, "::2", &msg.id.idiag_dst);

    EXPECT_FALSE(netLinkSocketDiag.IsMatchNetwork(&msg, "2001:db8::1"));
}

HWTEST_F(NetlinkSocketDiagTest, IsMatchNetwork005, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET;
    msg.id.idiag_src[0] = inet_addr("192.168.1.10");

    EXPECT_FALSE(netLinkSocketDiag.IsMatchNetwork(&msg, "2001:db8::1"));
}

HWTEST_F(NetlinkSocketDiagTest, IsMatchNetwork006, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET6;
    msg.id.idiag_src[0] = 0;
    msg.id.idiag_src[1] = 0;
    msg.id.idiag_src[2] = htonl(0xffff);
    msg.id.idiag_src[3] = inet_addr("192.168.1.10");
 
    EXPECT_TRUE(netLinkSocketDiag.IsMatchNetwork(&msg, "192.168.1.10"));
}

HWTEST_F(NetlinkSocketDiagTest, SetSocketDestroyType001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    int socketType = static_cast<int>(SocketDestroyType::DESTROY_SPECIAL_CELLULAR);
    netLinkSocketDiag.SetSocketDestroyType(socketType);
    EXPECT_EQ(netLinkSocketDiag.socketDestroyType_, SocketDestroyType::DESTROY_SPECIAL_CELLULAR);

    socketType = static_cast<int>(SocketDestroyType::DESTROY_DEFAULT_CELLULAR);
    netLinkSocketDiag.SetSocketDestroyType(socketType);
    EXPECT_EQ(netLinkSocketDiag.socketDestroyType_, SocketDestroyType::DESTROY_DEFAULT_CELLULAR);

    socketType = static_cast<int>(SocketDestroyType::DESTROY_DEFAULT);
    netLinkSocketDiag.SetSocketDestroyType(socketType);
    EXPECT_EQ(netLinkSocketDiag.socketDestroyType_, SocketDestroyType::DESTROY_DEFAULT);
}

HWTEST_F(NetlinkSocketDiagTest, CloseNetlinkSocket001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    netLinkSocketDiag.dumpSock_ = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_INET_DIAG);
    netLinkSocketDiag.destroySock_ = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_INET_DIAG);
    netLinkSocketDiag.CloseNetlinkSocket();
    EXPECT_EQ(netLinkSocketDiag.dumpSock_, -1);
    EXPECT_EQ(netLinkSocketDiag.destroySock_, -1);
}

HWTEST_F(NetlinkSocketDiagTest, StripV4MappedPrefix001, TestSize.Level1)
{
    std::string result = NetLinkSocketDiag::StripV4MappedPrefix("::ffff:192.168.1.1");
    EXPECT_EQ(result, "192.168.1.1");
}

HWTEST_F(NetlinkSocketDiagTest, StripV4MappedPrefix002, TestSize.Level1)
{
    std::string result = NetLinkSocketDiag::StripV4MappedPrefix("192.168.1.1");
    EXPECT_EQ(result, "192.168.1.1");
}

HWTEST_F(NetlinkSocketDiagTest, StripV4MappedPrefix003, TestSize.Level1)
{
    std::string result = NetLinkSocketDiag::StripV4MappedPrefix("2001:db8::1");
    EXPECT_EQ(result, "2001:db8::1");
}

HWTEST_F(NetlinkSocketDiagTest, StripV4MappedPrefix004, TestSize.Level1)
{
    std::string result = NetLinkSocketDiag::StripV4MappedPrefix("::ffff:invalid");
    EXPECT_EQ(result, "::ffff:invalid");
}

HWTEST_F(NetlinkSocketDiagTest, StripV4MappedPrefix005, TestSize.Level1)
{
    std::string result = NetLinkSocketDiag::StripV4MappedPrefix("::1");
    EXPECT_EQ(result, "::1");
}

HWTEST_F(NetlinkSocketDiagTest, StripV4MappedPrefix006, TestSize.Level1)
{
    std::string result = NetLinkSocketDiag::StripV4MappedPrefix("");
    EXPECT_EQ(result, "");
}

HWTEST_F(NetlinkSocketDiagTest, StripV4MappedPrefix007, TestSize.Level1)
{
    std::string result = NetLinkSocketDiag::StripV4MappedPrefix("::ffff:10.0.0.1");
    EXPECT_EQ(result, "10.0.0.1");
}

HWTEST_F(NetlinkSocketDiagTest, StripV4MappedPrefix008, TestSize.Level1)
{
    std::string result = NetLinkSocketDiag::StripV4MappedPrefix("::ffff:127.0.0.1");
    EXPECT_EQ(result, "127.0.0.1");
}

HWTEST_F(NetlinkSocketDiagTest, GetTcpNetPortStatesInfoV4Mapped001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET6;
    msg.id.idiag_src[0] = 0;
    msg.id.idiag_src[1] = 0;
    msg.id.idiag_src[2] = htonl(0xffff);
    msg.id.idiag_src[3] = inet_addr("192.168.1.10");
    msg.id.idiag_dst[0] = 0;
    msg.id.idiag_dst[1] = 0;
    msg.id.idiag_dst[2] = htonl(0xffff);
    msg.id.idiag_dst[3] = inet_addr("10.0.0.1");
    msg.id.idiag_sport = htons(12345);
    msg.id.idiag_dport = htons(80);
    msg.idiag_uid = 1000;
    msg.idiag_state = TCP_ESTABLISHED;

    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetTcpNetPortStatesInfo(&msg, netPortStatesInfo, 100);
    EXPECT_TRUE(ret);
    ASSERT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_.size(), 1U);
    EXPECT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_[0].tcpLocalIp_, "192.168.1.10");
    EXPECT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_[0].tcpRemoteIp_, "10.0.0.1");
}

HWTEST_F(NetlinkSocketDiagTest, GetTcpNetPortStatesInfoV6Native001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET6;
    inet_pton(AF_INET6, "2001:db8::1", &msg.id.idiag_src);
    inet_pton(AF_INET6, "2001:db8::2", &msg.id.idiag_dst);
    msg.id.idiag_sport = htons(12345);
    msg.id.idiag_dport = htons(80);
    msg.idiag_uid = 1000;
    msg.idiag_state = TCP_ESTABLISHED;

    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetTcpNetPortStatesInfo(&msg, netPortStatesInfo, 100);
    EXPECT_TRUE(ret);
    ASSERT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_.size(), 1U);
    EXPECT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_[0].tcpLocalIp_, "2001:db8::1");
    EXPECT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_[0].tcpRemoteIp_, "2001:db8::2");
}

HWTEST_F(NetlinkSocketDiagTest, GetTcpNetPortStatesInfoNull001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetTcpNetPortStatesInfo(nullptr, netPortStatesInfo, 100);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetlinkSocketDiagTest, GetTcpNetPortStatesInfoV4Direct001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET;
    msg.id.idiag_src[0] = inet_addr("192.168.1.10");
    msg.id.idiag_dst[0] = inet_addr("10.0.0.1");
    msg.id.idiag_sport = htons(12345);
    msg.id.idiag_dport = htons(80);
    msg.idiag_uid = 1000;
    msg.idiag_state = TCP_ESTABLISHED;

    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetTcpNetPortStatesInfo(&msg, netPortStatesInfo, 100);
    EXPECT_TRUE(ret);
    ASSERT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_.size(), 1U);
    EXPECT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_[0].tcpLocalIp_, "192.168.1.10");
    EXPECT_EQ(netPortStatesInfo.tcpNetPortStatesInfo_[0].tcpRemoteIp_, "10.0.0.1");
}

HWTEST_F(NetlinkSocketDiagTest, GetUdpNetPortStatesInfoV4Mapped001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET6;
    msg.id.idiag_src[0] = 0;
    msg.id.idiag_src[1] = 0;
    msg.id.idiag_src[2] = htonl(0xffff);
    msg.id.idiag_src[3] = inet_addr("192.168.1.20");
    msg.id.idiag_sport = htons(54321);
    msg.idiag_uid = 2000;

    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetUdpNetPortStatesInfo(&msg, netPortStatesInfo, 200);
    EXPECT_TRUE(ret);
    ASSERT_EQ(netPortStatesInfo.udpNetPortStatesInfo_.size(), 1U);
    EXPECT_EQ(netPortStatesInfo.udpNetPortStatesInfo_[0].udpLocalIp_, "192.168.1.20");
}

HWTEST_F(NetlinkSocketDiagTest, GetTcpNetPortStatesInfoInvalidAddr001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_UNSPEC;

    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetTcpNetPortStatesInfo(&msg, netPortStatesInfo, 100);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetlinkSocketDiagTest, GetUdpNetPortStatesInfoInvalidAddr001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_UNSPEC;

    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetUdpNetPortStatesInfo(&msg, netPortStatesInfo, 200);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetlinkSocketDiagTest, GetUdpNetPortStatesInfoV6Native001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET6;
    inet_pton(AF_INET6, "fe80::1", &msg.id.idiag_src);
    msg.id.idiag_sport = htons(54321);
    msg.idiag_uid = 2000;

    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetUdpNetPortStatesInfo(&msg, netPortStatesInfo, 200);
    EXPECT_TRUE(ret);
    ASSERT_EQ(netPortStatesInfo.udpNetPortStatesInfo_.size(), 1U);
    EXPECT_EQ(netPortStatesInfo.udpNetPortStatesInfo_[0].udpLocalIp_, "fe80::1");
}

HWTEST_F(NetlinkSocketDiagTest, GetUdpNetPortStatesInfoNull001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetUdpNetPortStatesInfo(nullptr, netPortStatesInfo, 200);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetlinkSocketDiagTest, GetUdpNetPortStatesInfoV4Direct001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET;
    msg.id.idiag_src[0] = inet_addr("192.168.1.20");
    msg.id.idiag_sport = htons(54321);
    msg.idiag_uid = 2000;

    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    bool ret = netLinkSocketDiag.GetUdpNetPortStatesInfo(&msg, netPortStatesInfo, 200);
    EXPECT_TRUE(ret);
    ASSERT_EQ(netPortStatesInfo.udpNetPortStatesInfo_.size(), 1U);
    EXPECT_EQ(netPortStatesInfo.udpNetPortStatesInfo_[0].udpLocalIp_, "192.168.1.20");
}
} // namespace nmd
} // namespace OHOS
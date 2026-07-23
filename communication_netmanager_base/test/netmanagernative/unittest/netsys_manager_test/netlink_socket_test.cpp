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

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_manager_constants.h"
#include "netlink_socket.h"
#include "netlink_msg.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;

void DealInfoFromKernelTest001()
{
    uint16_t clearThing = RTM_DELROUTE;
    uint32_t table = 0;
    DealInfoFromKernel(nullptr, clearThing, table);
}

void DealInfoFromKernelTest002()
{
    uint16_t clearThing = 0;
    uint32_t table = 0;
    struct nlmsghdr hdr = {0};
    DealInfoFromKernel(&hdr, clearThing, table);
}
} // namespace
class NetlinkSocketTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetlinkSocketTest::SetUpTestCase()
{
}

void NetlinkSocketTest::TearDownTestCase() {}

void NetlinkSocketTest::SetUp() {}

void NetlinkSocketTest::TearDown() {}

HWTEST_F(NetlinkSocketTest, SendNetlinkMsgToKernelTest001, TestSize.Level1)
{
    DealInfoFromKernelTest001();
    DealInfoFromKernelTest002();
    uint32_t table = 0;
    auto ret = SendNetlinkMsgToKernel(nullptr, table);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetlinkSocketTest, ClearRouteInfoTest001, TestSize.Level1)
{
    uint16_t clearThing = RTM_GETROUTE;
    uint32_t table = 0;
    auto ret = ClearRouteInfo(clearThing, table);
    EXPECT_GE(ret, -1);
}

HWTEST_F(NetlinkSocketTest, ClearRouteInfoTest002, TestSize.Level1)
{
    uint16_t clearThing = RTM_GETRULE;
    uint32_t table = 0;
    auto ret = ClearRouteInfo(clearThing, table);
    EXPECT_GE(ret, -1);
}

HWTEST_F(NetlinkSocketTest, ClearRouteInfoTest003, TestSize.Level1)
{
    uint16_t clearThing = RTM_DELROUTE;
    uint32_t table = 0;
    auto ret = ClearRouteInfo(clearThing, table);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetlinkSocketTest, GetRoutePropertyTest001, TestSize.Level1)
{
    int32_t property = 0;
    auto ret = GetRouteProperty(nullptr, property);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetlinkSocketTest, MacArrayToStringTest001, TestSize.Level1)
{
    uint8_t macArray[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    std::string result = MacArrayToString(macArray);
    EXPECT_EQ(result, "00:11:22:33:44:55");
}

HWTEST_F(NetlinkSocketTest, MacArrayToStringTest002, TestSize.Level1)
{
    uint8_t* macArray = nullptr;
    std::string result = MacArrayToString(macArray);
    EXPECT_EQ(result, "");
}

HWTEST_F(NetlinkSocketTest, MacArrayToStringTest003, TestSize.Level1)
{
    const uint8_t mac[] = {};
    std::string result = MacArrayToString(mac);
    EXPECT_EQ(result, "00:00:00:00:00:00");
}

HWTEST_F(NetlinkSocketTest, MacArrayToStringTest004, TestSize.Level1)
{
    const uint8_t mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    std::string result = MacArrayToString(mac);
    EXPECT_EQ(result, "00:00:00:00:00:00");
}

HWTEST_F(NetlinkSocketTest, MacArrayToStringTest005, TestSize.Level1)
{
    const uint8_t mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    std::string result = MacArrayToString(mac);
    EXPECT_EQ(result, "00:00:00:00:00:00");
}

HWTEST_F(NetlinkSocketTest, ReceiveMsgFromKernelTest001, TestSize.Level1)
{
    uint32_t table = 0;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfo;
    auto ret = ReceiveMsgFromKernel(nullptr, table, reinterpret_cast<void*>(&ipMacInfo));
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetlinkSocketTest, ReceiveMsgFromKernelTest002, TestSize.Level1)
{
    uint32_t table = 0;
    struct nlmsghdr netlinkMessage;
    auto ret = ReceiveMsgFromKernel(&netlinkMessage, table, nullptr);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetlinkSocketTest, ReceiveMsgFromKernelTest003, TestSize.Level1)
{
    uint32_t table = 0;
    auto ret = ReceiveMsgFromKernel(nullptr, table, nullptr);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetlinkSocketTest, ReceiveMsgFromKernelTest004, TestSize.Level1)
{
    uint32_t table = 0;
    nmd::NetlinkMsg nlmsg(NLM_F_ACK, nmd::NETLINK_MAX_LEN, 0);
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfo;
    auto ret = ReceiveMsgFromKernel(nlmsg.GetNetLinkMessage(), table, reinterpret_cast<void*>(&ipMacInfo));
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetlinkSocketTest, DealRcvMsgFromKernelTest001, TestSize.Level1)
{
    uint16_t type = 0;
    uint32_t table = 0;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfo;
    DealRcvMsgFromKernel(nullptr, type, table, reinterpret_cast<void*>(&ipMacInfo));
    EXPECT_EQ(type, 0);
}

HWTEST_F(NetlinkSocketTest, DealRcvMsgFromKernelTest002, TestSize.Level1)
{
    uint16_t type = 0;
    uint32_t table = 0;
    nlmsghdr nlmsgHeader;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfo;
    DealRcvMsgFromKernel(&nlmsgHeader, type, table, nullptr);
    EXPECT_EQ(type, 0);
}

HWTEST_F(NetlinkSocketTest, DealRcvMsgFromKernelTest003, TestSize.Level1)
{
    uint32_t table = 0;
    nlmsghdr nlmsgHeader;
    nlmsgHeader.nlmsg_type = RTM_GETRULE;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfo;
    DealRcvMsgFromKernel(&nlmsgHeader, RTM_GETRULE, table, reinterpret_cast<void*>(&ipMacInfo));
    EXPECT_EQ(table, 0);
}

HWTEST_F(NetlinkSocketTest, DealNeighInfoTest001, TestSize.Level1)
{
    uint16_t type = 0;
    uint32_t table = 0;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfoVec;
    DealNeighInfo(nullptr, type, table, ipMacInfoVec);
    EXPECT_EQ(type, 0);
}

HWTEST_F(NetlinkSocketTest, DealNeighInfoTest002, TestSize.Level1)
{
    uint32_t table = 0;
    nlmsghdr nlmsgHeader;
    nlmsgHeader.nlmsg_type = RTM_GETRULE;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfoVec;
    DealNeighInfo(&nlmsgHeader, nlmsgHeader.nlmsg_type, table, ipMacInfoVec);
    EXPECT_EQ(table, 0);
}

HWTEST_F(NetlinkSocketTest, DealNeighInfoTest003, TestSize.Level1)
{
    uint32_t table = 0;
    nlmsghdr nlmsgHeader;
    nlmsgHeader.nlmsg_type = RTM_NEWNEIGH;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfoVec;
    DealNeighInfo(&nlmsgHeader, nlmsgHeader.nlmsg_type, table, ipMacInfoVec);
    EXPECT_EQ(table, 0);
}

HWTEST_F(NetlinkSocketTest, DealNeighInfoTest004, TestSize.Level1)
{
    uint32_t table = 0;
    nlmsghdr nlmsgHeader;
    nlmsgHeader.nlmsg_type = RTM_DELNEIGH;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfoVec;
    DealNeighInfo(&nlmsgHeader, nlmsgHeader.nlmsg_type, table, ipMacInfoVec);
    EXPECT_EQ(table, 0);
}

HWTEST_F(NetlinkSocketTest, DealNeighInfoTest005, TestSize.Level1)
{
    uint32_t table = 0;
    nlmsghdr nlmsgHeader;
    nlmsgHeader.nlmsg_type = RTM_GETNEIGH;
    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfoVec;
    DealNeighInfo(&nlmsgHeader, nlmsgHeader.nlmsg_type, table, ipMacInfoVec);
    EXPECT_EQ(table, 0);
}

HWTEST_F(NetlinkSocketTest, DealNeighInfoTest006, TestSize.Level1)
{
    uint32_t table = 0;
    const size_t bufferSize = NLMSG_SPACE(sizeof(struct ndmsg));
    char buffer[bufferSize] = {0};
    nlmsghdr *nlmsgHeader = reinterpret_cast<nlmsghdr*>(buffer);
    nlmsgHeader->nlmsg_type = RTM_GETNEIGH;
    ndmsg *ndm = reinterpret_cast<ndmsg *>(NLMSG_DATA(&nlmsgHeader));
    ndm->ndm_type = RTN_UNICAST;

    std::vector<NetManagerStandard::NetIpMacInfo> ipMacInfoVec;
    DealNeighInfo(nlmsgHeader, nlmsgHeader->nlmsg_type, table, ipMacInfoVec);
    EXPECT_EQ(table, 0);
}

HWTEST_F(NetlinkSocketTest, SendNetlinkMsgsToKernelTest001, TestSize.Level1)
{
    std::vector<NetlinkMsg> msgs;
    auto ret = SendNetlinkMsgsToKernel(msgs);
    EXPECT_EQ(ret, -1);
    msgs.emplace_back(NLM_F_REQUEST | NLM_F_CREATE, NETLINK_MAX_LEN, 0);
    msgs.emplace_back(NLM_F_REQUEST | NLM_F_REPLACE, NETLINK_MAX_LEN, 0);
    msgs.emplace_back(NLM_F_REQUEST | NLM_F_EXCL, NETLINK_MAX_LEN, 0);
    ret = SendNetlinkMsgsToKernel(msgs);
    EXPECT_EQ(ret, -1);
    msgs.emplace_back(NLM_F_REQUEST | NLM_F_EXCL, NETLINK_MAX_LEN, getpid());
    ret = SendNetlinkMsgsToKernel(msgs);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetlinkSocketTest, SendNetlinkMsgsToKernelTest002, TestSize.Level1)
{
    std::vector<NetlinkMsg> msgs;
    NetlinkMsg msg(NLM_F_REQUEST, NETLINK_MAX_LEN, getpid());

    struct rtmsg routeMsg = {0};
    routeMsg.rtm_family = AF_INET;
    routeMsg.rtm_dst_len = 24;
    msg.AddRoute(RTM_NEWROUTE, routeMsg);
    msgs.push_back(std::move(msg));

    auto ret = SendNetlinkMsgsToKernel(msgs);
    EXPECT_EQ(ret, 28);
}
} // namespace nmd
} // namespace OHOS
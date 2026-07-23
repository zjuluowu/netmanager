/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "netlink_msg.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace OHOS::nmd;
class NetlinkMsgTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetlinkMsgTest::SetUpTestCase() {}

void NetlinkMsgTest::TearDownTestCase() {}

void NetlinkMsgTest::SetUp() {}

void NetlinkMsgTest::TearDown() {}

HWTEST_F(NetlinkMsgTest, AddRouteTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("AddRouteTest001 enter");
    uint16_t flags = 2;
    size_t maxBufLen = 50;
    int32_t pid = 513;
    NetlinkMsg netLinkMsg(flags, maxBufLen, pid);
    rtmsg rtmsg = {
        .rtm_family = 0,
        .rtm_dst_len = 10,
        .rtm_src_len = 10,
    };
    uint16_t action = 2;
    netLinkMsg.AddRoute(action, rtmsg);
    fib_rule_hdr hdr = {
        .family = 0,
        .dst_len = 10,
        .src_len = 10,
    };
    ifaddrmsg addrmsg = {
        .ifa_family = 0,
        .ifa_prefixlen = 10,
        .ifa_flags = 1,
    };
    netLinkMsg.AddRule(action, hdr);
    netLinkMsg.AddAddress(action, addrmsg);

    uint16_t action1 = 28;
    uint32_t index = 1;
    struct ndmsg ndm = {};
    ndm.ndm_family = AF_INET6;
    ndm.ndm_ifindex = index;
    ndm.ndm_pad1 = 0;
    ndm.ndm_pad2 = 0;
    ndm.ndm_state = NUD_PERMANENT;
    ndm.ndm_flags = 0;
    ndm.ndm_type = RTN_UNICAST;
    netLinkMsg.AddNeighbor(action1, ndm);
    size_t dataLength100 = 100;
    size_t dataLength10 = 10;
    int32_t ret = netLinkMsg.AddAttr(action, nullptr, dataLength100);
    EXPECT_EQ(ret, -1);
    char temp[10] = "123456789";
    char* data = temp;
    ret = netLinkMsg.AddAttr(action, data, dataLength100);
    EXPECT_EQ(ret, -1);
    ret = netLinkMsg.AddAttr(action, data, dataLength10);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetlinkMsgTest, AddLinkTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("AddLinkTest001 enter");
    uint16_t flags = 2;
    size_t maxBufLen = 50;
    int32_t pid = 513;
    NetlinkMsg netLinkMsg(flags, maxBufLen, pid);

    uint16_t action = RTM_NEWLINK;
    struct ifinfomsg ifm;
    ifm.ifi_family = AF_UNSPEC;
    ifm.ifi_type = 1;
    ifm.ifi_index = 0;
    ifm.ifi_flags = 0;
    ifm.ifi_change = 0;
    netLinkMsg.AddLink(action, ifm);

    struct nlattr *linkinfo = netLinkMsg.AddNestedStart(IFLA_LINKINFO);
    struct nlattr *datainfo = nullptr;
    netLinkMsg.AddNestedEnd(datainfo);
    netLinkMsg.AddNestedEnd(linkinfo);
    
    size_t dataLength10 = 10;
    char temp[10] = "123456789";
    char* data = temp;
    int32_t ret = netLinkMsg.AddAttr(action, data, dataLength10);
    EXPECT_EQ(ret, -1);
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
HWTEST_F(NetlinkMsgTest, InitNflogConfigTest001, TestSize.Level1)
{
    uint16_t flags = 0;
    size_t maxBufLen = sizeof(struct nlmsghdr) + sizeof(struct nfgenmsg);
    int32_t pid = 0;
    auto netlinkMsgPtr = std::make_shared<NetlinkMsg>(flags, maxBufLen, pid);
    auto result = netlinkMsgPtr->InitNflogConfig(1);
    EXPECT_EQ(result, true);
    netlinkMsgPtr->netlinkMessage_ = nullptr;
    result = netlinkMsgPtr->InitNflogConfig(1);
    EXPECT_EQ(result, false);
    netlinkMsgPtr->maxBufLen_ = 0;
    result = netlinkMsgPtr->InitNflogConfig(1);
    EXPECT_EQ(result, false);
}

HWTEST_F(NetlinkMsgTest, AddNlattrTest001, TestSize.Level1)
{
    uint16_t flags = 0;
    size_t maxBufLen = sizeof(struct nlmsghdr) + sizeof(struct nfgenmsg);
    int32_t pid = 0;
    auto netlinkMsgPtr = std::make_shared<NetlinkMsg>(flags, maxBufLen, pid);
    nfulnl_msg_config_cmd cmd{.command = NFULNL_CFG_CMD_BIND};
    auto result = netlinkMsgPtr->AddNlattr(NFULA_CFG_CMD, &cmd, sizeof(cmd));
    EXPECT_EQ(result, true);
    netlinkMsgPtr->netlinkMessage_ = nullptr;
    result = netlinkMsgPtr->AddNlattr(NFULA_CFG_CMD, &cmd, sizeof(cmd));
    EXPECT_EQ(result, false);
    result = netlinkMsgPtr->AddNlattr(NFULA_CFG_CMD, nullptr, sizeof(cmd));
    EXPECT_EQ(result, false);
    result = netlinkMsgPtr->AddNlattr(NFULA_CFG_CMD, &cmd, 0);
    EXPECT_EQ(result, false);
}
#endif
} // namespace NetsysNative
} // namespace OHOS

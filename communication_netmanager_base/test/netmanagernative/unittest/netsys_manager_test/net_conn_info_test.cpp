/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "net_conn_info.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing;
using namespace testing::ext;
const string LOCAL_IP = "192.168.10.1";
const string REMOTE_IP = "192.168.10.2";
const string LOCAL_IP_V6 = "1111:2222:3333:4444:5555:6666:7777:8888";
const string REMOTE_IP_V6 = "1111:2222:3333:4444:5555:6666:7777:9999";
constexpr uint16_t LOCAL_PORT = 1111;
constexpr uint16_t REMOTE_PORT = 2222;
void GetFiveTupleInfoIpv4(NetConnInfo &info)
{
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = LOCAL_IP;
    info.localPort_ = LOCAL_PORT;
    info.remoteAddress_ = REMOTE_IP;
    info.remotePort_ = REMOTE_PORT;
}

void GetFiveTupleInfoIpv6(NetConnInfo &info)
{
    info.protocolType_ = IPPROTO_UDP;
    info.family_ = NetConnInfo::Family::IPv6;
    info.localAddress_ = LOCAL_IP_V6;
    info.localPort_ = LOCAL_PORT;
    info.remoteAddress_ = REMOTE_IP_V6;
    info.remotePort_ = REMOTE_PORT;
}
} // namespace

class NetFiveTupleInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetFiveTupleInfoTest::SetUpTestCase() {}

void NetFiveTupleInfoTest::TearDownTestCase() {}

void NetFiveTupleInfoTest::SetUp() {}

void NetFiveTupleInfoTest::TearDown() {}

HWTEST_F(NetFiveTupleInfoTest, NetFiveTupleInfoTest001, TestSize.Level1)
{
    Parcel parcel;
    NetConnInfo info;
    GetFiveTupleInfoIpv4(info);
    EXPECT_TRUE(info.Marshalling(parcel));

    sptr<NetConnInfo> result = NetConnInfo::Unmarshalling(parcel);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(info.protocolType_, result->protocolType_);
    EXPECT_EQ(info.family_, result->family_);
    EXPECT_EQ(info.localAddress_, result->localAddress_);
    EXPECT_EQ(info.localPort_, result->localPort_);
    EXPECT_EQ(info.remoteAddress_, result->remoteAddress_);
    EXPECT_EQ(info.remotePort_, result->remotePort_);
}

HWTEST_F(NetFiveTupleInfoTest, NetFiveTupleInfoTest002, TestSize.Level1)
{
    NetConnInfo info;
    GetFiveTupleInfoIpv4(info);
    EXPECT_TRUE(info.CheckValid());

    info.protocolType_ = 100;
    EXPECT_FALSE(info.CheckValid());

    GetFiveTupleInfoIpv4(info);
    info.family_ = static_cast<NetConnInfo::Family>(100);
    EXPECT_FALSE(info.CheckValid());

    GetFiveTupleInfoIpv4(info);
    info.localAddress_ = "incorrect addr";
    EXPECT_FALSE(info.CheckValid());

    GetFiveTupleInfoIpv4(info);
    info.remoteAddress_ = "incorrect addr";
    EXPECT_FALSE(info.CheckValid());

    GetFiveTupleInfoIpv6(info);
    EXPECT_TRUE(info.CheckValid());

    info.localAddress_ = "incorrect addr";
    EXPECT_FALSE(info.CheckValid());

    GetFiveTupleInfoIpv6(info);
    info.remoteAddress_ = "incorrect addr";
    EXPECT_FALSE(info.CheckValid());
}
} // namespace NetManagerStandard
} // namespace OHOS
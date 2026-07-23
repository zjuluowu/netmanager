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

#include "message_parcel.h"
#include "net_link_info.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *TEST_IPV4_ADDR = "127.0.0.1";
} // namespace
class NetLinkInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetLinkInfoTest::SetUpTestCase() {}

void NetLinkInfoTest::TearDownTestCase() {}

void NetLinkInfoTest::SetUp() {}

void NetLinkInfoTest::TearDown() {}

sptr<NetLinkInfo> GetNetLinkInfo()
{
    sptr<NetLinkInfo> netLinkInfo = (std::make_unique<NetLinkInfo>()).release();
    netLinkInfo->ifaceName_ = "test";
    netLinkInfo->domain_ = "test";

    sptr<INetAddr> netAddr = (std::make_unique<INetAddr>()).release();
    netAddr->type_ = INetAddr::IPV4;
    netAddr->family_ = 0x10;
    netAddr->prefixlen_ = 0x17;
    netAddr->address_ = "0.0.0.0";
    netAddr->netMask_ = "0.0.0.0";
    netAddr->hostName_ = "netAddr";
    netLinkInfo->netAddrList_.push_back(*netAddr);

    sptr<Route> route = (std::make_unique<Route>()).release();
    route->iface_ = "iface0";
    route->destination_.type_ = INetAddr::IPV4;
    route->destination_.family_ = 0x10;
    route->destination_.prefixlen_ = 0x17;
    route->destination_.address_ = "0.0.0.0";
    route->destination_.netMask_ = "0.0.0.0";
    route->destination_.hostName_ = "netAddr";
    route->gateway_.type_ = INetAddr::IPV4;
    route->gateway_.family_ = 0x10;
    route->gateway_.prefixlen_ = 0x17;
    route->gateway_.address_ = "0.0.0.0";
    route->gateway_.netMask_ = "0.0.0.0";
    route->gateway_.hostName_ = "netAddr";
    netLinkInfo->routeList_.push_back(*route);

    netLinkInfo->mtu_ = 0x5DC;

    netLinkInfo->httpProxy_ = {TEST_IPV4_ADDR, 80, {"localhost"}};
    return netLinkInfo;
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: Test NetLinkInfo::Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(NetLinkInfoTest, UnmarshallingTest, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = GetNetLinkInfo();
    ASSERT_TRUE(netLinkInfo != nullptr);

    MessageParcel data;
    sptr<NetLinkInfo> netLinkInfo_ptr = nullptr;
    bool bRet = NetLinkInfo::Marshalling(data, netLinkInfo);
    ASSERT_TRUE(bRet == true);

    netLinkInfo_ptr = NetLinkInfo::Unmarshalling(data);
    ASSERT_TRUE(netLinkInfo_ptr != nullptr);
}

/**
 * @tc.name: InitializeTest
 * @tc.desc: Test NetLinkInfo::Initialize
 * @tc.type: FUNC
 */
HWTEST_F(NetLinkInfoTest, InitializeTest, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = GetNetLinkInfo();
    ASSERT_TRUE(netLinkInfo != nullptr);
    netLinkInfo->Initialize();
}

/**
 * @tc.name: ToStringTest
 * @tc.desc: Test NetLinkInfo::ToString
 * @tc.type: FUNC
 */
HWTEST_F(NetLinkInfoTest, ToStringTest, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = GetNetLinkInfo();
    EXPECT_NE(netLinkInfo, nullptr);
    std::string str = netLinkInfo->ToString("testTab");
    int32_t ret = 0;
    NETMGR_LOG_D("netLinkInfo.ToString string is : [%{public}s]", str.c_str());
    if (str.c_str() != nullptr) {
        ret = 1;
    }
}

/**
 * @tc.name: ToStringAddrTest
 * @tc.desc: Test NetLinkInfo::ToStringAddr
 * @tc.type: FUNC
 */
HWTEST_F(NetLinkInfoTest, ToStringAddrTest, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = GetNetLinkInfo();
    EXPECT_NE(netLinkInfo, nullptr);
    std::string str = netLinkInfo->ToStringAddr("testAddrTab");
    int32_t ret = 0;
    NETMGR_LOG_D("netLinkInfo.ToString string is : [%{public}s]", str.c_str());
    if (str.c_str() != nullptr) {
        ret = 1;
    }
}

/**
 * @tc.name: ToStringDnsTest
 * @tc.desc: Test NetLinkInfo::ToStringDns
 * @tc.type: FUNC
 */
HWTEST_F(NetLinkInfoTest, ToStringDnsTest, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = GetNetLinkInfo();
    EXPECT_NE(netLinkInfo, nullptr);
    std::string str = netLinkInfo->ToStringDns("testDnsTab");
    int32_t ret = 0;
    NETMGR_LOG_D("netLinkInfo.ToString string is : [%{public}s]", str.c_str());
    if (str.c_str() != nullptr) {
        ret = 1;
    }
}

/**
 * @tc.name: ToStringRouteTest
 * @tc.desc: Test NetLinkInfo::ToStringRoute
 * @tc.type: FUNC
 */
HWTEST_F(NetLinkInfoTest, ToStringRouteTest, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = GetNetLinkInfo();
    EXPECT_NE(netLinkInfo, nullptr);
    std::string str = netLinkInfo->ToStringRoute("testRouteTab");
    int32_t ret = 0;
    NETMGR_LOG_D("netLinkInfo.ToString string is : [%{public}s]", str.c_str());
    if (str.c_str() != nullptr) {
        ret = 1;
    }
}

/**
 * @tc.name: operatorAndMarshalling
 * @tc.desc: Test NetLinkInfo::operatorAndMarshalling
 * @tc.type: FUNC
 */
HWTEST_F(NetLinkInfoTest, operatorAndMarshallingTest, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = GetNetLinkInfo();
    ASSERT_TRUE(netLinkInfo != nullptr);
    NetLinkInfo netLinkInfoa = *netLinkInfo;
    EXPECT_EQ(netLinkInfoa.domain_, "test");
    Parcel data;
    bool bRet = netLinkInfo->Marshalling(data);
    ASSERT_TRUE(bRet == true);
}
} // namespace NetManagerStandard
} // namespace OHOS

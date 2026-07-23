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
#include "net_ip_mac_info.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class NetIpMacInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetIpMacInfoTest::SetUpTestCase() {}

void NetIpMacInfoTest::TearDownTestCase() {}

void NetIpMacInfoTest::SetUp() {}

void NetIpMacInfoTest::TearDown() {}

sptr<NetIpMacInfo> GetNetIpMacInfo()
{
    sptr<NetIpMacInfo> netIpMacInfo = (std::make_unique<NetIpMacInfo>()).release();
    netIpMacInfo->ipAddress_ = "test";
    netIpMacInfo->iface_ = "test";
    netIpMacInfo->macAddress_ = "test";
    netIpMacInfo->family_ = FAMILY_INVALID;

    return netIpMacInfo;
}

HWTEST_F(NetIpMacInfoTest, UnmarshallingTest001, TestSize.Level1)
{
    sptr<NetIpMacInfo> netIpMacInfo = GetNetIpMacInfo();
    ASSERT_TRUE(netIpMacInfo != nullptr);

    MessageParcel data;
    sptr<NetIpMacInfo> netIpMacInfo_ptr = nullptr;
    bool bRet = NetIpMacInfo::Marshalling(data, netIpMacInfo);
    ASSERT_TRUE(bRet);

    netIpMacInfo_ptr = NetIpMacInfo::Unmarshalling(data);
    ASSERT_TRUE(netIpMacInfo_ptr != nullptr);
}

HWTEST_F(NetIpMacInfoTest, operatorAndMarshallingTest001, TestSize.Level1)
{
    sptr<NetIpMacInfo> netIpMacInfo = GetNetIpMacInfo();
    ASSERT_TRUE(netIpMacInfo != nullptr);
    NetIpMacInfo netIpMacInfoa = *netIpMacInfo;
    EXPECT_EQ(netIpMacInfoa.ipAddress_, "test");
    Parcel data;
    bool bRet = netIpMacInfo->Marshalling(data);
    ASSERT_TRUE(bRet);
}

} // namespace NetManagerStandard
} // namespace OHOS
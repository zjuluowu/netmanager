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
#include "net_port_states_info.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class NetPortStatesInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetPortStatesInfoTest::SetUpTestCase() {}

void NetPortStatesInfoTest::TearDownTestCase() {}

void NetPortStatesInfoTest::SetUp() {}

void NetPortStatesInfoTest::TearDown() {}

sptr<NetPortStatesInfo> GetSystemNetPortStates()
{
    sptr<NetPortStatesInfo> netPortStatesInfo = (std::make_unique<NetPortStatesInfo>()).release();
    TcpNetPortStatesInfo tcpInfo;
    tcpInfo.tcpLocalIp_ = "0.0.0.0";
    tcpInfo.tcpLocalPort_ = 1;
    tcpInfo.tcpRemoteIp_ = "0.0.0.0";
    tcpInfo.tcpRemotePort_ = 1;
    tcpInfo.tcpUid_ = 1;
    tcpInfo.tcpPid_ = 1;
    tcpInfo.tcpState_ = 1;
    netPortStatesInfo->tcpNetPortStatesInfo_.emplace_back(tcpInfo);

    UdpNetPortStatesInfo udpInfo;
    udpInfo.udpLocalIp_ = "0.0.0.0";
    udpInfo.udpLocalPort_ = 1;
    udpInfo.udpUid_ = 1;
    udpInfo.udpPid_ = 1;
    netPortStatesInfo->udpNetPortStatesInfo_.emplace_back(udpInfo);

    return netPortStatesInfo;
}

HWTEST_F(NetPortStatesInfoTest, UnmarshallingTest001, TestSize.Level1)
{
    sptr<NetPortStatesInfo> netPortStatesInfo = GetSystemNetPortStates();
    ASSERT_TRUE(netPortStatesInfo != nullptr);

    MessageParcel data;
    sptr<NetPortStatesInfo> netPortStatesInfo_ptr = nullptr;
    bool bRet = NetPortStatesInfo::Marshalling(data, netPortStatesInfo);
    ASSERT_TRUE(bRet);

    netPortStatesInfo_ptr = NetPortStatesInfo::Unmarshalling(data);
    ASSERT_TRUE(netPortStatesInfo_ptr != nullptr);
}

HWTEST_F(NetPortStatesInfoTest, operatorAndMarshallingTest001, TestSize.Level1)
{
    sptr<NetPortStatesInfo> netPortStatesInfo = GetSystemNetPortStates();
    ASSERT_TRUE(netPortStatesInfo != nullptr);
    NetPortStatesInfo netPortStatesInfos = *netPortStatesInfo;
    EXPECT_EQ(netPortStatesInfos.tcpNetPortStatesInfo_.back().tcpLocalPort_, 1);
    Parcel data;
    bool bRet = netPortStatesInfo->Marshalling(data);
    ASSERT_TRUE(bRet);
}

} // namespace NetManagerStandard
} // namespace OHOS
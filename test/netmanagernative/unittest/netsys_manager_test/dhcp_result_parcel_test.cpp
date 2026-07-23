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

#include "dhcp_result_parcel.h"

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace testing::ext;
const std::string IFACE = "iface0";
const std::string IP_ADDR = "127.0.0.1";
const std::string GATE_WAY = "255.255.255.128";
const std::string SUB_NET = "127.0.0.1";
const std::string ROUTE_1 = "route";
const std::string ROUTE_2 = "route";
const std::string DNS_1 = "0.0.0.0";
const std::string DNS_2 = "8.8.8.8";
DhcpResultParcel GetDhcpResultParcelData()
{
    DhcpResultParcel info;
    info.iface_ = IFACE;
    info.ipAddr_ = IP_ADDR;
    info.gateWay_ = GATE_WAY;
    info.subNet_ = SUB_NET;
    info.route1_ = ROUTE_1;
    info.route2_ = ROUTE_2;
    info.dns1_ = DNS_1;
    info.dns2_ = DNS_2;
    return info;
}
} // namespace

class DhcpResultParcelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DhcpResultParcelTest::SetUpTestCase() {}

void DhcpResultParcelTest::TearDownTestCase() {}

void DhcpResultParcelTest::SetUp() {}

void DhcpResultParcelTest::TearDown() {}

HWTEST_F(DhcpResultParcelTest, InterfaceTest001, TestSize.Level1)
{
    DhcpResultParcel dhcpResultParcel = GetDhcpResultParcelData();
    Parcel parcel;
    bool isMarshlling = dhcpResultParcel.Marshalling(parcel);
    EXPECT_TRUE(isMarshlling);

    sptr<DhcpResultParcel> resultParcel = DhcpResultParcel::Unmarshalling(parcel);
    EXPECT_NE(resultParcel, nullptr);
}
} // namespace nmd
} // namespace OHOS

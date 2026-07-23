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

#ifdef GTEST_API_
    #define private public
    #define protected public
#endif
#include "net_all_capabilities.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class NetAllCapabilitiesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetAllCapabilitiesTest::SetUpTestCase() {}

void NetAllCapabilitiesTest::TearDownTestCase() {}

void NetAllCapabilitiesTest::SetUp() {}

void NetAllCapabilitiesTest::TearDown() {}

HWTEST_F(NetAllCapabilitiesTest, ToStringTest, TestSize.Level1)
{
    auto allCap = std::make_shared<NetAllCapabilities>();
    std::string tab = "tab";
    std::string result = allCap->ToString(tab);
    EXPECT_FALSE(result.empty());
    bool ret = allCap->CapsIsValid();
    EXPECT_TRUE(allCap->CapsIsNull());
    std::set<NetCap> caps = {
        NetCap::NET_CAPABILITY_MMS,
        NetCap::NET_CAPABILITY_NOT_METERED,
        NetCap::NET_CAPABILITY_INTERNET,
        NetCap::NET_CAPABILITY_NOT_VPN,
        NetCap::NET_CAPABILITY_VALIDATED,
        NetCap::NET_CAPABILITY_PORTAL,
        NetCap::NET_CAPABILITY_INTERNAL_DEFAULT,
        NetCap::NET_CAPABILITY_END
    };
    allCap->netCaps_ = caps;
    EXPECT_FALSE(allCap->CapsIsNull());
    std::set<NetBearType> bearType = {
        NetBearType::BEARER_CELLULAR,
        NetBearType::BEARER_WIFI,
        NetBearType::BEARER_BLUETOOTH,
        NetBearType::BEARER_ETHERNET,
        NetBearType::BEARER_VPN,
        NetBearType::BEARER_WIFI_AWARE,
        NetBearType::BEARER_DEFAULT
    };
    allCap->bearerTypes_ = bearType;
    result = allCap->ToString(tab);
    EXPECT_FALSE(result.empty());
    ret = allCap->CapsIsValid();
    EXPECT_FALSE(ret);
    allCap->netCaps_.clear();
    EXPECT_FALSE(allCap->CapsIsNull());
    allCap->bearerTypes_.clear();
    allCap->linkUpBandwidthKbps_ = 1;
    EXPECT_FALSE(allCap->CapsIsNull());
    allCap->linkUpBandwidthKbps_ = 0;
    allCap->linkDownBandwidthKbps_ = 1;
    EXPECT_FALSE(allCap->CapsIsNull());
}

HWTEST_F(NetAllCapabilitiesTest, ConvertCapToStrTest001, TestSize.Level1)
{
    auto allCap = std::make_shared<NetAllCapabilities>();
    std::string str;

    allCap->ConvertCapToStr(NET_CAPABILITY_MMS, str);
    EXPECT_EQ(str, "NET_CAPABILITY_MMS");

    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_INTERNET, str);
    EXPECT_EQ(str, "NET_CAPABILITY_INTERNET");

    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_NOT_METERED, str);
    EXPECT_EQ(str, "NET_CAPABILITY_NOT_METERED");

    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_VALIDATED, str);
    EXPECT_EQ(str, "NET_CAPABILITY_VALIDATED");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_BIP, str);
    EXPECT_EQ(str, "NET_CAPABILITY_BIP");
}

HWTEST_F(NetAllCapabilitiesTest, ConvertCapToStrTest002, TestSize.Level1)
{
    auto allCap = std::make_shared<NetAllCapabilities>();
    std::string str;
    
    allCap->ConvertCapToStr(NET_CAPABILITY_SUPL, str);
    EXPECT_EQ(str, "NET_CAPABILITY_SUPL");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_IA, str);
    EXPECT_EQ(str, "NET_CAPABILITY_IA");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_XCAP, str);
    EXPECT_EQ(str, "NET_CAPABILITY_XCAP");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_NOT_VPN, str);
    EXPECT_EQ(str, "NET_CAPABILITY_NOT_VPN");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_PORTAL, str);
    EXPECT_EQ(str, "NET_CAPABILITY_PORTAL");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_INTERNAL_DEFAULT, str);
    EXPECT_EQ(str, "NET_CAPABILITY_INTERNAL_DEFAULT");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_CHECKING_CONNECTIVITY, str);
    EXPECT_EQ(str, "NET_CAPABILITY_CHECKING_CONNECTIVITY");
}

HWTEST_F(NetAllCapabilitiesTest, ConvertCapToStrTest003, TestSize.Level1)
{
    auto allCap = std::make_shared<NetAllCapabilities>();
    std::string str;
    
    allCap->ConvertCapToStr(NET_CAPABILITY_END, str);
    EXPECT_EQ(str, "unknown NetCap");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_OEM_PAID, str);
    EXPECT_EQ(str, "unknown NetCap");
    
    str.clear();
    allCap->ConvertCapToStr(NET_CAPABILITY_OEM_PRIVATE, str);
    EXPECT_EQ(str, "unknown NetCap");
}

HWTEST_F(NetAllCapabilitiesTest, ParcelTest, TestSize.Level1)
{
    auto allCap = std::make_shared<NetAllCapabilities>();
    std::set<NetCap> caps = {
        NetCap::NET_CAPABILITY_MMS,
        NetCap::NET_CAPABILITY_NOT_METERED,
        NetCap::NET_CAPABILITY_INTERNET,
        NetCap::NET_CAPABILITY_NOT_VPN,
        NetCap::NET_CAPABILITY_VALIDATED,
        NetCap::NET_CAPABILITY_PORTAL,
        NetCap::NET_CAPABILITY_INTERNAL_DEFAULT,
        NetCap::NET_CAPABILITY_END
    };
    allCap->netCaps_ = caps;
    std::set<NetBearType> bearType = {
        NetBearType::BEARER_CELLULAR,
        NetBearType::BEARER_WIFI,
        NetBearType::BEARER_BLUETOOTH,
        NetBearType::BEARER_ETHERNET,
        NetBearType::BEARER_VPN,
        NetBearType::BEARER_WIFI_AWARE,
        NetBearType::BEARER_DEFAULT
    };
    allCap->bearerTypes_ = bearType;
    allCap->linkUpBandwidthKbps_ = allCap->linkDownBandwidthKbps_ = 1;
    Parcel other1;
    allCap->Marshalling(other1);
    NetAllCapabilities other;
    other.Unmarshalling(other1);
    EXPECT_EQ(other.netCaps_.size(), caps.size() - 1);
}
} // namespace NetManagerStandard
} // namespace OHOS
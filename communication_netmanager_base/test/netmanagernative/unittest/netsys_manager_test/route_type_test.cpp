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

#include "message_parcel.h"
#include "route_type.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
using namespace testing::ext;

class NetworkRouteInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetworkRouteInfoTest::SetUpTestCase() {}

void NetworkRouteInfoTest::TearDownTestCase() {}

void NetworkRouteInfoTest::SetUp() {}

void NetworkRouteInfoTest::TearDown() {}

/**
 * @tc.name: MarshallingTest001
 * @tc.desc: Test NetworkRouteInfo::Marshalling with valid data
 * @tc.type: FUNC
 */
HWTEST_F(NetworkRouteInfoTest, MarshallingTest001, TestSize.Level1)
{
    sptr<NetworkRouteInfo> routeInfo = new (std::nothrow) NetworkRouteInfo();
    ASSERT_NE(routeInfo, nullptr);

    routeInfo->ifName = "eth0";
    routeInfo->destination = "192.168.1.0/24";
    routeInfo->nextHop = "192.168.1.1";
    routeInfo->isExcludedRoute = false;

    MessageParcel parcel;
    bool ret = routeInfo->Marshalling(parcel);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: MarshallingTest002
 * @tc.desc: Test NetworkRouteInfo::Marshalling with empty strings
 * @tc.type: FUNC
 */
HWTEST_F(NetworkRouteInfoTest, MarshallingTest002, TestSize.Level1)
{
    sptr<NetworkRouteInfo> routeInfo = new (std::nothrow) NetworkRouteInfo();
    ASSERT_NE(routeInfo, nullptr);

    routeInfo->ifName = "";
    routeInfo->destination = "";
    routeInfo->nextHop = "";
    routeInfo->isExcludedRoute = true;

    MessageParcel parcel;
    bool ret = routeInfo->Marshalling(parcel);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: UnmarshallingTest001
 * @tc.desc: Test NetworkRouteInfo::Unmarshalling with valid data
 * @tc.type: FUNC
 */
HWTEST_F(NetworkRouteInfoTest, UnmarshallingTest001, TestSize.Level1)
{
    MessageParcel parcel;
    parcel.WriteString("wlan0");
    parcel.WriteString("10.0.0.0/8");
    parcel.WriteString("10.0.0.1");
    parcel.WriteBool(true);

    sptr<NetworkRouteInfo> routeInfo = NetworkRouteInfo::Unmarshalling(parcel);
    ASSERT_NE(routeInfo, nullptr);
    ASSERT_EQ(routeInfo->ifName, "wlan0");
    ASSERT_EQ(routeInfo->destination, "10.0.0.0/8");
    ASSERT_EQ(routeInfo->nextHop, "10.0.0.1");
    ASSERT_EQ(routeInfo->isExcludedRoute, true);
}

/**
 * @tc.name: UnmarshallingTest002
 * @tc.desc: Test NetworkRouteInfo::Unmarshalling with empty data
 * @tc.type: FUNC
 */
HWTEST_F(NetworkRouteInfoTest, UnmarshallingTest002, TestSize.Level1)
{
    MessageParcel parcel;
    parcel.WriteString("");
    parcel.WriteString("");
    parcel.WriteString("");
    parcel.WriteBool(false);

    sptr<NetworkRouteInfo> routeInfo = NetworkRouteInfo::Unmarshalling(parcel);
    ASSERT_NE(routeInfo, nullptr);
    ASSERT_EQ(routeInfo->ifName, "");
    ASSERT_EQ(routeInfo->destination, "");
    ASSERT_EQ(routeInfo->nextHop, "");
    ASSERT_EQ(routeInfo->isExcludedRoute, false);
}

/**
 * @tc.name: StaticMarshallingTest001
 * @tc.desc: Test static NetworkRouteInfo::Marshalling with valid object
 * @tc.type: FUNC
 */
HWTEST_F(NetworkRouteInfoTest, StaticMarshallingTest001, TestSize.Level1)
{
    MessageParcel parcel;
    sptr<NetworkRouteInfo> routeInfo = new (std::nothrow) NetworkRouteInfo();
    ASSERT_NE(routeInfo, nullptr);

    routeInfo->ifName = "ppp0";
    routeInfo->destination = "0.0.0.0/0";
    routeInfo->nextHop = "192.168.10.1";
    routeInfo->isExcludedRoute = false;

    bool ret = NetworkRouteInfo::Marshalling(parcel, routeInfo);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: StaticMarshallingTest002
 * @tc.desc: Test static NetworkRouteInfo::Marshalling with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(NetworkRouteInfoTest, StaticMarshallingTest002, TestSize.Level1)
{
    MessageParcel parcel;
    sptr<NetworkRouteInfo> routeInfo = nullptr;

    bool ret = NetworkRouteInfo::Marshalling(parcel, routeInfo);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: RoundTripTest001
 * @tc.desc: Test marshalling and unmarshalling round trip with valid data
 * @tc.type: FUNC
 */
HWTEST_F(NetworkRouteInfoTest, RoundTripTest001, TestSize.Level1)
{
    sptr<NetworkRouteInfo> originalRouteInfo = new (std::nothrow) NetworkRouteInfo();
    ASSERT_NE(originalRouteInfo, nullptr);

    originalRouteInfo->ifName = "eth1";
    originalRouteInfo->destination = "172.16.0.0/12";
    originalRouteInfo->nextHop = "172.16.0.1";
    originalRouteInfo->isExcludedRoute = true;

    MessageParcel parcel;
    bool marshallingRet = originalRouteInfo->Marshalling(parcel);
    ASSERT_TRUE(marshallingRet);

    sptr<NetworkRouteInfo> unmarshalledRouteInfo = NetworkRouteInfo::Unmarshalling(parcel);
    ASSERT_NE(unmarshalledRouteInfo, nullptr);

    ASSERT_EQ(unmarshalledRouteInfo->ifName, originalRouteInfo->ifName);
    ASSERT_EQ(unmarshalledRouteInfo->destination, originalRouteInfo->destination);
    ASSERT_EQ(unmarshalledRouteInfo->nextHop, originalRouteInfo->nextHop);
    ASSERT_EQ(unmarshalledRouteInfo->isExcludedRoute, originalRouteInfo->isExcludedRoute);
}
} // namespace nmd
} // namespace OHOS
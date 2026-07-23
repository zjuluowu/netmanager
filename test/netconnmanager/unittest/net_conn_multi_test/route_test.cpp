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
#include "net_mgr_log_wrapper.h"
#include "route.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class RouteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RouteTest::SetUpTestCase() {}

void RouteTest::TearDownTestCase() {}

void RouteTest::SetUp() {}

void RouteTest::TearDown() {}

/**
 * @tc.name: operatorTest
 * @tc.desc: Test Route::operator==
 * @tc.type: FUNC
 */
HWTEST_F(RouteTest, operatorTest, TestSize.Level1)
{
    Route routeScr;
    routeScr.iface_ = "testIface";
    Route routeDes = routeScr;
    ASSERT_TRUE(routeDes.iface_ == routeScr.iface_);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: Test static Route::Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RouteTest, UnmarshallingTest, TestSize.Level1)
{
    MessageParcel data;
    sptr<Route> route = new (std::nothrow) Route();
    ASSERT_TRUE(route != nullptr);
    bool bRet = Route::Marshalling(data, route);
    ASSERT_TRUE(bRet == true);

    sptr<Route> retRoute = Route::Unmarshalling(data);
    ASSERT_TRUE(retRoute != nullptr);
    bRet = route->Marshalling(data);
    ASSERT_TRUE(bRet == true);
}

/**
 * @tc.name: ToStringTest
 * @tc.desc: Test Route::ToString
 * @tc.type: FUNC
 */
HWTEST_F(RouteTest, ToStringTest, TestSize.Level1)
{
    sptr<Route> info = new (std::nothrow) Route();
    ASSERT_TRUE(info != nullptr);

    std::string str = info->ToString("testTab");
    NETMGR_LOG_D("Route.ToString string is : [%{public}s]", str.c_str());
}
} // namespace NetManagerStandard
} // namespace OHOS

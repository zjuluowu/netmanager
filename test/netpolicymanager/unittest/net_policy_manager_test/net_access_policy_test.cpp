/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include <thread>

#include <gtest/gtest.h>

#include "net_access_policy.h"

namespace OHOS {
namespace NetManagerStandard {

using namespace testing::ext;
class NetworkAccessPolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetworkAccessPolicyTest::SetUpTestCase() {}

void NetworkAccessPolicyTest::TearDownTestCase() {}

void NetworkAccessPolicyTest::SetUp() {}

void NetworkAccessPolicyTest::TearDown() {}

HWTEST_F(NetworkAccessPolicyTest, GetSupplierCallbackTest001, TestSize.Level1)
{
    NetworkAccessPolicy networkaccesspolicy;
    Parcel parcel;
    AccessPolicySave policies;
    bool flag = true;
    auto result = networkaccesspolicy.Marshalling(parcel, policies, flag);
    EXPECT_EQ(result, 0);
    flag = false;
    result = networkaccesspolicy.Marshalling(parcel, policies, flag);
    EXPECT_EQ(result, 0);
}
}
}
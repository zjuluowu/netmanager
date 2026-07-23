/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#define private public
#include "net_policy_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class NetPolicyExceptionServiceUt : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetPolicyExceptionServiceUt::SetUpTestCase() {}

void NetPolicyExceptionServiceUt::TearDownTestCase() {}

void NetPolicyExceptionServiceUt::SetUp() {}

void NetPolicyExceptionServiceUt::TearDown() {}

/**
 * @tc.name: IsUidNetAllowed
 * @tc.desc: Test NetPolicyService IsUidNetAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyExceptionServiceUt, IsUidNetAllowed, TestSize.Level1)
{
    int uit = 1000;
    bool metered = false;
    bool isAllowed = false;
    int32_t ret = DelayedSingleton<NetPolicyService>::GetInstance()->IsUidNetAllowed(uit, metered, isAllowed);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.name: RegisterNetPolicyCallback
 * @tc.desc: Test NetPolicyService RegisterNetPolicyCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyExceptionServiceUt, RegisterNetPolicyCallback, TestSize.Level1)
{
    int32_t ret = DelayedSingleton<NetPolicyService>::GetInstance()->RegisterNetPolicyCallback(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

/**
 * @tc.name: UnregisterNetPolicyCallback
 * @tc.desc: Test NetPolicyService UnregisterNetPolicyCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyExceptionServiceUt, UnregisterNetPolicyCallback, TestSize.Level1)
{
    int32_t ret = DelayedSingleton<NetPolicyService>::GetInstance()->UnregisterNetPolicyCallback(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}
} // namespace NetManagerStandard
} // namespace OHOS
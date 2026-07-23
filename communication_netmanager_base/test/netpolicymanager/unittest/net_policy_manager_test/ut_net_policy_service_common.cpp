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
#endif
#include "net_manager_constants.h"
#include "net_policy_service_common.h"
#include "net_settings.h"
#include "net_policy_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class UtNetPolicyServiceCommonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline auto instance_ = std::make_shared<NetPolicyServiceCommon>();
};

void UtNetPolicyServiceCommonTest::SetUpTestCase() {}

void UtNetPolicyServiceCommonTest::TearDownTestCase() {}

void UtNetPolicyServiceCommonTest::SetUp() {}

void UtNetPolicyServiceCommonTest::TearDown() {}

HWTEST_F(UtNetPolicyServiceCommonTest, ResetPoliciesTest001, TestSize.Level1)
{
    auto ret = instance_->ResetPolicies();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyServiceCommonTest, ResetPoliciesTest002, TestSize.Level1)
{
    DelayedSingleton<NetPolicyService>::GetInstance()->netPolicyRule_ = nullptr;
    auto ret = instance_->ResetPolicies();
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(UtNetPolicyServiceCommonTest, IsUidNetAllowedTest001, TestSize.Level1)
{
    uint32_t uid = 1251;
    bool metered = true;
    NetSettings::GetInstance().AddSystemUid(uid);
    auto ret = instance_->IsUidNetAllowed(uid, metered);
    EXPECT_TRUE(ret);
}
} // namespace NetManagerStandard
} // namespace OHOS
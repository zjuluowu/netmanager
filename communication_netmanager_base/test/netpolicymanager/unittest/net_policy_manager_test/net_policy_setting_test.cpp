/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "net_mgr_log_wrapper.h"
#include "net_policy_callback_test.h"
#include "net_policy_client.h"
#include "net_policy_constants.h"
#include "net_policy_inner_define.h"
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class NetPolicySettingTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    sptr<NetPolicyCallbackTest> GetINetPolicyCallbackSample() const;
};

void NetPolicySettingTest::SetUpTestCase()
{
}

void NetPolicySettingTest::TearDownTestCase()
{
}
void NetPolicySettingTest::SetUp() {}

void NetPolicySettingTest::TearDown() {}

HWTEST_F(NetPolicySettingTest, OpenPowerSave, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetPowerSavePolicy(true);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicySettingTest, ClosePowerSave, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetPowerSavePolicy(false);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicySettingTest, OpenDeviceIdle, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetDeviceIdlePolicy(true);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicySettingTest, CloseDeviceIdle, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetDeviceIdlePolicy(false);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicySettingTest, HasPermission, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->CheckPermission();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicySettingTest, NoPermission, TestSize.Level1)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->CheckPermission();
    EXPECT_EQ(result, NETMANAGER_ERR_PERMISSION_DENIED);
}
} // namespace NetManagerStandard
} // namespace OHOS

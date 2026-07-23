/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_policy_firewall.h"
#include "net_policy_rule.h"
#include "net_policy_service.h"
#include "net_policy_traffic.h"
#include "system_ability_definition.h"
#include "netmanager_base_test_security.h"
#include "net_policy_callback_proxy.h"
#include "net_policy_listener.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "want.h"
#include "mock_net_policy_db_clone.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::StrEq;
using ::testing::TypedEq;
using ::testing::ext::TestSize;
namespace {
constexpr uint32_t TEST_UID = 1;
constexpr const char *EXTENSION_BACKUP = "backup";
constexpr const char *EXTENSION_RESTORE = "restore";
}

class UtNetPolicyServiceNoCFI : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp(

    );
    void TearDown();
    static inline std::shared_ptr<NetPolicyService> instance_ = nullptr;
};

void UtNetPolicyServiceNoCFI::SetUpTestCase()
{
    instance_ = DelayedSingleton<NetPolicyService>::GetInstance();
    instance_->netPolicyRule_ = std::make_shared<NetPolicyRule>();
    instance_->netPolicyFirewall_ = std::make_shared<NetPolicyFirewall>();
    instance_->netPolicyTraffic_ = std::make_shared<NetPolicyTraffic>();
}

void UtNetPolicyServiceNoCFI::TearDownTestCase() {}

void UtNetPolicyServiceNoCFI::SetUp() {}

void UtNetPolicyServiceNoCFI::TearDown() {}

HWTEST_F(UtNetPolicyServiceNoCFI, OnStart001, TestSize.Level1)
{
    instance_->OnStart();
    EXPECT_EQ(instance_->state_, instance_->ServiceRunningState::STATE_STOPPED);
}

HWTEST_F(UtNetPolicyServiceNoCFI, OnExtension001, TestSize.Level1)
{
    EXPECT_CALL(NetPolicyDBClone::GetInstance(), OnBackup(_, _)).WillRepeatedly(Return(-1));
    EXPECT_CALL(NetPolicyDBClone::GetInstance(), OnRestore(_, _)).WillRepeatedly(Return(-1));
    std::string extension = EXTENSION_BACKUP;
    MessageParcel data;
    MessageParcel reply;
    instance_->OnExtension(extension, data, reply);

    MessageParcel data2;
    MessageParcel reply2;
    extension = EXTENSION_RESTORE;
    instance_->OnExtension(extension, data2, reply2);

    std::string extension2 = "";
    int32_t ret = instance_->OnExtension(extension2, data, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(UtNetPolicyServiceNoCFI, OnBackup001, TestSize.Level1)
{
    EXPECT_CALL(NetPolicyDBClone::GetInstance(), OnBackup(_, _)).WillRepeatedly(Return(-1));
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = instance_->OnBackup(data, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(UtNetPolicyServiceNoCFI, OnBackup002, TestSize.Level1)
{
    EXPECT_CALL(NetPolicyDBClone::GetInstance(), OnBackup(_, _)).WillRepeatedly(Return(0));
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = instance_->OnBackup(data, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(UtNetPolicyServiceNoCFI, OnBackup003, TestSize.Level1)
{
    EXPECT_CALL(NetPolicyDBClone::GetInstance(), OnBackup(_, _)).WillRepeatedly(Return(-1));
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = instance_->OnBackup(data, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(UtNetPolicyServiceNoCFI, OnRestore002, TestSize.Level1)
{
    EXPECT_CALL(NetPolicyDBClone::GetInstance(), OnRestore(_, _)).WillRepeatedly(Return(0));
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = instance_->OnRestore(data, reply);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(UtNetPolicyServiceNoCFI, OnRestore003, TestSize.Level1)
{
    EXPECT_CALL(NetPolicyDBClone::GetInstance(), OnRestore(_, _)).WillRepeatedly(Return(-1));
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = instance_->OnRestore(data, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(UtNetPolicyServiceNoCFI, OnRestore004, TestSize.Level1)
{
    EXPECT_CALL(NetPolicyDBClone::GetInstance(), OnRestore(_, _)).WillRepeatedly(Return(-1));
    MessageParcel data;
    MessageParcel reply;
    data.WriteFileDescriptor(-1);
    int32_t ret = instance_->OnRestore(data, reply);
    EXPECT_EQ(ret, -1);
}

} // namespace NetManagerStandard
} // namespace OHOS
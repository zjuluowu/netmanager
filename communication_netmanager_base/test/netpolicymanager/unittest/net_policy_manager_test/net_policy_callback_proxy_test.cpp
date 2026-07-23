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

#include <vector>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif
#include "net_policy_callback_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
#define DTEST_LOG std::cout << __func__ << ":" << __LINE__ << ":"
constexpr int64_t TEST_UID = 1010;

class MockNetIRemoteObject : public IRemoteObject {
public:
    MockNetIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}
    ~MockNetIRemoteObject() {}

    int32_t GetObjectRefCount() override
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        return eCode;
    }

    bool IsProxyObject() const override
    {
        return true;
    }

    bool CheckObjectLegality() const override
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface() override
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return 0;
    }

    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }

    void SetErrorCode(int errorCode)
    {
        eCode = errorCode;
    }

    private:
    int eCode = 0;
};

} // namespace

using namespace testing::ext;
class NetPolicyCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline sptr<MockNetIRemoteObject> remoteObj_ = nullptr;
};

void NetPolicyCallbackProxyTest::SetUpTestCase()
{
    remoteObj_ = new (std::nothrow) MockNetIRemoteObject();
}

void NetPolicyCallbackProxyTest::TearDownTestCase() { remoteObj_ = nullptr;}

void NetPolicyCallbackProxyTest::SetUp() {}

void NetPolicyCallbackProxyTest::TearDown() {}

HWTEST_F(NetPolicyCallbackProxyTest, NetUidPolicyChangeTest001, TestSize.Level1)
{
    NetPolicyCallbackProxy instance_(nullptr);
    EXPECT_EQ(instance_.NetUidPolicyChange(TEST_UID, 0), NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetUidPolicyChangeTest002, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_ERROR);
    NetPolicyCallbackProxy instance_(remoteObj_);
    EXPECT_EQ(instance_.NetUidPolicyChange(TEST_UID, 0), NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetUidPolicyChangeTest003, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_SUCCESS);
    NetPolicyCallbackProxy instance_(remoteObj_);
    EXPECT_EQ(instance_.NetUidPolicyChange(TEST_UID, 0), NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetUidRuleChangeTest001, TestSize.Level1)
{
    NetPolicyCallbackProxy instance_(nullptr);
    EXPECT_EQ(instance_.NetUidRuleChange(TEST_UID, 0), NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetUidRuleChangeTest002, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_ERROR);
    NetPolicyCallbackProxy instance_(remoteObj_);
    EXPECT_EQ(instance_.NetUidRuleChange(TEST_UID, 0), NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetUidRuleChangeTest003, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_SUCCESS);
    NetPolicyCallbackProxy instance_(remoteObj_);
    EXPECT_EQ(instance_.NetUidRuleChange(TEST_UID, 0), NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetBackgroundPolicyChangeTest001, TestSize.Level1)
{
    NetPolicyCallbackProxy instance_(nullptr);
    EXPECT_EQ(instance_.NetBackgroundPolicyChange(true), NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetBackgroundPolicyChangeTest002, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_ERROR);
    NetPolicyCallbackProxy instance_(remoteObj_);
    EXPECT_EQ(instance_.NetBackgroundPolicyChange(true), NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetBackgroundPolicyChangeTest003, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_SUCCESS);
    NetPolicyCallbackProxy instance_(remoteObj_);
    EXPECT_EQ(instance_.NetBackgroundPolicyChange(true), NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetQuotaPolicyChangeTest001, TestSize.Level1)
{
    NetPolicyCallbackProxy instance_(nullptr);
    std::vector<NetQuotaPolicy> netQuotaPolicys;
    EXPECT_EQ(instance_.NetQuotaPolicyChange(netQuotaPolicys), POLICY_ERR_QUOTA_POLICY_NOT_EXIST);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetQuotaPolicyChangeTest002, TestSize.Level1)
{
    NetPolicyCallbackProxy instance_(nullptr);
    std::vector<NetQuotaPolicy> netQuotaPolicys;
    NetQuotaPolicy quotaPolicy;
    netQuotaPolicys.emplace_back(quotaPolicy);
    EXPECT_EQ(instance_.NetQuotaPolicyChange(netQuotaPolicys), NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetQuotaPolicyChangeTest003, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_ERROR);
    NetPolicyCallbackProxy instance_(remoteObj_);
    std::vector<NetQuotaPolicy> netQuotaPolicys;
    NetQuotaPolicy quotaPolicy;
    netQuotaPolicys.emplace_back(quotaPolicy);
    EXPECT_EQ(instance_.NetQuotaPolicyChange(netQuotaPolicys), NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetQuotaPolicyChangeTest004, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_SUCCESS);
    NetPolicyCallbackProxy instance_(remoteObj_);
    std::vector<NetQuotaPolicy> netQuotaPolicys;
    NetQuotaPolicy quotaPolicy;
    netQuotaPolicys.emplace_back(quotaPolicy);
    EXPECT_EQ(instance_.NetQuotaPolicyChange(netQuotaPolicys), NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetStrategySwitchTest001, TestSize.Level1)
{
    NetPolicyCallbackProxy instance_(nullptr);
    std::string simId = "simId";
    EXPECT_EQ(instance_.NetStrategySwitch(simId, true), NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetStrategySwitchTest002, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_ERROR);
    NetPolicyCallbackProxy instance_(remoteObj_);
    std::string simId = "simId";
    EXPECT_EQ(instance_.NetStrategySwitch(simId, true), NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetStrategySwitchTest003, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_SUCCESS);
    NetPolicyCallbackProxy instance_(remoteObj_);
    std::string simId = "simId";
    EXPECT_EQ(instance_.NetStrategySwitch(simId, true), NETMANAGER_SUCCESS);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetMeteredIfacesChangeTest001, TestSize.Level1)
{
    NetPolicyCallbackProxy instance_(nullptr);
    std::vector<std::string> ifaces;
    EXPECT_EQ(instance_.NetMeteredIfacesChange(ifaces), NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetMeteredIfacesChangeTest002, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_ERROR);
    NetPolicyCallbackProxy instance_(remoteObj_);
    std::vector<std::string> ifaces;
    EXPECT_EQ(instance_.NetMeteredIfacesChange(ifaces), NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(NetPolicyCallbackProxyTest, NetMeteredIfacesChangeTest003, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_SUCCESS);
    NetPolicyCallbackProxy instance_(remoteObj_);
    std::vector<std::string> ifaces;
    EXPECT_EQ(instance_.NetMeteredIfacesChange(ifaces), NETMANAGER_SUCCESS);
}

} // namespace NetManagerStandard
} // namespace OHOS
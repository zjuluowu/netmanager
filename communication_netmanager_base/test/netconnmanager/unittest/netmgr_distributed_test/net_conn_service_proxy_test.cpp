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

#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "common_net_conn_callback_test.h"
#include "i_net_conn_service.h"
#include "i_net_detection_callback.h"
#include "i_net_factoryreset_callback.h"
#include "net_all_capabilities.h"
#include "net_conn_service_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
namespace {

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
        return 0;
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
    int eCode = NETMANAGER_SUCCESS;
};

class NetConnServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline sptr<MockNetIRemoteObject> remoteObj_ = std::make_unique<MockNetIRemoteObject>().release();
    static inline std::shared_ptr<NetConnServiceProxy> instance_ = std::make_shared<NetConnServiceProxy>(remoteObj_);
};

void NetConnServiceProxyTest::SetUpTestCase() {}

void NetConnServiceProxyTest::TearDownTestCase() {}

void NetConnServiceProxyTest::SetUp() {}

void NetConnServiceProxyTest::TearDown() {}

/**
 * @tc.name: EnableDistributedClientNet001
 * @tc.desc: Test EnableDistributedClientNet Branch.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, EnableDistributedClientNet001, TestSize.Level1)
{
    std::string virnicAddr = "1.189.55.61";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    int32_t ret = instance_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = instance_->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: EnableDistributedServerNet001
 * @tc.desc: Test EnableDistributedServerNet Branch.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, EnableDistributedServerNet001, TestSize.Level1)
{
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";

    int32_t ret = instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);

    bool isServer = true;
    std::string virnicName = "virnic";
    ret = instance_->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}
}
} // namespace NetManagerStandard
} // namespace OHOS

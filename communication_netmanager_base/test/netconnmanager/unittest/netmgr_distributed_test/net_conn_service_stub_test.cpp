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

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "common_net_conn_callback_test.h"
#include "net_conn_service_stub_test.h"
#include "net_interface_callback_stub.h"
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class NetConnServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetConnServiceStub> instance_ = std::make_shared<MockNetConnServiceStub>();
    static int32_t SendRemoteRequest(MessageParcel &data, ConnInterfaceCode code);
};

void NetConnServiceStubTest::SetUpTestCase() {}

void NetConnServiceStubTest::TearDownTestCase() {}

void NetConnServiceStubTest::SetUp() {}

void NetConnServiceStubTest::TearDown() {}

int32_t NetConnServiceStubTest::SendRemoteRequest(MessageParcel &data, ConnInterfaceCode code)
{
    MessageParcel reply;
    MessageOption option;
    return instance_->OnRemoteRequest(static_cast<uint32_t>(code), data, reply, option);
}

/**
 * @tc.name: OnEnableDistributedClientNet001
 * @tc.desc: Test OnEnableDistributedClientNet Branch.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceStubTest, OnEnableDistributedClientNet001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetConnServiceStub::GetDescriptor()), false);
    std::string virnicAddr = "1.189.55.61";
    ASSERT_NE(data.WriteString(virnicAddr), false);
    std::string virnicName = "virnic";
    ASSERT_NE(data.WriteString(virnicName), false);
    std::string iif = "wlan0";
    ASSERT_NE(data.WriteString(iif), false);
    int32_t ret = SendRemoteRequest(data, ConnInterfaceCode::CMD_NM_ENABLE_DISTRIBUTE_CLIENT_NET);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);

    MessageParcel data1;
    ASSERT_NE(data1.WriteInterfaceToken(NetConnServiceStub::GetDescriptor()), false);
    bool isServer = false;
    ASSERT_NE(data1.WriteBool(isServer), false);
    ret = SendRemoteRequest(data1, ConnInterfaceCode::CMD_NM_DISABLE_DISTRIBUTE_NET);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnEnableDistributedServerNet001
 * @tc.desc: Test OnEnableDistributedServerNet Branch.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceStubTest, OnEnableDistributedServerNet001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetConnServiceStub::GetDescriptor()), false);
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";
    ASSERT_NE(data.WriteString(iif), false);
    ASSERT_NE(data.WriteString(devIface), false);
    ASSERT_NE(data.WriteString(dstAddr), false);
    ASSERT_NE(data.WriteString(gw), false);
    int32_t ret = SendRemoteRequest(data, ConnInterfaceCode::CMD_NM_ENABLE_DISTRIBUTE_SERVER_NET);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);

    MessageParcel data1;
    ASSERT_NE(data1.WriteInterfaceToken(NetConnServiceStub::GetDescriptor()), false);
    bool isServer = true;
    ASSERT_NE(data1.WriteBool(isServer), false);
    ret = SendRemoteRequest(data1, ConnInterfaceCode::CMD_NM_DISABLE_DISTRIBUTE_NET);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnEnableDistributedServerNet002
 * @tc.desc: Test OnEnableDistributedServerNet Branch.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceStubTest, OnEnableDistributedServerNet002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    int32_t gw = 0;

    data.WriteString(iif);
    data.WriteString(devIface);
    data.WriteString(dstAddr);
    data.WriteInt32(gw);

    int32_t ret = instance_->OnEnableDistributedServerNet(data, reply);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnEnableDistributedServerNet003
 * @tc.desc: Test OnEnableDistributedServerNet Branch.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceStubTest, OnEnableDistributedServerNet003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";

    data.WriteString(iif);
    data.WriteString(devIface);
    data.WriteString(dstAddr);
    data.WriteString(gw);

    int32_t ret = instance_->OnEnableDistributedServerNet(data, reply);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS
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
#include "conn_ipc_interface_code.h"

namespace OHOS {
namespace NetManagerStandard {

class ErrorDeadFlowResetStub : public MockNetConnServiceStub {
public:
    int32_t IsDeadFlowResetTargetBundle(const std::string &bundleName, bool &flag) override
    {
        flag = false;
        return NETMANAGER_ERR_INTERNAL;
    }
};

using namespace testing::ext;

class NetConnServiceRegionalStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetConnServiceStub> instance_ = std::make_shared<MockNetConnServiceStub>();
};

void NetConnServiceRegionalStubTest::SetUpTestCase() {}

void NetConnServiceRegionalStubTest::TearDownTestCase() {}

void NetConnServiceRegionalStubTest::SetUp() {}

void NetConnServiceRegionalStubTest::TearDown() {}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto ret = instance_->OnRemoteRequest(0, data, reply, option);
    EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest002, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnServiceStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;
    auto ret = instance_->OnRemoteRequest(9999, data, reply, option);
    EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest003, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnServiceStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;
    auto ret = instance_->OnRemoteRequest(33, data, reply, option);
    EXPECT_GE(ret, IPC_STUB_UNKNOW_TRANS_ERR);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest004, TestSize.Level1)
{
    std::set<std::string> permissions;
    auto ret = instance_->OnRequestCheck(27, permissions);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest005, TestSize.Level1)
{
    std::set<std::string> permissions;
    auto ret = instance_->OnRequestCheck(9999, permissions);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest006, TestSize.Level1)
{
    std::set<std::string> permissions;
    auto ret = instance_->CheckPermission(permissions);
    EXPECT_EQ(ret, true);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest007, TestSize.Level1)
{
    std::set<std::string> permissions;
    auto ret = instance_->CheckPermissionWithCache(permissions);
    EXPECT_EQ(ret, true);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest008, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto ret = instance_->OnSetInternetPermission(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest009, TestSize.Level1)
{
    MessageParcel data;
    uint32_t uid = 1;
    data.WriteUint32(uid);
    MessageParcel reply;
    auto ret = instance_->OnSetInternetPermission(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest010, TestSize.Level1)
{
    MessageParcel data;
    uint32_t uid = 1;
    data.WriteUint32(uid);
    uint8_t allow = 1;
    data.WriteUint8(uid);
    MessageParcel reply;
    auto ret = instance_->OnSetInternetPermission(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest011, TestSize.Level1)
{
    MessageParcel data;
    int32_t size = 20;
    data.WriteInt32(size);
    MessageParcel reply;
    auto ret = instance_->OnEnableVnicNetwork(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest012, TestSize.Level1)
{
    MessageParcel data;
    int32_t size = 2;
    data.WriteInt32(size);
    MessageParcel reply;
    auto ret = instance_->OnEnableVnicNetwork(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest013, TestSize.Level1)
{
    MessageParcel data;
    int32_t size = 2;
    data.WriteInt32(size);
    for (int32_t index = 0; index < size; index++) {
        data.WriteInt32(index);
    }
    MessageParcel reply;
    auto ret = instance_->OnEnableVnicNetwork(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest014, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    uint32_t type = 10;
    data.ReadUint32(type);
    auto ret = instance_->OnRegisterNetSupplier(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest015, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto ret = instance_->OnRegisterNetSupplierCallback(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(NetConnServiceRegionalStubTest, NetConnServiceRegionalStubTest016, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto ret = instance_->OnRegisterNetConnCallback(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(NetConnServiceRegionalStubTest, OnIsDeadFlowResetTargetBundleTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto ret = instance_->OnIsDeadFlowResetTargetBundle(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

HWTEST_F(NetConnServiceRegionalStubTest, OnIsDeadFlowResetTargetBundleTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    std::string bundleName = "com.test.bundle";
    data.WriteString(bundleName);
    auto ret = instance_->OnIsDeadFlowResetTargetBundle(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceRegionalStubTest, OnIsDeadFlowResetTargetBundleTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    std::string bundleName = "";
    data.WriteString(bundleName);
    auto ret = instance_->OnIsDeadFlowResetTargetBundle(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceRegionalStubTest, OnIsDeadFlowResetTargetBundleTest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    std::string bundleName(257, 'a');
    data.WriteString(bundleName);
    auto ret = instance_->OnIsDeadFlowResetTargetBundle(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetConnServiceRegionalStubTest, OnIsDeadFlowResetTargetBundleTest005, TestSize.Level1)
{
    auto errorStub = std::make_shared<ErrorDeadFlowResetStub>();
    MessageParcel data;
    MessageParcel reply;
    std::string bundleName = "com.test.bundle";
    data.WriteString(bundleName);
    auto ret = errorStub->OnIsDeadFlowResetTargetBundle(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t result = NETMANAGER_SUCCESS;
    EXPECT_TRUE(reply.ReadInt32(result));
    EXPECT_EQ(result, NETMANAGER_ERR_INTERNAL);

    bool flag = false;
    EXPECT_FALSE(reply.ReadBool(flag));
}
}
}

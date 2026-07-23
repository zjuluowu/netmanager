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

#include <fcntl.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

#include "i_net_interface_callback.h"
#include "net_interface_config.h"
#include "net_interface_callback_stub.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
namespace {
constexpr uint64_t OUT_OF_RANGE_CODE = 100;
constexpr const char *TEST_IPV4_ADDR = "127.0.0.1";
constexpr const char *TEST_IFACE = "eth0";
constexpr const char *TEST_ROUTE = "fe80::/64";

class NetInterfaceCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetInterfaceStateCallbackStub> instance_ =
        std::make_shared<NetInterfaceStateCallbackStub>();
};

void NetInterfaceCallbackStubTest::SetUpTestCase() {}

void NetInterfaceCallbackStubTest::TearDownTestCase() {}

void NetInterfaceCallbackStubTest::SetUp() {}

void NetInterfaceCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequestTest001
 * @tc.desc: Test NetInterfaceStateCallbackStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnRemoteRequestTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(OUT_OF_RANGE_CODE, data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_ERR_DESCRIPTOR_MISMATCH);
}

/**
 * @tc.name: OnRemoteRequestTest002
 * @tc.desc: Test NetInterfaceStateCallbackStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnRemoteRequestTest002, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetInterfaceStateCallbackStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(OUT_OF_RANGE_CODE, data, reply, option);
    EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: OnInterfaceAddressUpdatedTest001
 * @tc.desc: Test NetInterfaceStateCallbackStub OnInterfaceAddressUpdated.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnInterfaceAddressUpdatedTest001, TestSize.Level1)
{
    int32_t ret = NETMANAGER_SUCCESS;
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetInterfaceStateCallbackStub::GetDescriptor())) {
        ret = NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = NETMANAGER_SUCCESS;
    if (!data.WriteString(TEST_IPV4_ADDR)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString(TEST_IFACE)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t flag = 0xffff;
    if (!data.WriteInt32(flag)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t scope = 1;
    if (!data.WriteInt32(scope)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    MessageParcel reply;
    MessageOption option;
    ret = instance_->OnRemoteRequest(static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDR_UPDATED),
                                     data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnInterfaceAddressRemovedTest001
 * @tc.desc: Test NetInterfaceStateCallbackStub OnInterfaceAddressRemoved.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnInterfaceAddressRemovedTest001, TestSize.Level1)
{
    int32_t ret = NETMANAGER_SUCCESS;
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetInterfaceStateCallbackStub::GetDescriptor())) {
        ret = NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = NETMANAGER_SUCCESS;
    if (!data.WriteString(TEST_IPV4_ADDR)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString(TEST_IFACE)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t flag = 0xffff;
    if (!data.WriteInt32(flag)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t scope = 1;
    if (!data.WriteInt32(scope)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    MessageParcel reply;
    MessageOption option;
    ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDR_REMOVED), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnInterfaceAddedTest001
 * @tc.desc: Test NetInterfaceStateCallbackStub OnInterfaceAdded.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnInterfaceAddedTest001, TestSize.Level1)
{
    int32_t ret = NETMANAGER_SUCCESS;
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetInterfaceStateCallbackStub::GetDescriptor())) {
        ret = NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString(TEST_IFACE)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    MessageParcel reply;
    MessageOption option;
    ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDED), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnInterfaceRemovedTest001
 * @tc.desc: Test NetInterfaceStateCallbackStub OnInterfaceRemoved.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnInterfaceRemovedTest001, TestSize.Level1)
{
    int32_t ret = NETMANAGER_SUCCESS;
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetInterfaceStateCallbackStub::GetDescriptor())) {
        ret = NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString(TEST_IFACE)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    MessageParcel reply;
    MessageOption option;
    ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_REMOVED), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnInterfaceChangedTest001
 * @tc.desc: Test NetInterfaceStateCallbackStub OnInterfaceChanged.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnInterfaceChangedTest001, TestSize.Level1)
{
    int32_t ret = NETMANAGER_SUCCESS;
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetInterfaceStateCallbackStub::GetDescriptor())) {
        ret = NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString(TEST_IFACE)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteBool(true)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    MessageParcel reply;
    MessageOption option;
    ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_CHANGED), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnInterfaceLinkStateChangedTest001
 * @tc.desc: Test NetInterfaceStateCallbackStub OnInterfaceLinkStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnInterfaceLinkStateChangedTest001, TestSize.Level1)
{
    int32_t ret = NETMANAGER_SUCCESS;
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetInterfaceStateCallbackStub::GetDescriptor())) {
        ret = NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString(TEST_IFACE)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteBool(true)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    MessageParcel reply;
    MessageOption option;
    ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_LINK_STATE_CHANGED), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnRouteChangedTest001
 * @tc.desc: Test NetInterfaceStateCallbackStub OnRouteChanged.
 * @tc.type: FUNC
 */
HWTEST_F(NetInterfaceCallbackStubTest, OnRouteChangedTest001, TestSize.Level1)
{
    int32_t ret = NETMANAGER_SUCCESS;
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetInterfaceStateCallbackStub::GetDescriptor())) {
        ret = NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteBool(true)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString(TEST_ROUTE)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString("")) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    if (!data.WriteString(TEST_IFACE)) {
        ret = NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    MessageParcel reply;
    MessageOption option;
    ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_ROUTE_CHANGED), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace
} // namespace NetManagerStandard
} // namespace OHOS
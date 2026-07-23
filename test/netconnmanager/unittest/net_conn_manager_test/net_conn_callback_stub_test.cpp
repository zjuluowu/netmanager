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

#include "i_net_conn_callback.h"
#include "net_all_capabilities.h"
#include "net_conn_callback_stub.h"
#include "net_manager_constants.h"
#include <fcntl.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
namespace {
constexpr uint64_t OUTOFFRANGECODE = 100;
constexpr int32_t TEST_NETID = 1010;
class TestNetConnCallbackStub : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetConnCallbackStub> instance_ = std::make_shared<NetConnCallbackStub>();
};

void TestNetConnCallbackStub::SetUpTestCase() {}

void TestNetConnCallbackStub::TearDownTestCase() {}

void TestNetConnCallbackStub::SetUp() {}

void TestNetConnCallbackStub::TearDown() {}

/**
 * @tc.name: OnRemoteRequestTest001
 * @tc.desc: Test NetConnCallbackStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnRemoteRequestTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(OUTOFFRANGECODE, data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_ERR_DESCRIPTOR_MISMATCH);
}

/**
 * @tc.name: OnRemoteRequestTest002
 * @tc.desc: Test NetConnCallbackStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnRemoteRequestTest002, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(OUTOFFRANGECODE, data, reply, option);
    EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: OnNetAvailableTest001
 * @tc.desc: Test NetConnCallbackStub OnNetAvailable.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnNetAvailableTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(ConnCallbackInterfaceCode::NET_AVAILABLE),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

/**
 * @tc.name: OnNetAvailableTest002
 * @tc.desc: Test NetConnCallbackStub OnNetAvailable.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnNetAvailableTest002, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    data.WriteInt32(TEST_NETID);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(ConnCallbackInterfaceCode::NET_AVAILABLE),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnNetCapabilitiesChangeTest001
 * @tc.desc: Test NetConnCallbackStub OnNetCapabilitiesChange.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnNetCapabilitiesChangeTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;
    NetAllCapabilities netCaps;
    netCaps.bearerTypes_.insert(NetBearType::BEARER_CELLULAR);
    netCaps.netCaps_.insert(NetCap::NET_CAPABILITY_MMS);
    data.WriteInt32(TEST_NETID);
    data.WriteUint32(netCaps.linkDownBandwidthKbps_);
    data.WriteUint32(netCaps.linkUpBandwidthKbps_);
    data.WriteUint32(netCaps.netCaps_.size());
    data.WriteUint32(NetCap::NET_CAPABILITY_MMS);
    data.WriteUint32(netCaps.bearerTypes_.size());
    data.WriteUint32(NetBearType::BEARER_CELLULAR);
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(ConnCallbackInterfaceCode::NET_CAPABILITIES_CHANGE),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnNetConnectionPropertiesChangeTest001
 * @tc.desc: Test NetConnCallbackStub OnNetConnectionPropertiesChange.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnNetConnectionPropertiesChangeTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;
    int32_t ret =instance_->OnRemoteRequest(
        static_cast<uint32_t>(ConnCallbackInterfaceCode::NET_CONNECTION_PROPERTIES_CHANGE), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);

    MessageParcel dataSuccess;
    dataSuccess.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    dataSuccess.WriteInt32(TEST_NETID);
    NetLinkInfo linkInfo;
    linkInfo.ifaceName_ = "ifacename";
    linkInfo.domain_ = "0.0.0.0";
    linkInfo.Marshalling(dataSuccess);
    ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(ConnCallbackInterfaceCode::NET_CONNECTION_PROPERTIES_CHANGE), dataSuccess, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnNetLostTest001
 * @tc.desc: Test NetConnCallbackStub OnNetLost.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnNetLostTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    data.WriteInt32(TEST_NETID);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(ConnCallbackInterfaceCode::NET_LOST),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnNetUnavailableTest001
 * @tc.desc: Test NetConnCallbackStub OnNetUnavailable.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnNetUnavailableTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(ConnCallbackInterfaceCode::NET_UNAVAILABLE),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnNetBlockStatusChangeTest001
 * @tc.desc: Test NetConnCallbackStub OnNetBlockStatusChange.
 * @tc.type: FUNC
 */
HWTEST_F(TestNetConnCallbackStub, OnNetBlockStatusChangeTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteInterfaceToken(NetConnCallbackStub::GetDescriptor());
    data.WriteInt32(TEST_NETID);
    data.WriteBool(false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(ConnCallbackInterfaceCode::NET_BLOCK_STATUS_CHANGE),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace
} // namespace NetManagerStandard
} // namespace OHOS
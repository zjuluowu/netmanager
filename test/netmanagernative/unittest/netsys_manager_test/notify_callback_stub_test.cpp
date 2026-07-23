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

#include "common_notify_callback_test.h"
#include "notify_callback_stub.h"

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace testing::ext;
} // namespace

class NotifyCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NotifyCallbackTest> notifyStub_ = nullptr;
};

void NotifyCallbackStubTest::SetUpTestCase()
{
    notifyStub_ = std::make_shared<NotifyCallbackTest>();
}

void NotifyCallbackStubTest::TearDownTestCase() {}

void NotifyCallbackStubTest::SetUp() {}

void NotifyCallbackStubTest::TearDown() {}

HWTEST_F(NotifyCallbackStubTest, OnInterfaceAddressUpdated001, TestSize.Level1)
{
    std::string addr = "192.161.0.5";
    std::string ifName = "test0";
    int32_t flags = 2;
    int32_t scope = 0;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteString(addr), false);
    ASSERT_NE(data.WriteString(ifName), false);
    ASSERT_NE(data.WriteUint32(flags), false);
    ASSERT_NE(data.WriteUint32(scope), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(100, data, reply, option);
    EXPECT_NE(ret, 0);

    ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_INTERFACE_ADDRESS_UPDATED),
                                       data, reply, option);
    EXPECT_NE(ret, 0);

    MessageParcel dataOk;
    ASSERT_NE(dataOk.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(dataOk.WriteString(addr), false);
    ASSERT_NE(dataOk.WriteString(ifName), false);
    ASSERT_NE(dataOk.WriteUint32(flags), false);
    ASSERT_NE(dataOk.WriteUint32(scope), false);
    ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_INTERFACE_ADDRESS_UPDATED),
                                       dataOk, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NotifyCallbackStubTest, OnInterfaceAddressRemoved001, TestSize.Level1)
{
    std::string addr = "192.161.0.5";
    std::string ifName = "test0";
    int32_t flags = 2;
    int32_t scope = 0;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteString(addr), false);
    ASSERT_NE(data.WriteString(ifName), false);
    ASSERT_NE(data.WriteUint32(flags), false);
    ASSERT_NE(data.WriteUint32(scope), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_INTERFACE_ADDRESS_REMOVED),
                                               data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NotifyCallbackStubTest, OnInterfaceAdded001, TestSize.Level1)
{
    std::string ifName = "test0";
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteString(ifName), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_INTERFACE_ADDED),
                                               data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NotifyCallbackStubTest, OnInterfaceRemoved001, TestSize.Level1)
{
    std::string ifName = "test0";
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteString(ifName), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_INTERFACE_REMOVED),
                                               data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NotifyCallbackStubTest, OnInterfaceChanged001, TestSize.Level1)
{
    std::string ifName = "test0";
    bool isUp = false;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteString(ifName), false);
    ASSERT_NE(data.WriteBool(isUp), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_INTERFACE_CHANGED),
                                               data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NotifyCallbackStubTest, OnInterfaceLinkStateChanged001, TestSize.Level1)
{
    std::string ifName = "test0";
    bool isUp = false;
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteString(ifName), false);
    ASSERT_NE(data.WriteBool(isUp), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(
        static_cast<uint32_t>(NotifyInterfaceCode::ON_INTERFACE_LINK_STATE_CHANGED), data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NotifyCallbackStubTest, OnRouteChanged001, TestSize.Level1)
{
    bool updated = false;
    std::string route = "192.168.0.1";
    std::string gateway = "192.168.0.1";
    std::string ifName = "test0";
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteBool(updated), false);
    ASSERT_NE(data.WriteString(route), false);
    ASSERT_NE(data.WriteString(gateway), false);
    ASSERT_NE(data.WriteString(ifName), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_ROUTE_CHANGED),
                                               data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NotifyCallbackStubTest, OnDhcpSuccess001, TestSize.Level1)
{
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    sptr<DhcpResultParcel> dhcpResult = new (std::nothrow) DhcpResultParcel;
    dhcpResult->iface_ = "test0";
    dhcpResult->ipAddr_ = "192.168.11.55";
    dhcpResult->gateWay_ = "192.168.10.1";
    dhcpResult->subNet_ = "255.255.255.0";
    dhcpResult->Marshalling(data);

    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_DHCP_SUCCESS),
                                               data, reply, option);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NotifyCallbackStubTest, OnBandwidthReachedLimit001, TestSize.Level1)
{
    std::string limitName = "limit";
    std::string iface = "test0";
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NotifyCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteString(limitName), false);
    ASSERT_NE(data.WriteString(iface), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = notifyStub_->OnRemoteRequest(static_cast<uint32_t>(NotifyInterfaceCode::ON_BANDWIDTH_REACHED_LIMIT),
                                               data, reply, option);
    EXPECT_EQ(ret, 0);
}
} // namespace nmd
} // namespace OHOS
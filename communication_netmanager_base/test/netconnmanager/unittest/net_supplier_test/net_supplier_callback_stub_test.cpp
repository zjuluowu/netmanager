/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#define private public
#include "net_manager_constants.h"
#include "net_supplier_callback_stub.h"
#include "net_supplier_callback_proxy.h"
#include "common_net_conn_callback_test.h"
namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing;
using namespace testing::ext;
static constexpr uint32_t MAX_NET_CAP_NUM = 32;
static constexpr uint32_t MAX_NET_BEARTYPE_NUM = 7;
} // namespace

class NetSupplierCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetSupplierCallbackStubTestCb> supplierCbStub_ = nullptr;
};

void NetSupplierCallbackStubTest::SetUpTestCase()
{
    supplierCbStub_ = std::make_shared<NetSupplierCallbackStubTestCb>();
    sptr<NetSupplierCallbackBase> callback = new (std::nothrow) NetSupplierCallbackBaseTestCb();
    supplierCbStub_->RegisterSupplierCallbackImpl(callback);
}

void NetSupplierCallbackStubTest::TearDownTestCase() {}

void NetSupplierCallbackStubTest::SetUp() {}

void NetSupplierCallbackStubTest::TearDown() {}

HWTEST_F(NetSupplierCallbackStubTest, RequestNetwork001, TestSize.Level1)
{
    std::string ident = "testsupid";
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_NOT_METERED);
    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetSupplierCallbackStub::GetDescriptor()), false);
    ASSERT_NE(data.WriteString(ident), false);
    uint32_t size = static_cast<uint32_t>(netCaps.size());
    ASSERT_NE(data.WriteUint32(size), false);
    for (auto netCap : netCaps) {
        data.WriteInt32(static_cast<uint32_t>(netCap));
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = supplierCbStub_->OnRemoteRequest(100, data, reply, option);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);

    ret = supplierCbStub_->OnRemoteRequest(
        static_cast<uint32_t>(SupplierInterfaceCode::NET_SUPPLIER_REQUEST_NETWORK), data, reply, option);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);

    MessageParcel dataOk;
    ASSERT_NE(dataOk.WriteInterfaceToken(NetSupplierCallbackStub::GetDescriptor()), false);
    ASSERT_NE(dataOk.WriteString(ident), false);
    size = static_cast<uint32_t>(netCaps.size());
    ASSERT_NE(dataOk.WriteUint32(size), false);
    for (auto netCap : netCaps) {
        dataOk.WriteInt32(static_cast<uint32_t>(netCap));
    }
    ret = supplierCbStub_->OnRemoteRequest(
        static_cast<uint32_t>(SupplierInterfaceCode::NET_SUPPLIER_REQUEST_NETWORK), dataOk, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetSupplierCallbackStubTest, ReleaseNetwork001, TestSize.Level1)
{
    NetRequest netrequest;
    netrequest.ident = "testsupid";
    netrequest.netCaps.insert(NetCap::NET_CAPABILITY_NOT_METERED);

    MessageParcel data;
    ASSERT_NE(data.WriteInterfaceToken(NetSupplierCallbackStub::GetDescriptor()), false);
    bool result = data.WriteUint32(netrequest.uid) && data.WriteUint32(netrequest.requestId) &&
                  data.WriteUint32(netrequest.registerType) && data.WriteString(netrequest.ident);
    ASSERT_NE(result, false);

    uint32_t size = static_cast<uint32_t>(netrequest.bearTypes.size());
    ASSERT_NE(data.WriteUint32(size), false);
    for (auto netBearType : netrequest.bearTypes) {
        ASSERT_NE(data.WriteInt32(netBearType), false);
    }

    size = static_cast<uint32_t>(netrequest.netCaps.size());
    ASSERT_NE(data.WriteUint32(size), false);
    for (auto netCap : netrequest.netCaps) {
        data.WriteInt32(static_cast<uint32_t>(netCap));
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = supplierCbStub_->OnRemoteRequest(
        static_cast<uint32_t>(SupplierInterfaceCode::NET_SUPPLIER_RELEASE_NETWORK), data, reply, option);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS
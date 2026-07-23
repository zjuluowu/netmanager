/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "refresh_http_proxy_callback_proxy.h"
#include "http_proxy.h"
#include "net_conn_constants.h"

namespace OHOS {
namespace NetManagerStandard {

using namespace testing::ext;
using ::testing::_;
using ::testing::Return;

class RemoteObjectMocker : public IRemoteObject {
public:
    RemoteObjectMocker() : IRemoteObject{u"RemoteObjectMocker"} {}
    ~RemoteObjectMocker() {}

    MOCK_METHOD(int32_t, GetObjectRefCount, (), (override));
    MOCK_METHOD(int, SendRequest, (uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option),
                (override));
    MOCK_METHOD(bool, IsProxyObject, (), (const, override));
    MOCK_METHOD(bool, IsObjectDead, (), (const, override));
    MOCK_METHOD(std::u16string, GetInterfaceDescriptor, (), (override));
    MOCK_METHOD(bool, CheckObjectLegality, (), (const, override));
    MOCK_METHOD(bool, AddDeathRecipient, (const sptr<DeathRecipient> &recipient), (override));
    MOCK_METHOD(bool, RemoveDeathRecipient, (const sptr<DeathRecipient> &recipient), (override));
    MOCK_METHOD(bool, Marshalling, (OHOS::Parcel & parcel), (const, override));
    MOCK_METHOD(sptr<IRemoteBroker>, AsInterface, (), (override));
    MOCK_METHOD(int, Dump, (int fd, const std::vector<std::u16string> &args), (override));
};

class RefreshHttpProxyCallbackProxyTest : public testing::Test {
protected:
    RemoteObjectMocker *remoteObjectMocker;
    sptr<RefreshHttpProxyCallbackProxy> proxy;

    void SetUp() override
    {
        remoteObjectMocker = new RemoteObjectMocker();
        sptr<IRemoteObject> impl(remoteObjectMocker);
        proxy = new (std::nothrow) RefreshHttpProxyCallbackProxy(impl);
    }

    void TearDown() override
    {
        remoteObjectMocker = nullptr;
        proxy = nullptr;
    }
};

/**
 * @tc.name: OnRefreshHttpProxyResultTest001
 * @tc.desc: Test RefreshHttpProxyCallbackProxy OnRefreshHttpProxyResult with SendRequest success.
 * @tc.type: FUNC
 */
HWTEST_F(RefreshHttpProxyCallbackProxyTest, OnRefreshHttpProxyResultTest001, TestSize.Level1)
{
    HttpProxy httpProxy;
    httpProxy.SetHost("testHost");
    httpProxy.SetPort(8080);
    EXPECT_CALL(*remoteObjectMocker, SendRequest(_, _, _, _)).WillOnce(Return(ERR_NONE));
    int32_t result = proxy->OnRefreshHttpProxyResult(NETMANAGER_SUCCESS, httpProxy);
    EXPECT_EQ(result, ERR_NONE);
}

/**
 * @tc.name: OnRefreshHttpProxyResultTest002
 * @tc.desc: Test RefreshHttpProxyCallbackProxy OnRefreshHttpProxyResult with SendRequest failed.
 * @tc.type: FUNC
 */
HWTEST_F(RefreshHttpProxyCallbackProxyTest, OnRefreshHttpProxyResultTest002, TestSize.Level1)
{
    HttpProxy httpProxy;
    httpProxy.SetHost("testHost");
    httpProxy.SetPort(8080);
    EXPECT_CALL(*remoteObjectMocker, SendRequest(_, _, _, _)).WillOnce(Return(ERR_TRANSACTION_FAILED));
    int32_t result = proxy->OnRefreshHttpProxyResult(NETMANAGER_ERR_INTERNAL, httpProxy);
    EXPECT_EQ(result, ERR_TRANSACTION_FAILED);
}

/**
 * @tc.name: OnRefreshHttpProxyResultTest003
 * @tc.desc: Test RefreshHttpProxyCallbackProxy OnRefreshHttpProxyResult with null remote object.
 * @tc.type: FUNC
 */
HWTEST_F(RefreshHttpProxyCallbackProxyTest, OnRefreshHttpProxyResultTest003, TestSize.Level1)
{
    sptr<RefreshHttpProxyCallbackProxy> nullProxy = new (std::nothrow) RefreshHttpProxyCallbackProxy(nullptr);
    HttpProxy httpProxy;
    httpProxy.SetHost("testHost");
    httpProxy.SetPort(8080);
    int32_t result = nullProxy->OnRefreshHttpProxyResult(NETMANAGER_SUCCESS, httpProxy);
    EXPECT_EQ(result, NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.name: WriteInterfaceTokenTest001
 * @tc.desc: Test RefreshHttpProxyCallbackProxy WriteInterfaceToken.
 * @tc.type: FUNC
 */
HWTEST_F(RefreshHttpProxyCallbackProxyTest, WriteInterfaceTokenTest001, TestSize.Level1)
{
    MessageParcel data;
    bool ret = proxy->WriteInterfaceToken(data);
    EXPECT_TRUE(ret);
}
} // namespace NetManagerStandard
} // namespace OHOS

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

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "refresh_http_proxy_callback_stub.h"
#include "conn_ipc_interface_code.h"
#include "http_proxy.h"
#include "net_conn_constants.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {

using namespace testing::ext;

class RefreshHttpProxyCallbackStubTest : public testing::Test {
protected:
    sptr<RefreshHttpProxyCallbackStub> stub_;

    void SetUp() override
    {
        stub_ = new (std::nothrow) RefreshHttpProxyCallbackStub();
    }

    void TearDown() override
    {
        stub_ = nullptr;
    }
};

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRemoteRequestTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RefreshHttpProxyCallbackStub::GetDescriptor());
    data.WriteInt32(NETMANAGER_SUCCESS);
    HttpProxy httpProxy;
    httpProxy.Marshalling(data);
    uint32_t code = static_cast<uint32_t>(RefreshHttpProxyCallbackInterfaceCode::ON_REFRESH_HTTP_PROXY_RESULT);
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRemoteRequestTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"InvalidDescriptor");
    uint32_t code = static_cast<uint32_t>(RefreshHttpProxyCallbackInterfaceCode::ON_REFRESH_HTTP_PROXY_RESULT);
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_ERR_DESCRIPTOR_MISMATCH);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRemoteRequestTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RefreshHttpProxyCallbackStub::GetDescriptor());
    uint32_t code = 9999;
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_NE(ret, NETMANAGER_ERR_DESCRIPTOR_MISMATCH);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRefreshHttpProxyResultInnerTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(NETMANAGER_SUCCESS);
    HttpProxy httpProxy;
    httpProxy.SetHost("testHost");
    httpProxy.SetPort(8080);
    httpProxy.Marshalling(data);
    int32_t ret = stub_->OnRefreshHttpProxyResultInner(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    int32_t replyResult = 0;
    reply.ReadInt32(replyResult);
    EXPECT_EQ(replyResult, NETMANAGER_SUCCESS);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRefreshHttpProxyResultInnerTest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = stub_->OnRefreshHttpProxyResultInner(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRefreshHttpProxyResultInnerTest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(NETMANAGER_SUCCESS);
    int32_t ret = stub_->OnRefreshHttpProxyResultInner(data, reply);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_DATA_FAIL);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRefreshHttpProxyResultTest001, TestSize.Level1)
{
    int32_t callbackResult = -1;
    HttpProxy callbackProxy;
    stub_->SetRefreshCallback([&callbackResult, &callbackProxy](int32_t result, const HttpProxy &httpProxy) {
        callbackResult = result;
        callbackProxy = httpProxy;
    });
    HttpProxy httpProxy;
    httpProxy.SetHost("testHost");
    httpProxy.SetPort(8080);
    int32_t ret = stub_->OnRefreshHttpProxyResult(NETMANAGER_SUCCESS, httpProxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(callbackResult, NETMANAGER_SUCCESS);
    EXPECT_EQ(callbackProxy.GetHost(), "testHost");
    EXPECT_EQ(callbackProxy.GetPort(), 8080);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRefreshHttpProxyResultTest002, TestSize.Level1)
{
    stub_->SetRefreshCallback(nullptr);
    HttpProxy httpProxy;
    httpProxy.SetHost("testHost");
    httpProxy.SetPort(8080);
    int32_t ret = stub_->OnRefreshHttpProxyResult(NETMANAGER_ERR_INTERNAL, httpProxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, OnRefreshHttpProxyResultTest003, TestSize.Level1)
{
    int32_t callbackResult = -1;
    HttpProxy callbackProxy;
    stub_->SetRefreshCallback([&callbackResult, &callbackProxy](int32_t result, const HttpProxy &httpProxy) {
        callbackResult = result;
        callbackProxy = httpProxy;
    });
    HttpProxy httpProxy;
    int32_t ret = stub_->OnRefreshHttpProxyResult(NETMANAGER_ERR_INTERNAL, httpProxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(callbackResult, NETMANAGER_ERR_INTERNAL);
    EXPECT_TRUE(callbackProxy.GetHost().empty());
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, SetRefreshCallbackTest001, TestSize.Level1)
{
    bool callbackInvoked = false;
    stub_->SetRefreshCallback([&callbackInvoked](int32_t result, const HttpProxy &httpProxy) {
        callbackInvoked = true;
    });
    HttpProxy httpProxy;
    stub_->OnRefreshHttpProxyResult(NETMANAGER_SUCCESS, httpProxy);
    EXPECT_TRUE(callbackInvoked);
}

HWTEST_F(RefreshHttpProxyCallbackStubTest, MemberFuncMapTest001, TestSize.Level1)
{
    auto itFunc = stub_->memberFuncMap_.find(
        static_cast<uint32_t>(RefreshHttpProxyCallbackInterfaceCode::ON_REFRESH_HTTP_PROXY_RESULT));
    EXPECT_NE(itFunc, stub_->memberFuncMap_.end());
    EXPECT_NE(itFunc->second, nullptr);
}

} // namespace NetManagerStandard
} // namespace OHOS

/*
 * Copyright (C) 2021-2023 Huawei Device Co., Ltd.
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

#include <memory>
#define private public
#include "net_conn_callback_stub.h"
#undef private
#include "net_conn_callback_test.h"
#include "net_conn_constants.h"
#include "net_manager_constants.h"

#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetConnCallbackTest::NetConnCallbackTest() {}

NetConnCallbackTest::~NetConnCallbackTest() {}

void NetConnCallbackTest::NotifyAll()
{
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    cv_.notify_all();
}

void NetConnCallbackTest::WaitFor(int timeoutSecond)
{
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    cv_.wait_for(callbackLock, std::chrono::seconds(timeoutSecond));
}

int32_t NetConnCallbackTest::NetAvailable(sptr<NetHandle> &netHandle)
{
    if (netHandle != nullptr) {
        return 0;
    }

    NETMGR_LOG_D("NetAvailable: netId = %{public}d", netHandle->GetNetId());
    NotifyAll();
    return 0;
}

int32_t NetConnCallbackTest::NetCapabilitiesChange(
    sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap)
{
    if (netHandle == nullptr || netAllCap == nullptr) {
        return 0;
    }

    NETMGR_LOG_D("NetCapabilitiesChange: netId = [%{public}d]", netHandle->GetNetId());
    NETMGR_LOG_D("[%{public}s]", netAllCap->ToString("|").c_str());
    NotifyAll();
    return 0;
}

int32_t NetConnCallbackTest::NetConnectionPropertiesChange(sptr<NetHandle> &netHandle,
    const sptr<NetLinkInfo> &info)
{
    if (netHandle == nullptr || info == nullptr) {
        return 0;
    }
    NETMGR_LOG_D("NetConnectionPropertiesChange: netId = %{public}d info = %{public}s", netHandle->GetNetId(),
        info->ToString(" ").c_str());
    NotifyAll();
    return 0;
}

int32_t NetConnCallbackTest::NetLost(sptr<NetHandle> &netHandle)
{
    if (netHandle == nullptr) {
        return 0;
    }
    NETMGR_LOG_D("NetLost: netId = %{public}d", netHandle->GetNetId());
    NotifyAll();
    return 0;
}

int32_t NetConnCallbackTest::NetUnavailable()
{
    NETMGR_LOG_D("NetUnavailable");
    NotifyAll();
    return 0;
}

int32_t NetConnCallbackTest::NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
{
    NETMGR_LOG_D("NetConnCallbackTest::NetLost: netId = %{public}d bolcked = %{public}s",
        netHandle->GetNetId(), blocked ? "true" : "false");
    NotifyAll();
    return 0;
}

using namespace testing::ext;
class NetConnCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetConnCallbackStub> instance_ = std::make_shared<NetConnCallbackStub>();
};

void NetConnCallbackStubTest::SetUpTestCase() {}

void NetConnCallbackStubTest::TearDownTestCase() {}

void NetConnCallbackStubTest::SetUp() {}

void NetConnCallbackStubTest::TearDown() {}

HWTEST_F(NetConnCallbackStubTest, OnNetLost001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = instance_->OnNetLost(data, reply);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int32_t ret = instance_->OnRemoteRequest(0, data, reply, option);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int32_t ret = instance_->OnRemoteRequest(100, data, reply, option);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, OnNetBlockStatusChange001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    int32_t ret = instance_->OnNetBlockStatusChange(data, reply);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, NetAvailable001, TestSize.Level1)
{
    sptr<NetHandle> handle = nullptr;
    int32_t ret = instance_->NetAvailable(handle);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, NetCapabilitiesChange001, TestSize.Level1)
{
    sptr<NetHandle> handle = nullptr;
    sptr<NetAllCapabilities> allCapabilities = nullptr;
    int32_t ret = instance_->NetCapabilitiesChange(handle, allCapabilities);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, NetConnectionPropertiesChange001, TestSize.Level1)
{
    sptr<NetHandle> handle = nullptr;
    sptr<NetLinkInfo> info = nullptr;
    int32_t ret = instance_->NetConnectionPropertiesChange(handle, info);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, NetLost001, TestSize.Level1)
{
    sptr<NetHandle> handle = nullptr;
    int32_t ret = instance_->NetLost(handle);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, NetUnavailable001, TestSize.Level1)
{
    int32_t ret = instance_->NetUnavailable();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnCallbackStubTest, NetBlockStatusChange001, TestSize.Level1)
{
    sptr<NetHandle> handle = nullptr;
    int32_t ret = instance_->NetBlockStatusChange(handle, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

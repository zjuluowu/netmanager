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

#include "net_dns_result_callback_proxy.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "net_manager_constants.h"

namespace OHOS {
namespace NetsysNative {
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

class NetDnsResultCallbackProxyTest : public testing::Test {
protected:
    RemoteObjectMocker *remoteObjectMocker;
    sptr<NetDnsResultCallbackProxy> proxy;

    void SetUp() override
    {
        remoteObjectMocker = new RemoteObjectMocker();
        sptr<IRemoteObject> impl(remoteObjectMocker);
        proxy = new (std::nothrow) NetDnsResultCallbackProxy(impl);
    }

    void TearDown() override
    {
        remoteObjectMocker = nullptr;
        proxy = nullptr;
    }
};

HWTEST_F(NetDnsResultCallbackProxyTest, NetDnsResultCallbackProxyTest_OnDnsResultReport_001, TestSize.Level0)
{
    uint32_t listsize = 1;
    std::list<NetDnsResultReport> dnsResultReport;
    int32_t ret = proxy->OnDnsResultReport(listsize, dnsResultReport);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetDnsResultCallbackProxyTest, NetDnsResultCallbackProxyTest_OnDnsResultReport_002, TestSize.Level0)
{
    uint32_t listsize = 0;
    std::list<NetDnsResultReport> dnsResultReport;
    EXPECT_CALL(*remoteObjectMocker, SendRequest(_, _, _, _)).WillOnce(Return(1));
    int32_t ret = proxy->OnDnsResultReport(listsize, dnsResultReport);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetDnsResultCallbackProxyTest, NetDnsResultCallbackProxyTest_OnDnsResultReport_003, TestSize.Level0)
{
    uint32_t listsize = 0;
    std::list<NetDnsResultReport> dnsResultReport;
    NetDnsResultReport report;
    dnsResultReport.push_back(report);
    int32_t ret = proxy->OnDnsResultReport(listsize, dnsResultReport);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetDnsResultCallbackProxyTest, NetDnsResultCallbackProxyTest_OnDnsQueryResultReport_001, TestSize.Level0)
{
    uint32_t listsize = 1;
    std::list<NetDnsQueryResultReport> dnsResultReport;
    int32_t ret = proxy->OnDnsQueryResultReport(listsize, dnsResultReport);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetDnsResultCallbackProxyTest, NetDnsResultCallbackProxyTest_OnDnsQueryResultReport_002, TestSize.Level0)
{
    uint32_t listsize = 0;
    std::list<NetDnsQueryResultReport> dnsResultReport;
    EXPECT_CALL(*remoteObjectMocker, SendRequest(_, _, _, _)).WillOnce(Return(1));
    int32_t ret = proxy->OnDnsQueryResultReport(listsize, dnsResultReport);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetDnsResultCallbackProxyTest, NetDnsResultCallbackProxyTest_OnDnsQueryResultReport_003, TestSize.Level0)
{
    uint32_t listsize = 1;
    std::list<NetDnsQueryResultReport> dnsResultReport;
    NetDnsQueryResultReport report;
    dnsResultReport.push_back(report);
    int32_t ret = proxy->OnDnsQueryResultReport(listsize, dnsResultReport);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetDnsResultCallbackProxyTest, NetDnsResultCallbackProxyTest_OnDnsQueryAbnormalReport_001, TestSize.Level0)
{
    uint32_t eventfailcause = 1;
    NetDnsQueryResultReport report;
    int32_t ret = proxy->OnDnsQueryAbnormalReport(eventfailcause, report);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

} // namespace NetsysNative
} // namespace OHOS
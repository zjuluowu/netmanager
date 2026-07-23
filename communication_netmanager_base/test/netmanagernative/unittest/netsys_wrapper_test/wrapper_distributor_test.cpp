/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "net_manager_constants.h"
#include "notify_callback_stub.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#ifdef GTEST_API_
#define private public
#define protected public
#endif
#include "wrapper_distributor.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
using namespace std;
using namespace NetManagerStandard;
using namespace NetsysNative;
constexpr int32_t TEST_SOCKET = 112;
constexpr int32_t TEST_FORMAT = NetlinkDefine::NETLINK_FORMAT_BINARY_UNICAST;
constexpr const char *WIFI_AP_DEFAULT_IFACE_NAME = "wlan0";
mutex EXTERN_MUTEX;

class NotifyCallbackImp : public NotifyCallbackStub {
public:
    int32_t OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName,
                                      int flags, int scope) override
    {
        if (ifName == WIFI_AP_DEFAULT_IFACE_NAME) {
            flags_ = flags;
        }
        return NETMANAGER_EXT_SUCCESS;
    }

    int32_t OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName,
                                      int flags, int scope) override
    {
        if (ifName == WIFI_AP_DEFAULT_IFACE_NAME) {
            flags_ = flags;
        }
        return NETMANAGER_EXT_SUCCESS;
    }

    int32_t OnInterfaceAdded(const std::string &ifName) override
    {
        ifnameContainer_.emplace_back(ifName);
        return NETMANAGER_EXT_SUCCESS;
    }

    int32_t OnInterfaceRemoved(const std::string &ifName) override
    {
        auto itfind = std::find(ifnameContainer_.begin(), ifnameContainer_.end(), ifName);
        if (itfind != ifnameContainer_.end()) {
            ifnameContainer_.erase(itfind);
        }

        return NETMANAGER_EXT_SUCCESS;
    }

    int32_t OnInterfaceChanged(const std::string &ifName, bool up) override
    {
        if (ifName == WIFI_AP_DEFAULT_IFACE_NAME) {
            isWifiInterfaceChanged_ = up;
        }
        return NETMANAGER_EXT_SUCCESS;
    }

    int32_t OnInterfaceLinkStateChanged(const std::string &ifName, bool up) override
    {
        if (ifName == WIFI_AP_DEFAULT_IFACE_NAME) {
            isWifiLinkStateUp_ = up;
        }
        return NETMANAGER_EXT_SUCCESS;
    }

    int32_t OnRouteChanged(bool updated, const std::string &route, const std::string &gateway,
                           const std::string &ifName) override
    {
        if (ifName == WIFI_AP_DEFAULT_IFACE_NAME) {
            isRouteUpdated_ = updated;
        }
        return NETMANAGER_EXT_SUCCESS;
    }

    int32_t OnDhcpSuccess(sptr<DhcpResultParcel> &dhcpResult) override
    {
        return NETMANAGER_EXT_SUCCESS;
    }

    int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface) override
    {
        if (iface == WIFI_AP_DEFAULT_IFACE_NAME) {
            alertName_ = limitName;
        }
        return NETMANAGER_EXT_SUCCESS;
    }

#ifdef FEATURE_NET_FIREWALL_ENABLE
    int32_t OnInterceptRecord(sptr<NetManagerStandard::InterceptRecord> &record) override
    {
        return 0;
    }
#endif

    vector<std::string> ifnameContainer_;
    std::string  alertName_;
    int flags_ = 0;
    bool isWifiInterfaceChanged_ = false;
    bool isWifiLinkStateUp_ = false;
    bool isRouteUpdated_ = false;
};

} // namespace

class WrapperDistributorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<WrapperDistributor> instance_ =
        std::make_shared<WrapperDistributor>(TEST_SOCKET, TEST_FORMAT, EXTERN_MUTEX);
};

void WrapperDistributorTest::SetUpTestCase() {}

void WrapperDistributorTest::TearDownTestCase() {}

void WrapperDistributorTest::SetUp() {}

void WrapperDistributorTest::TearDown() {}

HWTEST_F(WrapperDistributorTest, SocketErrorTest001, TestSize.Level1)
{
    int32_t testSocket = -1;
    std::unique_ptr<WrapperDistributor> receiver = std::make_unique<WrapperDistributor>(testSocket, TEST_FORMAT,
                                                                                        EXTERN_MUTEX);
    ASSERT_NE(receiver, nullptr);
}

HWTEST_F(WrapperDistributorTest, FormatErrorTest001, TestSize.Level1)
{
    int32_t testFormat = 6;
    std::unique_ptr<WrapperDistributor> distributor = std::make_unique<WrapperDistributor>(TEST_SOCKET, testFormat,
                                                                                           EXTERN_MUTEX);
    ASSERT_NE(distributor, nullptr);
}

HWTEST_F(WrapperDistributorTest, StartTest001, TestSize.Level1)
{
    int32_t ret = instance_->Start();
    EXPECT_EQ(ret, NetlinkResult::OK);
}

HWTEST_F(WrapperDistributorTest, StopTest001, TestSize.Level1)
{
    int32_t ret = instance_->Stop();
    EXPECT_EQ(ret, NetlinkResult::OK);
}

HWTEST_F(WrapperDistributorTest, RegisterNetlinkCallbacksTest001, TestSize.Level1)
{
    int32_t ret = instance_->RegisterNetlinkCallbacks(nullptr);
    EXPECT_EQ(ret, NetlinkResult::ERR_NULL_PTR);
}

HWTEST_F(WrapperDistributorTest, RegisterNetlinkCallbacksTest002, TestSize.Level1)
{
    std::shared_ptr<NetsysEventMessage> message =  nullptr;
    instance_->HandleDecodeSuccess(message);

    auto callbacks_ = std::make_shared<std::vector<sptr<NetsysNative::INotifyCallback>>>();
    sptr<NotifyCallbackImp> notifyCallback = new NotifyCallbackImp();
    callbacks_->push_back(notifyCallback);
    int32_t ret = instance_->RegisterNetlinkCallbacks(callbacks_);
    EXPECT_EQ(ret, NetlinkResult::OK);

    message = std::make_shared<NetsysEventMessage>();
    message->SetAction(NetsysEventMessage::Action::ADD);
    message->SetSubSys(NetsysEventMessage::SubSys::NET);
    message->PushMessage(NetsysEventMessage::Type::INTERFACE, WIFI_AP_DEFAULT_IFACE_NAME);
    instance_->HandleDecodeSuccess(message);
    EXPECT_EQ(notifyCallback->ifnameContainer_.size(), 1);

    message->SetAction(NetsysEventMessage::Action::REMOVE);
    instance_->HandleDecodeSuccess(message);
    EXPECT_EQ(notifyCallback->ifnameContainer_.size(), 0);

    message->SetAction(NetsysEventMessage::Action::CHANGE);
    instance_->HandleDecodeSuccess(message);
    EXPECT_TRUE(notifyCallback->isWifiInterfaceChanged_);

    message->SetAction(NetsysEventMessage::Action::LINKUP);
    instance_->HandleDecodeSuccess(message);
    EXPECT_TRUE(notifyCallback->isWifiLinkStateUp_);

    message->SetAction(NetsysEventMessage::Action::LINKDOWN);
    instance_->HandleDecodeSuccess(message);
    EXPECT_FALSE(notifyCallback->isWifiLinkStateUp_);

    message->SetAction(NetsysEventMessage::Action::ADDRESSUPDATE);
    message->PushMessage(NetsysEventMessage::Type::ADDRESS, "127.0.0.1");
    message->PushMessage(NetsysEventMessage::Type::FLAGS, "1");
    message->PushMessage(NetsysEventMessage::Type::SCOPE, "1");
    instance_->HandleDecodeSuccess(message);
    EXPECT_EQ(notifyCallback->flags_, 1);

    message->SetAction(NetsysEventMessage::Action::ADDRESSREMOVED);
    message->PushMessage(NetsysEventMessage::Type::FLAGS, "2");
    message->PushMessage(NetsysEventMessage::Type::SCOPE, "2");
    instance_->HandleDecodeSuccess(message);
    EXPECT_EQ(notifyCallback->flags_, 2);

    message->SetAction(NetsysEventMessage::Action::ROUTEUPDATED);
    message->PushMessage(NetsysEventMessage::Type::ROUTE, "route");
    message->PushMessage(NetsysEventMessage::Type::GATEWAY, "gateway");
    instance_->HandleDecodeSuccess(message);
    EXPECT_TRUE(notifyCallback->isRouteUpdated_);

    message->SetAction(NetsysEventMessage::Action::ROUTEREMOVED);
    instance_->HandleDecodeSuccess(message);
    EXPECT_FALSE(notifyCallback->isRouteUpdated_);

    message->SetSubSys(NetsysEventMessage::SubSys::QLOG);
    message->PushMessage(NetsysEventMessage::Type::ALERT_NAME, "labelName");
    instance_->HandleDecodeSuccess(message);
    EXPECT_EQ(notifyCallback->alertName_, "labelName");
}

HWTEST_F(WrapperDistributorTest, WrapperDistributorBranchTest001, TestSize.Level1)
{
    instance_->netlinkCallbacks_ = nullptr;
    std::shared_ptr<NetsysEventMessage> message = std::make_shared<NetsysEventMessage>();
    instance_->HandleDecodeSuccess(message);

    std::string ifName = "";
    instance_->NotifyInterfaceAdd(ifName);
    instance_->NotifyInterfaceRemove(ifName);
    instance_->NotifyInterfaceChange(ifName, false);
    instance_->NotifyInterfaceLinkStateChange(ifName, false);

    std::string labelName = "";
    instance_->NotifyQuotaLimitReache(labelName, ifName);
    std::string addr = "";
    int32_t flags = 0;
    int32_t scope = 0;
    instance_->NotifyInterfaceAddressUpdate(addr, ifName, flags, scope);
    instance_->NotifyInterfaceAddressRemove(addr, ifName, flags, scope);

    std::string route = "";
    std::string gateway = "";
    instance_->NotifyRouteChange(false, route, gateway, ifName);

    int32_t ret = instance_->RegisterNetlinkCallbacks(nullptr);
    EXPECT_EQ(ret, NetlinkResult::ERR_NULL_PTR);
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
HWTEST_F(WrapperDistributorTest, WrapperDistributorBranchTest002, TestSize.Level1)
{
    std::shared_ptr<WrapperDistributor> instance =
        std::make_shared<WrapperDistributor>(TEST_SOCKET, TEST_FORMAT, EXTERN_MUTEX);
    instance->netlinkCallbacks_ = nullptr;
    std::shared_ptr<NetsysEventMessage> message = std::make_shared<NetsysEventMessage>();

    instance->HandleSubSysNflog(message);

    auto callbacks_ = std::make_shared<std::vector<sptr<NetsysNative::INotifyCallback>>>();
    sptr<NotifyCallbackImp> notifyCallback = new (std::nothrow) NotifyCallbackImp();
    callbacks_->push_back(notifyCallback);
    int32_t ret = instance->RegisterNetlinkCallbacks(callbacks_);
    instance->HandleSubSysNflog(message);
    EXPECT_EQ(ret, NetlinkResult::OK);
}
#endif
} // namespace nmd
} // namespace OHOS
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <iostream>
#include <memory>

#include <gtest/gtest.h>

#include "i_netsys_service.h"
#include "iservice_registry.h"
#include "notify_callback_stub.h"
#include "system_ability_definition.h"

namespace OHOS::nmd {
using namespace testing::ext;
constexpr const char *IFACENAME = "wlan0";
bool g_flag = true;
sptr<OHOS::NetsysNative::INotifyCallback> nativeNotifyCallback_ = nullptr;
sptr<OHOS::NetsysNative::INetsysService> netsysNativeService_ = nullptr;
class NetlinkNativeNotifyCallBack : public OHOS::NetsysNative::NotifyCallbackStub {
public:
    int32_t OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName, int flags,
                                      int scope) override;
    int32_t OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName, int flags,
                                      int scope) override;
    int32_t OnInterfaceAdded(const std::string &ifName) override;
    int32_t OnInterfaceRemoved(const std::string &ifName) override;
    int32_t OnInterfaceChanged(const std::string &ifName, bool up) override;
    int32_t OnInterfaceLinkStateChanged(const std::string &ifName, bool up) override;
    int32_t OnRouteChanged(bool updated, const std::string &route, const std::string &gateway,
                           const std::string &ifName) override;
    int32_t OnDhcpSuccess(sptr<OHOS::NetsysNative::DhcpResultParcel> &dhcpResult) override;
    int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface) override;
};

class NetsysWrapperTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            return;
        }
        auto remote = samgr->GetSystemAbility(OHOS::COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
        if (remote == nullptr) {
            return;
        }
        nativeNotifyCallback_ = new NetlinkNativeNotifyCallBack();
        auto proxy = iface_cast<NetsysNative::INetsysService>(remote);
        netsysNativeService_ = proxy;
    }
    static void TearDownTestCase()
    {
        nativeNotifyCallback_ = nullptr;
        netsysNativeService_ = nullptr;
    }
};

int32_t NetlinkNativeNotifyCallBack::OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName,
                                                               int flags, int scope)
{
    std::cout << " [OnInterfaceAddressUpdated] " << ifName << " Address: " << addr << std::endl;
    EXPECT_FALSE(addr.empty());
    EXPECT_STRCASEEQ(IFACENAME, ifName.c_str());
    EXPECT_GE(flags, 0);
    EXPECT_GE(scope, 0);
    return 0;
}
int32_t NetlinkNativeNotifyCallBack::OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName,
                                                               int flags, int scope)
{
    std::cout << " [OnInterfaceAddressRemoved] " << ifName << " Address: " << addr << std::endl;
    EXPECT_FALSE(addr.empty());
    EXPECT_STRCASEEQ(IFACENAME, ifName.c_str());
    EXPECT_GE(flags, 0);
    EXPECT_GE(scope, 0);
    return 0;
}
int32_t NetlinkNativeNotifyCallBack::OnInterfaceAdded(const std::string &ifName)
{
    std::cout << " [OnInterfaceAdded] " << ifName << std::endl;
    EXPECT_STRCASEEQ(IFACENAME, ifName.c_str());
    return 0;
}
int32_t NetlinkNativeNotifyCallBack::OnInterfaceRemoved(const std::string &ifName)
{
    std::cout << " [OnInterfaceRemoved] " << ifName << std::endl;
    EXPECT_STRCASEEQ(IFACENAME, ifName.c_str());
    return 0;
}
int32_t NetlinkNativeNotifyCallBack::OnInterfaceChanged(const std::string &ifName, bool up)
{
    std::cout << " [OnInterfaceChanged] " << ifName << " status :" << (up ? "[update]" : "[remove]") << std::endl;
    EXPECT_STRCASEEQ(IFACENAME, ifName.c_str());
    if (g_flag) {
        EXPECT_TRUE(up);
    } else {
        EXPECT_FALSE(up);
    }
    return 0;
}
int32_t NetlinkNativeNotifyCallBack::OnInterfaceLinkStateChanged(const std::string &ifName, bool up)
{
    EXPECT_STRCASEEQ(IFACENAME, ifName.c_str());
    if (g_flag) {
        EXPECT_TRUE(up);
    } else {
        EXPECT_FALSE(up);
    }
    return 0;
}
int32_t NetlinkNativeNotifyCallBack::OnRouteChanged(bool updated, const std::string &route, const std::string &gateway,
                                                    const std::string &ifName)
{
    EXPECT_STRCASEEQ(IFACENAME, ifName.c_str());
    EXPECT_FALSE(route.empty());
    EXPECT_FALSE(gateway.empty());
    if (g_flag) {
        EXPECT_TRUE(updated);
    } else {
        EXPECT_FALSE(updated);
    }
    return 0;
}

int32_t NetlinkNativeNotifyCallBack::OnDhcpSuccess(sptr<OHOS::NetsysNative::DhcpResultParcel> &dhcpResult)
{
    std::cout << " [OnDhcpSuccess] " << std::endl;
    EXPECT_NE(dhcpResult, nullptr);
    return 0;
}
int32_t NetlinkNativeNotifyCallBack::OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface)
{
    std::cout << " [OnBandwidthReachedLimit] " << std::endl;
    EXPECT_FALSE(limitName.empty());
    EXPECT_FALSE(iface.empty());
    return 0;
}

HWTEST_F(NetsysWrapperTest, TestServiceGet001, TestSize.Level1)
{
    EXPECT_NE(netsysNativeService_, nullptr);
}
} // namespace OHOS::nmd
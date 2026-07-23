/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "ethernet_client.h"
#include "http_proxy.h"
#include "inet_addr.h"
#include "mac_address_info.h"
#include "interface_configuration.h"
#include "interface_state_callback_stub.h"
#include "interface_type.h"
#include "net_manager_constants.h"
#include "netmanager_ext_test_security.h"
#include "netmgr_ext_log_wrapper.h"
#include "refbase.h"
#include "singleton.h"
#include "static_configuration.h"
#include "gtest/gtest-message.h"
#include "gtest/gtest-test-part.h"
#include "gtest/hwext/gtest-ext.h"
#include "gtest/hwext/gtest-tag.h"

#define private public
#define protected public
#include "ethernet_client.h"
#include "ethernet_dhcp_controller.h"
#include "ethernet_management.h"
#include "ethernet_service.h"
#include "ethernet_service_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *DEV_NAME = "eth0";
constexpr const char *DEV_UP = "up";
constexpr const char *DEV_DOWN = "down";
constexpr const char *TEST_PROXY_HOST = "127.0.0.1";
constexpr const char *TEST_MAC_ADDRESS = "a0:0b:c1:d0:02:03";
constexpr uint16_t TEST_PROXY_PORT = 8080;
std::string INFO = "info";
constexpr const char *IFACE = "iface0";
const int32_t FD = 5;
const int32_t SYSTEM_ABILITY_INVALID = 666;
constexpr uint16_t DEPENDENT_SERVICE_All = 0x0003;
const int32_t RET_ZERO = 0;

class MonitorInterfaceStateCallback : public InterfaceStateCallbackStub {
public:
    int32_t OnInterfaceAdded(const std::string &ifName) override
    {
        std::cout << "OnInterfaceAdded ifName: " << ifName << std::endl;
        return 0;
    }

    int32_t OnInterfaceRemoved(const std::string &ifName) override
    {
        std::cout << "OnInterfaceRemoved ifName: " << ifName << std::endl;
        return 0;
    }

    int32_t OnInterfaceChanged(const std::string &ifName, bool up) override
    {
        std::cout << "OnInterfaceChange ifName: " << ifName << ", state: " << up << std::endl;
        return 0;
    }

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        std::u16string descriptor = data.ReadInterfaceToken();
        if (descriptor != InterfaceStateCallback::GetDescriptor()) {
            NETMGR_EXT_LOG_E("OnRemoteRequest get descriptor error.");
            return NETMANAGER_EXT_ERR_DESCRIPTOR_MISMATCH;
        }
        InterfaceStateCallback::Message msgCode = static_cast<InterfaceStateCallback::Message>(code);
        switch (msgCode) {
            case InterfaceStateCallback::Message::INTERFACE_STATE_ADD: {
                OnInterfaceAdded(data.ReadString());
                break;
            }
            case InterfaceStateCallback::Message::INTERFACE_STATE_REMOVE: {
                OnInterfaceRemoved(data.ReadString());
                break;
            }
            case InterfaceStateCallback::Message::INTERFACE_STATE_CHANGE: {
                OnInterfaceChanged(data.ReadString(), data.ReadBool());
                break;
            }
            default:
                return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
        return NETMANAGER_EXT_SUCCESS;
    }
};
} // namespace

class EthernetManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<InterfaceConfiguration> GetIfaceConfig();
    bool CheckIfaceUp(const std::string &iface);
};

void EthernetManagerTest::SetUpTestCase() {}

void EthernetManagerTest::TearDownTestCase() {}

void EthernetManagerTest::SetUp() {}

void EthernetManagerTest::TearDown() {}

sptr<InterfaceConfiguration> EthernetManagerTest::GetIfaceConfig()
{
    sptr<InterfaceConfiguration> ic = (std::make_unique<InterfaceConfiguration>()).release();
    if (!ic) {
        return ic;
    }
    INetAddr ipv4Addr;
    ipv4Addr.type_ = INetAddr::IPV4;
    ipv4Addr.family_ = 0x01;
    ipv4Addr.prefixlen_ = 0x01;
    ipv4Addr.address_ = "172.17.5.234";
    ipv4Addr.netMask_ = "255.255.254.0";
    ipv4Addr.hostName_ = "netAddr";
    ic->ipStatic_.ipAddrList_.push_back(ipv4Addr);
    INetAddr route;
    route.type_ = INetAddr::IPV4;
    route.family_ = 0x01;
    route.prefixlen_ = 0x01;
    route.address_ = "0.0.0.0";
    route.netMask_ = "0.0.0.0";
    route.hostName_ = "netAddr";
    ic->ipStatic_.routeList_.push_back(route);
    INetAddr gateway;
    gateway.type_ = INetAddr::IPV4;
    gateway.family_ = 0x01;
    gateway.prefixlen_ = 0x01;
    gateway.address_ = "172.17.4.1";
    gateway.netMask_ = "0.0.0.0";
    gateway.hostName_ = "netAddr";
    ic->ipStatic_.gatewayList_.push_back(gateway);
    INetAddr netMask;
    netMask.type_ = INetAddr::IPV4;
    netMask.family_ = 0x01;
    netMask.address_ = "255.255.255.0";
    netMask.hostName_ = "netAddr";
    ic->ipStatic_.netMaskList_.push_back(netMask);
    ic->httpProxy_ = {TEST_PROXY_HOST, TEST_PROXY_PORT, {}};
    INetAddr dns1;
    dns1.type_ = INetAddr::IPV4;
    dns1.family_ = 0x01;
    dns1.address_ = "8.8.8.8";
    dns1.hostName_ = "netAddr";
    INetAddr dns2;
    dns2.type_ = INetAddr::IPV4;
    dns2.family_ = 0x01;
    dns2.address_ = "114.114.114.114";
    dns2.hostName_ = "netAddr";
    ic->ipStatic_.dnsServers_.push_back(dns1);
    ic->ipStatic_.dnsServers_.push_back(dns2);
    return ic;
}

bool EthernetManagerTest::CheckIfaceUp(const std::string &iface)
{
    NetManagerExtAccessToken token;
    int32_t activeStatus = 0;
    (void)DelayedSingleton<EthernetClient>::GetInstance()->IsIfaceActive(iface, activeStatus);
    return activeStatus == 1;
}

/**
 * @tc.name: OnRemoteRequest
 * @tc.desc: Test EthernetManager OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, OnRemoteRequest, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = 0;
    ret = DelayedSingleton<MonitorInterfaceStateCallback>::GetInstance()->OnRemoteRequest(code, data, reply, option);
    EXPECT_NE(ret, NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: EthernetManager001
 * @tc.desc: Test EthernetManager SetIfaceConfig.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager001, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    sptr<InterfaceConfiguration> ic = GetIfaceConfig();
    ASSERT_EQ(DelayedSingleton<EthernetClient>::GetInstance()->SetIfaceConfig(DEV_NAME, ic), NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: EthernetManager0011
 * @tc.desc: Test EthernetManager SetIfaceConfig.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager0011, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    sptr<InterfaceConfiguration> ic = GetIfaceConfig();
    const char *DEV_NAME_1 = "eth3";
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->SetIfaceConfig(DEV_NAME_1, ic);
    ASSERT_EQ(ret, ETHERNET_ERR_DEVICE_INFORMATION_NOT_EXIST);
}

/**
 * @tc.name: EthernetManager002
 * @tc.desc: Test EthernetManager GetIfaceConfig.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager002, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    sptr<InterfaceConfiguration> ic;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetIfaceConfig(DEV_NAME, ic);
    ASSERT_TRUE(ic != nullptr);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: EthernetManager0021
 * @tc.desc: Test EthernetManager GetIfaceConfig.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager0021, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    sptr<InterfaceConfiguration> ic;
    const char *DEV_NAME_1 = "eth3";
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetIfaceConfig(DEV_NAME_1, ic);
    ASSERT_FALSE(ic != nullptr);
    EXPECT_EQ(ret, ETHERNET_ERR_DEVICE_INFORMATION_NOT_EXIST);
}

/**
 * @tc.name: EthernetManager003
 * @tc.desc: Test EthernetManager IsIfaceActive.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager003, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    int32_t activeStatus = -1;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->IsIfaceActive(DEV_NAME, activeStatus);
    ASSERT_EQ(activeStatus, 1);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: EthernetManager0031
 * @tc.desc: Test EthernetManager IsIfaceActive.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager0031, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    int32_t activeStatus = -1;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->IsIfaceActive("eth3", activeStatus);
    ASSERT_NE(activeStatus, 1);
    EXPECT_EQ(ret, ETHERNET_ERR_DEVICE_INFORMATION_NOT_EXIST);
}

/**
 * @tc.name: EthernetManager004
 * @tc.desc: Test EthernetManager GetAllActiveIfaces.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager004, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    std::vector<std::string> result;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetAllActiveIfaces(result);
    std::vector<std::string>::iterator it = std::find(result.begin(), result.end(), DEV_NAME);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
    ASSERT_TRUE(it != result.end());
}

/**
 * @tc.name: EthernetManager005
 * @tc.desc: Test EthernetManager GetMacAddress.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, EthernetManager005, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    std::vector<MacAddressInfo> mai;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetMacAddress(mai);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: ResetFactoryTest001
 * @tc.desc: Test EthernetManager ResetFactory.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, ResetFactoryTest001, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->ResetFactory();
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

/**
 * @tc.name: ResetFactoryTest002
 * @tc.desc: Test EthernetManager ResetFactory.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, ResetFactoryTest002, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->ResetFactory();
    EXPECT_EQ(ret, NETMANAGER_EXT_ERR_PERMISSION_DENIED);
}

/**
 * @tc.name: ResetFactoryTest003
 * @tc.desc: Test EthernetManager ResetFactory.
 * @tc.type: FUNC
 */
HWTEST_F(EthernetManagerTest, ResetFactoryTest003, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->ResetFactory();
    EXPECT_EQ(ret, NETMANAGER_EXT_ERR_PERMISSION_DENIED);
}

HWTEST_F(EthernetManagerTest, EthernetManager006, TestSize.Level1)
{
    ASSERT_NE(CheckIfaceUp(DEV_NAME), false);
    NetManagerExtAccessToken token;
    OHOS::nmd::InterfaceConfigurationParcel cfg;
    int32_t ret = DelayedSingleton<EthernetClient>::GetInstance()->GetInterfaceConfig(DEV_NAME, cfg);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
    ASSERT_FALSE(cfg.ifName.empty());
    ASSERT_FALSE(cfg.hwAddr.empty());
}

HWTEST_F(EthernetManagerTest, OnInterfaceAddressUpdatedTest001, TestSize.Level1)
{
    EthernetManagement ethernetmanagement;
    EthernetManagement::DevInterfaceStateCallback devinterfacestatecallback(ethernetmanagement);
    std::string addr;
    std::string ifName;
    int flags = 0;
    int scope = 0;
    int ret = devinterfacestatecallback.OnInterfaceAddressUpdated(addr, ifName, flags, scope);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

HWTEST_F(EthernetManagerTest, OnInterfaceAddressRemovedTest001, TestSize.Level1)
{
    EthernetManagement ethernetmanagement;
    EthernetManagement::DevInterfaceStateCallback devinterfacestatecallback(ethernetmanagement);
    std::string addr;
    std::string ifName;
    int flags = 0;
    int scope = 0;
    int ret = devinterfacestatecallback.OnInterfaceAddressRemoved(addr, ifName, flags, scope);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

HWTEST_F(EthernetManagerTest, OnInterfaceAddedTest001, TestSize.Level1)
{
    EthernetManagement ethernetmanagement;
    EthernetManagement::DevInterfaceStateCallback devinterfacestatecallback(ethernetmanagement);
    std::string iface = "eth0";
    int ret = devinterfacestatecallback.OnInterfaceAdded(iface);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}

HWTEST_F(EthernetManagerTest, OnInterfaceRemovedTest001, TestSize.Level1)
{
    EthernetManagement ethernetmanagement;
    EthernetManagement::DevInterfaceStateCallback devinterfacestatecallback(ethernetmanagement);
    std::string iface = "eth0";
    int ret = devinterfacestatecallback.OnInterfaceRemoved(iface);
    EXPECT_EQ(ret, NETMANAGER_EXT_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

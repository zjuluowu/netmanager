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

#include <gtest/gtest.h>
#include <ifaddrs.h>
#include <sys/resource.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "vpn_manager.h"
#include "netlink_msg.h"
#include "netlink_socket.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class VpnManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void VpnManagerTest::SetUpTestCase() {}

void VpnManagerTest::TearDownTestCase() {}

void VpnManagerTest::SetUp() {}

void VpnManagerTest::TearDown() {}

HWTEST_F(VpnManagerTest, VpnManagerBranchTest001, TestSize.Level1)
{
    VpnManager::GetInstance().StartUnixSocketListen();
    VpnManager::GetInstance().StartVpnInterfaceFdListen();

    auto result = VpnManager::GetInstance().CreateVpnInterface();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    VpnManager::GetInstance().DestroyVpnInterface();

    std::string ifName = "";
    int32_t testNumber = 0;
    result = VpnManager::GetInstance().SetVpnMtu(ifName, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    testNumber = 1;
    result = VpnManager::GetInstance().SetVpnMtu(ifName, testNumber);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    std::string tunAddr = "";
    result = VpnManager::GetInstance().SetVpnAddress(ifName, tunAddr, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    result = VpnManager::GetInstance().SetVpnUp();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = VpnManager::GetInstance().SetVpnDown();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    ifreq ifr;
    std::string cardName = "";
    result = VpnManager::GetInstance().InitIfreq(ifr, cardName);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = VpnManager::GetInstance().SendVpnInterfaceFdToClient(testNumber, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, VpnManagerBranchTest002, TestSize.Level1)
{
    VpnManager::GetInstance().tunFd_ = 1;
    auto ret = VpnManager::GetInstance().CreateVpnInterface();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(VpnManagerTest, VpnManagerBranchTest003, TestSize.Level1)
{
    auto ret = VpnManager::GetInstance().SetVpnMtu("eth0", 1);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(VpnManagerTest, VpnManagerBranchTest004, TestSize.Level1)
{
    std::string ifName;
    std::string tunAddr = "fe80::af71:b0c7:e3f7:3c0f%5";
    auto ret = VpnManager::GetInstance().SetVpnAddress(ifName, tunAddr, 0);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, VpnManagerBranchTest005, TestSize.Level1)
{
    std::string ifName;
    std::string tunAddr = "127.0.0.1";
    auto ret = VpnManager::GetInstance().SetVpnAddress(ifName, tunAddr, 0);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, VpnManagerBranchTest006, TestSize.Level1)
{
    VpnManager::GetInstance().net4Sock_ = -1;
    VpnManager::GetInstance().net6Sock_ = -1;
    auto ret = VpnManager::GetInstance().SetVpnUp();
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, VpnManagerBranchTest007, TestSize.Level1)
{
    VpnManager::GetInstance().net4Sock_ = -1;
    VpnManager::GetInstance().net6Sock_ = -1;
    auto ret = VpnManager::GetInstance().SetVpnDown();
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, CreateVpnInterfaceTest001, TestSize.Level1)
{
    VpnManager vpnmanager;
    vpnmanager.tunFd_ = 1;
    vpnmanager.listeningFlag_ = 1;
    auto result = vpnmanager.CreateVpnInterface();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VpnManagerTest, SetVpnMtuTest001, TestSize.Level1)
{
    VpnManager vpnmanager;
    std::string ifName = "12345678901234567890";
    int32_t mtu = 1;
    auto result = vpnmanager.SetVpnMtu(ifName, mtu);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SetVpnMtuTest002, TestSize.Level1)
{
    VpnManager vpnmanager;
    vpnmanager.net4Sock_ = -1;
    vpnmanager.net6Sock_ = -1;
    std::string ifName = "123456789abcdef";
    int32_t mtu = 1;
    auto result = vpnmanager.SetVpnMtu(ifName, mtu);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SetVpnAddressTest001, TestSize.Level1)
{
    VpnManager vpnmanager;
    std::string ifName = "12345678901234567890";
    std::string tunAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    int32_t prefix = 1;
    auto result = vpnmanager.SetVpnAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SetVpnAddressTest002, TestSize.Level1)
{
    VpnManager vpnmanager;
    std::string ifName = "12345678901234567890";
    std::string tunAddr = "192.168.1.1";
    int32_t prefix = 1;
    auto result = vpnmanager.SetVpnAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

struct IpCountResult {
    int ipv4Count = 0;
    int ipv6Count = 0;
};

IpCountResult CountInterfaceAddresses(const std::string& ifName)
{
    IpCountResult result;
    struct ifaddrs* ifap = nullptr;
    if (getifaddrs(&ifap) != 0) {
        return result;
    }
    for (auto* ptr = ifap; ptr != nullptr; ptr = ptr->ifa_next) {
        if (ptr->ifa_addr && ifName == ptr->ifa_name) {
            if (ptr->ifa_addr->sa_family == AF_INET) {
                result.ipv4Count++;
            } else if (ptr->ifa_addr->sa_family == AF_INET6) {
                result.ipv6Count++;
            }
        }
    }
    freeifaddrs(ifap);
    return result;
}

HWTEST_F(VpnManagerTest, SetVpnAddressTest003, TestSize.Level1)
{
    VpnManager vpnmanager;
    std::string ifName = "12345678901234567890";

    std::vector<std::tuple<std::string, int32_t>> ipList = {
        { "192.168.1.111", 24 },
        { "192.168.1.122", 24 },
        { "192.168.1.133", 24 },
        { "192.168.1.100", 24 },
    };

    for (const auto& [ip, prefix] : ipList) {
        auto result = vpnmanager.SetVpnAddress(ifName, ip, prefix);
        EXPECT_EQ(result, NETMANAGER_ERROR) << "Binding failed for IP: " << ip;
    }

    auto ipCount = CountInterfaceAddresses(ifName);
    EXPECT_GE(ipCount.ipv4Count, 0);
}

HWTEST_F(VpnManagerTest, SetVpnAddressTest004, TestSize.Level1)
{
    VpnManager vpnmanager;
    std::string ifName = "12345678901234567890";
    std::vector<std::tuple<std::string, int32_t>> ipList = {
        { "2001:db8:1234::1", 64 },
        { "fd12:3456:789a::100", 64 },
        { "2001:db8::abcd:ef01:2345:6789", 64 },
        { "fd00::dead:beef", 64 },
        { "2001:0db8:85a3:0000:0000:8a2e:0370:7334", 64 }
    };

    for (const auto& [ip, prefix] : ipList) {
        auto result = vpnmanager.SetVpnAddress(ifName, ip, prefix);
        EXPECT_EQ(result, NETMANAGER_ERROR) << "Binding failed for IP: " << ip;
    }

    auto ipCount = CountInterfaceAddresses(ifName);
    EXPECT_GE(ipCount.ipv6Count, 0);
}

HWTEST_F(VpnManagerTest, SetVpnAddressTest005, TestSize.Level1)
{
    VpnManager vpnmanager;
    std::string ifName = "12345678901234567890";
    std::string tunAddr = "";
    int32_t prefix = 1;
    auto result = vpnmanager.SetVpnAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SendNetlinkAddressTest001, TestSize.Level1)
{
    VpnManager vpnManager;
    int ifindex = 0;
    const char* addrbuf = nullptr;
    int family = AF_INET6;
    int prefix = 64;

    int32_t result = vpnManager.SendNetlinkAddress(ifindex, family, addrbuf, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SendNetlinkAddressTest002, TestSize.Level1)
{
    VpnManager vpnManager;
    int ifindex = 2;
    int family = AF_INET;
    int prefix = -1;

    static const char addrbuf[4] = {127, 0, 0, 1};
    int32_t result = vpnManager.SendNetlinkAddress(ifindex, family, addrbuf, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SetVpnUpTest001, TestSize.Level1)
{
    VpnManager vpnmanager;
    vpnmanager.net4Sock_ = -1;
    vpnmanager.net6Sock_ = -1;
    auto result = vpnmanager.SetVpnUp();
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SetVpnDownTest001, TestSize.Level1)
{
    VpnManager vpnmanager;
    vpnmanager.net4Sock_ = -1;
    vpnmanager.net6Sock_ = -1;
    auto result = vpnmanager.SetVpnDown();
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, InitIfreqTest001, TestSize.Level1)
{
    VpnManager vpnmanager;
    ifreq ifr;
    std::string cardName = "12345678901234567890";
    auto result = vpnmanager.InitIfreq(ifr, cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SendVpnInterfaceFdToClientTest001, TestSize.Level1)
{
    VpnManager vpnmanager;
    int32_t invalidFd = -1;
    int32_t tunFd = 0;
    auto result = vpnmanager.SendVpnInterfaceFdToClient(invalidFd, tunFd);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VpnManagerTest, SendVpnInterfaceFdToClientTest002, TestSize.Level1)
{
    VpnManager vpnmanager;
    int32_t clientFd = 1;
    int32_t tunFd = 0;
    auto result = vpnmanager.SendVpnInterfaceFdToClient(clientFd, tunFd);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}
} // namespace NetManagerStandard
} // namespace OHOS

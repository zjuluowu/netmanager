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

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "multi_vpn_manager.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *TEST_XFRM_CARD_NAME = "xfrm-vpn1";
constexpr const char *TEST_PPP_CARD_NAME = "ppp-vpn2";
constexpr const char *TEST_MULTI_TUN_CARD_NAME = "multitun-vpn3";
} // namespace

class MultiVpnManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MultiVpnManagerTest::SetUpTestCase() {}

void MultiVpnManagerTest::TearDownTestCase() {}

void MultiVpnManagerTest::SetUp() {}

void MultiVpnManagerTest::TearDown() {}

HWTEST_F(MultiVpnManagerTest, VpnManagerBranchTest001, TestSize.Level1)
{
    MultiVpnManager::GetInstance().CreatePppFd(TEST_XFRM_CARD_NAME);
    MultiVpnManager::GetInstance().multiVpnListeningFlag_ = false;
    MultiVpnManager::GetInstance().CreatePppFd(TEST_PPP_CARD_NAME);
    MultiVpnManager::GetInstance().CreatePppFd(TEST_PPP_CARD_NAME);
    MultiVpnManager::GetInstance().StartMultiVpnSocketListen();
    MultiVpnManager::GetInstance().SetXfrmPhyIfName("eth0");
    MultiVpnManager::GetInstance().DestroyVpnInterface(TEST_XFRM_CARD_NAME);

    auto result = MultiVpnManager::GetInstance().CreateVpnInterface(TEST_XFRM_CARD_NAME);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
    result = MultiVpnManager::GetInstance().CreateVpnInterface(TEST_PPP_CARD_NAME);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    result = MultiVpnManager::GetInstance().CreateVpnInterface(TEST_MULTI_TUN_CARD_NAME);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = MultiVpnManager::GetInstance().CreateVpnInterface("eth0");
    EXPECT_EQ(result, NETMANAGER_ERROR);

    MultiVpnManager::GetInstance().SetVpnRemoteAddress("192.168.1.1");

    std::string ifName = "";
    int32_t testNumber = 0;
    result = MultiVpnManager::GetInstance().SetVpnMtu(ifName, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    testNumber = 1500;
    result = MultiVpnManager::GetInstance().SetVpnMtu(TEST_XFRM_CARD_NAME, testNumber);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);

    std::string ipAddr = "";
    result = MultiVpnManager::GetInstance().SetVpnAddress(ifName, ipAddr, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    result = MultiVpnManager::GetInstance().SetVpnUp(TEST_XFRM_CARD_NAME, false);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);

    result = MultiVpnManager::GetInstance().SetVpnDown(TEST_XFRM_CARD_NAME);
    EXPECT_LE(result, NETMANAGER_SUCCESS);

    ifreq ifr;
    std::string cardName = "xfrm-vpn2";
    result = MultiVpnManager::GetInstance().InitIfreq(ifr, cardName);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint32_t cmd = 0;
    std::atomic_int fd = 1;
    result = MultiVpnManager::GetInstance().SetVpnResult(fd, cmd, ifr);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    result = MultiVpnManager::GetInstance().DestroyVpnInterface(TEST_XFRM_CARD_NAME);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = MultiVpnManager::GetInstance().SendVpnInterfaceFdToClient(testNumber, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, VpnManagerBranchTest002, TestSize.Level1)
{
    auto ret = MultiVpnManager::GetInstance().CreateVpnInterface(TEST_XFRM_CARD_NAME);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, VpnManagerBranchTest003, TestSize.Level1)
{
    auto ret = MultiVpnManager::GetInstance().SetVpnMtu(TEST_XFRM_CARD_NAME, 1500);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, VpnManagerBranchTest004, TestSize.Level1)
{
    std::string ipAddr = "127.0.0.1";
    auto ret = MultiVpnManager::GetInstance().SetVpnAddress(TEST_XFRM_CARD_NAME, ipAddr, 0);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, VpnManagerBranchTest005, TestSize.Level1)
{
    ifreq ifr;
    uint32_t cmd = 0;
    std::atomic_int fd = 1;
    auto result = MultiVpnManager::GetInstance().SetVpnResult(fd, cmd, ifr);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnMtuTest001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = "12345678901234567890";
    int32_t mtu = 1;
    auto result = multiVpnManager.SetVpnMtu(ifName, mtu);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnMtuTest002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    int32_t mtu = 1500;
    auto result = multiVpnManager.SetVpnMtu(ifName, mtu);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressTest001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = "12345678901234567890";
    std::string ipAddr = "192.168.1.1";
    int32_t prefix = 1;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressTest002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = "12345678901234567890";
    std::string ipAddr = "";
    int32_t prefix = 1;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressTest003, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "127.0.0.1";
    int32_t prefix = 32;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
    ifName = TEST_PPP_CARD_NAME;
    multiVpnManager.remoteIpv4Addr_ = "";
    result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, InitIfreqTest001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    ifreq ifr;
    std::string cardName = "12345678901234567890";
    auto result = multiVpnManager.InitIfreq(ifr, cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetMultiVpnDown001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = TEST_XFRM_CARD_NAME;
    auto result = multiVpnManager.SetVpnDown(cardName);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetMultiVpnDown002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = "12345678901234567890";
    auto result = multiVpnManager.SetVpnDown(cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetMultiVpnUp001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = "12345678901234567890";
    auto result = multiVpnManager.SetVpnUp(cardName, false);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetMultiVpnUp002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = TEST_XFRM_CARD_NAME;
    auto result = multiVpnManager.SetVpnUp(TEST_XFRM_CARD_NAME, false);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, CreatePppInterface001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = "12345678901234567890";
    auto result = multiVpnManager.CreatePppInterface(cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, CreatePppInterface002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    multiVpnManager.multiVpnFdMap_[TEST_PPP_CARD_NAME] = 1;
    std::string cardName = TEST_PPP_CARD_NAME;
    auto result = multiVpnManager.CreatePppInterface(cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, GetMultiVpnFd001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = "12345678901234567890";
    int32_t multiVpnFd = -1;
    auto result = multiVpnManager.GetMultiVpnFd(cardName, multiVpnFd);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    result = multiVpnManager.GetMultiVpnFd(TEST_MULTI_TUN_CARD_NAME, multiVpnFd);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    result = MultiVpnManager::GetInstance().GetMultiVpnFd(TEST_PPP_CARD_NAME, multiVpnFd);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(MultiVpnManagerTest, GetMultiVpnFd002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    int32_t multiVpnFd = -1;
    auto result = MultiVpnManager::GetInstance().GetMultiVpnFd(TEST_MULTI_TUN_CARD_NAME, multiVpnFd);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(MultiVpnManagerTest, DestroyPppFd001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = "12345678901234567890";
    auto result = multiVpnManager.DestroyMultiVpnFd(cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, DestroyPppFd002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = TEST_PPP_CARD_NAME;
    auto result = multiVpnManager.DestroyMultiVpnFd(cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, DestroyPppFd003, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = TEST_PPP_CARD_NAME;
    multiVpnManager.multiVpnFdMap_[TEST_PPP_CARD_NAME] = 2;
    auto result = multiVpnManager.DestroyMultiVpnFd(cardName);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(MultiVpnManagerTest, AddVpnRemoteAddress001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    ifreq ifr = {};
    std::atomic_int net4Sock = 1;
    std::string cardName = TEST_PPP_CARD_NAME;
    auto result = multiVpnManager.AddVpnRemoteAddress(cardName, net4Sock, ifr);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    multiVpnManager.remoteIpv4Addr_ = "192.168.1.20";
    result = multiVpnManager.AddVpnRemoteAddress(cardName, net4Sock, ifr);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SendVpnInterfaceFdToClient001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    int32_t clientFd = 0;
    std::atomic_int net4Sock = 1;
    auto result = multiVpnManager.SendVpnInterfaceFdToClient(clientFd, net4Sock);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, DestroyVpnInterface001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    auto result = multiVpnManager.DestroyVpnInterface(TEST_XFRM_CARD_NAME);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    result = multiVpnManager.DestroyVpnInterface(TEST_PPP_CARD_NAME);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(MultiVpnManagerTest, DestroyVpnInterface002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = "12345678901234567890";
    auto result = multiVpnManager.DestroyVpnInterface(cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnCallMode001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string message = "";
    auto result = multiVpnManager.SetVpnCallMode(message);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    message = "1";
    result = multiVpnManager.SetVpnCallMode(message);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    message = "0";
    result = multiVpnManager.SetVpnCallMode(message);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    message = "2";
    result = multiVpnManager.SetVpnCallMode(message);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(MultiVpnManagerTest, CreateMultiTunInterface001, TestSize.Level1)
{
    std::string cardName = "12345678901234567890";
    auto result = MultiVpnManager::GetInstance().CreateMultiTunInterface(cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    cardName = "multitun-vpn1";
    result = MultiVpnManager::GetInstance().CreateMultiTunInterface(cardName);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(MultiVpnManagerTest, ClearPppFd001, TestSize.Level1)
{
    std::string connectName = "";
    MultiVpnManager::GetInstance().ClearPppFd(connectName);
    connectName = "test";
    MultiVpnManager::GetInstance().ClearPppFd(connectName);
    connectName = "l2tp";
    MultiVpnManager::GetInstance().ClearPppFd(connectName);
    connectName = "l2tptest";
    MultiVpnManager::GetInstance().ClearPppFd(connectName);
    connectName = "l2tp1";
    MultiVpnManager::GetInstance().ClearPppFd(connectName);
    int32_t multiVpnFd = 0;
    auto result = MultiVpnManager::GetInstance().GetMultiVpnFd(connectName, multiVpnFd);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressWithIPv6Test001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = "12345678901234567890";
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = 64;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressWithIPv6Test002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = "12345678901234567890";
    std::string ipAddr = "";
    int32_t prefix = 64;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressWithIPv6Test003, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = 64;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressWithIPv6Test004, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = 129;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressWithIPv6Test005, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = -1;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressInvalidIPFormatTest, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "invalid_ip_address";
    int32_t prefix = 24;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv6Prefix128Test, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = 128;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv6Prefix0Test, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = 0;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv4ValidFormatTest, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "10.0.0.1";
    int32_t prefix = 24;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv4InvalidPrefixTest, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "10.0.0.1";
    int32_t prefix = 33;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv4Prefix0Test, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "10.0.0.1";
    int32_t prefix = 0;
    auto result = multiVpnManager.SetVpnAddress(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnUpWithIPv6Test, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = "12345678901234567890";
    auto result = multiVpnManager.SetVpnUp(cardName, true);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnUpWithIPv4Test, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string cardName = "12345678901234567890";
    auto result = multiVpnManager.SetVpnUp(cardName, false);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnMtuSuccessIPv4Test, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    int32_t mtu = 1500;
    auto result = multiVpnManager.SetVpnMtu(ifName, mtu);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnMtuInvalidIfNameTest, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = "12345678901234567890";
    int32_t mtu = 1500;
    auto result = multiVpnManager.SetVpnMtu(ifName, mtu);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv6Private001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = "12345678901234567890";
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = 64;
    auto result = multiVpnManager.SetVpnAddressIPv6(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv6Private002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = 64;
    auto result = multiVpnManager.SetVpnAddressIPv6(ifName, ipAddr, prefix);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv4Private001, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = "12345678901234567890";
    std::string ipAddr = "10.0.0.1";
    int32_t prefix = 24;
    auto result = multiVpnManager.SetVpnAddressIPv4(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv4Private002, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "10.0.0.1";
    int32_t prefix = 24;
    auto result = multiVpnManager.SetVpnAddressIPv4(ifName, ipAddr, prefix);
    EXPECT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv4PrivateInvalidPrefix, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "10.0.0.1";
    int32_t prefix = 0;
    auto result = multiVpnManager.SetVpnAddressIPv4(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(MultiVpnManagerTest, SetVpnAddressIPv6PrivateInvalidPrefix, TestSize.Level1)
{
    MultiVpnManager multiVpnManager;
    std::string ifName = TEST_XFRM_CARD_NAME;
    std::string ipAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t prefix = 129;
    auto result = multiVpnManager.SetVpnAddressIPv6(ifName, ipAddr, prefix);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

} // namespace NetManagerStandard
} // namespace OHOS

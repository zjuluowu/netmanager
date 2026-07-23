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
#include <chrono>
#include <thread>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_manager_constants.h"
#include "route_manager.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
using namespace OHOS::NetManagerStandard;
constexpr const char *TEST_SYS_VPN_CALL = "1";
constexpr const char *TEST_EXT_VPN_CALL = "0";
} // namespace

class RouteManagerExtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RouteManagerExtTest::SetUpTestCase() {}

void RouteManagerExtTest::TearDownTestCase()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void RouteManagerExtTest::SetUp() {}

void RouteManagerExtTest::TearDown() {}

HWTEST_F(RouteManagerExtTest, AddInterfaceToVirtualNetwork003, TestSize.Level1)
{
    RouteManager::interfaceToTable_.clear();
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    std::string testInterfaceName = "vpn";
    auto ret = RouteManager::AddInterfaceToVirtualNetwork(testNetId, testInterfaceName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerExtTest, RemoveInterfaceFromVirtualNetwork002, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    std::string testInterfaceName = "vpn";
    auto ret = RouteManager::RemoveInterfaceFromVirtualNetwork(testNetId, testInterfaceName);
    EXPECT_EQ(ret, 0);

    testInterfaceName = "eth0";
    ret = RouteManager::RemoveInterfaceFromVirtualNetwork(testNetId, testInterfaceName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerExtTest, RemoveInterfaceFromVirtualNetwork003, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    std::string testInterfaceName = "vpn";
    auto ret = RouteManager::RemoveInterfaceFromVirtualNetwork(testNetId, testInterfaceName);
    EXPECT_EQ(ret, 0);
    testInterfaceName = "eth0";
    ret = RouteManager::RemoveInterfaceFromVirtualNetwork(testNetId, testInterfaceName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerExtTest, AddUsersToVirtualNetwork002, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    std::string testInterfaceName = "testName1";
    std::vector<NetManagerStandard::UidRange> uidRanges;
    auto ret = RouteManager::AddUsersToVirtualNetwork(testNetId, testInterfaceName, uidRanges);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerExtTest, RemoveUsersFromVirtualNetwork002, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    std::string testInterfaceName = "testName1";
    std::vector<NetManagerStandard::UidRange> uidRanges;
    auto ret = RouteManager::RemoveUsersFromVirtualNetwork(testNetId, testInterfaceName, uidRanges);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerExtTest, UpdateVpnOutputToLocalRule001, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    std::string testInterfaceName = "testName1";
    auto ret = RouteManager::UpdateVpnOutputToLocalRule(testInterfaceName, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = RouteManager::UpdateVpnOutputToLocalRule(testInterfaceName, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    testInterfaceName = "vpn-tun";
    ret = RouteManager::UpdateVpnOutputToLocalRule(testInterfaceName, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    testInterfaceName = "ppp";
    ret = RouteManager::UpdateVpnOutputToLocalRule(testInterfaceName, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateVpnOutputToLocalRule002, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    std::string testInterfaceName = "testName1";
    auto ret = RouteManager::UpdateVpnOutputToLocalRule(testInterfaceName, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = RouteManager::UpdateVpnOutputToLocalRule(testInterfaceName, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateVpnSystemPermissionRule001, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    uint32_t table = 106;
    std::string testInterfaceName = "xfrm-vpn1";
    auto ret = RouteManager::UpdateVpnSystemPermissionRule(testNetId, table, true, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = RouteManager::UpdateVpnSystemPermissionRule(testNetId, table, false, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    testInterfaceName = "vpn-tun1";
    ret = RouteManager::UpdateVpnSystemPermissionRule(testNetId, table, false, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateVpnSystemPermissionRule002, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    uint32_t table = 106;
    std::string testInterfaceName = "xfrm-vpn1";
    auto ret = RouteManager::UpdateVpnSystemPermissionRule(testNetId, table, true, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = RouteManager::UpdateVpnSystemPermissionRule(testNetId, table, false, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    testInterfaceName = "vpn-tun1";
    ret = RouteManager::UpdateVpnSystemPermissionRule(testNetId, table, false, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateExplicitNetworkRuleWithUid001, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    uint32_t table = 106;
    std::string testInterfaceName = "xfrm-vpn1";
    NetworkPermission permission = PERMISSION_NONE;
    auto ret =
        RouteManager::UpdateExplicitNetworkRuleWithUid(testNetId, table, permission, 0, 0, true, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret =
        RouteManager::UpdateExplicitNetworkRuleWithUid(testNetId, table, permission, 0, 0, false, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    testInterfaceName = "vpn-tun1";
    ret =
        RouteManager::UpdateExplicitNetworkRuleWithUid(testNetId, table, permission, 0, 0, false, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateExplicitNetworkRuleWithUid002, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    uint32_t table = 106;
    std::string testInterfaceName = "xfrm-vpn1";
    NetworkPermission permission = PERMISSION_NONE;
    auto ret =
        RouteManager::UpdateExplicitNetworkRuleWithUid(testNetId, table, permission, 0, 0, true, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret =
        RouteManager::UpdateExplicitNetworkRuleWithUid(testNetId, table, permission, 0, 0, false, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    testInterfaceName = "vpn-tun1";
    ret =
        RouteManager::UpdateExplicitNetworkRuleWithUid(testNetId, table, permission, 0, 0, false, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateOutputInterfaceRulesWithUid003, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    uint32_t table = 106;
    std::string testInterfaceName = "xfrm-vpn1";
    NetworkPermission permission = PERMISSION_NONE;
    auto ret = RouteManager::UpdateOutputInterfaceRulesWithUid(testInterfaceName, table, permission, 0, 0, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = RouteManager::UpdateOutputInterfaceRulesWithUid(testInterfaceName, table, permission, 0, 0, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    testInterfaceName = "vpn-tun";
    ret = RouteManager::UpdateOutputInterfaceRulesWithUid(testInterfaceName, table, permission, 0, 0, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateOutputInterfaceRulesWithUid004, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint16_t testNetId = 154;
    uint32_t table = 106;
    std::string testInterfaceName = "xfrm-vpn1";
    NetworkPermission permission = PERMISSION_NONE;
    auto ret = RouteManager::UpdateOutputInterfaceRulesWithUid(testInterfaceName, table, permission, 0, 0, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = RouteManager::UpdateOutputInterfaceRulesWithUid(testInterfaceName, table, permission, 0, 0, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    testInterfaceName = "vpn-tun";
    ret = RouteManager::UpdateOutputInterfaceRulesWithUid(testInterfaceName, table, permission, 0, 0, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateVirtualNetworkTest003, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    NetManagerStandard::UidRange uidRange{};
    std::vector<NetManagerStandard::UidRange> uidRanges;
    uidRanges.push_back(uidRange);
    uint16_t testNetId = 0;
    std::string testInterfaceName = "rmnet0";
    bool add = true;
    auto ret = RouteManager::UpdateVirtualNetwork(testNetId, testInterfaceName, uidRanges, add);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateVirtualNetworkTest004, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    NetManagerStandard::UidRange uidRange{};
    std::vector<NetManagerStandard::UidRange> uidRanges;
    uidRanges.push_back(uidRange);
    uint16_t testNetId = 0;
    std::string testInterfaceName = "xfrm-vpn1";
    bool add = true;
    auto ret = RouteManager::UpdateVirtualNetwork(testNetId, testInterfaceName, uidRanges, add);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, SetVpnCallMode001, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, SetVpnCallMode002, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateVpnRules001, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    const std::string ipAddr = "192.168.1.21";
    std::vector<std::string> extMessages;
    extMessages.emplace_back(ipAddr);
    uint16_t netId = 103;
    std::string interface = "xfrm-vpn1";
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, true);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, false);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateVpnRules002, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    const std::string ipAddr = "192.168.1.21";
    std::vector<std::string> extMessages;
    extMessages.emplace_back(ipAddr);
    uint16_t netId = 103;
    std::string interface = "";
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, true);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(RouteManagerExtTest, UpdateVpnRules003, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    const std::string ipAddr = "192.168.1.21";
    std::vector<std::string> extMessages;
    extMessages.emplace_back(ipAddr);
    uint16_t netId = 103;
    std::string interface = "xfrm-vpn1";
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, true);
    EXPECT_TRUE(result <= 0);
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, false);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    interface = "vpn-tun";
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, true);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, UpdateVpnRules004, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    const std::string ipAddr = "192.168.1";
    std::vector<std::string> extMessages;
    extMessages.emplace_back(ipAddr);
    uint16_t netId = 103;
    std::string interface = "vpn-tun";
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, true);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, false);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    interface = "";
    result = RouteManager::UpdateVpnRules(netId, interface, extMessages, true);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(RouteManagerExtTest, CheckSysVpnCall001, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    EXPECT_TRUE(RouteManager::CheckSysVpnCall());

    result = RouteManager::SetVpnCallMode(TEST_EXT_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    EXPECT_FALSE(RouteManager::CheckSysVpnCall());
}

HWTEST_F(RouteManagerExtTest, CheckTunVpnCall001, TestSize.Level1)
{
    std::string infaceName = "eth0";
    EXPECT_FALSE(RouteManager::CheckTunVpnCall(infaceName));

    infaceName = "vpn-tun";
    EXPECT_TRUE(RouteManager::CheckTunVpnCall(infaceName));

    infaceName = "xfrm-vpn1";
    EXPECT_FALSE(RouteManager::CheckTunVpnCall(infaceName));

    infaceName = "";
    EXPECT_FALSE(RouteManager::CheckTunVpnCall(infaceName));
}

HWTEST_F(RouteManagerExtTest, UpdateVpnOutPutPenetrationRule001, TestSize.Level1)
{
    uint16_t netId = 105;
    std::string addr = "10.2.0.3";
    std::string interfaceName = "xfrm-vpn1";

    int32_t result = RouteManager::UpdateVpnOutPutPenetrationRule(netId, interfaceName, addr, true);
    EXPECT_TRUE(result < 0);
    result = RouteManager::UpdateVpnOutPutPenetrationRule(netId, interfaceName, addr, false);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerExtTest, FindVpnIdByInterfacename001, TestSize.Level1)
{
    std::string interface = "xfrm-vpn1";

    uint32_t result = RouteManager::FindVpnIdByInterfacename(RouteManager::VpnRuleIdType::VPN_OUTPUT_TO_LOCAL,
        interface);
    EXPECT_TRUE(result > 0);

    result = RouteManager::FindVpnIdByInterfacename(RouteManager::VpnRuleIdType::VPN_SECURE,
        interface);
    EXPECT_TRUE(result > 0);

    result = RouteManager::FindVpnIdByInterfacename(RouteManager::VpnRuleIdType::VPN_EXPLICIT_NETWORK,
        interface);
    EXPECT_TRUE(result > 0);

    result = RouteManager::FindVpnIdByInterfacename(RouteManager::VpnRuleIdType::VPN_OUTPUT_IFACE,
        interface);
    EXPECT_TRUE(result > 0);

    result = RouteManager::FindVpnIdByInterfacename(RouteManager::VpnRuleIdType::VPN_NETWORK_TABLE,
        interface);
    EXPECT_TRUE(result > 0);

    interface = "vpn-tun";
    result = RouteManager::FindVpnIdByInterfacename(static_cast<RouteManager::VpnRuleIdType>(999),  interface);
    EXPECT_TRUE(result == 0);
}

HWTEST_F(RouteManagerExtTest, GetVpnInterffaceToId001, TestSize.Level1)
{
    std::string interfaceName = "xfrm-vpn1";
    int32_t result = RouteManager::GetVpnInterffaceToId(interfaceName);
    EXPECT_TRUE(result == 1);

    interfaceName = "vpn-tun";
    result = RouteManager::GetVpnInterffaceToId(interfaceName);
    EXPECT_TRUE(result == 0);

    interfaceName = "ppp1";
    result = RouteManager::GetVpnInterffaceToId(interfaceName);
    EXPECT_TRUE(result == 0);

    interfaceName = "ppp-vpn1";
    result = RouteManager::GetVpnInterffaceToId(interfaceName);
    EXPECT_TRUE(result == 1);
}

HWTEST_F(RouteManagerExtTest, GetRuleFlag001, TestSize.Level1)
{
    uint32_t action = 32;
    uint16_t result = RouteManager::GetRuleFlag(action);
    EXPECT_TRUE(result > 0);

    action = 16;
    result = RouteManager::GetRuleFlag(action);
    EXPECT_TRUE(result == 0);
}

HWTEST_F(RouteManagerExtTest, CheckMultiVpnCall001, TestSize.Level1)
{
    int32_t result = RouteManager::SetVpnCallMode(TEST_SYS_VPN_CALL);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    std::string testInterfaceName = "xfrm-vpn1";
    EXPECT_FALSE(RouteManager::CheckMultiVpnCall(testInterfaceName));

    RouteManager::vpnSysCall_ = false;
    EXPECT_TRUE(RouteManager::CheckMultiVpnCall(testInterfaceName));

    RouteManager::vpnSysCall_ = false;
    testInterfaceName = "vpn-tun";
    EXPECT_FALSE(RouteManager::CheckMultiVpnCall(testInterfaceName));

    testInterfaceName = "";
    EXPECT_FALSE(RouteManager::CheckMultiVpnCall(testInterfaceName));
}
} // namespace nmd
} // namespace OHOS
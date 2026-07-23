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

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "netlink_msg.h"
#include "net_manager_constants.h"
#include "route_manager.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
using namespace OHOS::NetManagerStandard;
} // namespace

class RouteManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RouteManagerTest::SetUpTestCase() {}

void RouteManagerTest::TearDownTestCase() {}

void RouteManagerTest::SetUp() {}

void RouteManagerTest::TearDown() {}

HWTEST_F(RouteManagerTest, AddRouteTest001, TestSize.Level1)
{
    uint32_t testRouteType = 6;
    bool flag = false;
    NetworkRouteInfo networkRouteInfo;
    auto ret = RouteManager::AddRoute(static_cast<RouteManager::TableType>(testRouteType), networkRouteInfo, flag);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, AddRouteTest002, TestSize.Level1)
{
    bool flag = false;
    NetworkRouteInfo networkRouteInfo;
    auto ret = RouteManager::AddRoute(RouteManager::TableType::INTERFACE, networkRouteInfo, flag);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddRouteTest003, TestSize.Level1)
{
    bool flag = false;
    NetworkRouteInfo networkRouteInfo;
    auto ret = RouteManager::AddRoute(RouteManager::TableType::LOCAL_NETWORK, networkRouteInfo, flag);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddRouteTest004, TestSize.Level1)
{
    bool flag = false;
    NetworkRouteInfo networkRouteInfo;
    auto ret = RouteManager::AddRoute(RouteManager::TableType::VPN_NETWORK, networkRouteInfo, flag);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveRouteTest001, TestSize.Level1)
{
    uint32_t testRouteType = 6;
    auto ret = RouteManager::RemoveRoute(static_cast<RouteManager::TableType>(testRouteType), {}, {}, {}, false);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, RemoveRouteTest002, TestSize.Level1)
{
    auto ret = RouteManager::RemoveRoute(RouteManager::TableType::INTERFACE, {}, {}, {}, false);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveRouteTest003, TestSize.Level1)
{
    auto ret = RouteManager::RemoveRoute(RouteManager::TableType::LOCAL_NETWORK, {}, {}, {}, false);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveRouteTest004, TestSize.Level1)
{
    auto ret = RouteManager::RemoveRoute(RouteManager::TableType::VPN_NETWORK, {}, {}, {}, false);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, UpdateRouteTest001, TestSize.Level1)
{
    uint32_t testRouteType = 6;
    auto ret = RouteManager::UpdateRoute(static_cast<RouteManager::TableType>(testRouteType), {}, {}, {});
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, UpdateRouteTest002, TestSize.Level1)
{
    auto ret = RouteManager::UpdateRoute(RouteManager::TableType::INTERFACE, {}, {}, {});
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, UpdateRouteTest003, TestSize.Level1)
{
    auto ret = RouteManager::UpdateRoute(RouteManager::TableType::LOCAL_NETWORK, {}, {}, {});
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, UpdateRouteTest004, TestSize.Level1)
{
    auto ret = RouteManager::UpdateRoute(RouteManager::TableType::VPN_NETWORK, {}, {}, {});
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToDefaultNetworkTest001, TestSize.Level1)
{
    auto ret = RouteManager::AddInterfaceToDefaultNetwork({}, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToDefaultNetworkTest002, TestSize.Level1)
{
    const std::string testInterfaceName = "testInterface";
    auto ret = RouteManager::AddInterfaceToDefaultNetwork(testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToDefaultNetworkTest003, TestSize.Level1)
{
    const std::string testInterfaceName = "eth0";
    auto ret = RouteManager::AddInterfaceToDefaultNetwork(testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToDefaultNetworkTest004, TestSize.Level1)
{
    const std::string testInterfaceName = "wlan0";
    auto ret = RouteManager::AddInterfaceToDefaultNetwork(testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromDefaultNetworkTest001, TestSize.Level1)
{
    auto ret = RouteManager::RemoveInterfaceFromDefaultNetwork({}, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromDefaultNetworkTest002, TestSize.Level1)
{
    const std::string testInterfaceName = "testInterface";
    auto ret = RouteManager::RemoveInterfaceFromDefaultNetwork(testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromDefaultNetworkTest003, TestSize.Level1)
{
    const std::string testInterfaceName = "eth0";
    auto ret = RouteManager::RemoveInterfaceFromDefaultNetwork(testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromDefaultNetworkTest004, TestSize.Level1)
{
    const std::string testInterfaceName = "wlan0";
    auto ret = RouteManager::RemoveInterfaceFromDefaultNetwork(testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, AddInterfaceToPhysicalNetworkTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth0";
    auto ret = RouteManager::AddInterfaceToPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToPhysicalNetworkTest002, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret = RouteManager::AddInterfaceToPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NETWORK);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToPhysicalNetworkTest003, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "wlan0";
    auto ret = RouteManager::AddInterfaceToPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_SYSTEM);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToPhysicalNetworkTest004, TestSize.Level1)
{
    uint16_t testNetId = 154;
    auto ret = RouteManager::AddInterfaceToPhysicalNetwork(testNetId, {}, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToPhysicalNetworkTest005, TestSize.Level1)
{
    uint16_t testNetId = 1;
    const std::string testInterfaceName = "rmnet0";
    auto ret = RouteManager::AddInterfaceToPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromPhysicalNetworkTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth0";
    auto ret = RouteManager::RemoveInterfaceFromPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromPhysicalNetworkTest002, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret = RouteManager::RemoveInterfaceFromPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NETWORK);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromPhysicalNetworkTest003, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "wlan0";
    auto ret = RouteManager::RemoveInterfaceFromPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_SYSTEM);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromPhysicalNetworkTest004, TestSize.Level1)
{
    uint16_t testNetId = 154;
    auto ret = RouteManager::RemoveInterfaceFromPhysicalNetwork(testNetId, {}, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromPhysicalNetworkTest005, TestSize.Level1)
{
    uint16_t testNetId = 1;
    const std::string testInterfaceName = "rmnet0";
    auto ret = RouteManager::RemoveInterfaceFromPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NONE);
    EXPECT_LE(ret, 0);

    ret = RouteManager::RemoveInterfaceFromPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NONE);
    EXPECT_GE(ret, -1);
}

HWTEST_F(RouteManagerTest, ModifyPhysicalNetworkPermissionTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret =
        RouteManager::ModifyPhysicalNetworkPermission(testNetId, testInterfaceName, PERMISSION_NONE, PERMISSION_NONE);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, ModifyPhysicalNetworkPermissionTest002, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret = RouteManager::ModifyPhysicalNetworkPermission(testNetId, testInterfaceName, PERMISSION_NONE,
                                                             PERMISSION_NETWORK);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, ModifyPhysicalNetworkPermissionTest003, TestSize.Level1)
{
    uint16_t testNetId = 154;
    auto ret = RouteManager::ModifyPhysicalNetworkPermission(testNetId, {}, PERMISSION_NETWORK, PERMISSION_NONE);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, ModifyPhysicalNetworkPermissionTest004, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret =
        RouteManager::ModifyPhysicalNetworkPermission(testNetId, testInterfaceName, PERMISSION_SYSTEM, PERMISSION_NONE);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, ModifyPhysicalNetworkPermissionTest005, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret =
        RouteManager::ModifyPhysicalNetworkPermission(testNetId, testInterfaceName, PERMISSION_NONE, PERMISSION_SYSTEM);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, ModifyPhysicalNetworkPermissionTest006, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret = RouteManager::ModifyPhysicalNetworkPermission(testNetId, testInterfaceName, PERMISSION_SYSTEM,
                                                             PERMISSION_SYSTEM);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, AddInterfaceToLocalNetworkTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret = RouteManager::AddInterfaceToLocalNetwork(testNetId, testInterfaceName);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToLocalNetworkTest002, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth0";
    auto ret = RouteManager::AddInterfaceToLocalNetwork(testNetId, testInterfaceName);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToLocalNetworkTest003, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "testInterfaceName";
    auto ret = RouteManager::AddInterfaceToLocalNetwork(testNetId, testInterfaceName);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToLocalNetworkTest004, TestSize.Level1)
{
    uint16_t testNetId = 154;
    auto ret = RouteManager::AddInterfaceToLocalNetwork(testNetId, {});
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromLocalNetworkTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret = RouteManager::RemoveInterfaceFromLocalNetwork(testNetId, testInterfaceName);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromLocalNetworkTest002, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth0";
    auto ret = RouteManager::RemoveInterfaceFromLocalNetwork(testNetId, testInterfaceName);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromLocalNetworkTest003, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "testInterfaceName";
    auto ret = RouteManager::RemoveInterfaceFromLocalNetwork(testNetId, testInterfaceName);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromLocalNetworkTest004, TestSize.Level1)
{
    uint16_t testNetId = 154;
    auto ret = RouteManager::RemoveInterfaceFromLocalNetwork(testNetId, {});
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, EnableSharingTest001, TestSize.Level1)
{
    const std::string input;
    const std::string output;
    auto ret = RouteManager::EnableSharing(input, output);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, EnableSharingTest002, TestSize.Level1)
{
    const std::string input = "eth0";
    const std::string output;
    auto ret = RouteManager::EnableSharing(input, output);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, EnableSharingTest003, TestSize.Level1)
{
    const std::string input;
    const std::string output = "sla0";
    auto ret = RouteManager::EnableSharing(input, output);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, EnableSharingTest004, TestSize.Level1)
{
    const std::string input = "test";
    const std::string output = "dds0";
    auto ret = RouteManager::EnableSharing(input, output);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, EnableSharingTest005, TestSize.Level1)
{
    const std::string input = "wlan0";
    const std::string output = "eth3";
    auto ret = RouteManager::EnableSharing(input, output);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, DisableSharingTest001, TestSize.Level1)
{
    const std::string input;
    const std::string output;
    auto ret = RouteManager::DisableSharing(input, output);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, DisableSharingTest002, TestSize.Level1)
{
    const std::string input = "eth0";
    const std::string output;
    auto ret = RouteManager::DisableSharing(input, output);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, DisableSharingTest003, TestSize.Level1)
{
    const std::string input;
    const std::string output = "sla0";
    auto ret = RouteManager::DisableSharing(input, output);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, DisableSharingTest004, TestSize.Level1)
{
    const std::string input = "test";
    const std::string output = "dds0";
    auto ret = RouteManager::DisableSharing(input, output);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, DisableSharingTest005, TestSize.Level1)
{
    const std::string input = "wlan0";
    const std::string output = "eth3";
    auto ret = RouteManager::DisableSharing(input, output);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, ReadAddrTest001, TestSize.Level1)
{
    const std::string addr;
    auto ret = RouteManager::ReadAddr(addr, nullptr);
    EXPECT_EQ(ret, -EINVAL);
}

HWTEST_F(RouteManagerTest, ReadAddrTest002, TestSize.Level1)
{
    const std::string addr = "/";
    InetAddr res;
    auto ret = RouteManager::ReadAddr(addr, &res);
    EXPECT_EQ(ret, -EINVAL);
}

HWTEST_F(RouteManagerTest, ReadAddrTest003, TestSize.Level1)
{
    const std::string addr = "48541/451564";
    InetAddr res;
    auto ret = RouteManager::ReadAddr(addr, &res);
    EXPECT_EQ(ret, -EINVAL);
}

HWTEST_F(RouteManagerTest, ReadAddrTest004, TestSize.Level1)
{
    const std::string addr = "48541adfa/451564dfa";
    InetAddr res;
    auto ret = RouteManager::ReadAddr(addr, &res);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, ReadAddrTest005, TestSize.Level1)
{
    const std::string addr = "gsga:4557/56445:::df?";
    InetAddr res;
    auto ret = RouteManager::ReadAddr(addr, &res);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, ReadAddrGwTest001, TestSize.Level1)
{
    const std::string addr;
    auto ret = RouteManager::ReadAddrGw(addr, nullptr);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, ReadAddrGwTest002, TestSize.Level1)
{
    const std::string addr = "/";
    InetAddr res;
    auto ret = RouteManager::ReadAddrGw(addr, &res);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, ReadAddrGwTest003, TestSize.Level1)
{
    const std::string addr = "48541/451564";
    InetAddr res;
    auto ret = RouteManager::ReadAddrGw(addr, &res);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, ReadAddrGwTest004, TestSize.Level1)
{
    const std::string addr = "48541adfa/451564dfa";
    InetAddr res;
    auto ret = RouteManager::ReadAddrGw(addr, &res);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, ReadAddrGwTest005, TestSize.Level1)
{
    const std::string addr = "gsga:4557/56445:::df?";
    InetAddr res;
    auto ret = RouteManager::ReadAddrGw(addr, &res);
    EXPECT_LE(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToVirtualNetwork001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    std::string testInterfaceName = "testName0";
    auto ret = RouteManager::AddInterfaceToVirtualNetwork(testNetId, testInterfaceName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromVirtualNetwork001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    std::string testInterfaceName = "testName0";
    auto ret = RouteManager::RemoveInterfaceFromVirtualNetwork(testNetId, testInterfaceName);

    testInterfaceName = "notexist";
    ret = RouteManager::RemoveInterfaceFromVirtualNetwork(testNetId, testInterfaceName);
    EXPECT_TRUE(ret == -1 || ret == 0);
}

HWTEST_F(RouteManagerTest, AddUsersToVirtualNetwork001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    std::string testInterfaceName = "testName1";
    std::vector<NetManagerStandard::UidRange> uidRanges;
    auto ret = RouteManager::AddUsersToVirtualNetwork(testNetId, testInterfaceName, uidRanges);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveUsersFromVirtualNetwork001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    std::string testInterfaceName = "testName1";
    std::vector<NetManagerStandard::UidRange> uidRanges;
    auto ret = RouteManager::RemoveUsersFromVirtualNetwork(testNetId, testInterfaceName, uidRanges);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, UpdateVnicRoute001, TestSize.Level1)
{
    std::string testInterfaceName = "testName1";
    auto ret = RouteManager::UpdateVnicRoute(testInterfaceName, {}, {}, true);
    EXPECT_EQ(ret, -1);

    ret = RouteManager::UpdateVnicRoute(testInterfaceName, {}, {}, false);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, UpdateVnicUidRangesRule001, TestSize.Level1)
{
    std::vector<NetManagerStandard::UidRange> uidRanges;
    auto ret = RouteManager::UpdateVnicUidRangesRule(uidRanges, true);
    EXPECT_EQ(ret, 0);

    ret = RouteManager::UpdateVnicUidRangesRule(uidRanges, false);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromPhysicalNetworkTest007, TestSize.Level1)
{
    uint16_t testNetId = 0;
    std::string testInterfaceName = "rmnet0";
    auto ret = RouteManager::RemoveInterfaceFromPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NONE);
    EXPECT_NE(ret, NETMANAGER_ERR_INTERNAL);
    testNetId = 1;
    ret = RouteManager::RemoveInterfaceFromPhysicalNetwork(testNetId, testInterfaceName, PERMISSION_NONE);
    EXPECT_NE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, ModifyPhysicalNetworkPermissionTest007, TestSize.Level1)
{
    uint16_t testNetId = 154;
    const std::string testInterfaceName = "eth1";
    auto ret =
        RouteManager::ModifyPhysicalNetworkPermission(testNetId, testInterfaceName, PERMISSION_NONE, PERMISSION_NONE);
    EXPECT_GE(ret, -1);
}

HWTEST_F(RouteManagerTest, UpdateVirtualNetworkTest002, TestSize.Level1)
{
    NetManagerStandard::UidRange uidRange{};
    std::vector<NetManagerStandard::UidRange> uidRanges;
    uidRanges.push_back(uidRange);
    uint16_t testNetId = 0;
    std::string testInterfaceName = "rmnet0";
    bool add = true;
    auto ret = RouteManager::UpdateVirtualNetwork(testNetId, testInterfaceName, uidRanges, add);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, RemoveInterfaceFromPhysicalNetworkTest006, TestSize.Level1)
{
    uint32_t table = 1;
    uid_t uidStart = 1;
    bool add = true;
    int32_t ret = RouteManager::UpdateVpnUidRangeRule(table, uidStart, uidStart, true);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, UpdateOutputInterfaceRulesWithUidTest001, TestSize.Level1)
{
    const std::string interface = "interface";
    uint32_t table = 1;
    NetworkPermission permission = PERMISSION_NETWORK;
    uid_t uidStart = 1;
    bool add = true;
    int32_t ret =
        RouteManager::UpdateOutputInterfaceRulesWithUid(interface, table, permission, uidStart, uidStart, add);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, AddInterfaceToLocalNetworkTest005, TestSize.Level1)
{
    uint16_t testNetId = 1;
    std::string testInterfaceName = "eth0";
    auto ret = RouteManager::AddInterfaceToLocalNetwork(testNetId, testInterfaceName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, ClearRoutesTest001, TestSize.Level1)
{
    int32_t netId = 0;
    const std::string interfaceName = "eth0";
    std::map<std::string, uint32_t> interfaceToTable;
    interfaceToTable[interfaceName] = RT_TABLE_UNSPEC;
    RouteManager::interfaceToTable_ = interfaceToTable;
    auto ret = RouteManager::ClearRoutes(interfaceName, netId);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, UpdatePhysicalNetworkTest001, TestSize.Level1)
{
    int32_t netId = 0;
    const std::string interfaceName = "eth0";
    int32_t ret = RouteManager::UpdatePhysicalNetwork(netId, interfaceName, PERMISSION_NONE, true);
    EXPECT_LE(ret, 0);
    ret = RouteManager::UpdatePhysicalNetwork(netId, interfaceName, PERMISSION_NONE, false);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, UpdateIncomingPacketMarkTest001, TestSize.Level1)
{
    int32_t netId = 0;
    const std::string interfaceName = "eth0";

    int32_t ret = RouteManager::UpdateIncomingPacketMark(netId, interfaceName, PERMISSION_NONE, true);
    EXPECT_EQ(ret, 0);
    ret = RouteManager::UpdateIncomingPacketMark(netId, interfaceName, PERMISSION_NONE, false);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, UpdateSharingNetworkTest001, TestSize.Level1)
{
    uint16_t netId = 1;
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "eth0";
    int32_t ret = RouteManager::UpdateSharingNetwork(netId, inputInterface, outputInterface);
    EXPECT_EQ(ret, -1);

    std::map<std::string, uint32_t> interfaceToTable;
    interfaceToTable[inputInterface] = 1;
    RouteManager::interfaceToTable_ = interfaceToTable;
    ret = RouteManager::UpdateSharingNetwork(netId, inputInterface, outputInterface);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(RouteManagerTest, UpdateRuleInfoTest001, TestSize.Level1)
{
    uint32_t action = 1;
    uint8_t ruleType = 1;
    RuleInfo ruleInfo{0, 1, 1, 0, "ruleIif", "ruleOif", "ruleSrcIp", "ruleDstIp"};
    uid_t uidStart = 0;
    uid_t uidEnd = 1;

    int32_t ret = RouteManager::UpdateRuleInfo(action, ruleType, ruleInfo, uidStart, uidEnd);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    ruleInfo.ruleMask = 1;
    ret = RouteManager::UpdateRuleInfo(action, ruleType, ruleInfo, uidStart, uidEnd);
    EXPECT_EQ(ret, -ENOTUNIQ); // -76
    uint8_t family = 1;
    ret = RouteManager::SendRuleToKernelEx(action, family, ruleType, ruleInfo, uidStart, uidEnd);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(RouteManagerTest, UpdateDistributedRuleTest001, TestSize.Level1)
{
    uint32_t action = 1;
    uint8_t ruleType = 1;
    RuleInfo ruleInfo{0, 1, 1, 0, "ruleIif", "ruleOif", "ruleSrcIp", "ruleDstIp:"};
    uid_t uidStart = 0;
    uid_t uidEnd = 1;

    int32_t ret = RouteManager::UpdateDistributedRule(action, ruleType, ruleInfo, uidStart, uidEnd);
    EXPECT_EQ(ret, -ENOTUNIQ); // -76
    ruleInfo.ruleTable = 1;
    ret = RouteManager::UpdateDistributedRule(action, ruleType, ruleInfo, uidStart, uidEnd);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerTest, SendRouteToKernelTest005, TestSize.Level1)
{
    uint16_t action = 1;
    uint16_t routeFlag = 1;
    rtmsg msg{};
    RouteInfo routeInfo{1, "", "http/a:6:0:df", ""};
    uint32_t index = 0;

    int32_t ret = RouteManager::SendRouteToKernel(action, routeFlag, msg, routeInfo, index);
    EXPECT_EQ(ret, -1);
    routeInfo.routeNextHop = "http/a:6:0:df";
    ret = RouteManager::SendRouteToKernel(action, routeFlag, msg, routeInfo, index);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, GetRouteTableFromTypeTest005, TestSize.Level1)
{
    RouteManager::TableType tableType = RouteManager::INTERNAL_DEFAULT;
    std::string interfaceName = "eth0";
    uint32_t ret = RouteManager::GetRouteTableFromType(tableType, interfaceName);
    EXPECT_NE(ret, 1);
}

HWTEST_F(RouteManagerTest, GetRouteTableFromTypeTest006, TestSize.Level1)
{
    RouteManager::TableType tableType = RouteManager::UNREACHABLE_NETWORK;
    std::string interfaceName = "eth0";
    uint32_t ret = RouteManager::GetRouteTableFromType(tableType, interfaceName);
    EXPECT_EQ(ret, 80);
}

HWTEST_F(RouteManagerTest, AddInterfaceToVirtualNetwork002, TestSize.Level1)
{
    uint16_t testNetId = 154;
    std::string testInterfaceName = "vpn";
    auto ret = RouteManager::AddInterfaceToVirtualNetwork(testNetId, testInterfaceName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, EnableDistributedClientNet001, TestSize.Level1)
{
    std::string virNicAddr;
    std::string virNicName;
    std::string iif;
    auto ret = RouteManager::EnableDistributedClientNet(virNicAddr, virNicName, iif);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, UpdateIncomingPacketMark001, TestSize.Level1)
{
    uint16_t netId = 123;
    std::string interfaceName = "123";
    NetworkPermission permission = PERMISSION_NONE;
    bool add = true;
    auto ret = RouteManager::UpdateIncomingPacketMark(netId, interfaceName, permission, add);
    EXPECT_EQ(ret, 0);
    add = false;
    ret = RouteManager::UpdateIncomingPacketMark(netId, interfaceName, permission, add);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(RouteManagerTest, GetRouteTableFromType001, TestSize.Level1)
{
    RouteManager::TableType tableType = RouteManager::INTERNAL_DEFAULT;
    std::string interfaceName = "123";
    auto ret = RouteManager::GetRouteTableFromType(tableType, interfaceName);
    EXPECT_EQ(ret, 1);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
HWTEST_F(RouteManagerTest, UpdateEnterpriseRoute001, TestSize.Level1)
{
    std::string ifname = "wlan0";
    uint32_t uid = 20000138;
    bool add = true;
    int32_t ret = RouteManager::UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
 
HWTEST_F(RouteManagerTest, UpdateEnterpriseRoute002, TestSize.Level1)
{
    std::string ifname = "wlan0";
    uint32_t uid = 0;
    bool add = true;
    int32_t ret = RouteManager::UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
 
HWTEST_F(RouteManagerTest, UpdateEnterpriseRoute003, TestSize.Level1)
{
    std::string ifname = "notexist";
    uint32_t uid = 0;
    bool add = true;
    int32_t ret = RouteManager::UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}
#endif

HWTEST_F(RouteManagerTest, SetSharingUnreachableIpRule001, TestSize.Level1)
{
    std::string interfaceName = "123";
    std::string ip = "1.1.1.1";
    uint8_t family = 2;
    auto ret = RouteManager::SetSharingUnreachableIpRule(RTM_NEWRULE, interfaceName, ip, family);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(RouteManagerTest, SetRuleMsgTable001, TestSize.Level1)
{
    NetlinkMsg nlmsg(NLM_F_CREATE, NETLINK_MAX_LEN, 1);
    RuleInfo ruleInfo;
    ruleInfo.ruleTable = RT_TABLE_UNSPEC;
    auto ret = RouteManager::SetRuleMsgTable(nlmsg, ruleInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ruleInfo.ruleTable = 80;
    nlmsg.netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    ret = RouteManager::SetRuleMsgTable(nlmsg, ruleInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    NetlinkMsg nlmsg1(NLM_F_CREATE, 1, 1);
    ret = RouteManager::SetRuleMsgTable(nlmsg1, ruleInfo);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, SetRuleMsgFwmark001, TestSize.Level1)
{
    NetlinkMsg nlmsg(NLM_F_CREATE, NETLINK_MAX_LEN, 1);
    RuleInfo ruleInfo;
    ruleInfo.ruleMask = 0;
    auto ret = RouteManager::SetRuleMsgFwmark(nlmsg, ruleInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ruleInfo.ruleMask = 1;
    nlmsg.netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    ret = RouteManager::SetRuleMsgFwmark(nlmsg, ruleInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    NetlinkMsg nlmsg1(NLM_F_CREATE, 1, 1);
    ret = RouteManager::SetRuleMsgFwmark(nlmsg1, ruleInfo);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, SetRuleMsgUidRange001, TestSize.Level1)
{
    NetlinkMsg nlmsg(NLM_F_CREATE, NETLINK_MAX_LEN, 1);
    uid_t uidStart = -1;
    uid_t uidEnd = -1;
    auto ret = RouteManager::SetRuleMsgUidRange(nlmsg, uidStart, uidEnd);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    uidStart = 1000;
    uidEnd = 1000;
    nlmsg.netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    ret = RouteManager::SetRuleMsgUidRange(nlmsg, uidStart, uidEnd);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    NetlinkMsg nlmsg1(NLM_F_CREATE, 1, 1);
    ret = RouteManager::SetRuleMsgUidRange(nlmsg1, uidStart, uidEnd);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, SetRuleMsgIfName001, TestSize.Level1)
{
    NetlinkMsg nlmsg(NLM_F_CREATE, NETLINK_MAX_LEN, 1);
    uint16_t type = 1;
    std::string ifNameNull = "";
    auto ret = RouteManager::SetRuleMsgIfName(nlmsg, ifNameNull, type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string ifName = "test";
    nlmsg.netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    ret = RouteManager::SetRuleMsgIfName(nlmsg, ifName, type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    NetlinkMsg nlmsg1(NLM_F_CREATE, 1, 1);
    ret = RouteManager::SetRuleMsgIfName(nlmsg1, ifName, type);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(RouteManagerTest, SetRuleMsgIp001, TestSize.Level1)
{
    NetlinkMsg nlmsg(NLM_F_CREATE, NETLINK_MAX_LEN, 1);
    uint16_t type = 1;
    std::string ipNull = "";
    auto ret = RouteManager::SetRuleMsgIp(nlmsg, ipNull, type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string ip = "1.1.1.1";
    nlmsg.netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    ret = RouteManager::SetRuleMsgIp(nlmsg, ip, type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    NetlinkMsg nlmsg1(NLM_F_CREATE, 1, 1);
    ret = RouteManager::SetRuleMsgIp(nlmsg1, ip, type);
    EXPECT_EQ(ret, -1);

    std::string ipErr = "error";
    ret = RouteManager::SetRuleMsgIp(nlmsg, ipErr, type);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_TableUnspec_DelRule_NotEmpty
 * @tc.name: when table is RT_TABLE_UNSPEC, action is RTM_DELRULE and outputInterface is not empty
 * @tc.desc: returns result when table is not found but action is RTM_DELRULE and outputInterface is not empty
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_TableUnspec_DelRule_NotEmpty, TestSize.Level1)
{
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "notexist";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_DELRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_TableUnspec_AddRule
 * @tc.name: Test UpdateSharingNetwork when table is RT_TABLE_UNSPEC and action is RTM_NEWRULE
 * @tc.desc: Verify that UpdateSharingNetwork returns -1 when table is not found and action is RTM_NEWRULE
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_TableUnspec_AddRule, TestSize.Level1)
{
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "notexist";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_TableUnspec_DelRule_Empty
 * @tc.name: when table is RT_TABLE_UNSPEC, action is RTM_DELRULE and outputInterface is empty
 * @tc.desc:  returns -1 when table is not found, action is RTM_DELRULE but outputInterface is empty
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_TableUnspec_DelRule_Empty, TestSize.Level1)
{
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_DELRULE, inputInterface, outputInterface);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_OutputInterfaceNotEmpty
 * @tc.name: Test UpdateSharingNetwork when outputInterface is not empty
 * @tc.desc: Verify that UpdateSharingNetwork uses outputInterface when it is not empty
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_OutputInterfaceNotEmpty, TestSize.Level1)
{
    const std::string inputInterface = "wlan0";
    const std::string outputInterface = "eth3";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_OutputInterfaceEmpty
 * @tc.name: Test UpdateSharingNetwork when outputInterface is empty
 * @tc.desc: Verify that UpdateSharingNetwork uses RULEOIF_NULL when outputInterface is empty
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_OutputInterfaceEmpty, TestSize.Level1)
{
    const std::string inputInterface = "wlan0";
    const std::string outputInterface = "";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_Tunv4Prefix_AddRule
 * @tc.name: Test UpdateSharingNetwork when outputInterface has "tunv4-" prefix and action is RTM_NEWRULE
 * @tc.desc: Verify that UpdateSharingNetwork records tunv4 interface info when adding rule with tunv4- prefix
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_Tunv4Prefix_AddRule, TestSize.Level1)
{
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "tunv4-0";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_Tunv4Prefix_MultipleDigits_AddRule
 * @tc.name: Test UpdateSharingNetwork when outputInterface has "tunv4-" prefix with multiple digits
 * @tc.desc: Verify that UpdateSharingNetwork handles tunv4- interface with multiple digit suffix
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_Tunv4Prefix_MultipleDigits_AddRule, TestSize.Level1)
{
    const std::string inputInterface = "wlan0";
    const std::string outputInterface = "tunv4-123";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_Tunv4Prefix_DelRule_Matching
 * @tc.name: Test UpdateSharingNetwork when deleting tunv4 rule with matching interface
 * @tc.desc: Verify that UpdateSharingNetwork uses stored tableId when deleting tunv4 rule
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_Tunv4Prefix_DelRule_Matching, TestSize.Level1)
{
    // First add a tunv4- rule to store the interface info
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "tunv4-99";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);

    // Then delete with the same interface - should use stored tableId
    ret = RouteManager::UpdateSharingNetwork(RTM_DELRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_Tunv4Prefix_DelRule_NotMatching
 * @tc.name: Test UpdateSharingNetwork when deleting tunv4 rule with non-matching interface
 * @tc.desc: Verify that UpdateSharingNetwork returns -1 when deleting with different interface
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_Tunv4Prefix_DelRule_NotMatching, TestSize.Level1)
{
    // First add a tunv4- rule to store the interface info
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "tunv4-1";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);

    // Then try to delete with different interface - should return -1
    const std::string differentOutputInterface = "tunv4-2";
    ret = RouteManager::UpdateSharingNetwork(RTM_DELRULE, inputInterface, differentOutputInterface);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_NonTunv4Interface_AddRule
 * @tc.name: Test UpdateSharingNetwork with non-tunv4 interface when adding rule
 * @tc.desc: Verify that UpdateSharingNetwork works normally for non-tunv4 interfaces
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_NonTunv4Interface_AddRule, TestSize.Level1)
{
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "wlan1";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_NonTunv4Interface_DelRule
 * @tc.name: Test UpdateSharingNetwork with non-tunv4 interface when deleting rule
 * @tc.desc: Verify that UpdateSharingNetwork works normally for non-tunv4 interfaces
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_NonTunv4Interface_DelRule, TestSize.Level1)
{
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "wlan1";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_DELRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_Tunv4Prefix_SimilarPrefix_NotMatch
 * @tc.name: Test UpdateSharingNetwork when outputInterface has similar but different prefix
 * @tc.desc: Verify that UpdateSharingNetwork does not match interfaces with similar prefixes like "tunv4"
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_Tunv4Prefix_SimilarPrefix_NotMatch, TestSize.Level1)
{
    const std::string inputInterface = "eth0";
    // Interface that starts with "tunv4" but not "tunv4-"
    const std::string outputInterface = "tunv40";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.number: RouteManager_UpdateSharingNetwork_Tunv4Prefix_EmptyInput
 * @tc.name: Test UpdateSharingNetwork with empty inputInterface and tunv4- prefix
 * @tc.desc: Verify that UpdateSharingNetwork handles empty inputInterface with tunv4- output
 */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_Tunv4Prefix_EmptyInput, TestSize.Level1)
{
    const std::string inputInterface = "";
    const std::string outputInterface = "tunv4-5";
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);
}

/**
+ * @tc.number: RouteManager_UpdateSharingNetwork_Tunv4Prefix_DelRule_AfterClear
+ * @tc.name: Test UpdateSharingNetwork when deleting tunv4 rule after it was cleared
+ * @tc.desc: Verify that UpdateSharingNetwork returns -1 when stored tunv4 interface is cleared
+ */
HWTEST_F(RouteManagerTest, UpdateSharingNetwork_Tunv4Prefix_DelRule_AfterClear, TestSize.Level1)
{
    const std::string inputInterface = "eth0";
    const std::string outputInterface = "tunv4-77";

    // Add and then delete to clear the stored variable
    auto ret = RouteManager::UpdateSharingNetwork(RTM_NEWRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);

    ret = RouteManager::UpdateSharingNetwork(RTM_DELRULE, inputInterface, outputInterface);
    EXPECT_LE(ret, 0);

    // Try to delete again - should return -1 since stored variable is cleared
    ret = RouteManager::UpdateSharingNetwork(RTM_DELRULE, inputInterface, outputInterface);
    EXPECT_EQ(ret, -1);
}

} // namespace nmd
} // namespace OHOS
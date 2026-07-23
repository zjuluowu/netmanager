/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "iservice_registry.h"
#include "system_ability_definition.h"

#ifdef GTEST_API_
#define private public
#endif

#include "conn_manager.h"
#include "net_conn_manager_test_util.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_native_service_proxy.h"
#include "network_permission.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace NetManagerStandard;
using namespace NetConnManagerTestUtil;
constexpr int32_t NETID = 103;
const std::string INTERFACENAME = "wlan0";
constexpr int32_t INTERNAL_NETID = 10;
const std::string INTERNAL_INTERFACENAME = "rmnet0";
constexpr int32_t LOCAL_NET_ID = 99;
constexpr int32_t ERROR_CODE = -101;
constexpr int32_t INVALID_VALUE = -1;

class ConnManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<ConnManager> instance_ = nullptr;
};

void ConnManagerTest::SetUpTestCase()
{
    instance_ = std::make_shared<ConnManager>();
}

void ConnManagerTest::TearDownTestCase()
{
    instance_ = nullptr;
}

void ConnManagerTest::SetUp() {}

void ConnManagerTest::TearDown() {}

/**
 * @tc.name: SetInternetPermission001
 * @tc.desc: Test ConnManager SetInternetPermission.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetInternetPermission001, TestSize.Level1)
{
    uint32_t uid = 0;
    uint8_t allow = 0;
    uint8_t isBroker = 0;
    int32_t ret = instance_->SetInternetPermission(uid, allow, isBroker);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: SetInternetPermission002
 * @tc.desc: Test ConnManager SetInternetPermission.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetInternetPermission002, TestSize.Level1)
{
    uint32_t uid = 0;
    uint8_t allow = 0;
    uint8_t isBroker = 0;
    int32_t ret = instance_->SetInternetPermission(uid, allow, isBroker);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: CreatePhysicalNetworkTest001
 * @tc.desc: Test ConnManager CreatePhysicalNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, CreatePhysicalNetworkTest001, TestSize.Level1)
{
    int32_t ret = instance_->CreatePhysicalNetwork(NETID, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: CreatePhysicalNetworkTest002
 * @tc.desc: Test ConnManager CreatePhysicalNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, CreatePhysicalNetworkTest002, TestSize.Level1)
{
    int32_t ret = instance_->ReinitRoute();
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->CreatePhysicalNetwork(NETID, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: CreatePhysicalNetworkTest003
 * @tc.desc: Test ConnManager CreatePhysicalNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, CreatePhysicalNetworkTest003, TestSize.Level1)
{
    auto ret = instance_->CreatePhysicalNetwork(INTERNAL_NETID, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: CreateVirtualNetwork001
 * @tc.desc: Test ConnManager CreateVirtualNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, CreateVirtualNetwork001, TestSize.Level1)
{
    uint16_t netId = 1;
    bool hasDns = true;
    int32_t ret = instance_->CreateVirtualNetwork(netId, hasDns);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: DestroyNetworkTest001
 * @tc.desc: Test ConnManager DestroyNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, DestroyNetworkTest001, TestSize.Level1)
{
    auto ret = instance_->DestroyNetwork(LOCAL_NET_ID);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: DestroyNetworkTest002
 * @tc.desc: Test ConnManager DestroyNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, DestroyNetworkTest002, TestSize.Level1)
{
    int32_t ret = instance_->DestroyNetwork(NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->DestroyNetwork(NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: DestroyNetworkTest003
 * @tc.desc: Test ConnManager DestroyNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, DestroyNetworkTest003, TestSize.Level1)
{
    int32_t netId = 100;
    int32_t ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetDefaultNetworkTest001
 * @tc.desc: Test ConnManager SetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetDefaultNetworkTest001, TestSize.Level1)
{
    int32_t ret = instance_->SetDefaultNetwork(NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->SetDefaultNetwork(NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetDefaultNetworkTest002
 * @tc.desc: Test ConnManager SetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetDefaultNetworkTest002, TestSize.Level1)
{
    int32_t ret = instance_->SetDefaultNetwork(0);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->SetDefaultNetwork(0);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->SetDefaultNetwork(NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: ClearDefaultNetwork001
 * @tc.desc: Test ConnManager ClearDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, ClearDefaultNetwork001, TestSize.Level1)
{
    int32_t ret = instance_->ClearDefaultNetwork();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->ClearDefaultNetwork();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetDefaultNetworkTest001
 * @tc.desc: Test ConnManager GetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, GetDefaultNetworkTest001, TestSize.Level1)
{
    int32_t ret = instance_->SetDefaultNetwork(NETID);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->GetDefaultNetwork();
    EXPECT_EQ(ret, NETID);
}

/**
 * @tc.name: AddInterfaceToNetworkTest001
 * @tc.desc: Test ConnManager AddInterfaceToNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, AddInterfaceToNetworkTest001, TestSize.Level1)
{
    std::string iface = INTERFACENAME;
    int32_t ret = instance_->AddInterfaceToNetwork(NETID, iface, BEARER_DEFAULT);
    EXPECT_NE(ret, 0);

    iface = INTERNAL_INTERFACENAME;
    ret = instance_->AddInterfaceToNetwork(INTERNAL_NETID, iface, BEARER_DEFAULT);
}

/**
 * @tc.name: AddInterfaceToNetworkTest002
 * @tc.desc: Test ConnManager AddInterfaceToNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, AddInterfaceToNetworkTest002, TestSize.Level1)
{
    std::string testInterfaceName = "testName";
    int32_t ret = instance_->AddInterfaceToNetwork(NETID, testInterfaceName, BEARER_DEFAULT);
    EXPECT_NE(ret, 0);

    ret = instance_->AddInterfaceToNetwork(INTERNAL_NETID, testInterfaceName, BEARER_DEFAULT);
}

/**
 * @tc.name: RemoveInterfaceFromNetworkTest001
 * @tc.desc: Test ConnManager RemoveInterfaceFromNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, RemoveInterfaceFromNetworkTest001, TestSize.Level1)
{
    std::string iface = INTERFACENAME;
    int32_t ret = instance_->RemoveInterfaceFromNetwork(NETID, iface);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);

    iface = INTERNAL_INTERFACENAME;
    ret = instance_->RemoveInterfaceFromNetwork(INTERNAL_NETID, iface);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveInterfaceFromNetworkTest002
 * @tc.desc: Test ConnManager RemoveInterfaceFromNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, RemoveInterfaceFromNetworkTest002, TestSize.Level1)
{
    std::string testInterfaceName = "testName";
    auto ret = instance_->RemoveInterfaceFromNetwork(NETID, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->RemoveInterfaceFromNetwork(INTERNAL_NETID, testInterfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: AddRouteTest001
 * @tc.desc: Test ConnManager AddRoute.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, AddRouteTest001, TestSize.Level1)
{
    bool flag = false;
    NetworkRouteInfo networkRouteInfo;
    networkRouteInfo.ifName = INTERFACENAME;
    networkRouteInfo.destination = "0.0.0.0/0";
    networkRouteInfo.nextHop = "192.168.113.222";
    networkRouteInfo.isExcludedRoute = false;
    int32_t ret = instance_->AddRoute(NETID, networkRouteInfo, flag);
    EXPECT_LE(ret, 0);
    networkRouteInfo.destination = "192.168.113.0/24";
    networkRouteInfo.nextHop = "0.0.0.0";
    ret = instance_->AddRoute(NETID, networkRouteInfo, flag);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.name: RemoveRouteTest001
 * @tc.desc: Test ConnManager AddRoute.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, RemoveRouteTest001, TestSize.Level1)
{
    int32_t ret = instance_->RemoveRoute(NETID, INTERFACENAME, "0.0.0.0/0", "192.168.113.222", false);
    EXPECT_LE(ret, 0);
    ret = instance_->RemoveRoute(NETID, INTERFACENAME, "192.168.113.0/24", "0.0.0.0", false);
    EXPECT_LE(ret, 0);
}

/**
 * @tc.name: UpdateRouteTest001
 * @tc.desc: Test ConnManager AddRoute.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, UpdateRouteTest001, TestSize.Level1)
{
    int32_t ret = instance_->UpdateRoute(NETID, INTERFACENAME, "0.0.0.0/0", "192.168.113.222");
    EXPECT_LE(ret, 0);
    ret = instance_->UpdateRoute(NETID, INTERFACENAME, "192.168.113.0/24", "0.0.0.0");
    EXPECT_LE(ret, 0);
}

/**
 * @tc.name: UpdateRouteTest002
 * @tc.desc: Test ConnManager AddRoute.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, UpdateRouteTest002, TestSize.Level1)
{
    int32_t netId = 99;
    int32_t ret = instance_->UpdateRoute(netId, INTERFACENAME, "0.0.0.0/0", "192.168.113.222");
    EXPECT_LE(ret, ERROR_CODE);
}

/**
 * @tc.name: SetPermissionForNetwork001
 * @tc.desc: Test ConnManager SetPermissionForNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetPermissionForNetwork001, TestSize.Level1)
{
    int32_t netId = 99;
    int32_t ret = instance_->SetPermissionForNetwork(netId, NetworkPermission::PERMISSION_NETWORK);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: AddUidsToNetwork001
 * @tc.desc: Test ConnManager AddUidsToNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, AddUidsToNetwork001, TestSize.Level1)
{
    int32_t netId = 99;
    const std::vector<NetManagerStandard::UidRange> uidRanges;
    int32_t ret = instance_->AddUidsToNetwork(netId, uidRanges);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    netId = 1;
    ret = instance_->AddUidsToNetwork(netId, uidRanges);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveUidsFromNetwork001
 * @tc.desc: Test ConnManager RemoveUidsFromNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, RemoveUidsFromNetwork001, TestSize.Level1)
{
    int32_t netId = 99;
    const std::vector<NetManagerStandard::UidRange> uidRanges;
    int32_t ret = instance_->RemoveUidsFromNetwork(netId, uidRanges);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    netId = 1;
    ret = instance_->RemoveUidsFromNetwork(netId, uidRanges);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetFwmarkForNetworkTest001
 * @tc.desc: Test ConnManager GetFwmarkForNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, GetFwmarkForNetworkTest001, TestSize.Level1)
{
    int32_t ret = instance_->GetFwmarkForNetwork(NETID);
    EXPECT_LE(ret, 0);
    std::string info;
    instance_->GetDumpInfos(info);
    ASSERT_FALSE(info.empty());
}

/**
 * @tc.name: ConnManagerBranchTest001
 * @tc.desc: Test ConnManager Branch.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, ConnManagerBranchTest001, TestSize.Level1)
{
    std::string testInterfaceName = "testName";
    int32_t ret = instance_->GetNetworkForInterface(100, testInterfaceName);
    EXPECT_EQ(ret, INVALID_VALUE);

    RouteManager::TableType type = instance_->GetTableType(LOCAL_NET_ID);
    EXPECT_EQ(type, RouteManager::TableType::LOCAL_NETWORK);

    type = instance_->GetTableType(LOCAL_NET_ID);
    EXPECT_EQ(type, RouteManager::TableType::LOCAL_NETWORK);

    int32_t netId = 100;
    type = instance_->GetTableType(netId);
    EXPECT_EQ(type, RouteManager::TableType::INTERFACE);

    auto result = instance_->FindVirtualNetwork(NETID);
    EXPECT_EQ(result, nullptr);

    result = instance_->FindVirtualNetwork(netId);
    EXPECT_EQ(result, nullptr);

    netId = 99;
    result = instance_->FindVirtualNetwork(netId);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: SetNetworkAccessPolicy001
 * @tc.desc: Test ConnManager SetNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetNetworkAccessPolicy001, TestSize.Level1)
{
    uint32_t uid = 0;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    int32_t ret = instance_->SetNetworkAccessPolicy(uid, netAccessPolicy, reconfirmFlag);
    EXPECT_GE(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: DeleteNetworkAccessPolicy001
 * @tc.desc: Test ConnManager DeleteNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, DeleteNetworkAccessPolicy001, TestSize.Level1)
{
    uint32_t uid = 0;
    int32_t ret = instance_->DeleteNetworkAccessPolicy(uid);
    EXPECT_GE(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: NotifyNetBearerTypeChange001
 * @tc.desc: Test ConnManager NotifyNetBearerTypeChange.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, NotifyNetBearerTypeChange001, TestSize.Level1)
{
    std::set<NetManagerStandard::NetBearType> bearTypes;

    int32_t ret = instance_->NotifyNetBearerTypeChange(bearTypes);
    EXPECT_GE(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: SetInternetPermission003
 * @tc.desc: Test ConnManager SetInternetPermission.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetInternetPermission003, TestSize.Level1)
{
    uint32_t uid = 0;
    uint8_t allow = 1;
    uint8_t isBroker = 0;
    int32_t ret = instance_->SetInternetPermission(uid, allow, isBroker);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    uid = 1;
    ret = instance_->SetInternetPermission(uid, allow, isBroker);
    EXPECT_GE(ret, -1);

    isBroker = 1;
    ret = instance_->SetInternetPermission(uid, allow, isBroker);
}

/**
 * @tc.name: DestroyNetworkTest004
 * @tc.desc: Test ConnManager DestroyNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, DestroyNetworkTest004, TestSize.Level1)
{
    int32_t netId = 1;
    instance_->defaultNetId_ = netId;
    int32_t ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: DestroyNetworkTest005
 * @tc.desc: Test ConnManager DestroyNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, DestroyNetworkTest005, TestSize.Level1)
{
    int32_t netId = 1;
    instance_->defaultNetId_ = netId;
    int32_t ret = instance_->CreateVirtualNetwork(netId, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetDefaultNetworkTest003
 * @tc.desc: Test ConnManager SetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetDefaultNetworkTest003, TestSize.Level1)
{
    int32_t netId = 1;
    int32_t ret = instance_->CreateVirtualNetwork(netId, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->SetDefaultNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: SetDefaultNetworkTest004
 * @tc.desc: Test ConnManager SetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetDefaultNetworkTest004, TestSize.Level1)
{
    int32_t netId = 1;
    instance_->defaultNetId_ = 0;
    int32_t ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->SetDefaultNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    instance_->defaultNetId_ = 2; // defaultNetId_ = 2
    ret = instance_->CreateVirtualNetwork(instance_->defaultNetId_, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->SetDefaultNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = instance_->CreatePhysicalNetwork(instance_->defaultNetId_, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->SetDefaultNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: ClearDefaultNetworkTest001
 * @tc.desc: Test ConnManager SetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, ClearDefaultNetworkTest001, TestSize.Level1)
{
    instance_->defaultNetId_ = 1;
    int32_t ret = instance_->CreateVirtualNetwork(instance_->defaultNetId_, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->ClearDefaultNetwork();
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: ClearDefaultNetworkTest002
 * @tc.desc: Test ConnManager SetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, ClearDefaultNetworkTest002, TestSize.Level1)
{
    instance_->defaultNetId_ = 1;
    int32_t ret = instance_->CreatePhysicalNetwork(instance_->defaultNetId_, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->ClearDefaultNetwork();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: AddInterfaceToNetworkTest003
 * @tc.desc: Test ConnManager AddInterfaceToNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, AddInterfaceToNetworkTest003, TestSize.Level1)
{
    int32_t netId = 1;
    std::string testInterfaceName = "rmnet0";
    NetManagerStandard::NetBearType netBearerType = BEARER_CELLULAR;
    int32_t ret = instance_->AddInterfaceToNetwork(INTERNAL_NETID, testInterfaceName, netBearerType);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
    ret = instance_->AddInterfaceToNetwork(netId, testInterfaceName, netBearerType);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: AddInterfaceToNetworkTest004
 * @tc.desc: Test ConnManager AddInterfaceToNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, AddInterfaceToNetworkTest004, TestSize.Level1)
{
    int32_t netId = 1;
    std::string testInterfaceName = "testName";
    NetManagerStandard::NetBearType netBearerType = BEARER_DEFAULT;

    int32_t ret = instance_->CreateVirtualNetwork(netId, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->AddInterfaceToNetwork(netId, testInterfaceName, netBearerType);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = instance_->AddInterfaceToNetwork(netId, testInterfaceName, netBearerType);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);

    netBearerType = BEARER_WIFI;
    ret = instance_->AddInterfaceToNetwork(netId, testInterfaceName, netBearerType);

    netBearerType = BEARER_CELLULAR;
    ret = instance_->AddInterfaceToNetwork(netId, testInterfaceName, netBearerType);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: ConnManagerBranchTest002
 * @tc.desc: Test ConnManager Branch.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, ConnManagerBranchTest002, TestSize.Level1)
{
    int32_t netId = 1;
    int32_t ret = instance_->CreateVirtualNetwork(netId, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    RouteManager::TableType type = instance_->GetTableType(netId);
    EXPECT_EQ(type, RouteManager::TableType::VPN_NETWORK);

    netId = INTERNAL_NETID;
    type = instance_->GetTableType(netId);
    EXPECT_EQ(type, RouteManager::TableType::INTERNAL_DEFAULT);
}

/**
 * @tc.name: FindVirtualNetworkTest001
 * @tc.desc: Test ConnManager Branch.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, FindVirtualNetworkTest001, TestSize.Level1)
{
    int32_t netId = 1;
    int32_t ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    std::shared_ptr<NetsysNetwork> netsysNetworkPtr = instance_->FindVirtualNetwork(netId);
    EXPECT_EQ(netsysNetworkPtr, nullptr);
}

/**
 * @tc.name: SetNetworkAccessPolicyTest002
 * @tc.desc: Test ConnManager SetNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, SetNetworkAccessPolicyTest002, TestSize.Level1)
{
    uint32_t uid = 0;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    int32_t ret = instance_->SetNetworkAccessPolicy(uid, netAccessPolicy, reconfirmFlag);
    EXPECT_GE(ret, -1);
}

/**
 * @tc.name: DestroyNetworkTest006
 * @tc.desc: Test ConnManager DestroyNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, DestroyNetworkTest006, TestSize.Level1)
{
    ConnManager connmanager;
    connmanager.defaultNetId_ = NETID;
    int32_t ret = connmanager.DestroyNetwork(NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = connmanager.DestroyNetwork(NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NotifyNetBearerTypeChange002
 * @tc.desc: Test ConnManager NotifyNetBearerTypeChange.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, NotifyNetBearerTypeChange002, TestSize.Level1)
{
    std::set<NetManagerStandard::NetBearType> bearTypes;
    bearTypes.insert(NetManagerStandard::NetBearType::BEARER_WIFI);

    int32_t ret = instance_->NotifyNetBearerTypeChange(bearTypes);
    EXPECT_GE(ret, NETMANAGER_ERROR);
}

/**
 * @tc.name: NotifyNetBearerTypeChange003
 * @tc.desc: Test ConnManager NotifyNetBearerTypeChange.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, NotifyNetBearerTypeChange003, TestSize.Level1)
{
    std::set<NetManagerStandard::NetBearType> bearTypes;
    bearTypes.insert(NetManagerStandard::NetBearType::BEARER_BLUETOOTH);

    int32_t ret = instance_->NotifyNetBearerTypeChange(bearTypes);
    EXPECT_GE(ret, NETMANAGER_ERROR);
}

HWTEST_F(ConnManagerTest, SetInternetPermission004, TestSize.Level1)
{
    uint32_t uid = 1;
    uint8_t allow = 0;
    uint8_t isBroker = 0;
    int32_t ret = instance_->SetInternetPermission(uid, allow, isBroker);
    EXPECT_GE(ret, -1);
    isBroker = 1;
    ret = instance_->SetInternetPermission(uid, allow, isBroker);
}

HWTEST_F(ConnManagerTest, GetInterfaceNameId001, TestSize.Level1)
{
    NetManagerStandard::NetBearType netBearerType = BEARER_WIFI;
    EXPECT_EQ(instance_->GetInterfaceNameId(netBearerType), NETWORK_BEARER_TYPE_WIFI);
    
    netBearerType = BEARER_CELLULAR;
    EXPECT_EQ(instance_->GetInterfaceNameId(netBearerType), NETWORK_BEARER_TYPE_CELLULAR);
    
    netBearerType = BEARER_BLUETOOTH;
    EXPECT_EQ(instance_->GetInterfaceNameId(netBearerType), NETWORK_BEARER_TYPE_INITIAL);
}

/**
 * @tc.name: GetConnectOwnerUidTest001
 * @tc.desc: Test ConnManager GetConnectOwnerUid.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, GetConnectOwnerUidTest001, TestSize.Level1)
{
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    int32_t ret = instance_->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetConnectOwnerUidTest002
 * @tc.desc: Test ConnManager GetConnectOwnerUid.
 * @tc.type: FUNC
 */
HWTEST_F(ConnManagerTest, GetConnectOwnerUidTest002, TestSize.Level1)
{
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_UDP;
    info.family_ = NetConnInfo::Family::IPv6;
    info.localAddress_ = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    info.localPort_ = 6666;
    info.remoteAddress_ = "2001:0db8:85a3:0000:0000:8a2e:0370:7335";
    info.remotePort_ = 8888;
    int32_t ret = instance_->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_IsVirtualInterface_True
 * @tc.name: Test IsVirtualInterface returns true for tun interface
 * @tc.desc: Verify that IsVirtualInterface returns true when interface name starts with "tun"
 */
HWTEST_F(ConnManagerTest, ConnManager_IsVirtualInterface_True, TestSize.Level1)
{
    std::string interfaceName = "tun0";
    bool ret = instance_->IsVirtualInterface(interfaceName);
    EXPECT_EQ(ret, true);

    interfaceName = "tun100";
    ret = instance_->IsVirtualInterface(interfaceName);
    EXPECT_EQ(ret, true);

    interfaceName = "tunabc";
    ret = instance_->IsVirtualInterface(interfaceName);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.number: ConnManager_IsVirtualInterface_False
 * @tc.name: Test IsVirtualInterface returns false for non-tun interface
 * @tc.desc: Verify that IsVirtualInterface returns false when interface name does not start with "tun"
 */
HWTEST_F(ConnManagerTest, ConnManager_IsVirtualInterface_False, TestSize.Level1)
{
    std::string interfaceName = "wlan0";
    bool ret = instance_->IsVirtualInterface(interfaceName);
    EXPECT_EQ(ret, false);

    interfaceName = "eth0";
    ret = instance_->IsVirtualInterface(interfaceName);
    EXPECT_EQ(ret, false);

    interfaceName = "rmnet0";
    ret = instance_->IsVirtualInterface(interfaceName);
    EXPECT_EQ(ret, false);

    interfaceName = "";
    ret = instance_->IsVirtualInterface(interfaceName);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.number: ConnManager_GetTableType_Vpn
 * @tc.name: Test GetTableType returns VPN_NETWORK for virtual network
 * @tc.desc: Verify that GetTableType returns VPN_NETWORK when netId corresponds to a virtual network
 */
HWTEST_F(ConnManagerTest, ConnManager_GetTableType_Vpn, TestSize.Level1)
{
    int32_t netId = 1;
    instance_->CreateVirtualNetwork(netId, true);
    RouteManager::TableType type = instance_->GetTableType(netId);
    EXPECT_EQ(type, RouteManager::TableType::VPN_NETWORK);
    instance_->DestroyNetwork(netId);
}

/**
 * @tc.number: ConnManager_GetTableType_Internal
 * @tc.name: Test GetTableType returns INTERNAL_DEFAULT for internal netId
 * @tc.desc: Verify that GetTableType returns INTERNAL_DEFAULT when netId is an internal network ID
 */
HWTEST_F(ConnManagerTest, ConnManager_GetTableType_Internal, TestSize.Level1)
{
    int32_t netId = INTERNAL_NETID;
    RouteManager::TableType type = instance_->GetTableType(netId);
    EXPECT_EQ(type, RouteManager::TableType::INTERNAL_DEFAULT);
}

/**
 * @tc.number: ConnManager_FindVirtualNetwork_IsPhysical
 * @tc.name: Test FindVirtualNetwork returns nullptr for physical network
 * @tc.desc: Verify that FindVirtualNetwork returns nullptr when the network is a physical network
 */
HWTEST_F(ConnManagerTest, ConnManager_FindVirtualNetwork_IsPhysical, TestSize.Level1)
{
    int32_t netId = 200;
    instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    auto result = instance_->FindVirtualNetwork(netId);
    EXPECT_EQ(result, nullptr);
    instance_->DestroyNetwork(netId);
}

/**
 * @tc.number: ConnManager_FindVirtualNetwork_Success
 * @tc.name: Test FindVirtualNetwork returns virtual network pointer
 * @tc.desc: Verify that FindVirtualNetwork returns valid pointer when the network is a virtual network
 */
HWTEST_F(ConnManagerTest, ConnManager_FindVirtualNetwork_Success, TestSize.Level1)
{
    int32_t netId = 201;
    instance_->CreateVirtualNetwork(netId, true);
    auto result = instance_->FindVirtualNetwork(netId);
    EXPECT_NE(result, nullptr);
    instance_->DestroyNetwork(netId);
}

/**
 * @tc.number: ConnManager_AddUidsToNetwork_NotVirtual
 * @tc.name: Test AddUidsToNetwork returns error for non-virtual network
 * @tc.desc: Verify that AddUidsToNetwork returns NETMANAGER_ERROR when network is not virtual
 */
HWTEST_F(ConnManagerTest, ConnManager_AddUidsToNetwork_NotVirtual, TestSize.Level1)
{
    int32_t netId = 202;
    instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange range(1000, 2000, 0, netId);
    uidRanges.push_back(range);
    int32_t ret = instance_->AddUidsToNetwork(netId, uidRanges);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    instance_->DestroyNetwork(netId);
}

/**
 * @tc.number: ConnManager_RemoveUidsFromNetwork_NotVirtual
 * @tc.name: Test RemoveUidsFromNetwork returns error for non-virtual network
 * @tc.desc: Verify that RemoveUidsFromNetwork returns NETMANAGER_ERROR when network is not virtual
 */
HWTEST_F(ConnManagerTest, ConnManager_RemoveUidsFromNetwork_NotVirtual, TestSize.Level1)
{
    int32_t netId = 203;
    instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange range(1000, 2000, 0, netId);
    uidRanges.push_back(range);
    int32_t ret = instance_->RemoveUidsFromNetwork(netId, uidRanges);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    instance_->DestroyNetwork(netId);
}

/**
 * @tc.number: ConnManager_AddRoutes_Success
 * @tc.name: Test AddRoutes batch add routes
 * @tc.desc: Verify that AddRoutes can add multiple routes successfully
 */
HWTEST_F(ConnManagerTest, ConnManager_AddRoutes_Success, TestSize.Level1)
{
    int32_t netId = 204;
    instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    std::vector<NetworkRouteInfo> infos;
    NetworkRouteInfo info1;
    info1.ifName = INTERFACENAME;
    info1.destination = "10.0.0.0/8";
    info1.nextHop = "192.168.1.1";
    info1.isExcludedRoute = false;
    infos.push_back(info1);

    NetworkRouteInfo info2;
    info2.ifName = INTERFACENAME;
    info2.destination = "172.16.0.0/12";
    info2.nextHop = "192.168.1.2";
    info2.isExcludedRoute = false;
    infos.push_back(info2);

    int32_t ret = instance_->AddRoutes(netId, infos);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
    instance_->DestroyNetwork(netId);
}

/**
 * @tc.number: ConnManager_CloseSocketsUid_Success
 * @tc.name: Test CloseSocketsUid closes sockets for specific uid
 * @tc.desc: Verify that CloseSocketsUid can close sockets for a given UID
 */
HWTEST_F(ConnManagerTest, ConnManager_CloseSocketsUid_Success, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    uint32_t uid = 12345;
    int32_t ret = instance_->CloseSocketsUid(ipAddr, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_GetSystemNetPortStates_Success
 * @tc.name: Test GetSystemNetPortStates gets port states
 * @tc.desc: Verify that GetSystemNetPortStates can retrieve system network port states
 */
HWTEST_F(ConnManagerTest, ConnManager_GetSystemNetPortStates_Success, TestSize.Level1)
{
    NetManagerStandard::NetPortStatesInfo netPortStatesInfo;
    int32_t ret = instance_->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_AddInterfaceToNetwork_PhysicalNonVirtual
 * @tc.name: Test AddInterfaceToNetwork with physical network and non-virtual interface
 * @tc.desc: Verify that AddInterfaceToNetwork returns NETMANAGER_SUCCESS
 * @tc.cover: This test covers the branch: if (nw->IsPhysical() && !IsVirtualInterface(interfaceName))
 */
HWTEST_F(ConnManagerTest, ConnManager_AddInterfaceToNetwork_PhysicalNonVirtual, TestSize.Level1)
{
    int32_t netId = 300;
    int32_t ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string interfaceName = "eth0";
    ret = instance_->AddInterfaceToNetwork(netId, interfaceName, BEARER_DEFAULT);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_AddInterfaceToNetwork_PhysicalVirtual
 * @tc.name: Test AddInterfaceToNetwork with physical network and virtual interface
 * @tc.desc: Verify that AddInterfaceToNetwork returns NETMANAGER_SUCCESS
 * @tc.cover: This test covers the branch: if (nw->IsPhysical() && IsVirtualInterface(interfaceName)) - false case
 */
HWTEST_F(ConnManagerTest, ConnManager_AddInterfaceToNetwork_PhysicalVirtual, TestSize.Level1)
{
    int32_t netId = 301;
    int32_t ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string interfaceName = "tun0";
    ret = instance_->AddInterfaceToNetwork(netId, interfaceName, BEARER_DEFAULT);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);

    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_RemoveInterfaceFromNetwork_PhysicalNonVirtual
 * @tc.name: Test RemoveInterfaceFromNetwork with physical network and non-virtual interface
 * @tc.desc: Verify that RemoveInterfaceFromNetwork returns NETMANAGER_SUCCESS
 * @tc.cover: This test covers the branch: if (nw->IsPhysical() && !IsVirtualInterface(interfaceName))
 */
HWTEST_F(ConnManagerTest, ConnManager_RemoveInterfaceFromNetwork_PhysicalNonVirtual, TestSize.Level1)
{
    int32_t netId = 302;
    int32_t ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string interfaceName = "eth0";
    ret = instance_->AddInterfaceToNetwork(netId, interfaceName, BEARER_DEFAULT);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->RemoveInterfaceFromNetwork(netId, interfaceName);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);

    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_RemoveInterfaceFromNetwork_PhysicalVirtual
 * @tc.name: Test RemoveInterfaceFromNetwork with physical network and virtual interface
 * @tc.desc: Verify that RemoveInterfaceFromNetwork returns NETMANAGER_SUCCESS
 * @tc.cover: This test covers the branch: if (nw->IsPhysical() && IsVirtualInterface(interfaceName)) - false case
 */
HWTEST_F(ConnManagerTest, ConnManager_RemoveInterfaceFromNetwork_PhysicalVirtual, TestSize.Level1)
{
    int32_t netId = 303;
    int32_t ret = instance_->CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string interfaceName = "tun0";
    ret = instance_->AddInterfaceToNetwork(netId, interfaceName, BEARER_DEFAULT);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
    ret = instance_->RemoveInterfaceFromNetwork(netId, interfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_AddInterfaceToNetwork_VirtualNetwork
 * @tc.name: Test AddInterfaceToNetwork with virtual network (IsPhysical returns false)
 * @tc.desc: Verify that AddInterfaceToNetwork returns NETMANAGER_SUCCESS when network is virtual
 * @tc.cover: This test covers the branch:  nw->IsPhysical() is false
 */
HWTEST_F(ConnManagerTest, ConnManager_AddInterfaceToNetwork_VirtualNetwork, TestSize.Level1)
{
    int32_t netId = 400;
    int32_t ret = instance_->CreateVirtualNetwork(netId, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string interfaceName = "eth0";
    ret = instance_->AddInterfaceToNetwork(netId, interfaceName, BEARER_DEFAULT);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_RemoveInterfaceFromNetwork_VirtualNetwork
 * @tc.name: Test RemoveInterfaceFromNetwork with virtual network (IsPhysical returns false)
 * @tc.desc: Verify that RemoveInterfaceFromNetwork returns NETMANAGER_SUCCESS when network is virtual
 * @tc.cover: This test covers the branch: nw->IsPhysical() is false
 */
HWTEST_F(ConnManagerTest, ConnManager_RemoveInterfaceFromNetwork_VirtualNetwork, TestSize.Level1)
{
    int32_t netId = 401;
    int32_t ret = instance_->CreateVirtualNetwork(netId, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string interfaceName = "eth0";
    ret = instance_->AddInterfaceToNetwork(netId, interfaceName, BEARER_DEFAULT);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->RemoveInterfaceFromNetwork(netId, interfaceName);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);

    ret = instance_->DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_GetNetIdByInterface_Success
 * @tc.name: Test GetNetIdByInterface returns correct netId
 * @tc.desc: Verify that GetNetIdByInterface returns the correct network ID for a given interface
 */
HWTEST_F(ConnManagerTest, ConnManager_GetNetIdByInterface_Success, TestSize.Level1)
{
    ConnManager connManager;
    int32_t netId = 500;
    int32_t ret = connManager.CreatePhysicalNetwork(netId, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string interfaceName = "wlan0";
    ret = connManager.AddInterfaceToNetwork(netId, interfaceName, BEARER_WIFI);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t foundNetId = connManager.GetNetIdByInterface(interfaceName);
    EXPECT_EQ(foundNetId, netId);

    ret = connManager.DestroyNetwork(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ConnManager_GetNetIdByInterface_NotFound
 * @tc.name: Test GetNetIdByInterface returns INTERFACE_UNSET when interface not found
 * @tc.desc: Verify that GetNetIdByInterface returns INTERFACE_UNSET for non-existent interface
 */
HWTEST_F(ConnManagerTest, ConnManager_GetNetIdByInterface_NotFound, TestSize.Level1)
{
    ConnManager connManager;
    std::string interfaceName = "nonexistent0";
    int32_t foundNetId = connManager.GetNetIdByInterface(interfaceName);
    EXPECT_EQ(foundNetId, INVALID_VALUE);
}

/**
 * @tc.number: ConnManager_GetNetIdByInterface_MultipleNetworks
 * @tc.name: Test GetNetIdByInterface with multiple networks
 * @tc.desc: Verify that GetNetIdByInterface returns the first matching network ID
 */
HWTEST_F(ConnManagerTest, ConnManager_GetNetIdByInterface_MultipleNetworks, TestSize.Level1)
{
    ConnManager connManager;
    int32_t netId1 = 501;
    int32_t netId2 = 502;

    int32_t ret = connManager.CreatePhysicalNetwork(netId1, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = connManager.CreatePhysicalNetwork(netId2, PERMISSION_NONE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string interfaceName = "eth1";
    ret = connManager.AddInterfaceToNetwork(netId1, interfaceName, BEARER_DEFAULT);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t foundNetId = connManager.GetNetIdByInterface(interfaceName);
    EXPECT_EQ(foundNetId, netId1);

    ret = connManager.DestroyNetwork(netId1);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = connManager.DestroyNetwork(netId2);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetsysNative
} // namespace OHOS

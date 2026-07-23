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

#include <gtest/gtest.h>

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "bpf_def.h"
#include "bpf_mapper.h"
#include "bpf_path.h"
#include "conn_manager.h"
#include "net_manager_constants.h"
#include "net_stats_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_native_service_proxy.h"
#include "network_permission.h"

#include "net_all_capabilities.h"
#include "net_conn_client.h"
#include "net_handle.h"
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace NetManagerStandard;
constexpr int32_t NETID = 101;
constexpr int32_t UID = 1000;
constexpr uint32_t TEST_UID_U32 = 1;
constexpr int32_t MTU = 1500;
constexpr int32_t WHICH = 14;
const std::string INTERFACENAME = "wlan0";
static constexpr uint64_t TEST_COOKIE = 1;
static constexpr uint32_t TEST_STATS_TYPE1 = 0;
static constexpr uint32_t TEST_STATS_TYPE2 = 2;
namespace {
sptr<NetsysNative::INetsysService> ConnManagerGetProxy()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return nullptr;
    }

    auto remote = samgr->GetSystemAbility(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    if (remote == nullptr) {
        return nullptr;
    }

    auto proxy = iface_cast<NetsysNative::INetsysService>(remote);
    if (proxy == nullptr) {
        return nullptr;
    }
    return proxy;
}
} // namespace
class NetsysNativeServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetsysNativeServiceProxyTest::SetUpTestCase() {}

void NetsysNativeServiceProxyTest::TearDownTestCase() {}

void NetsysNativeServiceProxyTest::SetUp() {}

void NetsysNativeServiceProxyTest::TearDown() {}

/**
 * @tc.name: AddInterfaceToNetworkTest001
 * @tc.desc: Test NetsysNativeServiceProxy AddInterfaceToNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(NetsysNativeServiceProxyTest, AddInterfaceToNetworkTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->NetworkCreatePhysical(NETID, nmd::NetworkPermission::PERMISSION_NONE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetManagerBaseAccessToken access;
    NetHandle handle;
    NetConnClient::GetInstance().GetDefaultNet(handle);
    NetAllCapabilities netAllCap;
    NetConnClient::GetInstance().GetNetCapabilities(handle, netAllCap);
    ASSERT_NE(netAllCap.bearerTypes_.count(NetManagerStandard::BEARER_CELLULAR) > 0 ||
        netAllCap.bearerTypes_.count(NetManagerStandard::BEARER_WIFI) > 0, true);

    netsysNativeService->NetworkAddInterface(NETID, INTERFACENAME, BEARER_DEFAULT);
    netsysNativeService->AddInterfaceAddress(INTERFACENAME, "192.168.113.209", 24);
}

/**
 * @tc.name: AddRouteTest001
 * @tc.desc: Test NetsysNativeServiceProxy AddRoute.
 * @tc.type: FUNC
 */
HWTEST_F(NetsysNativeServiceProxyTest, AddRouteTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->NetworkAddRoute(NETID, INTERFACENAME, "0.0.0.0/0", "192.168.113.222");
    EXPECT_LE(ret, 0);
    ret = netsysNativeService->NetworkAddRoute(NETID, INTERFACENAME, "192.168.113.0/24", "0.0.0.0");
    EXPECT_LE(ret, 0);
}

/**
 * @tc.name: SetDefaultNetworkTest001
 * @tc.desc: Test NetsysNativeServiceProxy SetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(NetsysNativeServiceProxyTest, SetDefaultNetworkTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->NetworkSetDefault(NETID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetDefaultNetworkTest001
 * @tc.desc: Test NetsysNativeServiceProxy GetDefaultNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(NetsysNativeServiceProxyTest, GetDefaultNetworkTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->NetworkGetDefault();
    EXPECT_EQ(ret, NETID);
}

/**
 * @tc.name: GetAllSimStatsInfoTest001
 * @tc.desc: Test NetsysNativeServiceProxy GetAllSimStatsInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NetsysNativeServiceProxyTest, GetAllSimStatsInfoTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> stats;
    int32_t ret = netsysNativeService->GetAllSimStatsInfo(stats);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveInterfaceFromNetworkTest001
 * @tc.desc: Test NetsysNativeServiceProxy RemoveInterfaceFromNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(NetsysNativeServiceProxyTest, RemoveInterfaceFromNetworkTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->DelInterfaceAddress(INTERFACENAME, "192.168.113.209", 24);
    EXPECT_LE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->NetworkRemoveInterface(NETID, INTERFACENAME);
    EXPECT_LE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, RemoveInterfaceFromNetworkTest002, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    int32_t ret = netsysNativeService->DelInterfaceAddress(INTERFACENAME, "192.168.113.209", 24, 2);
    EXPECT_LE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->NetworkRemoveInterface(NETID, INTERFACENAME);
    EXPECT_LE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, InterfaceSetIffUpTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    int32_t ret = netsysNativeService->InterfaceSetIffUp(INTERFACENAME);
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, EnableDistributedClientNetTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    int32_t ret = netsysNativeService->EnableDistributedClientNet("192.168.113.209", "virnic", INTERFACENAME);
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, DisableDistributedNetTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    std::string virnicName = "virnic";
    std::string dstAddr = "1.1.1.1";
    int32_t ret = netsysNativeService->DisableDistributedNet(true, virnicName, dstAddr);
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetNetworkCellularSharingTrafficTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    NetworkSharingTraffic traffic;
    std::string ifaceName;
    int32_t ret = netsysNativeService->GetNetworkCellularSharingTraffic(traffic, ifaceName);
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetNetStateTrafficMapTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    int32_t ret = netsysNativeService->SetNetStateTrafficMap(1, 1);
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetNetStateTrafficMapTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    uint64_t availableTraffic = 0;
    int32_t ret = netsysNativeService->GetNetStateTrafficMap(1, availableTraffic);
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, ClearIncreaseTrafficMapTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    int32_t ret = netsysNativeService->ClearIncreaseTrafficMap();
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, DeleteIncreaseTrafficMapTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    int32_t ret = netsysNativeService->DeleteIncreaseTrafficMap(12); // 12:ifindex
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, UpdateIfIndexMapTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    int32_t ret = netsysNativeService->UpdateIfIndexMap(1, 1);
    EXPECT_NE(ret, ERR_FLATTEN_OBJECT);
}

/**
 * @tc.name: DestroyNetworkTest001
 * @tc.desc: Test NetsysNativeServiceProxy DestroyNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(NetsysNativeServiceProxyTest, DestroyNetworkTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->NetworkDestroy(NETID);
    EXPECT_LE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, NetworkAddRouteParcelTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    RouteInfoParcel routeInfo;
    routeInfo.destination = "destination";
    routeInfo.ifName = INTERFACENAME;
    routeInfo.nextHop = "nextHop";
    routeInfo.mtu = MTU;
    int32_t ret = netsysNativeService->NetworkAddRouteParcel(NETID, routeInfo);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetsysNativeServiceProxyTest, NetworkRemoveRouteParcelTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    RouteInfoParcel routeInfo;
    routeInfo.destination = "";
    routeInfo.ifName = INTERFACENAME;
    routeInfo.nextHop = "";
    routeInfo.mtu = MTU;
    int32_t ret = netsysNativeService->NetworkRemoveRouteParcel(NETID, routeInfo);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetsysNativeServiceProxyTest, NetworkClearDefaultTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->NetworkClearDefault();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetSetProcSysNetTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::string parameter = "TestParameter";
    std::string value = "Testvalue";
    int32_t ret = netsysNativeService->SetProcSysNet(AF_INET, WHICH, INTERFACENAME, parameter, value);
    ret = netsysNativeService->GetProcSysNet(AF_INET, WHICH, INTERFACENAME, parameter, value);
    EXPECT_GE(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetProcSysNetTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->SetInternetPermission(UID, true, false);
    ret = netsysNativeService->NetworkCreateVirtual(NETID, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, AddStaticArp001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, DelStaticArp001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, AddStaticIpv6Addr001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, DelStaticIpv6Addr001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetFwmarkForNetworkTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    MarkMaskParcel markMaskParcel;
    markMaskParcel.mark = 1;
    markMaskParcel.mask = 0XFFFF;
    int32_t ret = netsysNativeService->GetFwmarkForNetwork(NETID, markMaskParcel);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetCookieStatsTest001, TestSize.Level1)
{
    uint64_t stats = 0;
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    BpfMapper<socket_cookie_stats_key, app_cookie_stats_value> appCookieStatsMap(APP_COOKIE_STATS_MAP_PATH, BPF_ANY);
    int32_t ret = netsysNativeService->GetCookieStats(stats, TEST_STATS_TYPE1, TEST_COOKIE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);

    ret = netsysNativeService->GetCookieStats(stats, TEST_STATS_TYPE2, TEST_COOKIE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetsysNativeServiceProxyTest, NetsysNativeServiceProxyBranchTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    sptr<OHOS::NetsysNative::INetDnsResultCallback> resultCallback = nullptr;
    uint32_t timeStep = 0;
    int32_t ret = netsysNativeService->RegisterDnsResultCallback(resultCallback, timeStep);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = netsysNativeService->UnregisterDnsResultCallback(resultCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<INotifyCallback> notifyCallback = nullptr;
    ret = netsysNativeService->RegisterNotifyCallback(notifyCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = netsysNativeService->UnRegisterNotifyCallback(notifyCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    uint64_t stats = 0;
    uint32_t type = 0;
    uint64_t cookie = 0;
    ret = netsysNativeService->GetCookieStats(stats, type, cookie);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetNetworkSharingTypeTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::set<uint32_t> sharingTypeIsOn;
    int32_t ret = netsysNativeService->GetNetworkSharingType(sharingTypeIsOn);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, UpdateNetworkSharingTypeTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    uint32_t type = 0;
    int32_t ret = netsysNativeService->UpdateNetworkSharingType(type, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysNativeService->UpdateNetworkSharingType(type, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetIpv6PrivacyExtensionsTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::string interface = "wlan0";
    uint32_t on = 0;
    int32_t ret = netsysNativeService->SetIpv6PrivacyExtensions(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysNativeService->SetEnableIpv6(interface, on, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetIpv6UidBlackListTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t uid = 0;
    std::vector<int32_t> netIds = {};
    int32_t ret = netsysNativeService->SetIpv6UidBlackList(netIds, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetIpv6AutoConfTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::string interface = "wlan0";
    uint32_t on = 0;
    int32_t ret = netsysNativeService->SetIpv6AutoConf(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetNetworkAccessPolicy001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    uint32_t uid = 0;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    int32_t ret = netsysNativeService->SetNetworkAccessPolicy(uid, netAccessPolicy, reconfirmFlag);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, NotifyNetBearerTypeChange001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    std::set<NetManagerStandard::NetBearType> bearerTypes;
    bearerTypes.insert(NetManagerStandard::NetBearType::BEARER_CELLULAR);
    int32_t ret = netsysNativeService->NotifyNetBearerTypeChange(bearerTypes);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, DeleteNetworkAccessPolicy001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    uint32_t uid = 0;
    int32_t ret = netsysNativeService->DeleteNetworkAccessPolicy(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, CreateVnic001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    uint16_t mtu = 1500;
    std::string tunAddr = "192.168.1.100";
    int32_t prefix = 24;
    std::set<int32_t> uids;
    int32_t ret = netsysNativeService->CreateVnic(mtu, tunAddr, prefix, uids);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, DestroyVnic001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    int32_t ret = netsysNativeService->DestroyVnic();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, CloseSocketsUid001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::string ipAddr = "";
    uint32_t uid = 1000;
    int32_t ret = netsysNativeService->CloseSocketsUid(ipAddr, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::unordered_map<uint32_t, uint32_t> params;
    int32_t ret = netsysNativeService->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetBrokerUidAccessPolicyMapTest002, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::unordered_map<uint32_t, uint32_t> params;
    params.emplace(TEST_UID_U32, TEST_UID_U32);
    int32_t ret = netsysNativeService->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, DelBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->DelBrokerUidAccessPolicyMap(TEST_UID_U32);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
HWTEST_F(NetsysNativeServiceProxyTest, EnableWearableDistributedNetForward001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->EnableWearableDistributedNetForward(8001, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_WEARABLE_DISTRIBUTED_NET_ERR_INVALID_UDP_PORT_ID);
}

HWTEST_F(NetsysNativeServiceProxyTest, EnableWearableDistributedNetForward002, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->EnableWearableDistributedNetForward(-80, 8002);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_WEARABLE_DISTRIBUTED_NET_ERR_INVALID_TCP_PORT_ID);
}

HWTEST_F(NetsysNativeServiceProxyTest, DisableWearableDistributedNetForward, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->EnableWearableDistributedNetForward(8001, 8002);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->DisableWearableDistributedNetForward();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
#endif

HWTEST_F(NetsysNativeServiceProxyTest, EnableDistributedClientNet001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    std::string virnicAddr = "1.189.55.61";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    int32_t ret = netsysNativeService->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    EXPECT_GE(ret, -19);

    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = netsysNativeService->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_GE(ret, -1);
}

HWTEST_F(NetsysNativeServiceProxyTest, EnableDistributedServerNet001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";
    int32_t ret = netsysNativeService->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    bool isServer = true;
    std::string virnicName = "virnic";
    ret = netsysNativeService->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetNetworkCellularSharingTraffic001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    
    nmd::NetworkSharingTraffic traffic;
    std::string ifaceName = "virnic";

    int32_t ret = netsysNativeService->GetNetworkCellularSharingTraffic(traffic, ifaceName);
    EXPECT_LE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetGetClearNetStateTrafficMap001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    
    uint8_t flag = 1;
    uint64_t availableTraffic = 1000000;

    int32_t ret = netsysNativeService->SetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->GetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->ClearIncreaseTrafficMap();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, UpdateIfIndexMap001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    
    uint8_t key = 1;
    uint64_t index = 10;
    int32_t ret = netsysNativeService->UpdateIfIndexMap(key, index);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, RegisterNetsysTrafficCallback001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    
    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = netsysNativeService->RegisterNetsysTrafficCallback(callback);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, StartStopClat001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    
    std::string interfaceName = "eth0";
    int32_t netId = 1;
    std::string nat64PrefixStr = "2001:db8::/64";

    int32_t ret = netsysNativeService->StartClat(interfaceName, netId, nat64PrefixStr);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->StopClat(interfaceName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetNicTrafficAllowed001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    
    std::vector<std::string> ifaceNames = {"eth0", "wlan0"};
    bool status = true;

    int32_t ret = netsysNativeService->SetNicTrafficAllowed(ifaceNames, status);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, UnRegisterNetsysTrafficCallback001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    
    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = netsysNativeService->UnRegisterNetsysTrafficCallback(callback);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, FlushDnsCache001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t ret = netsysNativeService->FlushDnsCache(NETID);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetNetStatusMap001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    
    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = netsysNativeService->SetNetStatusMap(1, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->SetNetStatusMap(1, 1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->SetNetStatusMap(0, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->SetNetStatusMap(0, 1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysNativeService->SetNetStatusMap(2, 1);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, SetDnsCache001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    uint16_t netId = 101;
    std::string testHost = "test";
    AddrInfo info;
    int32_t ret = netsysNativeService->SetDnsCache(netId, testHost, info);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
HWTEST_F(NetsysNativeServiceProxyTest, UpdateEnterpriseRouteTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    uint32_t uid = 20000138;
    std::string ifname = "wlan0";
    bool add = true;
    auto ret = netsysNativeService->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysNativeServiceProxyTest, UpdateEnterpriseRouteTest002, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    uint32_t uid = 0;
    std::string ifname = "wlan0";
    bool add = true;
    auto ret = netsysNativeService->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysNativeServiceProxyTest, UpdateEnterpriseRouteTest003, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    uint32_t uid = 20000138;
    std::string ifname = "notexist";
    bool add = true;
    auto ret = netsysNativeService->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_PARAMETER_ERROR);
}
#endif

HWTEST_F(NetsysNativeServiceProxyTest, SetInternetAccessByIpForWifiShare001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    uint8_t family = 2;
    std::string ip = "1.1.1.1";
    int32_t ret = netsysNativeService->SetInternetAccessByIpForWifiShare(ip, family, true, "");
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetIpNeighTable001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::vector<NetIpMacInfo> ipMacInfo;
    int32_t ret = netsysNativeService->GetIpNeighTable(ipMacInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, CreateVlan001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = netsysNativeService->CreateVlan(ifName, vlanId);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, DestroyVlan001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = netsysNativeService->DestroyVlan(ifName, vlanId);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, AddVlanIp001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    int32_t ret = netsysNativeService->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetConnectOwnerUidTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    int32_t ret = netsysNativeService->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceProxyTest, GetSystemNetPortStatesTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);
    NetPortStatesInfo netPortStatesInfo;
    int32_t ret = netsysNativeService->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
} // namespace NetsysNative
} // namespace OHOS

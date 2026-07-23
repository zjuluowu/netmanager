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

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "common_net_conn_callback_test.h"
#include "http_proxy.h"
#include "net_all_capabilities.h"
#include "net_conn_callback_stub.h"
#include "net_conn_client.h"
#include "net_conn_constants.h"
#include "net_conn_service.h"
#include "refresh_http_proxy_callback_stub.h"
#include "net_conn_types.h"
#include "net_detection_callback_test.h"
#include "net_factoryreset_callback_stub.h"
#include "net_http_proxy_tracker.h"
#include "net_interface_callback_stub.h"
#include "net_manager_center.h"
#include "net_mgr_log_wrapper.h"
#include "net_probe_callback_test.h"
#include "netmanager_base_test_security.h"
#include "netsys_controller.h"
#include "parameters.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr uint32_t TEST_TIMEOUTMS = 1000;
constexpr int32_t TEST_NETID = 3;
constexpr int32_t TEST_SOCKETFD = 2;
const int32_t NET_ID = 2;
const int32_t SOCKET_FD = 2;
const int32_t ZERO_VALUE = 0;
const int32_t INVALID_VALUE = 10;
constexpr const char *TEST_IDENT = "testIdent";
constexpr const char *TEST_HOST = "testHost";
constexpr const char *TEST_PROXY_HOST = "testHttpProxy";
constexpr const char *TEST_IPV4_ADDR = "127.0.0.1";
constexpr const char *TEST_IPV6_ADDR = "240C:1:1:1::1";
constexpr const char *TEST_DOMAIN1 = ".com";
constexpr const char *TEST_DOMAIN2 = "test.com";
constexpr const char *TEST_DOMAIN3 = "testcom";
constexpr const char *TEST_DOMAIN4 = "com.test";
constexpr const char *TEST_DOMAIN5 = "test.co.uk";
constexpr const char *TEST_DOMAIN6 = "test.com.com";
constexpr const char *TEST_DOMAIN7 = "test1.test2.test3.test4.test5.com";
constexpr const char *TEST_DOMAIN8 = "http://www.example.com";
constexpr const char *TEST_DOMAIN9 = "https://www.example.com";
constexpr const char *TEST_DOMAIN10 = "httpd://www.example.com";
constexpr const char *TEST_LONG_HOST =
    "0123456789qwertyuiopasdfghjklzxcvbnm[]:;<>?!@#$%^&()AEFFEqwdqwrtfasfj4897qwe465791qwr87tq4fq7t8qt4654qwr";
constexpr const char *TEST_LONG_EXCLUSION_LIST =
    "www.test0.com,www.test1.com,www.test2.com,www.test3.com,www.test4.com,www.test5.com,www.test6.com,www.test7.com,"
    "www.test8.com,www.test9.com,www.test10.com,www.test11.com,www.test12.com,www.test12.com,www.test12.com,www.test13."
    "com,www.test14.com,www.test15.com,www.test16.com,www.test17.com,www.test18.com,www.test19.com,www.test20.com";
constexpr const char *NET_CONN_MANAGER_WORK_THREAD = "NET_CONN_MANAGER_WORK_THREAD";
constexpr int64_t TEST_UID = 1010;
constexpr uint32_t TEST_NOTEXISTSUPPLIER = 1000;
constexpr int32_t MAIN_USERID = 100;
constexpr int32_t INVALID_USERID = 1;
constexpr long SUCCESS_CODE = 204;
constexpr int32_t RETRY_TIMES = 3;
constexpr const char *PROXY_NAME = "123456789";
constexpr int32_t PROXY_NAME_SIZE = 9;

sptr<INetConnCallback> g_callback = new (std::nothrow) NetConnCallbackStubCb();
sptr<INetDetectionCallback> g_detectionCallback = new (std::nothrow) NetDetectionCallbackTest();
uint32_t g_supplierId = 0;
uint32_t g_vpnSupplierId = 0;
} // namespace

class NetConnServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetConnServiceTest::SetUpTestCase()
{
    NetConnService::GetInstance()->OnStart();
    if (NetConnService::GetInstance()->state_ != NetConnService::STATE_RUNNING) {
        NetConnService::GetInstance()->netConnEventRunner_ =
            AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
        ASSERT_NE(NetConnService::GetInstance()->netConnEventRunner_, nullptr);
        NetConnService::GetInstance()->netConnEventHandler_ =
            std::make_shared<NetConnEventHandler>(NetConnService::GetInstance()->netConnEventRunner_);
        NetConnService::GetInstance()->serviceIface_ = std::make_unique<NetConnServiceIface>().release();
        NetManagerCenter::GetInstance().RegisterConnService(NetConnService::GetInstance()->serviceIface_);
        NetHttpProxyTracker httpProxyTracker;
        HttpProxy httpProxy;
        httpProxy.SetPort(0);
        httpProxyTracker.ReadFromSettingsData(httpProxy);
        NetConnService::GetInstance()->SendHttpProxyChangeBroadcast(httpProxy);
    }
}

void NetConnServiceTest::TearDownTestCase() {}

void NetConnServiceTest::SetUp() {}

void NetConnServiceTest::TearDown() {}

HWTEST_F(NetConnServiceTest, OnRemoveSystemAbility001, TestSize.Level1)
{
    std::string deviceId = "dev1";
    NetConnService::GetInstance()->OnRemoveSystemAbility(COMM_NETSYS_NATIVE_SYS_ABILITY_ID, deviceId);
    EXPECT_TRUE(NetConnService::GetInstance()->hasSARemoved_);
}

HWTEST_F(NetConnServiceTest, OnAddSystemAbility001, TestSize.Level1)
{
    std::string deviceId = "dev1";
    NetConnService::GetInstance()->OnAddSystemAbility(COMM_NETSYS_NATIVE_SYS_ABILITY_ID, deviceId);
    EXPECT_FALSE(NetConnService::GetInstance()->hasSARemoved_);
    NetConnService::GetInstance()->OnAddSystemAbility(COMM_NET_POLICY_MANAGER_SYS_ABILITY_ID, deviceId);
    EXPECT_FALSE(NetConnService::GetInstance()->hasSARemoved_);
    NetConnService::GetInstance()->OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, deviceId);
    EXPECT_FALSE(NetConnService::GetInstance()->hasSARemoved_);
    NetConnService::GetInstance()->OnAddSystemAbility(-1, deviceId);
    EXPECT_FALSE(NetConnService::GetInstance()->hasSARemoved_);
}

HWTEST_F(NetConnServiceTest, IsSupplierMatchRequestAndNetworkTest001, TestSize.Level1)
{
    sptr<NetSupplier> supplier = nullptr;
    bool ret = NetConnService::GetInstance()->IsSupplierMatchRequestAndNetwork(supplier);
    EXPECT_FALSE(ret);

    NetConnService::GetInstance()->CreateDefaultRequest();
    ret = NetConnService::GetInstance()->IsSupplierMatchRequestAndNetwork(supplier);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, SystemReadyTest001, TestSize.Level1)
{
    if (!NetConnService::GetInstance()->registerToService_) {
        NetConnService::GetInstance()->state_ = NetConnService::STATE_RUNNING;
    }

    NetConnService::GetInstance()->OnStart();
    int32_t ret = NetConnService::GetInstance()->SystemReady();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetInternetPermissionTest001, TestSize.Level1)
{
    uint8_t allow = 1;
    int32_t ret = NetConnService::GetInstance()->SetInternetPermission(TEST_UID, allow);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetDefaultNetTest000, TestSize.Level1)
{
    int32_t netId = 0;
    auto ret = NetConnService::GetInstance()->GetDefaultNet(netId);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegisterNetSupplierTest001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_MMS);
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    int32_t ret = NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_DEFAULT, TEST_IDENT,
        netCaps, g_supplierId);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);

    ret = NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_ETHERNET, TEST_IDENT,
        netCaps, g_supplierId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_VPN, TEST_IDENT,
        netCaps, g_vpnSupplierId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegisterNetSupplierTest002, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNAL_DEFAULT);
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    auto ret = NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_CELLULAR, TEST_IDENT,
        netCaps, g_supplierId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, CmdCloseSocketsUid001, TestSize.Level1)
{
    int32_t netId = 100;
    uint32_t uid = 20020157;
    int32_t ret = NetConnService::GetInstance()->CloseSocketsUid(netId, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegisterNetSupplierCallbackTest001, TestSize.Level1)
{
    sptr<INetSupplierCallback> callback = new (std::nothrow) NetSupplierCallbackStubTestCb();
    ASSERT_NE(callback, nullptr);
    std::set<NetCap> netCaps;
    auto ret = NetConnService::GetInstance()->RegisterNetSupplierCallback(g_supplierId, callback);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
}

HWTEST_F(NetConnServiceTest, UpdateNetSupplierInfoTest001, TestSize.Level1)
{
    sptr<NetSupplierInfo> netSupplierInfo = nullptr;
    auto ret = NetConnService::GetInstance()->UpdateNetSupplierInfo(g_supplierId, netSupplierInfo);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    netSupplierInfo = new (std::nothrow) NetSupplierInfo();
    ASSERT_NE(netSupplierInfo, nullptr);
    ret = NetConnService::GetInstance()->UpdateNetSupplierInfo(TEST_NOTEXISTSUPPLIER, netSupplierInfo);
    EXPECT_EQ(ret, NET_CONN_ERR_NO_SUPPLIER);

    netSupplierInfo->isAvailable_ = true;
    netSupplierInfo->uid_ = TEST_UID;
    netSupplierInfo->ident_ = "0";
    netSupplierInfo->score_ = 90;
    ret = NetConnService::GetInstance()->UpdateNetSupplierInfo(g_vpnSupplierId, netSupplierInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(NetConnService::GetInstance()->FindNetSupplier(g_vpnSupplierId)->GetNetScore(), 90);

    netSupplierInfo->isAvailable_ = false;
    netSupplierInfo->ident_ = "";
    netSupplierInfo->score_ = 0;
    ret = NetConnService::GetInstance()->UpdateNetSupplierInfo(g_vpnSupplierId, netSupplierInfo);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
    EXPECT_EQ(NetConnService::GetInstance()->FindNetSupplier(g_vpnSupplierId)->GetNetScore(), 90);
}

HWTEST_F(NetConnServiceTest, UpdateNetLinkInfoTest001, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = nullptr;
    auto ret = NetConnService::GetInstance()->UpdateNetLinkInfo(g_supplierId, netLinkInfo);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    netLinkInfo = new (std::nothrow) NetLinkInfo();
    ret = NetConnService::GetInstance()->UpdateNetLinkInfo(TEST_NOTEXISTSUPPLIER, netLinkInfo);
    EXPECT_EQ(ret, NET_CONN_ERR_NO_SUPPLIER);

    ASSERT_NE(netLinkInfo, nullptr);
    netLinkInfo->httpProxy_.SetHost(TEST_HOST);
    ret = NetConnService::GetInstance()->UpdateNetLinkInfo(g_supplierId, netLinkInfo);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
}

HWTEST_F(NetConnServiceTest, UpdateNetLinkInfoTest002, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    netLinkInfo->ifaceName_ = "rmnet0";
    INetAddr netAddr;
    netAddr.type_ = INetAddr::IPV4;
    netAddr.hostName_ = "testHost";
    netLinkInfo->netAddrList_.push_back(netAddr);
    auto ret = NetConnService::GetInstance()->UpdateNetLinkInfo(g_supplierId, netLinkInfo);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
    ret = NetConnService::GetInstance()->IsIfaceNameInUse("rmnet0", 1);
    EXPECT_FALSE(ret);
    ret = NetConnService::GetInstance()->IsIfaceNameInUse("rmnet0", 100);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, RequestNetConnectionTest001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    netSpecifier->netCapabilities_.bearerTypes_.emplace(NetManagerStandard::BEARER_CELLULAR);
    netSpecifier->netCapabilities_.netCaps_.emplace(NetManagerStandard::NET_CAPABILITY_INTERNAL_DEFAULT);
    ASSERT_NE(netSpecifier, nullptr);
    auto ret = NetConnService::GetInstance()->RequestNetConnection(netSpecifier, g_callback, TEST_TIMEOUTMS);
    EXPECT_EQ(ret, NETSYS_SUCCESS);

    sptr<INetConnCallback> callback = nullptr;
    uint32_t timeoutMS = 0;
    sptr<NetSpecifier> invalidNetSpecifier = nullptr;
    ret = NetConnService::GetInstance()->RequestNetConnection(invalidNetSpecifier, callback, timeoutMS);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    NetConnService::RegisterType registerType = NetConnService::NetConnService::RegisterType::INVALIDTYPE;
    uint32_t reqId = 0;
    uint32_t uid = 0;
    NetConnService::GetInstance()->FindSameCallback(g_callback, reqId, registerType, uid);
    EXPECT_EQ(registerType, NetConnService::RegisterType::REQUEST);

    ret = NetConnService::GetInstance()->UnregisterNetConnCallback(g_callback);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RequestNetConnectionTest002, TestSize.Level1)
{
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    int64_t TEST_CALLBACK_UID = 1111;
    auto ret = -1;
    vector<sptr<INetConnCallback>> uidCallbacks;
    for (int32_t i = 1; i <= 2000; ++i) {
        sptr<INetConnCallback> uidCallback = new (std::nothrow) NetConnCallbackStubCb();
        ret = NetConnService::GetInstance()->RequestNetConnectionAsync(netSpecifier, uidCallback, 0,
                                                                                        TEST_CALLBACK_UID);
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
        uidCallbacks.push_back(uidCallback);
    }
    sptr<INetConnCallback> uidCallback = new (std::nothrow) NetConnCallbackStubCb();
    ret = NetConnService::GetInstance()->RequestNetConnectionAsync(netSpecifier, uidCallback, 0, TEST_CALLBACK_UID);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_OVER_MAX_REQUEST_NUM);
    for (auto& callback : uidCallbacks) {
        ret = NetConnService::GetInstance()->UnregisterNetConnCallbackAsync(callback, TEST_CALLBACK_UID);
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    }
}

HWTEST_F(NetConnServiceTest, RegisterNetConnCallbackTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->RegisterNetConnCallback(g_callback);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
}

HWTEST_F(NetConnServiceTest, UnregisterNetConnCallbackTest001, TestSize.Level1)
{
    sptr<INetConnCallback> netCallback = new (std::nothrow) NetConnCallbackStubCb();
    auto ret = NetConnService::GetInstance()->UnregisterNetConnCallback(netCallback);
    EXPECT_EQ(ret, NET_CONN_ERR_CALLBACK_NOT_FOUND);

    ret = NetConnService::GetInstance()->UnregisterNetConnCallback(g_callback);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegisterNetConnCallbackTest002, TestSize.Level1)
{
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    ASSERT_NE(netSpecifier, nullptr);
    auto ret = NetConnService::GetInstance()->RegisterNetConnCallback(netSpecifier, g_callback,
                                                                                        TEST_TIMEOUTMS);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegisterNetConnCallbackTest003, TestSize.Level1)
{
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    int64_t TEST_CALLBACK_UID = 1111;
    auto ret = -1;
    vector<sptr<INetConnCallback>> uidCallbacks;
    for (int32_t i = 1; i <= 2000; ++i) {
        sptr<INetConnCallback> uidCallback = new (std::nothrow) NetConnCallbackStubCb();
        ret = NetConnService::GetInstance()->RegisterNetConnCallbackAsync(netSpecifier, uidCallback, 0,
                                                                                        TEST_CALLBACK_UID);
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
        uidCallbacks.push_back(uidCallback);
    }
    sptr<INetConnCallback> uidCallback = new (std::nothrow) NetConnCallbackStubCb();
    ret = NetConnService::GetInstance()->RegisterNetConnCallbackAsync(netSpecifier, uidCallback, 0, TEST_CALLBACK_UID);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_OVER_MAX_REQUEST_NUM);
    for (auto& callback : uidCallbacks) {
        ret = NetConnService::GetInstance()->UnregisterNetConnCallbackAsync(callback, TEST_CALLBACK_UID);
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    }
}

HWTEST_F(NetConnServiceTest, RegisterNetConnCallbackTest004, TestSize.Level1)
{
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    int64_t TEST_CALLBACK_UID = 1111;
    auto ret = -1;
    system::SetParameter("persist.edm.mms_disable", "true");
    vector<sptr<INetConnCallback>> uidCallbacks;
    for (int32_t i = 1; i <= 2000; ++i) {
        sptr<INetConnCallback> uidCallback = new (std::nothrow) NetConnCallbackStubCb();
        ret = NetConnService::GetInstance()->RegisterNetConnCallbackAsync(netSpecifier, uidCallback, 0,
                                                                                        TEST_CALLBACK_UID);
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
        uidCallbacks.push_back(uidCallback);
    }
    sptr<INetConnCallback> uidCallback = new (std::nothrow) NetConnCallbackStubCb();
    ret = NetConnService::GetInstance()->RegisterNetConnCallbackAsync(netSpecifier, uidCallback, 0, TEST_CALLBACK_UID);
    OHOS::system::SetParameter("persist.edm.mms_disable", "false");
    EXPECT_EQ(ret, NET_CONN_ERR_NET_OVER_MAX_REQUEST_NUM);
    for (auto& callback : uidCallbacks) {
        ret = NetConnService::GetInstance()->UnregisterNetConnCallbackAsync(callback, TEST_CALLBACK_UID);
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    }
}

HWTEST_F(NetConnServiceTest, RegisterNetDetectionCallbackTest001, TestSize.Level1)
{
    sptr<INetDetectionCallback> callback_ = nullptr;
    auto ret = NetConnService::GetInstance()->RegisterNetDetectionCallback(TEST_NETID, callback_);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = NetConnService::GetInstance()->RegisterNetDetectionCallback(TEST_NETID, g_detectionCallback);
    EXPECT_EQ(ret, NET_CONN_ERR_NETID_NOT_FOUND);

    ret = NetConnService::GetInstance()->RegisterNetDetectionCallback(MIN_NET_ID, g_detectionCallback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, UnRegisterNetDetectionCallbackTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->UnRegisterNetDetectionCallback(MIN_NET_ID, g_detectionCallback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, UpdateNetStateForTestTest001, TestSize.Level1)
{
    int32_t netState = 0;
    sptr<NetSpecifier> netSpecifier = nullptr;
    auto ret = NetConnService::GetInstance()->UpdateNetStateForTest(netSpecifier, netState);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    netSpecifier = new (std::nothrow) NetSpecifier();
    ret = NetConnService::GetInstance()->UpdateNetStateForTest(netSpecifier, netState);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

#ifdef SUPPORT_SYSVPN
HWTEST_F(NetConnServiceTest, IsCallingUserSupplierTest001, TestSize.Level1)
{
    uint32_t supplierId = 100400;
    auto ret = NetConnService::GetInstance()->IsCallingUserSupplier(supplierId);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, IsCallingUserSupplierTest002, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    int32_t regRet = NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_VPN, TEST_IDENT,
        netCaps, g_vpnSupplierId);
    EXPECT_EQ(regRet, NETMANAGER_SUCCESS);
    auto ret = NetConnService::GetInstance()->IsCallingUserSupplier(g_vpnSupplierId);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceTest, IsCallingUserSupplierTest003, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    int32_t regRet = NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_VPN, TEST_IDENT,
        netCaps, g_vpnSupplierId);
    EXPECT_EQ(regRet, NETMANAGER_SUCCESS);
    auto supplier = NetConnService::GetInstance()->FindNetSupplier(g_vpnSupplierId);

    sptr<NetSupplierInfo> netSupplierInfo = new (std::nothrow) NetSupplierInfo();
    netSupplierInfo->uid_ = TEST_UID;
    NetConnService::GetInstance()->UpdateNetSupplierInfo(g_vpnSupplierId, netSupplierInfo);
    auto ret = NetConnService::GetInstance()->IsCallingUserSupplier(g_vpnSupplierId);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, IsCallingUserSupplierTest004, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    int32_t regRet = NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_VPN, TEST_IDENT,
        netCaps, g_vpnSupplierId);
    EXPECT_EQ(regRet, NETMANAGER_SUCCESS);

    auto supplier = NetConnService::GetInstance()->FindNetSupplier(g_vpnSupplierId);

    sptr<NetSupplierInfo> netSupplierInfo = new (std::nothrow) NetSupplierInfo();
    netSupplierInfo->uid_ = -1;
    NetConnService::GetInstance()->UpdateNetSupplierInfo(g_vpnSupplierId, netSupplierInfo);

    std::list<int32_t> netIdList;
    auto ret1 = NetConnService::GetInstance()->GetAllNets(netIdList);
    EXPECT_EQ(ret1, NETMANAGER_SUCCESS);

    auto ret2 = NetConnService::GetInstance()->IsCallingUserSupplier(g_vpnSupplierId);
    EXPECT_FALSE(ret2);
}

HWTEST_F(NetConnServiceTest, IsCallingUserSupplierTest005, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    int32_t regRet = NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_VPN, TEST_IDENT,
        netCaps, g_vpnSupplierId);
    EXPECT_EQ(regRet, NETMANAGER_SUCCESS);

    auto supplier = NetConnService::GetInstance()->FindNetSupplier(g_vpnSupplierId);

    sptr<NetSupplierInfo> netSupplierInfo = new (std::nothrow) NetSupplierInfo();
    netSupplierInfo->uid_ = TEST_UID;
    NetConnService::GetInstance()->UpdateNetSupplierInfo(g_vpnSupplierId, netSupplierInfo);

    std::list<int32_t> netIdList;
    auto ret1 = NetConnService::GetInstance()->GetAllNets(netIdList);
    EXPECT_EQ(ret1, NETMANAGER_SUCCESS);

    auto ret2 = NetConnService::GetInstance()->IsCallingUserSupplier(g_vpnSupplierId);
    EXPECT_FALSE(ret2);
}
#endif // SUPPORT_SYSVPN

HWTEST_F(NetConnServiceTest, GetAllNetsTest001, TestSize.Level1)
{
    std::list<int32_t> netIdList;
    auto ret = NetConnService::GetInstance()->GetAllNets(netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetSpecificUidNetTest001, TestSize.Level1)
{
    int32_t defaultNetId = 5;
    auto ret = NetConnService::GetInstance()->GetDefaultNet(defaultNetId);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_NE(defaultNetId, 0);

    std::list<int32_t> netIdList;
    ret = NetConnService::GetInstance()->GetSpecificNet(BEARER_VPN, netIdList);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_GE(netIdList.size(), 0);

    int32_t netID = 0;
    NetConnService::GetInstance()->GetSpecificUidNet(TEST_NOTEXISTSUPPLIER, netID);
    EXPECT_EQ(netID, defaultNetId);

    NetConnService::GetInstance()->GetSpecificUidNet(TEST_UID, netID);
    EXPECT_EQ(netID, *netIdList.begin());
}

HWTEST_F(NetConnServiceTest, GetConnectionPropertiesTest001, TestSize.Level1)
{
    NetLinkInfo info;
    auto ret = NetConnService::GetInstance()->GetConnectionProperties(TEST_NETID, info);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);

    int32_t defaultNetId = 0;
    ret = NetConnService::GetInstance()->GetDefaultNet(defaultNetId);

    ret = NetConnService::GetInstance()->GetConnectionProperties(defaultNetId, info);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetNetExtAttributeTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->SetNetExtAttribute(TEST_NETID, "text");
    ASSERT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceTest, SetNetExtAttributeTest002, TestSize.Level1)
{
    std::string str(10241, 'c');
    int32_t defaultNetId = 0;
    NetConnService::GetInstance()->GetDefaultNet(defaultNetId);
    auto ret = NetConnService::GetInstance()->SetNetExtAttribute(defaultNetId, str);
    ASSERT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetConnServiceTest, SetNetExtAttributeTest003, TestSize.Level1)
{
    int32_t defaultNetId = 0;
    NetConnService::GetInstance()->GetDefaultNet(defaultNetId);
    auto ret = NetConnService::GetInstance()->SetNetExtAttribute(defaultNetId, "test");
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetNetExtAttributeTest001, TestSize.Level1)
{
    std::string str;
    auto ret = NetConnService::GetInstance()->GetNetExtAttribute(TEST_NETID, str);
    ASSERT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceTest, GetNetExtAttributeTest002, TestSize.Level1)
{
    int32_t defaultNetId = 0;
    NetConnService::GetInstance()->GetDefaultNet(defaultNetId);
    NetConnService::GetInstance()->SetNetExtAttribute(defaultNetId, "test");
    std::string str;
    auto ret = NetConnService::GetInstance()->GetNetExtAttribute(defaultNetId, str);
    ASSERT_EQ(str, "test");
}

HWTEST_F(NetConnServiceTest, NetDetectionTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->NetDetection(TEST_NETID);
    EXPECT_EQ(ret, NET_CONN_ERR_NETID_NOT_FOUND);

    ret = NetConnService::GetInstance()->NetDetection(MIN_NET_ID);
    EXPECT_EQ(ret, NET_CONN_ERR_NETID_NOT_FOUND);
}

HWTEST_F(NetConnServiceTest, NetDetectionTest002, TestSize.Level1)
{
    std::string rawUrl = "";
    PortalResponse resp;
    auto ret = NetConnService::GetInstance()->NetDetection(rawUrl, resp);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetConnServiceTest, GetNetIdByIdentifierTest001, TestSize.Level1)
{
    std::list<int32_t> netIdList;
    auto ret = NetConnService::GetInstance()->GetNetIdByIdentifier("", netIdList);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    ret = NetConnService::GetInstance()->GetNetIdByIdentifier(TEST_IDENT, netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetDefaultNetTest001, TestSize.Level1)
{
    int32_t netId = 0;
    auto ret = NetConnService::GetInstance()->GetDefaultNet(netId);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, HasDefaultNetTest001, TestSize.Level1)
{
    bool bFlag = false;
    auto ret = NetConnService::GetInstance()->HasDefaultNet(bFlag);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(bFlag);

    ret = DelayedSingleton<NetConnService>::GetInstance()->HasDefaultNet(bFlag);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_FALSE(bFlag);
}

HWTEST_F(NetConnServiceTest, GetNetCapabilitiesTest001, TestSize.Level1)
{
    int32_t netId = 0;
    int32_t ret = NetConnService::GetInstance()->GetDefaultNet(netId);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);

    NetAllCapabilities netAllCap;
    ret = NetConnService::GetInstance()->GetNetCapabilities(TEST_NETID, netAllCap);
    ASSERT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);

    ret = NetConnService::GetInstance()->GetNetCapabilities(netId, netAllCap);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetAirplaneModeTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->SetAirplaneMode(true);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetAirplaneModeTest002, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->SetAirplaneMode(false);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetAirplaneModeTest004, TestSize.Level1)
{
    system::SetParameter("persist.edm.airplane_mode_disable", "true");
    auto ret = NetConnService::GetInstance()->SetAirplaneMode(true);
    ASSERT_NE(ret, NETMANAGER_SUCCESS);
    ret = NetConnService::GetInstance()->SetAirplaneMode(false);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    system::SetParameter("persist.edm.airplane_mode_disable", "false");
    ret = NetConnService::GetInstance()->SetAirplaneMode(true);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = NetConnService::GetInstance()->SetAirplaneMode(false);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, IsDefaultNetMeteredTest001, TestSize.Level1)
{
    bool bRes = false;
    auto ret = NetConnService::GetInstance()->IsDefaultNetMetered(bRes);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);

    ret = DelayedSingleton<NetConnService>::GetInstance()->IsDefaultNetMetered(bRes);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(bRes);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest001, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_PROXY_HOST, 0, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest002, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN1, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest003, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN2, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest004, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN3, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest005, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN4, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest006, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN5, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest007, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN6, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest008, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN7, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest009, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN8, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest010, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN9, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest011, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_DOMAIN10, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest012, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_IPV4_ADDR, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest013, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_IPV6_ADDR, 8080, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest014, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_LONG_HOST, 8080, {TEST_LONG_EXCLUSION_LIST}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest015, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_IPV4_ADDR, 8080, {}};
    // user is existed, so return succ.
    httpProxy.SetUserId(MAIN_USERID);
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest016, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_LONG_HOST, 8080, {TEST_LONG_EXCLUSION_LIST}};
    // user is not existed, so return error.
    httpProxy.SetUserId(INVALID_USERID);
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_NE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest017, TestSize.Level1)
{
    HttpProxy httpProxy = {"", 0, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest018, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_PROXY_HOST, 0, {}};
    auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest019, TestSize.Level1)
{
    int32_t userId;
    int32_t ret = NetConnService::GetInstance()->GetActiveUserId(userId);
    if (ret == NETMANAGER_SUCCESS) {
        HttpProxy httpProxy = {"", 0, {}};
        auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
        ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    }
}

HWTEST_F(NetConnServiceTest, SetGlobalHttpProxyTest020, TestSize.Level1)
{
    int32_t userId;
    int32_t ret = NetConnService::GetInstance()->GetActiveUserId(userId);
    if (ret == NETMANAGER_SUCCESS) {
        HttpProxy httpProxy = {TEST_PROXY_HOST, 0, {}};
        auto ret = NetConnService::GetInstance()->SetGlobalHttpProxy(httpProxy);
        ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    }
}

HWTEST_F(NetConnServiceTest, GetGlobalHttpProxyTest001, TestSize.Level1)
{
    HttpProxy getGlobalHttpProxy;
    int32_t ret = NetConnService::GetInstance()->GetGlobalHttpProxy(getGlobalHttpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetGlobalHttpProxyTest002, TestSize.Level1)
{
    HttpProxy getGlobalHttpProxy;
    getGlobalHttpProxy.SetUserId(MAIN_USERID);
    int32_t ret = NetConnService::GetInstance()->GetGlobalHttpProxy(getGlobalHttpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetGlobalHttpProxyTest003, TestSize.Level1)
{
    HttpProxy getGlobalHttpProxy;
    getGlobalHttpProxy.SetUserId(INVALID_USERID);
    int32_t ret = NetConnService::GetInstance()->GetGlobalHttpProxy(getGlobalHttpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ASSERT_TRUE(getGlobalHttpProxy.GetHost().empty());
}

HWTEST_F(NetConnServiceTest, GetDefaultHttpProxyTest001, TestSize.Level1)
{
    int32_t bindNetId = 0;
    HttpProxy defaultHttpProxy;
    int32_t ret = NetConnService::GetInstance()->GetDefaultHttpProxy(bindNetId, defaultHttpProxy);
    ASSERT_TRUE(ret == NET_CONN_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetDefaultHttpProxyTest002, TestSize.Level1)
{
    int32_t bindNetId = NET_ID;
    HttpProxy defaultHttpProxy;
    int32_t ret = NetConnService::GetInstance()->GetDefaultHttpProxy(bindNetId, defaultHttpProxy);
    ASSERT_TRUE(ret == NET_CONN_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetDefaultHttpProxyTest004, TestSize.Level1)
{
    int32_t bindNetId = NET_ID;
    HttpProxy defaultHttpProxy;
    defaultHttpProxy.SetUserId(MAIN_USERID);
    int32_t ret = NetConnService::GetInstance()->GetDefaultHttpProxy(bindNetId, defaultHttpProxy);
    ASSERT_TRUE(ret == NET_CONN_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetDefaultHttpProxyTest005, TestSize.Level1)
{
    int32_t bindNetId = NET_ID;
    HttpProxy defaultHttpProxy;
    defaultHttpProxy.SetUserId(INVALID_USERID);
    int32_t ret = NetConnService::GetInstance()->GetDefaultHttpProxy(bindNetId, defaultHttpProxy);
    ASSERT_TRUE(ret == NET_CONN_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetLocalUserIdTest001, TestSize.Level1)
{
    int32_t userId;
    int32_t ret = NetConnService::GetInstance()->GetLocalUserId(userId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RefreshGlobalHttpProxyTest001
 * @tc.desc: Test NetConnService RefreshGlobalHttpProxy with empty host.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceTest, RefreshGlobalHttpProxyTest001, TestSize.Level1)
{
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    int32_t ret = NetConnService::GetInstance()->RefreshGlobalHttpProxy(callback);
    EXPECT_NE(ret, NET_CONN_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RefreshGlobalHttpProxyTest002, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    int32_t userId = 0;
    service->GetActiveUserId(userId);
    HttpProxy cachedProxy;
    cachedProxy.SetHost(TEST_PROXY_HOST);
    cachedProxy.SetPort(8080);
    SecureData userName;
    userName.append(PROXY_NAME, PROXY_NAME_SIZE);
    cachedProxy.SetUserName(userName);
    cachedProxy.SetUserId(userId);
    service->globalHttpProxyCache_.EnsureInsert(userId, cachedProxy);
    service->httpProxyThreadNeedRun_ = true;

    service->refreshInProgress_ = true;
    service->lastRefreshTime_ = std::chrono::steady_clock::now() - std::chrono::seconds(100);
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    auto ret = service->RefreshGlobalHttpProxy(callback);
    service->httpProxyThreadNeedRun_ = false;
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RefreshGlobalHttpProxyTest003, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    int32_t userId = 0;
    service->GetActiveUserId(userId);
    HttpProxy cachedProxy;
    cachedProxy.SetHost(TEST_PROXY_HOST);
    cachedProxy.SetPort(8080);
    SecureData userName;
    userName.append(PROXY_NAME, PROXY_NAME_SIZE);
    cachedProxy.SetUserName(userName);
    cachedProxy.SetUserId(userId);
    service->globalHttpProxyCache_.EnsureInsert(userId, cachedProxy);
    service->httpProxyThreadNeedRun_ = true;

    service->refreshInProgress_ = false;
    service->lastRefreshTime_ = std::chrono::steady_clock::now() - std::chrono::seconds(100);
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    auto ret = service->RefreshGlobalHttpProxy(callback);
    service->httpProxyThreadNeedRun_ = false;
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RefreshGlobalHttpProxyTest004, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    int32_t userId = 0;
    service->GetActiveUserId(userId);
    HttpProxy cachedProxy;
    cachedProxy.SetHost(TEST_PROXY_HOST);
    cachedProxy.SetPort(8080);
    SecureData userName;
    userName.append(PROXY_NAME, PROXY_NAME_SIZE);
    cachedProxy.SetUserName(userName);
    cachedProxy.SetUserId(userId);
    service->globalHttpProxyCache_.EnsureInsert(userId, cachedProxy);
    service->httpProxyThreadNeedRun_ = true;

    service->refreshInProgress_ = false;
    service->lastRefreshTime_ = std::chrono::steady_clock::now();
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    auto ret = service->RefreshGlobalHttpProxy(callback);
    service->httpProxyThreadNeedRun_ = false;
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceTest, IsRefreshRateLimitedTest001, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->lastRefreshTime_ = std::chrono::steady_clock::now();
    service->lastRefreshProxy_.SetHost(TEST_PROXY_HOST);
    service->lastRefreshProxy_.SetPort(8080);
    HttpProxy currentProxy;
    currentProxy.SetHost(TEST_PROXY_HOST);
    currentProxy.SetPort(8080);
    bool limited = service->IsRefreshRateLimited(currentProxy);
    EXPECT_TRUE(limited);
}

HWTEST_F(NetConnServiceTest, IsRefreshRateLimitedTest002, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->lastRefreshTime_ = std::chrono::steady_clock::now() - std::chrono::seconds(20);
    service->lastRefreshProxy_.SetHost(TEST_PROXY_HOST);
    service->lastRefreshProxy_.SetPort(8080);
    HttpProxy currentProxy;
    currentProxy.SetHost(TEST_PROXY_HOST);
    currentProxy.SetPort(8080);
    bool limited = service->IsRefreshRateLimited(currentProxy);
    EXPECT_FALSE(limited);
}

HWTEST_F(NetConnServiceTest, IsRefreshRateLimitedTest003, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->lastRefreshTime_ = std::chrono::steady_clock::now();
    service->lastRefreshProxy_.SetHost(TEST_PROXY_HOST);
    service->lastRefreshProxy_.SetPort(8080);
    HttpProxy currentProxy;
    currentProxy.SetHost("different_host");
    currentProxy.SetPort(8080);
    bool limited = service->IsRefreshRateLimited(currentProxy);
    EXPECT_FALSE(limited);
}

HWTEST_F(NetConnServiceTest, IsRefreshRateLimitedTest004, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = true;
    service->refreshResultReady_ = false;
    std::thread setter([&service]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        service->NotifyRefreshGlobalHttpProxyResult(SUCCESS_CODE);
    });
    HttpProxy currentProxy;
    currentProxy.SetHost(TEST_PROXY_HOST);
    currentProxy.SetPort(8080);
    HttpProxy httpProxy;
    service->ExecuteRefreshInFfrt(currentProxy);
    setter.join();
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
}

HWTEST_F(NetConnServiceTest, IsRefreshRateLimitedTest005, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = true;
    service->refreshResultReady_ = false;
    std::thread setter([&service]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        service->NotifyRefreshGlobalHttpProxyResult(0);
    });
    HttpProxy currentProxy;
    currentProxy.SetHost(TEST_PROXY_HOST);
    currentProxy.SetPort(8080);
    HttpProxy httpProxy;
    service->ExecuteRefreshInFfrt(currentProxy);
    setter.join();
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
}
    
HWTEST_F(NetConnServiceTest, IsRefreshRateLimitedTest006, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = false;
    service->lastRefreshTime_ = std::chrono::steady_clock::now() - std::chrono::seconds(20);
    service->lastRefreshProxy_.SetHost(TEST_PROXY_HOST);
    service->lastRefreshProxy_.SetPort(8080);
    service->httpProxyThreadNeedRun_ = true;
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    int32_t ret = service->RefreshGlobalHttpProxy(callback);
    service->httpProxyThreadNeedRun_ = false;
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, PerformProxyCurlProbeTest001, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    long code = service->PerformProxyCurlProbe(nullptr);
    EXPECT_EQ(code, 0);
}

HWTEST_F(NetConnServiceTest, PerformProxyCurlProbeTest002, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    CURL *curl = nullptr;
    curl = curl_easy_init();
    long code = service->PerformProxyCurlProbe(curl);
    curl_easy_cleanup(curl);
    EXPECT_NE(code, SUCCESS_CODE);
}

HWTEST_F(NetConnServiceTest, NotifyRefreshGlobalHttpProxyResultTest001, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = false;
    service->NotifyRefreshGlobalHttpProxyResult(SUCCESS_CODE);
    EXPECT_FALSE(service->refreshResultReady_);
}

HWTEST_F(NetConnServiceTest, NotifyRefreshGlobalHttpProxyResultTest002, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = true;
    service->refreshResultReady_ = false;
    service->NotifyRefreshGlobalHttpProxyResult(SUCCESS_CODE);
    EXPECT_TRUE(service->refreshAuthSuccess_);
    EXPECT_TRUE(service->refreshResultReady_);
    service->refreshInProgress_ = false;
    service->refreshResultReady_ = false;
}

HWTEST_F(NetConnServiceTest, NotifyRefreshGlobalHttpProxyResultTest003, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = true;
    service->refreshResultReady_ = false;
    service->NotifyRefreshGlobalHttpProxyResult(0);
    EXPECT_FALSE(service->refreshAuthSuccess_);
    EXPECT_TRUE(service->refreshResultReady_);
    service->refreshInProgress_ = false;
    service->refreshResultReady_ = false;
}

HWTEST_F(NetConnServiceTest, WaitForNextActiveCycleTest001, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->httpProxyThreadNeedRun_ = false;
    uint32_t retryTimes = RETRY_TIMES;
    service->WaitForNextActiveCycle(retryTimes, 0);
    service->httpProxyThreadNeedRun_ = true;
    EXPECT_EQ(retryTimes, RETRY_TIMES);
}

HWTEST_F(NetConnServiceTest, WaitForNextActiveCycleTest002, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->httpProxyThreadNeedRun_ = true;
    uint32_t retryTimes = RETRY_TIMES;
    service->WaitForNextActiveCycle(retryTimes, 0);
    service->httpProxyThreadNeedRun_ = false;
    EXPECT_EQ(retryTimes, RETRY_TIMES - 1);
}

HWTEST_F(NetConnServiceTest, WaitForNextActiveCycleTest003, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->httpProxyThreadNeedRun_ = false;
    service->httpProxyThreadCv_.notify_all();
    uint32_t retryTimes = 0;
    std::thread notifier([&service]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        service->httpProxyThreadCv_.notify_all();
    });
    service->httpProxyThreadNeedRun_ = true;
    service->WaitForNextActiveCycle(retryTimes, 0);
    service->httpProxyThreadNeedRun_ = false;
    notifier.join();
    EXPECT_EQ(retryTimes, RETRY_TIMES);
}

HWTEST_F(NetConnServiceTest, WaitForNextActiveCycleTest004, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->httpProxyThreadNeedRun_ = false;
    service->httpProxyThreadCv_.notify_all();
    uint32_t retryTimes = RETRY_TIMES;
    std::thread notifier([&service]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        service->httpProxyThreadCv_.notify_all();
    });
    service->httpProxyThreadNeedRun_ = true;
    service->WaitForNextActiveCycle(retryTimes, SUCCESS_CODE);
    service->httpProxyThreadNeedRun_ = false;
    notifier.join();
    EXPECT_EQ(retryTimes, RETRY_TIMES);
}

HWTEST_F(NetConnServiceTest, PrepareRefreshGlobalHttpProxyTest001, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->lastRefreshTime_ = std::chrono::steady_clock::now();
    service->lastRefreshProxy_.SetHost(TEST_PROXY_HOST);
    service->lastRefreshProxy_.SetPort(8080);
    service->refreshInProgress_ = false;
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    HttpProxy currentProxy;
    currentProxy.SetHost(TEST_PROXY_HOST);
    currentProxy.SetPort(8080);
    auto ret = service->PrepareRefreshGlobalHttpProxy(currentProxy, callback);
    EXPECT_EQ(ret, NET_CONN_ERR_HTTP_PROXY_INVALID);
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
}

HWTEST_F(NetConnServiceTest, PrepareRefreshGlobalHttpProxyTest002, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = true;
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    HttpProxy currentProxy;
    currentProxy.SetHost("different_host");
    currentProxy.SetPort(9090);
    auto ret = service->PrepareRefreshGlobalHttpProxy(currentProxy, callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
}

HWTEST_F(NetConnServiceTest, PrepareRefreshGlobalHttpProxyTest003, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = false;
    service->lastRefreshTime_ = std::chrono::steady_clock::now() - std::chrono::seconds(20);
    service->lastRefreshProxy_.SetHost(TEST_PROXY_HOST);
    service->lastRefreshProxy_.SetPort(8080);
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    HttpProxy currentProxy;
    currentProxy.SetHost(TEST_PROXY_HOST);
    currentProxy.SetPort(8080);
    auto ret = service->PrepareRefreshGlobalHttpProxy(currentProxy, callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(service->refreshInProgress_);
    service->refreshInProgress_ = false;
    service->refreshCallbacks_.clear();
}

HWTEST_F(NetConnServiceTest, ExecuteRefreshInFfrtTimeoutTest, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    service->refreshInProgress_ = true;
    service->refreshResultReady_ = false;
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    service->refreshCallbacks_.push_back(callback);
    HttpProxy currentProxy;
    currentProxy.SetHost(TEST_PROXY_HOST);
    currentProxy.SetPort(8080);
    service->ExecuteRefreshInFfrt(currentProxy);
    EXPECT_FALSE(service->refreshInProgress_);
    service->refreshCallbacks_.clear();
}

HWTEST_F(NetConnServiceTest, LoadCurrentProxyForRefreshTest001, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    int32_t userId = 0;
    service->GetActiveUserId(userId);
    HttpProxy cachedProxy;
    cachedProxy.SetHost("");
    HttpProxy currentProxy;
    service->globalHttpProxyCache_.EnsureInsert(userId, cachedProxy);
    auto ret = service->LoadCurrentProxyForRefresh(currentProxy);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceTest, LoadCurrentProxyForRefreshTest002, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    int32_t userId = 0;
    service->GetActiveUserId(userId);
    HttpProxy cachedProxy;
    cachedProxy.SetHost(TEST_PROXY_HOST);
    cachedProxy.SetPort(8080);
    HttpProxy currentProxy;
    service->globalHttpProxyCache_.EnsureInsert(userId, cachedProxy);
    auto ret = service->LoadCurrentProxyForRefresh(currentProxy);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceTest, LoadCurrentProxyForRefreshTest003, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    int32_t userId = 0;
    service->GetActiveUserId(userId);
    HttpProxy cachedProxy;
    cachedProxy.SetHost(TEST_PROXY_HOST);
    cachedProxy.SetPort(8080);
    SecureData userName;
    userName.append(PROXY_NAME, PROXY_NAME_SIZE);
    cachedProxy.SetUserName(userName);
    cachedProxy.SetUserId(userId);
    service->globalHttpProxyCache_.EnsureInsert(userId, cachedProxy);
    service->httpProxyThreadNeedRun_ = false;
    HttpProxy currentProxy;
    auto ret = service->LoadCurrentProxyForRefresh(currentProxy);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceTest, LoadCurrentProxyForRefreshTest004, TestSize.Level1)
{
    auto &service = NetConnService::GetInstance();
    int32_t userId = 0;
    service->GetActiveUserId(userId);
    HttpProxy cachedProxy;
    cachedProxy.SetHost(TEST_PROXY_HOST);
    cachedProxy.SetPort(8080);
    SecureData userName;
    userName.append(PROXY_NAME, PROXY_NAME_SIZE);
    cachedProxy.SetUserName(userName);
    cachedProxy.SetUserId(userId);
    service->globalHttpProxyCache_.EnsureInsert(userId, cachedProxy);
    service->httpProxyThreadNeedRun_ = true;
    HttpProxy currentProxy;
    auto ret = service->LoadCurrentProxyForRefresh(currentProxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    service->httpProxyThreadNeedRun_ = false;
}

HWTEST_F(NetConnServiceTest, GetActiveUserIdTest001, TestSize.Level1)
{
    int32_t userId;
    int32_t ret = NetConnService::GetInstance()->GetActiveUserId(userId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetTest001, TestSize.Level1)
{
    std::list<int32_t> netIdList;
    int32_t ret = NetConnService::GetInstance()->GetSpecificNet(BEARER_CELLULAR, netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = NetConnService::GetInstance()->GetSpecificNet(BEARER_DEFAULT, netIdList);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);

    ret = NetConnService::GetInstance()->GetSpecificNetByIdent(BEARER_CELLULAR, "test", netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = NetConnService::GetInstance()->GetSpecificNetByIdent(BEARER_DEFAULT, "test", netIdList);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);

    ret = NetConnService::GetInstance()->RestrictBackgroundChanged(false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = NetConnService::GetInstance()->RestrictBackgroundChanged(false);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_NO_RESTRICT_BACKGROUND);

    NetConnService::GetInstance()->HandleDetectionResult(TEST_NOTEXISTSUPPLIER, VERIFICATION_STATE);
    NetConnService::GetInstance()->HandleDetectionResult(g_supplierId, VERIFICATION_STATE);

    std::vector<std::u16string> args;
    args.emplace_back(u"dummy data");
    ret = NetConnService::GetInstance()->Dump(SOCKET_FD, args);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = DelayedSingleton<NetConnService>::GetInstance()->Dump(SOCKET_FD, args);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetIfaceNamesTest001, TestSize.Level1)
{
    std::list<std::string> ifaceNames;
    auto ret = NetConnService::GetInstance()->GetIfaceNames(BEARER_DEFAULT, ifaceNames);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);

    ret = NetConnService::GetInstance()->GetIfaceNames(BEARER_VPN, ifaceNames);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetIfaceNameByTypeTest001, TestSize.Level1)
{
    std::string ifaceName;
    auto ret = NetConnService::GetInstance()->GetIfaceNameByType(BEARER_DEFAULT, TEST_IDENT, ifaceName);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);

    ret = NetConnService::GetInstance()->GetIfaceNameByType(BEARER_BLUETOOTH, TEST_IDENT, ifaceName);
    EXPECT_EQ(ret, NET_CONN_ERR_NO_SUPPLIER);

    ret = NetConnService::GetInstance()->GetIfaceNameByType(BEARER_VPN, TEST_IDENT, ifaceName);
    EXPECT_TRUE(ret == NET_CONN_ERR_NO_SUPPLIER || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetIfaceNameIdentMapsTest001, TestSize.Level1)
{
    SafeMap<std::string, std::string> data;
    auto ret = NetConnService::GetInstance()->GetIfaceNameIdentMaps(BEARER_CELLULAR, data);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetAppNetTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->SetAppNet(TEST_NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegisterNetInterfaceCallbackTest001, TestSize.Level1)
{
    sptr<INetInterfaceStateCallback> callback = nullptr;
    auto ret = NetConnService::GetInstance()->RegisterNetInterfaceCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    ret = NetConnService::GetInstance()->UnregisterNetInterfaceCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    ret = NetConnService::GetInstance()->RegisterNetInterfaceCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    ret = NetConnService::GetInstance()->UnregisterNetInterfaceCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    NetConnService::GetInstance()->interfaceStateCallback_ =
        new (std::nothrow) NetConnService::NetInterfaceStateCallback();
    ASSERT_NE(NetConnService::GetInstance()->interfaceStateCallback_, nullptr);
    NetsysController::GetInstance().RegisterCallback(NetConnService::GetInstance()->interfaceStateCallback_);

    ret = NetConnService::GetInstance()->RegisterNetInterfaceCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = NetConnService::GetInstance()->UnregisterNetInterfaceCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetNetInterfaceConfigurationTest001, TestSize.Level1)
{
    NetInterfaceConfiguration config;
    auto ret = NetConnService::GetInstance()->GetNetInterfaceConfiguration("wlan0", config);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, AddNetworkRouteTest001, TestSize.Level1)
{
    int32_t netId = 10;
    std::string ifName = "wlan0";
    std::string destination = "0.0.0.0/0";
    std::string nextHop = "0.0.0.1234";
    int32_t ret = NetConnService::GetInstance()->AddNetworkRoute(netId, ifName, destination, nextHop);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, RemoveNetworkRouteTest001, TestSize.Level1)
{
    int32_t netId = 10;
    std::string ifName = "wlan0";
    std::string destination = "0.0.0.0/0";
    std::string nextHop = "0.0.0.1234";
    int32_t ret = NetConnService::GetInstance()->RemoveNetworkRoute(netId, ifName, destination, nextHop);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, AddInterfaceAddressTest001, TestSize.Level1)
{
    std::string ifName = "wlan0";
    std::string ipAddr = "0.0.0.1";
    int32_t prefixLength = 23;
    int32_t ret = NetConnService::GetInstance()->AddInterfaceAddress(ifName, ipAddr, prefixLength);
    EXPECT_NE(NetsysController::GetInstance().netsysService_, nullptr);
}

HWTEST_F(NetConnServiceTest, DelInterfaceAddressTest001, TestSize.Level1)
{
    std::string ifName = "wlan0";
    std::string ipAddr = "0.0.0.1";
    int32_t prefixLength = 23;
    int32_t ret = NetConnService::GetInstance()->DelInterfaceAddress(ifName, ipAddr, prefixLength);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, AddStaticArpTest001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    int32_t ret = NetConnService::GetInstance()->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, DelStaticArpTest001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    int32_t ret = NetConnService::GetInstance()->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, AddStaticIpv6AddrTest001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    int32_t ret = NetConnService::GetInstance()->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, DelStaticIpv6AddrTest001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    int32_t ret = NetConnService::GetInstance()->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, NetConnServiceBranchTest001, TestSize.Level1)
{
    NetConnService::GetInstance()->OnStart();
    EXPECT_EQ(NetConnService::GetInstance()->state_, NetConnService::STATE_RUNNING);
    NetConnService::GetInstance()->OnStop();
    EXPECT_EQ(NetConnService::GetInstance()->state_, NetConnService::STATE_STOPPED);
    bool result = NetConnService::GetInstance()->Init();
    EXPECT_FALSE(result);

    uint32_t reqId = 0;
    result = NetConnService::GetInstance()->FindSameCallback(nullptr, reqId);
    EXPECT_FALSE(result);

    sptr<NetSupplier> supplier = nullptr;
    std::shared_ptr<NetActivate> netActivateNetwork = nullptr;
    auto ret = NetConnService::GetInstance()->FindBestNetworkForRequest(supplier, netActivateNetwork);
    EXPECT_EQ(ret, ZERO_VALUE);

    NetConnService::GetInstance()->SendAllRequestToNetwork(nullptr);

    NetConnService::GetInstance()->SendRequestToAllNetwork(nullptr);

    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    ASSERT_NE(netLinkInfo, nullptr);
    netLinkInfo->httpProxy_.SetHost(TEST_HOST);
    ret = NetConnService::GetInstance()->UpdateNetLinkInfo(g_supplierId, netLinkInfo);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    supplier = NetConnService::GetInstance()->FindNetSupplier(g_supplierId);
    ASSERT_NE(supplier, nullptr);

    CallbackType type = CallbackType::CALL_TYPE_LOST;
    NetConnService::GetInstance()->CallbackForSupplier(supplier, type);
    type = CallbackType::CALL_TYPE_UPDATE_CAP;
    NetConnService::GetInstance()->CallbackForSupplier(supplier, type);
    type = CallbackType::CALL_TYPE_UPDATE_LINK;
    NetConnService::GetInstance()->CallbackForSupplier(supplier, type);
    type = CallbackType::CALL_TYPE_BLOCK_STATUS;
    NetConnService::GetInstance()->CallbackForSupplier(supplier, type);

    uint32_t validType = INVALID_VALUE;
    type = static_cast<CallbackType>(validType);
    NetConnService::GetInstance()->CallbackForSupplier(supplier, type);

    ret = NetConnService::GetInstance()->RegisterNetConnCallbackAsync(nullptr, nullptr, 0, TEST_UID);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceTest, NetConnServiceBranchTest002, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->UnregisterNetConnCallbackAsync(nullptr, TEST_UID);
    EXPECT_NE(ret, NETSYS_SUCCESS);

    sptr<NetSupplier> supplier = nullptr;
    sptr<NetSupplier> newSupplier = nullptr;
    NetConnService::GetInstance()->MakeDefaultNetWork(supplier, newSupplier);

    ret = NetConnService::GetInstance()->ActivateNetwork(nullptr, nullptr, 0, 0);
    EXPECT_NE(ret, NETSYS_SUCCESS);
}

HWTEST_F(NetConnServiceTest, NetDetectionForDnsHealthTest001, TestSize.Level1)
{
    int32_t netId = 0;
    auto ret = NetConnService::GetInstance()->GetDefaultNet(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    bool dnsHealthSuccess = true;
    bool dnsHealthFail = false;
    ret = NetConnService::GetInstance()->NetDetectionForDnsHealth(netId, dnsHealthSuccess);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    ret = NetConnService::GetInstance()->NetDetectionForDnsHealth(netId, dnsHealthFail);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, FactoryResetNetworkTest001, TestSize.Level1)
{
    NetConnService::GetInstance()->netFactoryResetCallback_ =
        new (std::nothrow) NetFactoryResetCallback();
    ASSERT_NE(NetConnService::GetInstance()->netFactoryResetCallback_, nullptr);
    auto ret = NetConnService::GetInstance()->FactoryResetNetwork();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegisterNetFactoryResetCallbackTest001, TestSize.Level1)
{
    sptr<INetFactoryResetCallback> callback = nullptr;
    auto ret = NetConnService::GetInstance()->RegisterNetFactoryResetCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    NetConnService::GetInstance()->netFactoryResetCallback_ =
        new (std::nothrow) NetFactoryResetCallback();
    ASSERT_NE(NetConnService::GetInstance()->netFactoryResetCallback_, nullptr);

    callback = new (std::nothrow) NetFactoryResetCallbackStub();
    ret = NetConnService::GetInstance()->RegisterNetFactoryResetCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, NetConnServiceBranchTest003, TestSize.Level1)
{
    HttpProxy proxy;
    uint32_t supplierId = 0;
    std::string testString = "test";
    int32_t testInt = 0;
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    std::set<NetCap> netCaps;
    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    NetConnService::GetInstance()->RegisterNetSupplier(NetBearType::BEARER_BLUETOOTH, testString, netCaps, supplierId);
    NetConnService::GetInstance()->UnregisterNetSupplier(supplierId);
    NetConnService::GetInstance()->UpdateGlobalHttpProxy(proxy);
    NetConnService::GetInstance()->OnNetActivateTimeOut(nullptr);
    NetConnService::GetInstance()->UnregisterNetSupplierAsync(supplierId, true, callingUid);
    sptr<NetSupplier> supplier = nullptr;
    NetConnService::GetInstance()->CallbackForSupplier(supplier, CallbackType::CALL_TYPE_AVAILABLE);

    sptr<INetSupplierCallback> supplierCallback = nullptr;
    auto ret = NetConnService::GetInstance()->RegisterNetSupplierCallbackAsync(supplierId, supplierCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = NetConnService::GetInstance()->RegisterNetSupplierCallback(supplierId, supplierCallback);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    sptr<INetConnCallback> callback = nullptr;
    uint32_t timeoutMS = 0;
    sptr<NetSpecifier> netSpecifier = nullptr;
    ret = NetConnService::GetInstance()->RegisterNetConnCallback(netSpecifier, callback, timeoutMS);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = NetConnService::GetInstance()->RequestNetConnection(netSpecifier, callback, timeoutMS);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = NetConnService::GetInstance()->UnregisterNetConnCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    sptr<INetDetectionCallback> detectionCallback = nullptr;
    ret = NetConnService::GetInstance()->RegUnRegNetDetectionCallback(testInt, detectionCallback, false);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = NetConnService::GetInstance()->UpdateNetStateForTest(netSpecifier, testInt);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    sptr<NetSupplierInfo> netSupplierInfo = nullptr;
    ret = NetConnService::GetInstance()->UpdateNetSupplierInfo(testInt, netSupplierInfo);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    sptr<NetLinkInfo> netLinkInfo = nullptr;
    ret = NetConnService::GetInstance()->UpdateNetLinkInfo(testInt, netLinkInfo);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = NetConnService::GetInstance()->NetDetection(testInt);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = NetConnService::GetInstance()->RestrictBackgroundChanged(false);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, NetConnServiceBranchTest004, TestSize.Level1)
{
    NetConnService::GetInstance()->RequestAllNetworkExceptDefault();

    NetConnService::NetInterfaceStateCallback stateCallback;
    std::string testString = "test";
    int32_t testInt = 0;
    auto ret = stateCallback.OnInterfaceAddressUpdated(testString, testString, testInt, testInt);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = stateCallback.OnInterfaceAddressRemoved(testString, testString, testInt, testInt);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = stateCallback.OnInterfaceAdded(testString);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = stateCallback.OnInterfaceRemoved(testString);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = stateCallback.OnInterfaceChanged(testString, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = stateCallback.OnInterfaceLinkStateChanged(testString, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = stateCallback.OnRouteChanged(false, testString, testString, testString);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    NetsysControllerCallback::DhcpResult dhcpResult;
    ret = stateCallback.OnDhcpSuccess(dhcpResult);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = stateCallback.OnBandwidthReachedLimit(testString, testString);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    sptr<INetInterfaceStateCallback> interfaceStateCallback = nullptr;
    ret = stateCallback.RegisterInterfaceCallback(interfaceStateCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    ret = stateCallback.UnregisterInterfaceCallback(interfaceStateCallback);
    EXPECT_EQ(ret, NET_CONN_ERR_CALLBACK_NOT_FOUND);
}

HWTEST_F(NetConnServiceTest, NetConnServiceBranchTest005, TestSize.Level1)
{
    NetHttpProxyTracker httpProxyTracker;
    std::string exclusions = "";
    NetConnService::GetInstance()->GetPreferredRegex();
    std::list<std::string> list = httpProxyTracker.ParseExclusionList(exclusions);
    EXPECT_TRUE(list.empty());

    std::string result = httpProxyTracker.GetExclusionsAsString(list);
    EXPECT_TRUE(result.empty());

    uint32_t supplierId = 10;
    int32_t type = 0;
    auto ret = NetConnService::GetInstance()->RegisterSlotType(supplierId, type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string slotType = "";
    ret = NetConnService::GetInstance()->GetSlotType(slotType);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string url = "";
    PreferCellularType preferCellular = PreferCellularType::NOT_PREFER;
    ret = NetConnService::GetInstance()->IsPreferCellularUrl(url, preferCellular);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    NetConnService::GetInstance()->netFactoryResetCallback_ = nullptr;
    ret = NetConnService::GetInstance()->FactoryResetNetwork();
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceTest, NetConnServiceBranchTest007, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->UnregisterNetConnCallbackAsync(nullptr, TEST_UID);
    EXPECT_NE(ret, NETSYS_SUCCESS);

    sptr<NetSupplier> supplier = nullptr;
    sptr<NetSupplier> newSupplier = nullptr;
    NetConnService::GetInstance()->MakeDefaultNetWork(supplier, newSupplier);

    ret = NetConnService::GetInstance()->ActivateNetwork(nullptr, nullptr, 0, 0, REQUEST);
    EXPECT_NE(ret, NETSYS_SUCCESS);
}

HWTEST_F(NetConnServiceTest, FindSupplierWithInternetByBearerType001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_MMS);
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    uint32_t supplierId = 0;
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    int32_t ret = NetConnService::GetInstance()->RegisterNetSupplierAsync(NetBearType::BEARER_WIFI, TEST_IDENT,
        netCaps, supplierId, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::vector<sptr<NetSupplier>> suppliers =
        NetConnService::GetInstance()->FindSupplierWithInternetByBearerType(NetBearType::BEARER_WIFI, TEST_IDENT);
    EXPECT_FALSE(suppliers.empty());
}

HWTEST_F(NetConnServiceTest, UpdateSupplierScore001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_MMS);
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    uint32_t supplierId = 0;
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    int32_t ret = NetConnService::GetInstance()->RegisterNetSupplierAsync(NetBearType::BEARER_WIFI, TEST_IDENT,
        netCaps, supplierId, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    NetConnService::GetInstance()->MakeDefaultNetWork(NetConnService::GetInstance()->defaultNetSupplier_,
        NetConnService::GetInstance()->netSuppliers_[supplierId]);
    ret = NetConnService::GetInstance()->UpdateSupplierScoreAsync(supplierId, QUALITY_POOR_STATE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    auto supplier = NetConnService::GetInstance()->FindNetSupplier(supplierId);
    supplier->SetDetectionDone();
    EXPECT_EQ(supplier->GetRealScore(), supplier->GetNetScore() - DIFF_SCORE_BETWEEN_GOOD_POOR);
}

HWTEST_F(NetConnServiceTest, UpdateSupplierScore002, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_MMS);
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    uint32_t supplierId = 0;
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    int32_t ret = NetConnService::GetInstance()->RegisterNetSupplierAsync(NetBearType::BEARER_WIFI, TEST_IDENT,
        netCaps, supplierId, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = NetConnService::GetInstance()->UpdateSupplierScoreAsync(supplierId, QUALITY_POOR_STATE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, UpdateSupplierScore003, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_MMS);
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    uint32_t supplierId = 0;
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    int32_t ret = NetConnService::GetInstance()->RegisterNetSupplierAsync(NetBearType::BEARER_WIFI, TEST_IDENT,
        netCaps, supplierId, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    ASSERT_NE(netLinkInfo, nullptr);
    netLinkInfo->ifaceName_ = "wlan0";
    INetAddr netAddr;
    netAddr.type_ = INetAddr::IPV4;
    netAddr.hostName_ = "testHost";
    netLinkInfo->netAddrList_.push_back(netAddr);
    ret = NetConnService::GetInstance()->UpdateNetLinkInfoAsync(supplierId, netLinkInfo, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = NetConnService::GetInstance()->UpdateSupplierScoreAsync(supplierId, QUALITY_POOR_STATE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetDefaultSupplierId001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_MMS);
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    uint32_t supplierId = 0;
    int32_t callingUid = IPCSkeleton::GetCallingUid();
    int32_t ret = NetConnService::GetInstance()->RegisterNetSupplierAsync(NetBearType::BEARER_WIFI, TEST_IDENT,
        netCaps, supplierId, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = NetConnService::GetInstance()->GetDefaultSupplierId(NetBearType::BEARER_WIFI, TEST_IDENT,
        supplierId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, EnableVnicNetwork001, TestSize.Level1)
{
    sptr<NetManagerStandard::NetLinkInfo> linkInfo = nullptr;
    std::set<int32_t> uids;

    linkInfo = new (std::nothrow) NetManagerStandard::NetLinkInfo();
    ASSERT_NE(linkInfo, nullptr);

    int32_t ret = NetConnService::GetInstance()->EnableVnicNetworkAsync(linkInfo, uids);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
}

HWTEST_F(NetConnServiceTest, EnableVnicNetwork002, TestSize.Level1)
{
    sptr<NetManagerStandard::NetLinkInfo> linkInfo = nullptr;
    std::set<int32_t> uids;

    linkInfo = new (std::nothrow) NetManagerStandard::NetLinkInfo();
    ASSERT_NE(linkInfo, nullptr);

    NetManagerStandard::INetAddr inetAddr;
    inetAddr.type_ = NetManagerStandard::INetAddr::IpType::IPV4;
    inetAddr.family_ = 0x01;
    inetAddr.address_ = "10.0.0.2";
    inetAddr.netMask_ = "255.255.255.0";
    inetAddr.hostName_ = "localhost";
    inetAddr.port_ = 80;
    inetAddr.prefixlen_ = 24;

    linkInfo->ifaceName_ = "vnic-tun";
    linkInfo->netAddrList_.push_back(inetAddr);
    linkInfo->mtu_ = 1500;

    int32_t ret = NetConnService::GetInstance()->EnableVnicNetworkAsync(linkInfo, uids);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, DisableVnicNetwork001, TestSize.Level1)
{
    int32_t ret = NetConnService::GetInstance()->DisableVnicNetworkAsync();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, CmdCloseSocketsUid002, TestSize.Level1)
{
    int32_t netId = 100;
    uint32_t uid = 20020157;
    int32_t ret = NetConnService::GetInstance()->CloseSocketsUid(netId, uid);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, SystemReadyTest002, TestSize.Level1)
{
    if (!NetConnService::GetInstance()->registerToService_) {
        NetConnService::GetInstance()->state_ = NetConnService::STATE_STOPPED;
    }

    NetConnService::GetInstance()->OnStart();
    int32_t ret = NetConnService::GetInstance()->SystemReady();
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, UnregisterNetSupplierTest001, TestSize.Level1)
{
    uint32_t supplierId = 0;
    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    int32_t ret = NetConnService::GetInstance()->UnregisterNetSupplier(supplierId);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, RegisterNetSupplierCallbackAsync001, TestSize.Level1)
{
    uint32_t supplierId = -1;
    sptr<INetSupplierCallback> callback = new (std::nothrow) NetSupplierCallbackStubTestCb();
    ASSERT_NE(callback, nullptr);
    auto ret = NetConnService::GetInstance()->RegisterNetSupplierCallbackAsync(supplierId, callback);
    EXPECT_EQ(ret, NET_CONN_ERR_NO_SUPPLIER);
}

HWTEST_F(NetConnServiceTest, RequestNetConnectionAsyncTest001, TestSize.Level1)
{
    uint32_t callingUid = 1;
    sptr<NetSpecifier> netSpecifier = nullptr;
    sptr<INetConnCallback> uidCallback = nullptr;
    int32_t ret = NetConnService::GetInstance()->RequestNetConnectionAsync(netSpecifier, uidCallback, 0, callingUid);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceTest, NetDetectionForDnsHealthSyncTest001, TestSize.Level1)
{
    int32_t netId = 1;
    bool dnsHealthSuccess = true;
    auto netConnService = std::make_shared<NetConnService>();
    int32_t ret = netConnService->NetDetectionForDnsHealthSync(netId, dnsHealthSuccess);
    EXPECT_EQ(ret, NET_DETECTION_FAIL);
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    std::set<NetCap> netCasps;
    netConnService->netSuppliers_[99] =
        sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "", netCasps);
    netConnService->netSuppliers_[99]->SetNetwork(network);
    ret = netConnService->NetDetectionForDnsHealthSync(netId, dnsHealthSuccess);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, NNetDetectionForDnsHealthSyncTest001, TestSize.Level1)
{
    int32_t netId = 1;
    bool dnsHealthSuccess = true;
    auto netConnService = std::make_shared<NetConnService>();
    int32_t ret = netConnService->NetDetectionForDnsHealthSync(netId, dnsHealthSuccess);
    EXPECT_EQ(ret, NET_DETECTION_FAIL);
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    std::set<NetCap> netCasps;
    netConnService->netSuppliers_[99] =
        sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "", netCasps);
    netConnService->netSuppliers_[99]->SetNetwork(network);
    ret = netConnService->NetDetectionForDnsHealthSync(netId, dnsHealthSuccess);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetIfaceNameIdentMapsTest002, TestSize.Level1)
{
    SafeMap<std::string, std::string> data;
    auto ret = NetConnService::GetInstance()->GetIfaceNameIdentMaps((NetBearType)-1, data);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);
    ret = NetConnService::GetInstance()->GetIfaceNameIdentMaps(BEARER_CELLULAR, data);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceTest, NotFindBestSupplierTest001, TestSize.Level1)
{
    uint32_t reqId = 0;
    std::shared_ptr<NetActivate> active = nullptr;
    std::string netSupplierIdent = "test";
    const std::set<NetCap> netCaps = {NetCap::NET_CAPABILITY_SUPL};
    sptr<NetSupplier> supplier = new (std::nothrow) NetSupplier(NetBearType::BEARER_WIFI, netSupplierIdent, netCaps);
    sptr<INetConnCallback> callback = nullptr;
    NetConnService::GetInstance()->NotFindBestSupplier(reqId, active, supplier, callback);
    ASSERT_TRUE(callback == nullptr);

    callback = new (std::nothrow) NetConnCallbackStubCb();
    NetConnService::GetInstance()->NotFindBestSupplier(reqId, active, supplier, callback);
    ASSERT_TRUE(callback != nullptr);
}

HWTEST_F(NetConnServiceTest, GetDefaultHttpProxyTest003, TestSize.Level1)
{
    HttpProxy httpProxy = {"", 8080, {}};
    int32_t ret = NetConnService::GetInstance()->GetGlobalHttpProxy(httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);

    int32_t bindNetId = 0;
    ret = NetConnService::GetInstance()->GetDefaultHttpProxy(bindNetId, httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);

    httpProxy = {"host", 8080, {}};
    ret = NetConnService::GetInstance()->GetDefaultHttpProxy(bindNetId, httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);

    NetConnService::GetInstance()->defaultNetSupplier_ = nullptr;
    ret = NetConnService::GetInstance()->GetDefaultHttpProxy(bindNetId, httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);

    std::string netSupplierIdent = "test";
    const std::set<NetCap> netCaps = {NetCap::NET_CAPABILITY_SUPL};
    sptr<NetSupplier> supplier = new (std::nothrow) NetSupplier(NetBearType::BEARER_WIFI, netSupplierIdent, netCaps);
    NetConnService::GetInstance()->defaultNetSupplier_ = supplier;
    ret = NetConnService::GetInstance()->GetDefaultHttpProxy(bindNetId, httpProxy);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, IsValidDecValueTest001, TestSize.Level1)
{
    std::string inputValue = "12345678910";
    bool ret = NetConnService::GetInstance()->IsValidDecValue(inputValue);
    ASSERT_FALSE(ret);

    inputValue = "123456";
    ret = NetConnService::GetInstance()->IsValidDecValue(inputValue);
    ASSERT_TRUE(ret);

    inputValue = "6.66";
    ret = NetConnService::GetInstance()->IsValidDecValue(inputValue);
    ASSERT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, RegisterPreAirplaneCallbackTest001, TestSize.Level1)
{
    sptr<IPreAirplaneCallback> callback = nullptr;
    int32_t ret = NetConnService::GetInstance()->RegisterPreAirplaneCallback(callback);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);

    ret = NetConnService::GetInstance()->UnregisterPreAirplaneCallback(callback);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetAirplaneModeTest003, TestSize.Level1)
{
    bool state = true;
    sptr<IPreAirplaneCallback> preAirplane;
    std::map<int32_t, sptr<IPreAirplaneCallback>> preAirplaneMap{{1, preAirplane}};
    NetConnService::GetInstance()->preAirplaneCallbacks_ = preAirplaneMap;
    int32_t ret = NetConnService::GetInstance()->SetAirplaneMode(state);
    ASSERT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceTest, NetDetectionForDnsHealthTest002, TestSize.Level1)
{
    int32_t netId = 1;
    bool dnsHealthSuccess = true;
    NetConnService::GetInstance()->Init();
    int32_t ret = NetConnService::GetInstance()->NetDetectionForDnsHealth(netId, dnsHealthSuccess);
    ASSERT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, NetConnServiceBranchTest006, TestSize.Level1)
{
    std::string addr = "addr";
    std::string ifName = "name";
    int flags = 1;
    int scope = 1;
    NetConnService::NetInterfaceStateCallback stateCallback;
    sptr<INetInterfaceStateCallback> ifaceStateCallback = nullptr;
    stateCallback.ifaceStateCallbacks_.push_back(ifaceStateCallback);

    int32_t ret = stateCallback.OnInterfaceAddressUpdated(addr, ifName, flags, scope);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = stateCallback.OnInterfaceAddressRemoved(addr, ifName, flags, scope);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = stateCallback.OnInterfaceAdded(addr);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = stateCallback.OnInterfaceRemoved(addr);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = stateCallback.OnInterfaceChanged(addr, true);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = stateCallback.OnInterfaceLinkStateChanged(addr, true);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ret = stateCallback.RegisterInterfaceCallback(ifaceStateCallback);
    ASSERT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    ifaceStateCallback = new (std::nothrow) NetInterfaceStateCallbackStub();
    NetConnService::GetInstance()->RequestAllNetworkExceptDefault();
    ret = stateCallback.RegisterInterfaceCallback(ifaceStateCallback);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, OnAddSystemAbilityTest002, TestSize.Level1)
{
    int32_t systemAbilityId = COMM_NETSYS_NATIVE_SYS_ABILITY_ID;
    std::string deviceId = "654321";
    NetConnService::GetInstance()->hasSARemoved_ = false;
    NetConnService::GetInstance()->OnAddSystemAbility(systemAbilityId, deviceId);
    ASSERT_FALSE(NetConnService::GetInstance()->hasSARemoved_);
    NetConnService::GetInstance()->OnRemoveSystemAbility(systemAbilityId, deviceId);
    ASSERT_TRUE(NetConnService::GetInstance()->hasSARemoved_);
    NetConnService::GetInstance()->OnAddSystemAbility(systemAbilityId, deviceId);
    ASSERT_FALSE(NetConnService::GetInstance()->hasSARemoved_);

    systemAbilityId = ACCESS_TOKEN_MANAGER_SERVICE_ID;
    NetConnService::GetInstance()->registerToService_ = true;
    NetConnService::GetInstance()->OnAddSystemAbility(systemAbilityId, deviceId);
    ASSERT_TRUE(NetConnService::GetInstance()->registerToService_);
    NetConnService::GetInstance()->registerToService_ = false;
    NetConnService::GetInstance()->OnAddSystemAbility(systemAbilityId, deviceId);
    ASSERT_TRUE(NetConnService::GetInstance()->registerToService_);
}

HWTEST_F(NetConnServiceTest, RegisterSlotTypeTest002, TestSize.Level1)
{
    uint32_t supplierId = 0;
    int32_t type = 1;
    std::string netSupplierIdent = "test";
    const std::set<NetCap> netCaps = {NetCap::NET_CAPABILITY_SUPL};
    sptr<NetSupplier> supplier = new (std::nothrow) NetSupplier(NetBearType::BEARER_WIFI, netSupplierIdent, netCaps);
    std::map<uint32_t, sptr<NetSupplier>> netSupplierMap = {{1, supplier}};
    NetConnService::GetInstance()->netSuppliers_ = netSupplierMap;
    NetConnService::GetInstance()->Init();
    int32_t ret = NetConnService::GetInstance()->RegisterSlotType(supplierId, type);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);

    supplierId = 1;
    ret = NetConnService::GetInstance()->RegisterSlotType(supplierId, type);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetSlotTypeTest001, TestSize.Level1)
{
    std::string slotType = "";
    NetConnService::GetInstance()->defaultNetSupplier_ = nullptr;
    NetConnService::GetInstance()->Init();
    int32_t ret = NetConnService::GetInstance()->GetSlotType(slotType);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    std::string netSupplierIdent = "test";
    const std::set<NetCap> netCaps = {NetCap::NET_CAPABILITY_SUPL};
    sptr<NetSupplier> supplier = new (std::nothrow) NetSupplier(NetBearType::BEARER_WIFI, netSupplierIdent, netCaps);
    NetConnService::GetInstance()->defaultNetSupplier_ = supplier;
    ret = NetConnService::GetInstance()->GetSlotType(slotType);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, OnRemoteDiedTest001, TestSize.Level1)
{
    wptr<IRemoteObject> remoteObject = nullptr;
    NetConnService::GetInstance()->OnRemoteDied(remoteObject);
    ASSERT_TRUE(remoteObject == nullptr);
}

HWTEST_F(NetConnServiceTest, DisableVnicNetworkTest001, TestSize.Level1)
{
    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    int32_t ret = NetConnService::GetInstance()->DisableVnicNetwork();
    ASSERT_EQ(ret, NETMANAGER_ERROR);
    NetConnService::GetInstance()->Init();
    ret = NetConnService::GetInstance()->DisableVnicNetwork();
    ASSERT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, EnableDistributedClientNetTest001, TestSize.Level1)
{
    std::string virnicAddr = "127.0.0.1";
    std::string virnicName = "virnic";
    std::string iif = "iif";
    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    int32_t ret = NetConnService::GetInstance()->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    ASSERT_EQ(ret, NETMANAGER_ERROR);
    NetConnService::GetInstance()->Init();
    ret = NetConnService::GetInstance()->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    ASSERT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, EnableDistributedClientNetTest002, TestSize.Level1)
{
    std::string virnicAddr = "";
    std::string virnicName = "virnic";
    std::string iif = "";
    int32_t ret = NetConnService::GetInstance()->EnableDistributedClientNetAsync(virnicAddr, virnicName, iif);
    ASSERT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
    iif = "iif";
    ret = NetConnService::GetInstance()->EnableDistributedClientNetAsync(virnicAddr, virnicName, iif);
    ASSERT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
    virnicAddr = "127.0.0.1";
    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    ret = NetConnService::GetInstance()->EnableDistributedClientNetAsync(virnicAddr, virnicName, iif);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    NetConnService::GetInstance()->Init();
    ret = NetConnService::GetInstance()->EnableDistributedClientNetAsync(virnicAddr, virnicName, iif);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, EnableDistributedServerNetTest001, TestSize.Level1)
{
    std::string devIface = "devIface";
    std::string virnicAddr = "127.0.0.1";
    std::string iif = "iif";
    std::string gw = "0.0.0.0";
    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    int32_t ret = NetConnService::GetInstance()->EnableDistributedServerNet(iif, devIface, virnicAddr, gw);
    ASSERT_EQ(ret, NETMANAGER_ERROR);
    NetConnService::GetInstance()->Init();
    ret = NetConnService::GetInstance()->EnableDistributedServerNet(iif, devIface, virnicAddr, gw);
    ASSERT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, EnableDistributedServerNetTest002, TestSize.Level1)
{
    std::string virnicAddr = "";
    std::string iif = "";
    std::string devIface = "devIface";
    std::string gw = "";
    int32_t ret = NetConnService::GetInstance()->EnableDistributedServerNetAsync(iif, devIface, virnicAddr, gw);
    ASSERT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
    iif = "iif";
    ret = NetConnService::GetInstance()->EnableDistributedServerNetAsync(iif, devIface, virnicAddr, gw);
    ASSERT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
    virnicAddr = "127.0.0.1";
    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    ret = NetConnService::GetInstance()->EnableDistributedServerNetAsync(iif, devIface, virnicAddr, gw);
    ASSERT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
    NetConnService::GetInstance()->Init();
    ret = NetConnService::GetInstance()->EnableDistributedServerNetAsync(iif, devIface, virnicAddr, gw);
    ASSERT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetConnServiceTest, DisableDistributedNetTest001, TestSize.Level1)
{
    bool isServer = false;
    std::string virnicName = "virnic";
    std::string dstAddr = "1.1.1.1";
    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    int32_t ret = NetConnService::GetInstance()->DisableDistributedNet(isServer, virnicName, dstAddr);
    ASSERT_EQ(ret, NETMANAGER_ERROR);
    NetConnService::GetInstance()->Init();
    ret = NetConnService::GetInstance()->DisableDistributedNet(isServer, virnicName, dstAddr);
    ASSERT_EQ(ret, NETMANAGER_ERROR);

    NetConnService::GetInstance()->netConnEventHandler_ = nullptr;
    ret = NetConnService::GetInstance()->DisableDistributedNetAsync(isServer, virnicName, dstAddr);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    NetConnService::GetInstance()->Init();
    ret = NetConnService::GetInstance()->DisableDistributedNetAsync(isServer, virnicName, dstAddr);
    ASSERT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetConnServiceTest, SetInterfaceUpTest001, TestSize.Level1)
{
    std::string ifName = "wlan0";
    auto ret = NetConnService::GetInstance()->SetInterfaceUp(ifName);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetInterfaceDownTest001, TestSize.Level1)
{
    std::string ifName = "wlan0";
    auto ret = NetConnService::GetInstance()->SetInterfaceDown(ifName);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetNetInterfaceIpAddressTest001, TestSize.Level1)
{
    std::string ifName = "wlan0";
    std::string ipAddr = "0.0.0.1";
    auto ret = NetConnService::GetInstance()->SetNetInterfaceIpAddress(ifName, ipAddr);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, UpdateNetCaps001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_MMS);
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);

    auto ret = NetConnService::GetInstance()->UpdateNetCaps(netCaps, g_supplierId);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceTest, UpdateNetCaps002, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    netCaps.insert(NetCap::NET_CAPABILITY_NOT_VPN);
    std::string netConnManagerWorkThread = "NET_CONN_MANAGER_WORK_THREAD";
    NetConnService::GetInstance()->netConnEventRunner_ = AppExecFwk::EventRunner::Create(netConnManagerWorkThread);
    NetConnService::GetInstance()->netConnEventHandler_
        = std::make_shared<NetConnEventHandler>(NetConnService::GetInstance()->netConnEventRunner_);
        
    auto ret = NetConnService::GetInstance()->UpdateNetCaps(netCaps, g_supplierId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}


HWTEST_F(NetConnServiceTest, SendHttpProxyChangeBroadcast001, TestSize.Level1)
{
    NetHttpProxyTracker httpProxyTracker;
    HttpProxy httpProxy;
    httpProxy.SetPort(0);
    httpProxyTracker.ReadFromSettingsData(httpProxy);
    NetConnService::GetInstance()->SendHttpProxyChangeBroadcast(httpProxy);
    int32_t userId;
    int32_t ret = NetConnService::GetInstance()->GetActiveUserId(userId);
    if (ret == NETMANAGER_SUCCESS) {
        NetConnService::GetInstance()->SendHttpProxyChangeBroadcast(httpProxy);
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    }
}

HWTEST_F(NetConnServiceTest, SetPacUrlTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->SetPacUrl("text");
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetPacUrlTest001, TestSize.Level1)
{
    std::string pacUrl;
    auto ret = NetConnService::GetInstance()->GetPacUrl(pacUrl);
    ASSERT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetAppIsFrozenedTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->SetAppIsFrozened(20020177, true);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetAppIsFrozenedTest002, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->SetAppIsFrozened(20020177, false);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, EnableAppFrozenedCallbackLimitationTest001, TestSize.Level1)
{
    auto ret = NetConnService::GetInstance()->EnableAppFrozenedCallbackLimitation(true);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, SetReuseSupplierIdTest001, TestSize.Level1)
{
    uint32_t supplierId = 1004;
    uint32_t reuseSupplierId = 1008;
    bool add = false;
    auto ret = NetConnService::GetInstance()->SetReuseSupplierId(supplierId, reuseSupplierId, add);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    supplierId = -1;
    ret = NetConnService::GetInstance()->SetReuseSupplierId(supplierId, reuseSupplierId, add);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, ReplaceUserIdForUriTest001, TestSize.Level1)
{
    NetHttpProxyTracker httpProxyTracker;
    const char *uri = "";
    int32_t userId = 123;
    auto ret = httpProxyTracker.ReplaceUserIdForUri(uri, userId);
    EXPECT_EQ(ret, "");
}

HWTEST_F(NetConnServiceTest, QueryTraceRouteTest001, TestSize.Level1)
{
    const std::string destination = "www.text.com";
    int32_t maxJumpNumber = 30;
    int32_t packetsType = 1;
    std::string traceRouteInfo = "";
    auto ret = NetConnService::GetInstance()->QueryTraceRoute(destination, maxJumpNumber,
        packetsType, traceRouteInfo, true);
    EXPECT_EQ("", traceRouteInfo);
}

HWTEST_F(NetConnServiceTest, QueryTraceRouteTest002, TestSize.Level1)
{
    const std::string destination = "test";
    int32_t maxJumpNumber = 30;
    int32_t packetsType = 1;
    std::string traceRouteInfo = "";
    auto ret = NetConnService::GetInstance()->QueryTraceRoute(destination, maxJumpNumber,
        packetsType, traceRouteInfo, true);
    EXPECT_EQ("", traceRouteInfo);
}

HWTEST_F(NetConnServiceTest, OnReceiveEventTest001, TestSize.Level1)
{
    EventFwk::CommonEventData data;
    EventFwk::Want want;
    want.SetAction("usual.event.USER_SWITCHED");
    data.SetWant(want);
    NetConnService::GetInstance()->OnReceiveEvent(data);
    EXPECT_TRUE(NetConnService::GetInstance()->isDataShareReady_);
}

HWTEST_F(NetConnServiceTest, OnReceiveEventTest002, TestSize.Level1)
{
    EventFwk::CommonEventData data;
    EventFwk::Want want;
    want.SetAction("usual.event.DATA_SHARE_READY");
    data.SetWant(want);
    NetConnService::GetInstance()->OnReceiveEvent(data);
    EXPECT_TRUE(NetConnService::GetInstance()->isDataShareReady_);
}

HWTEST_F(NetConnServiceTest, OnReceiveEventTest003, TestSize.Level1)
{
    EventFwk::CommonEventData data;
    EventFwk::Want want;
    want.SetAction("usual.event.POWER_MANAGER_STATE_CHANGED");
    data.SetWant(want);
    NetConnService::GetInstance()->OnReceiveEvent(data);
    EXPECT_TRUE(NetConnService::GetInstance()->isDataShareReady_);
}

HWTEST_F(NetConnServiceTest, OnReceiveEventTest004, TestSize.Level1)
{
    EventFwk::CommonEventData data;
    EventFwk::Want want;
    want.SetAction("usual.event.SCREEN_OFF");
    data.SetWant(want);
    NetConnService::GetInstance()->OnReceiveEvent(data);
    EXPECT_TRUE(NetConnService::GetInstance()->isDataShareReady_);
}

HWTEST_F(NetConnServiceTest, OnReceiveEventTest005, TestSize.Level1)
{
    EventFwk::CommonEventData data;
    EventFwk::Want want;
    want.SetAction("usual.event.SCREEN_ON");
    data.SetWant(want);
    NetConnService::GetInstance()->OnReceiveEvent(data);
    EXPECT_TRUE(NetConnService::GetInstance()->isDataShareReady_);
}

HWTEST_F(NetConnServiceTest, RegUnRegisterNetProbeCallback001, TestSize.Level1)
{
    int32_t testNetId = 9999;
    std::shared_ptr<IDualStackProbeCallback> cb = nullptr;
    auto ret = NetConnService::GetInstance()->RegUnRegisterNetProbeCallback(testNetId, cb, false);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegUnRegisterNetProbeCallback002, TestSize.Level1)
{
    int32_t testNetId = 9999;
    std::shared_ptr<IDualStackProbeCallback> cb = std::make_shared<NetProbeCallbackTest>();
    auto ret = NetConnService::GetInstance()->RegUnRegisterNetProbeCallback(testNetId, cb, false);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, RegUnRegisterNetProbeCallback003, TestSize.Level1)
{
    int32_t testNetId = 999;
    auto netConnService = std::make_shared<NetConnService>();
    auto network = std::make_shared<Network>(testNetId, testNetId, NetBearType::BEARER_ETHERNET, nullptr);
    std::set<NetCap> netCasps;
    netConnService->netSuppliers_[99] =
        sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "", netCasps);
    netConnService->netSuppliers_[99]->SetNetwork(network);
    std::shared_ptr<IDualStackProbeCallback> cb = std::make_shared<NetProbeCallbackTest>();
    auto ret = netConnService->RegUnRegisterNetProbeCallback(testNetId, cb, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = netConnService->RegUnRegisterNetProbeCallback(testNetId, cb, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, NetProbe001, TestSize.Level1)
{
    int32_t testNetId = 9999;
    auto ret = NetConnService::GetInstance()->DualStackProbe(testNetId);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, NetProbe002, TestSize.Level1)
{
    int32_t testNetId = 9999;
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->DualStackProbe(testNetId);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, NetProbe003, TestSize.Level1)
{
    int32_t testNetId = 100;
    auto ret = NetConnService::GetInstance()->DualStackProbe(testNetId);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, NetProbe004, TestSize.Level1)
{
    int32_t netId;
    NetConnService::GetInstance()->GetDefaultNet(netId);
    auto ret = NetConnService::GetInstance()->DualStackProbe(netId);
    EXPECT_NE(netId, 0);
}

HWTEST_F(NetConnServiceTest, UpdateNetProbeTime001, TestSize.Level1)
{
    int32_t testProbeTime = 5 * 1000;
    int32_t supplierId = 9999;
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->UpdateDualStackProbeTime(testProbeTime);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    netConnService->netSuppliers_[supplierId] = nullptr;
    ret = netConnService->UpdateDualStackProbeTime(testProbeTime);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetIpNeighTableTest001, TestSize.Level1)
{
    std::vector<NetIpMacInfo> ipMacInfo;
    auto ret = NetConnService::GetInstance()->GetIpNeighTable(ipMacInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, CreateVlanTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->CreateVlan(ifName, vlanId);
    EXPECT_TRUE(ret == NETMANAGER_ERR_OPERATION_FAILED || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, DestroyVlanTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->DestroyVlan(ifName, vlanId);
    EXPECT_TRUE(ret == NETMANAGER_ERR_OPERATION_FAILED || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, AddVlanIpTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_TRUE(ret == NETMANAGER_ERR_OPERATION_FAILED || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, DeleteVlanIpTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->DeleteVlanIp(ifName, vlanId, ip, mask);
    EXPECT_TRUE(ret == NETMANAGER_ERR_ADDR_NOT_FOUND || ret == NETMANAGER_ERR_OPERATION_FAILED ||
                ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetConnectOwnerUidTest001, TestSize.Level1)
{
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    auto ret = NetConnService::GetInstance()->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, GetSystemNetPortStatesTest001, TestSize.Level1)
{
    NetPortStatesInfo netPortStatesInfo;
    auto ret = NetConnService::GetInstance()->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, FindProxyForURL001, TestSize.Level1)
{
    std::string url = "";
    std::string host = "";
    std::string proxy = "11";
    NetConnService::GetInstance()->FindProxyForURL(url, host, proxy);
    EXPECT_TRUE(proxy.empty());
}

HWTEST_F(NetConnServiceTest, FindProxyForURL002, TestSize.Level1)
{
    std::string url = "http://127.0.0.1:3888/test";
    std::string host = "";
    std::string proxy = "11";
    NetConnService::GetInstance()->FindProxyForURL(url, host, proxy);
    EXPECT_TRUE(host.empty());
}

HWTEST_F(NetConnServiceTest, IsPreferCellularUrlTest001, TestSize.Level1) {
    std::string url = "https://rcs.cmpassport.com";
    PreferCellularType preferCellular = PreferCellularType::NOT_PREFER;
    auto ret = NetConnService::GetInstance()->IsPreferCellularUrl(url, preferCellular);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(preferCellular != PreferCellularType::END);
}

HWTEST_F(NetConnServiceTest, IsPreferCellularUrlTest002, TestSize.Level1) {
    std::string url = "https://example.ctm.net";
    PreferCellularType preferCellular = PreferCellularType::NOT_PREFER;
    auto ret = NetConnService::GetInstance()->IsPreferCellularUrl(url, preferCellular);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(preferCellular != PreferCellularType::END);
}

HWTEST_F(NetConnServiceTest, IsPreferCellularUrlTest003, TestSize.Level1) {
    std::string url = "https://exampleasjfaspoifqanfakjb";
    PreferCellularType preferCellular = PreferCellularType::NOT_PREFER;
    auto ret = NetConnService::GetInstance()->IsPreferCellularUrl(url, preferCellular);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(preferCellular != PreferCellularType::END);
}

HWTEST_F(NetConnServiceTest, IsInPreferredListTest001, TestSize.Level1) {
    std::string hostName = "example.com";
    std::vector<std::string> regexList = {"^example\\.[a-z]{2,}$", "^test\\.[a-z]{2,}$"};
    auto ret = NetConnService::GetInstance()->IsInPreferredList(hostName, regexList);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceTest, IsInPreferredListTest002, TestSize.Level1) {
    std::string hostName = "example.org";
    std::vector<std::string> regexList = {"^example\\.[a-z]{2,}$", "^test\\.[a-z]{2,}$"};
    auto ret = NetConnService::GetInstance()->IsInPreferredList(hostName, regexList);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, IsInPreferredListTest003, TestSize.Level1) {
    std::string hostName = "example.com";
    std::vector<std::string> regexList = {};
    auto ret = NetConnService::GetInstance()->IsInPreferredList(hostName, regexList);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, IsInPreferredListTest004, TestSize.Level1) {
    std::string hostName = "";
    std::vector<std::string> regexList = {"^example\\.[a-z]{2,}$", "^test\\.[a-z]{2,}$"};
    auto ret = NetConnService::GetInstance()->IsInPreferredList(hostName, regexList);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, IsInPreferredListTest005, TestSize.Level1) {
    std::string hostName = "example.com";
    std::vector<std::string> regexList = {"invalid[regex", "another[invalid[regex"};
    auto ret = NetConnService::GetInstance()->IsInPreferredList(hostName, regexList);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, UpdateUidDeadFlowResetTest001, TestSize.Level1)
{
    std::vector<std::string> bundleNameVec;
    bundleNameVec.push_back("com.test.bundle1");
    bundleNameVec.push_back("com.test.bundle2");
    int32_t ret = NetConnService::GetInstance()->UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, UpdateUidDeadFlowResetTest002, TestSize.Level1)
{
    std::vector<std::string> bundleNameVec;
    int32_t ret = NetConnService::GetInstance()->UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, IsDeadFlowResetTargetBundleTest001, TestSize.Level1)
{
    std::vector<std::string> bundleNameVec;
    bundleNameVec.push_back("com.test.target");
    bundleNameVec.push_back("com.test.other");
    int32_t ret = NetConnService::GetInstance()->UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    bool flag = false;
    ret = NetConnService::GetInstance()->IsDeadFlowResetTargetBundle("com.test.target", flag);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(flag);
}

HWTEST_F(NetConnServiceTest, IsDeadFlowResetTargetBundleTest002, TestSize.Level1)
{
    std::vector<std::string> bundleNameVec;
    bundleNameVec.push_back("com.test.bundle1");
    int32_t ret = NetConnService::GetInstance()->UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    bool flag = false;
    ret = NetConnService::GetInstance()->IsDeadFlowResetTargetBundle("com.test.notexist", flag);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_FALSE(flag);
}

HWTEST_F(NetConnServiceTest, IsDeadFlowResetTargetBundleTest003, TestSize.Level1)
{
    std::vector<std::string> bundleNameVec;
    int32_t ret = NetConnService::GetInstance()->UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    bool flag = false;
    ret = NetConnService::GetInstance()->IsDeadFlowResetTargetBundle("com.test.empty", flag);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_FALSE(flag);
}

HWTEST_F(NetConnServiceTest, IsDeadFlowResetTargetBundleTest004, TestSize.Level1)
{
    bool flag = false;
    int32_t ret = NetConnService::GetInstance()->IsDeadFlowResetTargetBundle("", flag);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_FALSE(flag);
}

HWTEST_F(NetConnServiceTest, DecreaseNetActivates001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    netConnService->DecreaseNetActivates(1099, callback);
    EXPECT_TRUE(netConnService->netUidActivates_.empty());
}

HWTEST_F(NetConnServiceTest, DecreaseNetActivates002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    sptr<INetConnCallback> callback1 = sptr<NetConnCallbackStubCb>::MakeSptr();
    sptr<INetConnCallback> callback2 = sptr<NetConnCallbackStubCb>::MakeSptr();
    sptr<NetSpecifier> netSpecifier = nullptr;
    auto active1 = netConnService->CreateNetActivateRequest(netSpecifier, callback1, 0, 0, 1099);
    auto active2 = netConnService->CreateNetActivateRequest(netSpecifier, callback2, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active1);
    netConnService->netUidActivates_[1099].push_back(active2);
    netConnService->DecreaseNetActivates(1099, callback2);
    EXPECT_EQ(netConnService->netUidActivates_.size(), 1);
}

HWTEST_F(NetConnServiceTest, DecreaseNetActivates003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    netConnService->netUidActivates_[1099].push_back(nullptr);
    netConnService->DecreaseNetActivates(1099, callback);
    EXPECT_FALSE(netConnService->netUidActivates_.empty());
}

HWTEST_F(NetConnServiceTest, DecreaseNetActivates004, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    sptr<NetSpecifier> netSpecifier = sptr<NetSpecifier>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, nullptr, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active);
    netConnService->DecreaseNetActivates(1099, callback);
    EXPECT_FALSE(netConnService->netUidActivates_.empty());
}

HWTEST_F(NetConnServiceTest, ActivateNetwork001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->controlFunc_ = [](const NetRequest &netRequest) -> bool { return true; };
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    sptr<NetSpecifier> netSpecifier = sptr<NetSpecifier>::MakeSptr();
    auto ret = netConnService->ActivateNetwork(netSpecifier, callback, 0, 0, 0);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, OnNetActivateTimeOut001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->OnNetActivateTimeOut(nullptr);
    EXPECT_EQ(netConnService->netConnEventHandler_, nullptr);
}

HWTEST_F(NetConnServiceTest, OnNetActivateTimeOut002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    sptr<NetSpecifier> netSpecifier = nullptr;
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netConnService->OnNetActivateTimeOut(active);
    EXPECT_EQ(netConnService->netConnEventHandler_, nullptr);
}

HWTEST_F(NetConnServiceTest, OnNetActivateTimeOut003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    ASSERT_NE(netConnService->netConnEventRunner_, nullptr);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->OnNetActivateTimeOut(nullptr);
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
}

HWTEST_F(NetConnServiceTest, OnNetActivateTimeOut004, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    ASSERT_NE(netConnService->netConnEventRunner_, nullptr);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    sptr<NetSpecifier> netSpecifier = nullptr;
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netConnService->OnNetActivateTimeOut(active);
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
}

HWTEST_F(NetConnServiceTest, OnNetActivateTimeOut005, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    ASSERT_NE(netConnService->netConnEventRunner_, nullptr);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    sptr<NetSpecifier> netSpecifier = nullptr;
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    std::set<NetCap> netCasps;
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "", netCasps);
    active->SetServiceSupply(netSupplier);
    netConnService->OnNetActivateTimeOut(active);
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
    EXPECT_TRUE(netSupplier->requestList_.empty());
}

HWTEST_F(NetConnServiceTest, FindSameCallback001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t reqId = 0;
    NetConnService::RegisterType registerType = NetConnService::RegisterType::INVALIDTYPE;
    uint32_t uid = 0;
    netConnService->netUidActivates_[1099].push_back(nullptr);
    sptr<NetSpecifier> netSpecifier = nullptr;
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active);
    bool ret = netConnService->FindSameCallback(callback, reqId, registerType, uid);
    EXPECT_EQ(uid, 1099);
    EXPECT_EQ(reqId, active->GetRequestId());
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceTest, FindSameCallback002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t reqId = 0;
    NetConnService::RegisterType registerType = NetConnService::RegisterType::INVALIDTYPE;
    uint32_t uid = 0;
    sptr<NetSpecifier> netSpecifier = nullptr;
    auto active1 = netConnService->CreateNetActivateRequest(netSpecifier, nullptr, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active1);
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active2 = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active2);
    bool ret = netConnService->FindSameCallback(callback, reqId, registerType, uid);
    EXPECT_EQ(uid, 1099);
    EXPECT_EQ(reqId, active2->GetRequestId());
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceTest, FindSameCallback003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t reqId = 0;
    NetConnService::RegisterType registerType = NetConnService::RegisterType::INVALIDTYPE;
    uint32_t uid = 0;
    sptr<NetSpecifier> netSpecifier = nullptr;
    sptr<INetConnCallback> callback1 = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active1 = netConnService->CreateNetActivateRequest(netSpecifier, callback1, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active1);
    sptr<INetConnCallback> callback2 = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active2 = netConnService->CreateNetActivateRequest(netSpecifier, callback2, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active2);
    bool ret = netConnService->FindSameCallback(callback2, reqId, registerType, uid);
    EXPECT_EQ(uid, 1099);
    EXPECT_EQ(reqId, active2->GetRequestId());
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceTest, FindSameCallback004, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t reqId = 0;
    NetConnService::RegisterType registerType = NetConnService::RegisterType::INVALIDTYPE;
    uint32_t uid = 0;
    sptr<NetSpecifier> netSpecifier = nullptr;
    sptr<INetConnCallback> callback1 = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active2 = netConnService->CreateNetActivateRequest(netSpecifier, callback1, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(nullptr);
    sptr<INetConnCallback> callback2 = sptr<NetConnCallbackStubCb>::MakeSptr();
    bool ret = netConnService->FindSameCallback(callback2, reqId, registerType, uid);
    EXPECT_EQ(uid, 0);
    EXPECT_EQ(reqId, 0);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, FindBestNetworkForAllRequest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netUidActivates_[1099].push_back(nullptr);
    netConnService->FindBestNetworkForAllRequest();
    EXPECT_EQ(netConnService->netUidActivates_.size(), 1);
}

HWTEST_F(NetConnServiceTest, FindBestNetworkForAllRequest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "Test", netCaps);
    auto network = std::make_shared<Network>(100, 1000, NetBearType::BEARER_ETHERNET, nullptr);
    network->UpdateNetConnState(NET_CONN_STATE_CONNECTED);
    netSupplier->SetNetwork(network);
    netConnService->netSuppliers_.emplace(1000, netSupplier);
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active);
    netConnService->FindBestNetworkForAllRequest();
    EXPECT_EQ(netConnService->netUidActivates_.size(), 1);
    EXPECT_EQ(active->GetServiceSupply(), netSupplier);
}

HWTEST_F(NetConnServiceTest, FindBestNetworkForAllRequest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "Test", netCaps);
    auto network = std::make_shared<Network>(100, 1000, NetBearType::BEARER_ETHERNET, nullptr);
    network->UpdateNetConnState(NET_CONN_STATE_CONNECTED);
    netSupplier->SetNetwork(network);
    netConnService->netSuppliers_.emplace(1000, netSupplier);
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    active->SetServiceSupply(netSupplier);
    netConnService->netUidActivates_[1099].push_back(active);
    netConnService->FindBestNetworkForAllRequest();
    EXPECT_EQ(netConnService->netUidActivates_.size(), 1);
    EXPECT_EQ(active->GetServiceSupply(), netSupplier);
}

HWTEST_F(NetConnServiceTest, FindBestNetworkForAllRequest004, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSupplier1 = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "Test", netCaps);
    auto netSupplier2 = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "Test", netCaps);
    netSupplier1->isAcceptUnvaliad = true;
    netSupplier2->netScore_ = 0;
    auto network = std::make_shared<Network>(100, 1000, NetBearType::BEARER_ETHERNET, nullptr);
    network->UpdateNetConnState(NET_CONN_STATE_CONNECTED);
    netSupplier1->SetNetwork(network);
    netConnService->netSuppliers_.emplace(1000, netSupplier1);
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    active->SetServiceSupply(netSupplier2);
    netConnService->netUidActivates_[1099].push_back(active);
    netConnService->FindBestNetworkForAllRequest();
    EXPECT_EQ(netConnService->netUidActivates_.size(), 1);
    EXPECT_EQ(active->GetServiceSupply(), netSupplier1);
}

HWTEST_F(NetConnServiceTest, RequestAllNetworkExceptDefault001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    netConnService->defaultNetSupplier_ = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "Test", netCaps);
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netConnService->defaultNetActivate_  = netConnService->CreateNetActivateRequest(netSpecifier, nullptr, 0, 0, 1099);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    netSupplier->isAcceptUnvaliad = true;
    netConnService->netSuppliers_.emplace(1000, netSupplier);
    EXPECT_TRUE(netSupplier->requestList_.empty());
    netConnService->RequestAllNetworkExceptDefault();
    EXPECT_FALSE(netSupplier->requestList_.empty());
}

HWTEST_F(NetConnServiceTest, SendAllRequestToNetwork001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netUidActivates_[1099].push_back(nullptr);
    auto netSpecifier1 = sptr<NetSpecifier>::MakeSptr();
    netSpecifier1->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier1->SetType(BEARER_ETHERNET);
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active1 = netConnService->CreateNetActivateRequest(netSpecifier1, callback, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active1);
    auto netSpecifier2 = sptr<NetSpecifier>::MakeSptr();
    netSpecifier2->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier2->SetType(BEARER_WIFI);
    auto active2 = netConnService->CreateNetActivateRequest(netSpecifier2, callback, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active2);
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    EXPECT_TRUE(netSupplier->requestList_.empty());
    netConnService->SendAllRequestToNetwork(netSupplier);
    EXPECT_FALSE(netSupplier->requestList_.empty());
}

HWTEST_F(NetConnServiceTest, CallbackForSupplier001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier->SetType(BEARER_WIFI);
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    netConnService->CallbackForSupplier(netSupplier, CALL_TYPE_AVAILABLE);
    EXPECT_FALSE(netSupplier->HasBestRequest(active->GetRequestId()));
    EXPECT_NE(active->GetNetCallback(), nullptr);
}

HWTEST_F(NetConnServiceTest, CallbackForSupplier002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier->SetType(BEARER_WIFI);
    sptr<NetConnCallbackStubCb> callback = nullptr;
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    netConnService->CallbackForSupplier(netSupplier, CALL_TYPE_AVAILABLE);
    EXPECT_FALSE(netSupplier->HasBestRequest(active->GetRequestId()));
    EXPECT_EQ(active->GetNetCallback(), nullptr);
}

HWTEST_F(NetConnServiceTest, CallbackForSupplier003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier->SetType(BEARER_WIFI);
    sptr<NetConnCallbackStubCb> callback = nullptr;
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    netSupplier->AddBestRequest(active->GetRequestId());
    netConnService->CallbackForSupplier(netSupplier, CALL_TYPE_AVAILABLE);
    EXPECT_TRUE(netSupplier->HasBestRequest(active->GetRequestId()));
    EXPECT_EQ(active->GetNetCallback(), nullptr);
}

HWTEST_F(NetConnServiceTest, CallbackForSupplier004, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier->SetType(BEARER_WIFI);
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    netSupplier->AddBestRequest(active->GetRequestId());
    netConnService->CallbackForSupplier(netSupplier, CALL_TYPE_LOST);
    EXPECT_TRUE(netSupplier->HasBestRequest(active->GetRequestId()));
    EXPECT_NE(active->GetNetCallback(), nullptr);
    EXPECT_FALSE(netConnService->FindNotifyLostDelayCache(netSupplier->GetNetId()));
    EXPECT_FALSE(netConnService->CheckNotifyLostDelay(active, netSupplier->GetNetId(), CALL_TYPE_LOST));
}

HWTEST_F(NetConnServiceTest, CallbackForSupplier005, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier->SetType(BEARER_WIFI);
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    netSupplier->AddBestRequest(active->GetRequestId());
    netConnService->notifyLostDelayCache_.EnsureInsert(0, true);
    netConnService->CallbackForSupplier(netSupplier, CALL_TYPE_LOST);
    EXPECT_TRUE(netSupplier->HasBestRequest(active->GetRequestId()));
    EXPECT_NE(active->GetNetCallback(), nullptr);
    EXPECT_TRUE(netConnService->FindNotifyLostDelayCache(netSupplier->GetNetId()));
    EXPECT_FALSE(netConnService->CheckNotifyLostDelay(active, netSupplier->GetNetId(), CALL_TYPE_LOST));
}

HWTEST_F(NetConnServiceTest, CallbackForSupplier006, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier->SetType(BEARER_WIFI);
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    netSupplier->AddBestRequest(active->GetRequestId());
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    ASSERT_NE(netConnService->netConnEventRunner_, nullptr);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->uidLostDelaySet_.insert(active->GetUid());
    netConnService->CallbackForSupplier(netSupplier, CALL_TYPE_LOST);
    EXPECT_TRUE(netSupplier->HasBestRequest(active->GetRequestId()));
    EXPECT_NE(active->GetNetCallback(), nullptr);
    EXPECT_FALSE(netConnService->FindNotifyLostDelayCache(netSupplier->GetNetId()));
    EXPECT_TRUE(netConnService->CheckNotifyLostDelay(active, netSupplier->GetNetId(), CALL_TYPE_LOST));
}

HWTEST_F(NetConnServiceTest, CallbackForSupplier007, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier->SetType(BEARER_WIFI);
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    netSupplier->AddBestRequest(active->GetRequestId());
    netConnService->notifyLostDelayCache_.EnsureInsert(0, true);
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    ASSERT_NE(netConnService->netConnEventRunner_, nullptr);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->uidLostDelaySet_.insert(active->GetUid());
    netConnService->CallbackForSupplier(netSupplier, CALL_TYPE_LOST);
    EXPECT_TRUE(netSupplier->HasBestRequest(active->GetRequestId()));
    EXPECT_NE(active->GetNetCallback(), nullptr);
    EXPECT_TRUE(netConnService->FindNotifyLostDelayCache(netSupplier->GetNetId()));
    EXPECT_TRUE(netConnService->CheckNotifyLostDelay(active, netSupplier->GetNetId(), CALL_TYPE_LOST));
}

HWTEST_F(NetConnServiceTest, IsSupplierMatchRequestAndNetwork001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    bool ret = netConnService->IsSupplierMatchRequestAndNetwork(netSupplier);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, IsSupplierMatchRequestAndNetwork002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapability(NET_CAPABILITY_INTERNET);
    netSpecifier->SetType(BEARER_WIFI);
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    bool ret = netConnService->IsSupplierMatchRequestAndNetwork(netSupplier);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceTest, IsSupplierMatchRequestAndNetwork003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::set<NetCap> netCaps = { NET_CAPABILITY_INTERNET };
    auto netSupplier = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_WIFI, "Test", netCaps);
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(nullptr, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    bool ret = netConnService->IsSupplierMatchRequestAndNetwork(netSupplier);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceTest, RegisterNetRequestControlFunc001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    bool ret = netConnService->RegisterNetRequestControlFunc([](const NetRequest& netRequest) -> bool { return true; });
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceTest, GetAllNetRequest001, TestSize.Level1)
{
    std::vector<NetRequest> netRequestList;
    NetRequest netRequest;
    netRequestList.push_back(netRequest);
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netRequestList.empty());
    netConnService->GetAllNetRequest(netRequestList);
    EXPECT_TRUE(netRequestList.empty());
}

HWTEST_F(NetConnServiceTest, GetAllNetRequest002, TestSize.Level1)
{
    std::vector<NetRequest> netRequestList;
    NetRequest netRequest;
    netRequestList.push_back(netRequest);
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates;
    netConnService->netUidActivates_.emplace(1099, netActivates);
    EXPECT_FALSE(netRequestList.empty());
    netConnService->GetAllNetRequest(netRequestList);
    EXPECT_TRUE(netRequestList.empty());
}

HWTEST_F(NetConnServiceTest, GetAllNetRequest003, TestSize.Level1)
{
    std::vector<NetRequest> netRequestList;
    NetRequest netRequest;
    netRequestList.push_back(netRequest);
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    netConnService->netUidActivates_.emplace(1099, netActivates);
    EXPECT_FALSE(netRequestList.empty());
    netConnService->GetAllNetRequest(netRequestList);
    EXPECT_TRUE(netRequestList.empty());
}

HWTEST_F(NetConnServiceTest, GetAllNetRequest004, TestSize.Level1)
{
    std::vector<NetRequest> netRequestList;
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    netConnService->netUidActivates_.emplace(1099, netActivates);
    sptr<NetSpecifier> netSpecifier = nullptr;
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netConnService->netUidActivates_[1099].push_back(active);
    EXPECT_TRUE(netRequestList.empty());
    netConnService->GetAllNetRequest(netRequestList);
    EXPECT_FALSE(netRequestList.empty());
}

HWTEST_F(NetConnServiceTest, UpdateNetRequestControlState001, TestSize.Level1)
{
    std::vector<NetRequest> netRequestList;
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->UpdateNetRequestControlState(netRequestList);
    EXPECT_TRUE(netConnService->netUidActivates_.empty());
}

HWTEST_F(NetConnServiceTest, UpdateNetRequestControlState002, TestSize.Level1)
{
    std::vector<NetRequest> netRequestList;
    NetRequest netRequest;
    netRequest.uid = 1099;
    netRequestList.push_back(netRequest);
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->UpdateNetRequestControlState(netRequestList);
    EXPECT_TRUE(netConnService->netUidActivates_.empty());
}

HWTEST_F(NetConnServiceTest, UpdateNetRequestControlState003, TestSize.Level1)
{
    std::vector<NetRequest> netRequestList;
    NetRequest netRequest;
    netRequest.uid = 1099;
    netRequestList.push_back(netRequest);
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates;
    netConnService->netUidActivates_.emplace(1099, netActivates);
    netConnService->UpdateNetRequestControlState(netRequestList);
    EXPECT_FALSE(netConnService->netUidActivates_.empty());
}

HWTEST_F(NetConnServiceTest, UpdateNetRequestControlState004, TestSize.Level1)
{
    std::vector<NetRequest> netRequestList;
    NetRequest netRequest;
    netRequest.uid = 1099;
    netRequest.isControlled = true;
    netRequestList.push_back(netRequest);
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<std::shared_ptr<NetActivate>> netActivates = {nullptr};
    sptr<NetSpecifier> netSpecifier = nullptr;
    sptr<INetConnCallback> callback = sptr<NetConnCallbackStubCb>::MakeSptr();
    auto active = netConnService->CreateNetActivateRequest(netSpecifier, callback, 0, 0, 1099);
    netActivates.push_back(active);
    netConnService->netUidActivates_.emplace(1099, netActivates);
    EXPECT_FALSE(active->GetNetRequest().isControlled);
    netConnService->UpdateNetRequestControlState(netRequestList);
    EXPECT_TRUE(active->GetNetRequest().isControlled);
    EXPECT_FALSE(netConnService->netUidActivates_.empty());
}
} // namespace NetManagerStandard
} // namespace OHOS

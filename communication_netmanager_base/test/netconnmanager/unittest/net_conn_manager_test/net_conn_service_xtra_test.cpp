/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include <curl/curl.h>

#include "common_net_conn_callback_test.h"
#include "http_proxy.h"
#include "ipc_skeleton.h"
#include "net_all_capabilities.h"
#include "net_conn_callback_stub.h"
#include "net_conn_client.h"
#include "net_conn_constants.h"
#include "net_conn_service.h"
#include "net_conn_types.h"
#include "net_detection_callback_test.h"
#include "net_factoryreset_callback_stub.h"
#include "net_http_proxy_tracker.h"
#include "net_interface_callback_stub.h"
#include "net_manager_center.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_test_security.h"
#include "netsys_controller.h"
#include "system_ability_definition.h"
#include "common_mock_net_remote_object_test.h"
#include "parameter.h"
#include "parameters.h"

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

sptr<INetConnCallback> g_callback = new (std::nothrow) NetConnCallbackStubCb();
sptr<INetDetectionCallback> g_detectionCallback = new (std::nothrow) NetDetectionCallbackTest();
uint32_t g_supplierId = 0;
uint32_t g_vpnSupplierId = 0;
} // namespace

class NetConnServiceExtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetConnServiceExtTest::SetUpTestCase() {}

void NetConnServiceExtTest::TearDownTestCase() {}

void NetConnServiceExtTest::SetUp() {}

void NetConnServiceExtTest::TearDown() {}

HWTEST_F(NetConnServiceExtTest, CheckIfSettingsDataReadyTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->isDataShareReady_ = true;
    auto ret = netConnService->CheckIfSettingsDataReady();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceExtTest, CheckIfSettingsDataReadyTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->isDataShareReady_ = false;
    auto ret = netConnService->CheckIfSettingsDataReady();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceExtTest, OnNetSupplierRemoteDiedTest001, TestSize.Level1)
{
    wptr<IRemoteObject> remoteObject = nullptr;
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    netConnService->OnNetSupplierRemoteDied(remoteObject);
}

HWTEST_F(NetConnServiceExtTest, OnNetSupplierRemoteDiedTest002, TestSize.Level1)
{
    wptr<IRemoteObject> remoteObject = new MockNetIRemoteObject();
    EXPECT_NE(remoteObject, nullptr);
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->OnNetSupplierRemoteDied(remoteObject);
}

HWTEST_F(NetConnServiceExtTest, OnNetSupplierRemoteDiedTest003, TestSize.Level1)
{
    wptr<IRemoteObject> remoteObject = new MockNetIRemoteObject();
    EXPECT_NE(remoteObject, nullptr);
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->OnNetSupplierRemoteDied(remoteObject);
}

HWTEST_F(NetConnServiceExtTest, RemoveNetSupplierDeathRecipientTest002, TestSize.Level1)
{
    sptr<INetSupplierCallback> callback = nullptr;
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    netConnService->RemoveNetSupplierDeathRecipient(callback);
}

HWTEST_F(NetConnServiceExtTest, RequestNetConnectionAsyncTest002, TestSize.Level1)
{
    uint32_t callingUid = 1;
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    sptr<INetConnCallback> uidCallback = nullptr;
    auto netConnService = std::make_shared<NetConnService>();
    int32_t ret = netConnService->RequestNetConnectionAsync(netSpecifier, uidCallback, 0, callingUid);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceExtTest, UnregisterNetSupplierAsyncTest001, TestSize.Level1)
{
    uint32_t supplierId = 1;
    int32_t callingUid = 1;
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> netSupplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    netConnService->netSuppliers_[supplierId] = netSupplier;
    auto result = netConnService->FindNetSupplier(supplierId);
    EXPECT_EQ(result, netSupplier);
    netConnService->defaultNetSupplier_ = netSupplier;
    bool ignoreUid = true;
    auto ret = netConnService->UnregisterNetSupplierAsync(supplierId, ignoreUid, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, UnregisterNetSupplierAsyncTest002, TestSize.Level1)
{
    uint32_t supplierId = 1;
    int32_t callingUid = 1;
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> netSupplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    netConnService->netSuppliers_[supplierId] = netSupplier;
    auto result = netConnService->FindNetSupplier(supplierId);
    EXPECT_EQ(result, netSupplier);
    bool ignoreUid = false;
    netConnService->defaultNetSupplier_ = nullptr;
    EXPECT_EQ(netConnService->defaultNetSupplier_, nullptr);
    auto ret = netConnService->UnregisterNetSupplierAsync(supplierId, ignoreUid, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, ReportFaultEventIfUidNotMatchTest001, TestSize.Level1)
{
    uint32_t supplierId = 1;
    int32_t callingUid = 2;
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> netSupplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    netConnService->netSuppliers_[supplierId] = netSupplier;
    auto supplier = netConnService->FindNetSupplier(supplierId);
    EXPECT_EQ(supplier, netSupplier);
    int32_t uid = netSupplier->GetUid();
    EXPECT_NE(uid, callingUid);
    netConnService->ReportFaultEventIfUidNotMatch(supplier, callingUid);
}

HWTEST_F(NetConnServiceExtTest, HandleScreenEventTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->defaultNetSupplier_ = nullptr;
    EXPECT_EQ(netConnService->defaultNetSupplier_, nullptr);
    netConnService->HandleScreenEvent(true);
}

HWTEST_F(NetConnServiceExtTest, UpdateNetCapsAsyncTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::set<NetCap> netCaps;
    uint32_t supplierId = 0;
    EXPECT_EQ(netConnService->defaultNetSupplier_, nullptr);
    auto supplier = netConnService->FindNetSupplier(supplierId);
    EXPECT_EQ(supplier, nullptr);
    auto ret = netConnService->UpdateNetCapsAsync(netCaps, supplierId);
    EXPECT_EQ(ret, NET_CONN_ERR_NO_SUPPLIER);
}

HWTEST_F(NetConnServiceExtTest, UpdateNetCapsAsyncTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::set<NetCap> netCaps;
    uint32_t supplierId = 1;
    std::string netSupplierIdent;
    sptr<NetSupplier> netSupplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    netConnService->netSuppliers_[supplierId] = netSupplier;
    auto result = netConnService->FindNetSupplier(supplierId);
    EXPECT_EQ(result, netSupplier);
    auto supplier = netConnService->FindNetSupplier(supplierId);
    EXPECT_EQ(supplier, netSupplier);
    auto network = supplier->GetNetwork();
    EXPECT_EQ(network, nullptr);
    auto ret = netConnService->UpdateNetCapsAsync(netCaps, supplierId);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceExtTest, NetDetectionForDnsHealthSyncTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    int32_t netId = 0;
    auto ret = netConnService->NetDetectionForDnsHealthSync(netId, true);
    EXPECT_EQ(ret, NET_CONN_ERR_NETID_NOT_FOUND);
}

HWTEST_F(NetConnServiceExtTest, NetDetectionForDnsHealthSyncTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    int32_t netId = 1;
    auto ret = netConnService->NetDetectionForDnsHealthSync(netId, true);
    EXPECT_EQ(ret, NET_CONN_ERR_NETID_NOT_FOUND);
}

HWTEST_F(NetConnServiceExtTest, RestrictBackgroundChangedAsyncTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netSuppliers_.clear();
    netConnService->netSuppliers_[0] = nullptr;
    auto ret = netConnService->RestrictBackgroundChangedAsync(true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, RequestAllNetworkExceptDefaultTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->defaultNetSupplier_ = nullptr;
    EXPECT_FALSE(netConnService->registerToService_);
    netConnService->RequestAllNetworkExceptDefault();
}

HWTEST_F(NetConnServiceExtTest, RequestAllNetworkExceptDefaultTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->defaultNetSupplier_ = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_NE(netConnService->defaultNetSupplier_, nullptr);
    netConnService->RequestAllNetworkExceptDefault();
}

HWTEST_F(NetConnServiceExtTest, GenerateNetIdTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netIdLastValue_ = MAX_NET_ID;
    netConnService->defaultNetSupplier_ = nullptr;
    EXPECT_EQ(netConnService->defaultNetSupplier_, nullptr);
    auto ret = netConnService->GenerateNetId();
    EXPECT_EQ(ret, MIN_NET_ID);
}

HWTEST_F(NetConnServiceExtTest, GenerateInternalNetIdTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->internalNetIdLastValue_ = MAX_NET_ID;
    auto ret = netConnService->GenerateInternalNetId();
    EXPECT_EQ(ret, MIN_INTERNAL_NET_ID);
}

HWTEST_F(NetConnServiceExtTest, GenerateInternalNetIdTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->internalNetIdLastValue_ = MAX_NET_ID;
    int32_t netId = MIN_INTERNAL_NET_ID;
    int32_t supplierId = 99;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_ETHERNET, nullptr);
    std::set<NetCap> netCasps;
    netConnService->netSuppliers_[supplierId] = sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_ETHERNET, "", netCasps);
    netConnService->netSuppliers_[supplierId]->SetNetwork(network);
    auto ret = netConnService->GenerateInternalNetId();
    EXPECT_EQ(ret, 2);
}

HWTEST_F(NetConnServiceExtTest, NotFindBestSupplierTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    netConnService->NotFindBestSupplier(1, nullptr, nullptr, nullptr);
}

HWTEST_F(NetConnServiceExtTest, NotFindBestSupplierTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t reqId = 1;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_NE(supplier, nullptr);
    std::vector<std::shared_ptr<NetActivate>> activates;
    sptr<NetSpecifier> specifier = new (std::nothrow) NetSpecifier();
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    uint32_t uid = 1099;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, uid, handler);
    netConnService->NotFindBestSupplier(reqId, active, supplier, callback);
    int32_t netId = 123;
    netConnService->notifyLostDelayCache_.EnsureInsert(netId, true);
    netConnService->uidLostDelaySet_.insert(uid);
    netConnService->NotFindBestSupplier(reqId, active, supplier, callback);
    netConnService->uidLostDelaySet_.clear();
    netConnService->notifyLostDelayCache_.Clear();
}

HWTEST_F(NetConnServiceExtTest, NotFindBestSupplierTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_NE(supplier, nullptr);
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    netConnService->NotFindBestSupplier(1, nullptr, supplier, callback);
}

HWTEST_F(NetConnServiceExtTest, HandleCallbackTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    CallbackType type = CALL_TYPE_UPDATE_LINK;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_EQ(supplier->network_, nullptr);
    sptr<NetHandle> netHandle = nullptr;
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    netConnService->HandleCallback(supplier, netHandle, callback, type);
}

HWTEST_F(NetConnServiceExtTest, HandleCallbackTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    CallbackType type = CALL_TYPE_UNAVAILABLE;
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    EXPECT_NE(callback, nullptr);
    sptr<NetSupplier> supplier = nullptr;
    sptr<NetHandle> netHandle = nullptr;
    netConnService->HandleCallback(supplier, netHandle, callback, type);
}

HWTEST_F(NetConnServiceExtTest, CallbackForAvailableTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_EQ(supplier->network_, nullptr);
    netConnService->CallbackForAvailable(supplier, nullptr);
}

HWTEST_F(NetConnServiceExtTest, CallbackForAvailableTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_EQ(supplier->network_, nullptr);
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    netConnService->CallbackForAvailable(supplier, callback);
}

HWTEST_F(NetConnServiceExtTest, MakeDefaultNetWorkTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> oldSupplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_EQ(oldSupplier->network_, nullptr);
    sptr<NetSupplier> newSupplier = nullptr;
    netConnService->MakeDefaultNetWork(oldSupplier, newSupplier);
}

HWTEST_F(NetConnServiceExtTest, GetNetSupplierFromListTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netSuppliers_[1] = nullptr;
    std::string ident;
    auto ret = netConnService->GetNetSupplierFromList(BEARER_CELLULAR, ident);
    EXPECT_TRUE(ret.empty());
}

HWTEST_F(NetConnServiceExtTest, GetNetSupplierFromListTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netSuppliers_[1] = nullptr;
    std::string ident;
    std::set<NetCap> netCaps;
    auto ret = netConnService->GetNetSupplierFromList(BEARER_CELLULAR, ident, netCaps);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(NetConnServiceExtTest, GetSpecificNetTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    NetBearType bearerType = static_cast<NetBearType>(-1);
    std::list<int32_t> netIdList;
    auto ret = netConnService->GetSpecificNet(bearerType, netIdList);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);
}

HWTEST_F(NetConnServiceExtTest, GetSpecificNetTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netSuppliers_[1] = nullptr;
    NetBearType bearerType = BEARER_CELLULAR;
    std::list<int32_t> netIdList;
    auto ret = netConnService->GetSpecificNet(bearerType, netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, GetSpecificNetByIdentTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    NetBearType bearerType = static_cast<NetBearType>(-1);
    std::string ident;
    std::list<int32_t> netIdList;
    auto ret = netConnService->GetSpecificNetByIdent(bearerType, ident, netIdList);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);
}

HWTEST_F(NetConnServiceExtTest, GetSpecificNetByIdentTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netSuppliers_[1] = nullptr;
    NetBearType bearerType = BEARER_CELLULAR;
    std::string ident;
    std::list<int32_t> netIdList;
    auto ret = netConnService->GetSpecificNetByIdent(bearerType, ident, netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, GetAllNetsTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_EQ(netConnService->netConnEventHandler_, nullptr);
    std::list<int32_t> netIdList;
    auto ret = netConnService->GetAllNets(netIdList);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, GetConnectionPropertiesTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_EQ(netConnService->netConnEventHandler_, nullptr);
    NetLinkInfo info;
    auto ret = netConnService->GetConnectionProperties(0, info);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceExtTest, GetIfaceNamesTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::list<std::string> ifaceNames;
    auto ret = netConnService->GetIfaceNames(static_cast<NetBearType>(-1), ifaceNames);
    EXPECT_EQ(ret, NET_CONN_ERR_NET_TYPE_NOT_FOUND);
}

HWTEST_F(NetConnServiceExtTest, GetNetIdByIdentifierTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::list<int32_t> netIdList;
    netConnService->netSuppliers_[0] = nullptr;
    auto ret = netConnService->GetNetIdByIdentifier(TEST_IDENT, netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, GetDumpMessageTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->defaultNetSupplier_ = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_NE(netConnService->defaultNetSupplier_, nullptr);
    EXPECT_EQ(netConnService->defaultNetSupplier_->network_, nullptr);
    netConnService->dnsResultCallback_ = new NetDnsResultCallback();
    std::string message;
    netConnService->GetDumpMessage(message);
}

HWTEST_F(NetConnServiceExtTest, IsValidDecValueTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string inputValue = "inputValue test";
    auto ret = netConnService->IsValidDecValue(inputValue);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceExtTest, IsValidDecValueTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string inputValue = "input";
    auto ret = netConnService->IsValidDecValue(inputValue);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceExtTest, SetAirplaneModeTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_TRUE(netConnService->preAirplaneCallbacks_.empty());
    netConnService->preAirplaneCallbacks_[0] = new IPreAirplaneCallbackStubTestCb();
    netConnService->preAirplaneCallbacks_[1] = nullptr;
    auto ret = netConnService->SetAirplaneMode(true);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceExtTest, SetCurlOptionsTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    HttpProxy tempProxy;
    netConnService->SetCurlOptions(nullptr, tempProxy);
}

HWTEST_F(NetConnServiceExtTest, SetCurlOptionsTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    CURL *curl = curl_easy_init();
    ASSERT_NE(curl, nullptr);
    HttpProxy tempProxy;
    tempProxy.host_ = "http://127.0.0.1";
    tempProxy.port_ = 8080;
    SecureData username;
    username.append("testuser", strlen("testuser"));
    tempProxy.username_ = username;
    SecureData password;
    password.append("testpass", strlen("testpass"));
    tempProxy.password_ = password;
    netConnService->SetCurlOptions(curl, tempProxy);
    curl_easy_cleanup(curl);
}

HWTEST_F(NetConnServiceExtTest, SetCurlOptionsTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    CURL *curl = curl_easy_init();
    ASSERT_NE(curl, nullptr);
    HttpProxy tempProxy;
    tempProxy.host_ = "http://127.0.0.1";
    tempProxy.port_ = 8080;
    SecureData username;
    username.append("testuser", strlen("testuser"));
    tempProxy.username_ = username;
    netConnService->SetCurlOptions(curl, tempProxy);
    curl_easy_cleanup(curl);
}

HWTEST_F(NetConnServiceExtTest, GetHttpUrlFromConfigTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    std::string httpUrl;
    netConnService->GetHttpUrlFromConfig(httpUrl);
}

HWTEST_F(NetConnServiceExtTest, IsValidUserIdTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->IsValidUserId(-1);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceExtTest, GetValidUserIdFromProxyTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    HttpProxy httpProxy;
    httpProxy.SetUserId(NetConnService::ROOT_USER_ID);
    netConnService->GetValidUserIdFromProxy(httpProxy);
}

HWTEST_F(NetConnServiceExtTest, GetValidUserIdFromProxyTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    HttpProxy httpProxy;
    httpProxy.SetUserId(NetConnService::INVALID_USER_ID);
    netConnService->GetValidUserIdFromProxy(httpProxy);
}

HWTEST_F(NetConnServiceExtTest, NetDetectionForDnsHealthTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    auto ret = netConnService->NetDetectionForDnsHealth(1, true);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, NetDetectionForDnsHealthTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    auto ret = netConnService->NetDetectionForDnsHealth(1, true);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, LoadGlobalHttpProxyTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    NetConnService::UserIdType userIdType = NetConnService::UserIdType::LOCAL;
    HttpProxy httpProxy;
    netConnService->LoadGlobalHttpProxy(userIdType, httpProxy);
}

HWTEST_F(NetConnServiceExtTest, LoadGlobalHttpProxyTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    NetConnService::UserIdType userIdType = static_cast<NetConnService::UserIdType>(-1);
    HttpProxy httpProxy;
    netConnService->LoadGlobalHttpProxy(userIdType, httpProxy);
}

HWTEST_F(NetConnServiceExtTest, LoadGlobalHttpProxyTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    NetConnService::UserIdType userIdType = NetConnService::UserIdType::SPECIFY;
    HttpProxy httpProxy;
    httpProxy.SetUserId(1);
    netConnService->LoadGlobalHttpProxy(userIdType, httpProxy);
}

HWTEST_F(NetConnServiceExtTest, UpdateGlobalHttpProxyTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    HttpProxy httpProxy;
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
    netConnService->UpdateGlobalHttpProxy(httpProxy);
}

HWTEST_F(NetConnServiceExtTest, OnInterfaceAddressUpdatedTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    stateCallback.ifaceStateCallbacks_.push_back(callback);
    std::string testString = "test";
    int testInt = 0;
    auto ret = stateCallback.OnInterfaceAddressUpdated(testString, testString, testInt, testInt);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, OnInterfaceAddressRemovedTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    stateCallback.ifaceStateCallbacks_.push_back(callback);
    std::string testString = "test";
    int testInt = 0;
    auto ret = stateCallback.OnInterfaceAddressRemoved(testString, testString, testInt, testInt);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, OnInterfaceAddedTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    stateCallback.ifaceStateCallbacks_.push_back(callback);
    std::string testString = "test";
    auto ret = stateCallback.OnInterfaceAdded(testString);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, OnInterfaceRemovedTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    stateCallback.ifaceStateCallbacks_.push_back(callback);
    std::string testString = "test";
    auto ret = stateCallback.OnInterfaceRemoved(testString);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, OnInterfaceChangedTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    stateCallback.ifaceStateCallbacks_.push_back(callback);
    std::string testString = "test";
    auto ret = stateCallback.OnInterfaceChanged(testString, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, OnInterfaceLinkStateChangedTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    stateCallback.ifaceStateCallbacks_.push_back(callback);
    std::string testString = "test";
    auto ret = stateCallback.OnInterfaceLinkStateChanged(testString, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, OnRouteChangedTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    stateCallback.ifaceStateCallbacks_.push_back(callback);
    EXPECT_EQ(stateCallback.ifaceStateCallbacks_.size(), 2);
    std::string testString = "test";
    auto ret = stateCallback.OnRouteChanged(false, testString, testString, testString);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, RegisterInterfaceCallbackTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    EXPECT_NE(callback, nullptr);
    auto ret = stateCallback.RegisterInterfaceCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, RegisterInterfaceCallbackTest002, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    stateCallback.ifaceStateCallbacks_.push_back(nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    stateCallback.ifaceStateCallbacks_.push_back(callback);
    EXPECT_EQ(stateCallback.ifaceStateCallbacks_.size(), 2);
    std::string testString = "test";
    int testInt = 0;
    auto ret = stateCallback.RegisterInterfaceCallback(callback);
    EXPECT_EQ(ret, NET_CONN_ERR_SAME_CALLBACK);
}

HWTEST_F(NetConnServiceExtTest, OnNetIfaceStateRemoteDiedTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    EXPECT_TRUE(stateCallback.ifaceStateCallbacks_.empty());
    wptr<IRemoteObject> remoteObject = nullptr;
    stateCallback.OnNetIfaceStateRemoteDied(remoteObject);
}

HWTEST_F(NetConnServiceExtTest, OnNetIfaceStateRemoteDiedTest002, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    wptr<IRemoteObject> remoteObject = new MockNetIRemoteObject();
    EXPECT_NE(remoteObject, nullptr);
    stateCallback.OnNetIfaceStateRemoteDied(remoteObject);
}

HWTEST_F(NetConnServiceExtTest, AddIfaceDeathRecipientTest001, TestSize.Level1)
{
    NetConnService::NetInterfaceStateCallback stateCallback;
    stateCallback.netIfaceStateDeathRecipient_ = new (std::nothrow)
        NetConnService::NetInterfaceStateCallback::NetIfaceStateCallbackDeathRecipient(stateCallback);
    EXPECT_NE(stateCallback.netIfaceStateDeathRecipient_, nullptr);
    sptr<INetInterfaceStateCallback> callback = new (std::nothrow) NetInterfaceStateCallbackStub();
    EXPECT_NE(callback, nullptr);
    stateCallback.AddIfaceDeathRecipient(callback);
}

HWTEST_F(NetConnServiceExtTest, NetUidPolicyChangeTest001, TestSize.Level1)
{
    std::weak_ptr<NetConnService> netConnService;
    netConnService.reset();
    NetConnService::NetPolicyCallback policyCallback(netConnService);
    auto ret = policyCallback.NetUidPolicyChange(1, 1);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, NetUidPolicyChangeTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->defaultNetSupplier_ = sptr<NetSupplier>::MakeSptr(BEARER_CELLULAR, netSupplierIdent, netCaps);
    NetConnService::NetPolicyCallback policyCallback(netConnService);
    EXPECT_NE(policyCallback.netConnService_.lock(), nullptr);
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
    auto ret = policyCallback.NetUidPolicyChange(1, 1);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, NetUidPolicyChangeTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    NetConnService::NetPolicyCallback policyCallback(netConnService);
    EXPECT_NE(policyCallback.netConnService_.lock(), nullptr);
    auto ret = policyCallback.NetUidPolicyChange(1, 1);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, NetUidPolicyChangeTest004, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    NetConnService::NetPolicyCallback policyCallback(netConnService);
    EXPECT_NE(policyCallback.netConnService_.lock(), nullptr);
    netConnService->defaultNetSupplier_ = nullptr;
    auto ret = policyCallback.NetUidPolicyChange(1, 1);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, SendNetPolicyChangeTest001, TestSize.Level1)
{
    std::weak_ptr<NetConnService> netConnService;
    netConnService.reset();
    NetConnService::NetPolicyCallback policyCallback(netConnService);
    EXPECT_EQ(policyCallback.netConnService_.lock(), nullptr);
    policyCallback.NetUidPolicyChange(1, 1);
}

HWTEST_F(NetConnServiceExtTest, SendNetPolicyChangeTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    NetConnService::NetPolicyCallback policyCallback(netConnService);
    EXPECT_NE(policyCallback.netConnService_.lock(), nullptr);
    policyCallback.NetUidPolicyChange(1, 1);
}

HWTEST_F(NetConnServiceExtTest, SendNetPolicyChangeTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->defaultNetSupplier_ = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_NE(netConnService->defaultNetSupplier_, nullptr);
    NetConnService::NetPolicyCallback policyCallback(netConnService);
    policyCallback.NetUidPolicyChange(1, 1);
}

HWTEST_F(NetConnServiceExtTest, OnAddSystemAbilityTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->hasSARemoved_);
    std::string deviceId = "dev1";
    netConnService->OnAddSystemAbility(COMM_NETSYS_NATIVE_SYS_ABILITY_ID, deviceId);
}

HWTEST_F(NetConnServiceExtTest, OnAddSystemAbilityTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_FALSE(netConnService->registerToService_);
    std::string deviceId = "dev1";
    netConnService->OnAddSystemAbility(ACCESS_TOKEN_MANAGER_SERVICE_ID, deviceId);

    EXPECT_TRUE(netConnService->registerToService_);
    netConnService->OnAddSystemAbility(ACCESS_TOKEN_MANAGER_SERVICE_ID, deviceId);
}

HWTEST_F(NetConnServiceExtTest, OnRemoveSystemAbilityTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string deviceId = "dev1";
    netConnService->OnRemoveSystemAbility(COMM_NET_POLICY_MANAGER_SYS_ABILITY_ID, deviceId);
    EXPECT_FALSE(netConnService->hasSARemoved_);
}

HWTEST_F(NetConnServiceExtTest, IsSupplierMatchRequestAndNetworkTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->defaultNetActivate_ = nullptr;
    netConnService->CreateDefaultRequest();
    bool ret = netConnService->IsSupplierMatchRequestAndNetwork(netConnService->defaultNetSupplier_);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceExtTest, RecoverNetSysTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->defaultNetSupplier_ = sptr<NetSupplier>::MakeSptr(BEARER_CELLULAR, netSupplierIdent, netCaps);
    netConnService->netSuppliers_[0] = nullptr;
    netConnService->netSuppliers_[1] = netConnService->defaultNetSupplier_;
    netConnService->RecoverNetSys();
    EXPECT_NE(netConnService->netSuppliers_.size(), 0);
}

HWTEST_F(NetConnServiceExtTest, RecoverNetSysTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    EXPECT_EQ(netConnService->defaultNetSupplier_, nullptr);
    netConnService->netSuppliers_[1] = nullptr;
    netConnService->RecoverNetSys();
}

HWTEST_F(NetConnServiceExtTest, RegisterSlotTypeTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t supplierId = 10;
    int32_t type = 0;
    auto ret = netConnService->RegisterSlotType(supplierId, type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
    ret = netConnService->RegisterSlotType(supplierId, type);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    supplierId = 1;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->netSuppliers_[1] = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    ret = netConnService->RegisterSlotType(supplierId, type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, GetSlotTypeTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->netSuppliers_.emplace(1, sptr<NetSupplier>::MakeSptr(BEARER_CELLULAR, netSupplierIdent, netCaps));
    std::string type;
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
    EXPECT_EQ(netConnService->defaultNetSupplier_, nullptr);
    auto ret = netConnService->GetSlotType(type);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    netConnService->defaultNetSupplier_ = netConnService->netSuppliers_[1];
    EXPECT_NE(netConnService->defaultNetSupplier_, nullptr);
    ret = netConnService->GetSlotType(type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    netConnService->netConnEventHandler_ = nullptr;
    ret = netConnService->GetSlotType(type);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, OnNetSysRestartTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->OnNetSysRestart();
    EXPECT_EQ(netConnService->netConnEventHandler_, nullptr);
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->OnNetSysRestart();
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
}

HWTEST_F(NetConnServiceExtTest, IsIfaceNameInUseTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    int32_t netId = 1;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    supplier->network_ = network;
    supplier->netSupplierInfo_.isAvailable_ = true;
    supplier->network_->netLinkInfo_.ifaceName_ = "rmnet0";
    netConnService->netSuppliers_.clear();
    netConnService->netSuppliers_[1] = supplier;
    auto ret = netConnService->IsIfaceNameInUse("rmnet0", 100);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnServiceExtTest, FindSupplierWithInternetByBearerTypeTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->FindSupplierWithInternetByBearerType(NetBearType::BEARER_WIFI, TEST_IDENT);
    EXPECT_TRUE(ret.empty());
}

HWTEST_F(NetConnServiceExtTest, OnRemoteDiedTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->defaultNetSupplier_ = sptr<NetSupplier>::MakeSptr(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_NE(netConnService->defaultNetSupplier_, nullptr);
    wptr<IRemoteObject> remoteObject = nullptr;
    netConnService->OnRemoteDied(remoteObject);
}

HWTEST_F(NetConnServiceExtTest, OnRemoteDiedTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    wptr<IRemoteObject> remoteObject = new MockNetIRemoteObject();
    EXPECT_NE(remoteObject, nullptr);
    netConnService->OnRemoteDied(remoteObject);
}

HWTEST_F(NetConnServiceExtTest, FindSupplierForConnectedTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::vector<sptr<NetSupplier>> suppliers = {nullptr};
    auto ret = netConnService->FindSupplierForConnected(suppliers);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetConnServiceExtTest, OnReceiveEventTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->defaultNetSupplier_ = sptr<NetSupplier>::MakeSptr(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    NetConnService::NetConnListener listener(subscribeInfo, nullptr);
    EXPECT_NE(netConnService->defaultNetSupplier_, nullptr);
    EventFwk::CommonEventData eventData;
    listener.OnReceiveEvent(eventData);
}

HWTEST_F(NetConnServiceExtTest, EnableVnicNetworkTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    sptr<NetLinkInfo> netLinkInfo = new NetLinkInfo();
    const std::set<int32_t> uids;
    EXPECT_EQ(netConnService->netConnEventHandler_, nullptr);
    auto ret = netConnService->EnableVnicNetwork(netLinkInfo, uids);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    ret = netConnService->EnableVnicNetwork(netLinkInfo, uids);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, EnableVnicNetworkAsyncTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    sptr<NetLinkInfo> netLinkInfo = new NetLinkInfo();
    const std::set<int32_t> uids;

    NetManagerStandard::INetAddr inetAddr;
    inetAddr.type_ = NetManagerStandard::INetAddr::IpType::IPV4;
    inetAddr.family_ = 0x01;
    inetAddr.address_ = "10.0.0.2.1";
    inetAddr.netMask_ = "255.255.255.0";
    inetAddr.hostName_ = "localhost";
    inetAddr.port_ = 80;
    inetAddr.prefixlen_ = 24;
    netLinkInfo->ifaceName_ = "vnic-tun";
    netLinkInfo->netAddrList_.push_back(inetAddr);
    netLinkInfo->mtu_ = 1500;

    auto ret = netConnService->EnableVnicNetworkAsync(netLinkInfo, uids);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
}

HWTEST_F(NetConnServiceExtTest, DisableVnicNetworkTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->DisableVnicNetwork();
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    EXPECT_NE(netConnService->netConnEventHandler_, nullptr);
    ret = netConnService->DisableVnicNetwork();
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, EnableDistributedClientNetAsyncTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string virnicAddr;
    std::string virnicName = "virnic";
    std::string iif;
    auto ret = netConnService->EnableDistributedClientNetAsync(virnicAddr, virnicAddr, iif);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
}

HWTEST_F(NetConnServiceExtTest, EnableDistributedClientNetAsyncTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string virnicAddr = "192.168.1.300";
    std::string virnicName = "virnic";
    std::string iif = "eth0";
    auto ret = netConnService->EnableDistributedClientNetAsync(virnicAddr, virnicAddr, iif);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
}

HWTEST_F(NetConnServiceExtTest, EnableDistributedClientNetAsyncTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string virnicAddr = "192.168.1.5";
    std::string virnicName = "virnic";
    std::string iif = "eth0";
    auto ret = netConnService->EnableDistributedClientNetAsync(virnicAddr, virnicAddr, iif);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, EnableDistributedServerNetTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string iif = "eth0";
    std::string devIface = "bond0";
    std::string dstAddr = "192.168.1.100";
    std::string gw = "0.0.0.0";
    auto ret = netConnService->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    ret = netConnService->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, EnableDistributedServerNetAsyncTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string iif;
    std::string devIface;
    std::string dstAddr;
    std::string gw;
    auto ret = netConnService->EnableDistributedServerNetAsync(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);

    iif = "eth0";
    ret = netConnService->EnableDistributedServerNetAsync(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);

    devIface = "bond0";
    dstAddr = "192.168.1.300";
    ret = netConnService->EnableDistributedServerNetAsync(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
}

HWTEST_F(NetConnServiceExtTest, EnableDistributedServerNetAsyncTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string iif = "eth0";
    std::string devIface = "bond0";
    std::string dstAddr = "192.168.1.100";
    std::string gw = "0.0.0.0";
    auto ret = netConnService->EnableDistributedServerNetAsync(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetConnServiceExtTest, DisableDistributedNetTest001, TestSize.Level1)
{
    std::string virnicName = "virnic";
    std::string dstAddr = "1.1.1.1";
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->DisableDistributedNet(true, virnicName, dstAddr);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    ret = netConnService->DisableDistributedNet(true, virnicName, dstAddr);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceExtTest, DisableDistributedNetAsyncTest001, TestSize.Level1)
{
    std::string virnicName = "virnic";
    std::string dstAddr = "1.1.1.1";
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->DisableDistributedNetAsync(false, virnicName, dstAddr);
    EXPECT_TRUE(ret == NETMANAGER_ERR_OPERATION_FAILED || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, CloseSocketsUidAsyncTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    int32_t netId = 0;
    uint32_t uid = 1;
    auto ret = netConnService->CloseSocketsUidAsync(netId, uid);
    EXPECT_EQ(ret, NET_CONN_ERR_NETID_NOT_FOUND);

    netId = 1;
    ret = netConnService->CloseSocketsUidAsync(netId, uid);
    EXPECT_EQ(ret, NET_CONN_ERR_NETID_NOT_FOUND);
}

HWTEST_F(NetConnServiceExtTest, SetAppIsFrozenedAsyncTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t uid = 1;
    netConnService->netUidActivates_.clear();
    auto ret = netConnService->SetAppIsFrozenedAsync(uid, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, SetAppIsFrozenedAsyncTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t uid = 1;
    bool isFrozened = false;
    std::vector<std::shared_ptr<NetActivate>> activates;
    sptr<NetSpecifier> specifier = nullptr;
    sptr<INetConnCallback> callback = nullptr;
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);
    activates.push_back(active);
    activates[0]->SetIsAppFrozened(isFrozened);
    netConnService->netUidActivates_[uid] = activates;
    auto ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    isFrozened = true;
    ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    isFrozened = false;
    activates[0]->SetIsAppFrozened(true);
    activates[0]->SetLastCallbackType(CALL_TYPE_UNKNOWN);
    ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, SetAppIsFrozenedAsyncTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t uid = 1;
    bool isFrozened = false;
    std::vector<std::shared_ptr<NetActivate>> activates;
    sptr<NetSpecifier> specifier = nullptr;
    sptr<INetConnCallback> callback = nullptr;
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);
    activates.push_back(active);
    activates[0]->SetLastCallbackType(CallbackType::CALL_TYPE_AVAILABLE);
    netConnService->netUidActivates_[uid] = activates;
    auto ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    activates[0]->SetLastCallbackType(CallbackType::CALL_TYPE_LOST);
    ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    activates[0]->SetLastNetid(1);
    EXPECT_EQ(activates[0]->GetNetCallback(), nullptr);
    ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    activates[0]->netConnCallback_ = new (std::nothrow) NetConnCallbackStubCb();
    ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, SetAppIsFrozenedAsyncTest004, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t uid = 1;
    bool isFrozened = false;
    std::vector<std::shared_ptr<NetActivate>> activates;
    sptr<NetSpecifier> specifier = nullptr;
    sptr<INetConnCallback> callback = nullptr;
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);
    activates.push_back(active);
    activates[0]->SetLastCallbackType(CallbackType::CALL_TYPE_AVAILABLE);
    activates[0]->SetServiceSupply(nullptr);
    netConnService->netUidActivates_[uid] = activates;
    EXPECT_EQ(activates[0]->GetServiceSupply(), nullptr);
    auto ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    activates[0]->SetLastCallbackType(CallbackType::CALL_TYPE_LOST);
    ret = netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, SetAppIsFrozenedAsyncTest005, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    uint32_t uid = 1099;
    int32_t netId = 123;
    uint32_t reqId = 1;
    bool isFrozened = false;
    std::vector<std::shared_ptr<NetActivate>> activates;
    sptr<NetSpecifier> specifier = nullptr;
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler, uid, REQUEST);
    active->SetRequestId(reqId);
    active->lastNetId_ = netId;
    active->SetIsAppFrozened(!isFrozened);
    active->SetLastCallbackType(CallbackType::CALL_TYPE_LOST);
    active->SetServiceSupply(nullptr);
    activates.push_back(active);
    netConnService->netUidActivates_[uid] = activates;
    netConnService->notifyLostDelayCache_.EnsureInsert(netId, true);
    netConnService->uidLostDelaySet_.insert(uid);
    netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_TRUE(active->isNotifyLostDelay_);
    EXPECT_GE(active->notifyLostNetId_, 0);
}

HWTEST_F(NetConnServiceExtTest, SetAppIsFrozenedAsyncTest006, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t uid = 1;
    bool isFrozened = true;
    std::vector<std::shared_ptr<NetActivate>> activates;
    sptr<NetSpecifier> specifier = new (std::nothrow) NetSpecifier();
    specifier->SetTypes({BEARER_CELLULAR});
    sptr<NetSpecifier> specifier1 = new (std::nothrow) NetSpecifier();
    specifier1->SetTypes({BEARER_WIFI});
    sptr<INetConnCallback> callback = nullptr;
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);
    auto active1 = std::make_shared<NetActivate>(specifier1, callback, timeoutCallback, 0, handler);
    auto active2 = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);
    active2 = nullptr;
    activates.push_back(active);
    activates.push_back(active1);
    activates.push_back(active2);
    activates[0]->SetLastCallbackType(CallbackType::CALL_TYPE_AVAILABLE);
    activates[0]->SetServiceSupply(nullptr);
    netConnService->netUidActivates_[uid] = activates;

    netConnService->SetAppIsFrozenedAsync(uid, isFrozened);
    EXPECT_EQ(true, activates[0]->IsFrozenedSkip());
    activates[1]->SetIsFrozenedSkip(false);
    netConnService->SetAppIsFrozenedAsync(uid, !isFrozened);
    EXPECT_FALSE(activates[0]->IsFrozenedSkip());
}

HWTEST_F(NetConnServiceExtTest, HandleUnfrozenCallbackTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    sptr<NetSpecifier> specifier = new (std::nothrow) NetSpecifier();
    specifier->SetTypes({BEARER_CELLULAR});
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);

    uint32_t supplierId = 1;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_VALIDATED);
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    active->SetServiceSupply(supplier);
    active->SetLastCallbackType(CallbackType::CALL_TYPE_LOST);
    netConnService->HandleUnfrozenCallback(1, active);
    CallbackType callbackType = active->GetLastCallbackType();
    EXPECT_EQ(callbackType, CallbackType::CALL_TYPE_UNKNOWN);

    active->SetLastCallbackType(CallbackType::CALL_TYPE_AVAILABLE);
    netConnService->HandleUnfrozenCallback(1, active);
    callbackType = active->GetLastCallbackType();
    EXPECT_EQ(callbackType, CallbackType::CALL_TYPE_UNKNOWN);
}

HWTEST_F(NetConnServiceExtTest, HandleUnfrozenCallbackTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    sptr<NetSpecifier> specifier = new (std::nothrow) NetSpecifier();
    specifier->SetTypes({BEARER_CELLULAR});
    sptr<INetConnCallback> callback = nullptr;
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);

    active->SetServiceSupply(nullptr);
    active->SetLastCallbackType(CallbackType::CALL_TYPE_AVAILABLE);
    netConnService->HandleUnfrozenCallback(1, active);
    CallbackType callbackType = active->GetLastCallbackType();
    EXPECT_NE(callbackType, CallbackType::CALL_TYPE_UNKNOWN);

    active->SetLastCallbackType(CallbackType::CALL_TYPE_LOST);
    active->SetLastNetid(100);
    netConnService->HandleUnfrozenCallback(1, active);
    callbackType = active->GetLastCallbackType();
    EXPECT_EQ(callbackType, CallbackType::CALL_TYPE_UNKNOWN);

    sptr<INetConnCallback> callback1 = new (std::nothrow) NetConnCallbackStubCb();
    auto active1 = std::make_shared<NetActivate>(specifier, callback1, timeoutCallback, 0, handler);
    active1->SetServiceSupply(nullptr);
    active1->SetLastCallbackType(CallbackType::CALL_TYPE_LOST);
    active1->SetLastNetid(100);
    netConnService->HandleUnfrozenCallback(1, active1);
    callbackType = active1->GetLastCallbackType();
    EXPECT_EQ(callbackType, CallbackType::CALL_TYPE_UNKNOWN);
}

HWTEST_F(NetConnServiceExtTest, HandleNotifyLostDelayTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t uid = 1099;
    int32_t netId = 123;
    std::vector<std::shared_ptr<NetActivate>> activates;
    sptr<NetSpecifier> specifier = nullptr;
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    sptr<INetConnCallback> callback1 = nullptr;
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);
    auto active1 = std::make_shared<NetActivate>(specifier, callback1, timeoutCallback, 0, handler);
    auto active2 = std::make_shared<NetActivate>(specifier, callback1, timeoutCallback, 0, handler);
    active2 = nullptr;
    activates.push_back(active);
    activates.push_back(active1);
    activates.push_back(active2);
    netConnService->netUidActivates_[uid] = activates;
    active->isNotifyLostDelay_ = true;
    netConnService->HandleNotifyLostDelay(netId);
    EXPECT_TRUE(active->isNotifyLostDelay_);
    active->SetNeedSkipLostDelay(true);
    active1->SetNeedSkipLostDelay(true);
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_VALIDATED);
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    active->SetServiceSupply(supplier);
    netConnService->HandleNotifyLostDelay(netId);
    EXPECT_TRUE(active->isNotifyLostDelay_);
}

HWTEST_F(NetConnServiceExtTest, CheckNotifyLostDelayTest001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t uid = 100;
    int32_t netId = 123;
    bool ret = true;
    netConnService->netConnEventHandler_  = nullptr;
    sptr<NetSpecifier> specifier = new (std::nothrow) NetSpecifier();
    specifier->SetTypes({BEARER_CELLULAR});
    sptr<INetConnCallback> callback = nullptr;
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);
    active->netRequest_.uid = uid;
    ret = netConnService->CheckNotifyLostDelay(active, netId, CallbackType::CALL_TYPE_LOST);
    EXPECT_FALSE(ret);

    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    ret = netConnService->CheckNotifyLostDelay(active, netId, CallbackType::CALL_TYPE_UNKNOWN);
    EXPECT_FALSE(ret);

    ret = netConnService->CheckNotifyLostDelay(active, netId, CallbackType::CALL_TYPE_LOST);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetConnServiceExtTest, EnableAppFrozenedCallbackLimitationTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    auto ret = netConnService->EnableAppFrozenedCallbackLimitation(true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, SetReuseSupplierIdTest002, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t supplierId = 1;
    uint32_t reuseSupplierId = 1;
    netConnService->netSuppliers_.clear();
    netConnService->netSuppliers_[0] = nullptr;
    auto ret = netConnService->SetReuseSupplierId(supplierId, reuseSupplierId, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, SetReuseSupplierIdTest003, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t supplierId = 1;
    uint32_t reuseSupplierId = 2;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    supplier->supplierId_ = supplierId;
    netConnService->netSuppliers_[1] = supplier;
    auto ret = netConnService->SetReuseSupplierId(supplierId, reuseSupplierId, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    netConnService->netSuppliers_[1]->supplierId_ = reuseSupplierId;
    ret = netConnService->SetReuseSupplierId(supplierId, reuseSupplierId, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    netConnService->netSuppliers_[1]->supplierId_ = 0;
    ret = netConnService->SetReuseSupplierId(supplierId, reuseSupplierId, false);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceExtTest, HandleDetectionResult001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netConnService->defaultNetSupplier_ = new NetSupplier(BEARER_WIFI, netSupplierIdent, netCaps);
    EXPECT_NE(netConnService->defaultNetSupplier_, nullptr);
    uint32_t supplierId = 1;
    netConnService->defaultNetSupplier_->supplierId_ = supplierId;
    netConnService->isDelayHandleFindBestNetwork_ = true;
    netConnService->netSuppliers_[1] = netConnService->defaultNetSupplier_;
    netConnService->HandleDetectionResult(supplierId, VERIFICATION_STATE);
    EXPECT_FALSE(netConnService->isDelayHandleFindBestNetwork_);
    netConnService->defaultNetSupplier_ = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    netConnService->netSuppliers_[0] = netConnService->defaultNetSupplier_;
    std::string netWifiSupplierIdent;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_WIFI, netWifiSupplierIdent, netCaps);
    supplier->supplierId_ = supplierId;
    supplier->SetNetValid(QUALITY_POOR_STATE);
    netConnService->isDelayHandleFindBestNetwork_ = true;
    netConnService->delaySupplierId_ = supplierId;
    netConnService->netSuppliers_[1] = supplier;
    netConnService->HandleDetectionResult(supplierId, VERIFICATION_STATE);
    EXPECT_TRUE(netConnService->isDelayHandleFindBestNetwork_);
    netConnService->delaySupplierId_ = 0;
    netConnService->HandleDetectionResult(supplierId, VERIFICATION_STATE);
    netConnService->netSuppliers_.clear();
}

HWTEST_F(NetConnServiceExtTest, HandlePreFindBestNetworkForDelay001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->HandlePreFindBestNetworkForDelay(1, nullptr, true);
    uint32_t supplierId = 1;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_CHECKING_CONNECTIVITY);
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_WIFI, netSupplierIdent, netCaps);
    supplier->supplierId_ = supplierId;
    netConnService->isDelayHandleFindBestNetwork_ = true;
    netConnService->HandlePreFindBestNetworkForDelay(supplierId, supplier, true);
    netConnService->defaultNetSupplier_ = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    netConnService->netSuppliers_[1] = supplier;
    netConnService->isDelayHandleFindBestNetwork_ = false;
    OHOS::system::SetParameter("persist.booster.enable_wifi_delay_weak_signal", "false");
    netConnService->HandlePreFindBestNetworkForDelay(supplierId, supplier, true);
    EXPECT_FALSE(netConnService->isDelayHandleFindBestNetwork_);
    OHOS::system::SetParameter("persist.booster.enable_wifi_delay_weak_signal", "true");
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->HandlePreFindBestNetworkForDelay(supplierId, supplier, true);
    EXPECT_TRUE(netConnService->isDelayHandleFindBestNetwork_);
    netConnService->isDelayHandleFindBestNetwork_ = false;
    netConnService->HandlePreFindBestNetworkForDelay(supplierId, supplier, false);
    EXPECT_FALSE(netConnService->isDelayHandleFindBestNetwork_);
    OHOS::system::SetParameter("persist.booster.enable_wifi_delay_weak_signal", "false");
    netConnService->netSuppliers_.clear();
}

HWTEST_F(NetConnServiceExtTest, HandleFindBestNetworkForDelay001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->HandleFindBestNetworkForDelay();
    uint32_t supplierId = 1;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_VALIDATED);
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_WIFI, netSupplierIdent, netCaps);
    supplier->supplierId_ = supplierId;
    netConnService->delaySupplierId_ = supplierId;
    netConnService->netSuppliers_[1] = supplier;
    netConnService->isDelayHandleFindBestNetwork_ = true;
    netConnService->HandleFindBestNetworkForDelay();
    EXPECT_EQ(netConnService->delaySupplierId_, 0);
    netConnService->netSuppliers_.clear();
}

HWTEST_F(NetConnServiceExtTest, UpdateNetSupplierInfoAsyncInvalid001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t supplierId = 1;
    netConnService->isDelayHandleFindBestNetwork_ = false;
    netConnService->UpdateNetSupplierInfoAsyncInvalid(supplierId);
    EXPECT_FALSE(netConnService->isDelayHandleFindBestNetwork_);
}

HWTEST_F(NetConnServiceExtTest, RemoveDelayNetwork001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    netConnService->netConnEventRunner_ = AppExecFwk::EventRunner::Create(NET_CONN_MANAGER_WORK_THREAD);
    netConnService->netConnEventHandler_ = std::make_shared<NetConnEventHandler>(netConnService->netConnEventRunner_);
    netConnService->isDelayHandleFindBestNetwork_ = true;
    netConnService->RemoveDelayNetwork();
    EXPECT_FALSE(netConnService->isDelayHandleFindBestNetwork_);
}

HWTEST_F(NetConnServiceExtTest, UpdateNetSupplierInfoAsync001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t supplierId = 1;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_VALIDATED);
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_WIFI, netSupplierIdent, netCaps);
    supplier->supplierId_ = supplierId;
    supplier->netSupplierInfo_.isAvailable_ = true;
    int32_t netId = 123;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_WIFI, nullptr);
    supplier->SetNetwork(network);
    netConnService->delaySupplierId_ = supplierId;
    netConnService->netSuppliers_[1] = supplier;
    sptr<NetSupplierInfo> netSupplierInfo = new NetSupplierInfo();
    netSupplierInfo->isAvailable_ = false;
    netSupplierInfo->isRoaming_ = true;
    netSupplierInfo->strength_ = 0x64;
    netSupplierInfo->frequency_ = 0x10;
    netConnService->isDelayHandleFindBestNetwork_ = true;
    int32_t callingUid = 100;
    netConnService->UpdateNetSupplierInfoAsync(supplierId, netSupplierInfo, callingUid);
    EXPECT_FALSE(netConnService->isDelayHandleFindBestNetwork_);
    netSupplierInfo->isAvailable_ = true;
    netConnService->UpdateNetSupplierInfoAsync(supplierId, netSupplierInfo, callingUid);
}

HWTEST_F(NetConnServiceExtTest, CallbackForSupplier, TestSize.Level1)
{
    auto netConnService = NetConnService::GetInstance();
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_NE(supplier, nullptr);
    int32_t netId = 123;
    uint32_t supplierId = 1;
    netConnService->netSuppliers_[supplierId] = supplier;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_WIFI, nullptr);
    supplier->SetNetwork(network);
    uint32_t reqId = 1;
    supplier->bestReqList_.insert(reqId);
    sptr<NetSpecifier> specifier = nullptr;
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    uint32_t uid = 1099;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler, uid, REQUEST);
    active->SetRequestId(reqId);
    netConnService->netUidActivates_[uid].push_back(active);
    netConnService->notifyLostDelayCache_.EnsureInsert(netId, true);
    netConnService->uidLostDelaySet_.insert(uid);
    CallbackType type = CallbackType::CALL_TYPE_LOST;
    netConnService->CallbackForSupplier(supplier, type);
    EXPECT_TRUE(active->isNotifyLostDelay_);
    EXPECT_EQ(active->notifyLostNetId_, netId);
    netConnService->uidLostDelaySet_.clear();
    netConnService->notifyLostDelayCache_.Clear();
    netConnService->netUidActivates_.clear();
}

HWTEST_F(NetConnServiceExtTest, FindNotifyLostUid, TestSize.Level1)
{
    auto netConnService = NetConnService::GetInstance();
    uint32_t uid = 1099;
    netConnService->uidLostDelaySet_.insert(uid);
    auto res = netConnService->FindNotifyLostUid(uid);
    EXPECT_TRUE(res);
    netConnService->uidLostDelaySet_.clear();
}

HWTEST_F(NetConnServiceExtTest, StopNotifyLostDelay, TestSize.Level1)
{
    auto netConnService = NetConnService::GetInstance();
    int32_t netId = 123;
    netConnService->notifyLostDelayCache_.EnsureInsert(netId, true);
    netConnService->StopNotifyLostDelay(netId);
    auto res = netConnService->FindNotifyLostDelayCache(netId);
    EXPECT_FALSE(res);
    netConnService->uidLostDelaySet_.clear();
}

HWTEST_F(NetConnServiceExtTest, HandleNotifyLostDelay, TestSize.Level1)
{
    auto netConnService = NetConnService::GetInstance();
    uint32_t uid = 1099;
    int32_t netId = 123;
    netConnService->uidLostDelaySet_.insert(uid);
    std::vector<std::shared_ptr<NetActivate>> activates;
    sptr<NetSpecifier> specifier = nullptr;
    sptr<INetConnCallback> callback = new (std::nothrow) NetConnCallbackStubCb();
    std::weak_ptr<INetActivateCallback> timeoutCallback;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    auto active = std::make_shared<NetActivate>(specifier, callback, timeoutCallback, 0, handler);
    active->isNotifyLostDelay_ = true;
    active->notifyLostNetId_ = netId;
    active->netServiceSupplied_ = nullptr;
    activates.push_back(active);
    netConnService->netUidActivates_[uid] = activates;
    netConnService->HandleNotifyLostDelay(netId);
    EXPECT_FALSE(active->isNotifyLostDelay_);
    EXPECT_EQ(active->notifyLostNetId_, 0);
    netConnService->uidLostDelaySet_.clear();
}

HWTEST_F(NetConnServiceExtTest, UpdateUidLostDelay, TestSize.Level1)
{
    auto netConnService = NetConnService::GetInstance();
    uint32_t uid = 1099;
    std::set<uint32_t> uidLostDelaySet;
    uidLostDelaySet.insert(uid);
    netConnService->UpdateUidLostDelay(uidLostDelaySet);
    EXPECT_FALSE(netConnService->uidLostDelaySet_.empty());
    uidLostDelaySet.clear();
    netConnService->UpdateUidLostDelay(uidLostDelaySet);
    EXPECT_TRUE(netConnService->uidLostDelaySet_.empty());
}

HWTEST_F(NetConnServiceExtTest, UpdateNetSupplierInfoAsyncExpand001, TestSize.Level1)
{
    auto netConnService = std::make_shared<NetConnService>();
    uint32_t supplierId = 1;
    std::string netSupplierIdent;
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_VALIDATED);
    sptr<NetSupplier> supplier = new NetSupplier(BEARER_WIFI, netSupplierIdent, netCaps);
    supplier->supplierId_ = supplierId;
    supplier->netSupplierInfo_.isAvailable_ = true;
    int32_t netId = 123;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_WIFI, nullptr);
    supplier->SetNetwork(network);
    netConnService->delaySupplierId_ = supplierId;
    netConnService->netSuppliers_[1] = supplier;
    HttpProxy oldHttpProxy;
    oldHttpProxy.SetHost("192.168.1.1");
    netConnService->UpdateNetSupplierInfoAsyncExpand(supplier, oldHttpProxy);
    EXPECT_EQ(supplier->supplierId_, supplierId);
}

} // namespace NetManagerStandard
} // namespace OHOS

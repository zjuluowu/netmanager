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

#define private public
#include <gtest/gtest.h>

#include "http_proxy.h"
#include "iservice_registry.h"
#include "net_common_event_test.h"
#include "net_conn_callback_test.h"
#include "net_conn_client.h"
#include "net_conn_constants.h"
#include "net_detection_callback_test.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_test_security.h"
#include "system_ability_definition.h"
#include "net_detection_callback_stub.h"
#include "network.h"
#include "common_mock_net_conn_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int WAIT_TIME_SECOND_LONG = 5;
constexpr int WAIT_TIME_SECOND_NET_DETECTION = 2;
using namespace testing;
using namespace testing::ext;
} // namespace

std::shared_ptr<NetCommonEventTest> netCommonEventTest_ = nullptr;

class NetConnManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<NetLinkInfo> GetUpdateLinkInfoSample() const;

    sptr<NetConnCallbackTest> GetINetConnCallbackSample() const;

    sptr<NetDetectionCallbackTest> GetINetDetectionCallbackSample() const;

    void LogCapabilities(const std::list<sptr<NetHandle>> &netList) const;
    static sptr<INetConnService> GetProxy();
    inline static sptr<MockINetConnService> mockNetConnService = sptr<MockINetConnService>::MakeSptr();
};

void NetConnManagerTest::SetUpTestCase()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_HTTP_PROXY_CHANGE);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    netCommonEventTest_ = std::make_shared<NetCommonEventTest>(subscribeInfo);
    EventFwk::CommonEventManager::SubscribeCommonEvent(netCommonEventTest_);
    EXPECT_CALL(*mockNetConnService, AsObject()).WillRepeatedly(Return(mockNetConnService->AsObject()));
}

void NetConnManagerTest::TearDownTestCase()
{
    EventFwk::CommonEventManager::UnSubscribeCommonEvent(netCommonEventTest_);
    netCommonEventTest_.reset();
    mockNetConnService = nullptr;
}

void NetConnManagerTest::SetUp() {}

void NetConnManagerTest::TearDown() {}

sptr<NetLinkInfo> NetConnManagerTest::GetUpdateLinkInfoSample() const
{
    sptr<NetLinkInfo> netLinkInfo = (std::make_unique<NetLinkInfo>()).release();
    netLinkInfo->ifaceName_ = "test";
    netLinkInfo->domain_ = "test";

    sptr<INetAddr> netAddr = (std::make_unique<INetAddr>()).release();
    netAddr->type_ = INetAddr::IPV4;
    netAddr->family_ = 0x10;
    netAddr->prefixlen_ = 0x17;
    netAddr->address_ = "192.168.2.0";
    netAddr->netMask_ = "192.255.255.255";
    netAddr->hostName_ = "netAddr";
    netLinkInfo->netAddrList_.push_back(*netAddr);

    sptr<INetAddr> dns = (std::make_unique<INetAddr>()).release();
    dns->type_ = INetAddr::IPV4;
    dns->family_ = 0x10;
    dns->prefixlen_ = 0x17;
    dns->address_ = "192.168.2.0";
    dns->netMask_ = "192.255.255.255";
    dns->hostName_ = "netAddr";
    netLinkInfo->dnsList_.push_back(*dns);

    sptr<Route> route = (std::make_unique<Route>()).release();
    route->iface_ = "iface0";
    route->destination_.type_ = INetAddr::IPV4;
    route->destination_.family_ = 0x10;
    route->destination_.prefixlen_ = 0x17;
    route->destination_.address_ = "192.168.2.0";
    route->destination_.netMask_ = "192.255.255.255";
    route->destination_.hostName_ = "netAddr";
    route->gateway_.type_ = INetAddr::IPV4;
    route->gateway_.family_ = 0x10;
    route->gateway_.prefixlen_ = 0x17;
    route->gateway_.address_ = "192.168.2.0";
    route->gateway_.netMask_ = "192.255.255.255";
    route->gateway_.hostName_ = "netAddr";
    netLinkInfo->routeList_.push_back(*route);

    netLinkInfo->mtu_ = 0x5DC;

    return netLinkInfo;
}

sptr<NetConnCallbackTest> NetConnManagerTest::GetINetConnCallbackSample() const
{
    sptr<NetConnCallbackTest> callback = (std::make_unique<NetConnCallbackTest>()).release();
    return callback;
}

sptr<NetDetectionCallbackTest> NetConnManagerTest::GetINetDetectionCallbackSample() const
{
    sptr<NetDetectionCallbackTest> detectionCallback = (std::make_unique<NetDetectionCallbackTest>()).release();
    return detectionCallback;
}

void NetConnManagerTest::LogCapabilities(const std::list<sptr<NetHandle>> &netList) const
{
    for (auto it : netList) {
        std::cout << "netid = " << it->GetNetId() << std::endl;
        NetAllCapabilities netAllCap;
        NetConnClient::GetInstance().GetNetCapabilities(*it, netAllCap);
        std::cout << netAllCap.ToString("|") << std::endl;
    }
}

sptr<INetConnService> NetConnManagerTest::GetProxy()
{
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        std::cout << "NetConnService Get ISystemAbilityManager failed ... " << std::endl;
        return nullptr;
    }

    sptr<IRemoteObject> remote = systemAbilityMgr->CheckSystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID);
    if (remote) {
        sptr<INetConnService> NetConnService = iface_cast<INetConnService>(remote);
        std::cout << "NetConnService Get COMM_NET_CONN_MANAGER_SYS_ABILITY_ID success ... " << std::endl;
        return NetConnService;
    } else {
        std::cout << "NetConnService Get COMM_NET_CONN_MANAGER_SYS_ABILITY_ID fail ... " << std::endl;
        return nullptr;
    }
}

/**
 * @tc.name: NetConnManager001
 * @tc.desc: Test NetConnManager SystemReady.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager001, TestSize.Level1)
{
    int32_t result = NetConnClient::GetInstance().SystemReady();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager002
 * @tc.desc: Test NetConnManager RegisterNetSupplier.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager002, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    NetManagerBaseAccessToken token;
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};
    std::string ident = "ident01";
    uint32_t supplierId = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager003
 * @tc.desc: Test NetConnManager UnregisterNetSupplier.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager003, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UnregisterNetSupplier(_)).WillRepeatedly(Return(0));
    NetManagerBaseAccessToken token;
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};
    std::string ident = "ident02";
    uint32_t supplierId = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = netConnClient->UnregisterNetSupplier(supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager004
 * @tc.desc: Test NetConnManager UpdateNetSupplierInfo.
 * @tc.type: FUNC
 */

HWTEST_F(NetConnManagerTest, NetConnManager004, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UpdateNetSupplierInfo(_, _)).WillRepeatedly(Return(0));
    NetManagerBaseAccessToken token;
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};
    std::string ident = "ident03";
    uint32_t supplierId = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    sptr<NetSupplierInfo> netSupplierInfo = new NetSupplierInfo;
    netSupplierInfo->isAvailable_ = true;
    netSupplierInfo->isRoaming_ = true;
    netSupplierInfo->strength_ = 0x64;
    netSupplierInfo->frequency_ = 0x10;
    result = netConnClient->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager005
 * @tc.desc: Test NetConnManager UpdateNetLinkInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager005, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UpdateNetLinkInfo(_, _)).WillRepeatedly(Return(0));
    NetManagerBaseAccessToken token;
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};

    std::string ident = "ident04";
    uint32_t supplierId = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    sptr<NetLinkInfo> netLinkInfo = GetUpdateLinkInfoSample();
    result = netConnClient->UpdateNetLinkInfo(supplierId, netLinkInfo);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager006
 * @tc.desc: Test NetConnManager RegisterNetConnCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager006, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, RegisterNetConnCallback(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UnregisterNetConnCallback(_)).WillRepeatedly(Return(0));
    NetManagerBaseAccessToken token;
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET};

    std::string ident = "ident";
    uint32_t supplierId = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    sptr<NetSpecifier> netSpecifier = (std::make_unique<NetSpecifier>()).release();
    netSpecifier->ident_ = ident;
    netSpecifier->SetCapabilities(netCaps);
    sptr<NetConnCallbackTest> callback = GetINetConnCallbackSample();
    result = netConnClient->RegisterNetConnCallback(callback);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    if (result == NETMANAGER_SUCCESS) {
        sptr<INetConnService> proxy = NetConnManagerTest::GetProxy();
        if (proxy == nullptr) {
            return;
        }
        proxy->UpdateNetStateForTest(netSpecifier, 1);
        callback->WaitFor(WAIT_TIME_SECOND_LONG);
        int32_t netState = callback->GetNetState();
        std::cout << "NetConnManager006 RegisterNetConnCallback netState:" << netState << std::endl;
    } else {
        std::cout << "NetConnManager006 RegisterNetConnCallback return fail" << std::endl;
    }

    result = netConnClient->UnregisterNetConnCallback(callback);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager007
 * @tc.desc: Test NetConnManager RegisterNetDetectionCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager007, TestSize.Level1)
{
    NetHandle netHandle;
    int32_t result = NetConnClient::GetInstance().GetDefaultNet(netHandle);
    if (result != NETMANAGER_SUCCESS) {
        return ;
    }
    sptr<NetDetectionCallbackStub> callback = new NetDetectionCallbackStub();
    int32_t netId = netHandle.GetNetId();
    std::cout << "RegisterNetDetectionCallback netId:"<< netId << std::endl;
    result = NetConnClient::GetInstance().RegisterNetDetectionCallback(netId, callback);
    if (result == 0) {
        std::cout << "RegisterNetDetectionCallback register success" << std::endl;
        return;
    }
    std::cout << "RegisterNetDetectionCallback failed ret = %{public}d"<< result << std::endl;

    result = NetConnClient::GetInstance().UnRegisterNetDetectionCallback(netId, callback);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager008
 * @tc.desc: Test NetConnManager RegisterNetDetectionCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager008, TestSize.Level1)
{
    NetAllCapabilities netAllCap;
    EXPECT_CALL(*mockNetConnService, GetAllNets(_)).WillOnce(Invoke([](std::list<int32_t> &netIdList) {
        netIdList.push_back(100);
        return NETMANAGER_SUCCESS;
    }));
    EXPECT_CALL(*mockNetConnService, GetNetCapabilities(_, Ref(netAllCap))).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, NetDetection(_)).WillRepeatedly(Return(0));
    NetManagerBaseAccessToken token;
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    sptr<INetConnService> proxy = NetConnManagerTest::GetProxy();
    if (proxy == nullptr) {
        return;
    }
    std::list<sptr<NetHandle>> netList;
    int32_t result = netConnClient->GetAllNets(netList);
    std::cout << "netIdList size:" << netList.size() << std::endl;
    EXPECT_EQ(result, NetConnResultCode::NET_CONN_SUCCESS);
    sptr<NetDetectionCallbackTest> detectionCallback = GetINetDetectionCallbackSample();
    if (detectionCallback == nullptr) {
        return;
    }
    for (sptr<NetHandle> netHandle : netList) {
        netConnClient->GetNetCapabilities(*netHandle, netAllCap);
        std::cout << netAllCap.ToString("|") << std::endl;
        if (netAllCap.bearerTypes_.find(BEARER_WIFI) == netAllCap.bearerTypes_.end()) {
            continue;
        }
        int32_t netId = netHandle->GetNetId();
        result = proxy->RegisterNetDetectionCallback(netId, detectionCallback);
        EXPECT_EQ(result, NETMANAGER_SUCCESS);
        std::cout << "TestRegisterNetDetectionCallback netId:" << netId << " result:" << result << std::endl;
        result = netConnClient->NetDetection(*netHandle);
        EXPECT_EQ(result, NETMANAGER_SUCCESS);
        std::cout << "TestNetDetection result:" << result << std::endl;
        detectionCallback->WaitFor(WAIT_TIME_SECOND_NET_DETECTION);
        int32_t netDetectionRet = detectionCallback->GetNetDetectionResult();
        std::cout << "RegisterNetDetectionCallback netDetectionRet:" << netDetectionRet << std::endl;
        std::string urlRedirect = detectionCallback->GetUrlRedirect();
        std::cout << "RegisterNetDetectionCallback urlRedirect:" << urlRedirect << std::endl;

        result = proxy->UnRegisterNetDetectionCallback(netId, detectionCallback);
        EXPECT_EQ(result, NETMANAGER_SUCCESS);
        std::cout << "TestUnRegisterNetDetectionCallback result:" << result << std::endl;
        result = netConnClient->NetDetection(*netHandle);
        EXPECT_EQ(result, NETMANAGER_SUCCESS);
        std::cout << "TestNetDetection result:" << result << std::endl;
        detectionCallback->WaitFor(WAIT_TIME_SECOND_NET_DETECTION);
        netDetectionRet = detectionCallback->GetNetDetectionResult();
        EXPECT_EQ(netDetectionRet, static_cast<int32_t>(NetDetectionResultCode::NET_DETECTION_FAIL));
        std::cout << "RegisterNetDetectionCallback netDetectionRet:" << netDetectionRet << std::endl;
        urlRedirect = detectionCallback->GetUrlRedirect();
        EXPECT_TRUE(urlRedirect.empty());
        std::cout << "RegisterNetDetectionCallback urlRedirect:" << urlRedirect << std::endl;
    }
}

/**
 * @tc.name: NetConnManager009
 * @tc.desc: Test NetConnManager RegisterNetDetectionCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager009, TestSize.Level1)
{
    sptr<INetConnService> proxy = NetConnManagerTest::GetProxy();
    if (proxy == nullptr) {
        return;
    }

    sptr<NetDetectionCallbackTest> detectionCallback = GetINetDetectionCallbackSample();
    if (detectionCallback == nullptr) {
        return;
    }

    const int32_t netIdError = -1;
    int32_t result = 0;
    result = proxy->RegisterNetDetectionCallback(netIdError, detectionCallback);
    std::cout << "TestRegisterNetDetectionCallback netIdError:" << netIdError << " result:" << result << std::endl;
    EXPECT_NE(result, NETMANAGER_SUCCESS);
    result = proxy->UnRegisterNetDetectionCallback(netIdError, detectionCallback);
    std::cout << "TestUnRegisterNetDetectionCallback netIdError:" << netIdError << " result:" << result << std::endl;
    EXPECT_NE(result, NETMANAGER_SUCCESS);
    NetHandle netHError(netIdError);
    result = NetConnClient::GetInstance().NetDetection(netHError);
    std::cout << "TestNetDetection netIdError:" << netIdError << " result:" << result << std::endl;
    EXPECT_NE(result, NETMANAGER_SUCCESS);

    result = proxy->RegisterNetDetectionCallback(netIdError, nullptr);
    std::cout << "TestRegisterNetDetectionCallback nullptr result:" << result << std::endl;
    EXPECT_NE(result, NETMANAGER_SUCCESS);
    result = proxy->UnRegisterNetDetectionCallback(netIdError, nullptr);
    std::cout << "TestUnRegisterNetDetectionCallback nullptr result:" << result << std::endl;
    EXPECT_NE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager011
 * @tc.desc: Test NetConnManager GetSpecificNet.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager011, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};

    NetManagerBaseAccessToken token;
    std::string ident = "ident";
    uint32_t supplierId1 = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId1);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "supplierId1 : " << supplierId1 << std::endl;

    ident = "ident2";
    uint32_t supplierId2 = 0;
    result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId2);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "supplierId2 : " << supplierId2 << std::endl;

    ident = "ident3";
    uint32_t supplierId3 = 0;
    result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId3);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "supplierId3 : " << supplierId3 << std::endl;

    std::list<int32_t> netIdList;
    result = netConnClient->GetSpecificNet(bearerType, netIdList);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    for (auto it : netIdList) {
        std::cout << "netid = " << it << std::endl;
    }

    std::list<int32_t> netIdList1;
    result = netConnClient->GetSpecificNetByIdent(bearerType, "ident", netIdList1);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    for (auto it : netIdList) {
        std::cout << "netid = " << it << std::endl;
    }
}

/**
 * @tc.name: NetConnManager012
 * @tc.desc: Test NetConnManager GetAllNets.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager012, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, GetAllNets(_)).WillOnce(Invoke([](std::list<int32_t> &netIdList) {
        netIdList.push_back(100);
        return NETMANAGER_SUCCESS;
    }));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerTypeCel = BEARER_CELLULAR;
    NetBearType bearerTypeEth = BEARER_ETHERNET;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET};

    NetManagerBaseAccessToken token;
    std::string ident = "ident";
    uint32_t supplierId1 = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerTypeCel, ident, netCaps, supplierId1);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "supplierId1 : " << supplierId1 << std::endl;

    ident = "ident2";
    uint32_t supplierId2 = 0;
    result = netConnClient->RegisterNetSupplier(bearerTypeEth, ident, netCaps, supplierId2);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "supplierId2 : " << supplierId2 << std::endl;

    ident = "ident3";
    uint32_t supplierId3 = 0;
    result = netConnClient->RegisterNetSupplier(bearerTypeCel, ident, netCaps, supplierId3);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "supplierId3 : " << supplierId3 << std::endl;

    std::list<sptr<NetHandle>> netList;
    result = netConnClient->GetAllNets(netList);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    for (auto it : netList) {
        std::cout << "netid = " << it->GetNetId() << std::endl;
    }
}

/**
 * @tc.name: NetConnManager013
 * @tc.desc: Test NetConnManager GetNetCapabilities.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager013, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, GetAllNets(_)).WillOnce(Invoke([](std::list<int32_t> &netIdList) {
        netIdList.push_back(100);
        return NETMANAGER_SUCCESS;
    }));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};

    NetManagerBaseAccessToken token;
    std::string ident = "ident";
    uint32_t supplierId = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "supplierId : " << supplierId << std::endl;

    std::list<sptr<NetHandle>> netList;
    result = netConnClient->GetAllNets(netList);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    LogCapabilities(netList);
}

/**
 * @tc.name: NetConnManager014
 * @tc.desc: Test NetConnManager GetConnectionProperties.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager014, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UpdateNetLinkInfo(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, GetNetIdByIdentifier(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, GetConnectionProperties(_, _)).WillRepeatedly(Return(0));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};

    NetManagerBaseAccessToken token;
    std::string ident = "ident";
    uint32_t supplierId = 0;
    int32_t result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "supplierId : " << supplierId << std::endl;

    sptr<NetLinkInfo> netLinkInfo = GetUpdateLinkInfoSample();
    result = netConnClient->UpdateNetLinkInfo(supplierId, netLinkInfo);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    std::list<int32_t> netIdList;
    result = netConnClient->GetNetIdByIdentifier(ident, netIdList);
    EXPECT_EQ(result, NetConnResultCode::NET_CONN_SUCCESS);
    NetLinkInfo info;
    NetHandle netHandle(netIdList.front());
    result = netConnClient->GetConnectionProperties(netHandle, info);
    std::cout << "result = " << result << std::endl;
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << info.ToString("\n") << std::endl;
}

/**
 * @tc.name: NetConnManager015
 * @tc.desc: Test NetConnManager IsDefaultNetMetered.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager015, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, IsDefaultNetMetered(_)).WillRepeatedly(Return(0));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetManagerBaseAccessToken token;
    bool isMetered = false;
    int32_t result = netConnClient->IsDefaultNetMetered(isMetered);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    std::cout << "isMetered : " << isMetered << std::endl;
}

/**
 * @tc.name: NetConnManager016
 * @tc.desc: Test GetAllNets return CONNECTED network only.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager016, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UpdateNetSupplierInfo(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UnregisterNetSupplier(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UpdateNetLinkInfo(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, GetAllNets(_)).WillRepeatedly(Invoke([](std::list<int32_t> &netIdList) {
        netIdList.push_back(20);
        return NETMANAGER_SUCCESS;
    }));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    sptr<INetConnService> proxy = NetConnManagerTest::GetProxy();
    if (proxy == nullptr) {
        return;
    }
    NetManagerBaseAccessToken token;
    int32_t result;
    std::list<sptr<NetHandle>> netList;
    result = netConnClient->GetAllNets(netList);
    size_t originNetSize = netList.size();
    std::cout << "Origin netIdList size:" << originNetSize << std::endl;
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    // Add one network connections.
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET};
    std::string ident = "ident16";
    uint32_t supplierId = 0;
    result = netConnClient->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    // Check2: The size of netList increases by 1 when the new network is connected.
    sptr<NetSupplierInfo> netSupplierInfo = std::make_unique<NetSupplierInfo>().release();
    netSupplierInfo->isAvailable_ = true;
    netConnClient->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
    sptr<NetLinkInfo> netLinkInfo = GetUpdateLinkInfoSample();
    // Rollback to check2.
    netSupplierInfo->isAvailable_ = true;
    result = netConnClient->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = netConnClient->UpdateNetLinkInfo(supplierId, netLinkInfo);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    // Check4: The size of netList decreases by 1 when the net supplier is unregistered.
    result = netConnClient->UnregisterNetSupplier(supplierId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager017
 * @tc.desc: Test NetConnManager GetNetIdByIdentifier.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager017, TestSize.Level1)
{
    std::list<int32_t> netIdList;
    int32_t result;
    std::set<std::string> idents = {"eth0", "eth1", "simId1", "wifi"};
    for (auto ident : idents) {
        netIdList.clear();
        result = NetConnClient::GetInstance().GetNetIdByIdentifier(ident, netIdList);
        for (auto netId : netIdList) {
            std::cout << "Get net id:" << netId << " through ident:" << ident << std::endl;
        }
        EXPECT_GE(result, NETMANAGER_SUCCESS);
    }
}

/**
 * @tc.name: NetConnManager018
 * @tc.desc: Test NetConnManager GetGlobalHttpProxy
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager018, TestSize.Level1)
{
    HttpProxy httpProxy;
    int32_t ret = NetConnClient::GetInstance().GetGlobalHttpProxy(httpProxy);
    std::list<std::string> exclusionList = httpProxy.GetExclusionList();
    std::cout << "Get global http host:" << httpProxy.GetHost() << " ,port:" << httpProxy.GetPort() << std::endl;
    for (auto exclusion : exclusionList) {
        std::cout << "Get global http exclusion:" << exclusion << std::endl;
    }
    EXPECT_EQ(ret, NET_CONN_SUCCESS);
}

/**
 * @tc.name: NetConnManager019
 * @tc.desc: Test NetConnManager SetGlobalHttpProxy & GetGlobalHttpProxy
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager019, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, SetGlobalHttpProxy(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, GetGlobalHttpProxy(_)).WillRepeatedly(Return(0));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    std::string host = "178.169.139.180";
    uint16_t port = 8080;
    HttpProxy httpProxy = {host, port, {"example.com", "::1", "localhost"}};
    NetManagerBaseNotSystemToken token;
    int32_t ret = netConnClient->SetGlobalHttpProxy(httpProxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = netConnClient->GetGlobalHttpProxy(httpProxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    std::list<std::string> exclusionList = httpProxy.GetExclusionList();
    std::cout << "Get global http host:" << httpProxy.GetHost() << " ,port:" << httpProxy.GetPort() << std::endl;
    for (auto exclusion : exclusionList) {
        std::cout << "Get global http exclusion:" << exclusion << std::endl;
    }
}

/**
 * @tc.name: NetConnManager019
 * @tc.desc: Test NetConnManager SetGlobalHttpProxy & GetGlobalHttpProxy
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager020, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, SetGlobalHttpProxy(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, GetGlobalHttpProxy(_)).WillRepeatedly(Return(0));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    std::string host = "";
    uint16_t port = 0;
    HttpProxy httpProxy = {host, port, {}};
    NetManagerBaseNotSystemToken token;
    int32_t ret = netConnClient->SetGlobalHttpProxy(httpProxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = netConnClient->GetGlobalHttpProxy(httpProxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    std::list<std::string> exclusionList = httpProxy.GetExclusionList();
    std::cout << "Get global http host:" << httpProxy.GetHost() << " ,port:" << httpProxy.GetPort() << std::endl;
    for (auto exclusion : exclusionList) {
        std::cout << "Get global http exclusion:" << exclusion << std::endl;
    }
}

HWTEST_F(NetConnManagerTest, NetConnManager021, TestSize.Level1)
{
    sptr<INetConnService> proxy = NetConnManagerTest::GetProxy();
    if (proxy == nullptr) {
        std::cout << "-------NetConnManager021 GetProxy failed." << std::endl;
        return;
    }
    int32_t netId = 110;
    int32_t result = proxy->NetDetection(netId);
    EXPECT_NE(result, NETMANAGER_SUCCESS);

    NetBearType bearerType = BEARER_ETHERNET;
    std::list<std::string> ifaceNames;
    result = proxy->GetIfaceNames(bearerType, ifaceNames);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    std::string ident = "test111";
    std::string ifaceName;
    result = proxy->GetIfaceNameByType(bearerType, ident, ifaceName);
    EXPECT_NE(result, NETMANAGER_SUCCESS);

    int32_t uid = 1000;
    result = proxy->GetSpecificUidNet(uid, netId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnManagerTest, NetConnManager022, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, RegisterNetSupplier(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UpdateNetSupplierInfo(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UpdateNetLinkInfo(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, RequestNetConnection(_, _, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, UnregisterNetConnCallback(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetConnService, GetAllNets(_)).WillRepeatedly(Invoke([](std::list<int32_t> &netIdList) {
        netIdList.push_back(20);
        return NETMANAGER_SUCCESS;
    }));
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;
    NetManagerBaseAccessToken token;
    uint32_t supplierId = 0;
    std::string testString = "test";
    std::set<NetCap> netCaps{NetCap::NET_CAPABILITY_INTERNAL_DEFAULT};
    auto netSupplierRet = netConnClient->RegisterNetSupplier(
        NetBearType::BEARER_CELLULAR, testString, netCaps, supplierId);
    EXPECT_EQ(netSupplierRet, NETMANAGER_SUCCESS);
    sptr<NetSupplierInfo> netSupplierInfo = std::make_unique<NetSupplierInfo>().release();
    netSupplierInfo->isAvailable_ = true;
    netConnClient->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
    netConnClient->UpdateNetLinkInfo(supplierId, GetUpdateLinkInfoSample());
    std::string ident = "ident";
    sptr<NetSpecifier> netSpecifier = (std::make_unique<NetSpecifier>()).release();
    netSpecifier->ident_ = ident;
    netSpecifier->SetCapabilities(netCaps);
    sptr<NetConnCallbackTest> callback = GetINetConnCallbackSample();
    constexpr uint32_t TEST_TIMEOUTMS = 1000;
    auto netConnRet = netConnClient->RequestNetConnection(netSpecifier, callback, TEST_TIMEOUTMS);
    EXPECT_EQ(netConnRet, NETMANAGER_SUCCESS);

    std::list<sptr<NetHandle>> netList;
    auto ret = netConnClient->GetAllNets(netList);

    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    bool isInternalDefaultNetIdIn = false;
    for (auto net : netList) {
        if (net->GetNetId() >= MIN_INTERNAL_NET_ID && net->GetNetId() <= MAX_INTERNAL_NET_ID) {
            isInternalDefaultNetIdIn = true;
        }
    }

    EXPECT_TRUE(isInternalDefaultNetIdIn);
    auto unRegisterRet = netConnClient->UnregisterNetConnCallback(callback);
    EXPECT_EQ(unRegisterRet, NETSYS_SUCCESS);

    std::list<sptr<NetHandle>> otherNetList;
    auto result = netConnClient->GetAllNets(otherNetList);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    isInternalDefaultNetIdIn = false;
    for (auto net : otherNetList) {
        if (net->GetNetId() >= MIN_INTERNAL_NET_ID && net->GetNetId() <= MAX_INTERNAL_NET_ID) {
            isInternalDefaultNetIdIn = true;
        }
    }

    EXPECT_TRUE(isInternalDefaultNetIdIn);
}

/**
 * @tc.name: NetConnManager023
 * @tc.desc: Test NetConnManager GetConnectOwnerUid
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager023, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, GetConnectOwnerUid(_, _)).WillRepeatedly(Return(0));
    NetManagerBaseAccessToken token;
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;

    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    int32_t result = netConnClient->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnManager024
 * @tc.desc: Test NetConnManager GetConnectOwnerUid
 * @tc.type: FUNC
 */
HWTEST_F(NetConnManagerTest, NetConnManager024, TestSize.Level1)
{
    EXPECT_CALL(*mockNetConnService, GetConnectOwnerUid(_, _)).WillRepeatedly(Return(0));
    NetManagerBaseAccessToken token;
    auto netConnClient = std::make_shared<NetConnClient>();
    netConnClient->NetConnService_ = mockNetConnService;

    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "incorrect_ip_address";
    info.localPort_ = 1111;
    info.remoteAddress_ = "incorrect_ip_address";
    info.remotePort_ = 2222;
    int32_t result = netConnClient->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(result, NETMANAGER_ERR_INVALID_PARAMETER);
}
} // namespace NetManagerStandard
} // namespace OHOS

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
#include <memory>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_supplier.h"
#include "common_net_conn_callback_test.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr int32_t TEST_NETID = 12;
constexpr uint32_t TEST_SUPPLIERID = 214;
constexpr const char *TEST_IDENT = "testIdent";
}

class NetSupplierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline sptr<NetSupplier> supplier = nullptr;
};

void NetSupplierTest::SetUpTestCase()
{
    std::set<NetCap> netCaps;
    netCaps.insert(NET_CAPABILITY_INTERNET);
    supplier = new (std::nothrow) NetSupplier(NetBearType::BEARER_ETHERNET, TEST_IDENT, netCaps);
}

void NetSupplierTest::TearDownTestCase() {}

void NetSupplierTest::SetUp() {}

void NetSupplierTest::TearDown() {}

HWTEST_F(NetSupplierTest, GetSupplierCallbackTest001, TestSize.Level1)
{
    sptr<INetSupplierCallback> callBack = supplier->GetSupplierCallback();
    EXPECT_TRUE(callBack == nullptr);
}

HWTEST_F(NetSupplierTest, GetSupplierCallbackTest002, TestSize.Level1)
{
    sptr<INetSupplierCallback> callback = new (std::nothrow) NetSupplierCallbackStubTestCb();
    ASSERT_NE(callback, nullptr);
    supplier->RegisterSupplierCallback(callback);
    ASSERT_NE(supplier->GetSupplierCallback(), nullptr);
}

HWTEST_F(NetSupplierTest, UpdateNetSupplierInfoTest001, TestSize.Level1)
{
    sptr<INetSupplierCallback> callBack = supplier->GetSupplierCallback();
    EXPECT_TRUE(callBack != nullptr);
}

HWTEST_F(NetSupplierTest, UpdateNetLinkInfoTest001, TestSize.Level1)
{
    NetLinkInfo netLinkInfo{};
    supplier->netSupplierIdent_ = "simId";

    int32_t ret = supplier->UpdateNetLinkInfo(netLinkInfo);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetSupplierTest, SupplierConnectionTest001, TestSize.Level1)
{
    NetLinkInfo netLinkInfo{};
    std::set<NetCap> netCaps;
    NetRequest netRequest;
    supplier->netSupplierInfo_.isAvailable_ = true;
    supplier->netSupplierIdent_ = "Supplier";
    bool ret = supplier->SupplierConnection(netCaps, netRequest);
    EXPECT_TRUE(ret);

    supplier->netSupplierInfo_.isAvailable_ = false;
    supplier->netSupplierIdent_ = "simId";
    sptr<INetSupplierCallback> callback;
    supplier->netController_ = callback;
    ret = supplier->SupplierConnection(netCaps, netRequest);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetSupplierTest, SupplierDisconnectionTest001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    uint32_t uid = 0;
    supplier->netSupplierInfo_.isAvailable_ = true;
    supplier->netSupplierIdent_ = "simId";
    sptr<INetSupplierCallback> callback;
    supplier->netController_ = callback;
    bool ret = supplier->SupplierDisconnection(netCaps, uid);
    EXPECT_FALSE(ret);

    supplier->netSupplierInfo_.isAvailable_ = false;
    netCaps.insert(NetCap::NET_CAPABILITY_NOT_METERED);
    ret = supplier->SupplierDisconnection(netCaps, uid);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetSupplierTest, IsConnectedTest001, TestSize.Level1)
{
    supplier->network_ = nullptr;
    bool ret = supplier->IsConnected();
    EXPECT_FALSE(ret);
}

HWTEST_F(NetSupplierTest, ReceiveBestScoreTest001, TestSize.Level1)
{
    int32_t bestScore = 1;
    uint32_t supplierId = 0;
    supplier->supplierId_ = 0;
    NetBearType supplierType = NetBearType::BEARER_DEFAULT;
    NetRequest netrequest;
    netrequest.bearTypes.insert(supplierType);
    supplier->InitNetScore();
    supplier->ReceiveBestScore(bestScore, supplierId, netrequest);
    EXPECT_TRUE(supplierId == supplier->supplierId_);

    supplierId = 1;
    supplier->requestList_.insert(1);
    supplier->ReceiveBestScore(bestScore, supplierId, netrequest);
    EXPECT_FALSE(supplier->requestList_.empty());
}

HWTEST_F(NetSupplierTest, SetNetValidTest001, TestSize.Level1)
{
    NetDetectionStatus netState = CAPTIVE_PORTAL_STATE;
    supplier->netCaps_.InsertNetCap(NET_CAPABILITY_VALIDATED);
    supplier->SetNetValid(netState);
    EXPECT_FALSE(supplier->HasNetCap(NET_CAPABILITY_VALIDATED));

    netState = INVALID_DETECTION_STATE;
    supplier->netCaps_.InsertNetCap(NET_CAPABILITY_PORTAL);
    supplier->SetNetValid(netState);
    EXPECT_FALSE(supplier->HasNetCap(NET_CAPABILITY_PORTAL));
}

HWTEST_F(NetSupplierTest, SetDefaultTest001, TestSize.Level1)
{
    std::shared_ptr<Network> network = nullptr;
    supplier->SetNetwork(network);
    supplier->SetDefault();
    EXPECT_TRUE(supplier->network_ == nullptr);
}

HWTEST_F(NetSupplierTest, InitNetScoreTest001, TestSize.Level1)
{
    supplier->netSupplierType_ = BEARER_DEFAULT;
    auto iter = netTypeScore_.find(supplier->netSupplierType_);
    supplier->InitNetScore();
    EXPECT_TRUE(iter == netTypeScore_.end());
}

HWTEST_F(NetSupplierTest, NetSupplieroperatorTest001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NET_CAPABILITY_INTERNET);
    std::string netSupplierIdent = "netSupplierIdent";
    NetSupplier netSupplier1(BEARER_CELLULAR, netSupplierIdent, netCaps);
    NetSupplier netSupplier2(BEARER_CELLULAR, netSupplierIdent, netCaps);
    EXPECT_FALSE(netSupplier1 == netSupplier2);
    netSupplier2.netSupplierType_ = BEARER_BLUETOOTH;
    EXPECT_FALSE(netSupplier1 == netSupplier2);
}

HWTEST_F(NetSupplierTest, RemoveBestRequestTest001, TestSize.Level1)
{
    uint32_t reqId = 1;
    supplier->bestReqList_.insert(reqId);
    auto iter1 = supplier->bestReqList_.find(reqId);
    EXPECT_TRUE(iter1 != supplier->bestReqList_.end());
    supplier->RemoveBestRequest(reqId);
    auto iter2 = supplier->bestReqList_.find(reqId);
    EXPECT_TRUE(iter2 == supplier->bestReqList_.end());
}

HWTEST_F(NetSupplierTest, SetNetworkTest001, TestSize.Level1)
{
    std::shared_ptr<Network> network = nullptr;
    supplier->SetNetwork(network);
    ASSERT_EQ(supplier->GetSupplierCallback(), nullptr);
}

HWTEST_F(NetSupplierTest, GetRealScoreTest001, TestSize.Level1)
{
    supplier->isAcceptUnvaliad = true;
    auto result = supplier->GetRealScore();
    EXPECT_EQ(result, 100);
}

HWTEST_F(NetSupplierTest, ResumeNetworkInfoTest001, TestSize.Level1)
{
    auto result = supplier->ResumeNetworkInfo();
    EXPECT_EQ(result, false);
}

HWTEST_F(NetSupplierTest, TechToTypeTest001, TestSize.Level1)
{
    NetSlotTech techType = NetSlotTech::SLOT_TYPE_GSM;
    auto result = supplier->TechToType(techType);
    EXPECT_EQ(result, "2G");
    techType = NetSlotTech::SLOT_TYPE_LTE_CA;
    result = supplier->TechToType(techType);
    EXPECT_EQ(result, "4G");
}

HWTEST_F(NetSupplierTest, NetExtAttributeTest001, TestSize.Level1)
{
    supplier->SetNetExtAttribute(TEST_IDENT);
    EXPECT_EQ(supplier->GetNetExtAttribute(), TEST_IDENT);
}

HWTEST_F(NetSupplierTest, UpdateNetLinkInfo, TestSize.Level1) {
    NetLinkInfo netLinkInfo1;
    EXPECT_EQ(supplier->UpdateNetLinkInfo(netLinkInfo1), NET_CONN_ERR_INVALID_NETWORK);
    supplier->netSupplierInfo_.isAvailable_ = true;
    supplier->network_ = std::make_shared<Network>(0, 0, BEARER_CELLULAR, nullptr);
    EXPECT_EQ(supplier->UpdateNetLinkInfo(netLinkInfo1), NETMANAGER_SUCCESS);
    supplier->netSupplierInfo_.isAvailable_ = false;
    EXPECT_EQ(supplier->UpdateNetLinkInfo(netLinkInfo1), NET_CONN_ERR_INVALID_NETWORK);
}

HWTEST_F(NetSupplierTest, UpdateNetCapTest001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NET_CAPABILITY_INTERNET);
    netCaps.insert(NET_CAPABILITY_NOT_METERED);

    supplier->UpdateNetCap(netCaps);

    EXPECT_TRUE(supplier->netCaps_.HasNetCap(NET_CAPABILITY_INTERNET));
    EXPECT_TRUE(supplier->netCaps_.HasNetCap(NET_CAPABILITY_NOT_METERED));

    std::unique_lock<std::shared_mutex> lock(supplier->netAllCapabilities_.netCapsMutex_);
    EXPECT_TRUE(supplier->netAllCapabilities_.netCaps_.count(NET_CAPABILITY_INTERNET) > 0);
    EXPECT_TRUE(supplier->netAllCapabilities_.netCaps_.count(NET_CAPABILITY_NOT_METERED) > 0);
}

HWTEST_F(NetSupplierTest, UpdateNetCapTest002, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    netCaps.insert(NET_CAPABILITY_MMS);

    supplier->UpdateNetCap(netCaps);
    EXPECT_TRUE(supplier->netCaps_.HasNetCap(NET_CAPABILITY_MMS));
    EXPECT_FALSE(supplier->netCaps_.HasNetCap(NET_CAPABILITY_INTERNET));

    std::unique_lock<std::shared_mutex> lock(supplier->netAllCapabilities_.netCapsMutex_);
    EXPECT_TRUE(supplier->netAllCapabilities_.netCaps_.count(NET_CAPABILITY_MMS) > 0);
    EXPECT_FALSE(supplier->netAllCapabilities_.netCaps_.count(NET_CAPABILITY_INTERNET) > 0);
}

HWTEST_F(NetSupplierTest, SetOnceSuppress001, TestSize.Level1)
{
    supplier->netSupplierInfo_.isAvailable_ = false;
    supplier->SetOnceSuppress();
    EXPECT_TRUE(supplier->isOnceSuppress_);
    supplier->netSupplierInfo_.isAvailable_ = true;
    supplier->SetOnceSuppress();
    EXPECT_FALSE(supplier->isOnceSuppress_);
    supplier->netQuality_ = QUALITY_GOOD_STATE;
    supplier->SetOnceSuppress();
    EXPECT_FALSE(supplier->isOnceSuppress_);
    supplier->SetNetValid(QUALITY_GOOD_STATE);
    EXPECT_EQ(supplier->netQuality_, QUALITY_GOOD_STATE);
}

HWTEST_F(NetSupplierTest, RequestToConnect001, TestSize.Level1)
{
    std::set<NetCap> netCpas;
    auto netSupplier = std::make_shared<NetSupplier>(NetBearType::BEARER_CELLULAR, "Test", netCpas);
    NetRequest netrequest;
    netrequest.isControlled = true;
    bool ret = netSupplier->RequestToConnect(netrequest);
    EXPECT_TRUE(true);
}

HWTEST_F(NetSupplierTest, SelectAsBestNetwork001, TestSize.Level1)
{
    std::set<NetCap> netCpas;
    auto netSupplier = std::make_shared<NetSupplier>(NetBearType::BEARER_WIFI, "Test", netCpas);
    NetRequest netrequest;
    netrequest.requestId = 1;
    netrequest.isControlled = true;
    bool ret = netSupplier->SelectAsBestNetwork(netrequest);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetSupplierTest, SelectAsBestNetwork002, TestSize.Level1)
{
    std::set<NetCap> netCpas;
    auto netSupplier = std::make_shared<NetSupplier>(NetBearType::BEARER_WIFI, "Test", netCpas);
    netSupplier->requestList_.insert(1);
    NetRequest netrequest;
    netrequest.requestId = 1;
    netrequest.isControlled = false;
    bool ret = netSupplier->SelectAsBestNetwork(netrequest);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetSupplierTest, SelectAsBestNetwork003, TestSize.Level1)
{
    std::set<NetCap> netCpas;
    auto netSupplier = std::make_shared<NetSupplier>(NetBearType::BEARER_CELLULAR, "Test", netCpas);
    netSupplier->requestList_.insert(1);
    NetRequest netrequest;
    netrequest.requestId = 1;
    netrequest.isControlled = false;
    bool ret = netSupplier->SelectAsBestNetwork(netrequest);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetSupplierTest, SelectAsBestNetwork004, TestSize.Level1)
{
    std::set<NetCap> netCpas;
    auto netSupplier = std::make_shared<NetSupplier>(NetBearType::BEARER_CELLULAR, "Test", netCpas);
    netSupplier->requestList_.insert(1);
    NetRequest netrequest;
    netrequest.requestId = 1;
    netrequest.isControlled = true;
    bool ret = netSupplier->SelectAsBestNetwork(netrequest);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetSupplierTest, AddBestRequest001, TestSize.Level1)
{
    std::set<NetCap> netCpas;
    auto netSupplier = std::make_shared<NetSupplier>(NetBearType::BEARER_CELLULAR, "Test", netCpas);
    netSupplier->bestReqList_.insert(1);
    netSupplier->AddBestRequest(1);
    EXPECT_EQ(netSupplier->bestReqList_.size(), 1);
}
} // namespace NetManagerStandard
} // namespace OHOS

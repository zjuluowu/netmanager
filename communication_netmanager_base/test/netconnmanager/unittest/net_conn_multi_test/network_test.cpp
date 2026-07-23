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

#ifdef GTEST_API_
#define private public
#endif

#include "net_detection_callback_stub.h"
#include "net_manager_constants.h"
#include "network.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr int32_t TEST_NETID = 12;
constexpr int32_t INVALID_VALUE = 100;
constexpr uint32_t TEST_SUPPLIERID = 214;
constexpr const char *IDENT = "666";
constexpr const char *TEST_PROXY_HOST = "testHttpProxy";
constexpr const char *TEST_IFACE_NAME = "eth0";

class NetDetectionCallbackTest : public NetDetectionCallbackStub {
public:
    inline int32_t OnNetDetectionResultChanged(NetDetectionResultCode detectionResult,
                                               const std::string &urlRedirect) override
    {
        return NETMANAGER_SUCCESS;
    }
};
} // namespace

class NetworkTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<Network> instance_ = nullptr;
    static inline sptr<INetDetectionCallback> callabck_ = new (std::nothrow) NetDetectionCallbackTest();
};

void NetworkTest::SetUpTestCase()
{
    instance_ =
        std::make_shared<Network>(TEST_NETID, TEST_SUPPLIERID, NetBearType::BEARER_ETHERNET, nullptr);
}

void NetworkTest::TearDownTestCase() {}

void NetworkTest::SetUp() {}

void NetworkTest::TearDown() {}

HWTEST_F(NetworkTest, operatorTest001, TestSize.Level1)
{
    int32_t netId1 = 2;
    int32_t netId2 = 3;
    uint32_t supplierId = 4445;
    Network work0(netId1, supplierId, NetBearType::BEARER_ETHERNET, nullptr);
    Network work1(netId1, TEST_SUPPLIERID, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_TRUE(work0 == work1);
    Network work2(netId2, TEST_SUPPLIERID, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_FALSE(work1 == work2);
}

HWTEST_F(NetworkTest, GetNetIdTest001, TestSize.Level1)
{
    int32_t ret = instance_->GetNetId();
    EXPECT_EQ(ret, TEST_NETID);
}

HWTEST_F(NetworkTest, UpdateBasicNetworkTest001, TestSize.Level1)
{
    bool ret = instance_->UpdateBasicNetwork(true);
    EXPECT_TRUE(ret);
    ret = instance_->UpdateBasicNetwork(true);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, UpdateBasicNetworkTest002, TestSize.Level1)
{
    bool ret = instance_->UpdateBasicNetwork(false);
    EXPECT_TRUE(ret);
    ret = instance_->UpdateBasicNetwork(false);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoTest001, TestSize.Level1)
{
    NetLinkInfo info;
    bool ret = instance_->UpdateNetLinkInfo(info);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoTest002, TestSize.Level1)
{
    NetLinkInfo info;
    info.ifaceName_ = TEST_IFACE_NAME;
    info.ident_ = IDENT;
    instance_->UpdateStatsCached(info);
    EXPECT_TRUE(instance_ != nullptr);
}

HWTEST_F(NetworkTest, GetNetLinkInfoTest001, TestSize.Level1)
{
    NetLinkInfo ret = instance_->GetNetLinkInfo();
    EXPECT_FALSE(ret.ToString("").empty());
}

HWTEST_F(NetworkTest, UpdateTest001, TestSize.Level1)
{
    NetLinkInfo info;
    instance_->UpdateIpAddrs(info);
    instance_->UpdateInterfaces(info);
    instance_->UpdateRoutes(info);
    instance_->UpdateDns(info);
    instance_->UpdateMtu(info);
    instance_->UpdateTcpBufferSize(info);
    instance_->RegisterNetDetectionCallback(callabck_);
    int32_t ret = instance_->UnRegisterNetDetectionCallback(callabck_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkTest, StartNetDetectionTest001, TestSize.Level1)
{
    instance_->StartNetDetection(true);
    instance_->StartNetDetection(false);
    instance_->SetDefaultNetWork();
    instance_->ClearDefaultNetWorkNetId();
    instance_->UpdateNetConnState(NetConnState::NET_CONN_STATE_CONNECTING);
    bool ret = instance_->IsConnecting();
    EXPECT_TRUE(ret);
    instance_->UpdateNetConnState(NetConnState::NET_CONN_STATE_CONNECTED);
    ret = instance_->IsConnected();
    EXPECT_TRUE(ret);
    instance_->UpdateNetConnState(NetConnState::NET_CONN_STATE_DISCONNECTING);
    ret = instance_->IsConnecting();
    EXPECT_FALSE(ret);
    instance_->UpdateNetConnState(NetConnState::NET_CONN_STATE_DISCONNECTED);
    ret = instance_->IsConnected();
    EXPECT_FALSE(ret);
    instance_->UpdateNetConnState(NetConnState::NET_CONN_STATE_CONNECTING);
    std::string urlRedirect = "test_redirect";
    instance_->OnHandleNetMonitorResult(NetDetectionStatus::INVALID_DETECTION_STATE, urlRedirect);
}

HWTEST_F(NetworkTest, NetDetectionForDnsHealthTest001, TestSize.Level1)
{
    std::string urlRedirect = "test_redirect";
    instance_->OnHandleNetMonitorResult(NetDetectionStatus::INVALID_DETECTION_STATE, urlRedirect);
    instance_->NetDetectionForDnsHealth(true);
    instance_->OnHandleNetMonitorResult(NetDetectionStatus::VERIFICATION_STATE, urlRedirect);
    instance_->NetDetectionForDnsHealth(false);
    bool ret = instance_->IsConnecting();
    EXPECT_TRUE(ret);
    instance_->UpdateNetConnState(NetConnState::NET_CONN_STATE_CONNECTED);
    ret = instance_->IsConnected();
    EXPECT_TRUE(ret);
    instance_->UpdateNetConnState(NetConnState::NET_CONN_STATE_DISCONNECTING);
    ret = instance_->IsConnecting();
    EXPECT_FALSE(ret);
    instance_->UpdateNetConnState(NetConnState::NET_CONN_STATE_DISCONNECTED);
    ret = instance_->IsConnected();
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, NetworkTestBranchTest001, TestSize.Level1)
{
    sptr<INetDetectionCallback> callback = nullptr;
    instance_->RegisterNetDetectionCallback(callback);
    auto ret = instance_->UnRegisterNetDetectionCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    HttpProxy httpProxy = {TEST_PROXY_HOST, 0, {}};
    instance_->UpdateGlobalHttpProxy(httpProxy);

    NetConnState netConnState = NetConnState::NET_CONN_STATE_UNKNOWN;
    instance_->UpdateNetConnState(netConnState);
    EXPECT_EQ(instance_->state_, NetConnState::NET_CONN_STATE_UNKNOWN);

    instance_->netMonitor_ = nullptr;
    instance_->NetDetectionForDnsHealth(true);
    instance_->UpdateGlobalHttpProxy(httpProxy);

    int32_t internalRet = static_cast<int32_t>(VERIFICATION_STATE);
    NetDetectionResultCode code = instance_->NetDetectionResultConvert(internalRet);
    EXPECT_EQ(code, NetDetectionResultCode::NET_DETECTION_SUCCESS);

    internalRet = static_cast<int32_t>(CAPTIVE_PORTAL_STATE);
    code = instance_->NetDetectionResultConvert(internalRet);
    EXPECT_EQ(code, NetDetectionResultCode::NET_DETECTION_CAPTIVE_PORTAL);

    code = instance_->NetDetectionResultConvert(INVALID_VALUE);
    EXPECT_EQ(code, NetDetectionResultCode::NET_DETECTION_FAIL);
}

HWTEST_F(NetworkTest, ReleaseBasicNetworkTest001, TestSize.Level1)
{
    instance_->isNeedResume_ = true;
    auto ret = instance_->ReleaseBasicNetwork();
    EXPECT_EQ(ret, true);
}

HWTEST_F(NetworkTest, OnHandleNetMonitorResultTest001, TestSize.Level1)
{
    NetDetectionStatus netDetectionState = INVALID_DETECTION_STATE;
    std::string urlRedirect;
    std::shared_ptr<AppExecFwk::EventRunner> runner;
    instance_->eventHandler_ = std::make_shared<NetConnEventHandler>(runner);
    instance_->OnHandleNetMonitorResult(netDetectionState, urlRedirect);
    EXPECT_EQ(urlRedirect.size(), 0);
}

HWTEST_F(NetworkTest, ResumeNetworkInfoTest001, TestSize.Level1)
{
    auto result = instance_->ResumeNetworkInfo();
    EXPECT_EQ(result, true);
}

HWTEST_F(NetworkTest, OnHandleDualStackProbeResultTest001, TestSize.Level1)
{
    NetManagerStandard::DualStackProbeResultCode result = NetManagerStandard::DualStackProbeResultCode::PROBE_FAIL;
    instance_->OnHandleDualStackProbeResult(result);
    EXPECT_EQ(instance_->dualStackProbeCallback_.size(), 0);
}
} // namespace NetManagerStandard
} // namespace OHOS
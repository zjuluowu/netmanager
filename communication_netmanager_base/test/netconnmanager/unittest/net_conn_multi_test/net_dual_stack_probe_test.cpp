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
#endif
#include "net_dual_stack_probe.h"
#include "net_conn_service.h"
#include "net_manager_constants.h"
#include "i_net_monitor_callback.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr int32_t TEST_NETID = 999;
constexpr const char *TEST_DOMAIN = "connectivitycheck.platform.hicloud.com";
constexpr const char *TEST_HTTP_URL = "http://connectivitycheck.platform.hicloud.com/generate_204";
constexpr const char *TEST_HTTPS_URL = "https://connectivitycheck.platform.hicloud.com/generate_204";
class TestMonitorCallback : public INetMonitorCallback {
public:
    inline void OnHandleNetMonitorResult(NetDetectionStatus netDetectionState, const std::string &urlRedirect) override
    {
        (void)netDetectionState;
        (void)urlRedirect;
    }
    inline void OnHandleDualStackProbeResult(DualStackProbeResultCode dualStackProbeResultCode) override
    {
        (void)dualStackProbeResultCode;
    }
};
}

class NetDualStackProbeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static std::shared_ptr<NetDualStackProbe> CreateNetDualStackProbeInstance();
};

void NetDualStackProbeTest::SetUpTestCase() {}

void NetDualStackProbeTest::TearDownTestCase() {}

void NetDualStackProbeTest::SetUp() {}

void NetDualStackProbeTest::TearDown() {}

std::shared_ptr<NetDualStackProbe> NetDualStackProbeTest::CreateNetDualStackProbeInstance()
{
    std::weak_ptr<TestMonitorCallback> netMonitorCallback = std::make_shared<TestMonitorCallback>();
    int32_t netId = TEST_NETID;
    NetConnService::GetInstance()->GetDefaultNet(netId);
    std::string httpUrl = std::string(TEST_HTTP_URL);
    std::string httpsUrl = std::string(TEST_HTTPS_URL);
    auto instance = std::make_shared<NetDualStackProbe>(netId, NetBearType::BEARER_WIFI,
        NetLinkInfo(), httpUrl, httpsUrl, netMonitorCallback);
    return instance;
}

HWTEST_F(NetDualStackProbeTest, StartDualStackProbeThreadTest001, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    netDualStackProbe->isDualStackProbing_ = true;
    int32_t ret = netDualStackProbe->StartDualStackProbeThread(TEST_DOMAIN, TEST_DOMAIN, 0);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);

    
    netDualStackProbe->isDualStackProbing_ = false;
    ret = netDualStackProbe->StartDualStackProbeThread(TEST_DOMAIN, TEST_DOMAIN, 0);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetDualStackProbeTest, StartDualStackProbeTest001, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    netDualStackProbe->isDualStackProbing_ = true;
    std::shared_ptr<TestMonitorCallback> sp;
    netDualStackProbe->netMonitorCallback_ = sp;
    netDualStackProbe->StartDualStackProbe(TEST_DOMAIN, TEST_DOMAIN, 1000);
    EXPECT_FALSE(netDualStackProbe->isDualStackProbing_);

    netDualStackProbe->isDualStackProbing_ = true;
    std::weak_ptr<TestMonitorCallback> netMonitorCallback = std::make_shared<TestMonitorCallback>();
    netDualStackProbe->netMonitorCallback_ = netMonitorCallback;
    netDualStackProbe->StartDualStackProbe(TEST_DOMAIN, TEST_DOMAIN, 1000);
    EXPECT_FALSE(netDualStackProbe->isDualStackProbing_);

    netDualStackProbe->StartDualStackProbe(TEST_DOMAIN, TEST_DOMAIN, 1000);
    EXPECT_FALSE(netDualStackProbe->isDualStackProbing_);
}

HWTEST_F(NetDualStackProbeTest, StopDualStackProbeTest001, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    netDualStackProbe->isDualStackProbing_ = true;
    netDualStackProbe->StopDualStackProbe();
    EXPECT_FALSE(netDualStackProbe->isDualStackProbing_);
}

HWTEST_F(NetDualStackProbeTest, DualStackProbeTest001, TestSize.Level1)
{
    std::string testDomain = "";
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    auto resultCode = netDualStackProbe->DualStackProbe(testDomain, testDomain, 1000);
    EXPECT_EQ(resultCode, DualStackProbeResultCode::PROBE_FAIL);

    std::string domain = CommonUtils::ExtractDomainFormUrl(TEST_HTTP_URL);
    resultCode = netDualStackProbe->DualStackProbe(domain, TEST_DOMAIN, 1000);
    EXPECT_NE(resultCode, DualStackProbeResultCode::PROBE_PORTAL);
}

HWTEST_F(NetDualStackProbeTest, DoDnsResolveTest001, TestSize.Level1)
{
    std::string testDomain = "";
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    std::string ipv4AddrList;
    std::string ipv6AddrList;
    netDualStackProbe->DoDnsResolve(TEST_DOMAIN, TEST_DOMAIN, ipv4AddrList, ipv6AddrList);
    ipv4AddrList = "";
    ipv6AddrList = "";
    netDualStackProbe->DoDnsResolve(testDomain, testDomain, ipv4AddrList, ipv6AddrList);
    EXPECT_TRUE(ipv6AddrList.empty());
}

HWTEST_F(NetDualStackProbeTest, ProcessDnsResolveResultTest001, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    std::string testString1 = "";
    std::string testString2 = "";
    EXPECT_TRUE(netDualStackProbe->ProcessDnsResolveResult(testString1, testString2).empty());
    testString1 = "test";
    EXPECT_FALSE(netDualStackProbe->ProcessDnsResolveResult(testString1, testString2).empty());
    testString1 = "test";
    testString2 = "";
    EXPECT_FALSE(netDualStackProbe->ProcessDnsResolveResult(testString1, testString2).empty());
    testString2 = "test";
    EXPECT_FALSE(netDualStackProbe->ProcessDnsResolveResult(testString1, testString2).empty());
}

HWTEST_F(NetDualStackProbeTest, DoDualStackHttpProbeTest001, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    std::string ipv4AddrList;
    std::string ipv6AddrList;
    EXPECT_EQ(netDualStackProbe->DoDualStackHttpProbe(ipv4AddrList, ipv6AddrList, 0),
        DualStackProbeResultCode::PROBE_FAIL);
    ipv4AddrList = "192.168.1.1";
    EXPECT_EQ(netDualStackProbe->DoDualStackHttpProbe(ipv4AddrList, ipv6AddrList, 0),
        DualStackProbeResultCode::PROBE_FAIL);
    ipv4AddrList = "";
    ipv6AddrList = "192.168.1.1";
    EXPECT_EQ(netDualStackProbe->DoDualStackHttpProbe(ipv4AddrList, ipv6AddrList, 0),
        DualStackProbeResultCode::PROBE_FAIL);
    ipv4AddrList = "192.168.1.1";
    EXPECT_NE(netDualStackProbe->DoDualStackHttpProbe(ipv4AddrList, ipv6AddrList, 0),
        DualStackProbeResultCode::PROBE_PORTAL);
}

HWTEST_F(NetDualStackProbeTest, ProcessProbeResultTest001, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    auto httpThreadV4 = std::make_shared<ProbeThread>(
            TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
            ProbeType::PROBE_HTTP, TEST_HTTP_URL, TEST_HTTPS_URL);
    auto httpsThreadV4 = std::make_shared<ProbeThread>(
            TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
            ProbeType::PROBE_HTTPS, TEST_HTTP_URL, TEST_HTTPS_URL);
    auto httpThreadV6 = std::make_shared<ProbeThread>(
            TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
            ProbeType::PROBE_HTTP, TEST_HTTP_URL, TEST_HTTPS_URL);
    auto httpsThreadV6 = std::make_shared<ProbeThread>(
            TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
            ProbeType::PROBE_HTTPS, TEST_HTTP_URL, TEST_HTTPS_URL);

    httpThreadV4->isDetecting_ = false;
    httpsThreadV4->isDetecting_ = false;
    httpThreadV6->isDetecting_ = false;
    httpsThreadV6->isDetecting_ = false;

    auto ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_EQ(ret, DualStackProbeResultCode::PROBE_FAIL);

    httpThreadV4->httpProbe_->httpProbeResult_.responseCode_ = 204;
    httpThreadV6->httpProbe_->httpProbeResult_.responseCode_ = 204;
    ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_NE(ret, DualStackProbeResultCode::PROBE_FAIL);

    httpThreadV4->httpProbe_->httpProbeResult_.responseCode_ = 302;
    ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_NE(ret, DualStackProbeResultCode::PROBE_FAIL);

    httpThreadV4->httpProbe_->httpProbeResult_.responseCode_ = 0;
    ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_NE(ret, DualStackProbeResultCode::PROBE_FAIL);

    httpThreadV4->httpProbe_->httpProbeResult_.responseCode_ = 204;
    httpThreadV6->httpProbe_->httpProbeResult_.responseCode_ = 0;
    ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_NE(ret, DualStackProbeResultCode::PROBE_FAIL);
}

HWTEST_F(NetDualStackProbeTest, ProcessProbeResultTest002, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    auto httpThreadV4 = std::make_shared<ProbeThread>(
        TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
        ProbeType::PROBE_HTTP, TEST_HTTP_URL, TEST_HTTPS_URL);
    auto httpsThreadV4 = std::make_shared<ProbeThread>(
        TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
        ProbeType::PROBE_HTTPS, TEST_HTTP_URL, TEST_HTTPS_URL);
    auto httpThreadV6 = std::make_shared<ProbeThread>(
        TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
        ProbeType::PROBE_HTTP, TEST_HTTP_URL, TEST_HTTPS_URL);
    auto httpsThreadV6 = std::make_shared<ProbeThread>(
        TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
        ProbeType::PROBE_HTTPS, TEST_HTTP_URL, TEST_HTTPS_URL);

    httpThreadV4->isDetecting_ = false;
    httpsThreadV4->isDetecting_ = false;
    httpThreadV6->isDetecting_ = false;
    httpsThreadV6->isDetecting_ = false;

    httpThreadV4->probeDuration_ = 1 * 1000;
    httpsThreadV4->probeDuration_ = 10 * 1000;
    httpThreadV4->httpProbe_->httpProbeResult_.responseCode_ = 204;
    httpThreadV6->httpProbe_->httpProbeResult_.responseCode_ = 0;
    auto ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_NE(ret, DualStackProbeResultCode::PROBE_FAIL);

    httpThreadV4->probeDuration_ = 1 * 1000;
    ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_NE(ret, DualStackProbeResultCode::PROBE_FAIL);

    httpThreadV6->probeDuration_ = 1 * 1000;
    httpsThreadV6->probeDuration_ = 10 * 1000;
    httpThreadV4->httpProbe_->httpProbeResult_.responseCode_ = 0;
    httpThreadV6->httpProbe_->httpProbeResult_.responseCode_ = 204;
    ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_NE(ret, DualStackProbeResultCode::PROBE_FAIL);

    httpThreadV6->probeDuration_ = 1 * 1000;
    ret = netDualStackProbe->ProcessProbeResult(httpThreadV4, httpsThreadV4, httpThreadV6, httpsThreadV6);
    EXPECT_NE(ret, DualStackProbeResultCode::PROBE_FAIL);
}

HWTEST_F(NetDualStackProbeTest, GetThreadDetectResultTest001, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    std::shared_ptr<ProbeThread> thread = nullptr;
    auto ret = netDualStackProbe->GetThreadDetectResult(thread);
    EXPECT_EQ(ret.GetCode(), 0);

    auto testThread = std::make_shared<ProbeThread>(
        TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
        ProbeType::PROBE_HTTP, TEST_HTTP_URL, TEST_HTTPS_URL);
    testThread->isDetecting_ = true;
    ret = netDualStackProbe->GetThreadDetectResult(testThread);
    EXPECT_EQ(ret.GetCode(), 0);

    testThread->isDetecting_ = false;
    ret = netDualStackProbe->GetThreadDetectResult(testThread);
    EXPECT_EQ(ret.GetCode(), 0);

    testThread->probeType_ = ProbeType::PROBE_HTTP_FALLBACK;
    ret = netDualStackProbe->GetThreadDetectResult(testThread);
    EXPECT_EQ(ret.GetCode(), 0);

    testThread->probeType_ = ProbeType::PROBE_HTTPS;
    ret = netDualStackProbe->GetThreadDetectResult(testThread);
    EXPECT_EQ(ret.GetCode(), 0);
}

HWTEST_F(NetDualStackProbeTest, GetProbeDurationTimeTest001, TestSize.Level1)
{
    auto netDualStackProbe = CreateNetDualStackProbeInstance();
    std::shared_ptr<ProbeThread> httpThread = nullptr;
    std::shared_ptr<ProbeThread> httpsThread = nullptr;
    auto durationTime = netDualStackProbe->GetProbeDurationTime(httpThread, httpsThread);
    EXPECT_EQ(durationTime, 0);

    httpThread = std::make_shared<ProbeThread>(
    TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
    ProbeType::PROBE_HTTP, TEST_HTTP_URL, TEST_HTTPS_URL);
    durationTime = netDualStackProbe->GetProbeDurationTime(httpThread, httpsThread);
    EXPECT_EQ(durationTime, 0);

    httpsThread = std::make_shared<ProbeThread>(
        TEST_NETID, NetBearType::BEARER_WIFI, NetLinkInfo(), nullptr, nullptr,
        ProbeType::PROBE_HTTPS, TEST_HTTP_URL, TEST_HTTPS_URL);
    durationTime = netDualStackProbe->GetProbeDurationTime(httpThread, httpsThread);
    EXPECT_EQ(durationTime, 0);

    httpThread->isDetecting_ = false;
    httpsThread->isDetecting_ = false;

    httpThread->httpProbe_->httpProbeResult_.responseCode_ = 204;
    httpThread->probeDuration_ = 1 * 1000;
    durationTime = netDualStackProbe->GetProbeDurationTime(httpThread, httpsThread);
    EXPECT_NE(durationTime, 0);

    httpsThread->httpProbe_->httpsProbeResult_.responseCode_ = 204;
    httpsThread->probeDuration_ = 2 * 1000;
    durationTime = netDualStackProbe->GetProbeDurationTime(httpThread, httpsThread);
    EXPECT_NE(durationTime, 0);

    httpThread->httpProbe_->httpProbeResult_.responseCode_ = 0;
    durationTime = netDualStackProbe->GetProbeDurationTime(httpThread, httpsThread);
    EXPECT_NE(durationTime, 0);
}
} // namespace NetManagerStandard
} // namespace OHOS

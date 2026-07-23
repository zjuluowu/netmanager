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

#include <netdb.h>
#include <gtest/gtest.h>
#include <sys/socket.h>

#include "i_net_monitor_callback.h"
#include "net_manager_constants.h"
#define private public
#include "net_monitor.h"
#include "net_conn_service.h"
#undef private
#include "netmanager_base_common_utils.h"
#include "net_http_proxy_tracker.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr uint64_t LAST_DETECT_TIME = 0;
constexpr uint32_t TEST_NETID = 999;
constexpr int32_t SUCCESS_CODE = 204;
constexpr int32_t PORTAL_CODE_MIN = 200;
constexpr int32_t SIM_PORTAL_CODE = 302;
constexpr int32_t MAX_FAILED_DETECTION_DELAY_MS = 10 * 60 * 1000;
constexpr int32_t ONE_URL_DETECT_NUM = 4;
constexpr int32_t ALL_DETECT_THREAD_NUM = 8;
constexpr const char *NET_CONN_MANAGER_WORK_THREAD = "NET_CONN_MANAGER_WORK_THREAD";

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
} // namespace

class NetMonitorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<INetMonitorCallback> callback_ = std::make_shared<TestMonitorCallback>();
    static inline NetMonitorInfo info = {true, 0};
    static inline std::shared_ptr<NetMonitor> instance_ =
        std::make_shared<NetMonitor>(TEST_NETID, BEARER_DEFAULT, NetLinkInfo(), callback_, info);
    static std::shared_ptr<NetMonitor> CreateNetMonitorInstance();
};

void NetMonitorTest::SetUpTestCase()
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
    instance_->Start();
}

void NetMonitorTest::TearDownTestCase()
{
    instance_->Stop();
}

void NetMonitorTest::SetUp() {}

void NetMonitorTest::TearDown() {}

std::shared_ptr<NetMonitor> NetMonitorTest::CreateNetMonitorInstance()
{
    auto cb = std::make_shared<TestMonitorCallback>();
    NetMonitorInfo info = {true, 0};
    auto netMonitor = std::make_shared<NetMonitor>(
        TEST_NETID, BEARER_DEFAULT, NetLinkInfo(), cb, info);
    return netMonitor;
}

HWTEST_F(NetMonitorTest, IsDetectingTest001, TestSize.Level1)
{
    bool ret = instance_->IsDetecting();
    EXPECT_TRUE(ret);
    instance_->Detection();
    instance_->Stop();
}

HWTEST_F(NetMonitorTest, SendHttpProbe001, TestSize.Level1)
{
    std::string domain;
    std::string urlPath;
    NetHttpProbeResult probeResult = instance_->SendProbe();
    EXPECT_EQ(probeResult.IsFailed(), true);
}

HWTEST_F(NetMonitorTest, GetHttpProbeUrlFromConfig001, TestSize.Level1)
{
    instance_->GetHttpProbeUrlFromConfig();
    EXPECT_FALSE(instance_->httpUrl_.empty());
    EXPECT_FALSE(instance_->httpsUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpsUrl_.empty());
}
HWTEST_F(NetMonitorTest, GetHttpProbeUrlFromDataShare001, TestSize.Level1)
{
    instance_->GetHttpProbeUrlFromDataShare();
    EXPECT_TRUE(instance_->httpUrl_.empty());
    EXPECT_TRUE(instance_->httpsUrl_.empty());
    EXPECT_TRUE(instance_->fallbackHttpUrl_.empty());
    EXPECT_TRUE(instance_->fallbackHttpsUrl_.empty());
}

HWTEST_F(NetMonitorTest, GetHttpProbeUrlFromDataShare002, TestSize.Level1)
{
    ProbeUrls testProbeUrls;
    testProbeUrls.httpProbeUrlExt = "http://test.com/probe";
    testProbeUrls.httpsProbeUrlExt = "https://test.com/probe";
    testProbeUrls.fallbackHttpProbeUrlExt = "http://fallback.com/probe";
    testProbeUrls.fallbackHttpsProbeUrlExt = "";
    NetConnService::GetInstance()->probeUrl_ = testProbeUrls;
    bool result = instance_->GetHttpProbeUrlFromDataShare();

    EXPECT_FALSE(result);
    EXPECT_FALSE(instance_->httpUrl_.empty());
    EXPECT_FALSE(instance_->httpsUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpUrl_.empty());
    EXPECT_TRUE(instance_->fallbackHttpsUrl_.empty());
}

HWTEST_F(NetMonitorTest, GetHttpProbeUrlFromDataShare003, TestSize.Level1)
{
    ProbeUrls testProbeUrls;
    testProbeUrls.httpProbeUrlExt = "http://test.com/probe";
    testProbeUrls.httpsProbeUrlExt = "https://test.com/probe";
    testProbeUrls.fallbackHttpProbeUrlExt = "http://fallback.com/probe";
    testProbeUrls.fallbackHttpsProbeUrlExt = "https://fallback.com/probe";
    NetConnService::GetInstance()->probeUrl_ = testProbeUrls;
    bool result = instance_->GetHttpProbeUrlFromDataShare();

    EXPECT_TRUE(result);
    EXPECT_FALSE(instance_->httpUrl_.empty());
    EXPECT_FALSE(instance_->httpsUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpsUrl_.empty());
}

HWTEST_F(NetMonitorTest, GetHttpProbeUrlFromDataShare004, TestSize.Level1)
{
    instance_->isNeedSuffix_ = false;
    bool result = instance_->GetHttpProbeUrlFromDataShare();

    EXPECT_TRUE(result);
    EXPECT_FALSE(instance_->httpUrl_.empty());
    EXPECT_FALSE(instance_->httpsUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpsUrl_.empty());
    EXPECT_EQ(instance_->httpUrl_.find("_"), std::string::npos);
}

HWTEST_F(NetMonitorTest, GetHttpProbeUrlFromDataShare005, TestSize.Level1)
{
    instance_->isNeedSuffix_ = true;
    bool result = instance_->GetHttpProbeUrlFromDataShare();

    EXPECT_TRUE(result);
    EXPECT_FALSE(instance_->httpUrl_.empty());
    EXPECT_FALSE(instance_->httpsUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpsUrl_.empty());
    EXPECT_NE(instance_->httpUrl_.find("_"), std::string::npos);
}

HWTEST_F(NetMonitorTest, GetXReqIDFromConfig001, TestSize.Level1)
{
    instance_->GetXReqIDFromConfig();
    EXPECT_FALSE(instance_->xReqId_.empty());
    EXPECT_FALSE(instance_->xReqIdLen_ == 0);
}

HWTEST_F(NetMonitorTest, StartTest001, TestSize.Level1)
{
    bool ret = instance_->IsDetecting();
    EXPECT_FALSE(ret);
    instance_->Start();
    instance_->Stop();
    ret = instance_->IsDetecting();
    EXPECT_FALSE(ret);
}

HWTEST_F(NetMonitorTest, ProcessDetectionTest001, TestSize.Level1)
{
    NetHttpProbeResult probeResult;
    probeResult.responseCode_ = 204;
    NetDetectionStatus result;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, VERIFICATION_STATE);
    probeResult.responseCode_ = 302;
    instance_->netBearType_ = BEARER_CELLULAR;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);
    probeResult.responseCode_ = 200;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);
    probeResult.responseCode_ = 302;
    instance_->netBearType_ = BEARER_WIFI;
    instance_->detectionDelay_ = 0;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);
    instance_->detectionDelay_ = 10 * 60 * 1000;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);
    instance_->isDetecting_ = true;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);
    instance_->isDetecting_ = false;
    probeResult.responseCode_ = 0;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);
}

HWTEST_F(NetMonitorTest, DetectionTest001, TestSize.Level1)
{
    instance_->isDetecting_ = true;
    instance_->Detection();
    instance_->isDetecting_ = false;
    instance_->Detection();
    instance_->Stop();
    bool ret = instance_->IsDetecting();
    EXPECT_FALSE(ret);
}

HWTEST_F(NetMonitorTest, DetectionTest002, TestSize.Level1)
{
    instance_->isDetecting_ = true;
    instance_->detectionDelay_ = 0;
    instance_->Start();
    EXPECT_TRUE(instance_->IsDetecting());
    instance_->detectionDelay_ = 1;
    instance_->Start();
    EXPECT_TRUE(instance_->IsDetecting());
}

HWTEST_F(NetMonitorTest, CheckIfSettingsDataReadyTest001, TestSize.Level1)
{
    instance_->isDataShareReady_ = true;
    EXPECT_EQ(instance_->CheckIfSettingsDataReady(), true);
}

HWTEST_F(NetMonitorTest, ProcessDetectionTest002, TestSize.Level1)
{
    instance_->isDetecting_ = true;
    instance_->detectionDelay_ = 1;
    auto monitorCallback = instance_->netMonitorCallback_.lock();
    instance_->netMonitorCallback_.reset();
    instance_->Start();
    instance_->netMonitorCallback_ = monitorCallback;

    instance_->isDetecting_ = false;
    NetHttpProbeResult probeResult;
    probeResult.responseCode_ = SIM_PORTAL_CODE + 1;
    NetDetectionStatus result;
    instance_->isScreenOn_ = true;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);

    instance_->isScreenOn_ = false;
    instance_->netBearType_ = BEARER_CELLULAR;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);

    instance_->netBearType_ = BEARER_WIFI;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, CAPTIVE_PORTAL_STATE);

    probeResult.responseCode_ = PORTAL_CODE_MIN - 1;
    instance_->detectionDelay_ = MAX_FAILED_DETECTION_DELAY_MS;
    instance_->ProcessDetection(probeResult, result);
    EXPECT_EQ(result, INVALID_DETECTION_STATE);
}

HWTEST_F(NetMonitorTest, ProcessThreadDetectResultTest001, TestSize.Level1)
{
    auto latch = std::make_shared<TinyCountDownLatch>(ONE_URL_DETECT_NUM);
    auto latchAll = std::make_shared<TinyCountDownLatch>(ALL_DETECT_THREAD_NUM);
    auto netId = instance_->netId_;
    auto netBearType = instance_->netBearType_;
    auto &netLinkInfo = instance_->netLinkInfo_;
    auto httpUrl = instance_->httpUrl_;
    auto httpsUrl = instance_->httpsUrl_;
    auto httpProbeThread = std::make_shared<ProbeThread>(netId, netBearType,
        netLinkInfo, latch, latchAll, ProbeType::PROBE_HTTP, httpUrl, httpsUrl);
    auto httpsProbeThread = std::make_shared<ProbeThread>(netId, netBearType,
        netLinkInfo, latch, latchAll, ProbeType::PROBE_HTTPS, httpUrl, httpsUrl);
    auto backHttpThread = std::make_shared<ProbeThread>(netId, netBearType,
        netLinkInfo, latch, latchAll, ProbeType::PROBE_HTTP_FALLBACK, httpUrl, httpsUrl);
    auto backHttpsThread = std::make_shared<ProbeThread>(netId, netBearType,
        netLinkInfo, latch, latchAll, ProbeType::PROBE_HTTPS_FALLBACK, httpUrl, httpsUrl);
    httpProbeThread->httpProbe_->httpsProbeResult_.responseCode_ = PORTAL_CODE_MIN;
    auto ret = instance_->ProcessThreadDetectResult(httpProbeThread, httpsProbeThread, backHttpThread, backHttpsThread);
    EXPECT_EQ(ret.responseCode_, 0);

    httpProbeThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    backHttpThread->httpProbe_->httpsProbeResult_.responseCode_ = PORTAL_CODE_MIN;
    ret = instance_->ProcessThreadDetectResult(httpProbeThread, httpsProbeThread, backHttpThread, backHttpsThread);
    EXPECT_EQ(ret.responseCode_, 0);

    backHttpThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    httpsProbeThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    ret = instance_->ProcessThreadDetectResult(httpProbeThread, httpsProbeThread, backHttpThread, backHttpsThread);
    EXPECT_EQ(ret.responseCode_, SUCCESS_CODE);

    httpsProbeThread->httpProbe_->httpsProbeResult_.responseCode_ = PORTAL_CODE_MIN;
    backHttpsThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    ret = instance_->ProcessThreadDetectResult(httpProbeThread, httpsProbeThread, backHttpThread, backHttpsThread);
    EXPECT_EQ(ret.responseCode_, SUCCESS_CODE);
}

HWTEST_F(NetMonitorTest, ProcessThreadDetectResultTest002, TestSize.Level1)
{
    auto latch = std::make_shared<TinyCountDownLatch>(ONE_URL_DETECT_NUM);
    auto latchAll = std::make_shared<TinyCountDownLatch>(ALL_DETECT_THREAD_NUM);
    auto netId = instance_->netId_;
    auto netBearType = instance_->netBearType_;
    auto &netLinkInfo = instance_->netLinkInfo_;
    auto httpUrl = instance_->httpUrl_;
    auto httpsUrl = instance_->httpsUrl_;
    auto httpProbeThread = std::make_shared<ProbeThread>(netId, netBearType,
        netLinkInfo, latch, latchAll, ProbeType::PROBE_HTTP, httpUrl, httpsUrl);
    auto httpsProbeThread = std::make_shared<ProbeThread>(netId, netBearType,
        netLinkInfo, latch, latchAll, ProbeType::PROBE_HTTPS, httpUrl, httpsUrl);
    auto backHttpThread = std::make_shared<ProbeThread>(netId, netBearType,
        netLinkInfo, latch, latchAll, ProbeType::PROBE_HTTP_FALLBACK, httpUrl, httpsUrl);
    auto backHttpsThread = std::make_shared<ProbeThread>(netId, netBearType,
        netLinkInfo, latch, latchAll, ProbeType::PROBE_HTTPS_FALLBACK, httpUrl, httpsUrl);
    httpProbeThread->httpProbe_->httpsProbeResult_.responseCode_ = PORTAL_CODE_MIN - 1;
    backHttpsThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    httpsProbeThread->httpProbe_->httpsProbeResult_.responseCode_ = PORTAL_CODE_MIN;
    backHttpsThread->httpProbe_->httpsProbeResult_.responseCode_ = PORTAL_CODE_MIN;
    auto ret = instance_->ProcessThreadDetectResult(httpProbeThread, httpsProbeThread, backHttpThread, backHttpsThread);
    EXPECT_EQ(ret.responseCode_, PORTAL_CODE_MIN);

    httpProbeThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    backHttpThread->httpProbe_->httpsProbeResult_.responseCode_ = PORTAL_CODE_MIN - 1;
    ret = instance_->ProcessThreadDetectResult(httpProbeThread, httpsProbeThread, backHttpThread, backHttpsThread);
    EXPECT_EQ(ret.responseCode_, PORTAL_CODE_MIN);

    httpProbeThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    backHttpThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    ret = instance_->ProcessThreadDetectResult(httpProbeThread, httpsProbeThread, backHttpThread, backHttpsThread);
    EXPECT_NE(ret.responseCode_, SUCCESS_CODE);
}

HWTEST_F(NetMonitorTest, GetHttpProbeUrlFromConfigTest002, TestSize.Level1)
{
    auto isNeedSuffix = instance_->isNeedSuffix_;
    instance_->isNeedSuffix_ = false;
    instance_->GetHttpProbeUrlFromConfig();
    EXPECT_FALSE(instance_->httpsUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpUrl_.empty());
    EXPECT_FALSE(instance_->fallbackHttpsUrl_.empty());
    instance_->isNeedSuffix_ = isNeedSuffix;
}

HWTEST_F(NetMonitorTest, CreateProbeThreadTest001, TestSize.Level1)
{
    std::shared_ptr<TinyCountDownLatch> latch = std::make_shared<TinyCountDownLatch>(1);
    std::shared_ptr<TinyCountDownLatch> latchAll = std::make_shared<TinyCountDownLatch>(2);
    std::shared_ptr<ProbeThread> httpThread = nullptr;
    std::shared_ptr<ProbeThread> httpsThread = nullptr;
    instance_->netBearType_ = BEARER_CELLULAR;
    instance_->CreateProbeThread(httpThread, httpsThread, latch, latchAll, true);
    EXPECT_NE(httpThread, nullptr);
    instance_->CreateProbeThread(httpThread, httpsThread, latch, latchAll, false);
    instance_->netBearType_ = BEARER_WIFI;
    instance_->CreateProbeThread(httpThread, httpsThread, latch, latchAll, true);
    EXPECT_NE(httpThread, nullptr);
    instance_->CreateProbeThread(httpThread, httpsThread, latch, latchAll, false);
    EXPECT_NE(httpThread, nullptr);
}

HWTEST_F(NetMonitorTest, StartProbeTest001, TestSize.Level1)
{
    instance_->netBearType_ = BEARER_CELLULAR;
    instance_->Detection();
    bool ret = instance_->IsDetecting();
    EXPECT_FALSE(ret);
    instance_->netBearType_ = BEARER_WIFI;
    instance_->Detection();
    ret = instance_->IsDetecting();
    EXPECT_FALSE(ret);
}

HWTEST_F(NetMonitorTest, UpdateNetProbeTimeTest001, TestSize.Level1)
{
    auto netMonitor = CreateNetMonitorInstance();
    int32_t dualStackProbeTimeOut = 5 * 1000;
    netMonitor->UpdateDualStackProbeTime(dualStackProbeTimeOut);
    EXPECT_EQ(netMonitor->dualStackProbeTimeOut_, dualStackProbeTimeOut);
    
    dualStackProbeTimeOut = 0;
    netMonitor->UpdateDualStackProbeTime(dualStackProbeTimeOut);
    EXPECT_NE(netMonitor->dualStackProbeTimeOut_, dualStackProbeTimeOut);
}

HWTEST_F(NetMonitorTest, StopProbeTest001, TestSize.Level1)
{
    auto netMonitor = CreateNetMonitorInstance();
    netMonitor->dualStackProbe_ = nullptr;
    netMonitor->StopDualStackProbe();
    EXPECT_EQ(netMonitor->dualStackProbe_, nullptr);
    std::shared_ptr<TestMonitorCallback> sp;
    std::weak_ptr<TestMonitorCallback> netMonitorCallback = sp;
    std::string httpUrl = "";
    std::string httpsUrl = "";
    netMonitor->dualStackProbe_ = std::make_shared<NetDualStackProbe>(TEST_NETID, BEARER_DEFAULT,
        NetLinkInfo(), httpUrl, httpsUrl, netMonitorCallback);
    netMonitor->StopDualStackProbe();
    EXPECT_EQ(netMonitor->dualStackProbe_, nullptr);
}

HWTEST_F(NetMonitorTest, StartDualStackProbeThreadTest001, TestSize.Level1)
{
    auto netMonitor = CreateNetMonitorInstance();
    netMonitor->isDetecting_ = true;
    auto result = netMonitor->StartDualStackProbeThread();
    EXPECT_EQ(result, NETMANAGER_ERR_INTERNAL);

    netMonitor->isDetecting_ = false;
    netMonitor->dualStackProbe_ = nullptr;
    result = netMonitor->StartDualStackProbeThread();
    EXPECT_NE(result, NETMANAGER_ERR_INTERNAL);

    EXPECT_NE(netMonitor->dualStackProbe_, nullptr);
    netMonitor->dualStackProbe_->isDualStackProbing_ = true;
    result = netMonitor->StartDualStackProbeThread();
    EXPECT_EQ(result, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetMonitorTest, ExtractDomainFormUrlTest001, TestSize.Level1)
{
    std::string url = "";
    std::string ret = CommonUtils::ExtractDomainFormUrl(url);
    EXPECT_EQ(ret, url);

    url = "test.string";
    ret = CommonUtils::ExtractDomainFormUrl(url);
    EXPECT_EQ(ret, url);

    url = "test//string";
    ret = CommonUtils::ExtractDomainFormUrl(url);
    EXPECT_NE(ret, url);

    url = "/test//string";
    ret = CommonUtils::ExtractDomainFormUrl(url);
    EXPECT_NE(ret, url);
}
} // namespace NetManagerStandard
} // namespace OHOS
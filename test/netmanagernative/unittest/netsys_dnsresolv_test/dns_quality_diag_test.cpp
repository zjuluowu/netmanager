/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <fstream>
#include <sstream>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include <netdb.h>
#include "dns_quality_diag.h"
#include "net_handle.h"
#include "net_conn_client.h"
#include "common_notify_callback_test.h"
#include "refbase.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
const uint32_t MAX_RESULT_SIZE = 32;
}  // namespace

class DnsQualityDiagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    NetsysNative::NetDnsResultReport report;
    NetsysNative::NetDnsQueryResultReport queryReport;
    DnsQualityDiag dnsQualityDiag;
    struct AddrInfo addrinfoIpv4;
    struct AddrInfo addrinfoIpv6;
};

void DnsQualityDiagTest::SetUpTestCase() {}

void DnsQualityDiagTest::TearDownTestCase() {}

void DnsQualityDiagTest::SetUp() {}

void DnsQualityDiagTest::TearDown() {}

HWTEST_F(DnsQualityDiagTest, DnsQualityDiag_ShouldReturnZero_WhenCalled, TestSize.Level0)
{
    EXPECT_EQ(dnsQualityDiag.InitHandler(), 0);
}

HWTEST_F(DnsQualityDiagTest, GetInstanceShouldReturnSingletonInstance, TestSize.Level0)
{
    DnsQualityDiag &instance1 = DnsQualityDiag::GetInstance();
    DnsQualityDiag &instance2 = DnsQualityDiag::GetInstance();
    EXPECT_EQ(instance1.report_delay, instance2.report_delay);
}

HWTEST_F(DnsQualityDiagTest, ParseReportAddr_ShouldAddIPv4AndIPv6_WhenCalledWithValidAddrInfo, TestSize.Level0)
{
    uint32_t size = 2;
    addrinfoIpv4.aiFamily = AF_INET;
    addrinfoIpv6.aiFamily = AF_INET6;
    struct AddrInfo addrinfo[2] = { addrinfoIpv4, addrinfoIpv6 };

    int32_t returnCode = dnsQualityDiag.ParseReportAddr(size, addrinfo, report);

    EXPECT_EQ(report.addrlist_.size(), 2);
    EXPECT_EQ(returnCode, 0);
}

HWTEST_F(DnsQualityDiagTest, ParseDnsSever001, TestSize.Level0)
{
    NetsysNative::NetDnsResultReport reportTest;
    uint32_t size = 2;
    struct DnsServerInfo serverInfoV4;
    struct DnsServerInfo serverInfoV6;
    serverInfoV4.aiFamily = AF_INET;
    serverInfoV4.queryProtocol = 1;
    serverInfoV6.aiFamily = AF_INET6;
    serverInfoV6.queryProtocol = 2;
    struct DnsServerInfo serverInfo[2] = { serverInfoV4, serverInfoV6 };
    dnsQualityDiag.ParseDnsSever(size, serverInfo, reportTest);
    EXPECT_EQ(reportTest.serverlist_.size(), size);
}

HWTEST_F(DnsQualityDiagTest, ParseReportAddr_ShouldNotAddMoreThanMaxSize_WhenCalledWithMoreAddrInfo, TestSize.Level0)
{
    uint32_t size = MAX_RESULT_SIZE + 1;
    addrinfoIpv4.aiFamily = AF_INET;
    struct AddrInfo addrinfo[MAX_RESULT_SIZE + 1] = { addrinfoIpv4 };

    int32_t returnCode = dnsQualityDiag.ParseReportAddr(size, addrinfo, report);

    EXPECT_EQ(report.addrlist_.size(), MAX_RESULT_SIZE);
    EXPECT_EQ(returnCode, 0);
}

HWTEST_F(DnsQualityDiagTest, ReportDnsResult_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    uint16_t netId = 1;
    uint16_t uid = 1;
    uint32_t pid = 1;
    int32_t usedtime = 1;
    char name[] = "test";
    uint32_t size = 1;
    int32_t failreason = 0;
    QueryParam queryParam;
    AddrInfo addrinfo;
    DnsServerInfo serverinfo;
    EXPECT_EQ(dnsQualityDiag.ReportDnsResult(netId, uid, pid, usedtime, name, size, failreason, queryParam, &addrinfo,
        0, &serverinfo), 0);
}

HWTEST_F(DnsQualityDiagTest, ReportDnsResult_ShouldReturnZero_WhenCalled_02, TestSize.Level0)
{
    uint16_t netId = 1;
    uint16_t uid = 1;
    uint32_t pid = 1;
    int32_t usedtime = 1;
    char name[] = "test";
    uint32_t size = 1;
    int32_t failreason = 1;
    QueryParam queryParam;
    AddrInfo addrinfo;
    DnsServerInfo serverinfo;
    EXPECT_EQ(dnsQualityDiag.ReportDnsResult(netId, uid, pid, usedtime, name, size, failreason, queryParam, &addrinfo,
        0, &serverinfo), 0);
}

HWTEST_F(DnsQualityDiagTest, ReportDnsResult_ShouldIgnore_WhenQueryTypeIsOne, TestSize.Level0)
{
    uint16_t netId = 1;
    uint16_t uid = 1;
    uint32_t pid = 1;
    int32_t usedtime = 100;
    char name[] = "test";
    uint32_t size = 10;
    int32_t failreason = 0;
    QueryParam queryParam;
    queryParam.type = 1;
    AddrInfo addrinfo;
    DnsServerInfo serverinfo;
    int32_t result =
        dnsQualityDiag.ReportDnsResult(netId, uid, pid, usedtime, name, size, failreason, queryParam, &addrinfo,
        0, &serverinfo);
    EXPECT_EQ(result, 0);
}

HWTEST_F(DnsQualityDiagTest, ReportDnsQueryResult_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    struct PostDnsQueryParam queryParam;
    struct FamilyQueryInfoExt ipv4Info;
    ipv4Info.retCode = 0;
    ipv4Info.isNoAnswer = 0;
    ipv4Info.cname = 0;
    struct FamilyQueryInfoExt ipv6Info;
    ipv6Info.retCode = 0;
    ipv6Info.isNoAnswer = 0;
    ipv6Info.cname = 0;
    queryParam.netId = 100;
    queryParam.uid = 10000;
    queryParam.pid = 5896;
    queryParam.addrSize = 0;
    struct DnsProcessInfoExt processInfo;
    processInfo.queryTime = 0;
    processInfo.retCode = 0;
    processInfo.firstQueryEndDuration = 10;
    processInfo.firstQueryEnd2AppDuration = 10;
    processInfo.firstReturnType = 1;
    processInfo.isFromCache = 0;
    processInfo.sourceFrom = 2;
    processInfo.ipv4QueryInfo = ipv4Info;
    processInfo.ipv6QueryInfo = ipv6Info;
    AddrInfo addrinfo;
    queryParam.processInfo = processInfo;
    int32_t result =
        dnsQualityDiag.ReportDnsQueryResult(queryParam, &addrinfo, 0);
    EXPECT_EQ(result, 0);
}

HWTEST_F(DnsQualityDiagTest, ReportDnsQueryAbnormal_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    struct PostDnsQueryParam queryParam;
    struct FamilyQueryInfoExt ipv4Info;
    ipv4Info.retCode = 0;
    ipv4Info.isNoAnswer = 0;
    ipv4Info.cname = 0;
    struct FamilyQueryInfoExt ipv6Info;
    ipv6Info.retCode = 0;
    ipv6Info.isNoAnswer = 0;
    ipv6Info.cname = 0;
    queryParam.netId = 100;
    queryParam.uid = 10000;
    queryParam.pid = 5896;
    queryParam.addrSize = 0;
    struct DnsProcessInfoExt processInfo;
    processInfo.queryTime = 0;
    processInfo.retCode = 0;
    processInfo.firstQueryEndDuration = 10;
    processInfo.firstQueryEnd2AppDuration = 10;
    processInfo.firstReturnType = 1;
    processInfo.isFromCache = 0;
    processInfo.sourceFrom = 2;
    processInfo.ipv4QueryInfo = ipv4Info;
    processInfo.ipv6QueryInfo = ipv6Info;
    AddrInfo addrinfo;
    queryParam.processInfo = processInfo;
    int32_t result =
        dnsQualityDiag.ReportDnsQueryAbnormal(1, queryParam, &addrinfo);
    EXPECT_EQ(result, 0);
}

HWTEST_F(DnsQualityDiagTest, Handle_dns_abnormal_AddIPv4AndIPv6_WhenCalledWithValidAddrInfo, TestSize.Level0)
{
    std::shared_ptr<DnsAbnormalInfo> abnormalInfo =
        std::make_shared<DnsAbnormalInfo>();
    NetsysNative::NetDnsQueryResultReport queryReport;
    abnormalInfo->eventfailcause = 1;
    abnormalInfo->report = queryReport;
    EXPECT_EQ(dnsQualityDiag.handle_dns_abnormal(abnormalInfo), 0);
}

HWTEST_F(DnsQualityDiagTest, Handle_dns_abnormal_AddIPv4AndIPv6_WhenCalledWithNullptr, TestSize.Level0)
{
    std::shared_ptr<DnsAbnormalInfo> abnormalInfo = nullptr;
    EXPECT_EQ(dnsQualityDiag.handle_dns_abnormal(abnormalInfo), 0);
}

HWTEST_F(DnsQualityDiagTest, ParseDnsQueryReportAddr_AddIPv4AndIPv6_WhenCalledWithValidAddrInfo, TestSize.Level0)
{
    uint32_t size = 2;
    addrinfoIpv4.aiFamily = AF_INET;
    addrinfoIpv6.aiFamily = AF_INET6;
    struct AddrInfo addrinfo[2] = { addrinfoIpv4, addrinfoIpv6 };

    int32_t returnCode = dnsQualityDiag.ParseDnsQueryReportAddr(size, addrinfo, queryReport);

    EXPECT_EQ(queryReport.addrlist_.size(), 2);
    EXPECT_EQ(returnCode, 0);
}

HWTEST_F(DnsQualityDiagTest, ParseDnsQueryReportAddr_NotAddMoreThanMaxSize_WhenCalledWithMoreAddrInfo, TestSize.Level0)
{
    uint32_t size = MAX_RESULT_SIZE + 1;
    addrinfoIpv4.aiFamily = AF_INET;
    struct AddrInfo addrinfo[MAX_RESULT_SIZE + 1] = { addrinfoIpv4 };

    int32_t returnCode = dnsQualityDiag.ParseDnsQueryReportAddr(size, addrinfo, queryReport);

    EXPECT_EQ(queryReport.addrlist_.size(), MAX_RESULT_SIZE);
    EXPECT_EQ(returnCode, 0);
}

HWTEST_F(DnsQualityDiagTest, RegisterResultListener_ShouldReturnZero_WhenCalled, TestSize.Level0)
{
    sptr<NetsysNative::INetDnsResultCallback> callback = nullptr;
    uint32_t timeStep = 1;
    EXPECT_EQ(dnsQualityDiag.RegisterResultListener(callback, timeStep), 0);
}

HWTEST_F(DnsQualityDiagTest, UnregisterResultListener_ShouldReturnZero_WhenCalled, TestSize.Level0)
{
    sptr<NetsysNative::INetDnsResultCallback> callback = nullptr;
    EXPECT_EQ(dnsQualityDiag.UnregisterResultListener(callback), 0);
}

HWTEST_F(DnsQualityDiagTest, UnregisterResultListener_Unreg_After_Reg, TestSize.Level0)
{
    sptr<NetsysNative::INetDnsResultCallback> callback = new NetsysNative::DnsResultCallbackTest();
    uint32_t timeStep = 1;
    EXPECT_EQ(dnsQualityDiag.RegisterResultListener(callback, timeStep), 0);
    
    EXPECT_EQ(dnsQualityDiag.UnregisterResultListener(callback), 0);
}

HWTEST_F(DnsQualityDiagTest, SetLoopDelay_ShouldReturnZero_WhenCalled, TestSize.Level0)
{
    int32_t delay = 1;
    EXPECT_EQ(dnsQualityDiag.SetLoopDelay(delay), 0);
}

HWTEST_F(DnsQualityDiagTest, query_default_host_ShouldReturnZero_WhenCalled, TestSize.Level0)
{
    EXPECT_EQ(dnsQualityDiag.query_default_host(), 0);
}

HWTEST_F(DnsQualityDiagTest, handle_dns_loop_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    dnsQualityDiag.handler_started = false;
    EXPECT_EQ(dnsQualityDiag.handle_dns_loop(), 0);
}

HWTEST_F(DnsQualityDiagTest, handle_dns_loop_ShouldReturnZero_WhenCalled_02, TestSize.Level0)
{
    dnsQualityDiag.handler_started = true;
    EXPECT_EQ(dnsQualityDiag.handle_dns_loop(), 0);
}

HWTEST_F(DnsQualityDiagTest, handle_dns_fail_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    dnsQualityDiag.handler_started = false;
    EXPECT_EQ(dnsQualityDiag.handle_dns_fail(), 0);
}

HWTEST_F(DnsQualityDiagTest, handle_dns_fail_ShouldReturnZero_WhenCalled_02, TestSize.Level0)
{
    dnsQualityDiag.handler_started = true;
    EXPECT_EQ(dnsQualityDiag.handle_dns_fail(), 0);
}

HWTEST_F(DnsQualityDiagTest, send_dns_report_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    dnsQualityDiag.handler_started = false;
    EXPECT_EQ(dnsQualityDiag.send_dns_report(), 0);
}

HWTEST_F(DnsQualityDiagTest, send_dns_report_ShouldReturnZero_WhenCalled_02, TestSize.Level0)
{
    dnsQualityDiag.handler_started = true;
    EXPECT_EQ(dnsQualityDiag.send_dns_report(), 0);
}

HWTEST_F(DnsQualityDiagTest, send_dns_report_ShouldReturnZero_WhenCalled_03, TestSize.Level0)
{
    std::shared_ptr<NetsysNative::NetDnsResultReport> report;
    report = std::make_shared<NetsysNative::NetDnsResultReport>();
    EXPECT_EQ(dnsQualityDiag.add_dns_report(report), 0);
    EXPECT_TRUE(dnsQualityDiag.report_.size() > 0);

    dnsQualityDiag.handler_started = true;
    EXPECT_EQ(dnsQualityDiag.send_dns_report(), 0);
    EXPECT_EQ(dnsQualityDiag.report_.size(), 0);
}

HWTEST_F(DnsQualityDiagTest, send_dns_report_ShouldReturnZero_WhenCalled_04, TestSize.Level0)
{
    DnsQualityDiag dnsQualityDiag;
    dnsQualityDiag.handler_started = false;
    dnsQualityDiag.send_dns_report();
    EXPECT_EQ(dnsQualityDiag.dnsQueryReport_.size(), 0);
}

HWTEST_F(DnsQualityDiagTest, send_dns_report_ShouldReturnZero_WhenCalled_05, TestSize.Level0)
{
    DnsQualityDiag dnsQualityDiag;
    dnsQualityDiag.handler_started = true;
    NetsysNative::NetDnsQueryResultReport netDnsQueryResultReport;
    dnsQualityDiag.dnsQueryReport_.emplace_back(netDnsQueryResultReport);
    dnsQualityDiag.send_dns_report();
    EXPECT_EQ(dnsQualityDiag.dnsQueryReport_.size(), 0);
    EXPECT_EQ(dnsQualityDiag.resultListeners_.size(), 0);
}

HWTEST_F(DnsQualityDiagTest, send_dns_report_ShouldReturnZero_WhenCalled_06, TestSize.Level0)
{
    DnsQualityDiag dnsQualityDiag;
    dnsQualityDiag.handler_started = true;
    NetsysNative::NetDnsQueryResultReport netDnsQueryResultReport;
    dnsQualityDiag.dnsQueryReport_.emplace_back(netDnsQueryResultReport);
    auto callback = sptr<NetsysNative::DnsResultCallbackTest>::MakeSptr();
    dnsQualityDiag.resultListeners_.push_back(callback);
    dnsQualityDiag.send_dns_report();
    EXPECT_EQ(dnsQualityDiag.dnsQueryReport_.size(), 0);
    EXPECT_NE(dnsQualityDiag.resultListeners_.size(), 0);
}

HWTEST_F(DnsQualityDiagTest, add_dns_report_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    std::shared_ptr<NetsysNative::NetDnsResultReport> report;
    report = std::make_shared<NetsysNative::NetDnsResultReport>();
    EXPECT_EQ(dnsQualityDiag.add_dns_report(report), 0);
    EXPECT_EQ(dnsQualityDiag.report_.size(), 1);
}

HWTEST_F(DnsQualityDiagTest, add_dns_report_ShouldReturnZero_WhenCalled_02, TestSize.Level0)
{
    std::shared_ptr<NetsysNative::NetDnsResultReport> report = nullptr;
    EXPECT_EQ(dnsQualityDiag.add_dns_report(report), 0);
}

HWTEST_F(DnsQualityDiagTest, add_dns_report_ShouldNotAddReport_WhenReportListIsFull, TestSize.Level0)
{
    for (int i = 0; i < MAX_RESULT_SIZE; i++) {
        std::shared_ptr<NetsysNative::NetDnsResultReport> report = std::make_shared<NetsysNative::NetDnsResultReport>();
        dnsQualityDiag.add_dns_report(report);
    }
    std::shared_ptr<NetsysNative::NetDnsResultReport> report = std::make_shared<NetsysNative::NetDnsResultReport>();
    int32_t result = dnsQualityDiag.add_dns_report(report);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(dnsQualityDiag.report_.size(), MAX_RESULT_SIZE);

    uint16_t netId = 1;
    uint16_t uid = 1;
    uint32_t pid = 1;
    int32_t usedtime = 1;
    char name[] = "test";
    uint32_t size = 1;
    int32_t failreason = 1;
    QueryParam queryParam;
    AddrInfo addrinfo;
    DnsServerInfo serverinfo;
    EXPECT_EQ(dnsQualityDiag.ReportDnsResult(netId, uid, pid, usedtime, name, size, failreason, queryParam,
        &addrinfo, 0, &serverinfo), 0);
}

HWTEST_F(DnsQualityDiagTest, ReportDnsResult_ShouldIgnore_WhenFailCauseIsFirewallInterception, TestSize.Level0)
{
    uint16_t netId = 1;
    uint16_t uid = 1;
    uint32_t pid = 1;
    int32_t usedtime = 100;
    char name[] = "test";
    uint32_t size = 10;
    int32_t failreason = -1202;
    QueryParam queryParam;
    queryParam.type = 1;
    AddrInfo addrinfo;
    DnsServerInfo serverinfo;
    int32_t result =
        dnsQualityDiag.ReportDnsResult(netId, uid, pid, usedtime, name, size, failreason, queryParam, &addrinfo,
        0, &serverinfo);
    EXPECT_EQ(result, 0);
}

HWTEST_F(DnsQualityDiagTest, add_dns_query_report_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    std::shared_ptr<NetsysNative::NetDnsQueryResultReport> queryReport;
    queryReport = std::make_shared<NetsysNative::NetDnsQueryResultReport>();
    EXPECT_EQ(dnsQualityDiag.add_dns_query_report(queryReport), 0);
    EXPECT_EQ(dnsQualityDiag.dnsQueryReport_.size(), 1);
}

HWTEST_F(DnsQualityDiagTest, add_dns_query_report_ShouldReturnZero_WhenCalled_02, TestSize.Level0)
{
    std::shared_ptr<NetsysNative::NetDnsQueryResultReport> queryReport = nullptr;
    EXPECT_EQ(dnsQualityDiag.add_dns_query_report(queryReport), 0);
}

HWTEST_F(DnsQualityDiagTest, add_dns_query_report_ShouldNotAddReport_WhenReportListIsFull, TestSize.Level0)
{
    for (int i = 0; i < MAX_RESULT_SIZE; i++) {
        std::shared_ptr<NetsysNative::NetDnsQueryResultReport> report =
            std::make_shared<NetsysNative::NetDnsQueryResultReport>();
        dnsQualityDiag.add_dns_query_report(report);
    }
    std::shared_ptr<NetsysNative::NetDnsQueryResultReport> report =
        std::make_shared<NetsysNative::NetDnsQueryResultReport>();
    int32_t result = dnsQualityDiag.add_dns_query_report(report);
    ASSERT_EQ(result, 0);
    ASSERT_EQ(dnsQualityDiag.dnsQueryReport_.size(), MAX_RESULT_SIZE);
}

HWTEST_F(DnsQualityDiagTest, load_query_addr_ShouldReturnZero_WhenCalled, TestSize.Level0)
{
    const char *defaultAddr = "test";
    EXPECT_EQ(dnsQualityDiag.load_query_addr(defaultAddr), 0);
}

HWTEST_F(DnsQualityDiagTest, HandleEvent_ShouldReturnZero_WhenCalled_01, TestSize.Level0)
{
    dnsQualityDiag.handler_started = false;
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get();
    EXPECT_EQ(dnsQualityDiag.HandleEvent(event), 0);
}

HWTEST_F(DnsQualityDiagTest, HandleEvent_ShouldReturnZero_WhenCalled_02, TestSize.Level0)
{
    dnsQualityDiag.handler_started = true;
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(DnsQualityEventHandler::MSG_DNS_MONITOR_LOOP);
    EXPECT_EQ(dnsQualityDiag.HandleEvent(event), 0);
}

HWTEST_F(DnsQualityDiagTest, HandleEvent_ShouldReturnZero_WhenCalled_03, TestSize.Level0)
{
    dnsQualityDiag.handler_started = true;
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(DnsQualityEventHandler::MSG_DNS_QUERY_FAIL);
    EXPECT_EQ(dnsQualityDiag.HandleEvent(event), 0);
}

HWTEST_F(DnsQualityDiagTest, HandleEvent_ShouldReturnZero_WhenCalled_04, TestSize.Level0)
{
    dnsQualityDiag.handler_started = true;
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(DnsQualityEventHandler::MSG_DNS_REPORT_LOOP);
    EXPECT_EQ(dnsQualityDiag.HandleEvent(event), 0);
}

HWTEST_F(DnsQualityDiagTest, HandleEvent_ShouldReturnZero_WhenCalled_05, TestSize.Level0)
{
    dnsQualityDiag.handler_started = true;
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get(DnsQualityEventHandler::MSG_DNS_NEW_REPORT);
    EXPECT_EQ(dnsQualityDiag.HandleEvent(event), 0);
}

}  // namespace nmd
}  // namespace OHOS

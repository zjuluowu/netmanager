/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <fstream>
#include <sstream>

#include "dns_param_cache.h"
#include "dns_quality_diag.h"
#include "net_conn_client.h"
#include "net_handle.h"

namespace OHOS::nmd {
namespace {
using namespace OHOS::NetsysNative;
}

const char *DNS_DIAG_WORK_THREAD = "DNS_DIAG_WORK_THREAD";
const char *HW_HICLOUD_ADDR = "";
const uint32_t MAX_RESULT_SIZE = 32;
const char *URL_CFG_FILE = "/system/etc/netdetectionurl.conf";
const char *DNS_URL_HEADER = "DnsProbeUrl:";
const char NEW_LINE_STR = '\n';
const uint32_t TIME_DELAY = 500;
constexpr const uint32_t DNS_ABNORMAL_REPORT_INTERVAL = 2;
constexpr int32_t DNS_FAIL_REASON_FIREWALL = -1202;

DnsQualityDiag::DnsQualityDiag()
    : defaultNetId_(0),
      monitor_loop_delay(TIME_DELAY),
      report_delay(TIME_DELAY),
      handler_started(false),
      handler_(nullptr)
{
    InitHandler();
    load_query_addr(HW_HICLOUD_ADDR);
}

DnsQualityDiag &DnsQualityDiag::GetInstance()
{
    static DnsQualityDiag instance;
    return instance;
}

int32_t DnsQualityDiag::InitHandler()
{
    if (handler_ == nullptr) {
        std::shared_ptr<AppExecFwk::EventRunner> runner_ =
            AppExecFwk::EventRunner::Create(DNS_DIAG_WORK_THREAD);
        if (!runner_) {
            NETNATIVE_LOGE("Create net policy work event runner.");
        } else {
            handler_ = std::make_shared<DnsQualityEventHandler>(runner_);
        }
    }
    return 0;
}

int32_t DnsQualityDiag::ParseReportAddr(uint32_t size, AddrInfo* addrinfo, NetsysNative::NetDnsResultReport &report)
{
    for (uint8_t i = 0; i < size; i++) {
        NetsysNative::NetDnsResultAddrInfo ai;
        AddrInfo *tmp = &(addrinfo[i]);
        void* addr = NULL;
        char c_addr[INET6_ADDRSTRLEN];
        switch (tmp->aiFamily) {
            case AF_INET:
                ai.type_ = NetsysNative::ADDR_TYPE_IPV4;
                addr = &(tmp->aiAddr.sin.sin_addr);
                break;
            case AF_INET6:
                ai.type_ = NetsysNative::ADDR_TYPE_IPV6;
                addr = &(tmp->aiAddr.sin6.sin6_addr);
                break;
        }
        inet_ntop(tmp->aiFamily, addr, c_addr, sizeof(c_addr));
        ai.addr_ = c_addr;
        if (report.addrlist_.size() < MAX_RESULT_SIZE) {
            report.addrlist_.push_back(ai);
        } else {
            break;
        }
    }
    return 0;
}

void DnsQualityDiag::ParseDnsSever(uint32_t size, DnsServerInfo* serverInfo,
    NetsysNative::NetDnsResultReport &report)
{
    for (uint8_t i = 0; i < size; i++) {
        NetsysNative::NetDnsResultServerInfo ai;
        DnsServerInfo *tmp = &(serverInfo[i]);
        switch (tmp->aiFamily) {
            case AF_INET:
                ai.type_ = static_cast<uint32_t>(NetsysNative::ADDR_TYPE_IPV4);
                break;
            case AF_INET6:
                ai.type_ = static_cast<uint32_t>(NetsysNative::ADDR_TYPE_IPV6);
                break;
        }
        ai.protocol_ = tmp->queryProtocol == SOCK_STREAM ? NetsysNative::SERVER_PROTOCOL_TCP :
            NetsysNative::SERVER_PROTOCOL_UDP;
        ai.addr_ = tmp->addr;
        report.serverlist_.push_back(ai);
    }
}

int32_t DnsQualityDiag::ReportDnsResult(uint16_t netId, uint16_t uid, uint32_t pid, int32_t usedtime,
    char* name, uint32_t size, int32_t failreason, QueryParam queryParam, AddrInfo* addrinfo,
    uint32_t serverSize, DnsServerInfo *dnsserverinfo)
{
    if (failreason == DNS_FAIL_REASON_FIREWALL) {
        return 0;
    }
    std::unique_lock<std::mutex> locker(dnsReportResultMutex_);
    bool reportSizeReachLimit = (report_.size() >= MAX_RESULT_SIZE);
    locker.unlock();
    NETNATIVE_LOG_D("ReportDnsResult: %{public}d, %{public}d, %{public}d, %{public}d, %{public}d, %{public}d",
                    netId, uid, pid, usedtime, size, failreason);

    if (queryParam.type == 1) {
        // query from Netmanager ignore report
        return 0;
    }

    if (!reportSizeReachLimit) {
        NetsysNative::NetDnsResultReport report;
        report.netid_ = netId;
        report.uid_ = uid;
        report.pid_ = pid;
        report.timeused_ = static_cast<uint32_t>(usedtime);
        report.queryresult_ = static_cast<uint32_t>(failreason);
        report.host_ = name;
        report.querytime_ = static_cast<uint64_t>(queryParam.queryTime);
        if (failreason == 0) {
            ParseReportAddr(size, addrinfo, report);
            ParseDnsSever(serverSize, dnsserverinfo, report);
        }
        std::shared_ptr<NetsysNative::NetDnsResultReport> rpt =
            std::make_shared<NetsysNative::NetDnsResultReport>(report);
        auto event = AppExecFwk::InnerEvent::Get(DnsQualityEventHandler::MSG_DNS_NEW_REPORT, rpt);
        handler_->SendEvent(event);
    }
    return 0;
}

void DnsQualityDiag::GetDefaultDnsServerList(int32_t uid, std::vector<std::string> &servers)
{
    int32_t netId = DnsParamCache::GetInstance().GetDefaultNetwork();
    std::vector<std::string> domains;
    uint16_t baseTimeoutMsec;
    uint8_t retryCount;
    DnsParamCache::GetInstance().GetResolverConfig(
        netId, uid, servers, domains, baseTimeoutMsec, retryCount);
}

void DnsQualityDiag::FillDnsQueryResultReport(NetsysNative::NetDnsQueryResultReport &report,
    PostDnsQueryParam &queryParam)
{
    uint16_t resBitInfo = 0;
    report.uid_ = queryParam.uid;
    report.pid_ = queryParam.pid;
    report.addrSize_ = queryParam.addrSize;
    DnsProcessInfoExt processInfo = queryParam.processInfo;
    report.sourceFrom_ = processInfo.sourceFrom;
    std::string srcAddr(processInfo.srcAddr);
    report.srcAddr_ = srcAddr;
    std::vector<std::string> serverList;
    GetDefaultDnsServerList(queryParam.uid, serverList);
    report.dnsServerSize_ = static_cast<uint8_t>(serverList.size());
    report.dnsServerList_ = serverList;
    report.queryTime_ = static_cast<uint64_t>(processInfo.queryTime);
    report.host_ = processInfo.hostname;
    report.retCode_ = processInfo.retCode;
    report.firstQueryEndDuration_ = processInfo.firstQueryEndDuration;
    report.firstQueryEnd2AppDuration_ = processInfo.firstQueryEnd2AppDuration;
    report.ipv4RetCode_ = processInfo.ipv4QueryInfo.retCode;
    std::string ipv4ServerName(processInfo.ipv4QueryInfo.serverAddr);
    report.ipv4ServerName_ = ipv4ServerName;
    report.ipv6RetCode_ = processInfo.ipv6QueryInfo.retCode;
    std::string ipv6ServerName(processInfo.ipv6QueryInfo.serverAddr);
    report.ipv6ServerName_ = ipv6ServerName;
    resBitInfo |= (DnsParamCache::GetInstance().IsUseVpnDns(queryParam.uid) ? VPN_NET_FLAG : 0);
    resBitInfo |= (processInfo.isFromCache ? FROM_CACHE_FLAG : 0);
    resBitInfo |= (processInfo.ipv4QueryInfo.isNoAnswer ? IPV4_NO_ANSWER_FLAG : 0);
    resBitInfo |= (processInfo.ipv4QueryInfo.cname ? IPV4_CNAME_FLAG : 0);
    resBitInfo |= (processInfo.ipv6QueryInfo.isNoAnswer ? IPV6_NO_ANSWER_FLAG : 0);
    resBitInfo |= (processInfo.ipv6QueryInfo.cname ? IPV6_CNAME_FLAG : 0);
    report.resBitInfo_ = resBitInfo;
}

int32_t DnsQualityDiag::ParseDnsQueryReportAddr(uint8_t size,
    AddrInfo* addrinfo, NetsysNative::NetDnsQueryResultReport &report)
{
    for (uint8_t i = 0; i < size; i++) {
        NetsysNative::NetDnsQueryResultAddrInfo ai;
        AddrInfo *tmp = &(addrinfo[i]);
        void* addr = NULL;
        char c_addr[INET6_ADDRSTRLEN];
        switch (tmp->aiFamily) {
            case AF_INET:
                ai.type_ = NetsysNative::ADDR_TYPE_IPV4;
                addr = &(tmp->aiAddr.sin.sin_addr);
                break;
            case AF_INET6:
                ai.type_ = NetsysNative::ADDR_TYPE_IPV6;
                addr = &(tmp->aiAddr.sin6.sin6_addr);
                break;
        }
        inet_ntop(tmp->aiFamily, addr, c_addr, sizeof(c_addr));
        ai.addr_ = c_addr;
        if (report.addrlist_.size() < MAX_RESULT_SIZE) {
            report.addrlist_.push_back(ai);
        } else {
            break;
        }
    }
    return 0;
}

int32_t DnsQualityDiag::ReportDnsQueryResult(PostDnsQueryParam queryParam, AddrInfo* addrinfo, uint8_t addrSize)
{
    std::shared_lock<std::shared_mutex> lock(dnsQueryReportMutex_);
    bool reportSizeReachLimit = (dnsQueryReport_.size() >= MAX_RESULT_SIZE);
    lock.unlock();
    if (!reportSizeReachLimit) {
        NetsysNative::NetDnsQueryResultReport report;
        FillDnsQueryResultReport(report, queryParam);
        if (addrSize > 0 && addrinfo != nullptr) {
            ParseDnsQueryReportAddr(addrSize, addrinfo, report);
        }
        std::shared_ptr<NetsysNative::NetDnsQueryResultReport> rpt =
            std::make_shared<NetsysNative::NetDnsQueryResultReport>(report);
        auto event = AppExecFwk::InnerEvent::Get(DnsQualityEventHandler::MSG_DNS_QUERY_RESULT_REPORT, rpt);
        handler_->SendEvent(event);
    }

    return 0;
}

int32_t DnsQualityDiag::ReportDnsQueryAbnormal(uint32_t eventfailcause, PostDnsQueryParam queryParam,
    AddrInfo* addrinfo)
{
    std::unique_lock<std::mutex> locker(dnsAbnormalTimeMutex_);
    uint32_t timeNow = static_cast<uint32_t>(time(NULL));
    if (timeNow - last_dns_abnormal_report_time < DNS_ABNORMAL_REPORT_INTERVAL) {
        locker.unlock();
        return 0;
    }
    last_dns_abnormal_report_time = static_cast<uint32_t>(time(NULL));
    locker.unlock();
    NetsysNative::NetDnsQueryResultReport report;
    FillDnsQueryResultReport(report, queryParam);
    if (queryParam.addrSize > 0 && addrinfo != nullptr) {
        ParseDnsQueryReportAddr(queryParam.addrSize, addrinfo, report);
    }
    std::shared_ptr<DnsAbnormalInfo> rpt = std::make_shared<DnsAbnormalInfo>();
    rpt->eventfailcause = eventfailcause;
    rpt->report = report;
    auto event = AppExecFwk::InnerEvent::Get(DnsQualityEventHandler::MSG_DNS_QUERY_ABNORMAL_REPORT, rpt);
    handler_->SendEvent(event);
    return 0;
}

int32_t DnsQualityDiag::RegisterResultListener(const sptr<INetDnsResultCallback> &callback, uint32_t timeStep)
{
    if (callback == nullptr) {
        NETNATIVE_LOGE("callback is nullptr");
        return 0;
    }

    std::unique_lock<std::mutex> locker(resultListenersMutex_);
    report_delay = std::max(report_delay, timeStep);
    std::list<sptr<NetsysNative::INetDnsResultCallback>>::iterator iter;
    for (iter = resultListeners_.begin(); iter != resultListeners_.end(); ++iter) {
        if ((*iter)->AsObject().GetRefPtr() == callback->AsObject().GetRefPtr()) {
            NETNATIVE_LOGI("callback is already registered");
            break;
        }
    }
    if (iter == resultListeners_.end()) {
        resultListeners_.push_back(callback);
    }
    locker.unlock();

    if (handler_started != true) {
        handler_started = true;
        handler_->SendEvent(DnsQualityEventHandler::MSG_DNS_REPORT_LOOP, report_delay);
#if NETSYS_DNS_MONITOR
        handler_->SendEvent(DnsQualityEventHandler::MSG_DNS_MONITOR_LOOP, monitor_loop_delay);
#endif
    }
    NETNATIVE_LOG_D("RegisterResultListener, %{public}d", report_delay);

    return 0;
}

int32_t DnsQualityDiag::UnregisterResultListener(const sptr<INetDnsResultCallback> &callback)
{
    if (callback == nullptr) {
        NETNATIVE_LOGE("callback is nullptr");
        return 0;
    }
    std::lock_guard<std::mutex> locker(resultListenersMutex_);
    auto iter = resultListeners_.begin();
    while (iter != resultListeners_.end()) {
        if ((*iter)->AsObject().GetRefPtr() == callback->AsObject().GetRefPtr()) {
            iter = resultListeners_.erase(iter);
        } else {
            ++iter;
        }
    }

    if (resultListeners_.empty()) {
        handler_started = false;
    }
    NETNATIVE_LOG_D("UnregisterResultListener");
    
    return 0;
}

int32_t DnsQualityDiag::SetLoopDelay(int32_t delay)
{
    monitor_loop_delay = static_cast<uint32_t>(delay);
    return 0;
}

int32_t DnsQualityDiag::query_default_host()
{
#if NETSYS_DNS_MONITOR
    struct addrinfo *res;
    struct queryparam param;
    param.qp_type = 1;
#endif

    OHOS::NetManagerStandard::NetHandle netHandle;
    OHOS::NetManagerStandard::NetConnClient::GetInstance().GetDefaultNet(netHandle);
    int netid = netHandle.GetNetId();

    NETNATIVE_LOG_D("query_default_host: %{public}d, ", netid);

#if NETSYS_DNS_MONITOR
    param.qp_netid = netid;
    getaddrinfo_ext(queryAddr.c_str(), NULL, NULL, &res, &param);
    freeaddrinfo(res);
#endif

    return 0;
}

int32_t DnsQualityDiag::handle_dns_loop()
{
    if (handler_started) {
        std::unique_lock<std::mutex> locker(dnsReportResultMutex_);
        if (report_.size() == 0) {
            query_default_host();
        }
        locker.unlock();
        handler_->SendEvent(DnsQualityEventHandler::MSG_DNS_MONITOR_LOOP, monitor_loop_delay);
    }
    return 0;
}

int32_t DnsQualityDiag::handle_dns_fail()
{
    if (handler_started) {
        query_default_host();
    }
    return 0;
}

int32_t DnsQualityDiag::send_dns_report()
{
    std::unique_lock<std::mutex> locker(dnsReportResultMutex_);
    if (!handler_started) {
        report_.clear();
        return 0;
    }

    if (report_.size() > 0) {
        std::list<NetsysNative::NetDnsResultReport> reportSend(report_);
        report_.clear();
        locker.unlock();
        for (auto cb: resultListeners_) {
            cb->OnDnsResultReport(reportSend.size(), reportSend);
        }
    }

    std::unique_lock<std::shared_mutex> lock(dnsQueryReportMutex_);
    std::list<NetsysNative::NetDnsQueryResultReport> reportSend(dnsQueryReport_);
    dnsQueryReport_.clear();

    if (reportSend.size() > 0) {
        for (auto cb: resultListeners_) {
            cb->OnDnsQueryResultReport(reportSend.size(), reportSend);
        }
    }
    handler_->SendEvent(DnsQualityEventHandler::MSG_DNS_REPORT_LOOP, report_delay);
    return 0;
}

int32_t DnsQualityDiag::add_dns_query_report(std::shared_ptr<NetsysNative::NetDnsQueryResultReport> report)
{
    if (!report) {
        return 0;
    }
    std::unique_lock<std::shared_mutex> lock(dnsQueryReportMutex_);
    if (dnsQueryReport_.size() < MAX_RESULT_SIZE) {
        dnsQueryReport_.push_back(*report);
    }
    return 0;
}

int32_t DnsQualityDiag::handle_dns_abnormal(std::shared_ptr<DnsAbnormalInfo> abnormalInfo)
{
    if (!abnormalInfo) {
        return 0;
    }
    std::unique_lock<std::mutex> locker(resultListenersMutex_);
    for (auto cb: resultListeners_) {
        cb->OnDnsQueryAbnormalReport(abnormalInfo->eventfailcause, abnormalInfo->report);
    }
    return 0;
}

int32_t DnsQualityDiag::add_dns_report(std::shared_ptr<NetsysNative::NetDnsResultReport> report)
{
    if (!report) {
        NETNATIVE_LOGE("report is nullptr");
        return 0;
    }
    std::unique_lock<std::mutex> locker(dnsReportResultMutex_);
    if (report_.size() < MAX_RESULT_SIZE) {
        report_.push_back(*report);
    }
    return 0;
}

int32_t DnsQualityDiag::load_query_addr(const char* defaultAddr)
{
    if (!std::filesystem::exists(URL_CFG_FILE)) {
        NETNATIVE_LOGE("File not exist (%{public}s)", URL_CFG_FILE);
        queryAddr = defaultAddr;
        return 0;
    }

    std::ifstream file(URL_CFG_FILE);
    if (!file.is_open()) {
        NETNATIVE_LOGE("Open file failed (%{public}s)", strerror(errno));
        queryAddr = defaultAddr;
        return 0;
    }

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    auto pos = content.find(DNS_URL_HEADER);
    if (pos != std::string::npos) {
        pos += strlen(DNS_URL_HEADER);
        queryAddr = content.substr(pos, content.find(NEW_LINE_STR, pos) - pos);
    } else {
        queryAddr = defaultAddr;
    }
    NETNATIVE_LOG_D("Get queryAddr url:[%{public}s]]", queryAddr.c_str());

    return 0;
}

int32_t DnsQualityDiag::HandleEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (!handler_started) {
        NETNATIVE_LOGI("DnsQualityDiag Handler not started");
        return 0;
    }
    NETNATIVE_LOG_D("DnsQualityDiag Handler event: %{public}d", event->GetInnerEventId());

    switch (event->GetInnerEventId()) {
        case DnsQualityEventHandler::MSG_DNS_MONITOR_LOOP:
            handle_dns_loop();
            break;
        case DnsQualityEventHandler::MSG_DNS_QUERY_FAIL:
            handle_dns_fail();
            break;
        case DnsQualityEventHandler::MSG_DNS_REPORT_LOOP:
            send_dns_report();
            break;
        case DnsQualityEventHandler::MSG_DNS_NEW_REPORT:
            {
                auto report = event->GetSharedObject<NetsysNative::NetDnsResultReport>();
                add_dns_report(report);
            }
            break;
        case DnsQualityEventHandler::MSG_DNS_QUERY_RESULT_REPORT:
            {
                auto queryReport = event->GetSharedObject<NetsysNative::NetDnsQueryResultReport>();
                add_dns_query_report(queryReport);
            }
            break;
        case DnsQualityEventHandler::MSG_DNS_QUERY_ABNORMAL_REPORT:
            {
                auto queryReport = event->GetSharedObject<DnsAbnormalInfo>();
                handle_dns_abnormal(queryReport);
            }
            break;
    }
    return 0;
}
} // namespace OHOS::nmd

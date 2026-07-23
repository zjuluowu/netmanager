/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NETSYS_DNS_QUALITY_DIAG_H
#define NETSYS_DNS_QUALITY_DIAG_H

#include <iostream>
#include <map>
#include <time.h>

#include "dns_resolv_config.h"
#include "netnative_log_wrapper.h"
#include "dns_quality_event_handler.h"
#include "i_net_dns_result_callback.h"
#include "netsys_net_dns_result_data.h"
#include "dns_config_client.h"

#define FROM_CACHE_FLAG         (1 << 0)
#define VPN_NET_FLAG            (1 << 1)
#define IPV4_NO_ANSWER_FLAG     (1 << 2)
#define IPV4_CNAME_FLAG         (1 << 3)
#define IPV6_NO_ANSWER_FLAG     (1 << 4)
#define IPV6_CNAME_FLAG         (1 << 5)

namespace OHOS::nmd {

struct DnsAbnormalInfo {
    uint32_t eventfailcause;
    NetsysNative::NetDnsQueryResultReport report;
};

class DnsQualityDiag {
public:
    ~DnsQualityDiag() = default;

    static DnsQualityDiag &GetInstance();

    // for net_conn_service
    int32_t ReportDnsResult(uint16_t netId, uint16_t uid, uint32_t pid, int32_t usedtime, char* name,
                            uint32_t size, int32_t failreason, QueryParam param, AddrInfo* addrinfo,
                            uint32_t serverSize, DnsServerInfo* dnsseverinfo);

    int32_t ReportDnsQueryResult(PostDnsQueryParam queryParam, AddrInfo* addrinfo, uint8_t addrSize);

    int32_t ReportDnsQueryAbnormal(uint32_t eventfailcause, PostDnsQueryParam queryParam, AddrInfo* addrinfo);

    int32_t RegisterResultListener(const sptr<NetsysNative::INetDnsResultCallback> &callback, uint32_t timeStep);

    int32_t UnregisterResultListener(const sptr<NetsysNative::INetDnsResultCallback> &callback);

    int32_t SetLoopDelay(int32_t delay);

    int32_t HandleEvent(const AppExecFwk::InnerEvent::Pointer &event);

private:
    DnsQualityDiag();

    std::mutex cacheMutex_;

    std::mutex resultListenersMutex_;

    std::mutex dnsAbnormalTimeMutex_;

    std::mutex dnsReportResultMutex_;
    std::atomic_uint defaultNetId_;

    uint32_t monitor_loop_delay;

    uint32_t report_delay;

    uint32_t last_dns_abnormal_report_time = 0;

    std::atomic_bool handler_started;

    std::string queryAddr;

    std::list<sptr<NetsysNative::INetDnsResultCallback>> resultListeners_;

    std::shared_ptr<DnsQualityEventHandler> handler_;

    std::list<NetsysNative::NetDnsResultReport> report_;
    std::shared_mutex dnsQueryReportMutex_;
    std::list<NetsysNative::NetDnsQueryResultReport> dnsQueryReport_;

    int32_t InitHandler();
    int32_t query_default_host();
    int32_t handle_dns_loop();
    int32_t handle_dns_fail();
    int32_t send_dns_report();
    int32_t add_dns_report(std::shared_ptr<NetsysNative::NetDnsResultReport> report);
    int32_t add_dns_query_report(std::shared_ptr<NetsysNative::NetDnsQueryResultReport> report);
    int32_t load_query_addr(const char* defaultAddr);
    void GetDefaultDnsServerList(int32_t uid, std::vector<std::string> &servers);
    int32_t ParseReportAddr(uint32_t size, AddrInfo* addrinfo, NetsysNative::NetDnsResultReport &report);
    void FillDnsQueryResultReport(NetsysNative::NetDnsQueryResultReport &report,
        PostDnsQueryParam &queryParam);
    int32_t ParseDnsQueryReportAddr(uint8_t size,
        AddrInfo* addrinfo, NetsysNative::NetDnsQueryResultReport &report);
    int32_t handle_dns_abnormal(std::shared_ptr<DnsAbnormalInfo> abnormalInfo);
    void ParseDnsSever(uint32_t size, DnsServerInfo* serverInfo, NetsysNative::NetDnsResultReport &report);
};
} // namespace OHOS::nmd
#endif // NETSYS_DNS_QUALITY_DIAG_H

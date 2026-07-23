/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "net_dual_stack_probe.h"
#include "net_dns_resolve.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"
#include "tiny_count_down_latch.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int32_t DNS_RESOLVE_RESULT_WAIT_MS = 5 * 1000;
constexpr int32_t DNS_RESOLVE_THREAD_NUM = 2;
constexpr int32_t NET_PROBE_THREAD_NUM = 4;
constexpr int32_t NET_PROBE_SUCCESS_DURATION_THRESHOLD = 2 * 1000;
}

NetDualStackProbe::NetDualStackProbe(uint32_t netId, NetBearType bearType, const NetLinkInfo &netLinkInfo,
    std::string& httpUrl, std::string& httpsUrl, const std::weak_ptr<INetMonitorCallback> &callback)
    : netId_(netId), netBearType_(bearType), netLinkInfo_(netLinkInfo),
    httpUrl_(httpUrl), httpsUrl_(httpsUrl), netMonitorCallback_(callback)
{}

NetDualStackProbe::~NetDualStackProbe()
{}

int32_t NetDualStackProbe::StartDualStackProbeThread(
    const std::string& domain, const std::string& backDomain, int32_t timeOutDuration)
{
    if (isDualStackProbing_) {
        NETMGR_LOG_W("is dual stack detecting");
        return NETMANAGER_ERR_INTERNAL;
    }
    isDualStackProbing_ = true;
    std::weak_ptr<NetDualStackProbe> wp = shared_from_this();
    std::thread t([wp, domain, backDomain, timeOutDuration]() {
        auto netDualStackProbe = wp.lock();
        if (netDualStackProbe != nullptr) {
            netDualStackProbe->StartDualStackProbe(domain, backDomain, timeOutDuration);
        }
    });
    std::string threadName = "DualStackProbe";
    pthread_setname_np(t.native_handle(), threadName.c_str());
    t.detach();
    return NETMANAGER_SUCCESS;
}

void NetDualStackProbe::StartDualStackProbe(const std::string& domain,
    const std::string& backDomain, int32_t timeOutDuration)
{
    DualStackProbeResultCode resultCode = DualStackProbe(domain, backDomain, timeOutDuration);
    if (isDualStackProbing_) {
        auto monitorCallback = netMonitorCallback_.lock();
        if (monitorCallback == nullptr) {
            isDualStackProbing_ = false;
            return;
        }
        monitorCallback->OnHandleDualStackProbeResult(resultCode);
    }
    isDualStackProbing_ = false;
}

void NetDualStackProbe::StopDualStackProbe()
{
    NETMGR_LOG_E("Stop net[%{public}d] dual stack probe", netId_);
    isDualStackProbing_ = false;
}

DualStackProbeResultCode NetDualStackProbe::DualStackProbe(const std::string& domain,
    const std::string& backDomain, int32_t timeOutDuration)
{
    NETMGR_LOG_D("start dual stack probe");
    std::string addrIpv4List;
    std::string addrIpv6List;
    DoDnsResolve(domain, backDomain, addrIpv4List, addrIpv6List);
    DualStackProbeResultCode result = DoDualStackHttpProbe(addrIpv4List, addrIpv6List, timeOutDuration);
    return result;
}

void NetDualStackProbe::DoDnsResolve(const std::string& domain, const std::string& backDomain,
    std::string &ipv4AddrList, std::string &ipv6AddrList)
{
    std::shared_ptr<TinyCountDownLatch> dnsLatch = std::make_shared<TinyCountDownLatch>(DNS_RESOLVE_THREAD_NUM);
    std::shared_ptr<NetDnsResolve> dnsResolve = std::make_shared<NetDnsResolve>(netId_, dnsLatch, domain);
    std::shared_ptr<NetDnsResolve> backDnsResolve =
        std::make_shared<NetDnsResolve>(netId_, dnsLatch, backDomain);
    dnsResolve->Start();
    backDnsResolve->Start();
    dnsLatch->Await(std::chrono::milliseconds(DNS_RESOLVE_RESULT_WAIT_MS));

    std::string addrIpv4 = dnsResolve->GetDnsResolveResultByType(INetAddr::IpType::IPV4);
    std::string addrIpv6 = dnsResolve->GetDnsResolveResultByType(INetAddr::IpType::IPV6);
    std::string backAddrIpv4 = backDnsResolve->GetDnsResolveResultByType(INetAddr::IpType::IPV4);
    std::string backAddrIpv6 = backDnsResolve->GetDnsResolveResultByType(INetAddr::IpType::IPV6);
    ipv4AddrList = ProcessDnsResolveResult(addrIpv4, backAddrIpv4);
    ipv6AddrList = ProcessDnsResolveResult(addrIpv6, backAddrIpv6);
}

std::string NetDualStackProbe::ProcessDnsResolveResult(const std::string &addr, const std::string &backAddr)
{
    if (!addr.empty() && !backAddr.empty()) {
        return addr + "," + backAddr;
    }
    return addr.empty() ? backAddr : addr;
}

DualStackProbeResultCode NetDualStackProbe::DoDualStackHttpProbe(
    const std::string &ipv4AddrList, const std::string &ipv6AddrList, int32_t timeOutDuration)
{
    if (ipv4AddrList.empty() || ipv6AddrList.empty()) {
        NETMGR_LOG_I("ip is empty");
        return DualStackProbeResultCode::PROBE_FAIL;
    }
    std::lock_guard<std::mutex> probeLocker(probeMtx_);
    std::shared_ptr<TinyCountDownLatch> latch = std::make_shared<TinyCountDownLatch>(NET_PROBE_THREAD_NUM);
    std::shared_ptr<TinyCountDownLatch> latchAll = std::make_shared<TinyCountDownLatch>(NET_PROBE_THREAD_NUM);
    std::shared_ptr<ProbeThread> httpThreadV4 = std::make_shared<ProbeThread>(
            netId_, netBearType_, netLinkInfo_, latch, latchAll,
            ProbeType::PROBE_HTTP, httpUrl_, httpsUrl_, ipv4AddrList);
    std::shared_ptr<ProbeThread> httpsThreadV4 = std::make_shared<ProbeThread>(
            netId_, netBearType_, netLinkInfo_, latch, latchAll,
            ProbeType::PROBE_HTTPS, httpUrl_, httpsUrl_, ipv4AddrList);
    std::shared_ptr<ProbeThread> httpThreadV6 = std::make_shared<ProbeThread>(
            netId_, netBearType_, netLinkInfo_, latch, latchAll,
            ProbeType::PROBE_HTTP, httpUrl_, httpsUrl_, ipv6AddrList);
    std::shared_ptr<ProbeThread> httpsThreadV6 = std::make_shared<ProbeThread>(
            netId_, netBearType_, netLinkInfo_, latch, latchAll,
            ProbeType::PROBE_HTTPS, httpUrl_, httpsUrl_, ipv6AddrList);
    httpThreadV4->Start();
    httpsThreadV4->Start();
    httpThreadV6->Start();
    httpsThreadV6->Start();
    NETMGR_LOG_I("DualStackProbe time out:%{public}d", timeOutDuration);
    latch->Await(std::chrono::milliseconds(timeOutDuration));
    latchAll->Await(std::chrono::milliseconds(timeOutDuration));
    DualStackProbeResultCode result = ProcessProbeResult(httpThreadV4, httpsThreadV4,
        httpThreadV6, httpsThreadV6);
    return result;
}

DualStackProbeResultCode NetDualStackProbe::ProcessProbeResult(std::shared_ptr<ProbeThread>& httpThreadV4,
    std::shared_ptr<ProbeThread>& httpsThreadV4, std::shared_ptr<ProbeThread>& httpThreadV6,
    std::shared_ptr<ProbeThread>& httpsThreadV6)
{
    NetHttpProbeResult httpV4Result = GetThreadDetectResult(httpThreadV4);
    NetHttpProbeResult httpsV4Result = GetThreadDetectResult(httpsThreadV4);
    NetHttpProbeResult httpV6Result = GetThreadDetectResult(httpThreadV6);
    NetHttpProbeResult httpsV6Result = GetThreadDetectResult(httpsThreadV6);
    DualStackProbeResultCode result = DualStackProbeResultCode::PROBE_FAIL;
    bool isV4Success = httpV4Result.IsSuccessful() || httpsV4Result.IsSuccessful();
    bool isV6Success = httpV6Result.IsSuccessful() || httpsV6Result.IsSuccessful();
    uint64_t ipv4ProbeDuration = GetProbeDurationTime(httpThreadV4, httpsThreadV4);
    uint64_t ipv6ProbeDuration = GetProbeDurationTime(httpThreadV6, httpsThreadV6);
    NETMGR_LOG_I("Probe result, v4:%{public}d, v6:%{public}d", isV4Success, isV6Success);
    if (isV4Success && isV6Success) {
        result = DualStackProbeResultCode::PROBE_SUCCESS;
    } else if (httpV4Result.IsNeedPortal() || httpV6Result.IsNeedPortal()) {
        result = DualStackProbeResultCode::PROBE_PORTAL;
    } else if (isV4Success && ipv4ProbeDuration <= NET_PROBE_SUCCESS_DURATION_THRESHOLD) {
        result = DualStackProbeResultCode::PROBE_SUCCESS_IPV4;
    } else if (isV6Success && ipv6ProbeDuration <= NET_PROBE_SUCCESS_DURATION_THRESHOLD) {
        result = DualStackProbeResultCode::PROBE_SUCCESS_IPV6;
    }
    return result;
}

NetHttpProbeResult NetDualStackProbe::GetThreadDetectResult(std::shared_ptr<ProbeThread>& probeThread)
{
    NetHttpProbeResult result;
    if (probeThread == nullptr) {
        return result;
    }
    auto probeType = probeThread->GetProbeType();
    if (!probeThread->IsDetecting()) {
        if (probeType == ProbeType::PROBE_HTTP || probeType == ProbeType::PROBE_HTTP_FALLBACK) {
            return probeThread->GetHttpProbeResult();
        } else {
            return probeThread->GetHttpsProbeResult();
        }
    }
    return result;
}

uint64_t NetDualStackProbe::GetProbeDurationTime(
    std::shared_ptr<ProbeThread>& httpThread, std::shared_ptr<ProbeThread>& httpsThread)
{
    if (httpThread == nullptr || httpsThread == nullptr) {
        return 0;
    }
    uint64_t httpSuccessProbeTime = httpThread->GetProbeDurationTime();
    uint64_t httpsSuccessProbeTime = httpsThread->GetProbeDurationTime();
    bool isHttpProbeSuccess = GetThreadDetectResult(httpThread).IsSuccessful();
    bool isHttpsProbeSuccess = GetThreadDetectResult(httpsThread).IsSuccessful();
    if (isHttpProbeSuccess && isHttpsProbeSuccess) {
        return httpSuccessProbeTime < httpsSuccessProbeTime ? httpSuccessProbeTime : httpsSuccessProbeTime;
    } else if (isHttpProbeSuccess) {
        return httpSuccessProbeTime;
    } else if (isHttpsProbeSuccess) {
        return httpsSuccessProbeTime;
    }
    return 0;
}
} // namespace NetManagerStandard
} // namespace OHOS

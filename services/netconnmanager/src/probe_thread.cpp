/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <future>
#include <list>
#include <memory>
#include <netdb.h>
#include <regex>
#include <securec.h>
#include <sys/socket.h>
#include <thread>
#include <pthread.h>
#include <unistd.h>

#include "probe_thread.h"
#include "dns_config_client.h"
#include "event_report.h"
#include "fwmark_client.h"
#include "netmanager_base_common_utils.h"
#include "netsys_controller.h"
#include "net_http_proxy_tracker.h"
#include "net_mgr_log_wrapper.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {

static void NetProbeThread(const std::shared_ptr<ProbeThread> &probeThread)
{
    if (probeThread == nullptr) {
        NETMGR_LOG_E("probeThread is nullptr");
        return;
    }
    ProbeType type = probeThread->GetProbeType();
    probeThread->SendHttpProbe(type);
}

ProbeThread::ProbeThread(uint32_t netId, NetBearType bearType, const NetLinkInfo &netLinkInfo,
    std::shared_ptr<TinyCountDownLatch> latch, std::shared_ptr<TinyCountDownLatch> latchAll,
    ProbeType probeType, std::string httpUrl, std::string httpsUrl, std::string ipAddrList)
    : netId_(netId), probeType_(probeType), latch_(latch), latchAll_(latchAll), httpProbeUrl_(httpUrl),
    httpsProbeUrl_(httpsUrl), ipAddrList_(ipAddrList)
{
    httpProbe_ = std::make_unique<NetHttpProbe>(netId, bearType, netLinkInfo, probeType, ipAddrList);
}

ProbeThread::~ProbeThread()
{
    if (thread_.joinable()) {
        thread_.join();
    }
}

void ProbeThread::Start()
{
    NETMGR_LOG_D("Start net[%{public}d] monitor in", netId_);
    isDetecting_ = true;
    std::shared_ptr<ProbeThread> probeThead = shared_from_this();
    std::shared_ptr<ProbeThread> temp;
    std::atomic_store_explicit(&temp, std::move(probeThead), std::memory_order_release);
    thread_ = std::thread([thread = std::atomic_load_explicit(&temp, std::memory_order_acquire)] {
        return NetProbeThread(thread);
    });
    std::string threadName = "netDetectThread";
    pthread_setname_np(thread_.native_handle(), threadName.c_str());
    thread_.detach();
}

void ProbeThread::SendHttpProbe(ProbeType probeType)
{
    if (httpProbeUrl_.empty() || httpsProbeUrl_.empty()) {
        NETMGR_LOG_E("Net:[%{public}d] httpProbeUrl is empty", netId_);
        isDetecting_ = false;
        latch_->CountDown();
        latchAll_->CountDown();
        return;
    }

    if (httpProbe_ == nullptr) {
        NETMGR_LOG_E("Net:[%{public}d] httpProbe_ is nullptr", netId_);
        isDetecting_ = false;
        latch_->CountDown();
        latchAll_->CountDown();
        return;
    }

    probeDuration_ = CommonUtils::GetCurrentMilliSecond();
    if (httpProbe_->SendProbe(probeType, httpProbeUrl_, httpsProbeUrl_) != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Net:[%{public}d] send probe failed.", netId_);
        isDetecting_ = false;
        latch_->CountDown();
        latchAll_->CountDown();
        return;
    }
    probeDuration_ = CommonUtils::GetCurrentMilliSecond() - probeDuration_;

    if (IsConclusiveResult()) {
        isDetecting_ = false;
        while (latch_->GetCount() > 0 && ipAddrList_.empty()) {
            latch_->CountDown();
        }
        while (latchAll_->GetCount() > 0 && ipAddrList_.empty()) {
            latchAll_->CountDown();
        }
    }
    isDetecting_ = false;
    if (latch_->GetCount() > 0) {
        latch_->CountDown();
    }
    if (latchAll_->GetCount() > 0) {
        latchAll_->CountDown();
    }
}

bool ProbeThread::IsConclusiveResult()
{
    if ((probeType_ == ProbeType::PROBE_HTTP || probeType_ == ProbeType::PROBE_HTTP_FALLBACK) &&
        httpProbe_->GetHttpProbeResult().IsNeedPortal()) {
        NETMGR_LOG_I("http url detection result: portal");
        return true;
    }
    if ((probeType_ == ProbeType::PROBE_HTTPS || probeType_ == ProbeType::PROBE_HTTPS_FALLBACK) &&
        httpProbe_->GetHttpsProbeResult().IsSuccessful()) {
        NETMGR_LOG_I("https url detection result: success");
        return true;
    }
    return false;
}

void ProbeThread::UpdateGlobalHttpProxy(const HttpProxy &httpProxy)
{
    if (httpProbe_) {
        httpProbe_->UpdateGlobalHttpProxy(httpProxy);
    }
}

void ProbeThread::SetXReqId(const std::string& xReqId, int8_t xReqIdLen)
{
    if (httpProbe_) {
        httpProbe_->SetXReqId(xReqId, xReqIdLen);
    }
}

void ProbeThread::ProbeWithoutGlobalHttpProxy()
{
    httpProbe_->ProbeWithoutGlobalHttpProxy();
}

NetHttpProbeResult ProbeThread::GetHttpProbeResult()
{
    return httpProbe_->GetHttpProbeResult();
}

NetHttpProbeResult ProbeThread::GetHttpsProbeResult()
{
    return httpProbe_->GetHttpsProbeResult();
}

bool ProbeThread::IsDetecting()
{
    return isDetecting_.load();
}

ProbeType ProbeThread::GetProbeType()
{
    return probeType_;
}

uint64_t ProbeThread::GetProbeDurationTime()
{
    return probeDuration_;
}

}
}
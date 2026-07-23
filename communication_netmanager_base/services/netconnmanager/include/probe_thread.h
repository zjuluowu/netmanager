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

#ifndef NET_MANAGER_BASE_PROBE_THREAD_H
#define NET_MANAGER_BASE_PROBE_THREAD_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <pthread.h>

#include "refbase.h"

#include "i_net_monitor_callback.h"
#include "net_conn_types.h"
#include "net_http_probe.h"
#include "net_link_info.h"
#include "tiny_count_down_latch.h"

namespace OHOS {
namespace NetManagerStandard {
class ProbeThread : public std::enable_shared_from_this<ProbeThread> {
public:

    ProbeThread(uint32_t netId, NetBearType bearType, const NetLinkInfo &netLinkInfo,
        std::shared_ptr<TinyCountDownLatch> latch, std::shared_ptr<TinyCountDownLatch> latchAll,
        ProbeType probeType, std::string httpUrl, std::string httpsUrl, std::string ipAddrList = "");

    /**
     * Destroy the ProbeThread
     *
     */
    ~ProbeThread();

    /**
     * Start detection
     *
     */
    void Start();

    /**
     * Update global http proxy
     *
     */
    void UpdateGlobalHttpProxy(const HttpProxy &httpProxy);

    /**
     * Set global Req Id
     */
    void SetXReqId(const std::string& xReqId, int8_t xReqIdLen);

    /*
     * send http probe
    */
    void SendHttpProbe(ProbeType probeType);

    /*
     * Get Http detection Result
    */
    NetHttpProbeResult GetHttpProbeResult();

    /*
     * Get Https detection result
    */
    NetHttpProbeResult GetHttpsProbeResult();

    /*
     * set detection without proxy
    */
    void ProbeWithoutGlobalHttpProxy();

    /*
     * check http result whether portal or success
    */
    bool IsConclusiveResult();

    /*
     * check whether thread is detecting
    */
    bool IsDetecting();

    /*
     * get current probe type
    */
    ProbeType GetProbeType();

    /*
     * get probe duration time
    */
    uint64_t GetProbeDurationTime();

private:
    uint32_t netId_ = 0;
    std::unique_ptr<NetHttpProbe> httpProbe_;
    std::thread thread_;
    ProbeType probeType_;
    std::shared_ptr<TinyCountDownLatch> latch_;
    std::shared_ptr<TinyCountDownLatch> latchAll_;
    std::atomic<bool> isDetecting_ = false;
    std::string httpProbeUrl_;
    std::string httpsProbeUrl_;
    std::string ipAddrList_;
    uint64_t probeDuration_ = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_MANAGER_BASE_PROBE_THREAD_H

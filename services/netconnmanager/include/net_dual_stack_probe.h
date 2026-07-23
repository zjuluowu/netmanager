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
#ifndef NET_DUAL_STACK_PROBE_H
#define NET_DUAL_STACK_PROBE_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

#include "i_net_monitor_callback.h"
#include "net_conn_types.h"
#include "net_datashare_utils.h"
#include "net_http_probe.h"
#include "net_link_info.h"
#include "probe_thread.h"
#include "dual_stack_probe_callback.h"

namespace OHOS {
namespace NetManagerStandard {

class NetDualStackProbe : public std::enable_shared_from_this<NetDualStackProbe> {
public:
    NetDualStackProbe(uint32_t netId, NetBearType bearType, const NetLinkInfo &netLinkInfo,
        std::string& httpUrl, std::string& httpsUrl, const std::weak_ptr<INetMonitorCallback> &callback);
    ~NetDualStackProbe();

    int32_t StartDualStackProbeThread(const std::string& domain,
        const std::string& backDomain, int32_t timeOutDuration);
    void StartDualStackProbe(const std::string& domain, const std::string& backDomain, int32_t timeOutDuration);
    void StopDualStackProbe();

private:
    DualStackProbeResultCode DualStackProbe(const std::string& domain,
        const std::string& backDomain, int32_t timeOutDuration);
    void DoDnsResolve(const std::string& domain, const std::string& backDomain,
        std::string &ipv4AddrList, std::string &ipv6AddrList);
    std::string ProcessDnsResolveResult(const std::string &addr, const std::string &backAddr);
    DualStackProbeResultCode DoDualStackHttpProbe(const std::string &ipv4AddrList,
        const std::string &ipv6AddrList, int32_t timeOutDuration);
    DualStackProbeResultCode ProcessProbeResult(std::shared_ptr<ProbeThread>& httpThreadV4,
        std::shared_ptr<ProbeThread>& httpsThreadV4, std::shared_ptr<ProbeThread>& httpThreadV6,
        std::shared_ptr<ProbeThread>& httpsThreadV6);
    NetHttpProbeResult GetThreadDetectResult(std::shared_ptr<ProbeThread>& probeThread);
    uint64_t GetProbeDurationTime(std::shared_ptr<ProbeThread>& httpThread, std::shared_ptr<ProbeThread>& httpsThread);

    uint32_t netId_ = 0;
    NetBearType netBearType_;
    NetLinkInfo netLinkInfo_;
    std::string httpUrl_;
    std::string httpsUrl_;
    std::weak_ptr<INetMonitorCallback> netMonitorCallback_;

    std::atomic<bool> isDualStackProbing_ = false;
    std::mutex probeMtx_;
};

} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_DUAL_STACK_PROBE_H

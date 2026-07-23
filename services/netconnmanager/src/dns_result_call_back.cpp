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
#include "dns_result_call_back.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_conn_service.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr double FAIL_RATE = 0.6;
constexpr int32_t MAX_FAIL_VALUE = 3;

enum DnsFailReason {
    DNS_FAIL_REASON_PARAM_INVALID = -1101,
};

int32_t NetDnsResultCallback::OnDnsResultReport(uint32_t size,
    const std::list<NetsysNative::NetDnsResultReport> netDnsResultReport)
{
    netDnsResult_.Clear();
    IterateDnsReportResults(netDnsResultReport);
    netDnsResult_.Iterate([this](int32_t netid, NetDnsResult dnsResult) {
        double failRate = static_cast<double>(dnsResult.failReports_) / dnsResult.totalReports_;
        NETMGR_LOG_D("Reports: netId:%{public}d fail-total:%{public}d-%{public}d",
                     netid, dnsResult.totalReports_, dnsResult.failReports_);
        if (failRate > FAIL_RATE) {
            uint32_t failValue_ = 0;
            RequestNetDetection(failValue_, netid);
            NETMGR_LOG_D("Netdetection for dns fail, netId:%{public}d,totalReports:%{public}d, failReports:%{public}d,"
                         "failValue:%{public}d", netid, dnsResult.totalReports_, dnsResult.failReports_, failValue_);
        } else {
            NETMGR_LOG_D("Netdetection for dns success, netId:%{public}d, totalReports:%{public}d,"
                         "failReports:%{public}d", netid, dnsResult.totalReports_, dnsResult.failReports_);
            failCount_.EnsureInsert(netid, 0);
            int32_t result = NetConnService::GetInstance()->NetDetectionForDnsHealth(netid, true);
            if (result != 0) {
                NETMGR_LOG_E("NetDetectionForDnsHealth failed");
            }
        }
    });
    return NETMANAGER_SUCCESS;
}

void NetDnsResultCallback::RequestNetDetection(uint32_t &failValue_, uint32_t netid)
{
    if (!failCount_.Find(netid, failValue_)) {
        failValue_ = 1;
        failCount_.EnsureInsert(netid, failValue_);
    } else {
        failValue_++;
        if (failValue_ >= MAX_FAIL_VALUE) {
            NETMGR_LOG_I("netId:%{public}d start net detection with DNS fail value failValue:%{public}d",
                         netid, failValue_);
            NetConnService::GetInstance()->NetDetectionForDnsHealth(netid, false);
            failCount_.EnsureInsert(netid, 0);
        } else {
            failCount_.EnsureInsert(netid, failValue_);
        }
    }
}

void NetDnsResultCallback::GetDumpMessageForDnsResult(std::string &message)
{
    message.append("Dns result Info:\n");
    netDnsResult_.Iterate([&message](int32_t netid, NetDnsResult dnsResult) {
        message.append("\tnetId: " + std::to_string(netid) + "\n");
        message.append("\ttotalReports: " + std::to_string(dnsResult.totalReports_) + "\n");
        message.append("\tfailReports: " + std::to_string(dnsResult.failReports_) + "\n");
    });
}

void NetDnsResultCallback::IterateDnsReportResults(
    const std::list<NetsysNative::NetDnsResultReport> netDnsResultReport)
{
    int32_t defaultNetid = 0;
    int32_t result = NetConnService::GetInstance()->GetDefaultNet(defaultNetid);
    NETMGR_LOG_D("GetDefaultNet result: %{public}d, defaultNetid: %{public}d", result, defaultNetid);
    for (auto &it : netDnsResultReport) {
        NETMGR_LOG_D("netId_: %{public}d, queryResult_: %{public}d, pid_ : %{public}d",
                     it.netid_, it.queryresult_, it.pid_);
        if (!CheckDnsSentByResult(static_cast<int32_t>(it.queryresult_))) {
            continue;
        }
        NetDnsResult existResult;
        bool ret =  netDnsResult_.Find(it.netid_, existResult);
        if (!ret && it.netid_ == 0) {
            NetDnsResult newDefaultResult;
            if (!netDnsResult_.Find(defaultNetid, newDefaultResult)) {
                NetDnsResult defaultResult;
                defaultResult.totalReports_ = 1;
                defaultResult.failReports_ = it.queryresult_ == 0 ? 0 : 1;
                netDnsResult_.EnsureInsert(defaultNetid, defaultResult);
            } else {
                newDefaultResult = netDnsResult_.ReadVal(defaultNetid);
                newDefaultResult.totalReports_++;
                newDefaultResult.failReports_ += it.queryresult_ == 0 ? 0 : 1;
                netDnsResult_.EnsureInsert(defaultNetid, newDefaultResult);
            }
        } else if (!ret) {
            NetDnsResult newResult;
            newResult.totalReports_ = 1;
            newResult.failReports_ = it.queryresult_ == 0 ? 0 : 1;
            netDnsResult_.EnsureInsert(it.netid_, newResult);
        } else {
            existResult = netDnsResult_.ReadVal(it.netid_);
            existResult.totalReports_++;
            existResult.failReports_ += it.queryresult_ == 0 ? 0 : 1;
            netDnsResult_.EnsureInsert(it.netid_, existResult);
        }
    }
}

bool NetDnsResultCallback::CheckDnsSentByResult(int32_t result)
{
    if (result == DNS_FAIL_REASON_PARAM_INVALID) {
        return false;
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS

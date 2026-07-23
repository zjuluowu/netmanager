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

#ifndef DNS_RESULT_CALL_BACK_H
#define DNS_RESULT_CALL_BACK_H

#include <map>
#include <mutex>

#include "netsys_net_dns_result_data.h"
#include "netsys_dns_report_callback.h"
#include "safe_map.h"

namespace OHOS {
namespace NetManagerStandard {

struct NetDnsResult {
    uint32_t        totalReports_;
    uint32_t        failReports_;
};

class NetDnsResultCallback : public NetsysDnsReportCallback {
public:
    int32_t OnDnsResultReport(uint32_t size, const std::list<NetsysNative::NetDnsResultReport>);
    void GetDumpMessageForDnsResult(std::string &message);

private:
    void IterateDnsReportResults(const std::list<NetsysNative::NetDnsResultReport> netDnsResultReport);
    void RequestNetDetection(uint32_t &failvalue_, uint32_t netid);
    bool CheckDnsSentByResult(int32_t result);

private:
    SafeMap<uint32_t, NetDnsResult> netDnsResult_;
    SafeMap<uint32_t, uint32_t> failCount_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // DNS_RESULT_CALL_BACK_H

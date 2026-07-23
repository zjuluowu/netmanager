/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef NETSYS_DNS_REPORT_CALLBACK_H
#define NETSYS_DNS_REPORT_CALLBACK_H

#include "refbase.h"
#include "netsys_net_dns_result_data.h"

namespace OHOS {
namespace NetManagerStandard {
class NetsysDnsReportCallback : public RefBase {

public:
    virtual ~NetsysDnsReportCallback() {};
    virtual int32_t OnDnsResultReport(uint32_t size, const std::list<OHOS::NetsysNative::NetDnsResultReport>) = 0;
};


class NetsysDnsQueryReportCallback : public RefBase {
public:
    virtual ~NetsysDnsQueryReportCallback() {};
    virtual int32_t OnDnsQueryResultReport(uint32_t size,
        const std::list<OHOS::NetsysNative::NetDnsQueryResultReport> res) = 0;
    virtual int32_t OnDnsQueryAbnormalReport(uint32_t eventfailcause,
        const OHOS::NetsysNative::NetDnsQueryResultReport res) = 0;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NetsysDnsReportCallback

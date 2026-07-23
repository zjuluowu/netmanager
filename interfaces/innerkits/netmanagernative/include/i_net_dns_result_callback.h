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

#ifndef I_NET_DNS_RESULT_CALLBACK_H
#define I_NET_DNS_RESULT_CALLBACK_H

#include <string>
#include "iremote_stub.h"
#include "netsys_net_dns_result_data.h"

namespace OHOS {
namespace NetsysNative {
class INetDnsResultCallback : public IRemoteBroker {
public:
    virtual ~INetDnsResultCallback() = default;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetsysNative.INetDnsResultCallback");
    enum {
        ON_DNS_RESULT_REPORT = 0,
        ON_DNS_QUERY_RESULT_REPORT,
        ON_DNS_QUERY_ABNORMAL_REPORT,
    };

public:
    virtual int32_t OnDnsResultReport(uint32_t size, const std::list<NetDnsResultReport>) = 0;
    virtual int32_t OnDnsQueryResultReport(uint32_t size, const std::list<NetDnsQueryResultReport>) = 0;
    virtual int32_t OnDnsQueryAbnormalReport(uint32_t eventfailcause,
        const NetDnsQueryResultReport report) = 0;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // I_NET_DNS_RESULT_CALLBACK_H

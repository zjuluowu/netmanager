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
#ifndef  NET_DNS_RESULT_CALLBACK_PROXY_H
#define  NET_DNS_RESULT_CALLBACK_PROXY_H

#include "iremote_proxy.h"
#include "i_net_dns_result_callback.h"

namespace OHOS {
namespace NetsysNative {
class NetDnsResultCallbackProxy : public IRemoteProxy<INetDnsResultCallback> {
public:
    explicit NetDnsResultCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetDnsResultCallbackProxy() = default;

    int32_t OnDnsResultReport(uint32_t size, const std::list<NetDnsResultReport>) override;
    int32_t OnDnsQueryResultReport(uint32_t size, const std::list<NetDnsQueryResultReport>) override;
    int32_t OnDnsQueryAbnormalReport(uint32_t eventfailcause, const NetDnsQueryResultReport report) override;

private:
    static inline BrokerDelegator<NetDnsResultCallbackProxy> delegator_;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // NET_DNS_RESULT_CALLBACK_PROXY_H

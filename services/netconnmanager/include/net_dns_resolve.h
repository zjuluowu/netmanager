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
#ifndef NET_DNS_RESOLVE_H
#define NET_DNS_RESOLVE_H

#include <string>
#include "inet_addr.h"
#include "tiny_count_down_latch.h"

namespace OHOS {
namespace NetManagerStandard {

class NetDnsResolve : public std::enable_shared_from_this<NetDnsResolve> {
public:
    NetDnsResolve(uint32_t netId, std::shared_ptr<TinyCountDownLatch>& latch, const std::string& domain);
    ~NetDnsResolve();

    void Start();
    void StartDnsResolve();
    std::string GetDnsResolveResultByType(INetAddr::IpType ipType = INetAddr::IpType::UNKNOWN);
private:
    void GetAddrInfo();

    uint32_t netId_ = 0;
    std::shared_ptr<TinyCountDownLatch> latch_;
    std::string domain_;
    std::string resolveResultIpv4_;
    std::string resolveResultIpv6_;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_DNS_RESOLVE_H

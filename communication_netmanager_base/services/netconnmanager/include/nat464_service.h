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
#ifndef NAT464_SERVICE_H
#define NAT464_SERVICE_H

#include <string>

#include "clat_constants.h"
#include "ffrt.h"
#include "inet_addr.h"
#include "net_all_capabilities.h"

namespace OHOS {
namespace NetManagerStandard {

class NetSupplier;

class Nat464Service : public std::enable_shared_from_this<Nat464Service> {
public:
    Nat464Service(int32_t netId, const std::string &v6Iface);

    void UpdateService(Nat464UpdateFlag updateFlag);

    void MaybeUpdateV6Iface(const std::string &v6Iface);

private:
    void UpdateServiceState(Nat464UpdateFlag updateFlag);

    void StartPrefixDiscovery();

    void DiscoverPrefix();

    bool GetPrefixFromDns64();

    void StopPrefixDiscovery();

    void StartService();

    void StopService();

    int32_t netId_;
    std::string v6Iface_;
    std::string v4TunIface_;
    static inline ffrt::queue serviceUpdateQueue_{"Nat464ServiceUpdateState"};
    std::atomic<bool> tryStopDiscovery_;
    uint32_t discoveryCycleMs_;
    uint8_t discoveryIter_;
    INetAddr nat64PrefixFromDns_;

    volatile Nat464ServiceState serviceState_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif
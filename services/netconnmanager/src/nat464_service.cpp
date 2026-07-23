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
#include "nat464_service.h"

#include <algorithm>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

#include "ffrt.h"
#include "inet_addr.h"
#include "net_all_capabilities.h"
#include "net_interface_config.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netsys_controller.h"

namespace OHOS {
namespace NetManagerStandard {
Nat464Service::Nat464Service(int32_t netId, const std::string &v6Iface)
{
    netId_ = netId;
    v6Iface_ = v6Iface;
    v4TunIface_ = std::string(CLAT_PREFIX) + v6Iface;
    tryStopDiscovery_ = false;
    discoveryCycleMs_ = INITIAL_DISCOVERY_CYCLE_MS;
    discoveryIter_ = 1;
    serviceState_ = NAT464_SERVICE_STATE_IDLE;
}

void Nat464Service::MaybeUpdateV6Iface(const std::string &v6Iface)
{
    if (serviceState_ == NAT464_SERVICE_STATE_IDLE) {
        v6Iface_ = v6Iface;
        v4TunIface_ = std::string(CLAT_PREFIX) + v6Iface;
    }
}

void Nat464Service::UpdateService(Nat464UpdateFlag updateFlag)
{
    std::weak_ptr<Nat464Service> wp = shared_from_this();
    auto handle = serviceUpdateQueue_.submit_h([wp, updateFlag]() {
        if (auto sharedSelf = wp.lock()) {
            sharedSelf->UpdateServiceState(updateFlag);
        }
    }, ffrt::task_attr().name("UpdateNat464ServiceState"));
    serviceUpdateQueue_.wait(handle);
}

void Nat464Service::UpdateServiceState(Nat464UpdateFlag updateFlag)
{
    NETMGR_LOG_I("update nat464 service state");
    switch (serviceState_) {
        case NAT464_SERVICE_STATE_IDLE:
            if (updateFlag == NAT464_SERVICE_CONTINUE) {
                StartPrefixDiscovery();
                serviceState_ = NAT464_SERVICE_STATE_DISCOVERING;
            }
            break;

        case NAT464_SERVICE_STATE_DISCOVERING:
            if (updateFlag == NAT464_SERVICE_STOP) {
                StopPrefixDiscovery();
                serviceState_ = NAT464_SERVICE_STATE_IDLE;
            }
            if (updateFlag == NAT464_SERVICE_CONTINUE && !nat64PrefixFromDns_.address_.empty()) {
                StartService();
                serviceState_ = NAT464_SERVICE_STATE_RUNNING;
            }
            break;

        case NAT464_SERVICE_STATE_RUNNING:
            if (updateFlag == NAT464_SERVICE_STOP) {
                StopService();
                serviceState_ = NAT464_SERVICE_STATE_IDLE;
                break;
            }
            break;
    }
}

void Nat464Service::StartPrefixDiscovery()
{
    NETMGR_LOG_I("start to discover prefix64 from DNS64 server");
    std::weak_ptr<Nat464Service> wp = shared_from_this();
    ffrt::submit([wp]() {
            if (auto sharedSelf = wp.lock()) {
                sharedSelf->DiscoverPrefix();
            }
        }, {}, {}, ffrt::task_attr()
            .name(("Prefix64DiscoveryIter" + std::to_string(discoveryIter_)).c_str()));
}

void Nat464Service::DiscoverPrefix()
{
    if (tryStopDiscovery_) {
        NETMGR_LOG_I("stop flag is true, stop cycle");
        tryStopDiscovery_ = false;
        discoveryCycleMs_ = INITIAL_DISCOVERY_CYCLE_MS;
        discoveryIter_ = 1;
        return;
    }
    if (GetPrefixFromDns64()) {
        NETMGR_LOG_I("Get prefix64 from DNS64 server, stop cycle");
        discoveryCycleMs_ = INITIAL_DISCOVERY_CYCLE_MS;
        discoveryIter_ = 1;
        UpdateService(NAT464_SERVICE_CONTINUE);
    } else if (discoveryCycleMs_ > MAX_DISCOVERY_CYCLE_MS) {
        NETMGR_LOG_W("Fail to get prefix64 from DNS64 after %{public}u iterations, stop cycle", discoveryIter_);
    } else {
        NETMGR_LOG_I("Fail to get prefix64 from DNS64 server, try again after %{public}u ms", discoveryCycleMs_);
        ffrt::this_task::sleep_for(std::chrono::milliseconds(discoveryCycleMs_));
        discoveryIter_ += 1;
        discoveryCycleMs_ *= DISCOVERY_CYCLE_MULTIPLIER;
        std::weak_ptr<Nat464Service> wp = shared_from_this();
        ffrt::submit([wp]() {
                if (auto sharedSelf = wp.lock()) {
                    sharedSelf->DiscoverPrefix();
                }
            }, {}, {}, ffrt::task_attr()
                .name(("Prefix64DiscoveryIter" + std::to_string(discoveryIter_)).c_str()));
    }
}

bool Nat464Service::GetPrefixFromDns64()
{
    addrinfo hint = {};
    addrinfo *result;
    hint.ai_family = AF_INET6;

    queryparam qparam = {};
    qparam.qp_netid = netId_;
    qparam.qp_type = 1;

    int32_t ret = getaddrinfo_ext(IPV4_ONLY_HOST, NULL, &hint, &result, &qparam);
    if (ret != 0) {
        NETMGR_LOG_W("fail to get v6Addr of the well-known ipv4-only host from dns, errno: %{public}d", ret);
        return false;
    }

    INetAddr prefixAddr;
    for (addrinfo *tmp = result; tmp != nullptr; tmp = tmp->ai_next) {
        if (tmp->ai_family != AF_INET6) {
            continue;
        }
        auto addr = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
        char addrstr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &addr->sin6_addr, addrstr, sizeof(addrstr));
        prefixAddr.address_ = addrstr;
        prefixAddr.family_ = tmp->ai_family;
        prefixAddr.prefixlen_ = CLAT_PREFIX_BYTE_LEN * CHAR_BIT;
        break;
    }
    freeaddrinfo(result);

    nat64PrefixFromDns_ = prefixAddr;
    return true;
}

void Nat464Service::StopPrefixDiscovery()
{
    tryStopDiscovery_ = true;
}

void Nat464Service::StartService()
{
    if (serviceState_ == NAT464_SERVICE_STATE_RUNNING) {
        NETMGR_LOG_W("Nat464 service already started");
        return;
    }

    int32_t ret = NetsysController::GetInstance().StartClat(v6Iface_, netId_, nat64PrefixFromDns_.address_);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_W("fail to start clat, error no: %{public}d", ret);
        return;
    }
}

void Nat464Service::StopService()
{
    NetsysController::GetInstance().StopClat(v6Iface_);
    nat64PrefixFromDns_ = INetAddr();
}

} // namespace NetManagerStandard
} // namespace OHOS
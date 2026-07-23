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

#include <memory>
#include <netdb.h>
#include <sys/socket.h>
#include <thread>
#include <pthread.h>
#include <arpa/inet.h>
#include "securec.h"
#include "net_dns_resolve.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *ADDR_SEPARATOR = ",";
constexpr int32_t DOMAIN_IP_ADDR_LEN_MAX = 128;
}

NetDnsResolve::NetDnsResolve(uint32_t netId,
    std::shared_ptr<TinyCountDownLatch>& latch, const std::string& domain)
    : netId_(netId), latch_(latch), domain_(domain)
{}

NetDnsResolve::~NetDnsResolve() {}

void NetDnsResolve::Start()
{
    std::weak_ptr<NetDnsResolve> wp = shared_from_this();
    std::thread t([wp]() {
        auto dnsResolveThread = wp.lock();
        if (dnsResolveThread != nullptr) {
            dnsResolveThread->StartDnsResolve();
        }
    });
    std::string threadName = "netDnsResolveThread";
    pthread_setname_np(t.native_handle(), threadName.c_str());
    t.detach();
}

void NetDnsResolve::StartDnsResolve()
{
    NETMGR_LOG_D("start dns resolve, netId:%{public}d", netId_);
    GetAddrInfo();
    if (latch_) {
        latch_->CountDown();
    }
}

void NetDnsResolve::GetAddrInfo()
{
    if (domain_.empty()) {
        NETMGR_LOG_E("domain is empty");
        return;
    }

    struct addrinfo *result = nullptr;
    struct queryparam qparam = {};
    qparam.qp_netid = static_cast<int>(netId_);
    qparam.qp_type = QEURY_TYPE_NETSYS;

    int32_t ret = getaddrinfo_ext(domain_.c_str(), nullptr, nullptr, &result, &qparam);
    if (ret < 0) {
        NETMGR_LOG_E("Get net[%{public}d] address info failed,errno[%{public}d]:%{public}s", netId_, errno,
                     strerror(errno));
        return;
    }

    std::string ipAddress;
    char ip[DOMAIN_IP_ADDR_LEN_MAX] = {0};
    for (addrinfo *tmp = result; tmp != nullptr; tmp = tmp->ai_next) {
        errno_t err = memset_s(&ip, sizeof(ip), 0, sizeof(ip));
        if (err != EOK) {
            NETMGR_LOG_E("memset_s failed,err:%{public}d", err);
            freeaddrinfo(result);
            return;
        }
        if (tmp->ai_family == AF_INET) {
            auto addr = reinterpret_cast<sockaddr_in *>(tmp->ai_addr);
            if (!inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip))) {
                continue;
            }
            if (resolveResultIpv4_.find(ip) != std::string::npos) {
                continue;
            }
            resolveResultIpv4_ = resolveResultIpv4_.empty() ?
                (resolveResultIpv4_ + ip) : (resolveResultIpv4_ + ADDR_SEPARATOR + ip);
        } else if (tmp->ai_family == AF_INET6) {
            auto addr = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
            if (!inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip))) {
                continue;
            }
            if (resolveResultIpv6_.find(ip) != std::string::npos) {
                continue;
            }
            resolveResultIpv6_ = resolveResultIpv6_.empty() ?
                (resolveResultIpv6_ + ip) : (resolveResultIpv6_ + ADDR_SEPARATOR + ip);
        }
    }

    freeaddrinfo(result);
}

std::string NetDnsResolve::GetDnsResolveResultByType(INetAddr::IpType ipType)
{
    if (ipType == INetAddr::IpType::IPV4) {
        return resolveResultIpv4_;
    } else if (ipType == INetAddr::IpType::IPV6) {
        return resolveResultIpv6_;
    } else {
        return resolveResultIpv4_ + ADDR_SEPARATOR + resolveResultIpv6_;
    }
}
} // namespace NetManagerStandard
} // namespace OHOS

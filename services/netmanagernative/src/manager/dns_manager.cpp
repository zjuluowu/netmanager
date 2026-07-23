/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <pthread.h>
#include <thread>
#include <charconv>

#include "dns_resolv_listen.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "singleton.h"
#include "dns_quality_diag.h"
#ifdef QOS_MANAGER_ENABLE
#include "qos.h"
#include "concurrent_task_client.h"
#include <sys/resource.h>
#endif

#include "dns_manager.h"
#include <netdb.h>

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard::CommonUtils;
constexpr const char *IPV6_DEFAULT_GATEWAY = "::";
void StartListen()
{
    NETNATIVE_LOG_D("Enter threadStart");
#ifdef QOS_MANAGER_ENABLE
    std::unordered_map<std::string, std::string> payload;
    payload["pid"] = std::to_string(getpid());
    OHOS::ConcurrentTask::ConcurrentTaskClient::GetInstance().RequestAuth(payload);
    if (SetThreadQos(QOS::QosLevel::QOS_USER_INITIATED) != 0) {
        setpriority(PRIO_PROCESS, 0, PRIO_MIN);
    }
    NETNATIVE_LOGI("DnsMgerListen set qos end");
#endif
    DnsResolvListen().StartListen();
}

DnsManager::DnsManager() : dnsProxyListen_(std::make_shared<DnsProxyListen>())
{
    std::thread t(StartListen);
    std::string threadName = "DnsMgerListen";
    pthread_setname_np(t.native_handle(), threadName.c_str());
    t.detach();
#ifdef FEATURE_NET_FIREWALL_ENABLE
    firewallDomainRulesQueue_ = std::make_shared<ffrt::queue>("firewallDomainRulesQueue");
#endif
}

void DnsManager::EnableIpv6(uint16_t netId, std::string &destination, const std::string &nextHop, bool enable)
{
    if (!enable) {
        NETNATIVE_LOGI("Disable ipv6 dns, netId=%{public}d", netId);
        DnsParamCache::GetInstance().EnableIpv6(netId, false);
        return;
    }

    auto pos = destination.find("/");
    if (pos == std::string::npos) {
        return;
    }
    std::string ip = destination.substr(0, pos);
    std::string prefixStr = destination.substr(pos + 1);
    int prefix = -1;
    auto result = std::from_chars(prefixStr.data(), prefixStr.data() + prefixStr.size(), prefix);
    if (result.ec != std::errc()) {
        return;
    }

    if ((IsValidIPV6(ip) && prefix == 0) && (IsValidIPV6(nextHop) || nextHop.empty())) {
        NETNATIVE_LOGI("Enable ipv6 dns, netId=%{public}d", netId);
        DnsParamCache::GetInstance().EnableIpv6(netId);
    }
}

void DnsManager::EnableIpv4(uint16_t netId, const std::string &destination, const std::string &nextHop)
{
    std::string ip = "";
    auto pos = destination.find("/");
    if (pos != std::string::npos) {
        ip = destination.substr(0, pos);
    } else {
        ip = destination;
        NETNATIVE_LOGI("EnableIpv4 npos");
    }
    if ((IsValidIPV4(ip)) && (IsValidIPV4(nextHop) || nextHop.empty())) {
        DnsParamCache::GetInstance().EnableIpv4(netId);
    }
}

void DnsManager::SetClatDnsEnableIpv4(int32_t netId, bool enable)
{
    DnsParamCache::GetInstance().SetClatDnsEnableIpv4(netId, enable);
}

int32_t DnsManager::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMillis, uint8_t retryCount,
                                      const std::vector<std::string> &servers, const std::vector<std::string> &domains)
{
    NETNATIVE_LOG_D("manager_SetResolverConfig netId[%{public}d]", netId);
    return DnsParamCache::GetInstance().SetResolverConfig(netId, baseTimeoutMillis, retryCount, servers, domains);
}

int32_t DnsManager::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                      std::vector<std::string> &domains, uint16_t &baseTimeoutMillis,
                                      uint8_t &retryCount)
{
    NETNATIVE_LOG_D("manager_GetResolverConfig netId[%{public}d]", netId);
    return DnsParamCache::GetInstance().GetResolverConfig(netId, servers, domains, baseTimeoutMillis, retryCount);
}

int32_t DnsManager::CreateNetworkCache(uint16_t netId, bool isVpnNet)
{
    NETNATIVE_LOG_D("manager_CreateNetworkCache netId[%{public}d]", netId);
    return DnsParamCache::GetInstance().CreateCacheForNet(netId, isVpnNet);
}

int32_t DnsManager::DestroyNetworkCache(uint16_t netId, bool isVpnNet)
{
    return DnsParamCache::GetInstance().DestroyNetworkCache(netId, isVpnNet);
}

void DnsManager::SetDefaultNetwork(uint16_t netId)
{
    DnsParamCache::GetInstance().SetDefaultNetwork(netId);
}

void DnsManager::ShareDnsSet(uint16_t netId)
{
    dnsProxyListen_->SetParseNetId(netId);
}

void DnsManager::StartDnsProxyListen()
{
    dnsProxyListen_->OnListen();
    std::shared_ptr<DnsProxyListen> proxy = dnsProxyListen_;
    std::thread t([proxy] () {
        NETNATIVE_LOG_D("begin StartProxyListen");
        proxy->StartListen();
    });
    std::string threadName = "DnsPxyListen";
    pthread_setname_np(t.native_handle(), threadName.c_str());
    t.detach();
}

void DnsManager::StopDnsProxyListen()
{
    dnsProxyListen_->OffListen();
}

void DnsManager::GetDumpInfo(std::string &info)
{
    NETNATIVE_LOG_D("Get dump info");
    DnsParamCache::GetInstance().GetDumpInfo(info);
}

int32_t DnsManager::GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                                uint16_t netId, std::vector<AddrInfo> &res)
{
    if (netId == 0) {
        netId = DnsParamCache::GetInstance().GetDefaultNetwork();
        NETNATIVE_LOG_D("DnsManager DnsGetaddrinfo netId == 0 defaultNetId_ : %{public}d", netId);
    }
    struct addrinfo hint = {};
    struct addrinfo *result;
    struct queryparam qparam = {};

    if ((hostName.size() == 0) && (serverName.size() == 0)) {
        return -1;
    }

    qparam.qp_netid = netId;
    qparam.qp_type = 1;

    hint.ai_family = hints.aiFamily;
    hint.ai_flags = hints.aiFlags;
    hint.ai_protocol = hints.aiProtocol;
    hint.ai_socktype = hints.aiSockType;
 
    int32_t ret = getaddrinfo_ext(((hostName.size() == 0) ? NULL : hostName.c_str()),
                                  ((serverName.size() == 0) ? NULL : serverName.c_str()),
                                  &hint, &result, &qparam);
    if (ret == 0) {
        ret = FillAddrInfo(res, result);
        freeaddrinfo(result);
    }

    return ret;
}

int32_t DnsManager::RegisterDnsResultCallback(const sptr<NetsysNative::INetDnsResultCallback> &callback,
                                              uint32_t timeStep)
{
    return DnsQualityDiag::GetInstance().RegisterResultListener(callback, timeStep);
}

int32_t DnsManager::UnregisterDnsResultCallback(const sptr<NetsysNative::INetDnsResultCallback> &callback)
{
    return DnsQualityDiag::GetInstance().UnregisterResultListener(callback);
}

int32_t DnsManager::AddUidRange(int32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges)
{
    NETNATIVE_LOG_D("DnsManager::AddUidRange");
    return DnsParamCache::GetInstance().AddUidRange(netId, uidRanges);
}

int32_t DnsManager::DelUidRange(int32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges)
{
    NETNATIVE_LOG_D("DnsManager::DelUidRange");
    return DnsParamCache::GetInstance().DelUidRange(netId, uidRanges);
}

int32_t DnsManager::FillAddrInfo(std::vector<AddrInfo> &addrInfo, addrinfo *res)
{
    int32_t resNum = 0;
    addrinfo *tmp = res;

    while (tmp) {
        AddrInfo info;
        info.aiFlags = static_cast<int32_t>(tmp->ai_flags);
        info.aiFamily = static_cast<int32_t>(tmp->ai_family);
        info.aiSockType = static_cast<int32_t>(tmp->ai_socktype);
        info.aiProtocol = static_cast<int32_t>(tmp->ai_protocol);
        info.aiAddrLen = tmp->ai_addrlen;
        if (memcpy_s(&info.aiAddr, sizeof(info.aiAddr), tmp->ai_addr, tmp->ai_addrlen) != 0) {
            NETNATIVE_LOGE("memcpy_s failed");
        }
        if (strcpy_s(info.aiCanonName, sizeof(info.aiCanonName), tmp->ai_canonname) != 0) {
            NETNATIVE_LOGE("strcpy_s failed");
        }

        ++resNum;
        addrInfo.emplace_back(info);
        tmp = tmp->ai_next;
        if (resNum >= MAX_RESULTS) {
            break;
        }
    }
    NETNATIVE_LOGI("FillAddrInfo %{public}d", resNum);
    return 0;
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
int32_t DnsManager::SetFirewallDefaultAction(FirewallRuleAction inDefault, FirewallRuleAction outDefault)
{
    return DnsParamCache::GetInstance().SetFirewallDefaultAction(inDefault, outDefault);
}

int32_t DnsManager::SetFirewallCurrentUserId(int32_t userId)
{
    return DnsParamCache::GetInstance().SetFirewallCurrentUserId(userId);
}

int32_t DnsManager::SetFirewallRules(NetFirewallRuleType type, const std::vector<sptr<NetFirewallBaseRule>> &ruleList,
                                     bool isFinish)
{
    int32_t ret = DnsParamCache::GetInstance().SetFirewallRules(type, ruleList, isFinish);
    if (ret != 0) {
        return ret;
    }
    EeffectDomainRules(type, ruleList, isFinish);
    return 0;
}

void DnsManager::EeffectDomainRules(NetFirewallRuleType type, const std::vector<sptr<NetFirewallBaseRule>> &ruleList,
                                    bool isFinish)
{
    if (type != NetFirewallRuleType::RULE_DOMAIN || firewallDomainRulesQueue_ == nullptr) {
        return;
    }
    std::weak_ptr<DnsManager> wp = shared_from_this();
    firewallDomainRulesQueue_->submit([wp, ruleList, isFinish]() {
        auto sharedSelf = wp.lock();
        if (sharedSelf == nullptr) {
            return;
        }
        for (auto &rule : ruleList) {
            auto domainRule = firewall_rule_cast<NetFirewallDomainRule>(rule);
            if (domainRule == nullptr || domainRule->ruleAction != FirewallRuleAction::RULE_ALLOW) {
                continue;
            }
            sharedSelf->firewallDomainRules_.emplace_back(domainRule);
        }
        if (!isFinish) {
            return;
        }
        for (auto &rule : sharedSelf->firewallDomainRules_) {
            for (auto &domain : rule->domains) {
                AddrInfo addrInfo = {};
                addrInfo.aiFamily = AF_UNSPEC;
                addrInfo.aiSockType = SOCK_STREAM;
                addrInfo.aiProtocol = IPPROTO_TCP;
                std::vector<AddrInfo> res;
                sharedSelf->GetAddrInfo(domain.domain, "", addrInfo, 0, res);
            }
        }
        sharedSelf->firewallDomainRules_.clear();
    });
}

int32_t DnsManager::ClearFirewallRules(NetFirewallRuleType type)
{
    return DnsParamCache::GetInstance().ClearFirewallRules(type);
}

int32_t DnsManager::RegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    return DnsParamCache::GetInstance().RegisterNetFirewallCallback(callback);
}
int32_t DnsManager::UnRegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    return DnsParamCache::GetInstance().UnRegisterNetFirewallCallback(callback);
}
#endif

int32_t DnsManager::SetUserDefinedServerFlag(uint16_t netId, bool flag)
{
    NETNATIVE_LOGI("manager_SetUserDefinedServerFlag netId[%{public}d] flag[%{public}d]", netId, flag);
    return DnsParamCache::GetInstance().SetUserDefinedServerFlag(netId, flag);
}

int32_t DnsManager::FlushDnsCache(uint16_t netId)
{
    NETNATIVE_LOGI("manager_FlushDnsCache netId[%{public}d]", netId);
    return DnsParamCache::GetInstance().FlushDnsCache(netId);
}

int32_t DnsManager::SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo)
{
    DnsParamCache::GetInstance().SetDnsCache(netId, hostName, addrInfo);
    return 0;
}

int32_t DnsManager::SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid)
{
    NETNATIVE_LOGI("manager_SetIpv6UidBlackList uid:%{public}d", uid);
    DnsParamCache::GetInstance().SetIpv6UidBlackList(netIds, static_cast<uint32_t>(uid));
    return 0;
}
} // namespace nmd
} // namespace OHOS

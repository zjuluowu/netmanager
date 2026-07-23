/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <algorithm>
#include "netmanager_base_common_utils.h"
#include "dns_param_cache.h"
#include "netnative_log_wrapper.h"

#ifdef FEATURE_NET_FIREWALL_ENABLE
#include "bpf_netfirewall.h"
#include "netfirewall_parcel.h"
#include <ctime>
#endif

namespace OHOS::nmd {
using namespace OHOS::NetManagerStandard::CommonUtils;
namespace {
void GetVectorData(const std::vector<std::string> &data, std::string &result)
{
    result.append("{ ");
    std::for_each(data.begin(), data.end(), [&result](const auto &str) { result.append(ToAnonymousIp(str) + ", "); });
    result.append("}\n");
}
constexpr int RES_TIMEOUT = 4000;    // min. milliseconds between retries
constexpr int RES_DEFAULT_RETRY = 2; // Default
} // namespace

DnsParamCache::DnsParamCache() : defaultNetId_(0) {}

DnsParamCache &DnsParamCache::GetInstance()
{
    static DnsParamCache instance;
    return instance;
}

std::vector<std::string> DnsParamCache::SelectNameservers(const std::vector<std::string> &servers)
{
    std::vector<std::string> res = servers;
    if (res.size() > MAX_SERVER_NUM_EXT - 1) {
        res.resize(MAX_SERVER_NUM_EXT - 1);
    }
    return res;
}

std::vector<std::string> DnsParamCache::RemoveDuplicateNameservers(const std::vector<std::string> &servers)
{
    std::set<std::string> seen;
    std::vector<std::string> res;
    for (const auto& server : servers) {
        if (seen.find(server) == seen.end()) {
            seen.insert(server);
            res.push_back(server);
        }
    }
    return res;
}

int32_t DnsParamCache::CreateCacheForNet(uint16_t netId, bool isVpnNet)
{
    NETNATIVE_LOGI("DnsParamCache::CreateCacheForNet, netid:%{public}d,", netId);
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it != serverConfigMap_.end()) {
        NETNATIVE_LOGE("DnsParamCache::CreateCacheForNet, netid already exist, no need to create");
        return -EEXIST;
    }
    serverConfigMap_[netId].SetNetId(netId);
    if (isVpnNet) {
        NETNATIVE_LOGI("DnsParamCache::CreateCacheForNet clear all dns cache when vpn net create");
        for (auto iterator = serverConfigMap_.begin(); iterator != serverConfigMap_.end(); iterator++) {
            iterator->second.GetCache().Clear();
            iterator->second.ClearNodataCache();
            iterator->second.ClearIpv6UidBlackList();
        }
    }
    return 0;
}

int32_t DnsParamCache::DestroyNetworkCache(uint16_t netId, bool isVpnNet)
{
    NETNATIVE_LOGI("DnsParamCache::DestroyNetworkCache, netid:%{public}d, %{public}d", netId, isVpnNet);
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        return -ENOENT;
    }
    serverConfigMap_.erase(it);
    if (defaultNetId_ == netId) {
        defaultNetId_ = 0;
    }
    if (isVpnNet) {
        NETNATIVE_LOGI("DnsParamCache::DestroyNetworkCache clear all dns cache when vpn net destroy");
        for (auto it = serverConfigMap_.begin(); it != serverConfigMap_.end(); it++) {
            it->second.GetCache().Clear();
            it->second.ClearNodataCache();
            it->second.ClearIpv6UidBlackList();
        }
    }
    return 0;
}

int32_t DnsParamCache::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                         const std::vector<std::string> &servers,
                                         const std::vector<std::string> &domains)
{
    std::vector<std::string> nameservers = SelectNameservers(servers);
    NETNATIVE_LOG_D("DnsParamCache::SetResolverConfig, netid:%{public}d, numServers:%{public}d,", netId,
                    static_cast<int>(nameservers.size()));

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);

    // select_domains
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        NETNATIVE_LOGE("DnsParamCache::SetResolverConfig failed, netid is non-existent");
        return -ENOENT;
    }

    auto uniqueServers = RemoveDuplicateNameservers(servers);
    auto oldDnsServers = it->second.GetServers();
    std::sort(oldDnsServers.begin(), oldDnsServers.end());

    auto newDnsServers = uniqueServers;
    std::sort(newDnsServers.begin(), newDnsServers.end());

    if (oldDnsServers != newDnsServers) {
        it->second.GetCache().Clear();
        it->second.ClearNodataCache();
        it->second.ClearIpv6UidBlackList();
    }

    it->second.SetNetId(netId);
    it->second.SetServers(uniqueServers);
    it->second.SetDomains(domains);
    if (retryCount == 0) {
        it->second.SetRetryCount(RES_DEFAULT_RETRY);
    } else {
        it->second.SetRetryCount(retryCount);
    }
    if (baseTimeoutMsec == 0) {
        it->second.SetTimeoutMsec(RES_TIMEOUT);
    } else {
        it->second.SetTimeoutMsec(baseTimeoutMsec);
    }
    return 0;
}

void DnsParamCache::SetDefaultNetwork(uint16_t netId)
{
    defaultNetId_ = netId;
}

void DnsParamCache::EnableIpv6(uint16_t netId, bool enable)
{
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("get Config failed: netid is not have netid:%{public}d,", netId);
        return;
    }

    it->second.EnableIpv6(enable);
}

bool DnsParamCache::IsIpv6Enable(uint16_t netId)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("get Config failed: netid is not have netid:%{public}d,", netId);
        return false;
    }

    return it->second.IsIpv6Enable();
}

void DnsParamCache::EnableIpv4(uint16_t netId)
{
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("EnableIpv4 netid:%{public}d,", netId);
        return;
    }

    it->second.EnableIpv4();
}

void DnsParamCache::SetClatDnsEnableIpv4(int32_t netId, bool enable)
{
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("SetClatDnsEnableIpv4 netid:%{public}d,", netId);
        return;
    }
    NETNATIVE_LOGI("DnsParamCache SetClatDnsEnableIpv4, enable =%{public}d", enable);
    it->second.SetClatDnsEnableIpv4(enable);
}

bool DnsParamCache::IsIpv4Enable(uint16_t netId)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("IsIpv4Enable netid:%{public}d,", netId);
        return false;
    }
    return it->second.IsIpv4Enable() || it->second.IsClatIpv4Enable();
}

int32_t DnsParamCache::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                         std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                         uint8_t &retryCount)
{
    NETNATIVE_LOG_D("DnsParamCache::GetResolverConfig no uid");
    if (netId == 0) {
        netId = defaultNetId_;
        NETNATIVE_LOG_D("defaultNetId_ = [%{public}u]", netId);
    }

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("get Config failed: netid is not have netid:%{public}d,", netId);
        return -ENOENT;
    }

    servers = it->second.GetServers();
#ifdef FEATURE_NET_FIREWALL_ENABLE
    std::vector<std::string> dns;
    if (GetDnsServersByAppUid(GetCallingUid(), dns)) {
        DNS_CONFIG_PRINT("GetResolverConfig hit netfirewall");
        servers.assign(dns.begin(), dns.end());
    }
#endif
    domains = it->second.GetDomains();
    baseTimeoutMsec = it->second.GetTimeoutMsec();
    retryCount = it->second.GetRetryCount();

    return 0;
}

int32_t DnsParamCache::GetVpnResolverConfig(uint32_t uid, std::vector<std::string> &servers,
                                            std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                            uint8_t &retryCount)
{
    std::lock_guard<ffrt::mutex> uidLock(uidRangeMutex_);
    if (vpnNetId_.empty()) {
        return -1;
    }
    for (auto mem : vpnUidRanges_) {
        if (static_cast<int64_t>(uid) >= mem.begin_ && static_cast<int64_t>(uid) <= mem.end_) {
            std::lock_guard<ffrt::mutex> lock(cacheMutex_);
            auto it = serverConfigMap_.find(mem.netId_);
            if (it == serverConfigMap_.end()) {
                NETNATIVE_LOGE("vpn get Config failed: not have vpnnetid:%{public}d,", mem.netId_);
                continue;
            }
            servers = it->second.GetServers();
#ifdef FEATURE_NET_FIREWALL_ENABLE
            std::vector<std::string> dns;
            if (GetDnsServersByAppUid(GetCallingUid(), dns)) {
                DNS_CONFIG_PRINT("GetResolverConfig hit netfirewall");
                servers.assign(dns.begin(), dns.end());
            }
#endif
// LCOV_EXCL_START
            if (servers.empty()) {
                DNS_CONFIG_PRINT("GetVpnResolverConfig failed, servers is empty");
                return -1;
            }
// LCOV_EXCL_STOP
            domains = it->second.GetDomains();
            baseTimeoutMsec = it->second.GetTimeoutMsec();
            retryCount = it->second.GetRetryCount();
            return 0;
        }
    }
    return -1;
}

int32_t DnsParamCache::GetResolverConfig(uint16_t netId, uint32_t uid, std::vector<std::string> &servers,
                                         std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                         uint8_t &retryCount)
{
    NETNATIVE_LOG_D("DnsParamCache::GetResolverConfig has uid");
    if (netId == 0) {
        netId = defaultNetId_;
        NETNATIVE_LOG_D("defaultNetId_ = [%{public}u]", netId);
    }

    if (GetVpnResolverConfig(uid, servers, domains, baseTimeoutMsec, retryCount) != 0) {
        return GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
    }
    return 0;
}

int32_t DnsParamCache::GetDefaultNetwork() const
{
    return defaultNetId_;
}

void DnsParamCache::SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("SetDnsCache failed: netid is not have netid:%{public}d,", netId);
        return;
    }

    // LCOV_EXCL_START
    AddrInfoWithTtl addrInfoWithTtl;
    if (memcpy_s(&addrInfoWithTtl.addrInfo, sizeof(addrInfoWithTtl.addrInfo), &addrInfo,
                 sizeof(addrInfo)) != 0) {
        return;
    }
    // LCOV_EXCL_STOP
    addrInfoWithTtl.ttl = DEFAULT_DELAYED_COUNT;
    it->second.GetCache().Put(hostName, addrInfoWithTtl);
}

void DnsParamCache::SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfoWithTtl &addrInfo)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }
    uint32_t ttl = addrInfo.ttl;
    if (ttl == 0) {
        return;
    }
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("SetDnsCache failed: netid is not have netid:%{public}d,", netId);
        return;
    }

    // LCOV_EXCL_START
    AddrInfoWithTtl addrInfoWithTtl;
    if (memcpy_s(&addrInfoWithTtl.addrInfo, sizeof(addrInfoWithTtl.addrInfo), &addrInfo.addrInfo,
                 sizeof(addrInfo.addrInfo)) != 0) {
        return;
    }
    // LCOV_EXCL_STOP
    addrInfoWithTtl.ttl = ttl > DEFAULT_DELAYED_COUNT ? ttl : DEFAULT_DELAYED_COUNT;
    it->second.GetCache().Put(hostName, addrInfoWithTtl);
}

std::vector<AddrInfo> DnsParamCache::GetDnsCache(uint16_t netId, const std::string &hostName)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("GetDnsCache failed: netid is not have netid:%{public}d,", netId);
        return {};
    }

    auto infos = it->second.GetCache().Get(hostName);
    std::vector<AddrInfo> addrInfo;
    for (auto info : infos) {
        addrInfo.push_back(info.addrInfo);
    }
    return addrInfo;
}

void DnsParamCache::SetCacheDelayed(uint16_t netId, const std::string &hostName)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("SetCacheDelayed failed: netid is not have netid:%{public}d,", netId);
        return;
    }

    it->second.SetCacheDelayed(hostName);
}

int32_t DnsParamCache::AddUidRange(uint32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges)
{
    std::lock_guard<ffrt::mutex> guard(uidRangeMutex_);
    NETNATIVE_LOG_D("DnsParamCache::AddUidRange size = [%{public}zu]", uidRanges.size());
    if (std::find(vpnNetId_.begin(), vpnNetId_.end(), netId) == vpnNetId_.end()) {
        vpnNetId_.push_back(netId);
    }
    for (auto mem : uidRanges) {
        NETNATIVE_LOG_D(
            "GetResolverConfig AddUidRange begin %{public}d end %{public}d netId %{public}d priority %{public}d",
            mem.begin_, mem.end_, mem.netId_, mem.priorityId_);
    }
    vpnUidRanges_.erase(std::remove_if(vpnUidRanges_.begin(), vpnUidRanges_.end(),
                                       [netId](const NetManagerStandard::UidRange &range) {
                                           return range.netId_ == netId;
                                       }),
                        vpnUidRanges_.end());
    if (!uidRanges.empty()) {
        auto middle = vpnUidRanges_.insert(vpnUidRanges_.end(), uidRanges.begin(), uidRanges.end());
        std::inplace_merge(vpnUidRanges_.begin(), middle, vpnUidRanges_.end());
    }
    return 0;
}

int32_t DnsParamCache::DelUidRange(uint32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges)
{
    std::lock_guard<ffrt::mutex> guard(uidRangeMutex_);
    NETNATIVE_LOG_D("DnsParamCache::DelUidRange size = [%{public}zu]", uidRanges.size());
    auto it = std::find(vpnNetId_.begin(), vpnNetId_.end(), netId);
    if (it != vpnNetId_.end()) {
        vpnNetId_.erase(it);
    }
    auto end = std::set_difference(vpnUidRanges_.begin(), vpnUidRanges_.end(), uidRanges.begin(),
                                   uidRanges.end(), vpnUidRanges_.begin());
    vpnUidRanges_.erase(end, vpnUidRanges_.end());
    return 0;
}

bool DnsParamCache::IsVpnOpen() const
{
    return vpnUidRanges_.size();
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
int32_t DnsParamCache::GetUserId(int32_t appUid)
{
    int32_t userId = appUid / USER_ID_DIVIDOR;
    return userId > 0 ? userId : currentUserId_;
}

bool DnsParamCache::GetDnsServersByAppUid(int32_t appUid, std::vector<std::string> &servers)
{
    if (netFirewallDnsRuleMap_.empty()) {
        return false;
    }
    DNS_CONFIG_PRINT("GetDnsServersByAppUid: appUid=%{public}d", appUid);
    auto it = netFirewallDnsRuleMap_.find(appUid);
    if (it == netFirewallDnsRuleMap_.end()) {
        // if appUid not found, try to find invalid appUid=0;
        it = netFirewallDnsRuleMap_.find(0);
    }
    if (it != netFirewallDnsRuleMap_.end()) {
        int32_t userId = GetUserId(appUid);
        std::vector<sptr<NetFirewallDnsRule>> rules = it->second;
        for (const auto &rule : rules) {
            if (rule->userId != userId) {
                continue;
            }
            servers.emplace_back(rule->primaryDns);
            servers.emplace_back(rule->standbyDns);
        }
        return true;
    }
    return false;
}

int32_t DnsParamCache::SetFirewallRules(NetFirewallRuleType type,
                                        const std::vector<sptr<NetFirewallBaseRule>> &ruleList, bool isFinish)
{
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    NETNATIVE_LOGI("SetFirewallRules: size=%{public}zu isFinish=%{public}" PRId32, ruleList.size(), isFinish);
    if (ruleList.empty()) {
        NETNATIVE_LOGE("SetFirewallRules: rules is empty");
        return -1;
    }
    int32_t ret = 0;
    switch (type) {
        case NetFirewallRuleType::RULE_DNS: {
            for (const auto &rule : ruleList) {
                firewallDnsRules_.emplace_back(firewall_rule_cast<NetFirewallDnsRule>(rule));
            }
            if (isFinish) {
                ret = SetFirewallDnsRules(firewallDnsRules_);
                firewallDnsRules_.clear();
            }
            break;
        }
        case NetFirewallRuleType::RULE_DOMAIN: {
            ClearAllDnsCache();
            break;
        }
        default:
            break;
    }
    return ret;
}

int32_t DnsParamCache::SetFirewallDnsRules(const std::vector<sptr<NetFirewallDnsRule>> &ruleList)
{
    for (const auto &rule : ruleList) {
        std::vector<sptr<NetFirewallDnsRule>> rules;
        auto it = netFirewallDnsRuleMap_.find(rule->appUid);
        if (it != netFirewallDnsRuleMap_.end()) {
            rules = it->second;
        }
        rules.emplace_back(rule);
        netFirewallDnsRuleMap_.emplace(rule->appUid, std::move(rules));
    }
    return 0;
}

FirewallRuleAction DnsParamCache::GetFirewallRuleAction(int32_t appUid,
                                                        const std::vector<sptr<NetFirewallDomainRule>> &rules)
{
    int32_t userId = GetUserId(appUid);
    for (const auto &rule : rules) {
        if (rule->userId != userId) {
            continue;
        }
        if ((rule->appUid && appUid == rule->appUid) || !rule->appUid) {
            return rule->ruleAction;
        }
    }

    return FirewallRuleAction::RULE_INVALID;
}

int32_t DnsParamCache::SetFirewallDefaultAction(FirewallRuleAction inDefault, FirewallRuleAction outDefault)
{
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    DNS_CONFIG_PRINT("SetFirewallDefaultAction: firewallDefaultAction_: %{public}d", (int)outDefault);
    firewallDefaultAction_ = outDefault;
    return 0;
}

int32_t DnsParamCache::ClearFirewallRules(NetFirewallRuleType type)
{
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    switch (type) {
        case NetFirewallRuleType::RULE_DNS:
            firewallDnsRules_.clear();
            netFirewallDnsRuleMap_.clear();
            break;
        case NetFirewallRuleType::RULE_DOMAIN: {
            OHOS::NetManagerStandard::NetsysBpfNetFirewall::GetInstance()->ClearDomainCache();
            break;
        }
        case NetFirewallRuleType::RULE_ALL: {
            firewallDnsRules_.clear();
            netFirewallDnsRuleMap_.clear();
            OHOS::NetManagerStandard::NetsysBpfNetFirewall::GetInstance()->ClearDomainCache();
            break;
        }
        default:
            break;
    }
    return 0;
}

int32_t DnsParamCache::RegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    if (!callback) {
        return -1;
    }

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    callbacks_.emplace_back(callback);

    return 0;
}

int32_t DnsParamCache::UnRegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    if (!callback) {
        return -1;
    }

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it) {
        if (*it == callback) {
            callbacks_.erase(it);
            return 0;
        }
    }
    return -1;
}

int32_t DnsParamCache::SetFirewallCurrentUserId(int32_t userId)
{
    currentUserId_ = userId;
    ClearAllDnsCache();
    return 0;
}

void DnsParamCache::ClearAllDnsCache()
{
    NETNATIVE_LOGI("ClearAllDnsCache");
    for (auto it = serverConfigMap_.begin(); it != serverConfigMap_.end(); it++) {
        it->second.GetCache().Clear();
        it->second.ClearNodataCache();
        it->second.ClearIpv6UidBlackList();
    }
}
#endif

void DnsParamCache::GetDumpInfo(std::string &info)
{
    std::string dnsData;
    static const std::string TAB = "  ";
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    std::for_each(serverConfigMap_.begin(), serverConfigMap_.end(), [&dnsData](const auto &serverConfig) {
        dnsData.append(TAB + "NetId: " + std::to_string(serverConfig.second.GetNetId()) + "\n");
        dnsData.append(TAB + "TimeoutMsec: " + std::to_string(serverConfig.second.GetTimeoutMsec()) + "\n");
        dnsData.append(TAB + "RetryCount: " + std::to_string(serverConfig.second.GetRetryCount()) + "\n");
        dnsData.append(TAB + "Servers:");
        GetVectorData(serverConfig.second.GetServers(), dnsData);
        dnsData.append(TAB + "Domains:");
        GetVectorData(serverConfig.second.GetDomains(), dnsData);
    });
    info.append(dnsData);
}

int32_t DnsParamCache::SetUserDefinedServerFlag(uint16_t netId, bool flag)
{
    NETNATIVE_LOGI("DnsParamCache::SetUserDefinedServerFlag, netid:%{public}d, flag:%{public}d,", netId, flag);

    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    // select_domains
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        NETNATIVE_LOGE("DnsParamCache::SetUserDefinedServerFlag failed, netid is non-existent");
        return -ENOENT;
    }
    it->second.SetUserDefinedServerFlag(flag);
    return 0;
}

int32_t DnsParamCache::GetUserDefinedServerFlag(uint16_t netId, bool &flag)
{
    if (netId == 0) {
        netId = defaultNetId_;
        NETNATIVE_LOG_D("defaultNetId_ = [%{public}u]", netId);
    }
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("GetUserDefinedServerFlag failed: netid is not have netid:%{public}d,", netId);
        return -ENOENT;
    }
    flag = it->second.IsUserDefinedServer();
    return 0;
}

int32_t DnsParamCache::GetUserDefinedVpnServerFlag(uint32_t uid, bool &flag)
{
    std::lock_guard<ffrt::mutex> uidLock(uidRangeMutex_);
    if (vpnNetId_.empty()) {
        return -1;
    }

    for (auto mem : vpnUidRanges_) {
        if (static_cast<int64_t>(uid) >= mem.begin_ && static_cast<int64_t>(uid) <= mem.end_) {
            NETNATIVE_LOG_D("is vpn hap");
            std::lock_guard<ffrt::mutex> lock(cacheMutex_);
            auto it = serverConfigMap_.find(mem.netId_);
            if (it == serverConfigMap_.end()) {
                NETNATIVE_LOG_D("vpn get Config failed: not have vpnnetid:%{public}d,", mem.netId_);
                continue;
            }
            flag = it->second.IsUserDefinedServer();
            return 0;
        }
    }
    return -1;
}

int32_t DnsParamCache::GetUserDefinedServerFlag(uint16_t netId, bool &flag, uint32_t uid)
{
    if (netId == 0) {
        netId = defaultNetId_;
        NETNATIVE_LOG_D("defaultNetId_ = [%{public}u]", netId);
    }

    if (GetUserDefinedVpnServerFlag(uid, flag) != 0) {
        return GetUserDefinedServerFlag(netId, flag);
    }

    return 0;
}

bool DnsParamCache::IsUseVpnDns(uint32_t uid)
{
    std::lock_guard<ffrt::mutex> uidLock(uidRangeMutex_);
    for (auto mem : vpnUidRanges_) {
        if (static_cast<int64_t>(uid) >= mem.begin_ && static_cast<int64_t>(uid) <= mem.end_) {
            auto it = serverConfigMap_.find(mem.netId_);
            if (it == serverConfigMap_.end()) {
                continue;
            }
            return true;
        }
    }
    return false;
}

int32_t DnsParamCache::FlushDnsCache(uint16_t netId)
{
    if (netId == 0) {
        netId = defaultNetId_;
        NETNATIVE_LOG_D("defaultNetId_ = [%{public}u]", netId);
    }
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("FlushDnsCache failed: netid is non-existent netid:%{public}d,", netId);
        return -ENOENT;
    }
    it->second.GetCache().Clear();
    it->second.ClearNodataCache();
    it->second.ClearIpv6UidBlackList();
    return 0;
}

void DnsParamCache::SetNodataCache(uint16_t netId, const std::string &hostName)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("SetNodataCache failed: netid is not have netid:%{public}d,", netId);
        return;
    }
    NETNATIVE_LOGI("SetNodataCache netid:%{public}d", netId);
    it->second.SetNodataCache(hostName);
}

bool DnsParamCache::IsInNodataCache(uint16_t netId, const std::string &hostName)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }
    std::lock_guard<ffrt::mutex> guard(cacheMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("IsInNodataCache failed: netid is not have netid:%{public}d,", netId);
        return false;
    }
    bool enable = it->second.IsInNodataCache(hostName);
    if (enable) {
        NETNATIVE_LOGI("IsInNodataCache netid:%{public}d", netId);
    }
    return enable;
}

void DnsParamCache::SetIpv6UidBlackList(std::vector<int32_t> &netIds, uint32_t uid)
{
    size_t size = netIds.size();
    if (size <= 0) {
        return;
    }
 
    std::scoped_lock<ffrt::shared_mutex> writeLock(uidBlackListMutex_);
    for (auto& netId : netIds) {
        if (netId == 0) {
            netId = static_cast<int32_t>(defaultNetId_);
        }
 
        auto it = serverConfigMap_.find(netId);
        if (it == serverConfigMap_.end()) {
            DNS_CONFIG_PRINT("SetIpv6UidBlackList failed: is not have netid:%{public}d,", netId);
            continue;
        }
        NETNATIVE_LOGI("SetIpv6UidBlackList netid:%{public}d, uid:%{public}d", netId, uid);
        it->second.SetIpv6UidBlackList(uid);
    }
}
 
bool DnsParamCache::IsInIpv6UidBlackList(uint16_t netId, uint32_t uid)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }
 
    std::scoped_lock<ffrt::shared_mutex> readLock(uidBlackListMutex_);
    auto it = serverConfigMap_.find(netId);
    if (it == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("IsInIpv6UidBlackList failed: is not have netid:%{public}d,", netId);
        return false;
    }
    bool enable = it->second.IsInIpv6UidBlackList(uid);
    if (enable) {
        NETNATIVE_LOGI("IsInIpv6UidBlackList netid:%{public}d, uid:%{public}d", netId, uid);
    }
    return enable;
}
} // namespace OHOS::nmd

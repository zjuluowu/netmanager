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

#ifndef INCLUDE_MANAGER_DNS_MANAGER_H
#define INCLUDE_MANAGER_DNS_MANAGER_H

#include <vector>

#include "dns_param_cache.h"
#include "dns_proxy_listen.h"
#include "i_net_dns_result_callback.h"
#include "uid_range.h"
#ifdef FEATURE_NET_FIREWALL_ENABLE
#include "ffrt.h"
#endif

namespace OHOS {
namespace nmd {
class DnsManager : public std::enable_shared_from_this<DnsManager> {
public:
    DnsManager();
    ~DnsManager() = default;

    /**
     * Set the Resolver Config object
     *
     * @param netId network ID
     * @param baseTimeoutMillis base Timeout Ms, default 5000
     * @param retryCount retry Count, default 2
     * @param servers server name set in config
     * @param domains domain set in config
     * @return int32_t 0:success -1:failed
     */
    int32_t SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMillis, uint8_t retryCount,
                              const std::vector<std::string> &servers, const std::vector<std::string> &domains);

    /**
     * Get the Resolver Config object
     *
     * @param netId network ID
     * @param servers return value server name
     * @param domains return value doamin
     * @param baseTimeoutMillis return value Timeout Ms
     * @param retryCount return value retry Count
     * @return int32_t 0:success -1:failed
     */
    int32_t GetResolverConfig(uint16_t netId, std::vector<std::string> &servers, std::vector<std::string> &domains,
                              uint16_t &baseTimeoutMillis, uint8_t &retryCount);

    /**
     * Create a Network Cache object
     *
     * @param netId network ID
     * @return int32_t 0:success -1:failed
     */
    int32_t CreateNetworkCache(uint16_t netId, bool isVpnNet = false);

    /**
     * Set the Default Network object
     *
     * @param netId network ID
     */
    void SetDefaultNetwork(uint16_t netId);

    /**
     * Network share set netId
     *
     * @param netId network ID
     */
    void ShareDnsSet(uint16_t netId);

    /**
     * Start Dns proxy for network share
     *
     */
    void StartDnsProxyListen();

    /**
     * Stop Dns proxy for network share
     *
     */
    void StopDnsProxyListen();

    /**
     * Get the Dump Info object, this is for dump.
     *
     * @param info Infos for dump
     */
    void GetDumpInfo(std::string &info);

    /**
     * dns resolution object
     *
     * @param node hostname
     * @param service service name
     * @param hints limit
     * @param result return value
     * @param netId network id
     * @return int32_t  0 is success -1 is failed
     */
    int32_t GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                        uint16_t netId, std::vector<AddrInfo> &res);

    /**
     * destroy this netid's cache
     * @param netId network's id
     * @return destroy is success? 0 : -1
     */
    int32_t DestroyNetworkCache(uint16_t netId, bool isVpnNet = false);

#ifdef FEATURE_NET_FIREWALL_ENABLE
    /**
     * Set firewall default action
     *
     * @param inDefault Default action of NetFirewallRuleDirection:RULE_IN
     * @param outDefault Default action of NetFirewallRuleDirection:RULE_OUT
     * @return 0 if success or-1 if an error occurred
     */
    int32_t SetFirewallDefaultAction(FirewallRuleAction inDefault, FirewallRuleAction outDefault);

    /**
     * Set firewall current user id
     *
     * @param userId current user id
     * @return 0 if success or -1 if an error occurred
     */
    int32_t SetFirewallCurrentUserId(int32_t userId);

    /**
     * Set firewall rules to native
     *
     * @param type ip, dns, domain
     * @param ruleList list of NetFirewallIpRule
     * @param isFinish transmit finish or not
     * @return 0 if success or -1 if an error occurred
     */
    int32_t SetFirewallRules(NetFirewallRuleType type, const std::vector<sptr<NetFirewallBaseRule>> &ruleList,
                             bool isFinish);

    /**
     * Clear the Firewall rules
     *
     * @return 0 if success or-1 if an error occurred
     */
    int32_t ClearFirewallRules(NetFirewallRuleType type);

    /**
     * Register callback for recevie intercept event
     *
     * @param callback implement of INetFirewallCallback
     * @return 0 if success or -1 if an error occurred
     */
    int32_t RegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback);

    /**
     * Unregister callback for recevie intercept event
     *
     * @param callback register callback for recevie intercept event
     * @return 0 if success or -1 if an error occurred
     */
    int32_t UnRegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback);
#endif

    void EnableIpv6(uint16_t netId, std::string &destination, const std::string &nextHop, bool enable = true);
    void EnableIpv4(uint16_t netId, const std::string &destination, const std::string &nextHop);

    int32_t RegisterDnsResultCallback(const sptr<NetsysNative::INetDnsResultCallback> &callback, uint32_t timeStep);
    int32_t UnregisterDnsResultCallback(const sptr<NetsysNative::INetDnsResultCallback> &callback);

    int32_t AddUidRange(int32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges);
    int32_t DelUidRange(int32_t netId, const std::vector<NetManagerStandard::UidRange> &uidRanges);
    int32_t SetUserDefinedServerFlag(uint16_t netId, bool flag);
    int32_t FlushDnsCache(uint16_t netId);
    int32_t SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo);
    void SetClatDnsEnableIpv4(int32_t netId, bool enable);
    int32_t SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid);
private:
    std::shared_ptr<DnsProxyListen> dnsProxyListen_;
    int32_t FillAddrInfo(std::vector<AddrInfo> &addrInfo, addrinfo *res);
#ifdef FEATURE_NET_FIREWALL_ENABLE
    void EeffectDomainRules(NetFirewallRuleType type, const std::vector<sptr<NetFirewallBaseRule>> &ruleList,
        bool isFinish);
    std::vector<sptr<NetManagerStandard::NetFirewallDomainRule>> firewallDomainRules_;
    std::shared_ptr<ffrt::queue> firewallDomainRulesQueue_ = nullptr;
#endif
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_MANAGER_DNS_MANAGER_H

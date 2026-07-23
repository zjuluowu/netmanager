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

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "dns_manager.h"
#include "dns_config_client.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
std::string INTERFACE_NAME = "interface_name";
std::string INFO = "info";
const uint16_t NET_ID = 2;
uint16_t BASE_TIMEOUT_MILLIS = 2000;
uint8_t RETRY_COUNT = 3;
} // namespace

class DnsManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DnsManagerTest::SetUpTestCase() {}

void DnsManagerTest::TearDownTestCase() {}

void DnsManagerTest::SetUp() {}

void DnsManagerTest::TearDown() {}

HWTEST_F(DnsManagerTest, InterfaceTest001, TestSize.Level1)
{
    DnsManager dnsManager;
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    int32_t ret = dnsManager.SetResolverConfig(NET_ID, BASE_TIMEOUT_MILLIS, RETRY_COUNT, servers, domains);
    EXPECT_NE(ret, 0);

    ret = dnsManager.GetResolverConfig(NET_ID, servers, domains, BASE_TIMEOUT_MILLIS, RETRY_COUNT);
    EXPECT_NE(ret, 0);

    ret = dnsManager.CreateNetworkCache(NET_ID);
    EXPECT_EQ(ret, 0);

    dnsManager.SetDefaultNetwork(NET_ID);

    dnsManager.ShareDnsSet(NET_ID);

    dnsManager.StartDnsProxyListen();

    dnsManager.StopDnsProxyListen();

    dnsManager.GetDumpInfo(INFO);

    ret = dnsManager.DestroyNetworkCache(NET_ID);
    EXPECT_EQ(ret, 0);

    std::string hostName = "www.baidu.com";
    std::string serverName;
    AddrInfo hints;
    std::vector<AddrInfo> res;
    dnsManager.GetAddrInfo(hostName, serverName, hints, NET_ID, res);
}

HWTEST_F(DnsManagerTest, EnableIpv6Test001, TestSize.Level1)
{
    DnsManager dnsManager;
    uint16_t netId = 123;
    std::string destination = "123";
    std::string nextHop = "";
    dnsManager.EnableIpv6(netId, destination, nextHop);
    EXPECT_EQ(destination, "123");
    destination = "2001:db8:85a3::8a2e:370:7334";
    dnsManager.EnableIpv6(netId, destination, nextHop);
    nextHop = "2001:db8:85a3::8a2e:370:7335";
    dnsManager.EnableIpv6(netId, destination, nextHop);
    destination = "123";
    dnsManager.EnableIpv6(netId, destination, nextHop);
    destination = "2001:db8:85a3::8a2e:370:7334/64";
    dnsManager.EnableIpv6(netId, destination, nextHop);
    destination = "::/0";
    dnsManager.EnableIpv6(netId, destination, nextHop);
    EXPECT_FALSE(DnsParamCache::GetInstance().IsIpv6Enable(netId));
}

HWTEST_F(DnsManagerTest, EnableIpv6Test002, TestSize.Level1)
{
    DnsManager dnsManager;
    uint16_t netId = 124;
    std::string destination = "::/0";
    std::string nextHop = "fe80::1";

    dnsManager.CreateNetworkCache(netId);
    dnsManager.EnableIpv6(netId, destination, nextHop, true);
    EXPECT_TRUE(DnsParamCache::GetInstance().IsIpv6Enable(netId));

    dnsManager.EnableIpv6(netId, destination, nextHop, false);
    EXPECT_FALSE(DnsParamCache::GetInstance().IsIpv6Enable(netId));

    dnsManager.DestroyNetworkCache(netId);
}

HWTEST_F(DnsManagerTest, EnableIpv6Test003, TestSize.Level1)
{
    DnsManager dnsManager;
    uint16_t netId = 125;
    std::string destination = "";
    std::string nextHop = "";

    dnsManager.CreateNetworkCache(netId);
    dnsManager.EnableIpv6(netId, destination, nextHop, false);
    EXPECT_FALSE(DnsParamCache::GetInstance().IsIpv6Enable(netId));

    dnsManager.DestroyNetworkCache(netId);
}

HWTEST_F(DnsManagerTest, EnableIpv4Test001, TestSize.Level1)
{
    DnsManager dnsManager;
    uint16_t netId = 123;
    std::string destination = "123";
    std::string nextHop = "";
    dnsManager.CreateNetworkCache(netId);
    dnsManager.EnableIpv4(netId, destination, nextHop);
    EXPECT_EQ(destination, "123");
    destination = "192.100.100.1/ab";
    dnsManager.EnableIpv4(netId, destination, nextHop);
    nextHop = "192.100.100.1/0";
    dnsManager.EnableIpv4(netId, destination, nextHop);
    destination = "192.100.100.1";
    dnsManager.EnableIpv4(netId, destination, nextHop);
    destination = "0.0.0.0";
    dnsManager.EnableIpv4(netId, destination, nextHop);
    EXPECT_TRUE(DnsParamCache::GetInstance().IsIpv4Enable(netId));
    dnsManager.SetClatDnsEnableIpv4(netId, true);
    dnsManager.DestroyNetworkCache(netId);
}

HWTEST_F(DnsManagerTest, GetAddrInfoTest001, TestSize.Level1)
{
    DnsManager dnsManager;
    std::string hostName;
    std::string serverName = "www.baidu.com";
    AddrInfo hints;
    std::vector<AddrInfo> res;
    auto result = dnsManager.GetAddrInfo(hostName, serverName, hints, NET_ID, res);
    EXPECT_NE(result, -1);
}

HWTEST_F(DnsManagerTest, FillAddrInfoTest001, TestSize.Level1)
{
    DnsManager dnsManager;
    std::vector<AddrInfo> addrInfo;
    addrinfo *res = nullptr;
    auto result = dnsManager.FillAddrInfo(addrInfo, res);
    EXPECT_EQ(result, 0);
}

HWTEST_F(DnsManagerTest, SetDnsCacheTest001, TestSize.Level1)
{
    DnsManager dnsManager;
    uint16_t netId = 101;
    std::string testHost = "test";
    AddrInfo info;
    auto result = dnsManager.SetDnsCache(netId, testHost, info);
    EXPECT_EQ(result, 0);
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
HWTEST_F(DnsManagerTest, SetFirewallRules001, TestSize.Level1)
{
    DnsManager dnsManager;
    std::vector<sptr<NetFirewallBaseRule>> ruleList;
    int32_t ret = dnsManager.SetFirewallRules(NetFirewallRuleType::RULE_IP, ruleList, false);
    EXPECT_EQ(ret, -1);
    ruleList.emplace_back(nullptr);
    ret = dnsManager.SetFirewallRules(NetFirewallRuleType::RULE_IP, ruleList, false);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(dnsManager.firewallDomainRules_.empty());
}

HWTEST_F(DnsManagerTest, SetFirewallRules002, TestSize.Level1)
{
    DnsManager dnsManager;
    std::vector<sptr<NetFirewallBaseRule>> ruleList;
    ruleList.emplace_back(nullptr);
    auto domainRule = sptr<NetFirewallDomainRule>::MakeSptr();
    domainRule->ruleAction = FirewallRuleAction::RULE_DENY;
    ruleList.emplace_back(domainRule);
    int32_t ret = dnsManager.SetFirewallRules(NetFirewallRuleType::RULE_DOMAIN, ruleList, false);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(dnsManager.firewallDomainRules_.empty());
    ret = dnsManager.SetFirewallRules(NetFirewallRuleType::RULE_DOMAIN, ruleList, true);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(dnsManager.firewallDomainRules_.empty());
}

HWTEST_F(DnsManagerTest, SetFirewallRules003, TestSize.Level1)
{
    DnsManager dnsManager;
    std::vector<sptr<NetFirewallBaseRule>> ruleList;
    auto domainRule = sptr<NetFirewallDomainRule>::MakeSptr();
    domainRule->ruleAction = FirewallRuleAction::RULE_ALLOW;
    ruleList.emplace_back(domainRule);
    int32_t ret = dnsManager.SetFirewallRules(NetFirewallRuleType::RULE_DOMAIN, ruleList, false);
    EXPECT_EQ(ret, 0);
    EXPECT_FALSE(dnsManager.firewallDomainRules_.empty());
    ret = dnsManager.SetFirewallRules(NetFirewallRuleType::RULE_DOMAIN, ruleList, true);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(dnsManager.firewallDomainRules_.empty());
    NetFirewallDomainParam domains;
    domainRule->domains.emplace_back(domains);
    ret = dnsManager.SetFirewallRules(NetFirewallRuleType::RULE_DOMAIN, ruleList, true);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(dnsManager.firewallDomainRules_.empty());
}

HWTEST_F(DnsManagerTest, EeffectDomainRules001, TestSize.Level1)
{
    DnsManager dnsManager;
    std::vector<sptr<NetFirewallBaseRule>> ruleList;
    auto domainRule = sptr<NetFirewallDomainRule>::MakeSptr();
    domainRule->ruleAction = FirewallRuleAction::RULE_ALLOW;
    ruleList.emplace_back(domainRule);
    dnsManager.EeffectDomainRules(NetFirewallRuleType::RULE_DOMAIN, ruleList, false);
    EXPECT_FALSE(dnsManager.firewallDomainRules_.empty());
}

#endif
} // namespace nmd
} // namespace OHOS

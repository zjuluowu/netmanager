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

#include "dns_config_client.h"
#include "dns_param_cache.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace OHOS::nmd;
class DNSParamCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DNSParamCacheTest::SetUpTestCase() {}

void DNSParamCacheTest::TearDownTestCase() {}

void DNSParamCacheTest::SetUp() {}

void DNSParamCacheTest::TearDown() {}

HWTEST_F(DNSParamCacheTest, SetResolverConfigTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("SetResolverConfigTest001 enter");
    DnsParamCache dnsParCache;
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    servers.resize(MAX_SERVER_NUM + 1);
    uint16_t netId = 1;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    int32_t ret = dnsParCache.SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
    EXPECT_EQ(ret, -ENOENT);
}

HWTEST_F(DNSParamCacheTest, SetResolverConfigTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("SetResolverConfigTest002 enter");
    DnsParamCache dnsParCache;
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    servers.resize(MAX_SERVER_NUM + 1);
    std::string hostName = "hoseName";
    AddrInfo addrInfo;
    for (size_t i = 0; i < MAX_SERVER_NUM; i++) {
        dnsParCache.CreateCacheForNet(i + 1);
        dnsParCache.SetDnsCache(i, hostName.append(std::to_string(i)), addrInfo);
        servers.emplace_back(hostName);
    }
    uint16_t netId = 1;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    int32_t ret = dnsParCache.SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, SetResolverConfigTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("SetResolverConfigTest003 enter");
    DnsParamCache dnsParCache;
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    servers.resize(MAX_SERVER_NUM + 1);
    uint16_t netId = 1;
    std::string hostName = "hoseName";
    dnsParCache.GetDnsCache(netId, hostName);
    AddrInfo addrInfo;
    addrInfo.aiFlags = 100;
    for (size_t i = 0; i < MAX_SERVER_NUM; i++) {
        dnsParCache.CreateCacheForNet(i);
        dnsParCache.SetDnsCache(i, hostName.append(std::to_string(i)), addrInfo);
        servers.emplace_back(hostName.append(std::to_string(i)));
    }
    
    uint16_t baseTimeoutMsec = 100;
    uint8_t retryCount = 2;
    int32_t ret = dnsParCache.SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
    EXPECT_EQ(ret, 0);
    netId = 100;
    ret = dnsParCache.GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, -ENOENT);
    netId = 1;
    ret = dnsParCache.GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, 0);
    std::string info;
    dnsParCache.GetDumpInfo(info);
}

HWTEST_F(DNSParamCacheTest, CreateCacheForNetTest, TestSize.Level1)
{
    NETNATIVE_LOGI("CreateCacheForNetTest enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.SetDefaultNetwork(netId);
    int32_t ret = dnsParCache.CreateCacheForNet(netId);
    EXPECT_EQ(ret, 0);
    ret = dnsParCache.CreateCacheForNet(netId);
    EXPECT_EQ(ret, -EEXIST);
    netId = 0;
    std::string hostName = "hostName";
    dnsParCache.SetCacheDelayed(netId, hostName);
    netId = 2;
    dnsParCache.SetCacheDelayed(netId, hostName);
}

HWTEST_F(DNSParamCacheTest, DestroyNetworkCacheTest, TestSize.Level1)
{
    NETNATIVE_LOGI("DestroyNetworkCacheTest enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    int32_t ret = dnsParCache.DestroyNetworkCache(netId);
    EXPECT_EQ(ret, -ENOENT);
    dnsParCache.SetDefaultNetwork(netId);
    dnsParCache.CreateCacheForNet(netId);
    ret = dnsParCache.DestroyNetworkCache(netId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, EnableIpv6Test01, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.EnableIpv6(netId);
    EXPECT_FALSE(dnsParCache.IsIpv6Enable(netId));
}

HWTEST_F(DNSParamCacheTest, EnableIpv6Test02, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.CreateCacheForNet(netId);

    dnsParCache.EnableIpv6(netId);
    EXPECT_TRUE(dnsParCache.IsIpv6Enable(netId));

    dnsParCache.EnableIpv6(netId, false);
    EXPECT_FALSE(dnsParCache.IsIpv6Enable(netId));
}

HWTEST_F(DNSParamCacheTest, IsIpv6EnableTest01, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    uint16_t netId = 0;
    EXPECT_TRUE(netId == dnsParCache.defaultNetId_);

    bool ret = dnsParCache.IsIpv6Enable(netId);
    EXPECT_FALSE(ret);
}

HWTEST_F(DNSParamCacheTest, EnableIpv4Test01, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.SetClatDnsEnableIpv4(netId, true);
    EXPECT_TRUE(dnsParCache.IsIpv4Enable(netId));
    dnsParCache.SetClatDnsEnableIpv4(netId, false);
    EXPECT_FALSE(dnsParCache.IsIpv4Enable(netId));
    dnsParCache.EnableIpv4(netId);
    EXPECT_TRUE(dnsParCache.IsIpv4Enable(netId));
    dnsParCache.defaultNetId_ = netId;
    bool res = dnsParCache.IsIpv4Enable(0);
    EXPECT_TRUE(res);
    dnsParCache.DestroyNetworkCache(netId);
}

HWTEST_F(DNSParamCacheTest, GetResolverConfigTest05, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    servers.resize(MAX_SERVER_NUM + 1);
    uint16_t netId = 0;
    std::string hostName = "hoseName";
    dnsParCache.GetDnsCache(netId, hostName);
    AddrInfo addrInfo;
    addrInfo.aiFlags = 100;
    for (size_t i = 0; i < MAX_SERVER_NUM; i++) {
        dnsParCache.CreateCacheForNet(i);
        dnsParCache.SetDnsCache(i, hostName.append(std::to_string(i)), addrInfo);
        servers.emplace_back(hostName.append(std::to_string(i)));
    }

    uint16_t baseTimeoutMsec = 100;
    uint8_t retryCount = 2;
    int32_t ret = dnsParCache.GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
    EXPECT_TRUE(netId == dnsParCache.defaultNetId_);

    uint32_t uid = 1;
    ret = dnsParCache.GetResolverConfig(netId, uid, servers, domains, baseTimeoutMsec, retryCount);
    EXPECT_TRUE(netId == dnsParCache.defaultNetId_);
    ret = dnsParCache.GetResolverConfig(netId, uid, servers, domains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, GetDnsCacheTest01, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    uint16_t netId = 0;
    std::string hostName = "";
    auto ret = dnsParCache.GetDnsCache(netId, hostName);
    EXPECT_TRUE(netId == dnsParCache.defaultNetId_);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedServerFlagTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedServerFlagTest001 enter");
    DnsParamCache dnsParCache;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, 0);
    uidRanges.push_back(uidrange1);
    uint16_t netId = 0;
    bool flag = false;
    dnsParCache.SetDefaultNetwork(netId);
    dnsParCache.CreateCacheForNet(netId);
    int32_t ret = dnsParCache.GetUserDefinedServerFlag(netId, flag, 10001);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedServerFlagTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedServerFlagTest002 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 0;
    bool flag = false;
    dnsParCache.SetDefaultNetwork(netId);
    dnsParCache.CreateCacheForNet(netId);
    int32_t ret = dnsParCache.GetUserDefinedServerFlag(netId, flag);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedServerFlagTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedServerFlagTest003 enter");
    DnsParamCache dnsParCache;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, 0);
    uidRanges.push_back(uidrange1);
    uint16_t netId = 1;
    bool flag = false;
    dnsParCache.SetDefaultNetwork(netId);
    dnsParCache.CreateCacheForNet(netId);
    int32_t ret = dnsParCache.GetUserDefinedServerFlag(netId, flag, 9999);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedServerFlagTest004, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedServerFlagTest004 enter");
    DnsParamCache dnsParCache;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, 0);
    uidRanges.push_back(uidrange1);
    uint16_t netId = 0;
    dnsParCache.SetDefaultNetwork(1);
    dnsParCache.CreateCacheForNet(1);
    bool flag = false;
    int32_t ret = dnsParCache.GetUserDefinedServerFlag(netId, flag, 9999);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedServerFlagTest005, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedServerFlagTest005 enter");
    DnsParamCache dnsParCache;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, 0);
    uidRanges.push_back(uidrange1);
    uint16_t netId = 1;
    bool flag = false;
    int32_t ret = dnsParCache.GetUserDefinedServerFlag(netId, flag, 9999);
    EXPECT_EQ(ret, -ENOENT);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedServerFlagTest006, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedServerFlagTest006 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 0;
    bool flag = false;
    dnsParCache.SetDefaultNetwork(1);
    dnsParCache.CreateCacheForNet(1);
    int32_t ret = dnsParCache.GetUserDefinedServerFlag(netId, flag);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedServerFlagTest007, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedServerFlagTest007 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    bool flag = false;
    int32_t ret = dnsParCache.GetUserDefinedServerFlag(netId, flag);
    EXPECT_EQ(ret, -ENOENT);
}

HWTEST_F(DNSParamCacheTest, IsUseVpnDnsTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("IsUseVpnDnsTest007 enter");
    DnsParamCache dnsParCache;
    bool ret = dnsParCache.IsUseVpnDns(0);
    EXPECT_FALSE(ret);
}

HWTEST_F(DNSParamCacheTest, IsUseVpnDnsTest002, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    uint32_t vpnNetId = 100;
    std::vector<OHOS::NetManagerStandard::UidRange> uidRanges = {{1000, 2000, vpnNetId, 1}};
    dnsParCache.AddUidRange(vpnNetId, uidRanges);
    dnsParCache.CreateCacheForNet(vpnNetId, true);
    bool ret = dnsParCache.IsUseVpnDns(1500);
    EXPECT_TRUE(ret);
}

HWTEST_F(DNSParamCacheTest, IsUseVpnDnsTest003, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    uint32_t vpnNetId = 100;
    std::vector<OHOS::NetManagerStandard::UidRange> uidRanges = {{1000, 2000, vpnNetId, 1}};
    dnsParCache.AddUidRange(vpnNetId, uidRanges);
    bool ret = dnsParCache.IsUseVpnDns(1500);
    EXPECT_FALSE(ret);
}

HWTEST_F(DNSParamCacheTest, IsUseVpnDnsTest004, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    uint32_t vpnNetId = 100;
    std::vector<OHOS::NetManagerStandard::UidRange> uidRanges = {{1000, 2000, vpnNetId, 1}};
    dnsParCache.AddUidRange(vpnNetId, uidRanges);
    dnsParCache.CreateCacheForNet(vpnNetId, true);
    bool ret = dnsParCache.IsUseVpnDns(500);
    EXPECT_FALSE(ret);
}

HWTEST_F(DNSParamCacheTest, CreateCacheForNetTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("CreateCacheForNetTest001 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;

    dnsParCache.SetDefaultNetwork(netId);

    int32_t ret = dnsParCache.CreateCacheForNet(netId, false);
    EXPECT_EQ(ret, 0);

    uint16_t netId2 = 2;
    ret = dnsParCache.CreateCacheForNet(netId2, false);
    EXPECT_EQ(ret, 0);

    auto it1 = dnsParCache.serverConfigMap_.find(netId);
    auto it2 = dnsParCache.serverConfigMap_.find(netId2);
    EXPECT_NE(it1, dnsParCache.serverConfigMap_.end());
    EXPECT_NE(it2, dnsParCache.serverConfigMap_.end());

    uint16_t vpnNetId = 3;
    ret = dnsParCache.CreateCacheForNet(vpnNetId, true);
    EXPECT_EQ(ret, 0);

    auto it3 = dnsParCache.serverConfigMap_.find(vpnNetId);
    EXPECT_NE(it3, dnsParCache.serverConfigMap_.end());
}

HWTEST_F(DNSParamCacheTest, DestroyNetworkCacheTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("DestroyNetworkCacheTest001 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;

    dnsParCache.SetDefaultNetwork(netId);
    dnsParCache.CreateCacheForNet(netId);

    int32_t ret = dnsParCache.DestroyNetworkCache(netId, false);
    EXPECT_EQ(ret, 0);

    uint16_t netId2 = 2;
    ret = dnsParCache.DestroyNetworkCache(netId2, false);
    EXPECT_GE(ret, -2);
}

HWTEST_F(DNSParamCacheTest, SetUserDefinedServerFlagTest, TestSize.Level1)
{
    NETNATIVE_LOGI("CreateCacheForNetTest enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    int32_t ret = dnsParCache.SetUserDefinedServerFlag(netId, true);
    EXPECT_LE(ret, -2);
}

HWTEST_F(DNSParamCacheTest, FlushDnsCacheTest, TestSize.Level1)
{
    NETNATIVE_LOGI("FlushDnsCacheTest enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    int32_t ret = dnsParCache.FlushDnsCache(netId);
    EXPECT_EQ(ret, -2);
}

HWTEST_F(DNSParamCacheTest, FlushDnsCacheTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("FlushDnsCacheTest enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.defaultNetId_ = netId;
    dnsParCache.CreateCacheForNet(netId);
    int32_t ret = dnsParCache.FlushDnsCache(netId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, FlushDnsCacheTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("FlushDnsCacheTest enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 0;
    dnsParCache.defaultNetId_ = 1;
    dnsParCache.CreateCacheForNet(dnsParCache.defaultNetId_);
    int32_t ret = dnsParCache.FlushDnsCache(netId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, RemoveDuplicateNameserversTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("RemoveDuplicateNameserversTest enter");
    DnsParamCache dnsParCache;
    std::vector<std::string> servers;
    servers.emplace_back("1.1.1.1");
    servers.emplace_back("1.1.1.1");
    servers.emplace_back("2.2.2.2");
    servers.emplace_back("3.3.3.3");
    std::vector<std::string> res = dnsParCache.RemoveDuplicateNameservers(servers);
    EXPECT_FALSE(res.empty());
}

HWTEST_F(DNSParamCacheTest, SetDnsCacheTest001, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    dnsParCache.defaultNetId_ = 1;
    std::string hostName = "test";
    AddrInfo addrInfo;
    dnsParCache.SetDnsCache(0, hostName, addrInfo);

    dnsParCache.CreateCacheForNet(1);
    dnsParCache.SetDnsCache(1, hostName, addrInfo);

    AddrInfo addrInfoV6;
    addrInfoV6.aiFamily = 10;
    dnsParCache.SetDnsCache(1, hostName, addrInfoV6);

    auto res = dnsParCache.GetDnsCache(1, hostName);
    EXPECT_NE(res.size(), 0);
    dnsParCache.DestroyNetworkCache(1);
}

HWTEST_F(DNSParamCacheTest, SetDnsCacheTest002, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    dnsParCache.CreateCacheForNet(1);
    std::string hostName = "test";
    dnsParCache.SetCacheDelayed(1, hostName);

    AddrInfo addrInfo;
    addrInfo.aiFamily = 2;
    dnsParCache.SetDnsCache(1, hostName, addrInfo);
    dnsParCache.SetCacheDelayed(1, hostName);

    auto res = dnsParCache.GetDnsCache(1, hostName);
    EXPECT_NE(res.size(), 0);
    dnsParCache.DestroyNetworkCache(1);
}

HWTEST_F(DNSParamCacheTest, SetDnsCacheTest003, TestSize.Level1)
{
    DnsParamCache dnsParCache;
    dnsParCache.defaultNetId_ = 1;
    std::string hostName = "test";
    AddrInfoWithTtl addrInfo;
    dnsParCache.SetDnsCache(0, hostName, addrInfo);

    dnsParCache.CreateCacheForNet(1);
    addrInfo.addrInfo.aiFamily = 2;
    addrInfo.ttl = 0;
    dnsParCache.SetDnsCache(0, hostName, addrInfo);

    addrInfo.ttl = 50;
    dnsParCache.SetDnsCache(1, hostName, addrInfo);

    auto res = dnsParCache.GetDnsCache(1, hostName);
    EXPECT_NE(res.size(), 0);
    dnsParCache.DestroyNetworkCache(1);
}

HWTEST_F(DNSParamCacheTest, GetDefaultNetworkTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("GetDefaultNetworkTest001 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.SetDefaultNetwork(netId);
    int32_t ret = dnsParCache.GetDefaultNetwork();
    EXPECT_EQ(ret, netId);
}

HWTEST_F(DNSParamCacheTest, GetDefaultNetworkTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("GetDefaultNetworkTest002 enter");
    DnsParamCache dnsParCache;
    int32_t ret = dnsParCache.GetDefaultNetwork();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, AddUidRangeTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("AddUidRangeTest001 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 1;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, netId, 0);
    uidRanges.push_back(uidrange1);
    int32_t ret = dnsParCache.AddUidRange(netId, uidRanges);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, AddUidRangeTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("AddUidRangeTest002 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 1;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, netId, 0);
    uidRanges.push_back(uidrange1);
    NetManagerStandard::UidRange uidrange2(30000, 40000, netId, 0);
    uidRanges.push_back(uidrange2);
    int32_t ret = dnsParCache.AddUidRange(netId, uidRanges);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, DelUidRangeTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("DelUidRangeTest001 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 1;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, netId, 0);
    uidRanges.push_back(uidrange1);
    dnsParCache.AddUidRange(netId, uidRanges);
    int32_t ret = dnsParCache.DelUidRange(netId, uidRanges);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(DNSParamCacheTest, IsVpnOpenTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("IsVpnOpenTest001 enter");
    DnsParamCache dnsParCache;
    bool ret = dnsParCache.IsVpnOpen();
    EXPECT_FALSE(ret);
}

HWTEST_F(DNSParamCacheTest, IsVpnOpenTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("IsVpnOpenTest002 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 1;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, netId, 0);
    uidRanges.push_back(uidrange1);
    dnsParCache.AddUidRange(netId, uidRanges);
    bool ret = dnsParCache.IsVpnOpen();
    EXPECT_TRUE(ret);
}

HWTEST_F(DNSParamCacheTest, SetNodataCacheTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("SetNodataCacheTest001 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::string hostName = "test.example.com";
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    dnsParCache.SetNodataCache(netId, hostName);
    bool ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_TRUE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}

HWTEST_F(DNSParamCacheTest, SetNodataCacheTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("SetNodataCacheTest002 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::string hostName = "test.example.com";
    dnsParCache.SetNodataCache(netId, hostName);
    bool ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_FALSE(ret);
}

HWTEST_F(DNSParamCacheTest, IsInNodataCacheTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("IsInNodataCacheTest001 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::string hostName = "test.example.com";
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    bool ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_FALSE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}

HWTEST_F(DNSParamCacheTest, IsInNodataCacheTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("IsInNodataCacheTest002 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::string hostName = "test.example.com";
    bool ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_FALSE(ret);
}

HWTEST_F(DNSParamCacheTest, SetNodataCacheTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("SetNodataCacheTest003 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::string hostName = "test.example.com";
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    // Set cache first time
    dnsParCache.SetNodataCache(netId, hostName);
    bool ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_TRUE(ret);
    // Update existing cache entry - covers DnsResolvConfig::SetNodataCache branch for updating existing entry
    dnsParCache.SetNodataCache(netId, hostName);
    ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_TRUE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}

HWTEST_F(DNSParamCacheTest, IsInNodataCacheTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("IsInNodataCacheTest003 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::string hostName = "test.example.com";
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    dnsParCache.SetNodataCache(netId, hostName);
    bool ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_TRUE(ret);
    // Clear the cache to simulate expiration - covers DnsResolvConfig::IsInNodataCache branch for cache not exist
    dnsParCache.FlushDnsCache(netId);
    ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_FALSE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}

HWTEST_F(DNSParamCacheTest, SetNodataCacheTest004, TestSize.Level1)
{
    NETNATIVE_LOGI("SetNodataCacheTest004 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.CreateCacheForNet(netId);
    // Not enable IPv4, SetNodataCache should return directly without setting cache
    // This covers DnsResolvConfig::SetNodataCache branch: if (!IsIpv4Enable()) return;
    std::string hostName = "test.example.com";
    dnsParCache.SetNodataCache(netId, hostName);
    bool ret = dnsParCache.IsInNodataCache(netId, hostName);
    EXPECT_FALSE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}

HWTEST_F(DNSParamCacheTest, SetNodataCacheTest005, TestSize.Level1)
{
    NETNATIVE_LOGI("SetNodataCacheTest005 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    // Fill cache to MAX_NODATA_CACHE_SIZE (100) and verify oldest is removed
    // This covers DnsResolvConfig::SetNodataCache branch: if (nodataCache_.size() >= MAX_NODATA_CACHE_SIZE)
    for (size_t i = 0; i < MAX_NODATA_CACHE_SIZE + 1; ++i) {
        std::string hostName = "host" + std::to_string(i) + ".example.com";
        dnsParCache.SetNodataCache(netId, hostName);
    }
    // The first entry should be removed due to cache size limit
    std::string firstHostName = "host0.example.com";
    EXPECT_FALSE(dnsParCache.IsInNodataCache(netId, firstHostName));
    // The last entry should still exist
    std::string lastHostName = "host100.example.com";
    EXPECT_TRUE(dnsParCache.IsInNodataCache(netId, lastHostName));
    dnsParCache.DestroyNetworkCache(netId);
}

HWTEST_F(DNSParamCacheTest, GetDumpInfoTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("GetDumpInfoTest001 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.CreateCacheForNet(netId);
    std::vector<std::string> servers = {"8.8.8.8"};
    std::vector<std::string> domains = {"example.com"};
    dnsParCache.SetResolverConfig(netId, 1000, 3, servers, domains);
    std::string info;
    dnsParCache.GetDumpInfo(info);
    EXPECT_FALSE(info.empty());
    dnsParCache.DestroyNetworkCache(netId);
}

HWTEST_F(DNSParamCacheTest, AddUidRangeDedupTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("AddUidRangeDedupTest001 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 100;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId);
    uidRanges.push_back(uidrange1);
    dnsParCache.AddUidRange(netId, uidRanges);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 1u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 1u);
    dnsParCache.AddUidRange(netId, uidRanges);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 1u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 1u);
}

HWTEST_F(DNSParamCacheTest, AddUidRangeDedupTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("AddUidRangeDedupTest002 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    uint32_t netId2 = 200;
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    std::vector<NetManagerStandard::UidRange> uidRanges2;
    NetManagerStandard::UidRange uidrange2(30000, 40000, 0, netId2);
    uidRanges2.push_back(uidrange2);
    dnsParCache.AddUidRange(netId1, uidRanges1);
    dnsParCache.AddUidRange(netId2, uidRanges2);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 2u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 2u);
}

HWTEST_F(DNSParamCacheTest, AddUidRangeDedupTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("AddUidRangeDedupTest003 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 100;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId);
    NetManagerStandard::UidRange uidrange2(30000, 40000, 0, netId);
    uidRanges.push_back(uidrange1);
    uidRanges.push_back(uidrange2);
    dnsParCache.AddUidRange(netId, uidRanges);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 2u);
    std::vector<NetManagerStandard::UidRange> uidRangesDup;
    uidRangesDup.push_back(uidrange1);
    dnsParCache.AddUidRange(netId, uidRangesDup);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 1u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 1u);
}

HWTEST_F(DNSParamCacheTest, AddUidRangeDedupTest004, TestSize.Level1)
{
    NETNATIVE_LOGI("AddUidRangeDedupTest004 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 100;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId);
    uidRanges.push_back(uidrange1);
    dnsParCache.AddUidRange(netId, uidRanges);
    std::vector<NetManagerStandard::UidRange> emptyRanges;
    dnsParCache.AddUidRange(netId, emptyRanges);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 1u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 0u);
}

HWTEST_F(DNSParamCacheTest, DelUidRangeRemoveAllTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("DelUidRangeRemoveAllTest001 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 100;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId);
    uidRanges.push_back(uidrange1);
    dnsParCache.AddUidRange(netId, uidRanges);
    dnsParCache.AddUidRange(netId, uidRanges);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 1u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 1u);
    dnsParCache.DelUidRange(netId, uidRanges);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 0u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 0u);
}

HWTEST_F(DNSParamCacheTest, DelUidRangeRemoveAllTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("DelUidRangeRemoveAllTest002 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    uint32_t netId2 = 200;
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    std::vector<NetManagerStandard::UidRange> uidRanges2;
    NetManagerStandard::UidRange uidrange2(30000, 40000, 0, netId2);
    uidRanges2.push_back(uidrange2);
    dnsParCache.AddUidRange(netId1, uidRanges1);
    dnsParCache.AddUidRange(netId2, uidRanges2);
    dnsParCache.DelUidRange(netId1, uidRanges1);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 1u);
    EXPECT_EQ(dnsParCache.vpnNetId_.front(), netId2);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 1u);
}

HWTEST_F(DNSParamCacheTest, DelUidRangeRemoveAllTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("DelUidRangeRemoveAllTest003 enter");
    DnsParamCache dnsParCache;
    uint32_t netId = 999;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId);
    uidRanges.push_back(uidrange1);
    dnsParCache.DelUidRange(netId, uidRanges);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 0u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 0u);
}

HWTEST_F(DNSParamCacheTest, GetVpnResolverConfigContinueTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("GetVpnResolverConfigContinueTest001 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    uint32_t netId2 = 200;
    dnsParCache.CreateCacheForNet(netId2, true);
    std::vector<std::string> servers = {"8.8.8.8"};
    std::vector<std::string> domains = {"vpn.example.com"};
    dnsParCache.SetResolverConfig(netId2, 1000, 2, servers, domains);
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    dnsParCache.vpnNetId_.push_back(netId1);
    dnsParCache.vpnNetId_.push_back(netId2);
    dnsParCache.vpnUidRanges_.push_back(uidrange1);
    std::vector<NetManagerStandard::UidRange> uidRanges2;
    NetManagerStandard::UidRange uidrange2(10000, 20000, 0, netId2);
    uidRanges2.push_back(uidrange2);
    dnsParCache.vpnUidRanges_.push_back(uidrange2);
    std::vector<std::string> outServers;
    std::vector<std::string> outDomains;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    int32_t ret = dnsParCache.GetVpnResolverConfig(15000, outServers, outDomains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(outServers.size(), 1u);
    EXPECT_EQ(outServers[0], "8.8.8.8");
}

HWTEST_F(DNSParamCacheTest, GetVpnResolverConfigContinueTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("GetVpnResolverConfigContinueTest002 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    dnsParCache.vpnNetId_.push_back(netId1);
    dnsParCache.vpnUidRanges_.push_back(uidrange1);
    std::vector<std::string> outServers;
    std::vector<std::string> outDomains;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    int32_t ret = dnsParCache.GetVpnResolverConfig(15000, outServers, outDomains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedVpnServerFlagContinueTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedVpnServerFlagContinueTest001 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    uint32_t netId2 = 200;
    dnsParCache.CreateCacheForNet(netId2, true);
    dnsParCache.SetUserDefinedServerFlag(netId2, true);
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    dnsParCache.vpnNetId_.push_back(netId1);
    dnsParCache.vpnNetId_.push_back(netId2);
    dnsParCache.vpnUidRanges_.push_back(uidrange1);
    std::vector<NetManagerStandard::UidRange> uidRanges2;
    NetManagerStandard::UidRange uidrange2(10000, 20000, 0, netId2);
    uidRanges2.push_back(uidrange2);
    dnsParCache.vpnUidRanges_.push_back(uidrange2);
    bool flag = false;
    int32_t ret = dnsParCache.GetUserDefinedVpnServerFlag(15000, flag);
    EXPECT_EQ(ret, 0);
    EXPECT_TRUE(flag);
}

HWTEST_F(DNSParamCacheTest, GetUserDefinedVpnServerFlagContinueTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("GetUserDefinedVpnServerFlagContinueTest002 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    dnsParCache.vpnNetId_.push_back(netId1);
    dnsParCache.vpnUidRanges_.push_back(uidrange1);
    bool flag = false;
    int32_t ret = dnsParCache.GetUserDefinedVpnServerFlag(15000, flag);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(DNSParamCacheTest, IsUseVpnDnsContinueTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("IsUseVpnDnsContinueTest001 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    uint32_t netId2 = 200;
    dnsParCache.CreateCacheForNet(netId2, true);
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    dnsParCache.vpnUidRanges_.push_back(uidrange1);
    std::vector<NetManagerStandard::UidRange> uidRanges2;
    NetManagerStandard::UidRange uidrange2(10000, 20000, 0, netId2);
    uidRanges2.push_back(uidrange2);
    dnsParCache.vpnUidRanges_.push_back(uidrange2);
    bool ret = dnsParCache.IsUseVpnDns(15000);
    EXPECT_TRUE(ret);
}

HWTEST_F(DNSParamCacheTest, IsUseVpnDnsContinueTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("IsUseVpnDnsContinueTest002 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    dnsParCache.vpnUidRanges_.push_back(uidrange1);
    bool ret = dnsParCache.IsUseVpnDns(15000);
    EXPECT_FALSE(ret);
}

HWTEST_F(DNSParamCacheTest, VpnDnsResidueScenarioTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("VpnDnsResidueScenarioTest001 enter");
    DnsParamCache dnsParCache;
    uint32_t netId1 = 100;
    uint32_t netId2 = 200;
    std::vector<NetManagerStandard::UidRange> uidRanges1;
    NetManagerStandard::UidRange uidrange1(10000, 20000, 0, netId1);
    uidRanges1.push_back(uidrange1);
    dnsParCache.CreateCacheForNet(netId1, true);
    std::vector<std::string> servers1 = {"1.1.1.1"};
    std::vector<std::string> domains1 = {"vpn1.example.com"};
    dnsParCache.SetResolverConfig(netId1, 500, 1, servers1, domains1);
    dnsParCache.AddUidRange(netId1, uidRanges1);
    dnsParCache.AddUidRange(netId1, uidRanges1);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 1u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 1u);
    dnsParCache.DestroyNetworkCache(netId1, true);
    dnsParCache.DelUidRange(netId1, uidRanges1);
    EXPECT_EQ(dnsParCache.vpnNetId_.size(), 0u);
    EXPECT_EQ(dnsParCache.vpnUidRanges_.size(), 0u);
    dnsParCache.CreateCacheForNet(netId2, true);
    std::vector<std::string> servers2 = {"2.2.2.2"};
    std::vector<std::string> domains2 = {"vpn2.example.com"};
    dnsParCache.SetResolverConfig(netId2, 600, 2, servers2, domains2);
    std::vector<NetManagerStandard::UidRange> uidRanges2;
    NetManagerStandard::UidRange uidrange2(10000, 20000, 0, netId2);
    uidRanges2.push_back(uidrange2);
    dnsParCache.AddUidRange(netId2, uidRanges2);
    std::vector<std::string> outServers;
    std::vector<std::string> outDomains;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    int32_t ret = dnsParCache.GetVpnResolverConfig(15000, outServers, outDomains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(outServers.size(), 1u);
    EXPECT_EQ(outServers[0], "2.2.2.2");
}

HWTEST_F(DNSParamCacheTest, SetIpv6UidBlackListTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("SetIpv6UidBlackListTest001 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::vector<int32_t> netIds;
    netIds.emplace_back(netId);
    uint32_t uid = 1;
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    dnsParCache.SetIpv6UidBlackList(netIds, uid);
    bool ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_TRUE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}
 
HWTEST_F(DNSParamCacheTest, SetIpv6UidBlackListTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("SetIpv6UidBlackListTest002 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::vector<int32_t> netIds;
    netIds.emplace_back(netId);
    uint32_t uid = 1;
    dnsParCache.SetIpv6UidBlackList(netIds, uid);
    bool ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_FALSE(ret);
}
 
HWTEST_F(DNSParamCacheTest, SetIpv6UidBlackListTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("SetIpv6UidBlackListTest003 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::vector<int32_t> netIds;
    netIds.emplace_back(netId);
    uint32_t uid = 1;
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    // Set ipv6 uid black list first time
    dnsParCache.SetIpv6UidBlackList(netIds, uid);
    bool ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_TRUE(ret);
    // Update existing uid - covers DnsResolvConfig::SetIpv6UidBlackList branch for updating existing entry
    dnsParCache.SetIpv6UidBlackList(netIds, uid);
    ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_TRUE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}
 
HWTEST_F(DNSParamCacheTest, SetIpv6UidBlackListTest004, TestSize.Level1)
{
    NETNATIVE_LOGI("SetIpv6UidBlackListTest004 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.CreateCacheForNet(netId);
    // Not enable IPv4, SetIpv6UidBlackList should return directly without setting Ipv6 uid black list
    // This covers DnsResolvConfig::SetIpv6UidBlackList branch: if (!IsIpv4Enable()) return;
    std::vector<int32_t> netIds;
    netIds.emplace_back(netId);
    uint32_t uid = 1;
    dnsParCache.SetIpv6UidBlackList(netIds, uid);
    bool ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_FALSE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}
 
HWTEST_F(DNSParamCacheTest, SetIpv6UidBlackListTest005, TestSize.Level1)
{
    NETNATIVE_LOGI("SetIpv6UidBlackListTest005 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    std::vector<int32_t> netIds;
    netIds.emplace_back(netId);
    // Fill uid black list to MAX_IPV6_UID_BLACK_LIST_SIZE (32) and verify oldest is removed
    // This covers DnsResolvConfig::SetIpv6UidBlackList branch: ipv6UidBlackList_.size >= MAX_IPV6_UID_BLACK_LIST_SIZE
    for (size_t i = 0; i < MAX_IPV6_UID_BLACK_LIST_SIZE + 1; ++i) {
        uint32_t uid = i;
        dnsParCache.SetIpv6UidBlackList(netIds, uid);
    }
    // The first entry should be removed due to Ipv6 black list size limit
    uint32_t firstUid = 0;
    EXPECT_FALSE(dnsParCache.IsInIpv6UidBlackList(netId, firstUid));
    // The last entry should still exist
    uint32_t lastUid = MAX_IPV6_UID_BLACK_LIST_SIZE;
    EXPECT_TRUE(dnsParCache.IsInIpv6UidBlackList(netId, lastUid));
    dnsParCache.DestroyNetworkCache(netId);
}
 
HWTEST_F(DNSParamCacheTest, IsInIpv6UidBlackListTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("IsInIpv6UidBlackListTest001 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    uint32_t uid = 1;
    bool ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_FALSE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}
 
HWTEST_F(DNSParamCacheTest, IsInIpv6UidBlackListTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("IsInIpv6UidBlackListTest002 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    uint32_t uid = 1;
    bool ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_FALSE(ret);
}
 
HWTEST_F(DNSParamCacheTest, IsInIpv6UidBlackListTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("IsInIpv6UidBlackListTest003 enter");
    DnsParamCache dnsParCache;
    uint16_t netId = 1;
    std::vector<int32_t> netIds;
    netIds.emplace_back(netId);
    dnsParCache.CreateCacheForNet(netId);
    dnsParCache.EnableIpv4(netId);
    uint32_t uid = 1;
    dnsParCache.SetIpv6UidBlackList(netIds, uid);
    bool ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_TRUE(ret);
    // Clear the cache to simulate expiration - covers DnsResolvConfig::IsInIpv6UidBlackList branch for cache not exist
    dnsParCache.FlushDnsCache(netId);
    ret = dnsParCache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_FALSE(ret);
    dnsParCache.DestroyNetworkCache(netId);
}

} // namespace NetsysNative
} // namespace OHOS

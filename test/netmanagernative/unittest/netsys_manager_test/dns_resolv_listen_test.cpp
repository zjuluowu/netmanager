/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <arpa/inet.h>
#include <dlfcn.h>
#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "dns_config_client.h"
#include "dns_param_cache.h"
#include "dns_resolv_listen.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace OHOS::nmd;
constexpr const char *DNS_SO_PATH = "libnetsys_client.z.so";
static constexpr const int32_t CLIENT_SOCK_FD = 99999;
static constexpr const uint32_t NET_ID = 99999;
std::shared_ptr<DnsResolvListen> instance_ = nullptr;

typedef int32_t (*GetConfig)(uint16_t netId, struct ResolvConfig *config);
typedef int32_t (*GetConfigExt)(uint16_t netId, struct ResolvConfigExt *config);
typedef int32_t (*GetCache)(uint16_t netId, struct ParamWrapper param, struct AddrInfo addr_info[MAX_RESULTS],
                            uint32_t *num);

typedef int32_t (*SetCache)(uint16_t netId, struct ParamWrapper param, struct addrinfo *res);

class DnsResolvListenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DnsResolvListenTest::SetUpTestCase() {}

void DnsResolvListenTest::TearDownTestCase() {}

void DnsResolvListenTest::SetUp()
{
    instance_ = std::make_shared<DnsResolvListen>();
}

void DnsResolvListenTest::TearDown() {}

HWTEST_F(DnsResolvListenTest, NetSysGetResolvConfTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetSysGetResolvConf001 enter");
    void *handle = dlopen(DNS_SO_PATH, RTLD_LAZY);
    ASSERT_NE(handle, NULL);
    GetConfig func = (GetConfig)dlsym(handle, "NetSysGetResolvConf");
    ASSERT_NE(func, NULL);

    ResolvConfig config = {0};
    int ret = func(0, &config);
    dlclose(handle);
    EXPECT_GE(ret, -ENOENT);
}

HWTEST_F(DnsResolvListenTest, NetSysGetResolvConfExtTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetSysGetResolvConfExtTest001 enter");
    void *handle = dlopen(DNS_SO_PATH, RTLD_LAZY);
    ASSERT_NE(handle, NULL);
    GetConfigExt func = (GetConfigExt)dlsym(handle, "NetSysGetResolvConf");
    EXPECT_NE(func, NULL);
    ResolvConfigExt config = {0};
    int ret = func(0, &config);
    dlclose(handle);
}

HWTEST_F(DnsResolvListenTest, DnsResolvListenStartListenTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsResolvListenStartListenTest001 enter");
    std::shared_ptr<DnsResolvListen> listen = std::make_shared<DnsResolvListen>();
    EXPECT_TRUE(listen != nullptr);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest001 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    std::vector<std::string> servers = {"8.8.8.8"};
    std::vector<std::string> domains = {"example.com"};
    int32_t ret = cache.SetResolverConfig(netId, 1000, 3, servers, domains);
    EXPECT_EQ(ret, 0);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest002 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    cache.SetDefaultNetwork(netId);
    int32_t defaultNet = cache.GetDefaultNetwork();
    EXPECT_EQ(defaultNet, netId);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest003 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    cache.EnableIpv6(netId);
    bool isIpv6Enable = cache.IsIpv6Enable(netId);
    EXPECT_TRUE(isIpv6Enable);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest004, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest004 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    cache.EnableIpv4(netId);
    bool isIpv4Enable = cache.IsIpv4Enable(netId);
    EXPECT_TRUE(isIpv4Enable);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest005, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest005 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    cache.EnableIpv4(netId);
    std::string hostName = "test.example.com";
    cache.SetNodataCache(netId, hostName);
    bool isInNodataCache = cache.IsInNodataCache(netId, hostName);
    EXPECT_TRUE(isInNodataCache);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest006, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest006 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    cache.EnableIpv4(netId);
    std::string hostName = "test.example.com";
    cache.SetNodataCache(netId, hostName);
    cache.FlushDnsCache(netId);
    bool isInNodataCache = cache.IsInNodataCache(netId, hostName);
    EXPECT_FALSE(isInNodataCache);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest007, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest007 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint32_t netId = 100;
    std::vector<NetManagerStandard::UidRange> uidRanges;
    NetManagerStandard::UidRange uidrange1(10000, 20000, netId, 0);
    uidRanges.push_back(uidrange1);
    int32_t ret = cache.AddUidRange(netId, uidRanges);
    EXPECT_EQ(ret, 0);
    bool isVpnOpen = cache.IsVpnOpen();
    EXPECT_TRUE(isVpnOpen);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest008, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest008 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    bool flag = true;
    int32_t ret = cache.SetUserDefinedServerFlag(netId, flag);
    EXPECT_EQ(ret, 0);
    bool resultFlag = false;
    ret = cache.GetUserDefinedServerFlag(netId, resultFlag);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(resultFlag, flag);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest009, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest009 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    std::string hostName = "test.example.com";
    AddrInfo addrInfo;
    addrInfo.aiFamily = AF_INET;
    addrInfo.aiAddr.sin.sin_addr.s_addr = inet_addr("192.168.1.1");
    cache.SetDnsCache(netId, hostName, addrInfo);
    auto result = cache.GetDnsCache(netId, hostName);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].aiFamily, addrInfo.aiFamily);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest010, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest010 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    cache.CreateCacheForNet(netId);
    std::string hostName = "test.example.com";
    AddrInfo addrInfo;
    addrInfo.aiFamily = AF_INET;
    addrInfo.aiAddr.sin.sin_addr.s_addr = inet_addr("192.168.1.1");
    cache.SetDnsCache(netId, hostName, addrInfo);
    int32_t ret = cache.FlushDnsCache(netId);
    EXPECT_EQ(ret, 0);
    auto result = cache.GetDnsCache(netId, hostName);
    EXPECT_EQ(result.size(), 0);
    cache.DestroyNetworkCache(netId);
}

HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest011, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest011 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    std::vector<int32_t> netIds;
    netIds.emplace_back(netId);
    cache.CreateCacheForNet(netId);
    cache.EnableIpv4(netId);
    uint32_t uid = 1;
    cache.SetIpv6UidBlackList(netIds, uid);
    bool isInIpv6UidBlackList = cache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_TRUE(isInIpv6UidBlackList);
    cache.DestroyNetworkCache(netId);
}
 
HWTEST_F(DnsResolvListenTest, DnsParamCacheInteractionTest012, TestSize.Level1)
{
    NETNATIVE_LOGI("DnsParamCacheInteractionTest012 enter");
    DnsParamCache &cache = DnsParamCache::GetInstance();
    uint16_t netId = 100;
    std::vector<int32_t> netIds;
    netIds.emplace_back(netId);
    cache.CreateCacheForNet(netId);
    cache.EnableIpv4(netId);
    uint32_t uid = 1;
    cache.SetIpv6UidBlackList(netIds, uid);
    cache.FlushDnsCache(netId);
    bool isInIpv6UidBlackList = cache.IsInIpv6UidBlackList(netId, uid);
    EXPECT_FALSE(isInIpv6UidBlackList);
    cache.DestroyNetworkCache(netId);
}

} // namespace NetsysNative
} // namespace OHOS

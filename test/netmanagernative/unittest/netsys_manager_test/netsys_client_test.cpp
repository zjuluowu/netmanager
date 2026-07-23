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

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include <netinet/in.h>
#include "netsys_client.h"
#include "dns_config_client.h"
#include "app_net_client.h"

#ifdef __cplusplus
extern "C" {
#endif

void MakeDefaultDnsServer(char *server, size_t length);
int32_t NetSysGetResolvConf(uint16_t netId, struct ResolvConfig *config);
int32_t NetSysGetResolvCache(uint16_t netId, const struct ParamWrapper param,
    struct AddrInfo addrInfo[], uint32_t *num);
int32_t NetSysSetResolvCache(uint16_t netId, const struct ParamWrapper param, struct DnsAns *res, int32_t num);
int32_t NetSysGetDefaultNetwork(uint16_t netId, int32_t* currentNetId);
int32_t NetSysBindSocket(int32_t fd, uint32_t netId);
char *addr_to_string(const AlignedSockAddr *addr, char *buf, size_t len);
bool IsSystemUid(void);
bool IsLoopbackAddr(struct AddrInfo addrInfo[], int32_t addrSize);
bool IsAllCname(struct DnsProcessInfoExt *dnsProcessInfo);
bool IsAllNoAnswer(struct DnsProcessInfoExt *dnsProcessInfo);
bool IsFailCauseAllowedReport(int failcause);
int32_t GetQueryFailCause(struct DnsProcessInfoExt *dnsProcessInfo,
    struct AddrInfo addrInfo[], int32_t addrSize);
int32_t NetsysPostDnsAbnormal(int32_t failcause, struct DnsCacheInfo dnsInfo);
void HandleQueryAbnormalReport(struct DnsProcessInfoExt dnsProcessInfo,
    struct AddrInfo addrInfo[], int32_t addrSize);
int32_t NetSysPostDnsQueryResult(int netid, struct addrinfo *addr, char *srcAddr,
    struct DnsProcessInfo *processInfo);
int32_t FillBasicAddrInfo(struct AddrInfo *addrInfo, struct addrinfo *info);
int32_t FillBasicDnsServerInfo(struct DnsServerInfo *serverInfo, struct dnsserver *ds);
int32_t FillDnsServerInfo(struct DnsServerInfo dnsServerInfo[],
    struct dnsserver *res);
int32_t FillQueryParam(struct queryparam *orig, struct QueryParam *dest, struct recordinfo *info);
int32_t NetSysSetNodataCache(uint16_t netId, const char *host);
int32_t NetSysGetResolvConfExt(uint16_t netId, struct ResolvConfigExt *config);
int NetSysGetNodataCache(uint16_t netId, const char *host);
#ifdef __cplusplus
}
#endif

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
}

class NetsysClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetsysClientTest::SetUpTestCase() {}

void NetsysClientTest::TearDownTestCase() {}

void NetsysClientTest::SetUp() {}

void NetsysClientTest::TearDown() {}

HWTEST_F(NetsysClientTest, MakeDefaultDnsServerTest001, TestSize.Level1)
{
    char server[DEFAULT_SERVER_LENTH] = {0};
    size_t length = 1;
    int ret = memset_s(nullptr, length, 0, DEFAULT_SERVER_LENTH);
    EXPECT_NE(ret, 0);
    MakeDefaultDnsServer(nullptr, length);
    MakeDefaultDnsServer(server, length);
}

HWTEST_F(NetsysClientTest, NetSysGetResolvConfTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    struct ResolvConfig config;
    auto ret = NetSysGetResolvConf(netId, nullptr);
    EXPECT_EQ(ret, -EINVAL);

    SetNetForApp(1);
    NetSysGetResolvConf(netId, &config);

    config.nameservers[0][0] = '\0';
    NetSysGetResolvConf(netId, &config);
}

HWTEST_F(NetsysClientTest, NetSysGetResolvCacheTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    SetNetForApp(1);
    struct ParamWrapper param;
    param.host = nullptr;
    struct AddrInfo addrInfo[MAX_RESULTS];
    uint32_t num = 0;
    auto ret = NetSysGetResolvCache(netId, param, addrInfo, nullptr);
    EXPECT_EQ(ret, -EINVAL);

    char host[MAX_RESULTS] = {0};
    param.host = host;
    ret = NetSysGetResolvCache(netId, param, addrInfo, nullptr);
    EXPECT_EQ(ret, -EINVAL);

    strcpy_s(host, MAX_RESULTS, "test");
    ret = NetSysGetResolvCache(netId, param, addrInfo, nullptr);
    EXPECT_EQ(ret, -EINVAL);

    ret = NetSysGetResolvCache(netId, param, addrInfo, &num);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysClientTest, NetSysSetResolvCacheTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    SetNetForApp(1);
    struct ParamWrapper param;
    param.host = nullptr;
    auto ret = NetSysSetResolvCache(netId, param, nullptr, 0);
    EXPECT_EQ(ret, -EINVAL);

    char host[MAX_RESULTS] = {0};
    param.host = host;
    ret = NetSysSetResolvCache(netId, param, nullptr, 0);
    EXPECT_EQ(ret, -EINVAL);

    strcpy_s(host, MAX_RESULTS, "test");
    ret = NetSysSetResolvCache(netId, param, nullptr, 0);
    EXPECT_EQ(ret, -EINVAL);

    struct addrinfo addrInfo;
    struct DnsAns ans;
    ans.ai = &addrInfo;
    ans.ttl = 10;
    ret = NetSysSetResolvCache(netId, param, &ans, -1);
    EXPECT_EQ(ret, -EINVAL);
    
    ret = NetSysSetResolvCache(netId, param, &ans, 1);
    EXPECT_NE(ret, -EINVAL);
}

HWTEST_F(NetsysClientTest, NetSysGetDefaultNetworkTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    int32_t currentNetId = 1;
    auto ret = NetSysGetDefaultNetwork(netId, &currentNetId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysClientTest, NetSysBindSocketTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    int32_t fd = 0;
    auto ret = NetSysBindSocket(fd, netId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysClientTest, addr_to_stringTest001, TestSize.Level1)
{
    AlignedSockAddr addr;
    char buf[INET6_ADDRSTRLEN] = {0};
    size_t len = 0;
    addr.sa.sa_family = AF_INET;
    auto ret = addr_to_string(&addr, nullptr, len);
    EXPECT_EQ(ret, nullptr);

    len = INET_ADDRSTRLEN;
    ret = addr_to_string(&addr, buf, len);
    EXPECT_EQ(ret, buf);

    len = 0;
    addr.sa.sa_family = AF_INET6;
    ret = addr_to_string(&addr, nullptr, len);
    EXPECT_EQ(ret, nullptr);

    len = INET6_ADDRSTRLEN;
    ret = addr_to_string(&addr, buf, len);
    EXPECT_EQ(ret, buf);

    addr.sa.sa_family = 0;
    ret = addr_to_string(&addr, nullptr, len);
    EXPECT_EQ(ret, nullptr);
}

HWTEST_F(NetsysClientTest, IsSystemUidTest001, TestSize.Level1)
{
    auto ret = IsSystemUid();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetsysClientTest, IsLoopbackAddrTest001, TestSize.Level1)
{
    struct AddrInfo addrInfo[MAX_RESULTS];
    int32_t addrSize = 0;
    auto ret = IsLoopbackAddr(addrInfo, addrSize);
    EXPECT_FALSE(ret);

    addrSize = 1;
    addrInfo[0].aiAddr.sa.sa_family = 0;
    ret = IsLoopbackAddr(addrInfo, addrSize);
    EXPECT_FALSE(ret);

    addrInfo[0].aiAddr.sa.sa_family = AF_INET;
    inet_pton(AF_INET, LOOP_BACK_ADDR1, &addrInfo[0].aiAddr.sin.sin_addr);
    ret = IsLoopbackAddr(addrInfo, addrSize);
    EXPECT_TRUE(ret);

    inet_pton(AF_INET, LOOP_BACK_ADDR2, &addrInfo[0].aiAddr.sin.sin_addr);
    ret = IsLoopbackAddr(addrInfo, addrSize);
    EXPECT_TRUE(ret);

    inet_pton(AF_INET, "192.168.1.1", &addrInfo[0].aiAddr.sin.sin_addr);
    ret = IsLoopbackAddr(addrInfo, addrSize);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetsysClientTest, IsAllCnameTest001, TestSize.Level1)
{
    struct DnsProcessInfoExt dnsProcessInfo = {};
    dnsProcessInfo.isFromCache = true;
    auto ret = IsAllCname(&dnsProcessInfo);
    EXPECT_FALSE(ret);

    dnsProcessInfo.isFromCache = false;
    ret = IsAllCname(&dnsProcessInfo);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetsysClientTest, IsAllNoAnswerTest001, TestSize.Level1)
{
    struct DnsProcessInfoExt dnsProcessInfo = {};
    dnsProcessInfo.isFromCache = true;
    auto ret = IsAllNoAnswer(&dnsProcessInfo);
    EXPECT_FALSE(ret);

    dnsProcessInfo.isFromCache = false;
    dnsProcessInfo.retCode = 1;
    ret = IsAllNoAnswer(&dnsProcessInfo);
    EXPECT_FALSE(ret);

    dnsProcessInfo.retCode = 0;
    dnsProcessInfo.ipv4QueryInfo.isNoAnswer = 1;
    dnsProcessInfo.ipv6QueryInfo.isNoAnswer = 1;
    ret = IsAllNoAnswer(&dnsProcessInfo);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetsysClientTest, IsFailCauseAllowedReportTest001, TestSize.Level1)
{
    int failcause = FAIL_CAUSE_NONE;
    auto ret = IsFailCauseAllowedReport(failcause);
    EXPECT_FALSE(ret);

    failcause = FAIL_CAUSE_QUERY_FAIL;
    IsFailCauseAllowedReport(failcause);
}

HWTEST_F(NetsysClientTest, GetQueryFailCauseTest001, TestSize.Level1)
{
    struct DnsProcessInfoExt dnsProcessInfo;
    struct AddrInfo addrInfo[MAX_RESULTS];
    int32_t addrSize = 0;
    auto ret = GetQueryFailCause(nullptr, addrInfo, addrSize);
    EXPECT_EQ(ret, FAIL_CAUSE_NONE);

    dnsProcessInfo.retCode = 1;
    ret = GetQueryFailCause(&dnsProcessInfo, addrInfo, addrSize);
    EXPECT_EQ(ret, FAIL_CAUSE_QUERY_FAIL);

    dnsProcessInfo.retCode = 0;
    dnsProcessInfo.firstQueryEndDuration = QUERY_CALLBACK_RETURN_SLOW_THRESHOLD + 1;
    ret = GetQueryFailCause(&dnsProcessInfo, addrInfo, addrSize);
    EXPECT_EQ(ret, FAIL_CAUSE_FIRST_RETURN_SLOW);

    dnsProcessInfo.firstQueryEndDuration = QUERY_CALLBACK_RETURN_SLOW_THRESHOLD;
    dnsProcessInfo.firstQueryEnd2AppDuration = FIRST_RETURN_SLOW_THRESHOLD + 1;
    ret = GetQueryFailCause(&dnsProcessInfo, addrInfo, addrSize);
    EXPECT_TRUE(ret == FAIL_CAUSE_CALLBACK_RETURN_SLOW || ret == FAIL_CAUSE_FIRST_RETURN_SLOW);
}

HWTEST_F(NetsysClientTest, GetQueryFailCauseTest002, TestSize.Level1)
{
    struct DnsProcessInfoExt dnsProcessInfo;
    struct AddrInfo addrInfo[MAX_RESULTS];
    int32_t addrSize = 1;
    dnsProcessInfo.retCode = 0;
    dnsProcessInfo.firstQueryEndDuration = QUERY_CALLBACK_RETURN_SLOW_THRESHOLD;
    dnsProcessInfo.firstQueryEnd2AppDuration = FIRST_RETURN_SLOW_THRESHOLD;
    addrInfo[0].aiAddr.sa.sa_family = AF_INET;
    inet_pton(AF_INET, LOOP_BACK_ADDR1, &addrInfo[0].aiAddr.sin.sin_addr);
    auto ret = GetQueryFailCause(&dnsProcessInfo, addrInfo, addrSize);
    EXPECT_TRUE(ret == FAIL_CAUSE_RETURN_LOOPBACK_ADDR || ret == FAIL_CAUSE_FIRST_RETURN_SLOW);

    addrSize = 0;
    dnsProcessInfo.isFromCache = false;
    dnsProcessInfo.ipv4QueryInfo.cname = 1;
    dnsProcessInfo.ipv6QueryInfo.cname = 1;
    ret = GetQueryFailCause(&dnsProcessInfo, addrInfo, addrSize);
    EXPECT_TRUE(ret == FAIL_CAUSE_RETURN_CNAME || ret == FAIL_CAUSE_FIRST_RETURN_SLOW);

    dnsProcessInfo.ipv4QueryInfo.cname = 0;
    dnsProcessInfo.ipv6QueryInfo.cname = 0;
    dnsProcessInfo.retCode = 0;
    dnsProcessInfo.ipv4QueryInfo.isNoAnswer = 1;
    dnsProcessInfo.ipv6QueryInfo.isNoAnswer = 1;
    ret = GetQueryFailCause(&dnsProcessInfo, addrInfo, addrSize);

    dnsProcessInfo.ipv4QueryInfo.isNoAnswer = 0;
    ret = GetQueryFailCause(&dnsProcessInfo, addrInfo, addrSize);
}

HWTEST_F(NetsysClientTest, NetsysPostDnsAbnormalTest001, TestSize.Level1)
{
    int32_t failcause = 0;
    struct DnsCacheInfo dnsInfo;
    auto ret = NetsysPostDnsAbnormal(failcause, dnsInfo);
    ret = IsFailCauseAllowedReport(failcause);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetsysClientTest, HandleQueryAbnormalReportTest001, TestSize.Level1)
{
    struct DnsProcessInfoExt dnsProcessInfo;
    struct AddrInfo addrInfo[MAX_RESULTS];
    int32_t addrSize = 0;
    EXPECT_TRUE(IsSystemUid());
    HandleQueryAbnormalReport(dnsProcessInfo, addrInfo, addrSize);
}

HWTEST_F(NetsysClientTest, NetSysPostDnsQueryResultTest001, TestSize.Level1)
{
    int netId = 0;
    struct addrinfo addr;
    char srcAddr[MAX_RESULTS] = {0};
    struct DnsProcessInfo processInfo;
    auto ret = NetSysPostDnsQueryResult(netId, &addr, srcAddr, nullptr);
    EXPECT_EQ(ret, -1);

    processInfo.hostname = nullptr;
    ret = NetSysPostDnsQueryResult(netId, &addr, srcAddr, &processInfo);
    EXPECT_EQ(ret, -1);

    char hostname[MAX_RESULTS] = {0};
    processInfo.hostname = hostname;
    processInfo.retCode = 0;
    ret = NetSysPostDnsQueryResult(netId, nullptr, srcAddr, &processInfo);
    EXPECT_EQ(ret, 0);

    processInfo.retCode = 1;
    ret = NetSysPostDnsQueryResult(netId, &addr, nullptr, &processInfo);
    EXPECT_TRUE(ret == -1 || ret == 0);
}

HWTEST_F(NetsysClientTest, FillBasicAddrInfoTest001, TestSize.Level1)
{
    auto ret = FillBasicAddrInfo(NULL, NULL);
    EXPECT_EQ(ret, -1);
    
    struct AddrInfo addr;
    ret = FillBasicAddrInfo(&addr, NULL);
    EXPECT_EQ(ret, -1);

    struct addrinfo ai;
    ai.ai_addr = NULL;
    ai.ai_canonname = NULL;

    ai.ai_family = AF_INET6;
    ret = FillBasicAddrInfo(&addr, &ai);
    EXPECT_EQ(ret, 0);
    
    ai.ai_family = AF_INET;
    addr.aiAddr.sin.sin_addr.s_addr = 0;
    ret = FillBasicAddrInfo(&addr, &ai);
    EXPECT_EQ(ret, 0);
    
    ai.ai_family = AF_INET;
    addr.aiAddr.sin.sin_addr.s_addr = 1;
    ret = FillBasicAddrInfo(&addr, &ai);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysClientTest, NetSysGetResolvConfExtTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    struct ResolvConfigExt config;
    auto ret = NetSysGetResolvConfExt(netId, nullptr);
    EXPECT_EQ(ret, -EINVAL);

    SetNetForApp(1);
    ret = NetSysGetResolvConfExt(netId, &config);

    config.nameservers[0][0] = '\0';
    ret = NetSysGetResolvConfExt(netId, &config);
}

HWTEST_F(NetsysClientTest, NetSysIsIpv6EnableTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    SetNetForApp(1);
    auto ret = NetSysIsIpv6Enable(netId);
    EXPECT_GE(ret, -1);
}

HWTEST_F(NetsysClientTest, NetSysIsIpv4EnableTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    SetNetForApp(1);
    auto ret = NetSysIsIpv4Enable(netId);
    EXPECT_GE(ret, -1);
}

HWTEST_F(NetsysClientTest, NetSysSetNodataCacheTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    SetNetForApp(1);
    auto ret = NetSysSetNodataCache(netId, nullptr);
    EXPECT_EQ(ret, -EINVAL);

    ret = NetSysSetNodataCache(netId, "");
    EXPECT_EQ(ret, -EINVAL);

    ret = NetSysSetNodataCache(netId, "test.example.com");
    EXPECT_NE(ret, -EINVAL);
}

HWTEST_F(NetsysClientTest, NetSysGetNodataCacheTest001, TestSize.Level1)
{
    uint16_t netId = 0;
    SetNetForApp(1);
    auto ret = NetSysGetNodataCache(netId, nullptr);
    EXPECT_EQ(ret, 0);

    ret = NetSysGetNodataCache(netId, "");
    EXPECT_EQ(ret, 0);

    // First set the nodata cache, then get it
    ret = NetSysSetNodataCache(netId, "test.example.com");
    EXPECT_EQ(ret, 0);

    // Get the nodata cache that was just set
    ret = NetSysGetNodataCache(netId, "test.example.com");
    EXPECT_GE(ret, 0);
}

struct dnsserver* CreateDnsNode(int family, const char* ip, int protocol) {
    struct dnsserver* ds = (struct dnsserver*)malloc(sizeof(struct dnsserver));
    ds->query_protocol = protocol;
    ds->sa_next = NULL;
 
    if (family == AF_INET) {
        struct sockaddr_in* addr4 = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
        memset(addr4, 0, sizeof(struct sockaddr_in));
        addr4->sin_family = AF_INET;
        inet_pton(AF_INET, ip, &addr4->sin_addr);
        ds->sa = (struct sockaddr*)addr4;
    } else if (family == AF_INET6) {
        struct sockaddr_in6* addr6 = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6));
        memset(addr6, 0, sizeof(struct sockaddr_in6));
        addr6->sin6_family = AF_INET6;
        inet_pton(AF_INET6, ip, &addr6->sin6_addr);
        ds->sa = (struct sockaddr*)addr6;
    } else {
        struct sockaddr* sa_other = (struct sockaddr*)malloc(sizeof(struct sockaddr));
        sa_other->sa_family = family;
        ds->sa = sa_other;
    }
    return ds;
}
 
HWTEST_F(NetsysClientTest, FillBasicDnsServerInfo001, TestSize.Level1)
{
    auto ret = FillBasicDnsServerInfo(NULL, NULL);
    EXPECT_EQ(ret, -1);
    
    struct DnsServerInfo si;
    ret = FillBasicDnsServerInfo(&si, NULL);
    EXPECT_EQ(ret, -1);
 
    struct dnsserver ds;
    ds.query_protocol = 1;
    ds.sa_next = NULL;
    ds.sa = NULL;
    ret = FillBasicDnsServerInfo(&si, &ds);
    EXPECT_EQ(ret, -1);
}
 
HWTEST_F(NetsysClientTest, FillDnsServerInfo001, TestSize.Level1)
{
    struct dnsserver* head = CreateDnsNode(AF_INET, "1.1.1.1", 1);
    head->sa_next = CreateDnsNode(AF_INET6, "::1", 2);
 
    struct DnsServerInfo results[MAX_RESULTS];
    memset(results, 0, sizeof(results));
 
    int32_t count = FillDnsServerInfo(results, head);
    EXPECT_EQ(count, 2);
}
 
HWTEST_F(NetsysClientTest, FillDnsServerInfo002, TestSize.Level1)
{
    struct dnsserver* head = CreateDnsNode(999, "", 0);
 
    struct DnsServerInfo results[MAX_RESULTS];
    int32_t count = FillDnsServerInfo(results, head);
    EXPECT_EQ(count, 1);
}
 
HWTEST_F(NetsysClientTest, FillDnsServerInfo003, TestSize.Level1)
{
    struct dnsserver* head = CreateDnsNode(AF_INET, "1.2.3.4", 0);
    struct dnsserver* curr = head;
    for (int i = 0; i < MAX_RESULTS + 1; i++) {
        curr->sa_next = CreateDnsNode(AF_INET, "1.2.3.4", 0);
        curr = curr->sa_next;
    }
 
    struct DnsServerInfo results[MAX_RESULTS];
    int32_t count = FillDnsServerInfo(results, head);
 
    EXPECT_TRUE(count == MAX_RESULTS);
}
 
HWTEST_F(NetsysClientTest, FillQueryParam001, TestSize.Level1)
{
    struct queryparam qp;
    struct QueryParam dest;
    struct recordinfo record;
    EXPECT_EQ(FillQueryParam(&qp, &dest, &record), 0);
    EXPECT_EQ(FillQueryParam(&qp, &dest, NULL), 0);
}

} // namespace nmd
} // namespace OHOS
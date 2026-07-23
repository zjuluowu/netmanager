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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <arpa/inet.h>

#include "dns_config_client.h"
#include "dns_param_cache.h"
#include "net_conn_client.h"
#include "net_handle.h"
#include "netsys_client.h"
#ifdef USE_SELINUX
#include "selinux/selinux.h"
#endif
#include "dns_resolv_listen.h"

namespace OHOS::nmd {
const std::string PUBLIC_DNS_SERVER = "persist.sys.netsysnative_dns_servers_backup";
using namespace NetManagerStandard;
using namespace testing::ext;

class DnsResolvListenInternal {
public:
    DnsResolvListenInternal() = default;
    ~DnsResolvListenInternal()
    {
        if (serverSockFd_ > 0) {
            close(serverSockFd_);
        }
    }
    void StartListen();
    static void ProcGetConfigCommand(int clientSockFd, uint16_t netId, uint32_t uid);
    static void ProcGetConfigCommandExt(int clientSockFd, uint16_t netId, uint32_t uid);
#ifdef FEATURE_NET_FIREWALL_ENABLE
    static void ProcSetCacheCommand(const std::string &name, uint16_t netId, uint32_t callingUid,
                                    AddrInfoWithTtl addrInfo[MAX_RESULTS], uint32_t resNum);
    static void ProcGetCacheCommand(const std::string &name, int clientSockFd, uint16_t netId, uint32_t callingUid);
#endif
    static void ProcSetCacheCommand(const std::string &name, uint16_t netId, AddrInfoWithTtl addrInfo[MAX_RESULTS],
                                    uint32_t resNum);
    static void ProcGetCacheCommand(const std::string &name, int clientSockFd, uint16_t netId);
    static void ProcJudgeIpv6Command(int clientSockFd, uint16_t netId);
    static void ProcJudgeIpv4Command(int clientSockFd, uint16_t netId);
    static void ProcGetDefaultNetworkCommand(int clientSockFd);
    static void ProcBindSocketCommand(int32_t remoteFd, uint16_t netId);
    static void AddPublicDnsServers(ResolvConfig &sendData, size_t serverSize);
    static void AddPublicDnsServersExt(ResolvConfigExt &sendData, size_t serverSize);
    struct PostParam {
        uint32_t usedTime = 0;
        int32_t queryRet = 0;
        uint32_t aiSize = 0;
        QueryParam param{};
    };
    int32_t serverSockFd_ = -1;
};

class DnsResolvListenTest : public testing::Test {
protected:
    DnsResolvListen dnsResolvListen;
    DnsResolvListenInternal dnsResolvListenInternal;
    ResolvConfig createResolvConfig(int num);
};

ResolvConfig DnsResolvListenTest::createResolvConfig(int num)
{
    ResolvConfig resolvConfig;
    resolvConfig.error = 0; // 假设默认错误码为0
    resolvConfig.timeoutMs = 1000; // 假设默认超时时间为1000ms
    resolvConfig.retryCount = 3; // 假设默认重试次数为3
    resolvConfig.nonPublicNum = 1;

    // 将数字转换为字符串并赋值给nameservers
    for (int i = 0; i < num; ++i) {
        std::string numStr = std::to_string(i+1);
        int r = strcpy_s(resolvConfig.nameservers[i], sizeof(resolvConfig.nameservers[i]), numStr.c_str());
        if (r != 0) {
            printf("strcpy_s failed: %d\n", r);
        }
    }

    return resolvConfig;
}

HWTEST_F(DnsResolvListenTest, StartListen_ShouldStartListening_WhenCalled, TestSize.Level0)
{
    size_t serverSize = MAX_SERVER_NUM - 1;
    dnsResolvListen.StartListen();
    ASSERT_EQ(serverSize, 4);
}

/**
 * @tc.name  : AddPublicDnsServers_ShouldAddServer_01
 * @tc.number: DnsResolvListenTest_001
 * @tc.desc  : Test when serverSize is less than MAX_SERVER_NUM then AddPublicDnsServers should add server
 */
HWTEST_F(DnsResolvListenTest, AddPublicDnsServers_ShouldAddServer_01, TestSize.Level0)
{
    size_t serverSize = MAX_SERVER_NUM - 1;
    ResolvConfig sendData = createResolvConfig(serverSize);
    std::string dns(sendData.nameservers[serverSize]);
    EXPECT_EQ(dns, "");
    dnsResolvListenInternal.AddPublicDnsServers(sendData, serverSize);
    std::string dns1(sendData.nameservers[serverSize]);
}

/**
 * @tc.name  : AddPublicDnsServers_ShouldNotAddServer_01
 * @tc.number: DnsResolvListenTest_002
 * @tc.desc  : Test when serverSize is equal to MAX_SERVER_NUM then AddPublicDnsServers should not add server
 */
HWTEST_F(DnsResolvListenTest, AddPublicDnsServers_ShouldNotAddServer_01, TestSize.Level0)
{
    size_t serverSize = MAX_SERVER_NUM;
    ResolvConfig sendData = createResolvConfig(serverSize);
    std::string dns(sendData.nameservers[serverSize-1]);
    EXPECT_EQ(dns, "5");
    dnsResolvListenInternal.AddPublicDnsServers(sendData, serverSize);
    std::string dns1(sendData.nameservers[serverSize-1]);
    EXPECT_EQ(dns1, "5");
}

/**
 * @tc.name  : AddPublicDnsServers_ShouldNotAddServer_02
 * @tc.number: DnsResolvListenTest_003
 * @tc.desc  : Test when publicDnsServer already exists then AddPublicDnsServers should not add server
 */
HWTEST_F(DnsResolvListenTest, AddPublicDnsServers_ShouldNotAddServer_02, TestSize.Level0)
{
    ResolvConfig sendData = createResolvConfig(3);
    size_t serverSize = MAX_SERVER_NUM - 1;
    dnsResolvListenInternal.AddPublicDnsServers(sendData, serverSize);
    
    std::string dns_0(sendData.nameservers[0]);
    std::string dns_1(sendData.nameservers[1]);
    std::string dns_2(sendData.nameservers[2]);
    std::string dns_3(sendData.nameservers[3]);
    std::string dns_4(sendData.nameservers[4]);
    EXPECT_EQ(dns_0, "1");
    EXPECT_EQ(dns_1, "2");
    EXPECT_EQ(dns_2, "3");
    EXPECT_EQ(dns_3, "");
    
    std::string dns(sendData.nameservers[serverSize]);
    dnsResolvListenInternal.AddPublicDnsServers(sendData, serverSize);
    std::string dns1(sendData.nameservers[serverSize]);
    EXPECT_EQ(dns, dns1);
}

/**
 * @tc.name  : AddPublicDnsServersExt_ShouldNotAddServer_01
 * @tc.number: DnsResolvListenTest_002
 * @tc.desc  : Test when serverSize is equal to MAX_SERVER_NUM then AddPublicDnsServersExt should add server
 */
HWTEST_F(DnsResolvListenTest, AddPublicDnsServersExt_ShouldAddServer_01, TestSize.Level0)
{
    size_t serverSize = MAX_SERVER_NUM-1;
    ResolvConfigExt sendData = {0};
    std::string dns(sendData.nameservers[serverSize]);
    EXPECT_EQ(dns, "");
    dnsResolvListenInternal.AddPublicDnsServersExt(sendData, serverSize);
}

/**
 * @tc.name  : AddPublicDnsServersExt_ShouldNotAddServer_02
 * @tc.number: DnsResolvListenTest_003
 * @tc.desc  : Test when publicDnsServer already exists then AddPublicDnsServersExt should not add server
 */
HWTEST_F(DnsResolvListenTest, AddPublicDnsServersExt_ShouldNotAddServer_02, TestSize.Level0)
{
    ResolvConfigExt sendData = {0};
    size_t serverSize = MAX_SERVER_NUM;
    std::string dns(sendData.nameservers[0]);
    EXPECT_EQ(dns, "");
    dnsResolvListenInternal.AddPublicDnsServersExt(sendData, serverSize);
}

HWTEST_F(DnsResolvListenTest, ProcGetConfigCommand_ShouldHandleError_WhenGetResolverConfigFails, TestSize.Level0)
{
    int clientSockFd = 1;
    uint16_t netId = 0;
    uint32_t uid = 1000;
    dnsResolvListenInternal.ProcGetConfigCommand(clientSockFd, netId, uid);
    EXPECT_EQ(dnsResolvListenInternal.serverSockFd_, -1);
}

HWTEST_F(DnsResolvListenTest, ProcGetConfigCommandExt_ShouldHandleError_WhenGetResolverConfigFails, TestSize.Level0)
{
    int clientSockFd = 1;
    uint16_t netId = 0;
    uint32_t uid = 1000;
    dnsResolvListenInternal.ProcGetConfigCommandExt(clientSockFd, netId, uid);
    EXPECT_EQ(dnsResolvListenInternal.serverSockFd_, -1);
}

HWTEST_F(DnsResolvListenTest, ProcGetCacheCommand_ShouldReturnNull_WhenCacheIsEmpty_01, TestSize.Level0)
{
    std::string name = "testName";
    int clientSockFd = 1;
    uint16_t netId = 1;
    dnsResolvListenInternal.ProcGetCacheCommand(name, clientSockFd, netId);
    EXPECT_EQ(dnsResolvListenInternal.serverSockFd_, -1);
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
HWTEST_F(DnsResolvListenTest, ProcGetCacheCommand_ShouldReturnNull_WhenCacheIsEmpty_02, TestSize.Level0)
{
    std::string name = "testName";
    int clientSockFd = 1;
    uint16_t netId = 1;
    uint32_t callingUid = 1;
    dnsResolvListenInternal.ProcGetCacheCommand(name, clientSockFd, netId, callingUid);
    EXPECT_EQ(dnsResolvListenInternal.serverSockFd_, -1);
}
#endif

HWTEST_F(DnsResolvListenTest, ProcJudgeIpv6Command_EnableIpv6, TestSize.Level0)
{
    int clientSockFd = 1;
    uint16_t netId = 100;
    int enable = DnsParamCache::GetInstance().IsIpv6Enable(netId) ? 1 : 0;
    dnsResolvListenInternal.ProcJudgeIpv6Command(clientSockFd, netId);
    EXPECT_EQ(enable, 0);
}

HWTEST_F(DnsResolvListenTest, ProcGetDefaultNetworkCommand, TestSize.Level0)
{
    NetHandle netHandle;
    int clientSockFd = 1;
    dnsResolvListenInternal.ProcGetDefaultNetworkCommand(clientSockFd);
    EXPECT_EQ(netHandle.GetNetId(), 0);
}

HWTEST_F(DnsResolvListenTest, ProcBindSocketCommand, TestSize.Level0)
{
    int32_t remoteFd = 1;
    uint16_t netId = 100;
    dnsResolvListenInternal.ProcBindSocketCommand(remoteFd, netId);
    EXPECT_EQ(dnsResolvListenInternal.serverSockFd_, -1);
}

HWTEST_F(DnsResolvListenTest, ProcJudgeIpv4Command_EnableIpv4, TestSize.Level0)
{
    int clientSockFd = 1;
    uint16_t netId = 100;
    int enable = DnsParamCache::GetInstance().IsIpv4Enable(netId) ? 1 : 0;
    dnsResolvListenInternal.ProcJudgeIpv4Command(clientSockFd, netId);
    EXPECT_EQ(enable, 0);
}
}  // namespace OHOS::nmd
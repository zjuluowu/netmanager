/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include <dlfcn.h>

#include "dns_config_client.h"
#include "netnative_log_wrapper.h"
#include "netsys_client.h"
#include "netsys_native_service_proxy.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

using SetCache = int32_t (*)(uint16_t netId, struct ParamWrapper param, struct addrinfo *res);
using GetCache = int32_t (*)(uint16_t netId, struct ParamWrapper param,
                            struct AddrInfo addr_info[MAX_RESULTS],
                             uint32_t *num);
using GetConfig = int32_t (*)(uint16_t netId, struct ResolvConfig *config);

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
namespace {
sptr<NetsysNative::INetsysService> ConnManagerGetProxy()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return nullptr;
    }

    auto remote = samgr->GetSystemAbility(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    if (remote == nullptr) {
        return nullptr;
    }

    auto proxy = iface_cast<NetsysNative::INetsysService>(remote);
    if (proxy == nullptr) {
        return nullptr;
    }
    return proxy;
}
} // namespace
class ResolverConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ResolverConfigTest::SetUpTestCase() {}

void ResolverConfigTest::TearDownTestCase() {}

void ResolverConfigTest::SetUp() {}

void ResolverConfigTest::TearDown() {}

HWTEST_F(ResolverConfigTest, ResolverConfigTest001, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    int32_t ret = 0;
    ret = netsysNativeService->CreateNetworkCache(0);
    NETNATIVE_LOGE("NETSYS: CreateNetworkCache0   ret=%{public}d", ret);
    NETNATIVE_LOGE("NETSYS: SetResolverConfig0   ret=%{public}d", ret);
    NETNATIVE_LOGE("ResolverConfigTest001 ResolverConfigTest001 ResolverConfigTest001");
    EXPECT_EQ(ret, 0);
}

HWTEST_F(ResolverConfigTest, ResolverConfigTest002, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    int32_t ret = 0;
    ret = netsysNativeService->CreateNetworkCache(0);
    NETNATIVE_LOGE("ResolverConfigTest002 ResolverConfigTest002 ResolverConfigTest002");
    EXPECT_EQ(ret, 0);
}

HWTEST_F(ResolverConfigTest, ResolverConfigTest003, TestSize.Level1)
{
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    int32_t ret = 0;
    ret = netsysNativeService->CreateNetworkCache(0);
    NETNATIVE_LOGE("ResolverConfigTest003 ResolverConfigTest003 ResolverConfigTest003");
    EXPECT_EQ(ret, 0);
}

HWTEST_F(ResolverConfigTest, ResolverConfigTest004, TestSize.Level1)
{
    NETNATIVE_LOGI("ResolverConfigTest004 enter");
    OHOS::sptr<OHOS::NetsysNative::INetsysService> netsysNativeService = ConnManagerGetProxy();
    ASSERT_NE(netsysNativeService, nullptr);

    uint16_t netId = 1;
    uint16_t baseTimeoutMsec = 5000;
    uint8_t retryCount = 3;
    std::vector<std::string> servers = {"114.114.114.114", "8.8.8.8"};
    std::vector<std::string> domains = {"yahoo.com", "baidu.com"};

    uint16_t getBaseTimeoutMsec;
    uint8_t getRetryCount;
    std::vector<std::string> getServers;
    std::vector<std::string> getDomains;

    int res = netsysNativeService->CreateNetworkCache(netId);
    res = netsysNativeService->SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
    res = netsysNativeService->GetResolverConfig(netId, getServers, getDomains, getBaseTimeoutMsec,
                                                 getRetryCount);
    EXPECT_EQ(getBaseTimeoutMsec, baseTimeoutMsec);
    EXPECT_EQ(getRetryCount, retryCount);
    int serversNum = getServers.size() < servers.size() ? getServers.size() : servers.size();
    for (int i = 0; i < serversNum; i++) {
        EXPECT_EQ(servers[i], getServers[i]);
    }
    int domainsNum = getDomains.size() < domains.size() ? getDomains.size() : domains.size();
    for (int i = 0; i < domainsNum; i++) {
        EXPECT_EQ(domains[i], getDomains[i]);
    }
    res = netsysNativeService->DestroyNetworkCache(netId);
}
} // namespace NetsysNative
} // namespace OHOS

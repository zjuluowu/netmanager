/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#endif
#include "net_dns_resolve.h"
#include "net_conn_service.h"
#include "net_conn_constants.h"
#include "net_conn_types.h"
#include "net_mgr_log_wrapper.h"
#include "tiny_count_down_latch.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing;
using namespace testing::ext;
using namespace OHOS::NetManagerStandard;
constexpr uint32_t TEST_NETID = 999;
constexpr const char *TEST_STRING = "testString";
constexpr const char *TEST_DOMAIN = "connectivitycheck.platform.hicloud.com";
}

class DnsResolveTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DnsResolveTest::SetUpTestCase() {}

void DnsResolveTest::TearDownTestCase() {}

void DnsResolveTest::SetUp() {}

void DnsResolveTest::TearDown() {}

HWTEST_F(DnsResolveTest, StartDnsResolveTest001, TestSize.Level1)
{
    std::string domain = "";
    std::shared_ptr<TinyCountDownLatch> latch01 = nullptr;
    auto dnsResolve = std::make_shared<NetDnsResolve>(TEST_NETID, latch01, domain);;
    dnsResolve->StartDnsResolve();
    EXPECT_NE(dnsResolve, nullptr);

    auto latch02 = std::make_shared<TinyCountDownLatch>(1);
    auto dnsResolve2 = std::make_shared<NetDnsResolve>(TEST_NETID, latch02, domain);
    dnsResolve2->StartDnsResolve();
    EXPECT_EQ(latch02->GetCount(), 0);
}

HWTEST_F(DnsResolveTest, StartTest001, TestSize.Level1)
{
    std::string domain = "";
    std::shared_ptr<TinyCountDownLatch> latch = nullptr;
    auto dnsResolve = std::make_shared<NetDnsResolve>(TEST_NETID, latch, domain);;
    dnsResolve->Start();
    EXPECT_NE(dnsResolve, nullptr);
}

HWTEST_F(DnsResolveTest, GetAddrInfoTest001, TestSize.Level1)
{
    std::string domain = "";
    std::shared_ptr<TinyCountDownLatch> latch = nullptr;
    auto dnsResolve = std::make_shared<NetDnsResolve>(TEST_NETID, latch, domain);;
    dnsResolve->GetAddrInfo();
    EXPECT_EQ(dnsResolve->GetDnsResolveResultByType(), ",");
}

HWTEST_F(DnsResolveTest, GetAddrInfoTest002, TestSize.Level1)
{
    std::string domain = "invalid_domain";
    std::shared_ptr<TinyCountDownLatch> latch = nullptr;
    auto dnsResolve = std::make_shared<NetDnsResolve>(TEST_NETID, latch, domain);
    dnsResolve->GetAddrInfo();
    EXPECT_EQ(dnsResolve->GetDnsResolveResultByType(), ",");
}

HWTEST_F(DnsResolveTest, GetAddrInfoTest003, TestSize.Level1)
{
    std::string domain = TEST_DOMAIN;
    int32_t netId;
    NetConnService::GetInstance()->GetDefaultNet(netId);
    std::shared_ptr<TinyCountDownLatch> latch = nullptr;
    auto dnsResolve = std::make_shared<NetDnsResolve>(netId, latch, domain);
    dnsResolve->GetAddrInfo();
    EXPECT_NE(dnsResolve->GetDnsResolveResultByType(), "");
}

HWTEST_F(DnsResolveTest, GetDnsResolveResultByTypeTest001, TestSize.Level1)
{
    std::string domain = "";
    std::shared_ptr<TinyCountDownLatch> latch = nullptr;
    auto dnsResolve = std::make_shared<NetDnsResolve>(TEST_NETID, latch, domain);
    EXPECT_EQ(dnsResolve->GetDnsResolveResultByType(INetAddr::IpType::IPV4), dnsResolve->resolveResultIpv4_);
    EXPECT_EQ(dnsResolve->GetDnsResolveResultByType(INetAddr::IpType::IPV6), dnsResolve->resolveResultIpv6_);
    EXPECT_EQ(dnsResolve->GetDnsResolveResultByType(INetAddr::IpType::UNKNOWN),
        dnsResolve->resolveResultIpv4_ + "," + dnsResolve->resolveResultIpv6_);
}
} // namespace NetManagerStandard
} // namespace OHOS

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

#include "http_proxy.h"


namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;

class NetProxyFromStringTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetProxyFromStringTest::SetUpTestCase() {}

void NetProxyFromStringTest::TearDownTestCase() {}

void NetProxyFromStringTest::SetUp() {}

void NetProxyFromStringTest::TearDown() {}

HWTEST_F(NetProxyFromStringTest, ProxyToStringAndFromString001, TestSize.Level1)
{
    HttpProxy proxy {"192.168.1.1", 8080, {}};
    auto proxyStr = proxy.ToString();
    auto newProxyOpt = HttpProxy::FromString(proxyStr);
    ASSERT_TRUE(newProxyOpt.has_value());
    ASSERT_EQ(proxy, *newProxyOpt);
}

HWTEST_F(NetProxyFromStringTest, ProxyToStringAndFromString002, TestSize.Level1)
{
    HttpProxy proxy {"192.168.33.15", 8081, {"fake.domen", "another.domen"}};
    auto proxyStr = proxy.ToString();
    auto newProxyOpt = HttpProxy::FromString(proxyStr);
    ASSERT_TRUE(newProxyOpt.has_value());
    ASSERT_EQ(*newProxyOpt, proxy);
}

HWTEST_F(NetProxyFromStringTest, EmptyString, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("");
    ASSERT_FALSE(newProxyOpt.has_value());
}

HWTEST_F(NetProxyFromStringTest, InvalidString, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("not proxy");
    ASSERT_FALSE(newProxyOpt.has_value());
}

HWTEST_F(NetProxyFromStringTest, WithoutPort, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domen\t\t");
    ASSERT_FALSE(newProxyOpt.has_value());
}

HWTEST_F(NetProxyFromStringTest, WithoutDomain, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("\t0\t");
    ASSERT_TRUE(newProxyOpt.has_value());
    auto proxy = HttpProxy{"", 0, {}};
    ASSERT_EQ(*newProxyOpt, proxy);
}

HWTEST_F(NetProxyFromStringTest, ExclusionList001, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domain\t0\texclude.this");
    ASSERT_TRUE(newProxyOpt.has_value());
    auto proxy = HttpProxy{"fake.domain", 0, {"exclude.this"}};
    ASSERT_EQ(*newProxyOpt, proxy);
}

HWTEST_F(NetProxyFromStringTest, ExclusionList002, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domain\t0\texclude.this");
    ASSERT_TRUE(newProxyOpt.has_value());
    auto proxy = HttpProxy{"fake.domain", 0, {"exclude.this"}};
    ASSERT_EQ(*newProxyOpt, proxy);
}

HWTEST_F(NetProxyFromStringTest, ExclusionList003, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domain\t0\texclude.this,and.this");
    ASSERT_TRUE(newProxyOpt.has_value());
    auto proxy = HttpProxy{"fake.domain", 0, {"exclude.this", "and.this"}};
    ASSERT_EQ(*newProxyOpt, proxy);
}

HWTEST_F(NetProxyFromStringTest, ExclusionList004, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domain\t0\texclude.this,and.this,");
    ASSERT_TRUE(newProxyOpt.has_value());
    auto proxy = HttpProxy{"fake.domain", 0, {"exclude.this", "and.this"}};
    ASSERT_EQ(*newProxyOpt, proxy);
}

HWTEST_F(NetProxyFromStringTest, PortOutOfRange001, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domain\t-1\texclude.this,and.this");
    ASSERT_FALSE(newProxyOpt.has_value());
}

HWTEST_F(NetProxyFromStringTest, PortOutOfRange002, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domain\t70000\texclude.this,and.this");
    ASSERT_FALSE(newProxyOpt.has_value());
}

HWTEST_F(NetProxyFromStringTest, PortOutOfRange003, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domain\t4294967304\texclude.this,and.this");
    ASSERT_FALSE(newProxyOpt.has_value());
}

HWTEST_F(NetProxyFromStringTest, PortNotInteger, TestSize.Level1)
{
    auto newProxyOpt = HttpProxy::FromString("fake.domain\tportGoesHere\texclude.this,and.this");
    ASSERT_FALSE(newProxyOpt.has_value());
}
} // namespace NetManagerStandard
} // namespace OHOS
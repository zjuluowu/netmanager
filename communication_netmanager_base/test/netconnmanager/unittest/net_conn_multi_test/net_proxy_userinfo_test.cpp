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

#include "net_proxy_userinfo.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store.h"
#include "rdb_store_config.h"
#include "rdb_types.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *TEST_IPV4_ADDR = "127.0.0.1";
constexpr const char *PROXY_NAME = "123456789";
constexpr const int32_t PROXY_NAME_SIZE = 9;
}
using namespace testing::ext;

class NetProxyUserinfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetProxyUserinfoTest::SetUpTestCase() {}

void NetProxyUserinfoTest::TearDownTestCase() {}

void NetProxyUserinfoTest::SetUp() {}

void NetProxyUserinfoTest::TearDown() {}

HWTEST_F(NetProxyUserinfoTest, SaveHttpProxyUserAndPassTest001, TestSize.Level1)
{
    HttpProxy httpProxy = {TEST_IPV4_ADDR, 8080, {}};
    HttpProxy httpProxyForGet = {TEST_IPV4_ADDR, 8080, {}};
    SecureData name;
    name.append(PROXY_NAME, PROXY_NAME_SIZE);
    SecureData pwd;
    pwd.append(PROXY_NAME, PROXY_NAME_SIZE);
    httpProxy.SetUserName(name);
    httpProxy.SetPassword(pwd);
    NetProxyUserinfo::GetInstance().SaveHttpProxyHostPass(httpProxy);
    NetProxyUserinfo::GetInstance().GetHttpProxyHostPass(httpProxyForGet);
    EXPECT_EQ(httpProxyForGet.GetUsername().empty(), false);
}

HWTEST_F(NetProxyUserinfoTest, SaveHttpProxyHostPassTest001, TestSize.Level1)
{
    HttpProxy httpProxy;
    NetProxyUserinfo::GetInstance().SaveHttpProxyHostPass(httpProxy);
    EXPECT_EQ(httpProxy.GetUsername().empty(), true);
}

HWTEST_F(NetProxyUserinfoTest, GetHttpProxyHostPassTest001, TestSize.Level1)
{
    HttpProxy httpProxy;
    NetProxyUserinfo::GetInstance().GetHttpProxyHostPass(httpProxy);
    EXPECT_EQ(httpProxy.GetUsername().empty(), false);
}
} // namespace NetManagerStandard
} // namespace OHOS
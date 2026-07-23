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

#include "message_parcel.h"
#ifdef GTEST_API_
#define private public
#endif
#include "common_net_conn_callback_test.h"
#include "i_net_conn_callback.h"
#include "net_conn_client.h"
#include "net_conn_constants.h"
#include "net_factoryreset_callback_stub.h"
#include "net_interface_callback_stub.h"
#include "net_interface_config.h"
#include "net_manager_constants.h"
#include "netmanager_base_test_security.h"
#include "network.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class NetConnClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetConnClientTest::SetUpTestCase() {}

void NetConnClientTest::TearDownTestCase() {}

void NetConnClientTest::SetUp() {}

void NetConnClientTest::TearDown() {}
HWTEST_F(NetConnClientTest, EnableDistributedClientNet001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::string virnicAddr = "1.189.55.61";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    int32_t ret = NetConnClient::GetInstance().EnableDistributedClientNet(virnicAddr, virnicName, iif);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = NetConnClient::GetInstance().DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnClientTest, EnableDistributedServerNet001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::string dstAddr = "1.189.55.61";
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string gw = "0.0.0.0";
    int32_t ret = NetConnClient::GetInstance().EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    bool isServer = true;
    std::string virnicName = "virnic";
    ret = NetConnClient::GetInstance().DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

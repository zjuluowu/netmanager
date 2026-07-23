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

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "common_net_conn_callback_test.h"
#include "http_proxy.h"
#include "net_all_capabilities.h"
#include "net_conn_callback_stub.h"
#include "net_conn_client.h"
#include "net_conn_constants.h"
#include "net_conn_service.h"
#include "net_conn_types.h"
#include "net_factoryreset_callback_stub.h"
#include "net_interface_callback_stub.h"
#include "net_manager_center.h"
#include "net_mgr_log_wrapper.h"
#include "netsys_controller.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class NetConnServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetConnServiceTest::SetUpTestCase()
{
    NetConnService::GetInstance()->OnStart();
}

void NetConnServiceTest::TearDownTestCase() {}

void NetConnServiceTest::SetUp() {}

void NetConnServiceTest::TearDown() {}

HWTEST_F(NetConnServiceTest, EnableDistributedClientNet001, TestSize.Level1)
{
    std::string virnicAddr = "1.189.55.61";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    int32_t ret = NetConnService::GetInstance()->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);

    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = NetConnService::GetInstance()->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, EnableDistributedServerNet001, TestSize.Level1)
{
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";
    int32_t ret = NetConnService::GetInstance()->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);

    bool isServer = true;
    std::string virnicName = "virnic";
    ret = NetConnService::GetInstance()->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceTest, ActiveHttpProxy001, TestSize.Level1)
{
    NetConnService::GetInstance()->ActiveHttpProxy();
    EXPECT_FALSE(NetConnService::GetInstance()->isFallbackProbeWithProxy_);
}

} // namespace NetManagerStandard
} // namespace OHOS

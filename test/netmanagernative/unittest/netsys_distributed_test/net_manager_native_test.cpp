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

#include "net_manager_constants.h"
#include "net_manager_native.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
}

class NetManagerNativeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetManagerNative> instance_ = nullptr;
};

void NetManagerNativeTest::SetUpTestCase()
{
    instance_ = std::make_shared<NetManagerNative>();
}

void NetManagerNativeTest::TearDownTestCase() {}

void NetManagerNativeTest::SetUp() {}

void NetManagerNativeTest::TearDown() {}

HWTEST_F(NetManagerNativeTest, EnableDistributedClientNet001, TestSize.Level1)
{
    std::string virnicAddr = "1.189.55.61";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    auto ret = instance_->EnableDistributedClientNet(virnicAddr, virnicName, iif);

    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = instance_->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetManagerNativeTest, EnableDistributedServerNet001, TestSize.Level1)
{
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";
    auto ret = instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    bool isServer = true;
    std::string virnicName = "virnic";
    ret = instance_->DisableDistributedNet(isServer, virnicName, dstAddr);
}

HWTEST_F(NetManagerNativeTest, NetworkAddRoute001, TestSize.Level1)
{
    uint16_t netId = 123;
    std::string interfaceName = "wlan0";
    std::string destination = "1.189.55.61";
    std::string nextHop = "";
    bool isExcludeRoute = false;
    int32_t ret = instance_->NetworkAddRoute(netId, interfaceName, destination, nextHop, isExcludeRoute);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}
} // namespace nmd
} // namespace OHOS

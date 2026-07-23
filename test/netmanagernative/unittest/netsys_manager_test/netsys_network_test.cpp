/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "netsys_network.h"
#include "net_manager_constants.h"
#include "network_permission.h"
#include "physical_network.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace NetManagerStandard;
using namespace testing::ext;
std::string INTERFACE_NAME = "interface_name";
const uint16_t NET_ID = 2;
} // namespace

class NetsysNetworkTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetsysNetworkTest::SetUpTestCase() {}

void NetsysNetworkTest::TearDownTestCase() {}

void NetsysNetworkTest::SetUp() {}

void NetsysNetworkTest::TearDown() {}

HWTEST_F(NetsysNetworkTest, InterfaceTest001, TestSize.Level1)
{
    PhysicalNetwork physicNetwork(NET_ID, NetworkPermission::PERMISSION_NETWORK);
    NetsysNetwork *netsysNetwork = &physicNetwork;
    bool isExisted = netsysNetwork->ExistInterface(INTERFACE_NAME);
    EXPECT_FALSE(isExisted);

    int32_t ret = netsysNetwork->ClearInterfaces();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNetworkTest, InterfaceTest002, TestSize.Level1)
{
    PhysicalNetwork physicNetwork(NET_ID, NetworkPermission::PERMISSION_SYSTEM);
    NetsysNetwork *netsysNetwork = &physicNetwork;
    bool isExisted = netsysNetwork->ExistInterface(INTERFACE_NAME);
    EXPECT_FALSE(isExisted);

    int32_t ret = netsysNetwork->ClearInterfaces();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace nmd
} // namespace OHOS

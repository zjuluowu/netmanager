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

#include <algorithm>
#include <gtest/gtest.h>

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_controller.h"
#define private public
#define protected public
#include "interface_manager.h"
#include "physical_network.h"
#undef private
#undef protected

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace testing::ext;
using namespace OHOS::nmd;
using namespace OHOS::NetManagerStandard;
constexpr uint16_t TEST_NETID = 2000;
} // namespace
class PhysicalNetworkTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<PhysicalNetwork> instance_ = nullptr;
};

void PhysicalNetworkTest::SetUpTestCase()
{
    instance_ = std::make_shared<PhysicalNetwork>(TEST_NETID, NetworkPermission::PERMISSION_NETWORK);
}

void PhysicalNetworkTest::TearDownTestCase() {}

void PhysicalNetworkTest::SetUp() {}

void PhysicalNetworkTest::TearDown() {}

HWTEST_F(PhysicalNetworkTest, AddInterfaceTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("AddInterfaceTest001 enter");
    PhysicalNetwork physicNetwork(2, NetworkPermission::PERMISSION_NETWORK);
    std::string interfaceName1 = "wlan0";
    auto ifaceList = InterfaceManager::GetInterfaceNames();
    bool wlan0NotExist = std::find(ifaceList.begin(), ifaceList.end(), interfaceName1) == ifaceList.end();
    ASSERT_NE(wlan0NotExist, true);
    int32_t ret = physicNetwork.AddInterface(interfaceName1);
    EXPECT_EQ(ret, 0);
    physicNetwork.AddDefault();
    physicNetwork.interfaces_.insert(interfaceName1);
    ret = physicNetwork.AddInterface(interfaceName1);
    EXPECT_EQ(ret, 0);
    std::string interfaceName2 = "eth1";
    ifaceList = InterfaceManager::GetInterfaceNames();
    bool eth1Exist = std::find(ifaceList.begin(), ifaceList.end(), interfaceName2) != ifaceList.end();
    if (eth1Exist) {
        ret = physicNetwork.AddInterface(interfaceName2);
        EXPECT_EQ(ret, 0);
    }

    ret = physicNetwork.RemoveInterface(interfaceName1);
    physicNetwork.RemoveDefault();
    physicNetwork.RemoveInterface(interfaceName2);
    std::string interfaceName3 = "eth2";
    physicNetwork.RemoveInterface(interfaceName3);
    physicNetwork.IsPhysical();
    physicNetwork.GetPermission();
    physicNetwork.GetNetworkType();
}

HWTEST_F(PhysicalNetworkTest, AddInterfaceTest002, TestSize.Level1)
{
    std::string interfaceName = "test";
    auto ret = instance_->AddInterface(interfaceName);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    interfaceName = "wlan0";
    ret = instance_->AddInterface(interfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    interfaceName = "wlan1";
    auto ifaceList = InterfaceManager::GetInterfaceNames();
    bool wlan1Exist = std::find(ifaceList.begin(), ifaceList.end(), interfaceName) != ifaceList.end();
    ASSERT_NE(wlan1Exist, true);
    ret = instance_->AddInterface(interfaceName);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(PhysicalNetworkTest, AddInterfaceTest003, TestSize.Level1)
{
    instance_->interfaces_.clear();
    std::string interfaceName = "wlan0";
    instance_->isDefault_ = true;
    auto ret = instance_->AddInterface(interfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(PhysicalNetworkTest, AddInterfaceTest004, TestSize.Level1)
{
    instance_->interfaces_.clear();
    std::string interfaceName = "wlan0";
    instance_->isDefault_ = false;
    auto ret = instance_->AddInterface(interfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(PhysicalNetworkTest, RemoveInterfaceTest001, TestSize.Level1)
{
    std::string interfaceName = "test";
    auto ret = instance_->RemoveInterface(interfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    interfaceName = "wlan0";
    ret = instance_->RemoveInterface(interfaceName);
    EXPECT_TRUE(ret == NETMANAGER_ERROR || ret == NETMANAGER_SUCCESS);
    interfaceName = "wlan1";
    ret = instance_->RemoveInterface(interfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(PhysicalNetworkTest, RemoveInterfaceTest002, TestSize.Level1)
{
    instance_->interfaces_.clear();
    std::string interfaceName = "wlan0";
    instance_->isDefault_ = true;
    auto ret = instance_->AddInterface(interfaceName);
    ret = instance_->RemoveInterface(interfaceName);
    EXPECT_TRUE(ret == NETMANAGER_ERROR || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(PhysicalNetworkTest, RemoveInterfaceTest003, TestSize.Level1)
{
    instance_->interfaces_.clear();
    std::string interfaceName = "wlan0";
    instance_->isDefault_ = false;
    auto ret = instance_->AddInterface(interfaceName);
    ret = instance_->RemoveInterface(interfaceName);
    EXPECT_TRUE(ret == NETMANAGER_ERROR || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(PhysicalNetworkTest, RemoveInterfaceTest004, TestSize.Level1)
{
    instance_->interfaces_.clear();
    std::string interfaceName = "wlan0";
    instance_->isDefault_ = false;
    auto ret = instance_->RemoveInterface(interfaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(PhysicalNetworkTest, AddDefaultTest001, TestSize.Level1)
{
    instance_->AddDefault();
    EXPECT_TRUE(instance_->isDefault_);
    instance_->RemoveDefault();
    EXPECT_FALSE(instance_->isDefault_);
}

HWTEST_F(PhysicalNetworkTest, RemoveDefaultTest001, TestSize.Level1)
{
    instance_->RemoveDefault();
    EXPECT_FALSE(instance_->isDefault_);
}
} // namespace NetsysNative
} // namespace OHOS

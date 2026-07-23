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

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"

#define private public
#define protected public
#include "local_network.h"
#undef protected
#undef private

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace OHOS::nmd;
class LocalNetworkTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void LocalNetworkTest::SetUpTestCase() {}

void LocalNetworkTest::TearDownTestCase() {}

void LocalNetworkTest::SetUp() {}

void LocalNetworkTest::TearDown() {}

HWTEST_F(LocalNetworkTest, AddInterfaceTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("AddInterfaceTest001 enter");
    LocalNetwork localNetwork(1);
    std::string interfaceName = "waln0";
    int32_t ret = localNetwork.AddInterface(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    localNetwork.interfaces_.insert(interfaceName);
    ret = localNetwork.AddInterface(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = localNetwork.RemoveInterface(interfaceName);
    localNetwork.interfaces_.clear();
    ret = localNetwork.RemoveInterface(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
} // namespace NetsysNative
} // namespace OHOS

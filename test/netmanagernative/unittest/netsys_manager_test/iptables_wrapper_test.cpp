/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include <thread>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "iptables_wrapper.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace OHOS::nmd;
class IptablesWrapperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void IptablesWrapperTest::SetUpTestCase() {}

void IptablesWrapperTest::TearDownTestCase() {}

void IptablesWrapperTest::SetUp() {}

void IptablesWrapperTest::TearDown() {}

HWTEST_F(IptablesWrapperTest, RunCommandTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("RunCommandTest001 enter");
    std::shared_ptr<IptablesWrapper> wrapper = DelayedSingleton<IptablesWrapper>::GetInstance();
    std::string comdLine = "-L -n";
    std::string str = wrapper->RunCommandForRes(IpType::IPTYPE_IPV4, comdLine);
    const uint32_t waiteMS1 = 500;
    std::this_thread::sleep_for(std::chrono::milliseconds(waiteMS1));
    int32_t ret = wrapper->RunCommand(IpType::IPTYPE_IPV4, comdLine);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    const uint32_t waiteMS2 = 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(waiteMS2));
}

HWTEST_F(IptablesWrapperTest, RunCommandTest002, TestSize.Level1)
{
    std::shared_ptr<IptablesWrapper> wrapper = DelayedSingleton<IptablesWrapper>::GetInstance();
    IpType ipType = IpType::IPTYPE_IPV4;
    std::string comdLine = "-A INPUT -j LOG";
    auto ret = wrapper->RunCommand(ipType, comdLine);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(IptablesWrapperTest, RunCommandForResTest001, TestSize.Level1)
{
    std::shared_ptr<IptablesWrapper> wrapper = DelayedSingleton<IptablesWrapper>::GetInstance();
    IpType ipType = IpType::IPTYPE_IPV4;
    std::string comdLine = "-A INPUT -j LOG";
    auto ret = wrapper->RunCommandForRes(ipType, comdLine);
    EXPECT_EQ(ret, wrapper->result_);
}

HWTEST_F(IptablesWrapperTest, RunMutipleCommandsTest001, TestSize.Level1)
{
    std::shared_ptr<IptablesWrapper> wrapper = DelayedSingleton<IptablesWrapper>::GetInstance();
    IpType ipType = IpType::IPTYPE_IPV4;
    std::string comdLine = "-A INPUT -j LOG";
    std::vector<std::string> commands = {comdLine};
    auto ret = wrapper->RunMutipleCommands(ipType, commands);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(IptablesWrapperTest, RunCommandForResTest002, TestSize.Level1)
{
    std::shared_ptr<IptablesWrapper> wrapper = DelayedSingleton<IptablesWrapper>::GetInstance();
    IpType ipType = IpType::IPTYPE_IPV4;
    std::string comdLine = "-A INPUT -j LOG";
    wrapper->iptablesWrapperFfrtQueue_ = 0;
    auto ret = wrapper->RunCommandForRes(ipType, comdLine);
    EXPECT_EQ(ret, wrapper->result_);
}

HWTEST_F(IptablesWrapperTest, RunMutipleCommandsTest002, TestSize.Level1)
{
    std::shared_ptr<IptablesWrapper> wrapper = DelayedSingleton<IptablesWrapper>::GetInstance();
    IpType ipType = IpType::IPTYPE_IPV4;
    std::string comdLine = "-A INPUT -j LOG";
    std::vector<std::string> commands = {comdLine};
    wrapper->iptablesWrapperFfrtQueue_ = 0;
    auto ret = wrapper->RunMutipleCommands(ipType, commands);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(IptablesWrapperTest, RunRestoreCommands001, TestSize.Level1)
{
    std::shared_ptr<IptablesWrapper> wrapper = DelayedSingleton<IptablesWrapper>::GetInstance();
    IpType ipType = IpType::IPTYPE_IPV4V6;
    std::string cmd = "*filter\n";
    cmd.append("-A FORWARD -j ACCEPT\n");
    auto ret = wrapper->RunRestoreCommands(ipType, cmd);
    EXPECT_LE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    
    wrapper->iptablesWrapperFfrtQueue_ = 0;
    ret = wrapper->RunRestoreCommands(ipType, cmd);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}
} // namespace NetsysNative
} // namespace OHOS

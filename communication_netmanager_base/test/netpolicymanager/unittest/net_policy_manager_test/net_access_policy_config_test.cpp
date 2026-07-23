/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include <thread>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif
#include "net_access_policy_config.h"

namespace OHOS {
namespace NetManagerStandard {

using namespace testing::ext;
class NetAccessPolicyConfigUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetAccessPolicyConfigUtilsTest::SetUpTestCase() {}

void NetAccessPolicyConfigUtilsTest::TearDownTestCase() {}

void NetAccessPolicyConfigUtilsTest::SetUp() {}

void NetAccessPolicyConfigUtilsTest::TearDown() {}

HWTEST_F(NetAccessPolicyConfigUtilsTest, ReadFileTest001, TestSize.Level1)
{
    NetAccessPolicyConfigUtils config;
    std::string content;
    std::string path1 = "etc/netmanager/net_access_policy_config1.json";
    EXPECT_EQ(config.ReadFile(content, path1), false);
    
    std::string path2 = "etc/netmanager/net_access_policy_config.json";
}

HWTEST_F(NetAccessPolicyConfigUtilsTest, AddAndRemoveNetAccessPolicyConfigTest001, TestSize.Level1)
{
    std::vector<std::string> bundleNames = {"com.example.test01"};
    auto configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    int32_t num = configs.size();
    NetAccessPolicyConfigUtils::GetInstance().AddNetAccessPolicyConfig(bundleNames);
    configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    EXPECT_EQ(configs.size(), num + 1);

    NetAccessPolicyConfigUtils::GetInstance().RemoveNetAccessPolicyConfig(bundleNames);
    configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    EXPECT_EQ(configs.size(), num);
}

HWTEST_F(NetAccessPolicyConfigUtilsTest, AddAndRemoveNetAccessPolicyConfigTest002, TestSize.Level1)
{
    std::vector<std::string> bundleNames = {};
    auto configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    int32_t num = configs.size();
    NetAccessPolicyConfigUtils::GetInstance().AddNetAccessPolicyConfig(bundleNames);
    configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    EXPECT_EQ(configs.size(), num);

    NetAccessPolicyConfigUtils::GetInstance().RemoveNetAccessPolicyConfig(bundleNames);
    configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    EXPECT_EQ(configs.size(), num);
}

HWTEST_F(NetAccessPolicyConfigUtilsTest, AddAndRemoveNetAccessPolicyConfigTest003, TestSize.Level1)
{
    std::vector<std::string> existBundles = {"com.example.exist"};
    std::vector<std::string> notExistBundles = {"com.example.notexist"};
    auto configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    int32_t num = configs.size();
    NetAccessPolicyConfigUtils::GetInstance().AddNetAccessPolicyConfig(existBundles);
    configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    EXPECT_EQ(configs.size(), num + 1);

    NetAccessPolicyConfigUtils::GetInstance().AddNetAccessPolicyConfig(existBundles);
    configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    EXPECT_EQ(configs.size(), num + 1);

    NetAccessPolicyConfigUtils::GetInstance().RemoveNetAccessPolicyConfig(notExistBundles);
    configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    EXPECT_EQ(configs.size(), num + 1);
}
}
}
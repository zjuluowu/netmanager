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

#include "broadcast_manager.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
BroadcastInfo g_testInfo;
} // namespace

class BroadcastManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BroadcastManagerTest::SetUpTestCase()
{
    g_testInfo.action = "testAction";
    g_testInfo.data = "testData";
}

void BroadcastManagerTest::TearDownTestCase() {}

void BroadcastManagerTest::SetUp() {}

void BroadcastManagerTest::TearDown() {}

HWTEST_F(BroadcastManagerTest, SendBroadcastTest001, TestSize.Level1)
{
    std::map<std::string, bool> params = {{"test1", true}, {"test2", false}, {"test3", false}};
    bool ret = BroadcastManager::GetInstance().SendBroadcast(g_testInfo, params);
    EXPECT_TRUE(ret);
}

HWTEST_F(BroadcastManagerTest, SendBroadcastTest002, TestSize.Level1)
{
    std::map<std::string, int32_t> params = {{"test1", 1}, {"test2", 2}, {"test3", 3}};
    bool ret = BroadcastManager::GetInstance().SendBroadcast(g_testInfo, params);
    EXPECT_TRUE(ret);
}

HWTEST_F(BroadcastManagerTest, SendBroadcastTest003, TestSize.Level1)
{
    std::map<std::string, std::string> params = {{"test1", "testMsg1"}, {"test2", "testMsg2"}, {"test3", "testMsg3"}};
    bool ret = BroadcastManager::GetInstance().SendBroadcast(g_testInfo, params);
    EXPECT_TRUE(ret);
}

HWTEST_F(BroadcastManagerTest, SendBroadcastTest004, TestSize.Level1)
{
    std::map<std::string, std::string> params = {{"test1", "testMsg1"}, {"test2", "testMsg2"}, {"test3", "testMsg3"}};
    BroadcastInfo testInfo;
    bool ret = BroadcastManager::GetInstance().SendBroadcast(testInfo, params);
    EXPECT_FALSE(ret);
}

HWTEST_F(BroadcastManagerTest, SendBroadcastTest005, TestSize.Level1)
{
    std::map<std::string, int64_t> params = {{"test1", 1}, {"test2", 2}, {"test3", 3}};
    bool ret = BroadcastManager::GetInstance().SendBroadcast(g_testInfo, params);
    EXPECT_TRUE(ret);
}

HWTEST_F(BroadcastManagerTest, SendBroadcastTest006, TestSize.Level1)
{
    std::map<std::string, std::string> params = {{"test1", "testMsg1"}, {"test2", "testMsg2"}, {"test3", "testMsg3"}};
    BroadcastInfo info = {};
    g_testInfo.permission = "ohos.permission.INTERNET";
    bool ret = BroadcastManager::GetInstance().SendBroadcast(g_testInfo, params);
    EXPECT_TRUE(ret);
}
} // namespace NetManagerStandard
} // namespace OHOS
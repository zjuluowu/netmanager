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

#include "net_conn_event_handler.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
NetConnEventHandler::Callback g_testCallback = []() { std::cout << "Run event task" << std::endl; };
constexpr int64_t TEST_DELAY_TIME = 112;
constexpr const char *TEST_TASK_NAME = "test_task";
constexpr const char *TEST_RUNNER_CREATE = "RUNNER_CREATE";
} // namespace

class NetConnEventHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<AppExecFwk::EventRunner> runner_ =
        AppExecFwk::EventRunner::Create(TEST_RUNNER_CREATE);
    static inline std::shared_ptr<NetConnEventHandler> instance_ = std::make_shared<NetConnEventHandler>(runner_);
};

void NetConnEventHandlerTest::SetUpTestCase() {}

void NetConnEventHandlerTest::TearDownTestCase() {}

void NetConnEventHandlerTest::SetUp() {}

void NetConnEventHandlerTest::TearDown() {}

HWTEST_F(NetConnEventHandlerTest, PostAsyncTaskTest001, TestSize.Level1)
{
    bool ret = instance_->PostAsyncTask(g_testCallback, TEST_DELAY_TIME);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetConnEventHandlerTest, PostSyncTaskTest001, TestSize.Level1)
{
    instance_->RemoveAsyncTask(TEST_TASK_NAME);

    bool ret = instance_->PostSyncTask(g_testCallback);
    EXPECT_TRUE(ret);
}
} // namespace NetManagerStandard
} // namespace OHOS
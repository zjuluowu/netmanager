/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <ctime>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_manager_center.h"
#include "net_stats_callback_test.h"
#include "net_stats_constants.h"
#include "net_stats_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
#define DTEST_LOG std::cout << __func__ << ":" << __LINE__ << ":"
} // namespace

using namespace testing::ext;
class NetStatsServiceExceptionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetStatsService> instance_ = nullptr;
};

void NetStatsServiceExceptionTest::SetUpTestCase()
{
    instance_ = std::make_shared<NetStatsService>();
}

void NetStatsServiceExceptionTest::TearDownTestCase()
{
    instance_ = nullptr;
}

void NetStatsServiceExceptionTest::SetUp() {}

void NetStatsServiceExceptionTest::TearDown() {}

HWTEST_F(NetStatsServiceExceptionTest, DumpTest001, TestSize.Level1)
{
    int32_t testFd = 1001;
    std::vector<std::u16string> mockArgs;

    mockArgs.push_back(std::u16string(u"mockArg1"));
    mockArgs.push_back(std::u16string(u"mockArg2"));
    mockArgs.push_back(std::u16string(u"mockArg3"));
    mockArgs.push_back(std::u16string(u"mockArg4"));
    auto result = instance_->Dump(testFd, mockArgs);
    DTEST_LOG << "Dump result: " << result << std::endl;
    EXPECT_GE(result, -1);
}

HWTEST_F(NetStatsServiceExceptionTest, GetDumpMessageTest001, TestSize.Level1)
{
    std::string mockMessage;
    instance_->GetDumpMessage(mockMessage);
    EXPECT_FALSE(mockMessage.empty());
}

HWTEST_F(NetStatsServiceExceptionTest, InitTest001, TestSize.Level1)
{
    auto result = instance_->Init();
    EXPECT_FALSE(result);
}
} // namespace NetManagerStandard
} // namespace OHOS

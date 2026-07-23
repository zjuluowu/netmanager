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

#include "netsys_event_message.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
constexpr const char *TEST_DATA = "TEST_DATA";
} // namespace

class NetsysEventMessageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetsysEventMessage> instance_ = std::make_shared<NetsysEventMessage>();
};

void NetsysEventMessageTest::SetUpTestCase()
{
    instance_->PushMessage(NetsysEventMessage::Type::ADDRESS, TEST_DATA);
}

void NetsysEventMessageTest::TearDownTestCase() {}

void NetsysEventMessageTest::SetUp() {}

void NetsysEventMessageTest::TearDown() {}

HWTEST_F(NetsysEventMessageTest, GetMessageTest001, TestSize.Level1)
{
    std::string result = instance_->GetMessage(NetsysEventMessage::Type::ADDRESS);
    ASSERT_EQ(result, std::string(TEST_DATA));
}

HWTEST_F(NetsysEventMessageTest, GetMessageTest002, TestSize.Level1)
{
    instance_->DumpMessage();

    std::string result = instance_->GetMessage(NetsysEventMessage::Type::GATEWAY);
    ASSERT_TRUE(result.empty());
}
} // namespace nmd
} // namespace OHOS
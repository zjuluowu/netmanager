/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#endif

#include "data_receiver.h"
#include "netlink_define.h"

namespace OHOS {
namespace nmd {
namespace {
constexpr int32_t TEST_SOCKET = 112;
constexpr int32_t TEST_FORMAT = NetlinkDefine::NETLINK_FORMAT_BINARY_UNICAST;
using namespace testing::ext;
} // namespace

class DataReceiverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<DataReceiver> instance_ = std::make_shared<DataReceiver>(TEST_SOCKET, TEST_FORMAT);
};

void DataReceiverTest::SetUpTestCase() {}

void DataReceiverTest::TearDownTestCase() {}

void DataReceiverTest::SetUp() {}

void DataReceiverTest::TearDown() {}

HWTEST_F(DataReceiverTest, SocketErrorTest001, TestSize.Level1)
{
    int32_t testSocket = -1;
    std::unique_ptr<DataReceiver> receiver = std::make_unique<DataReceiver>(testSocket, TEST_FORMAT);
    ASSERT_NE(receiver, nullptr);
}

HWTEST_F(DataReceiverTest, FormatErrorTest002, TestSize.Level1)
{
    int32_t testFormat = 6;
    std::unique_ptr<DataReceiver> receiver = std::make_unique<DataReceiver>(TEST_SOCKET, testFormat);
    ASSERT_NE(receiver, nullptr);
}

HWTEST_F(DataReceiverTest, StartTest001, TestSize.Level1)
{
    DataReceiver::EventCallback callback = [](std::shared_ptr<NetsysEventMessage> msg) { (void)msg; };
    instance_->RegisterCallback(callback);

    int32_t ret = instance_->Start();
    EXPECT_EQ(ret, NetlinkResult::OK);
}

HWTEST_F(DataReceiverTest, StopTest001, TestSize.Level1)
{
    int32_t ret = instance_->Stop();
    EXPECT_EQ(ret, NetlinkResult::OK);
}

HWTEST_F(DataReceiverTest, DataReceiverBranchTest001, TestSize.Level1)
{
    int32_t socket = 0;
    instance_->StartReceive(socket);
    uid_t uid = 0;
    ssize_t count = instance_->ReceiveMessage(true, uid);
    EXPECT_LE(count, 0);
}
} // namespace nmd
} // namespace OHOS
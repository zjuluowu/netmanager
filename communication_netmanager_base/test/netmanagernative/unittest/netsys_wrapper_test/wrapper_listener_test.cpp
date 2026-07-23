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
#include <sys/socket.h>
#include <netinet/in.h>

#include "netlink_define.h"
#include "wrapper_listener.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
constexpr int32_t TEST_SOCKET = 56;
WrapperListener::RecvFunc g_func = [](int32_t socket) { std::cout << "Socket :" << socket << std::endl; };
} // namespace

class WrapperListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<WrapperListener> instance_ = std::make_shared<WrapperListener>(TEST_SOCKET, g_func);
};

void WrapperListenerTest::SetUpTestCase() {}

void WrapperListenerTest::TearDownTestCase() {}

void WrapperListenerTest::SetUp() {}

void WrapperListenerTest::TearDown() {}

HWTEST_F(WrapperListenerTest, StartTest001, TestSize.Level1)
{
    int32_t testSocket = -2;
    std::unique_ptr<WrapperListener> listener = std::make_unique<WrapperListener>(testSocket, g_func);
    auto ret = listener->Start();
    EXPECT_EQ(ret, NetlinkResult::ERROR);
    ret = listener->Stop();
    EXPECT_EQ(ret, NetlinkResult::ERROR);
}

HWTEST_F(WrapperListenerTest, StartTest002, TestSize.Level1)
{
    int32_t testSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    bind(testSocket, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
    std::unique_ptr<WrapperListener> listener = std::make_unique<WrapperListener>(testSocket, g_func);
    auto ret = listener->Start();
    EXPECT_EQ(ret, NetlinkResult::OK);
    ret = listener->Stop();
    EXPECT_EQ(ret, NetlinkResult::OK);
}
} // namespace nmd
} // namespace OHOS
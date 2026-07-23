/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "net_conn_constants.h"
#include "net_conn_types.h"
#include "net_handle.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class NetHandleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetHandleTest::SetUpTestCase() {}

void NetHandleTest::TearDownTestCase() {}

void NetHandleTest::SetUp() {}

void NetHandleTest::TearDown() {}

HWTEST_F(NetHandleTest, BindSocket001, TestSize.Level1)
{
    int32_t socketFd = 1;
    int32_t netId = 101;
    auto handler = DelayedSingleton<NetHandle>::GetInstance();
    handler->SetNetId(netId);
    int32_t result = handler->BindSocket(socketFd);
    ASSERT_TRUE(result == NETMANAGER_SUCCESS);
}

HWTEST_F(NetHandleTest, BindSocket002, TestSize.Level1)
{
    int32_t socketFd = -1;
    int32_t netId = 101;
    auto handler = DelayedSingleton<NetHandle>::GetInstance();
    handler->SetNetId(netId);
    int32_t result = handler->BindSocket(socketFd);
    ASSERT_TRUE(result == NETMANAGER_ERR_PARAMETER_ERROR);
}
} // namespace NetManagerStandard
} // namespace OHOS

/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#include "net_factoryreset_callback_test.h"

#include <iostream>
#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#endif

#include "net_factoryreset_callback.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;

NetFactoryResetCallbackTest::NetFactoryResetCallbackTest() {}

NetFactoryResetCallbackTest::~NetFactoryResetCallbackTest() {}


int32_t NetFactoryResetCallbackTest::OnNetFactoryReset()
{
    std::cout << "NetFactoryResetCallbackTest::OnNetFactoryReset"<< std::endl;
    return 0;
}

class NetFactoryResetCallbackBranchTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetFactoryResetCallbackBranchTest::SetUpTestCase() {}

void NetFactoryResetCallbackBranchTest::TearDownTestCase() {}

void NetFactoryResetCallbackBranchTest::SetUp() {}

void NetFactoryResetCallbackBranchTest::TearDown() {}

HWTEST_F(NetFactoryResetCallbackBranchTest, NetFactoryResetCallbackBranchTest001, TestSize.Level1)
{
    NetFactoryResetCallback callback;
    sptr<INetFactoryResetCallback> resetCallback = nullptr;
    int32_t ret = callback.UnregisterNetFactoryResetCallbackAsync(resetCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = callback.UnregisterNetFactoryResetCallback(resetCallback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

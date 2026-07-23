/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "net_info_observer.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
class NetInfoObserverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetInfoObserverTest::SetUpTestCase() {}

void NetInfoObserverTest::TearDownTestCase() {}

void NetInfoObserverTest::SetUp() {}

void NetInfoObserverTest::TearDown() {}

HWTEST_F(NetInfoObserverTest, NetConnectionPropertiesChangeTest001, TestSize.Level1)
{
    auto it = std::make_shared<NetInfoObserver>();
    auto netHandle = sptr<NetHandle>::MakeSptr();
    auto info = sptr<NetLinkInfo>::MakeSptr();
    int32_t ret = it->NetConnectionPropertiesChange(netHandle, info);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetInfoObserverTest, NetConnectionPropertiesChangeTest002, TestSize.Level1)
{
    auto it = std::make_shared<NetInfoObserver>();
    auto netHandle = sptr<NetHandle>::MakeSptr();
    auto info = sptr<NetLinkInfo>::MakeSptr();
    info->ident_ = "xx";
    int32_t ret = it->NetConnectionPropertiesChange(netHandle, info);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetInfoObserverTest, NetConnectionPropertiesChangeTest003, TestSize.Level1)
{
    auto it = std::make_shared<NetInfoObserver>();
    auto netHandle = sptr<NetHandle>::MakeSptr();
    auto info = nullptr;
    int32_t ret = it->NetConnectionPropertiesChange(netHandle, info);
    EXPECT_EQ(ret, -1);
}

}
}
}

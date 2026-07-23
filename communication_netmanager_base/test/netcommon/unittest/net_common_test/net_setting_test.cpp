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

#include "net_settings.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr uint32_t TEST_UID = 1001;
constexpr uint32_t TEST_UID_MAX = 666;
} // namespace

class NetSettingsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline NetSettings &instance_ = NetSettings::GetInstance();
};

void NetSettingsTest::SetUpTestCase()
{
    for (uint32_t i = 0; i < TEST_UID_MAX; i++) {
        instance_.AddSystemUid(i);
    }
}

void NetSettingsTest::TearDownTestCase() {}

void NetSettingsTest::SetUp() {}

void NetSettingsTest::TearDown() {}

HWTEST_F(NetSettingsTest, ConstructorTest001, TestSize.Level1)
{
    std::unique_ptr<NetSettings> settings = std::make_unique<NetSettings>();
    ASSERT_NE(settings, nullptr);
}

HWTEST_F(NetSettingsTest, IsUidForegroundTest001, TestSize.Level1)
{
    bool ret = instance_.IsUidForeground(TEST_UID);
    ASSERT_FALSE(ret);
}

HWTEST_F(NetSettingsTest, IsUidForegroundTest002, TestSize.Level1)
{
    uint32_t testUid = 1111;
    instance_.SetForegroundUid(testUid);
    bool ret = instance_.IsUidForeground(testUid);
    ASSERT_TRUE(ret);
}

HWTEST_F(NetSettingsTest, IsSystemTest001, TestSize.Level1) {
    uint32_t testSysUid = 500;
    bool ret = instance_.IsSystem(testSysUid);
    ASSERT_TRUE(ret);
}

HWTEST_F(NetSettingsTest, IsSystemTest002, TestSize.Level1) {
    bool ret = instance_.IsSystem(TEST_UID);
    ASSERT_FALSE(ret);
}

HWTEST_F(NetSettingsTest, RemoveSystemUidTest001, TestSize.Level1)
{
    uint32_t testSysUid = 100;
    instance_.RemoveSystemUid(testSysUid);
    bool ret = instance_.IsSystem(testSysUid);
    ASSERT_FALSE(ret);
}

HWTEST_F(NetSettingsTest, RouteUtilsBranchTest001, TestSize.Level1)
{
    uint32_t testSysUid = 0;
    instance_.RemoveSystemUid(testSysUid);
    bool ret = instance_.IsSystem(testSysUid);
    ASSERT_FALSE(ret);
}
} // namespace NetManagerStandard
} // namespace OHOS
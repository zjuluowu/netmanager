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

#include "netmanager_base_permission.h"
#include <ipc_skeleton.h>
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class NetManagerPermissionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetManagerPermissionTest::SetUpTestCase() {}

void NetManagerPermissionTest::TearDownTestCase() {}

void NetManagerPermissionTest::SetUp() {}

void NetManagerPermissionTest::TearDown() {}

HWTEST_F(NetManagerPermissionTest, CheckPermissionTest001, TestSize.Level1)
{
    auto ret = NetManagerPermission::CheckPermission({});
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerPermissionTest, CheckPermissionWithCacheTest001, TestSize.Level1)
{
    auto ret = NetManagerPermission::CheckPermissionWithCache({});
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerPermissionTest, CheckPermissionWithCacheTest002, TestSize.Level1)
{
    NetManagerBaseNoPermissionToken token;

    auto firstRet = NetManagerPermission::CheckPermissionWithCache(Permission::GET_NETWORK_INFO);
    auto secondRet = NetManagerPermission::CheckPermissionWithCache(Permission::GET_NETWORK_INFO);

    EXPECT_FALSE(firstRet);
    EXPECT_FALSE(secondRet);
}

HWTEST_F(NetManagerPermissionTest, CheckPermissionWithCacheTest003, TestSize.Level1)
{
    NetManagerBaseAccessToken token;

    auto firstRet = NetManagerPermission::CheckPermissionWithCache(Permission::GET_NETWORK_INFO);
    auto secondRet = NetManagerPermission::CheckPermissionWithCache(Permission::GET_NETWORK_INFO);

    EXPECT_TRUE(firstRet);
    EXPECT_TRUE(secondRet);
}

HWTEST_F(NetManagerPermissionTest, IsSystemCallerTest001, TestSize.Level1)
{
    auto ret = NetManagerPermission::IsSystemCaller();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetManagerPermissionTest, CheckNetSysInternalPermissionTest001, TestSize.Level1)
{
    auto ret = NetManagerPermission::CheckNetSysInternalPermission({});
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerPermissionTest, CheckNetSysInternalPermissionTest002, TestSize.Level1)
{
    auto ret = NetManagerPermission::CheckNetSysInternalPermission({"ohos.permission.MANAGE_VPN"});
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerPermissionTest, GetApiVersionTest001, TestSize.Level1)
{
    auto ret = NetManagerPermission::GetApiVersion();
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetManagerPermissionTest, CheckUidPermission001, TestSize.Level1)
{
    std::vector<uint32_t> allowedUids;
    bool ret = NetManagerPermission::CheckUidPermission(allowedUids);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerPermissionTest, CheckUidPermission002, TestSize.Level1)
{
    std::vector<uint32_t> allowedUids;
    allowedUids.push_back(IPCSkeleton::GetCallingUid());
    bool ret = NetManagerPermission::CheckUidPermission(allowedUids);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetManagerPermissionTest, CheckUidPermission003, TestSize.Level1)
{
    std::vector<uint32_t> allowedUids;
    allowedUids.push_back(0);
    allowedUids.push_back(IPCSkeleton::GetCallingUid());
    bool ret = NetManagerPermission::CheckUidPermission(allowedUids);
    EXPECT_TRUE(ret);
}
} // namespace NetManagerStandard
} // namespace OHOS
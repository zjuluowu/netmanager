/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include <memory>

#include "message_parcel.h"
#include "net_conn_constants.h"
#include "net_datashare_utils.h"
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;

constexpr const char *AIRPLANE_MODE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=airplane_mode";
constexpr const char *KEY_AIRPLANE_MODE = "settings.telephony.airplanemode";

std::unique_ptr<NetDataShareHelperUtils> netDataShareHelperUtils_ = nullptr;
class NetDataShareHelperUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetDataShareHelperUtilsTest::SetUpTestCase()
{
    netDataShareHelperUtils_ = std::make_unique<NetDataShareHelperUtils>();
}

void NetDataShareHelperUtilsTest::TearDownTestCase()
{
    netDataShareHelperUtils_.reset();
}

void NetDataShareHelperUtilsTest::SetUp() {}

void NetDataShareHelperUtilsTest::TearDown() {}

/**
 * @tc.name: InsertTest001
 * @tc.desc: Test NetDataShareHelperUtils::Insert
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, InsertTest001, TestSize.Level1)
{
    std::string airplaneMode = "1";
    Uri uri(AIRPLANE_MODE_URI);
    int32_t ret = netDataShareHelperUtils_->Insert(uri, KEY_AIRPLANE_MODE, airplaneMode);
    EXPECT_TRUE(ret == -1 || ret == 0);

    airplaneMode = "0";
    ret = netDataShareHelperUtils_->Insert(uri, KEY_AIRPLANE_MODE, airplaneMode);
    EXPECT_TRUE(ret == -1 || ret == 0);
}

/**
 * @tc.name: InsertTest002
 * @tc.desc: Test NetDataShareHelperUtils::Insert
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, InsertTest002, TestSize.Level1)
{
    NetManagerBaseDataShareToken token;
    std::string airplaneMode = "1";
    Uri uri(AIRPLANE_MODE_URI);
    int32_t ret = netDataShareHelperUtils_->Insert(uri, KEY_AIRPLANE_MODE, airplaneMode);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);

    airplaneMode = "0";
    ret = netDataShareHelperUtils_->Insert(uri, KEY_AIRPLANE_MODE, airplaneMode);
    ASSERT_FALSE(ret == NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UpdateTest001
 * @tc.desc: Test NetDataShareHelperUtils::Update
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, UpdateTest001, TestSize.Level1)
{
    std::string airplaneMode = "1";
    Uri uri(AIRPLANE_MODE_URI);
    int32_t ret = netDataShareHelperUtils_->Update(uri, KEY_AIRPLANE_MODE, airplaneMode);
    ASSERT_TRUE(ret == NETMANAGER_ERROR);

    airplaneMode = "0";
    ret = netDataShareHelperUtils_->Update(uri, KEY_AIRPLANE_MODE, airplaneMode);
    ASSERT_FALSE(ret == NETMANAGER_ERROR);
}

/**
 * @tc.name: UpdateTest002
 * @tc.desc: Test NetDataShareHelperUtils::Update
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, UpdateTest002, TestSize.Level1)
{
    NetManagerBaseDataShareToken token;
    std::string airplaneMode = "1";
    Uri uri(AIRPLANE_MODE_URI);
    int32_t ret = netDataShareHelperUtils_->Update(uri, KEY_AIRPLANE_MODE, airplaneMode);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);

    airplaneMode = "0";
    ret = netDataShareHelperUtils_->Update(uri, KEY_AIRPLANE_MODE, airplaneMode);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
}

/**
 * @tc.name: QueryTest001
 * @tc.desc: Test NetDataShareHelperUtils::Query
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, QueryTest001, TestSize.Level1)
{
    std::string airplaneMode;
    Uri uri(AIRPLANE_MODE_URI);
    int32_t ret = netDataShareHelperUtils_->Query(uri, KEY_AIRPLANE_MODE, airplaneMode);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
    std::cout << "QueryTest result:" << airplaneMode << std::endl;
}

/**
 * @tc.name: QueryTest002
 * @tc.desc: Test NetDataShareHelperUtils::Query
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, QueryTest002, TestSize.Level1)
{
    NetManagerBaseDataShareToken token;
    std::string airplaneMode;
    Uri uri(AIRPLANE_MODE_URI);
    int32_t ret = netDataShareHelperUtils_->Query(uri, KEY_AIRPLANE_MODE, airplaneMode);
    ASSERT_TRUE(ret == NETMANAGER_SUCCESS);
    std::cout << "QueryTest result:" << airplaneMode << std::endl;
}
} // namespace NetManagerStandard
} // namespace OHOS

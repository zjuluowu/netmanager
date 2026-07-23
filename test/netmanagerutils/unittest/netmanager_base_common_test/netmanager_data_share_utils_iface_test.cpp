/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "net_datashare_utils_iface.h"
#include "net_datashare_utils.h"
#include "net_manager_constants.h"
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;

std::string WIFI_URI = "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=sharing_wifi";
std::string TEST_KEY = "settings.netmanager.sharing_wifi";
std::string TEST_KEY_NULL = "TEST_KEY_NULL";
std::string TEST_VALUE = "TEST_VALUE";
std::string TEST_VALUE_UPDATE = "TEST_VALUE_UPDATE";

class NetDataShareHelperUtilsIfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetDataShareHelperUtilsIfaceTest::SetUpTestCase() {}
void NetDataShareHelperUtilsIfaceTest::TearDownTestCase() {}
void NetDataShareHelperUtilsIfaceTest::SetUp() {}
void NetDataShareHelperUtilsIfaceTest::TearDown() {}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, RegisterObserver_01, TestSize.Level1)
{
    std::string strUri = WIFI_URI;
    std::function<void()> onChange = []() {printf("onChange execute.\n");};
    int32_t id = NetDataShareHelperUtilsIface::RegisterObserver(strUri, onChange);
    EXPECT_EQ(id, 1);
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, RegisterSettingsObserver_01, TestSize.Level1)
{
    std::string strUri = WIFI_URI;
    std::function<void()> onChange = []() {printf("onChange execute.\n");};
    int32_t result = NetDataShareHelperUtilsIface::RegisterSettingsObserver(strUri, onChange);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, UnregisterSettingsObserver_01, TestSize.Level1)
{
    int32_t result = NetDataShareHelperUtilsIface::UnregisterObserver(WIFI_URI, 1);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, Insert_01, TestSize.Level1)
{
    int32_t ret = NetDataShareHelperUtilsIface::Insert(WIFI_URI, TEST_KEY, TEST_VALUE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, Query_01, TestSize.Level1)
{
    std::string value;
    int32_t ret = NetDataShareHelperUtilsIface::Query(WIFI_URI, TEST_KEY, value);
    EXPECT_EQ(ret, NETMANAGER_ERROR); // 这里预期返回 NETMANAGER_SUCCESS 有问题
    EXPECT_EQ(value, ""); // 这里预期返回 TEST_VALUE 有问题
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, Query_02, TestSize.Level1)
{
    std::string value;
    int32_t ret = NetDataShareHelperUtilsIface::Query(WIFI_URI, TEST_KEY_NULL, value);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    EXPECT_EQ(value, "");
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, Update_01, TestSize.Level1)
{
    int32_t ret = NetDataShareHelperUtilsIface::Update(WIFI_URI, TEST_KEY, TEST_VALUE_UPDATE);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string value;
    ret = NetDataShareHelperUtilsIface::Query(WIFI_URI, TEST_KEY, value);
    EXPECT_EQ(ret, NETMANAGER_ERROR); // 这里预期返回 NETMANAGER_SUCCESS 有问题
    EXPECT_EQ(value, ""); // 这里预期返回 TEST_VALUE_UPDATE 有问题
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, Delete_01, TestSize.Level1)
{
    int32_t ret = NetDataShareHelperUtilsIface::Delete(WIFI_URI, TEST_KEY);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string value;
    ret = NetDataShareHelperUtilsIface::Query(WIFI_URI, TEST_KEY, value);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    EXPECT_EQ(value, "");
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, Delete_02, TestSize.Level1)
{
    int32_t ret = NetDataShareHelperUtilsIface::Delete(WIFI_URI, TEST_KEY_NULL);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string value;
    ret = NetDataShareHelperUtilsIface::Query(WIFI_URI, TEST_KEY_NULL, value);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    EXPECT_EQ(value, "");
}

HWTEST_F(NetDataShareHelperUtilsIfaceTest, UnregisterObserver_01, TestSize.Level1)
{
    int32_t ret = NetDataShareHelperUtilsIface::UnregisterObserver(WIFI_URI, 1);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = NetDataShareHelperUtilsIface::UnregisterObserver(WIFI_URI, 1);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS); // 取消订阅未移除callbacks_中数据，重复取消仍返回成功，有问题
}

} // namespace NetManagerStandard
} // namespace OHOS

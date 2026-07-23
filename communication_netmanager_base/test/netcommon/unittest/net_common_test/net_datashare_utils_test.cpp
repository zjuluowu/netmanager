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

#include "net_datashare_utils.h"
#include "net_manager_constants.h"
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;

constexpr const char *SHARING_WIFI_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=sharing_wifi";
constexpr const char *SHARING_USB_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=sharing_usb";
constexpr const char *SHARING_BLUETOOTH_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=sharing_bluetooth";
constexpr const char *SHARING_NOTEXIST_TEST_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=not_exist_test";

constexpr const char *KEY_SHARING_WIFI = "settings.netmanager.sharing_wifi";
constexpr const char *KEY_SHARING_USB = "settings.netmanager.sharing_usb";
constexpr const char *KEY_SHARING_BLUETOOTH = "settings.netmanager.sharing_bluetooth";
constexpr const char *KEY_NOTEXIST_TEST = "settings.netmanager.not_exist_test";

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
    NetManagerBaseAccessToken token;

    Uri wifiUri(SHARING_WIFI_URI);
    Uri usbUri(SHARING_USB_URI);
    Uri bluetoothUri(SHARING_BLUETOOTH_URI);
    std::string on = "1";
    std::string off = "0";
    std::string status;

    // insert will be failed after insert success because data is exist
    int32_t ret = netDataShareHelperUtils_->Query(wifiUri, KEY_SHARING_WIFI, status);
    if (ret != NETMANAGER_SUCCESS) {
        ret = netDataShareHelperUtils_->Insert(wifiUri, KEY_SHARING_WIFI, on);
        EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    }

    ret = netDataShareHelperUtils_->Query(usbUri, KEY_SHARING_USB, status);
    if (ret != NETMANAGER_SUCCESS) {
        ret = netDataShareHelperUtils_->Insert(usbUri, KEY_SHARING_USB, on);
        EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    }

    ret = netDataShareHelperUtils_->Query(bluetoothUri, KEY_SHARING_BLUETOOTH, status);
    if (ret != NETMANAGER_SUCCESS) {
        ret = netDataShareHelperUtils_->Insert(bluetoothUri, KEY_SHARING_BLUETOOTH, on);
        EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    }

    // test for multi insert fail sence
    ret = netDataShareHelperUtils_->Insert(wifiUri, KEY_SHARING_WIFI, off);
    EXPECT_TRUE(ret == NETMANAGER_ERROR || ret == NETMANAGER_SUCCESS);
    ret = netDataShareHelperUtils_->Insert(usbUri, KEY_SHARING_USB, off);
    EXPECT_TRUE(ret == NETMANAGER_ERROR || ret == NETMANAGER_SUCCESS);
    ret = netDataShareHelperUtils_->Insert(bluetoothUri, KEY_SHARING_BLUETOOTH, off);
    EXPECT_TRUE(ret == NETMANAGER_ERROR || ret == NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UpdateTest001
 * @tc.desc: Test NetDataShareHelperUtils::Update
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, UpdateTest001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;

    std::string on = "1";
    Uri wifiUri(SHARING_WIFI_URI);
    int32_t ret = netDataShareHelperUtils_->Update(wifiUri, KEY_SHARING_WIFI, on);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);

    std::string off = "0";
    ret = netDataShareHelperUtils_->Update(wifiUri, KEY_SHARING_WIFI, off);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);

    Uri usbUri(SHARING_USB_URI);
    ret = netDataShareHelperUtils_->Update(usbUri, KEY_SHARING_USB, on);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);

    ret = netDataShareHelperUtils_->Update(usbUri, KEY_SHARING_USB, off);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);

    Uri bluetoothUri(SHARING_BLUETOOTH_URI);
    ret = netDataShareHelperUtils_->Update(bluetoothUri, KEY_SHARING_BLUETOOTH, on);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);

    ret = netDataShareHelperUtils_->Update(bluetoothUri, KEY_SHARING_BLUETOOTH, off);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
}

/**
 * @tc.name: UpdateTest002
 * @tc.desc: Test NetDataShareHelperUtils::Update
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, UpdateTest002, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::string status;
    std::string on = "1";
    std::string off = "0";

    Uri wifiUri(SHARING_WIFI_URI);
    int32_t ret = netDataShareHelperUtils_->Update(wifiUri, KEY_SHARING_WIFI, on);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    ret = netDataShareHelperUtils_->Query(wifiUri, KEY_SHARING_WIFI, status);
    EXPECT_NE(status, "123");

    ret = netDataShareHelperUtils_->Update(wifiUri, KEY_SHARING_WIFI, off);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    ret = netDataShareHelperUtils_->Query(wifiUri, KEY_SHARING_WIFI, status);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    EXPECT_NE(status, "123");

    Uri usbUri(SHARING_USB_URI);
    ret = netDataShareHelperUtils_->Update(usbUri, KEY_SHARING_USB, on);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    ret = netDataShareHelperUtils_->Query(wifiUri, KEY_SHARING_USB, status);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    EXPECT_NE(status, "123");
    ret = netDataShareHelperUtils_->Update(usbUri, KEY_SHARING_USB, on); // test for multi update
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    ret = netDataShareHelperUtils_->Query(wifiUri, KEY_SHARING_USB, status);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    EXPECT_NE(status, "123");

    ret = netDataShareHelperUtils_->Update(usbUri, KEY_SHARING_USB, off);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);

    Uri bluetoothUri(SHARING_BLUETOOTH_URI);
    ret = netDataShareHelperUtils_->Update(bluetoothUri, KEY_SHARING_BLUETOOTH, on);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);

    ret = netDataShareHelperUtils_->Update(bluetoothUri, KEY_SHARING_BLUETOOTH, off);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
}

/**
 * @tc.name: QueryTest001
 * @tc.desc: Test NetDataShareHelperUtils::Query
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, QueryTest001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;

    std::string status;
    Uri wifiUri(SHARING_WIFI_URI);
    int32_t ret = netDataShareHelperUtils_->Query(wifiUri, KEY_SHARING_WIFI, status);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    std::cout << "Wifi QueryTest result: " << status << std::endl;

    Uri usbUri(SHARING_USB_URI);
    ret = netDataShareHelperUtils_->Query(usbUri, KEY_SHARING_USB, status);
    std::cout << "Usb QueryTest result: " << status << std::endl;

    Uri bluetoothUri(SHARING_BLUETOOTH_URI);
    ret = netDataShareHelperUtils_->Query(bluetoothUri, KEY_SHARING_BLUETOOTH, status);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    std::cout << "Bluetooth QueryTest result: " << status << std::endl;
}

/**
 * @tc.name: QueryTest002
 * @tc.desc: Test NetDataShareHelperUtils::Query
 * @tc.type: FUNC
 */
HWTEST_F(NetDataShareHelperUtilsTest, QueryTest002, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::string status;
    std::string on = "1";
    std::string off = "0";

    Uri notExistTestUri(SHARING_NOTEXIST_TEST_URI);
    int32_t ret = netDataShareHelperUtils_->Query(notExistTestUri, KEY_NOTEXIST_TEST, status);
    std::cout << "Not Exist Test result: " << status << std::endl;

    // update include insert action
    ret = netDataShareHelperUtils_->Update(notExistTestUri, KEY_NOTEXIST_TEST, off);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);

    ret = netDataShareHelperUtils_->Query(notExistTestUri, KEY_NOTEXIST_TEST, status);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
    EXPECT_FALSE(status == off || status == on);
    std::cout << "Not Exist Test result: " << status << std::endl;
}

} // namespace NetManagerStandard
} // namespace OHOS

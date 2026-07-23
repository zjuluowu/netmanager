/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "net_manager_constants.h"
#include "ohos/init_data.h"
#include <gtest/gtest.h>

#define private public
#include "icu_helper.h"

namespace OHOS::NetManagerStandard {
namespace {
constexpr const char *UNASSIGNED_HOST_NAME = "www.\u08A0.com";
constexpr const char *UNASSIGNED_HOST_NAME_ASCII = "www.xn--pyb.com";
constexpr const char *UNICODE_HOST = "www.中国你好.com";
constexpr const char *ASCII_HOST = "www.xn--fiq02ab1ogvc.com";
constexpr const char *INVALID_HOST = "www..com";
const std::string MAX_HOST_CASE =
    std::string(63, 'a') + "." + std::string(63, 'b') + "." + std::string(63, 'c') + "." + std::string(63, 'd');
const std::string OVER_MAX_HOST_CASE =
    std::string(63, 'a') + "." + std::string(63, 'b') + "." + std::string(63, 'c') + "." + std::string(63, 'd') + ".";
} // namespace

using namespace testing::ext;

class ICUHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ICUHelperTest::SetUpTestCase()
{
    // Set the environment variable for ICU data directory.
    // This ensures ICU can locate the required .dat files under the system directory during test execution.
    SetHwIcuDirectory();
}

void ICUHelperTest::TearDownTestCase() {}

void ICUHelperTest::SetUp() {}

void ICUHelperTest::TearDown() {}

/**
 * @tc.name: GetDnsASCIITest001
 * @tc.desc: Test GetDnsASCII.
 * @tc.type: FUNC
 */
HWTEST_F(ICUHelperTest, GetDnsASCIITest001, TestSize.Level1)
{
    std::string ascii;
    auto ret = ICUHelper::GetDnsASCII(UNICODE_HOST, ConversionProcess::NO_CONFIGURATION, ascii);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(ascii, ASCII_HOST);
}

/**
 * @tc.name: GetDnsUnicodeTest001
 * @tc.desc: Test GetDnsUnicode.
 * @tc.type: FUNC
 */
HWTEST_F(ICUHelperTest, GetDnsUnicodeTest001, TestSize.Level1)
{
    std::string unicode;
    auto ret = ICUHelper::GetDnsUnicode(ASCII_HOST, ConversionProcess::NO_CONFIGURATION, unicode);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(unicode, UNICODE_HOST);
}

/**
 * @tc.name: ConvertIDNTest001
 * @tc.desc: Test ConvertIDN.
 * @tc.type: FUNC
 */
HWTEST_F(ICUHelperTest, ConvertIDNTest001, TestSize.Level1)
{
    std::string output;
    auto ret = ICUHelper::ConvertIDN(OVER_MAX_HOST_CASE, ConversionProcess::NO_CONFIGURATION, true, output);
    EXPECT_TRUE(ret == NETMANAGER_ERR_INVALID_PARAMETER);

    ret = ICUHelper::ConvertIDN("", ConversionProcess::NO_CONFIGURATION, true, output);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    ret = ICUHelper::ConvertIDN(INVALID_HOST, ConversionProcess::NO_CONFIGURATION, true, output);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

/**
 * @tc.name: ConvertIDNTest002
 * @tc.desc: Test ConvertIDN.
 * @tc.type: FUNC
 */
HWTEST_F(ICUHelperTest, ConvertIDNTest002, TestSize.Level1)
{
    std::string output;
    auto ret = ICUHelper::ConvertIDN(UNICODE_HOST, ConversionProcess::NO_CONFIGURATION, true, output);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(output, ASCII_HOST);
}

/**
 * @tc.name: ConvertIDNTest003
 * @tc.desc: Test ConvertIDN.
 * @tc.type: FUNC
 */
HWTEST_F(ICUHelperTest, ConvertIDNTest003, TestSize.Level1)
{
    std::string output;
    auto ret = ICUHelper::ConvertIDN(UNICODE_HOST, ConversionProcess::ALLOW_UNASSIGNED, true, output);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(output, ASCII_HOST);
}

/**
 * @tc.name: ConvertIDNTest004
 * @tc.desc: Test ConvertIDN.
 * @tc.type: FUNC
 */
HWTEST_F(ICUHelperTest, ConvertIDNTest004, TestSize.Level1)
{
    std::string output;
    auto ret = ICUHelper::ConvertIDN(UNICODE_HOST, ConversionProcess::USE_STD3_ASCII_RULES, true, output);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(output, ASCII_HOST);
}

/**
 * @tc.name: ConvertIDNTest005
 * @tc.desc: Test ConvertIDN.
 * @tc.type: FUNC
 */
HWTEST_F(ICUHelperTest, ConvertIDNTest005, TestSize.Level1)
{
    std::string output;
    auto ret = ICUHelper::ConvertIDN(UNASSIGNED_HOST_NAME, ConversionProcess::ALLOW_UNASSIGNED, true, output);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(output, UNASSIGNED_HOST_NAME_ASCII);

    ret = ICUHelper::ConvertIDN(UNASSIGNED_HOST_NAME, ConversionProcess::USE_STD3_ASCII_RULES, true, output);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}
} // namespace OHOS::NetManagerStandard

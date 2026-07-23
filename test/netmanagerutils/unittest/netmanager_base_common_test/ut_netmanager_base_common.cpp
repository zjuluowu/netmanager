/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <fstream>
#include <sys/stat.h>

#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *TEST_TEXT = "adfsjfjkfk^#$ajf!@!#$#kjd nck?fgnf<kdjnf>kjask?.fcnvdkjfn kjdkj.,.vd";
constexpr const char *SPLIT = "?";
constexpr const char *TEST_IP = "155.153.144.154";
constexpr const char *TEST_IPV4 = "534/6::45/144.15:4::44";
constexpr const char *TEST_DOMAIN1 = "123445";
constexpr const char *TEST_DOMAIN2 = ".com";
constexpr const char *TEST_DOMAIN3 = "test.com";
constexpr const char *TEST_DOMAIN4 = "testcom";
constexpr const char *TEST_DOMAIN5 = "com.test";
constexpr const char *TEST_DOMAIN6 = "test.co.uk";
constexpr const char *TEST_DOMAIN7 = "test.com.com";
constexpr const char *TEST_DOMAIN8 = "test1.test2.test3.test4.test5.com";
constexpr const char *DEFAULT_IPV6_ANY_INIT_ADDR = "::";

const std::string TEST_DOMAIN9 = "www.test.com";
const std::string TEST_DOMAIN10 = "*";
const std::string TEST_DOMAIN11 = "";
const std::string TEST_DOMAIN12 = "*.test.*";
const std::string TEST_DOMAIN13 = "*.test./{*";

constexpr int32_t MAX_IPV6_PREFIX_LENGTH = 128;
constexpr uint32_t ADDREDD_LEN = 16;
constexpr int32_t BIT_32 = 32;
constexpr int32_t BIT_24 = 24;
constexpr int32_t BIT_16 = 16;
constexpr int32_t BIT_8 = 8;
} // namespace
class UtNetmanagerBaseCommon : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};
void UtNetmanagerBaseCommon::SetUpTestCase() {}

void UtNetmanagerBaseCommon::TearDownTestCase() {}

void UtNetmanagerBaseCommon::SetUp() {}

void UtNetmanagerBaseCommon::TearDown() {}

/**
 * @tc.name: UtNetmanagerBaseCommon001
 * @tc.desc: Test UtNetmanagerBaseCommon ForkExec.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, UtNetmanagerBaseCommon001, TestSize.Level1)
{
    std::string out;
    CommonUtils::ForkExec("/system/bin/ls -a", &out);
    ASSERT_FALSE(out.empty());
    std::cout << "out: " << out << std::endl;
}

/**
 * @tc.name: UtNetmanagerBaseCommon002
 * @tc.desc: Test UtNetmanagerBaseCommon ForkExec.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, UtNetmanagerBaseCommon002, TestSize.Level1)
{
    std::string out;
    CommonUtils::ForkExec("/system/bin/ls -l", &out);
    ASSERT_FALSE(out.empty());
    std::cout << "out: " << out << std::endl;
}

/**
 * @tc.name: UtNetmanagerBaseCommon003
 * @tc.desc: Test UtNetmanagerBaseCommon ForkExec.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, UtNetmanagerBaseCommon003, TestSize.Level1)
{
    CommonUtils::ForkExec("/system/bin/mount -o rw,remount /");
    CommonUtils::ForkExec("/system/bin/mkdir uttest");
    std::string out;
    CommonUtils::ForkExec("/system/bin/ls -a", &out);
    ASSERT_TRUE(out.find("uttest") != std::string::npos);
    CommonUtils::ForkExec("/system/bin/rm -rf uttest");
}

/**
 * @tc.name: UtNetmanagerBaseCommon004
 * @tc.desc: Test UtNetmanagerBaseCommon ForkExec.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, UtNetmanagerBaseCommon004, TestSize.Level1)
{
    std::ofstream file;
    std::string filePath = "./run.sh";
    file.open(filePath, std::ios::out);
    if (file.is_open()) {
        file << "#!/bin/sh" << std::endl;
        file << "sleep 15" << std::endl;
        file.close();
    }
    chmod(filePath.c_str(), 0777);
    CommonUtils::ForkExec("./run.sh");
    std::string out;
    CommonUtils::ForkExec("./run.sh", &out);
    ASSERT_TRUE(out.empty());
}

/**
 * @tc.name: SplitTest001
 * @tc.desc: Test UtNetmanagerBaseCommon Split.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, SplitTest001, TestSize.Level1)
{
    std::vector<std::string> result = CommonUtils::Split(TEST_TEXT, SPLIT);
    ASSERT_FALSE(result.empty());
}

/**
 * @tc.name: SplitTest002
 * @tc.desc: Test UtNetmanagerBaseCommon Split.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, SplitTest002, TestSize.Level1)
{
    std::vector<std::string> result = CommonUtils::Split({}, SPLIT);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: StripTest001
 * @tc.desc: Test UtNetmanagerBaseCommon Strip.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, StripTest001, TestSize.Level1)
{
    auto result = CommonUtils::Strip(TEST_TEXT, '?');
    ASSERT_FALSE(result.empty());
}

/**
 * @tc.name: IsValidIPV4Test001
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidIPV4.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidIPV4Test001, TestSize.Level1)
{
    auto result = CommonUtils::IsValidIPV4(TEST_TEXT);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsValidIPV4Test002
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidIPV4.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidIPV4Test002, TestSize.Level1)
{
    auto result = CommonUtils::IsValidIPV4({});
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsValidIPV6Test001
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidIPV6.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidIPV6Test001, TestSize.Level1)
{
    auto result = CommonUtils::IsValidIPV6(TEST_TEXT);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsValidIPV6Test002
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidIPV6.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidIPV6Test002, TestSize.Level1)
{
    auto result = CommonUtils::IsValidIPV6({});
    ASSERT_FALSE(result);
}

/**
 * @tc.name: GetAddrFamilyTest001
 * @tc.desc: Test UtNetmanagerBaseCommon GetAddrFamily.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, GetAddrFamilyTest001, TestSize.Level1)
{
    auto result = CommonUtils::GetAddrFamily(TEST_IP);
    ASSERT_NE(result, 0);
}

/**
 * @tc.name: GetMaskLengthTest001
 * @tc.desc: Test UtNetmanagerBaseCommon GetMaskLength.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, GetMaskLengthTest001, TestSize.Level1)
{
    auto result = CommonUtils::GetMaskLength(TEST_TEXT);
    ASSERT_NE(result, 0);
}

/**
 * @tc.name: ConvertIpv4AddressTest001
 * @tc.desc: Test UtNetmanagerBaseCommon ConvertIpv4Address.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ConvertIpv4AddressTest001, TestSize.Level1)
{
    auto result = CommonUtils::ConvertIpv4Address(0);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: ConvertIpv4AddressTest002
 * @tc.desc: Test UtNetmanagerBaseCommon ConvertIpv4Address.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ConvertIpv4AddressTest002, TestSize.Level1)
{
    auto result = CommonUtils::ConvertIpv4Address(ADDREDD_LEN);
    ASSERT_FALSE(result.empty());
}

/**
 * @tc.name: ConvertIpv4AddressTest003
 * @tc.desc: Test UtNetmanagerBaseCommon ConvertIpv4Address.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ConvertIpv4AddressTest003, TestSize.Level1)
{
    auto result = CommonUtils::ConvertIpv4Address(TEST_IP);
    ASSERT_NE(result, static_cast<uint32_t>(0));
}

/**
 * @tc.name: ConvertIpv4AddressTest004
 * @tc.desc: Test UtNetmanagerBaseCommon ConvertIpv4Address.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ConvertIpv4AddressTest004, TestSize.Level1)
{
    std::string addr;
    auto result = CommonUtils::ConvertIpv4Address(addr);
    ASSERT_EQ(result, static_cast<uint32_t>(0));
}

/**
 * @tc.name: Ipv4PrefixLenTest001
 * @tc.desc: Test UtNetmanagerBaseCommon ConvertIpv4Address.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, Ipv4PrefixLenTest001, TestSize.Level1)
{
    std::string addr;
    auto result = CommonUtils::ConvertIpv4Address(addr);
    ASSERT_EQ(result, static_cast<uint32_t>(0));
}

/**
 * @tc.name: Ipv4PrefixLenTest002
 * @tc.desc: Test UtNetmanagerBaseCommon ConvertIpv4Address.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, Ipv4PrefixLenTest002, TestSize.Level1)
{
    auto result = CommonUtils::ConvertIpv4Address(TEST_IP);
    ASSERT_NE(result, static_cast<uint32_t>(0));
}

/**
 * @tc.name: ParseIntTest001
 * @tc.desc: Test UtNetmanagerBaseCommon ParseInt.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ParseIntTest001, TestSize.Level1)
{
    std::string testStr = "123";
    int32_t value = 0;
    auto result = CommonUtils::ParseInt(testStr, &value);
    ASSERT_NE(value, 0);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: ParseIntTest002
 * @tc.desc: Test UtNetmanagerBaseCommon ParseInt.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ParseIntTest002, TestSize.Level1)
{
    std::string testStr = "abcdfagdshrfsth";
    int32_t value = 0;
    auto result = CommonUtils::ParseInt(testStr, &value);
    ASSERT_EQ(value, 0);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: ParseIntTest003
 * @tc.desc: Test UtNetmanagerBaseCommon ParseInt.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ParseIntTest003, TestSize.Level1)
{
    std::string testStr = "44514564121561456745456891564564894";
    int32_t value = 0;
    auto result = CommonUtils::ParseInt(testStr, &value);
    ASSERT_EQ(value, 0);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: ParseIntTest004
 * @tc.desc: Test UtNetmanagerBaseCommon ParseInt.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ParseIntTest004, TestSize.Level1)
{
    std::string testStr = "-156423456123512423456146";
    int32_t value = 0;
    auto result = CommonUtils::ParseInt(testStr, &value);
    ASSERT_EQ(value, 0);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: ConvertToInt64Test001
 * @tc.desc: Test UtNetmanagerBaseCommon ConvertToInt64.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ConvertToInt64Test001, TestSize.Level1)
{
    std::string testStr = "145689";
    auto result = CommonUtils::ConvertToInt64(testStr);
    ASSERT_NE(result, 0);
}

/**
 * @tc.name: ToAnonymousIpTest001
 * @tc.desc: Test UtNetmanagerBaseCommon ToAnonymousIp.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ToAnonymousIpTest001, TestSize.Level1)
{
    auto result = CommonUtils::ToAnonymousIp(TEST_IPV4);
    ASSERT_FALSE(result.empty());
}

/**
 * @tc.name: ToAnonymousIpTest002
 * @tc.desc: Test UtNetmanagerBaseCommon ToAnonymousIp.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ToAnonymousIpTest002, TestSize.Level1)
{
    auto result = CommonUtils::ToAnonymousIp(TEST_IP);
    ASSERT_FALSE(result.empty());
}

/*
 * @tc.name: ToAnonymousIpTest003
 * @tc.desc: Test UtNetmanagerBaseCommon ToAnonymousIp.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ToAnonymousIpTest003, TestSize.Level1)
{
    auto result = CommonUtils::ToAnonymousIp({});
    ASSERT_TRUE(result.empty());
}

/*
 * @tc.name: ToAnonymousIpTest004
 * @tc.desc: Test UtNetmanagerBaseCommon ToAnonymousIp.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ToAnonymousIpTest004, TestSize.Level1)
{
    std::string testIpv4 = "12.34.45.56";
    auto result = CommonUtils::ToAnonymousIp(testIpv4, true);
    EXPECT_EQ(result, "12.**.**.56");
    auto result1 = CommonUtils::ToAnonymousIp(testIpv4);
    EXPECT_EQ(result1, "12.34.**.**");
    
    std::string testIpv6 = "ab:56:df:66:ac6:783:bdb5:8902";
    auto result2 = CommonUtils::ToAnonymousIp(testIpv6, true);
    EXPECT_NE(result2, "ab:**:**:**:***:***:***:8902");
    auto result3 = CommonUtils::ToAnonymousIp(testIpv6);
    EXPECT_EQ(result3, "ab:56:**:**:***:***:****:****");

    std::string testIpv4Mask = "12.34.45.56/24";
    result = CommonUtils::ToAnonymousIp(testIpv4Mask, true);
    EXPECT_EQ(result, "12.**.**.56/24");
    
    std::string testIpv6Mask = "ab:56:df:66:ac6:783:bdb5:8902/64";
    result2 = CommonUtils::ToAnonymousIp(testIpv6Mask, true);
    EXPECT_NE(result2, "ab:**:**:**:***:***:***:8902/64");
}

/*
 * @tc.name: MaskIpMiddleTest001
 * @tc.desc: Test UtNetmanagerBaseCommon MaskIpMiddle.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, MaskIpMiddleTest001, TestSize.Level1)
{
    std::string testIpv1 = "fe80::1";
    auto result1 = CommonUtils::ToAnonymousIp(testIpv1, true);
    EXPECT_EQ(result1, "****::*");

    std::string testIpv2 = "::1";
    auto result2 = CommonUtils::ToAnonymousIp(testIpv2, true);
    EXPECT_EQ(result2, "::*");

    std::string testIpv3 = "2001::";
    auto result3 = CommonUtils::ToAnonymousIp(testIpv3, true);
    EXPECT_EQ(result3, "****::");
}

/**
 * @tc.name: StrToIntTest001
 * @tc.desc: Test UtNetmanagerBaseCommon StrToInt.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, StrToIntTest001, TestSize.Level1)
{
    std::string testStr = "145689";
    auto result = CommonUtils::StrToInt(testStr);
    ASSERT_NE(result, 0);
}

/**
 * @tc.name: StrToUintTest001
 * @tc.desc: Test UtNetmanagerBaseCommon StrToUint.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, StrToUintTest001, TestSize.Level1)
{
    std::string testStr = "145689";
    auto result = CommonUtils::StrToUint(testStr);
    ASSERT_NE(result, static_cast<uint32_t>(0));
}

/**
 * @tc.name: StrToBoolTest001
 * @tc.desc: Test UtNetmanagerBaseCommon StrToBool.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, StrToBoolTest001, TestSize.Level1)
{
    std::string testStr = "145689";
    auto result = CommonUtils::StrToBool(testStr);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: StrToLongTest001
 * @tc.desc: Test UtNetmanagerBaseCommon StrToUint.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, StrToLongTest001, TestSize.Level1)
{
    std::string testStr = "145689";
    auto result = CommonUtils::StrToLong(testStr);
    ASSERT_NE(result, 0);
}

/**
 * @tc.name: IsValidDomainTest001
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidDomain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest001, TestSize.Level1)
{
    auto result = CommonUtils::IsValidDomain(TEST_DOMAIN1);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsValidDomainTest002
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidDomain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest002, TestSize.Level1)
{
    auto result = CommonUtils::IsValidDomain(TEST_DOMAIN2);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsValidDomainTest003
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidDomain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest003, TestSize.Level1)
{
    auto result = CommonUtils::IsValidDomain(TEST_DOMAIN3);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsValidDomainTest004
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidDomain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest004, TestSize.Level1)
{
    auto result = CommonUtils::IsValidDomain(TEST_DOMAIN4);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsValidDomainTest005
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidDomain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest005, TestSize.Level1)
{
    auto result = CommonUtils::IsValidDomain(TEST_DOMAIN5);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsValidDomainTest006
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidDomain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest006, TestSize.Level1)
{
    auto result = CommonUtils::IsValidDomain(TEST_DOMAIN6);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsValidDomainTest007
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidDomain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest007, TestSize.Level1)
{
    auto result = CommonUtils::IsValidDomain(TEST_DOMAIN7);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsValidDomainTest008
 * @tc.desc: Test UtNetmanagerBaseCommon IsValidDomain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest008, TestSize.Level1)
{
    auto result = CommonUtils::IsValidDomain(TEST_DOMAIN8);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: ToLowerTest008
 * @tc.desc: Test UtNetmanagerBaseCommon ToLower.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, ToLowerTest008, TestSize.Level1)
{
    std::string res = "HeLLo worLd";
    auto result = CommonUtils::ToLower(res);
    EXPECT_EQ(result, "hello world");
}

/**
 * @tc.name: GetMaskByLengthTest008
 * @tc.desc: Test UtNetmanagerBaseCommon GetMaskByLength.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, GetMaskByLengthTest008, TestSize.Level1)
{
    uint32_t length = 8;
    auto result = CommonUtils::GetMaskByLength(length);
    EXPECT_NE(result, "255.255.255.0");
}

/**
 * @tc.name: Ipv4PrefixLenTest008
 * @tc.desc: Test UtNetmanagerBaseCommon Ipv4PrefixLen.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, Ipv4PrefixLenTest008, TestSize.Level1)
{
    std::string ip = {};
    auto result = CommonUtils::Ipv4PrefixLen(ip);
    EXPECT_EQ(result, 0);
    ip = "192.168.0";
    result = CommonUtils::Ipv4PrefixLen(ip);
    EXPECT_EQ(result, 0);
    ip = "255.255.255.255";
    result = CommonUtils::Ipv4PrefixLen(ip);
    EXPECT_EQ(result, BIT_32);
    ip = "255.255.255.0";
    result = CommonUtils::Ipv4PrefixLen(ip);
    EXPECT_EQ(result, BIT_24);
    ip = "255.255.0.0";
    result = CommonUtils::Ipv4PrefixLen(ip);
    EXPECT_EQ(result, BIT_16);
    ip = "255.0.0.0";
    result = CommonUtils::Ipv4PrefixLen(ip);
    EXPECT_EQ(result, BIT_8);
    ip = "255.192.0.0";
    result = CommonUtils::Ipv4PrefixLen(ip);
    EXPECT_EQ(result, 10);
}

/**
 * @tc.name: StrToUint64Test008
 * @tc.desc: Test UtNetmanagerBaseCommon StrToUint64.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, StrToUint64Test008, TestSize.Level1)
{
    std::string value = {};
    uint64_t defaultErr = 0;
    auto result = CommonUtils::StrToUint64(value, defaultErr);
    EXPECT_EQ(result, defaultErr);
    value = "100";
    uint64_t value2 = 100;
    result = CommonUtils::StrToUint64(value, defaultErr);
    EXPECT_EQ(result, value2);
}

HWTEST_F(UtNetmanagerBaseCommon, Trim, TestSize.Level2)
{
    std::string str = "    trim   ";
    std::string strResult = CommonUtils::Trim(str);
    EXPECT_STREQ(strResult.c_str(), "trim");
}

HWTEST_F(UtNetmanagerBaseCommon, UrlRegexParse001, TestSize.Level2)
{
    bool isMatch = CommonUtils::UrlRegexParse(TEST_DOMAIN9, TEST_DOMAIN10);
    EXPECT_EQ(isMatch, true);
}

HWTEST_F(UtNetmanagerBaseCommon, UrlRegexParse002, TestSize.Level2)
{
    bool isMatch = CommonUtils::UrlRegexParse(TEST_DOMAIN9, TEST_DOMAIN11);
    EXPECT_EQ(isMatch, false);
}

HWTEST_F(UtNetmanagerBaseCommon, UrlRegexParse003, TestSize.Level2)
{
    bool isMatch = CommonUtils::UrlRegexParse(TEST_DOMAIN9, TEST_DOMAIN12);
    EXPECT_EQ(isMatch, true);
}

HWTEST_F(UtNetmanagerBaseCommon, UrlRegexParse004, TestSize.Level2)
{
    bool isMatch = CommonUtils::UrlRegexParse(TEST_DOMAIN9, TEST_DOMAIN9);
    EXPECT_EQ(isMatch, true);
}

HWTEST_F(UtNetmanagerBaseCommon, IsUrlRegexValid001, TestSize.Level2)
{
    bool isValid = CommonUtils::IsUrlRegexValid(TEST_DOMAIN12);
    EXPECT_EQ(isValid, true);
}

HWTEST_F(UtNetmanagerBaseCommon, IsUrlRegexValid002, TestSize.Level2)
{
    bool isValid = CommonUtils::IsUrlRegexValid(TEST_DOMAIN13);
    EXPECT_EQ(isValid, false);
}

HWTEST_F(UtNetmanagerBaseCommon, GetIpv6Prefix001, TestSize.Level2)
{
    std::string ipv6Addr = TEST_IP;
    uint8_t prefixLen = MAX_IPV6_PREFIX_LENGTH;
    auto result = CommonUtils::GetIpv6Prefix(ipv6Addr, prefixLen);
    EXPECT_EQ(result, ipv6Addr);

    prefixLen = MAX_IPV6_PREFIX_LENGTH - 1;
    result = CommonUtils::GetIpv6Prefix(ipv6Addr, prefixLen);
    EXPECT_EQ(result, DEFAULT_IPV6_ANY_INIT_ADDR);
}

HWTEST_F(UtNetmanagerBaseCommon, Ipv6PrefixLen001, TestSize.Level2)
{
    std::string ip = "";
    auto result = CommonUtils::Ipv6PrefixLen(ip);
    EXPECT_EQ(result, 0);

    result = CommonUtils::Ipv6PrefixLen(TEST_IP);
    EXPECT_EQ(result, 0);
}

HWTEST_F(UtNetmanagerBaseCommon, HasInternetPermission001, TestSize.Level2)
{
    bool result = CommonUtils::HasInternetPermission();
    EXPECT_TRUE(result);
}

HWTEST_F(UtNetmanagerBaseCommon, CheckIfaceName001, TestSize.Level2)
{
    std::string name = "";
    bool result = CommonUtils::CheckIfaceName(name);
    EXPECT_FALSE(result);

    name = TEST_TEXT;
    result = CommonUtils::CheckIfaceName(name);
    EXPECT_FALSE(result);
}

/**
 * @tc.name  : ExtractMetaRefreshUrl_ShouldReturnUrl_WhenHttpEquivRefreshFound
 * @tc.number: ExtractMetaRefreshUrlTest_001
 * @tc.desc  : 测试当 HTML 内容中包含 `http-equiv="refresh"` 时,函数应返回正确的 URL
 */
HWTEST_F(UtNetmanagerBaseCommon,
    ATC_ExtractMetaRefreshUrl_ShouldReturnUrl_WhenHttpEquivRefreshFound, TestSize.Level2)
{
    std::string htmlContent = R"(<html>
<head>
<meta http-equiv="refresh" content="0;url=https://example.com">
</head>
<body>
</body>
</html>)";
 
    std::string expectedUrl = "https://example.com";
    std::string actualUrl = CommonUtils::ExtractMetaRefreshUrl(htmlContent);
 
    EXPECT_EQ(actualUrl, expectedUrl);
}
 
/**
 * @tc.name  : ExtractMetaRefreshUrl_ShouldReturnUrl_WhenHttpEquivRefreshFoundWithDifferentCase
 * @tc.number: ExtractMetaRefreshUrlTest_002
 * @tc.desc  : 测试当 HTML 内容中包含 `http-equiv='refresh'` 时,函数应返回正确的 URL
 */
HWTEST_F(UtNetmanagerBaseCommon,
    ATC_ExtractMetaRefreshUrl_ShouldReturnUrl_WhenHttpEquivRefreshFoundWithDifferentCase, TestSize.Level2)
{
    std::string htmlContent = R"(<html>
<head>
<meta http-equiv='refresh' content="0;url=https://example.com">
</head>
<body>
</body>
</html>)";
 
    std::string expectedUrl = "https://example.com";
    std::string actualUrl = CommonUtils::ExtractMetaRefreshUrl(htmlContent);
 
    EXPECT_EQ(actualUrl, expectedUrl);
}
 
/**
 * @tc.name  : ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenHttpEquivNotFound
 * @tc.number: ExtractMetaRefreshUrlTest_003
 * @tc.desc  : 测试当 HTML 内容中不包含 `http-equiv="refresh"` 或 `http-equiv='refresh'` 时,函数应返回空字符串
 */
HWTEST_F(UtNetmanagerBaseCommon,
    ATC_ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenHttpEquivNotFound, TestSize.Level2)
{
    std::string htmlContent = R"(<html>
<head>
<meta http-equiv="other" content="0">
</head>
<body>
</body>
</html>)";
 
    std::string expectedUrl = "";
    std::string actualUrl = CommonUtils::ExtractMetaRefreshUrl(htmlContent);
 
    EXPECT_EQ(actualUrl, expectedUrl);
}
 
/**
 * @tc.name  : ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenContentNotFound
 * @tc.number: ExtractMetaRefreshUrlTest_004
 * @tc.desc  : 测试当 HTML 内容中不包含 `content=` 时,函数应返回空字符串
 */
HWTEST_F(UtNetmanagerBaseCommon,
    ATC_ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenContentNotFound, TestSize.Level0)
{
    std::string htmlContent = R"(<html>
<head>
<meta http-equiv="refresh" content="0">
</head>
<body>
</body>
</html>)";
 
    std::string expectedUrl = "";
    std::string actualUrl = CommonUtils::ExtractMetaRefreshUrl(htmlContent);
 
    EXPECT_EQ(actualUrl, expectedUrl);
}
 
 
/**
 * @tc.name  : ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenContentNotFound
 * @tc.number: ExtractMetaRefreshUrlTest_005
 * @tc.desc  : 测试当 HTML 内容中不包含 `content=` 时,函数应返回空字符串
 */
HWTEST_F(UtNetmanagerBaseCommon,
    ATC_ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenContentNotFound1, TestSize.Level0)
{
    std::string htmlContent = R"(<html>
<head>
<meta http-equiv="refresh" other="0">
</head>
<body>
</body>
</html>)";
 
    std::string expectedUrl = "";
    std::string actualUrl = CommonUtils::ExtractMetaRefreshUrl(htmlContent);
 
    EXPECT_EQ(actualUrl, expectedUrl);
}
 
/**
 * @tc.name  : ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenUrlNotFound
 * @tc.number: ExtractMetaRefreshUrlTest_006
 * @tc.desc  : 测试当 HTML 内容中不包含 `url=` 时,函数应返回空字符串
 */
HWTEST_F(UtNetmanagerBaseCommon,
    ATC_ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenUrlNotFound, TestSize.Level0)
{
    std::string htmlContent = R"(<html>
<head>
<meta http-equiv="refresh" content="0;other='https://example.com'">
</head>
<body>
</body>
</html>)";
 
    std::string expectedUrl = "";
    std::string actualUrl = CommonUtils::ExtractMetaRefreshUrl(htmlContent);
 
    EXPECT_EQ(actualUrl, expectedUrl);
}
 
/**
 * @tc.name  : ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenUrlNotFound
 * @tc.number: ExtractMetaRefreshUrlTest_007
 * @tc.desc  : 测试当 HTML 内容中不包含 `'"` 时,函数应返回空字符串
 */
HWTEST_F(UtNetmanagerBaseCommon,
    ATC_ExtractMetaRefreshUrl_ShouldReturnEmptyString_WhenUrlNotFound1, TestSize.Level0)
{
    std::string htmlContent = R"(<html>
<head>
<meta http-equiv="refresh" content=0;url=https://example.com)";
 
    std::string expectedUrl = "";
    std::string actualUrl = CommonUtils::ExtractMetaRefreshUrl(htmlContent);
 
    EXPECT_EQ(actualUrl, expectedUrl);
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL01, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL("https:////www.example.com?data_string");
    EXPECT_STREQ(hostname.c_str(), "www.example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL02, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL(R"(https:/\\\\\\///\\/www.example.com?data_string)");
    EXPECT_STREQ(hostname.c_str(), "www.example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL03, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL("");
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL04, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL("https://");
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL05, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL("https://www.example.com:8080");
    EXPECT_STREQ(hostname.c_str(), "www.example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL06, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL("https://www.example.com/for/test");
    EXPECT_STREQ(hostname.c_str(), "www.example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL07, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL(R"(https:/\\\\\)");
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL08, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL(R"(https://www.example.com/watch/80033982:sadsda?dd\\\df)");
    EXPECT_STREQ(hostname.c_str(), "www.example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL09, TestSize.Level2)
{
    std::string hostname =
        CommonUtils::GetHostnameFromURL(R"(https://www.example.com:8080/watch/80033982:sadsda?dd\\\df)");
    EXPECT_STREQ(hostname.c_str(), "www.example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL10, TestSize.Level2)
{
    std::string url = "example.com:98421/dsdsd?dsdsds";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL11, TestSize.Level2)
{
    std::string url = R"(\/\/\/\/\/\/\/\////\)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL12, TestSize.Level2)
{
    std::string url = "http://www.example.com";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "www.example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL13, TestSize.Level2)
{
    std::string url = "https://www.example-test.com";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "www.example-test.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL14, TestSize.Level2)
{
    std::string url = "ftp://www.baidu-test.com";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "www.baidu-test.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL15, TestSize.Level2)
{
    std::string url = R"(\\\/\/\/\/\/\///\/\\\:80808)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL16, TestSize.Level2)
{
    std::string url = R"(?????DSdsafhu34r3urihiu45t794\\56y&^&*%$^&$&*&^%*&((*)))";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL17, TestSize.Level2)
{
    std::string url = R"(16456465221-*/*/\)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "16456465221-*");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL18, TestSize.Level2)
{
    std::string url = "czvxkhcvjhkgfidkh";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "czvxkhcvjhkgfidkh");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL19, TestSize.Level2)
{
    std::string url = "hcd   dfdf4efd446576";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "hcd   dfdf4efd446576");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL20, TestSize.Level2)
{
    std::string url = " ";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), " ");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL21, TestSize.Level2)
{
    std::string url = "                             ";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "                             ");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL22, TestSize.Level2)
{
    std::string url = R"(dsd!!!@@#$$%%%^df\\)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "dsd!!!@@#$$%%%^df");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL23, TestSize.Level2)
{
    std::string url = "http://example.com";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL24, TestSize.Level2)
{
    std::string url = "example.com";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "example.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL25, TestSize.Level2)
{
    std::string url = "https:////??::||///stackoverflow.com";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL26, TestSize.Level2)
{
    std::string url = R"(https://\\\154545\\\stackoverflow.com)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "154545");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL27, TestSize.Level2)
{
    std::string url = R"(https://\\\\\\////\\\\stackoverflow.com)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "stackoverflow.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL28, TestSize.Level2)
{
    std::string url = R"(https:/\151\\\\23243435)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "151");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL29, TestSize.Level2)
{
    std::string url = R"(https:\\)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL30, TestSize.Level2)
{
    std::string url = R"(""""\\"""""""""""""")";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), R"("""")");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL31, TestSize.Level2)
{
    std::string url = ":::::::dfsfd::::::::::::";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL32, TestSize.Level2)
{
    std::string url = "1--**--4545";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "1--**--4545");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL33, TestSize.Level2)
{
    std::string url = R"( https:\)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), " https");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL34, TestSize.Level2)
{
    std::string url = " https:////";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL35, TestSize.Level2)
{
    std::string url = " saasa";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), " saasa");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL36, TestSize.Level2)
{
    std::string url = R"(|||///\\\)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "|||");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL37, TestSize.Level2)
{
    std::string url = "-- fdsf";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "-- fdsf");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL38, TestSize.Level2)
{
    std::string url = "xnmku:9090?(sdfgjhg)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "xnmku");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL39, TestSize.Level2)
{
    std::string url = "oooxxx111-===";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "oooxxx111-===");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL40, TestSize.Level2)
{
    std::string url = R"($^%(_*_()*+_)(YU(\_)))";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "$^%(_*_()*+_)(YU(");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL41, TestSize.Level2)
{
    std::string url = R"(万维网.com:9090\)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "万维网.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL42, TestSize.Level2)
{
    std::string url = R"(https://\\\中文测试)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "中文测试");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL43, TestSize.Level2)
{
    std::string url = R"(http://\\\中文测试?中文数据)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "中文测试");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL44, TestSize.Level2)
{
    std::string url = R"(http://\\\中文测试：8080?中文数据)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "中文测试：8080");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL45, TestSize.Level2)
{
    std::string url = R"(http：：：/\\\中文测试：8080?中文数据)";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "http：：：");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL46, TestSize.Level2)
{
    std::string url = R"(（）“”{}P{{}:\、、、})";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "（）“”{}P{{}");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL47, TestSize.Level2)
{
    std::string url = R"(（）“”{}P{http://{}:\、、、})";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "{}");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL48, TestSize.Level2)
{
    std::string url = R"(（）“===\\///?=”{}P{{的‘；‘’；’}:\、、、})";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "（）“===");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL49, TestSize.Level2)
{
    std::string url = R"(（）“”{}P{{；‘k:’}:\、、、})";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "（）“”{}P{{；‘k");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL50, TestSize.Level2)
{
    std::string url = R"(（）“”{}P{0%%%VVV{}:\、、、})";
    std::string hostname = CommonUtils::GetHostnameFromURL(url);
    EXPECT_STREQ(hostname.c_str(), "（）“”{}P{0%%%VVV{}");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL51, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL("https://www.alibaba.com/idesk?idesk:idesk");
    EXPECT_STREQ(hostname.c_str(), "www.alibaba.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL52, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL("https://www.alibaba.com:8081/idesk?idesk:idesk");
    EXPECT_STREQ(hostname.c_str(), "www.alibaba.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetHostnameFromURL53, TestSize.Level2)
{
    std::string hostname = CommonUtils::GetHostnameFromURL("https://www.alibaba.com?data_string");
    EXPECT_STREQ(hostname.c_str(), "www.alibaba.com");
}

HWTEST_F(UtNetmanagerBaseCommon, GetGatewayAddr001, TestSize.Level2)
{
    EXPECT_EQ(CommonUtils::GetGatewayAddr("10.21.156.a", "255.255.255.0"), "");
    EXPECT_EQ(CommonUtils::GetGatewayAddr("10.21.156.222", "255.255.b.0"), "");
    EXPECT_EQ(CommonUtils::GetGatewayAddr("10.21.156.222", "255.255.255.0"), "10.21.156.1");
}

HWTEST_F(UtNetmanagerBaseCommon, IpToInt001, TestSize.Level2)
{
    uint32_t ipIntAddr;
    EXPECT_FALSE(CommonUtils::IpToInt("10.158.2c00", ipIntAddr));
    EXPECT_TRUE(CommonUtils::IpToInt("10.158.200.0", ipIntAddr));
}

HWTEST_F(UtNetmanagerBaseCommon, IpToString001, TestSize.Level2)
{
    std::string ipStrAddr; // "10.158.200.0"
    EXPECT_TRUE(CommonUtils::IpToString(178178048, ipStrAddr));
}

HWTEST_F(UtNetmanagerBaseCommon, IsSim001, TestSize.Level2)
{
    std::string bundleName = "123";
    EXPECT_FALSE(CommonUtils::IsSim(bundleName));
}

HWTEST_F(UtNetmanagerBaseCommon, IsInstallSourceFromSim001, TestSize.Level2)
{
    std::string installSource = "com.zhuoyi.appstore.lite";
    EXPECT_TRUE(CommonUtils::IsInstallSourceFromSim(installSource));
}

HWTEST_F(UtNetmanagerBaseCommon, IsSim2001, TestSize.Level2)
{
    std::string bundleName = "123";
    EXPECT_FALSE(CommonUtils::IsSim2(bundleName));
}

HWTEST_F(UtNetmanagerBaseCommon, IsInstallSourceFromSim2001, TestSize.Level2)
{
    std::string installSource = "com.easy.abroad";
    EXPECT_TRUE(CommonUtils::IsInstallSourceFromSim2(installSource));
}

HWTEST_F(UtNetmanagerBaseCommon, IsSimAnco001, TestSize.Level2)
{
    std::string bundleName = "com.zhuoyi.appstore.lite";
    EXPECT_TRUE(CommonUtils::IsSimAnco(bundleName));
}

HWTEST_F(UtNetmanagerBaseCommon, IsSim2Anco001, TestSize.Level2)
{
    std::string bundleName = "com.easy.abroad";
    EXPECT_TRUE(CommonUtils::IsSim2Anco(bundleName));
}

HWTEST_F(UtNetmanagerBaseCommon, GetTodayMidnightTimestamp001, TestSize.Level2)
{
    int hour = 12;
    int min = 30;
    int sec = 30;
    std::string filePath = "";
    CommonUtils::DeleteFile(filePath);
    EXPECT_NE(CommonUtils::GetTodayMidnightTimestamp(hour, min, sec), 0);
}

HWTEST_F(UtNetmanagerBaseCommon, Strip001, TestSize.Level2)
{
    std::string str = "abca";
    char ch = 'a';
    EXPECT_EQ(CommonUtils::Strip(str, ch), "bc");
}

HWTEST_F(UtNetmanagerBaseCommon, IsValidDomainTest009, TestSize.Level1)
{
    std::string domain = "";
    auto result = CommonUtils::IsValidDomain(domain);
    ASSERT_FALSE(result);
}

HWTEST_F(UtNetmanagerBaseCommon, TrimTest001, TestSize.Level2)
{
    std::string str = "";
    auto result = CommonUtils::Trim(str);
    EXPECT_EQ(result, "");
}

HWTEST_F(UtNetmanagerBaseCommon, UrlRegexParse005, TestSize.Level2)
{
    std::string str = "abc!";
    std::string patternStr = "abc!";
    auto result = CommonUtils::UrlRegexParse(str, patternStr);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: AnonymizeIptablesCommandTest001
 * @tc.desc: Test UtNetmanagerBaseCommon AnonymizeIptablesCommand.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetmanagerBaseCommon, AnonymizeIptablesCommandTest001, TestSize.Level1)
{
    std::string command = "iptables -t filter -A ohfw_dozable -m owner --uid-owner 0-9999 -j RETURN -w 5";
    auto result = CommonUtils::AnonymizeIptablesCommand(command);
    ASSERT_FALSE(result.empty());
}

HWTEST_F(UtNetmanagerBaseCommon, IsUsableGlobalIpv6AddrTest001, TestSize.Level2)
{
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr(""));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("::"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("::1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("2001:db8::1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("2001:db8::ffff"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("fe80::1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("fe80::ffff"));
}

HWTEST_F(UtNetmanagerBaseCommon, IsUsableGlobalIpv6AddrTest002, TestSize.Level2)
{
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("fc00::1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("fdff::ffff"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("ff00::1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("ff02::1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("fec0::1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("fec0::ffff"));
    EXPECT_TRUE(CommonUtils::IsUsableGlobalIpv6Addr("2001:4860::1"));
    EXPECT_TRUE(CommonUtils::IsUsableGlobalIpv6Addr("2400::1"));
    EXPECT_TRUE(CommonUtils::IsUsableGlobalIpv6Addr("2a00::1"));
}

HWTEST_F(UtNetmanagerBaseCommon, IsUsableGlobalIpv6AddrTest003, TestSize.Level2)
{
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("invalid.ipv6"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("192.168.1.1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("2001::4860::zzzz"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("2001:0010::1"));
    EXPECT_FALSE(CommonUtils::IsUsableGlobalIpv6Addr("2001:0010::ffff"));
}

HWTEST_F(UtNetmanagerBaseCommon, IsValidAddressTest001, TestSize.Level2)
{
    EXPECT_FALSE(CommonUtils::IsValidAddress(""));
    EXPECT_FALSE(CommonUtils::IsValidAddress("abc"));
    EXPECT_FALSE(CommonUtils::IsValidAddress("0.0.0.0"));
    EXPECT_FALSE(CommonUtils::IsValidAddress("::"));
}
HWTEST_F(UtNetmanagerBaseCommon, IsIPv6LinkLocal001, TestSize.Level0)
{
    std::string emptyAddr = "";
    EXPECT_FALSE(CommonUtils::IsIPv6LinkLocal(emptyAddr));
}

HWTEST_F(UtNetmanagerBaseCommon, IsIPv6LinkLocal002, TestSize.Level0)
{
    std::string invalidAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334"; // 无效的IPv6地址
    EXPECT_FALSE(CommonUtils::IsIPv6LinkLocal(invalidAddr));
}

HWTEST_F(UtNetmanagerBaseCommon, IsIPv6LinkLocal003, TestSize.Level0)
{
    std::string linkLocalAddr = "fe80::1%eth0"; // 链路本地IPv6地址
    EXPECT_TRUE(CommonUtils::IsIPv6LinkLocal(linkLocalAddr));
}

HWTEST_F(UtNetmanagerBaseCommon, IsIPv6LinkLocal004, TestSize.Level0)
{
    std::string nonLinkLocalAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7335"; // 非链路本地IPv6地址
    EXPECT_FALSE(CommonUtils::IsIPv6LinkLocal(nonLinkLocalAddr));
}

HWTEST_F(UtNetmanagerBaseCommon, IsIPv6LinkLocal005, TestSize.Level0)
{
    std::string addrWithoutPercent = "fe80::1"; // 没有百分号部分的IPv6地址
    EXPECT_TRUE(CommonUtils::IsIPv6LinkLocal(addrWithoutPercent));
}

HWTEST_F(UtNetmanagerBaseCommon, RemoveDeleteControlFromPath001, TestSize.Level0)
{
    const char* path1 = "/data/cxy";
    CommonUtils::RemoveDeleteControlFromPath(path1);
    const char* path2 = "/data/service/el1/public/netmanager/";
    CommonUtils::RemoveDeleteControlFromPath(path2);
    CommonUtils::RemoveDeleteControlFromPath(path2);
    std::string emptyAddr = "";
    EXPECT_FALSE(CommonUtils::IsIPv6LinkLocal(emptyAddr));
}

HWTEST_F(UtNetmanagerBaseCommon, RemoveDeleteControlFlag001, TestSize.Level0)
{
    int fd = 0;
    CommonUtils::RemoveDeleteControlFlag(fd);
    fd = 1;
    CommonUtils::RemoveDeleteControlFlag(fd);
    std::string emptyAddr = "";
    EXPECT_FALSE(CommonUtils::IsIPv6LinkLocal(emptyAddr));
}

HWTEST_F(UtNetmanagerBaseCommon, SerializeStringVector001, TestSize.Level0)
{
    std::vector<std::string> testStrVec;
    testStrVec.push_back("111");
    testStrVec.push_back("222");
    std::string result = CommonUtils::SerializeStringVector(testStrVec);
    EXPECT_FALSE(result.empty());
}
} // namespace NetManagerStandard
} // namespace OHOS

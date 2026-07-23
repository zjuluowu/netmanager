/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "cJSON.h"

#ifdef GTEST_API_
#define private public
#endif
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "bundle_mgr_proxy.h"
#include "net_mgr_log_wrapper.h"
#include "net_manager_constants.h"
#include "network_security_config.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
    using namespace testing::ext;

    const std::string TEST_TRUST_ANCHORS(R"([{"certificates": "@resource/raw/ca"}])");

    const std::string TEST_DOMAINS(R"([{"include-subdomains": false, "name": "baidu.com"},
                                       {"include-subdomains": true, "name": "taobao.com"}])");

    const std::string TEST_COMPONENT_Network(R"({"NetworkKit": true})");
    const std::string TEST_COMPONENT_ArkWeb(R"({"ArkWeb": false})");

    const std::string TEST_PINSET(R"({
                    "expiration": "2024-8-6",
                    "pin": [
                    {"digest-algorithm": "sha256", "digest": "Q9TCQAWqP4t+eq41xnKaUgJdrPWqyG5L+Ni2YzMhqdY="},
                    {"digest-algorithm": "sha256", "digest": "Q6TCQAWqP4t+eq41xnKaUgJdrPWqyG5L+Ni2YzMhqdY="}
                    ]})");
    
    const std::string TEST_CLEARTEXT_TRAFFIC_PERMITTED(R"([{"cleartextTrafficPermitted": false}])");
} // namespace

std::shared_ptr<NetworkSecurityConfig> g_networkSecurityConfig;

class NetworkSecurityConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetworkSecurityConfigTest::SetUpTestCase() {}

void NetworkSecurityConfigTest::TearDownTestCase() {}

void NetworkSecurityConfigTest::SetUp() {}

void NetworkSecurityConfigTest::TearDown() {}

void BuildTestJsonObject(std::string &content, cJSON* &json)
{
    json = cJSON_Parse(content.c_str());
}

/**
 * @tc.name: IsCACertFileNameTest001
 * @tc.desc: Test NetworkSecurityConfig::IsCACertFileName
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsCACertFileNameTest001, TestSize.Level1)
{
    std::string fileName("cafile.Pem");
    std::cout << "IsCACertFileNameTest001 In" << std::endl;
    auto ret = NetworkSecurityConfig::GetInstance().IsCACertFileName(fileName.c_str());
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: GetCAFilesFromPathTest001
 * @tc.desc: Test NetworkSecurityConfig::GetCAFilesFromPath
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, GetCAFilesFromPathTest001, TestSize.Level1)
{
    std::string caPath("/etc/security/certificates/test");
    std::vector<std::string> caFiles;
    std::cout << "GetCAFilesFromPathTest001 In" << std::endl;
    NetworkSecurityConfig::GetInstance().GetCAFilesFromPath(caPath, caFiles);
    EXPECT_EQ(caFiles.size(), 0);
}

/**
 * @tc.name: AddSurfixToCACertFileNameTest001
 * @tc.desc: Test NetworkSecurityConfig::AddSurfixToCACertFileName
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, AddSurfixToCACertFileNameTest001, TestSize.Level1)
{
    std::string caPath("/etc/security/certificates/test");
    std::set<std::string> allFileNames;
    std::string caFile("cacert.pem");
    std::cout << "AddSurfixToCACertFileNameTest001 In" << std::endl;
    NetworkSecurityConfig::GetInstance().AddSurfixToCACertFileName(caPath, allFileNames, caFile);
    EXPECT_EQ(allFileNames.size(), 1);
}

/**
 * @tc.name: ReadCertFileTest001
 * @tc.desc: Test NetworkSecurityConfig::ReadCertFile
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, ReadCertFileTest001, TestSize.Level1)
{
    std::string caFile("cacert.pem");
    std::cout << "ReadCertFileTest001 In" << std::endl;
    auto ret = NetworkSecurityConfig::GetInstance().ReadCertFile(caFile);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetRehashedCADirName001
 * @tc.desc: Test NetworkSecurityConfig::GetRehashedCADirName
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, GetRehashedCADirName001, TestSize.Level1)
{
    std::string caPath("/etc/security/certificates/test");
    std::string caPathHashValue = "eb6bf998a72433bdeb4eeb32cfa4bc15";
    std::cout << "GetRehashedCADirName001 In" << std::endl;
    auto ret = NetworkSecurityConfig::GetInstance().GetRehashedCADirName(caPath);
    EXPECT_EQ(ret, caPathHashValue);
}

/**
 * @tc.name: BuildRehasedCAPath001
 * @tc.desc: Test NetworkSecurityConfig::BuildRehasedCAPath
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, BuildRehasedCAPath001, TestSize.Level1)
{
    std::string caPath("/etc/security/certificates/test");
    std::cout << "BuildRehasedCAPath001 In" << std::endl;
    auto ret = NetworkSecurityConfig::GetInstance().BuildRehasedCAPath(caPath);
    EXPECT_NE(ret, "");
}

/**
 * @tc.name: GetRehasedCAPath001
 * @tc.desc: Test NetworkSecurityConfig::GetRehasedCAPath
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, GetRehasedCAPath001, TestSize.Level1)
{
    std::string caPath("/etc/security/certificates/test");
    std::cout << "GetRehasedCAPath001 In" << std::endl;
    auto ret = NetworkSecurityConfig::GetInstance().GetRehasedCAPath(caPath);
    EXPECT_NE(ret, "");
}

/**
 * @tc.name: ReHashCAPathForX509001
 * @tc.desc: Test NetworkSecurityConfig::ReHashCAPathForX509
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, ReHashCAPathForX509001, TestSize.Level1)
{
    std::string caPath("/etc/security/certificates/test");
    std::cout << "ReHashCAPathForX509001 In" << std::endl;
    auto ret = NetworkSecurityConfig::GetInstance().ReHashCAPathForX509(caPath);
    EXPECT_EQ(ret, "");
}

/**
 * @tc.name: ParseJsonTrustAnchorsTest001
 * @tc.desc: Test NetworkSecurityConfig::ParseJsonTrustAnchors, not applying for
 * permission,return NETMANAGER_ERR_PERMISSION_DENIED
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, ParseJsonTrustAnchorsTest001, TestSize.Level1)
{
    cJSON *root = nullptr;
    TrustAnchors trustAnchors;

    std::string jsonTxt(TEST_TRUST_ANCHORS);
    BuildTestJsonObject(jsonTxt, root);

    std::cout << "ParseJsonTrustAnchorsTest001 In" << std::endl;
    NetworkSecurityConfig::GetInstance().ParseJsonTrustAnchors(root, trustAnchors);
    EXPECT_EQ(trustAnchors.certs_[0], "@resource/raw/ca");
}

/**
 * @tc.name: ParseJsonDomainsTest001
 * @tc.desc: Test NetworkSecurityConfig::ParseJsonDomains, not applying for
 * permission,return NETMANAGER_ERR_PERMISSION_DENIED
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, ParseJsonDomainsTest001, TestSize.Level1)
{
    cJSON *root = nullptr;
    std::vector<Domain> domains;

    std::string jsonTxt(TEST_DOMAINS);
    BuildTestJsonObject(jsonTxt, root);

    std::cout << "ParseJsonDomainsTest001 In" << std::endl;
    NetworkSecurityConfig::GetInstance().ParseJsonDomains(root, domains);
    ASSERT_EQ(domains[0].domainName_, "baidu.com");
    ASSERT_EQ(domains[0].includeSubDomains_, false);
    ASSERT_EQ(domains[1].domainName_, "taobao.com");
    EXPECT_EQ(domains[1].includeSubDomains_, true);
}

/**
 * @tc.name: ParseJsonPinSet001
 * @tc.desc: Test NetworkSecurityConfig::ParseJsonPinSet, not applying for
 * permission,return NETMANAGER_ERR_PERMISSION_DENIED
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, ParseJsonPinSet001, TestSize.Level1)
{
    cJSON *root = nullptr;
    PinSet pinSet;

    std::string jsonTxt(TEST_PINSET);
    BuildTestJsonObject(jsonTxt, root);

    std::cout << "ParseJsonPinSet001 In" << std::endl;
    NetworkSecurityConfig::GetInstance().ParseJsonPinSet(root, pinSet);
    ASSERT_EQ(pinSet.expiration_, "2024-8-6");
    ASSERT_EQ(pinSet.pins_[0].digestAlgorithm_, "sha256");
    ASSERT_EQ(pinSet.pins_[0].digest_, "Q9TCQAWqP4t+eq41xnKaUgJdrPWqyG5L+Ni2YzMhqdY=");
    ASSERT_EQ(pinSet.pins_[1].digestAlgorithm_, "sha256");
    EXPECT_EQ(pinSet.pins_[1].digest_, "Q6TCQAWqP4t+eq41xnKaUgJdrPWqyG5L+Ni2YzMhqdY=");
}

/**
 * @tc.name: HWTEST_F(NetworkSecurityConfigTest, GetPinSetForHostName001, TestSize.Level1)
 * @tc.desc: Test NetworkSecurityConfig::GetPinSetForHostName, not applying for
 * permission,return NETMANAGER_ERR_PERMISSION_DENIED
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, GetPinSetForHostName001, TestSize.Level1)
{
    PinSet pinSet;

    std::string hostname("www.example.com");
    std::string pins;

    std::cout << "GetPinSetForHostName001 In" << std::endl;
    auto ret = NetworkSecurityConfig::GetInstance().GetPinSetForHostName(hostname, pins);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: HWTEST_F(NetworkSecurityConfigTest, ParseJsonCleartextPermitted001, TestSize.Level1)
 * @tc.desc: Test NetworkSecurityConfig::ParseJsonCleartextPermitted, not applying for
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, ParseJsonCleartextPermitted001, TestSize.Level1)
{
    cJSON *root = nullptr;
    bool cleartextPermitted;

    std::string jsonTxt(TEST_CLEARTEXT_TRAFFIC_PERMITTED);
    BuildTestJsonObject(jsonTxt, root);

    std::cout << "ParseJsonCleartextPermitted001 In" << std::endl;
    NetworkSecurityConfig::GetInstance().ParseJsonCleartextPermitted(root, cleartextPermitted);
    EXPECT_TRUE(cleartextPermitted);
}

/**
 * @tc.name: HWTEST_F(NetworkSecurityConfigTest, IsCleartextPermitted001, TestSize.Level1)
 * @tc.desc: Test NetworkSecurityConfig::IsCleartextPermitted, not applying for
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsCleartextPermitted001, TestSize.Level1)
{
    NetworkSecurityConfig::GetInstance().baseConfig_.cleartextTrafficPermitted_ = true;
    std::cout << "IsCleartextPermitted001 In" << std::endl;
    bool isclearpermitted;
    auto ret = NetworkSecurityConfig::GetInstance().IsCleartextPermitted(isclearpermitted);
    EXPECT_EQ(ret, 0);
    auto ret2 = NetworkSecurityConfig::GetInstance().IsCleartextPermitted("", isclearpermitted);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HWTEST_F(NetworkSecurityConfigTest, IsCleartextPermitted002, TestSize.Level1)
 * @tc.desc: Test NetworkSecurityConfig::IsCleartextPermitted, not applying for
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsCleartextPermitted002, TestSize.Level1)
{
    NetworkSecurityConfig::GetInstance().baseConfig_.cleartextTrafficPermitted_ = false;
    std::vector<DomainConfig> domainConfigs;
    DomainConfig domainConfig;
    domainConfig.cleartextTrafficPermitted_ = true;
    std::vector<Domain> domains;
    Domain domain;
    domain.domainName_ = "www.text.com";
    domains.push_back(domain);
    domainConfig.domains_ = domains;
    domainConfigs.push_back(domainConfig);
    std::cout << "IsCleartextPermitted001 In" << std::endl;
    bool isclearpermitted;
    auto ret = NetworkSecurityConfig::GetInstance().IsCleartextPermitted("www.text.com", isclearpermitted);
    EXPECT_EQ(ret, 0);
    auto ret2 = NetworkSecurityConfig::GetInstance().IsCleartextPermitted("www.text2.com", isclearpermitted);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: IsCACertFileNameTest002
 * @tc.desc: Test NetworkSecurityConfig::IsCACertFileName
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsCACertFileNameTest002, TestSize.Level1)
{
    std::string fileName("c");
    std::cout << "IsCACertFileNameTest002 In" << std::endl;
    auto ret = NetworkSecurityConfig::GetInstance().IsCACertFileName(fileName.c_str());
    EXPECT_NE(ret, true);
}

/**
 * @tc.name: AddSurfixToCACertFileNameTest002
 * @tc.desc: Test NetworkSecurityConfig::AddSurfixToCACertFileName
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, AddSurfixToCACertFileNameTest002, TestSize.Level1)
{
    std::string caPath("/etc/security/certificates/test");
    std::set<std::string> allFileNames;
    allFileNames.insert("123");
    std::string caFile("cacert.pem");
    std::cout << "AddSurfixToCACertFileNameTest002 In" << std::endl;
    NetworkSecurityConfig::GetInstance().AddSurfixToCACertFileName(caPath, allFileNames, caFile);
    EXPECT_EQ(allFileNames.size(), 2);
}

/**
 * @tc.name: IsPinOpenModeTest001
 * @tc.desc: Test NetworkSecurityConfig::IsPinOpenMode
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsPinOpenModeTest001, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    Domain domain1;
    domain1.domainName_ = "example.com";
    DomainConfig config;
    config.domains_.push_back(domain1);
    networksecurityconfig.domainConfigs_.push_back(config);
    std::string hostname("example.com");
    std::cout << "IsPinOpenModeTest001 In" << std::endl;
    auto ret = networksecurityconfig.IsPinOpenMode(hostname);
    EXPECT_NE(ret, true);
    hostname = "example.com2";
    ret = networksecurityconfig.IsPinOpenMode(hostname);
    EXPECT_NE(ret, true);
}

/**
 * @tc.name: IsPinOpenModeTest002
 * @tc.desc: Test NetworkSecurityConfig::IsPinOpenMode
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsPinOpenModeTest002, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    Domain domain1;
    domain1.domainName_ = "example.com";
    Pin pin;
    pin.digestAlgorithm_ = "123";
    pin.digest_ = "123";
    PinSet pinset;
    pinset.isOpenMode = true;
    pinset.shouldVerifyRootCa_ = true;
    pinset.pins_.push_back(pin);
    pinset.expiration_ = "123";
    DomainConfig config;
    config.domains_.push_back(domain1);
    config.pinSet_ = pinset;
    networksecurityconfig.domainConfigs_.push_back(config);
    std::string hostname = "example.com";
    std::cout << "IsPinOpenModeTest002 In" << std::endl;
    auto ret = networksecurityconfig.IsPinOpenModeVerifyRootCa(hostname);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: IsPinOpenModeVerifyRootCaTest001
 * @tc.desc: Test NetworkSecurityConfig::IsPinOpenModeVerifyRootCa
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsPinOpenModeVerifyRootCaTest001, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    Domain domain1;
    domain1.domainName_ = "example.com";
    DomainConfig config;
    config.domains_.push_back(domain1);
    networksecurityconfig.domainConfigs_.push_back(config);
    std::string hostname;
    std::cout << "IsPinOpenModeVerifyRootCaTest001 In" << std::endl;
    auto ret = networksecurityconfig.IsPinOpenModeVerifyRootCa(hostname);
    EXPECT_NE(ret, true);
    hostname = "example.com";
    ret = networksecurityconfig.IsPinOpenModeVerifyRootCa(hostname);
    hostname = "example.com2";
    ret = networksecurityconfig.IsPinOpenModeVerifyRootCa(hostname);
    EXPECT_NE(ret, true);
}

/**
 * @tc.name: IsPinOpenModeVerifyRootCaTest002
 * @tc.desc: Test NetworkSecurityConfig::IsPinOpenModeVerifyRootCa
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsPinOpenModeVerifyRootCaTest002, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    Domain domain1;
    domain1.domainName_ = "example.com";
    Pin pin;
    pin.digestAlgorithm_ = "123";
    pin.digest_ = "123";
    PinSet pinset;
    pinset.isOpenMode = true;
    pinset.shouldVerifyRootCa_ = true;
    pinset.pins_.push_back(pin);
    pinset.expiration_ = "123";
    DomainConfig config;
    config.domains_.push_back(domain1);
    config.pinSet_ = pinset;
    networksecurityconfig.domainConfigs_.push_back(config);
    std::string hostname = "example.com";
    std::cout << "IsPinOpenModeVerifyRootCaTest002 In" << std::endl;
    auto ret = networksecurityconfig.IsPinOpenModeVerifyRootCa(hostname);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: IsPinOpenModeVerifyRootCaTest003
 * @tc.desc: Test NetworkSecurityConfig::IsPinOpenModeVerifyRootCa
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsPinOpenModeVerifyRootCaTest003, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    Domain domain1;
    domain1.domainName_ = "example.com";
    Pin pin;
    pin.digestAlgorithm_ = "123";
    pin.digest_ = "123";
    PinSet pinset;
    pinset.isOpenMode = false;
    pinset.shouldVerifyRootCa_ = true;
    pinset.pins_.push_back(pin);
    pinset.expiration_ = "123";
    DomainConfig config;
    config.domains_.push_back(domain1);
    config.pinSet_ = pinset;
    networksecurityconfig.domainConfigs_.push_back(config);
    std::string hostname = "example.com";
    std::cout << "IsPinOpenModeVerifyRootCaTest003 In" << std::endl;
    auto ret = networksecurityconfig.IsPinOpenModeVerifyRootCa(hostname);
    EXPECT_NE(ret, true);
}

HWTEST_F(NetworkSecurityConfigTest, IsPinOpenModeVerifyRootCaTest004, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    std::string hostname("");
    auto ret = networksecurityconfig.IsPinOpenModeVerifyRootCa(hostname);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetPinSetForHostNameTest001
 * @tc.desc: Test NetworkSecurityConfig::GetPinSetForHostName
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, GetPinSetForHostNameTest001, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    Domain domain1;
    domain1.domainName_ = "example.com";
    DomainConfig config;
    config.domains_.push_back(domain1);
    networksecurityconfig.domainConfigs_.push_back(config);
    std::string hostname = "example.com";
    std::string pins;
    std::cout << "GetPinSetForHostNameTest001 In" << std::endl;
    auto ret = networksecurityconfig.GetPinSetForHostName(hostname, pins);
    EXPECT_NE(ret, true);
    hostname = "example.com2";
    ret = networksecurityconfig.GetPinSetForHostName(hostname, pins);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetPinSetForHostNameTest002
 * @tc.desc: Test NetworkSecurityConfig::GetPinSetForHostName
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, GetPinSetForHostNameTest002, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    Domain domain1;
    domain1.domainName_ = "example.com";
    Pin pin;
    pin.digestAlgorithm_ = "123";
    pin.digest_ = "123";
    PinSet pinset;
    pinset.isOpenMode = true;
    pinset.shouldVerifyRootCa_ = true;
    pinset.pins_.push_back(pin);
    pinset.expiration_ = "123";
    DomainConfig config;
    config.domains_.push_back(domain1);
    config.pinSet_ = pinset;
    networksecurityconfig.domainConfigs_.push_back(config);
    std::string hostname = "example.com";
    std::string pins;
    std::cout << "GetPinSetForHostNameTest002 In" << std::endl;
    auto ret = networksecurityconfig.GetPinSetForHostName(hostname, pins);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetPinSetForHostNameTest003
 * @tc.desc: Test NetworkSecurityConfig::GetPinSetForHostName
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, GetPinSetForHostNameTest003, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    Domain domain1;
    domain1.domainName_ = "example.com";
    Pin pin;
    pin.digestAlgorithm_ = "123";
    pin.digest_ = "123";
    PinSet pinset;
    pinset.isOpenMode = true;
    pinset.shouldVerifyRootCa_ = true;
    pinset.pins_.push_back(pin);
    pinset.expiration_ = "";
    DomainConfig config;
    config.domains_.push_back(domain1);
    config.pinSet_ = pinset;
    networksecurityconfig.domainConfigs_.push_back(config);
    std::string hostname = "example.com";
    std::string pins;
    std::cout << "GetPinSetForHostNameTest003 In" << std::endl;
    auto ret = networksecurityconfig.GetPinSetForHostName(hostname, pins);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    pins = "123";
    ret = networksecurityconfig.GetPinSetForHostName(hostname, pins);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkSecurityConfigTest, GetPinSetForHostNameTest004, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    std::string hostname("www.example.com");
    std::string pins;
    auto ret = networksecurityconfig.GetPinSetForHostName(hostname, pins);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkSecurityConfigTest, GetTrustAnchorsForHostName001, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    std::string hostname("www.example.com");
    std::vector<std::string> certs;
    auto ret = networksecurityconfig.GetTrustAnchorsForHostName(hostname, certs);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: IsCleartextPermittedTest001
 * @tc.desc: Test NetworkSecurityConfig::IsCleartextPermitted
 * @tc.type: FUNC
 */
HWTEST_F(NetworkSecurityConfigTest, IsCleartextPermittedTest001, TestSize.Level1)
{
    std::string hostname = "example.com";
    NetworkSecurityConfig networksecurityconfig;
    bool cleartextPermitted = true;
    auto ret = networksecurityconfig.IsCleartextPermitted(hostname, cleartextPermitted);
    Domain domain1;
    domain1.domainName_ = "example.com";
    DomainConfig config;
    config.domains_.push_back(domain1);
    networksecurityconfig.domainConfigs_.push_back(config);
    std::cout << "IsCleartextPermittedTest001 In" << std::endl;
    ret = networksecurityconfig.IsCleartextPermitted(hostname, cleartextPermitted);
    EXPECT_NE(ret, true);
    hostname = "example.com2";
    ret = networksecurityconfig.IsCleartextPermitted(hostname, cleartextPermitted);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkSecurityConfigTest, IsCleartextPermittedTest002, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    bool isclearpermitted;
    auto ret = networksecurityconfig.IsCleartextPermitted(isclearpermitted);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkSecurityConfigTest, IsCleartextPermittedTest003, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    bool isclearpermitted;
    auto ret = networksecurityconfig.IsCleartextPermitted("www.testCleartextPermitted.com", isclearpermitted);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkSecurityConfigTest, IsUserDnsCacheTest001, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    bool isUserDnsCache = networksecurityconfig.isUserDnsCache_;
    networksecurityconfig.isUserDnsCache_ = false;
    auto ret = networksecurityconfig.IsUserDnsCache();
    EXPECT_FALSE(ret);
    networksecurityconfig.isUserDnsCache_ = isUserDnsCache;
}

HWTEST_F(NetworkSecurityConfigTest, IsCleartextCfgByComponentTest001, TestSize.Level1)
{
    NetworkSecurityConfig networksecurityconfig;
    bool isCleartextCfg;
    EXPECT_EQ(networksecurityconfig.IsCleartextCfgByComponent("Network Kit", isCleartextCfg), NETMANAGER_SUCCESS);
    EXPECT_TRUE(isCleartextCfg);
    EXPECT_EQ(networksecurityconfig.IsCleartextCfgByComponent("ArkWeb", isCleartextCfg), NETMANAGER_SUCCESS);
    EXPECT_FALSE(isCleartextCfg);
    EXPECT_EQ(networksecurityconfig.IsCleartextCfgByComponent("RCP", isCleartextCfg), NETMANAGER_ERR_INVALID_PARAMETER);
}

/**
* @tc.name: HWTEST_F(NetworkSecurityConfigTest, ParseJsonComponentCfg001, TestSize.Level1)
* @tc.desc: Test NetworkSecurityConfig::ParseJsonComponentCfg, not applying for
* @tc.type: FUNC
*/
HWTEST_F(NetworkSecurityConfigTest, ParseJsonComponentCfg001, TestSize.Level1)
{
    cJSON *root = nullptr;
    ComponentCfg componentCfg;
    std::string jsonTxt(TEST_COMPONENT_Network);
    BuildTestJsonObject(jsonTxt, root);
    std::cout << "ParseJsonComponentCfg001 In" << std::endl;
    NetworkSecurityConfig::GetInstance().ParseJsonComponentCfg(root, componentCfg);
    EXPECT_FALSE(componentCfg["NetworkKit"]);
}

/**
* @tc.name: HWTEST_F(NetworkSecurityConfigTest, ParseJsonComponentCfg002, TestSize.Level1)
* @tc.desc: Test NetworkSecurityConfig::ParseJsonComponentCfg, not applying for
* @tc.type: FUNC
*/
HWTEST_F(NetworkSecurityConfigTest, ParseJsonComponentCfg002, TestSize.Level1)
{
    cJSON *root = nullptr;
    ComponentCfg componentCfg;
    std::string jsonTxt(TEST_COMPONENT_ArkWeb);
    BuildTestJsonObject(jsonTxt, root);
    std::cout << "ParseJsonComponentCfg002 In" << std::endl;
    NetworkSecurityConfig::GetInstance().ParseJsonComponentCfg(root, componentCfg, "ArkWeb");
    EXPECT_FALSE(componentCfg["ArkWeb"]);
}

/**
* @tc.name: HWTEST_F(NetworkSecurityConfigTest, ParseJsonComponentCfg003, TestSize.Level1)
* @tc.desc: Test NetworkSecurityConfig::ParseJsonComponentCfg, not applying for
* @tc.type: FUNC
*/
HWTEST_F(NetworkSecurityConfigTest, ParseJsonComponentCfg003, TestSize.Level1)
{
    cJSON *root = nullptr;
    ComponentCfg componentCfg;
    NetworkSecurityConfig::GetInstance().ParseJsonComponentCfg(root, componentCfg);
    std::string jsonTxt(TEST_COMPONENT_ArkWeb);
    BuildTestJsonObject(jsonTxt, root);
    std::cout << "ParseJsonComponentCfg003 In" << std::endl;
    NetworkSecurityConfig::GetInstance().ParseJsonComponentCfg(root, componentCfg);
    EXPECT_FALSE(componentCfg["ArkWeb"]);
}
}
}

/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifdef GTEST_API_
#define private public
#endif
#include "net_http_probe.h"
#include "net_http_probe_result.h"
#include "net_link_info.h"
#include "net_manager_constants.h"
#include "net_proxy_userinfo.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr int32_t TEST_NETID = 999;
constexpr const char *TEST_PROXY_HOST = "testHttpProxy";
constexpr const char *TEST_STRING = "testString";
constexpr const char *TEST_HTTP_URL = "http://connectivitycheck.platform.hicloud.com/generate_204";
constexpr const char *TEST_HTTPS_URL = "https://connectivitycheck.platform.hicloud.com/generate_204";

class NetHttpProbeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetHttpProbe> instance_ = nullptr;
};

void NetHttpProbeTest::SetUpTestCase()
{
    instance_ = std::make_shared<NetHttpProbe>(TEST_NETID, NetBearType::BEARER_DEFAULT, NetLinkInfo(), PROBE_HTTP);
}

void NetHttpProbeTest::TearDownTestCase() {}

void NetHttpProbeTest::SetUp() {}

void NetHttpProbeTest::TearDown() {}

HWTEST_F(NetHttpProbeTest, SendProbeTest001, TestSize.Level1)
{
    instance_->GetHttpProbeResult();
    instance_->GetHttpsProbeResult();
    HttpProxy httpProxy = {TEST_PROXY_HOST, 0, {}};
    NetLinkInfo info;
    instance_->UpdateNetLinkInfo(info);
    instance_->UpdateGlobalHttpProxy(httpProxy);
    int32_t ret = instance_->SendProbe(PROBE_HTTP_HTTPS, TEST_HTTP_URL, TEST_HTTPS_URL);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetHttpProbeTest, NetHttpProbeBranchTest001, TestSize.Level1)
{
    instance_->SendHttpProbeRequest();
    instance_->RecvHttpProbeResponse();

    std::string domain = "";
    std::string result = instance_->GetAddrInfo(domain);
    ASSERT_TRUE(result.empty());

    int32_t port = 0;
    bool ret = instance_->SetResolveOption(ProbeType::PROBE_HTTP, domain, TEST_STRING, port);
    ASSERT_FALSE(ret);

    ret = instance_->SetResolveOption(ProbeType::PROBE_HTTP, TEST_STRING, TEST_STRING, port);
    ASSERT_FALSE(ret);

    ret = instance_->SetResolveOption(ProbeType::PROBE_HTTPS, TEST_STRING, TEST_STRING, port);
    ASSERT_FALSE(ret);

    bool useProxy = true;
    ret = instance_->SendDnsProbe(ProbeType::PROBE_HTTPS, TEST_STRING, TEST_STRING, useProxy);
    ASSERT_TRUE(ret);

    ret = instance_->SendDnsProbe(ProbeType::PROBE_HTTPS, TEST_STRING, TEST_STRING, useProxy);
    ASSERT_TRUE(ret);

    instance_->ipAddrList_ = "192.168.1.1";
    ret = instance_->SendDnsProbe(ProbeType::PROBE_HTTPS, TEST_STRING, TEST_STRING, useProxy);
    instance_->ipAddrList_ = "";
    ASSERT_TRUE(ret);
}

HWTEST_F(NetHttpProbeTest, CheckCurlGlobalInitState001, TestSize.Level1)
{
    instance_->isCurlInit_ = false;
    bool ret = instance_->CheckCurlGlobalInitState();
    ASSERT_TRUE(ret);
}

HWTEST_F(NetHttpProbeTest, SetResolveOption001, TestSize.Level1)
{
    std::string domain = "http:";
    std::string ipAddress = "127.0.0.1";
    int32_t port = 0;

    bool ret = instance_->SetResolveOption(ProbeType::PROBE_HTTP, domain, ipAddress, port);
    ASSERT_FALSE(ret);

    instance_->CleanHttpCurl();
    ASSERT_TRUE(instance_->httpResolveList_ == nullptr);

    ret = instance_->SetResolveOption(ProbeType::PROBE_HTTPS, domain, ipAddress, port);
    ASSERT_FALSE(ret);

    instance_->CleanHttpCurl();
    ASSERT_TRUE(instance_->httpsResolveList_ == nullptr);
}

HWTEST_F(NetHttpProbeTest, ExtractDomainFormUrl001, TestSize.Level1)
{
    string url = "";

    std::string ret = instance_->ExtractDomainFormUrl(url);
    EXPECT_EQ(ret, std::string());
    ret = instance_->ExtractDomainFormUrl(TEST_STRING);
    EXPECT_EQ(ret, TEST_STRING);
}

HWTEST_F(NetHttpProbeTest, SetHttpOptions001, TestSize.Level1)
{
    ProbeType probeType = ProbeType::PROBE_HTTP_HTTPS;
    CURL *curl = nullptr;
    std::string url = "";

    bool ret = instance_->SetHttpOptions(probeType, curl, url);
    ASSERT_FALSE(ret);
    ret = instance_->SetHttpOptions(probeType, instance_->httpCurl_, url);
    ASSERT_FALSE(ret);
    ret = instance_->InitHttpCurl(ProbeType::PROBE_HTTP);
    ASSERT_TRUE(ret);
    ret = instance_->SetHttpOptions(ProbeType::PROBE_HTTP, instance_->httpCurl_, TEST_HTTPS_URL);
    ASSERT_TRUE(ret);
}

HWTEST_F(NetHttpProbeTest, SetProxyOptionTest001, TestSize.Level1)
{
    ProbeType probeType = ProbeType::PROBE_HTTP;
    HttpProxy httpProxy = {"", 0, {}};
    bool useHttpProxy = false;
    instance_->defaultUseGlobalHttpProxy_ = true;

    instance_->UpdateGlobalHttpProxy(httpProxy);
    bool ret = instance_->SetProxyOption(probeType, useHttpProxy);
    ASSERT_TRUE(ret);

    instance_->CleanHttpCurl();
    httpProxy = {"httpProxy", 0, {}};
    instance_->UpdateGlobalHttpProxy(httpProxy);
    ret = instance_->SetProxyOption(probeType, useHttpProxy);
    ASSERT_TRUE(ret);

    ret = instance_->InitHttpCurl(probeType);
    ASSERT_TRUE(ret);
    ret = instance_->SetProxyOption(probeType, useHttpProxy);
    ASSERT_TRUE(ret);

    probeType = ProbeType::PROBE_HTTPS;
    instance_->CleanHttpCurl();
    ret = instance_->SetProxyOption(probeType, useHttpProxy);
    ASSERT_TRUE(ret);

    ret = instance_->InitHttpCurl(probeType);
    ASSERT_TRUE(ret);
    ret = instance_->SetProxyOption(probeType, useHttpProxy);
    ASSERT_TRUE(ret);
}

HWTEST_F(NetHttpProbeTest, SendDnsProbeTest001, TestSize.Level1)
{
    bool useProxy = false;
    std::string httpUrl = "";
    std::string httpsUrl = "";
    bool ret = instance_->SendDnsProbe(ProbeType::PROBE_HTTP, httpUrl, TEST_STRING, useProxy);
    ASSERT_FALSE(ret);

    ret = instance_->SendDnsProbe(ProbeType::PROBE_HTTPS, TEST_STRING, httpsUrl, useProxy);
    ASSERT_FALSE(ret);
}

HWTEST_F(NetHttpProbeTest, SendHttpProbeRequestTest001, TestSize.Level1)
{
    instance_->CleanHttpCurl();
    instance_->SendHttpProbeRequest();
    ASSERT_TRUE(instance_->curlMulti_ == nullptr);
    instance_->RecvHttpProbeResponse();
    ASSERT_TRUE(instance_->curlMulti_ == nullptr);

    bool ret = instance_->InitHttpCurl(ProbeType::PROBE_HTTP);
    ASSERT_TRUE(ret);
    instance_->SendHttpProbeRequest();
    ASSERT_TRUE(instance_->curlMulti_ != nullptr);
    instance_->RecvHttpProbeResponse();
    ASSERT_TRUE(instance_->curlMulti_ != nullptr);
}

HWTEST_F(NetHttpProbeTest, LoadProxyTest001, TestSize.Level1)
{
    ProbeType probeType = ProbeType::PROBE_HTTP;
    instance_->defaultUseGlobalHttpProxy_ = true;
    HttpProxy httpProxy = {"httpProxy", 0, {}};
    std::string proxyHost = "proxyHost";
    int32_t proxyPort = 0;

    instance_->UpdateGlobalHttpProxy(httpProxy);
    bool ret = instance_->LoadProxy(proxyHost, proxyPort);
    ASSERT_TRUE(ret);

    instance_->defaultUseGlobalHttpProxy_ = false;
    NetLinkInfo netLinkInfo;
    netLinkInfo.httpProxy_ = {"127.0.0.1", 80, {"localhost"}};
    instance_->UpdateNetLinkInfo(netLinkInfo);
    ret = instance_->LoadProxy(proxyHost, proxyPort);
    ASSERT_TRUE(ret);
}

HWTEST_F(NetHttpProbeTest, SetUserInfoTest001, TestSize.Level1)
{
    CURL *curlHandler = curl_easy_init();
    ASSERT_NE(curlHandler, nullptr);
    HttpProxy httpProxy = {"127.0.0.1", 8080, {}};
    SecureData username;
    username.append("testuser", strlen("testuser"));
    SecureData password;
    password.append("testpass", strlen("testpass"));
    httpProxy.SetUserName(username);
    httpProxy.SetPassword(password);
    NetProxyUserinfo::GetInstance().SaveHttpProxyHostPass(httpProxy);
    bool ret = instance_->SetUserInfo(curlHandler);
    EXPECT_TRUE(ret);
    curl_easy_cleanup(curlHandler);
}

HWTEST_F(NetHttpProbeTest, SetUserInfoTest002, TestSize.Level1)
{
    CURL *curlHandler = curl_easy_init();
    ASSERT_NE(curlHandler, nullptr);
    HttpProxy httpProxy = {"127.0.0.1", 8080, {}};
    SecureData username;
    username.append("testuser", strlen("testuser"));
    httpProxy.SetUserName(username);
    NetProxyUserinfo::GetInstance().SaveHttpProxyHostPass(httpProxy);
    bool ret = instance_->SetUserInfo(curlHandler);
    EXPECT_TRUE(ret);
    curl_easy_cleanup(curlHandler);
}
} // namespace
} // namespace NetManagerStandard
} // namespace OHOS

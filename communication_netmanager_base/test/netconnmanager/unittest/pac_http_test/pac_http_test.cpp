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

#include "curl/curl.h"
#include "net_connection.h"
#include "gtest/gtest.h"
#include <iostream>

#include "pac_server.h"
#include "proxy_server.h"
#include "security_config.h"
#include "net_manager_constants.h"
#define PORT_9000 9000
#define PORT_9001 9001
#define PORT_3888 3888
#define PORT_3889 3889
#define PORT_8889 8889
#define PORT_5889 5889

using namespace OHOS::NetManagerStandard;
std::map<int32_t, std::shared_ptr<ProxyServer>> services;
#define TIME_OUT_S 30
#define SIZE_BUFFER 1024
void StartProxyServer(int32_t port)
{
    std::shared_ptr<ProxyServer> server = std::make_shared<ProxyServer>(port);
    services.insert({port, server});
    server->Start();
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

std::string Request(std::string url, std::string ip, uint16_t port)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = nullptr;
        std::string proxyStr = "GlobalProxyIp: " + ip;
        std::string proxyPortStr = "GlobalProxyPort: " + std::to_string(port);
        headers = curl_slist_append(headers, proxyStr.c_str());
        headers = curl_slist_append(headers, proxyPortStr.c_str());
        headers = curl_slist_append(headers, "X-Custom-Header: CustomValue");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (!ip.empty()) {
            std::string proxy = "http://" + ip + ":" + std::to_string(port);
            printf(
                "\033[32m"
                "curl %s use poxy %s \n"
                "\033[0m",
                url.c_str(), proxy.c_str());
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIME_OUT_S);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            printf(
                "\033[32m"
                "HTTP Status Code: %d Response:%s \n"
                "\033[0m",
                httpCode, readBuffer.c_str());
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return readBuffer;
}

int32_t SetPacFileUrl(std::string url)
{
    int32_t ret = OH_NetConn_SetPacFileUrl(url.c_str());
#define TIME 500
    usleep(TIME);
    return ret;
}

std::tuple<int32_t, std::string> FindProxyForURL(std::string url)
{
    char proxy[SIZE_BUFFER];
    int32_t ret = OH_NetConn_FindProxyForURL(url.c_str(), nullptr, proxy);
    return {ret, proxy};
}

#if 1
void SetupTestEnvironment()
{
    // Set up SetPacFileUrl permission
    SetUpPermission();
}

void TestInitialPacSetting()
{
    std::string pacFileUrl = "http://127.0.0.1:8888/";
    int32_t ret = SetPacFileUrl(pacFileUrl);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
    // Setting failed, url is not accessible
    OH_NetConn_SetProxyMode(PROXY_MODE_AUTO);
}

static std::string GetHeaderValue(const std::string &request, const std::string &headerName)
{
    std::string headerPrefix = headerName + ": ";
    size_t pos = request.find(headerPrefix);
    if (pos == std::string::npos) {
        return "";
    }
    size_t valueStart = pos + headerPrefix.length();
    size_t valueEnd = request.find("\r\n", valueStart);
    if (valueEnd == std::string::npos) {
        return request.substr(valueStart);
    }
    return request.substr(valueStart, valueEnd - valueStart);
}

void TestDefaultHttpProxy()
{
    std::string res;
    NetConn_HttpProxy proxy;
    int32_t ret = OH_NetConn_GetDefaultHttpProxy(&proxy);

    res = Request("https://getman.cn/echo", "", 1);
    sleep(1);
    printf("response %s \n", res.c_str());
    EXPECT_NE(GetHeaderValue(res, "HOST"), "Getman.cn");
}

void TestDirectAccess()
{
    // Start pac 8888 server
    StartHttpServer(8888, "", "");
    sleep(1);
    // Directly access test url, no proxy
    std::string res = Request("http://127.0.0.1:8888/test", "", 0);
    EXPECT_EQ(res.empty(), false);
}

void TestValidPacSetting(const std::string &pacFileUrl)
{
    int32_t ret = SetPacFileUrl(pacFileUrl);
    EXPECT_GE(ret, 0);
    ret = OH_NetConn_SetProxyMode(PROXY_MODE_AUTO);
    EXPECT_EQ(ret, 0);

    // Get local pac proxy server
    NetConn_HttpProxy proxy;
    ret = OH_NetConn_GetDefaultHttpProxy(&proxy);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(std::string(proxy.host).empty(), false);
    EXPECT_EQ(proxy.port > SIZE_BUFFER, true);
    printf("http proxy %s %d \n", proxy.host, proxy.port);
}

void TestFindProxyForURL(const std::string &url, const std::string &expectedResult)
{
    auto result = FindProxyForURL(url);
    EXPECT_EQ(std::get<0>(result), 0);
}

void TestUsingProxyServer(const std::string &url, const std::string &expectedProxyPort = "")
{
    NetConn_HttpProxy proxy;
    int32_t ret = OH_NetConn_GetDefaultHttpProxy(&proxy);
    EXPECT_EQ(ret, 0);
    Request(url, proxy.host, proxy.port);
}

void TestHttpsRequest()
{
    NetConn_HttpProxy proxy;
    OH_NetConn_GetDefaultHttpProxy(&proxy);

    std::string res = Request("https://getman.cn/echo", proxy.host, proxy.port);
    sleep(1);
    printf("xxxxxxx %s \n", res.c_str());
    EXPECT_EQ(ProxyServer::proxServerTargetUrl, "https://getman.cn:443");
    EXPECT_EQ(ProxyServer::proxServerPort, PORT_9001);
    GetHeaderValue(res, "GLOBALPROXYIP");
    GetHeaderValue(res, "GLOBALPROXYPORT");
}

void SetupPacServer(int32_t port, const std::string &script)
{
    StartHttpServer(port, "", script);
    std::string pacFileUrl = "http://127.0.0.1:" + std::to_string(port) + "/";
    TestValidPacSetting(pacFileUrl);
}

void TestMultipleProxyPacScript()
{
    // Reset pac proxy information, switch multiple proxy pac script
    std::string fileScript = ProxyServer::pacScripts[ALL_DIRECT];
#define PORT_5890 5890
    SetupPacServer(PORT_5890, fileScript);
    // Test new proxy server
    TestFindProxyForURL("http://127.0.0.1:3888/test", "PROXY 127.0.0.1:9000;PROXY 127.0.0.1:9001; DIRECT");
    // Test with first proxy
    TestUsingProxyServer("http://127.0.0.1:3888/test", "9000");
    // Stop first proxy and test with second proxy
    services[PORT_9000]->Stop();
    sleep(1);
    TestUsingProxyServer("http://127.0.0.1:3888/test", "9001");
    // Stop second proxy and test direct connection
    services[PORT_9001]->Stop();
    sleep(1);
    TestUsingProxyServer("http://127.0.0.1:3888/test", "");
}

TEST(MyTests, PacFileUrlClient)
{
    SetupTestEnvironment();
    TestInitialPacSetting();
    TestDefaultHttpProxy();
    TestDirectAccess();
    TestValidPacSetting("http://127.0.0.1:8888/");
    StartHttpServer(PORT_3888, "", "");
    StartHttpServer(PORT_3889, "", "");
    TestFindProxyForURL("http://127.0.0.1:3888/test", "DIRECT");
    TestUsingProxyServer("http://127.0.0.1:3888/test");
    StartProxyServer(PORT_9000);
    StartProxyServer(PORT_9001);
    SetupPacServer(PORT_8889, ProxyServer::pacScripts[LOCAL_PROXY_9000]);
    TestFindProxyForURL("http://127.0.0.1:3888/test", "PROXY 127.0.0.1:9000");
    TestUsingProxyServer("http://127.0.0.1:3888/test", "9000");
    SetupPacServer(PORT_5889, ProxyServer::pacScripts[LOCAL_PROXY_9001]);
    TestFindProxyForURL("http://127.0.0.1:3888/test", "PROXY 127.0.0.1:9001");
    TestUsingProxyServer("http://127.0.0.1:3888/test", "9001");
    TestHttpsRequest();
    TestMultipleProxyPacScript();
    UnsetUpPermission();
}
#endif
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
#include "net_conn_client.h"
#include "net_connection.h"
#include "pac_server.h"
#include "proxy_server.h"
#include "security_config.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace OHOS::NetManagerStandard;
std::map<int32_t, std::shared_ptr<ProxyServer>> services;
#define PACPROXYSERVER 9000
#define GLOBALPROXYSERVER 9001
#define PORT_8080 8080
#define PORT_8889 8889
#define TIME_500_MS 500
#define TIMEOUT_10_S 30

static void StartProxyServer(int32_t port)
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

static std::string Request(std::string url, std::string ip, uint16_t port)
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
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT_10_S);
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

static int32_t SetPacFileUrl(std::string url)
{
    int32_t ret = OH_NetConn_SetPacFileUrl(url.c_str());
    usleep(TIME_500_MS);
    return ret;
}

static std::tuple<int32_t, std::string> FindProxyForURL(std::string url)
{
    char proxy[1024];
    int32_t ret = OH_NetConn_FindProxyForURL(url.c_str(), nullptr, proxy);
    return {ret, proxy};
}

void TestAutoMode()
{
    NetConn_HttpProxy proxy;
    int32_t ret = -1;
    OHOS::NetManagerStandard::ProxyModeType mode;
    std::string script = ProxyServer::pacScripts[LOCAL_PROXY_9000];
    StartHttpServer(PORT_8889, "", script);
    std::string pacFileUrl = "http://127.0.0.1:8889/";
    ret = SetPacFileUrl(pacFileUrl);
    EXPECT_EQ(ret, 0);
    ret = OH_NetConn_SetProxyMode(PROXY_MODE_AUTO);
    EXPECT_EQ(ret, 0);

    ret = OH_NetConn_GetProxyMode(&mode);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(mode, ProxyModeType::PROXY_MODE_AUTO);

    ret = OH_NetConn_GetDefaultHttpProxy(&proxy);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(std::string(proxy.host), "127.0.0.1");
    printf("mode %d host:%s port:%d \n", mode, proxy.host, proxy.port);

    // Test FindProxyForURL default rule returns DIRECT
    auto result = FindProxyForURL("http://127.0.0.1:3888/test");
    EXPECT_EQ(std::get<0>(result), 0);
    EXPECT_EQ(std::get<1>(result), "PROXY 127.0.0.1:9000");

    std::string url = "http://127.0.0.1:8080/test";
    std::string res = Request(url, proxy.host, proxy.port);
    printf("res %s %s \n", url.c_str(), res.c_str());
    EXPECT_EQ(res.empty(), false);
}

TEST(PROXY_SWITCH_TEST, PacFileUrlClient)
{
    SetUpPermission();
    StartHttpServer(PORT_8080, "", "");
    StartProxyServer(GLOBALPROXYSERVER);
    StartProxyServer(PACPROXYSERVER);
    TestAutoMode();
    OHOS::NetManagerStandard::ProxyModeType mode;
    int32_t ret = OH_NetConn_SetProxyMode(PROXY_MODE_OFF);
    EXPECT_EQ(ret, 0);
    NetConn_HttpProxy proxy;
    ret = OH_NetConn_GetProxyMode(&mode);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(mode, 0);

    ret = OH_NetConn_GetDefaultHttpProxy(&proxy);
    EXPECT_EQ(ret, 0);
    printf("mode %d host:%s port:%d \n", mode, proxy.host, proxy.port);
    UnsetUpPermission();
}
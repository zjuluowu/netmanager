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
#include "net_pac_local_proxy_server.h"
#include "securec.h"
#include "gtest/gtest.h"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace OHOS::NetManagerStandard;

#define HEADER_LARGE 10240

struct RequestOptions {
    bool largeHeader = false;
    bool ccr = false;
    int32_t timeout = 30;
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

struct curl_slist *SetupHeaders(const std::string &ip, uint16_t port, bool largeHeader)
{
    struct curl_slist *headers = nullptr;
    std::string proxyStr = "GlobalProxyIp: " + ip;
    std::string proxyPortStr = "GlobalProxyPort: " + std::to_string(port);
    headers = curl_slist_append(headers, proxyStr.c_str());
    headers = curl_slist_append(headers, proxyPortStr.c_str());
    headers = curl_slist_append(headers, "X-Custom-Header: CustomValue");
    if (largeHeader) {
        for (int32_t i = 0; i < HEADER_LARGE; i++) {
            std::string testHeader = "testHeader";
            testHeader.append(std::to_string(i)).append(": ").append(std::to_string(i));
            headers = curl_slist_append(headers, testHeader.c_str());
        }
    }
    return headers;
}

static void SetupProxyAndUrl(CURL *curl, const std::string &url, const std::string &ip, uint16_t port, bool ccr)
{
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    if (ccr) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "CONNECT");
    }
    if (!ip.empty()) {
        std::string proxy = "http://" + ip + ":" + std::to_string(port);
        printf(
            "\033[32m"
            "curl %s use proxy %s \n"
            "\033[0m",
            url.c_str(), proxy.c_str());
        curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
    }
}

static void SetupCommonOptions(CURL *curl, std::string &readBuffer, int32_t timeout)
{
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
}

static CURLcode PerformRequest(CURL *curl, std::string &readBuffer)
{
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    } else {
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    }
    return res;
}

static std::string Request(std::string url, std::string ip, uint16_t port, RequestOptions options = RequestOptions())
{
    CURL *curl = nullptr;
    CURLcode res;
    std::string readBuffer;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *headers = SetupHeaders(ip, port, options.largeHeader);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        SetupProxyAndUrl(curl, url, ip, port, options.ccr);
        SetupCommonOptions(curl, readBuffer, options.timeout);
        res = PerformRequest(curl, readBuffer);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return readBuffer;
}

#if 1
TEST(ProxyServerTest, CreateProxyTest)
{
    int32_t portStart = 1024;
    int32_t portEnd = 65535;
    int32_t port = ProxyServer::FindAvailablePort(portStart, portEnd);
    auto proxy = std::make_shared<ProxyServer>(port, 0);
    proxy->Start();
    int32_t port1 = ProxyServer::FindAvailablePort(portStart, portEnd);
    auto proxy1 = std::make_shared<ProxyServer>(port1, 0);
    proxy1->Start();
    EXPECT_EQ(proxy->IsRunning(), true);
}

TEST(ProxyServerTest, CreateProxyTest1)
{
    int32_t port = 8888;
    auto proxy = std::make_shared<ProxyServer>(port, 0);
    proxy->Start();
    auto proxy1 = std::make_shared<ProxyServer>(port, 0);
    EXPECT_EQ(proxy1->Start(), false);
}

TEST(ProxyServerTest, CreateProxyTest3)
{
    int32_t port = 8888;
    auto proxy = std::make_shared<ProxyServer>(port, 0);
    proxy->Start();
    proxy->SetFindPacProxyFunction([](std::string, std::string) { return "DIRECT"; });
    std::string res = Request("http://sssssssssssss.com", "127.0.0.1", 8888);
    EXPECT_EQ(res.empty(), false);

    res = Request("http://icanhazip.com/", "127.0.0.1", 8888);
    EXPECT_EQ(res.empty(), false);
}

TEST(ProxyServerTest, CreateProxyTest4)
{
    int32_t port = 8888;
    auto proxy = std::make_shared<ProxyServer>(port, 0);
    proxy->Start();
    proxy->SetFindPacProxyFunction([](std::string, std::string) { return "SOCKS4 127.0.0.1:1234"; });
    std::string res = Request("https://www.example.com/", "127.0.0.1", 8888);
    EXPECT_EQ(res.empty(), true);
    proxy->SetFindPacProxyFunction([](std::string, std::string) { return "DIRECT"; });
    res = Request("https://www.example.com/", "127.0.0.1", 8888);
    res = Request("https://www.example.com/", "127.0.0.1", 8888, RequestOptions{false});
    res = Request("https://www.example.com/", "127.0.0.1", 8888, RequestOptions{true});
    EXPECT_EQ(res.empty(), true);
    RequestOptions opts;
    opts.ccr = true;
    res = Request("https://www.example.com", "127.0.0.1", 8888, opts);
}

TEST(ProxyServerTest, CreateProxyTest10)
{
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](std::string, std::string) { return "DIRECT"; });
    std::vector<std::thread> threads;
#define THREAD_COUNT 10
    for (int32_t i = 0; i < THREAD_COUNT; ++i) {
        RequestOptions opts;
        opts.timeout = 2;
        threads.emplace_back([]() {
            Request("https://www.example.com/", "127.0.0.1", 8000, RequestOptions{false, false, 2});
        });
    }
    proxy->Stop();
    for (auto &thread : threads) {
        thread.join();
    }
}

TEST(ProxyServerTest, CreateProxyTest6)
{
    auto proxy8000 = std::make_shared<ProxyServer>(8000, -1);
    EXPECT_EQ(proxy8000->Start(), true);
    EXPECT_EQ(proxy8000->Start(), false);
}

TEST(ProxyServerTest, CreateProxyTest9)
{
    auto proxy0 = std::make_shared<ProxyServer>(8000, 0);
    auto proxy1 = std::make_shared<ProxyServer>(8000, 0);
    EXPECT_EQ(proxy1->Start(), true);
    EXPECT_EQ(proxy0->Start(), false);
}

TEST(ProxyServerTest, CreateProxyTest8)
{
    std::vector<int32_t> fds;
    struct rlimit limit;
    long maxFd = sysconf(_SC_OPEN_MAX);
    for (int32_t i = 0; i < maxFd + 3; i++) {
        int32_t fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == -1) {
            EXPECT_EQ(ProxyServer::FindAvailablePort(9000, 9000), -1);
            auto proxy8000 = std::make_shared<ProxyServer>(8000, -1);
            EXPECT_EQ(proxy8000->Start(), false);
            break;
        } else {
            fds.push_back(fd);
        }
    }
    for (auto fd : fds) {
        close(fd);
    }
}

TEST(ProxyServerTest, CreateProxyTest7)
{
    auto proxy8000 = std::make_shared<ProxyServer>(8000, -1);
    EXPECT_EQ(proxy8000->Start(), true);
    int32_t port = ProxyServer::FindAvailablePort(8000, 8000);
    EXPECT_EQ(port, -1);
}

TEST(ProxyServerTest, CreateProxyTest5)
{
    auto proxy8000 = std::make_shared<ProxyServer>(8000, 0);
    EXPECT_EQ(proxy8000->Start(), true);
    auto proxy8001 = std::make_shared<ProxyServer>(8001, 0);
    proxy8001->SetFindPacProxyFunction([](auto p1, auto p2) { return "DIRECT"; });
    EXPECT_EQ(proxy8001->Start(), true);
    auto proxy8002 = std::make_shared<ProxyServer>(8002, 0);
    proxy8002->SetFindPacProxyFunction([](auto p1, auto p2) { return "DIRECT"; });
    EXPECT_EQ(proxy8002->Start(), true);
    auto proxy8003 = std::make_shared<ProxyServer>(8003, 0);
    proxy8003->SetFindPacProxyFunction([](auto p1, auto p2) { return "DIRECT"; });
    EXPECT_EQ(proxy8003->Start(), true);

    proxy8000->SetFindPacProxyFunction(
        [](auto p1, auto p2) { return "PROXY 127.0.0.1:8001;PROXY 127.0.0.1:8002; PROXY 127.0.0.1:8003"; });

    std::string res = Request("http://127.0.0.1/", "127.0.0.1", 8000);
    EXPECT_EQ(res.empty(), true);
    res = Request("https://127.0.0.1/", "127.0.0.1", 8000);
    EXPECT_EQ(res.empty(), true);
    res = Request("http://icanhazip.com/", "127.0.0.1", 8000);
    res = Request("https://www.example.com/", "127.0.0.1", 8000, RequestOptions{false, false, 30});
    proxy8001->Stop();
    res = Request("https://www.example.com/", "127.0.0.1", 8000);
    proxy8002->Stop();
    res = Request("http://icanhazip.com/", "127.0.0.1", 8000);
    res = Request("https://www.example.com/", "127.0.0.1", 8000);
    proxy8003->Stop();
    res = Request("https://www.example.com/", "127.0.0.1", 8000);
    EXPECT_EQ(res.empty(), true);
    res = Request("http://icanhazip.com/", "127.0.0.1", 8000);
    EXPECT_EQ(res.empty(), true);
}

TEST(ProxyServerTest, CreateProxyTest11)
{
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "DIRECT"; });
    int32_t sockfd = -1;
    struct sockaddr_in serverAddr;
    struct hostent *server;
    std::string host = "127.0.0.1";
#define LARGET_SIZE (1024 * 50)
    std::string path(LARGET_SIZE, 'A');
    int32_t port = 8000;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    }
    server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Failed to resolve host: " << host << std::endl;
        close(sockfd);
    }
    memset_s(&serverAddr, sizeof(serverAddr), 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy_s(&serverAddr.sin_addr, sizeof(serverAddr.sin_addr), server->h_addr_list[0], server->h_length);
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }

    std::string response;
    std::string request =
        "GET /" + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n" + "\r\n";
    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send request failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    response.clear();
    std::vector<char> buffer(4096);
    ssize_t bytesReceived;
    while ((bytesReceived = recv(sockfd, buffer.data(), buffer.size(), 0)) > 0) {
        response.append(buffer.data(), bytesReceived);
    }
    if (bytesReceived < 0) {
        std::cerr << "Receive response failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    EXPECT_GE(bytesReceived, -1);
    close(sockfd);
}

TEST(ProxyServerTest, CreateProxyTest12)
{
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "DIRECT"; });
    int32_t sockfd = -1;
    struct sockaddr_in serverAddr;
    struct hostent *server;
    std::string host = "127.0.0.1";
    std::string path;
    int32_t port = 8000;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    }
    server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Failed to resolve host: " << host << std::endl;
        close(sockfd);
    }
    memset_s(&serverAddr, sizeof(serverAddr), 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy_s(&serverAddr.sin_addr, sizeof(serverAddr.sin_addr), server->h_addr_list[0], server->h_length);
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }

    std::string response;
    std::string request = "\r\n\r\n";
    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send request failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    response.clear();
    std::vector<char> buffer(4096);
    ssize_t bytesReceived;
    while ((bytesReceived = recv(sockfd, buffer.data(), buffer.size(), 0)) > 0) {
        response.append(buffer.data(), bytesReceived);
    }
    if (bytesReceived < 0) {
        std::cerr << "Receive response failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    EXPECT_EQ(bytesReceived, 0);
    close(sockfd);
}

TEST(ProxyServerTest, CreateProxyTest13)
{
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "DIRECT"; });
    int32_t sockfd = -1;
    struct sockaddr_in serverAddr;
    struct hostent *server;
    std::string host = "127.0.0.1";
    std::string path;
    int32_t port = 8000;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    }
    server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Failed to resolve host: " << host << std::endl;
        close(sockfd);
    }
    memset_s(&serverAddr, sizeof(serverAddr), 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy_s(&serverAddr.sin_addr, sizeof(serverAddr.sin_addr), server->h_addr_list[0], server->h_length);
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }

    std::string response;
    std::string request = "CONNECT \r\n\r\n";
    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send request failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    response.clear();
    std::vector<char> buffer(4096);
    ssize_t bytesReceived;
    while ((bytesReceived = recv(sockfd, buffer.data(), buffer.size(), 0)) > 0) {
        response.append(buffer.data(), bytesReceived);
    }
    if (bytesReceived < 0) {
        std::cerr << "Receive response failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    EXPECT_EQ(bytesReceived, 0);
    close(sockfd);
}

TEST(ProxyServerTest, CreateProxyTest14)
{
    long maxFd = sysconf(_SC_OPEN_MAX);
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "DIRECT"; });
    std::vector<int32_t> fds;
    struct rlimit limit;
    for (int32_t i = 0; i < maxFd; i++) {
        int32_t fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == -1) {
            RequestOptions opts;
            opts.largeHeader = false;
            std::string res = Request("https://www.example.com/", "127.0.0.1", 8000, opts);
            EXPECT_EQ(res.empty(), true);
            break;
        } else {
            fds.push_back(fd);
        }
    }
    for (auto fd : fds) {
        close(fd);
    }
}

TEST(ProxyServerTest, CreateProxyTest15)
{
    long maxFd = sysconf(_SC_OPEN_MAX);
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    std::string res = Request("http://icanhazip.com/", "127.0.0.1", 8000);
    EXPECT_EQ(res.empty(), true);
}

TEST(ProxyServerTest, CreateProxyTest16)
{
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "DIRECT"; });
    int32_t sockfd = -1;
    struct sockaddr_in serverAddr;
    struct hostent *server;
    std::string host = "127.0.0.1";
    std::string path;
    int32_t port = 8000;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    }
    server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Failed to resolve host: " << host << std::endl;
        close(sockfd);
    }
    memset_s(&serverAddr, sizeof(serverAddr), 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy_s(&serverAddr.sin_addr, sizeof(serverAddr.sin_addr), server->h_addr_list[0], server->h_length);
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }

    std::string response;
    std::string request = "GET \r\n\r\n";
    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send request failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    response.clear();
    std::vector<char> buffer(4096);
    ssize_t bytesReceived;
    while ((bytesReceived = recv(sockfd, buffer.data(), buffer.size(), 0)) > 0) {
        response.append(buffer.data(), bytesReceived);
    }
    if (bytesReceived < 0) {
        std::cerr << "Receive response failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    EXPECT_EQ(bytesReceived, 0);
    close(sockfd);
}

TEST(ProxyServerTest, CreateProxyTest17)
{
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "DIRECT"; });
    int32_t sockfd = -1;
    struct sockaddr_in serverAddr;
    struct hostent *server;
    std::string host = "127.0.0.1";
    std::string path;
    int32_t port = 8000;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    }
    server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Failed to resolve host: " << host << std::endl;
        close(sockfd);
    }
    memset_s(&serverAddr, sizeof(serverAddr), 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy_s(&serverAddr.sin_addr, sizeof(serverAddr.sin_addr), server->h_addr_list[0], server->h_length);
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }

    std::string response;
    std::string request = "GET \r\n \r\n\r\n";
    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send request failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    response.clear();
    std::vector<char> buffer(4096);
    ssize_t bytesReceived;
    while ((bytesReceived = recv(sockfd, buffer.data(), buffer.size(), 0)) > 0) {
        response.append(buffer.data(), bytesReceived);
    }
    if (bytesReceived < 0) {
        std::cerr << "Receive response failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    EXPECT_EQ(bytesReceived, 0);
    close(sockfd);
}

TEST(ProxyServerTest, CreateProxyTest18)
{
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "DIRECT"; });
    int32_t sockfd = -1;
    struct sockaddr_in serverAddr;
    struct hostent *server;
    std::string host = "127.0.0.1";
    std::string path;
    int32_t port = 8000;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    }
    server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Failed to resolve host: " << host << std::endl;
        close(sockfd);
    }
    memset_s(&serverAddr, sizeof(serverAddr), 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy_s(&serverAddr.sin_addr, sizeof(serverAddr.sin_addr), server->h_addr_list[0], server->h_length);
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }

    std::string response;
    std::string request = "GET \r\n Host: 127.0.0.1 \r\n\r\n";
    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send request failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    response.clear();
    std::vector<char> buffer(4096);
    ssize_t bytesReceived;
    while ((bytesReceived = recv(sockfd, buffer.data(), buffer.size(), 0)) > 0) {
        response.append(buffer.data(), bytesReceived);
    }
    if (bytesReceived < 0) {
        std::cerr << "Receive response failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    EXPECT_EQ(bytesReceived, 0);
    close(sockfd);
}

TEST(ProxyServerTest, CreateProxyTest19)
{
    auto proxy = std::make_shared<ProxyServer>(8000, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "DIRECT"; });
    int32_t sockfd = -1;
    struct sockaddr_in serverAddr;
    struct hostent *server;
    std::string host = "127.0.0.1";
    std::string path;
    int32_t port = 8000;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
    }
    server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Failed to resolve host: " << host << std::endl;
        close(sockfd);
    }
    memset_s(&serverAddr, sizeof(serverAddr), 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    memcpy_s(&serverAddr.sin_addr, sizeof(serverAddr.sin_addr), server->h_addr_list[0], server->h_length);
    if (connect(sockfd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }

    std::string response;
    std::string request = "GET \r\n Host: 127.0.0.1:9999 \r\n\r\n";
    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send request failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    response.clear();
    std::vector<char> buffer(4096);
    ssize_t bytesReceived;
    while ((bytesReceived = recv(sockfd, buffer.data(), buffer.size(), 0)) > 0) {
        response.append(buffer.data(), bytesReceived);
    }
    if (bytesReceived < 0) {
        std::cerr << "Receive response failed: " << strerror(errno) << std::endl;
        close(sockfd);
    }
    EXPECT_EQ(bytesReceived, 0);
    close(sockfd);
}

TEST(ProxyServerTest, CreateProxyTest20)
{
    auto proxy = std::make_shared<ProxyServer>(8888, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction(
        [](auto s, auto s1) { return "DIRECT 127.0.0.1:abc;PROXY 127.0.0.1:abc;DIRECT;PROXY 127.0.0.1"; });
    std::string res = Request("http://icanhazip.com/", "127.0.0.1", 8888);
}

TEST(ProxyServerTest, 21)
{
    auto proxy = std::make_shared<ProxyServer>(8888, 1);
    EXPECT_EQ(proxy->Start(), true);
    proxy->SetFindPacProxyFunction([](auto s, auto s1) { return "PROXY 127.0.0.1:111111111111 ;"; });
    std::string res = Request("http://icanhazip.com/", "127.0.0.1", 8888);
    EXPECT_EQ(res.empty(), true);
}
#endif
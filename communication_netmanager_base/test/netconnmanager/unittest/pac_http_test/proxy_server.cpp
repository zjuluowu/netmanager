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
#define LOG printf
#define ERROR printf
#include "proxy_server.h"
#include "securec.h"
#include "sys/time.h"
#include <algorithm>
#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <random>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define PRINT_RED_FMT_LN(fmt, ...) printf("\033[31m" fmt "\n\033[0m", ##__VA_ARGS__)
#define NUM_4 4
#define NUM_10 10
#define TIME_OUT 5
#define PORT_8080 8080
#define PORT_80 80
#define PORT_1080 1080
#define PORT_443 443
constexpr int BUFFER_SIZE = 1024 * 64;
constexpr int MS_1 = 1000;
using namespace OHOS::NetManagerStandard;

std::map<std::string, std::string> ProxyServer::pacScripts;
std::string ProxyServer::proxServerTargetUrl;
int32_t ProxyServer::proxServerPort;

ProxyServer::ProxyServer(int32_t port, int32_t numThreads)
    : port_(port), serverSocket_(-1), numThreads_(numThreads), running_(false)
{
    if (numThreads_ <= 0) {
        numThreads_ = std::thread::hardware_concurrency();
        if (numThreads_ <= 0) {
            numThreads_ = NUM_4;
        }
    }
    pacScripts = {
        {LOCAL_PROXY_9000,
         "function FindProxyForURL(url, host) {\n"
         "    return \"PROXY 127.0.0.1:9000\";\n"
         "}"},
        {LOCAL_PROXY_9001,
         "function FindProxyForURL(url, host) {\n"
         "    return \"PROXY 127.0.0.1:9001\";\n"
         "}"},
        {ALL_DIRECT,
         "function FindProxyForURL(url, host) {\n"
         "    return \"PROXY 127.0.0.1:9000;PROXY 127.0.0.1:9001; DIRECT\";\n"
         "}"},
    };
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
    }
    ResetStats();
}

ProxyServer::~ProxyServer()
{
    Stop();
}

void ProxyServer::SetFindPacProxyFunction(std::function<std::string(std::string, std::string)> pac)
{
    pacFunction_ = pac;
}

bool ProxyServer::Start()
{
    if (running_) {
        PRINT_RED_FMT_LN("server is runing \n");
        return false;
    }
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        PRINT_RED_FMT_LN("create socket fail \n");
        return false;
    }
    int32_t opt = 1;
    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        PRINT_RED_FMT_LN("SO_REUSEADDR fail \n");
        close(serverSocket_);
        serverSocket_ = -1;
        return false;
    }
    int32_t flags = fcntl(serverSocket_, F_GETFL, 0);
    if (flags < 0 || fcntl(serverSocket_, F_SETFL, flags | O_NONBLOCK) < 0) {
        printf("O_NONBLOCK fail \n");
        close(serverSocket_);
        serverSocket_ = -1;
        return false;
    }
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);
    if (bind(serverSocket_, reinterpret_cast<const sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0) {
        close(serverSocket_);
        serverSocket_ = -1;
        printf("bind port %d fail \n", port_);
        return false;
    }
    if (listen(serverSocket_, backlog) < 0) {
        close(serverSocket_);
        serverSocket_ = -1;
        printf("listen port %d fail \n", port_);
        return false;
    }
    LOG("run localserver on port:%d threads: %d \n", port_, numThreads_);
    running_ = true;
    ResetStats();
    for (int32_t i = 0; i < numThreads_; i++) {
        workers_.push_back(std::thread(&ProxyServer::WorkerThread, this));
    }
    acceptThread_ = std::thread(&ProxyServer::AcceptLoop, this);
    return true;
}

void ProxyServer::Stop()
{
    if (!running_) {
        return;
    }
    running_ = false;
    queueCondition_.notify_all();
    if (acceptThread_.joinable()) {
        acceptThread_.join();
    }
    for (auto &worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers_.clear();
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        while (!taskQueue_.empty()) {
            close(taskQueue_.front().clientSocket);
            taskQueue_.pop();
        }
    }
    if (serverSocket_ >= 0) {
        close(serverSocket_);
        serverSocket_ = -1;
    }
}

bool ProxyServer::IsRunning() const
{
    return running_;
}

std::shared_ptr<Stats> ProxyServer::GetStats()
{
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

void ProxyServer::ResetStats()
{
    std::lock_guard<std::mutex> lock(statsMutex_);
    stats_ = std::make_shared<Stats>();
    stats_->startTime = std::chrono::steady_clock::now();
}

double ProxyServer::GetThroughput() const
{
    std::lock_guard<std::mutex> lock(statsMutex_);
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - stats_->startTime).count();
    if (duration <= 0) {
        return 0.0;
    }
    uint64_t totalBytes = stats_->bytesReceived + stats_->bytesSent;
    return static_cast<double>(totalBytes) / duration;
}

std::string ProxyServer::GetRequestMethod(const std::string &header)
{
    size_t spacePos = header.find(' ');
    if (spacePos == std::string::npos) {
        return "";
    }
    return header.substr(0, spacePos);
}

bool ProxyServer::ParseConnectRequest(const std::string &header, std::string &host, int32_t &port)
{
    size_t methodEnd = header.find(' ');
    if (methodEnd == std::string::npos) {
        return false;
    }
    size_t hostStart = methodEnd + 1;
    size_t hostEnd = header.find(' ', hostStart);
    if (hostEnd == std::string::npos) {
        return false;
    }
    std::string hostPort = header.substr(hostStart, hostEnd - hostStart);
    size_t colonPos = hostPort.find(':');
    if (colonPos != std::string::npos) {
        host = hostPort.substr(0, colonPos);
        port = std::stoi(hostPort.substr(colonPos + 1));
    } else {
        host = hostPort;
        port = PORT_443;
    }
    return true;
}

bool ProxyServer::ParseHttpRequest(const std::string &header, std::string &host, int32_t &port)
{
    size_t hostPos = header.find("Host: ");
    if (hostPos == std::string::npos) {
        return false;
    }
    size_t hostEnd = header.find("\r\n", hostPos);
    if (hostEnd == std::string::npos) {
        return false;
    }
    std::string hostLine = header.substr(hostPos + 6, hostEnd - hostPos - 6);
    size_t colonPos = hostLine.find(':');
    if (colonPos != std::string::npos) {
        host = hostLine.substr(0, colonPos);
        port = std::stoi(hostLine.substr(colonPos + 1));
    } else {
        host = hostLine;
        port = PORT_80;
    }
    return true;
}

static bool HandlePollError(int32_t ret, int32_t errnoVal)
{
    if (ret < 0) {
        if (errnoVal == EINTR) {
            return false;
        }
        std::cerr << "Poll失败: " << strerror(errnoVal) << std::endl;
        return true;
    }
    return false;
}

static bool CheckPollHupOrErr(const struct pollfd *fds)
{
    return (fds[0].revents & (POLLHUP | POLLERR)) || (fds[1].revents & (POLLHUP | POLLERR));
}

static bool TransferData(int32_t srcFd, int32_t dstFd, char *buffer, size_t bufferSize, std::shared_ptr<Stats> stats)
{
    int32_t n = recv(srcFd, buffer, bufferSize, 0);
    if (n <= 0) {
        return true;
    }
    stats->bytesReceived += n;
    if (send(dstFd, buffer, n, 0) <= 0) {
        return true;
    }
    stats->bytesSent += n;
    return false;
}

ssize_t ProxyServer::SendAll(int sockfd, const void* buffer, size_t length, int32_t flag)
{
    const char* ptr = static_cast<const char*>(buffer);
    size_t totalSent = 0;
    while (totalSent < length) {
        ssize_t sent = send(sockfd, ptr + totalSent, length - totalSent, flag);
        if (sent < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                usleep(MS_1);
                continue;
            } else {
                return -1;
            }
        } else if (sent == 0) {
            break;
        }
        totalSent += static_cast<size_t>(sent);
    }
    return totalSent;
}

void ProxyServer::TunnelData(int client, int server)
{
    struct pollfd fds[2];
    fds[0].fd = client;
    fds[0].events = POLLIN;
    fds[1].fd = server;
    fds[1].events = POLLIN;
    char buffer[BUFFER_SIZE];
    auto lastActivity = std::chrono::steady_clock::now();
    while (running_) {
        int ret = poll(fds, 2, 100);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        auto now = std::chrono::steady_clock::now();
        if (ret == 0) {
            if (now - lastActivity > std::chrono::seconds(TIME_OUT)) {
                break;
            }
            continue;
        }
        lastActivity = now;
        if ((static_cast<unsigned short>(fds[0].revents) & (POLLHUP | POLLERR)) ||
                (static_cast<unsigned short>(fds[1].revents) & (POLLHUP | POLLERR))) {
            break;
        }
        if (fds[0].revents & POLLIN) {
            int n = recv(client, buffer, BUFFER_SIZE, 0);
            if (n <= 0) {
                break;
            }
            if (SendAll(server, buffer, n, 0) <= 0) {
                break;
            }
        }
        if (static_cast<unsigned short>(fds[1].revents) & POLLIN) {
            int n = recv(server, buffer, BUFFER_SIZE, 0);
            if (n <= 0) {
                break;
            }
            if (SendAll(client, buffer, n, 0) <= 0) {
                break;
            }
        }
    }
}

std::string ProxyServer::ReceiveResponseHeader(int32_t socket)
{
    std::string header;
    char buffer[bufferSize];

    int bytesRead;
    while ((bytesRead = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        header.append(buffer);
        if (header.find("\r\n\r\n") != std::string::npos) {
            break;
        }
        if (header.size() > maxHeaderSize) {
            break;
        }
    }
    return header;
}

bool ProxyServer::IsPortAvailable(int32_t port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        return false;
    }

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&optval), sizeof(optval)) < 0) {
        close(sockfd);
        return false;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int bindResult = bind(sockfd, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr));
    close(sockfd);
    return (bindResult == 0);
}

int ProxyServer::FindAvailablePort(int32_t startPort, int32_t endPort)
{
    std::vector<int> portsToTry;
    for (int port = startPort; port <= endPort; ++port) {
        portsToTry.push_back(port);
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(portsToTry.begin(), portsToTry.end(), std::default_random_engine(seed));

    for (int port : portsToTry) {
        if (IsPortAvailable(port)) {
            return port;
        }
    }

    return -1;
}

int ProxyServer::ConnectToServer(const std::string &host, int port)
{
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *rp;
    int serverSocket = -1;
    int ret = memset_s(&hints, sizeof(struct addrinfo), 0, sizeof(hints));
    if (ret != 0) {
        return -1;
    }
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    std::string portStr = std::to_string(port);
    int status = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result);
    if (status != 0) {
        return -1;
    }
    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        serverSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (serverSocket < 0) {
            continue;
        }
        struct timeval timeout;
        timeout.tv_sec = TIME_OUT;
        timeout.tv_usec = 0;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0 ||
            setsockopt(serverSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
            close(serverSocket);
            serverSocket = -1;
            continue;
        }
        if (connect(serverSocket, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(serverSocket);
        serverSocket = -1;
    }
    freeaddrinfo(result);
    if (serverSocket < 0) {
        return -1;
    }
    return serverSocket;
}

int ProxyServer::ConnectViaUpstreamProxy(const std::string &targetHost, int targetPort,
                                         const std::string &originalRequest, std::string proxyHost, int proxyPort)
{
    int proxySocket = ConnectToServer(proxyHost, proxyPort);
    if (proxySocket < 0) {
        ERROR("connect upstream proxy fail %s:%d\n", proxyHost.c_str(), proxyPort);
        return -1;
    }
    printf("ConnectToServer %s : %d \n", proxyHost.c_str(), proxyPort);
    printf("originalRequest ########## \n %s\n###########\n", originalRequest.c_str());
    if (send(proxySocket, originalRequest.c_str(), originalRequest.length(), 0) < 0) {
        close(proxySocket);
        return -1;
    }

    stats_->bytesSent += originalRequest.length();

    return proxySocket;
}

int ProxyServer::ConnectViaUpstreamProxyHttps(const std::string &targetHost, int targetPort, std::string proxyHost,
                                              int proxyPort)
{
    int proxySocket = ConnectToServer(proxyHost, proxyPort);
    if (proxySocket < 0) {
        ERROR("connect upproxy fail %s:%d fail", proxyHost.c_str(), proxyPort);
        return -1;
    }
    std::ostringstream connectRequest;
    connectRequest << "CONNECT " << targetHost << ":" << targetPort << " HTTP/1.1\r\n"
                   << "Host: " << targetHost << ":" << targetPort << "\r\n"
                   << "Proxy-Connection: Keep-Alive\r\n"
                   << "\r\n";
    std::string requestStr = connectRequest.str();
    if (send(proxySocket, requestStr.c_str(), requestStr.length(), 0) < 0) {
        ERROR("send CONNECT to upstream proxy fail \n");
        close(proxySocket);
        return -1;
    }
    stats_->bytesSent += requestStr.length();
    std::string response = ReceiveResponseHeader(proxySocket);
    if (response.empty() || response.find("HTTP/1.1 200") == std::string::npos) {
        std::cerr << "上游代理拒绝CONNECT请求: " << response << std::endl;
        close(proxySocket);
        return -1;
    }
    stats_->bytesReceived += response.length();
    return proxySocket;
}

std::string ProxyServer::GetRequestUrl(const std::string &header)
{
    std::string method = GetRequestMethod(header);
    std::string url;
    if (method == "CONNECT") {
        size_t methodEnd = header.find(' ');
        if (methodEnd == std::string::npos) {
            return "";
        }
        size_t hostStart = methodEnd + 1;
        size_t hostEnd = header.find(' ', hostStart);
        if (hostEnd == std::string::npos) {
            return "";
        }
        std::string hostPort = header.substr(hostStart, hostEnd - hostStart);
        url = "https://" + hostPort;
    } else {
        size_t methodEnd = header.find(' ');
        if (methodEnd == std::string::npos) {
            return "";
        }
        size_t pathStart = methodEnd + 1;
        size_t pathEnd = header.find(' ', pathStart);
        if (pathEnd == std::string::npos) {
            return "";
        }
        std::string path = header.substr(pathStart, pathEnd - pathStart);
        if (path.find("://") != std::string::npos) {
            return path;
        }
        std::string host;
        int port = PORT_80;
        if (!ParseHttpRequest(header, host, port)) {
            return path;
        }
        url = "http://";
        url += host;
        if (port != PORT_80) {
            url += ":" + std::to_string(port);
        }
        if (!path.empty() && path[0] != '/') {
            url += "/";
        }
        url += path;
    }

    return url;
}

bool ProxyServer::ParseProxyInfo(std::string url, std::string host, std::string &proxyType, std::string &proxyHost,
                                 int32_t &proxyPort)
{
    std::string pacScirpt;
    if (pacFunction_) {
        pacScirpt = pacFunction_(url, host);
    }
    if (pacScirpt.empty()) {
        return false;
    }
    return ParsePacResult(pacScirpt, proxyType, proxyHost, proxyPort);
}

bool ProxyServer::ParsePacResult(const std::string &pacResult, std::string &proxyType, std::string &proxyHost,
                                 int32_t &proxyPort)
{
    proxyType = "";
    proxyHost = "";
    proxyPort = 0;
    if (pacResult.empty()) {
        return false;
    }
    std::istringstream stream(pacResult);
    std::string rule;
    while (std::getline(stream, rule, ';')) {
        rule.erase(0, rule.find_first_not_of(" \t"));
        rule.erase(rule.find_last_not_of(" \t") + 1);
        if (rule.empty()) {
            continue;
        }
        size_t spacePos = rule.find(' ');
        if (spacePos == std::string::npos) {
            proxyType = rule;
            if (proxyType == "DIRECT") {
                return true;
            }
            continue;
        }
        proxyType = rule.substr(0, spacePos);
        size_t hostStart = rule.find_first_not_of(" \t", spacePos);
        if (hostStart == std::string::npos) {
            continue;
        }
        std::string hostPort = rule.substr(hostStart);
        size_t colonPos = hostPort.find(':');
        if (colonPos == std::string::npos) {
            proxyHost = hostPort;
            if (proxyType == "PROXY" || proxyType == "HTTP") {
                proxyPort = PORT_8080;
            } else if (proxyType == "SOCKS" || proxyType == "SOCKS5") {
                proxyPort = PORT_1080;
            } else if (proxyType == "SOCKS4") {
                proxyPort = PORT_1080;
            } else {
                proxyPort = PORT_8080;
            }
        } else {
            proxyHost = hostPort.substr(0, colonPos);
            proxyPort = std::stoi(hostPort.substr(colonPos + 1));
        }
        return true;
    }
    return false;
}

std::string AddHttpHeader(const std::string &httpMessage, const std::string &headerName, const std::string &headerValue)
{
    size_t headersEnd = httpMessage.find("\r\n\r\n");
    if (headersEnd == std::string::npos) {
        headersEnd = httpMessage.length();
    }
    std::string newHeader = headerName + ": " + headerValue + "\r\n";
    return httpMessage.substr(0, headersEnd) + newHeader + httpMessage.substr(headersEnd);
}

void ProxyServer::HandleClient(int32_t clientSocket)
{
    stats_->activeConnections++;

    // 设置超时
    if (!SetSocketTimeout(clientSocket)) {
        CleanupConnection(clientSocket);
        return;
    }

    // 读取请求头
    std::string requestHeader;
    if (!ReadRequestHeader(clientSocket, requestHeader)) {
        CleanupConnection(clientSocket);
        return;
    }

    printf("\033[33mproxy server read client data %.32s \n\033[0m", requestHeader.c_str());

    std::string method = GetRequestMethod(requestHeader);
    std::string url = GetRequestUrl(requestHeader);

    if (method == "CONNECT") {
        HandleConnectRequest(clientSocket, requestHeader, url);
    } else {
        HandleHttpRequest(clientSocket, requestHeader, url);
    }
}

bool ProxyServer::SetSocketTimeout(int32_t socket)
{
    struct timeval timeout;
    timeout.tv_sec = TIME_OUT;
    timeout.tv_usec = 0;

    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        return false;
    }
    return true;
}

bool ProxyServer::ReadRequestHeader(int32_t clientSocket, std::string &requestHeader)
{
    char buffer[bufferSize];
    int bytesReceived = 0;

    while ((bytesReceived = recv(clientSocket, buffer, bufferSize - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        requestHeader += buffer;
        stats_->bytesReceived += bytesReceived;

        if (requestHeader.find("\r\n\r\n") != std::string::npos) {
            break;
        }

        if (requestHeader.size() > maxHeaderSize) {
            return false;
        }
    }

    return bytesReceived > 0;
}

void ProxyServer::CleanupConnection(int32_t clientSocket)
{
    close(clientSocket);
    stats_->activeConnections--;
}

void ProxyServer::HandleConnectRequest(int32_t clientSocket, const std::string &requestHeader, const std::string &url)
{
    stats_->httpsRequests++;
    std::string host;
    int port;
    if (!ParseConnectRequest(requestHeader, host, port)) {
        SendErrorResponse(clientSocket, "HTTP/1.1 400 Bad Request\r\n\r\n");
        return;
    }
    proxServerTargetUrl = url;
    proxServerPort = port_;
    LOG("local proxy server port:%d url:%s host:%s port:%d \n", port_, url.c_str(), host.c_str(), port);
    std::string header;
    int serverSocket = EstablishServerConnection(url, host, port, "HTTPS", header);
    if (serverSocket < 0) {
        SendErrorResponse(clientSocket, "HTTP/1.1 502 Bad Gateway\r\n\r\n");
        return;
    }
    const char *response = "HTTP/1.1 200 Connection Established\r\n\r\n";
    int responseLen = strlen(response);
    if (send(clientSocket, response, responseLen, 0) < 0) {
        std::cerr << "发送CONNECT响应失败" << std::endl;
        close(serverSocket);
        CleanupConnection(clientSocket);
        return;
    }
    stats_->bytesSent += responseLen;
    TunnelData(clientSocket, serverSocket);
    close(serverSocket);
    CleanupConnection(clientSocket);
}

void ProxyServer::HandleHttpRequest(int32_t clientSocket, std::string &requestHeader, const std::string &url)
{
    stats_->httpRequests++;
    std::string host;
    int port;
    if (!ParseHttpRequest(requestHeader, host, port)) {
        std::cerr << "无法解析HTTP请求" << std::endl;
        SendErrorResponse(clientSocket, "HTTP/1.1 400 Bad Request\r\n\r\n");
        return;
    }
    LOG("\033[33mconnect to localport:%d %s %s \n\033[0m", port_, url.c_str(), host.c_str());
    int serverSocket = EstablishServerConnection(url, host, port, "HTTP", requestHeader);
    if (serverSocket < 0) {
        SendErrorResponse(clientSocket, "HTTP/1.1 502 Bad Gateway\r\n\r\n");
        return;
    }
    ForwardResponseToClient(clientSocket, serverSocket);
    close(serverSocket);
    CleanupConnection(clientSocket);
}

int ProxyServer::EstablishServerConnection(const std::string &url, const std::string &host, int32_t port,
                                           const std::string &requestType, std::string &requestHeader)
{
    std::string proxyType;
    std::string proxyHost;
    int proxyPort;
    bool useUpstreamProxy = ParseProxyInfo(url, host, proxyType, proxyHost, proxyPort);
    int serverSocket = -1;
    if (useUpstreamProxy && (proxyType == "PROXY" || proxyType == "HTTP")) {
        if (requestType == "HTTPS") {
            serverSocket = ConnectViaUpstreamProxyHttps(host, port, proxyHost, proxyPort);
        } else {
            serverSocket = ConnectViaUpstreamProxy(host, port, requestHeader, proxyHost, proxyPort);
        }
    } else if (requestType == "HTTP") {
        requestHeader = AddHttpHeader(requestHeader, "Proxy-Port", std::to_string(port_));
        printf("\033[33mnot proxy info direct send %.32s \n\033[0m", requestHeader.c_str());
        serverSocket = ConnectToServer(host, port);
        if (serverSocket >= 0) {
            if (send(serverSocket, requestHeader.c_str(), requestHeader.length(), 0) < 0) {
                close(serverSocket);
                serverSocket = -1;
            } else {
                stats_->bytesSent += requestHeader.length();
            }
        }
    } else {
        serverSocket = ConnectToServer(host, port);
    }
    return serverSocket;
}

void ProxyServer::SendErrorResponse(int32_t clientSocket, const char *response)
{
    send(clientSocket, response, strlen(response), 0);
    CleanupConnection(clientSocket);
}

void ProxyServer::ForwardResponseToClient(int32_t clientSocket, int32_t serverSocket)
{
    char buffer[bufferSize];
    int bytesReceived;

    while ((bytesReceived = recv(serverSocket, buffer, bufferSize, 0)) > 0) {
        stats_->bytesReceived += bytesReceived;

        if (send(clientSocket, buffer, bytesReceived, 0) < 0) {
            break;
        }

        stats_->bytesSent += bytesReceived;
    }
}
void ProxyServer::AddTask(const ClientTask &task)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        taskQueue_.push(task);
    }
    queueCondition_.notify_one();
}

void ProxyServer::WorkerThread()
{
    while (running_) {
        ClientTask task = {-1, {}};

        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCondition_.wait(lock, [this] { return !taskQueue_.empty() || !running_; });

            if (!running_ && taskQueue_.empty()) {
                break;
            }

            if (!taskQueue_.empty()) {
                task = taskQueue_.front();
                taskQueue_.pop();
            }
        }

        if (task.clientSocket >= 0) {
            HandleClient(task.clientSocket);
        }
    }
}

void ProxyServer::AcceptLoop()
{
    while (running_) {
        struct pollfd fd;
        fd.fd = serverSocket_;
        fd.events = POLLIN;
        int ret = poll(&fd, 1, 1000);
        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "Poll失败: " << strerror(errno) << std::endl;
            break;
        }
        if (ret == 0) {
            continue;
        }
        if (!(fd.revents & POLLIN)) {
            continue;
        }
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket_, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrLen);
        if (clientSocket < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            std::cerr << "接受连接失败: " << strerror(errno) << std::endl;
            continue;
        }
        stats_->totalConnections++;
        AddTask(ClientTask(clientSocket, clientAddr));
    }
}

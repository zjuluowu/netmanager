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

#include <sys/time.h>
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
#include <securec.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "netmanager_base_log.h"
#include "net_pac_local_proxy_server.h"
#include "netmanager_base_common_utils.h"
#include "securec.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int HTTPS_PORT = 443;
constexpr int HTTP_PORT = 80;
constexpr int TIME_OUT = 5;
constexpr int TIME_OUT_S = 5;
constexpr int THREAD_COUNT = 4;
constexpr int BUFFER_SIZE = 1024 * 64;
constexpr int BACKLOG = 128;
constexpr int MAX_HEADER_SIZE = 8192;
constexpr int MS_1 = 1000;
constexpr const char DEFAULT_URL[] = "127.0.0.1";
constexpr const char CONNECT_STR[] = "CONNECT";
constexpr const char PROTOCOL_SEPARATOR[] = "://";
constexpr const char HTTPS_PREFIX[] = "https://";
constexpr const char HTTP_PREFIX[] = "http://";
constexpr const char HOST_STR[] = "Host: ";
constexpr const char COLON_STR[] = ":";
constexpr const char SPACE_STR[] = " ";
constexpr const char PATH_STR[] = "/";
constexpr const char EMPTY_STR[] = "";
constexpr const char WHITESPACE_STR[] = " \t";
constexpr const char CRLF[] = "\r\n";
constexpr const char CRLF2[] = "\r\n\r\n";
constexpr const char HTTP_1_1_200[] = "HTTP/1.1 200";
constexpr const char HTTP_1_1[] = " HTTP/1.1\r\n";
constexpr const char PROXY_CONNECTION[] = "Proxy-Connection: Keep-Alive\r\n\r\n";
constexpr const char HTTP_1_1_400[] = "HTTP/1.1 400 Bad Request\r\n\r\n";
constexpr const char HTTP_1_1_502[] = "HTTP/1.1 502 Bad Gateway\r\n\r\n";
constexpr const char HTTP_1_1_200_CONNECTED[] = "HTTP/1.1 200 Connection Established\r\n\r\n";
constexpr const char DIRECT_STR[] = "DIRECT";
constexpr const char SOCKS_STR[] = "SOCKS";
constexpr const char SOCK5_STR[] = "SOCK5";
constexpr const char SOCK4_STR[] = "SOCK4";
constexpr const char PROXY_STR[] = "PROXY";
constexpr const char HTTP_STR[] = "HTTP";
constexpr const char HTTPS_STR[] = "HTTPS";
constexpr char SPACE_CHAR = ' ';
constexpr char COLON_CHAR = ':';
constexpr char PATH_CHAR = '/';
constexpr char NULL_CHAR = '\0';
constexpr char SEMICOLON_CHAR = ';';
} // namespace

ProxyServer::ProxyServer(int port, int numThreads)
    : port_(port), serverSocket_(-1), numThreads_(numThreads), running_(false)
{
    if (numThreads_ <= 0) {
        numThreads_ = static_cast<int>(std::thread::hardware_concurrency());
        if (numThreads_ <= 0) {
            numThreads_ = THREAD_COUNT;
        }
    }
}

ProxyServer::~ProxyServer()
{
    Stop();
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

void ProxyServer::SetFindPacProxyFunction(std::function<std::string(std::string, std::string)> pac)
{
    pacFunction_ = pac;
}

bool ProxyServer::Start()
{
    if (running_) {
        return false;
    }
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0) {
        NETMGR_LOG_E("create socket fail");
        return false;
    }
    int opt = 1;
    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        NETMGR_LOG_E("set socket SO_REUSEADDR fail");
        close(serverSocket_);
        serverSocket_ = -1;
        return false;
    }
    int flags = fcntl(serverSocket_, F_GETFL, 0);
    if (flags < 0 || fcntl(serverSocket_, F_SETFL, static_cast<unsigned short>(flags) | O_NONBLOCK) < 0) {
        NETMGR_LOG_E("set socket O_NONBLOCK fail");
        close(serverSocket_);
        serverSocket_ = -1;
        return false;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, DEFAULT_URL, &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(port_);
    if (bind(serverSocket_, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        close(serverSocket_);
        serverSocket_ = -1;
        return false;
    }
    if (listen(serverSocket_, BACKLOG) < 0) {
        close(serverSocket_);
        serverSocket_ = -1;
        return false;
    }
    running_ = true;
    for (int i = 0; i < numThreads_; i++) {
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
            close(taskQueue_.front().clientSocket_);
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

std::string ProxyServer::GetRequestMethod(const std::string &header)
{
    size_t spacePos = header.find(SPACE_CHAR);
    if (spacePos == std::string::npos)
        return EMPTY_STR;
    return header.substr(0, spacePos);
}

bool ProxyServer::ParseConnectRequest(const std::string &header, std::string &host, int &port)
{
    size_t methodEnd = header.find(SPACE_CHAR);
    if (methodEnd == std::string::npos)
        return false;
    size_t hostStart = methodEnd + 1;
    size_t hostEnd = header.find(SPACE_CHAR, hostStart);
    if (hostEnd == std::string::npos)
        return false;
    std::string hostPort = header.substr(hostStart, hostEnd - hostStart);
    size_t colonPos = hostPort.find(COLON_CHAR);
    if (colonPos != std::string::npos) {
        host = hostPort.substr(0, colonPos);
        port = CommonUtils::StrToInt(hostPort.substr(colonPos + 1));
    } else {
        host = hostPort;
        port = HTTPS_PORT;
    }
    return true;
}

bool ProxyServer::ParseHttpRequest(const std::string &header, std::string &host, int &port)
{
    size_t hostPos = header.find(HOST_STR);
    if (hostPos == std::string::npos)
        return false;
    size_t hostEnd = header.find(CRLF, hostPos);
    if (hostEnd == std::string::npos)
        return false;
    std::string hostLine = header.substr(hostPos + 6, hostEnd - hostPos - 6);
    size_t colonPos = hostLine.find(COLON_CHAR);
    if (colonPos != std::string::npos) {
        host = hostLine.substr(0, colonPos);
        port = CommonUtils::StrToInt(hostLine.substr(colonPos + 1));
    } else {
        host = hostLine;
        port = HTTP_PORT;
    }
    return true;
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
            if (errno == EINTR)
                continue;
            NETMGR_LOG_E("socket poll fail");
            break;
        }
        auto now = std::chrono::steady_clock::now();
        if (ret == 0) {
            if (now - lastActivity > std::chrono::seconds(TIME_OUT)) {
                NETMGR_LOG_D("Connection idle timeout, closing");
                break;
            }
            continue;
        }
        lastActivity = now;
        if ((static_cast<unsigned short>(fds[0].revents) & (POLLHUP | POLLERR)) ||
                (static_cast<unsigned short>(fds[1].revents) & (POLLHUP | POLLERR)))
            break;
        if ((static_cast<unsigned short>(fds[0].revents) & POLLIN) {
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

std::string ProxyServer::ReceiveResponseHeader(int socket)
{
    std::string header;
    char buffer[BUFFER_SIZE];
    int bytesRead;
    bool headerComplete = false;
    while (!headerComplete && header.size() < MAX_HEADER_SIZE) {
        bytesRead = recv(socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            header.append(buffer, bytesRead);
            if (header.find(CRLF2) != std::string::npos) {
                headerComplete = true;
            }
        } else if (bytesRead == 0) {
            NETMGR_LOG_W("Connection closed while receiving header, got %{private}zu bytes", header.size());
            break;
        } else {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                NETMGR_LOG_W("Socket timeout while receiving header");
                break;
            } else {
                NETMGR_LOG_E("recv error while receiving header: %{private}s", strerror(errno));
                break;
            }
        }
    }
    if (header.size() >= MAX_HEADER_SIZE && !headerComplete) {
        NETMGR_LOG_W("Header size limit reached without finding complete header");
    }
    return header;
}

int ProxyServer::ConnectToServer(const std::string &host, int port)
{
    struct addrinfo hints;
    struct addrinfo *result;
    struct addrinfo *rp;
    int serverSocket = -1;
    int ret = memset_s(&hints, sizeof(struct addrinfo), 0, sizeof(hints));
    if (ret != 0) {
        NETMGR_LOG_E("memset_s hints fail:");
        return -1;
    }
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    std::string portStr = std::to_string(port);
    int status = getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result);
    if (status != 0) {
        NETMGR_LOG_E("getaddrinfo fail: %s", gai_strerror(status));
        return -1;
    }
    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        serverSocket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (serverSocket < 0) {
            continue;
        }
        struct timeval timeout;
        timeout.tv_sec = TIME_OUT_S;
        timeout.tv_usec = 0;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0 ||
            setsockopt(serverSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
            NETMGR_LOG_E("set socket timeout fail: %s", strerror(errno));
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
        NETMGR_LOG_E("connect to %s:%d fail", host.c_str(), port);
        return -1;
    }
    return serverSocket;
}

int ProxyServer::ConnectViaUpstreamProxy(const std::string &targetHost, int targetPort,
                                         const std::string &originalRequest, std::string proxyHost, int proxyPort)
{
    int proxySocket = ConnectToServer(proxyHost, proxyPort);
    if (proxySocket < 0) {
        NETMGR_LOG_E("connect upstream proxy fail %{private}s:%{private}d", proxyHost.c_str(), proxyPort);
        return -1;
    }
    if (SendAll(proxySocket, originalRequest.c_str(), originalRequest.length(), 0) < 0) {
        close(proxySocket);
        return -1;
    }
    return proxySocket;
}

int ProxyServer::ConnectViaUpstreamProxyHttps(const std::string &targetHost, int targetPort, std::string proxyHost,
                                              int proxyPort)
{
    int proxySocket = ConnectToServer(proxyHost, proxyPort);
    if (proxySocket < 0) {
        NETMGR_LOG_E("connect upproxy fail %{private}s:%{private}d fail", proxyHost.c_str(), proxyPort);
        return -1;
    }
    std::ostringstream connectRequest;
    connectRequest << CONNECT_STR << SPACE_STR << targetHost << COLON_STR << targetPort << HTTP_1_1
                   << HOST_STR << targetHost << COLON_STR << targetPort << CRLF << PROXY_CONNECTION;
    std::string requestStr = connectRequest.str();
    if (SendAll(proxySocket, requestStr.c_str(), requestStr.length(), 0) < 0) {
        NETMGR_LOG_E("send CONNECT to upstream proxy fail ");
        close(proxySocket);
        return -1;
    }
    std::string response = ReceiveResponseHeader(proxySocket);
    if (response.empty() || response.find(HTTP_1_1_200) == std::string::npos) {
        close(proxySocket);
        return -1;
    }
    return proxySocket;
}

std::string ProxyServer::GetRequestUrl(const std::string &header)
{
    std::string method = GetRequestMethod(header);
    std::string url;
    if (method == CONNECT_STR) {
        size_t methodEnd = header.find(SPACE_CHAR);
        if (methodEnd == std::string::npos)
            return EMPTY_STR;
        size_t hostStart = methodEnd + 1;
        size_t hostEnd = header.find(SPACE_CHAR, hostStart);
        if (hostEnd == std::string::npos)
            return EMPTY_STR;
        std::string hostPort = header.substr(hostStart, hostEnd - hostStart);
        url = HTTPS_PREFIX + hostPort;
    } else {
        size_t methodEnd = header.find(SPACE_CHAR);
        if (methodEnd == std::string::npos)
            return EMPTY_STR;
        size_t pathStart = methodEnd + 1;
        size_t pathEnd = header.find(SPACE_CHAR, pathStart);
        if (pathEnd == std::string::npos)
            return EMPTY_STR;
        std::string path = header.substr(pathStart, pathEnd - pathStart);
        if (path.find(PROTOCOL_SEPARATOR) != std::string::npos) {
            return path;
        }
        std::string host;
        int port = HTTP_PORT;
        if (!ParseHttpRequest(header, host, port)) {
            return path;
        }
        url = HTTP_PREFIX;
        url += host;
        if (port != HTTP_PORT) {
            url += COLON_STR + std::to_string(port);
        }
        if (!path.empty() && path[0] != PATH_CHAR) {
            url += PATH_STR;
        }
        url += path;
    }
    return url;
}

void ProxyServer::GetProxyList(std::string url, std::string host, std::vector<ProxyConfig> &proxyList)
{
    std::string pacScript;
    if (pacFunction_) {
        pacScript = pacFunction_(url, host);
    }
    if (pacScript.empty()) {
        return;
    }
    ParsePacResult(pacScript, proxyList);
}

void ProxyServer::ParsePacResult(const std::string &pacResult, std::vector<ProxyConfig> &proxyList)
{
    proxyList.clear();
    if (pacResult.empty()) {
        return;
    }
    std::istringstream stream(pacResult);
    std::string rule;
    while (std::getline(stream, rule, SEMICOLON_CHAR)) {
        rule.erase(0, rule.find_first_not_of(WHITESPACE_STR));
        rule.erase(rule.find_last_not_of(WHITESPACE_STR) + 1);
        if (rule.empty()) {
            continue;
        }
        ProxyConfig config;
        size_t spacePos = rule.find(SPACE_CHAR);
        if (spacePos == std::string::npos) {
            config.type = rule;
            if (config.type == DIRECT_STR) {
                config.host = EMPTY_STR;
                config.port = 0;
                proxyList.push_back(config);
            }
            continue;
        }
        config.type = rule.substr(0, spacePos);
        if (config.type == DIRECT_STR) {
            config.host = EMPTY_STR;
            config.port = 0;
            proxyList.push_back(config);
            continue;
        }
        size_t hostStart = rule.find_first_not_of(WHITESPACE_STR, spacePos);
        if (hostStart == std::string::npos) {
            continue;
        }
        std::string hostPort = rule.substr(hostStart);
        size_t colonPos = hostPort.find(COLON_CHAR);
        if (colonPos == std::string::npos) {
            config.host = hostPort;
        } else {
            config.host = hostPort.substr(0, colonPos);
            config.port = CommonUtils::StrToInt(hostPort.substr(colonPos + 1));
            if (config.port == -1) {
                config.port = 0;
                NETMGR_LOG_E("pac config port paser fail");
            }
        }
        proxyList.push_back(config);
    }
}

bool ProxyServer::ReadRequestHeader(int clientSocket, std::string &requestHeader)
{
    char buffer[BUFFER_SIZE];
    struct timeval timeout;
    timeout.tv_sec = TIME_OUT;
    timeout.tv_usec = 0;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        return false;
    }
    int bytesReceived = 0;
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesReceived] = NULL_CHAR;
        requestHeader += buffer;
        if (requestHeader.find(CRLF2) != std::string::npos) {
            return true;
        }
        if (requestHeader.size() >= MAX_HEADER_SIZE) {
            return false;
        }
    }
    return bytesReceived > 0;
}

int ProxyServer::TryConnectWithProxyList(const std::string &targetHost, int targetPort,
                                         const std::vector<ProxyConfig> &proxyList, bool isHttps,
                                         const std::string &requestHeader)
{
    int serverSocket = -1;
    for (const auto &[proxyType, proxyHost, proxyPort] : proxyList) {
        if (proxyType == SOCKS_STR || proxyType == SOCK5_STR || proxyType == SOCK4_STR) {
            NETMGR_LOG_D("SOCKS proxy not implemented yet: %{private}s:%{private}d", proxyHost.c_str(), proxyPort);
            continue;
        }
        if (proxyType == DIRECT_STR) {
            serverSocket = HandleDirectConnection(targetHost, targetPort, isHttps, requestHeader);
        } else if (proxyType == PROXY_STR || proxyType == HTTP_STR) {
            serverSocket = HandleProxyConnection(targetHost, targetPort, proxyHost, proxyPort, isHttps, requestHeader);
        }
        if (serverSocket >= 0) {
            LogSuccessfulConnection(proxyType, targetHost, targetPort, proxyHost, proxyPort);
            break;
        } else {
            LogFailedConnection(proxyType, targetHost, targetPort, proxyHost, proxyPort);
        }
    }
    return serverSocket;
}

int ProxyServer::HandleDirectConnection(const std::string &targetHost, int targetPort, bool isHttps,
                                        const std::string &requestHeader)
{
    NETMGR_LOG_D("Trying DIRECT connection to %{private}s:%{private}d", targetHost.c_str(), targetPort);
    int serverSocket = ConnectToServer(targetHost, targetPort);
    if (serverSocket >= 0 && !isHttps && !requestHeader.empty()) {
        if (SendAll(serverSocket, requestHeader.c_str(), requestHeader.length(), 0) < 0) {
            close(serverSocket);
            serverSocket = -1;
        }
    }
    return serverSocket;
}

int ProxyServer::HandleProxyConnection(const std::string &targetHost, int targetPort, const std::string &proxyHost,
                                       int proxyPort, bool isHttps, const std::string &requestHeader)
{
    NETMGR_LOG_D("Trying HTTP proxy %{private}s:%{private}d for %{private}s:%{private}d",
        proxyHost.c_str(), proxyPort, targetHost.c_str(), targetPort);
    if (isHttps) {
        return ConnectViaUpstreamProxyHttps(targetHost, targetPort, proxyHost, proxyPort);
    } else {
        return ConnectViaUpstreamProxy(targetHost, targetPort, requestHeader, proxyHost, proxyPort);
    }
}

void ProxyServer::LogSuccessfulConnection(const std::string &proxyType, const std::string &targetHost, int targetPort,
                                          const std::string &proxyHost, int proxyPort)
{
    NETMGR_LOG_D("Successfully connected via %{private}s %{private}s:%{private}d", proxyType.c_str(),
        proxyType == DIRECT_STR ? targetHost.c_str() : proxyHost.c_str(),
        proxyType == DIRECT_STR ? targetPort : proxyPort);
}

void ProxyServer::LogFailedConnection(const std::string &proxyType, const std::string &targetHost, int targetPort,
                                      const std::string &proxyHost, int proxyPort)
{
    NETMGR_LOG_D("Failed to connect via %{private}s %{private}s:%{private}d", proxyType.c_str(),
        proxyType == DIRECT_STR ? targetHost.c_str() : proxyHost.c_str(),
        proxyType == DIRECT_STR ? targetPort : proxyPort);
}

void ProxyServer::SendErrorResponse(int clientSocket, const char *response)
{
    SendAll(clientSocket, response, strlen(response), 0);
}

void ProxyServer::HandleConnectRequest(int clientSocket, const std::string &requestHeader, const std::string &url)
{
    std::string host;
    int port;
    if (!ParseConnectRequest(requestHeader, host, port)) {
        SendErrorResponse(clientSocket, HTTP_1_1_400);
        return;
    }

    std::vector<ProxyConfig> proxyList;
    GetProxyList(url, host, proxyList);
    int serverSocket = TryConnectWithProxyList(host, port, proxyList, true);
    if (serverSocket < 0) {
        SendErrorResponse(clientSocket, HTTP_1_1_502);
        return;
    }
    if (SendAll(clientSocket, HTTP_1_1_200_CONNECTED, strlen(HTTP_1_1_200_CONNECTED), 0) < 0) {
        NETMGR_LOG_E("send CONNECT Response fail");
        close(serverSocket);
        return;
    }
    TunnelData(clientSocket, serverSocket);
    close(serverSocket);
}

void ProxyServer::ForwardData(int fromSocket, int toSocket)
{
    char buffer[BUFFER_SIZE];
    int bytesReceived;
    while ((bytesReceived = recv(fromSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        if (SendAll(toSocket, buffer, bytesReceived, 0) < 0) {
            break;
        }
    }
}

void ProxyServer::HandleHttpRequest(int clientSocket, const std::string &requestHeader, const std::string &url)
{
    std::string host;
    int port;
    if (!ParseHttpRequest(requestHeader, host, port)) {
        NETMGR_LOG_E("Parse Http Header Fail");
        SendErrorResponse(clientSocket, HTTP_1_1_400);
        return;
    }
    NETMGR_LOG_D("HTTP request - local port:%{private}d url:%{private}s host:%{private}s",
        port_, url.c_str(), host.c_str());
    std::vector<ProxyConfig> proxyList;
    GetProxyList(url, host, proxyList);
    int serverSocket = TryConnectWithProxyList(host, port, proxyList, false, requestHeader);
    if (serverSocket < 0) {
        SendErrorResponse(clientSocket, HTTP_1_1_502);
        return;
    }
    ForwardData(serverSocket, clientSocket);
    close(serverSocket);
}

void ProxyServer::HandleClient(int clientSocket)
{
    std::string requestHeader;
    if (!ReadRequestHeader(clientSocket, requestHeader)) {
        close(clientSocket);
        return;
    }
    std::string method = GetRequestMethod(requestHeader);
    std::string url = GetRequestUrl(requestHeader);
    if (method == CONNECT_STR) {
        HandleConnectRequest(clientSocket, requestHeader, url);
    } else {
        HandleHttpRequest(clientSocket, requestHeader, url);
    }
    close(clientSocket);
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
        if (task.clientSocket_ >= 0) {
            HandleClient(task.clientSocket_);
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
            if (errno == EINTR)
                continue;
            NETMGR_LOG_E("socket poll fail");
            break;
        }
        if (ret == 0) {
            continue;
        }
        if (!(static_cast<unsigned short>(fd.revents) & POLLIN)) {
            continue;
        }
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket_, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            continue;
        }
        AddTask(ClientTask(clientSocket, clientAddr));
    }
}

bool ProxyServer::IsPortAvailable(int port)
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
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));
    if (!inet_pton(AF_INET, DEFAULT_URL, &(serverAddr.sin_addr))) {
        return false;
    }
    int bindResult = bind(sockfd, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr));
    if (bindResult < 0) {
        const char *errmsg = strerror(errno);
        NETMGR_LOG_E("bind error %{private}s", errmsg);
    }
    close(sockfd);
    return (bindResult == 0);
}

int ProxyServer::FindAvailablePort(int startPort, int endPort)
{
    std::vector<int> portsToTry;
    for (int port = startPort; port <= endPort; ++port) {
        portsToTry.push_back(port);
    }
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(portsToTry.begin(), portsToTry.end(), std::default_random_engine(seed));
    for (int port : portsToTry) {
        if (IsPortAvailable(port)) {
            return port;
        }
    }
    return -1;
}
} // namespace NetManagerStandard
} // namespace OHOS

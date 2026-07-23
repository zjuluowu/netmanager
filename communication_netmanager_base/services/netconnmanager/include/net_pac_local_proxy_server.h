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
#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <netinet/in.h>
#include <queue>
#include <string>
#include <thread>
#include <vector>
namespace OHOS {
namespace NetManagerStandard {

class ProxyServer {
public:
    ProxyServer(int port, int numThreads);
    ~ProxyServer();
    void SetFindPacProxyFunction(std::function<std::string(std::string, std::string)> pac);
    bool Start();
    void Stop();
    bool IsRunning() const;
    double GetThroughput() const;
    static int FindAvailablePort(int startPort, int endPort);
private:
    struct ProxyConfig {
        std::string type;
        std::string host;
        int port;
    };
    ssize_t SendAll(int sockfd, const void* buffer, size_t length, int32_t flag);
    static bool IsPortAvailable(int port);
    void ParsePacResult(const std::string &pacResult, std::vector<ProxyConfig> &proxyList);
    int HandleDirectConnection(const std::string &targetHost, int targetPort, bool isHttps,
                               const std::string &requestHeader);
    int HandleProxyConnection(const std::string &targetHost, int targetPort, const std::string &proxyHost,
                              int proxyPort, bool isHttps, const std::string &requestHeader);
    void LogSuccessfulConnection(const std::string &proxyType,
                            const std::string &targetHost, int targetPort,
                            const std::string &proxyHost, int proxyPort);
    void LogFailedConnection(const std::string &proxyType,
                            const std::string &targetHost, int targetPort,
                            const std::string &proxyHost, int proxyPort);
    void GetProxyList(std::string url, std::string host, std::vector<ProxyConfig> &proxyList);
    bool ReadRequestHeader(int clientSocket, std::string &requestHeader);
    int TryConnectWithProxyList(const std::string &targetHost, int targetPort,
                                const std::vector<ProxyConfig> &proxyList, bool isHttps,
                                const std::string &requestHeader = "");
    void HandleConnectRequest(int clientSocket, const std::string &requestHeader, const std::string &url);
    void HandleHttpRequest(int clientSocket, const std::string &requestHeader, const std::string &url);
    void SendErrorResponse(int clientSocket, const char *response);
    void ForwardData(int fromSocket, int toSocket);
    struct ClientTask {
        int clientSocket_;
        struct sockaddr_in clientAddr_;
        ClientTask(int socket, const struct sockaddr_in &addr) : clientSocket_(socket), clientAddr_(addr) {}
    };
    void HandleClient(int clientSocket);
    std::string GetRequestUrl(const std::string &header);
    std::string GetRequestMethod(const std::string &header);
    bool ParseConnectRequest(const std::string &header, std::string &host, int &port);
    bool ParseHttpRequest(const std::string &header, std::string &host, int &port);
    void TunnelData(int client, int server);
    int ConnectToServer(const std::string &host, int port);
    int ConnectViaUpstreamProxy(const std::string &targetHost, int targetPort, const std::string &originalRequest,
                                std::string proxyHost, int proxyPort);
    int ConnectViaUpstreamProxyHttps(const std::string &targetHost, int targetPort, std::string proxyHost,
                                     int proxyPort);
    void AcceptLoop();
    void WorkerThread();
    void AddTask(const ClientTask &task);
    std::string ReceiveResponseHeader(int socket);
    int port_;
    int serverSocket_;
    int numThreads_;
    std::atomic<bool> running_;
    std::thread acceptThread_;
    std::vector<std::thread> workers_;
    std::function<std::string(std::string, std::string)> pacFunction_;
    std::queue<ClientTask> taskQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // PROXY_SERVER_H

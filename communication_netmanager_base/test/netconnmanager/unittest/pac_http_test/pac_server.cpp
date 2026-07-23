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

#include "thread"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "functional"

static std::function<void()> g_handler;
static std::string g_defaultFileContent =
    "// PAC (Proxy Auto-Configuration) 脚本示例\n"
    "// 包含所有主要PAC函数的使用示例，只演示基础函数使用。\n"
    "// 目前扩展辅助函数功能支持不全面，不建议使用。\n"
    "// isInNetEx和isInNet功能相同\n"
    "// myIpAddressEx返回所有本地ip\n"
    "// dnsResolveEx和dnsResolve相同，只返回一个地址\n"
    "// sortIpAddressList不支持\n"
    "\n"
    "function isIP(str) {\n"
    "    // 正则表达式检查是否为有效的 IP 地址\n"
    "    var ipPattern = "
    "/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|["
    "01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;\n"
    "    return ipPattern.test(str);\n"
    "}\n"
    "\n"
    "function FindProxyForURL(url, host) {\n"
    "    // console.info(1);\n"
    "    // 本机地址直接访问\n"
    "    if (host === \"127.0.0.1\" || host === \"localhost\") {\n"
    "        return \"DIRECT\";\n"
    "    }\n"
    "    // 2. 检查本地域名\n"
    "    //本机域名直接访问\n"
    "    if (dnsDomainIs(host, \".local\") ||\n"
    "        dnsDomainIs(host, \".localhost\") ||\n"
    "        localHostOrDomainIs(host, \"localhost\")) {\n"
    "        return \"DIRECT\";\n"
    "    }\n"
    "    // 3. 检查内网地址\n"
    "    // 内网地址直接访问\n"
    "    var myIP = myIpAddress();\n"
    "    if (isInNet(host, \"192.168.0.0\", \"255.255.0.0\") ||\n"
    "        isInNet(host, \"10.0.0.0\", \"255.0.0.0\") ||\n"
    "        isInNet(host, \"172.16.0.0\", \"255.240.0.0\") ||\n"
    "        isInNet(myIP, \"192.168.0.0\", \"255.255.0.0\")) {\n"
    "        return \"DIRECT\";\n"
    "    }\n"
    "    // 5 ip地址使用代理\n"
    "    if (isIP(host)) {\n"
    "        return \"PROXY special-proxy.com:8000\";\n"
    "    }\n"
    "    var pattern=/baidu/g; \n"
    "    if(pattern.test(host)){\n"
    "        return \"PROXY special-proxy.com:7000\";\n"
    "    }\n"
    "    // 4. 检查纯主机使用代理\n"
    "    if (isPlainHostName(host)) {\n"
    "        return \"PROXY special-proxy.com:8001\";\n"
    "    }\n"
    "    // 4. 基于时间的规则\n"
    "    // 周一到周五的时间\n"
    "    if (weekdayRange(\"MON\", \"FRI\")) {\n"
    "        //九点到18点的时间\n"
    "        if (timeRange(9, 18)) {\n"
    "            if (dnsDomainIs(host, \".company.com\")) {\n"
    "                return \"PROXY special-proxy.com:8002\";\n"
    "            }\n"
    "        } else {\n"
    "            return \"PROXY special-proxy.com:8102\";\n"
    "        }\n"
    "    }\n"
    "    // 5. 基于日期的规则\n"
    "    // 1月到3月\n"
    "    if (dateRange(\"JAN\", \"MAR\")) {\n"
    "        // 域名判断\n"
    "        if (shExpMatch(host, \"*.special.com\")) {\n"
    "            return \"PROXY special-proxy.com:8003\";\n"
    "        }else{\n"
    "            return \"PROXY special-proxy.com:8103\";\n"
    "        }\n"
    "    }\n"
    "    // 6. 检查域名级别\n"
    "    if (dnsDomainLevels(host) >= 3) {\n"
    "        // 多级域名使用特定代理\n"
    "        return \"PROXY special-proxy.com:8004\";\n"
    "    }\n"
    "    // 7. 可解析性检查\n"
    "    if (isResolvable(host)) {\n"
    "        var resolvedIP = dnsResolve(host);\n"
    "        if (resolvedIP && isInNet(resolvedIP, \"116.205.4.0\", \"255.255.255.0\")) {\n"
    "            return \"PROXY special-proxy.com:8005\";\n"
    "        }\n"
    "    }\n"
    "    // 8. Shell表达式匹配\n"
    "    if (shExpMatch(url, \"http://download*.example.com/*\") ||\n"
    "        shExpMatch(url, \"https://*.cdn.com/*\")) {\n"
    "        return \"PROXY special-proxy.com:8006\";\n"
    "    }\n"
    "    // 扩展函数示例（部分浏览器支持）\n"
    "    try {\n"
    "        // 9. 扩展IP检查 返回所有的本地ip，myIpAddress只会返回127.0.0.1，\n"
    "        var allMyIPs = myIpAddressEx();\n"
    "        // isInNetEx和isInNet功能相同\n"
    "        if (allMyIPs && isInNetEx(host, allMyIPs)) {\n"
    "            return \"PROXY special-proxy.com:8007\";\n"
    "        }\n"
    "        // 10. 扩展DNS解析\n"
    "        //dnsResolveEx和dnsResolve相同，不支持返回多个域名的ip\n"
    "        var allIPs = dnsResolveEx(host);\n"
    "        if (allIPs) {\n"
    "            //sortIpAddressList函数不支持\n"
    "            var sortedIPs = sortIpAddressList(allIPs);\n"
    "            // 使用排序后的IP列表进行进一步处理\n"
    "        }\n"
    "        // 11. 扩展可解析性检查\n"
    "        //isResolvableEx和isResolvable相同\n"
    "        if (isResolvableEx(host)) {\n"
    "            // 进行额外的处理\n"
    "        }\n"
    "    } catch (e) {\n"
    "        // 扩展函数不支持时的降级处理\n"
    "    }\n"
    "    // 默认规则\n"
    "    return \"PROXY default-proxy.com:8080; DIRECT\";\n"
    "}";

bool IsTestRequest(const std::string &request)
{
    std::size_t pos = request.find("GET ");
    if (pos != std::string::npos) {
        std::string pathPart = request.substr(pos + 4);
        std::size_t httpPos = pathPart.find("http://");
        if (httpPos != std::string::npos) {
            std::size_t slashPos = pathPart.find("/", httpPos + 7);
            if (slashPos != std::string::npos) {
                pathPart = pathPart.substr(slashPos);
            }
        }
        return (pathPart.find("/test ") == 0 || pathPart.find("/test/ ") == 0 || pathPart.find("/test\r") == 0 ||
                pathPart.find("/test/\r") == 0 || pathPart.find("/test\n") == 0 || pathPart.find("/test/\n") == 0);
    }
    return false;
}

void SetTestHttpHandler(std::function<void()> function)
{
    g_handler = function;
}

std::string GetHeaderValue(const std::string &request, const std::string &headerName)
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

int32_t InitializeServerSocket(int32_t port, const std::string &ip)
{
    int32_t serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        return -1;
    }
    int32_t opt = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        return -1;
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    if (ip.empty()) {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, ip.c_str(), &(address.sin_addr));
    }
    address.sin_port = htons(port);
    if (bind(serverFd, reinterpret_cast<const sockaddr *>(&address), sizeof(address)) < 0) {
        return -1;
    }
#define TIME_OUT_3S 3
    if (listen(serverFd, TIME_OUT_3S) < 0) {
        return -1;
    }
    return serverFd;
}

void HandleTestRequest(int32_t socket, const std::string &request, struct sockaddr_in &address)
{
    std::string clientIp = inet_ntoa(address.sin_addr);
    int32_t clientPort = ntohs(address.sin_port);
    bool isProxy = request.find("Proxy-Connection") != std::string::npos;
    std::string info = "{\"ClientIp\":\"";
    info += clientIp;
    info += "\",";
    info += "\"ClientPort\":\"";
    info += std::to_string(clientPort);
    info += "\",";
    info += "\"Proxy-Connection\":\"";
    info += GetHeaderValue(request, "Proxy-Connection");
    info += "\",";
    info += "\"GlobalProxyIp\":\"";
    info += GetHeaderValue(request, "GlobalProxyIp");
    info += "\",";
    info += "\"GlobalProxyPort\":\"";
    info += GetHeaderValue(request, "GlobalProxyPort");
    info += "\",";
    info += "\"Proxy-Port\":\"";
    info += GetHeaderValue(request, "Proxy-Port");
    info += "\",";
    info += "\"isProxy\":\"";
    info += std::to_string(isProxy);
    info += "\"}";
    printf("\033[34mproxy server read client data %s \n\033[0m", info.c_str());
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/json; charset=UTF-8\r\n"
        "Content-Length: " +
        std::to_string(info.length()) +
        "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        info;
    send(socket, response.c_str(), response.length(), 0);
}

void HandlePacRequest(int32_t socket, const std::string &content)
{
    std::string filename = "download.txt";
    printf("\033[34msend pac script %.128s \n\033[0m", content.c_str());

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=UTF-8\r\n"
        "Content-Disposition: attachment; filename=\"" +
        filename +
        "\"\r\n"
        "Content-Length: " +
        std::to_string(content.length()) +
        "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        content;

    send(socket, response.c_str(), response.length(), 0);
}

void HandleClientConnection(int32_t serverFd, std::string pacScript)
{
    struct sockaddr_in address;
    int32_t addrlen = sizeof(address);
    if (pacScript.empty()) {
        pacScript = g_defaultFileContent;
    }
#define SIZE_1024 1024
    char buffer[SIZE_1024] = {0};

    int32_t clientSocket =
        accept(serverFd, reinterpret_cast<sockaddr *>(&address), reinterpret_cast<socklen_t *>(&addrlen));
    if (clientSocket < 0) {
        return;
    }

    read(clientSocket, buffer, SIZE_1024);
    std::string request(buffer);

    if (IsTestRequest(request)) {
        if (g_handler) {
            g_handler();
        }
        HandleTestRequest(clientSocket, request, address);
    } else {
        HandlePacRequest(clientSocket, pacScript);
    }

    close(clientSocket);
}

static bool g_isRunning = true;

void StartHttpServer(int32_t port, std::string ip, std::string pacScript)
{
    std::thread httpThread([port, ip, pacScript]() {
        int32_t serverFd = InitializeServerSocket(port, ip);
        if (serverFd < 0) {
            return 1;
        }
        std::string displayIp = ip.empty() ? "127.0.0.1" : ip;
        std::cout << "Pac Server Start ，PacFileURL http://" << displayIp << ":" << port << "/" << std::endl;
        while (g_isRunning) {
            HandleClientConnection(serverFd, pacScript);
        }
        close(serverFd);
        return 0;
    });

    httpThread.detach();
}
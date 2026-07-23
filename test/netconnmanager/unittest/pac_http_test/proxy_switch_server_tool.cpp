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
#define PROXY_PORT_9000 9000
#define PROXY_PORT_9001 9001
#define PROT_8080 8080
#define PROT_8889 8889
#define TIME 20

static int32_t g_counter = 0;

static void StartProxyServer(int32_t port)
{
    std::shared_ptr<ProxyServer> server = std::make_shared<ProxyServer>(port);
    services.insert({port, server});
    server->Start();
}

TEST(PROXY_SWITCH_TEST, PacFileUrlClient)
{
    StartProxyServer(PROXY_PORT_9001);
    StartProxyServer(PROXY_PORT_9000);
    services[PROXY_PORT_9000]->Stop();
    services[PROXY_PORT_9001]->Stop();
    SetUpPermission();
    StartHttpServer(PROT_8080, "", "");
    SetTestHttpHandler([]() {
        int32_t n = g_counter % 3;
        switch (n) {
            case 0:
                services[PROXY_PORT_9001]->Start();
                break;
            case 1:
                services[PROXY_PORT_9000]->Start();
                break;
            default:
                break;
        }
        g_counter++;
    });
    std::string script = ProxyServer::pacScripts[ALL_DIRECT];
    printf(" pac script  %s \n", script.c_str());
    StartHttpServer(PROT_8889, "", script);
    EXPECT_EQ(services.empty(), false);
    sleep(TIME);
    UnsetUpPermission();
}
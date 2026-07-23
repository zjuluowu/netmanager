/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <iostream>
#include <memory>

#include "iservice_registry.h"
#include "netsys_native_service_proxy.h"
#include "system_ability_definition.h"
#include "netnative_log_wrapper.h"
#include "test.h"

using namespace OHOS;
using namespace OHOS::NetsysNative;
using namespace OHOS::nmd;
enum class InterfaceMenu {
    SET_RESOLVER_CONFIG = 0,
    CREATE_NETWORK_CACHE = 1,
    FLUSH_NETWORK_CACHE = 2,
    DESTROY_NETWORK_CACHE = 3,
    GET_ADDR_INFO = 4,
    INTERFACE_SET_MTU = 5,
    INTERFACE_GET_MTU = 6,
    REGISTER_NOTIFY_CALLBACK = 7,
    NETWORK_ADD_ROUTE = 8,
    NETWORK_REMOVE_ROUTE = 9,
    NETWORK_ADD_ROUTE_PARCEL = 10,
    NETWORK_REMOVE_ROUTE_PARCEL = 11,
    NETWORK_SET_DEFAULT = 12,
    NETWORK_GET_DEFAULT = 13,
    NETWORK_CREATE_PHYSICAL = 14,
    INTERFACE_ADD_ADDRESS = 15,
    INTERFACE_DEL_ADDRESS = 16,
    NETWORK_ADD_INTERFACE = 17,
    NETWORK_REMOVE_INTERFACE = 18,
    GET_FWMARK_FOR_NETWORK = 19,
    INTERFACE_SET_CFG = 20,
    NETWORK_ClEAR_DEFAULT = 21,
    GET_PROC_SYS_NET = 22,
    INTERFACE_GET_CFG = 23,
    GET_RESOLVER_CONFIG = 24,
    NETWORK_CLEAR_DEFAULT = 25,
    NETWORK_GET_DEFAULT_UNION = 26,
    NETWORK_ADD_INTERFACE_WIFI = 27,
    NETWORK_REMOVE_INTERFACE_WIFI = 28,
    NETWORK_SET_DEFAULT_WIFI = 29,
    SET_TCP_BUFFER_SIZES = 30,
    INPUT_QUIT = 100,
};

namespace {
std::map<InterfaceMenu, NetsysTestFunc> g_memberFuncMap;
void Init()
{
    g_memberFuncMap[InterfaceMenu::SET_RESOLVER_CONFIG] = TestSetResolverConfig;
    g_memberFuncMap[InterfaceMenu::CREATE_NETWORK_CACHE] = TestCreateNetworkCache;
    g_memberFuncMap[InterfaceMenu::FLUSH_NETWORK_CACHE] = TestFlushNetworkCache;
    g_memberFuncMap[InterfaceMenu::DESTROY_NETWORK_CACHE] = TestDestroyNetworkCache;
    g_memberFuncMap[InterfaceMenu::INTERFACE_SET_MTU] = TestInterfaceSetMtu;
    g_memberFuncMap[InterfaceMenu::INTERFACE_GET_MTU] = TestInterfaceGetMtu;
    g_memberFuncMap[InterfaceMenu::REGISTER_NOTIFY_CALLBACK] = TestRegisterNotifyCallback;
    g_memberFuncMap[InterfaceMenu::NETWORK_ADD_ROUTE] = TestNetworkAddRoute;
    g_memberFuncMap[InterfaceMenu::NETWORK_REMOVE_ROUTE] = TestNetworkRemoveRoute;
    g_memberFuncMap[InterfaceMenu::NETWORK_ADD_ROUTE_PARCEL] = TestNetworkAddRouteParcel;
    g_memberFuncMap[InterfaceMenu::NETWORK_REMOVE_ROUTE_PARCEL] = TestNetWorkRemoveRouteParcel;
    g_memberFuncMap[InterfaceMenu::NETWORK_SET_DEFAULT] = TestNetworkSetDefault;
    g_memberFuncMap[InterfaceMenu::NETWORK_GET_DEFAULT] = TestNetworkGetDefault;
    g_memberFuncMap[InterfaceMenu::NETWORK_CLEAR_DEFAULT] = TestNetworkClearDefault;
    g_memberFuncMap[InterfaceMenu::NETWORK_CREATE_PHYSICAL] = TestNetworkCreatePhysical;
    g_memberFuncMap[InterfaceMenu::INTERFACE_ADD_ADDRESS] = TestInterfaceAddAddress;
    g_memberFuncMap[InterfaceMenu::INTERFACE_DEL_ADDRESS] = TestInterfaceDelAddress;
    g_memberFuncMap[InterfaceMenu::NETWORK_ADD_INTERFACE] = TestNetworkAddInterface;
    g_memberFuncMap[InterfaceMenu::NETWORK_REMOVE_INTERFACE] = TestNetworkRemoveInterface;
    g_memberFuncMap[InterfaceMenu::GET_FWMARK_FOR_NETWORK] = TestGetFwmarkForNetwork;
    g_memberFuncMap[InterfaceMenu::INTERFACE_SET_CFG] = TestInterfaceSetCfg;
    g_memberFuncMap[InterfaceMenu::NETWORK_ClEAR_DEFAULT] = TestNetworkClearDefault;
    g_memberFuncMap[InterfaceMenu::GET_PROC_SYS_NET] = TestNetGetProcSysNet;
    g_memberFuncMap[InterfaceMenu::INTERFACE_GET_CFG] = TestInterfaceGetCfg;
    g_memberFuncMap[InterfaceMenu::GET_RESOLVER_CONFIG] = TestGetResolverConfig;
    g_memberFuncMap[InterfaceMenu::NETWORK_GET_DEFAULT_UNION] = TestNetworkGetDefaultUnion;

    g_memberFuncMap[InterfaceMenu::NETWORK_ADD_INTERFACE_WIFI] = TestNetworkAddInterfaceWIFI;
    g_memberFuncMap[InterfaceMenu::NETWORK_REMOVE_INTERFACE_WIFI] = TestNetworkRemoveInterfaceWIFI;
    g_memberFuncMap[InterfaceMenu::NETWORK_SET_DEFAULT_WIFI] = TestNetworkSetDefaultWIFI;
    g_memberFuncMap[InterfaceMenu::SET_TCP_BUFFER_SIZES] = TestSetTcpBufferSizes;
}

void Prompt()
{
    printf(
        "\n-----------start test netsys api--------------\n"
        "0 TestSetResolverConfig\n"
        "1 TestCreateNetworkCache\n"
        "2 TestFlushNetworkCache\n"
        "3 TestDestroyNetworkCache\n"
        "5 TestInterfaceSetMtu\n"
        "6 TestInterfaceGetMtu\n"
        "7 TestRegisterNotifyCallback\n"
        "8 TestNetworkAddRoute\n"
        "9 TestNetworkRemoveRoute\n"
        "10 TestNetworkAddRouteParcel\n"
        "11 TestNetWorkRemoveRouteParcel\n"
        "12 TestNetworkSetDefault\n"
        "13 TestNetworkGetDefault\n"
        "14 TestNetworkCreatePhysical\n"
        "15 TestInterfaceAddAddress\n"
        "16 TestInterfaceDelAddress\n"
        "17 TestNetworkAddInterface\n"
        "18 TestNetworkRemoveInterface\n"
        "19 TestGetFwmarkForNetwork\n"
        "20 TestInterfaceSetCfg\n"
        "21 TestNetworkClearDefault\n"
        "22 TestNetGetProcSysNet\n"
        "23 TestInterfaceGetCfg\n"
        "24 TestGetResolverConfig\n"
        "25 TestNetworkClearDefault\n"
        "26 TestNetworkGetDefaultUnion\n"
        "27 TestNetworkAddInterfaceWIFI\n"
        "28 TestNetworkRemoveInterfaceWIFI\n"
        "29 TestNetworkSetDefaultWIFI\n"
        "30 SetTcpBufferSizes\n"
        "100:exit \n");
}

int32_t GetInputData()
{
    int32_t input;
    std::cin >> input;
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore();
        printf("Input error, please input number again\n");
        std::cin >> input;
    }
    return input;
}

void ProcessInput(bool &loopFlag)
{
    int32_t inputCMD = GetInputData();
    printf("Your choice is %d \n", inputCMD);
    auto itFunc = g_memberFuncMap.find((InterfaceMenu)inputCMD);
    if (itFunc != g_memberFuncMap.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            (*memberFunc)();
            return;
        }
    }
    printf("inputCMD is:[%d]\n", inputCMD);
    switch (static_cast<InterfaceMenu>(inputCMD)) {
        case InterfaceMenu::INPUT_QUIT: {
            loopFlag = false;
            printf("exit...\n");
            break;
        }
        default:
            printf("please input correct number...\n");
            break;
    }
}
}
int main(int argc, char const *argv[])
{
    Init();
    bool loopFlag = true;
    while (loopFlag) {
        Prompt();
        ProcessInput(loopFlag);
    }

    NETNATIVE_LOGI("...exit test...");
    return 0;
}

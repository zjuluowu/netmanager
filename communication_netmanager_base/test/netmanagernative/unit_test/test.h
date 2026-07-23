/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NETSYS_MAIN_TEST_H
#define NETSYS_MAIN_TEST_H

#include <cstdint>
#include <iostream>
#include <map>

#include "i_netsys_service.h"
#include "test_notify_callback.h"

using NetsysTestFunc = void (*)();

void TestSetResolverConfig(void);
void TestCreateNetworkCache(void);
void TestFlushNetworkCache(void);
void TestDestroyNetworkCache(void);
void TestInterfaceSetMtu(void);
void TestInterfaceGetMtu(void);
void TestRegisterNotifyCallback(void);
void TestNetworkAddRoute(void);
void TestNetworkRemoveRoute(void);
void TestNetworkAddRouteParcel(void);
void TestNetWorkRemoveRouteParcel(void);
void TestNetworkSetDefault(void);
void TestNetworkGetDefault(void);
void TestNetworkCreatePhysical(void);
void TestInterfaceAddAddress(void);
void TestInterfaceDelAddress(void);
void TestNetworkAddInterface(void);
void TestNetworkRemoveInterface(void);
void TestGetFwmarkForNetwork(void);
void TestInterfaceSetCfg(void);
void TestNetworkClearDefault(void);
void TestNetGetProcSysNet(void);
void TestInterfaceGetCfg(void);
void TestGetResolverConfig(void);
void TestNetworkGetDefaultUnion(void);

void TestNetworkSetDefaultWIFI(void);
void TestNetworkAddInterfaceWIFI(void);
void TestNetworkRemoveInterfaceWIFI(void);
void TestSetTcpBufferSizes(void);

#endif // NETSYS_MAIN_TEST_H

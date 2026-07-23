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
#include <securec.h>

#include "i_netsys_service.h"
#include "iservice_registry.h"
#include "netnative_log_wrapper.h"
#include "netsys_native_service_proxy.h"
#include "system_ability_definition.h"
#include "test.h"
#include "test_notify_callback.h"
using namespace OHOS::nmd;
using namespace OHOS;
using namespace OHOS::NetsysNative;

namespace {
OHOS::sptr<OHOS::NetsysNative::INotifyCallback> callback_ =
    (std::make_unique<OHOS::NetsysNative::TestNotifyCallback>()).release();
sptr<INetsysService> GetProxyK()
{
    NETNATIVE_LOGE("Get samgr >>>>>>>>>>>>>>>>>>>>>>>>>>");
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    NETNATIVE_LOGE("Get samgr %{public}p", samgr.GetRefPtr());
    std::cout << "Get samgr  " << samgr.GetRefPtr() << std::endl;

    auto remote = samgr->GetSystemAbility(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    NETNATIVE_LOGE("Get remote %{public}p", remote.GetRefPtr());
    std::cout << "Get remote " << remote.GetRefPtr() << std::endl;

    auto proxy = iface_cast<NetsysNative::INetsysService>(remote);
    if (proxy != nullptr) {
        NETNATIVE_LOGE("Get proxy %{public}p", proxy.GetRefPtr());
        std::cout << "Get proxy " << proxy.GetRefPtr() << std::endl;
    } else {
        std::cout << "Get proxy nullptr" << std::endl;
    }

    return proxy;
}

auto netsysServiceK_ = GetProxyK();
} // namespace

namespace {
void FreeNetsysAddrInfo(struct addrinfo *aihead)
{
    struct addrinfo *ai;
    struct addrinfo *ainext;
    for (ai = aihead; ai != nullptr; ai = ainext) {
        if (ai->ai_addr != nullptr) {
            free(ai->ai_addr);
        }

        if (ai->ai_canonname != nullptr) {
            free(ai->ai_canonname);
        }
        ainext = ai->ai_next;
        free(ai);
    }
}
} // namespace

void TestSetResolverConfig()
{
    int ret = netsysServiceK_->CreateNetworkCache(0);
    NETNATIVE_LOGE("NETSYS: CreateNetworkCache0   ret=%{public}d", ret);
    ret = netsysServiceK_->CreateNetworkCache(1);
    NETNATIVE_LOGE("NETSYS: CreateNetworkCache1   ret=%{public}d", ret);
}

void TestGetResolverConfig()
{
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    uint16_t baseTimeoutMsec;
    uint8_t retryCount;
    int num = 3;
    for (int i = 0; i < num; i++) {
        int32_t ret = netsysServiceK_->GetResolverConfig(i, servers, domains, baseTimeoutMsec, retryCount);
        NETNATIVE_LOGE("NETSYS: getResolverConfig   ret=%{public}d, iii=%{public}d", ret, i);
        NETNATIVE_LOGE("NETSYS:  server size %{public}d, domains  size %{public}d",
                       static_cast<int32_t>(servers.size()), static_cast<int32_t>(domains.size()));
    }
}

void TestInterfaceGetCfg()
{
    std::cout << "hello" << std::endl;
}

void TestCreateNetworkCache()
{
    int ret = -1;
    ret = netsysServiceK_->CreateNetworkCache(0);
    NETNATIVE_LOGE("NETSYS: CreateNetworkCache0   ret=%{public}d", ret);
    ret = netsysServiceK_->CreateNetworkCache(1);
    NETNATIVE_LOGE("NETSYS: CreateNetworkCache1   ret=%{public}d", ret);
}

void TestFlushNetworkCache()
{
    int ret = -1;
    ret = netsysServiceK_->FlushNetworkCache(0);
    NETNATIVE_LOGE("NETSYS: FlushNetworkCache0   ret=%{public}d", ret);
    ret = netsysServiceK_->FlushNetworkCache(1);
    NETNATIVE_LOGE("NETSYS: FlushNetworkCache1   ret=%{public}d", ret);
}

void TestDestroyNetworkCache()
{
    int ret = -1;
    ret = netsysServiceK_->DestroyNetworkCache(1);
    NETNATIVE_LOGE("NETSYS: DestroyNetworkCache1   ret=%{public}d", ret);
}

void TestInterfaceSetMtu()
{
    int ret = -1;
    std::string ifName = "eth0";
    int mtu = 1200;
    std::cout << "begin to GetMtu" << std::endl;
    ret = netsysServiceK_->GetInterfaceMtu(ifName);
    NETNATIVE_LOGE("NETSYS: GetMtu   ago  ret=%{public}d", ret);
    std::cout << "begin to SetMtu" << std::endl;
    ret = netsysServiceK_->SetInterfaceMtu(ifName, mtu);
    NETNATIVE_LOGE("NETSYS: SetMtu   ret=%{public}d", ret);
    std::cout << "begin22 to GetMtu" << std::endl;
    ret = netsysServiceK_->GetInterfaceMtu(ifName);
    NETNATIVE_LOGE("NETSYS: GetMtu   ret=%{public}d", ret);
}

void TestInterfaceGetMtu()
{
    int ret = -1;
    std::string ifName = "eth0";
    int mtu = 1200;
    std::cout << "begin to GetMtu" << std::endl;
    ret = netsysServiceK_->GetInterfaceMtu(ifName);
    NETNATIVE_LOGE("NETSYS: GetMtu   ago  ret=%{public}d", ret);
    std::cout << "begin to SetMtu" << std::endl;
    ret = netsysServiceK_->SetInterfaceMtu(ifName, mtu);
    NETNATIVE_LOGE("NETSYS: SetMtu   ret=%{public}d", ret);
    std::cout << "begin22 to GetMtu" << std::endl;
    ret = netsysServiceK_->GetInterfaceMtu(ifName);
    NETNATIVE_LOGE("NETSYS: GetMtu   ret=%{public}d", ret);
}

void TestRegisterNotifyCallback()
{
    if (netsysServiceK_ == nullptr || callback_ == nullptr) {
        std::cout << "TestRegisterNotifyCallback netsysServiceK_ or callback is nullptr" << std::endl;
        return;
    }
    std::cout << "enter TestRegisterNotifyCallback " << std::endl;
    int32_t ret = netsysServiceK_->RegisterNotifyCallback(callback_);
    std::cout << "TestRegisterNotifyCallback ret:" << ret << std::endl;

    return;
}

void TestSetTcpBufferSizes()
{
    std::string tcpBufferSizes = "524288,1048576,2097152,262144,524288,1048576";
    std::cout << "begin to SetTcpBufferSizes" << std::endl;
    ret = netsysServiceK_->SetTcpBufferSizes(tcpBufferSizes);
    NETNATIVE_LOGE("NETSYS: SetTcpBufferSizes   ret=%{public}d", ret);
}
/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "i_netsys_service.h"
#include "iservice_registry.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "netsys_native_service_proxy.h"
#include "system_ability_definition.h"
#include "test.h"
#include <iostream>
#include <memory>

using namespace OHOS::nmd;
using namespace OHOS;
using namespace OHOS::NetsysNative;
const int NETID = 12;
const int NETID_TEST = 13;
const int IPVERSION = 4;
const int MASK_MAX = 65535;
namespace {
sptr<INetsysService> GetProxyR()
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

auto netsysServiceR_ = GetProxyR();
} // namespace

void TestNetworkAddRoute()
{
    NETNATIVE_LOGI("Entry TestNetworkAddRoute...");
    if (netsysServiceR_ == nullptr) {
        std::cout << "TestNetworkAddRoute netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    netsysServiceR_->NetworkCreatePhysical(NETID, nmd::NetworkPermission::PERMISSION_NONE);
    NETNATIVE_LOGI("NetworkAddInterface, net");
    int ret = netsysServiceR_->NetworkAddInterface(NETID, "eth0");
    NETNATIVE_LOGE("result %{public}d", ret);
    ret = netsysServiceR_->NetworkAddRoute(NETID, "eth0", "192.168.1.3/24", "192.168.1.1");
    NETNATIVE_LOGE("ret=%{public}d", ret);
}

void TestNetworkRemoveRoute()
{
    int ret = -1;
    if (netsysServiceR_ == nullptr) {
        std::cout << "TestNetworkRemoveRoute netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    ret = netsysServiceR_->NetworkRemoveRoute(NETID, "eth0", "192.168.1.3/32", "192.168.1.1");
    NETNATIVE_LOGE("ret=%{public}d", ret);
}

void TestNetworkAddRouteParcel()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << "TestNetworkAddRouteParcel netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    nmd::RouteInfoParcel routeInfoParcel = {"", "", "", 1500};
    netsysServiceR_->NetworkCreatePhysical(NETID, nmd::NetworkPermission::PERMISSION_NONE);
    netsysServiceR_->NetworkAddInterface(NETID, "eth0");
    netsysServiceR_->NetworkAddRouteParcel(NETID, routeInfoParcel);
    nmd::MarkMaskParcel testFwmark;
    int32_t result = netsysServiceR_->GetFwmarkForNetwork(NETID, testFwmark);
    std::cout << "TestNetworkAddRouteParcel result " << result << std::endl;
    if (testFwmark.mark != NETID) {
        std::cout << "mark is " << testFwmark.mark << std::endl;
    }
    if (testFwmark.mask != MASK_MAX) {
        std::cout << "mark is " << testFwmark.mark << std::endl;
    }
}

void TestNetWorkRemoveRouteParcel()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << "TestNetworkRemoveRouteParcel netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    nmd::RouteInfoParcel routeInfoParcel = {"", "", "", 1500};
    netsysServiceR_->NetworkCreatePhysical(NETID, nmd::NetworkPermission::PERMISSION_NONE);
    netsysServiceR_->NetworkAddInterface(NETID, "eth0");
    netsysServiceR_->NetworkRemoveRouteParcel(NETID, routeInfoParcel);
}

void TestNetworkSetDefault()
{
    int netid = NETID;
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestNetworkSetDefault netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->NetworkSetDefault(netid);
    std::cout << "  TestNetworkSetDefault   ret =" << ret << std::endl;
}

void TestNetworkSetDefaultWIFI()
{
    int netid = NETID_TEST;
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestNetworkSetDefaultWIFI netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->NetworkSetDefault(netid);
    std::cout << "  TestNetworkSetDefaultWIFI   ret =" << ret << std::endl;
}

void TestNetworkGetDefault()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestNetworkGetDefault netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->NetworkGetDefault();
    std::cout << "  TestNetworkGetDefault   ret =" << ret << std::endl;
}

void TestNetworkClearDefault()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestNetworkClearDefault netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->NetworkClearDefault();
    std::cout << "  TestNetworkClearDefault   ret =" << ret << std::endl;
}

void TestNetworkGetDefaultUnion()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestNetworkGetDefaultUnion netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int32_t ret = netsysServiceR_->NetworkCreatePhysical(NETID, OHOS::nmd::NetworkPermission::PERMISSION_NONE);
    NETNATIVE_LOGE("NetworkCreatePhysical   ret =%{public}d", ret);
    int32_t id = netsysServiceR_->NetworkGetDefault();
    NETNATIVE_LOGE("NetworkDefault   id =%{public}d", id);
    netsysServiceR_->NetworkSetDefault(NETID);
    id = netsysServiceR_->NetworkGetDefault();
    NETNATIVE_LOGE("NetworkDefault  after SET  id =%{public}d", id);
    ret = netsysServiceR_->NetworkCreatePhysical(NETID_TEST, OHOS::nmd::NetworkPermission::PERMISSION_NONE);
    NETNATIVE_LOGE("NetworkCreatePhysical_A   ret =%{public}d", ret);
    id = netsysServiceR_->NetworkGetDefault();
    NETNATIVE_LOGE("NetworkDefault   id =%{public}d", id);
    netsysServiceR_->NetworkSetDefault(NETID_TEST);
    id = netsysServiceR_->NetworkGetDefault();
    NETNATIVE_LOGE("NetworkDefault  after SET  id =%{public}d", id);
    netsysServiceR_->NetworkClearDefault();
    id = netsysServiceR_->NetworkGetDefault();
    NETNATIVE_LOGE("NetworkDefault  after clear default  id =%{public}d", id);
}

void TestNetworkCreatePhysical()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << "  TestNetworkCreatePhysical netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->NetworkCreatePhysical(NETID, OHOS::nmd::NetworkPermission::PERMISSION_NONE);
    std::cout << "  TestNetworkCreatePhysical   ret =" << ret << std::endl;
}

void TestInterfaceAddAddress()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << "  TestAddInterfaceAddress netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->AddInterfaceAddress("eth0", "172.17.5.245", 23);
    std::cout << "  TestAddInterfaceAddress    ret =" << ret << std::endl;
}

void TestInterfaceDelAddress()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << "  TestDelInterfaceAddress netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->DelInterfaceAddress("eth0", "172.17.5.245", 23);
    std::cout << "  TestDelInterfaceAddress    ret =" << ret << std::endl;
}

void TestNetworkAddInterface()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << "  TestNetworkAddInterface  netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = -1;
    ret = netsysServiceR_->NetworkCreatePhysical(NETID, OHOS::nmd::NetworkPermission::PERMISSION_NONE);
    NETNATIVE_LOGE("createPhysical  ret = %{public}d", ret);
    ret = netsysServiceR_->NetworkAddInterface(NETID, "rmnet0");
    NETNATIVE_LOGE("networkAddInterface   ret = %{public}d", ret);
}

void TestNetworkRemoveInterface()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestNetworkRemoveInterface  netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->NetworkRemoveInterface(NETID, "rmnet0");
    NETNATIVE_LOGE("networkRemoveInterface ret = %{public}d", ret);
}

void TestNetworkAddInterfaceWIFI()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << "  TestNetworkAddInterfaceWIFI  netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = -1;
    ret = netsysServiceR_->NetworkCreatePhysical(NETID_TEST, OHOS::nmd::NetworkPermission::PERMISSION_NONE);
    NETNATIVE_LOGE("TestNetworkAddInterfaceWIFI  ret = %{public}d", ret);
    ret = netsysServiceR_->NetworkAddInterface(NETID, "wlan0");
    NETNATIVE_LOGE("TestNetworkAddInterfaceWIFI   ret = %{public}d", ret);
}

void TestNetworkRemoveInterfaceWIFI()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestNetworkRemoveInterfaceWIFI  netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    int ret = netsysServiceR_->NetworkRemoveInterface(NETID_TEST, "wlan0");
    NETNATIVE_LOGE("TestNetworkRemoveInterfaceWIFI ret = %{public}d", ret);
}

void TestGetFwmarkForNetwork()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestGetFwmakrForNetwork  netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    OHOS::nmd::MarkMaskParcel testFwmark = {0, 0};
    int ret = netsysServiceR_->GetFwmarkForNetwork(NETID, testFwmark);
    NETNATIVE_LOGE("mark %{public}d,mask %{public}d,  ret=%{public}d", testFwmark.mark, testFwmark.mask, ret);
}

void TestInterfaceSetCfg()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestInterfaceSetCfg  netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    OHOS::nmd::InterfaceConfigurationParcel parcel;
    parcel.ifName = "eth0";
    NETNATIVE_LOGE("ZZZZ:TestInterfaceSetCfg");
    int ret = netsysServiceR_->GetInterfaceConfig(parcel);
    NETNATIVE_LOGE("before: parcel get ipv4Addr = %{public}s", CommonUtils::ToAnonymousIp(parcel.ipv4Addr).c_str());
    parcel.ipv4Addr = std::string("192.168.55.121");
    ret = netsysServiceR_->SetInterfaceConfig(parcel);
    NETNATIVE_LOGE("SetInterfaceConfig  ret  %{public}d", ret);
    ret = netsysServiceR_->GetInterfaceConfig(parcel);
    NETNATIVE_LOGE("after: parcel get ipv4Addr = %{public}s", CommonUtils::ToAnonymousIp(parcel.ipv4Addr).c_str());
}

void TestNetGetProcSysNet()
{
    if (netsysServiceR_ == nullptr) {
        std::cout << " TestNetGetProcSysNet  netsysServiceR_ is nullptr" << std::endl;
        return;
    }
    std::string value = "5";
    int ret = -1;
    NETNATIVE_LOGE("SetProcSysNet  start");
    ret = netsysServiceR_->SetProcSysNet(IPVERSION, 1, std::string("eth0"), std::string("disable_policy"), value);
    NETNATIVE_LOGE("SetProcSysNet  ret:%{public}d, value:%{public}s \n", ret, value.c_str());
    std::string readValue;
    ret = netsysServiceR_->GetProcSysNet(IPVERSION, 1, std::string("eth0"), std::string("disable_policy"), readValue);
    NETNATIVE_LOGE("GetProcSysNet  ret:%{public}d, readValue=%{public}s\n", ret, readValue.c_str());
    NETNATIVE_LOGE("NetGetProcSysNet  OVER");
}

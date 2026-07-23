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

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <pthread.h>
#include <unistd.h>

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "net_conn_constants.h"
#include "net_conn_types.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"
#include "netsys_native_client.h"
#include "netsys_native_service_proxy.h"
#include "ipc_skeleton.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
namespace OHOS {
namespace NetManagerStandard {
static constexpr const char *DEV_NET_TUN_PATH = "/dev/net/tun";
static constexpr const char *IF_CFG_UP = "up";
static constexpr const char *IF_CFG_DOWN = "down";
static constexpr const char *NETSYS_ROUTE_INIT_DIR_PATH = "/data/service/el1/public/netmanager/route";
static constexpr uint32_t WAIT_FOR_SERVICE_TIME_S = 1;
static constexpr uint32_t MAX_GET_SERVICE_COUNT = 30;
static constexpr uint32_t IPV4_MAX_LENGTH = 32;
static constexpr int UID_FOUNDATION = 5523;

NetsysNativeClient::NativeNotifyCallback::NativeNotifyCallback(std::weak_ptr<NetsysNativeClient> netsysNativeClient)
    : netsysNativeClient_(netsysNativeClient)
{
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceAddressUpdated(const std::string &addr,
                                                                            const std::string &ifName, int flags,
                                                                            int scope)
{
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbObjMutex_);
    for (auto cb = netsysNativeClient->cbObjects_.begin(); cb != netsysNativeClient->cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbObjects_.erase(cb);
        } else {
            (*cb)->OnInterfaceAddressUpdated(addr, ifName, flags, scope);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceAddressRemoved(const std::string &addr,
                                                                            const std::string &ifName, int flags,
                                                                            int scope)
{
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbObjMutex_);
    for (auto cb = netsysNativeClient->cbObjects_.begin(); cb != netsysNativeClient->cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbObjects_.erase(cb);
        } else {
            (*cb)->OnInterfaceAddressRemoved(addr, ifName, flags, scope);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceAdded(const std::string &ifName)
{
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbObjMutex_);
    for (auto cb = netsysNativeClient->cbObjects_.begin(); cb != netsysNativeClient->cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbObjects_.erase(cb);
        } else {
            (*cb)->OnInterfaceAdded(ifName);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceRemoved(const std::string &ifName)
{
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbObjMutex_);
    for (auto cb = netsysNativeClient->cbObjects_.begin(); cb != netsysNativeClient->cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbObjects_.erase(cb);
        } else {
            (*cb)->OnInterfaceRemoved(ifName);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceChanged(const std::string &ifName, bool up)
{
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbObjMutex_);
    for (auto cb = netsysNativeClient->cbObjects_.begin(); cb != netsysNativeClient->cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbObjects_.erase(cb);
        } else {
            (*cb)->OnInterfaceChanged(ifName, up);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceLinkStateChanged(const std::string &ifName, bool up)
{
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbObjMutex_);
    for (auto cb = netsysNativeClient->cbObjects_.begin(); cb != netsysNativeClient->cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbObjects_.erase(cb);
        } else {
            (*cb)->OnInterfaceLinkStateChanged(ifName, up);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnRouteChanged(bool updated, const std::string &route,
                                                                 const std::string &gateway, const std::string &ifName)
{
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbObjMutex_);
    for (auto cb = netsysNativeClient->cbObjects_.begin(); cb != netsysNativeClient->cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbObjects_.erase(cb);
        } else {
            (*cb)->OnRouteChanged(updated, route, gateway, ifName);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnDhcpSuccess(sptr<OHOS::NetsysNative::DhcpResultParcel> &dhcpResult)
{
    NETMGR_LOG_I("OnDhcpSuccess");
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netsysNativeClient->ProcessDhcpResult(dhcpResult);
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnBandwidthReachedLimit(const std::string &limitName,
                                                                          const std::string &iface)
{
    NETMGR_LOG_I("OnBandwidthReachedLimit");
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netsysNativeClient->ProcessBandwidthReachedLimit(limitName, iface);
    return NETMANAGER_SUCCESS;
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
int32_t NetsysNativeClient::NativeNotifyCallback::OnInterceptRecord(sptr<NetManagerStandard::InterceptRecord> &record)
{
    NETMGR_LOG_I("OnInterceptRecord");
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard<std::shared_mutex> lock(netsysNativeClient->firewallCallbackMutex_);
    for (auto callback : netsysNativeClient->firewallCallbacks_) {
        callback->OnIntercept(record);
    }
    return NETMANAGER_SUCCESS;
}
#endif

NetsysNativeClient::NativeNetDnsResultCallback::NativeNetDnsResultCallback(
    std::weak_ptr<NetsysNativeClient> netsysNativeClient) : netsysNativeClient_(netsysNativeClient)
{
}

int32_t NetsysNativeClient::NativeNetDnsResultCallback::OnDnsResultReport(uint32_t size,
    std::list<OHOS::NetsysNative::NetDnsResultReport> res)
{
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient has destory");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbDnsReportObjMutex_);
    for (auto cb = netsysNativeClient->cbDnsReportObjects_.begin();
         cb != netsysNativeClient->cbDnsReportObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbDnsReportObjects_.erase(cb);
        } else {
            (*cb)->OnDnsResultReport(size, res);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNetDnsResultCallback::OnDnsQueryResultReport(uint32_t size,
    std::list<OHOS::NetsysNative::NetDnsQueryResultReport> res)
{
    NETMGR_LOG_D("NetsysNativeClient OnDnsQueryResultReport");
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbDnsQueryReportObjMutex_);
    for (auto cb = netsysNativeClient->cbDnsQueryReportObjects_.begin();
         cb != netsysNativeClient->cbDnsQueryReportObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbDnsQueryReportObjects_.erase(cb);
        } else {
            (*cb)->OnDnsQueryResultReport(size, res);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNetDnsResultCallback::OnDnsQueryAbnormalReport(
    uint32_t eventfailcause, OHOS::NetsysNative::NetDnsQueryResultReport res)
{
    NETMGR_LOG_I("NetsysNativeClient OnDnsQueryAbnormalReport");
    auto netsysNativeClient = netsysNativeClient_.lock();
    if (netsysNativeClient == nullptr) {
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(netsysNativeClient->cbDnsQueryReportObjMutex_);
    for (auto cb = netsysNativeClient->cbDnsQueryReportObjects_.begin();
         cb != netsysNativeClient->cbDnsQueryReportObjects_.end();) {
        if (*cb == nullptr) {
            cb = netsysNativeClient->cbDnsQueryReportObjects_.erase(cb);
        } else {
            (*cb)->OnDnsQueryAbnormalReport(eventfailcause, res);
            ++cb;
        }
    }
    return NETMANAGER_SUCCESS;
}

NetsysNativeClient::NetsysNativeClient() = default;

void NetsysNativeClient::Init()
{
    RegisterNotifyCallback();
}

NetsysNativeClient::~NetsysNativeClient()
{
    NETMGR_LOG_I("~NetsysNativeClient : Destroy NetsysNativeService");
    if (netsysNativeService_ == nullptr || deathRecipient_ == nullptr) {
        return;
    }

    sptr<IRemoteObject> local = netsysNativeService_->AsObject();
    if (local == nullptr) {
        return;
    }
    local->RemoveDeathRecipient(deathRecipient_);

    UnRegisterNotifyCallback();
}

int32_t NetsysNativeClient::SetInternetPermission(uint32_t uid, uint8_t allow)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    auto callingUid = IPCSkeleton::GetCallingUid();
    return proxy->SetInternetPermission(uid, allow, callingUid != UID_FOUNDATION);
}

int32_t NetsysNativeClient::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    NETMGR_LOG_I("Create Physical network: netId[%{public}d], permission[%{public}d]", netId, permission);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkCreatePhysical(netId, permission);
}

int32_t NetsysNativeClient::NetworkCreateVirtual(int32_t netId, bool hasDns)
{
    NETMGR_LOG_I("Create Virtual network: netId[%{public}d], hasDns[%{public}d]", netId, hasDns);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkCreateVirtual(netId, hasDns);
}

int32_t NetsysNativeClient::NetworkDestroy(int32_t netId, bool isVpnNet)
{
    NETMGR_LOG_I("Destroy network: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkDestroy(netId, isVpnNet);
}

int32_t NetsysNativeClient::CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix,
                                       const std::set<int32_t> &uids)
{
    NETMGR_LOG_I("Create vnic");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->CreateVnic(mtu, tunAddr, prefix, uids);
}

int32_t NetsysNativeClient::DestroyVnic()
{
    NETMGR_LOG_I("Destroy vnic");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DestroyVnic();
}

int32_t NetsysNativeClient::EnableDistributedClientNet(const std::string &virnicAddr,
    const std::string &virnicName, const std::string &iif)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->EnableDistributedClientNet(virnicAddr, virnicName, iif);
}

int32_t NetsysNativeClient::EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                                       const std::string &dstAddr, const std::string &gw)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
}

int32_t NetsysNativeClient::DisableDistributedNet(
    bool isServer, const std::string &virnicName, const std::string &dstAddr)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DisableDistributedNet(isServer, virnicName, dstAddr);
}

int32_t NetsysNativeClient::NetworkAddUids(int32_t netId, const std::vector<UidRange> &uidRanges)
{
    NETMGR_LOG_I("Add uids to vpn network: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkAddUids(netId, uidRanges);
}

int32_t NetsysNativeClient::NetworkDelUids(int32_t netId, const std::vector<UidRange> &uidRanges)
{
    NETMGR_LOG_I("Remove uids from vpn network: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkDelUids(netId, uidRanges);
}

int32_t NetsysNativeClient::NetworkAddInterface(int32_t netId, const std::string &iface, NetBearType netBearerType)
{
    NETMGR_LOG_I("Add network interface: netId[%{public}d], iface[%{public}s, bearerType[%{public}u]]", netId,
                 iface.c_str(), netBearerType);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkAddInterface(netId, iface, netBearerType);
}

int32_t NetsysNativeClient::NetworkRemoveInterface(int32_t netId, const std::string &iface)
{
    NETMGR_LOG_I("Remove network interface: netId[%{public}d], iface[%{public}s]", netId, iface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkRemoveInterface(netId, iface);
}

int32_t NetsysNativeClient::NetworkAddRoute(int32_t netId, const std::string &ifName, const std::string &destination,
                                            const std::string &nextHop, bool isExcludedRoute)
{
    NETMGR_LOG_I("Add Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s], \
        isExcludedRoute[%{public}d]", netId, ifName.c_str(), ToAnonymousIp(destination).c_str(),
        ToAnonymousIp(nextHop).c_str(), isExcludedRoute);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkAddRoute(netId, ifName, destination, nextHop, isExcludedRoute);
}

int32_t NetsysNativeClient::NetworkAddRoutes(int32_t netId, const std::vector<nmd::NetworkRouteInfo> &infos)
{
    NETMGR_LOG_I("Add Routes: netId[%{public}d], infos[%{public}zu]", netId, infos.size());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkAddRoutes(netId, infos);
}

int32_t NetsysNativeClient::NetworkRemoveRoute(int32_t netId, const std::string &ifName, const std::string &destination,
    const std::string &nextHop, bool isExcludedRoute)
{
    NETMGR_LOG_D("Remove Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s],"
        "isExcludedRoute[%{public}d]", netId, ifName.c_str(), ToAnonymousIp(destination).c_str(),
        ToAnonymousIp(nextHop).c_str(), isExcludedRoute);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkRemoveRoute(netId, ifName, destination, nextHop, isExcludedRoute);
}

int32_t NetsysNativeClient::GetInterfaceConfig(OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    NETMGR_LOG_D("Get interface config: ifName[%{public}s]", cfg.ifName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetInterfaceConfig(cfg);
}

int32_t NetsysNativeClient::SetInterfaceConfig(const OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    NETMGR_LOG_D("Set interface config: ifName[%{public}s]", cfg.ifName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetInterfaceConfig(cfg);
}

int32_t NetsysNativeClient::SetInterfaceDown(const std::string &iface)
{
    NETMGR_LOG_D("Set interface down: iface[%{public}s]", iface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    OHOS::nmd::InterfaceConfigurationParcel ifCfg;
    ifCfg.ifName = iface;
    proxy->GetInterfaceConfig(ifCfg);
    auto fit = std::find(ifCfg.flags.begin(), ifCfg.flags.end(), IF_CFG_UP);
    if (fit != ifCfg.flags.end()) {
        ifCfg.flags.erase(fit);
    }
    ifCfg.flags.emplace_back(IF_CFG_DOWN);
    return proxy->SetInterfaceConfig(ifCfg);
}

int32_t NetsysNativeClient::SetInterfaceUp(const std::string &iface)
{
    NETMGR_LOG_D("Set interface up: iface[%{public}s]", iface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    OHOS::nmd::InterfaceConfigurationParcel ifCfg;
    ifCfg.ifName = iface;
    proxy->GetInterfaceConfig(ifCfg);
    auto fit = std::find(ifCfg.flags.begin(), ifCfg.flags.end(), IF_CFG_DOWN);
    if (fit != ifCfg.flags.end()) {
        ifCfg.flags.erase(fit);
    }
    ifCfg.flags.emplace_back(IF_CFG_UP);
    return proxy->SetInterfaceConfig(ifCfg);
}

void NetsysNativeClient::ClearInterfaceAddrs(const std::string &ifName)
{
    NETMGR_LOG_D("Clear addrs: ifName[%{public}s]", ifName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return;
    }
}

int32_t NetsysNativeClient::GetInterfaceMtu(const std::string &ifName)
{
    NETMGR_LOG_D("Get mtu: ifName[%{public}s]", ifName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetInterfaceMtu(ifName);
}

int32_t NetsysNativeClient::SetInterfaceMtu(const std::string &ifName, int32_t mtu)
{
    NETMGR_LOG_D("Set mtu: ifName[%{public}s], mtu[%{public}d]", ifName.c_str(), mtu);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetInterfaceMtu(ifName, mtu);
}

int32_t NetsysNativeClient::SetTcpBufferSizes(const std::string &tcpBufferSizes)
{
    NETMGR_LOG_D("Set tcp buffer sizes: tcpBufferSizes[%{public}s]", tcpBufferSizes.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetTcpBufferSizes(tcpBufferSizes);
}

int32_t NetsysNativeClient::AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                                int32_t prefixLength)
{
    NETMGR_LOG_D("Add address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]",
        ifName.c_str(), ToAnonymousIp(ipAddr).c_str(), prefixLength);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->AddInterfaceAddress(ifName, ipAddr, prefixLength);
}

int32_t NetsysNativeClient::DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                                int32_t prefixLength)
{
    NETMGR_LOG_D("Delete address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]",
        ifName.c_str(), ToAnonymousIp(ipAddr).c_str(), prefixLength);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DelInterfaceAddress(ifName, ipAddr, prefixLength);
}

int32_t NetsysNativeClient::DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                                int32_t prefixLength, int socketType)
{
    NETMGR_LOG_D("Delete address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]",
        ifName.c_str(), ToAnonymousIp(ipAddr).c_str(), prefixLength);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DelInterfaceAddress(ifName, ipAddr, prefixLength, socketType);
}

int32_t NetsysNativeClient::InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    NETMGR_LOG_D("Set Ip Address: ifaceName[%{public}s], ipAddr[%{public}s]",
        ifaceName.c_str(), ToAnonymousIp(ipAddress).c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->InterfaceSetIpAddress(ifaceName, ipAddress);
}

int32_t NetsysNativeClient::InterfaceSetIffUp(const std::string &ifaceName)
{
    NETMGR_LOG_D("Set Iff Up: ifaceName[%{public}s]", ifaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->InterfaceSetIffUp(ifaceName);
}

int32_t NetsysNativeClient::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                              const std::vector<std::string> &servers,
                                              const std::vector<std::string> &domains)
{
    NETMGR_LOG_D("Set resolver config: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
}

int32_t NetsysNativeClient::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                              std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                              uint8_t &retryCount)
{
    NETMGR_LOG_D("Get resolver config: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
}

int32_t NetsysNativeClient::CreateNetworkCache(uint16_t netId, bool isVpnNet)
{
    NETMGR_LOG_D("create dns cache: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->CreateNetworkCache(netId, isVpnNet);
}

int32_t NetsysNativeClient::DestroyNetworkCache(uint16_t netId, bool isVpnNet)
{
    NETMGR_LOG_D("Destroy dns cache: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DestroyNetworkCache(netId, isVpnNet);
}

int32_t NetsysNativeClient::GetAddrInfo(const std::string &hostName, const std::string &serverName,
                                        const AddrInfo &hints, uint16_t netId, std::vector<AddrInfo> &res)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("GetAddrInfo netsysNativeService_ is null");
        return NET_CONN_ERR_SERVICE_UPDATE_NET_LINK_INFO_FAIL;
    }
    return proxy->GetAddrInfo(hostName, serverName, hints, netId, res);
}

int32_t NetsysNativeClient::GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
    nmd::NetworkSharingTraffic &traffic)
{
    NETMGR_LOG_D("NetsysNativeClient GetNetworkSharingTraffic");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetNetworkSharingTraffic(downIface, upIface, traffic);
}

int32_t NetsysNativeClient::GetNetworkCellularSharingTraffic(nmd::NetworkSharingTraffic &traffic,
    std::string &ifaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetNetworkCellularSharingTraffic");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetNetworkCellularSharingTraffic(traffic, ifaceName);
}

int32_t NetsysNativeClient::SetDpaCellularSharingTraffic(nmd::NetworkDpaTrafficReport &traffic)
{
    auto proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->SetDpaCellularSharingTraffic(traffic);
}

int64_t NetsysNativeClient::GetCellularRxBytes()
{
    NETMGR_LOG_D("NetsysNativeClient GetCellularRxBytes");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetCellularTxBytes()
{
    NETMGR_LOG_D("NetsysNativeClient GetCellularTxBytes");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetAllRxBytes()
{
    NETMGR_LOG_D("NetsysNativeClient GetAllRxBytes");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetAllTxBytes()
{
    NETMGR_LOG_D("NetsysNativeClient GetAllTxBytes");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetUidRxBytes(uint32_t uid)
{
    NETMGR_LOG_D("NetsysNativeClient GetUidRxBytes uid is [%{public}u]", uid);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetUidTxBytes(uint32_t uid)
{
    NETMGR_LOG_D("NetsysNativeClient GetUidTxBytes uid is [%{public}u]", uid);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetUidOnIfaceRxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetUidOnIfaceRxBytes uid is [%{public}u] iface name is [%{public}s]", uid,
                 interfaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetUidOnIfaceTxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetUidOnIfaceTxBytes uid is [%{public}u] iface name is [%{public}s]", uid,
                 interfaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetIfaceRxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetIfaceRxBytes iface name is [%{public}s]", interfaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetIfaceTxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetIfaceTxBytes iface name is [%{public}s]", interfaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

std::vector<std::string> NetsysNativeClient::InterfaceGetList()
{
    NETMGR_LOG_D("NetsysNativeClient InterfaceGetList");
    std::vector<std::string> ret;
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return ret;
    }
    proxy->InterfaceGetList(ret);
    return ret;
}

std::vector<std::string> NetsysNativeClient::UidGetList()
{
    NETMGR_LOG_D("NetsysNativeClient UidGetList");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return {};
    }
    return {};
}

int64_t NetsysNativeClient::GetIfaceRxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetIfaceRxPackets iface name is [%{public}s]", interfaceName.c_str());
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetIfaceTxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetIfaceTxPackets iface name is [%{public}s]", interfaceName.c_str());
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::SetDefaultNetWork(int32_t netId)
{
    NETMGR_LOG_D("NetsysNativeClient SetDefaultNetWork");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkSetDefault(netId);
}

int32_t NetsysNativeClient::ClearDefaultNetWorkNetId()
{
    NETMGR_LOG_D("NetsysNativeClient ClearDefaultNetWorkNetId");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::IpEnableForwarding(const std::string &requestor)
{
    NETMGR_LOG_D("NetsysNativeClient IpEnableForwarding: requestor[%{public}s]", requestor.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IpEnableForwarding(requestor);
}

int32_t NetsysNativeClient::IpDisableForwarding(const std::string &requestor)
{
    NETMGR_LOG_D("NetsysNativeClient IpDisableForwarding: requestor[%{public}s]", requestor.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IpDisableForwarding(requestor);
}

int32_t NetsysNativeClient::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETMGR_LOG_D("NetsysNativeClient EnableNat: intIface[%{public}s] intIface[%{public}s]", downstreamIface.c_str(),
                 upstreamIface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->EnableNat(downstreamIface, upstreamIface);
}

int32_t NetsysNativeClient::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETMGR_LOG_D("NetsysNativeClient DisableNat: intIface[%{public}s] intIface[%{public}s]", downstreamIface.c_str(),
                 upstreamIface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DisableNat(downstreamIface, upstreamIface);
}

int32_t NetsysNativeClient::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IpfwdAddInterfaceForward(fromIface, toIface);
}

int32_t NetsysNativeClient::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    NETMGR_LOG_D("NetsysNativeClient IpfwdRemoveInterfaceForward: fromIface[%{public}s], toIface[%{public}s]",
                 fromIface.c_str(), toIface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IpfwdRemoveInterfaceForward(fromIface, toIface);
}

int32_t NetsysNativeClient::ShareDnsSet(uint16_t netId)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->ShareDnsSet(netId);
}

int32_t NetsysNativeClient::StartDnsProxyListen()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StartDnsProxyListen();
}

int32_t NetsysNativeClient::StopDnsProxyListen()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StopDnsProxyListen();
}

int32_t NetsysNativeClient::RegisterNetsysNotifyCallback(const NetsysNotifyCallback &callback)
{
    (void)callback;
    NETMGR_LOG_D("NetsysNativeClient RegisterNetsysNotifyCallback");
    return NETMANAGER_SUCCESS;
}

__attribute__((no_sanitize("cfi"))) sptr<OHOS::NetsysNative::INetsysService> NetsysNativeClient::GetProxy()
{
    std::lock_guard lock(mutex_);
    if (netsysNativeService_) {
        return netsysNativeService_;
    }

    NETMGR_LOG_D("Execute GetSystemAbilityManager");
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient samgr null");
        return nullptr;
    }

    auto remote = samgr->GetSystemAbility(OHOS::COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    if (remote == nullptr) {
        NETMGR_LOG_D("Get remote service failed");
        return nullptr;
    }

    deathRecipient_ = sptr<NetNativeConnDeathRecipient>::MakeSptr(shared_from_this());
    if (deathRecipient_ == nullptr) {
        NETMGR_LOG_E("Recipient new failed!");
        return nullptr;
    }

    if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
        NETMGR_LOG_E("add death recipient failed");
        return nullptr;
    }

    netsysNativeService_ = iface_cast<NetsysNative::INetsysService>(remote);
    if (netsysNativeService_ == nullptr) {
        NETMGR_LOG_E("Get remote service proxy failed");
        return nullptr;
    }

    return netsysNativeService_;
}

void NetsysNativeClient::RegisterNotifyCallback()
{
    std::thread t([client = shared_from_this()]() {
        uint32_t count = 0;
        while (client->GetProxy() == nullptr && count < MAX_GET_SERVICE_COUNT) {
            std::this_thread::sleep_for(std::chrono::seconds(WAIT_FOR_SERVICE_TIME_S));
            count++;
        }
        auto proxy = client->GetProxy();
        NETMGR_LOG_W("Get proxy %{public}s, count: %{public}u", proxy == nullptr ? "failed" : "success", count);
        if (proxy != nullptr) {
            if (client->nativeNotifyCallback_ == nullptr) {
                client->nativeNotifyCallback_ = sptr<NativeNotifyCallback>::MakeSptr(client);
            }

            NETMGR_LOG_D("call proxy->RegisterNotifyCallback");
            proxy->RegisterNotifyCallback(client->nativeNotifyCallback_);

            if (client->nativeNetDnsResultCallback_ == nullptr) {
                client->nativeNetDnsResultCallback_ = sptr<NativeNetDnsResultCallback>::MakeSptr(client);
            }

            NETMGR_LOG_D("call proxy->RegisterDnsResultCallback");
            proxy->RegisterDnsResultCallback(client->nativeNetDnsResultCallback_, client->dnsReportTimeStep);
        }
    });
    std::string threadName = "netsysGetProxy";
    pthread_setname_np(t.native_handle(), threadName.c_str());
    t.detach();
}

void NetsysNativeClient::UnRegisterNotifyCallback()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return;
    }
    if (nativeNotifyCallback_ != nullptr) {
        proxy->UnRegisterNotifyCallback(nativeNotifyCallback_);
        nativeNotifyCallback_ = nullptr;
    }

    if (nativeNetDnsResultCallback_ != nullptr) {
        proxy->UnregisterDnsResultCallback(nativeNetDnsResultCallback_);
        nativeNetDnsResultCallback_ = nullptr;
    }
}

void NetsysNativeClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    NETMGR_LOG_D("on remote died");
    if (remote == nullptr) {
        NETMGR_LOG_E("remote object is nullptr");
        return;
    }

    std::lock_guard lock(mutex_);
    if (netsysNativeService_ == nullptr) {
        NETMGR_LOG_E("netsysNativeService_ is nullptr");
        return;
    }

    sptr<IRemoteObject> local = netsysNativeService_->AsObject();
    if (local != remote.promote()) {
        NETMGR_LOG_E("proxy and stub is not same remote object");
        return;
    }
    local->RemoveDeathRecipient(deathRecipient_);

    if (access(NETSYS_ROUTE_INIT_DIR_PATH, F_OK) == 0) {
        NETMGR_LOG_D("NetConnService netsys restart, clear NETSYS_ROUTE_INIT_DIR_PATH");
        rmdir(NETSYS_ROUTE_INIT_DIR_PATH);
    }

    netsysNativeService_ = nullptr;

    RegisterNotifyCallback();
}

int32_t NetsysNativeClient::BindNetworkServiceVpn(int32_t socketFd)
{
    NETMGR_LOG_D("NetsysNativeClient::BindNetworkServiceVpn: socketFd[%{public}d]", socketFd);
    /* netsys provide default interface name */
    const char *defaultNetName = "wlan0";
    socklen_t defaultNetNameLen = strlen(defaultNetName);
    /* set socket by option. */
    int32_t ret = setsockopt(socketFd, SOL_SOCKET, SO_MARK, defaultNetName, defaultNetNameLen);
    if (ret < 0) {
        NETMGR_LOG_E("The SO_BINDTODEVICE of setsockopt failed.");
        return NETSYS_ERR_VPN;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::EnableVirtualNetIfaceCard(int32_t socketFd, struct ifreq &ifRequest, int32_t &ifaceFd)
{
    NETMGR_LOG_D("NetsysNativeClient::EnableVirtualNetIfaceCard: socketFd[%{public}d]", socketFd);
    int32_t ifaceFdTemp = 0;
    if ((ifaceFdTemp = open(DEV_NET_TUN_PATH, O_RDWR)) < 0) {
        NETMGR_LOG_E("VPN tunnel device open was failed.");
        return NETSYS_ERR_VPN;
    }

    /*
     * Flags:
     * IFF_TUN   - TUN device (no Ethernet headers)
     * IFF_TAP   - TAP device
     * IFF_NO_PI - Do not provide packet information
     **/
    ifRequest.ifr_flags = IFF_TUN | IFF_NO_PI;
    /**
     * Try to create the device. if it cannot assign the device interface name, kernel can
     * allocate the next device interface name. for example, there is tun0, kernel can
     * allocate tun1.
     **/
    if (ioctl(ifaceFdTemp, TUNSETIFF, &ifRequest) < 0) {
        NETMGR_LOG_E("The TUNSETIFF of ioctl failed, ifRequest.ifr_name[%{public}s]", ifRequest.ifr_name);
        close(ifaceFdTemp);
        return NETSYS_ERR_VPN;
    }

    /* Activate the device */
    ifRequest.ifr_flags = IFF_UP;
    if (ioctl(socketFd, SIOCSIFFLAGS, &ifRequest) < 0) {
        NETMGR_LOG_E("The SIOCSIFFLAGS of ioctl failed.");
        close(ifaceFdTemp);
        return NETSYS_ERR_VPN;
    }

    ifaceFd = ifaceFdTemp;
    return NETMANAGER_SUCCESS;
}

static inline in_addr_t *AsInAddr(sockaddr *sa)
{
    return &(reinterpret_cast<sockaddr_in *>(sa))->sin_addr.s_addr;
}

int32_t NetsysNativeClient::SetIpAddress(int32_t socketFd, const std::string &ipAddress, int32_t prefixLen,
                                         struct ifreq &ifRequest)
{
    NETMGR_LOG_D("NetsysNativeClient::SetIpAddress: socketFd[%{public}d]", socketFd);

    ifRequest.ifr_addr.sa_family = AF_INET;
    ifRequest.ifr_netmask.sa_family = AF_INET;

    /* inet_pton is IP ipAddress translation to binary network byte order. */
    if (inet_pton(AF_INET, ipAddress.c_str(), AsInAddr(&ifRequest.ifr_addr)) != 1) {
        NETMGR_LOG_E("inet_pton failed.");
        return NETSYS_ERR_VPN;
    }
    if (ioctl(socketFd, SIOCSIFADDR, &ifRequest) < 0) {
        NETMGR_LOG_E("The SIOCSIFADDR of ioctl failed.");
        return NETSYS_ERR_VPN;
    }
    in_addr_t addressPrefixLength = prefixLen ? (~0 << (IPV4_MAX_LENGTH - prefixLen)) : 0;
    *AsInAddr(&ifRequest.ifr_netmask) = htonl(addressPrefixLength);
    if (ioctl(socketFd, SIOCSIFNETMASK, &ifRequest)) {
        NETMGR_LOG_E("The SIOCSIFNETMASK of ioctl failed.");
        return NETSYS_ERR_VPN;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::SetBlocking(int32_t ifaceFd, bool isBlock)
{
    NETMGR_LOG_D("NetsysNativeClient::SetBlocking");
    int32_t blockingFlag = 0;
    blockingFlag = fcntl(ifaceFd, F_GETFL);
    if (blockingFlag < 0) {
        NETMGR_LOG_E("The blockingFlag of fcntl failed.");
        return NETSYS_ERR_VPN;
    }

    if (!isBlock) {
        blockingFlag = static_cast<int>(static_cast<uint32_t>(blockingFlag) | static_cast<uint32_t>(O_NONBLOCK));
    } else {
        blockingFlag = static_cast<int>(static_cast<uint32_t>(blockingFlag) | static_cast<uint32_t>(~O_NONBLOCK));
    }

    if (fcntl(ifaceFd, F_SETFL, blockingFlag) < 0) {
        NETMGR_LOG_E("The F_SETFL of fcntl failed.");
        return NETSYS_ERR_VPN;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_D("NetsysNativeClient::StartDhcpClient");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StartDhcpClient(iface, bIpv6);
}

int32_t NetsysNativeClient::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_D("NetsysNativeClient::StopDhcpClient");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StopDhcpClient(iface, bIpv6);
}

int32_t NetsysNativeClient::RegisterCallback(const sptr<NetsysControllerCallback> &callback)
{
    NETMGR_LOG_D("NetsysNativeClient::RegisterCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(cbObjMutex_);
    cbObjects_.push_back(callback);
    return NETMANAGER_SUCCESS;
}

void NetsysNativeClient::ProcessDhcpResult(sptr<OHOS::NetsysNative::DhcpResultParcel> &dhcpResult)
{
    NETMGR_LOG_I("NetsysNativeClient::ProcessDhcpResult");
    std::lock_guard lock(cbObjMutex_);
    NetsysControllerCallback::DhcpResult result;
    for (auto cb = cbObjects_.begin(); cb != cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = cbObjects_.erase(cb);
        } else {
            result.iface_ = dhcpResult->iface_;
            result.ipAddr_ = dhcpResult->ipAddr_;
            result.gateWay_ = dhcpResult->gateWay_;
            result.subNet_ = dhcpResult->subNet_;
            result.route1_ = dhcpResult->route1_;
            result.route2_ = dhcpResult->route2_;
            result.dns1_ = dhcpResult->dns1_;
            result.dns2_ = dhcpResult->dns2_;
            (*cb)->OnDhcpSuccess(result);
            ++cb;
        }
    }
}

int32_t NetsysNativeClient::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    NETMGR_LOG_D("NetsysNativeClient StartDhcpService");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StartDhcpService(iface, ipv4addr);
}

int32_t NetsysNativeClient::StopDhcpService(const std::string &iface)
{
    NETMGR_LOG_D("NetsysNativeClient StopDhcpService");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StopDhcpService(iface);
}

void NetsysNativeClient::ProcessBandwidthReachedLimit(const std::string &limitName, const std::string &iface)
{
    NETMGR_LOG_D("NetsysNativeClient ProcessBandwidthReachedLimit, limitName=%{public}s, iface=%{public}s",
                 limitName.c_str(), iface.c_str());
    std::lock_guard lock(cbObjMutex_);
    for (auto cb = cbObjects_.begin(); cb != cbObjects_.end();) {
        if (*cb == nullptr) {
            cb = cbObjects_.erase(cb);
        } else {
            (*cb)->OnBandwidthReachedLimit(limitName, iface);
            ++cb;
        }
    }
}

int32_t NetsysNativeClient::BandwidthEnableDataSaver(bool enable)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthEnableDataSaver(enable);
}

int32_t NetsysNativeClient::BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthSetIfaceQuota(ifName, bytes);
}

int32_t NetsysNativeClient::BandwidthRemoveIfaceQuota(const std::string &ifName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthRemoveIfaceQuota(ifName);
}

int32_t NetsysNativeClient::BandwidthAddDeniedList(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthAddDeniedList(uid);
}

int32_t NetsysNativeClient::BandwidthRemoveDeniedList(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthRemoveDeniedList(uid);
}

int32_t NetsysNativeClient::BandwidthAddAllowedList(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthAddAllowedList(uid);
}

int32_t NetsysNativeClient::BandwidthRemoveAllowedList(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthRemoveAllowedList(uid);
}

int32_t NetsysNativeClient::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FirewallSetUidsAllowedListChain(chain, uids);
}

int32_t NetsysNativeClient::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FirewallSetUidsDeniedListChain(chain, uids);
}

int32_t NetsysNativeClient::FirewallEnableChain(uint32_t chain, bool enable)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FirewallEnableChain(chain, enable);
}

int32_t NetsysNativeClient::FirewallSetUidRule(uint32_t chain, const std::vector<uint32_t> &uids, uint32_t firewallRule)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FirewallSetUidRule(chain, uids, firewallRule);
}

int32_t NetsysNativeClient::GetTotalStats(uint64_t &stats, uint32_t type)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetTotalStats(stats, type);
}

int32_t NetsysNativeClient::GetUidStats(uint64_t &stats, uint32_t type, uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetUidStats(stats, type, uid);
}

int32_t NetsysNativeClient::GetIfaceStats(uint64_t &stats, uint32_t type, const std::string &interfaceName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetIfaceStats(stats, type, interfaceName);
}

int32_t NetsysNativeClient::GetAllSimStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetAllSimStatsInfo(stats);
}

int32_t NetsysNativeClient::DeleteSimStatsInfo(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DeleteSimStatsInfo(uid);
}

int32_t NetsysNativeClient::GetAllStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetAllStatsInfo(stats);
}

int32_t NetsysNativeClient::DeleteStatsInfo(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DeleteStatsInfo(uid);
}

int32_t NetsysNativeClient::SetNetStateTrafficMap(uint8_t flag, uint64_t availableTraffic)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetNetStateTrafficMap(flag, availableTraffic);
}

int32_t NetsysNativeClient::GetNetStateTrafficMap(uint8_t flag, uint64_t &availableTraffic)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetNetStateTrafficMap(flag, availableTraffic);
}

int32_t NetsysNativeClient::ClearIncreaseTrafficMap()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->ClearIncreaseTrafficMap();
}

int32_t NetsysNativeClient::ClearSimStatsBpfMap()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->ClearSimStatsBpfMap();
}

int32_t NetsysNativeClient::DeleteIncreaseTrafficMap(uint64_t ifIndex)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DeleteIncreaseTrafficMap(ifIndex);
}

int32_t NetsysNativeClient::UpdateIfIndexMap(int8_t key, uint64_t index)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UpdateIfIndexMap(key, index);
}

int32_t NetsysNativeClient::SetNetStatusMap(uint8_t type, uint8_t value)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetNetStatusMap(type, value);
}

int32_t NetsysNativeClient::SetIptablesCommandForRes(const std::string &cmd, std::string &respond,
    NetsysNative::IptablesType ipType)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetIptablesCommandForRes(cmd, respond, ipType);
}

int32_t NetsysNativeClient::SetIpCommandForRes(const std::string &cmd, std::string &respond)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetIpCommandForRes(cmd, respond);
}

int32_t NetsysNativeClient::NetDiagPingHost(const OHOS::NetsysNative::NetDiagPingOption &pingOption,
                                            const sptr<OHOS::NetsysNative::INetDiagCallback> &callback)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetDiagPingHost(pingOption, callback);
}

int32_t NetsysNativeClient::NetDiagGetRouteTable(std::list<OHOS::NetsysNative::NetDiagRouteTable> &routeTables)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetDiagGetRouteTable(routeTables);
}

int32_t NetsysNativeClient::NetDiagGetSocketsInfo(OHOS::NetsysNative::NetDiagProtocolType socketType,
                                                  OHOS::NetsysNative::NetDiagSocketsInfo &socketsInfo)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetDiagGetSocketsInfo(socketType, socketsInfo);
}

int32_t NetsysNativeClient::NetDiagGetInterfaceConfig(std::list<OHOS::NetsysNative::NetDiagIfaceConfig> &configs,
                                                      const std::string &ifaceName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetDiagGetInterfaceConfig(configs, ifaceName);
}

int32_t NetsysNativeClient::NetDiagUpdateInterfaceConfig(const OHOS::NetsysNative::NetDiagIfaceConfig &config,
                                                         const std::string &ifaceName, bool add)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetDiagUpdateInterfaceConfig(config, ifaceName, add);
}

int32_t NetsysNativeClient::NetDiagSetInterfaceActiveState(const std::string &ifaceName, bool up)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetDiagSetInterfaceActiveState(ifaceName, up);
}

int32_t NetsysNativeClient::AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                         const std::string &ifName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->AddStaticArp(ipAddr, macAddr, ifName);
}

int32_t NetsysNativeClient::DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                         const std::string &ifName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DelStaticArp(ipAddr, macAddr, ifName);
}

int32_t NetsysNativeClient::AddStaticIpv6Addr(const std::string &ipAddr, const std::string &macAddr,
    const std::string &ifName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
}

int32_t NetsysNativeClient::DelStaticIpv6Addr(const std::string &ipAddr, const std::string &macAddr,
    const std::string &ifName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
}

int32_t NetsysNativeClient::GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo)
{
    NETMGR_LOG_D("get ip neigh table");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetIpNeighTable(ipMacInfo);
}

int32_t NetsysNativeClient::GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo,
                                               int32_t &ownerUid)
{
    NETMGR_LOG_D("get connect owner uid");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetConnectOwnerUid(netConnInfo, ownerUid);
}

int32_t NetsysNativeClient::GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo)
{
    NETMGR_LOG_D("get system net port states");
    auto proxy = GetProxy();
    // LCOV_EXCL_START This will never happen.
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->GetSystemNetPortStates(netPortStatesInfo);
}

int32_t NetsysNativeClient::RegisterDnsResultCallback(
    const sptr<OHOS::NetManagerStandard::NetsysDnsReportCallback> &callback, uint32_t timeStep)
{
    NETMGR_LOG_I("NetsysNativeClient::RegisterCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(cbDnsReportObjMutex_);
    cbDnsReportObjects_.push_back(callback);
    dnsReportTimeStep = timeStep;
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::UnregisterDnsResultCallback(
    const sptr<OHOS::NetManagerStandard::NetsysDnsReportCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(cbDnsReportObjMutex_);
    cbDnsReportObjects_.remove(callback);
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::RegisterDnsQueryResultCallback(
    const sptr<OHOS::NetManagerStandard::NetsysDnsQueryReportCallback> &callback)
{
    NETMGR_LOG_I("NetsysNativeClient::RegisterDnsQueryResultCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(cbDnsQueryReportObjMutex_);
    cbDnsQueryReportObjects_.push_back(callback);
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::UnregisterDnsQueryResultCallback(
    const sptr<OHOS::NetManagerStandard::NetsysDnsQueryReportCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::lock_guard lock(cbDnsQueryReportObjMutex_);
    cbDnsQueryReportObjects_.remove(callback);
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::GetCookieStats(uint64_t &stats, uint32_t type, uint64_t cookie)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetCookieStats(stats, type, cookie);
}

int32_t NetsysNativeClient::GetNetworkSharingType(std::set<uint32_t>& sharingTypeIsOn)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetNetworkSharingType(sharingTypeIsOn);
}

int32_t NetsysNativeClient::UpdateNetworkSharingType(uint32_t type, bool isOpen)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UpdateNetworkSharingType(type, isOpen);
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
int32_t NetsysNativeClient::SetFirewallRules(NetFirewallRuleType type,
                                             const std::vector<sptr<NetFirewallBaseRule>> &ruleList, bool isFinish)
{
    NETMGR_LOG_E("NetsysNativeClient::SetFirewallRules");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetFirewallRules(type, ruleList, isFinish);
}

int32_t NetsysNativeClient::SetFirewallDefaultAction(int32_t userId, FirewallRuleAction inDefault,
    FirewallRuleAction outDefault)
{
    NETMGR_LOG_D("NetsysNativeClient::SetFirewallDefaultAction");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetFirewallDefaultAction(userId, inDefault, outDefault);
}

int32_t NetsysNativeClient::SetFirewallCurrentUserId(int32_t userId)
{
    NETMGR_LOG_D("NetsysNativeClient::SetFirewallCurrentUserId");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetFirewallCurrentUserId(userId);
}

int32_t NetsysNativeClient::ClearFirewallRules(NetFirewallRuleType type)
{
    NETMGR_LOG_D("NetsysNativeClient::ClearFirewallRules");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->ClearFirewallRules(type);
}

void NetsysNativeClient::RegisterFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    if (callback == nullptr) {
        return;
    }
    std::lock_guard<std::shared_mutex> lock(firewallCallbackMutex_);
    for (auto it = firewallCallbacks_.begin(); it != firewallCallbacks_.end(); ++it) {
        if ((*it)->AsObject().GetRefPtr() == callback->AsObject().GetRefPtr()) {
            return;
        }
    }
    firewallCallbacks_.push_back(callback);
}

void NetsysNativeClient::UnregisterFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    if (callback == nullptr) {
        return;
    }
    std::lock_guard<std::shared_mutex> lock(firewallCallbackMutex_);
    for (auto it = firewallCallbacks_.begin(); it != firewallCallbacks_.end(); ++it) {
        if ((*it)->AsObject().GetRefPtr() == callback->AsObject().GetRefPtr()) {
            firewallCallbacks_.erase(it);
            break;
        }
    }
}

int32_t NetsysNativeClient::RegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    NETMGR_LOG_D("NetsysNativeClient::RegisterNetFirewallCallback");
    RegisterFirewallCallback(callback);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->RegisterNetFirewallCallback(callback);
}

int32_t NetsysNativeClient::UnRegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    NETMGR_LOG_D("NetsysNativeClient::UnRegisterNetFirewallCallback");
    UnregisterFirewallCallback(callback);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UnRegisterNetFirewallCallback(callback);
}
#endif

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
int32_t NetsysNativeClient::EnableWearableDistributedNetForward(const int32_t tcpPortId, const int32_t udpPortId)
{
    NETMGR_LOG_I("Enabling wearable distributed net forward for TCP port and UDP port");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->EnableWearableDistributedNetForward(tcpPortId, udpPortId);
}

int32_t NetsysNativeClient::DisableWearableDistributedNetForward()
{
    NETMGR_LOG_I("Disabling wearable distributed net forward");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DisableWearableDistributedNetForward();
}
#endif

int32_t NetsysNativeClient::RegisterNetsysTrafficCallback(const sptr<NetsysNative::INetsysTrafficCallback> &callback)
{
    NETMGR_LOG_I("NetsysNativeClient::RegisterNetsysTrafficCallback");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->RegisterNetsysTrafficCallback(callback);
}

int32_t NetsysNativeClient::UnRegisterNetsysTrafficCallback(const sptr<NetsysNative::INetsysTrafficCallback> &callback)
{
    NETMGR_LOG_D("NetsysNativeClient::UnRegisterNetsysTrafficCallback");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UnRegisterNetsysTrafficCallback(callback);
}

int32_t NetsysNativeClient::SetIpv6PrivacyExtensions(const std::string &interfaceName, const uint32_t on)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetIpv6PrivacyExtensions(interfaceName, on);
}

int32_t NetsysNativeClient::SetEnableIpv6(const std::string &interfaceName, const uint32_t on, bool needRestart)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetEnableIpv6(interfaceName, on, needRestart);
}

int32_t NetsysNativeClient::SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("SetIpv6UidBlackList proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetIpv6UidBlackList(netIds, uid);
}

int32_t NetsysNativeClient::SetIpv6AutoConf(const std::string &interfaceName, const uint32_t on)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetIpv6AutoConf(interfaceName, on);
}

int32_t NetsysNativeClient::SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetNetworkAccessPolicy(uid, policy, reconfirmFlag);
}

int32_t NetsysNativeClient::DeleteNetworkAccessPolicy(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->DeleteNetworkAccessPolicy(uid);
}

int32_t NetsysNativeClient::ClearFirewallAllRules()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->ClearFirewallAllRules();
}

int32_t NetsysNativeClient::NotifyNetBearerTypeChange(std::set<NetBearType> bearerTypes)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->NotifyNetBearerTypeChange(bearerTypes);
}

int32_t NetsysNativeClient::StartClat(const std::string &interfaceName, int32_t netId,
                                      const std::string &nat64PrefixStr)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StartClat(interfaceName, netId, nat64PrefixStr);
}

int32_t NetsysNativeClient::StopClat(const std::string &interfaceName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StopClat(interfaceName);
}

int32_t NetsysNativeClient::SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetNicTrafficAllowed(ifaceNames, status);
}

#ifdef SUPPORT_SYSVPN
int32_t NetsysNativeClient::ProcessVpnStage(NetsysNative::SysVpnStageCode stage, const std::string &message)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->ProcessVpnStage(stage, message);
}

int32_t NetsysNativeClient::UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UpdateVpnRules(netId, extMessages, add);
}
#endif // SUPPORT_SYSVPN

int32_t NetsysNativeClient::CloseSocketsUid(const std::string &ipAddr, uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->CloseSocketsUid(ipAddr, uid);
}

int32_t NetsysNativeClient::SetBrokerUidAccessPolicyMap(const std::unordered_map<uint32_t, uint32_t> &uidMaps)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetBrokerUidAccessPolicyMap(uidMaps);
}

int32_t NetsysNativeClient::DelBrokerUidAccessPolicyMap(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DelBrokerUidAccessPolicyMap(uid);
}

int32_t NetsysNativeClient::SetUserDefinedServerFlag(uint16_t netId, bool isUserDefinedServer)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetUserDefinedServerFlag(netId, isUserDefinedServer);
}

int32_t NetsysNativeClient::FlushDnsCache(uint16_t netId)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FlushDnsCache(netId);
}

int32_t NetsysNativeClient::SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetDnsCache(netId, hostName, addrInfo);
}

int32_t NetsysNativeClient::CreateVlan(const std::string &ifName, uint32_t vlanId)
{
    auto proxy = GetProxy();
    // LCOV_EXCL_START This will never happen.
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->CreateVlan(ifName, vlanId);
}

int32_t NetsysNativeClient::DestroyVlan(const std::string &ifName, uint32_t vlanId)
{
    auto proxy = GetProxy();
    // LCOV_EXCL_START This will never happen.
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->DestroyVlan(ifName, vlanId);
}

int32_t NetsysNativeClient::AddVlanIp(const std::string &ifName, uint32_t vlanId,
                                      const std::string &ip, uint32_t mask)
{
    auto proxy = GetProxy();
    // LCOV_EXCL_START This will never happen.
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->AddVlanIp(ifName, vlanId, ip, mask);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
int32_t NetsysNativeClient::UpdateEnterpriseRoute(const std::string &interfaceName, uint32_t uid, bool add)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UpdateEnterpriseRoute(interfaceName, uid, add);
}
#endif

int32_t NetsysNativeClient::SetInternetAccessByIpForWifiShare(
    const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
}
} // namespace NetManagerStandard
} // namespace OHOS

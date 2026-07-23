/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "net_connection_callback.h"
#include "net_connection_ffi.h"
#include "net_connection_impl.h"
#include "netmanager_base_log.h"

namespace OHOS::NetManagerStandard {
int32_t ConnectionCallbackObserver::NetAvailable(sptr<NetHandle> &netHandle)
{
    if (netHandle == nullptr) {
        return 0;
    }
    std::shared_lock<std::shared_mutex> lock(g_netConnectionsMutex);
    auto netConnection = NET_CONNECTIONS_FFI.find(this);
    if (netConnection == NET_CONNECTIONS_FFI.end() || netConnection->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netConnection handle");
        return 0;
    }
    if (netConnection->second->netAvailible.size() == 0) {
        NETMANAGER_BASE_LOGE("no NetAvailable func registered");
        return 0;
    }
    int32_t id = netHandle->GetNetId();
    int len = static_cast<int>(netConnection->second->netAvailible.size());
    for (int i = 0; i < len; i++) {
        netConnection->second->netAvailible[i](id);
    }
    return 0;
}

bool SetCapability(CNetCapabilities &capabilities, const std::set<NetBearType> &bearerTypes,
                   const std::set<NetCap> &netCaps)
{
    if (capabilities.bearedTypeSize > 0) {
        capabilities.bearerTypes = static_cast<int32_t *>(malloc(sizeof(int32_t) * capabilities.bearedTypeSize));
        if (capabilities.bearerTypes == nullptr) {
            NETMANAGER_BASE_LOGE("NetCapabilitiesChange malloc bearerTypes failed");
            return false;
        }
        int j = 0;
        for (auto it = bearerTypes.begin(); it != bearerTypes.end(); ++it, ++j) {
            capabilities.bearerTypes[j] = *it;
        }
    }

    if (capabilities.networkCapSize > 0) {
        capabilities.networkCap = static_cast<int32_t *>(malloc(sizeof(int32_t) * capabilities.networkCapSize));
        if (capabilities.networkCap == nullptr) {
            NETMANAGER_BASE_LOGE("NetCapabilitiesChange malloc networkCap failed");
            free(capabilities.bearerTypes);
            capabilities.bearerTypes = nullptr;
            return false;
        }
        int j = 0;
        for (auto it = netCaps.begin(); it != netCaps.end(); ++it, ++j) {
            capabilities.networkCap[j] = *it;
        }
    }
    return true;
}

int32_t ConnectionCallbackObserver::NetCapabilitiesChange(sptr<NetHandle> &netHandle,
                                                          const sptr<NetAllCapabilities> &netAllCap)
{
    if (netHandle == nullptr || netAllCap == nullptr) {
        NETMANAGER_BASE_LOGE("NetCapabilitiesChange param is nullptr");
        return 0;
    }
    std::shared_lock<std::shared_mutex> lock(g_netConnectionsMutex);
    auto netConnection = NET_CONNECTIONS_FFI.find(this);
    if (netConnection == NET_CONNECTIONS_FFI.end() || netConnection->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netConnection handle");
        return 0;
    }
    if (netConnection->second->netCapabilitiesChange.size() == 0) {
        NETMANAGER_BASE_LOGE("no NetCapabilitiesChange func registered");
        return 0;
    }

    int32_t id = netHandle->GetNetId();

    int len = static_cast<int>(netConnection->second->netCapabilitiesChange.size());
    for (int i = 0; i < len; i++) {
        auto bearTypes = netAllCap->bearerTypes_;
        auto netCaps = netAllCap->netCaps_;

        CNetCapabilities capabilities = {.bearedTypeSize = bearTypes.size(),
                                         .networkCapSize = netCaps.size(),
                                         .linkUpBandwidthKbps = netAllCap->linkUpBandwidthKbps_,
                                         .linkDownBandwidthKbps = netAllCap->linkDownBandwidthKbps_,
                                         .bearerTypes = nullptr,
                                         .networkCap = nullptr};
        if (!SetCapability(capabilities, bearTypes, netCaps)) {
            return 0;
        }

        CNetCapabilityInfo info = {.netHandle = id, .netCap = capabilities};
        netConnection->second->netCapabilitiesChange[i](info);
    }
    return 0;
}

void SetConnectionProp(CConnectionProperties &props, const sptr<NetLinkInfo> &info)
{
    if (props.linkAddressSize > 0) {
        props.linkAddresses = static_cast<CLinkAddress *>(malloc(sizeof(CLinkAddress) * props.linkAddressSize));
        if (props.linkAddresses == nullptr) {
            props.linkAddressSize = 0;
            return;
        }
        int i = 0;
        for (auto it = info->netAddrList_.begin(); it != info->netAddrList_.end(); ++it, ++i) {
            CNetAddress netAddr{.address = MallocCString(it->address_), .family = it->family_, .port = it->port_};
            props.linkAddresses[i] = CLinkAddress{.address = netAddr, .prefixLength = it->prefixlen_};
        }
    }

    if (props.dnsSize > 0) {
        props.dnses = static_cast<CNetAddress *>(malloc(sizeof(CNetAddress) * props.dnsSize));
        if (props.dnses == nullptr) {
            return;
        }
        int i = 0;
        for (auto it = info->dnsList_.begin(); it != info->dnsList_.end(); ++it, ++i) {
            props.dnses[i] =
                CNetAddress{.address = MallocCString(it->address_), .family = it->family_, .port = it->port_};
        }
    }

    if (props.routeSize > 0) {
        props.routes = static_cast<CRouteInfo *>(malloc(sizeof(CRouteInfo) * props.routeSize));
        if (props.routes == nullptr) {
            return;
        }
        int i = 0;
        for (auto it = info->routeList_.begin(); it != info->routeList_.end(); ++it, ++i) {
            CNetAddress destAddr = {.address = MallocCString(it->destination_.address_),
                                    .family = it->destination_.family_,
                                    .port = it->destination_.port_};
            CLinkAddress dest = {.address = destAddr, .prefixLength = it->destination_.prefixlen_};
            CNetAddress gateway = {.address = MallocCString(it->gateway_.address_),
                                   .family = it->gateway_.family_,
                                   .port = it->gateway_.port_};
            props.routes[i] = CRouteInfo{.interfaceName = MallocCString(it->iface_),
                                         .destination = dest,
                                         .gateway = gateway,
                                         .hasGateway = it->hasGateway_,
                                         .isDefaultRoute = it->isDefaultRoute_};
        }
    }
}

int32_t ConnectionCallbackObserver::NetConnectionPropertiesChange(sptr<NetHandle> &netHandle,
                                                                  const sptr<NetLinkInfo> &info)
{
    if (netHandle == nullptr || info == nullptr) {
        NETMANAGER_BASE_LOGE("NetConnectionPropertiesChange param is nullptr");
        return 0;
    }
    std::shared_lock<std::shared_mutex> lock(g_netConnectionsMutex);
    auto netConnection = NET_CONNECTIONS_FFI.find(this);
    if (netConnection == NET_CONNECTIONS_FFI.end() || netConnection->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netConnection handle");
        return 0;
    }
    if (netConnection->second->netConnectionPropertiesChange.size() == 0) {
        return 0;
    }

    int32_t id = netHandle->GetNetId();
    int len = static_cast<int>(netConnection->second->netConnectionPropertiesChange.size());
    for (int i = 0; i < len; i++) {
        CConnectionProperties props = {.interfaceName = MallocCString(info->ifaceName_),
                                       .domains = MallocCString(info->domain_),
                                       .linkAddressSize = info->netAddrList_.size(),
                                       .dnsSize = info->dnsList_.size(),
                                       .routeSize = info->routeList_.size(),
                                       .mtu = info->mtu_,
                                       .linkAddresses = nullptr,
                                       .dnses = nullptr,
                                       .routes = nullptr};
        SetConnectionProp(props, info);
        netConnection->second->netConnectionPropertiesChange[i](id, props);
    }
    return 0;
}

int32_t ConnectionCallbackObserver::NetLost(sptr<NetHandle> &netHandle)
{
    if (netHandle == nullptr) {
        return 0;
    }
    std::shared_lock<std::shared_mutex> lock(g_netConnectionsMutex);
    auto netConnection = NET_CONNECTIONS_FFI.find(this);
    if (netConnection == NET_CONNECTIONS_FFI.end() || netConnection->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netConnection handle");
        return 0;
    }
    if (netConnection->second->netLost.size() == 0) {
        NETMANAGER_BASE_LOGE("no NetLost func registered");
        return 0;
    }
    int32_t id = netHandle->GetNetId();
    int32_t len = static_cast<int32_t>(netConnection->second->netLost.size());
    for (int32_t i = 0; i < len; i++) {
        netConnection->second->netLost[i](id);
    }
    return 0;
}

int32_t ConnectionCallbackObserver::NetUnavailable()
{
    std::shared_lock<std::shared_mutex> lock(g_netConnectionsMutex);
    auto netConnection = NET_CONNECTIONS_FFI.find(this);
    if (netConnection == NET_CONNECTIONS_FFI.end() || netConnection->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netConnection handle");
        return 0;
    }
    if (netConnection->second->netUnavailable.size() == 0) {
        NETMANAGER_BASE_LOGE("no NetUnavailable func registered");
        return 0;
    }
    int len = static_cast<int>(netConnection->second->netUnavailable.size());
    for (int i = 0; i < len; i++) {
        netConnection->second->netUnavailable[i]();
    }
    return 0;
}

int32_t ConnectionCallbackObserver::NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
{
    std::shared_lock<std::shared_mutex> lock(g_netConnectionsMutex);
    auto netConnection = NET_CONNECTIONS_FFI.find(this);
    if (netConnection == NET_CONNECTIONS_FFI.end() || netConnection->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netConnection handle");
        return 0;
    }
    if (netConnection->second->netBlockStatusChange.size() == 0) {
        NETMANAGER_BASE_LOGE("no NetBlockStatusChange func registered");
        return 0;
    }
    int32_t id = netHandle->GetNetId();
    int len = static_cast<int>(netConnection->second->netBlockStatusChange.size());
    for (int i = 0; i < len; i++) {
        netConnection->second->netBlockStatusChange[i](id, blocked);
    }
    return 0;
}
} // namespace OHOS::NetManagerStandard

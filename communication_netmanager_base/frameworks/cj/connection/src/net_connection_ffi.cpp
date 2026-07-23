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

#include "net_connection_ffi.h"
#include "connection_exec.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "net_all_capabilities.h"
#include "net_conn_client.h"
#include "net_connection_impl.h"
#include "net_handle.h"
#include "net_link_info.h"
#include "net_manager_constants.h"
#include "net_specifier.h"
#include <arpa/inet.h>
#include <netdb.h>

namespace OHOS::NetManagerStandard {

constexpr int32_t NO_PERMISSION_CODE = 1;
constexpr int32_t RESOURCE_UNAVALIEBLE_CODE = 11;
constexpr int32_t NET_UNREACHABLE_CODE = 101;
static constexpr size_t MAX_IPV4_STR_LEN_FFI = 16;
static constexpr size_t MAX_IPV6_STR_LEN_FFI = 64;

EXTERN_C_START
int32_t TransErrorCode(int32_t error)
{
    switch (error) {
        case NO_PERMISSION_CODE:
            return NETMANAGER_ERR_PERMISSION_DENIED;
        case RESOURCE_UNAVALIEBLE_CODE:
            return NETMANAGER_ERR_INVALID_PARAMETER;
        case NET_UNREACHABLE_CODE:
            return NETMANAGER_ERR_INTERNAL;
        default:
            return NETMANAGER_ERR_OPERATION_FAILED;
    }
}

int64_t CJ_CreateNetConnection(CNetSpecifier netSpecifier, uint32_t timeout)
{
    auto connection = FFI::FFIData::Create<NetConnectionProxy>(netSpecifier, timeout);
    if (!connection) {
        return ERR_INVALID_INSTANCE_CODE;
    }
    return connection->GetID();
}

void CJ_ReleaseNetConnection(int64_t connId)
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(connId);
    if (!instance) {
        return;
    }
    instance->Release();
    FFI::FFIData::Release(connId);
}

int32_t CJ_GetDefaultNet(int32_t &netId)
{
    NetHandle netHandle;
    auto ret = NetConnClient::GetInstance().GetDefaultNet(netHandle);
    if (ret == NETMANAGER_SUCCESS) {
        netId = netHandle.GetNetId();
    }
    return ret;
}

void ParseAddrInfo(addrinfo *res, std::vector<CNetAddress> &addresses_)
{
    for (addrinfo *tmp = res; tmp != nullptr; tmp = tmp->ai_next) {
        std::string host_;
        if (tmp->ai_family == AF_INET) {
            auto addr = reinterpret_cast<sockaddr_in *>(tmp->ai_addr);
            char ip[MAX_IPV4_STR_LEN_FFI] = {0};
            inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
            host_ = ip;
        } else if (tmp->ai_family == AF_INET6) {
            auto addr = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
            char ip[MAX_IPV6_STR_LEN_FFI] = {0};
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
            host_ = ip;
        }

        uint16_t port = 0;
        if (tmp->ai_addr->sa_family == AF_INET) {
            auto addr4 = reinterpret_cast<sockaddr_in *>(tmp->ai_addr);
            port = addr4->sin_port;
        } else if (tmp->ai_addr->sa_family == AF_INET6) {
            auto addr6 = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
            port = addr6->sin6_port;
        }

        CNetAddress address = {.address = MallocCString(host_), .family = tmp->ai_addr->sa_family, .port = port};
        addresses_.emplace_back(address);
    }
}

RetNetAddressArr CJ_GetAddressesByName(int32_t netId, const char *host)
{
    RetNetAddressArr ret = {.code = NETMANAGER_ERROR, .size = 0, .data = nullptr};
    addrinfo *res = nullptr;
    queryparam param;
    param.qp_type = QEURY_TYPE_NORMAL;
    param.qp_netid = netId;
    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if (host == nullptr) {
        ret.code = NETMANAGER_ERR_INVALID_PARAMETER;
        return ret;
    }
    int status = getaddrinfo_ext(host, nullptr, &hints, &res, &param);
    if (status < 0) {
        ret.code = TransErrorCode(errno);
        NETMANAGER_BASE_LOGE("getaddrinfo_ext errno %{public}d %{public}s", errno, strerror(errno));
        return ret;
    }
    ret.code = status;

    std::vector<CNetAddress> addresses_;
    ParseAddrInfo(res, addresses_);
    freeaddrinfo(res);

    ret.size = static_cast<int64_t>(addresses_.size());
    if (ret.size > 0) {
        ret.data = static_cast<CNetAddress *>(malloc(sizeof(CNetAddress) * ret.size));
        if (ret.data == nullptr) {
            ret.code = NETMANAGER_ERR_INTERNAL;
            return ret;
        }
        for (int64_t i = 0; i < ret.size; i++) {
            ret.data[i] = CNetAddress(addresses_[i]);
        }
    }
    return ret;
}

int32_t CJ_IsDefaultNetMetered(bool &ret)
{
    ret = false;
    return NetConnClient::GetInstance().IsDefaultNetMetered(ret);
}

int32_t CJ_HasDefaultNet(bool &ret)
{
    ret = false;
    return NetConnClient::GetInstance().HasDefaultNet(ret);
}

int32_t CJ_GetNetCapabilities(int32_t netId, CNetCapabilities &ret)
{
    NetHandle netHandle{netId};
    NetAllCapabilities capabilities;
    auto code = NetConnClient::GetInstance().GetNetCapabilities(netHandle, capabilities);
    if (code == NETMANAGER_SUCCESS) {
        ret.linkUpBandwidthKbps = capabilities.linkUpBandwidthKbps_;
        ret.linkDownBandwidthKbps = capabilities.linkDownBandwidthKbps_;
        ret.bearedTypeSize = static_cast<int64_t>(capabilities.bearerTypes_.size());
        ret.networkCapSize = static_cast<int64_t>(capabilities.netCaps_.size());
        if (ret.bearedTypeSize > 0) {
            ret.bearerTypes = static_cast<int32_t *>(malloc(sizeof(int32_t) * ret.bearedTypeSize));
            if (ret.bearerTypes == nullptr) {
                return NETMANAGER_ERR_INTERNAL;
            }
            int i = 0;
            for (auto it = capabilities.bearerTypes_.begin(); it != capabilities.bearerTypes_.end(); ++it) {
                ret.bearerTypes[i] = *it;
                i++;
            }
        }
        if (ret.networkCapSize > 0) {
            ret.networkCap = static_cast<int32_t *>(malloc(sizeof(int32_t) * ret.networkCapSize));
            if (ret.networkCap == nullptr) {
                free(ret.bearerTypes);
                ret.bearerTypes = nullptr;
                return NETMANAGER_ERR_INTERNAL;
            }
            int i = 0;
            for (auto it = capabilities.netCaps_.begin(); it != capabilities.netCaps_.end(); ++it) {
                ret.networkCap[i] = *it;
                i++;
            }
        }
    }
    return code;
}

void FreeLinkAddresses(CConnectionProperties &ret)
{
    if (ret.linkAddresses == nullptr) {
        return;
    }
    for (int64_t i = 0; i < ret.linkAddressSize; ++i) {
        if (ret.linkAddresses[i].address.address == nullptr) {
            continue;
        }
        free(ret.linkAddresses[i].address.address);
        ret.linkAddresses[i].address.address = nullptr;
    }
    free(ret.linkAddresses);
    ret.linkAddresses = nullptr;
}

void FreeDnses(CConnectionProperties &ret)
{
    if (ret.dnses == nullptr) {
        return;
    }
    for (int64_t i = 0; i < ret.dnsSize; ++i) {
        if (ret.dnses[i].address == nullptr) {
            continue;
        }
        free(ret.dnses[i].address);
        ret.dnses[i].address = nullptr;
    }
    free(ret.dnses);
    ret.dnses = nullptr;
}

bool SetLinkAddr(NetLinkInfo &linkInfo, CConnectionProperties &ret)
{
    if (ret.linkAddressSize > 0) {
        ret.linkAddresses = static_cast<CLinkAddress *>(malloc(sizeof(CLinkAddress) * ret.linkAddressSize));
        if (ret.linkAddresses == nullptr) {
            return false;
        }
        int i = 0;
        for (auto it = linkInfo.netAddrList_.begin(); it != linkInfo.netAddrList_.end(); ++it, ++i) {
            CNetAddress netAddr{.address = MallocCString(it->address_), .family = it->family_, .port = it->port_};
            ret.linkAddresses[i] = CLinkAddress{.address = netAddr, .prefixLength = it->prefixlen_};
        }
    }
    return true;
}

bool SetDns(NetLinkInfo &linkInfo, CConnectionProperties &ret)
{
    if (ret.dnsSize > 0) {
        ret.dnses = static_cast<CNetAddress *>(malloc(sizeof(CNetAddress) * ret.dnsSize));
        if (ret.dnses == nullptr) {
            FreeLinkAddresses(ret);
            return false;
        }
        int i = 0;
        for (auto it = linkInfo.dnsList_.begin(); it != linkInfo.dnsList_.end(); ++it, ++i) {
            ret.dnses[i] =
                CNetAddress{.address = MallocCString(it->address_), .family = it->family_, .port = it->port_};
        }
    }
    return true;
}

bool SetRoute(NetLinkInfo &linkInfo, CConnectionProperties &ret)
{
    if (ret.routeSize > 0) {
        ret.routes = static_cast<CRouteInfo *>(malloc(sizeof(CRouteInfo) * ret.routeSize));
        if (ret.routes == nullptr) {
            FreeLinkAddresses(ret);
            FreeDnses(ret);
            return false;
        }
        int i = 0;

        for (auto it = linkInfo.routeList_.begin(); it != linkInfo.routeList_.end(); ++it, ++i) {
            CNetAddress destAddr = {.address = MallocCString(it->destination_.address_),
                                    .family = it->destination_.family_,
                                    .port = it->destination_.port_};
            CLinkAddress dest = {.address = destAddr, .prefixLength = it->destination_.prefixlen_};
            CNetAddress gateway = {.address = MallocCString(it->gateway_.address_),
                                   .family = it->gateway_.family_,
                                   .port = it->gateway_.port_};
            ret.routes[i] = CRouteInfo{.interfaceName = MallocCString(it->iface_),
                                       .destination = dest,
                                       .gateway = gateway,
                                       .hasGateway = it->hasGateway_,
                                       .isDefaultRoute = it->isDefaultRoute_};
        }
    }
    return true;
}

int32_t CJ_GetConnectionProperties(int32_t netId, CConnectionProperties &ret)
{
    NetHandle netHandle{netId};
    NetLinkInfo linkInfo;
    auto code = NetConnClient::GetInstance().GetConnectionProperties(netHandle, linkInfo);
    if (code == NETMANAGER_SUCCESS) {
        ret.interfaceName = MallocCString(linkInfo.ifaceName_);
        ret.domains = MallocCString(linkInfo.domain_);
        ret.mtu = linkInfo.mtu_;
        ret.linkAddresses = nullptr;
        ret.dnses = nullptr;
        ret.routes = nullptr;
        ret.linkAddressSize = static_cast<int64_t>(linkInfo.netAddrList_.size());
        ret.dnsSize = static_cast<int64_t>(linkInfo.dnsList_.size());
        ret.routeSize = static_cast<int64_t>(linkInfo.routeList_.size());

        if (!SetLinkAddr(linkInfo, ret) || !SetDns(linkInfo, ret) || !SetRoute(linkInfo, ret)) {
            free(ret.interfaceName);
            free(ret.domains);
            ret.interfaceName = nullptr;
            ret.domains = nullptr;
            return NETMANAGER_ERR_INTERNAL;
        }
    }
    return code;
}

int32_t CJ_GetGlobalHttpProxy(CHttpProxy &chttpProxy)
{
    HttpProxy httpProxy;
    auto ret = NetConnClient::GetInstance().GetGlobalHttpProxy(httpProxy);
    if (ret == NETMANAGER_SUCCESS) {
        chttpProxy.host = MallocCString(httpProxy.GetHost());
        chttpProxy.port = httpProxy.GetPort();
        auto list = httpProxy.GetExclusionList();
        chttpProxy.exclusionListSize = static_cast<int64_t>(list.size());
        chttpProxy.exclusionList = MallocCStringList(list);
    }
    return ret;
}

int32_t CJ_GetDefaultHttpProxy(CHttpProxy &chttpProxy)
{
    HttpProxy httpProxy;
    auto ret = NetConnClient::GetInstance().GetDefaultHttpProxy(httpProxy);
    if (ret == NETMANAGER_SUCCESS) {
        chttpProxy.host = MallocCString(httpProxy.GetHost());
        chttpProxy.port = httpProxy.GetPort();
        auto list = httpProxy.GetExclusionList();
        chttpProxy.exclusionListSize = static_cast<int64_t>(list.size());
        chttpProxy.exclusionList = MallocCStringList(list);
    }
    return ret;
}

int32_t CJ_SetGlobalHttpProxy(CHttpProxy cHttpProxy)
{
    std::string host(cHttpProxy.host);
    std::string newHost = host;
    std::list<std::string> exclusionList;
    for (int64_t i = 0; i < cHttpProxy.exclusionListSize; ++i) {
        std::string tmp(cHttpProxy.exclusionList[i]);
        std::string item = tmp;
        exclusionList.push_back(item);
    }
    HttpProxy httpProxy(newHost, cHttpProxy.port, exclusionList);
    return NetConnClient::GetInstance().SetGlobalHttpProxy(httpProxy);
}

int32_t CJ_GetAppNet(int32_t &netId)
{
    return NetConnClient::GetInstance().GetAppNet(netId);
}

int32_t CJ_SetAppNet(int32_t netId)
{
    return NetConnClient::GetInstance().SetAppNet(netId);
}

RetDataCArrI32 CJ_GetAllNets()
{
    std::list<sptr<NetHandle>> netList;
    auto code = NetConnClient::GetInstance().GetAllNets(netList);
    CArrI32 data = {.head = nullptr, .size = 0 };
    RetDataCArrI32 ret = {.code = code, .data = data};
    if (code != NETMANAGER_SUCCESS) {
        return ret;
    }
    auto listSize = netList.size();
    ret.data.size = static_cast<int64_t>(listSize);
    if (listSize > 0) {
        int32_t *retValue = static_cast<int32_t *>(malloc(sizeof(int32_t) * listSize));
        if (retValue == nullptr) {
            ret.code = NETMANAGER_ERR_INTERNAL;
            return ret;
        }
        int i = 0;
        for (auto it = netList.begin(); it != netList.end(); ++it) {
            NetHandle netHandle = *it->GetRefPtr();
            retValue[i] = netHandle.GetNetId();
            i++;
        }
        ret.data.head = retValue;
    }

    return ret;
}

int32_t CJ_EnableAirplaneMode()
{
    return NetConnClient::GetInstance().SetAirplaneMode(true);
}

int32_t CJ_DisableAirplaneMode()
{
    return NetConnClient::GetInstance().SetAirplaneMode(false);
}

int32_t CJ_ReportNetConnected(int32_t netId)
{
    NetHandle netHandle{netId};
    return NetConnClient::GetInstance().NetDetection(netHandle);
}

int32_t CJ_ReportNetDisconnected(int32_t netId)
{
    NetHandle netHandle{netId};
    return NetConnClient::GetInstance().NetDetection(netHandle);
}

int32_t CJ_NetHandleBindSocket(int32_t netId, int socketFd)
{
    NetHandle handle(netId);
    return handle.BindSocket(socketFd);
}

int32_t CJ_NetConnectionRegister(int64_t id)
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(id);
    if (!instance) {
        NETMANAGER_BASE_LOGE("NetConnectionProxy instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->RegisterCallback();
}

int32_t CJ_NetConnectionUnRegister(int64_t id)
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(id);
    if (!instance) {
        NETMANAGER_BASE_LOGE("NetConnectionProxy instance not exist %{public}" PRId64, id);
        return ERR_INVALID_INSTANCE_CODE;
    }
    return instance->UnregisterCallback();
}

void CJ_OnNetAvailable(int64_t connId, void (*callback)(int32_t))
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(connId);
    if (!instance) {
        NETMANAGER_BASE_LOGE("NetConnectionProxy instance not exist %{public}" PRId64, connId);
        return;
    }
    instance->OnNetAvailible(callback);
}

void CJ_OnNetBlockStatusChange(int64_t connId, void (*callback)(int32_t, bool))
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(connId);
    if (!instance) {
        NETMANAGER_BASE_LOGE("NetConnectionProxy instance not exist %{public}" PRId64, connId);
        return;
    }
    instance->OnNetBlockStatusChange(callback);
}

void CJ_OnNetCapabilitiesChange(int64_t connId, void (*callback)(CNetCapabilityInfo))
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(connId);
    if (!instance) {
        NETMANAGER_BASE_LOGE("NetConnectionProxy instance not exist %{public}" PRId64, connId);
        return;
    }
    instance->OnNetCapabilitiesChange(callback);
}

void CJ_OnNetConnectionPropertiesChange(int64_t connId, void (*callback)(int32_t, CConnectionProperties))
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(connId);
    if (!instance) {
        NETMANAGER_BASE_LOGE("NetConnectionProxy instance not exist %{public}" PRId64, connId);
        return;
    }
    instance->OnNetConnectionPropertiesChange(callback);
}

void CJ_OnNetLost(int64_t connId, void (*callback)(int32_t))
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(connId);
    if (!instance) {
        NETMANAGER_BASE_LOGE("NetConnectionProxy instance not exist %{public}" PRId64, connId);
        return;
    }
    instance->OnNetLost(callback);
}

void CJ_OnNetUnavailable(int64_t connId, void (*callback)())
{
    auto instance = FFI::FFIData::GetData<NetConnectionProxy>(connId);
    if (!instance) {
        NETMANAGER_BASE_LOGE("NetConnectionProxy instance not exist %{public}" PRId64, connId);
        return;
    }
    instance->OnNetUnavailable(callback);
}

char *MallocCString(const std::string &origin)
{
    if (origin.empty()) {
        return nullptr;
    }
    auto len = origin.length() + 1;
    char *res = static_cast<char *>(malloc(sizeof(char) * len));
    if (res == nullptr) {
        return nullptr;
    }
    return std::char_traits<char>::copy(res, origin.c_str(), len);
}

char **MallocCStringList(std::list<std::string> &list)
{
    auto size = list.size();
    if (size <= 0) {
        return nullptr;
    }
    auto arr = static_cast<char **>(malloc(sizeof(char *) * size));
    if (arr == nullptr) {
        return nullptr;
    }
    int i = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        arr[i] = MallocCString(*it);
        i++;
    }
    return arr;
}

napi_value FfiConvertNetHandle2Napi(napi_env env, uint32_t netId)
{
    NetHandle netHandle{netId};
    return ConnectionExec::CreateNetHandle(env, &netHandle);
}
EXTERN_C_END
} // namespace OHOS::NetManagerStandard
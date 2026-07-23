/*
* Copyright (c) 2023-2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
 */

#include <map>
#include <memory>
#include <new>
#include <sstream>

#include "net_conn_client.h"
#include "net_connection_adapter.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "securec.h"

namespace OHOS::NetManagerStandard {

using BearTypeMap = std::map<NetBearType, NetConn_NetBearerType>;
using ReverseBearTypeMap = std::map<NetConn_NetBearerType, NetBearType>;
using NetCapMap = std::map<NetCap, NetConn_NetCap>;
using ReverseNetCapMap = std::map<NetConn_NetCap, NetCap>;

static BearTypeMap bearTypeMap = {{BEARER_CELLULAR, NETCONN_BEARER_CELLULAR},
                                  {BEARER_WIFI, NETCONN_BEARER_WIFI},
                                  {BEARER_BLUETOOTH, NETCONN_BEARER_BLUETOOTH},
                                  {BEARER_ETHERNET, NETCONN_BEARER_ETHERNET},
                                  {BEARER_VPN, NETCONN_BEARER_VPN}};

static ReverseBearTypeMap reverseBearTypeMap = {{NETCONN_BEARER_CELLULAR, BEARER_CELLULAR},
                                                {NETCONN_BEARER_WIFI, BEARER_WIFI},
                                                {NETCONN_BEARER_BLUETOOTH, BEARER_BLUETOOTH},
                                                {NETCONN_BEARER_ETHERNET, BEARER_ETHERNET},
                                                {NETCONN_BEARER_VPN, BEARER_VPN}};

static NetCapMap netCapMap = {{NET_CAPABILITY_MMS, NETCONN_NET_CAPABILITY_MMS},
                              {NET_CAPABILITY_SUPL, NETCONN_NET_CAPABILITY_SUPL},
                              {NET_CAPABILITY_DUN, NETCONN_NET_CAPABILITY_DUN},
                              {NET_CAPABILITY_IA, NETCONN_NET_CAPABILITY_IA},
                              {NET_CAPABILITY_XCAP, NETCONN_NET_CAPABILITY_XCAP},
                              {NET_CAPABILITY_BIP, NETCONN_NET_CAPABILITY_BIP},
                              {NET_CAPABILITY_NOT_METERED, NETCONN_NET_CAPABILITY_NOT_METERED},
                              {NET_CAPABILITY_INTERNET, NETCONN_NET_CAPABILITY_INTERNET},
                              {NET_CAPABILITY_NOT_VPN, NETCONN_NET_CAPABILITY_NOT_VPN},
                              {NET_CAPABILITY_VALIDATED, NETCONN_NET_CAPABILITY_VALIDATED},
                              {NET_CAPABILITY_PORTAL, NETCONN_NET_CAPABILITY_PORTAL},
                              {NET_CAPABILITY_CHECKING_CONNECTIVITY, NETCONN_NET_CAPABILITY_CHECKING_CONNECTIVITY}};

static ReverseNetCapMap reverseNetCapMap = {
    {NETCONN_NET_CAPABILITY_MMS, NET_CAPABILITY_MMS},
    {NETCONN_NET_CAPABILITY_SUPL, NET_CAPABILITY_SUPL},
    {NETCONN_NET_CAPABILITY_DUN, NET_CAPABILITY_DUN},
    {NETCONN_NET_CAPABILITY_IA, NET_CAPABILITY_IA},
    {NETCONN_NET_CAPABILITY_XCAP, NET_CAPABILITY_XCAP},
    {NETCONN_NET_CAPABILITY_BIP, NET_CAPABILITY_BIP},
    {NETCONN_NET_CAPABILITY_NOT_METERED, NET_CAPABILITY_NOT_METERED},
    {NETCONN_NET_CAPABILITY_INTERNET, NET_CAPABILITY_INTERNET},
    {NETCONN_NET_CAPABILITY_NOT_VPN, NET_CAPABILITY_NOT_VPN},
    {NETCONN_NET_CAPABILITY_VALIDATED, NET_CAPABILITY_VALIDATED},
    {NETCONN_NET_CAPABILITY_PORTAL, NET_CAPABILITY_PORTAL},
    {NETCONN_NET_CAPABILITY_CHECKING_CONNECTIVITY, NET_CAPABILITY_CHECKING_CONNECTIVITY}};

// LCOV_EXCL_START
static int32_t Conv2Ch(const std::string s, char *ch)
{
    if (s.length() > NETCONN_MAX_STR_LEN - 1) {
        NETMGR_LOG_E("string out of memory");
        return NETMANAGER_ERR_INTERNAL;
    }
    if (strcpy_s(ch, NETCONN_MAX_STR_LEN, s.c_str()) != EOK) {
        NETMGR_LOG_E("string copy failed");
        return NETMANAGER_ERR_INTERNAL;
    }
    return NETMANAGER_SUCCESS;
}

static int32_t Conv2INetAddr(const INetAddr &netAddrObj, NetConn_NetAddr *netAddr)
{
    netAddr->family = netAddrObj.family_;
    netAddr->prefixlen = netAddrObj.prefixlen_;
    netAddr->port = netAddrObj.port_;

    int32_t ret = Conv2Ch(netAddrObj.address_, netAddr->address);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    return NETMANAGER_SUCCESS;
}

int32_t Conv2NetHandleList(const std::list<sptr<NetHandle>> &netHandleObjList, NetConn_NetHandleList *netHandleList)
{
    int32_t i = 0;
    for (const auto &netHandleObj : netHandleObjList) {
        if (i > NETCONN_MAX_NET_SIZE - 1) {
            NETMGR_LOG_E("netHandleList out of memory");
            return NETMANAGER_ERR_INTERNAL;
        }
        netHandleList->netHandles[i++].netId = (*netHandleObj).GetNetId();
    }
    netHandleList->netHandleListSize = netHandleObjList.size();
    return NETMANAGER_SUCCESS;
}

int32_t Conv2NetHandle(NetHandle &netHandleObj, NetConn_NetHandle *netHandle)
{
    netHandle->netId = netHandleObj.GetNetId();
    return NETMANAGER_SUCCESS;
}

int32_t Conv2NetHandleObj(NetConn_NetHandle *netHandle, NetHandle &netHandleObj)
{
    netHandleObj.SetNetId(netHandle->netId);
    return NETMANAGER_SUCCESS;
}

int32_t Conv2HttpProxy(const HttpProxy &httpProxyObj, NetConn_HttpProxy *httpProxy)
{
    int32_t ret = Conv2Ch(httpProxyObj.GetHost(), httpProxy->host);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    httpProxy->port = httpProxyObj.GetPort();

    int32_t i = 0;
    for (const auto &exclusion : httpProxyObj.GetExclusionList()) {
        if (i > NETCONN_MAX_EXCLUSION_SIZE - 1) {
            NETMGR_LOG_E("exclusionList out of memory");
            return NETMANAGER_ERR_INTERNAL;
        }
        ret = Conv2Ch(exclusion, httpProxy->exclusionList[i++]);
        if (ret != NETMANAGER_SUCCESS) {
            return ret;
        }
    }

    httpProxy->exclusionListSize = static_cast<int32_t>(httpProxyObj.GetExclusionList().size());

    return NETMANAGER_SUCCESS;
}

void ConvertNetConn2HttpProxy(const NetConn_HttpProxy &netConn, HttpProxy &httpProxyObj)
{
    httpProxyObj.SetHost(std::string(netConn.host));
    httpProxyObj.SetPort(netConn.port);
    std::list<std::string> exclusionList;
    for (int32_t i = 0; i < netConn.exclusionListSize; i++) {
        exclusionList.emplace_back(netConn.exclusionList[i]);
    }
    httpProxyObj.SetExclusionList(exclusionList);
}

int32_t Conv2NetLinkInfo(NetLinkInfo &infoObj, NetConn_ConnectionProperties *prop)
{
    int32_t ret = Conv2Ch(infoObj.ifaceName_, prop->ifaceName);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    ret = Conv2Ch(infoObj.domain_, prop->domain);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    ret = Conv2Ch(infoObj.tcpBufferSizes_, prop->tcpBufferSizes);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    int32_t i = 0;
    for (const auto &netAddr : infoObj.netAddrList_) {
        if (i > NETCONN_MAX_ADDR_SIZE - 1) {
            NETMGR_LOG_E("netAddrList out of memory");
            return NETMANAGER_ERR_INTERNAL;
        }
        ret = Conv2INetAddr(netAddr, &(prop->netAddrList[i++]));
        if (ret != NETMANAGER_SUCCESS) {
            return ret;
        }
    }
    prop->netAddrListSize = static_cast<int32_t>(infoObj.netAddrList_.size());

    i = 0;
    for (const auto &dns : infoObj.dnsList_) {
        if (i > NETCONN_MAX_ADDR_SIZE - 1) {
            NETMGR_LOG_E("dnsList out of memory");
            return NETMANAGER_ERR_INTERNAL;
        }
        ret = Conv2INetAddr(dns, &(prop->dnsList[i++]));
        if (ret != NETMANAGER_SUCCESS) {
            return ret;
        }
    }
    prop->dnsListSize = static_cast<int32_t>(infoObj.dnsList_.size());

    ret = Conv2HttpProxy(infoObj.httpProxy_, &(prop->httpProxy));
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    return NETMANAGER_SUCCESS;
}

int32_t Conv2NetAllCapabilities(NetAllCapabilities &netAllCapsObj, NetConn_NetCapabilities *netAllCaps)
{
    netAllCaps->linkUpBandwidthKbps = netAllCapsObj.linkUpBandwidthKbps_;
    netAllCaps->linkDownBandwidthKbps = netAllCapsObj.linkDownBandwidthKbps_;

    int32_t i = 0;
    for (const auto &netCap : netAllCapsObj.netCaps_) {
        if (i > NETCONN_MAX_CAP_SIZE - 1) {
            NETMGR_LOG_E("netCapsList out of memory");
            return NETMANAGER_ERR_INTERNAL;
        }

        NetCapMap::iterator iterMap = netCapMap.find(netCap);
        if (iterMap == netCapMap.end()) {
            NETMGR_LOG_E("unknown netCapMap key");
            return NETMANAGER_ERR_INTERNAL;
        }
        netAllCaps->netCaps[i++] = iterMap->second;
    }
    netAllCaps->netCapsSize = static_cast<int32_t>(netAllCapsObj.netCaps_.size());

    i = 0;
    for (const auto &bearType : netAllCapsObj.bearerTypes_) {
        if (i > NETCONN_MAX_BEARER_TYPE_SIZE - 1) {
            NETMGR_LOG_E("bearerTypes out of memory");
            return NETMANAGER_ERR_INTERNAL;
        }

        BearTypeMap::iterator iterMap = bearTypeMap.find(bearType);
        if (iterMap == bearTypeMap.end()) {
            NETMGR_LOG_E("unknown bearTypeMap key");
            return NETMANAGER_ERR_INTERNAL;
        }
        netAllCaps->bearerTypes[i++] = iterMap->second;
    }
    netAllCaps->bearerTypesSize = static_cast<int32_t>(netAllCapsObj.bearerTypes_.size());

    return NETMANAGER_SUCCESS;
}

int32_t ConvFromNetAllCapabilities(NetAllCapabilities &netAllCapsObj, NetConn_NetCapabilities *netAllCaps)
{
    netAllCapsObj.linkUpBandwidthKbps_ = netAllCaps->linkUpBandwidthKbps;
    netAllCapsObj.linkDownBandwidthKbps_ = netAllCaps->linkDownBandwidthKbps;

    if (netAllCaps->netCapsSize > NETCONN_MAX_CAP_SIZE) {
        NETMGR_LOG_E("netCapsList out of memory");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    for (int32_t i = 0; i < netAllCaps->netCapsSize; ++i) {
        auto netCap = netAllCaps->netCaps[i];
        auto iterMap = reverseNetCapMap.find(netCap);
        if (iterMap == reverseNetCapMap.end()) {
            NETMGR_LOG_E("unknown netCapMap key");
            return NETMANAGER_ERR_PARAMETER_ERROR;
        }
        netAllCapsObj.netCaps_.insert(iterMap->second);
    }

    if (netAllCaps->bearerTypesSize > NETCONN_MAX_BEARER_TYPE_SIZE) {
        NETMGR_LOG_E("bearerTypes out of memory");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    for (int32_t i = 0; i < netAllCaps->bearerTypesSize; ++i) {
        auto bearType = netAllCaps->bearerTypes[i];
        auto iterMap = reverseBearTypeMap.find(bearType);
        if (iterMap == reverseBearTypeMap.end()) {
            NETMGR_LOG_E("unknown bearTypeMap key");
            return NETMANAGER_ERR_PARAMETER_ERROR;
        }
        netAllCapsObj.bearerTypes_.insert(iterMap->second);
    }

    return NETMANAGER_SUCCESS;
}

std::vector<std::string> splitStr(const std::string &str, const char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int32_t Conv2TraceRouteInfoRtt(const std::string &rttStr, uint32_t (*rtt)[NETCONN_MAX_RTT_NUM])
{
    if (rtt == nullptr) {
        return NETMANAGER_ERR_INTERNAL;
    }
    std::vector<std::string> tokens = splitStr(rttStr, ';');
    uint32_t tokensSize = tokens.size();
    for (uint32_t i = 0; i < tokensSize; ++i) {
        if (i >= NETCONN_MAX_RTT_NUM) {
            return NETMANAGER_SUCCESS;
        }
        double num;
        std::istringstream iss(tokens[i]);
        if (iss >> num) {
            (*rtt)[i] = static_cast<uint32_t>(num);
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t Conv2TraceRouteInfo(const std::string &traceRouteInfoStr, NetConn_TraceRouteInfo *traceRouteInfo,
                            uint32_t maxJumpNumber)
{
    if (traceRouteInfo == nullptr) {
        return NETMANAGER_ERR_INTERNAL;
    }

    // traceRouteInfo is "1 *.*.*.*;2;3;4 ..." pos is space position
    const uint32_t pos2 = 2;
    const uint32_t pos3 = 3;
    std::vector<std::string> tokens = splitStr(traceRouteInfoStr, ' ');
    uint32_t tokensSize = static_cast<uint32_t>(tokens.size());
    for (uint32_t i = 0; i * pos3 < tokensSize; i++) {
        if (i >= maxJumpNumber) {
            return NETMANAGER_SUCCESS;
        }
        uint32_t num = 0;
        std::istringstream iss(tokens[i * pos3]);
        if (iss >> num) {
            traceRouteInfo[i].jumpNo = static_cast<uint8_t>(num);
        }
        if (strcpy_s(traceRouteInfo[i].address, NETCONN_MAX_STR_LEN, tokens[i * pos3 + 1].c_str()) != 0) {
            NETMGR_LOG_E("Conv2TraceRouteInfo string copy failed");
            return NETMANAGER_ERR_INTERNAL;
        }
        if (Conv2TraceRouteInfoRtt(tokens[i * pos3 + pos2], &traceRouteInfo[i].rtt) != NETMANAGER_SUCCESS) {
            return NETMANAGER_ERR_INTERNAL;
        }
    }
    return NETMANAGER_SUCCESS;
}

void InvokeRefreshCallback(OH_NetConn_GlobalHttpProxyRefreshCallback callback, int32_t ret, const HttpProxy &httpProxy,
    void *userContext)
{
    if (callback == nullptr) {
        return;
    }
    if (ret != NETMANAGER_SUCCESS || httpProxy.GetHost().empty()) {
        callback(NETMANAGER_ERR_INTERNAL, nullptr, userContext);
        return;
    }
    NetConn_HttpProxy netHttpProxy;
    int32_t retConv = Conv2HttpProxy(httpProxy, &netHttpProxy);
    if (retConv != NETMANAGER_SUCCESS) {
        callback(ret, nullptr, userContext);
    } else {
        callback(ret, &netHttpProxy, userContext);
    }
}

NetConnCallbackStubAdapter::NetConnCallbackStubAdapter(NetConn_NetConnCallback *callback)
{
    this->callback_.onNetworkAvailable = callback->onNetworkAvailable;
    this->callback_.onNetCapabilitiesChange = callback->onNetCapabilitiesChange;
    this->callback_.onConnetionProperties = callback->onConnetionProperties;
    this->callback_.onNetLost = callback->onNetLost;
    this->callback_.onNetUnavailable = callback->onNetUnavailable;
    this->callback_.onNetBlockStatusChange = callback->onNetBlockStatusChange;
}

int32_t NetConnCallbackStubAdapter::NetAvailable(sptr<NetHandle> &netHandle)
{
    if (this->callback_.onNetworkAvailable == nullptr || netHandle == nullptr) {
        return NETMANAGER_SUCCESS;
    }
    NetConn_NetHandle netHandleInner;
    int32_t ret = Conv2NetHandle(*netHandle, &netHandleInner);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    this->callback_.onNetworkAvailable(&netHandleInner);
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStubAdapter::NetCapabilitiesChange(sptr<NetHandle> &netHandle,
                                                          const sptr<NetAllCapabilities> &netAllCap)
{
    if (this->callback_.onNetCapabilitiesChange == nullptr || netHandle == nullptr || netAllCap == nullptr) {
        return NETMANAGER_SUCCESS;
    }
    NetConn_NetHandle netHandleInner;
    NetConn_NetCapabilities netAllCapsInner;
    int32_t ret = Conv2NetHandle(*netHandle, &netHandleInner);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    ret = Conv2NetAllCapabilities(*netAllCap, &netAllCapsInner);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    this->callback_.onNetCapabilitiesChange(&netHandleInner, &netAllCapsInner);
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStubAdapter::NetConnectionPropertiesChange(sptr<NetHandle> &netHandle,
                                                                  const sptr<NetLinkInfo> &info)
{
    if (this->callback_.onConnetionProperties == nullptr || netHandle == nullptr || info == nullptr) {
        return NETMANAGER_SUCCESS;
    }
    NetConn_NetHandle netHandleInner;
    auto netInfoInner = std::unique_ptr<NetConn_ConnectionProperties>(
        new (std::nothrow) NetConn_ConnectionProperties());
    if (netInfoInner == nullptr) {
        NETMGR_LOG_E("NetConn_ConnectionProperties alloc failed");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    int32_t ret = Conv2NetHandle(*netHandle, &netHandleInner);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    ret = Conv2NetLinkInfo(*info, netInfoInner.get());
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    this->callback_.onConnetionProperties(&netHandleInner, netInfoInner.get());
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStubAdapter::NetLost(sptr<NetHandle> &netHandle)
{
    if (this->callback_.onNetLost == nullptr || netHandle == nullptr) {
        return NETMANAGER_SUCCESS;
    }
    NetConn_NetHandle netHandleInner;
    int32_t ret = Conv2NetHandle(*netHandle, &netHandleInner);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }

    this->callback_.onNetLost(&netHandleInner);
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStubAdapter::NetUnavailable()
{
    if (this->callback_.onNetUnavailable == nullptr) {
        return NETMANAGER_SUCCESS;
    }
    this->callback_.onNetUnavailable();
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackStubAdapter::NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
{
    if (this->callback_.onNetBlockStatusChange == nullptr || netHandle == nullptr) {
        return NETMANAGER_SUCCESS;
    }
    NetConn_NetHandle netHandleInner;
    int32_t ret = Conv2NetHandle(*netHandle, &netHandleInner);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    this->callback_.onNetBlockStatusChange(&netHandleInner, blocked);
    return NETMANAGER_SUCCESS;
}

NetConnCallbackManager &NetConnCallbackManager::GetInstance()
{
    static NetConnCallbackManager instance;
    return instance;
}

int32_t NetConnCallbackManager::RegisterNetConnCallback(NetConn_NetSpecifier *specifier,
                                                        NetConn_NetConnCallback *netConnCallback,
                                                        const uint32_t &timeout, uint32_t *callbackId)
{
    sptr<NetConnCallbackStubAdapter> callback = sptr<NetConnCallbackStubAdapter>::MakeSptr(netConnCallback);
    sptr<NetSpecifier> specifierInner = new NetSpecifier;

    if (specifier != nullptr) {
        int32_t ret = ConvFromNetAllCapabilities(specifierInner->netCapabilities_, &specifier->caps);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("ConvFromNetAllCapabilities failed");
            return ret;
        }
        if (specifier->bearerPrivateIdentifier != nullptr) {
            specifierInner->ident_ = std::string(specifier->bearerPrivateIdentifier);
        }
        ret = NetConnClient::GetInstance().RegisterNetConnCallback(specifierInner, callback, timeout);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("RegisterNetConnCallback failed");
            return ret;
        }
    } else {
        int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(callback);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("RegisterNetConnCallback failed");
            return ret;
        }
    }

    std::lock_guard<std::mutex> lock(this->callbackMapMutex_);
    *callbackId = this->index_++;
    this->callbackMap_[*callbackId] = callback;
    return NETMANAGER_SUCCESS;
}

int32_t NetConnCallbackManager::UnregisterNetConnCallback(uint32_t callbackId)
{
    std::lock_guard<std::mutex> lock(this->callbackMapMutex_);
    auto it = this->callbackMap_.find(callbackId);
    if (it != this->callbackMap_.end()) {
        int32_t ret = NetConnClient::GetInstance().UnregisterNetConnCallback(it->second);
        this->callbackMap_.erase(it);
        return ret;
    } else {
        return NET_CONN_ERR_CALLBACK_NOT_FOUND;
    }
}
// LCOV_EXCL_STOP
} // namespace OHOS::NetManagerStandard

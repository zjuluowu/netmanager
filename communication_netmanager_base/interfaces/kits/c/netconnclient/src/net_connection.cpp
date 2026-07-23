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

#include <netdb.h>
#include <atomic>

#include "net_probe.h"
#include "net_connection.h"
#include "net_conn_client.h"
#include "net_connection_adapter.h"
#include "net_connection_type.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"

using namespace OHOS::NetManagerStandard;

constexpr int32_t VALID_NETID_START = 100;
constexpr int32_t PAC_URL_MAX_LEN = 1024;

static int32_t ErrorCodeTrans(int status)
{
    int32_t ret;
    switch (status) {
        case EAI_BADFLAGS:
            if (errno == EPERM || errno == EACCES) {
                ret = NETMANAGER_ERR_PERMISSION_DENIED;
            } else {
                ret = NETMANAGER_ERR_PARAMETER_ERROR;
            }
            break;
        case EAI_SERVICE:
            ret = NETMANAGER_ERR_OPERATION_FAILED;
            break;
        default:
            ret = NETMANAGER_ERR_INTERNAL;
            break;
    }
    return ret;
}

int32_t OH_NetConn_GetAddrInfo(char *host, char *serv, struct addrinfo *hint, struct addrinfo **res, int32_t netId)
{
    int32_t ret = NETMANAGER_SUCCESS;
    int status = 0;
    struct queryparam qp_param;
    if (host == nullptr || res == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetAddrInfo received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    if (strlen(host) == 0) {
        NETMGR_LOG_E("OH_NetConn_GetAddrInfo received invalid host");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    if (netId > 0 && netId < VALID_NETID_START) {
        NETMGR_LOG_E("OH_NetConn_GetAddrInfo received invalid netId");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    if (memset_s(&qp_param, sizeof(struct queryparam), 0, sizeof(struct queryparam)) != EOK) {
        NETMGR_LOG_E("OH_NetConn_GetAddrInfo memset_s failed!");
        return NETMANAGER_ERR_MEMSET_FAIL;
    }
    qp_param.qp_netid = netId;
    qp_param.qp_type = 0;

    status = getaddrinfo_ext(host, serv, hint, res, &qp_param);
    if (status < 0) {
        NETMGR_LOG_E("OH_NetConn_GetAddrInfo fail status:%{public}d", status);
        ret = ErrorCodeTrans(status);
    }

    return ret;
}

int32_t OH_NetConn_FreeDnsResult(struct addrinfo *res)
{
    if (res == nullptr) {
        NETMGR_LOG_E("OH_NetConn_FreeDnsResult received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    freeaddrinfo(res);

    return NETMANAGER_SUCCESS;
}

int32_t OH_NetConn_GetAllNets(NetConn_NetHandleList *netHandleList)
{
    if (netHandleList == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetAllNets received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    std::list<OHOS::sptr<NetHandle>> netHandleObjList;
    int32_t ret = NetConnClient::GetInstance().GetAllNets(netHandleObjList);
    int32_t retConv = Conv2NetHandleList(netHandleObjList, netHandleList);
    if (retConv != NETMANAGER_SUCCESS) {
        return retConv;
    }
    return ret;
}

int32_t OH_NetConn_HasDefaultNet(int32_t *hasDefaultNet)
{
    if (hasDefaultNet == nullptr) {
        NETMGR_LOG_E("OH_NetConn_HasDefaultNet received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    bool flagBool = false;
    int32_t ret = NetConnClient::GetInstance().HasDefaultNet(flagBool);
    *hasDefaultNet = flagBool;
    return ret;
}

int32_t OH_NetConn_GetDefaultNet(NetConn_NetHandle *netHandle)
{
    if (netHandle == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetDefaultNet received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    NetHandle netHandleObj = NetHandle();
    int32_t ret = NetConnClient::GetInstance().GetDefaultNet(netHandleObj);
    int32_t retConv = Conv2NetHandle(netHandleObj, netHandle);
    if (retConv != NETMANAGER_SUCCESS) {
        return retConv;
    }
    return ret;
}

int32_t OH_NetConn_IsDefaultNetMetered(int32_t *isMetered)
{
    if (isMetered == nullptr) {
        NETMGR_LOG_E("OH_NetConn_IsDefaultNetMetered received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    bool flagBool = false;
    int32_t ret = NetConnClient::GetInstance().IsDefaultNetMetered(flagBool);
    *isMetered = flagBool;
    return ret;
}

int32_t OH_NetConn_GetConnectionProperties(NetConn_NetHandle *netHandle, NetConn_ConnectionProperties *prop)
{
    if (netHandle == nullptr || prop == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetConnectionProperties received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    NetHandle netHandleObj = NetHandle();
    int32_t retConv = Conv2NetHandleObj(netHandle, netHandleObj);
    if (retConv != NETMANAGER_SUCCESS) {
        return retConv;
    }
    NetLinkInfo infoObj = NetLinkInfo();
    int32_t ret = NetConnClient::GetInstance().GetConnectionProperties(netHandleObj, infoObj);
    retConv = Conv2NetLinkInfo(infoObj, prop);
    if (retConv != NETMANAGER_SUCCESS) {
        return retConv;
    }
    return ret;
}

int32_t OH_NetConn_GetNetCapabilities(NetConn_NetHandle *netHandle, NetConn_NetCapabilities *netAllCapabilities)
{
    if (netHandle == nullptr || netAllCapabilities == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetNetCapabilities received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    NetHandle netHandleObj = NetHandle();
    int32_t retConv = Conv2NetHandleObj(netHandle, netHandleObj);
    if (retConv != NETMANAGER_SUCCESS) {
        return retConv;
    }
    NetAllCapabilities netAllCapsObj = NetAllCapabilities();
    int32_t ret = NetConnClient::GetInstance().GetNetCapabilities(netHandleObj, netAllCapsObj);
    retConv = Conv2NetAllCapabilities(netAllCapsObj, netAllCapabilities);
    if (retConv != NETMANAGER_SUCCESS) {
        return retConv;
    }
    return ret;
}

int32_t OH_NetConn_GetDefaultHttpProxy(NetConn_HttpProxy *httpProxy)
{
    if (httpProxy == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetDefaultHttpProxy received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    HttpProxy httpProxyObj = HttpProxy();
    int32_t ret = NetConnClient::GetInstance().GetDefaultHttpProxy(httpProxyObj);
    int32_t retConv = Conv2HttpProxy(httpProxyObj, httpProxy);
    if (retConv != NETMANAGER_SUCCESS) {
        return retConv;
    }
    return ret;
}

int32_t OHOS_NetConn_RegisterDnsResolver(OH_NetConn_CustomDnsResolver resolver)
{
    if (resolver == nullptr) {
        NETMGR_LOG_E("OHOS_NetConn_RegisterDnsResolver received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    int32_t ret = setdnsresolvehook(resolver);
    if (ret < 0) {
        ret = NETMANAGER_ERR_PARAMETER_ERROR;
    }
    return ret;
}

int32_t OHOS_NetConn_UnregisterDnsResolver()
{
    int32_t ret = removednsresolvehook();
    return ret;
}

int32_t OH_NetConn_RegisterDnsResolver(OH_NetConn_CustomDnsResolver resolver)
{
    if (resolver == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterDnsResolver received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    int32_t ret = setdnsresolvehook(resolver);
    if (ret < 0) {
        ret = NETMANAGER_ERR_PARAMETER_ERROR;
    }
    return ret;
}

int32_t OH_NetConn_UnregisterDnsResolver()
{
    int32_t ret = removednsresolvehook();
    return ret;
}

int32_t OH_NetConn_RegisterCustomDnsResolver(OH_NetConn_CustomDnsResolver resolver)
{
    if (resolver == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterCustomDnsResolver received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    int32_t ret = setcustomdnsresolver(resolver);
    if (ret < 0) {
        ret = NET_CONN_ERR_SAME_CALLBACK;
    }
    return ret;
}

int32_t OH_NetConn_UnregisterCustomDnsResolver()
{
    int32_t ret = removecustomdnsresolver();
    if (ret != NETMANAGER_SUCCESS) {
        ret = NETMANAGER_ERR_INTERNAL;
    }
    return ret;
}

int32_t OH_NetConn_BindSocket(int32_t socketFd, NetConn_NetHandle *netHandle)
{
    if (netHandle == nullptr) {
        NETMGR_LOG_E("OH_NetConn_BindSocket netHandle is NULL");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    if (socketFd < 0) {
        NETMGR_LOG_E("OH_NetConn_BindSocket socketFd is invalid");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    if (netHandle->netId < VALID_NETID_START) {
        NETMGR_LOG_E("OH_NetConn_BindSocket netId is invalid");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    int32_t ret = NetConnClient::GetInstance().BindSocket(socketFd, netHandle->netId);
    return ret;
}

static int32_t RegisterErrorCodeTrans(int32_t err)
{
    switch (err) {
        case NETMANAGER_SUCCESS:                    // fall through
        case NETMANAGER_ERR_PERMISSION_DENIED:      // fall through
        case NETMANAGER_ERR_PARAMETER_ERROR:        // fall through
        case NETMANAGER_ERR_OPERATION_FAILED:       // fall through
        case NET_CONN_ERR_CALLBACK_NOT_FOUND:       // fall through
        case NET_CONN_ERR_SAME_CALLBACK:            // fall through
        case NET_CONN_ERR_NET_OVER_MAX_REQUEST_NUM:
            return err;
        default:
            return NETMANAGER_ERR_INTERNAL;
    }
}

int32_t OH_NetConn_RegisterNetConnCallback(NetConn_NetSpecifier *specifier, NetConn_NetConnCallback *netConnCallback,
                                           uint32_t timeout, uint32_t *callbackId)
{
    if (specifier == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterNetConnCallback specifier is NULL");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    if (netConnCallback == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterNetConnCallback netConnCallback is NULL");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    if (callbackId == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterNetConnCallback callbackId is NULL");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    int32_t ret = NetConnCallbackManager::GetInstance().RegisterNetConnCallback(specifier, netConnCallback, timeout,
                                                                                callbackId);
    return RegisterErrorCodeTrans(ret);
}

int32_t OH_NetConn_RegisterDefaultNetConnCallback(NetConn_NetConnCallback *netConnCallback, uint32_t *callbackId)
{
    if (netConnCallback == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterNetConnCallback netConnCallback is NULL");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    if (callbackId == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterNetConnCallback callbackId is NULL");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    int32_t ret = NetConnCallbackManager::GetInstance().RegisterNetConnCallback(nullptr, netConnCallback, 0,
                                                                                callbackId);
    return RegisterErrorCodeTrans(ret);
}

int32_t OH_NetConn_UnregisterNetConnCallback(uint32_t callBackId)
{
    int32_t ret = NetConnCallbackManager::GetInstance().UnregisterNetConnCallback(callBackId);
    return RegisterErrorCodeTrans(ret);
}

int32_t OH_NetConn_SetAppHttpProxy(NetConn_HttpProxy *httpProxy)
{
    if (httpProxy == nullptr) {
        NETMGR_LOG_E("OH_NetConn_SetAppHttpProxy received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    HttpProxy httpProxyObj;
    ConvertNetConn2HttpProxy(*httpProxy, httpProxyObj);
    int32_t ret = NetConnClient::GetInstance().SetAppHttpProxy(httpProxyObj);
    return ret;
}

int32_t OH_NetConn_RegisterAppHttpProxyCallback(OH_NetConn_AppHttpProxyChange appHttpProxyChange, uint32_t *callbackId)
{
    if (appHttpProxyChange == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterAppHttpProxyCallback received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    if (callbackId == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RegisterAppHttpProxyCallback received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto opration = [appHttpProxyChange](const HttpProxy& httpProxy) {
        NetConn_HttpProxy netHttpProxy;
        int32_t retConv = Conv2HttpProxy(httpProxy, &netHttpProxy);
        if (retConv != NETMANAGER_SUCCESS) {
            appHttpProxyChange(nullptr);
        } else {
            appHttpProxyChange(&netHttpProxy);
        }
    };
    uint32_t id;
    NetConnClient::GetInstance().RegisterAppHttpProxyCallback(opration, id);
    *callbackId = id;
    return NETMANAGER_SUCCESS;
}

void OH_NetConn_UnregisterAppHttpProxyCallback(uint32_t callbackId)
{
    NetConnClient::GetInstance().UnregisterAppHttpProxyCallback(callbackId);
}

int32_t OH_NetConn_SetPacUrl(const char *pacUrl)
{
    if (pacUrl == nullptr) {
        NETMGR_LOG_E("OH_NetConn_SetPacUrl received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    int32_t ret = NetConnClient::GetInstance().SetPacUrl(std::string(pacUrl));
    return ret;
}

int32_t OH_NetConn_GetPacUrl(char *pacUrl)
{
    if (pacUrl == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetPacUrl received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    std::string pacUrlstr = "";
    int32_t ret = NetConnClient::GetInstance().GetPacUrl(pacUrlstr);
    if (strcpy_s(pacUrl, PAC_URL_MAX_LEN, pacUrlstr.c_str()) != 0) {
        NETMGR_LOG_E("OH_NetConn_GetPacUrl string copy failed");
        return NETMANAGER_ERR_INTERNAL;
    }
    return ret;
}

int32_t OH_NetConn_SetProxyMode(const OHOS::NetManagerStandard::ProxyModeType mode)
{
    int32_t ret = NetConnClient::GetInstance().SetProxyMode(mode);
    return ret;
}

int32_t OH_NetConn_GetProxyMode(OHOS::NetManagerStandard::ProxyModeType *mode)
{
    if (mode == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetProxyMode received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    OHOS::NetManagerStandard::ProxyModeType temp;
    int32_t ret = NetConnClient::GetInstance().GetProxyMode(temp);
    if (ret) {
        NETMGR_LOG_E("OH_NetConn_GetProxyMode string copy failed");
        return RegisterErrorCodeTrans(ret);
    }
    *mode = temp;
    return ret;
}

int32_t OH_NetConn_SetPacFileUrl(const char *pacUrl)
{
    if (pacUrl == nullptr) {
        NETMGR_LOG_E("OH_NetConn_SetPacUrl received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    int32_t ret = NetConnClient::GetInstance().SetPacFileUrl(std::string(pacUrl));
    return ret;
}

int32_t OH_NetConn_GetPacFileUrl(char *pacUrl)
{
    if (pacUrl == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetPacUrl received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    std::string pacUrlstr = "";
    int32_t ret = NetConnClient::GetInstance().GetPacFileUrl(pacUrlstr);
    if (strcpy_s(pacUrl, PAC_URL_MAX_LEN, pacUrlstr.c_str()) != 0) {
        NETMGR_LOG_E("OH_NetConn_GetPacUrl string copy failed");
        return NETMANAGER_ERR_INTERNAL;
    }
    return ret;
}

int32_t OH_NetConn_FindProxyForURL(const char *url, const char *host, char *proxy)
{
    if (url == nullptr) {
        NETMGR_LOG_E("OH_NetConn_GetPacUrl received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    std::string pacProxyStr = "";
    std::string hostStr;
    if (host && strlen(host) > 0) {
        hostStr.append(host);
    }
    int32_t ret = NetConnClient::GetInstance().FindProxyForURL(url, pacProxyStr, hostStr);
    if (strcpy_s(proxy, PAC_URL_MAX_LEN, pacProxyStr.c_str()) != 0) {
        NETMGR_LOG_E("OH_NetConn_GetPacUrl string copy failed");
        return NETMANAGER_ERR_INTERNAL;
    }
    return ret;
}

int32_t OH_NetConn_QueryProbeResult(const char *destination, int32_t duration,
                                    OHOS::NetManagerStandard::NetConn_ProbeResultInfo *result)
{
    if (destination == nullptr || result == nullptr) {
        NETMGR_LOG_E("OH_NetConn_QueryProbeResult received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    std::string dest(destination);
    NetProbe np;
    int ret = np.QueryProbeResult(dest, duration, *result);
    if (ret != 0) {
        NETMGR_LOG_E("Query probe result failed.");
    }
    return ret;
}

int32_t OH_NetConn_QueryTraceRoute(const char *destination, OHOS::NetManagerStandard::NetConn_TraceRouteOption *option,
    OHOS::NetManagerStandard::NetConn_TraceRouteInfo *traceRouteInfo)
{
    if (destination == nullptr || traceRouteInfo == nullptr) {
        NETMGR_LOG_E("OH_NetConn_QueryTraceRoute received invalid parameters");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    int32_t packetsType = NetConn_PacketsType::NETCONN_PACKETS_ICMP;
    int32_t maxJumpNumber = NETCONN_MAX_JUMP_NUM;
    if (option != nullptr) {
        maxJumpNumber = static_cast<int32_t>(option->maxJumpNumber);
        if (maxJumpNumber > NETCONN_MAX_JUMP_NUM) {
            return NETMANAGER_ERR_PARAMETER_ERROR;
        }
        packetsType = static_cast<int32_t>(option->packetsType);
    }
    std::string traceRouteInfoStr = "";
    int32_t ret = NetConnClient::GetInstance().QueryTraceRoute(std::string(destination), maxJumpNumber, packetsType,
                                                               traceRouteInfoStr);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("OH_NetConn_QueryTraceRoute query failed with error code: %d", ret);
        return ret;
    }
    if (Conv2TraceRouteInfo(traceRouteInfoStr, traceRouteInfo, maxJumpNumber) != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("OH_NetConn_QueryTraceRoute conv2 routeinfo failed");
    }
    return ret;
}

int32_t OH_NetConn_RefreshGlobalHttpProxyWithCallback(OH_NetConn_GlobalHttpProxyRefreshCallback callback,
    void *userContext)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("OH_NetConn_RefreshGlobalHttpProxyWithCallback callback is NULL");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto refreshCallback = [callback, userContext](int32_t ret, const HttpProxy &httpProxy) {
        InvokeRefreshCallback(callback, ret, httpProxy, userContext);
    };
    int32_t ret = NetConnClient::GetInstance().RefreshGlobalHttpProxy(refreshCallback);
    if (ret != NETMANAGER_SUCCESS) {
        return ret;
    }
    return NETMANAGER_SUCCESS;
}

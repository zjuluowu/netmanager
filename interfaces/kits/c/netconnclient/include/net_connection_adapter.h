/*
* Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef NATIVE_NET_CONN_ADAPTER_H
#define NATIVE_NET_CONN_ADAPTER_H

#include <map>
#include <mutex>

#include "http_proxy.h"
#include "net_all_capabilities.h"
#include "net_conn_callback_stub.h"
#include "net_connection_type.h"
#include "net_handle.h"
#include "net_link_info.h"
#include "refbase.h"

namespace OHOS::NetManagerStandard {

int32_t Conv2NetHandle(NetHandle &netHandleObj, NetConn_NetHandle *netHandle);

int32_t Conv2NetHandleObj(NetConn_NetHandle *netHandle, NetHandle &netHandleObj);

int32_t Conv2NetHandleList(const std::list<sptr<NetHandle>> &netHandleObjList, NetConn_NetHandleList *netHandleList);

int32_t Conv2NetLinkInfo(NetLinkInfo &infoObj, NetConn_ConnectionProperties *prop);

int32_t Conv2NetAllCapabilities(NetAllCapabilities &netAllCapsObj, NetConn_NetCapabilities *netAllCaps);

int32_t ConvFromNetAllCapabilities(NetAllCapabilities &netAllCapsObj, NetConn_NetCapabilities *netAllCaps);

int32_t Conv2HttpProxy(const HttpProxy &httpProxyObj, NetConn_HttpProxy *httpProxy);

void ConvertNetConn2HttpProxy(const NetConn_HttpProxy &netConn, HttpProxy &httpProxyObj);

int32_t Conv2TraceRouteInfo(
    const std::string &traceRouteInfoStr, NetConn_TraceRouteInfo *traceRouteInfo, uint32_t maxJumpNumber);

int32_t Conv2TraceRouteInfoRtt(const std::string &rttStr, uint32_t (*rtt)[NETCONN_MAX_RTT_NUM]);

void InvokeRefreshCallback(OH_NetConn_GlobalHttpProxyRefreshCallback callback, int32_t ret, const HttpProxy &httpProxy,
    void *userContext);

class NetConnCallbackStubAdapter : public NetConnCallbackStub {
public:
    NetConnCallbackStubAdapter(NetConn_NetConnCallback *callback);

    int32_t NetAvailable(sptr<NetHandle> &netHandle) override;
    int32_t NetCapabilitiesChange(sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap) override;
    int32_t NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info) override;
    int32_t NetLost(sptr<NetHandle> &netHandle) override;
    int32_t NetUnavailable() override;
    int32_t NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked) override;

private:
    NetConn_NetConnCallback callback_{};
};

class NetConnCallbackManager {
public:
    static NetConnCallbackManager &GetInstance();
    int32_t RegisterNetConnCallback(NetConn_NetSpecifier *specifier, NetConn_NetConnCallback *netConnCallback,
                                    const uint32_t &timeout, uint32_t *callbackId);
    int32_t UnregisterNetConnCallback(uint32_t callbackId);

private:
    NetConnCallbackManager() = default;
    std::mutex callbackMapMutex_;
    std::map<uint32_t, sptr<INetConnCallback>> callbackMap_;
    uint32_t index_{0};
};

} // namespace OHOS::NetManagerStandard
#endif /* NATIVE_NET_CONN_ADAPTER_H */
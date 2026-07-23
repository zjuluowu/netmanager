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
#include "net_connection_impl.h"
#include "cj_lambda.h"
#include <shared_mutex>
#include "net_conn_client.h"
#include "netmanager_base_log.h"
#include "net_specifier.h"

namespace OHOS::NetManagerStandard {
std::map<ConnectionCallbackObserver *, NetConnectionImpl *> NET_CONNECTIONS_FFI;
std::shared_mutex g_netConnectionsMutex;

NetConnectionProxy::NetConnectionProxy(CNetSpecifier specifier, uint32_t timeout)
{
    NetConnectionImpl *netConnection = NetConnectionImpl::MakeNetConnection();
    if (netConnection == nullptr) {
        return;
    }
    if (specifier.hasSpecifier) {
        netConnection->hasNetSpecifier_ = true;
        NetSpecifier netSpecifier;
        netSpecifier.ident_ = std::string(specifier.bearerPrivateIdentifier);
        for (int64_t i = 0; i < specifier.netCapabilities.bearedTypeSize; i++) {
            auto netbear = static_cast<NetBearType>(specifier.netCapabilities.bearerTypes[i]);
            netSpecifier.netCapabilities_.bearerTypes_.insert(netbear);
        }
        for (int64_t i = 0; i < specifier.netCapabilities.networkCapSize; i++) {
            auto cap = static_cast<NetCap>(specifier.netCapabilities.networkCap[i]);
            netSpecifier.netCapabilities_.netCaps_.insert(cap);
        }
        netSpecifier.netCapabilities_.linkUpBandwidthKbps_ = specifier.netCapabilities.linkUpBandwidthKbps;
        netSpecifier.netCapabilities_.linkDownBandwidthKbps_ = specifier.netCapabilities.linkDownBandwidthKbps;
        netConnection->netSpecifier_ = netSpecifier;
    }
    if (timeout > 0) {
        netConnection->hasTimeout_ = true;
        netConnection->timeout_ = timeout;
    }
    netConn_ = netConnection;
}

int32_t NetConnectionProxy::RegisterCallback()
{
    sptr<INetConnCallback> callback = netConn_->GetObserver();
    if (netConn_->hasNetSpecifier_) {
        sptr<NetSpecifier> specifier = new NetSpecifier(netConn_->netSpecifier_);
        int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(specifier, callback, netConn_->timeout_);
        NETMANAGER_BASE_LOGI("Register result hasNetSpecifier_ and hasTimeout_ %{public}d", ret);
        return ret;
    }
    int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(callback);
    return ret;
}

int32_t NetConnectionProxy::UnregisterCallback()
{
    sptr<INetConnCallback> callback = netConn_->GetObserver();

    int32_t ret = NetConnClient::GetInstance().UnregisterNetConnCallback(callback);
    if (ret != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("Unregister result %{public}d", ret);
    }
    return ret;
}

void NetConnectionProxy::OnNetAvailible(void (*callback)(int32_t))
{
    netConn_->netAvailible.push_back(CJLambda::Create(callback));
}

void NetConnectionProxy::OnNetBlockStatusChange(void (*callback)(int32_t, bool))
{
    netConn_->netBlockStatusChange.push_back(CJLambda::Create(callback));
}

void NetConnectionProxy::OnNetCapabilitiesChange(void (*callback)(CNetCapabilityInfo))
{
    netConn_->netCapabilitiesChange.push_back(CJLambda::Create(callback));
}

void NetConnectionProxy::OnNetConnectionPropertiesChange(void (*callback)(int32_t, CConnectionProperties))
{
    netConn_->netConnectionPropertiesChange.push_back(CJLambda::Create(callback));
}

void NetConnectionProxy::OnNetLost(void (*callback)(int32_t))
{
    netConn_->netLost.push_back(CJLambda::Create(callback));
}

void NetConnectionProxy::OnNetUnavailable(void (*callback)())
{
    netConn_->netUnavailable.push_back(CJLambda::Create(callback));
}

void NetConnectionProxy::Release()
{
    netConn_->DeleteNetConnection(netConn_);
}

NetConnectionImpl::NetConnectionImpl()
    : hasNetSpecifier_(false), hasTimeout_(false), timeout_(0), observer_(new ConnectionCallbackObserver)
{
}

NetConnectionImpl *NetConnectionImpl::MakeNetConnection()
{
    std::unique_lock lock(g_netConnectionsMutex);
    auto netConnection = new NetConnectionImpl();
    if (netConnection) {
        NET_CONNECTIONS_FFI[netConnection->observer_.GetRefPtr()] = netConnection;
    }
    return netConnection;
}

void NetConnectionImpl::DeleteNetConnection(NetConnectionImpl *netConnection)
{
    std::unique_lock lock(g_netConnectionsMutex);
    NET_CONNECTIONS_FFI.erase(netConnection->observer_.GetRefPtr());
    delete netConnection;
}

sptr<ConnectionCallbackObserver> NetConnectionImpl::GetObserver() const
{
    return observer_;
}
} // namespace OHOS::NetManagerStandard
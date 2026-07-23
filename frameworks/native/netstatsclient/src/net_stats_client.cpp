/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "net_stats_client.h"
#include <thread>

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "net_all_capabilities.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "sys/socket.h"
#include "net_stats_service_proxy.h"

static constexpr uint32_t WAIT_FOR_SERVICE_TIME_MS = 500;
static constexpr uint32_t MAX_GET_SERVICE_COUNT = 10;

namespace OHOS {
namespace NetManagerStandard {
NetStatsClient::NetStatsClient() : netStatsService_(nullptr), deathRecipient_(nullptr), callback_(nullptr) {}

NetStatsClient::~NetStatsClient()
{
    NETMGR_LOG_I("~NetStatsClient : Destroy NetStatsClient");
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        return;
    }
 
    auto serviceRemote = proxy->AsObject();
    if (serviceRemote == nullptr) {
        return;
    }
 
    serviceRemote->RemoveDeathRecipient(deathRecipient_);
}

int32_t NetStatsClient::RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback)
{
    NETMGR_LOG_D("RegisterNetStatsCallback client in");
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    int32_t ret = proxy->RegisterNetStatsCallback(callback);
    if (ret == NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("RegisterNetStatsCallback success, save callback");
        std::unique_lock<std::shared_mutex> lock(callbackMutex_);
        callback_ = callback;
    }

    return ret;
}

int32_t NetStatsClient::UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    int32_t ret = proxy->UnregisterNetStatsCallback(callback);
    if (ret == NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("UnRegisterNetStatsCallback success, delete callback");
        std::unique_lock<std::shared_mutex> lock(callbackMutex_);
        callback_ = nullptr;
    }

    return ret;
}

sptr<INetStatsService> NetStatsClient::GetProxy()
{
    std::lock_guard lock(mutex_);

    if (netStatsService_ != nullptr) {
        NETMGR_LOG_D("get proxy is ok");
        return netStatsService_;
    }

    NETMGR_LOG_D("execute GetSystemAbilityManager");
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        NETMGR_LOG_E("NetPolicyManager::GetProxy(), get SystemAbilityManager failed");
        return nullptr;
    }

    sptr<IRemoteObject> remote = sam->CheckSystemAbility(COMM_NET_STATS_MANAGER_SYS_ABILITY_ID);
    if (remote == nullptr) {
        NETMGR_LOG_D("get Remote service failed");
        return nullptr;
    }

    deathRecipient_ = new (std::nothrow) NetStatsDeathRecipient(*this);
    if (deathRecipient_ == nullptr) {
        NETMGR_LOG_E("get deathRecipient_ failed");
        return nullptr;
    }
    if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
        NETMGR_LOG_E("add death recipient failed");
        return nullptr;
    }

    netStatsService_ = iface_cast<INetStatsService>(remote);
    if (netStatsService_ == nullptr) {
        NETMGR_LOG_E("get Remote service proxy failed");
        return nullptr;
    }
    return netStatsService_;
}

void NetStatsClient::RecoverCallback()
{
    uint32_t count = 0;
    while (GetProxy() == nullptr && count < MAX_GET_SERVICE_COUNT) {
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_FOR_SERVICE_TIME_MS));
        count++;
    }
    auto proxy = GetProxy();
    NETMGR_LOG_W("Get proxy %{public}s, count: %{public}u", proxy == nullptr ? "failed" : "success", count);
    std::shared_lock<std::shared_mutex> lock(callbackMutex_);
    if (proxy != nullptr && callback_ != nullptr) {
        int32_t ret = proxy->RegisterNetStatsCallback(callback_);
        NETMGR_LOG_D("Register result %{public}d", ret);
    }
}

void NetStatsClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    NETMGR_LOG_D("on remote died");
    if (remote == nullptr) {
        NETMGR_LOG_E("remote object is nullptr");
        return;
    }

    {
        std::lock_guard lock(mutex_);
        if (netStatsService_ == nullptr) {
            NETMGR_LOG_E("NetConnService_ is nullptr");
            return;
        }

        sptr<IRemoteObject> local = netStatsService_->AsObject();
        if (local != remote.promote()) {
            NETMGR_LOG_E("proxy and stub is not same remote object");
            return;
        }

        local->RemoveDeathRecipient(deathRecipient_);
        netStatsService_ = nullptr;
    }

    std::shared_lock<std::shared_mutex> lock(callbackMutex_);
    if (callback_ != nullptr) {
        NETMGR_LOG_D("on remote died recover callback");
        std::thread t([sp = shared_from_this()]() { sp->RecoverCallback(); });
        std::string threadName = "nestatsRecoverCallback";
        pthread_setname_np(t.native_handle(), threadName.c_str());
        t.detach();
    }
}

int32_t NetStatsClient::GetIfaceRxBytes(uint64_t &stats, const std::string &interfaceName)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetIfaceRxBytes(stats, interfaceName);
}

int32_t NetStatsClient::GetIfaceTxBytes(uint64_t &stats, const std::string &interfaceName)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetIfaceTxBytes(stats, interfaceName);
}

int32_t NetStatsClient::GetCellularRxBytes(uint64_t &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetCellularRxBytes(stats);
}

int32_t NetStatsClient::GetCellularTxBytes(uint64_t &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetCellularTxBytes(stats);
}

int32_t NetStatsClient::GetAllRxBytes(uint64_t &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetAllRxBytes(stats);
}

int32_t NetStatsClient::GetAllTxBytes(uint64_t &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetAllTxBytes(stats);
}

int32_t NetStatsClient::GetUidRxBytes(uint64_t &stats, uint32_t uid)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetUidRxBytes(stats, uid);
}

int32_t NetStatsClient::GetUidTxBytes(uint64_t &stats, uint32_t uid)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetUidTxBytes(stats, uid);
}

int32_t NetStatsClient::GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                            NetStatsInfo &statsInfo)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetIfaceStatsDetail(iface, start, end, statsInfo);
}

int32_t NetStatsClient::GetUidStatsDetail(const std::string &iface, uint32_t uid, uint64_t start, uint64_t end,
                                          NetStatsInfo &statsInfo)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetUidStatsDetail(iface, uid, start, end, statsInfo);
}

int32_t NetStatsClient::UpdateIfacesStats(const std::string &iface, uint64_t start, uint64_t end,
                                          const NetStatsInfo &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UpdateIfacesStats(iface, start, end, stats);
}

int32_t NetStatsClient::UpdateStatsData()
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UpdateStatsData();
}

int32_t NetStatsClient::ResetFactory()
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->ResetFactory();
}

int32_t NetStatsClient::GetAllStatsInfo(std::vector<NetStatsInfo> &infos)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetAllStatsInfo(infos);
}

int32_t NetStatsClient::GetAllContainerStatsInfo(std::vector<NetStatsInfo> &infos)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetAllSimStatsInfo(infos);
}

int32_t NetStatsClient::GetTrafficStatsByNetwork(std::unordered_map<uint32_t, NetStatsInfo> &infos,
                                                 const sptr<NetStatsNetwork> &network)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    if (network == nullptr) {
        NETMGR_LOG_E("network is nullptr");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    if (network->startTime_ > network->endTime_) {
        NETMGR_LOG_E("network is invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    if (network->type_ > static_cast<uint32_t>(BEARER_DEFAULT)) {
        NETMGR_LOG_E("network is invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    return proxy->GetTrafficStatsByNetwork(infos, *network);
}

int32_t NetStatsClient::GetMonthTrafficStatsByNetwork(uint32_t simId, uint64_t &monthData)
{
    sptr<INetStatsService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->GetMonthTrafficStatsByNetwork(simId, monthData);
}

int32_t NetStatsClient::GetTrafficStatsByUidNetwork(std::vector<NetStatsInfoSequence> &infos, uint32_t uid,
                                                    const sptr<NetStatsNetwork> &network)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    if (network == nullptr) {
        NETMGR_LOG_E("network is nullptr");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    if (network->startTime_ > network->endTime_) {
        NETMGR_LOG_E("network is invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    if (network->type_ > static_cast<uint32_t>(BEARER_DEFAULT)) {
        NETMGR_LOG_E("network is invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    return proxy->GetTrafficStatsByUidNetwork(infos, uid, *network);
}

int32_t NetStatsClient::SetAppStats(const PushStatsInfo &info)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetAppStats(info);
}

int32_t NetStatsClient::SetDpaAppStats(const NetStatsInfo &info)
{
    sptr<INetStatsService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->SetDpaAppStats(info);
}

int32_t NetStatsClient::SaveSharingTraffic(const NetStatsInfo &infos)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SaveSharingTraffic(infos);
}

int32_t NetStatsClient::GetSockfdRxBytes(uint64_t &stats, int32_t sockfd)
{
    if (sockfd <= 0) {
        NETMGR_LOG_E("sockfd is invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    uint64_t optrval = 0;
    uint32_t optlen = sizeof(optrval);
    if (getsockopt(sockfd, SOL_SOCKET, SO_COOKIE, &optrval, &optlen) == -1) {
        NETMGR_LOG_E("getsockopt error");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return proxy->GetCookieRxBytes(stats, optrval);
}

int32_t NetStatsClient::GetSockfdTxBytes(uint64_t &stats, int32_t sockfd)
{
    if (sockfd <= 0) {
        NETMGR_LOG_E("sockfd is invalid");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    uint64_t optrval = 0;
    uint32_t optlen = sizeof(optrval);
    if (getsockopt(sockfd, SOL_SOCKET, SO_COOKIE, &optrval, &optlen) == -1) {
        NETMGR_LOG_E("getsockopt error");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return proxy->GetCookieTxBytes(stats, optrval);
}

int32_t NetStatsClient::SetCalibrationTraffic(uint32_t simId, int64_t remainingData, uint64_t totalMonthlyData)
{
    sptr<INetStatsService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->SetCalibrationTraffic(simId, remainingData, totalMonthlyData);
}

int32_t NetStatsClient::SetTrafficPlanInfo(int32_t simId, int32_t param, int64_t value)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetTrafficPlanInfo(simId, param, value);
}

int32_t NetStatsClient::GetTrafficPlanInfo(int32_t simId, int32_t param, int64_t &value)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetTrafficPlanInfo(simId, param, value);
}

extern "C" int32_t GetUidTxBytesEx(uint64_t *stats, uint32_t uid)
{
    NETMGR_LOG_D("GetUidTxBytesEx in");
    if (!stats) {
        NETMGR_LOG_E("stats is null");
        return -1;
    }
    return DelayedSingleton<NetManagerStandard::NetStatsClient>::GetInstance()->GetUidTxBytes(*stats, uid);
}

extern "C" int32_t GetUidRxBytesEx(uint64_t *stats, uint32_t uid)
{
    NETMGR_LOG_D("GetUidRxBytesEx in");
    if (!stats) {
        NETMGR_LOG_E("stats is null");
        return -1;
    }
    return DelayedSingleton<NetManagerStandard::NetStatsClient>::GetInstance()->GetUidRxBytes(*stats, uid);
}
} // namespace NetManagerStandard
} // namespace OHOS

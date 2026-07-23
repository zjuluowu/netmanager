/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "net_policy_client.h"
#include <thread>

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "net_mgr_log_wrapper.h"

static constexpr uint32_t WAIT_FOR_SERVICE_TIME_MS = 500;
static constexpr uint32_t MAX_GET_SERVICE_COUNT = 10;

namespace OHOS {
namespace NetManagerStandard {
NetPolicyClient::NetPolicyClient() : netPolicyService_(nullptr), deathRecipient_(nullptr), callback_(nullptr) {}

NetPolicyClient::~NetPolicyClient()
{
    NETMGR_LOG_I("~NetPolicyClient : Destroy NetPolicyClient");
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        return;
    }
 
    auto serviceRemote = proxy->AsObject();
    if (serviceRemote == nullptr) {
        return;
    }
    serviceRemote->RemoveDeathRecipient(deathRecipient_);
}

NetPolicyClient& NetPolicyClient::GetInstance()
{
    static std::shared_ptr<NetPolicyClient> instance = std::make_shared<NetPolicyClient>();
    return *instance;
}

int32_t NetPolicyClient::SetPolicyByUid(uint32_t uid, uint32_t policy)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetPolicyByUid(uid, policy);
}

int32_t NetPolicyClient::GetPolicyByUid(uint32_t uid, uint32_t &policy)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetPolicyByUid(uid, policy);
}

int32_t NetPolicyClient::GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->GetUidsByPolicy(policy, uids);
}

int32_t NetPolicyClient::IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IsUidNetAllowed(uid, metered, isAllowed);
}

int32_t NetPolicyClient::IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->IsUidNetAllowed(uid, ifaceName, isAllowed);
}

int32_t NetPolicyClient::IsUidNetAccess(uint32_t uid, bool isMetered, bool &isAllowed)
{
    return IsUidNetAllowed(uid, isMetered, isAllowed);
}

int32_t NetPolicyClient::IsUidNetAccess(uint32_t uid, const std::string &ifaceName, bool &isAllowed)
{
    return IsUidNetAllowed(uid, ifaceName, isAllowed);
}

sptr<INetPolicyService> NetPolicyClient::GetProxy()
{
    std::lock_guard lock(mutex_);

    if (netPolicyService_ != nullptr) {
        NETMGR_LOG_D("get proxy is ok");
        return netPolicyService_;
    }

    NETMGR_LOG_I("execute GetSystemAbilityManager");
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        NETMGR_LOG_E("NetPolicyManager::GetProxy(), get SystemAbilityManager failed");
        return nullptr;
    }

    sptr<IRemoteObject> remote = sam->CheckSystemAbility(COMM_NET_POLICY_MANAGER_SYS_ABILITY_ID);
    if (remote == nullptr) {
        NETMGR_LOG_D("get Remote service failed");
        return nullptr;
    }

    deathRecipient_ = new (std::nothrow) NetPolicyDeathRecipient(*this);
    if (deathRecipient_ == nullptr) {
        NETMGR_LOG_E("get deathRecipient_ failed");
        return nullptr;
    }
    if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
        NETMGR_LOG_E("add death recipient failed");
        return nullptr;
    }

    netPolicyService_ = iface_cast<INetPolicyService>(remote);
    if (netPolicyService_ == nullptr) {
        NETMGR_LOG_E("get Remote service proxy failed");
        return nullptr;
    }

    return netPolicyService_;
}

void NetPolicyClient::RecoverCallback()
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
        int32_t ret = proxy->RegisterNetPolicyCallback(callback_);
        NETMGR_LOG_D("Register result %{public}d", ret);
    }
}

void NetPolicyClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    NETMGR_LOG_D("on remote died");
    if (remote == nullptr) {
        NETMGR_LOG_E("remote object is nullptr");
        return;
    }
    {
        std::lock_guard lock(mutex_);
        if (netPolicyService_ == nullptr) {
            NETMGR_LOG_E("netPolicyService_ is nullptr");
            return;
        }

        sptr<IRemoteObject> local = netPolicyService_->AsObject();
        if (local != remote.promote()) {
            NETMGR_LOG_E("proxy and stub is not same remote object");
            return;
        }

        local->RemoveDeathRecipient(deathRecipient_);
        netPolicyService_ = nullptr;
    }

    std::shared_lock<std::shared_mutex> lock(callbackMutex_);
    if (callback_ != nullptr) {
        NETMGR_LOG_D("on remote died recover callback");
        std::thread t([sp = shared_from_this()]() { sp->RecoverCallback(); });
        std::string threadName = "netpolicyRecoverCallback";
        pthread_setname_np(t.native_handle(), threadName.c_str());
        t.detach();
    }
}

int32_t NetPolicyClient::RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback)
{
    NETMGR_LOG_D("RegisterNetPolicyCallback client in");
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    int32_t ret = proxy->RegisterNetPolicyCallback(callback);
    if (ret == NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("RegisterNetPolicyCallback success, save callback");
        std::unique_lock<std::shared_mutex> lock(callbackMutex_);
        callback_ = callback;
    }
    
    return ret;
}

int32_t NetPolicyClient::UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    int32_t ret = proxy->UnregisterNetPolicyCallback(callback);
    if (ret == NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("UnRegisterNetPolicyCallback success, delete callback");
        std::unique_lock<std::shared_mutex> lock(callbackMutex_);
        callback_ = nullptr;
    }

    return ret;
}

int32_t NetPolicyClient::SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    if (quotaPolicies.empty()) {
        NETMGR_LOG_E("quotaPolicies is empty");
        return NetPolicyResultCode::POLICY_ERR_INVALID_QUOTA_POLICY;
    }

    if (quotaPolicies.size() > QUOTA_POLICY_MAX_SIZE) {
        NETMGR_LOG_E("quotaPolicies's size is greater than the maximum, size is [%{public}zu]", quotaPolicies.size());
        return NetPolicyResultCode::POLICY_ERR_INVALID_QUOTA_POLICY;
    }

    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetNetQuotaPolicies(quotaPolicies);
}

int32_t NetPolicyClient::GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->GetNetQuotaPolicies(quotaPolicies);
}

NetPolicyResultCode NetPolicyClient::SetFactoryPolicy(const std::string &simId)
{
    return static_cast<NetPolicyResultCode>(ResetPolicies(simId));
}

int32_t NetPolicyClient::ResetPolicies(const std::string &simId)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->ResetPolicies(simId);
}

int32_t NetPolicyClient::SetBackgroundPolicy(bool isBackgroundPolicyAllow)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetBackgroundPolicy(isBackgroundPolicyAllow);
}

int32_t NetPolicyClient::GetBackgroundPolicy(bool &backgroundPolicy)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->GetBackgroundPolicy(backgroundPolicy);
}

int32_t NetPolicyClient::GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetBackgroundPolicyByUid(uid, backgroundPolicyOfUid);
}

NetPolicyResultCode NetPolicyClient::SetSnoozePolicy(int8_t netType, const std::string &simId)
{
    return static_cast<NetPolicyResultCode>(UpdateRemindPolicy(netType, simId, RemindType::REMIND_TYPE_LIMIT));
}

int32_t NetPolicyClient::UpdateRemindPolicy(int32_t netType, const std::string &simId, uint32_t remindType)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->UpdateRemindPolicy(netType, simId, remindType);
}

NetPolicyResultCode NetPolicyClient::SetIdleTrustlist(uint32_t uid, bool isTrustlist)
{
    return static_cast<NetPolicyResultCode>(SetDeviceIdleTrustlist({uid}, isTrustlist));
}

int32_t NetPolicyClient::SetDeviceIdleTrustlist(const std::vector<uint32_t> &uids, bool isAllowed)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetDeviceIdleTrustlist(uids, isAllowed);
}

NetPolicyResultCode NetPolicyClient::GetIdleTrustlist(std::vector<uint32_t> &uids)
{
    return static_cast<NetPolicyResultCode>(GetDeviceIdleTrustlist(uids));
}

int32_t NetPolicyClient::GetDeviceIdleTrustlist(std::vector<uint32_t> &uids)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->GetDeviceIdleTrustlist(uids);
}

int32_t NetPolicyClient::SetDeviceIdlePolicy(bool enable)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetDeviceIdlePolicy(enable);
}

int32_t NetPolicyClient::GetPowerSaveTrustlist(std::vector<uint32_t> &uids)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->GetPowerSaveTrustlist(uids);
}

int32_t NetPolicyClient::SetPowerSaveTrustlist(const std::vector<uint32_t> &uids, bool isAllowed)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetPowerSaveTrustlist(uids, isAllowed);
}

int32_t NetPolicyClient::SetPowerSavePolicy(bool enable)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetPowerSavePolicy(enable);
}

int32_t NetPolicyClient::CheckPermission()
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->CheckPermission();
}

int32_t NetPolicyClient::SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetNetworkAccessPolicy(uid, policy, reconfirmFlag);
}

int32_t NetPolicyClient::AddNetworkAccessPolicy(const std::vector<std::string> &bundleNames)
{
    sptr<INetPolicyService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->AddNetworkAccessPolicy(bundleNames);
}

int32_t NetPolicyClient::RemoveNetworkAccessPolicy(const std::vector<std::string> &bundleNames)
{
    sptr<INetPolicyService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    return proxy->RemoveNetworkAccessPolicy(bundleNames);
}

int32_t NetPolicyClient::GetNetworkAccessPolicy(AccessPolicyParameter parameter, AccessPolicySave& policy)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetNetworkAccessPolicy(parameter, policy);
}

int32_t NetPolicyClient::GetSelfNetworkAccessPolicy(NetAccessPolicy& policy)
{
    sptr<INetPolicyService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP
    int32_t ret =  proxy->GetSelfNetworkAccessPolicy(policy);
    return ret;
}

int32_t NetPolicyClient::NotifyNetAccessPolicyDiag(uint32_t uid)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->NotifyNetAccessPolicyDiag(uid);
}

int32_t NetPolicyClient::SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status)
{
    sptr<INetPolicyService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->SetNicTrafficAllowed(ifaceNames, status);
}

int32_t NetPolicyClient::SetInternetAccessByIpForWifiShare(
    const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName)
{
    sptr<INetPolicyService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP

    return proxy->SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
}

int32_t NetPolicyClient::SetIdleDenyPolicy(bool enable)
{
    sptr<INetPolicyService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP

    return proxy->SetIdleDenyPolicy(enable);
}

int32_t NetPolicyClient::SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd)
{
    sptr<INetPolicyService> proxy = GetProxy();
    // LCOV_EXCL_START
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    // LCOV_EXCL_STOP

    return proxy->SetUidsDeniedListChain(uids, isAdd);
}

} // namespace NetManagerStandard
} // namespace OHOS

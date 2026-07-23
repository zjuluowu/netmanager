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

#include "net_policy_service.h"

#include <algorithm>
#include <dlfcn.h>

#include "system_ability_definition.h"

#include "bundle_constants.h"
#include "bundle_mgr_proxy.h"
#include "ffrt_inner.h"
#include "iservice_registry.h"
#include "net_access_policy_config.h"
#include "net_manager_center.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_policy_base.h"
#include "net_policy_constants.h"
#include "net_policy_core.h"
#include "net_policy_file.h"
#include "net_policy_inner_define.h"
#include "net_quota_policy.h"
#include "net_settings.h"
#include "netmanager_base_common_utils.h"
#include "netmanager_base_permission.h"
#include "system_ability_definition.h"
#include "net_policy_listener.h"
#include "net_policy_db_clone.h"
#include "net_access_policy_dialog.h"
#include "os_account_manager.h"
#include "system_timer.h"
#include "unique_fd.h"
#include "ipc_skeleton.h"

#ifdef __LP64__
const std::string LIB_LOAD_PATH = "/system/lib64/libnet_access_policy_dialog.z.so";
#else
const std::string LIB_LOAD_PATH = "/system/lib/libnet_access_policy_dialog.z.so";
#endif

using GetNetBundleClass = OHOS::NetManagerStandard::INetAccessPolicyDialog *(*)();
namespace OHOS {
namespace NetManagerStandard {
namespace {
const std::string LIB_NET_BUNDLE_UTILS_PATH = "libnet_bundle_utils.z.so";
constexpr const char *INSTALL_SOURCE_DEFAULT = "default";
constexpr const char *EXTENSION_BACKUP = "backup";
constexpr const char *EXTENSION_RESTORE = "restore";
constexpr const char *EXTENSION_SUCCESS = "netpolicy extension success";
constexpr const char *EXTENSION_FAIL = "netpolicy extension fail";
constexpr uint64_t DELAY_US = 30 * 1000 * 1000;
constexpr uint32_t DAY_MILLISECONDS = 24 * 60 * 60 * 1000;
constexpr int32_t HMOS_FUSION_MANAGER_UID = 7558;

sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        NETMGR_LOG_E("fail to get system ability mgr.");
        return nullptr;
    }

    auto remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        NETMGR_LOG_E("fail to get bundle manager proxy.");
        return nullptr;
    }

    sptr<AppExecFwk::IBundleMgr> iBundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (iBundleMgr == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return nullptr;
    }
    return iBundleMgr;
}
const int32_t MAIN_USER_ID = 100;
const int32_t NET_ACCESS_POLICY_ALLOW_VALUE = 1;
} // namespace
static std::atomic<bool> g_RegisterToService(
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<NetPolicyService>::GetInstance().get()));

NetPolicyService::NetPolicyService()
    : SystemAbility(COMM_NET_POLICY_MANAGER_SYS_ABILITY_ID, true), state_(STATE_STOPPED)
{
    isDisplayTrafficAncoList_ = CommonUtils::IsNeedDisplayTrafficAncoList();
}

NetPolicyService::~NetPolicyService() = default;

// LCOV_EXCL_START
void NetPolicyService::OnStart()
{
    NETMGR_LOG_I("OnStart");
    if (state_ == STATE_RUNNING) {
        NETMGR_LOG_W("NetPolicyService already start.");
        return;
    }

    if (!g_RegisterToService) {
        g_RegisterToService =
            SystemAbility::MakeAndRegisterAbility(DelayedSingleton<NetPolicyService>::GetInstance().get());
        if (!g_RegisterToService) {
            NETMGR_LOG_E("Register to local sa manager failed again, give up.");
            return;
        }
    }

    state_ = STATE_RUNNING;
    Init();
}

void NetPolicyService::OnStop()
{
    handler_.reset();
    netPolicyCore_.reset();
    netPolicyCallback_.reset();
    netPolicyTraffic_.reset();
    netPolicyFirewall_.reset();
    netPolicyRule_.reset();
    state_ = STATE_STOPPED;
    g_RegisterToService = false;
}

int32_t NetPolicyService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    NETMGR_LOG_D("Start policy Dump, fd: %{public}d", fd);
    std::string result;
    GetDumpMessage(result);
    int32_t ret = dprintf(fd, "%s\n", result.c_str());
    return ret < 0 ? NETMANAGER_ERR_PARAMETER_ERROR : NETMANAGER_SUCCESS;
}
// LCOV_EXCL_STOP

void NetPolicyService::Init()
{
    NETMGR_LOG_D("Init");
    AddSystemAbilityListener(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID);
    AddSystemAbilityListener(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    AddSystemAbilityListener(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    std::weak_ptr<NetPolicyService> wp = shared_from_this();
#ifndef UNITTEST_FORBID_FFRT
    ffrtQueue_.submit([wp]() {
#endif
        auto sharedSelf = wp.lock();
        // LCOV_EXCL_START
        if (sharedSelf == nullptr) {
            NETMGR_LOG_E("NetPolicyService::Init sharedSelf = nullptr");
            return;
        }
        // LCOV_EXCL_STOP
        sharedSelf->serviceComm_ = (std::make_unique<NetPolicyServiceCommon>()).release();
        NetManagerCenter::GetInstance().RegisterPolicyService(sharedSelf->serviceComm_);
        sharedSelf->netPolicyCore_ = DelayedSingleton<NetPolicyCore>::GetInstance();
        sharedSelf->netPolicyCallback_ = DelayedSingleton<NetPolicyCallback>::GetInstance();
        sharedSelf->netPolicyTraffic_ = sharedSelf->netPolicyCore_->CreateCore<NetPolicyTraffic>();
        sharedSelf->netPolicyFirewall_ = sharedSelf->netPolicyCore_->CreateCore<NetPolicyFirewall>();
        sharedSelf->netPolicyRule_ = sharedSelf->netPolicyCore_->CreateCore<NetPolicyRule>();
        NetAccessPolicyRDB netAccessPolicy;
        netAccessPolicy.InitRdbStore();
        sharedSelf->UpdateNetAccessPolicyToMapFromDB();
#ifndef NETMANAGER_TEST
        if (!sharedSelf->Publish(DelayedSingleton<NetPolicyService>::GetInstance().get())) {
            NETMGR_LOG_E("Register to sa manager failed");
        }
#endif
#ifndef UNITTEST_FORBID_FFRT
    }, ffrt::task_attr().name("FfrtNetPolicyServiceInit"));
    ffrtQueue_.submit([wp]() {
#endif
        auto sharedPtr = wp.lock();
        // LCOV_EXCL_START
        if (sharedPtr == nullptr) {
            NETMGR_LOG_E("NetPolicyService::Init sharedSelf = nullptr");
            return;
        }
        // LCOV_EXCL_STOP
        sharedPtr->SetBrokerUidAccessPolicyMap(std::nullopt);
#ifndef UNITTEST_FORBID_FFRT
    }, ffrt::task_attr().name("InitSetBrokerUidAccessPolicyMapFunc").delay(DELAY_US));
#endif
}

// LCOV_EXCL_START
void NetPolicyService::ListenCommonEvent()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_RESTORE_START);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    std::shared_ptr<NetPolicyListener> subscriber = std::make_shared<NetPolicyListener>(
        subscribeInfo, std::static_pointer_cast<NetPolicyService>(shared_from_this()));
    EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber);

    // Subscribe RGM status changed event with publisher UID control
    EventFwk::MatchingSkills rgmMatchingSkills;
    rgmMatchingSkills.AddEvent(COMMON_EVENT_STATUS_CHANGED);
    EventFwk::CommonEventSubscribeInfo rgmSubscribeInfo(rgmMatchingSkills);
    rgmSubscribeInfo.SetPublisherUid(HMOS_FUSION_MANAGER_UID);
    std::shared_ptr<NetPolicyListener> rgmSubscriber = std::make_shared<NetPolicyListener>(
        rgmSubscribeInfo, std::static_pointer_cast<NetPolicyService>(shared_from_this()));
    EventFwk::CommonEventManager::SubscribeCommonEvent(rgmSubscriber);
}
// LCOV_EXCL_STOP

int32_t NetPolicyService::SetPolicyByUid(uint32_t uid, uint32_t policy)
{
    NETMGR_LOG_I("SetPolicyByUid uid[%{public}d] policy[%{public}d]", uid, policy);
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyRule_->TransPolicyToRule(uid, policy);
}

int32_t NetPolicyService::GetPolicyByUid(uint32_t uid, uint32_t &policy)
{
    NETMGR_LOG_D("GetPolicyByUid uid[%{public}d]", uid);
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyRule_->GetPolicyByUid(uid, policy);
}

int32_t NetPolicyService::GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids)
{
    NETMGR_LOG_D("GetUidsByPolicy policy[%{public}d]", policy);
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyRule_->GetUidsByPolicy(policy, uids);
}

int32_t NetPolicyService::IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed)
{
    NETMGR_LOG_I("IsUidNetAllowed uid[%{public}d metered[%{public}d]", uid, metered);
    if (NetSettings::GetInstance().IsSystem(uid)) {
        isAllowed = true;
        return NETMANAGER_SUCCESS;
    }
    if (netPolicyRule_ != nullptr) {
        return netPolicyRule_->IsUidNetAllowed(uid, metered, isAllowed);
    }
    return NETMANAGER_ERR_LOCAL_PTR_NULL;
}

int32_t NetPolicyService::IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed)
{
    NETMGR_LOG_D("IsUidNetAllowed uid[%{public}d ifaceName[%{public}s]", uid, ifaceName.c_str());
    if (netPolicyTraffic_ == nullptr) {
        return 0;
    }
    if (netPolicyTraffic_->IsMeteredIfaces(ifaceName)) {
        return IsUidNetAllowed(uid, true, isAllowed);
    }
    return IsUidNetAllowed(uid, false, isAllowed);
}

int32_t NetPolicyService::RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback)
{
    NETMGR_LOG_I("RegisterNetPolicyCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("RegisterNetPolicyCallback parameter callback is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    if (netPolicyCallback_ == nullptr) {
        NETMGR_LOG_E("netPolicyCallback_ is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return netPolicyCallback_->RegisterNetPolicyCallback(callback);
}

int32_t NetPolicyService::UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback)
{
    NETMGR_LOG_I("UnregisterNetPolicyCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("UnregisterNetPolicyCallback parameter callback is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    if (netPolicyCallback_ == nullptr) {
        NETMGR_LOG_E("netPolicyCallback_ is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyCallback_->UnregisterNetPolicyCallback(callback);
}

int32_t NetPolicyService::SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    NETMGR_LOG_I("SetNetQuotaPolicies quotaPolicySize[%{public}zd]", quotaPolicies.size());
    if (netPolicyTraffic_ == nullptr) {
        NETMGR_LOG_E("netPolicyTraffic_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyTraffic_->UpdateQuotaPolicies(quotaPolicies);
}

int32_t NetPolicyService::GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies)
{
    NETMGR_LOG_D("GetNetQuotaPolicies begin");
    if (netPolicyTraffic_ == nullptr) {
        NETMGR_LOG_E("netPolicyTraffic_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyTraffic_->GetNetQuotaPolicies(quotaPolicies);
}

int32_t NetPolicyService::ResetPolicies(const std::string &simId)
{
    NETMGR_LOG_I("ResetPolicies begin");
    if (netPolicyRule_ != nullptr && netPolicyFirewall_ != nullptr && netPolicyTraffic_ != nullptr) {
        netPolicyRule_->ResetPolicies();
        netPolicyFirewall_->ResetPolicies();
        netPolicyTraffic_->ResetPolicies(simId);
        NETMGR_LOG_I("ResetPolicies end.");
        return NETMANAGER_SUCCESS;
    }
    return NETMANAGER_ERR_LOCAL_PTR_NULL;
}

int32_t NetPolicyService::SetBackgroundPolicy(bool allow)
{
    NETMGR_LOG_I("SetBackgroundPolicy allow[%{public}d]", allow);
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyRule_->SetBackgroundPolicy(allow);
}

int32_t NetPolicyService::GetBackgroundPolicy(bool &backgroundPolicy)
{
    NETMGR_LOG_D("GetBackgroundPolicy begin");
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyRule_->GetBackgroundPolicy(backgroundPolicy);
}

int32_t NetPolicyService::GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid)
{
    NETMGR_LOG_D("GetBackgroundPolicyByUid uid[%{public}d]", uid);
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyRule_->GetBackgroundPolicyByUid(uid, backgroundPolicyOfUid);
}

int32_t NetPolicyService::UpdateRemindPolicy(int32_t netType, const std::string &simId, uint32_t remindType)
{
    NETMGR_LOG_I("UpdateRemindPolicy start");
    if (netPolicyTraffic_ == nullptr) {
        NETMGR_LOG_E("netPolicyTraffic_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyTraffic_->UpdateRemindPolicy(netType, simId, remindType);
}

int32_t NetPolicyService::SetDeviceIdleTrustlist(const std::vector<uint32_t> &uids, bool isAllowed)
{
    NETMGR_LOG_D("SetDeviceIdleTrustlist start");
    if (netPolicyFirewall_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyFirewall_->SetDeviceIdleTrustlist(uids, isAllowed);
}

int32_t NetPolicyService::GetDeviceIdleTrustlist(std::vector<uint32_t> &uids)
{
    NETMGR_LOG_D("GetDeviceIdleTrustlist start");
    if (netPolicyFirewall_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyFirewall_->GetDeviceIdleTrustlist(uids);
}

int32_t NetPolicyService::SetDeviceIdlePolicy(bool enable)
{
    NETMGR_LOG_I("SetDeviceIdlePolicy enable[%{public}d]", enable);
    if (netPolicyFirewall_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyFirewall_->UpdateDeviceIdlePolicy(enable);
}

int32_t NetPolicyService::GetPowerSaveTrustlist(std::vector<uint32_t> &uids)
{
    NETMGR_LOG_D("GetPowerSaveTrustlist start");
    if (netPolicyFirewall_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyFirewall_->GetPowerSaveTrustlist(uids);
}

int32_t NetPolicyService::SetPowerSaveTrustlist(const std::vector<uint32_t> &uids, bool isAllowed)
{
    NETMGR_LOG_I("SetPowerSaveTrustlist start");
    if (netPolicyFirewall_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyFirewall_->SetPowerSaveTrustlist(uids, isAllowed);
}

int32_t NetPolicyService::SetPowerSavePolicy(bool enable)
{
    NETMGR_LOG_I("SetPowerSavePolicy enable[%{public}d]", enable);
    if (netPolicyFirewall_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyFirewall_->UpdatePowerSavePolicy(enable);
}

int32_t NetPolicyService::SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd)
{
    NETMGR_LOG_I("SetUidsDeniedListChain start");
    if (netPolicyFirewall_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyFirewall_->SetUidsDeniedListChain(uids, isAdd);
}

int32_t NetPolicyService::SetIdleDenyPolicy(bool enable)
{
    NETMGR_LOG_I("SetIdleDenyPolicy enable[%{public}d]", enable);
    if (netPolicyFirewall_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    return netPolicyFirewall_->UpdateIdleDenyPolicy(enable);
}

int32_t NetPolicyService::GetDumpMessage(std::string &message)
{
    if (netPolicyRule_ == nullptr || netPolicyTraffic_ == nullptr) {
        NETMGR_LOG_E("netPolicyFirewall_ or netPolicyTraffic_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netPolicyRule_->GetDumpMessage(message);
    netPolicyTraffic_->GetDumpMessage(message);
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyService::CheckPermission()
{
    return NETMANAGER_SUCCESS;
}

void NetPolicyService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    NETMGR_LOG_I("OnAddSystemAbility systemAbilityId[%{public}d]", systemAbilityId);
    if (systemAbilityId == COMM_NET_CONN_MANAGER_SYS_ABILITY_ID) {
        RegisterFactoryResetCallback();
    }
    if (systemAbilityId == COMMON_EVENT_SERVICE_ID) {
        ListenCommonEvent();
    }
    if (systemAbilityId == BUNDLE_MGR_SERVICE_SYS_ABILITY_ID) {
        std::weak_ptr<NetPolicyService> wp = shared_from_this();
#ifndef UNITTEST_FORBID_FFRT
        ffrtQueue_.submit([wp]() {
#endif
        auto sharedSelf = wp.lock();
        // LCOV_EXCL_START
        if (sharedSelf == nullptr) {
            return;
        }
        // LCOV_EXCL_STOP
        sharedSelf->SetBrokerUidAccessPolicyMap(std::nullopt);
#ifndef UNITTEST_FORBID_FFRT
        }, ffrt::task_attr().name("SetBrokerUidAccessPolicyMapFunc").delay(DELAY_US));
#endif
#ifndef UNITTEST_FORBID_FFRT
        ffrtQueue_.submit([wp]() {
#endif
        auto sharedPtr = wp.lock();
        // LCOV_EXCL_START
        if (sharedPtr == nullptr) {
            return;
        }
        // LCOV_EXCL_STOP
        sharedPtr->OverwriteNetAccessPolicyToDBFromConfig();
        sharedPtr->UpdateNetAccessPolicyToMapFromDB();
#ifndef UNITTEST_FORBID_FFRT
        }, ffrt::task_attr().name("NetworkAccessPolicyConfigFlush"));
#endif
    }
    if (systemAbilityId == COMM_NETSYS_NATIVE_SYS_ABILITY_ID) {
        if (hasSARemoved_) {
            OnNetSysRestart();
            hasSARemoved_ = false;
        }
    }
}

void NetPolicyService::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    NETMGR_LOG_I("OnRemoveSystemAbility systemAbilityId[%{public}d]", systemAbilityId);
    if (systemAbilityId == COMM_NETSYS_NATIVE_SYS_ABILITY_ID) {
        hasSARemoved_ = true;
    }
}

void NetPolicyService::OnNetSysRestart()
{
    NETMGR_LOG_I("OnNetSysRestart");
    
    if (netPolicyRule_ != nullptr) {
        netPolicyRule_->TransPolicyToRule();
    }
}

int32_t NetPolicyService::FactoryResetPolicies()
{
    NETMGR_LOG_I("FactoryResetPolicies begin");
    if (netPolicyRule_ != nullptr && netPolicyFirewall_ != nullptr && netPolicyTraffic_ != nullptr) {
        netPolicyRule_->ResetPolicies();
        netPolicyFirewall_->ResetPolicies();
        netPolicyTraffic_->ResetPolicies();
        ResetNetAccessPolicy();
        NETMGR_LOG_I("FactoryResetPolicies end.");
        return NETMANAGER_SUCCESS;
    }
    return NETMANAGER_ERR_LOCAL_PTR_NULL;
}

void NetPolicyService::RegisterFactoryResetCallback()
{
    NETMGR_LOG_I("RegisterFactetCallback enter.");

    if (netFactoryResetCallback_ == nullptr) {
        netFactoryResetCallback_ =
            (std::make_unique<FactoryResetCallBack>(std::static_pointer_cast<NetPolicyService>(shared_from_this())))
                .release();
    }

    if (netFactoryResetCallback_ != nullptr) {
        int32_t ret = NetManagerCenter::GetInstance().RegisterNetFactoryResetCallback(netFactoryResetCallback_);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("RegisterFactoryResetCallback ret[%{public}d]", ret);
        }
    } else {
        NETMGR_LOG_E("netFactoryResetCallback_ is null");
    }
}


int32_t NetPolicyService::RefreshNetworkAccessPolicyFromConfig()
{
    NETMGR_LOG_I("RefreshNetworkAccessPolicyFromConfigs Enter.");
    OverwriteNetAccessPolicyToDBFromConfig();
    UpdateNetAccessPolicyToMapFromDB();
    return NETMANAGER_SUCCESS;
}

void NetPolicyService::OverwriteNetAccessPolicyToDBFromConfig()
{
    std::vector<NetAccessPolicyConfig> configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    if (configs.empty()) {
        NETMGR_LOG_W("configs is empty");
        return;
    }

    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return;
    }
    NetAccessPolicyRDB netAccessPolicyRdb;
    auto ret = ERR_OK;
    int32_t userId = MAIN_USER_ID;
    if (GetActivatedOsAccountId(userId) != NETMANAGER_SUCCESS) {
        NETMGR_LOG_W("use default userId.");
    }
    for (size_t i = 0; i < configs.size(); i++) {
        if (!configs[i].disableWlanSwitch && !configs[i].disableCellularSwitch) {
            continue;
        }
        auto uid = bundleMgrProxy->GetUidByBundleName(configs[i].bundleName, userId);
        if (uid == -1) {
            NETMGR_LOG_E("Failed to get uid from bundleName. [%{public}s]", configs[i].bundleName.c_str());
            continue;
        }
        NetAccessPolicyData policyData;
        policyData.wifiPolicy = NET_ACCESS_POLICY_ALLOW_VALUE;
        policyData.cellularPolicy = NET_ACCESS_POLICY_ALLOW_VALUE;
        auto ret = netAccessPolicyRdb.QueryByUid(uid, policyData);
        if (configs[i].disableWlanSwitch) {
            policyData.wifiPolicy = NET_ACCESS_POLICY_ALLOW_VALUE;
        }
        if (configs[i].disableCellularSwitch) {
            policyData.cellularPolicy = NET_ACCESS_POLICY_ALLOW_VALUE;
        }
        policyData.setFromConfigFlag = 0;
        if (ret != NETMANAGER_SUCCESS) {
            policyData.uid = uid;
            netAccessPolicyRdb.InsertData(policyData);
            continue;
        }
        netAccessPolicyRdb.UpdateByUid(uid, policyData);
    }
}

int32_t NetPolicyService::GetActivatedOsAccountId(int32_t &userId)
{
    std::vector<int32_t> activatedOsAccountIds;
    int ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("QueryActiveOsAccountIds failed. ret is %{public}d", ret);
        return NETMANAGER_ERR_INTERNAL;
    }
    if (activatedOsAccountIds.empty()) {
        NETMGR_LOG_E("QueryActiveOsAccountIds is empty");
        return NETMANAGER_ERR_INTERNAL;
    }
    userId = activatedOsAccountIds[0];
    NETMGR_LOG_I("QueryActiveOsAccountIds is %{public}d", userId);
    return NETMANAGER_SUCCESS;
}

void NetPolicyService::UpdateNetAccessPolicyToMapFromDB()
{
    NETMGR_LOG_I("UpdateNetAccessPolicyToMapFromDB enter.");
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return;
    }
    NetAccessPolicyRDB netAccessPolicy;
    std::vector<NetAccessPolicyData> result = netAccessPolicy.QueryAll();
    for (size_t i = 0; i < result.size(); i++) {
        NetworkAccessPolicy policy;
        policy.wifiAllow = result[i].wifiPolicy;
        policy.cellularAllow = result[i].cellularPolicy;
        if (netPolicyRule_ == nullptr) {
            NETMGR_LOG_E("netPolicyRule_ is nullptr");
            break;
        }
        (void)netPolicyRule_->SetNetworkAccessPolicy(result[i].uid, policy, result[i].setFromConfigFlag);
    }
}

void NetPolicyService::ResetNetAccessPolicy()
{
    NETMGR_LOG_I("ResetNetAccessPolicy enter.");
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return;
    }
    NetAccessPolicyRDB netAccessPolicyRdb;
    std::vector<NetAccessPolicyData> result = netAccessPolicyRdb.QueryAll();
    for (size_t i = 0; i < result.size(); i++) {
        if (result[i].wifiPolicy && result[i].cellularPolicy) {
            continue;
        }
        result[i].wifiPolicy = 1;
        result[i].cellularPolicy = 1;
        netAccessPolicyRdb.UpdateByUid(result[i].uid, result[i]);
        NetworkAccessPolicy policy;
        policy.wifiAllow = result[i].wifiPolicy;
        policy.cellularAllow = result[i].cellularPolicy;
        if (netPolicyRule_ == nullptr) {
            NETMGR_LOG_E("netPolicyRule_ is nullptr");
            break;
        }
        (void)netPolicyRule_->SetNetworkAccessPolicy(result[i].uid, policy, result[i].setFromConfigFlag);
    }
}

// Do not post into event handler, because this interface should have good performance
int32_t NetPolicyService::SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag)
{
    NETMGR_LOG_I("SetNetworkAccessPolicy enter.");
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    NetAccessPolicyData data;
    data.uid = uid;
    data.wifiPolicy = policy.wifiAllow;
    data.cellularPolicy = policy.cellularAllow;
    data.setFromConfigFlag = !reconfirmFlag;
    NetAccessPolicyRDB netAccessPolicy;
    netAccessPolicy.InsertData(data);

    return netPolicyRule_->SetNetworkAccessPolicy(uid, policy, !reconfirmFlag);
}

int32_t NetPolicyService::GetNetworkAccessPolicy(AccessPolicyParameter parameter, AccessPolicySave &policy)
{
    NETMGR_LOG_I("GetNetworkAccessPolicy enter.");
    NetAccessPolicyRDB netAccessPolicy;
    uint32_t userId = parameter.callingUid / AppExecFwk::Constants::BASE_USER_RANGE;

    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return NETMANAGER_ERR_INTERNAL;
    }

    if (parameter.flag) {
        std::string uidBundleName;
        if (bundleMgrProxy->GetNameForUid(parameter.uid, uidBundleName) == ERR_OK) {
            UpdateNetworkAccessPolicyFromConfig(uidBundleName, policy.policy);
        } else {
            NETMGR_LOG_E("GetNameForUid Failed");
        }
        NetAccessPolicyData policyData;
        if (netAccessPolicy.QueryByUid(parameter.uid, policyData) != NETMANAGER_SUCCESS) {
            policy.policy.wifiAllow = true;
            policy.policy.cellularAllow = true;
            return NETMANAGER_SUCCESS;
        }
        policy.policy.wifiAllow = policyData.wifiPolicy;
        policy.policy.cellularAllow = policyData.cellularPolicy;
        return NETMANAGER_SUCCESS;
    }

    std::vector<AppExecFwk::ApplicationInfo> appInfos;
    bool retC = bundleMgrProxy->GetApplicationInfos(AppExecFwk::ApplicationFlag::GET_APPLICATION_INFO_WITH_PERMISSION,
                                                    static_cast<uint32_t>(userId), appInfos);
    if (!retC) {
        NETMGR_LOG_E("GetApplicationInfos Error");
        return NETMANAGER_ERR_INTERNAL;
    }
    for (const auto &appInfo : appInfos) {
        NetworkAccessPolicy policyTmp;
        NetAccessPolicyData policyData;
        if (netAccessPolicy.QueryByUid(appInfo.uid, policyData) == NETMANAGER_SUCCESS) {
            policyTmp.wifiAllow = policyData.wifiPolicy;
            policyTmp.cellularAllow = policyData.cellularPolicy;
        } else {
            policyTmp.wifiAllow = true;
            policyTmp.cellularAllow = true;
        }
        UpdateNetworkAccessPolicyFromConfig(appInfo.bundleName, policyTmp);
        policy.uid_policies.insert(std::pair<uint32_t, NetworkAccessPolicy>(appInfo.uid, policyTmp));
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyService::AddNetworkAccessPolicy(const std::vector<std::string> &bundleNames)
{
    NetAccessPolicyConfigUtils::GetInstance().AddNetAccessPolicyConfig(bundleNames);
    RefreshNetworkAccessPolicyFromConfig();
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyService::RemoveNetworkAccessPolicy(const std::vector<std::string> &bundleNames)
{
    NetAccessPolicyConfigUtils::GetInstance().RemoveNetAccessPolicyConfig(bundleNames);
    RefreshNetworkAccessPolicyFromConfig();
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyService::GetSelfNetworkAccessPolicy(NetAccessPolicy &policy)
{
    uint32_t callingUid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());
    
    AccessPolicyParameter parameter;
    parameter.flag = true;
    parameter.uid = callingUid;
    parameter.callingUid = callingUid;
    
    AccessPolicySave policySave;
    int32_t result = GetNetworkAccessPolicy(parameter, policySave);
    // LCOV_EXCL_START
    if (result == NETMANAGER_SUCCESS) {
        // Convert to NetAccessPolicy
        policy.allowWiFi = policySave.policy.wifiAllow;
        policy.allowCellular = policySave.policy.cellularAllow;
    }
    NETMGR_LOG_I("GetSelfNetworkAccessPolicy. uid:%{public}d, %{public}d, %{public}d",
        callingUid, policy.allowWiFi, policy.allowCellular);
    // LCOV_EXCL_STOP
    return result;
}

int32_t NetPolicyService::DeleteNetworkAccessPolicy(uint32_t uid)
{
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return netPolicyRule_->DeleteNetworkAccessPolicy(uid);
}

int32_t NetPolicyService::NotifyNetAccessPolicyDiag(uint32_t uid)
{
    NETMGR_LOG_I("NotifyNetAccessPolicyDiag");

    std::lock_guard<std::mutex> lock(mutex_);
    void *handler = dlopen(LIB_LOAD_PATH.c_str(), RTLD_LAZY | RTLD_NODELETE);
    if (handler == nullptr) {
        NETMGR_LOG_E("load failed, failed reason : %{public}s", dlerror());
        return NETMANAGER_ERR_INTERNAL;
    }

    GetNetBundleClass GetNetAccessPolicyDialog = (GetNetBundleClass)dlsym(handler, "GetNetAccessPolicyDialog");
    if (GetNetAccessPolicyDialog == nullptr) {
        NETMGR_LOG_E("GetNetAccessPolicyDialog faild, failed reason : %{public}s", dlerror());
        dlclose(handler);
        return NETMANAGER_ERR_INTERNAL;
    }
    auto netPolicyDialog = GetNetAccessPolicyDialog();
    if (netPolicyDialog == nullptr) {
        NETMGR_LOG_E("netPolicyDialog is nullptr");
        dlclose(handler);
        return NETMANAGER_ERR_INTERNAL;
    }

    auto ret = netPolicyDialog->ConnectSystemUi(uid);
    if (!ret) {
        NETMGR_LOG_E("netPolicyDialog ConnectSystemUi failed");
        dlclose(handler);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    NETMGR_LOG_D("NotifyNetAccessPolicyDiag success");
    dlclose(handler);

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyService::SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status)
{
    if (netPolicyRule_ == nullptr) {
        NETMGR_LOG_E("netPolicyRule_ is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    return netPolicyRule_->PolicySetNicTrafficAllowed(ifaceNames, status);
}

int32_t NetPolicyService::SetInternetAccessByIpForWifiShare(
    const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName)
{
    NETMGR_LOG_I("SetInternetAccessByIpForWifiShare addr[%{public}s], access[%{public}d]",
        NetManagerStandard::CommonUtils::ToAnonymousIp(ipAddr, true).c_str(), accessInternet);
    return NetsysController::GetInstance().SetInternetAccessByIpForWifiShare(
        ipAddr, family, accessInternet, clientNetIfName);
}

void NetPolicyService::SetBrokerUidAccessPolicyMap(std::optional<uint32_t> uid)
{
    NETMGR_LOG_I("SetBrokerUidAccessPolicyMap Enter. uid[%{public}u]", uid.has_value() ? uid.value() : 0);
    std::unordered_map<uint32_t, SampleBundleInfo> sampleBundleInfos = GetSampleBundleInfosForActiveUser();
    if (sampleBundleInfos.empty()) {
        NETMGR_LOG_W("bundleInfos is empty");
        return;
    }
    auto uidFindRet = std::find_if(sampleBundleInfos.begin(), sampleBundleInfos.end(),
                                   [uid](const auto &item) { return uid.has_value() && uid == item.second.uid_; });
    auto simFindRet = std::find_if(sampleBundleInfos.begin(), sampleBundleInfos.end(),
                                   [](const auto &item) { return CommonUtils::IsSim(item.second.bundleName_); });
    auto sim2FindRet = std::find_if(sampleBundleInfos.begin(), sampleBundleInfos.end(),
                                    [](const auto &item) { return CommonUtils::IsSim2(item.second.bundleName_); });
    NETMGR_LOG_I("SetBrokerUidAccessPolicyMap findRet[%{public}d, %{public}d], uidBundleName[%{public}s]",
                 simFindRet != sampleBundleInfos.end(), sim2FindRet != sampleBundleInfos.end(),
                 uidFindRet != sampleBundleInfos.end() ? uidFindRet->second.bundleName_.c_str() : "");
    std::unordered_map<uint32_t, uint32_t> params;
    if (simFindRet != sampleBundleInfos.end() && simFindRet->second.Valid()) {
        params.emplace(UINT16_MAX, simFindRet->second.uid_);
    }
    for (auto iter = sampleBundleInfos.begin(); iter != sampleBundleInfos.end(); iter++) {
        if (uid.has_value() && uidFindRet != sampleBundleInfos.end() &&
            !CommonUtils::IsSim(uidFindRet->second.bundleName_) &&
            !CommonUtils::IsSim2(uidFindRet->second.bundleName_) && uid.value() != iter->first) {
            continue;
        }
        if (simFindRet != sampleBundleInfos.end() && simFindRet->second.Valid() &&
            (CommonUtils::IsSim(iter->second.bundleName_) || CommonUtils::IsSimAnco(iter->second.bundleName_) ||
            iter->second.installSource_ == INSTALL_SOURCE_DEFAULT)) {
            params.emplace(iter->second.uid_, simFindRet->second.uid_);
            continue;
        }
        if (simFindRet != sampleBundleInfos.end() && simFindRet->second.Valid() &&
            (CommonUtils::IsInstallSourceFromSim(iter->second.installSource_))) {
            params.emplace(iter->second.uid_, isDisplayTrafficAncoList_ ? iter->second.uid_ : simFindRet->second.uid_);
            continue;
        }
        if (sim2FindRet != sampleBundleInfos.end() && sim2FindRet->second.Valid() &&
            (CommonUtils::IsSim2(iter->second.bundleName_) || CommonUtils::IsSim2Anco(iter->second.bundleName_) ||
            iter->second.installSource_ == INSTALL_SOURCE_DEFAULT)) {
            params.emplace(iter->second.uid_, sim2FindRet->second.uid_);
            continue;
        }
        if (sim2FindRet != sampleBundleInfos.end() && sim2FindRet->second.Valid() &&
            CommonUtils::IsInstallSourceFromSim2(iter->second.installSource_)) {
            params.emplace(iter->second.uid_, isDisplayTrafficAncoList_ ? iter->second.uid_ :
                sim2FindRet->second.uid_);
            continue;
        }
    }
    auto ret = NetsysController::GetInstance().SetBrokerUidAccessPolicyMap(params);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("SetBrokerUidAccessPolicyMap failed.");
    }
}

void NetPolicyService::DelBrokerUidAccessPolicyMap(uint32_t uid)
{
    NETMGR_LOG_I("DelBrokerUidAccessPolicyMap Enter");
    auto ret = NetsysController::GetInstance().DelBrokerUidAccessPolicyMap(uid);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("DelBrokerUidAccessPolicyMap failed.");
    }
}

std::unordered_map<uint32_t, SampleBundleInfo> NetPolicyService::GetSampleBundleInfosForActiveUser()
{
    void *handler = dlopen(LIB_NET_BUNDLE_UTILS_PATH.c_str(), RTLD_LAZY | RTLD_NODELETE);
    if (handler == nullptr) {
        NETMGR_LOG_E("load lib failed, reason : %{public}s", dlerror());
        return std::unordered_map<uint32_t, SampleBundleInfo>{};
    }
    using GetNetBundleClass = INetBundle *(*)();
    auto getNetBundle = (GetNetBundleClass)dlsym(handler, "GetNetBundle");
    if (getNetBundle == nullptr) {
        NETMGR_LOG_E("GetNetBundle failed, reason : %{public}s", dlerror());
        dlclose(handler);
        return std::unordered_map<uint32_t, SampleBundleInfo>{};
    }
    auto netBundle = getNetBundle();
    if (netBundle == nullptr) {
        NETMGR_LOG_E("netBundle is nullptr");
        dlclose(handler);
        return std::unordered_map<uint32_t, SampleBundleInfo>{};
    }
    std::optional<std::unordered_map<uint32_t, SampleBundleInfo>> result = netBundle->ObtainBundleInfoForActive();
    dlclose(handler);
    if (!result.has_value()) {
        NETMGR_LOG_W("ObtainBundleInfoForActive is nullopt");
        return std::unordered_map<uint32_t, SampleBundleInfo>{};
    }
    return result.value();
}

void NetPolicyService::UpdateNetworkAccessPolicyFromConfig(const std::string &bundleName, NetworkAccessPolicy &policy)
{
    std::vector<NetAccessPolicyConfig> configs = NetAccessPolicyConfigUtils::GetInstance().GetNetAccessPolicyConfig();
    if (configs.empty()) {
        NETMGR_LOG_W("net access policy configs is empty");
        return;
    }
    auto policyConfig = std::find_if(configs.begin(), configs.end(),
                                     [&bundleName](const auto &item) { return item.bundleName == bundleName; });
    if (policyConfig == configs.end()) {
        return;
    }
    policy.wifiSwitchDisable = policyConfig->disableWlanSwitch;
    policy.cellularSwitchDisable = policyConfig->disableCellularSwitch;
}

int32_t NetPolicyService::OnExtension(const std::string& extension, MessageParcel& data, MessageParcel& reply)
{
    NETMGR_LOG_E("extension is %{public}s.", extension.c_str());
    if (extension == EXTENSION_BACKUP) {
        return OnBackup(data, reply);
    } else if (extension == EXTENSION_RESTORE) {
        return OnRestore(data, reply);
    }
    return 0;
}

int32_t NetPolicyService::OnBackup(MessageParcel& data, MessageParcel& reply)
{
    NETMGR_LOG_I("OnBackup start");
    UniqueFd fd(-1);
    std::string replyCode = EXTENSION_SUCCESS;
    std::string tmp = "";
    int ret = NetPolicyDBClone::GetInstance().OnBackup(fd, tmp);
    if (ret < 0) {
        NETMGR_LOG_E("OnBackup fail: backup data fail!");
        replyCode = EXTENSION_FAIL;
    }
    if (reply.WriteFileDescriptor(fd) == false || reply.WriteString(replyCode) == false) {
        close(fd.Release());
        CommonUtils::DeleteFile(POLICY_DATABASE_BACKUP_FILE);
        NETMGR_LOG_E("OnBackup fail: reply write fail!");
        return -1;
    }
    close(fd.Release());
    CommonUtils::DeleteFile(POLICY_DATABASE_BACKUP_FILE);
    return 0;
}

int32_t NetPolicyService::OnRestore(MessageParcel& data, MessageParcel& reply)
{
    UniqueFd fd(data.ReadFileDescriptor());

    std::string replyCode = EXTENSION_SUCCESS;
    std::string tmp = "";
    int ret = NetPolicyDBClone::GetInstance().OnRestore(fd, tmp);
    if (ret < 0) {
        NETMGR_LOG_E("OnRestore fail: restore data fail! ret:%{public}d", ret);
        replyCode = EXTENSION_FAIL;
        return -1;
    }
    if (reply.WriteString(replyCode) == false) {
        close(fd.Release());
        CommonUtils::DeleteFile(POLICY_DATABASE_BACKUP_FILE);
        NETMGR_LOG_E("OnRestore fail: reply write fail!");
        return -1;
    }
    close(fd.Release());
    CommonUtils::DeleteFile(POLICY_DATABASE_BACKUP_FILE);
    return 0;
}

int32_t NetPolicyService::OnRestoreSingleApp(const std::string &bundleName)
{
    int ret = NetPolicyDBClone::GetInstance().OnRestoreSingleApp(bundleName);
    NETMGR_LOG_I("OnRestoreSingleApp ret:%{public}d", ret);
    return ret;
}
} // namespace NetManagerStandard
} // namespace OHOS

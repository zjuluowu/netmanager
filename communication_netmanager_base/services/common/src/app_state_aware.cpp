/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "app_state_aware.h"

#include "app_mgr_constants.h"
#include "iservice_registry.h"
#include <ability_manager_client.h>
#include "system_ability_definition.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
std::mutex AppStateAwareManager::instanceMutex_;
AppStateAwareManager::AppStateAwareManager()
{
    SubscribeAppState();
}

AppStateAwareManager::~AppStateAwareManager()
{
    UnSubscribeAppState();
}

AppStateAwareManager &AppStateAwareManager::GetInstance()
{
    std::lock_guard<std::mutex> lock(instanceMutex_);
    static AppStateAwareManager gAppStateAwareManager;
    if (!gAppStateAwareManager.appStateObserver_) {
        if (gAppStateAwareManager.retryCount_ < MAX_RETRY_COUNT) {
            gAppStateAwareManager.SubscribeAppState();
        }
    }
    return gAppStateAwareManager;
}

bool AppStateAwareManager::SubscribeAppState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    retryCount_++;
    if (appStateObserver_) {
        NETMGR_LOG_I("SubscribeAppState: appStateObserver_ has register");
        return false;
    }
    sptr<AppExecFwk::IAppMgr> appMgrProxy = GetAppMgr();
    // LCOV_EXCL_START
    if (!appMgrProxy) {
        return false;
    }
    appStateObserver_ = sptr<AppStateObserver>::MakeSptr();
    auto err = appMgrProxy->RegisterApplicationStateObserver(appStateObserver_);
    if (err != 0) {
        NETMGR_LOG_I("SubscribeAppState error, code = %{public}d", err);
        appStateObserver_ = nullptr;
        return false;
    }
    // LCOV_EXCL_STOP
    return true;
}

void AppStateAwareManager::UnSubscribeAppState()
{
    NETMGR_LOG_I("UnSubscribeAppState start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!appStateObserver_) {
        NETMGR_LOG_I("UnSubscribeAppState: appStateObserver_ is nullptr");
        return;
    }
    sptr<AppExecFwk::IAppMgr> appMgrProxy = GetAppMgr();
    // LCOV_EXCL_START
    if (appMgrProxy) {
        appMgrProxy->UnregisterApplicationStateObserver(appStateObserver_);
        appStateObserver_ = nullptr;
        retryCount_ = 0;
    }
    // LCOV_EXCL_STOP
    NETMGR_LOG_I("UnSubscribeAppState end");
}

void AppStateAwareManager::RegisterAppStateAwareCallback(const AppStateAwareCallback &appStateAwareCallback)
{
    appStateAwareCallback_ = appStateAwareCallback;
}

sptr<AppExecFwk::IAppMgr> AppStateAwareManager::GetAppMgr()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    // LCOV_EXCL_START
    if (systemAbilityManager == nullptr) {
        NETMGR_LOG_I("get SystemAbilityManager failed");
        return nullptr;
    }
    // LCOV_EXCL_STOP

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(APP_MGR_SERVICE_ID);
    if (remoteObject == nullptr) {
        NETMGR_LOG_I("get App Manager Service failed");
        return nullptr;
    }
    return iface_cast<AppExecFwk::IAppMgr>(remoteObject);
}

bool AppStateAwareManager::IsForegroundApp(const uint32_t uid)
{
    uint32_t foregroundAppUid = static_cast<uint32_t>(foregroundAppUid_.load());
    return foregroundAppUid == uid;
}

// LCOV_EXCL_START
void AppStateAwareManager::OnForegroundApplicationChanged(
    const AppExecFwk::AppStateData &appStateData)
{
    int32_t state = appStateData.state;
    auto appState = static_cast<AppExecFwk::ApplicationState>(state);
    
    if (appState == AppExecFwk::ApplicationState::APP_STATE_FOREGROUND
        && foregroundAppUid_ != appStateData.uid) {
        foregroundAppUid_ = appStateData.uid;
        if (appStateAwareCallback_.OnForegroundAppChanged != nullptr) {
            appStateAwareCallback_.OnForegroundAppChanged(foregroundAppUid_);
        }
    }
}

void AppStateObserver::OnForegroundApplicationChanged(
    const AppExecFwk::AppStateData &appStateData)
{
    NETMGR_LOG_I("%{public}s bundleName: %{public}s, uid: %{public}d, state: %{public}d, isFocused: %{public}d",
        __func__, appStateData.bundleName.c_str(), appStateData.uid, appStateData.state, appStateData.isFocused);
    AppStateAwareManager::GetInstance().OnForegroundApplicationChanged(appStateData);
}
// LCOV_EXCL_STOP

} // namespace NetManagerStandard
} // namespace OHOS

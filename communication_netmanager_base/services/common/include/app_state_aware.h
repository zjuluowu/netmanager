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

#ifndef APP_STATE_AWARE_H
#define APP_STATE_AWARE_H

#include "appmgr/app_mgr_interface.h"
#include "appmgr/app_state_data.h"
#include "iremote_object.h"
#include "appmgr/application_state_observer_stub.h"

namespace OHOS {
namespace NetManagerStandard {

constexpr uint32_t MAX_RETRY_COUNT = 5;

struct AppStateAwareCallback {
    std::function<void(const uint32_t uid)> OnForegroundAppChanged;
};

class AppStateObserver : public AppExecFwk::ApplicationStateObserverStub {
public:
    void OnForegroundApplicationChanged(const AppExecFwk::AppStateData &appStateData) override;
};

class AppStateAwareManager {
public:
    explicit AppStateAwareManager();
    ~AppStateAwareManager();
    static AppStateAwareManager &GetInstance();
    sptr<AppExecFwk::IAppMgr> GetAppMgr();
    void RegisterAppStateAwareCallback(const AppStateAwareCallback &appStateAwareCallback);
    void RegisterAppStateObserver();
    bool SubscribeAppState();
    void UnSubscribeAppState();
    void OnForegroundApplicationChanged(const AppExecFwk::AppStateData &appStateData);
    bool IsForegroundApp(const uint32_t uid);
private:
    std::atomic<int32_t> foregroundAppUid_;
    std::mutex mutex_ {};
    sptr<AppStateObserver> appStateObserver_ = nullptr;
    AppStateAwareCallback appStateAwareCallback_;
    uint32_t retryCount_ = 0;

    static std::mutex instanceMutex_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif

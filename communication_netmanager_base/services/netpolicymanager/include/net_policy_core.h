/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef NET_POLICY_CORE_H
#define NET_POLICY_CORE_H

#include <string>
#include <vector>
#include <mutex>

#include "app_mgr_client.h"
#include "app_state_callback_host.h"
#include "application_state_observer_stub.h"
#include "event_handler.h"

#include "net_manager_center.h"
#include "net_policy_callback.h"
#include "net_policy_file.h"
#include "net_policy_inner_define.h"
#include "netsys_policy_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyEventHandler;
class NetPolicyBase;

struct PolicyEvent {
    int32_t eventId = 0;
    std::shared_ptr<NetPolicyBase> sender = nullptr;
    bool deviceIdleMode = false;
    std::set<uint32_t> deviceIdleList;
    bool powerSaveMode = false;
    std::set<uint32_t> powerSaveList;
    uint32_t deletedUid = 0;
    uint32_t uid = 0;
};

class NetPolicyCore : public std::enable_shared_from_this<NetPolicyCore> {
    DECLARE_DELAYED_SINGLETON(NetPolicyCore);

public:
    template <typename NetPolicyBase> std::shared_ptr<NetPolicyBase> CreateCore()
    {
        std::shared_ptr<NetPolicyBase> core = std::make_shared<NetPolicyBase>();
        core->Init();
        std::unique_lock<std::shared_mutex> lock(coreMutex_);
        cores_.push_back(core);
        return core;
    }
    void Init(std::shared_ptr<NetPolicyEventHandler> &handler);

    /**
     * Handle the event from NetPolicyCore
     *
     * @param eventId The event id
     * @param eventData The event data ptr
     */
    void HandleEvent(int32_t eventId, std::shared_ptr<PolicyEvent> eventData);

    /**
     * Send events to other policy cores.
     *
     * @param eventId The event id
     * @param eventData The event data
     * @param delayTime The delay time, if need the message send delay
     */
    void SendEvent(int32_t eventId, std::shared_ptr<PolicyEvent> &eventData, int64_t delayTime = 0);

private:
    void SubscribeCommonEvent();
    void SendAppStatusMessage(const AppExecFwk::AppProcessData &appProcessData);

private:
    class AppStatus : public AppExecFwk::AppStateCallbackHost {
    public:
        AppStatus(std::shared_ptr<NetPolicyCore> core)
        {
            appStatus_ = core;
        }

        inline void OnAppStateChanged(const AppExecFwk::AppProcessData &appProcessData) override
        {
            if (appStatus_ != nullptr) {
                appStatus_->SendAppStatusMessage(appProcessData);
            }
        }

        inline void OnAbilityRequestDone(const sptr<IRemoteObject> &token,
                                         const AppExecFwk::AbilityState state) override
        {
            return;
        }

    private:
        std::shared_ptr<NetPolicyCore> appStatus_ = nullptr;
    };

    class ReceiveMessage : public EventFwk::CommonEventSubscriber {
    public:
        ReceiveMessage(const EventFwk::CommonEventSubscribeInfo &subscriberInfo, std::shared_ptr<NetPolicyCore> core);

        virtual void OnReceiveEvent(const EventFwk::CommonEventData &eventData) override;

    private:
        std::shared_ptr<NetPolicyCore> receiveMessage_ = nullptr;
    };

private:
    std::vector<std::shared_ptr<NetPolicyBase>> cores_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<NetPolicyEventHandler> handler_;
    sptr<AppExecFwk::IAppStateCallback> netAppStatusCallback_ = nullptr;
    std::shared_ptr<ReceiveMessage> subscriber_ = nullptr;
    std::shared_mutex coreMutex_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_CORE_H
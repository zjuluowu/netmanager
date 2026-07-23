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

#include "net_policy_core.h"

#include <pthread.h>
#include <thread>

#include "net_mgr_log_wrapper.h"
#include "net_policy_base.h"
#include "net_policy_event_handler.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace AppExecFwk;
namespace {
constexpr const char *DEVICE_IDLE_MODE_KEY = "sleeping";
constexpr uint32_t AGAIN_REGISTER_CALLBACK_INTERVAL = 500;
constexpr uint32_t CORE_EVENT_PRIORITY = 1;
constexpr uint32_t MAX_RETRY_TIMES = 10;
} // namespace

NetPolicyCore::NetPolicyCore() = default;

NetPolicyCore::~NetPolicyCore()
{
    cores_.clear();
}

void NetPolicyCore::Init(std::shared_ptr<NetPolicyEventHandler> &handler)
{
    handler_ = handler;
    SubscribeCommonEvent();

    netAppStatusCallback_ = new (std::nothrow) AppStatus((std::static_pointer_cast<NetPolicyCore>(shared_from_this())));
    if (netAppStatusCallback_ == nullptr) {
        NETMGR_LOG_E("netAppStatusCallback is nullptr.");
        return;
    }
    std::thread t([netPolicyCore = shared_from_this()]() {
        auto appManager = std::make_unique<AppMgrClient>();
        uint32_t count = 0;
        int32_t connectResult = AppMgrResultCode::ERROR_SERVICE_NOT_READY;
        while (connectResult != AppMgrResultCode::RESULT_OK && count <= MAX_RETRY_TIMES) {
            std::this_thread::sleep_for(std::chrono::milliseconds(AGAIN_REGISTER_CALLBACK_INTERVAL));
            connectResult = appManager->ConnectAppMgrService();
            count++;
        }
        if (count > MAX_RETRY_TIMES && connectResult != AppMgrResultCode::RESULT_OK) {
            NETMGR_LOG_E("Connect AppMgrService fail.");
        } else {
            appManager->RegisterAppStateCallback(netPolicyCore->netAppStatusCallback_);
        }
    });

    std::string threadName = "NetPolicyInit";
    pthread_setname_np(t.native_handle(), threadName.c_str());
    t.detach();
}

void NetPolicyCore::HandleEvent(int32_t eventId, std::shared_ptr<PolicyEvent> eventData)
{
    std::shared_lock<std::shared_mutex> lock(coreMutex_);
    for (const auto &core : cores_) {
        if (eventData && core && core != eventData->sender) {
            core->HandleEvent(eventId, eventData);
        }
    }
}

void NetPolicyCore::SendEvent(int32_t eventId, std::shared_ptr<PolicyEvent> &eventData, int64_t delayTime)
{
    NETMGR_LOG_D("NetPolicyCore SendEvent: eventId[%{public}d]", eventId);
    auto event = AppExecFwk::InnerEvent::Get(eventId, eventData);
    if (handler_ == nullptr) {
        NETMGR_LOG_E("handler is null");
        return;
    }

    handler_->SendEvent(event, delayTime);
}

void NetPolicyCore::SubscribeCommonEvent()
{
    NETMGR_LOG_D("SubscribeCommonEvent");
    std::thread t([netPolicyCore = shared_from_this()]() {
        EventFwk::MatchingSkills matchingSkills;
        matchingSkills.AddEvent(COMMON_EVENT_POWER_SAVE_MODE_CHANGED);
        matchingSkills.AddEvent(COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED);
        matchingSkills.AddEvent(COMMON_EVENT_PACKAGE_REMOVED);
        EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
        subscribeInfo.SetPriority(CORE_EVENT_PRIORITY);
        netPolicyCore->subscriber_ = std::make_shared<ReceiveMessage>(subscribeInfo, netPolicyCore);
        uint32_t count = 0;
        bool result = false;
        while (!result && count <= MAX_RETRY_TIMES) {
            std::this_thread::sleep_for(std::chrono::milliseconds(AGAIN_REGISTER_CALLBACK_INTERVAL));
            result = EventFwk::CommonEventManager::SubscribeCommonEvent(netPolicyCore->subscriber_);
            count++;
        }
        if (count > MAX_RETRY_TIMES || !result) {
            NETMGR_LOG_E("SubscribeCommonEvent fail.");
        } else {
            NETMGR_LOG_D("SubscribeCommonEvent successful");
        }
    });
    std::string threadName = "PolicyEvent";
    pthread_setname_np(t.native_handle(), threadName.c_str());
    t.detach();
}

void NetPolicyCore::ReceiveMessage::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    if (receiveMessage_ == nullptr) {
        NETMGR_LOG_E("receive message is nullptr");
        return;
    }
    const auto &action = eventData.GetWant().GetAction();
    const auto &data = eventData.GetData();
    const auto &code = eventData.GetCode();
    if (action == COMMON_EVENT_POWER_SAVE_MODE_CHANGED) {
#ifdef NETMGR_POWER_SAVE_ENABLE
        bool isPowerSave = (code == SAVE_MODE || code == LOWPOWER_MODE);
        auto policyEvent = std::make_shared<PolicyEvent>();
        policyEvent->powerSaveMode = isPowerSave;
        receiveMessage_->SendEvent(NetPolicyEventHandler::MSG_POWER_SAVE_MODE_CHANGED, policyEvent);
#endif
        return;
    }

    if (action == COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED) {
        bool isDeviceIdle = eventData.GetWant().GetBoolParam(DEVICE_IDLE_MODE_KEY, false);
        auto policyEvent = std::make_shared<PolicyEvent>();
        policyEvent->deviceIdleMode = isDeviceIdle;
        receiveMessage_->SendEvent(NetPolicyEventHandler::MSG_DEVICE_IDLE_MODE_CHANGED, policyEvent);
        return;
    }

    if (action == COMMON_EVENT_PACKAGE_REMOVED) {
        if (eventData.GetWant().GetIntParam(AppExecFwk::Constants::UID, 0) < 0) {
            NETMGR_LOG_E("error:deletedUid < 0!,return");
            return;
        }
        uint32_t deletedUid = static_cast<uint32_t>(eventData.GetWant().GetIntParam(AppExecFwk::Constants::UID, 0));
        auto policyEvent = std::make_shared<PolicyEvent>();
        policyEvent->deletedUid = deletedUid;
        receiveMessage_->SendEvent(NetPolicyEventHandler::MSG_UID_REMOVED, policyEvent);
        return;
    }
    NETMGR_LOG_E("Unknow action:[%{public}s], data:[%{public}s], code:[%{public}d]", action.c_str(), data.c_str(),
                 code);
}

void NetPolicyCore::SendAppStatusMessage(const AppProcessData &appProcessData)
{
    for (const auto &appdata : appProcessData.appDatas) {
        auto policyEvent = std::make_shared<PolicyEvent>();
        NETMGR_LOG_D(
            "SendAppStatusMessage : appProcessData.appState[%{public}d] appProcessName[%{public}s] uid[%{public}d]",
            appProcessData.appState, appProcessData.processName.c_str(), appdata.uid);
        policyEvent->uid = appdata.uid;
        if (appProcessData.appState == ApplicationState::APP_STATE_FOREGROUND) {
            SendEvent(NetPolicyEventHandler::MSG_UID_STATE_FOREGROUND, policyEvent);
        }

        if (appProcessData.appState == ApplicationState::APP_STATE_BACKGROUND) {
            SendEvent(NetPolicyEventHandler::MSG_UID_STATE_BACKGROUND, policyEvent);
        }
    }
}

NetPolicyCore::ReceiveMessage::ReceiveMessage(const EventFwk::CommonEventSubscribeInfo &subscriberInfo,
                                              std::shared_ptr<NetPolicyCore> core)
    : EventFwk::CommonEventSubscriber(subscriberInfo), receiveMessage_(core)
{
}
} // namespace NetManagerStandard
} // namespace OHOS

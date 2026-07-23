/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "net_policy_event_handler.h"

#include "net_mgr_log_wrapper.h"
#include "net_policy_core.h"

namespace OHOS {
namespace NetManagerStandard {
NetPolicyEventHandler::NetPolicyEventHandler(const std::shared_ptr<NetPolicyCore> &core, ffrt::queue& ffrtQueue)
    : core_(core), ffrtQueue_(ffrtQueue)
{
}

void NetPolicyEventHandler::ProcessEvent(int32_t eventId, std::shared_ptr<PolicyEvent> eventData)
{
    if (core_ == nullptr) {
        NETMGR_LOG_E("Net policy core is null.");
        return;
    }
    core_->HandleEvent(eventId, eventData);
}

void NetPolicyEventHandler::SendEvent(const AppExecFwk::InnerEvent::Pointer &event, int64_t delayTime)
{
    auto eventId = static_cast<int32_t>(event->GetInnerEventId());
    auto eventData = event->GetSharedObject<PolicyEvent>();
    std::weak_ptr<NetPolicyEventHandler> wp = shared_from_this();
#ifndef UNITTEST_FORBID_FFRT
    ffrtQueue_.submit([wp, eventId, eventData] {
#endif
        auto sharedSelf = wp.lock();
        // LCOV_EXCL_START
        if (sharedSelf == nullptr) {
            return;
        }
        // LCOV_EXCL_STOP
        sharedSelf->ProcessEvent(eventId, eventData);
#ifndef UNITTEST_FORBID_FFRT
    }, ffrt::task_attr().delay(static_cast<uint64_t>(delayTime)).name("FfrtSendEvent"));
#endif
}
} // namespace NetManagerStandard
} // namespace OHOS

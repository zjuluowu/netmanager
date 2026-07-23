/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef NET_POLICY_LISTENER_H
#define NET_POLICY_LISTENER_H

#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "net_policy_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *COMMON_EVENT_STATUS_CHANGED = "usual.event.RGM_STATUS_CHANGED";
}
class NetPolicyListener : public EventFwk::CommonEventSubscriber {
public:
    explicit NetPolicyListener(const EventFwk::CommonEventSubscribeInfo &sp,
                               std::shared_ptr<NetPolicyService> NetPolicy);
    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

private:
    std::shared_ptr<NetPolicyService> netPolicyService_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_LISTENER_H
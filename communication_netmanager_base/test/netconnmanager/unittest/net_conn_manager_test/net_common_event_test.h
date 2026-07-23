/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef NET_COMMON_EVENT_TEST_H
#define NET_COMMON_EVENT_TEST_H

#include <condition_variable>
#include <mutex>

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"

namespace OHOS {
namespace NetManagerStandard {
class NetCommonEventTest : public EventFwk::CommonEventSubscriber {
public:
    explicit NetCommonEventTest(const EventFwk::CommonEventSubscribeInfo &sp);
    ~NetCommonEventTest();

    void OnReceiveEvent(const EventFwk::CommonEventData &eventData) override;

private:
    std::mutex mutex_;
    std::condition_variable cv_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_COMMON_EVENT_TEST_H

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

#include "net_common_event_test.h"

namespace OHOS {
namespace NetManagerStandard {
NetCommonEventTest::NetCommonEventTest(const EventFwk::CommonEventSubscribeInfo &sp) : CommonEventSubscriber(sp) {}

NetCommonEventTest::~NetCommonEventTest()
{
    std::cout << "~NetCommonEventTest" << std::endl;
}

void NetCommonEventTest::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    const auto &action = eventData.GetWant().GetAction();
    const auto &data = eventData.GetData();
    const auto &code = eventData.GetCode();
    std::cout << "OnReceiveEvent action:" << action << " , data:" << data << ", code:" << code << std::endl;
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_HTTP_PROXY_CHANGE) {
        std::string param = eventData.GetWant().GetStringParam("HttpProxy");
        std::cout << "Received global httpProxy changed,http proxy info:" << param << std::endl;
    }
}
} // namespace NetManagerStandard
} // namespace OHOS

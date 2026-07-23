/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DNS_QUALITY_EVENT_HANDLER_H
#define DNS_QUALITY_EVENT_HANDLER_H

#include <iostream>

#include "event_handler.h"
#include "event_runner.h"
#include "singleton.h"

namespace OHOS::nmd {
class DnsQualityEventHandler : public AppExecFwk::EventHandler {
public:
    static constexpr int32_t MSG_DNS_MONITOR_LOOP = 1;
    static constexpr int32_t MSG_DNS_QUERY_FAIL = 2;
    static constexpr int32_t MSG_DNS_REPORT_LOOP = 3;
    static constexpr int32_t MSG_DNS_NEW_REPORT = 4;
    static constexpr int32_t MSG_DNS_QUERY_RESULT_REPORT = 5;
    static constexpr int32_t MSG_DNS_QUERY_ABNORMAL_REPORT = 6;

    DnsQualityEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    virtual ~DnsQualityEventHandler() override;

    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
};
} // namespace OHOS::nmd
#endif // DNS_QUALITY_EVENT_HANDLER_H

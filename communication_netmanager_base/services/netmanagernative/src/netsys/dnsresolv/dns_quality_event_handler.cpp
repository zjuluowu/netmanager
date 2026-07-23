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

#include "dns_quality_event_handler.h"
#include "dns_quality_diag.h"

namespace OHOS::nmd {
DnsQualityEventHandler::DnsQualityEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : EventHandler(runner)
{
}

DnsQualityEventHandler::~DnsQualityEventHandler() = default;

void DnsQualityEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        NETNATIVE_LOGE("DnsQualityEventHandler::ProcessEvent::parameter error");
        return;
    }
    DnsQualityDiag::GetInstance().HandleEvent(event);
}
} // namespace OHOS::nmd

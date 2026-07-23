/*
 * Copyright (C) 2025-2025 Huawei Device Co., Ltd.
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

#include "system_timer.h"
#include "common_timer_errors.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {

NetmanagerSysTimer::NetmanagerSysTimer() {}

NetmanagerSysTimer::~NetmanagerSysTimer() {}

NetmanagerSysTimer::NetmanagerSysTimer(bool repeat, uint64_t interval, bool isExact)
{
    this->repeat = repeat;
    this->interval = interval;
    this->type = TIMER_TYPE_REALTIME + TIMER_TYPE_WAKEUP;
    if (isExact) {
        this->type =
            static_cast<int32_t>(static_cast<uint32_t>(TIMER_TYPE_WAKEUP) | static_cast<uint32_t>(TIMER_TYPE_EXACT));
    }
}

void NetmanagerSysTimer::OnTrigger()
{
    if (callBack_ != nullptr) {
        callBack_();
    }
}

void NetmanagerSysTimer::SetCallbackInfo(const std::function<void()> &callBack)
{
    this->callBack_ = callBack;
}

void NetmanagerSysTimer::SetType(const int &type)
{
    this->type = type;
}

void NetmanagerSysTimer::SetRepeat(bool repeat)
{
    this->repeat = repeat;
}

void NetmanagerSysTimer::SetInterval(const uint64_t &interval)
{
    this->interval = interval;
}

void NetmanagerSysTimer::SetWantAgent(std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> wantAgent)
{
    this->wantAgent = wantAgent;
}
} // namespace NetManagerStandard
} // namespace OHOS
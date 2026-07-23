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

#ifndef NET_STATS_SYSTEM_TIMER_H
#define NET_STATS_SYSTEM_TIMER_H

#include "itimer_info.h"
#include "time_service_client.h"
#include "timer.h"
#include <cstdint>
#include <functional>
#include <string>
#include <sys/time.h>
#include <vector>

namespace OHOS {
namespace NetManagerStandard {
class NetmanagerSysTimer : public MiscServices::ITimerInfo {
public:
    NetmanagerSysTimer();
    NetmanagerSysTimer(bool repeat, uint64_t interval, bool isExact = false);
    virtual ~NetmanagerSysTimer();
    void OnTrigger() override;
    void SetType(const int &type) override;
    void SetRepeat(bool repeat) override;
    void SetInterval(const uint64_t &interval) override;
    void SetWantAgent(std::shared_ptr<OHOS::AbilityRuntime::WantAgent::WantAgent> _wantAgent) override;
    void SetCallbackInfo(const std::function<void()> &callBack);
    int32_t GetTodayMidnightTimestamp(int hour, int min, int sec);

private:
    std::function<void()> callBack_ = nullptr;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_SYSTEM_TIMER_H
/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef NET_MANAGER_TIMER_H
#define NET_MANAGER_TIMER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "ffrt.h"
#include "ffrt_inner.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr const int TIMER_MAX_INTERVAL_MS = 200;
class FfrtTimer {
public:
    FfrtTimer() : stopStatus_(true), tryStopFlag_(false) {}

    FfrtTimer(const FfrtTimer &timer)
    {
        stopStatus_ = timer.stopStatus_.load();
        tryStopFlag_ = timer.tryStopFlag_.load();
    }

    ~FfrtTimer()
    {
        Stop();
        StopPro();
    }

    void StartPro(uint64_t interval, void *data, void (*taskFun)(void *))
    {
        StopPro();
        timer_ = ffrt_timer_start(ffrt_qos_default, interval, data, taskFun, true);
    }

    void StopPro()
    {
        if (timer_ != ffrt_error) {
            auto ret = ffrt_timer_stop(ffrt_qos_default, timer_);
        }
    }

    void Start(int interval, std::function<void()> taskFun)
    {
        if (stopStatus_ == false) {
            return;
        }
        stopStatus_ = false;
        std::function<void()> startTask = [this, interval, taskFun]() {
            while (!tryStopFlag_) {
                OneTiming(interval);
                if (!tryStopFlag_) {
                    taskFun();
                }
            }

            std::lock_guard<ffrt::mutex> locker(mutex_);
            stopStatus_ = true;
            timerCond_.notify_one();
        };
        ffrt::submit(std::move(startTask), {}, {}, ffrt::task_attr().name("timeStartTask"));
    }

    void Stop()
    {
        if (stopStatus_ || tryStopFlag_) {
            return;
        }
        tryStopFlag_ = true;
        std::unique_lock<ffrt::mutex> locker(mutex_);
        timerCond_.wait(locker, [this] { return stopStatus_ == true; });

        if (stopStatus_ == true) {
            tryStopFlag_ = false;
        }
    }

private:
    void OneTiming(int time)
    {
        int repeatCount = (time > TIMER_MAX_INTERVAL_MS) ? (time / TIMER_MAX_INTERVAL_MS) : 0;
        int remainTime = (time > TIMER_MAX_INTERVAL_MS) ? (time % TIMER_MAX_INTERVAL_MS) : time;
        while (!tryStopFlag_) {
            if (repeatCount > 0) {
                ffrt::this_task::sleep_for(std::chrono::milliseconds(TIMER_MAX_INTERVAL_MS));
            } else {
                if (remainTime) {
                    ffrt::this_task::sleep_for(std::chrono::milliseconds(remainTime));
                }
                break;
            }
            repeatCount--;
        }
    }

private:
    std::atomic<bool> stopStatus_;
    std::atomic<bool> tryStopFlag_;
    ffrt::mutex mutex_;
    ffrt::condition_variable timerCond_;
    ffrt_timer_t timer_ = ffrt_error;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_MANAGER_TIMER_H

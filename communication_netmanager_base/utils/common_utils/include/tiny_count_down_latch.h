/*
 * Copyright (C) 2024-2024 Huawei Device Co., Ltd.
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

#ifndef NET_MANAGER_BASE_TINY_COUNT_DOWN_LATCH_H
#define NET_MANAGER_BASE_TINY_COUNT_DOWN_LATCH_H

#include <mutex>
#include <condition_variable>
#include <chrono>

namespace OHOS {
namespace NetManagerStandard {

class TinyCountDownLatch {
public:
    explicit TinyCountDownLatch(int32_t count);
    void CountDown();
    int32_t GetCount();
    void Await();
    template<class Rep, class Period>
    bool Await(const std::chrono::duration<Rep, Period>& time)
    {
        std::unique_lock<std::mutex> lk(mtx_);
        if (count_ > 0) {
            return conditionVar_.wait_for(lk, time, [this] { return count_ == 0; });
        }
        return true;
    }

private:
    std::mutex mtx_;
    std::condition_variable conditionVar_;
    int32_t count_ = 0;
};
} // NetManagerStandard
} // OHOS
#endif
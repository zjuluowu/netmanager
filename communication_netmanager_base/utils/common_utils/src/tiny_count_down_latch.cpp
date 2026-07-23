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

#include "tiny_count_down_latch.h"

namespace OHOS {
namespace NetManagerStandard {

TinyCountDownLatch::TinyCountDownLatch(int32_t count) : count_(count) {}

void TinyCountDownLatch::CountDown()
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (count_ > 0) {
        --count_;
        if (count_ == 0) {
            conditionVar_.notify_all();
        }
    }
}

void TinyCountDownLatch::Await()
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (count_ > 0) {
        conditionVar_.wait(lock, [this] {return count_ == 0;});
    }
}

int32_t TinyCountDownLatch::GetCount()
{
    std::unique_lock<std::mutex> lock(mtx_);
    return count_;
}

} // NetManagerStandard
} // OHOS
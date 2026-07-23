/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATION_NETMANAGER_BASE_DELAYED_QUEUE_H
#define COMMUNICATION_NETMANAGER_BASE_DELAYED_QUEUE_H

#include <array>
#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

#include "ffrt_inner.h"

#ifndef CROSS_PLATFORM
#include "netnative_log_wrapper.h"
#endif

namespace OHOS::NetManagerStandard {

typedef struct {
    uint32_t index;
    uint32_t delayTime;
} elemParam;

template <typename T, size_t ARRAY_SIZE, size_t DELAYED_COUNT> class DelayedQueue {
public:
    DelayedQueue() : index_(0), needRun_(true)
    {
        pthread_ = ffrt::thread([this]() {
#ifndef CROSS_PLATFORM
            size_t allCounter = 0;
#endif
            while (needRun_) {
                {
                    std::lock_guard<ffrt::mutex> guard(mutex_);
#ifndef CROSS_PLATFORM
                    size_t counter = 0;
                    for (auto &temp : elems_) {
                        counter += temp.size();
                    }
                    if (allCounter != counter) {
                        NETNATIVE_LOGI("dns:%{public}zu", counter);
                        allCounter = counter;
                    }
#endif
                    for (const auto &elem : elems_[index_]) {
                        if (!elem) {
                            continue;
                        }
                        ExecuteElem(elem);
                    }
                    elems_[index_].clear();
                }
                if (!needRun_) {
                    break;
                }
                std::unique_lock<ffrt::mutex> needRunLock(needRunMutex_);
                needRunCondition_.wait_for(needRunLock, std::chrono::seconds(1), [this] { return !needRun_; });
                std::lock_guard<ffrt::mutex> guard(mutex_);
                index_ = (index_ + 1) % (ARRAY_SIZE + DELAYED_COUNT);
            }
        });
    }

    ~DelayedQueue()
    {
        // set needRun_ = false, and notify the thread to wake
        needRun_ = false;
        needRunCondition_.notify_all();
        if (pthread_.joinable()) {
            pthread_.join();
        }
    }

    void Put(const std::shared_ptr<T> &elem)
    {
        std::lock_guard<ffrt::mutex> guard(mutex_);
        if (indexMap_.find(elem) != indexMap_.end()) {
            uint32_t oldIndex = indexMap_[elem].index;
            if (oldIndex < elems_.size() &&
                (elems_[oldIndex].find(elem) != elems_[oldIndex].end())) {
                elems_[oldIndex].erase(elem);
            }
        }
        UpdateDelayTime(DELAYED_COUNT, elem);
    }

    void Put(const std::shared_ptr<T> &elem, uint32_t delayTime)
    {
        std::lock_guard<ffrt::mutex> guard(mutex_);
        if (delayTime == 0) {
            return;
        }
        if (indexMap_.find(elem) != indexMap_.end()) {
            uint32_t oldIndex = indexMap_[elem].index;
            if (oldIndex < elems_.size() &&
                (elems_[oldIndex].find(elem) != elems_[oldIndex].end())) {
                elems_[oldIndex].erase(elem);
            }
        }
        UpdateDelayTime(delayTime, elem);
    }

private:
    void UpdateDelayTime(uint32_t delayTime, const std::shared_ptr<T> &elem)
    {
        elemParam newParam;
        if (delayTime > DELAYED_COUNT) {
            newParam.index = (index_ + DELAYED_COUNT) % (ARRAY_SIZE + DELAYED_COUNT);
            newParam.delayTime = delayTime - DELAYED_COUNT;
        } else {
            newParam.index = (index_ + delayTime) % (ARRAY_SIZE + DELAYED_COUNT);
            newParam.delayTime = 0;
        }
        elems_[newParam.index].insert(elem);
        indexMap_[elem] = newParam;
    }

    void ExecuteElem(const std::shared_ptr<T> &elem)
    {
        elemParam &oldParam = indexMap_[elem];
        if (oldParam.delayTime != 0) {
            UpdateDelayTime(oldParam.delayTime, elem);
        } else {
            elem->Execute();
            uint32_t updateTime = elem->GetUpdateTime();
            if (updateTime > 0) {
                UpdateDelayTime(updateTime, elem);
            } else {
                indexMap_.erase(elem);
            }
        }
    }

    ffrt::thread pthread_;
    uint32_t index_;
    ffrt::mutex mutex_;
    std::atomic_bool needRun_;
    ffrt::condition_variable needRunCondition_;
    ffrt::mutex needRunMutex_;
    std::array<std::set<std::shared_ptr<T>, std::owner_less<std::shared_ptr<T>>>, ARRAY_SIZE + DELAYED_COUNT> elems_;
    std::map<std::shared_ptr<T>, elemParam, std::owner_less<std::shared_ptr<T>>> indexMap_;
};
} // namespace OHOS::NetManagerStandard

#endif // COMMUNICATION_NETMANAGER_BASE_DELAYED_QUEUE_H

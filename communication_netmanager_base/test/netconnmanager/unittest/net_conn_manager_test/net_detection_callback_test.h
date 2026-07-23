/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NET_DETECTION_CALLBACK_TEST_H
#define NET_DETECTION_CALLBACK_TEST_H

#include <condition_variable>
#include <mutex>

#include "net_detection_callback_stub.h"

namespace OHOS {
namespace NetManagerStandard {
class NetDetectionCallbackTest : public NetDetectionCallbackStub {
public:
    NetDetectionCallbackTest();
    ~NetDetectionCallbackTest() override;
    int32_t OnNetDetectionResultChanged(
        NetDetectionResultCode detectionResult, const std::string &urlRedirect) override;

    void WaitFor(int timeoutSecond);

    int32_t GetNetDetectionResult() const
    {
        return netDetectionResult_;
    }

    std::string GetUrlRedirect() const
    {
        return urlRedirect_;
    }

private:
    void NotifyAll();
    std::mutex callbackMutex_;
    std::condition_variable cv_;
    int32_t netDetectionResult_ = 0;
    std::string urlRedirect_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_DETECTION_CALLBACK_TEST_H

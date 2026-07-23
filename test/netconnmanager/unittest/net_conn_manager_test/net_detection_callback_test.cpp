/*
 * Copyright (C) 2021-2024 Huawei Device Co., Ltd.
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

#include "net_detection_callback_test.h"

#include <iostream>

namespace OHOS {
namespace NetManagerStandard {
NetDetectionCallbackTest::NetDetectionCallbackTest() {}

NetDetectionCallbackTest::~NetDetectionCallbackTest() {}

void NetDetectionCallbackTest::NotifyAll()
{
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    cv_.notify_all();
}

void NetDetectionCallbackTest::WaitFor(int timeoutSecond)
{
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    netDetectionResult_ = static_cast<int32_t>(NetDetectionResultCode::NET_DETECTION_FAIL);
    urlRedirect_.clear();
    cv_.wait_for(callbackLock, std::chrono::seconds(timeoutSecond));
}

int32_t NetDetectionCallbackTest::OnNetDetectionResultChanged(
    NetDetectionResultCode detectionResult, const std::string &urlRedirect)
{
    netDetectionResult_ = static_cast<int32_t>(detectionResult);
    urlRedirect_ = urlRedirect;
    switch (detectionResult) {
        case NetDetectionResultCode::NET_DETECTION_FAIL:
            std::cout << "NetDetectionResultCode::NET_DETECTION_FAIL" << std::endl;
            break;
        case NetDetectionResultCode::NET_DETECTION_SUCCESS:
            std::cout << "NetDetectionResultCode::NET_DETECTION_SUCCESS" << std::endl;
            break;
        case NetDetectionResultCode::NET_DETECTION_CAPTIVE_PORTAL:
            std::cout << "NetDetectionResultCode::NET_DETECTION_CAPTIVE_PORTAL" << std::endl;
            break;
        default:
            break;
    }
    std::cout << "TestNetConnCallback::OnNetDetectionResultChanged detectionResult:" <<
        netDetectionResult_ << " urlRedirect:" << urlRedirect_ << std::endl;
    NotifyAll();
    return 0;
}
} // namespace NetManagerStandard
} // namespace OHOS

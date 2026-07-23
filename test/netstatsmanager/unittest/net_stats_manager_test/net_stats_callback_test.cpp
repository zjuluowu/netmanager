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

#include "net_stats_callback_test.h"

#include "net_stats_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetStatsCallbackTest::NetStatsCallbackTest() {}

NetStatsCallbackTest::~NetStatsCallbackTest() {}

void NetStatsCallbackTest::NotifyAll()
{
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    cv_.notify_all();
}

void NetStatsCallbackTest::WaitFor(int32_t timeoutSecond)
{
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    cv_.wait_for(callbackLock, std::chrono::seconds(timeoutSecond));
}

int32_t NetStatsCallbackTest::NetIfaceStatsChanged(const std::string &iface)
{
    NETMGR_LOG_D("unittest NetIfaceStatsChanged, iface[%{public}s]", iface.c_str());

    iface_ = iface;
    NotifyAll();

    return 0;
}

int32_t NetStatsCallbackTest::NetUidStatsChanged(const std::string &iface, uint32_t uid)
{
    NETMGR_LOG_D("unittest NetIfaceStatsChanged, iface[%{public}s], uid[%{public}d]", iface.c_str(), uid);
    iface_ = iface;
    uid_ = uid;
    NotifyAll();
    return 0;
}
} // namespace NetManagerStandard
} // namespace OHOS

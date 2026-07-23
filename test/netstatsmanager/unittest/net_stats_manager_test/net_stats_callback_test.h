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

#ifndef NET_STATS_CALLBACK_TEST_H
#define NET_STATS_CALLBACK_TEST_H

#include <mutex>
#include <condition_variable>

#include "net_stats_callback_stub.h"
#include "net_stats_constants.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsCallbackTest : public NetStatsCallbackStub {
public:
    NetStatsCallbackTest();
    ~NetStatsCallbackTest() override;
    int32_t NetIfaceStatsChanged(const std::string &iface) override;
    int32_t NetUidStatsChanged(const std::string &iface, uint32_t uid) override;
    void WaitFor(int32_t timeoutSecond);

    std::string GetIface() const
    {
        return iface_;
    }
    uint32_t GetUid() const
    {
        return uid_;
    }

private:
    void NotifyAll();
    std::string iface_ = "eth0";
    uint32_t uid_ = 0;
    std::mutex callbackMutex_;
    std::condition_variable cv_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_CALLBACK_TEST_H

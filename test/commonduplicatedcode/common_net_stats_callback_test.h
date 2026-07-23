/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef COMMON_NET_STATS_CALLBACK_TEST_H
#define COMMON_NET_STATS_CALLBACK_TEST_H

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_callback_stub.h"
#include "net_stats_constants.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsCallbackTestCb : public NetStatsCallbackStub {
public:
    NetStatsCallbackTestCb() = default;
    ~NetStatsCallbackTestCb() override {};

    int32_t NetIfaceStatsChanged(const std::string &iface) override
    {
        NETMGR_LOG_I("NetIfaceStatsChanged iface:%{public}s", iface.c_str());
        return NETMANAGER_SUCCESS;
    }

    int32_t NetUidStatsChanged(const std::string &iface, uint32_t uid) override
    {
        NETMGR_LOG_I("NetIfaceStatsChanged iface:%{public}s uid:%{public}d", iface.c_str(), uid);
        return NETMANAGER_SUCCESS;
    }
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMON_NET_STATS_CALLBACK_TEST_H

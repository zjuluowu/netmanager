/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATION_MONTH_NETMANAGER_BASE_GET_TRAFFIC_STATS_BY_NETWORK_CONTEXT_H
#define COMMUNICATION_MONTH_NETMANAGER_BASE_GET_TRAFFIC_STATS_BY_NETWORK_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include <napi/native_api.h>

#include "base_context.h"
#include "net_stats_info.h"

namespace OHOS {
namespace NetManagerStandard {
class GetMonthTrafficStatsByNetworkContext final : public BaseContext {
public:
    GetMonthTrafficStatsByNetworkContext() = delete;
    explicit GetMonthTrafficStatsByNetworkContext(napi_env env, std::shared_ptr<EventManager>& manager);

    void SetSimId(uint32_t simId);
    void SetMonthTrafficData(uint64_t data);
    uint32_t GetSimId() const;
    uint64_t &GetMonthTrafficData();

    void ParseParams(napi_value *params, size_t paramsCount);

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);

    bool CheckNetworkParams(napi_value *params, size_t paramsCount);

private:
    uint32_t simId_ = UINT32_MAX;
    uint64_t trafficData_ = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_MONTH_NETMANAGER_BASE_GET_TRAFFIC_STATS_BY_NETWORK_CONTEXT_H
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

#include "get_month_traffic_stats_context_by_network.h"

#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {

GetMonthTrafficStatsByNetworkContext::GetMonthTrafficStatsByNetworkContext(
    napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}

void GetMonthTrafficStatsByNetworkContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("checkParamsType false");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    simId_ = NapiUtils::GetUint32FromValue(GetEnv(), params[ARG_INDEX_0]);
    SetParseOK(true);
}

bool GetMonthTrafficStatsByNetworkContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number;
    }
    return false;
}

void GetMonthTrafficStatsByNetworkContext::SetSimId(uint32_t simId)
{
    simId_ = simId;
}

void GetMonthTrafficStatsByNetworkContext::SetMonthTrafficData(uint64_t data)
{
    trafficData_ = data;
}

uint32_t GetMonthTrafficStatsByNetworkContext::GetSimId() const
{
    return simId_;
}

uint64_t &GetMonthTrafficStatsByNetworkContext::GetMonthTrafficData()
{
    return trafficData_;
}

} // namespace NetManagerStandard
} // namespace OHOS
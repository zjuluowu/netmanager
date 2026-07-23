/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef SET_TRAFFIC_PLAN_INFO_CONTEXT_H
#define SET_TRAFFIC_PLAN_INFO_CONTEXT_H

#include <cstdint>
#include <memory>

#include "base_context.h"
#include "traffic_plan_param.h"

namespace OHOS {
namespace NetManagerStandard {

class SetTrafficPlanInfoContext : public BaseContext {
public:
    SetTrafficPlanInfoContext(napi_env env, std::shared_ptr<EventManager>& manager);
    ~SetTrafficPlanInfoContext() override = default;

    void ParseParams(napi_value *params, size_t paramsCount);
    
    int32_t GetSimId() const;
    TrafficPlanParam GetParam() const;
    int64_t GetValue() const;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);

private:
    int32_t simId_ = 0;
    TrafficPlanParam param_ = TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH;
    int64_t value_ = 0;
};

} // namespace NetManagerStandard
} // namespace OHOS

#endif // SET_TRAFFIC_PLAN_INFO_CONTEXT_H

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
 
#ifndef NETMANAGER_BASE_NAPI_QUERY_TRACE_ROUTE_H
#define NETMANAGER_BASE_NAPI_QUERY_TRACE_ROUTE_H
 
#include <cstddef>
#include <napi/native_api.h>
#include "base_context.h"
#include "event_manager.h"
#include "net_trace_route_info.h"

namespace OHOS::NetManagerStandard {
class QueryTraceRouteContext : public BaseContext {
public:
    QueryTraceRouteContext() = delete;
    QueryTraceRouteContext(napi_env env, std::shared_ptr<EventManager> &manager);
    void ParseParams(napi_value *params, size_t paramsCount);
    bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount);
    int32_t Conv2TraceRouteInfo(const std::string &traceRouteInfoStr,
        std::vector<TraceRouteInfo> &traceRouteInfo, uint32_t maxJumpNumber);
    int32_t Conv2TraceRouteInfoRtt(const std::string &rttStr, std::vector<uint32_t> &rtt);

public:
    std::string dest_;
    TraceRouteOptions option_;
    std::string traceRouteInfoStr_;
    std::vector<TraceRouteInfo> traceRouteInfo_;
};

} // namespace OHOS::NetManagerStandard
#endif // NETMANAGER_BASE_NAPI_QUERY_TRACE_ROUTE_H
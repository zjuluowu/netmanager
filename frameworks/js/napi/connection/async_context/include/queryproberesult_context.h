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
 
#ifndef NETMANAGER_BASE_NAPI_QUERY_PROBE_RESULT_H
#define NETMANAGER_BASE_NAPI_QUERY_PROBE_RESULT_H
 
#include <cstddef>
#include <napi/native_api.h>
#include "base_context.h"
#include "event_manager.h"
#include "net_trace_route_info.h"

namespace OHOS::NetManagerStandard {
class QueryProbeResultContext : public BaseContext {
public:
    QueryProbeResultContext() = delete;
    QueryProbeResultContext(napi_env env, std::shared_ptr<EventManager> &manager);
    void ParseParams(napi_value *params, size_t paramsCount);
    bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount);

public:
    std::string dest_;
    int32_t duration_;
    NetConn_ProbeResultInfo probeResultInfo_;
};

} // namespace OHOS::NetManagerStandard
#endif // NETMANAGER_BASE_NAPI_QUERY_PROBE_RESULT_H
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
 
#include "querytraceroute_context.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"
#include <sstream>
#include <cstring>

#define NETCONN_MAX_STR_LEN 256

namespace OHOS {
namespace NetManagerStandard {
QueryTraceRouteContext::QueryTraceRouteContext(napi_env env,
    std::shared_ptr<EventManager> &manager) : BaseContext(env, manager)
{
    traceRouteInfoStr_ = "";
}

void QueryTraceRouteContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(GetEnv(), params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetParseOK(false);
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }
    dest_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_0]);
    if (paramsCount == PARAM_JUST_OPTIONS) {
        SetParseOK(true);
        return;
    }
    if (NapiUtils::HasNamedProperty(GetEnv(), params[ARG_INDEX_1], "maxJumpNumber")) {
        option_.maxJumpNumber_ = NapiUtils::GetInt32Property(GetEnv(),
            params[ARG_INDEX_1], "maxJumpNumber");
    }
    if (NapiUtils::HasNamedProperty(GetEnv(), params[ARG_INDEX_1], "packetsType")) {
        option_.packetsType_ = static_cast<NetConn_PacketsType>(
            NapiUtils::GetInt32Property(GetEnv(), params[ARG_INDEX_1], "packetsType"));
    }
    SetParseOK(true);
}

bool QueryTraceRouteContext::CheckParamsType(napi_env env, napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_string;
    }
    if (paramsCount == PARAM_DOUBLE_OPTIONS) {
        return NapiUtils::GetValueType(env, params[ARG_INDEX_0]) == napi_string &&
            NapiUtils::GetValueType(env, params[ARG_INDEX_1]) == napi_object;
    }
    return false;
}

std::vector<std::string> splitStr(const std::string &str, const char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int32_t QueryTraceRouteContext::Conv2TraceRouteInfoRtt(const std::string &rttStr, std::vector<uint32_t> &rtt)
{
    std::vector<std::string> tokens = splitStr(rttStr, ';');
    uint32_t tokensSize = tokens.size();
    // tokens: max min avg std
    // rtt: min avg max std
    std::string max = tokens[0];
    uint8_t maxIdx = 2;
    for (uint32_t i = 0; i < tokensSize && i < NETCONN_MAX_RTT_NUM; ++i) {
        if (i < tokensSize - maxIdx) {
            tokens[i] = tokens[i+1];
        } else if (i == tokensSize - maxIdx) {
            tokens[i] = max;
        }
        uint32_t num = 0;
        std::istringstream iss(tokens[i]);
        if (iss >> num) {
            rtt.push_back(num);
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t QueryTraceRouteContext::Conv2TraceRouteInfo(const std::string &traceRouteInfoStr,
    std::vector<TraceRouteInfo> &traceRouteInfo, uint32_t maxJumpNumber)
{
    if (traceRouteInfoStr == "") {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    // traceRouteInfo is "1 *.*.*.*;2;3;4 ..." pos is space position
    const uint32_t pos2 = 2;
    const uint32_t pos3 = 3;
    std::vector<std::string> tokens = splitStr(traceRouteInfoStr, ' ');
    uint32_t tokensSize = static_cast<uint32_t>(tokens.size());
    for (uint32_t i = 0; i < tokensSize / pos3 && i < maxJumpNumber; i++) {
        TraceRouteInfo info;
        uint32_t num = 0;
        std::istringstream iss(tokens[i * pos3]);
        if (iss >> num) {
            info.jumpNo_ = static_cast<uint8_t>(num);
        }
        info.address_ = tokens[i * pos3 + 1];
        if (Conv2TraceRouteInfoRtt(tokens[i * pos3 + pos2], info.rtt_) != NETMANAGER_SUCCESS) {
            return NETMANAGER_ERR_INTERNAL;
        }
        traceRouteInfo.push_back(info);
    }
    return NETMANAGER_SUCCESS;
}

} // namespace NetManagerStandard
} // namespace OHOS

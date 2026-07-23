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

#include "getconnectowneruid_context.h"
#include "napi_constant.h"
#include "netmanager_base_log.h"
#include "netmanager_base_permission.h"

namespace OHOS {
namespace NetManagerStandard {

constexpr uint32_t MAX_PORT = 65535;

GetConnectOwnerUidContext::GetConnectOwnerUidContext(napi_env env, std::shared_ptr<EventManager>& manager)
    : BaseContext(env, manager) {}

void GetConnectOwnerUidContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("check params type failed");
        SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        SetParseOK(false);
        return;
    }

    protocolType_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]);
    if (!ParseAddress(params[ARG_INDEX_1], localAddress_) || !ParseAddress(params[ARG_INDEX_2], remoteAddress_) ||
        (localAddress_.GetJsValueFamily() != remoteAddress_.GetJsValueFamily())) {
        NETMANAGER_BASE_LOGE("local and remote address parse failed");
        SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        SetParseOK(false);
        return;
    }

    SetParseOK(true);
}

bool GetConnectOwnerUidContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (!params || paramsCount != PARAM_TRIPLE_OPTIONS ||
        NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) != napi_number ||
        NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) != napi_object ||
        NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_2]) != napi_object) {
        return false;
    }

    return true;
}

bool GetConnectOwnerUidContext::ParseAddress(napi_value param, NetAddress &address)
{
    if (NapiUtils::HasNamedProperty(GetEnv(), param, "address")) {
        std::string addr = NapiUtils::GetStringPropertyUtf8(GetEnv(), param, "address");
        address.SetAddress(addr);
    } else {
        return false;
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), param, "family")) {
        uint32_t family = NapiUtils::GetUint32Property(GetEnv(), param, "family");
        address.SetFamilyByJsValue(family);
    }

    if (address.GetJsValueFamily() != static_cast<uint32_t>(NetAddress::Family::IPv4) &&
        address.GetJsValueFamily() != static_cast<uint32_t>(NetAddress::Family::IPv6)) {
        return false;
    }

    if (NapiUtils::HasNamedProperty(GetEnv(), param, "port")) {
        uint32_t port = NapiUtils::GetUint32Property(GetEnv(), param, "port");
        if (port > MAX_PORT) {
            return false;
        }
        address.SetPort(static_cast<uint16_t>(port));
    }

    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
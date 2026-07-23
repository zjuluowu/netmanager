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
 
#ifndef NETMANAGER_BASE_NAPI_SET_OR_GET_PAC_URL_H
#define NETMANAGER_BASE_NAPI_SET_OR_GET_PAC_URL_H
 
#include <cstddef>
 
#include <napi/native_api.h>
#include "base_context.h"
#include "event_manager.h"
#include "net_all_capabilities.h"

namespace OHOS::NetManagerStandard {
class ProxyModeContext : public BaseContext {
public:
    ProxyModeContext() = delete;
    ProxyModeContext(napi_env env, std::shared_ptr<EventManager> &manager);
    void ParseParams(napi_value *params, size_t paramsCount);
    bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount);

public:
    OHOS::NetManagerStandard::ProxyModeType mode_;
};

class FindPacFileUrlContext : public BaseContext {
public:
    FindPacFileUrlContext() = delete;
    FindPacFileUrlContext(napi_env env, std::shared_ptr<EventManager> &manager);
    void ParseParams(napi_value *params, size_t paramsCount);
    bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount);

public:
    std::string url_;
    std::string proxy_;
};

class SetPacFileUrlContext : public BaseContext {
public:
    SetPacFileUrlContext() = delete;
    SetPacFileUrlContext(napi_env env, std::shared_ptr<EventManager> &manager);
    void ParseParams(napi_value *params, size_t paramsCount);
    bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount);

public:
    std::string pacUrl_;
};

class GetPacFileUrlContext : public BaseContext {
public:
    GetPacFileUrlContext() = delete;
    GetPacFileUrlContext(napi_env env, std::shared_ptr<EventManager> &manager);
    void ParseParams(napi_value *params, size_t paramsCount);

public:
    std::string pacUrl_;
};

class SetPacUrlContext : public BaseContext {
public:
    SetPacUrlContext() = delete;
    SetPacUrlContext(napi_env env, std::shared_ptr<EventManager>& manager);
    void ParseParams(napi_value *params, size_t paramsCount);
    bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount);
public:
    std::string pacUrl_;
};
 
class GetPacUrlContext : public BaseContext {
public:
    GetPacUrlContext() = delete;
    GetPacUrlContext(napi_env env, std::shared_ptr<EventManager>& manager);
    void ParseParams(napi_value *params, size_t paramsCount);
public:
    std::string pacUrl_;
};
} // namespace OHOS::NetManagerStandard
#endif // NETMANAGER_BASE_NAPI_SET_OR_GET_PAC_URL_H
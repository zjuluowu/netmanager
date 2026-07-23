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
 
#ifndef NETMANAGER_BASE_NAPI_SET_OR_GET_NET_EXT_ATTRIBUTE_H
#define NETMANAGER_BASE_NAPI_SET_OR_GET_NET_EXT_ATTRIBUTE_H
 
#include <cstddef>
 
#include <napi/native_api.h>
#include "base_context.h"
#include "net_handle.h"
 
namespace OHOS::NetManagerStandard {
class SetNetExtAttributeContext : public BaseContext {
public:
    SetNetExtAttributeContext() = delete;
    SetNetExtAttributeContext(napi_env env, std::shared_ptr<EventManager>& manager);
    void ParseParams(napi_value *params, size_t paramsCount);
    bool CheckParamsType(napi_env env, napi_value *params, size_t paramsCount);
public:
    std::string netExtAttribute_;
    NetHandle netHandle_;
};
 
class GetNetExtAttributeContext : public BaseContext {
public:
    GetNetExtAttributeContext() = delete;
    GetNetExtAttributeContext(napi_env env, std::shared_ptr<EventManager>& manager);
    void ParseParams(napi_value *params, size_t paramsCount);
public:
    std::string netExtAttribute_;
    NetHandle netHandle_;
};
} // namespace OHOS::NetManagerStandard
#endif // NETMANAGER_BASE_NAPI_SET_OR_GET_NET_EXT_ATTRIBUTE_H

/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
 
#ifndef COMMUNICATIONNETMANAGERBASE_NET_HANDLE_INTERFACE_H
#define COMMUNICATIONNETMANAGERBASE_NET_HANDLE_INTERFACE_H
    
#include <napi/native_api.h>
 
namespace OHOS {
namespace NetManagerStandard {
class NetHandleInterface final {
public:
    static constexpr const char *PROPERTY_NET_ID = "netId";
    static constexpr const char *FUNCTION_GET_ADDRESSES_BY_NAME = "getAddressesByName";
    static constexpr const char *FUNCTION_GET_ADDRESS_BY_NAME = "getAddressByName";
    static constexpr const char *FUNCTION_GET_ADDRESSES_BY_NAME_WITH_OPTION = "getAddressesByNameWithOptions";
    static constexpr const char *FUNCTION_BIND_SOCKET = "bindSocket";
 
    static napi_value GetAddressesByName(napi_env env, napi_callback_info info);
    static napi_value GetAddressByName(napi_env env, napi_callback_info info);
    static napi_value GetAddressesByNameWithOptions(napi_env env, napi_callback_info info);
    static napi_value BindSocket(napi_env env, napi_callback_info info);
};
 
} // namespace NetManagerStandard
} // namespace OHOS
#endif /* COMMUNICATIONNETMANAGERBASE_NET_HANDLE_INTERFACE_H */
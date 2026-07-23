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

#include "net_handle_interface.h"

#include "connection_async_work.h"
#include "module_template.h"
#include "bindsocket_context.h"
#include "getaddressbyname_context.h"

namespace OHOS {
namespace NetManagerStandard {

napi_value NetHandleInterface::GetAddressesByName(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetAddressByNameContext>(
        env, info, FUNCTION_GET_ADDRESSES_BY_NAME,
        [](napi_env theEnv, napi_value thisVal, GetAddressByNameContext *context) -> bool {
            context->netId_ = NapiUtils::GetInt32Property(theEnv, thisVal, PROPERTY_NET_ID);
            return true;
        },
    ConnectionAsyncWork::NetHandleAsyncWork::ExecGetAddressesByName,
        ConnectionAsyncWork::NetHandleAsyncWork::GetAddressesByNameCallback);
}

napi_value NetHandleInterface::GetAddressByName(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetAddressByNameContext>(
        env, info, FUNCTION_GET_ADDRESSES_BY_NAME,
        [](napi_env theEnv, napi_value thisVal, GetAddressByNameContext *context) -> bool {
            context->netId_ = NapiUtils::GetInt32Property(theEnv, thisVal, PROPERTY_NET_ID);
            return true;
        },
        ConnectionAsyncWork::NetHandleAsyncWork::ExecGetAddressByName,
        ConnectionAsyncWork::NetHandleAsyncWork::GetAddressByNameCallback);
}

napi_value NetHandleInterface::GetAddressesByNameWithOptions(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<GetAddressByNameWithOptionsContext>(
        env, info, FUNCTION_GET_ADDRESSES_BY_NAME_WITH_OPTION,
        [](napi_env theEnv, napi_value thisVal, GetAddressByNameWithOptionsContext *context) -> bool {
            context->netId_ = NapiUtils::GetInt32Property(theEnv, thisVal, PROPERTY_NET_ID);
            return true;
        },
    ConnectionAsyncWork::NetHandleAsyncWork::ExecGetAddressesByNameWithOptions,
        ConnectionAsyncWork::NetHandleAsyncWork::GetAddressesByNameWithOptionsCallback);
}

napi_value NetHandleInterface::BindSocket(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<BindSocketContext>(
        env, info, FUNCTION_BIND_SOCKET,
        [](napi_env theEnv, napi_value thisVal, BindSocketContext *context) -> bool {
            context->netId_ = NapiUtils::GetInt32Property(theEnv, thisVal, PROPERTY_NET_ID);
            return true;
        },
        ConnectionAsyncWork::NetHandleAsyncWork::ExecBindSocket,
        ConnectionAsyncWork::NetHandleAsyncWork::BindSocketCallback);
}

} // namespace NetManagerStandard
} // namespace OHOS
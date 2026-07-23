/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "network_module.h"
#include "gettype_context.h"
#include "module_template.h"
#include "netmanager_base_log.h"
#include "network_async_work.h"
#include "network_exec.h"
#include "network_observer.h"
#include "subscribe_context.h"
#include "unsubscribe_context.h"

namespace OHOS::NetManagerStandard {
napi_value NetworkModule::InitNetworkModule(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_FUNCTION(FUNCTION_GET_TYPE, GetType),
        DECLARE_NAPI_FUNCTION(FUNCTION_SUBSCRIBE, Subscribe),
        DECLARE_NAPI_FUNCTION(FUNCTION_UNSUBSCRIBE, Unsubscribe),
    };
    NapiUtils::DefineProperties(env, exports, properties);
    auto manager = std::make_shared<EventManager>();
    auto observer = new NetworkObserver;
    observer->SetManager(manager);
    {
        std::lock_guard<std::shared_mutex> lock(g_observerMapMtx);
        g_observerMap[manager] = observer;
    }

    auto finalizer = [](napi_env, void *data, void *) {
        auto sharedManager = reinterpret_cast<std::shared_ptr<EventManager> *>(data);
        if (sharedManager != nullptr) {
            delete sharedManager;
        }
    };
    auto sharedManager = new (std::nothrow) std::shared_ptr<EventManager>();
    if (sharedManager == nullptr) {
        return exports;
    }
    *sharedManager = manager;
    napi_wrap(env, exports, reinterpret_cast<void *>(sharedManager), finalizer, nullptr, nullptr);
    NapiUtils::SetEnvValid(env);
    auto envWrapper = new (std::nothrow) napi_env;
    if (envWrapper == nullptr) {
        NETMANAGER_BASE_LOGE("EnvWrapper create fail!");
        return exports;
    }
    *envWrapper = env;
    napi_add_env_cleanup_hook(env, NapiUtils::HookForEnvCleanup, envWrapper);
    return exports;
}

napi_value NetworkModule::GetType(napi_env env, napi_callback_info info)
{
    NETMANAGER_BASE_LOGD("GetType is called");
    return ModuleTemplate::InterfaceWithoutManager<GetTypeContext>(
        env, info, "SystemNetworkGetType", nullptr, NetworkAsyncWork::ExecGetType, NetworkAsyncWork::GetTypeCallback);
}

napi_value NetworkModule::Subscribe(napi_env env, napi_callback_info info)
{
    NETMANAGER_BASE_LOGI("Subscribe is called");
    return ModuleTemplate::Interface<SubscribeContext>(env, info, "SystemNetworkSubscribe", nullptr,
                                                       NetworkAsyncWork::ExecSubscribe,
                                                       NetworkAsyncWork::SubscribeCallback);
}

napi_value NetworkModule::Unsubscribe(napi_env env, napi_callback_info info)
{
    NETMANAGER_BASE_LOGI("Unsubscribe is called");
    return ModuleTemplate::InterfaceSync<UnsubscribeContext>(
        env, info, "SystemNetworkUnsubscribe", nullptr, NetworkExec::ExecUnsubscribe, NetworkExec::UnsubscribeCallback);
}

NAPI_MODULE(network, NetworkModule::InitNetworkModule)
} // namespace OHOS::NetManagerStandard
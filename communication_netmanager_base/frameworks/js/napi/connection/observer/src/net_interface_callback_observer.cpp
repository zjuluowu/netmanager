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

#include "net_interface_callback_observer.h"

#include "connection_exec.h"
#include "constant.h"
#include "netinterface.h"
#include "netmanager_base_log.h"

namespace OHOS::NetManagerStandard {
int32_t NetInterfaceCallbackObserver::OnInterfaceAddressUpdated(const std::string &addr,
    const std::string &ifName, int32_t flags, int32_t scope)
{
    std::shared_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    auto netInterface = NET_INTERFACES.find(this);
    if (netInterface == NET_INTERFACES.end()) {
        NETMANAGER_BASE_LOGE("can not find netInterface key");
        return 0;
    }
    if (netInterface->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netInterface handle");
        return 0;
    }
    auto manager = netInterface->second->GetEventManager();
    if (manager == nullptr) {
        return 0;
    }
    if (!manager->HasEventListener(EVENT_IFACE_ADDRESS_UPDATED)) {
        NETMANAGER_BASE_LOGE("no %{public}s listener", EVENT_IFACE_ADDRESS_UPDATED);
        return 0;
    }

    auto handler = [addr, ifName, flags, scope, manager](napi_env env) {
        auto obj = CreateInterfaceAddressUpdateParam(env, addr, ifName, flags, scope);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(env), obj};
        manager->Emit(EVENT_IFACE_ADDRESS_UPDATED, arg);
    };
    manager->EmitByUvWithModuleId(EVENT_IFACE_ADDRESS_UPDATED, handler, netInterface->second->moduleId_);
    return 0;
}

int32_t NetInterfaceCallbackObserver::OnInterfaceAddressRemoved(const std::string &addr,
    const std::string &ifName, int32_t flags, int32_t scope)
{
    std::shared_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    auto netInterface = NET_INTERFACES.find(this);
    if (netInterface == NET_INTERFACES.end()) {
        NETMANAGER_BASE_LOGE("can not find netInterface key");
        return 0;
    }
    if (netInterface->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netInterface handle");
        return 0;
    }
    auto manager = netInterface->second->GetEventManager();
    if (manager == nullptr) {
        return 0;
    }
    if (!manager->HasEventListener(EVENT_IFACE_ADDRESS_REMOVED)) {
        NETMANAGER_BASE_LOGE("no %{public}s listener", EVENT_IFACE_ADDRESS_REMOVED);
        return 0;
    }

    auto handler = [addr, ifName, flags, scope, manager](napi_env env) {
        auto obj = CreateInterfaceAddressUpdateParam(env, addr, ifName, flags, scope);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(env), obj};
        manager->Emit(EVENT_IFACE_ADDRESS_REMOVED, arg);
    };
    manager->EmitByUvWithModuleId(EVENT_IFACE_ADDRESS_REMOVED, handler, netInterface->second->moduleId_);
    return 0;
}

int32_t NetInterfaceCallbackObserver::OnInterfaceAdded(const std::string &ifName)
{
    std::shared_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    auto netInterface = NET_INTERFACES.find(this);
    if (netInterface == NET_INTERFACES.end()) {
        NETMANAGER_BASE_LOGE("can not find netInterface key");
        return 0;
    }
    if (netInterface->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netInterface handle");
        return 0;
    }
    auto manager = netInterface->second->GetEventManager();
    if (manager == nullptr) {
        return 0;
    }
    if (!manager->HasEventListener(EVENT_IFACE_ADDED)) {
        NETMANAGER_BASE_LOGE("no %{public}s listener", EVENT_IFACE_ADDED);
        return 0;
    }

    auto handler = [ifName, manager](napi_env env) {
        auto obj = CreateInterfaceUpdateParam(env, ifName);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(env), obj};
        manager->Emit(EVENT_IFACE_ADDED, arg);
    };
    manager->EmitByUvWithModuleId(EVENT_IFACE_ADDED, handler, netInterface->second->moduleId_);
    return 0;
}

int32_t NetInterfaceCallbackObserver::OnInterfaceRemoved(const std::string &ifName)
{
    std::shared_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    auto netInterface = NET_INTERFACES.find(this);
    if (netInterface == NET_INTERFACES.end()) {
        NETMANAGER_BASE_LOGE("can not find netInterface key");
        return 0;
    }
    if (netInterface->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netInterface handle");
        return 0;
    }
    auto manager = netInterface->second->GetEventManager();
    if (manager == nullptr) {
        return 0;
    }
    if (!manager->HasEventListener(EVENT_IFACE_REMOVED)) {
        NETMANAGER_BASE_LOGE("no %{public}s listener", EVENT_IFACE_REMOVED);
        return 0;
    }

    auto handler = [ifName, manager](napi_env env) {
        auto obj = CreateInterfaceUpdateParam(env, ifName);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(env), obj};
        manager->Emit(EVENT_IFACE_REMOVED, arg);
    };
    manager->EmitByUvWithModuleId(EVENT_IFACE_REMOVED, handler, netInterface->second->moduleId_);
    return 0;
}

int32_t NetInterfaceCallbackObserver::OnInterfaceChanged(const std::string &ifName, bool up)
{
    std::shared_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    auto netInterface = NET_INTERFACES.find(this);
    if (netInterface == NET_INTERFACES.end()) {
        NETMANAGER_BASE_LOGE("can not find netInterface key");
        return 0;
    }
    if (netInterface->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netInterface handle");
        return 0;
    }
    auto manager = netInterface->second->GetEventManager();
    if (manager == nullptr) {
        return 0;
    }
    if (!manager->HasEventListener(EVENT_IFACE_CHANGED)) {
        NETMANAGER_BASE_LOGE("no %{public}s listener", EVENT_IFACE_CHANGED);
        return 0;
    }

    auto handler = [ifName, up, manager](napi_env env) {
        auto obj = CreateInterfaceChangedParam(env, ifName, up);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(env), obj};
        manager->Emit(EVENT_IFACE_CHANGED, arg);
    };
    manager->EmitByUvWithModuleId(EVENT_IFACE_CHANGED, handler, netInterface->second->moduleId_);
    return 0;
}

int32_t NetInterfaceCallbackObserver::OnInterfaceLinkStateChanged(const std::string &ifName, bool up)
{
    std::shared_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    auto netInterface = NET_INTERFACES.find(this);
    if (netInterface == NET_INTERFACES.end()) {
        NETMANAGER_BASE_LOGE("can not find netInterface key");
        return 0;
    }
    if (netInterface->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netInterface handle");
        return 0;
    }
    auto manager = netInterface->second->GetEventManager();
    if (manager == nullptr) {
        return 0;
    }
    if (!manager->HasEventListener(EVENT_IFACE_LINK_STATE_CHANGED)) {
        NETMANAGER_BASE_LOGE("no %{public}s listener", EVENT_IFACE_LINK_STATE_CHANGED);
        return 0;
    }

    auto handler = [ifName, up, manager](napi_env env) {
        auto obj = CreateInterfaceChangedParam(env, ifName, up);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(env), obj};
        manager->Emit(EVENT_IFACE_LINK_STATE_CHANGED, arg);
    };
    manager->EmitByUvWithModuleId(EVENT_IFACE_LINK_STATE_CHANGED, handler, netInterface->second->moduleId_);
    return 0;
}

int32_t NetInterfaceCallbackObserver::OnRouteChanged(bool updated, const std::string &route,
    const std::string &gateway, const std::string &ifName)
{
    std::shared_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    auto netInterface = NET_INTERFACES.find(this);
    if (netInterface == NET_INTERFACES.end()) {
        NETMANAGER_BASE_LOGE("can not find netInterface key");
        return 0;
    }
    if (netInterface->second == nullptr) {
        NETMANAGER_BASE_LOGE("can not find netInterface handle");
        return 0;
    }
    auto manager = netInterface->second->GetEventManager();
    if (manager == nullptr) {
        return 0;
    }
    if (!manager->HasEventListener(EVENT_IFACE_ROUTE_CHANGED)) {
        NETMANAGER_BASE_LOGE("no %{public}s listener", EVENT_IFACE_ROUTE_CHANGED);
        return 0;
    }

    auto handler = [updated, route, gateway, ifName, manager](napi_env env) {
        auto obj = CreateRouteChangeParam(env, updated, route, gateway, ifName);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(env), obj};
        manager->Emit(EVENT_IFACE_ROUTE_CHANGED, arg);
    };
    manager->EmitByUvWithModuleId(EVENT_IFACE_ROUTE_CHANGED, handler, netInterface->second->moduleId_);
    return 0;
}

napi_value NetInterfaceCallbackObserver::CreateInterfaceAddressUpdateParam(napi_env env,
    const std::string &addr, const std::string &ifName, int32_t flags, int32_t scope)
{
    napi_value obj = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, obj) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_INTERFACE_NAME, ifName);
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_ADDRESS, addr);
    NapiUtils::SetInt32Property(env, obj, KEY_FLAGS, flags);
    NapiUtils::SetInt32Property(env, obj, KEY_SCOPE, scope);
    return obj;
}

napi_value NetInterfaceCallbackObserver::CreateInterfaceUpdateParam(napi_env env,
    const std::string &ifName)
{
    napi_value obj = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, obj) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_INTERFACE_NAME, ifName);
    return obj;
}

napi_value NetInterfaceCallbackObserver::CreateInterfaceChangedParam(napi_env env,
    const std::string &ifName, bool up)
{
    napi_value obj = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, obj) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_INTERFACE_NAME, ifName);
    NapiUtils::SetBooleanProperty(env, obj, KEY_UP, up);
    return obj;
}

napi_value NetInterfaceCallbackObserver::CreateRouteChangeParam(napi_env env, bool updated,
    const std::string &route, const std::string &gateway, const std::string &ifName)
{
    napi_value obj = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, obj) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_INTERFACE_NAME, ifName);
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_ROUTE, route);
    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_GATE_WAY, gateway);
    NapiUtils::SetBooleanProperty(env, obj, KEY_UPDATED, updated);
    return obj;
}
} // namespace OHOS::NetManagerStandard

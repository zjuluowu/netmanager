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

#include "network_observer.h"
#include "net_conn_client.h"
#include "network_constant.h"

#include "netmanager_base_log.h"
#include "securec.h"

static constexpr const char *NETWORK_NONE = "none";

static constexpr const char *NETWORK_WIFI = "WiFi";

static std::mutex OBSERVER_MUTEX;

namespace OHOS::NetManagerStandard {
std::map<std::shared_ptr<EventManager>, sptr<NetworkObserver>> g_observerMap;
std::shared_mutex g_observerMapMtx;

static napi_value MakeNetworkResponse(napi_env env, NetworkType *data)
{
    auto deleter = [](NetworkType *t) { delete t; };
    std::unique_ptr<NetworkType, decltype(deleter)> netType(data, deleter);

    napi_value obj = NapiUtils::CreateObject(env);
    if (netType->bearerTypes.find(BEARER_WIFI) != netType->bearerTypes.end()) {
        NapiUtils::SetStringPropertyUtf8(env, obj, KEY_TYPE, NETWORK_WIFI);
        NapiUtils::SetBooleanProperty(env, obj, KEY_METERED, false);
        return obj;
    }

    if (netType->bearerTypes.find(BEARER_CELLULAR) != netType->bearerTypes.end()) {
        std::string type = "";
        int32_t ret = NetConnClient::GetInstance().GetSlotType(type);
        if (ret != NETMANAGER_SUCCESS || type.empty()) {
            type = "none";
        }
        NapiUtils::SetStringPropertyUtf8(env, obj, KEY_TYPE, type);
        NapiUtils::SetBooleanProperty(env, obj, KEY_METERED, true);
        return obj;
    }

    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_TYPE, NETWORK_NONE);
    NapiUtils::SetBooleanProperty(env, obj, KEY_METERED, false);
    return obj;
}

int32_t NetworkObserver::NetAvailable(sptr<NetHandle> &netHandle)
{
    return 0;
}

int32_t NetworkObserver::NetCapabilitiesChange(sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap)
{
    NETMANAGER_BASE_LOGI("NetworkObserver::NetCapabilitiesChange");

    std::lock_guard<std::mutex> lock(OBSERVER_MUTEX);
    if (!manager_) {
        NETMANAGER_BASE_LOGI("no event manager");
        return 0;
    }

    if (manager_->HasEventListener(EVENT_SUBSCRIBE)) {
        auto netType = new NetworkType;
        netType->bearerTypes = netAllCap->bearerTypes_;
        manager_->EmitByUv(EVENT_SUBSCRIBE, netType, CallbackTemplate<MakeNetworkResponse>);
    } else {
        NETMANAGER_BASE_LOGI("NO EVENT_SUBSCRIBE");
    }
    return 0;
}

int32_t NetworkObserver::NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info)
{
    return 0;
}

int32_t NetworkObserver::NetLost(sptr<NetHandle> &netHandle)
{
    return 0;
}

int32_t NetworkObserver::NetUnavailable()
{
    NETMANAGER_BASE_LOGI("NetworkObserver::NetUnavailable");

    std::lock_guard<std::mutex> lock(OBSERVER_MUTEX);
    if (!manager_) {
        NETMANAGER_BASE_LOGI("no event manager");
        return 0;
    }
    if (manager_->HasEventListener(EVENT_SUBSCRIBE)) {
        auto netType = new NetworkType;
        manager_->EmitByUv(EVENT_SUBSCRIBE, netType, CallbackTemplate<MakeNetworkResponse>);
    } else {
        NETMANAGER_BASE_LOGI("NO EVENT_SUBSCRIBE");
    }
    return 0;
}

int32_t NetworkObserver::NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
{
    return 0;
}

void NetworkObserver::SetManager(std::shared_ptr<EventManager>& manager)
{
    manager_ = manager;
}
} // namespace OHOS::NetManagerStandard

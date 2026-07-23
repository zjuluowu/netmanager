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

#include "network_exec.h"

#include "net_conn_client.h"
#include "net_manager_constants.h"
#include "network_constant.h"

#include "napi_utils.h"
#include "netmanager_base_log.h"
#include "network_observer.h"
#include "securec.h"

namespace OHOS::NetManagerStandard {
static constexpr const int ERROR_PARAM_NUM = 2;
static constexpr const char *ERROR_MSG = "failed";
static constexpr const char *NETWORK_NONE = "none";
static constexpr const char *NETWORK_WIFI = "WiFi";
static constexpr const uint32_t DEFAULT_TIMEOUT_MS = 1000;

static napi_value MakeNetworkResponse(napi_env env, const std::set<NetBearType> &bearerTypes)
{
    napi_value obj = NapiUtils::CreateObject(env);
    if (bearerTypes.find(BEARER_WIFI) != bearerTypes.end()) {
        NapiUtils::SetStringPropertyUtf8(env, obj, KEY_TYPE, NETWORK_WIFI);
        NapiUtils::SetBooleanProperty(env, obj, KEY_METERED, false);
        return obj;
    }

    if (bearerTypes.find(BEARER_CELLULAR) != bearerTypes.end()) {
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

bool NetworkExec::ExecGetType(GetTypeContext *context)
{
    NETMANAGER_BASE_LOGD("ExecGetType");
    NetHandle handle;
    auto ret = NetConnClient::GetInstance().GetDefaultNet(handle);
    if (ret != NETMANAGER_SUCCESS) {
        context->SetErrorCode(ret);
        return ret == NETMANAGER_SUCCESS;
    }

    if (handle.GetNetId() == 0) {
        return true;
    }

    NetAllCapabilities cap;
    ret = NetConnClient::GetInstance().GetNetCapabilities(handle, cap);
    if (ret == NETMANAGER_SUCCESS) {
        context->SetCap(cap);
    }

    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value NetworkExec::GetTypeCallback(GetTypeContext *context)
{
    if (!context->IsExecOK()) {
        napi_value fail = context->GetFailCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), fail) == napi_function) {
            napi_value argv[ERROR_PARAM_NUM] = {
                NapiUtils::CreateStringUtf8(context->GetEnv(), ERROR_MSG),
                NapiUtils::CreateInt32(context->GetEnv(), context->GetErrorCode()),
            };
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), fail,
                                    ERROR_PARAM_NUM, argv);
        }

        napi_value complete = context->GetCompleteCallback();
        // if ok complete will be called in observer
        if (NapiUtils::GetValueType(context->GetEnv(), complete) == napi_function) {
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), complete, 0,
                                    nullptr);
        }
    } else {
        napi_value success = context->GetSuccessCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), success) == napi_function) {
            auto cap = context->GetCap();
            auto obj = MakeNetworkResponse(context->GetEnv(), cap.bearerTypes_);
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), success, 1, &obj);
        }
    }

    return NapiUtils::GetUndefined(context->GetEnv());
}

bool NetworkExec::ExecSubscribe(SubscribeContext *context)
{
    NETMANAGER_BASE_LOGI("ExecSubscribe");
    auto manager = context->GetManager();

    std::shared_lock<std::shared_mutex> lock(g_observerMapMtx);
    auto iter = g_observerMap.find(manager);
    if (iter == g_observerMap.end() || iter->second == nullptr) {
        NETMANAGER_BASE_LOGE("callback is null");
        return false;
    }
    auto callback = iter->second;
    lock.unlock();
    sptr<NetSpecifier> specifier = new NetSpecifier;
    specifier->netCapabilities_.netCaps_.insert(NET_CAPABILITY_INTERNET);
    NetConnClient::GetInstance().UnregisterNetConnCallback(callback);
    int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(specifier, callback, DEFAULT_TIMEOUT_MS);

    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value NetworkExec::SubscribeCallback(SubscribeContext *context)
{
    if (!context->IsExecOK()) {
        napi_value fail = context->GetFailCallback();
        if (NapiUtils::GetValueType(context->GetEnv(), fail) == napi_function) {
            napi_value argv[ERROR_PARAM_NUM] = {
                NapiUtils::CreateStringUtf8(context->GetEnv(), ERROR_MSG),
                NapiUtils::CreateInt32(context->GetEnv(), context->GetErrorCode()),
            };
            NapiUtils::CallFunction(context->GetEnv(), NapiUtils::GetUndefined(context->GetEnv()), fail,
                                    ERROR_PARAM_NUM, argv);
        }
    }

    return NapiUtils::GetUndefined(context->GetEnv());
}

bool NetworkExec::ExecUnsubscribe(UnsubscribeContext *context)
{
    NETMANAGER_BASE_LOGI("ExecUnsubscribe");
    auto manager = context->GetManager();
    std::shared_lock<std::shared_mutex> lock(g_observerMapMtx);
    auto iter = g_observerMap.find(manager);
    if (iter == g_observerMap.end() || iter->second == nullptr) {
        NETMANAGER_BASE_LOGE("callback is null");
        return false;
    }
    auto callback = iter->second;
    lock.unlock();
    int32_t ret = NetConnClient::GetInstance().UnregisterNetConnCallback(callback);
    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value NetworkExec::UnsubscribeCallback(UnsubscribeContext *context)
{
    context->GetManager()->DeleteListener(EVENT_SUBSCRIBE);
    return NapiUtils::GetUndefined(context->GetEnv());
}
} // namespace OHOS::NetManagerStandard
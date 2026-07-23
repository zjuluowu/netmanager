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

#include "net_policy_callback_observer.h"

#include "constant.h"
#include "net_manager_constants.h"
#include "net_policy_constants.h"
#include "net_quota_policy.h"
#include "netmanager_base_log.h"
#include "netpolicy_exec.h"
#include "policy_observer_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
int32_t NetPolicyCallbackObserver::NetUidPolicyChange(uint32_t uid, uint32_t policy)
{
    if (!PolicyObserverWrapper::GetInstance().GetEventManager()->HasEventListener(EVENT_POLICY_UID_POLICY)) {
        NETMANAGER_BASE_LOGE("no event listener find %{public}s", EVENT_POLICY_UID_POLICY);
        return NETMANAGER_SUCCESS;
    }
    auto pair = new std::pair<uint32_t, uint32_t>(uid, policy);
    PolicyObserverWrapper::GetInstance().GetEventManager()->EmitByUv(EVENT_POLICY_UID_POLICY, pair,
                                                                     NetUidPolicyChangeCallback);
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallbackObserver::NetUidRuleChange(uint32_t uid, uint32_t rule)
{
    if (!PolicyObserverWrapper::GetInstance().GetEventManager()->HasEventListener(EVENT_POLICY_UID_RULE)) {
        NETMANAGER_BASE_LOGE("no event listener find %{public}s", EVENT_POLICY_UID_RULE);
        return NETMANAGER_SUCCESS;
    }
    auto pair = new std::pair<uint32_t, uint32_t>(uid, rule);
    PolicyObserverWrapper::GetInstance().GetEventManager()->EmitByUv(EVENT_POLICY_UID_RULE, pair,
                                                                     NetUidRuleChangeCallback);
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallbackObserver::NetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    if (!PolicyObserverWrapper::GetInstance().GetEventManager()->HasEventListener(EVENT_POLICY_QUOTA_POLICY)) {
        NETMANAGER_BASE_LOGE("no event listener find %{public}s", EVENT_POLICY_QUOTA_POLICY);
        return NETMANAGER_SUCCESS;
    }
    auto vec = new std::vector<NetQuotaPolicy>(quotaPolicies.size());
    vec->assign(quotaPolicies.begin(), quotaPolicies.end());
    PolicyObserverWrapper::GetInstance().GetEventManager()->EmitByUv(EVENT_POLICY_QUOTA_POLICY, vec,
                                                                     NetQuotaPolicyChangeCallback);
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallbackObserver::NetMeteredIfacesChange(std::vector<std::string> &ifaces)
{
    if (!PolicyObserverWrapper::GetInstance().GetEventManager()->HasEventListener(EVENT_POLICY_METERED_IFACES)) {
        NETMANAGER_BASE_LOGE("no event listener find %{public}s", EVENT_POLICY_METERED_IFACES);
        return NETMANAGER_SUCCESS;
    }
    auto vec = new std::vector<std::string>;
    vec->assign(ifaces.begin(), ifaces.end());
    PolicyObserverWrapper::GetInstance().GetEventManager()->EmitByUv(EVENT_POLICY_METERED_IFACES, vec,
                                                                     NetMeteredIfacesChangeCallback);
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyCallbackObserver::NetBackgroundPolicyChange(bool isBackgroundPolicyAllow)
{
    if (!PolicyObserverWrapper::GetInstance().GetEventManager()->HasEventListener(EVENT_POLICY_BACKGROUND_POLICY)) {
        NETMANAGER_BASE_LOGE("no event listener find %{public}s", EVENT_POLICY_BACKGROUND_POLICY);
        return NETMANAGER_SUCCESS;
    }
    PolicyObserverWrapper::GetInstance().GetEventManager()->EmitByUv(
        EVENT_POLICY_BACKGROUND_POLICY, new bool(isBackgroundPolicyAllow), NetBackgroundPolicyChangeCallback);
    return NETMANAGER_SUCCESS;
}

napi_value NetPolicyCallbackObserver::CreateNetUidPolicyChangeParam(napi_env env, void *data)
{
    auto pair = static_cast<std::pair<uint32_t, uint32_t> *>(data);
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetUint32Property(env, obj, KEY_UID, pair->first);
    NapiUtils::SetUint32Property(env, obj, KEY_POLICY, pair->second);
    delete pair;
    return obj;
}

napi_value NetPolicyCallbackObserver::CreateNetUidRuleChangeParam(napi_env env, void *data)
{
    auto pair = static_cast<std::pair<uint32_t, uint32_t> *>(data);
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetUint32Property(env, obj, KEY_UID, pair->first);
    NapiUtils::SetUint32Property(env, obj, KEY_RULE, pair->second);
    delete pair;
    return obj;
}

napi_value NetPolicyCallbackObserver::CreateNetQuotaPolicyChangeParam(napi_env env, void *data)
{
    auto quotaPolicies = static_cast<std::vector<NetQuotaPolicy> *>(data);
    auto arraySize = quotaPolicies->size();
    napi_value callbackValue = NapiUtils::CreateArray(env, arraySize);
    for (size_t i = 0; i < arraySize; i++) {
        const auto item = quotaPolicies->at(i);
        napi_value elementObject = NetPolicyExec::CreateNetQuotaPolicy(env, item);
        NapiUtils::SetArrayElement(env, callbackValue, i, elementObject);
    }
    delete quotaPolicies;
    return callbackValue;
}

napi_value NetPolicyCallbackObserver::CreateMeteredIfacesChangeParam(napi_env env, void *data)
{
    auto ifaces = static_cast<std::vector<std::string> *>(data);
    auto arraySize = ifaces->size();
    napi_value callbackValue = NapiUtils::CreateArray(env, arraySize);
    for (size_t i = 0; i < arraySize; i++) {
        std::string item = ifaces->at(i);
        napi_value elementObject = NapiUtils::CreateStringUtf8(env, item);
        NapiUtils::SetArrayElement(env, callbackValue, i, elementObject);
    }
    delete ifaces;
    return callbackValue;
}

napi_value NetPolicyCallbackObserver::CreateNetBackgroundPolicyChangeParam(napi_env env, void *data)
{
    auto isAllowPtr = static_cast<bool *>(data);
    if (isAllowPtr == nullptr) {
        return nullptr;
    }
    bool isAllow = *isAllowPtr;
    napi_value jsValue = NapiUtils::GetBoolean(env, isAllow);
    delete isAllowPtr;
    return jsValue;
}

void NetPolicyCallbackObserver::NetUidPolicyChangeCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetUidPolicyChangeParam>(work, status);
}

void NetPolicyCallbackObserver::NetUidRuleChangeCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetUidRuleChangeParam>(work, status);
}

void NetPolicyCallbackObserver::NetQuotaPolicyChangeCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetQuotaPolicyChangeParam>(work, status);
}

void NetPolicyCallbackObserver::NetMeteredIfacesChangeCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateMeteredIfacesChangeParam>(work, status);
}

void NetPolicyCallbackObserver::NetBackgroundPolicyChangeCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetBackgroundPolicyChangeParam>(work, status);
}
} // namespace NetManagerStandard
} // namespace OHOS

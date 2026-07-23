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

#include "netpolicy_exec.h"

#include "napi_utils.h"
#include "net_policy_client.h"
#include "net_policy_constants.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
bool NetPolicyExec::ExecSetPolicyByUid(SetPolicyByUidContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().SetPolicyByUid(context->uid_, context->policy_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetPolicyByUid error, uid = %{public}s, policy = %{public}d, result = %{public}d",
                             std::to_string(context->uid_).c_str(), context->policy_, result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetPolicyByUid(GetPolicyByUidContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().GetPolicyByUid(context->uid_, context->policy_);
    if (result != NETMANAGER_SUCCESS) {
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetUidsByPolicy(GetUidsByPolicyContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().GetUidsByPolicy(context->policy_, context->uidTogether_);
    if (result != NETMANAGER_SUCCESS) {
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecSetBackgroundPolicy(SetBackgroundPolicyContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().SetBackgroundPolicy(context->isAllowed_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetBackgroundPolicy error, isAllowed = %{public}d, result = %{public}d",
                             context->isAllowed_, result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetBackgroundPolicy(GetBackgroundPolicyContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().GetBackgroundPolicy(context->backgroundPolicy_);
    if (result != NETMANAGER_SUCCESS) {
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetNetQuotaPolicies(GetNetQuotaPoliciesContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().GetNetQuotaPolicies(context->quotaPolicys_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecGetNetQuotaPolicies error, result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecSetNetQuotaPolicies(SetNetQuotaPoliciesContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().SetNetQuotaPolicies(context->quotaPolicys_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetNetQuotaPolicies error, result = %{public}d, arr size = %{public}zu", result,
                             context->quotaPolicys_.size());
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecRestoreAllPolicies(RestoreAllPoliciesContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().ResetPolicies(context->iccid_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecRestoreAllPolicies error, result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecIsUidNetAllowed(IsUidNetAllowedContext *context)
{
    int32_t result = NETMANAGER_SUCCESS;
    if (context->isBoolean_) {
        result = NetPolicyClient::GetInstance().IsUidNetAllowed(context->uid_, context->isMetered_, context->isUidNet_);
    } else {
        result = NetPolicyClient::GetInstance().IsUidNetAllowed(context->uid_, context->iface_, context->isUidNet_);
    }

    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecIsUidNetAllowed error, result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecSetDeviceIdleTrustlist(SetDeviceIdleTrustlistContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().SetDeviceIdleTrustlist(context->uids_, context->isAllow_);
    if (result != NETMANAGER_SUCCESS) {
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetDeviceIdleTrustlist(GetDeviceIdleTrustlistContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().GetDeviceIdleTrustlist(context->uids_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecGetDeviceIdleTrustlist error: result = %{public}d, arr size = %{public}zu", result,
                             context->uids_.size());
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecSetPowerSaveTrustlist(SetPowerSaveTrustlistContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().SetPowerSaveTrustlist(context->uids_, context->isAllow_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetPowerSaveTrustlist error: result = %{public}d, arr size = %{public}zu", result,
                             context->uids_.size());
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetPowerSaveTrustlist(GetPowerSaveTrustlistContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().GetPowerSaveTrustlist(context->uids_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecGetPowerSaveTrustlist error: result = %{public}d, arr size = %{public}zu", result,
                             context->uids_.size());
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetBackgroundPolicyByUid(GetBackgroundPolicyByUidContext *context)
{
    int32_t result =
        NetPolicyClient::GetInstance().GetBackgroundPolicyByUid(context->uid_, context->backgroundPolicyOfUid_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecGetBackgroundPolicyByUid error: result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecResetPolicies(ResetPoliciesContext *context)
{
    int32_t result = NetPolicyClient::GetInstance().ResetPolicies(context->iccid_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecResetPolicies error: result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecUpdateRemindPolicy(UpdateRemindPolicyContext *context)
{
    int32_t result =
        NetPolicyClient::GetInstance().UpdateRemindPolicy(context->netType_, context->iccid_, context->remindType_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecUpdateRemindPolicy error: result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecSetNetworkAccessPolicy(SetNetworkAccessPolicyContext *context)
{
    int32_t errorCode = NetPolicyClient::GetInstance().SetNetworkAccessPolicy(context->uid_, context->policy_,
                                                                              context->isReconfirmFlag_);
    if (errorCode != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec SetNetworkAccessPolicy failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

bool NetPolicyExec::ExecGetNetworkAccessPolicy(GetNetworkAccessPolicyContext *context)
{
    int32_t errorCode =
        NetPolicyClient::GetInstance().GetNetworkAccessPolicy(context->policy_parmeter_, context->policy_save_);
    if (errorCode != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec GetNetworkAccessPolicy failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

bool NetPolicyExec::ExecGetSelfNetworkAccessPolicy(GetSelfNetworkAccessPolicyContext *context)
{
    int32_t errorCode = NetPolicyClient::GetInstance().GetSelfNetworkAccessPolicy(context->policy_);
    if (errorCode != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec GetSelfNetworkAccessPolicy failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value NetPolicyExec::SetPolicyByUidCallback(SetPolicyByUidContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::GetPolicyByUidCallback(GetPolicyByUidContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->policy_);
}

napi_value NetPolicyExec::GetUidsByPolicyCallback(GetUidsByPolicyContext *context)
{
    napi_value uids = NapiUtils::CreateArray(context->GetEnv(), context->uidTogether_.size());
    uint32_t index = 0;
    for (const auto &uid : context->uidTogether_) {
        napi_value element = NapiUtils::CreateInt32(context->GetEnv(), uid);
        NapiUtils::SetArrayElement(context->GetEnv(), uids, index++, element);
    }
    return uids;
}

napi_value NetPolicyExec::SetBackgroundPolicyCallback(SetBackgroundPolicyContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::GetBackgroundPolicyCallback(GetBackgroundPolicyContext *context)
{
    return NapiUtils::GetBoolean(context->GetEnv(), context->backgroundPolicy_);
}

napi_value NetPolicyExec::GetNetQuotaPoliciesCallback(GetNetQuotaPoliciesContext *context)
{
    napi_value callbackValue = NapiUtils::CreateArray(context->GetEnv(), context->quotaPolicys_.size());
    uint32_t index = 0;
    for (const auto &quotaPolicy : context->quotaPolicys_) {
        napi_value element = NetPolicyExec::CreateNetQuotaPolicy(context->GetEnv(), quotaPolicy);
        NapiUtils::SetArrayElement(context->GetEnv(), callbackValue, index++, element);
    }
    return callbackValue;
}

void NetPolicyExec::FillNetWorkMatchRule(napi_env env, napi_value elementObject, const NetQuotaPolicy &netQuotaPolicy)
{
    napi_value netWorkMatchRule =  NapiUtils::CreateObject(env);
    NapiUtils::SetInt32Property(env, netWorkMatchRule, "netType", netQuotaPolicy.networkmatchrule.netType);
    NapiUtils::SetStringPropertyUtf8(env, netWorkMatchRule, "simId", netQuotaPolicy.networkmatchrule.simId);
    NapiUtils::SetStringPropertyUtf8(env, netWorkMatchRule, "identity", netQuotaPolicy.networkmatchrule.ident);
    NapiUtils::SetNamedProperty(env, elementObject, "networkMatchRule", netWorkMatchRule);
}

void NetPolicyExec::FillQuotaPolicy(napi_env env, napi_value elementObject, const NetQuotaPolicy &netQuotaPolicy)
{
    napi_value quotaPolicy =  NapiUtils::CreateObject(env);
    NapiUtils::SetStringPropertyUtf8(env, quotaPolicy, "periodDuration", netQuotaPolicy.quotapolicy.periodDuration);
    NapiUtils::SetInt64Property(env, quotaPolicy, "warningBytes", netQuotaPolicy.quotapolicy.warningBytes);
    NapiUtils::SetInt64Property(env, quotaPolicy, "limitBytes", netQuotaPolicy.quotapolicy.limitBytes);
    NapiUtils::SetInt64Property(env, quotaPolicy, "lastWarningRemind", netQuotaPolicy.quotapolicy.lastWarningRemind);
    NapiUtils::SetInt64Property(env, quotaPolicy, "lastLimitRemind", netQuotaPolicy.quotapolicy.lastLimitRemind);
    NapiUtils::SetBooleanProperty(env, quotaPolicy, "metered", netQuotaPolicy.quotapolicy.metered);
    NapiUtils::SetInt32Property(env, quotaPolicy, "limitAction", netQuotaPolicy.quotapolicy.limitAction);
    NapiUtils::SetNamedProperty(env, elementObject, "quotaPolicy", quotaPolicy);
}

napi_value NetPolicyExec::CreateNetQuotaPolicy(napi_env env, const NetQuotaPolicy &item)
{
    napi_value elementObject = NapiUtils::CreateObject(env);
    FillNetWorkMatchRule(env, elementObject, item);
    FillQuotaPolicy(env, elementObject, item);
    return elementObject;
}

napi_value NetPolicyExec::SetNetQuotaPoliciesCallback(SetNetQuotaPoliciesContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::RestoreAllPoliciesCallback(RestoreAllPoliciesContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::IsUidNetAllowedCallback(IsUidNetAllowedContext *context)
{
    return NapiUtils::GetBoolean(context->GetEnv(), context->isUidNet_);
}

napi_value NetPolicyExec::SetDeviceIdleTrustlistCallback(SetDeviceIdleTrustlistContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::GetDeviceIdleTrustlistCallback(GetDeviceIdleTrustlistContext *context)
{
    napi_value list = NapiUtils::CreateArray(context->GetEnv(), context->uids_.size());
    uint32_t index = 0;
    for (const auto &uid : context->uids_) {
        napi_value element = NapiUtils::CreateUint32(context->GetEnv(), uid);
        NapiUtils::SetArrayElement(context->GetEnv(), list, index++, element);
    }
    return list;
}

napi_value NetPolicyExec::SetPowerSaveTrustlistCallback(SetPowerSaveTrustlistContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::GetPowerSaveTrustlistCallback(GetPowerSaveTrustlistContext *context)
{
    napi_value list = NapiUtils::CreateArray(context->GetEnv(), context->uids_.size());
    uint32_t index = 0;
    for (const auto &uid : context->uids_) {
        napi_value element = NapiUtils::CreateUint32(context->GetEnv(), uid);
        NapiUtils::SetArrayElement(context->GetEnv(), list, index++, element);
    }
    return list;
}

napi_value NetPolicyExec::GetBackgroundPolicyByUidCallback(GetBackgroundPolicyByUidContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), static_cast<int32_t>(context->backgroundPolicyOfUid_));
}

napi_value NetPolicyExec::ResetPoliciesCallback(ResetPoliciesContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::UpdateRemindPolicyCallback(UpdateRemindPolicyContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::SetNetworkAccessPolicyCallback(SetNetworkAccessPolicyContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::GetNetworkAccessPolicyCallback(GetNetworkAccessPolicyContext *context)
{
    if (context->policy_parmeter_.flag) {
        napi_value obj = NapiUtils::CreateObject(context->GetEnv());
        NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "allowWiFi", context->policy_save_.policy.wifiAllow);
        NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "allowCellular",
                                      context->policy_save_.policy.cellularAllow);
        if (context->policy_save_.policy.wifiSwitchDisable) {
            NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "alwaysAllowWiFi",
                                          context->policy_save_.policy.wifiSwitchDisable);
        }
        if (context->policy_save_.policy.cellularSwitchDisable) {
            NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "alwaysAllowCellular",
                                          context->policy_save_.policy.cellularSwitchDisable);
        }
        return obj;
    }

    napi_value result = NapiUtils::CreateObject(context->GetEnv());
    for (const auto &item : context->policy_save_.uid_policies) {
        napi_value obj = NapiUtils::CreateObject(context->GetEnv());
        NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "allowWiFi", item.second.wifiAllow);
        NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "allowCellular", item.second.cellularAllow);
        if (item.second.wifiSwitchDisable) {
            NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "alwaysAllowWiFi", item.second.wifiSwitchDisable);
        }
        if (item.second.cellularSwitchDisable) {
            NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "alwaysAllowCellular",
                                          item.second.cellularSwitchDisable);
        }
        NapiUtils::SetNamedProperty(context->GetEnv(), result, std::to_string(item.first).c_str(), obj);
    }
    return result;
}

napi_value NetPolicyExec::GetSelfNetworkAccessPolicyCallback(GetSelfNetworkAccessPolicyContext *context)
{
    napi_value obj = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "allowWiFi", context->policy_.allowWiFi);
    NapiUtils::SetBooleanProperty(context->GetEnv(), obj, "allowCellular", context->policy_.allowCellular);
    return obj;
}
} // namespace NetManagerStandard
} // namespace OHOS

/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "policy_ani.h"
#include "errorcode_convertor.h"
#include "net_quota_policy.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "wrapper.rs.h"
#include "ability_context.h"
#include "ui_extension_context.h"
#include "ui_content.h"
#include "ipc_skeleton.h"
#include "net_policy_callback_stub.h"
#include "net_access_policy.h"
#include "want.h"

namespace {

// Safe conversion: uint32_t -> int32_t, clamping to INT32_MAX and logging on overflow.
// UID/POLICY/RULE are uint32_t in the system service interface but the ANI bridge
// expects i32. Under normal conditions (APP UID ~2M) overflow never triggers, but we
// guard against it to avoid silently wrapping into negative values.
constexpr int32_t ToI32Saturating(uint32_t value, const char *field_name)
{
    if (value > static_cast<uint32_t>(INT32_MAX)) {
        return INT32_MAX;
    }
    return static_cast<int32_t>(value);
}

} // anonymous namespace

namespace OHOS {
namespace NetManagerAni {

// Inline definitions to avoid depending on NAPI netpolicy module
struct AppBaseContext {
    std::shared_ptr<OHOS::AbilityRuntime::AbilityContext> abilityContext = nullptr;
    std::shared_ptr<OHOS::AbilityRuntime::UIExtensionContext> uiExtensionContext = nullptr;
};

// Forward declaration needed by ModalUICallback
static OHOS::Ace::UIContent* GetUIContent(std::shared_ptr<AppBaseContext> &asyncContext);

class ModalUICallback {
public:
    explicit ModalUICallback(std::shared_ptr<AppBaseContext> baseContext) : baseContext_(baseContext) {}
    void OnRelease(int32_t releaseCode) { CloseModalUI(); }
    void SetSessionId(int32_t sessionId) { sessionId_ = sessionId; }
private:
    int32_t sessionId_ = 0;
    std::shared_ptr<AppBaseContext> baseContext_ = nullptr;
    void CloseModalUI()
    {
        auto uiContent = GetUIContent(baseContext_);
        if (uiContent != nullptr) {
            uiContent->CloseModalUIExtension(sessionId_);
        }
    }
};

static OHOS::Ace::UIContent* GetUIContent(std::shared_ptr<AppBaseContext> &asyncContext)
{
    if (!asyncContext) {
        return nullptr;
    }
    if (asyncContext->abilityContext != nullptr) {
        auto* uic = asyncContext->abilityContext->GetUIContent();
        return uic;
    }
    if (asyncContext->uiExtensionContext != nullptr) {
        auto* uic = asyncContext->uiExtensionContext->GetUIContent();
        return uic;
    }
    return nullptr;
}

static bool StartUiExtensionAbility(OHOS::AAFwk::Want &request, std::shared_ptr<AppBaseContext> &asyncContext)
{
    auto uiContent = GetUIContent(asyncContext);
    if (uiContent == nullptr) {
        return false;
    }
    auto callback = std::make_shared<ModalUICallback>(asyncContext);
    OHOS::Ace::ModalUIExtensionCallbacks extensionCallbacks = {
        std::bind(&ModalUICallback::OnRelease, callback, std::placeholders::_1)
    };
    OHOS::Ace::ModalUIExtensionConfig config;
    config.isProhibitBack = false;
    int32_t sessionId = uiContent->CreateModalUIExtension(request, extensionCallbacks, config);
    if (sessionId == 0) {
        return false;
    }
    callback->SetSessionId(sessionId);
    return true;
}

rust::String GetErrorCodeAndMessage(int32_t &errorCode)
{
    NetManagerStandard::NetBaseErrorCodeConvertor convertor;
    return rust::string(convertor.ConvertErrorCode(errorCode));
}

NetAccessPolicyInner GetSelfNetworkAccessPolicy(int32_t &ret)
{
    NetAccessPolicyInner policy = {true, true};
    NetManagerStandard::NetAccessPolicy netPolicy;
    ret = NetManagerStandard::NetPolicyClient::GetInstance().GetSelfNetworkAccessPolicy(netPolicy);
    if (ret == NetManagerStandard::NETMANAGER_SUCCESS) {
        policy.allowWiFi = netPolicy.allowWiFi;
        policy.allowCellular = netPolicy.allowCellular;
    }
    return policy;
}

// Define observer class implementing NetPolicyCallback to receive policy events
class PolicyEventCallbackObserverAni : public NetManagerStandard::NetPolicyCallbackStub {
public:
    PolicyEventCallbackObserverAni() {}
    virtual ~PolicyEventCallbackObserverAni() {}

    int32_t NetUidPolicyChange(uint32_t uid, uint32_t policy) override
    {
        NetUidPolicyInfo info{.uid = ToI32Saturating(uid, "uid"), .policy = ToI32Saturating(policy, "policy")};
        execute_net_uid_policy_changed(info);
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

    int32_t NetUidRuleChange(uint32_t uid, uint32_t rule) override
    {
        NetUidRuleInfo info{.uid = ToI32Saturating(uid, "uid"), .rule = ToI32Saturating(rule, "rule")};
        execute_net_uid_rule_changed(info);
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

    int32_t NetQuotaPolicyChange(const std::vector<NetManagerStandard::NetQuotaPolicy> &quotaPolicies) override
    {
        rust::Vec<NetQuotaPolicyAni> vec;
        for (const auto &p : quotaPolicies) {
            NetQuotaPolicyAni ani;
            ani.network_match_rule.net_type = p.networkmatchrule.netType;
            ani.network_match_rule.identity = rust::string(p.networkmatchrule.ident);
            ani.network_match_rule.sim_id = rust::string(p.networkmatchrule.simId);
            ani.quota_policy.period_duration = rust::string(p.quotapolicy.periodDuration);
            ani.quota_policy.warning_bytes = p.quotapolicy.warningBytes;
            ani.quota_policy.limit_bytes = p.quotapolicy.limitBytes;
            ani.quota_policy.last_warning_remind = p.quotapolicy.lastWarningRemind;
            ani.quota_policy.last_limit_remind = p.quotapolicy.lastLimitRemind;
            ani.quota_policy.metered = p.quotapolicy.metered;
            ani.quota_policy.limit_action = p.quotapolicy.limitAction;
            vec.push_back(std::move(ani));
        }
        execute_net_quota_policy_changed(vec);
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

    int32_t NetBackgroundPolicyChange(bool isBackgroundPolicyAllow) override
    {
        execute_net_background_policy_changed(isBackgroundPolicyAllow);
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

    int32_t NetMeteredIfacesChange(std::vector<std::string> &ifaces) override
    {
        MeteredIfaces info;
        for (auto &iface : ifaces) {
            info.ifaces.push_back(rust::string(iface));
        }
        execute_net_metered_ifaces_changed(info);
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }
};

// Observer instance and registration flag
sptr<PolicyEventCallbackObserverAni> g_policyEventCallbackObserverAni =
    sptr<PolicyEventCallbackObserverAni>(new (std::nothrow) PolicyEventCallbackObserverAni());

std::atomic_bool g_isPolicyObserverRegistered = false;

int32_t RegisterNetQuotaPolicyChangeCallback()
{
    // Atomically acquire the registration slot. If another thread already set the flag
    // to true (or is in the process of registering), bail out with success.
    bool expected = false;
    if (!g_isPolicyObserverRegistered.compare_exchange_strong(expected, true)) {
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

    if (g_policyEventCallbackObserverAni == nullptr) {
        g_isPolicyObserverRegistered = false; // rollback
        return NetManagerStandard::NETMANAGER_ERR_PARAMETER_ERROR;
    }

    int32_t ret = NetManagerStandard::NetPolicyClient::GetInstance()
        .RegisterNetPolicyCallback(g_policyEventCallbackObserverAni);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        g_isPolicyObserverRegistered = false; // rollback so retry is possible
    }
    return ret;
}

int32_t UnregisterNetQuotaPolicyChangeCallback()
{
    // Atomically acquire the unregistration slot. If the flag was already false,
    // another thread has already unregistered — nothing to do.
    bool expected = true;
    if (!g_isPolicyObserverRegistered.compare_exchange_strong(expected, false)) {
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

    if (g_policyEventCallbackObserverAni == nullptr) {
        g_isPolicyObserverRegistered = true; // rollback
        return NetManagerStandard::NETMANAGER_ERR_PARAMETER_ERROR;
    }

    auto ret = NetManagerStandard::NetPolicyClient::GetInstance()
        .UnregisterNetPolicyCallback(g_policyEventCallbackObserverAni);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        g_isPolicyObserverRegistered = true; // rollback so retry is possible
    }
    return ret;
}

static constexpr const char* SETTINGS_PACKAGE_NAME = "com.hmos.communicationsetting";
static constexpr const char* SETTINGS_ABILITY_NAME = "NetAccessPolicySettingUIExtension";
static constexpr const char* UIEXTENSION_TYPE_KEY = "ability.want.params.uiExtensionType";
static constexpr const char* UIEXTENSION_TYPE_VALUE = "sys/commonUI";
static constexpr const char* CONTEXT_TYPE_KEY = "storeKit.ability.contextType";
static constexpr const char* UI_ABILITY_CONTEXT_VALUE = "uiAbility";
static constexpr const char* UI_EXTENSION_CONTEXT_VALUE = "uiExtension";
static constexpr const char* APP_UID = "appUid";

int32_t ShowAppNetPolicySettings(int64_t context)
{
    // Step 1: Extract Context from ANI nativeContext pointer.
    // ETS Context.nativeContext holds the raw address of a std::weak_ptr<Context>,
    // marshalled through ani_rs → cxx bridge as int64_t. This is the standard ANI
    // Context passing mechanism used across the framework.
    // context == 0 is already rejected in Rust before crossing the FFI boundary.
    // We keep this check as defense-in-depth in case of FFI marshalling corruption.
    if (context == 0) {
        return NetManagerStandard::NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto weakCtx = reinterpret_cast<std::weak_ptr<OHOS::AbilityRuntime::Context>*>(
        static_cast<uintptr_t>(context));
    auto ctx = weakCtx->lock();
    if (ctx == nullptr) {
        return NetManagerStandard::NETMANAGER_ERR_PARAMETER_ERROR;
    }

    // Step 2: Convert to AbilityContext or UIExtensionContext (same as original NAPI logic)
    auto loadProductContext = std::make_shared<AppBaseContext>();

    auto abilityCtx = OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::AbilityContext>(ctx);
    if (abilityCtx != nullptr) {
        loadProductContext->abilityContext = abilityCtx;
    } else {
        auto uiExtCtx = OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::UIExtensionContext>(ctx);
        if (uiExtCtx != nullptr) {
            loadProductContext->uiExtensionContext = uiExtCtx;
        } else {
            return NetManagerStandard::NETMANAGER_ERR_PARAMETER_ERROR;
        }
    }

    // Step 3: Get calling UID (same as original NAPI, via IPCSkeleton)
    uint32_t currentUid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());

    // Step 4: Create Want with settings element and params (same as original NAPI)
    OHOS::AAFwk::Want want;
    want.SetElementName(std::string(SETTINGS_PACKAGE_NAME), std::string(SETTINGS_ABILITY_NAME));
    want.SetParam(std::string(UIEXTENSION_TYPE_KEY), std::string(UIEXTENSION_TYPE_VALUE));

    // Step 5: Set context type flag (same as original NAPI)
    want.SetParam(std::string(CONTEXT_TYPE_KEY),
        loadProductContext->uiExtensionContext != nullptr ?
        std::string(UI_EXTENSION_CONTEXT_VALUE) : std::string(UI_ABILITY_CONTEXT_VALUE));
    want.SetParam(std::string(APP_UID), static_cast<int64_t>(currentUid));

    // Step 6: Launch UI extension ability (same as original NAPI)
    if (!StartUiExtensionAbility(want, loadProductContext)) {
        return NetManagerStandard::NETMANAGER_ERR_INTERNAL;
    }

    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t SetBackgroundAllowed(bool allowed)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().SetBackgroundPolicy(allowed);
}

int32_t RestoreAllPolicies(rust::String iccid)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().ResetPolicies(std::string(iccid));
}

int32_t SetPowerTrustlist(rust::Vec<uint32_t> ids, bool allowed)
{
    std::vector<uint32_t> vec;
    for (auto &uid : ids) {
        vec.push_back(uid);
    }
    return NetManagerStandard::NetPolicyClient::GetInstance().SetPowerSaveTrustlist(vec, allowed);
}

int32_t UpdateRemindPolicy(int32_t netType, rust::String iccid, uint32_t remindType)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().UpdateRemindPolicy(
        netType, std::string(iccid), remindType);
}

int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().GetPolicyByUid(uid, policy);
}

int32_t GetBackgroundPolicy(bool &backgroundPolicy)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().GetBackgroundPolicy(backgroundPolicy);
}

int32_t GetPowerSaveTrustlist(rust::Vec<uint32_t> &result)
{
    std::vector<uint32_t> uids;
    int32_t ret = NetManagerStandard::NetPolicyClient::GetInstance().GetPowerSaveTrustlist(uids);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        return ret;
    }
    for (auto uid : uids) {
        result.push_back(uid);
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t GetDeviceIdleTrustlist(rust::Vec<uint32_t> &result)
{
    std::vector<uint32_t> uids;
    int32_t ret = NetManagerStandard::NetPolicyClient::GetInstance().GetDeviceIdleTrustlist(uids);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        return ret;
    }
    for (auto uid : uids) {
        result.push_back(uid);
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().IsUidNetAllowed(uid, metered, isAllowed);
}

int32_t IsUidNetAllowedByIface(uint32_t uid, rust::String iface, bool &isAllowed)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().IsUidNetAllowed(uid, std::string(iface), isAllowed);
}

int32_t SetPolicyByUid(uint32_t uid, uint32_t policy)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().SetPolicyByUid(uid, policy);
}

int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid)
{
    return NetManagerStandard::NetPolicyClient::GetInstance().GetBackgroundPolicyByUid(uid, backgroundPolicyOfUid);
}

int32_t SetDeviceIdleTrustlist(rust::Vec<uint32_t> uids, bool isAllowed)
{
    std::vector<uint32_t> vec;
    for (auto &uid : uids) {
        vec.push_back(uid);
    }
    return NetManagerStandard::NetPolicyClient::GetInstance().SetDeviceIdleTrustlist(vec, isAllowed);
}

int32_t GetUidsByPolicy(uint32_t policy, rust::Vec<uint32_t> &result)
{
    std::vector<uint32_t> uids;
    int32_t ret = NetManagerStandard::NetPolicyClient::GetInstance().GetUidsByPolicy(policy, uids);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        return ret;
    }
    for (auto uid : uids) {
        result.push_back(uid);
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t GetNetworkAccessPolicyByUid(uint32_t uid, NetworkAccessPolicyAni &result)
{
    NetManagerStandard::AccessPolicyParameter parameter;
    parameter.flag = true;
    parameter.uid = uid;
    NetManagerStandard::AccessPolicySave save;
    int32_t ret = NetManagerStandard::NetPolicyClient::GetInstance().GetNetworkAccessPolicy(parameter, save);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        return ret;
    }
    result.wifiAllow = save.policy.wifiAllow;
    result.cellularAllow = save.policy.cellularAllow;
    result.wifiSwitchDisable = save.policy.wifiSwitchDisable;
    result.cellularSwitchDisable = save.policy.cellularSwitchDisable;
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t GetAllNetworkAccessPolicies(rust::Vec<UidNetworkPolicyAni> &result)
{
    NetManagerStandard::AccessPolicyParameter parameter;
    parameter.flag = false;
    parameter.uid = 0;
    NetManagerStandard::AccessPolicySave save;
    int32_t ret = NetManagerStandard::NetPolicyClient::GetInstance().GetNetworkAccessPolicy(parameter, save);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        return ret;
    }
    for (const auto &kv : save.uid_policies) {
        UidNetworkPolicyAni item;
        item.uid = ToI32Saturating(kv.first, "uid");
        item.policy.wifiAllow = kv.second.wifiAllow;
        item.policy.cellularAllow = kv.second.cellularAllow;
        item.policy.wifiSwitchDisable = kv.second.wifiSwitchDisable;
        item.policy.cellularSwitchDisable = kv.second.cellularSwitchDisable;
        result.push_back(std::move(item));
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t GetNetQuotaPoliciesTyped(rust::Vec<NetQuotaPolicyAni> &result)
{
    std::vector<NetManagerStandard::NetQuotaPolicy> policies;
    int32_t ret = NetManagerStandard::NetPolicyClient::GetInstance().GetNetQuotaPolicies(policies);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        return ret;
    }
    for (const auto &p : policies) {
        NetQuotaPolicyAni ani;
        ani.network_match_rule.net_type = p.networkmatchrule.netType;
        ani.network_match_rule.identity = rust::string(p.networkmatchrule.ident);
        ani.network_match_rule.sim_id = rust::string(p.networkmatchrule.simId);
        ani.quota_policy.period_duration = rust::string(p.quotapolicy.periodDuration);
        ani.quota_policy.warning_bytes = p.quotapolicy.warningBytes;
        ani.quota_policy.limit_bytes = p.quotapolicy.limitBytes;
        ani.quota_policy.last_warning_remind = p.quotapolicy.lastWarningRemind;
        ani.quota_policy.last_limit_remind = p.quotapolicy.lastLimitRemind;
        ani.quota_policy.metered = p.quotapolicy.metered;
        ani.quota_policy.limit_action = p.quotapolicy.limitAction;
        result.push_back(std::move(ani));
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t SetNetworkAccessPolicyTyped(uint32_t uid, NetworkAccessPolicyAni policy, bool reconfirmFlag)
{
    NetManagerStandard::NetworkAccessPolicy nativePolicy;
    nativePolicy.wifiAllow = policy.wifiAllow;
    nativePolicy.cellularAllow = policy.cellularAllow;
    nativePolicy.wifiSwitchDisable = policy.wifiSwitchDisable;
    nativePolicy.cellularSwitchDisable = policy.cellularSwitchDisable;
    return NetManagerStandard::NetPolicyClient::GetInstance()
        .SetNetworkAccessPolicy(uid, nativePolicy, reconfirmFlag);
}

// 接收类型化的 NetQuotaPolicyAni 数组，直接转换为系统结构，无需 JSON 解析。
int32_t SetNetQuotaPolicies(rust::Vec<NetQuotaPolicyAni> policies)
{
    if (policies.empty()) {
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

    std::vector<NetManagerStandard::NetQuotaPolicy> vec;
    vec.reserve(policies.size());

    for (const auto &ani : policies) {
        NetManagerStandard::NetQuotaPolicy p;
        p.networkmatchrule.netType = ani.network_match_rule.net_type;
        p.networkmatchrule.ident = static_cast<std::string>(ani.network_match_rule.identity);
        p.networkmatchrule.simId = static_cast<std::string>(ani.network_match_rule.sim_id);
        p.quotapolicy.periodDuration = static_cast<std::string>(ani.quota_policy.period_duration);
        p.quotapolicy.warningBytes = ani.quota_policy.warning_bytes;
        p.quotapolicy.limitBytes = ani.quota_policy.limit_bytes;
        p.quotapolicy.lastWarningRemind = ani.quota_policy.last_warning_remind;
        p.quotapolicy.lastLimitRemind = ani.quota_policy.last_limit_remind;
        p.quotapolicy.metered = ani.quota_policy.metered;
        p.quotapolicy.limitAction = ani.quota_policy.limit_action;
        vec.push_back(std::move(p));
    }

    return NetManagerStandard::NetPolicyClient::GetInstance().SetNetQuotaPolicies(vec);
}

} // namespace NetManagerAni
} // namespace OHOS

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

#include "show_app_net_policy_settings.h"
#include "napi_base_context.h"
#include "ui_content.h"
#include "netmanager_base_common_utils.h"
#include "netmanager_base_log.h"
#include "data_ability_helper.h"
#include "ipc_skeleton.h"
namespace OHOS {
namespace NetManagerStandard {
 
static constexpr const char* SETTINGS_PACKAGE_NAME = "com.huawei.hmos.communicationsetting";
static constexpr const char* SETTINGS_ABILITY_NAME = "NetAccessPolicySettingUIExtension";
static constexpr const char* UIEXTENSION_TYPE_KEY = "ability.want.params.uiExtensionType";
static constexpr const char* UIEXTENSION_TYPE_VALUE = "sys/commonUI";
static constexpr const char* CONTEXT_TYPE_KEY = "storeKit.ability.contextType";
static constexpr const char* UI_ABILITY_CONTEXT_VALUE = "uiAbility";
static constexpr const char* UI_EXTENSION_CONTEXT_VALUE = "uiExtension";
static constexpr const char* APP_UID = "appUid";

#define ARGS_ONE 1
#define ARGS_TWO 2
#define PARAM0 0
#define PARAM1 1
struct AsyncCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    napi_ref callbackRef;
    std::shared_ptr<OHOS::AppExecFwk::DataAbilityHelper> dataAbilityHelper;
    std::string key;
    std::string value;
    std::string uri;
    std::string tableName;
    int status;
    std::shared_ptr<OHOS::DataShare::DataShareHelper> dataShareHelper = nullptr;
    bool useNonSilent;
};

napi_value wrap_void_to_js(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

bool StartUiExtensionAbility(OHOS::AAFwk::Want &request, std::shared_ptr<AppBaseContext> &asyncContext)
{
    NETMGR_LOG_I("StartUiExtensionAbility");
    auto uiContent = GetUIContent(asyncContext);
    if (uiContent == nullptr) {
        NETMGR_LOG_E("UIContent is nullptr");
        return false;
    }
    std::string info = uiContent->GetContentInfo();
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

OHOS::Ace::UIContent* GetUIContent(std::shared_ptr<AppBaseContext> &asyncContext)
{
    if (!asyncContext) return nullptr;
    OHOS::Ace::UIContent* uiContent = nullptr;
    if (asyncContext->abilityContext != nullptr) {
        uiContent = asyncContext->abilityContext->GetUIContent();
    } else if (asyncContext->uiExtensionContext != nullptr) {
        uiContent = asyncContext->uiExtensionContext->GetUIContent();
    }
    return uiContent;
}

ModalUICallback::ModalUICallback(std::shared_ptr<AppBaseContext> baseContext)
{
    baseContext_ = baseContext;
}

void ModalUICallback::CloseModalUI()
{
    auto uiContent = GetUIContent(baseContext_);
    if (uiContent == nullptr) {
        return;
    }
    uiContent->CloseModalUIExtension(sessionId_);
}

void ModalUICallback::SetSessionId(int32_t sessionId)
{
    sessionId_ = sessionId;
}

void ModalUICallback::OnRelease(int32_t releaseCode)
{
    CloseModalUI();
}

bool ParseAbilityContext(napi_env env, const napi_value &obj,
    std::shared_ptr<OHOS::AbilityRuntime::AbilityContext> &abilityContext,
    std::shared_ptr<OHOS::AbilityRuntime::UIExtensionContext> &uiExtensionContext)
{
    bool stageMode = false;
    napi_status status = OHOS::AbilityRuntime::IsStageContext(env, obj, stageMode);
    if (status != napi_ok || !stageMode) {
        return false;
    }
    auto context = OHOS::AbilityRuntime::GetStageModeContext(env, obj);
    if (context == nullptr) {
        return false;
    }
    abilityContext = OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::AbilityContext>(context);
    if (abilityContext != nullptr) {
        return true;
    }
    uiExtensionContext = OHOS::AbilityRuntime::Context::ConvertTo<OHOS::AbilityRuntime::UIExtensionContext>(context);
    if (uiExtensionContext == nullptr) {
        return false;
    }
    return true;
}

bool CheckParam(napi_env env, AsyncCallbackInfo* asyncCallbackInfo, napi_callback_info info,
    size_t &argc, napi_value* argv)
{
    napi_valuetype valueType;
    napi_status ret = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (ret != napi_ok) {
        return false;
    }
    if (argc == ARGS_ONE) {
        ret = napi_typeof(env, argv[PARAM0], &valueType);
        if (ret != napi_ok || valueType != napi_object) {
            return false;
        }
    } else if (argc == ARGS_TWO) {
        ret = napi_typeof(env, argv[PARAM0], &valueType);
        if (ret != napi_ok || valueType != napi_object) {
            return false;
        }
        ret = napi_typeof(env, argv[PARAM1], &valueType);
        if (ret != napi_ok || valueType != napi_function) {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

napi_value SettingsCompletePromise(napi_env env, AsyncCallbackInfo* asyncCallbackInfo, napi_value result)
{
    NETMGR_LOG_I("settings complete promise.");
    if (!asyncCallbackInfo) {
        return nullptr;
    }
    napi_value promise;
    napi_deferred deferred;
    napi_create_promise(env, &deferred, &promise);
    asyncCallbackInfo->deferred = deferred;
    napi_resolve_deferred(env, asyncCallbackInfo->deferred, result);
    return promise;
}

napi_value SettingsInCompletePromise(napi_env env, AsyncCallbackInfo* asyncCallbackInfo, napi_value result)
{
    NETMGR_LOG_I("settings incomplete promise.");
    if (!asyncCallbackInfo) {
        return nullptr;
    }
    napi_value promise;
    napi_deferred deferred;
    napi_create_promise(env, &deferred, &promise);
    asyncCallbackInfo->deferred = deferred;
    napi_reject_deferred(env, asyncCallbackInfo->deferred, result);
    return promise;
}

napi_value ShowAppNetPolicySettings(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_TWO;
    napi_value argv[ARGS_TWO] = { 0 };
    std::shared_ptr<AsyncCallbackInfo> asyncCallbackInfo = std::make_shared<AsyncCallbackInfo>();
    bool isInvalid = CheckParam(env, asyncCallbackInfo.get(), info, argc, argv);
    if (!isInvalid) {
        NETMGR_LOG_I("param is invalid.");
        napi_value result = wrap_void_to_js(env);
        napi_value promise = SettingsInCompletePromise(env, asyncCallbackInfo.get(), result);
        NETMGR_LOG_E("2100001 - Invalid parameter value.");
        return promise;
    }
    auto loadProductContext = std::make_shared<AppBaseContext>();
    if (!ParseAbilityContext(env, argv[0], loadProductContext->abilityContext,
        loadProductContext->uiExtensionContext)) {
        napi_value result = wrap_void_to_js(env);
        napi_value promise = SettingsInCompletePromise(env, asyncCallbackInfo.get(), result);
        NETMGR_LOG_E("2100001 - Invalid parameter value.");
        return promise;
    }
    uint32_t currentUid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());
    OHOS::AAFwk::Want want;
    want.SetElementName(std::string(SETTINGS_PACKAGE_NAME), std::string(SETTINGS_ABILITY_NAME));
    want.SetParam(std::string(UIEXTENSION_TYPE_KEY), std::string(UIEXTENSION_TYPE_VALUE));
    want.SetParam(std::string(CONTEXT_TYPE_KEY),
        loadProductContext->uiExtensionContext != nullptr ?
        std::string((UI_EXTENSION_CONTEXT_VALUE)) : std::string(UI_ABILITY_CONTEXT_VALUE));
    want.SetParam(std::string(APP_UID), static_cast<long>(currentUid));
    if (!StartUiExtensionAbility(want, loadProductContext)) {
        NETMGR_LOG_E("2100003 - System internal error.");
        napi_value result = wrap_void_to_js(env);
        napi_value promise = SettingsInCompletePromise(env, asyncCallbackInfo.get(), result);
        return promise;
    }
    napi_value result = wrap_void_to_js(env);
    napi_value promise = SettingsCompletePromise(env, asyncCallbackInfo.get(), result);
    return promise;
}
}
}
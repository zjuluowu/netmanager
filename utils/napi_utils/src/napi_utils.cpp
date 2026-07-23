/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "napi_utils.h"

#include <atomic>
#include <cstring>
#include <initializer_list>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_set>
#include <unordered_map>

#include "netmanager_base_log.h"
#include "securec.h"

namespace OHOS {
namespace NetManagerStandard {
namespace NapiUtils {
namespace {
static constexpr const int MAX_STRING_LENGTH = 65536;
constexpr const char *CODE = "code";
constexpr const char *MSG = "message";
} // namespace

static std::unordered_set<napi_env> unorderedSetEnv;
static std::recursive_mutex mutexForEnv;

class WorkData {
public:
    WorkData() = delete;

    WorkData(napi_env env, void *data, void (*handler)(napi_env env, napi_status status, void *data))
        : env_(env), data_(data), handler_(handler)
    {
    }

    napi_env env_;
    void *data_;
    void (*handler_)(napi_env env, napi_status status, void *data);
};

struct UvHandlerQueue : public std::queue<UvHandler> {
    UvHandler Pop();
    void Push(const UvHandler &handler);

private:
    std::mutex mutex;
};

static std::mutex g_mutex;
static std::unordered_map<uint64_t, std::shared_ptr<UvHandlerQueue>> g_handlerQueueMap;
static const char *const HTTP_UV_SYNC_QUEUE_NAME = "NET_CONNECTION_UV_SYNC_QUEUE_NAME";

UvHandler UvHandlerQueue::Pop()
{
    std::lock_guard lock(mutex);
    if (empty()) {
        return {};
    }
    auto s = front();
    pop();
    return s;
}

void UvHandlerQueue::Push(const UvHandler &handler)
{
    std::lock_guard lock(mutex);
    push(handler);
}

napi_value GetGlobal(napi_env env)
{
    napi_value undefined = GetUndefined(env);
    napi_value global = nullptr;
    NAPI_CALL_BASE(env, napi_get_global(env, &global), undefined);
    return global;
}

uint64_t CreateUvHandlerQueue(napi_env env)
{
    static std::atomic<uint64_t> id = 1; // start from 1
    uint64_t newId = id++;
    NETMANAGER_BASE_LOGI("newId = %{public}s, id = %{public}s", std::to_string(newId).c_str(),
                         std::to_string(id).c_str());

    auto global = GetGlobal(env);
    auto queueWrapper = CreateObject(env);
    SetNamedProperty(env, global, HTTP_UV_SYNC_QUEUE_NAME, queueWrapper);
    {
        std::lock_guard lock(g_mutex);
        g_handlerQueueMap.emplace(newId, std::make_shared<UvHandlerQueue>());
    }
    napi_wrap(
        env, queueWrapper, reinterpret_cast<void *>(newId),
        [](napi_env env, void *data, void *) {
            auto id = reinterpret_cast<uint64_t>(data);
            std::lock_guard lock(g_mutex);
            g_handlerQueueMap.erase(id);
        },
        nullptr, nullptr);
    auto envWrapper = new (std::nothrow) napi_env;
    if (envWrapper == nullptr) {
        return newId;
    }
    *envWrapper = env;
    napi_add_env_cleanup_hook(
        env,
        [](void *data) {
            NapiUtils::HandleNapiCleanUp(data);
        },
        envWrapper);
    return newId;
}

void HandleNapiCleanUp(void *data)
{
    auto envWrapper = reinterpret_cast<napi_env *>(data);
    if (envWrapper == nullptr) {
        return;
    }
    auto env = *envWrapper;
    delete envWrapper;
    if (env == nullptr) {
        return;
    }
    auto closeScope = [env](napi_handle_scope scope) { NapiUtils::CloseScope(env, scope); };
    std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(NapiUtils::OpenScope(env), closeScope);
    auto queueWrapper = NapiUtils::GetValueFromGlobal(env, HTTP_UV_SYNC_QUEUE_NAME);
    if (queueWrapper == nullptr) {
        return;
    }
    void *result = nullptr;
    napi_remove_wrap(env, queueWrapper, &result);
    auto id = reinterpret_cast<uint64_t>(result);
    std::lock_guard lock(g_mutex);
    g_handlerQueueMap.erase(id);
}

napi_value GetValueFromGlobal(napi_env env, const std::string &className)
{
    auto global = NapiUtils::GetGlobal(env);
    if (NapiUtils::GetValueType(env, global) == napi_undefined) {
        return GetUndefined(env);
    }
    return NapiUtils::GetNamedProperty(env, global, className);
}

static uv_after_work_cb MakeUvCallback()
{
    return [](uv_work_t *work, int status) {
        if (!work) {
            return;
        }
        std::unique_ptr<uv_work_t> workHandle(work);

        if (!work->data) {
            return;
        }
        auto env = reinterpret_cast<napi_env>(work->data);
        if (!env) {
            return;
        }

        auto closeScope = [env](napi_handle_scope scope) { NapiUtils::CloseScope(env, scope); };
        std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(NapiUtils::OpenScope(env), closeScope);
        auto queueWrapper = GetValueFromGlobal(env, HTTP_UV_SYNC_QUEUE_NAME);
        if (!queueWrapper) {
            return;
        }
        void *theId = nullptr;
        napi_unwrap(env, queueWrapper, &theId);
        if (!theId) { // that is why moduleId is started from 1
            return;
        }
        UvHandler handler;
        {
            std::lock_guard lock(g_mutex);
            auto it = g_handlerQueueMap.find(reinterpret_cast<uint64_t>(theId));
            if (it == g_handlerQueueMap.end()) {
                return;
            }
            handler = it->second->Pop();
        }
        if (handler) {
            handler(env);
        }
    };
}

void CreateUvQueueWorkByModuleId(napi_env env, const UvHandler &handler, uint64_t id)
{
    uv_loop_s *loop = nullptr;
    if (!IsEnvValid(env)) {
        NETMANAGER_BASE_LOGE("the env is invalid");
        return;
    }
    napi_get_uv_event_loop(env, &loop);
    if (!loop) {
        return;
    }
    uv_work_t *work = nullptr;
    {
        std::lock_guard lock(g_mutex);
        auto it = g_handlerQueueMap.find(id);
        if (it == g_handlerQueueMap.end()) {
            return;
        }
        work = new (std::nothrow) uv_work_t;
        if (work == nullptr) {
            return;
        }
        work->data = env;
        it->second->Push(handler);
    }

    if (work) {
        int32_t ret = uv_queue_work_with_qos(loop, work, [](uv_work_t *) {}, MakeUvCallback(), uv_qos_default);
        if (ret != 0) {
            delete work;
        }
    }
}

napi_valuetype GetValueType(napi_env env, napi_value value)
{
    if (value == nullptr) {
        return napi_undefined;
    }

    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, value, &valueType), napi_undefined);
    return valueType;
}

/* named property */
bool HasNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    bool hasProperty = false;
    NAPI_CALL_BASE(env, napi_has_named_property(env, object, propertyName.c_str(), &hasProperty), false);
    return hasProperty;
}

napi_value GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, object, propertyName.c_str(), &value));
    return value;
}

void SetNamedProperty(napi_env env, napi_value object, const std::string &name, napi_value value)
{
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, object, name.c_str(), value));
}

std::vector<std::string> GetPropertyNames(napi_env env, napi_value object)
{
    std::vector<std::string> ret;
    napi_value names = nullptr;
    NAPI_CALL_BASE(env, napi_get_property_names(env, object, &names), ret);
    uint32_t length = 0;
    NAPI_CALL_BASE(env, napi_get_array_length(env, names, &length), ret);
    for (uint32_t index = 0; index < length; ++index) {
        napi_value name = nullptr;
        if (napi_get_element(env, names, index, &name) != napi_ok) {
            continue;
        }
        if (GetValueType(env, name) != napi_string) {
            continue;
        }
        ret.emplace_back(GetStringFromValueUtf8(env, name));
    }
    return ret;
}

/* UINT32 */
napi_value CreateUint32(napi_env env, uint32_t code)
{
    napi_value value = nullptr;
    if (napi_create_uint32(env, code, &value) != napi_ok) {
        return nullptr;
    }
    return value;
}

uint32_t GetUint32FromValue(napi_env env, napi_value value)
{
    uint32_t ret = 0;
    NAPI_CALL_BASE(env, napi_get_value_uint32(env, value, &ret), 0);
    return ret;
}

uint32_t GetUint32Property(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return 0;
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    return GetUint32FromValue(env, value);
}

void SetUint32Property(napi_env env, napi_value object, const std::string &name, uint32_t value)
{
    napi_value jsValue = CreateUint32(env, value);
    if (GetValueType(env, jsValue) != napi_number) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), jsValue);
}

/* INT32 */
napi_value CreateInt32(napi_env env, int32_t code)
{
    napi_value value = nullptr;
    if (napi_create_int32(env, code, &value) != napi_ok) {
        return nullptr;
    }
    return value;
}

int32_t GetInt32FromValue(napi_env env, napi_value value)
{
    int32_t ret = 0;
    NAPI_CALL_BASE(env, napi_get_value_int32(env, value, &ret), 0);
    return ret;
}

int32_t GetInt32Property(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return 0;
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    return GetInt32FromValue(env, value);
}

void SetInt32Property(napi_env env, napi_value object, const std::string &name, int32_t value)
{
    napi_value jsValue = CreateInt32(env, value);
    if (GetValueType(env, jsValue) != napi_number) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), jsValue);
}

/* INT64 */
napi_value CreateInt64(napi_env env, int64_t code)
{
    napi_value value = nullptr;
    if (napi_create_int64(env, code, &value) != napi_ok) {
        return nullptr;
    }
    return value;
}

int64_t GetInt64Property(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return 0;
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    return GetInt64FromValue(env, value);
}
int64_t GetInt64FromValue(napi_env env, napi_value value)
{
    int64_t ret = 0;
    NAPI_CALL_BASE(env, napi_get_value_int64(env, value, &ret), 0);
    return ret;
}
void SetInt64Property(napi_env env, napi_value object, const std::string &name, int64_t value)
{
    napi_value jsValue = CreateInt64(env, value);
    if (GetValueType(env, jsValue) != napi_number) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), jsValue);
}

/* String UTF8 */
napi_value CreateStringUtf8(napi_env env, const std::string &str)
{
    napi_value value = nullptr;
    if (napi_create_string_utf8(env, str.c_str(), strlen(str.c_str()), &value) != napi_ok) {
        return nullptr;
    }
    return value;
}

std::string GetStringFromValueUtf8(napi_env env, napi_value value)
{
    std::string result;
    char str[MAX_STRING_LENGTH] = {0};
    size_t length = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, value, str, MAX_STRING_LENGTH, &length), result);
    if (length > MAX_STRING_LENGTH) {
        result.append(str, MAX_STRING_LENGTH);
        return result;
    }
    if (length > 0) {
        return result.append(str, length);
    }
    return result;
}

SecureData GetSecureDataFromValueUtf8(napi_env env, napi_value value)
{
    SecureData result;
    char str[MAX_STRING_LENGTH] = {0};
    size_t length = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, value, str, MAX_STRING_LENGTH, &length), result);
    if (length > 0) {
        result.append(str, length);
    }
    return result;
}

std::string GetStringPropertyUtf8(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return "";
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    return GetStringFromValueUtf8(env, value);
}

SecureData GetSecureDataPropertyUtf8(napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = GetNamedProperty(env, object, propertyName);
    return GetSecureDataFromValueUtf8(env, value);
}

void SetStringPropertyUtf8(napi_env env, napi_value object, const std::string &name, const std::string &value)
{
    napi_value jsValue = CreateStringUtf8(env, value);
    if (GetValueType(env, jsValue) != napi_string) {
        return;
    }
    napi_set_named_property(env, object, name.c_str(), jsValue);
}

napi_status SetVectorUint8Property(napi_env env, napi_value object, const std::string &name,
    const std::vector<uint8_t> &value)
{
    napi_value array;
    napi_status status = napi_create_array_with_length(env, value.size(), &array);
    if (status != napi_ok) {
        NETMANAGER_BASE_LOGE("failed to create array! field: %{public}s", name.c_str());
        return status;
    }

    for (size_t i = 0; i < value.size(); ++i) {
        napi_value ele;
        napi_status status = napi_create_int32(env, value[i], &ele);
        if (status != napi_ok) {
            NETMANAGER_BASE_LOGE("failed to create int32!");
            return status;
        }
        status = napi_set_element(env, array, i, ele);
        if (status != napi_ok) {
            NETMANAGER_BASE_LOGE("failed to set element, status: %{public}d!", status);
            return status;
        }
    }
    if (napi_set_named_property(env, object, name.c_str(), array) != napi_ok) {
        NETMANAGER_BASE_LOGE("failed to set %{public}s named property!", name.c_str());
    }
    return status;
}
 
bool GetVectorUint8Property(napi_env env, napi_value object, const std::string &propertyName,
    std::vector<uint8_t> &vec)
{
    bool hasProperty = false;
    uint32_t length = 0;
    NAPI_CALL_BASE(env, napi_has_named_property(env, object, propertyName.c_str(), &hasProperty), {});
    napi_value fieldvalue;
    if (!hasProperty || napi_get_named_property(env, object, propertyName.c_str(), &fieldvalue) != napi_ok) {
        NETMANAGER_BASE_LOGE("GetVectorUint8Property, no property: %{public}s", propertyName.c_str());
        return false;
    }
    NAPI_CALL_BASE(env, napi_get_array_length(env, fieldvalue, &length), false);
    vec.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        napi_value element;
        NAPI_CALL_BASE(env, napi_get_element(env, fieldvalue, i, &element), false);
        int32_t result = 0;
        NAPI_CALL_BASE(env, napi_get_value_int32(env, element, &result), false);
        if (result > 0xFF || result < 0) {
            return napi_invalid_arg;
        }
        vec.emplace_back(static_cast<uint8_t>(result));
    }
    return true;
}

/* array buffer */
bool ValueIsArrayBuffer(napi_env env, napi_value value)
{
    bool isArrayBuffer = false;
    NAPI_CALL_BASE(env, napi_is_arraybuffer(env, value, &isArrayBuffer), false);
    return isArrayBuffer;
}

void *GetInfoFromArrayBufferValue(napi_env env, napi_value value, size_t *length)
{
    if (length == nullptr) {
        return nullptr;
    }

    void *data = nullptr;
    NAPI_CALL(env, napi_get_arraybuffer_info(env, value, &data, length));
    return data;
}

napi_value CreateArrayBuffer(napi_env env, size_t length, void **data)
{
    if (length == 0) {
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_arraybuffer(env, length, data, &result));
    return result;
}

/* object */
napi_value CreateObject(napi_env env)
{
    napi_value object = nullptr;
    NAPI_CALL(env, napi_create_object(env, &object));
    return object;
}

/* undefined */
napi_value GetUndefined(napi_env env)
{
    napi_value undefined = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &undefined));
    return undefined;
}

/* function */
napi_value CallFunction(napi_env env, napi_value recv, napi_value func, size_t argc, const napi_value *argv)
{
    napi_value res = nullptr;
    NAPI_CALL(env, napi_call_function(env, recv, func, argc, argv, &res));
    return res;
}

napi_value CreateFunction(napi_env env, const std::string &name, napi_callback func, void *arg)
{
    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_function(env, name.c_str(), strlen(name.c_str()), func, arg, &res));
    return res;
}

/* reference */
napi_ref CreateReference(napi_env env, napi_value callback)
{
    napi_ref callbackRef = nullptr;
    NAPI_CALL(env, napi_create_reference(env, callback, 1, &callbackRef));
    return callbackRef;
}

napi_value GetReference(napi_env env, napi_ref callbackRef)
{
    napi_value callback = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, callbackRef, &callback));
    return callback;
}

void DeleteReference(napi_env env, napi_ref callbackRef)
{
    (void)napi_delete_reference(env, callbackRef);
}

/* boolean */
bool GetBooleanProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    if (!HasNamedProperty(env, object, propertyName)) {
        return false;
    }
    napi_value value = GetNamedProperty(env, object, propertyName);
    bool ret = false;
    NAPI_CALL_BASE(env, napi_get_value_bool(env, value, &ret), false);
    return ret;
}

void SetBooleanProperty(napi_env env, napi_value object, const std::string &name, bool value)
{
    napi_value jsValue = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, value, &jsValue));
    if (GetValueType(env, jsValue) != napi_boolean) {
        return;
    }

    napi_set_named_property(env, object, name.c_str(), jsValue);
}

napi_value GetBoolean(napi_env env, bool value)
{
    napi_value jsValue = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, value, &jsValue));
    return jsValue;
}

bool GetBooleanValue(napi_env env, napi_value value)
{
    bool ret = false;
    NAPI_CALL_BASE(env, napi_get_value_bool(env, value, &ret), 0);
    return ret;
}

/* define properties */
void DefineProperties(napi_env env, napi_value object,
                      const std::initializer_list<napi_property_descriptor> &properties)
{
    napi_property_descriptor descriptors[properties.size()];
    std::copy(properties.begin(), properties.end(), descriptors);

    (void)napi_define_properties(env, object, properties.size(), descriptors);
}

/* array */
napi_value CreateArray(napi_env env, size_t length)
{
    if (length == 0) {
        napi_value res = nullptr;
        NAPI_CALL(env, napi_create_array(env, &res));
        return res;
    }
    napi_value res = nullptr;
    NAPI_CALL(env, napi_create_array_with_length(env, length, &res));
    return res;
}

void SetArrayElement(napi_env env, napi_value array, uint32_t index, napi_value value)
{
    (void)napi_set_element(env, array, index, value);
}

bool IsArray(napi_env env, napi_value value)
{
    bool result = false;
    NAPI_CALL_BASE(env, napi_is_array(env, value, &result), false);
    return result;
}

uint32_t GetArrayLength(napi_env env, napi_value arr)
{
    uint32_t arrayLength = 0;
    NAPI_CALL_BASE(env, napi_get_array_length(env, arr, &arrayLength), 0);
    return arrayLength;
}

napi_value GetArrayElement(napi_env env, napi_value arr, uint32_t index)
{
    napi_value elementValue = nullptr;
    NAPI_CALL(env, napi_get_element(env, arr, index, &elementValue));
    return elementValue;
}

/* libuv */
void CreateUvQueueWork(napi_env env, void *data, void(handler)(uv_work_t *, int status))
{
    uv_loop_s *loop = nullptr;
    if (!IsEnvValid(env)) {
        NETMANAGER_BASE_LOGE("the env is invalid");
        return;
    }
    napi_get_uv_event_loop(env, &loop);
    if (!loop) {
        NETMANAGER_BASE_LOGE("napi get uv event loop is null");
        return;
    }
    auto work = new uv_work_t;
    work->data = data;

    int32_t ret = uv_queue_work_with_qos(loop, work, [](uv_work_t *) {}, handler, uv_qos_default);
    if (ret != 0) {
        delete work;
    }
}

/* scope */
napi_handle_scope OpenScope(napi_env env)
{
    napi_handle_scope scope = nullptr;
    NAPI_CALL(env, napi_open_handle_scope(env, &scope));
    return scope;
}

void CloseScope(napi_env env, napi_handle_scope scope)
{
    (void)napi_close_handle_scope(env, scope);
}

napi_value CreateEnumConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    return thisArg;
}

/* error */
napi_value CreateErrorMessage(napi_env env, int32_t errorCode, const std::string &errorMessage)
{
    napi_value result = CreateObject(env);
    SetNamedProperty(env, result, CODE, CreateInt32(env, errorCode));
    SetNamedProperty(env, result, MSG, CreateStringUtf8(env, errorMessage));
    return result;
}

void HookForEnvCleanup(void *data)
{
    std::lock_guard<std::recursive_mutex> lock(mutexForEnv);
    auto envWrapper = reinterpret_cast<napi_env *>(data);
    if (envWrapper == nullptr) {
        return;
    }
    auto env = *envWrapper;
    delete envWrapper;
    if (env == nullptr) {
        return;
    }
    auto pos = unorderedSetEnv.find(env);
    if (pos == unorderedSetEnv.end()) {
        NETMANAGER_BASE_LOGE("The env is not in the unordered set");
        return;
    }
    NETMANAGER_BASE_LOGD("env clean up, erase from the unordered set");
    unorderedSetEnv.erase(pos);
}

void SetEnvValid(napi_env env)
{
    std::lock_guard<std::recursive_mutex> lock(mutexForEnv);
    unorderedSetEnv.emplace(env);
}

bool IsEnvValid(napi_env env)
{
    std::lock_guard<std::recursive_mutex> lock(mutexForEnv);
    auto pos = unorderedSetEnv.find(env);
    if (pos == unorderedSetEnv.end()) {
        return false;
    }
    return true;
}
} // namespace NapiUtils
} // namespace NetManagerStandard
} // namespace OHOS

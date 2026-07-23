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

#include <cstring>
#include <string>
#include <curl/curl.h>
#include <jerryscript.h>

#include "net_pac_manager.h"
#include "pac_functions.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr char NULL_CHAR = '\0';
constexpr const char FIND_FUNC[] = "FindProxyForURL";
constexpr const char USER_AGENT[] = "libcurl-agent/1.0";
const std::string TRUE = "true";
const std::string FALSE = "false";
const std::string EMPTY = "";
constexpr int32_t HTTP_CODE_200 = 200;
} // namespace

static std::string g_script;

NetPACManager::NetPACManager() : pacScriptVal_(jerry_create_undefined()), status_(false), engineInitialized_(false)
{
}

NetPACManager::~NetPACManager()
{
    std::lock_guard<std::mutex> guard{pacMutex_};
    if (jerry_value_is_undefined(pacScriptVal_) == false) {
        jerry_release_value(pacScriptVal_);
        pacScriptVal_ = jerry_create_undefined();
    }
    if (engineInitialized_) {
        jerry_cleanup();
        engineInitialized_ = false;
    }
}

bool NetPACManager::InitPACScript(const std::string &script)
{
    ReleasePACScript();
    const char *pac_script = script.c_str();
    jerry_init(JERRY_INIT_EMPTY);
    engineInitialized_ = true;
    PacFunctions::RegisterPacFunctions();
    pacScriptVal_ = jerry_parse(NULL, 0, (jerry_char_t *)pac_script, strlen(pac_script), JERRY_PARSE_NO_OPTS);
    if (jerry_value_is_error(pacScriptVal_)) {
        jerry_value_t error_value = jerry_get_value_from_error(pacScriptVal_, false);
        jerry_release_value(pacScriptVal_);
        jerry_release_value(error_value);
        pacScriptVal_ = jerry_create_undefined();
        status_ = false;
        return false;
    }
    status_ = true;
    return true;
}

bool NetPACManager::InitPACScriptWithURL(const std::string &scriptUrl)
{
    std::lock_guard<std::mutex> guard{pacMutex_};
    scriptFileUrl_ = scriptUrl;
    g_script.clear();
    DownloadPACScript(scriptUrl);
    if (g_script.empty()) {
        ReleasePACScript();
        status_ = false;
        return false;
    }
    return InitPACScript(g_script);
}

void NetPACManager::ReleasePACScript()
{
    if (jerry_value_is_undefined(pacScriptVal_) == false) {
        jerry_release_value(pacScriptVal_);
        pacScriptVal_ = jerry_create_undefined();
    }
    if (engineInitialized_) {
        jerry_cleanup();
        engineInitialized_ = false;
    }
}


static void ReleaseValues(const std::vector<jerry_value_t> &values)
{
    for (auto value : values) {
        jerry_release_value(value);
    }
}

PAC_STATUS NetPACManager::FindProxyForURL(const std::string &url, const std::string &hostStr, std::string &proxy)
{
    if (!status_ && !InitPACScriptWithURL(scriptFileUrl_)) {
        return PAC_SCRIPT_DOWNLOAD_ERROR;
    }
    std::lock_guard<std::mutex> guard{pacMutex_};
    if (!engineInitialized_ || jerry_value_is_undefined(pacScriptVal_)) {
        return PAC_SCRIPT_FUNCTION_ERROR;
    }
    std::string host = hostStr.empty() ? ParseHost(url) : hostStr;
    jerry_value_t result = jerry_run(pacScriptVal_);
    if (jerry_value_is_error(result)) {
        jerry_value_t error_value = jerry_get_value_from_error(result, false);
        jerry_release_value(result);
        jerry_release_value(error_value);
        return PAC_SCRIPT_RUN_ERROR;
    }
    jerry_value_t global_object = jerry_get_global_object();
    jerry_value_t func_name = jerry_create_string(reinterpret_cast<const jerry_char_t *>(FIND_FUNC));
    jerry_value_t func = jerry_get_property(global_object, func_name);
    if (!jerry_value_is_function(func)) {
        ReleaseValues({func, func_name, global_object, result});
        return PAC_SCRIPT_FUNCTION_ERROR;
    }
    jerry_value_t args[2] = {jerry_create_string(reinterpret_cast<const jerry_char_t *>(url.c_str())),
                             jerry_create_string(reinterpret_cast<const jerry_char_t *>(host.c_str()))};
    jerry_value_t call_result = jerry_call_function(func, global_object, args, 2);
    PAC_STATUS status = PAC_OK;
    if (!jerry_value_is_error(call_result)) {
        if (jerry_value_is_string(call_result)) {
            jerry_length_t strLength = jerry_get_string_length(call_result);
            if (strLength > 0) {
                char buffer[strLength + 1];
                jerry_string_to_char_buffer(call_result, reinterpret_cast<jerry_char_t *>(buffer), strLength);
                buffer[strLength] = NULL_CHAR;
                proxy.append(buffer, strLength);
            }
        } else if (jerry_value_is_boolean(call_result)) {
            proxy.append(jerry_get_boolean_value(call_result) ? TRUE : FALSE);
        } else if (jerry_value_is_number(call_result)) {
            proxy.append(std::to_string(static_cast<int>(jerry_get_number_value(call_result))));
        }
    } else {
        jerry_value_t error_value = jerry_get_value_from_error(call_result, false);
        jerry_release_value(error_value);
        status = PAC_SCRIPT_CALL_ERROR;
    }
    ReleaseValues({call_result, args[0], args[1], func, func_name, global_object, result});
    return status;
}

static size_t WriteToString(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    std::string *mem = static_cast<std::string *>(userp);
    mem->append(static_cast<char *>(contents), realsize);
    g_script.append(static_cast<char *>(contents), realsize);
    return realsize;
}

void NetPACManager::DownloadPACScript(const std::string &url)
{
    struct CurlGlobalGuard {
        CurlGlobalGuard()
        {
            curl_global_init(CURL_GLOBAL_ALL);
        }
        ~CurlGlobalGuard()
        {
            curl_global_cleanup();
        }
    } curlGuard;
    CURL *curl_handle = curl_easy_init();
    if (!curl_handle) {
        return;
    }
    struct CurlHandleDeleter {
        void operator()(CURL *handle)
        {
            if (handle)
                curl_easy_cleanup(handle);
        }
    };
    std::unique_ptr<CURL, CurlHandleDeleter> handleGuard(curl_handle);
    std::string data;
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteToString);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK) {
        NETMGR_LOG_E("CURL perform failed: %{private}s", curl_easy_strerror(res));
        return;
    }
    int responseCode;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &responseCode);
    if (responseCode != HTTP_CODE_200) {
        NETMGR_LOG_E("HTTP request failed with status code: %d", responseCode);
        return;
    }
    if (data.empty()) {
        NETMGR_LOG_E("Downloaded PAC script is empty");
        return;
    }
    if (data.find("FindProxyForURL") == std::string::npos) {
        NETMGR_LOG_W("Downloaded content may not be a valid PAC script (no FindProxyForURL function found)");
    }
    return;
}

std::string NetPACManager::ParseHost(const std::string &url)
{
    CURLU *cu = curl_url();
    if (!cu) {
        return EMPTY;
    }
    struct CurlUrlDeleter {
        void operator()(CURLU *handle)
        {
            if (handle)
                curl_url_cleanup(handle);
        }
    };
    std::unique_ptr<CURLU, CurlUrlDeleter> urlGuard(cu);
    CURLUcode uc = curl_url_set(cu, CURLUPART_URL, url.c_str(), 0);
    if (uc != CURLUE_OK) {
        return EMPTY;
    }
    char *host = nullptr;
    uc = curl_url_get(cu, CURLUPART_HOST, &host, 0);
    std::string result;
    if (uc == CURLUE_OK && host) {
        result = host;
        curl_free(host);
    }
    return result;
}
} // namespace NetManagerStandard
} // namespace OHOS
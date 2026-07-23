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

#include <regex>
#include "net_http_proxy_tracker.h"

#include "base64_utils.h"
#include "net_datashare_utils.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *EXCLUSIONS_SPLIT_SYMBOL = ",";
constexpr const char *DEFAULT_HTTP_PROXY_HOST = "NONE";
constexpr const char *DEFAULT_HTTP_PROXY_PORT = "0";
constexpr const char *DEFAULT_HTTP_PROXY_EXCLUSION_LIST = "NONE";
constexpr const char *USER_PROXY_HOST_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/"
    "USER_SETTINGSDATA_##USERID##?Proxy=true&key=global_proxy_host";
constexpr const char *USER_PROXY_PORT_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/"
    "USER_SETTINGSDATA_##USERID##?Proxy=true&key=global_proxy_port";
constexpr const char *USER_PROXY_EXCLUSIONS_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/"
    "USER_SETTINGSDATA_##USERID##?Proxy=true&key=global_proxy_exclusions";
constexpr const char *USER_URI_PATTERN = "##USERID##";
constexpr const char *GLOBAL_PROXY_PORT_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=global_proxy_port";
constexpr const char *GLOBAL_PROXY_EXCLUSIONS_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=global_proxy_exclusions";
constexpr const char *KEY_GLOBAL_PROXY_HOST = "settings.netmanager.proxy_host";
constexpr const char *KEY_GLOBAL_PROXY_PORT = "settings.netmanager.proxy_port";
constexpr const char *KEY_GLOBAL_PROXY_EXCLUSIONS = "settings.netmanager.proxy_exclusions";
} // namespace

void NetHttpProxyTracker::ReadFromSettingsData(HttpProxy &httpProxy)
{
    Uri hostUri(GLOBAL_PROXY_HOST_URI);
    Uri portUri(GLOBAL_PROXY_PORT_URI);
    Uri exclusionsUri(GLOBAL_PROXY_EXCLUSIONS_URI);
    KeyUri keyUri = {hostUri, portUri, exclusionsUri};
    ReadFromSettingsData(httpProxy, keyUri);
}

bool NetHttpProxyTracker::WriteToSettingsData(HttpProxy &httpProxy)
{
    Uri hostUri(GLOBAL_PROXY_HOST_URI);
    Uri portUri(GLOBAL_PROXY_PORT_URI);
    Uri exclusionsUri(GLOBAL_PROXY_EXCLUSIONS_URI);
    KeyUri keyUri = {hostUri, portUri, exclusionsUri};
    return WriteToSettingsData(httpProxy, keyUri);
}

void NetHttpProxyTracker::ReadFromSettingsDataUser(HttpProxy &httpProxy, int32_t userId)
{
    Uri hostUri(ReplaceUserIdForUri(USER_PROXY_HOST_URI, userId));
    Uri portUri(ReplaceUserIdForUri(USER_PROXY_PORT_URI, userId));
    Uri exclusionsUri(ReplaceUserIdForUri(USER_PROXY_EXCLUSIONS_URI, userId));
    KeyUri keyUri = {hostUri, portUri, exclusionsUri};
    ReadFromSettingsData(httpProxy, keyUri);
}

bool NetHttpProxyTracker::WriteToSettingsDataUser(HttpProxy &httpProxy, int32_t userId)
{
    Uri hostUri(ReplaceUserIdForUri(USER_PROXY_HOST_URI, userId));
    Uri portUri(ReplaceUserIdForUri(USER_PROXY_PORT_URI, userId));
    Uri exclusionsUri(ReplaceUserIdForUri(USER_PROXY_EXCLUSIONS_URI, userId));
    KeyUri keyUri = {hostUri, portUri, exclusionsUri};
    return WriteToSettingsData(httpProxy, keyUri);
}

void NetHttpProxyTracker::ReadFromSettingsData(HttpProxy &httpProxy, KeyUri keyUri)
{
    auto dataShareHelperUtils = std::make_unique<NetDataShareHelperUtils>();
    std::string proxyHost;
    std::string proxyPort;
    std::string proxyExclusions;
    int32_t ret = dataShareHelperUtils->Query(keyUri.hostUri_, KEY_GLOBAL_PROXY_HOST, proxyHost);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("Query global proxy host failed.");
    }
    std::string host = Base64::Decode(proxyHost);
    host = (host == DEFAULT_HTTP_PROXY_HOST ? "" : host);

    ret = dataShareHelperUtils->Query(keyUri.portUri_, KEY_GLOBAL_PROXY_PORT, proxyPort);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("Query global proxy port failed.");
    }
    uint16_t port = (proxyPort.empty() || host.empty()) ? 0 : static_cast<uint16_t>(CommonUtils::StrToUint(proxyPort));

    ret = dataShareHelperUtils->Query(keyUri.exclusionsUri_, KEY_GLOBAL_PROXY_EXCLUSIONS, proxyExclusions);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_D("Query global proxy exclusions failed.");
    }
    std::list<std::string> exclusionList =
        host.empty() ? std::list<std::string>() : ParseExclusionList(proxyExclusions);
    httpProxy = {host, port, exclusionList};
}

bool NetHttpProxyTracker::WriteToSettingsData(HttpProxy &httpProxy, KeyUri keyUri)
{
    std::string host =
        httpProxy.GetHost().empty() ? Base64::Encode(DEFAULT_HTTP_PROXY_HOST) : Base64::Encode(httpProxy.GetHost());
    auto dataShareHelperUtils = std::make_unique<NetDataShareHelperUtils>();
    int32_t ret = dataShareHelperUtils->Update(keyUri.hostUri_, KEY_GLOBAL_PROXY_HOST, host);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Set host to datashare failed");
        return false;
    }

    std::string port = httpProxy.GetHost().empty() ? DEFAULT_HTTP_PROXY_PORT : std::to_string(httpProxy.GetPort());
    ret = dataShareHelperUtils->Update(keyUri.portUri_, KEY_GLOBAL_PROXY_PORT, port);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Set port:%{public}s to datashare failed", port.c_str());
        return false;
    }

    std::string exclusions = GetExclusionsAsString(httpProxy.GetExclusionList());
    exclusions = (httpProxy.GetHost().empty() || exclusions.empty()) ? DEFAULT_HTTP_PROXY_EXCLUSION_LIST : exclusions;
    ret = dataShareHelperUtils->Update(keyUri.exclusionsUri_, KEY_GLOBAL_PROXY_EXCLUSIONS, exclusions);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Set exclusions:%{public}s to datashare", exclusions.c_str());
        return false;
    }
    httpProxy.SetExclusionList(ParseExclusionList(exclusions));
    if (!httpProxy.GetUsername().empty()) {
        auto userInfoHelp = NetProxyUserinfo::GetInstance();
        userInfoHelp.SaveHttpProxyHostPass(httpProxy);
    }
    return true;
}

std::list<std::string> NetHttpProxyTracker::ParseExclusionList(const std::string &exclusions) const
{
    std::list<std::string> exclusionList;
    if (exclusions.empty() || exclusions == DEFAULT_HTTP_PROXY_EXCLUSION_LIST) {
        return exclusionList;
    }
    size_t startPos = 0;
    size_t searchPos = exclusions.find(EXCLUSIONS_SPLIT_SYMBOL);
    std::string exclusion;
    while (searchPos != std::string::npos) {
        exclusion = exclusions.substr(startPos, (searchPos - startPos));
        exclusionList.push_back(exclusion);
        startPos = searchPos + 1;
        searchPos = exclusions.find(EXCLUSIONS_SPLIT_SYMBOL, startPos);
    }
    exclusion = exclusions.substr(startPos, (exclusions.size() - startPos));
    exclusionList.push_back(exclusion);
    return exclusionList;
}

std::string NetHttpProxyTracker::GetExclusionsAsString(const std::list<std::string> &exclusionList) const
{
    std::string exclusions;
    int32_t index = 0;
    for (const auto &exclusion : exclusionList) {
        if (exclusion.empty()) {
            continue;
        }
        if (index > 0) {
            exclusions = exclusions + EXCLUSIONS_SPLIT_SYMBOL;
        }
        exclusions = exclusions + exclusion;
        index++;
    }
    return exclusions;
}

std::string NetHttpProxyTracker::ReplaceUserIdForUri(const char *uri, int32_t userId)
{
    if (strlen(uri) <= 0) {
        return "";
    }
    std::regex pattern(USER_URI_PATTERN);
    return std::regex_replace(uri, pattern, std::to_string(userId));
}
} // namespace NetManagerStandard
} // namespace OHOS
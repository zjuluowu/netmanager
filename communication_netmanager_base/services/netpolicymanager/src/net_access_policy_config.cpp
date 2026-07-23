/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "net_access_policy_config.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <vector>

#include "config_policy_utils.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *PATH = "etc/netmanager/net_access_policy_config.json";
constexpr const char *ARRAY_NAME = "configs";
constexpr const char *ITEM_BUNDLE_NAME = "bundleName";
constexpr const char *ITEM_DISABLE_WLAN_SWITCH = "disableWlanSwitch";
constexpr const char *ITEM_DISABLE_CELLULAR_SWITCH = "disableCellularSwitch";
constexpr size_t MAX_NET_ACCESS_COUNT = 1000;
} // namespace
NetAccessPolicyConfigUtils &NetAccessPolicyConfigUtils::GetInstance()
{
    static NetAccessPolicyConfigUtils instance;
    return instance;
}

std::vector<NetAccessPolicyConfig> NetAccessPolicyConfigUtils::GetNetAccessPolicyConfig()
{
    std::lock_guard<ffrt::mutex> lock(lock_);
    Init();
    size_t totalSize = netAccessPolicyConfigs_.size() + dynamicNetAccessPolicyConfigs_.size();
    // LCOV_EXCL_START
    if (totalSize > MAX_NET_ACCESS_COUNT) {
        NETMGR_LOG_W("Total configs exceed limit(%{public}zu)", totalSize);
        return netAccessPolicyConfigs_;
    }
    // LCOV_EXCL_STOP

    std::vector<NetAccessPolicyConfig> result;
    result.reserve(totalSize);
    
    result.insert(result.end(), netAccessPolicyConfigs_.begin(), netAccessPolicyConfigs_.end());
    
    for (const auto &config : dynamicNetAccessPolicyConfigs_) {
        result.push_back(config.second);
    }
    return result;
}

void NetAccessPolicyConfigUtils::AddNetAccessPolicyConfig(const std::vector<std::string> &bundleNames)
{
    if (bundleNames.empty()) {
        NETMGR_LOG_W("bundle names is empty.");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(lock_);
    for (const auto &bundleName : bundleNames) {
        if (dynamicNetAccessPolicyConfigs_.find(bundleName) != dynamicNetAccessPolicyConfigs_.end()) {
            NETMGR_LOG_W("Bundle: %{public}s has already been added.", bundleName.c_str());
            continue;
        }
        NetAccessPolicyConfig config;
        config.bundleName = bundleName;
        config.disableWlanSwitch = true;
        config.disableCellularSwitch = true;
        dynamicNetAccessPolicyConfigs_.emplace(bundleName, config);
    }
}

void NetAccessPolicyConfigUtils::RemoveNetAccessPolicyConfig(const std::vector<std::string> &bundleNames)
{
    if (bundleNames.empty()) {
        NETMGR_LOG_W("bundle names is empty.");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(lock_);
    for (const auto &bundleName : bundleNames) {
        if (dynamicNetAccessPolicyConfigs_.erase(bundleName) == 0) {
            NETMGR_LOG_W("Bundle: %{public}s not found.", bundleName.c_str());
        }
    }
}

void NetAccessPolicyConfigUtils::Init()
{
    if (isInit_) {
        return;
    }
    netAccessPolicyConfigs_.clear();
    ParseNetAccessPolicyConfigs();
    isInit_ = true;
}

void NetAccessPolicyConfigUtils::ParseNetAccessPolicyConfigs()
{
    std::string content;
    if (!ReadFile(content, PATH)) {
        NETMGR_LOG_E("read json file failed.");
        return;
    }
    if (content.empty()) {
        NETMGR_LOG_E("read content is empty.");
        return;
    }
    cJSON *root = cJSON_Parse(content.c_str());
    if (root == nullptr) {
        NETMGR_LOG_E("json root is nullptr.");
        return;
    }
    cJSON *configs = cJSON_GetObjectItem(root, ARRAY_NAME);
    if (configs == nullptr || !cJSON_IsArray(configs) || cJSON_GetArraySize(configs) == 0) {
        cJSON_Delete(root);
        configs = nullptr;
        root = nullptr;
        return;
    }

    cJSON *item = nullptr;
    for (int i = 0; i < cJSON_GetArraySize(configs); i++) {
        item = cJSON_GetArrayItem(configs, i);
        if (item == nullptr) {
            NETMGR_LOG_E("config item is nullptr.");
            continue;
        }
        NetAccessPolicyConfig tmp;
        tmp.bundleName = ParseString(cJSON_GetObjectItem(item, ITEM_BUNDLE_NAME));
        tmp.disableWlanSwitch = ParseBoolean(cJSON_GetObjectItem(item, ITEM_DISABLE_WLAN_SWITCH));
        tmp.disableCellularSwitch = ParseBoolean(cJSON_GetObjectItem(item, ITEM_DISABLE_CELLULAR_SWITCH));
        netAccessPolicyConfigs_.push_back(tmp);
    }

    cJSON_Delete(root);
    configs = nullptr;
    root = nullptr;
}

bool NetAccessPolicyConfigUtils::ReadFile(std::string &content, const std::string &fileName)
{
    char buf[PATH_MAX];
    char* cfgFilePath = GetOneCfgFile(fileName.c_str(), buf, PATH_MAX);
    char realPath[PATH_MAX] = {0};
    if (!cfgFilePath || strlen(cfgFilePath) == 0 || strlen(cfgFilePath) > PATH_MAX ||
        !realpath(cfgFilePath, realPath)) {
        NETMGR_LOG_E("file does not exist");
        return false;
    }

    struct stat st;
    if (stat(realPath, &st) != 0) {
        NETMGR_LOG_E("stat file fail");
        return false;
    }

    std::fstream file(realPath, std::fstream::in);
    if (!file.is_open()) {
        NETMGR_LOG_E("file open fail");
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    file.close();
    return true;
}

std::string NetAccessPolicyConfigUtils::ParseString(cJSON *value)
{
    if (cJSON_IsString(value) && value->valuestring != nullptr) {
        return value->valuestring;
    }
    return "";
}

bool NetAccessPolicyConfigUtils::ParseBoolean(cJSON *value)
{
    if (cJSON_IsBool(value)) {
        return cJSON_IsTrue(value);
    }
    return false;
}

int32_t NetAccessPolicyConfigUtils::ParseInt32(cJSON *value)
{
    if (cJSON_IsNumber(value)) {
        return value->valueint;
    }
    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS
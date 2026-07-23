/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "net_policy_file.h"

#include <fcntl.h>
#include <string>

#include "net_manager_center.h"
#include "net_mgr_log_wrapper.h"
#include "net_policy_file_event_handler.h"
#include "net_policy_inner_define.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {
bool CheckFilePath(const std::string &fileName, std::string &realPath)
{
    char tmpPath[PATH_MAX] = {0};
    if (!realpath(fileName.c_str(), tmpPath)) {
        NETMGR_LOG_E("file name is illegal");
        return false;
    }
    if (strcmp(tmpPath, POLICY_FILE_NAME) != 0) {
        NETMGR_LOG_E("file path is illegal");
        return false;
    }
    realPath = tmpPath;
    return true;
}
} // namespace
constexpr const char *NET_POLICY_WORK_THREAD = "NET_POLICY_FILE_WORK_THREAD";

NetPolicyFile::NetPolicyFile()
{
    InitPolicy();
}

NetPolicyFile::~NetPolicyFile() = default;

bool NetPolicyFile::ReadFile(const std::string &fileName)
{
    NETMGR_LOG_D("read [%{public}s] from disk.", fileName.c_str());
    struct stat st;
    if (stat(fileName.c_str(), &st) != 0) {
        NETMGR_LOG_E("stat file fail");
        return false;
    }

    std::string realPath;
    if (!CheckFilePath(fileName, realPath)) {
        NETMGR_LOG_E("file does not exist");
        return false;
    }

    std::fstream file(realPath.c_str(), std::fstream::in);
    if (!file.is_open()) {
        NETMGR_LOG_E("file open fail");
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileContent = buffer.str();
    file.close();
    return Json2Obj(fileContent, netPolicy_);
}

bool NetPolicyFile::ReadFile()
{
    return ReadFile(POLICY_FILE_NAME) || ReadFile(POLICY_FILE_BAK_NAME);
}

bool NetPolicyFile::WriteFile()
{
    auto data = std::make_shared<PolicyFileEvent>();
    Obj2Json(netPolicy_, data->json);
    auto event = AppExecFwk::InnerEvent::Get(NetPolicyFileEventHandler::MSG_POLICY_FILE_WRITE, data);
    auto handler = GetHandler();
    if (!handler) {
        NETMGR_LOG_E("NetPolicyFileEventHandler not existed");
        return false;
    }
    handler->SendWriteEvent(event);
    return true;
}

const std::vector<UidPolicy> &NetPolicyFile::ReadUidPolicies()
{
    return netPolicy_.uidPolicies;
}

void NetPolicyFile::ParseUidPolicy(const cJSON* const root, NetPolicy &netPolicy)
{
    cJSON *netUidPolicy = cJSON_GetObjectItem(root, CONFIG_UID_POLICY);
    if (netUidPolicy == nullptr) {
        return;
    }
    uint32_t size = cJSON_GetArraySize(netUidPolicy);
    UidPolicy uidPolicy;
    for (uint32_t i = 0; i < size; i++) {
        cJSON *uidPolicyItem = cJSON_GetArrayItem(netUidPolicy, i);
        if (uidPolicyItem == nullptr) {
            NETMGR_LOG_E("uidPolicyItem is null");
            continue;
        }
        cJSON *uid = cJSON_GetObjectItem(uidPolicyItem, CONFIG_UID);
        uidPolicy.uid = cJSON_GetStringValue(uid);
        NETMGR_LOG_D("uid: %{public}s", uidPolicy.uid.c_str());
        cJSON *policy = cJSON_GetObjectItem(uidPolicyItem, CONFIG_POLICY);
        uidPolicy.policy = cJSON_GetStringValue(policy);
        NETMGR_LOG_D("policy: %{public}s", uidPolicy.policy.c_str());
        netPolicy.uidPolicies.push_back(uidPolicy);
    }
}

void NetPolicyFile::ParseBackgroundPolicy(const cJSON* const root, NetPolicy &netPolicy)
{
    cJSON *netBackgroundPolicy = cJSON_GetObjectItem(root, CONFIG_BACKGROUND_POLICY);
    if (netBackgroundPolicy != nullptr) {
        cJSON *status = cJSON_GetObjectItem(netBackgroundPolicy, CONFIG_BACKGROUND_POLICY_STATUS);
        netPolicy.backgroundPolicyStatus = cJSON_GetStringValue(status);
        NETMGR_LOG_D("backgroundPolicyStatus: %{public}s", netPolicy.backgroundPolicyStatus.c_str());
    }
}

void NetPolicyFile::ParseQuotaPolicy(const cJSON* const root, NetPolicy &netPolicy)
{
    cJSON *netQuotaPolicy = cJSON_GetObjectItem(root, CONFIG_QUOTA_POLICY);
    if (netQuotaPolicy == nullptr) {
        return;
    }
    NetPolicyQuota quotaPolicy;
    uint32_t size = cJSON_GetArraySize(netQuotaPolicy);
    NETMGR_LOG_D("netQuotaPolicy size: %{public}u", size);
    for (uint32_t i = 0; i < size; i++) {
        cJSON *quotaPolicyItem = cJSON_GetArrayItem(netQuotaPolicy, i);
        if (quotaPolicyItem == nullptr) {
            NETMGR_LOG_E("quotaPolicyItem is null");
            continue;
        }
        cJSON *netType = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_NETTYPE);
        quotaPolicy.netType = cJSON_GetStringValue(netType);
        cJSON *simId = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_SUBSCRIBERID);
        quotaPolicy.simId = cJSON_GetStringValue(simId);
        cJSON *periodStartTime = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_PERIODSTARTTIME);
        quotaPolicy.periodStartTime = cJSON_GetStringValue(periodStartTime);
        cJSON *periodDuration = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_PERIODDURATION);
        quotaPolicy.periodDuration = cJSON_GetStringValue(periodDuration);
        cJSON *warningBytes = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_WARNINGBYTES);
        quotaPolicy.warningBytes = cJSON_GetStringValue(warningBytes);
        cJSON *limitBytes = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_LIMITBYTES);
        quotaPolicy.limitBytes = cJSON_GetStringValue(limitBytes);
        cJSON *lastLimitSnooze = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_LASTLIMITSNOOZE);
        quotaPolicy.lastLimitSnooze = cJSON_GetStringValue(lastLimitSnooze);
        cJSON *metered = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_METERED);
        quotaPolicy.metered = cJSON_GetStringValue(metered);
        cJSON *ident = cJSON_GetObjectItem(quotaPolicyItem, CONFIG_QUOTA_POLICY_IDENT);
        quotaPolicy.ident = cJSON_GetStringValue(ident);
        NETMGR_LOG_D("netType:%{public}s, simId:%{public}s, perioST:%{public}s, perioDt:%{public}s, ident:%{public}s,\
                     warningBytes:%{public}s, limitBytes:%{public}s, lastLimitSnooze:%{public}s, metered:%{public}s",
                     quotaPolicy.netType.c_str(), quotaPolicy.simId.c_str(), quotaPolicy.periodStartTime.c_str(),
                     quotaPolicy.periodDuration.c_str(), quotaPolicy.ident.c_str(), quotaPolicy.warningBytes.c_str(),
                     quotaPolicy.limitBytes.c_str(), quotaPolicy.lastLimitSnooze.c_str(), quotaPolicy.metered.c_str());
        netPolicy.netQuotaPolicies.push_back(quotaPolicy);
    }
}

void NetPolicyFile::ParseFirewallRule(const cJSON* const root, NetPolicy &netPolicy)
{
    cJSON *netFirewallRules = cJSON_GetObjectItem(root, CONFIG_FIREWALL_RULE);
    if (netFirewallRules == nullptr) {
        return;
    }
    std::unique_lock<ffrt::mutex> lock(netFirewallRulesMutex_);
    uint32_t size = cJSON_GetArraySize(netFirewallRules);
    for (uint32_t i = 0; i < size; i++) {
        cJSON *firewallRulesItem = cJSON_GetArrayItem(netFirewallRules, i);
        std::string firewallRulesItemStr = firewallRulesItem->string;
        uint32_t chainType = CommonUtils::StrToUint(firewallRulesItemStr);
        cJSON *netDeniedList = cJSON_GetObjectItem(firewallRulesItem, CONFIG_FIREWALL_RULE_DENIEDLIST);
        cJSON *netAllowedList = cJSON_GetObjectItem(firewallRulesItem, CONFIG_FIREWALL_RULE_ALLOWEDLIST);
        uint32_t itemSize = cJSON_GetArraySize(netDeniedList);
        for (uint32_t j = 0; j < itemSize; j++) {
            cJSON *netDeniedListItem = cJSON_GetArrayItem(netDeniedList, j);
            std::string netDeniedListItemStr = cJSON_GetStringValue(netDeniedListItem);
            uint32_t deniedListNumber = CommonUtils::StrToUint(netDeniedListItemStr);
            NETMGR_LOG_D("netFirewallRules.deniedList: %{public}u", deniedListNumber);
            netPolicy.netFirewallRules[chainType].deniedList.insert(deniedListNumber);
        }
        itemSize = cJSON_GetArraySize(netAllowedList);
        for (uint32_t j = 0; j < itemSize; j++) {
            cJSON *netAllowedListItem = cJSON_GetArrayItem(netAllowedList, j);
            std::string netAllowedListItemStr = cJSON_GetStringValue(netAllowedListItem);
            uint32_t allowedListNumber = CommonUtils::StrToUint(netAllowedListItemStr);
            NETMGR_LOG_D("netFirewallRules.allowedList: %{public}u", allowedListNumber);
            netPolicy.netFirewallRules[chainType].allowedList.insert(allowedListNumber);
        }
    }
}

bool NetPolicyFile::Json2Obj(const std::string &content, NetPolicy &netPolicy)
{
    if (content.empty()) {
        return false;
    }

    cJSON *root = cJSON_Parse(content.c_str());
    if (root == nullptr) {
        return false;
    }

    cJSON *hosVersion = cJSON_GetObjectItem(root, CONFIG_HOS_VERSION);
    if (hosVersion == nullptr) {
        netPolicy.hosVersion = HOS_VERSION;
    } else {
        netPolicy.hosVersion = cJSON_GetStringValue(hosVersion);
        NETMGR_LOG_E("hosVersion: %{public}s", netPolicy.hosVersion.c_str());
    }

    // parse uid policy from file
    ParseUidPolicy(root, netPolicy);

    // parse background policy from file
    ParseBackgroundPolicy(root, netPolicy);

    // parse quota policy from file
    ParseQuotaPolicy(root, netPolicy);

    // parse firewall rule from file
    ParseFirewallRule(root, netPolicy);

    cJSON_Delete(root);
    return true;
}

bool NetPolicyFile::Obj2Json(const NetPolicy &netPolicy, std::string &content)
{
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }

    if (netPolicy_.hosVersion.empty()) {
        netPolicy_.hosVersion = HOS_VERSION;
    }
    cJSON_AddItemToObject(root, CONFIG_HOS_VERSION, cJSON_CreateString(netPolicy_.hosVersion.c_str()));
    AddUidPolicy(root);
    AddBackgroundPolicy(root);
    AddQuotaPolicy(root);
    AddFirewallRule(root);
    char *jsonStr = cJSON_Print(root);
    if (jsonStr == nullptr) {
        NETMGR_LOG_E("jsonStr write fail");
        cJSON_Delete(root);
        return false;
    }
    content = jsonStr;
    cJSON_Delete(root);
    cJSON_free(jsonStr);
    NETMGR_LOG_D("content: %{public}s", content.c_str());
    return true;
}

void NetPolicyFile::AddQuotaPolicy(cJSON *root)
{
    cJSON *quotaPolicy = cJSON_CreateArray();

    uint32_t size = netPolicy_.netQuotaPolicies.size();
    for (uint32_t i = 0; i < size; i++) {
        cJSON *quotaPolicyItem = cJSON_CreateObject();
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_NETTYPE,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].netType.c_str()));
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_SUBSCRIBERID,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].simId.c_str()));
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_PERIODSTARTTIME,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].periodStartTime.c_str()));
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_PERIODDURATION,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].periodDuration.c_str()));
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_WARNINGBYTES,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].warningBytes.c_str()));
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_LIMITBYTES,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].limitBytes.c_str()));
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_LASTLIMITSNOOZE,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].lastLimitSnooze.c_str()));
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_METERED,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].metered.c_str()));
        cJSON_AddItemToObject(quotaPolicyItem, CONFIG_QUOTA_POLICY_IDENT,
                              cJSON_CreateString(netPolicy_.netQuotaPolicies[i].ident.c_str()));
        cJSON_AddItemToArray(quotaPolicy, quotaPolicyItem);
    }

    cJSON_AddItemToObject(root, CONFIG_QUOTA_POLICY, quotaPolicy);
}

void NetPolicyFile::AddUidPolicy(cJSON *root)
{
    cJSON *uidPolicy = cJSON_CreateArray();

    uint32_t size = netPolicy_.uidPolicies.size();
    for (uint32_t i = 0; i < size; i++) {
        cJSON *uidPolicyItem = cJSON_CreateObject();
        cJSON_AddItemToObject(uidPolicyItem, CONFIG_UID, cJSON_CreateString(netPolicy_.uidPolicies[i].uid.c_str()));
        cJSON_AddItemToObject(uidPolicyItem, CONFIG_POLICY,
                              cJSON_CreateString(netPolicy_.uidPolicies[i].policy.c_str()));
        cJSON_AddItemToArray(uidPolicy, uidPolicyItem);
    }

    cJSON_AddItemToObject(root, CONFIG_UID_POLICY, uidPolicy);
}

void NetPolicyFile::AddBackgroundPolicy(cJSON *root)
{
    cJSON *backgroundPolicy = cJSON_CreateObject();

    if (netPolicy_.backgroundPolicyStatus.empty()) {
        netPolicy_.backgroundPolicyStatus = BACKGROUND_POLICY_ALLOW;
    }
    cJSON_AddItemToObject(backgroundPolicy, CONFIG_BACKGROUND_POLICY_STATUS,
                          cJSON_CreateString(netPolicy_.backgroundPolicyStatus.c_str()));
    cJSON_AddItemToObject(root, CONFIG_BACKGROUND_POLICY, backgroundPolicy);
}

void NetPolicyFile::AddFirewallRule(cJSON *root)
{
    std::unique_lock<ffrt::mutex> lock(netFirewallRulesMutex_);
    cJSON *firewallRuleObj = cJSON_CreateObject();
    for (auto &&[k, v] : netPolicy_.netFirewallRules) {
        NETMGR_LOG_D("read k[%{public}d].", k);
        cJSON *deniedListArr = cJSON_CreateArray();
        cJSON *allowedListArr = cJSON_CreateArray();
        cJSON *firewallRuleItem = cJSON_CreateObject();
        for (auto &it : v.deniedList) {
            cJSON_AddItemToArray(deniedListArr, cJSON_CreateString(std::to_string(it).c_str()));
        }
        for (auto &it : v.allowedList) {
            cJSON_AddItemToArray(allowedListArr, cJSON_CreateString(std::to_string(it).c_str()));
        }
        cJSON_AddItemToObject(firewallRuleItem, CONFIG_FIREWALL_RULE_DENIEDLIST, deniedListArr);
        cJSON_AddItemToObject(firewallRuleItem, CONFIG_FIREWALL_RULE_ALLOWEDLIST, allowedListArr);
        cJSON_AddItemToObject(firewallRuleObj, std::to_string(k).c_str(), firewallRuleItem);
    }
    cJSON_AddItemToObject(root, CONFIG_FIREWALL_RULE, firewallRuleObj);
}

uint32_t NetPolicyFile::ArbitrationWritePolicyToFile(uint32_t uid, uint32_t policy)
{
    uint32_t size = netPolicy_.uidPolicies.size();
    bool haveUidAndPolicy = false;
    uint32_t oldPolicy = NET_POLICY_NONE;
    for (uint32_t i = 0; i < size; i++) {
        auto uidTemp = CommonUtils::StrToUint(netPolicy_.uidPolicies[i].uid.c_str());
        if (uid == uidTemp) {
            haveUidAndPolicy = true;
            oldPolicy = uidTemp;
        }
    }

    if (haveUidAndPolicy) {
        if (oldPolicy != policy && policy == NET_POLICY_NONE) {
            return NET_POLICY_UID_OP_TYPE_DELETE;
        }

        if (oldPolicy != policy && policy != NET_POLICY_NONE) {
            return NET_POLICY_UID_OP_TYPE_UPDATE;
        }

        return NET_POLICY_UID_OP_TYPE_DO_NOTHING;
    }

    if (policy == NET_POLICY_NONE) {
        return NET_POLICY_UID_OP_TYPE_DO_NOTHING;
    }
    return NET_POLICY_UID_OP_TYPE_ADD;
}

void NetPolicyFile::WritePolicyByUid(uint32_t uid, uint32_t policy)
{
    uint32_t netUidPolicyOpType = ArbitrationWritePolicyToFile(uid, policy);
    WritePolicyByUid(netUidPolicyOpType, uid, policy);
}

void NetPolicyFile::WritePolicyByUid(uint32_t netUidPolicyOpType, uint32_t uid, uint32_t policy)
{
    NETMGR_LOG_D("Write File start, model:[%{public}u]", netUidPolicyOpType);
    if (netUidPolicyOpType == NetUidPolicyOpType::NET_POLICY_UID_OP_TYPE_UPDATE) {
        for (auto &uidPolicy : netPolicy_.uidPolicies) {
            if (uidPolicy.uid == std::to_string(uid)) {
                uidPolicy.policy = std::to_string(policy);
                break;
            }
        }
    } else if (netUidPolicyOpType == NetUidPolicyOpType::NET_POLICY_UID_OP_TYPE_DELETE) {
        for (auto iter = netPolicy_.uidPolicies.begin(); iter != netPolicy_.uidPolicies.end(); ++iter) {
            if (iter->uid == std::to_string(uid)) {
                netPolicy_.uidPolicies.erase(iter);
                break;
            }
        }
    } else if (netUidPolicyOpType == NetUidPolicyOpType::NET_POLICY_UID_OP_TYPE_ADD) {
        UidPolicy uidPolicy;
        uidPolicy.uid = std::to_string(uid);
        uidPolicy.policy = std::to_string(static_cast<uint32_t>(policy));
        netPolicy_.uidPolicies.push_back(uidPolicy);
    } else {
        NETMGR_LOG_D("Need to do nothing!");
    }

    WriteFile();
}

bool NetPolicyFile::UpdateQuotaPolicyExist(const NetQuotaPolicy &quotaPolicy)
{
    if (netPolicy_.netQuotaPolicies.empty()) {
        NETMGR_LOG_E("UpdateQuotaPolicyExist netQuotaPolicies is empty");
        return false;
    }

    for (uint32_t i = 0; i < netPolicy_.netQuotaPolicies.size(); ++i) {
        if (quotaPolicy.networkmatchrule.simId == netPolicy_.netQuotaPolicies[i].simId &&
            netPolicy_.netQuotaPolicies[i].netType == std::to_string(quotaPolicy.networkmatchrule.netType)) {
            netPolicy_.netQuotaPolicies[i].lastLimitSnooze = std::to_string(quotaPolicy.quotapolicy.lastLimitRemind);
            netPolicy_.netQuotaPolicies[i].limitBytes = std::to_string(quotaPolicy.quotapolicy.limitBytes);
            netPolicy_.netQuotaPolicies[i].metered = std::to_string(quotaPolicy.quotapolicy.metered);
            netPolicy_.netQuotaPolicies[i].netType = std::to_string(quotaPolicy.networkmatchrule.netType);
            netPolicy_.netQuotaPolicies[i].periodDuration = quotaPolicy.quotapolicy.periodDuration;
            netPolicy_.netQuotaPolicies[i].periodStartTime = std::to_string(quotaPolicy.quotapolicy.periodStartTime);
            netPolicy_.netQuotaPolicies[i].ident = quotaPolicy.networkmatchrule.ident;
            netPolicy_.netQuotaPolicies[i].simId = quotaPolicy.networkmatchrule.simId;
            netPolicy_.netQuotaPolicies[i].warningBytes = std::to_string(quotaPolicy.quotapolicy.warningBytes);
            return true;
        }
    }

    return false;
}

bool NetPolicyFile::WriteQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    netPolicy_.netQuotaPolicies.clear();
    uint32_t vSize = quotaPolicies.size();
    NetPolicyQuota quotaPolicy;
    for (uint32_t i = 0; i < vSize; i++) {
        if (UpdateQuotaPolicyExist(quotaPolicies[i])) {
            NETMGR_LOG_E("quotaPolicies:periodDuration[%{public}s], don't write this quotaPolicies!",
                         quotaPolicies[i].quotapolicy.periodDuration.c_str());
            continue;
        }
        quotaPolicy.lastLimitSnooze = std::to_string(quotaPolicies[i].quotapolicy.lastLimitRemind);
        quotaPolicy.limitBytes = std::to_string(quotaPolicies[i].quotapolicy.limitBytes);
        quotaPolicy.metered = std::to_string(quotaPolicies[i].quotapolicy.metered);
        quotaPolicy.ident = quotaPolicies[i].networkmatchrule.ident;
        quotaPolicy.netType = std::to_string(quotaPolicies[i].networkmatchrule.netType);
        quotaPolicy.periodDuration = quotaPolicies[i].quotapolicy.periodDuration;
        quotaPolicy.periodStartTime = std::to_string(quotaPolicies[i].quotapolicy.periodStartTime);
        quotaPolicy.simId = quotaPolicies[i].networkmatchrule.simId;
        quotaPolicy.warningBytes = std::to_string(quotaPolicies[i].quotapolicy.warningBytes);
        netPolicy_.netQuotaPolicies.push_back(quotaPolicy);
    }

    return WriteFile();
}

void NetPolicyFile::ReadQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies)
{
    NetQuotaPolicy quotaPolicyTmp;
    for (const auto &quotaPolicy : netPolicy_.netQuotaPolicies) {
        ToQuotaPolicy(quotaPolicy, quotaPolicyTmp);
        quotaPolicies.push_back(quotaPolicyTmp);
    }
}

int32_t NetPolicyFile::ReadFirewallRules(uint32_t chainType, std::set<uint32_t> &allowedList,
                                         std::set<uint32_t> &deniedList)
{
    std::unique_lock<ffrt::mutex> lock(netFirewallRulesMutex_);
    auto &&w = netPolicy_.netFirewallRules[chainType].allowedList;
    auto &&b = netPolicy_.netFirewallRules[chainType].deniedList;
    allowedList.insert(w.begin(), w.end());
    deniedList.insert(b.begin(), b.end());
    return NETMANAGER_SUCCESS;
}

void NetPolicyFile::WriteFirewallRules(uint32_t chainType, const std::set<uint32_t> &allowedList,
                                       const std::set<uint32_t> &deniedList)
{
    std::unique_lock<ffrt::mutex> lock(netFirewallRulesMutex_);
    netPolicy_.netFirewallRules[chainType].allowedList.clear();
    netPolicy_.netFirewallRules[chainType].deniedList.clear();
    netPolicy_.netFirewallRules[chainType].allowedList.insert(allowedList.begin(), allowedList.end());
    netPolicy_.netFirewallRules[chainType].deniedList.insert(deniedList.begin(), deniedList.end());
    lock.unlock();
    WriteFile();
}

int32_t NetPolicyFile::ResetPolicies()
{
    std::unique_lock<ffrt::mutex> lock(netFirewallRulesMutex_);
    netPolicy_.uidPolicies.clear();
    netPolicy_.backgroundPolicyStatus = BACKGROUND_POLICY_ALLOW;
    netPolicy_.netQuotaPolicies.clear();
    netPolicy_.netFirewallRules.clear();
    lock.unlock();
    WriteFile();

    return NETMANAGER_SUCCESS;
}

void NetPolicyFile::WriteBackgroundPolicy(bool backgroundPolicy)
{
    if (backgroundPolicy) {
        netPolicy_.backgroundPolicyStatus = BACKGROUND_POLICY_ALLOW;
    } else {
        netPolicy_.backgroundPolicyStatus = BACKGROUND_POLICY_REJECT;
    }

    WriteFile();
}

bool NetPolicyFile::ReadBackgroundPolicy()
{
    return netPolicy_.backgroundPolicyStatus == BACKGROUND_POLICY_ALLOW;
}

std::shared_ptr<NetPolicyFileEventHandler> NetPolicyFile::GetHandler()
{
    static auto handler = [this]() -> std::shared_ptr<NetPolicyFileEventHandler> {
        return std::make_shared<NetPolicyFileEventHandler>(NET_POLICY_WORK_THREAD);
    }();
    return handler;
}

bool NetPolicyFile::InitPolicy()
{
    ResetPolicies();
    return ReadFile();
}

void NetPolicyFile::RemoveInexistentUid(uint32_t uid)
{
    WritePolicyByUid(NetUidPolicyOpType::NET_POLICY_UID_OP_TYPE_DELETE, uid, 0);
}
} // namespace NetManagerStandard
} // namespace OHOS

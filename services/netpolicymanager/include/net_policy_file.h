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

#ifndef NET_POLICY_FILE_H
#define NET_POLICY_FILE_H

#include <climits>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "cJSON.h"
#include "singleton.h"

#include "netmanager_base_common_utils.h"
#include "net_policy_constants.h"
#include "net_policy_file_event_handler.h"
#include "net_policy_inner_define.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
enum NetUidPolicyOpType {
    NET_POLICY_UID_OP_TYPE_DO_NOTHING = 0,
    NET_POLICY_UID_OP_TYPE_ADD = 1,
    NET_POLICY_UID_OP_TYPE_DELETE = 2,
    NET_POLICY_UID_OP_TYPE_UPDATE = 3,
};

class NetPolicyFile : public std::enable_shared_from_this<NetPolicyFile> {
    DECLARE_DELAYED_SINGLETON(NetPolicyFile);

public:
    /**
     * Init by reading policy from file.
     * @return true Return true means init policy successful.
     * @return false Return false means init policy failed.
     */
    bool InitPolicy();

    /**
     * Reset policy to default.
     */
    int32_t ResetPolicies();

    /**
     * Used by net_policy_rule.cpp to get policy from file.
     *
     * @return const std::vector<UidPolicy>&
     */
    const std::vector<UidPolicy> &ReadUidPolicies();

    /**
     * Used by net_policy_rule.cpp to write policy to file.
     *
     * @param uid The specified UID of app.
     * @param policy The network policy for application.
     *      For details, see {@link NetUidPolicy}.
     */
    void WritePolicyByUid(uint32_t uid, uint32_t policy);

    /**
     * Used by net_policy_traffic.cpp to get quota policy from file.
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     */
    void ReadQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * Used by net_policy_rule.cpp to write quota policy to file.
     *
     * @param quotaPolicies  The list of network quota policy, {@link NetQuotaPolicy}.
     * @return true Return true means successful.
     * @return false Return false means failed.
     */
    bool WriteQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * Used by net_policy_rule.cpp to get background policy from file.
     *
     * @return true Return true means allow access net on background.
     * @return false Return false means reject access net on background.
     */
    bool ReadBackgroundPolicy();

    /**
     * Used by net_policy_rule.cpp to write background policy to file.
     *
     * @param allowBackground Allow or Reject access net on background.
     */
    void WriteBackgroundPolicy(bool allowBackground);

    /**
     * Used by net_policy_firewall.cpp to get firewall policy from file.
     *
     * @param chainType The firewall's type.Include "Powersave" or "DeviceIdle".
     * @param allowedList Firewall's allowed list.
     * @param deniedList Firewall's denied list.
     */
    int32_t ReadFirewallRules(uint32_t chainType, std::set<uint32_t> &allowedList, std::set<uint32_t> &deniedList);

    /**
     * Used by net_policy_firewall.cpp to write firewall policy from file.
     *
     * @param chainType The firewall's type.Include "Powersave" or "DeviceIdle".
     * @param allowedList Firewall's allowed list.
     * @param deniedList Firewall's denied list.
     */
    void WriteFirewallRules(uint32_t chainType, const std::set<uint32_t> &allowedList,
                            const std::set<uint32_t> &deniedList);

    /**
     * Used by net_policy_rule.cpp, when an app is removed from system,
     * this uid will be also remove from file.
     *
     * @param uid The specified UID of app that removed.
     */
    void RemoveInexistentUid(uint32_t uid);

private:
    bool Json2Obj(const std::string &content, NetPolicy &netPolicy);
    bool Obj2Json(const NetPolicy &netPolicy, std::string &content);

    bool ReadFile(const std::string &filePath);
    bool ReadFile();
    bool WriteFile();

    void AddUidPolicy(cJSON *root);
    void AddBackgroundPolicy(cJSON *root);
    void AddQuotaPolicy(cJSON *root);
    void AddFirewallRule(cJSON *root);

    void ParseUidPolicy(const cJSON* const root, NetPolicy &netPolicy);
    void ParseBackgroundPolicy(const cJSON* const root, NetPolicy &netPolicy);
    void ParseQuotaPolicy(const cJSON* const root, NetPolicy &netPolicy);
    void ParseFirewallRule(const cJSON* const root, NetPolicy &netPolicy);

    bool UpdateQuotaPolicyExist(const NetQuotaPolicy &quotaPolicy);
    uint32_t ArbitrationWritePolicyToFile(uint32_t uid, uint32_t policy);
    void WritePolicyByUid(uint32_t netUidPolicyOpType, uint32_t uid, uint32_t policy);

    inline void ToQuotaPolicy(const NetPolicyQuota& netPolicyQuota, NetQuotaPolicy &quotaPolicy)
    {
        quotaPolicy.quotapolicy.lastLimitRemind = CommonUtils::StrToLong(netPolicyQuota.lastLimitSnooze, REMIND_NEVER);
        quotaPolicy.quotapolicy.limitBytes = CommonUtils::StrToLong(netPolicyQuota.limitBytes, DATA_USAGE_UNKNOWN);
        quotaPolicy.quotapolicy.metered = CommonUtils::StrToBool(netPolicyQuota.metered, false);
        quotaPolicy.networkmatchrule.netType = CommonUtils::StrToInt(netPolicyQuota.netType, BEARER_DEFAULT);
        quotaPolicy.quotapolicy.periodDuration = netPolicyQuota.periodDuration;
        quotaPolicy.quotapolicy.periodStartTime = CommonUtils::StrToLong(netPolicyQuota.periodStartTime);
        quotaPolicy.networkmatchrule.simId = netPolicyQuota.simId;
        quotaPolicy.quotapolicy.warningBytes = CommonUtils::StrToLong(netPolicyQuota.warningBytes, DATA_USAGE_UNKNOWN);
        quotaPolicy.networkmatchrule.ident = netPolicyQuota.ident;
    }

    std::shared_ptr<NetPolicyFileEventHandler> GetHandler();
    ffrt::mutex netFirewallRulesMutex_;
public:
    NetPolicy netPolicy_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_FILE_H

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

#ifndef NET_POLICY_FIREWALL_H
#define NET_POLICY_FIREWALL_H

#include <shared_mutex>
#include "firewall_rule.h"
#include "net_policy_base.h"
#include "net_policy_file.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyFirewall : public NetPolicyBase {
public:
    NetPolicyFirewall() : deviceIdleMode_(false) {}
    void Init();

    /**
     * Set the UID into device idle allow list.
     *
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allow list or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetDeviceIdleTrustlist(const std::vector<uint32_t> &uids, bool isAllowed);

    /**
     * Get the allow list of UID in device idle mode.
     *
     * @param uids The list of UIDs
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetDeviceIdleTrustlist(std::vector<uint32_t> &uids);

    /**
     * Process network policy in device idle mode.
     *
     * @param enable Device idle mode is open or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t UpdateDeviceIdlePolicy(bool enable);

    /**
     * Reset network firewall rules.
     *
     */
    void ResetPolicies();

    /**
     * Set the Power Save Allowed List object.
     *
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allow list or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetPowerSaveTrustlist(const std::vector<uint32_t> &uids, bool isAllowed);

    /**
     * Get the Power Save Allowed List object.
     *
     * @param uids The list of UIDs.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetPowerSaveTrustlist(std::vector<uint32_t> &uids);

    /**
     * Process network policy in device idle mode.
     *
     * @param enable Power save mode is open or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t UpdatePowerSavePolicy(bool enable);

    /**
     * Process network policy in idle deny mode.
     *
     * @param enable idle deny mode is open or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t UpdateIdleDenyPolicy(bool enable);

    /**
     * Set the Idle Deny Allowed List object.
     *
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into add list or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd);

    /**
     * Handle the event from NetPolicyCore
     *
     * @param eventId The event id
     * @param policyEvent The informations passed from other core
     */
    void HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent);

private:
    void UpdateFirewallPolicyList(uint32_t chainType, const std::vector<uint32_t> &uids, bool isAllowed);
    void DeleteUid(uint32_t uid);
    std::string GetBundleNameForUid(uint32_t uid);
    std::string GetUidsBundleStr(const std::vector<uint32_t> &uids);
    void ReportFirewallPolicyChange(const std::string &callingFnc, uint32_t chainType, bool enable,
        std::shared_ptr<FirewallRule> &rule);

private:
    std::shared_ptr<FirewallRule> deviceIdleFirewallRule_;
    std::shared_ptr<FirewallRule> powerSaveFirewallRule_;
    std::shared_ptr<FirewallRule> idleDenyFirewallRule_;
    bool deviceIdleMode_ = false;
    bool powerSaveMode_ = false;
    bool idleDenyMode_ = false;
    std::set<uint32_t> deviceIdleAllowedList_;
    std::set<uint32_t> deviceIdleDeniedList_;
    std::set<uint32_t> powerSaveAllowedList_;
    std::set<uint32_t> powerSaveDeniedList_;
    std::shared_mutex listMutex_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_FIREWALL_H

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

#ifndef NET_POLICY_CLIENT_H
#define NET_POLICY_CLIENT_H

#include <map>
#include <shared_mutex>
#include "singleton.h"

#include "i_net_policy_service.h"
#include "net_access_policy.h"
#include "net_policy_constants.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyClient : public std::enable_shared_from_this<NetPolicyClient> {
public:
    NetPolicyClient();
    ~NetPolicyClient();
    static NetPolicyClient& GetInstance();
    /**
     * Set the network policy for the specified UID.
     *
     * @param uid The specified UID of app.
     * @param policy The network policy for application, for details, see {@link NetUidPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t SetPolicyByUid(uint32_t uid, uint32_t policy);

    /**
     * Get the network policy of the specified UID.
     *
     * @param uid The specified UID of app.
     * @param policy The network policy of the specified UID application, for details, see {@link NetUidPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy);

    /**
     * Get the application UIDs of the specified policy.
     *
     * @param policy the network policy of the current UID of application, for details, see {@link NetUidPolicy}.
     * @param uids put the result into uids
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids);

    /**
     * Get the status whether the specified uid app can access the metered network or non-metered network.
     *
     * @param uid The specified UID of application.
     * @param metered Indicates meterd network or non-metered network.
     * @param isAllowed Put the result into "isAllowed".
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed);

    /**
     * Get the status whether the specified uid app can access the specified iface network.
     *
     * @param uid The specified UID of application.
     * @param ifaceName Iface name.
     * @param isAllowed Put the result into "isAllowed".
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed);

    /**
     * @deprecated
     */
    [[nodiscard]] int32_t IsUidNetAccess(uint32_t uid, bool isMetered, bool &isAllowed);

    /**
     * @deprecated
     */
    [[nodiscard]] int32_t IsUidNetAccess(uint32_t uid, const std::string &ifaceName, bool &isAllowed);

    /**
     * Register network policy change callback.
     *
     * @param callback The callback of INetPolicyCallback interface.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback);

    /**
     * Unregister network policy change callback.
     *
     * @param callback The callback of INetPolicyCallback interface.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback);

    /**
     * Set network policies.
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * Get network policies.
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * SetFactoryPolicy reset policys for simId.
     *
     * @param simId ID, get from telephone module
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     * @deprecated
     */
    NetPolicyResultCode SetFactoryPolicy(const std::string &simId);

    /**
     * Reset network policies\rules\quota policies\firewall rules.
     *
     * @param simId Specify the matched simId of quota policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t ResetPolicies(const std::string &simId);

    /**
     * Control if apps can use data on background.
     *
     * @param isAllowed Allow apps to use data on background or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t SetBackgroundPolicy(bool isAllowed);

    /**
     * Get the status if apps can use data on background.
     *
     * @param backgroundPolicy Put the background policy's value
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t GetBackgroundPolicy(bool &backgroundPolicy);

    /**
     * Get the background network restriction policy for the specified uid.
     *
     * @param uid The specified UID of application.
     * @param backgroundPolicyOfUid The result of this uid's background policy,{@link NetBackgroundPolicy}
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid);

    /**
     * SetSnoozePolicy for Hibernate current policy
     *
     * @param netType {@link NetBearType}.
     * @param simId Specify the matched simId of quota policy when netType is cellular.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     * @deprecated
     */
    NetPolicyResultCode SetSnoozePolicy(int8_t netType, const std::string &simId);

    /**
     * Update the limit or warning remind time of quota policy.
     *
     * @param netType {@link NetBearType}.
     * @param simId Specify the matched simId of quota policy when netType is cellular.
     * @param remindType {@link RemindType}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t UpdateRemindPolicy(int32_t netType, const std::string &simId, uint32_t remindType);

    /**
     * SetIdleTrustlist for add trust list for Idle status
     *
     * @param uid uid
     * @param isTrustlist true/false
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     * @deprecated
     */
    NetPolicyResultCode SetIdleTrustlist(uint32_t uid, bool isTrustlist);

    /**
     * Set the UID into device idle allow list.
     *
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allow list or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t SetDeviceIdleTrustlist(const std::vector<uint32_t> &uid, bool isAllowed);

    /**
     * GetIdleTrustlist for get trust list for Idle status
     *
     * @param uids The uids are into into allow list
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     * @deprecated
     */
    NetPolicyResultCode GetIdleTrustlist(std::vector<uint32_t> &uids);

    /**
     * Get the allow list of UID in device idle mode.
     *
     * @param uids The list of UIDs
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t GetDeviceIdleTrustlist(std::vector<uint32_t> &uids);

    /**
     * Process network policy in device idle mode.
     *
     * @param enable Device idle mode is open or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetDeviceIdlePolicy(bool enable);

    /**
     * Get the allow list of UID in power save mode.
     *
     * @param uids The list of UIDs
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t GetPowerSaveTrustlist(std::vector<uint32_t> &uids);

    /**
     * Get the allow list of UID in power save mode.
     *
     * @param uid The list of UIDs
     * @param isAllowed The UID is into allow list or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t SetPowerSaveTrustlist(const std::vector<uint32_t> &uid, bool isAllowed);

    /**
     *  Set the Power Save Policy object
     *
     * @param enable Power save mode is open or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t SetPowerSavePolicy(bool enable);

    /**
     * Check if you have permission
     *
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    [[nodiscard]] int32_t CheckPermission();

    /**
     * Set the policy to access the network of the specified application.
     *
     * @param uid The specified UID of application.
     * @param policy The network access policy of application, {@link NetworkAccessPolicy}.
     * @param reconfirmFlag true means a reconfirm diaglog trigger while policy deny network access.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag);

    /**
     * Query the network access policy of the specified application or all applications.
     *
     * @param parameter Indicate to get all or an application network access policy, {@link AccessPolicyParameter}.
     * @param policy The network access policy of application, {@link AccessPolicySave}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetNetworkAccessPolicy(AccessPolicyParameter parameter, AccessPolicySave& policy);

    /**
     * Query the network access policy of the calling application.
     *
     * @param policy The network access policy of application, {@link NetAccessPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetSelfNetworkAccessPolicy(NetAccessPolicy& policy);

    int32_t NotifyNetAccessPolicyDiag(uint32_t uid);

    /**
     * Set NIC Traffic allowed or disallowed
     *
     * @param ifaceNames ifaceNames
     * @param status true for allowed, false for disallowed
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status);

    /**
     * Set internet access policy by sta ip for wifi hotpot
     *
     * @param ipAddr sta ip addr
     * @param family ipv4 or ipv6
     * @param accessInternet true for allowed, false for disallowed
     * @param clientNetIfName sta interface name, default as null
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetInternetAccessByIpForWifiShare(
        const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName = "");

    /**
     *  Set the Idle Deny Policy object
     *
     * @param enable idle deny mode is open or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    [[nodiscard]] int32_t SetIdleDenyPolicy(bool enable);

    /**
     * Get the allow list of UID in idle deny mode.
     *
     * @param uid The list of UIDs
     * @param isAdd The UID is into add list or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd);

    /**
     * Add applications' network access to be undisablable.
     *
     * @param bundleNames The bundle names of the applications to be added to the policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t AddNetworkAccessPolicy(const std::vector<std::string> &bundleNames);

    /**
     * Remove applications' network access to be undisablable.
     *
     * @param bundleNames The bundle names of the applications to be removed from the policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t RemoveNetworkAccessPolicy(const std::vector<std::string> &bundleNames);

private:
    class NetPolicyDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit NetPolicyDeathRecipient(NetPolicyClient &client) : client_(client) {}
        ~NetPolicyDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }

    private:
        NetPolicyClient &client_;
    };

private:
    sptr<INetPolicyService> GetProxy();
    void RecoverCallback();
    void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    std::mutex mutex_;
    sptr<INetPolicyService> netPolicyService_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    std::shared_mutex callbackMutex_;
    sptr<INetPolicyCallback> callback_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_CLIENT_H

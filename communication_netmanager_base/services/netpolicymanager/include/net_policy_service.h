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

#ifndef NET_POLICY_SERVICE_H
#define NET_POLICY_SERVICE_H

#include <atomic>
#include <mutex>

#include "event_runner.h"
#include "singleton.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "net_bundle.h"
#include "net_policy_callback.h"
#include "net_policy_event_handler.h"
#include "net_policy_firewall.h"
#include "net_policy_rule.h"
#include "net_policy_service_common.h"
#include "net_policy_service_stub.h"
#include "net_policy_traffic.h"
#include "net_access_policy.h"
#include "net_access_policy_rdb.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyService : public SystemAbility,
                         public NetPolicyServiceStub,
                         public std::enable_shared_from_this<NetPolicyService> {
    DECLARE_DELAYED_SINGLETON(NetPolicyService)
    DECLARE_SYSTEM_ABILITY(NetPolicyService)

public:
    void OnStart() override;
    void OnStop() override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    int32_t OnExtension(const std::string& extension, MessageParcel& data, MessageParcel& reply) override;

    /**
     * Set the network policy for the specified UID.
     * @param uid The specified UID of app.
     * @param policy The network policy for application.
     *      For details, see {@link NetUidPolicy}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetPolicyByUid(uint32_t uid, uint32_t policy) override;

    /**
     * Get the network policy of the specified UID.
     * @param uid The specified UID of app.
     * @param policy Return this uid's policy.
     *      For details, see {@link NetUidPolicy}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy) override;

    /**
     * Get the application UIDs of the specified policy.
     * @param policy the network policy of the current UID of application.
     *      For details, see {@link NetUidPolicy}.
     * @param uids The list of UIDs
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids) override;

    /**
     * Get the status whether the specified uid app can access the metered network or non-metered network.
     * @param uid The specified UID of application.
     * @param metered Indicates metered network or non-metered network.
     * @param isAllowed Return true means it's allowed to access the network.
     *      Return false means it's not allowed to access the network.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed) override;

    /**
     * Get the status whether the specified uid app can access the specified iface network.
     * @param uid The specified UID of application.
     * @param ifaceName Iface name.
     * @param isAllowed Return true means it's allowed to access the network.
     *      Return false means it's not allowed to access the network.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed) override;

    /**
     * Register network policy change callback.
     * @param callback Interface type pointer.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) override;

    /**
     * Unregister network policy change callback.
     * @param callback Interface type pointer.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) override;

    /**
     * Set policies by NetPolicyQuotaPolicy.
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies) override;

    /**
     * Get network policies.
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies) override;

    /**
     * Reset network policies\rules\quota policies\firewall rules.
     * @param simId Specify the matched simId of quota policy.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t ResetPolicies(const std::string &simId) override;

    /**
     * Control if apps can use data on background.
     * @param allowBackground Allow apps to use data on background.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetBackgroundPolicy(bool allowBackground) override;

    /**
     * Get the status if apps can use data on background.
     * @param backgroundPolicy True is allowed to use data on background.
     *      False is not allowed to use data on background.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetBackgroundPolicy(bool &backgroundPolicy) override;

    /**
     * Get the background network restriction policy for the specified uid.
     * @param uid uid The specified UID of application.
     * @param backgroundPolicyOfUid The specified UID of backgroundPolicy.
     *      For details, see {@link NetBackgroundPolicy}.
     * @return uint32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid) override;

    /**
     * Update the limit or warning remind time of quota policy.
     * @param netType {@link NetBearType}.
     * @param simId Specify the matched simId of quota policy when netType is cellular.
     * @param remindType {@link RemindType}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t UpdateRemindPolicy(int32_t netType, const std::string &simId, uint32_t remindType) override;

    /**
     * Set the UID into device idle allow list.
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allowed list or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetDeviceIdleTrustlist(const std::vector<uint32_t> &uids, bool isAllowed) override;

    /**
     * Get the allow list of UID in device idle mode.
     * @param uids The list of UIDs
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetDeviceIdleTrustlist(std::vector<uint32_t> &uids) override;

    /**
     * Process network policy in device idle mode.
     * @param enable Device idle mode is open or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetDeviceIdlePolicy(bool enable) override;

    /**
     * Get the allow list of UID in power save mode.
     *
     * @param uids The list of UIDs.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetPowerSaveTrustlist(std::vector<uint32_t> &uids) override;

    /**
     * Set the UID into power save allow list.
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allowed list or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetPowerSaveTrustlist(const std::vector<uint32_t> &uids, bool isAllowed) override;

    /**
     * Process network policy in Power Save mode.
     *
     * @param enable Power save mode is open or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetPowerSavePolicy(bool enable) override;

    /**
     * Check if you have permission
     *
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t CheckPermission() override;
	
	/**
     * factory reset net policies
     *
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t FactoryResetPolicies() override;

    /**
     * Set the policy to access the network of the specified application.
     *
     * @param uid The specified UID of application.
     * @param policy The network access policy of application, {@link NetworkAccessPolicy}.
     * @param reconfirmFlag true means a reconfirm diaglog trigger while policy deny network access.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag) override;

    /**
     * Query the network access policy of the specified application or all applications.
     *
     * @param parameter Indicate to get all or an application network access policy, {@link AccessPolicyParameter}.
     * @param policy The network access policy of application, {@link AccessPolicySave}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetNetworkAccessPolicy(AccessPolicyParameter parameter, AccessPolicySave& policy) override;

    /**
     * Add applications' network access to be undisablable.
     *
     * @param bundleNames The bundle names of the applications to be added to the policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t AddNetworkAccessPolicy(const std::vector<std::string> &bundleNames) override;
    
    /**
     * Remove applications' network access to be undisablable.
     *
     * @param bundleNames The bundle names of the applications to be removed from the policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t RemoveNetworkAccessPolicy(const std::vector<std::string> &bundleNames) override;

    /**
     * Query the network access policy of the calling application.
     *
     * @param policy The network access policy of application, {@link NetAccessPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetSelfNetworkAccessPolicy(NetAccessPolicy& policy) override;

    NetAccessPolicyRDB GetNetAccessPolicyDBHandler()
    {
        static NetAccessPolicyRDB netAccessPolicy;
        return netAccessPolicy;
    }
    int32_t DeleteNetworkAccessPolicy(uint32_t uid);
    int32_t NotifyNetAccessPolicyDiag(uint32_t uid) override;
    int32_t RefreshNetworkAccessPolicyFromConfig();

    /**
     * Set NIC Traffic allowed or disallowed
     *
     * @param ifaceNames ifaceNames
     * @param status true for allowed, false for disallowed
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status) override;

    int32_t SetInternetAccessByIpForWifiShare(
        const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName) override;

    int32_t SetIdleDenyPolicy(bool enable) override;

    int32_t SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd) override;

    void SetBrokerUidAccessPolicyMap(std::optional<uint32_t> uid);
    void DelBrokerUidAccessPolicyMap(uint32_t uid);
    int32_t OnRestoreSingleApp(const std::string &bundleName);

    int32_t OnBackup(MessageParcel& data, MessageParcel& reply);
    int32_t OnRestore(MessageParcel& data, MessageParcel& reply);

protected:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

private:
    void Init();
    int32_t GetDumpMessage(std::string &message);
    void ListenCommonEvent();

    void OnNetSysRestart();
    void ResetNetAccessPolicy();
    void UpdateNetAccessPolicyToMapFromDB();
    void OverwriteNetAccessPolicyToDBFromConfig();
    void UpdateNetworkAccessPolicyFromConfig(const std::string &bundleName, NetworkAccessPolicy &policy);
    int32_t GetActivatedOsAccountId(int32_t &userId);
    std::unordered_map<uint32_t, SampleBundleInfo> GetSampleBundleInfosForActiveUser();

private:
    enum ServiceRunningState {
        STATE_STOPPED = 0,
        STATE_RUNNING,
    };

    std::shared_ptr<NetPolicyTraffic> netPolicyTraffic_ = nullptr;
    std::shared_ptr<NetPolicyFirewall> netPolicyFirewall_ = nullptr;
    std::shared_ptr<NetPolicyRule> netPolicyRule_ = nullptr;
    std::shared_ptr<NetPolicyFile> netPolicyFile_ = nullptr;
    ServiceRunningState state_ = ServiceRunningState::STATE_STOPPED;
    std::shared_ptr<NetPolicyCore> netPolicyCore_ = nullptr;
    std::shared_ptr<NetPolicyCallback> netPolicyCallback_ = nullptr;
    sptr<NetPolicyServiceCommon> serviceComm_ = nullptr;
    std::mutex mutex_;
    std::vector<uint16_t> monthDay_;

    bool hasSARemoved_ = false;
    bool isDisplayTrafficAncoList_ = false;
    uint64_t netPolicySysTimerId_ = 0;

private:
    void RegisterFactoryResetCallback();

    class FactoryResetCallBack : public IRemoteStub<INetFactoryResetCallback> {
    public:
        FactoryResetCallBack(std::shared_ptr<NetPolicyService> netPolicy)
        {
            netPolicy_ = netPolicy;
        }

        int32_t OnNetFactoryReset()
        {
            if (netPolicy_ != nullptr) {
                netPolicy_->FactoryResetPolicies();
                return NETMANAGER_SUCCESS;
            } else {
                return NETMANAGER_ERR_LOCAL_PTR_NULL;
            }
        }
    private:
        std::shared_ptr<NetPolicyService> netPolicy_ = nullptr;
    };
    sptr<INetFactoryResetCallback> netFactoryResetCallback_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_SERVICE_H

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

#ifndef I_NET_POLICY_SERVICE_H
#define I_NET_POLICY_SERVICE_H

#include "iremote_broker.h"

#include "i_net_policy_callback.h"
#include "net_policy_constants.h"
#include "net_quota_policy.h"
#include "policy_ipc_interface_code.h"
#include "net_access_policy.h"

namespace OHOS {
namespace NetManagerStandard {
class INetPolicyService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetPolicyService");

public:
    /**
     * Set the network policy for the specified UID.
     *
     * @param uid The specified UID of app.
     * @param policy The network policy for application.
     *      For details, see {@link NetUidPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetPolicyByUid(uint32_t uid, uint32_t policy) = 0;

    /**
     * Get the network policy of the specified UID.
     *
     * @param uid The specified UID of app.
     * @param policy Return this uid's policy.
     *      For details, see {@link NetUidPolicy}.
     * int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy) = 0;

    /**
     * Get the application UIDs of the specified policy.
     *
     * @param policy the network policy of the current UID of application.
     *      For details, see {@link NetUidPolicy}.
     * @param uids The list of UIDs
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids) = 0;

    /**
     * Get the status whether the specified uid app can access the metered network or non-metered network.
     *
     * @param uid The specified UID of application.
     * @param metered Indicates metered network or non-metered network.
     * @param isAllowed True means it's allowed to access the network.
     *      False means it's not allowed to access the network.
     * @return Returns it's allowed or not to access the network.
     */
    virtual int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed) = 0;

    /**
     * Get the status whether the specified uid app can access the specified iface network.
     *
     * @param uid The specified UID of application.
     * @param ifaceName Iface name.
     * @param isAllowed True means it's allowed to access the network.
     *      False means it's not allowed to access the network.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed) = 0;

    /**
     * Register network policy change callback.
     *
     * @param callback The callback of INetPolicyCallback interface.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) = 0;

    /**
     * Unregister network policy change callback.
     *
     * @param callback The callback of INetPolicyCallback interface.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) = 0;

    /**
     * Set network policies.
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies) = 0;

    /**
     * Get network policies.
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies) = 0;

    /**
     * Update the limit or warning remind time of quota policy.
     *
     * @param netType {@link NetBearType}.
     * @param simId Specify the matched simId of quota policy when netType is cellular.
     * @param remindType {@link RemindType}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t UpdateRemindPolicy(int32_t netType, const std::string &simId, uint32_t remindType) = 0;

    /**
     * Set the UID into device idle allow list.
     *
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allow list or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetDeviceIdleTrustlist(const std::vector<uint32_t> &uids, bool isAllowed) = 0;

    /**
     * Get the allow list of UID in device idle mode.
     *
     * @param uids The list of UIDs
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetDeviceIdleTrustlist(std::vector<uint32_t> &uids) = 0;

    /**
     * Process network policy in device idle mode.
     *
     * @param enable Device idle mode is open or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetDeviceIdlePolicy(bool enable) = 0;

    /**
     * Reset network policies\rules\quota policies\firewall rules.
     *
     * @param simId Specify the matched simId of quota policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t ResetPolicies(const std::string &simId) = 0;

    /**
     * Control if apps can use data on background.
     *
     * @param isAllowed Allow apps to use data on background.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetBackgroundPolicy(bool isAllowed) = 0;

    /**
     * Get the status if apps can use data on background.
     *
     * @param backgroundPolicy True is allowed to use data on background.
     *      False is not allowed to use data on background.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetBackgroundPolicy(bool &backgroundPolicy) = 0;

    /**
     * Get the background network restriction policy for the specified uid.
     *
     * @param uid The specified UID of application.
     * @param backgroundPolicyOfUid The specified UID of backgroundPolicy.
     *      For details, see {@link NetBackgroundPolicy}.
     * @return uint32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid) = 0;

    /**
     * Get the Power Save Allowed List object
     *
     * @param uids The list of UIDs
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetPowerSaveTrustlist(std::vector<uint32_t> &uids) = 0;

    /**
     * Set the Power Save Allowed List object
     *
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allow list or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetPowerSaveTrustlist(const std::vector<uint32_t> &uids, bool isAllowed) = 0;

    /**
     * Set the Power Save Policy object
     *
     * @param enable Power save mode is open or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetPowerSavePolicy(bool enable) = 0;

    /**
     * Check if you have permission
     *
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t CheckPermission() = 0;

    /**
     * factory reset net policies
     *
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t FactoryResetPolicies() = 0;

    /**
     * Set the policy to access the network of the specified application.
     *
     * @param uid The specified UID of application.
     * @param policy The network access policy of application, {@link NetworkAccessPolicy}.
     * @param reconfirmFlag true means a reconfirm diaglog trigger while policy deny network access.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag) = 0;

    /**
     * Query the network access policy of the specified application or all applications.
     *
     * @param parameter Indicate to get all or an application network access policy, {@link AccessPolicyParameter}.
     * @param policy The network access policy of application, {@link AccessPolicySave}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetNetworkAccessPolicy(AccessPolicyParameter parameter, AccessPolicySave& policy) = 0;

    /**
     * Query the network access policy of the calling application.
     *
     * @param policy The network access policy of application, {@link NetAccessPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetSelfNetworkAccessPolicy(NetAccessPolicy& policy) = 0;

    virtual int32_t NotifyNetAccessPolicyDiag(uint32_t uid) = 0;

    /**
     * Set NIC Traffic allowed or disallowed
     *
     * @param ifaceNames ifaceNames
     * @param status true for allowed, false for disallowed
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status) = 0;

    /**
     * Set internet access policy by sta ip for wifi hotpot
     *
     * @param ipAddr sta ip addr
     * @param family ipv4 or ipv6
     * @param accessInternet true for allowed, false for disallowed
     * @param clientNetIfName sta interface name, default as null
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetInternetAccessByIpForWifiShare(
        const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName = "") = 0;
    
    /**
     *  Set the Idle Deny Policy object
     *
     * @param enable idle deny mode is open or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetIdleDenyPolicy(bool enable) = 0;

    /**
     * Get the allow list of UID in idle deny mode.
     *
     * @param uid The list of UIDs
     * @param isAdd The UID is into add list or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd) = 0;

    /**
     * Add applications' network access to be undisablable.
     *
     * @param bundleNames The bundle names of the applications to be added to the policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t AddNetworkAccessPolicy(const std::vector<std::string> &bundleNames) = 0;

    /**
     * Remove applications' network access to be undisablable.
     *
     * @param bundleNames The bundle names of the applications to be removed from the policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t RemoveNetworkAccessPolicy(const std::vector<std::string> &bundleNames) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_NET_POLICY_SERVICE_H

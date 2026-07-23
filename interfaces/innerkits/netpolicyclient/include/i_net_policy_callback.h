/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef I_NET_POLICY_CALLBACK_H
#define I_NET_POLICY_CALLBACK_H

#include <string>

#include "iremote_broker.h"

#include "net_policy_constants.h"
#include "net_quota_policy.h"
#include "policy_ipc_interface_code.h"

namespace OHOS {
namespace NetManagerStandard {
class INetPolicyCallback : public IRemoteBroker {
public:
    virtual ~INetPolicyCallback() = default;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetPolicyCallback");

public:
    /**
     * Notify the net uid policy change
     *
     * @param uid The specified UID of app.
     * @param policy The network policy for application.
     *      For details, see {@link NetUidPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t NetUidPolicyChange(uint32_t uid, uint32_t policy) = 0;

    /**
     * Notify the net uid rule change
     *
     * @param uid The specified UID of app.
     * @param rule The network rule for application.
     *      For details, see {@link NetUidRule}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t NetUidRuleChange(uint32_t uid, uint32_t rule) = 0;

    /**
     * Notify the quota policy change
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t NetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies) = 0;

    /**
     * Notify the metered ifaces change
     *
     * @param ifaces The vector of metered ifaces
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t NetMeteredIfacesChange(std::vector<std::string> &ifaces) = 0;

    /**
     * Notify the background policy change
     *
     * @param isBackgroundPolicyAllow The background is allow or not
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t NetBackgroundPolicyChange(bool isBackgroundPolicyAllow) = 0;

    /**
     * @deprecated
     */
    virtual int32_t NetStrategySwitch(const std::string &simId, bool enable) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_NET_POLICY_CALLBACK_H
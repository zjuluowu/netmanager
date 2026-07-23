/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_POLICY_CALLBACK_TEST_H
#define NET_POLICY_CALLBACK_TEST_H

#include <condition_variable>
#include <mutex>

#include "net_policy_callback_stub.h"
#include "net_policy_constants.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyCallbackTest : public NetPolicyCallbackStub {
public:
    NetPolicyCallbackTest();
    ~NetPolicyCallbackTest() override;
    int32_t NetUidPolicyChange(uint32_t uid, uint32_t policy) override;
    int32_t NetUidRuleChange(uint32_t uid, uint32_t rule) override;
    int32_t NetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies) override;
    int32_t NetMeteredIfacesChange(std::vector<std::string> &ifaces) override;
    int32_t NetBackgroundPolicyChange(bool isBackgroundPolicyAllow) override;
    void WaitFor(int32_t timeoutSecond);

    uint32_t GetPolicy() const
    {
        return uidPolicy_;
    }
    uint32_t GetUid() const
    {
        return uid_;
    }

    uint32_t GetRule() const
    {
        return rule_;
    }

    bool GetBackgroundPolicy() const
    {
        return isBackgroundPolicyAllow_;
    }

    uint32_t GetQuotaPoliciesSize() const
    {
        return quotaPoliciesSize_;
    }

private:
    void NotifyAll();
    uint32_t uidPolicy_ = 0;
    uint32_t uid_ = 0;
    uint32_t rule_ = 1 << 7;
    bool isBackgroundPolicyAllow_ = true;
    std::mutex callbackMutex_;
    std::condition_variable cv_;

    uint32_t quotaPoliciesSize_ = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_CALLBACK_TEST_H

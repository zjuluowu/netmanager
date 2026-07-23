/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_SUBSCRIBER_H
#define NET_STATS_SUBSCRIBER_H

#include "os_account_manager.h"

namespace OHOS {
namespace NetManagerStandard {

class NetStatsAccountSubscriber final : public AccountSA::OsAccountSubscriber {
public:
    NetStatsAccountSubscriber(const AccountSA::OsAccountSubscribeInfo &info) : AccountSA::OsAccountSubscriber(info) {};
    ~NetStatsAccountSubscriber() override;
    void OnStateChanged(const AccountSA::OsAccountStateData &data) override;
    void OnAccountsChanged(const int &id) override;
    void OnAccountsSwitch(const int &newId, const int &oldId) override;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif
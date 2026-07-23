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

#ifndef NET_STATS_TRAFFICLIMIT_DIALOG_H
#define NET_STATS_TRAFFICLIMIT_DIALOG_H

#include <mutex>
#include <iostream>

#include "ability_connect_callback_interface.h"
#include "ability_connect_callback_stub.h"

#include "timer.h"

namespace OHOS {
namespace NetManagerStandard {
class TrafficLimitDialog {
public:
    TrafficLimitDialog();
    ~TrafficLimitDialog();
    bool PopUpTrafficLimitDialog(int32_t simId);

private:
    DISALLOW_COPY_AND_MOVE(TrafficLimitDialog);
    class TrafficLimitAbilityConn : public OHOS::AAFwk::AbilityConnectionStub {
        void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
            int32_t resultCode) override;
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    public:
        void CloseDialog();
    private:
        std::shared_mutex remoteObjectMutex_;
        sptr<IRemoteObject> remoteObject_ = nullptr;
    };
    bool ShowTrafficLimitDialog();
    bool UnShowTrafficLimitDialog();
    sptr<TrafficLimitAbilityConn> trafficlimitAbilityConn_ = nullptr;
    std::mutex opMutex_;
    bool isDialogOpen_ = false;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif

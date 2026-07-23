/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NET_FACTORYRESET_CALLBACK_H
#define NET_FACTORYRESET_CALLBACK_H

#include <mutex>
#include <string>
#include <vector>

#include "singleton.h"
#include "refbase.h"
#include "ffrt.h"
#include "event_handler.h"
#include "i_net_factoryreset_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class NetFactoryResetCallback : public RefBase {

public:
    NetFactoryResetCallback();
    ~NetFactoryResetCallback() {}
    /**
     * Register net factory reset callback.
     * @param callback Interface type pointer.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetConnResultCode}.
     */
    int32_t RegisterNetFactoryResetCallbackAsync(const sptr<INetFactoryResetCallback> &callback);

    /**
     * Unregister net factory reset callback.
     * @param callback Interface type pointer.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetConnResultCode}.
     */
    int32_t UnregisterNetFactoryResetCallbackAsync(const sptr<INetFactoryResetCallback> &callback);

    /**
     * Notify network factory reset.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetConnResultCode}.
     */
    int32_t NotifyNetFactoryResetAsync();

private:
    int32_t RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback);
    int32_t UnregisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback);
    int32_t NotifyNetFactoryReset();

private:
    std::vector<sptr<INetFactoryResetCallback>> callbacks_;
    std::shared_ptr<ffrt::queue> factoryResetCallFfrtQueue_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_FACTORYRESET_CALLBACK_H

/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "net_conn_manager_test_util.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "net_mgr_log_wrapper.h"
#include "netsys_native_service_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
namespace NetConnManagerTestUtil {
sptr<NetsysNative::INetsysService> ConnManagerGetProxy()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return nullptr;
    }

    auto remote = samgr->GetSystemAbility(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    if (remote == nullptr) {
        return nullptr;
    }

    auto proxy = iface_cast<NetsysNative::INetsysService>(remote);
    if (proxy == nullptr) {
        return nullptr;
    }
    return proxy;
}
} // namespace NetConnManagerTestUtil
} // namespace NetManagerStandard
} // namespace OHOS
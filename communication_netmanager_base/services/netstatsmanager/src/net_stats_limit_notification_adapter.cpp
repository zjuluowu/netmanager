/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include <dlfcn.h>
#include <mutex>

#include "net_mgr_log_wrapper.h"
#include "net_stats_limit_notification_adapter.h"

namespace OHOS {
namespace NetManagerStandard {

static void *g_ntfDl = nullptr;
static std::mutex g_ntfDlLock;

bool g_registerNotificationOps(std::shared_ptr<INetMgrStatsLimitNotification> &notificationPtr)
{
    std::lock_guard<std::mutex> lock(g_ntfDlLock);

    if (notificationPtr != nullptr) {
        return true;
    }

    if (g_ntfDl == nullptr) {
        g_ntfDl = dlopen(NETMGR_STATS_LIMIT_NOTIFICATION_LIB, RTLD_LAZY);
        // LCOV_EXCL_START
        if (g_ntfDl == nullptr) {
            NETMGR_LOG_E("%{public}s load failed, error: %{public}s", NETMGR_STATS_LIMIT_NOTIFICATION_LIB, dlerror());
            return false;
        }
        // LCOV_EXCL_STOP
        NETMGR_LOG_I("Success to load %{public}s", NETMGR_STATS_LIMIT_NOTIFICATION_LIB);
    }

    (void)dlerror();

    GetNotificationInstancePtrFunc getInstancePtr = (GetNotificationInstancePtrFunc)dlsym(
        g_ntfDl, "_ZN4OHOS18NetManagerStandard31NetMgrNetStatsLimitNotification14GetInstancePtrEv");
    // LCOV_EXCL_START
    if (getInstancePtr == nullptr) {
        NETMGR_LOG_E("get dynamic symbol failed, error: %{public}s", dlerror());
        if (dlclose(g_ntfDl)) {
            NETMGR_LOG_W("%{public}s close failed, error: %{public}s", NETMGR_STATS_LIMIT_NOTIFICATION_LIB, dlerror());
        }
        g_ntfDl = nullptr;
        return false;
    }
    // LCOV_EXCL_STOP

    notificationPtr = getInstancePtr();
    // LCOV_EXCL_START
    if (notificationPtr == nullptr) {
        NETMGR_LOG_E("GetNotificationInstancePtr returned null");
        if (dlclose(g_ntfDl)) {
            NETMGR_LOG_W("%{public}s close failed, error: %{public}s", NETMGR_STATS_LIMIT_NOTIFICATION_LIB, dlerror());
        }
        g_ntfDl = nullptr;
        return false;
    }
    // LCOV_EXCL_STOP

    return true;
}

// LCOV_EXCL_START
void UnregisterNotificationOps(std::shared_ptr<INetMgrStatsLimitNotification> &notificationPtr)
{
    std::lock_guard<std::mutex> lock(gNtfDlLock);

    if (g_ntfDl == nullptr) {
        notificationPtr = nullptr;
        return;
    }

    notificationPtr = nullptr;
    int32_t ret = dlclose(g_ntfDl);
    if (ret != 0) {
        NETMGR_LOG_W("%{public}s close failed, error: %{public}s", NETMGR_STATS_LIMIT_NOTIFICATION_LIB, dlerror());
    }
    g_ntfDl = nullptr;
}
// LCOV_EXCL_STOP

} // namespace NetManagerStandard
} // namespace OHOS

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

#ifndef NET_STATS_LIMIT_NOTIFICATION_ADAPTER_H
#define NET_STATS_LIMIT_NOTIFICATION_ADAPTER_H

#include <memory>

#include "net_stats_notification_interface.h"

namespace OHOS {
namespace NetManagerStandard {

constexpr const char NETMGR_STATS_LIMIT_NOTIFICATION_LIB[] = "libnetmanager_stats_limit_notification.z.so";

using GetNotificationInstancePtrFunc = std::shared_ptr<INetMgrStatsLimitNotification> (*)();

bool g_registerNotificationOps(std::shared_ptr<INetMgrStatsLimitNotification> &notificationPtr);
void UnregisterNotificationOps(std::shared_ptr<INetMgrStatsLimitNotification> &notificationPtr);

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_LIMIT_NOTIFICATION_ADAPTER_H

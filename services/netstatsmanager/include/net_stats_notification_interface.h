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

#ifndef NET_STATS_NOTIFICATION_INTERFACE_H
#define NET_STATS_NOTIFICATION_INTERFACE_H

#include <string>

namespace OHOS {
namespace NetManagerStandard {

typedef void (*NetMgrStatsLimitNtfCallback)(int notificationId);

enum NetMgrStatsLimitNotificationId : int {
    NETMGR_STATS_LIMIT_DAY = 115800,
    NETMGR_STATS_LIMIT_MONTH = 115801,
    NETMGR_STATS_ALERT_MONTH = 115802,
};

class INetMgrStatsLimitNotification {
public:
    virtual ~INetMgrStatsLimitNotification() = default;
    virtual void PublishNetStatsLimitNotification(int notificationId, int32_t simId, bool isDualCard) = 0;
    virtual void RegNotificationCallback(NetMgrStatsLimitNtfCallback callback) = 0;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_NOTIFICATION_INTERFACE_H

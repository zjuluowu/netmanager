/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_NOTIFICATION_H
#define NET_STATS_NOTIFICATION_H

#include <mutex>
#include <string>

#include "image_source.h"
#include "pixel_map.h"
#include "notification_helper.h"
#include "number_format.h"

#include "net_stats_notification_interface.h"

static const int32_t COMM_NETSYS_NATIVE_SYS_ABILITY_ID = 1158;

namespace OHOS {
namespace NetManagerStandard {
class NetMgrNetStatsLimitNotification : public INetMgrStatsLimitNotification {

public:
    NetMgrNetStatsLimitNotification();
    ~NetMgrNetStatsLimitNotification();
    static std::shared_ptr<NetMgrNetStatsLimitNotification> GetInstance();
    static std::shared_ptr<INetMgrStatsLimitNotification> GetInstancePtr(void);

    void PublishNetStatsLimitNotification(int notificationId, int32_t simId, bool isDaulCard) override;
    void RegNotificationCallback(NetMgrStatsLimitNtfCallback callback) override;

private:
    NetMgrNetStatsLimitNotification(const NetMgrNetStatsLimitNotification&) = delete;
    NetMgrNetStatsLimitNotification &operator=(const NetMgrNetStatsLimitNotification&) = delete;

    void GetPixelMap();
    void UpdateResourceMap();
    void ParseJSONFile(const std::string& filePath, std::map<std::string, std::string>& container);
    std::unique_ptr<OHOS::Global::I18n::NumberFormat> GetNumberFormatter();

    std::string GetDayNotificationText();
    std::string GetMonthNotificationText();
    std::string GetMonthAlertText();
    std::string GetNotificationTitle(std::string &notificationType);
    bool SetTitleAndText(int notificationId, std::shared_ptr<Notification::NotificationNormalContent> content,
                         bool isDaulCard);
    std::string GetTrafficNum(double traffic);

    void SetWantAgent(Notification::NotificationRequest &request);

    std::shared_ptr<Media::PixelMap> netmgrStatsLimitIconPixelMap_{};
    std::map<std::string, std::string> resourceMap;
    std::map<std::string, std::string> languageMap;
    std::string localeBaseName;
    std::mutex mutex_;
    const int NETMGR_TRAFFIC_NTF_CONTROL_FLAG = 0;
    int32_t simId_ = 0;

    static std::mutex instanceLock_;
    static std::shared_ptr<NetMgrNetStatsLimitNotification> instance_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMGR_LIMIT_NOTIFICATION_H

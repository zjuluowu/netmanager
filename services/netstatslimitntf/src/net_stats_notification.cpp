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
 * WITHstrNum WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "net_stats_notification.h"
#include "net_stats_service.h"

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "cJSON.h"
#include "file_ex.h"
#include "locale_config.h"
#include "locale_info.h"
#include "locale_matcher.h"
#include "securec.h"
#include "want_agent_helper.h"
#include "want_agent_info.h"

#include "core_service_client.h"
#include "image_source.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_utils.h"
#include "notification_normal_content.h"
#include "pixel_map.h"
#include "net_manager_center.h"

namespace OHOS {
namespace NetManagerStandard {

static const int NTF_AUTO_DELETE_TIME = 10000;

// static const int COMM_NETSYS_NATIVE_SYS_ABILITY_ID = 1158

static const int32_t SLOT_ID_00 = 0;
static const int32_t CARD_ID_01 = 1;
static const int32_t CARD_ID_02 = 2;

static const int32_t UNIT_CONVERT_1024 = 1024;
static const int32_t TWO_PRECISION = 2;

static const int32_t TWO_CHAR = 2;

// keys in json file
static constexpr const char *KEY_STRING = "string";
static constexpr const char *KEY_NAME = "name";
static constexpr const char *KEY_VALUE = "value";
static constexpr const char *KEY_NETWORK_MONTH_LIMIT_SINGLE_TITLE = "netstats_excess_monthlimit_notofication_title";
static constexpr const char *KEY_NETWORK_MONTH_MARK_SINGLE_TITLE = "netstats_excess_monthmark_notofication_title";
static constexpr const char *KEY_NETWORK_DAY_MARK_SINGLE_TITLE = "netstats_excess_daymark_notofication_title";

static constexpr const char *KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE = "netstats_excess_monthlimit_notofication_title_sub";
static constexpr const char *KEY_NETWORK_MONTH_MARK_DUAL_TITLE = "netstats_excess_monthmark_notofication_title_sub";
static constexpr const char *KEY_NETWORK_DAY_MARK_DUAL_TITLE = "netstats_excess_daymark_notofication_title_sub";

// eSIM related title keys
static constexpr const char *KEY_NETWORK_MONTH_LIMIT_ESIM_TITLE =
    "netstats_excess_monthlimit_notofication_title_sub_esim";
static constexpr const char *KEY_NETWORK_MONTH_MARK_ESIM_TITLE =
    "netstats_excess_monthmark_notofication_title_sub_esim";
static constexpr const char *KEY_NETWORK_DAY_MARK_ESIM_TITLE = "netstats_excess_daymark_notofication_title_sub_esim";

static constexpr const char *KEY_MONTH_LIMIT_TEXT = "netstats_month_limit_message";
static constexpr const char *KEY_MONTH_NOTIFY_TEXT = "netstats_month_notify_message";
static constexpr const char *KEY_DAILY_NOTIFY_TEXT = "netstats_daily_notify_message";

// NOTE: icon and json path must be absolute path
// all locales are listed at: global_i18n-master\global_i18n-master\frameworks\intl\etc\supported_locales.xml
static constexpr const char *NETWORK_ICON_PATH = "//system/etc/netmanager_base/resources/network_ic.png";
static constexpr const char *DEFAULT_LANGUAGE_NAME_EN = "base";
static constexpr const char *LOCALE_TO_RESOURCE_PATH =
    "//system/etc/netmanager_base/resources/locale_to_resourcePath.json";
static constexpr const char *LANGUAGE_RESOURCE_PARENT_PATH = "//system/etc/netmanager_base/resources/";
static constexpr const char *LANGUAGE_RESOURCE_CHILD_PATH = "/element/string.json";
static constexpr const char *SETTING_BUNDLE_NAME = "com.huawei.hmos.settings";
static constexpr const char *SETTING_ABILITY_NAME = "com.huawei.hmos.settings.MainAbility";
static constexpr const char *URI = "settings://mobile_data_manage_entry";

static std::mutex g_callbackMutex{};
static NetMgrStatsLimitNtfCallback g_netMgrStatsLimitNtfCallback = nullptr;

void NetMgrNetStatsLimitNotification::ParseJSONFile(
    const std::string& filePath, std::map<std::string, std::string>& container)
{
    std::string content;
    LoadStringFromFile(filePath, content);

    cJSON *json = cJSON_Parse(content.c_str());
    if (json == nullptr) {
        NETMGR_LOG_I("ParseJSONFile: json null. filepath = %{public}s", filePath.c_str());
        return;
    }

    cJSON *resJson = cJSON_GetObjectItemCaseSensitive(json, KEY_STRING);

    if (resJson == nullptr) {
        NETMGR_LOG_I("ParseJSONFile: resJson null. filepath = %{public}s", filePath.c_str());
    } else {
        container.clear();
        cJSON *resJsonEach = nullptr;
        cJSON_ArrayForEach(resJsonEach, resJson)
        {
            cJSON *key = cJSON_GetObjectItemCaseSensitive(resJsonEach, KEY_NAME);
            cJSON *value = cJSON_GetObjectItemCaseSensitive(resJsonEach, KEY_VALUE);
            container.insert(std::pair<std::string, std::string>(key->valuestring, value->valuestring));
        }
    }

    cJSON_Delete(json);
}

void NetMgrNetStatsLimitNotification::UpdateResourceMap()
{
    OHOS::Global::I18n::LocaleInfo locale(Global::I18n::LocaleConfig::GetSystemLocale());
    std::string curBaseName = locale.GetBaseName();
    if (localeBaseName == curBaseName) {
        return;
    }

    NETMGR_LOG_I("UpdateResourceMap: change from %{public}s to %{public}s", localeBaseName.c_str(),
                 curBaseName.c_str());
    localeBaseName = curBaseName;

    std::string languagePath = DEFAULT_LANGUAGE_NAME_EN;
    if (languageMap.find(localeBaseName) != languageMap.end()) {
        languagePath = languageMap[localeBaseName];
    } else {
        for (auto &eachPair : languageMap) {
            OHOS::Global::I18n::LocaleInfo eachLocale(eachPair.first);
            if (OHOS::Global::I18n::LocaleMatcher::Match(&locale, &eachLocale)) {
                languagePath = eachPair.second;
                break;
            }
        }
    }

    std::string resourcePath = LANGUAGE_RESOURCE_PARENT_PATH + languagePath + LANGUAGE_RESOURCE_CHILD_PATH;
    char tmpPath[PATH_MAX] = {0};
    if (!realpath(resourcePath.c_str(), tmpPath)) {
        NETMGR_LOG_E("file name is illegal");
        return;
    }
    resourcePath = tmpPath;
    NETMGR_LOG_I("UpdateResourceMap: resourcePath = %{public}s", resourcePath.c_str());
    if (!std::filesystem::exists(resourcePath)) {
        NETMGR_LOG_E("resource path not exist: %{public}s", resourcePath.c_str());
        return;
    }
    /* 从resourcePath中拿到resourceMap */
    ParseJSONFile(resourcePath, resourceMap);
}

std::string NetMgrNetStatsLimitNotification::GetDayNotificationText()
{
    NETMGR_LOG_I("start NetMgrNetStatsLimitNotification::GetDayNotificationText, simId:%{public}d", simId_);
    if (resourceMap.find(KEY_DAILY_NOTIFY_TEXT) == resourceMap.end() ||
        resourceMap[KEY_DAILY_NOTIFY_TEXT].find("%s") == std::string::npos) {
        NETMGR_LOG_E("GetDayNotificationText error");
        return "";
    }

    std::string outText = resourceMap[KEY_DAILY_NOTIFY_TEXT];
    int32_t simId = simId_;
    uint64_t traffic = 0;
    uint16_t dayPercent = 0;
    bool ret = NetManagerCenter::GetInstance().GetDailyMarkBySimId(simId, dayPercent);
    NetManagerCenter::GetInstance().GetMonthlyLimitBySimId(simId, traffic);
    if (!ret) {
        NETMGR_LOG_E("simId does not exist:: simId %{public}d", simId);
        return "";
    }

    double dailyTraffic = static_cast<double>(traffic) / 100 * dayPercent;
    std::string num = GetTrafficNum(dailyTraffic);
    outText = outText.replace(outText.find("%s"), TWO_CHAR, num);
    NETMGR_LOG_I("NetMgrNetStatsLimitNotification::outText [%{public}s]", outText.c_str());
    return outText;
}

std::string NetMgrNetStatsLimitNotification::GetMonthNotificationText()
{
    if (resourceMap.find(KEY_MONTH_NOTIFY_TEXT) == resourceMap.end() ||
        resourceMap[KEY_MONTH_NOTIFY_TEXT].find("%s") == std::string::npos) {
        NETMGR_LOG_E("GetMonthNotificationText error");
        return "";
    }
    std::string outText = resourceMap[KEY_MONTH_NOTIFY_TEXT];

    int32_t simId = simId_;
    uint16_t monUsedPercent = 0;
    bool ret = NetManagerCenter::GetInstance().GetMonthlyMarkBySimId(simId, monUsedPercent);
    if (!ret) {
        NETMGR_LOG_E("simId does not exist:: simId %{public}d", simId);
        return "";
    }
    std::string style = "percent";
    std::string unitStyle = "short";
    std::map<std::string, std::string> mp = {{"style", style}, {"unitStyle", unitStyle}};

    std::string systemLocalStr = Global::I18n::LocaleConfig::GetSystemLocale();
    std::vector<std::string> local{systemLocalStr};
    std::unique_ptr<Global::I18n::NumberFormat> numFmt = std::make_unique<Global::I18n::NumberFormat>(local, mp);
    double monUsed = monUsedPercent / 100.0; // 100.0: converting a percentage to a decimal
    std::string str = numFmt->Format(monUsed);
    std::string percent = str;
    auto ret_order = Global::I18n::LocaleConfig::IsRTL(systemLocalStr);
    if (ret_order) {
        percent = "‭" + str + "‬";
    }
    outText = outText.replace(outText.find("%s"), TWO_CHAR, percent);
    NETMGR_LOG_I("GetMonthNotificationText outText [%{public}s]", outText.c_str());
    return outText;
}

std::string NetMgrNetStatsLimitNotification::GetMonthAlertText()
{
    if (resourceMap.find(KEY_MONTH_LIMIT_TEXT) == resourceMap.end() ||
        resourceMap[KEY_MONTH_LIMIT_TEXT].find("%s") == std::string::npos) {
        NETMGR_LOG_E("GetMonthAlertText error");
        return "";
    }

    std::string outText = resourceMap[KEY_MONTH_LIMIT_TEXT];
    int32_t simId = simId_;
    uint64_t traffic = 0;
    bool ret = NetManagerCenter::GetInstance().GetMonthlyLimitBySimId(simId, traffic);
    NETMGR_LOG_I("GetMonthAlertText trafficLimit:%{public}" PRIu64 "", traffic);
    if (!ret) {
        return "";
    }
    std::string num = GetTrafficNum(static_cast<double>(traffic));
    outText = outText.replace(outText.find("%s"), TWO_CHAR, num);
    NETMGR_LOG_I("GetMonthAlertText::outText [%{public}s]", outText.c_str());
    return outText;
}

std::string NetMgrNetStatsLimitNotification::GetNotificationTitle(std::string &notificationType)
{
    NETMGR_LOG_I("start NetMgrNetStatsLimitNotification::GetNotificationTitle");
    
    // Get all SIM account info list
    std::vector<Telephony::IccAccountInfo> iccAccountInfoList;
    int32_t ret = Telephony::CoreServiceClient::GetInstance().GetAllSimAccountInfoList(iccAccountInfoList);
    
    int32_t simId = simId_;
    bool isEsim = false;
    int32_t simLabelIndex = 0;
    
    // LCOV_EXCL_START
    if (ret == 0 && !iccAccountInfoList.empty()) {
        // Find the matching IccAccountInfo by simId
        for (const auto &info : iccAccountInfoList) {
            if (info.simId == simId) {
                isEsim = info.isEsim;
                simLabelIndex = info.simLabelIndex;
                break;
            }
        }
    } else {
        // Fallback to original logic if GetAllSimAccountInfoList fails
        int32_t slotId = Telephony::CoreServiceClient::GetInstance().GetSlotId(simId);
        NETMGR_LOG_I("GetNotificationTitle fallback. simId:%{public}d, slotId:%{public}d", simId, slotId);
        simLabelIndex = slotId + 1;
    }
    
    NETMGR_LOG_I("GetNotificationTitle. simId:%{public}d, isEsim:%{public}d, simLabelIndex:%{public}d",
                 simId, isEsim, simLabelIndex);
    
    // Select title template based on isEsim
    std::string titleKey = notificationType;
    if (isEsim) {
        if (notificationType == KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE) {
            titleKey = KEY_NETWORK_MONTH_LIMIT_ESIM_TITLE;
        } else if (notificationType == KEY_NETWORK_MONTH_MARK_DUAL_TITLE) {
            titleKey = KEY_NETWORK_MONTH_MARK_ESIM_TITLE;
        } else if (notificationType == KEY_NETWORK_DAY_MARK_DUAL_TITLE) {
            titleKey = KEY_NETWORK_DAY_MARK_ESIM_TITLE;
        }
    }
    // LCOV_EXCL_STOP
    
    if (resourceMap.find(titleKey) == resourceMap.end()) {
        NETMGR_LOG_E("cannot get title from resources, titleKey: %{public}s", titleKey.c_str());
        return "";
    }
    
    std::string outText = resourceMap[titleKey];
    if (outText.find("%d") == std::string::npos) {
        NETMGR_LOG_I("incorrect format %{public}s", outText.c_str());
        return "";
    }
    
    outText = outText.replace(outText.find("%d"), TWO_CHAR, std::to_string(simLabelIndex));
    return outText;
}

bool NetMgrNetStatsLimitNotification::SetTitleAndText(int notificationId,
                                                      std::shared_ptr<Notification::NotificationNormalContent> content,
                                                      bool isDualCard)
{
    NETMGR_LOG_I("start NetMgrNetStatsLimitNotification::SetTitleAndText");
    if (content == nullptr) {
        NETMGR_LOG_E("content is null");
        return false;
    }

    std::string title = "";
    if (isDualCard) {
        title = (notificationId == NETMGR_STATS_LIMIT_DAY) ? KEY_NETWORK_DAY_MARK_DUAL_TITLE : title;
        title = (notificationId == NETMGR_STATS_LIMIT_MONTH) ? KEY_NETWORK_MONTH_MARK_DUAL_TITLE : title;
        title = (notificationId == NETMGR_STATS_ALERT_MONTH) ? KEY_NETWORK_MONTH_LIMIT_DUAL_TITLE : title;
    } else {
        title = (notificationId == NETMGR_STATS_LIMIT_DAY) ? KEY_NETWORK_DAY_MARK_SINGLE_TITLE : title;
        title = (notificationId == NETMGR_STATS_LIMIT_MONTH) ? KEY_NETWORK_MONTH_MARK_SINGLE_TITLE : title;
        title = (notificationId == NETMGR_STATS_ALERT_MONTH) ? KEY_NETWORK_MONTH_LIMIT_SINGLE_TITLE : title;
    }

    if (resourceMap.find(title) == resourceMap.end()) {
        NETMGR_LOG_E("cannot get title from resources");
        return false;
    }
    std::string strTitle;
    if (isDualCard) {
        strTitle = GetNotificationTitle(title);
    } else {
        strTitle = resourceMap[title];
    }
    NETMGR_LOG_I("NetMgrNetStatsLimitNotification: strTitle = %{public}s", strTitle.c_str());

    std::string strText;
    switch (notificationId) {
        case NETMGR_STATS_LIMIT_DAY:
            strText = GetDayNotificationText();
            break;
        case NETMGR_STATS_LIMIT_MONTH:
            strText = GetMonthNotificationText();
            break;
        case NETMGR_STATS_ALERT_MONTH:
            strText = GetMonthAlertText();
            break;
        default:
            NETMGR_LOG_I("unknown notification ID");
            return false;
    }
    content->SetText(strText);
    content->SetTitle(strTitle);
    NETMGR_LOG_I("end NetMgrNetStatsLimitNotification::SetTitleAndText");
    return true;
}

void NetMgrNetStatsLimitNotification::GetPixelMap()
{
    if (netmgrStatsLimitIconPixelMap_ != nullptr) {
        return;
    }

    if (!std::filesystem::exists(NETWORK_ICON_PATH)) {
        return;
    }

    uint32_t errorCode = 0;
    Media::SourceOptions opts;
    opts.formatHint = "image/png";
    auto imageSource = Media::ImageSource::CreateImageSource(NETWORK_ICON_PATH, opts, errorCode);
    if (imageSource == nullptr) {
        NETMGR_LOG_I("CreateImageSource null");
        return;
    }
    Media::DecodeOptions decodeOpts;
    std::unique_ptr<Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    netmgrStatsLimitIconPixelMap_ = std::move(pixelMap);
}

std::mutex NetMgrNetStatsLimitNotification::instanceLock_;
std::shared_ptr<NetMgrNetStatsLimitNotification> NetMgrNetStatsLimitNotification::instance_ = nullptr;
std::shared_ptr<NetMgrNetStatsLimitNotification> NetMgrNetStatsLimitNotification::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lockGuard(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<NetMgrNetStatsLimitNotification>();
            return instance_;
        }
    }
    return instance_;
}

NetMgrNetStatsLimitNotification::NetMgrNetStatsLimitNotification()
{
    std::lock_guard<std::mutex> lock(mutex_);
    NETMGR_LOG_I("start NetMgrNetStatsLimitNotification");
    ParseJSONFile(LOCALE_TO_RESOURCE_PATH, languageMap);
    UpdateResourceMap();
    GetPixelMap();
    NETMGR_LOG_I("end NetMgrNetStatsLimitNotification");
}

NetMgrNetStatsLimitNotification::~NetMgrNetStatsLimitNotification()
{
    NETMGR_LOG_I("NetMgr Notification destructor enter.");
}

void NetMgrNetStatsLimitNotification::SetWantAgent(Notification::NotificationRequest &request)
{
    auto want = std::make_shared<AAFwk::Want>();
    want->SetElementName(SETTING_BUNDLE_NAME, SETTING_ABILITY_NAME);
    want->SetUri(URI);
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    wants.push_back(want);

    std::vector<AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(AbilityRuntime::WantAgent::WantAgentConstant::Flags::CONSTANT_FLAG);

    AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(
        0, AbilityRuntime::WantAgent::WantAgentConstant::OperationType::START_ABILITY, flags, wants, nullptr);
    auto wantAgent = AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
    request.SetWantAgent(wantAgent);
}

void NetMgrNetStatsLimitNotification::PublishNetStatsLimitNotification(int notificationId, int simId, bool isDualCard)
{
    std::lock_guard<std::mutex> lock(mutex_);
    simId_ = simId;
    NETMGR_LOG_I("PublishNetMgrNetStatsLimitNotification: id = %{public}d", notificationId);
    UpdateResourceMap();
    std::shared_ptr<Notification::NotificationNormalContent> notificationContent =
        std::make_shared<Notification::NotificationNormalContent>();
    if (notificationContent == nullptr) {
        NETMGR_LOG_E("get notification content nullptr");
        return;
    }
    if (!SetTitleAndText(notificationId, notificationContent, isDualCard)) {
        NETMGR_LOG_E("error setting title and text");
        return;
    }

    std::shared_ptr<Notification::NotificationContent> content =
        std::make_shared<Notification::NotificationContent>(notificationContent);
    if (content == nullptr) {
        NETMGR_LOG_E("get notification content nullptr");
        return;
    }

    Notification::NotificationRequest request;
    request.SetNotificationId(static_cast<int32_t>(notificationId));
    request.SetContent(content);
    request.SetCreatorUid(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    request.SetAutoDeletedTime(NTF_AUTO_DELETE_TIME);
    request.SetTapDismissed(true);
    request.SetSlotType(OHOS::Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    request.SetNotificationControlFlags(NETMGR_TRAFFIC_NTF_CONTROL_FLAG);
    request.SetLittleIcon(netmgrStatsLimitIconPixelMap_);
    SetWantAgent(request);
    int ret = Notification::NotificationHelper::PublishNotification(request);
    NETMGR_LOG_I("publish notification result = %{public}d", ret);
}

void NetMgrNetStatsLimitNotification::RegNotificationCallback(NetMgrStatsLimitNtfCallback callback)
{
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    g_netMgrStatsLimitNtfCallback = callback;
}

std::string NetMgrNetStatsLimitNotification::GetTrafficNum(double traffic)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    const char* unitFullNamesLower[] = {"byte", "kilobyte", "megabyte", "gigabyte", "terabyte"};
    int record = 0;
    while (traffic >= UNIT_CONVERT_1024 && record < 4) { // 4: units array max index
        traffic /= UNIT_CONVERT_1024;
        record++;
    }
    std::string style = "unit";
    std::string unit = unitFullNamesLower[record];
    std::string unitStyle = "short";
    std::map<std::string, std::string> mp = {{"style", style}, {"unit", unit}, {"unitStyle", unitStyle}};
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << traffic; // 2: 保留两位小数
    std::string strt = oss.str();
    char* end = NULL;
    double value = std::strtod(strt.c_str(), &end);
    std::string systemLocalStr = Global::I18n::LocaleConfig::GetSystemLocale();
    std::vector<std::string> local{systemLocalStr};
    std::unique_ptr<Global::I18n::NumberFormat> numFmt = std::make_unique<Global::I18n::NumberFormat>(local, mp);
    std::string str = numFmt->Format(value);
    auto ret = Global::I18n::LocaleConfig::IsRTL(systemLocalStr);
    if (ret) {
        return "‭" + str + "‬";
    }
    return str;
}

std::shared_ptr<INetMgrStatsLimitNotification> NetMgrNetStatsLimitNotification::GetInstancePtr()
{
    return NetMgrNetStatsLimitNotification::GetInstance();
}

}  // namespace NetManagerStandard
}  // namespace OHOS

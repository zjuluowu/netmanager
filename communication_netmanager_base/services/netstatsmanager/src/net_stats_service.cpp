/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "net_stats_service.h"

#include <dlfcn.h>
#include <net/if.h>
#include <sys/time.h>
#include <unistd.h>
#include <chrono>
#include <format>
#include <regex>

#include <cinttypes>

#include <initializer_list>

#include "bpf_stats.h"
#include "bpf_path.h"
#include "bpf_def.h"
#include "broadcast_manager.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "ffrt_inner.h"
#include "net_bundle.h"
#include "net_stats_cached.h"
#include "net_manager_center.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_constants.h"
#include "net_stats_database_defines.h"
#include "net_stats_service_common.h"
#include "netmanager_base_common_utils.h"
#include "netmanager_base_permission.h"
#include "netmanager_hitrace.h"
#include "netsys_controller.h"
#include "system_ability_definition.h"
#include "net_stats_utils.h"
#ifdef SUPPORT_TRAFFIC_STATISTIC
#include "cellular_data_client.h"
#include "core_service_client.h"
#include "net_conn_client.h"
#include "cellular_data_types.h"
#include "net_stats_limit_notification_adapter.h"
#include "net_stats_rdb.h"
#include "telephony_observer_broker.h"
#include "telephony_observer_client.h"
#endif // SUPPORT_TRAFFIC_STATISTIC
#include "iptables_wrapper.h"
#ifdef SUPPORT_NETWORK_SHARE
#include "networkshare_client.h"
#include "networkshare_constants.h"
#endif // SUPPORT_NETWORK_SHARE
#include "system_timer.h"
#include "net_stats_subscriber.h"
#include "ipc_skeleton.h"
#include "net_conn_client.h"
#include "cJSON.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;
namespace {
constexpr std::initializer_list<NetBearType> BEAR_TYPE_LIST = {
    NetBearType::BEARER_CELLULAR, NetBearType::BEARER_WIFI, NetBearType::BEARER_BLUETOOTH,
    NetBearType::BEARER_ETHERNET, NetBearType::BEARER_VPN,  NetBearType::BEARER_WIFI_AWARE,
};
constexpr uint32_t DEFAULT_UPDATE_TRAFFIC_INFO_CYCLE_MS = 30 * 60 * 1000;
constexpr uint32_t DAY_SECONDS = 2 * 24 * 60 * 60;
constexpr uint32_t DAY_MILLISECONDS = 24 * 60 * 60 * 1000;
constexpr int32_t TRAFFIC_NOTIFY_TYPE = 3;
constexpr int32_t SLOT_0 = 0;
constexpr int32_t SLOT_1 = 1;
constexpr const char* UID = "uid";
const std::string LIB_NET_BUNDLE_UTILS_PATH = "libnet_bundle_utils.z.so";
constexpr uint64_t DELAY_US = 35 * 1000 * 1000;
constexpr uint64_t UPDATE_FLAG_DELAY_US = 500 * 1000;
constexpr uint32_t UPDATE_BPF_MAP_DELAY_US = 6 * 1000 * 1000;
constexpr const char* COMMON_EVENT_STATUS = "usual.event.RGM_STATUS_CHANGED";
constexpr const char* STATUS_FIELD = "rgmStatus";
const std::string STATUS_UNLOCKED = "rgm_user_unlocked";

enum NetStatusType : uint8_t {
    WIFI_TYPE = 0,
    CELLULAR_TYPE = 1,
};

enum NetStatusConn : uint8_t {
    NON_CONNECTED = 0,
    CONNECTED = 1,
};
#ifdef SUPPORT_TRAFFIC_STATISTIC
static constexpr uint32_t TELEPHONY_EVENT_MASK =
    Telephony::TelephonyObserverBroker::OBSERVER_MASK_SIM_STATE |
    Telephony::TelephonyObserverBroker::OBSERVER_MASK_ICC_ACCOUNT;
#endif // SUPPORT_TRAFFIC_STATISTIC
constexpr const char *EXTENSION_BACKUP = "backup";
constexpr const char *EXTENSION_RESTORE = "restore";
constexpr const uint64_t NET_STATS_REPORT_DELAY = static_cast<uint64_t>(2 * 60 * 60 * 1000) * 1000;  // 2h
constexpr const int32_t HIVIEW_UID = 1201;
constexpr const char *NET_STATS_CALLED_EVENT = "custom.event.NET_STATS_CALLED";
constexpr const char *NET_STATS_CALL_INFO_KEY = "NET_STATS_CALL_INFO";
constexpr int32_t API_VERSION_26 = 26;
} // namespace
const bool REGISTER_LOCAL_RESULT =
    SystemAbility::MakeAndRegisterAbility(NetStatsService::GetInstance().get());

NetStatsService::NetStatsService()
    : SystemAbility(COMM_NET_STATS_MANAGER_SYS_ABILITY_ID, true), registerToService_(false), state_(STATE_STOPPED)
{
    netStatsCallback_ = std::make_shared<NetStatsCallback>();
    netStatsCached_ = std::make_shared<NetStatsCached>();
    trafficPlanService_ = std::make_unique<NetStatsTrafficPlanService>();
    recordReportFfrtQueue_ = std::make_shared<ffrt::queue>("NetStatsReport");
#ifdef SUPPORT_TRAFFIC_STATISTIC
    trafficObserver_ = std::make_unique<TrafficObserver>().release();
    netStatsCalibrate_ = std::make_shared<NetStatsCalibrate>();
#ifndef UNITTEST_FORBID_FFRT
    trafficPlanFfrtQueue_ = std::make_shared<ffrt::queue>("TrafficPlanStatistic");
#endif
#endif // SUPPORT_TRAFFIC_STATISTIC
}

NetStatsService::~NetStatsService() = default;

std::mutex NetStatsService::instanceLock_;
std::shared_ptr<NetStatsService> NetStatsService::instance_ = nullptr;

std::shared_ptr<NetStatsService> NetStatsService::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lockGuard(instanceLock_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<NetStatsService>();
            return instance_;
        }
    }
    return instance_;
}

// LCOV_EXCL_START
void NetStatsService::OnStart()
{
    if (state_ == STATE_RUNNING) {
        NETMGR_LOG_D("the state is already running");
        return;
    }
    if (!Init()) {
        NETMGR_LOG_E("init failed");
        return;
    }
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    AddSystemAbilityListener(TIME_SERVICE_ID);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    AddSystemAbilityListener(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    AddSystemAbilityListener(TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID);
#endif // SUPPORT_TRAFFIC_STATISTIC
    AddSystemAbilityListener(SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN);
    AddSystemAbilityListener(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID);
    state_ = STATE_RUNNING;
    sptr<NetStatsBaseService> baseService = new (std::nothrow) NetStatsServiceCommon();
    if (baseService == nullptr) {
        NETMGR_LOG_E("Net stats base service instance create failed");
        return;
    }
    NetManagerCenter::GetInstance().RegisterStatsService(baseService);
    netStatsCalibrate_->InitChangeToIfaceTime();
}

void NetStatsService::StartSysTimer()
{
    NETMGR_LOG_I("NetStatsService StartSysTimer");
    std::lock_guard<std::mutex> lock(timerMutex_);
    if (netStatsSysTimerId_ != 0) {
        NETMGR_LOG_E("netStatsSysTimerId_ is not zero, value is %{public}" PRIu64, netStatsSysTimerId_);
        return;
    }
    std::shared_ptr<NetmanagerSysTimer> netStatsSysTimer =
        std::make_unique<NetmanagerSysTimer>(true, DAY_MILLISECONDS, true);
    std::function<void()> callback = [this]() {
#ifdef SUPPORT_TRAFFIC_STATISTIC
        NetStatsRDB netStats;
        netStats.BackUpNetStatsFreqDB(NOTICE_DATABASE_NAME, NOTICE_DATABASE_BACK_NAME);
        UpdateBpfMapTimerTask();
#endif // SUPPORT_TRAFFIC_STATISTIC
        UpdateStatsDataInner();
    };
    netStatsSysTimer->SetCallbackInfo(callback);
    netStatsSysTimer->SetName("netstats_data_persistence_timer");
    netStatsSysTimerId_ = MiscServices::TimeServiceClient::GetInstance()->CreateTimer(netStatsSysTimer);
    uint64_t todayStartTime = static_cast<uint64_t>(CommonUtils::GetTodayMidnightTimestamp(23, 59, 55)) * 1000;
    MiscServices::TimeServiceClient::GetInstance()->StartTimer(netStatsSysTimerId_, todayStartTime);
    NETMGR_LOG_I("netStatsSysTimerId_ success. value is %{public}" PRIu64, netStatsSysTimerId_);
}

void NetStatsService::StopSysTimer()
{
    std::lock_guard<std::mutex> lock(timerMutex_);
    if (netStatsSysTimerId_ == 0) {
        NETMGR_LOG_W("netStatsSysTimerId_ is zero");
        return;
    }
    MiscServices::TimeServiceClient::GetInstance()->StopTimer(netStatsSysTimerId_);
    MiscServices::TimeServiceClient::GetInstance()->DestroyTimer(netStatsSysTimerId_);
    netStatsSysTimerId_ = 0;
    NETMGR_LOG_I("stop netStatsSysTimerId_ success");
}

int32_t NetStatsService::ModifySysTimer()
{
    std::lock_guard<std::mutex> lock(timerMutex_);
    if (netStatsSysTimerId_ == 0) {
        NETMGR_LOG_E("netStatsSysTimerId_ is zero");
        return NETMANAGER_ERROR;
    }
    MiscServices::TimeServiceClient::GetInstance()->StopTimer(netStatsSysTimerId_);
    uint64_t todayStartTime = static_cast<uint64_t>(CommonUtils::GetTodayMidnightTimestamp(23, 59, 55)) * 1000;
    MiscServices::TimeServiceClient::GetInstance()->StartTimer(netStatsSysTimerId_, todayStartTime);
    NETMGR_LOG_I("ModifySysTimer netStatsSysTimerId_ success. timer: %{public}" PRIu64, todayStartTime);
    return NETMANAGER_SUCCESS;
}

void NetStatsService::OnStop()
{
    state_ = STATE_STOPPED;
    registerToService_ = true;
}

int32_t NetStatsService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    NETMGR_LOG_D("Start Dump, fd: %{public}d", fd);
    std::string result;
    GetDumpMessage(result);
    int32_t ret = dprintf(fd, "%s\n", result.c_str());
    return ret < 0 ? STATS_DUMP_MESSAGE_FAIL : NETMANAGER_SUCCESS;
}

void NetStatsService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    NETMGR_LOG_I("OnAddSystemAbility: systemAbilityId:%{public}d", systemAbilityId);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    if (systemAbilityId == COMM_NETSYS_NATIVE_SYS_ABILITY_ID) {
        StartTrafficOvserver();
        return;
    }
    if (systemAbilityId == TELEPHONY_STATE_REGISTRY_SYS_ABILITY_ID) {
        SubscribeTelephonyInfo();
        return;
    }
#endif // SUPPORT_TRAFFIC_STATISTIC
    if (systemAbilityId == BUNDLE_MGR_SERVICE_SYS_ABILITY_ID) {
        RefreshUidStatsFlag(DELAY_US);
        return;
    }
    if (systemAbilityId == TIME_SERVICE_ID) {
        StartSysTimer();
        return;
    }

    if (systemAbilityId == SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN) {
        InitPrivateUserId();
        StartAccountObserver();
        return;
    }
    if (systemAbilityId == COMM_NET_CONN_MANAGER_SYS_ABILITY_ID) {
        StartNetObserver();
        return;
    }
    RegisterCommonEvent();
}

void NetStatsService::RegisterCommonEvent()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SHUTDOWN);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_CELLULAR_DATA_STATE_CHANGED);
#endif // SUPPORT_TRAFFIC_STATISTIC
    matchingSkills.AddEvent(COMMON_EVENT_STATUS);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_TIME_CHANGED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_WIFI_CONN_STATE);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscriber_ = std::make_shared<NetStatsListener>(subscribeInfo);
    subscriber_->RegisterStatsCallback(EventFwk::CommonEventSupport::COMMON_EVENT_SHUTDOWN,
        [this](const EventFwk::Want &want) {
            NETMGR_LOG_I("Net stats shutdown event");
            return UpdateStatsData();
        });
    subscriber_->RegisterStatsCallback(
        EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED, [this](const EventFwk::Want &want) {
            uint32_t uid = want.GetIntParam(UID, 0);
            NETMGR_LOG_D("Net Manager delete uid, uid:[%{public}d]", uid);
            return CommonEventPackageRemoved(uid);
        });
    subscriber_->RegisterStatsCallback(
        EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED, [this](const EventFwk::Want &want) {
            uint32_t uid = want.GetIntParam(UID, 0);
            NETMGR_LOG_D("Net Manager add uid, uid:[%{public}d]", uid);
            return CommonEventPackageAdded(uid);
        });
    subscriber_->RegisterStatsCallback(COMMON_EVENT_STATUS, [this](const EventFwk::Want &want) -> bool {
        std::string status = want.GetStringParam(STATUS_FIELD);
        NETMGR_LOG_I("Net Manager status changed, status:[%{public}s]", status.c_str());
        if (status == STATUS_UNLOCKED) {
            RefreshUidStatsFlag(0);
        }
        return true;
    });
    RegisterCommonTelephonyEvent();
    RegisterCommonTimeEvent();
    RegisterCommonNetStatusEvent();
    EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
}

void NetStatsService::RegisterCommonNetStatusEvent()
{
    subscriber_->RegisterStatsCallbackData(
        EventFwk::CommonEventSupport::COMMON_EVENT_WIFI_CONN_STATE, [this](const EventFwk::CommonEventData& eventData) {
            int32_t state = eventData.GetCode();
            NETMGR_LOG_I("COMMON_EVENT_WIFI_CONN_STATE: %{public}d", state);
            if (state == 4) { // 4:OHOS::Wifi::ConnState::CONNECTED
                return UpdateNetStatusMap(0, 1);
            } else {
                return UpdateNetStatusMap(0, 0);
            }
            return false;
        });
}

void NetStatsService::RegisterCommonTelephonyEvent()
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    subscriber_->RegisterStatsCallback(
        EventFwk::CommonEventSupport::COMMON_EVENT_CELLULAR_DATA_STATE_CHANGED, [this](const EventFwk::Want &want) {
            int32_t slotId = want.GetIntParam("slotId", -1);
            int32_t dataState = want.GetIntParam("dataState", -1);
            return CommonEventCellularDataStateChanged(slotId, dataState);
        });
#endif // SUPPORT_TRAFFIC_STATISTIC
}

void NetStatsService::RegisterCommonTimeEvent()
{
    subscriber_->RegisterStatsCallback(
        EventFwk::CommonEventSupport::COMMON_EVENT_TIME_CHANGED, [this](const EventFwk::Want &want) {
            NETMGR_LOG_I("COMMON_EVENT_TIME_CHANGED");
            ModifySysTimer();
#ifdef SUPPORT_TRAFFIC_STATISTIC
            UpdateAllHistoryDateInfo();
#endif // SUPPORT_TRAFFIC_STATISTIC
            return true;
        });
    subscriber_->RegisterStatsCallback(
        EventFwk::CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED, [this](const EventFwk::Want &want) -> bool {
            NETMGR_LOG_I("COMMON_EVENT_TIMEZONE_CHANGED");
            ModifySysTimer();
#ifdef SUPPORT_TRAFFIC_STATISTIC
            UpdateAllHistoryDateInfo();
#endif // SUPPORT_TRAFFIC_STATISTIC
            return true;
        });
}

bool NetStatsService::UpdateNetStatusMap(uint8_t type, uint8_t value)
{
    int32_t ret = NetsysController::GetInstance().SetNetStatusMap(type, value);
    if (ret != NETMANAGER_SUCCESS) {
        return false;
    }
    return true;
}

int32_t NetStatsService::GetMonthTrafficStatsByNetwork(uint32_t simId, uint64_t &monthDataIpc)
{
#ifndef SUPPORT_TRAFFIC_STATISTIC
    monthDataIpc = 0;
    return NETMANAGER_ERR_OPERATION_FAILED;
#else
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }

    if (!NetStatsUtils::IsSimIdValid(simId)) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    uint64_t monthData = netStatsCached_->GetMonthTrafficData(simId);
    if (monthData != UINT64_MAX) {
        monthDataIpc = monthData;
        NETMGR_LOG_I("monthDataIpc data: %{public}" PRIu64, monthDataIpc);
        return NETMANAGER_SUCCESS;
    }

    auto infoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!infoPtr) {
        NETMGR_LOG_E("infoPtr nullptr");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    int32_t beginDate = infoPtr->startDate;

    std::unordered_map<uint32_t, NetStatsInfo> infos;
    NetStatsNetwork networkInfo;
    networkInfo.type_ = 0;  // 0:cellular
    networkInfo.simId_ = simId;
    networkInfo.startTime_ = static_cast<uint64_t>(NetStatsUtils::GetStartTimestamp(beginDate));
    networkInfo.endTime_ = static_cast<uint64_t>(CommonUtils::GetTodayMidnightTimestamp(23, 59, 59)); // 23:59:59
    int32_t ret = GetTrafficStatsByNetwork(infos, networkInfo);
    monthData = 0;
    for (const auto &info : infos) {
        monthData += info.second.rxBytes_;
        monthData += info.second.txBytes_;
    }
    monthDataIpc = monthData;
    NETMGR_LOG_I("GetTrafficStatsByNetwork data: %{public}" PRIu64, monthDataIpc);
#endif // SUPPORT_TRAFFIC_STATISTIC
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::OnExtension(const std::string& extension, MessageParcel& data, MessageParcel& reply)
{
    NETMGR_LOG_E("extension is %{public}s.", extension.c_str());
#ifdef SUPPORT_TRAFFIC_STATISTIC
    if (extension == EXTENSION_BACKUP) {
        return trafficPlanService_->OnBackup(data, reply);
    } else if (extension == EXTENSION_RESTORE) {
        return trafficPlanService_->OnRestore(data, reply);
    }
#endif
    return 0;
}

void NetStatsService::InitPrivateUserId()
{
    std::vector<AccountSA::OsAccountInfo> osAccountInfos;
    AccountSA::OsAccountManager::QueryAllCreatedOsAccounts(osAccountInfos);
    for (auto info : osAccountInfos) {
        AccountSA::OsAccountType accountType;
        AccountSA::OsAccountManager::GetOsAccountType(info.GetLocalId(), accountType);
        NETMGR_LOG_I("InitPrivateUserId, info: %{public}d", info.GetLocalId());
        if (accountType == AccountSA::OsAccountType::PRIVATE) {
            netStatsCached_->SetCurPrivateUserId(info.GetLocalId());
            netStatsCached_->SetPrivateStatus(true);
        }
    }
    int32_t defaultUserId = -1;
    int32_t ret = AccountSA::OsAccountManager::GetDefaultActivatedOsAccount(defaultUserId);
    NETMGR_LOG_I("default userId: %{public}d", defaultUserId);
    netStatsCached_->SetCurDefaultUserId(defaultUserId);
}

int32_t NetStatsService::ProcessOsAccountChanged(int32_t userId, AccountSA::OsAccountState state)
{
    NETMGR_LOG_I("OsAccountChanged toId: %{public}d, state:%{public}d", userId, state);
    if (state == AccountSA::OsAccountState::CREATED) {
        AccountSA::OsAccountType accountType;
        AccountSA::OsAccountManager::GetOsAccountType(userId, accountType);
        if (accountType == AccountSA::OsAccountType::PRIVATE) {
            netStatsCached_->SetCurPrivateUserId(userId);
            netStatsCached_->SetPrivateStatus(true);
        }
        return 0;
    }
    if (state == AccountSA::OsAccountState::STOPPING || state ==  AccountSA::OsAccountState::STOPPED ||
        state ==  AccountSA::OsAccountState::REMOVED) {
        if (netStatsCached_->GetCurPrivateUserId() != userId) {
            return 0;
        }
        netStatsCached_->SetCurPrivateUserId(-1);  // -1:invalid userID
        auto handler = std::make_unique<NetStatsDataHandler>();
        if (handler == nullptr) {
            NETMGR_LOG_E("handler is nullptr");
            return static_cast<int32_t>(NETMANAGER_ERR_INTERNAL);
        }
        handler->UpdateStatsFlagByUserId(userId, STATS_DATA_FLAG_UNINSTALLED);
        handler->UpdateSimStatsFlagByUserId(userId, STATS_DATA_FLAG_UNINSTALLED);
        handler->UpdateSimStatsFlagByUserId(SIM_PRIVATE_USERID, STATS_DATA_FLAG_UNINSTALLED);
        netStatsCached_->SetPrivateStatus(false);
        UpdateStatsData();
        isUpdate_ = false;
        return 0;
    }
    if (state == AccountSA::OsAccountState::SWITCHED) {
        if (userId == netStatsCached_->GetCurPrivateUserId()) {
            netStatsCached_->SetPrivateStatus(true);
            AddUidStatsFlag(UPDATE_FLAG_DELAY_US);
        }
    }
    return 0;
}

void NetStatsService::GetDumpMessage(std::string &message)
{
    message.append("Net Stats Info:\n");
    uint64_t rxBytes = 0;
    uint64_t txBytes = 0;
    uint64_t rxPackets = 0;
    uint64_t txPackets = 0;
    NetsysController::GetInstance().GetTotalStats(rxBytes, static_cast<uint32_t>(StatsType::STATS_TYPE_RX_BYTES));
    NetsysController::GetInstance().GetTotalStats(txBytes, static_cast<uint32_t>(StatsType::STATS_TYPE_TX_BYTES));
    NetsysController::GetInstance().GetTotalStats(rxPackets, static_cast<uint32_t>(StatsType::STATS_TYPE_RX_PACKETS));
    NetsysController::GetInstance().GetTotalStats(txPackets, static_cast<uint32_t>(StatsType::STATS_TYPE_TX_PACKETS));

    message.append("\tRxBytes: " + std::to_string(rxBytes) + "\n");
    message.append("\tTxBytes: " + std::to_string(txBytes) + "\n");
    message.append("\tRxPackets: " + std::to_string(rxPackets) + "\n");
    message.append("\tTxPackets: " + std::to_string(txPackets) + "\n");
    std::for_each(BEAR_TYPE_LIST.begin(), BEAR_TYPE_LIST.end(), [&message, this](const auto &bearType) {
        std::list<std::string> ifaceNames;
        if (NetManagerCenter::GetInstance().GetIfaceNames(bearType, ifaceNames)) {
            return;
        }
        uint64_t rx = 0;
        uint64_t tx = 0;
        for (const auto &name : ifaceNames) {
            GetIfaceRxBytes(rx, name);
            GetIfaceTxBytes(tx, name);
            message.append("\t" + name + "-TxBytes: " + std::to_string(tx));
            message.append("\t" + name + "-RxBytes: " + std::to_string(rx));
        }
    });
}

bool NetStatsService::Init()
{
    if (!REGISTER_LOCAL_RESULT) {
        NETMGR_LOG_E("Register to local sa manager failed");
        registerToService_ = false;
        return false;
    }
    if (!registerToService_) {
#ifndef NETMANAGER_TEST
        if (!Publish(NetStatsService::GetInstance().get())) {
            NETMGR_LOG_E("Register to sa manager failed");
            return false;
        }
#endif
        registerToService_ = true;
    }
    if (nullptr == netStatsCached_) {
        return false;
    }
    netStatsCached_->SetCallbackManager(netStatsCallback_);
    auto ret = netStatsCached_->StartCached();
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Start cached failed");
        return false;
    }
    AddSystemAbilityListener(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    uint64_t delay = DELAY_US / 2;
    RefreshUidStatsFlag(delay);

#ifdef SUPPORT_TRAFFIC_STATISTIC
#ifndef UNITTEST_FORBID_FFRT
    trafficTimer_ = std::make_unique<FfrtTimer>();
    trafficTimer_->Start(DEFAULT_UPDATE_TRAFFIC_INFO_CYCLE_MS, [this]() { UpdateBpfMapTimer(); });
#endif
    NetStatsRDB netStats;
    netStats.InitRdbStore();
#endif // SUPPORT_TRAFFIC_STATISTIC
    return true;
}

int32_t NetStatsService::RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback)
{
    NETMGR_LOG_I("Enter RegisterNetStatsCallback");
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    if (callback == nullptr) {
        NETMGR_LOG_E("RegisterNetStatsCallback parameter callback is null");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    netStatsCallback_->RegisterNetStatsCallback(callback);
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback)
{
    NETMGR_LOG_I("Enter UnregisterNetStatsCallback");
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    if (callback == nullptr) {
        NETMGR_LOG_E("UnregisterNetStatsCallback parameter callback is null");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    netStatsCallback_->UnregisterNetStatsCallback(callback);
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::GetIfaceRxBytes(uint64_t &stats, const std::string &interfaceName)
{
    return NetsysController::GetInstance().GetIfaceStats(stats, static_cast<uint32_t>(StatsType::STATS_TYPE_RX_BYTES),
                                                         interfaceName);
}

int32_t NetStatsService::GetIfaceTxBytes(uint64_t &stats, const std::string &interfaceName)
{
    return NetsysController::GetInstance().GetIfaceStats(stats, static_cast<uint32_t>(StatsType::STATS_TYPE_TX_BYTES),
                                                         interfaceName);
}

int32_t NetStatsService::GetCellularRxBytes(uint64_t &stats)
{
    std::list<std::string> ifaceNames;
    if (!GetIfaceNamesFromManager(ifaceNames)) {
        return STATS_ERR_GET_IFACE_NAME_FAILED;
    }

    for (const auto &name : ifaceNames) {
        uint64_t totalCellular = 0;
        auto ret = NetsysController::GetInstance().GetIfaceStats(
            totalCellular, static_cast<uint32_t>(StatsType::STATS_TYPE_RX_BYTES), name);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("Get iface stats failed result: %{public}d", ret);
            return ret;
        }
        stats += totalCellular;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::GetCellularTxBytes(uint64_t &stats)
{
    std::list<std::string> ifaceNames;
    if (!GetIfaceNamesFromManager(ifaceNames)) {
        return STATS_ERR_GET_IFACE_NAME_FAILED;
    }

    uint64_t totalCellular = 0;
    for (const auto &name : ifaceNames) {
        auto ret = NetsysController::GetInstance().GetIfaceStats(
            totalCellular, static_cast<uint32_t>(StatsType::STATS_TYPE_TX_BYTES), name);
        if (ret != NETMANAGER_SUCCESS) {
            NETMGR_LOG_E("Get iface stats failed result: %{public}d", ret);
            return ret;
        }
        stats += totalCellular;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::GetAllRxBytes(uint64_t &stats)
{
    NETMGR_LOG_D("Enter GetAllRxBytes");
    return NetsysController::GetInstance().GetTotalStats(stats, static_cast<uint32_t>(StatsType::STATS_TYPE_RX_BYTES));
}

int32_t NetStatsService::GetAllTxBytes(uint64_t &stats)
{
    NETMGR_LOG_D("Enter GetAllTxBytes");
    return NetsysController::GetInstance().GetTotalStats(stats, static_cast<uint32_t>(StatsType::STATS_TYPE_TX_BYTES));
}

int32_t NetStatsService::GetUidRxBytes(uint64_t &stats, uint32_t uid)
{
    uint32_t callingUid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());
    int32_t version = NetManagerPermission::GetApiVersion();
    NETMGR_LOG_D("Enter GetUidRxBytes, uid:%{public}d, version:%{public}d, callingUid:%{public}d",
        uid, version, callingUid);
    RecordCallingData("GetUidRxBytes", uid);
    if (uid != callingUid && version >= API_VERSION_26) {
        if (!NetManagerPermission::CheckPermission(Permission::GET_NETWORK_STATS)) {
            return NETMANAGER_ERR_PERMISSION_DENIED;
        }
    }
    return NetsysController::GetInstance().GetUidStats(stats, static_cast<uint32_t>(StatsType::STATS_TYPE_RX_BYTES),
                                                       uid);
}

int32_t NetStatsService::GetUidTxBytes(uint64_t &stats, uint32_t uid)
{
    uint32_t callingUid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());
    int32_t version = NetManagerPermission::GetApiVersion();
    NETMGR_LOG_D("Enter GetUidTxBytes, uid:%{public}d, version:%{public}d, callingUid:%{public}d",
        uid, version, callingUid);
    RecordCallingData("GetUidTxBytes", uid);
    if (uid != callingUid && version >= API_VERSION_26) {
        if (!NetManagerPermission::CheckPermission(Permission::GET_NETWORK_STATS)) {
            return NETMANAGER_ERR_PERMISSION_DENIED;
        }
    }
    return NetsysController::GetInstance().GetUidStats(stats, static_cast<uint32_t>(StatsType::STATS_TYPE_TX_BYTES),
                                                       uid);
}

void NetStatsService::RecordCallingData(const std::string &callingFunction, uint32_t uid)
{
    uint32_t callingUid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());
    SampleBundleInfo callingBundleInfo = GetSampleBundleInfoForUid(callingUid);
    std::string bundleName = callingBundleInfo.bundleName_;
    cJSON *recordJson = cJSON_CreateObject();
    if (recordJson == nullptr) {
        NETMGR_LOG_E("recordJson create failed");
        return;
    }
    cJSON_AddNumberToObject(recordJson, "uid", callingUid);
    cJSON_AddStringToObject(recordJson, "bundleName", bundleName.c_str());
    cJSON_AddStringToObject(recordJson, "function", callingFunction.c_str());
    cJSON_AddNumberToObject(recordJson, "paramUid", uid);
    char *pRecordJson = cJSON_PrintUnformatted(recordJson);
    cJSON_Delete(recordJson);
    if (!pRecordJson) {
        return;
    }
    std::string record(pRecordJson);
    NETMGR_LOG_D("RecordCallingData %{public}s", record.c_str());
    {
        std::lock_guard<ffrt::mutex> lock(recordCallingDataMutex_);
        callingRecordSet_.insert(record);
    }
    cJSON_free(pRecordJson);
    if (!isPostDelayReport_) {
        isPostDelayReport_ = true;
        std::weak_ptr<NetStatsService> wp = shared_from_this();
#ifndef UNITTEST_FORBID_FFRT
        recordReportFfrtQueue_->submit([wp]() {
#endif
                if (auto sharedSelf = wp.lock()) {
                    sharedSelf->ReportCallingData();
                }
#ifndef UNITTEST_FORBID_FFRT
            }, ffrt::task_attr().name("ReportCallingData").delay(NET_STATS_REPORT_DELAY));
#endif
    }
}

void NetStatsService::ReportCallingData()
{
    std::lock_guard<ffrt::mutex> lock(recordCallingDataMutex_);
    if (callingRecordSet_.empty()) {
        return;
    }
    BroadcastInfo info;
    info.action = NET_STATS_CALLED_EVENT;
    info.subscriberUid = HIVIEW_UID;

    std::string dataArray("[");
    for (auto it = callingRecordSet_.begin(); it != callingRecordSet_.end(); ++it) {
        if (it != callingRecordSet_.begin()) {
            dataArray.append(",");
        }
        dataArray.append(*it);
    }
    dataArray.append("]");
    NETMGR_LOG_D("ReportCallingData %{public}s", dataArray.c_str());
    std::map<std::string, std::string> param = {{NET_STATS_CALL_INFO_KEY, dataArray}};
    BroadcastManager::GetInstance().SendBroadcast(info, param);
    callingRecordSet_.clear();
    isPostDelayReport_ = false;
}

int32_t NetStatsService::GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                             NetStatsInfo &statsInfo)
{
    // Start of get traffic data by interface name.
    NETMGR_LOG_D("Enter GetIfaceStatsDetail, iface= %{public}s", iface.c_str());
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService GetIfaceStatsDetail start");
    if (start > end) {
        NETMGR_LOG_E("start is after end.");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    std::vector<NetStatsInfo> allInfo;
    auto history = std::make_unique<NetStatsHistory>();
    int32_t ret = history->GetHistory(allInfo, iface, start, end);

    if (netStatsCached_ == nullptr) {
        NETMGR_LOG_E("netStatsCached_ is fail");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netStatsCached_->GetIfaceStatsCached(allInfo);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Get traffic stats data failed");
        return ret;
    }
    std::for_each(allInfo.begin(), allInfo.end(), [&statsInfo, &iface, &start, &end](const auto &info) {
        if (info.iface_ == iface && info.date_ >= start && info.date_ <= end) {
            statsInfo += info;
        }
    });
    statsInfo.iface_ = iface;
    statsInfo.date_ = end;
    // End of get traffic data by interface name.
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService GetIfaceStatsDetail end");
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::GetUidStatsDetail(const std::string &iface, uint32_t uid, uint64_t start, uint64_t end,
                                           NetStatsInfo &statsInfo)
{
    // Start of get traffic data by usr id.
    NETMGR_LOG_D("Enter GetIfaceStatsDetail, iface= %{public}s uid= %{public}d", iface.c_str(), uid);
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService GetUidStatsDetail start");
    if (start > end) {
        NETMGR_LOG_E("start is after end.");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    std::vector<NetStatsInfo> allInfo;
    auto history = std::make_unique<NetStatsHistory>();
    int32_t ret = history->GetHistory(allInfo, iface, uid, start, end);
    if (netStatsCached_ == nullptr) {
        NETMGR_LOG_E("netStatsCached_ is fail");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netStatsCached_->GetUidStatsCached(allInfo);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Get traffic stats data failed");
        return ret;
    }
    std::for_each(allInfo.begin(), allInfo.end(), [&statsInfo, &iface, &uid, &start, &end](const auto &info) {
        if (info.iface_ == iface && info.uid_ == uid && info.date_ >= start && info.date_ <= end) {
            statsInfo += info;
        }
    });
    statsInfo.uid_ = uid;
    statsInfo.iface_ = iface;
    statsInfo.date_ = end;
    // End of get traffic data by usr id.
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("NetStatsService GetUidStatsDetail end");
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::UpdateIfacesStats(const std::string &iface, uint64_t start, uint64_t end,
                                           const NetStatsInfo &stats)
{
    // Start of update traffic data by date.
    NETMGR_LOG_I("UpdateIfacesStats ifaces is %{public}s", iface.c_str());
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService UpdateIfacesStats start");
    if (start > end) {
        NETMGR_LOG_E("start is after end.");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    std::vector<NetStatsInfo> infos;
    infos.push_back(stats);
    auto handler = std::make_unique<NetStatsDataHandler>();
    auto ret = handler->DeleteByDate(IFACE_TABLE, start, end);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Update ifaces stats failed");
    }
    ret = handler->WriteStatsData(infos, IFACE_TABLE);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Update ifaces stats failed");
        return STATS_ERR_WRITE_DATA_FAIL;
    }
    // End of update traffic data by date.
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("NetStatsService UpdateIfacesStats end");
    return ret;
}

int32_t NetStatsService::UpdateStatsData()
{
    NETMGR_LOG_I("Enter UpdateStatsData.");
    if (!NetManagerPermission::IsSystemCaller()) {
        return NETMANAGER_ERR_NOT_SYSTEM_CALL;
    }
    if (!NetManagerPermission::CheckPermission(Permission::CONNECTIVITY_INTERNAL)) {
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    if (netStatsCached_ == nullptr) {
        NETMGR_LOG_E("Cached is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netStatsCached_->ForceUpdateStats();
    NETMGR_LOG_D("End UpdateStatsData.");
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::UpdateStatsDataInner()
{
    NETMGR_LOG_I("Enter UpdateStatsDataInner.");
    if (netStatsCached_ == nullptr) {
        NETMGR_LOG_E("Cached is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netStatsCached_->ForceUpdateStats();
    NETMGR_LOG_I("End UpdateStatsDataInner.");
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::ResetFactory()
{
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    auto handler = std::make_unique<NetStatsDataHandler>();
    return handler->ClearData();
}

int32_t NetStatsService::GetAllStatsInfo(std::vector<NetStatsInfo> &infos)
{
    NETMGR_LOG_D("Enter GetAllStatsInfo.");
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    if (netStatsCached_ != nullptr) {
        netStatsCached_->GetUidPushStatsCached(infos);
        netStatsCached_->GetAllPushStatsCached(infos);
    } else {
        NETMGR_LOG_E("Cached is nullptr");
    }
    return NetsysController::GetInstance().GetAllStatsInfo(infos);
}

int32_t NetStatsService::GetAllSimStatsInfo(std::vector<NetStatsInfo> &infos)
{
    NETMGR_LOG_D("Enter GetAllSimStatsInfo.");
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    return NetsysController::GetInstance().GetAllSimStatsInfo(infos);
}

#ifdef SUPPORT_NETWORK_SHARE
bool NetStatsService::IsSharingOn()
{
    int32_t share = 0;
    int ret = DelayedSingleton<NetManagerStandard::NetworkShareClient>::GetInstance()->IsSharing(share);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        NETMGR_LOG_E("get sharing state res: %{public}d, isSharing: %{public}d", ret, share);
        return false;
    }
    return share == NetManagerStandard::NETWORKSHARE_IS_SHARING;
}

void NetStatsService::GetSharingStats(std::vector<NetStatsInfo> &sharingStats, uint32_t endtime)
{
    if (endtime > netStatsCached_->GetWriteDateTime()) {
        // 跑在非ipc线程防止鉴权失败
        bool isSharingOn = false;
        auto task = ffrt::submit_h([&isSharingOn, this]() { isSharingOn = NetStatsService::IsSharingOn(); }, {}, {},
            ffrt::task_attr().name("isSharingOn"));
        ffrt::wait({task});
        if (isSharingOn) {
            NETMGR_LOG_D("GetSharingStats enter");
            netStatsCached_->GetIptablesStatsCached(sharingStats);
        }
    }
}
#endif

int32_t NetStatsService::GetTrafficStatsByNetwork(std::unordered_map<uint32_t, NetStatsInfo> &infos,
                                                  const NetStatsNetwork &networkIpc)
{
    NETMGR_LOG_D("Enter GetTrafficStatsByNetwork.");
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService GetTrafficStatsByNetwork start");
    if (netStatsCached_ == nullptr) {
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork(networkIpc);
    if (network == nullptr) {
        NETMGR_LOG_E("param network is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::string ident;
    if (network->type_ == 0) {
        ident = std::to_string(network->simId_);
    }
    uint32_t start = network->startTime_;
    uint32_t end = network->endTime_;
    NETMGR_LOG_D("param: ident=%{public}s, start=%{public}u, end=%{public}u", ident.c_str(), start, end);
    auto history = std::make_unique<NetStatsHistory>();
    if (history == nullptr) {
        NETMGR_LOG_E("history is null");
        return NETMANAGER_ERR_INTERNAL;
    }
    std::vector<NetStatsInfo> allInfo;
    int32_t ret = history->GetHistoryByIdent(allInfo, ident, start, end);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("get history by ident failed, err code=%{public}d", ret);
        return ret;
    }
    netStatsCached_->GetKernelStats(allInfo);
    netStatsCached_->GetUidPushStatsCached(allInfo);
    netStatsCached_->GetUidStatsCached(allInfo);
    netStatsCached_->GetUidSimStatsCached(allInfo);
#ifdef SUPPORT_NETWORK_SHARE
    GetSharingStats(allInfo, end);
#endif
    MergeTrafficStatsByAccount(allInfo);
    FilterTrafficStatsByNetwork(allInfo, infos, ident, start, end);
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService GetTrafficStatsByNetwork end");
    return NETMANAGER_SUCCESS;
}

void NetStatsService::FilterTrafficStatsByNetwork(std::vector<NetStatsInfo> &allInfo,
    std::unordered_map<uint32_t, NetStatsInfo> &infos,
    const std::string ident, uint32_t startTime, uint32_t endTime)
{
    std::for_each(allInfo.begin(), allInfo.end(), [&infos, &ident, &startTime, &endTime](NetStatsInfo &info) {
        if (ident != info.ident_ || startTime > info.date_ || endTime < info.date_) {
            return;
        }
        if (info.flag_ == STATS_DATA_FLAG_UNINSTALLED) {
            info.uid_ = UNINSTALLED_UID;
        }
        auto item = infos.find(info.uid_);
        if (item == infos.end()) {
            infos.emplace(info.uid_, info);
        } else {
            item->second += info;
        }
    });
}

void NetStatsService::MergeTrafficStatsByAccount(std::vector<NetStatsInfo> &infos)
{
    int32_t curUserId = -1;
    int32_t ret = AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(curUserId);
    int32_t defaultUserId = netStatsCached_->GetCurDefaultUserId();
    if (ret != 0) {
        NETMGR_LOG_E("get userId error. ret1: %{public}d", ret);
    }

    if (curUserId == defaultUserId) {
        for (auto &info : infos) {
            if (info.userId_ == netStatsCached_->GetCurPrivateUserId() || info.userId_ == SIM_PRIVATE_USERID) {
                info.uid_ = OTHER_ACCOUNT_UID;
            }
        }
    } else if (curUserId == netStatsCached_->GetCurPrivateUserId()) {
        for (auto &info : infos) {
            if (info.userId_ != curUserId && info.userId_ != SIM_PRIVATE_USERID) {
                info.uid_ = DEFAULT_ACCOUNT_UID;
            }
        }
    } else {
        NETMGR_LOG_W("curUserId:%{public}d, defaultUserId:%{public}d", curUserId, defaultUserId);
    }
}

int32_t NetStatsService::GetHistoryData(std::vector<NetStatsInfo> &infos, std::string ident,
    uint32_t uid, uint32_t start, uint32_t end)
{
    auto history = std::make_unique<NetStatsHistory>();
    if (history == nullptr) {
        NETMGR_LOG_E("history is null");
        return NETMANAGER_ERR_INTERNAL;
    }
    if (uid != DEFAULT_ACCOUNT_UID && uid != OTHER_ACCOUNT_UID) {
        int32_t ret = history->GetHistory(infos, uid, ident, start, end);
        return ret;
    }
    int32_t userId = -1;
    if (uid == DEFAULT_ACCOUNT_UID) {
        userId = netStatsCached_->GetCurDefaultUserId();
        history->GetHistoryByIdentAndUserIdWithAppend(infos, ident, userId, start, end);
        history->GetHistoryByIdentAndUserIdWithAppend(infos, ident, SYSTEM_DEFAULT_USERID, start, end);
    } else if (netStatsCached_->GetCurPrivateUserId() != -1) {
        userId = netStatsCached_->GetCurPrivateUserId();
        history->GetHistoryByIdentAndUserIdWithAppend(infos, ident, userId, start, end);
        history->GetHistoryByIdentAndUserIdWithAppend(infos, ident, SIM_PRIVATE_USERID, start, end);
    }
    if (userId == -1) {
        NETMGR_LOG_E("GetHistoryData error. uid:%{public}u, curPrivateUserId: %{public}d",
            uid, netStatsCached_->GetCurPrivateUserId());
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::GetTrafficStatsByUidNetwork(std::vector<NetStatsInfoSequence> &infos, uint32_t uid,
                                                     const NetStatsNetwork &networkIpc)
{
    NETMGR_LOG_D("Enter GetTrafficStatsByUidNetwork. uid: %{public}" PRIu32, uid);
    int32_t checkPermission = CheckNetManagerAvailable();
    uint32_t callingUid = static_cast<uint32_t>(IPCSkeleton::GetCallingUid());
    if (checkPermission != NETMANAGER_SUCCESS && uid != callingUid) {
        return checkPermission;
    }

    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService GetTrafficStatsByUidNetwork start");
    if (netStatsCached_ == nullptr) {
        NETMGR_LOG_E("Cached is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    sptr<NetStatsNetwork> network = new (std::nothrow) NetStatsNetwork(networkIpc);
    if (network == nullptr) {
        NETMGR_LOG_E("param network is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    std::string ident;
    if (network->type_ == 0) {
        ident = std::to_string(network->simId_);
    }
    uint32_t start = network->startTime_;
    uint32_t end = network->endTime_;
    NETMGR_LOG_D("GetTrafficStatsByUidNetwork param: "
        "uid=%{public}u, ident=%{public}s, start=%{public}u, end=%{public}u", uid, ident.c_str(), start, end);

    if (!NetManagerPermission::IsSystemCaller() &&
        (NetStatsUtils::IsLessThanOneMonthAgoPrecise(start) || NetStatsUtils::IsLessThanOneMonthAgoPrecise(end))) {
        NETMGR_LOG_E("timestamp error");
        return STATS_ERR_TIMESTAMP_ERROR;
    }

    std::vector<NetStatsInfo> allInfo;
    int32_t ret = GetHistoryData(allInfo, ident, uid, start, end);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("get history by uid and ident failed, err code=%{public}d", ret);
        return ret;
    }

    netStatsCached_->GetKernelStats(allInfo);
    netStatsCached_->GetUidPushStatsCached(allInfo);
    netStatsCached_->GetUidStatsCached(allInfo);
    netStatsCached_->GetUidSimStatsCached(allInfo);
#ifdef SUPPORT_NETWORK_SHARE
    if (uid == IPTABLES_UID) {
        GetSharingStats(allInfo, end);
    }
#endif
    FilterTrafficStatsByUidNetwork(allInfo, infos, uid, ident, start, end);
    DeleteTrafficStatsByAccount(infos, uid);
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService GetTrafficStatsByUidNetwork end");
    return NETMANAGER_SUCCESS;
}

void NetStatsService::DeleteTrafficStatsByAccount(std::vector<NetStatsInfoSequence> &infos, uint32_t uid)
{
    int32_t defaultUserId = netStatsCached_->GetCurDefaultUserId();
    if (uid == DEFAULT_ACCOUNT_UID) {
        for (auto it = infos.begin(); it != infos.end();) {
            if (it->info_.userId_ != defaultUserId && it->info_.userId_ != SYSTEM_DEFAULT_USERID) {
                it = infos.erase(it);
            } else {
                ++it;
            }
        }
    } else if (uid == OTHER_ACCOUNT_UID) {
        for (auto it = infos.begin(); it != infos.end();) {
            if (it->info_.userId_ == defaultUserId || it->info_.userId_ == SYSTEM_DEFAULT_USERID) {
                it = infos.erase(it);
            } else {
                ++it;
            }
        }
    } else if (uid == Sim_UID || uid == SIM2_UID) {
        int32_t curUserId = -1;
        AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(curUserId);
        if (curUserId == defaultUserId) {
            EraseNetStatsInfoByUserId(infos, SYSTEM_DEFAULT_USERID);
        } else {
            EraseNetStatsInfoByUserId(infos, SIM_PRIVATE_USERID);
        }
    }
}

void NetStatsService::EraseNetStatsInfoByUserId(std::vector<NetStatsInfoSequence> &infos, int32_t userId)
{
    for (auto it = infos.begin(); it != infos.end();) {
        if (it->info_.userId_ != userId) {
            it = infos.erase(it);
        } else {
            ++it;
        }
    }
}

void NetStatsService::FilterTrafficStatsByUidNetwork(std::vector<NetStatsInfo> &allInfo,
    std::vector<NetStatsInfoSequence> &infos, const uint32_t uid,
    const std::string ident, uint32_t startTime, uint32_t endTime)
{
    std::for_each(allInfo.begin(), allInfo.end(),
        [this, &infos, &uid, &ident, &startTime, &endTime](const NetStatsInfo &info) {
        if (uid != DEFAULT_ACCOUNT_UID && uid != OTHER_ACCOUNT_UID && uid != info.uid_) {
            return;
        }

        if (ident != info.ident_ || startTime > info.date_ || endTime < info.date_) {
            return;
        }
        if (info.flag_ == STATS_DATA_FLAG_UNINSTALLED) {
            return;
        }
        MergeTrafficStats(infos, info, endTime);
    });
}

int32_t NetStatsService::SetAppStats(const PushStatsInfo &info)
{
    NETMGR_LOG_D("Enter SetAppStats.");
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService SetAppStats start");
    if (netStatsCached_ == nullptr) {
        NETMGR_LOG_E("Cached is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netStatsCached_->SetAppStats(info);
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService SetAppStats end");
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::SetDpaAppStats(const NetStatsInfo &info)
{
    NETMGR_LOG_D("Enter SetDpaAppStats.");
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService SetDpaAppStats start");
    if (netStatsCached_ == nullptr) {
        NETMGR_LOG_E("Cached is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netStatsCached_->SetDpaAppStats(info);
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService SetDpaAppStats end");
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::SaveSharingTraffic(const NetStatsInfo &infos)
{
    NETMGR_LOG_D("Enter SaveSharingTraffic");
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService SaveSharingTraffic start");
    if (netStatsCached_ == nullptr) {
        NETMGR_LOG_E("Cached is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    netStatsCached_->SaveSharingTraffic(infos);
    NetmanagerHiTrace::NetmanagerStartSyncTrace("NetStatsService SaveSharingTraffic end");
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::GetCookieRxBytes(uint64_t &stats, uint64_t cookie)
{
    return NetsysController::GetInstance().GetCookieStats(stats, static_cast<uint32_t>(StatsType::STATS_TYPE_RX_BYTES),
                                                          cookie);
}

int32_t NetStatsService::GetCookieTxBytes(uint64_t &stats, uint64_t cookie)
{
    return NetsysController::GetInstance().GetCookieStats(stats, static_cast<uint32_t>(StatsType::STATS_TYPE_TX_BYTES),
                                                          cookie);
}

void NetStatsService::MergeTrafficStats(std::vector<NetStatsInfoSequence> &statsInfoSequences, const NetStatsInfo &info,
                                        uint32_t currentTimestamp, bool isNeedMerge)
{
    NetStatsInfoSequence tmp;
    tmp.startTime_ = info.date_;
    tmp.endTime_ = info.date_;
    tmp.info_ = info;
    uint32_t previousTimestamp = currentTimestamp;
    if (!isNeedMerge) {
        previousTimestamp = currentTimestamp > DAY_SECONDS ? currentTimestamp - DAY_SECONDS : 0;
    }
    if (info.date_ > previousTimestamp) {
        statsInfoSequences.push_back(std::move(tmp));
        return;
    }
    auto findRet = std::find_if(
        statsInfoSequences.begin(), statsInfoSequences.end(), [&info, previousTimestamp](const auto &item) {
            return item.endTime_ < previousTimestamp && CommonUtils::IsSameNaturalDay(info.date_, item.endTime_);
        });
    if (findRet == statsInfoSequences.end()) {
        statsInfoSequences.push_back(std::move(tmp));
        return;
    }
    (*findRet).info_ += info;
}

bool NetStatsService::GetIfaceNamesFromManager(std::list<std::string> &ifaceNames)
{
    int32_t ret = NetManagerCenter::GetInstance().GetIfaceNames(BEARER_CELLULAR, ifaceNames);
    if (ret != NETMANAGER_SUCCESS || ifaceNames.empty()) {
        NETMGR_LOG_D("Iface list is empty, ret = %{public}d", ret);
        return false;
    }
    ifaceNames.sort();
    ifaceNames.erase(std::unique(ifaceNames.begin(), ifaceNames.end()), ifaceNames.end());
    return true;
}

std::unordered_map<uint32_t, SampleBundleInfo> NetStatsService::GetSampleBundleInfosForActiveUser()
{
    void *handler = dlopen(LIB_NET_BUNDLE_UTILS_PATH.c_str(), RTLD_LAZY | RTLD_NODELETE);
    if (handler == nullptr) {
        NETMGR_LOG_E("load lib failed, reason : %{public}s", dlerror());
        return std::unordered_map<uint32_t, SampleBundleInfo>{};
    }
    using GetNetBundleClass = INetBundle *(*)();
    auto getNetBundle = (GetNetBundleClass)dlsym(handler, "GetNetBundle");
    if (getNetBundle == nullptr) {
        NETMGR_LOG_E("GetNetBundle failed, reason : %{public}s", dlerror());
        dlclose(handler);
        return std::unordered_map<uint32_t, SampleBundleInfo>{};
    }
    auto netBundle = getNetBundle();
    if (netBundle == nullptr) {
        NETMGR_LOG_E("netBundle is nullptr");
        dlclose(handler);
        return std::unordered_map<uint32_t, SampleBundleInfo>{};
    }
    std::optional<std::unordered_map<uint32_t, SampleBundleInfo>> result = netBundle->ObtainBundleInfoForActive();
    dlclose(handler);
    if (!result.has_value()) {
        NETMGR_LOG_W("ObtainBundleInfoForActive is nullopt");
        return std::unordered_map<uint32_t, SampleBundleInfo>{};
    }
    return result.value();
}

SampleBundleInfo NetStatsService::GetSampleBundleInfoForUid(uint32_t uid)
{
    void *handler = dlopen(LIB_NET_BUNDLE_UTILS_PATH.c_str(), RTLD_LAZY | RTLD_NODELETE);
    if (handler == nullptr) {
        NETMGR_LOG_E("load lib failed, reason : %{public}s", dlerror());
        return SampleBundleInfo{};
    }
    using GetNetBundleClass = INetBundle *(*)();
    auto getNetBundle = (GetNetBundleClass)dlsym(handler, "GetNetBundle");
    if (getNetBundle == nullptr) {
        NETMGR_LOG_E("GetNetBundle failed, reason : %{public}s", dlerror());
        dlclose(handler);
        return SampleBundleInfo{};
    }
    auto netBundle = getNetBundle();
    if (netBundle == nullptr) {
        NETMGR_LOG_E("netBundle is nullptr");
        dlclose(handler);
        return SampleBundleInfo{};
    }
    std::optional<SampleBundleInfo> result = netBundle->ObtainBundleInfoForUid(uid);
    dlclose(handler);
    if (!result.has_value()) {
        NETMGR_LOG_W("ObtainBundleInfoForUid is nullopt");
        return SampleBundleInfo{};
    }
    return result.value();
}

void NetStatsService::RefreshUidStatsFlag(uint64_t delay)
{
    std::function<void()> uidInstallSourceFunc = [this]() {
        auto tmp = GetSampleBundleInfosForActiveUser();
        for (auto iter = tmp.begin(); iter != tmp.end(); ++iter) {
            if (CommonUtils::IsSim(iter->second.bundleName_) ||
                CommonUtils::IsSim2(iter->second.bundleName_)) {
                netStatsCached_->SetUidSimSampleBundle(iter->first, iter->second);
            }
        }
        netStatsCached_->ClearUidStatsFlag();
        netStatsCached_->SetUidStatsFlag(tmp);
    };
    ffrt::submit(std::move(uidInstallSourceFunc), {}, {}, ffrt::task_attr().name("RefreshUidStatsFlag").delay(delay));
}

void NetStatsService::AddUidStatsFlag(uint64_t delay)
{
    std::shared_ptr<NetStatsService> selfPtr = shared_from_this();
    std::function<void()> uidInstallSourceFunc = [selfPtr]() {
        if (selfPtr->isUpdate_ || selfPtr->netStatsCached_ == nullptr) {
            return;
        }

        auto tmp = selfPtr->GetSampleBundleInfosForActiveUser();
        for (auto iter = tmp.begin(); iter != tmp.end(); ++iter) {
            if (CommonUtils::IsSim(iter->second.bundleName_) ||
                CommonUtils::IsSim2(iter->second.bundleName_)) {
                selfPtr->netStatsCached_->SetUidSimSampleBundle(iter->first, iter->second);
            }
        }
        selfPtr->netStatsCached_->SetUidStatsFlag(tmp);
        selfPtr->isUpdate_ = true;
    };
    ffrt::submit(std::move(uidInstallSourceFunc), {}, {}, ffrt::task_attr().name("AddUidStatsFlag").delay(delay));
}

bool NetStatsService::CommonEventPackageAdded(uint32_t uid)
{
    SampleBundleInfo sampleBundleInfo = GetSampleBundleInfoForUid(uid);
    if (CommonUtils::IsSim(sampleBundleInfo.bundleName_) ||
        CommonUtils::IsSim2(sampleBundleInfo.bundleName_)) {
        uint64_t delay = 0;
        if (netStatsCached_->GetUidSimSampleBundlesSize() == 0) {
            delay = DELAY_US;
            netStatsCached_->ForceCachedStats();
        }
        RefreshUidStatsFlag(delay);
    } else {
        std::unordered_map<uint32_t, SampleBundleInfo> tmp{{uid, sampleBundleInfo}};
        netStatsCached_->SetUidStatsFlag(tmp);
    }
    return true;
}

void NetStatsService::StartNetObserver()
{
    std::unique_lock<ffrt::shared_mutex> lock(netconnCallbackMutex_);
    NETMGR_LOG_I("StartNetObserver start");
    if (netconnCallback_ == nullptr) {
        netconnCallback_ = sptr<NetInfoObserver>::MakeSptr();
    }
    NetManagerStandard::NetSpecifier netSpecifier;
    NetManagerStandard::NetAllCapabilities netAllCapabilities;
    netAllCapabilities.netCaps_.insert(NetManagerStandard::NetCap::NET_CAPABILITY_INTERNET);
    netSpecifier.ident_ = "";
    netSpecifier.netCapabilities_ = netAllCapabilities;
    sptr<NetManagerStandard::NetSpecifier> specifier =
        sptr<NetManagerStandard::NetSpecifier>::MakeSptr(netSpecifier);
    int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(specifier, netconnCallback_, 0);
    if (ret != 0) {
        NETMGR_LOG_E("StartNetObserver fail, ret = %{public}d", ret);
        return;
    }
}
 
void NetStatsService::ProcessDefaultSimIdChanged(std::string simId)
{
    netStatsCached_->CacheStatsSim();
    NetsysController::GetInstance().ClearSimStatsBpfMap();
    netStatsCached_->UpdateDefaultSimId(simId);
}

bool NetStatsService::CommonEventPackageRemoved(uint32_t uid)
{
    if (static_cast<int32_t>(uid / USER_ID_DIVIDOR) != netStatsCached_->GetCurDefaultUserId() &&
        static_cast<int32_t>(uid / USER_ID_DIVIDOR) != SYSTEM_DEFAULT_USERID &&
        static_cast<int32_t>(uid / USER_ID_DIVIDOR) != netStatsCached_->GetCurPrivateUserId() &&
        static_cast<int32_t>(uid / USER_ID_DIVIDOR_SIM) != SIM_PRIVATE_USERID) {
        NETMGR_LOG_E("CommonEventPackageRemoved uid:%{public}d", uid);
        return true;
    }
    auto handler = std::make_unique<NetStatsDataHandler>();
    if (handler == nullptr) {
        NETMGR_LOG_E("Net Manager package removed, get db handler failed. uid:[%{public}d]", uid);
        return static_cast<int32_t>(NETMANAGER_ERR_INTERNAL);
    }
    auto ret1 = handler->UpdateStatsFlag(uid, STATS_DATA_FLAG_UNINSTALLED);
    if (ret1 != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Net Manager update stats flag failed, uid:[%{public}d]", uid);
    }
    auto ret2 = handler->UpdateSimStatsFlag(uid, STATS_DATA_FLAG_UNINSTALLED);
    if (ret2 != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Net Manager update sim stats flag failed, uid:[%{public}d]", uid);
    }
    auto ffrtHandle = netStatsCached_->ForceArchiveStats(uid);
    if (netStatsCached_->GetUidSimSampleBundle(uid).has_value()) {
        ffrt::wait({ffrtHandle});
        RefreshUidStatsFlag(0);
    }
    return ret1 != NETMANAGER_SUCCESS ? ret1 : ret2;
}

int32_t NetStatsService::CheckNetManagerAvailable()
{
    if (!NetManagerPermission::IsSystemCaller()) {
        NETMGR_LOG_E("Permission check failed.");
        return NETMANAGER_ERR_NOT_SYSTEM_CALL;
    }
    if (!NetManagerPermission::CheckPermission(Permission::GET_NETWORK_STATS)) {
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    return NETMANAGER_SUCCESS;
}

void NetStatsService::StartAccountObserver()
{
    NETMGR_LOG_I("StartAccountObserver start");
    std::set<AccountSA::OsAccountState> states = {
        AccountSA::OsAccountState::STOPPING, AccountSA::OsAccountState::CREATED,
        AccountSA::OsAccountState::SWITCHING, AccountSA::OsAccountState::SWITCHED, AccountSA::OsAccountState::UNLOCKED,
        AccountSA::OsAccountState::STOPPED, AccountSA::OsAccountState::REMOVED };
    bool withHandShake = false;
    AccountSA::OsAccountSubscribeInfo subscribeInfo(states, withHandShake);
    accountSubscriber_ = std::make_shared<NetStatsAccountSubscriber>(subscribeInfo);
    ErrCode errCode = AccountSA::OsAccountManager::SubscribeOsAccount(accountSubscriber_);
    if (errCode != 0) {
        NETMGR_LOG_E("SubscribeOsAccount error. errCode:%{public}d", errCode);
    }
    NETMGR_LOG_I("StartAccountObserver end");
}

int32_t NetStatsService::SetCalibrationTraffic(uint32_t simId, int64_t remainingData, uint64_t totalMonthlyData)
{
    NETMGR_LOG_I("SetCalibrationTraffic. simId:%{public}d, remainingData:%{public}" PRId64 "\
, totalMonthlyData:%{public}" PRIu64, simId, remainingData, totalMonthlyData);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }

    if (!NetStatsUtils::IsSimIdValid(simId)) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    if (remainingData > 0 && static_cast<uint64_t>(remainingData) > totalMonthlyData) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    netStatsCached_->CacheIfaceStats();
    uint64_t usedTraffic = 0;
    if (totalMonthlyData != UINT64_MAX) {
        usedTraffic = totalMonthlyData - static_cast<uint64_t>(remainingData);
        netStatsCalibrate_->UpdateCalibrationInfo(simId, usedTraffic);
    }

#ifndef UNITTEST_FORBID_FFRT
    if (!trafficPlanFfrtQueue_) {
        return NETMANAGER_ERR_INTERNAL;
    }
    trafficPlanFfrtQueue_->submit([this, simId, totalMonthlyData, remainingData]() {
#endif
        auto infoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
        if (totalMonthlyData != UINT64_MAX && infoPtr) {
            infoPtr->trafficLimit = totalMonthlyData;
        }
        if (totalMonthlyData == UINT64_MAX && infoPtr) {
            uint64_t usedTraffic = infoPtr->trafficLimit - static_cast<uint64_t>(remainingData);
            if (remainingData > 0 && infoPtr->trafficLimit < static_cast<uint64_t>(remainingData)) {
                usedTraffic = 0;
            }
            netStatsCalibrate_->UpdateCalibrationInfo(simId, usedTraffic);
        }

        trafficPlanService_->ResetNotifyState(simId);
        UpdateHistoryData(simId);
        UpdateBpfMap(simId);
#ifndef UNITTEST_FORBID_FFRT
    });
#endif
    return NETMANAGER_SUCCESS;
#else
    NETMGR_LOG_E("not support set calibration traffic");
    return NETMANAGER_ERR_CAPABILITY_NOT_SUPPORTED;
#endif // SUPPORT_TRAFFIC_STATISTIC
}

int32_t NetStatsService::SetTrafficPlanInfo(int32_t simId, int32_t param, int64_t value)
{
    NETMGR_LOG_I("SetTrafficPlanInfo start, simId: %{public}d, param: %{public}d, value: %{public}" PRId64,
                 simId, param, value);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }

    if (trafficPlanService_ == nullptr) {
        NETMGR_LOG_E("trafficPlanService_ is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }

    if (param < static_cast<int32_t>(TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH) ||
        param > static_cast<int32_t>(TrafficPlanParam::DAILY_LIMIT_PERCENTAGE)) {
        NETMGR_LOG_E("Invalid traffic plan param: %{public}d", param);
        return TRAFFIC_PLAN_ERR_INVALID_PARAM;
    }

    int32_t ret = trafficPlanService_->SetTrafficPlanInfo(simId, static_cast<TrafficPlanParam>(param), value);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("SetTrafficPlanInfo failed, ret: %{public}d", ret);
        return ret;
    }
    uint8_t flag = 0;
    NETMGR_LOG_I("TrafficPlanParamToFlag start");
    if (trafficPlanService_->TrafficPlanParamToFlag(static_cast<TrafficPlanParam>(param), flag)) {
        UpdateSettingsdata(simId, flag, value);
    } else {
        NETMGR_LOG_I("no need update settings data");
    }

    NETMGR_LOG_I("NetStatsService::SetTrafficPlanInfo success");
    return NETMANAGER_SUCCESS;
#else
    return NETMANAGER_ERR_INTERNAL;
#endif
}

int32_t NetStatsService::GetTrafficPlanInfo(int32_t simId, int32_t param, int64_t &value)
{
    NETMGR_LOG_I("GetTrafficPlanInfo start, simId: %{public}d, param: %{public}d",
                 simId, static_cast<int32_t>(param));
#ifdef SUPPORT_TRAFFIC_STATISTIC
    int32_t checkPermission = CheckNetManagerAvailable();
    if (checkPermission != NETMANAGER_SUCCESS) {
        return checkPermission;
    }

    if (trafficPlanService_ == nullptr) {
        NETMGR_LOG_E("trafficPlanService_ is nullptr");
        return NETMANAGER_ERR_INTERNAL;
    }

    if (param < static_cast<int32_t>(TrafficPlanParam::DISPLAY_TRAFFIC_SWITCH) ||
        param > static_cast<int32_t>(TrafficPlanParam::DAILY_LIMIT_PERCENTAGE)) {
        NETMGR_LOG_E("Invalid traffic plan param: %{public}d", param);
        return TRAFFIC_PLAN_ERR_INVALID_PARAM;
    }

    int32_t ret = trafficPlanService_->GetTrafficPlanInfo(simId, static_cast<TrafficPlanParam>(param), value);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("GetTrafficPlanInfo failed, ret: %{public}d", ret);
        return ret;
    }

    NETMGR_LOG_I("GetTrafficPlanInfo success, value: %{public}" PRId64, value);
    return NETMANAGER_SUCCESS;
#else
    return NETMANAGER_ERR_INTERNAL;
#endif
}

#ifdef SUPPORT_TRAFFIC_STATISTIC
void NetStatsService::UpdateBpfMapTimer()
{
    NETMGR_LOG_I("UpdateBpfMapTimer start");
    if (!trafficPlanFfrtQueue_) {
        NETMGR_LOG_E("FFRT Init Fail");
        return;
    }
#ifndef UNITTEST_FORBID_FFRT
    trafficPlanFfrtQueue_->submit([this]() {
#endif
        int32_t primarySlotId = NetStatsUtils::GetPrimarySlotId();
        int32_t primarySimId = Telephony::CoreServiceClient::GetInstance().GetSimId(primarySlotId);
        int slaveSlotId = primarySlotId == 0 ? 1 : 0;
        int32_t slaveSimId = Telephony::CoreServiceClient::GetInstance().GetSimId(slaveSlotId);
        UpdateBpfMap(primarySimId);
        UpdateBpfMap(slaveSimId);
#ifndef UNITTEST_FORBID_FFRT
    });
#endif
}

void NetStatsService::UpdateBpfMapTimerTask()
{
#ifndef UNITTEST_FORBID_FFRT
    ffrt::submit([this] {
#endif
        UpdateBpfMapTimer();
#ifndef UNITTEST_FORBID_FFRT
        }, ffrt::task_attr().name("update_bpfmap_timer_task").delay(UPDATE_BPF_MAP_DELAY_US));
#endif
}

bool NetStatsService::CommonEventSimStateChanged(int32_t slotId, int32_t simState)
{
    if (!trafficPlanFfrtQueue_) {
        NETMGR_LOG_E("FFRT Init Fail");
        return false;
    }

#ifndef UNITTEST_FORBID_FFRT
    trafficPlanFfrtQueue_->submit([this, slotId, simState]() {
#endif
        CommonEventSimStateChangedFfrt(slotId, simState);
#ifndef UNITTEST_FORBID_FFRT
    });
#endif
    return true;
}

bool NetStatsService::CommonEventSimStateChangedFfrt(int32_t slotId, int32_t simState)
{
    NETMGR_LOG_I("CommonEventSimStateChanged slotId: %{public}d, simState:%{public}d", slotId,  simState);
    if (!NetStatsUtils::IsSlotIdValid(slotId)) {
        NETMGR_LOG_E("CommonEventSimStateChanged slotId invalid, value: %{public}d", slotId);
        return false;
    }

    int32_t simId = Telephony::CoreServiceClient::GetInstance().GetSimId(slotId);

    if (simState == static_cast<int32_t>(Telephony::SimState::SIM_STATE_LOADED)) {
        if (netStatsCalibrate_->InitCalibrationInfo(simId)) {
            UpdateHistoryData(simId);
        }

        if (!trafficPlanService_->IsSimIdExistInMap(simId)) {
            // Initialize traffic plan info for this SIM card when it's loaded
            trafficPlanService_->InitTrafficPlanInfo(simId);
            trafficPlanService_->UpdateNetStatsToMapFromDB(simId);
            NETMGR_LOG_I("settingsTrafficMap_.insert(simId). simId:%{public}d", simId);
        } else {
            NETMGR_LOG_I("settingsTrafficMap_ has simId:%{public}d", simId);
        }
    } else if (simState == static_cast<int32_t>(Telephony::SimState::SIM_STATE_NOT_PRESENT)) {
        trafficPlanService_->DeleteTrafficPlanInfo(slotId);
    }
    return true;
}

bool NetStatsService::CommonEventCellularDataStateChanged(int32_t slotId, int32_t dataState)
{
    UpdateNetStatusMapCellular(dataState);
    if (!trafficPlanFfrtQueue_) {
        NETMGR_LOG_E("FFRT Init Fail");
        return false;
    }
#ifndef UNITTEST_FORBID_FFRT
    trafficPlanFfrtQueue_->submit([this, slotId, dataState]() {
#endif
        CellularDataStateChangedFfrt(slotId, dataState);
#ifndef UNITTEST_FORBID_FFRT
    });
#endif
    return true;
}

void NetStatsService::UpdateNetStatusMapCellular(int32_t dataState)
{
    if (dataState == static_cast<int32_t>(Telephony::DataConnectState::DATA_STATE_CONNECTED)) {
        UpdateNetStatusMap(NetStatusType::CELLULAR_TYPE, NetStatusConn::CONNECTED);
    } else {
        UpdateNetStatusMap(NetStatusType::CELLULAR_TYPE, NetStatusConn::NON_CONNECTED);
    }
}

bool NetStatsService::CellularDataStateChangedFfrt(int32_t slotId, int32_t dataState)
{
    NETMGR_LOG_I("slotId:%{public}d, dateState:%{public}d", slotId, dataState);
    if (!NetStatsUtils::IsSlotIdValid(slotId)) {
        return false;
    }

    int32_t simId = Telephony::CoreServiceClient::GetInstance().GetSimId(slotId);

    netStatsCalibrate_->InitCalibrationInfo(simId);
    if (!trafficPlanService_->GetTrafficPlanInfoBySimId(simId)) {
        trafficPlanService_->InitTrafficPlanInfo(simId);
        trafficPlanService_->UpdateNetStatsToMapFromDB(simId);
    }

    if (dataState != static_cast<int32_t>(Telephony::DataConnectState::DATA_STATE_CONNECTED)) {
        uint64_t ifIndex = UINT64_MAX;
        if (GetIfIndex(simId, ifIndex)) {
            NETMGR_LOG_E("simIdToIfIndexMap erase, simId: %{public}d", simId);
            ClearTrafficMapBySlotId(slotId, ifIndex);
            std::unique_lock<ffrt::shared_mutex> lock(simIdToIfIndexMapMutex_);
            simIdToIfIndexMap_.erase(simId);
        }
        return true;
    }

    int32_t ret = NetConnClient::GetInstance().GetIfaceNameIdentMaps(
        NetBearType::BEARER_CELLULAR, ifaceNameIdentMap_);
    if (ret != NETMANAGER_SUCCESS || ifaceNameIdentMap_.IsEmpty()) {
        NETMGR_LOG_E("error or empty.ret: %{public}d, ifaceNameIdentMap size: %{public}u",
            ret, ifaceNameIdentMap_.Size());
        return false;
    }
    NETMGR_LOG_I("ifaceNameIdentMap size: %{public}d", ifaceNameIdentMap_.Size());
    uint64_t ifIndex = UINT64_MAX;
    ifaceNameIdentMap_.Iterate([this, simId, &ifIndex](const std::string &k, const std::string &v) {
        if (v == std::to_string(simId)) {
            ifIndex = if_nametoindex(k.c_str());
            NETMGR_LOG_E("curIfIndex_:%{public}" PRIu64, ifIndex);
        }
    });
    uint64_t ifIndexRecords = UINT64_MAX;
    if (GetIfIndex(simId, ifIndexRecords) && ifIndexRecords == ifIndex) {
        NETMGR_LOG_E("not need process");
        return true;
    }
    UpdateCurActiviteSimChanged(simId, ifIndex);
    return true;
}

void NetStatsService::StartTrafficOvserver()
{
    NETMGR_LOG_I("StartTrafficOvserver start");
    if (trafficObserver_ == nullptr) {
        trafficObserver_ = std::make_unique<TrafficObserver>().release();
    }
    if (trafficObserver_ == nullptr) {
        return;
    }
    int32_t ret = NetsysController::GetInstance().RegisterNetsysTrafficCallback(trafficObserver_);
    if (ret != 0) {
        NETMGR_LOG_E("StartTrafficOvserver fail, ret = %{public}d", ret);
        return;
    }
}

// LCOV_EXCL_STOP

void NetStatsService::UpdateCurActiviteSimChanged(int32_t simId, uint64_t ifIndex)
{
    AddSimIdInTwoMap(simId, ifIndex);

    auto infoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!infoPtr) {
        NETMGR_LOG_E("UpdateCurActiviteSimChanged infoPtr nullptr");
        return;
    }
    NETMGR_LOG_I("AddSimIdInTwoMap insert settingsInfo: %{public}s", infoPtr->ToString().c_str());

    int32_t slotId = Telephony::CoreServiceClient::GetInstance().GetSlotId(simId);
    if (infoPtr->trafficLimit == UINT64_MAX || infoPtr->unlimitTrafficSwitch == 1) {
        SetTrafficMapMaxValue(slotId);
    } else {
        UpdateBpfMap(simId);
    }
}

// LCOV_EXCL_START
bool NetStatsService::IsSimIdExist(int32_t simId)
{
    std::shared_lock<ffrt::shared_mutex> lock(simIdToIfIndexMapMutex_);
    return simIdToIfIndexMap_.find(simId) != simIdToIfIndexMap_.end();
}

bool NetStatsService::GetIfIndex(int32_t simId, uint64_t &ifIndex)
{
    ifIndex = UINT64_MAX;
    std::shared_lock<ffrt::shared_mutex> lock(simIdToIfIndexMapMutex_);
    auto itIfIndex = simIdToIfIndexMap_.find(simId);
    if (itIfIndex == simIdToIfIndexMap_.end()) {
        return false;
    }
    ifIndex = itIfIndex->second;
    return true;
}

void NetStatsService::AddSimIdInTwoMap(int32_t simId, uint64_t ifIndex)
{
    NETMGR_LOG_I("AddSimIdInTwoMap. simId:%{public}d, ifIndex:%{public}" PRIu64, simId, ifIndex);
    uint64_t ifIndexRecords = UINT64_MAX;
    if (GetIfIndex(simId, ifIndexRecords)) {
        int32_t slotId = Telephony::CoreServiceClient::GetInstance().GetSlotId(simId);
        if (!NetStatsUtils::IsSlotIdValid(slotId)) {
            return;
        }
        ClearTrafficMapBySlotId(slotId, ifIndexRecords);
    }
    std::unique_lock<ffrt::shared_mutex> lock(simIdToIfIndexMapMutex_);
    simIdToIfIndexMap_[simId] = ifIndex;
    lock.unlock();

    if (!trafficPlanService_->GetTrafficPlanInfoBySimId(simId)) {
        NETMGR_LOG_E("trafficPlanInfoMap_ not find simId: %{public}d", simId);
        trafficPlanService_->InitTrafficPlanInfo(simId);
        trafficPlanService_->UpdateNetStatsToMapFromDB(simId);
    }
}

void NetStatsService::ClearTrafficMapBySlotId(int32_t slotId, uint64_t ifIndex)
{
    NETMGR_LOG_I("ClearTrafficMapBySlotId slotId:%{public}d, ifIndex: %{public}" PRIu64, slotId, ifIndex);
    NetsysController::GetInstance().DeleteIncreaseTrafficMap(ifIndex);
    NetsysController::GetInstance().UpdateIfIndexMap(slotId, UINT64_MAX);
    SetTrafficMapMaxValue(slotId);
}

void NetStatsService::GetAllUsedCellularTraffic(const sptr<NetStatsNetwork> &network, uint64_t &allUsedTraffic)
{
    allUsedTraffic = 0;
    std::vector<NetStatsInfo> netStatsInfos;
    // history
    GetHistoryTrafficInfo(network, netStatsInfos, true);  // true: contain calibrate data
    // cached
    netStatsCached_->GetIfaceStatsCached(netStatsInfos);
    // bpfmap
    netStatsCached_->GetKernelRmnetIfaceStats(netStatsInfos);

    for (auto it = netStatsInfos.begin(); it != netStatsInfos.end(); ++it) {
        if (it->ident_ == std::to_string(network->simId_)) {
            allUsedTraffic += it->rxBytes_;
            allUsedTraffic += it->txBytes_;
        }
    }
    NETMGR_LOG_I("GetAllUsedCellularTraffic simId:%{public}d, startTime: %{public}" PRIu64 ", \
endTime: %{public}" PRIu64 ", allUsedTraffic: %{public}" PRIu64,
        network->simId_, network->startTime_, network->endTime_, allUsedTraffic);
    return;
}

// 柱状图
void NetStatsService::GetDailyTrafficStatsByNetwork(const sptr<NetStatsNetwork> &network,
    std::vector<NetStatsInfoSequence> &infos)
{
    std::vector<NetStatsInfo> netStatsInfos;
    GetHistoryTrafficInfo(network, netStatsInfos, false);
    netStatsCached_->GetIfaceStatsCached(netStatsInfos);
    netStatsCached_->GetKernelRmnetIfaceStats(netStatsInfos);

    for (auto info : netStatsInfos) {
        MergeTrafficStats(infos, info, network->endTime_, true);
    }
}

void NetStatsService::GetHistoryTrafficInfo(const sptr<NetStatsNetwork> &network,
    std::vector<NetStatsInfo> &infos, bool isNeedCalibrate)
{
    if (network == nullptr) {
        return;
    }

    CalibrateInfo calibrateInfo;
    uint32_t changeToIfaceTime = netStatsCalibrate_->GetChangeToIfaceTime();
    bool ret = netStatsCalibrate_->GetCalibrationInfo(network->simId_, calibrateInfo);
    NETMGR_LOG_I("GetHistoryTrafficInfo ret:%{public}d, cali.start:%{public}u, network->startTime_:%{public}" PRIu64 "\
 cali.end:%{public}u, network->endTime_:%{public}" PRIu64 ", changeToIfaceTime:%{public}d",
        ret, calibrateInfo.startTime, network->startTime_,
        calibrateInfo.endTime, network->endTime_, changeToIfaceTime);

    if (ret && calibrateInfo.startTime >= network->startTime_ && calibrateInfo.endTime <= network->endTime_ &&
        isNeedCalibrate) {
        NETMGR_LOG_I("GetHistoryTrafficInfo  cali + iface");
        NetStatsNetwork networkTmp;
        networkTmp.type_ = network->type_;
        networkTmp.startTime_ = calibrateInfo.endTime;
        networkTmp.endTime_ = network->endTime_;
        networkTmp.simId_ = network->simId_;
        int32_t ret = GetHitstoryTrafficInIfaceTable(networkTmp, infos);

        NetStatsInfo caliNetStatsInfo;
        caliNetStatsInfo.ident_ = std::to_string(network->simId_);
        caliNetStatsInfo.date_ = calibrateInfo.endTime;
        caliNetStatsInfo.uid_ = CALIBRATE_UID;
        caliNetStatsInfo.iface_ = "rmnet0"; // calibrate data defalut in rmnet0
        caliNetStatsInfo.rxBytes_ = calibrateInfo.usedTraffic;
        infos.push_back(caliNetStatsInfo);
    } else if (changeToIfaceTime > network->startTime_ && changeToIfaceTime < network->endTime_) {
        NETMGR_LOG_I("GetHistoryTrafficInfo  uid + iface");
        NetStatsNetwork networkTmp;
        networkTmp.type_ = network->type_;
        networkTmp.startTime_ = network->startTime_;
        networkTmp.endTime_ = changeToIfaceTime;
        networkTmp.simId_ = network->simId_;

        std::vector<NetStatsInfo> infosUid;
        GetHistoryTrafficInUidTable(networkTmp, infosUid);
        networkTmp.startTime_ = changeToIfaceTime;
        networkTmp.endTime_ = network->endTime_;
        std::vector<NetStatsInfo> infosIface;
        GetHitstoryTrafficInIfaceTable(networkTmp, infosIface);
        infos.insert(infos.end(), infosUid.begin(), infosUid.end());
        infos.insert(infos.end(), infosIface.begin(), infosIface.end());
    } else {
        GetHitstoryTrafficInIfaceTable(*network, infos);
    }

    PrintSumNetStatsInfo(infos);
}

void NetStatsService::PrintSumNetStatsInfo(const std::vector<NetStatsInfo> &infos)
{
    uint64_t allUsedTraffic = 0;
    for (auto it = infos.begin(); it != infos.end(); ++it) {
        allUsedTraffic += it->rxBytes_;
        allUsedTraffic += it->txBytes_;
    }
    NETMGR_LOG_I("PrintSumNetStatsInfo: %{public}" PRIu64, allUsedTraffic);
}

int32_t NetStatsService::GetHistoryTrafficInUidTable(const NetStatsNetwork &network,
    std::vector<NetStatsInfo> &infos)
{
    std::string ident;
    if (network.type_ == 0) {
        ident = std::to_string(network.simId_);
    }
    uint32_t start = network.startTime_;
    uint32_t end = network.endTime_;
    NETMGR_LOG_I("GetHistory UidTable: ident=%{public}s, start=%{public}u, end=%{public}u", ident.c_str(), start, end);
    auto history = std::make_unique<NetStatsHistory>();
    if (history == nullptr) {
        NETMGR_LOG_E("history is null");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret = history->GetHistoryByIdent(infos, ident, start, end);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("get history by ident failed, err code=%{public}d", ret);
        return ret;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsService::GetHitstoryTrafficInIfaceTable(const NetStatsNetwork &network,
    std::vector<NetStatsInfo> &infos)
{
    std::string ident;
    if (network.type_ == 0) {
        ident = std::to_string(network.simId_);
    }

    uint32_t start = network.startTime_;
    uint32_t end = network.endTime_;
    NETMGR_LOG_I("GetHitstory IfaceTable:ident=%{public}s,start=%{public}u,end=%{public}u", ident.c_str(), start, end);
    auto history = std::make_unique<NetStatsHistory>();
    if (history == nullptr) {
        NETMGR_LOG_E("history is null");
        return NETMANAGER_ERR_INTERNAL;
    }
    int32_t ret = history->GetIfaceTableHistoryByIdent(infos, ident, start, end);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("get history by ident failed, err code=%{public}d", ret);
        return ret;
    }
    return NETMANAGER_SUCCESS;
}

void NetStatsService::UpdateBpfMap(int32_t simId)
{
    NETMGR_LOG_I("UpdateBpfMap start. simId:%{public}d", simId);
    uint64_t ifIndex = UINT64_MAX;

    auto infoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!infoPtr || !GetIfIndex(simId, ifIndex)) {
        NETMGR_LOG_E("UpdateBpfMap error. simId: %{public}d error", simId);
        return;
    }

    int32_t slotId = Telephony::CoreServiceClient::GetInstance().GetSlotId(simId);
    if (!NetStatsUtils::IsSlotIdValid(slotId)) {
        return;
    }

    NetsysController::GetInstance().DeleteIncreaseTrafficMap(ifIndex);
    NetsysController::GetInstance().UpdateIfIndexMap(slotId, ifIndex);

    PrintTrafficSettingsMapInfo(simId);

    uint64_t monthlyAvailable = UINT64_MAX;
    uint64_t monthlyMarkAvailable = UINT64_MAX;
    uint64_t dailyMarkAvailable = UINT64_MAX;
    bool ret = CalculateTrafficAvailable(simId, monthlyAvailable, monthlyMarkAvailable, dailyMarkAvailable);
    if (!ret) {
        NETMGR_LOG_E("CalculateTrafficAvailable error or open unlimit");
        return;
    }

    NETMGR_LOG_I("GetTrafficMap before write. monthlyAvailable:%{public}" PRIu64", \
monthlyMarkAvailable:%{public}" PRIu64", dailyMarkAvailable:%{public}" PRIu64,
        monthlyAvailable, monthlyMarkAvailable, dailyMarkAvailable);
    NetsysController::GetInstance().SetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_MONTHLY_LIMIT, monthlyAvailable);
    NetsysController::GetInstance().SetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_MONTHLY_MARK, monthlyMarkAvailable);
    NetsysController::GetInstance().SetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_DAILY_MARK, dailyMarkAvailable);

    PrintTrafficBpfMapInfo(slotId);

    if (infoPtr->trafficLimit == UINT64_MAX) {
        return;
    }

    if (monthlyAvailable == UINT64_MAX) {
        NotifyTrafficAlert(simId, NET_STATS_MONTHLY_LIMIT);
    } else if (monthlyMarkAvailable == UINT64_MAX) {
        NotifyTrafficAlert(simId, NET_STATS_MONTHLY_MARK);
    } else if (dailyMarkAvailable == UINT64_MAX) {
        NotifyTrafficAlert(simId, NET_STATS_DAILY_MARK);
    }
}

void NetStatsService::PrintTrafficSettingsMapInfo(int32_t simId)
{
    auto infoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (infoPtr != nullptr) {
        NETMGR_LOG_I("settingsInfo-> %{public}s", infoPtr->ToString().c_str());
    }
}

void NetStatsService::PrintTrafficBpfMapInfo(int32_t slotId)
{
    uint64_t monthlyAvailableMap = UINT64_MAX;
    uint64_t monthlyMarkAvailableMap = UINT64_MAX;
    uint64_t dailyMarkAvailableMap = UINT64_MAX;
    NetsysController::GetInstance().GetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_MONTHLY_LIMIT, monthlyAvailableMap);
    NetsysController::GetInstance().GetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_MONTHLY_MARK, monthlyMarkAvailableMap);
    NetsysController::GetInstance().GetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_DAILY_MARK, dailyMarkAvailableMap);
    NETMGR_LOG_I("GetTrafficMap after write. monthlyAvailable:%{public}" PRIu64", \
monthlyMarkAvailable:%{public}" PRIu64", dailyMarkAvailable:%{public}" PRIu64,
        monthlyAvailableMap, monthlyMarkAvailableMap, dailyMarkAvailableMap);
}

bool NetStatsService::CalculateTrafficAvailable(int32_t simId, uint64_t &monthlyAvailable,
    uint64_t &monthlyMarkAvailable, uint64_t &dailyMarkAvailable)
{
    NETMGR_LOG_I("CalculateTrafficAvailable enter");
    auto trafficPlaninfoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return false;
    }

    if (trafficPlaninfoPtr->trafficLimit== UINT64_MAX) {
        return true;
    }

    sptr<NetStatsNetwork> network = sptr<NetStatsNetwork>::MakeSptr();
    network->startTime_ =
        static_cast<uint64_t>(NetStatsUtils::GetStartTimestamp(trafficPlaninfoPtr->startDate));
    if (netStatsCalibrate_->IsExistCalibrationInfo(simId)) {  // 如果发生过校准，则需要查询1号开始
        network->startTime_ = static_cast<uint64_t>(NetStatsUtils::GetStartTimestamp(1));
        NETMGR_LOG_I("has Calibration");
    }

    network->endTime_ = static_cast<uint64_t>(NetStatsUtils::GetNowTimestamp());
    network->type_ = 0;
    network->simId_ = static_cast<uint32_t>(simId);
    uint64_t allUsedTraffic = 0;
    GetAllUsedCellularTraffic(network, allUsedTraffic);
    NETMGR_LOG_I("CalculateTrafficAvailable month used: %{public}" PRIu64, allUsedTraffic);

    if (trafficPlaninfoPtr->unlimitTrafficSwitch != 1) {
        if (trafficPlaninfoPtr->trafficLimit > allUsedTraffic) {
            monthlyAvailable = trafficPlaninfoPtr->trafficLimit - allUsedTraffic;
        }
        uint64_t monthTmp = (trafficPlaninfoPtr->trafficLimit / 100.0) *
            trafficPlaninfoPtr->monthlyLimitPercentage;

        if (monthTmp > allUsedTraffic) {
            monthlyMarkAvailable = monthTmp - allUsedTraffic;
        }

        uint64_t todayStartTime = static_cast<uint64_t>(NetStatsUtils::GetTodayStartTimestamp());
        network->startTime_ = todayStartTime;
        uint64_t allTodayUsedTraffix = 0;
        GetAllUsedCellularTraffic(network, allTodayUsedTraffix);
        NETMGR_LOG_I("CalculateTrafficAvailable today used: %{public}" PRIu64, allTodayUsedTraffix);

        uint64_t dayTmp = (trafficPlaninfoPtr->trafficLimit / 100.0) * trafficPlaninfoPtr->dailyLimitPercentage;
        if (dayTmp > allTodayUsedTraffix) {
            dailyMarkAvailable = dayTmp - allTodayUsedTraffix;
        }

        return true;
    }
    return false;
}

void NetStatsService::SetTrafficMapMaxValue()
{
    NETMGR_LOG_I("SetTrafficMapMaxValue");
    NetsysController::GetInstance().SetNetStateTrafficMap(NET_STATS_MONTHLY_LIMIT, UINT64_MAX);
    NetsysController::GetInstance().SetNetStateTrafficMap(NET_STATS_MONTHLY_MARK, UINT64_MAX);
    NetsysController::GetInstance().SetNetStateTrafficMap(NET_STATS_DAILY_MARK, UINT64_MAX);
}

void NetStatsService::SetTrafficMapMaxValue(int32_t slotId)
{
    NETMGR_LOG_I("SetTrafficMapMaxValue");
    if (!NetStatsUtils::IsSlotIdValid(slotId)) {
        return;
    }
    NetsysController::GetInstance().SetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_MONTHLY_LIMIT, UINT64_MAX);
    NetsysController::GetInstance().SetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_MONTHLY_MARK, UINT64_MAX);
    NetsysController::GetInstance().SetNetStateTrafficMap(
        slotId * TRAFFIC_NOTIFY_TYPE + NET_STATS_DAILY_MARK, UINT64_MAX);
}

int32_t NetStatsService::UpdateSettingsdata(int32_t simId, uint8_t flag, uint64_t value)
{
    NETMGR_LOG_I("SetTrafficUpdateSettingsdataMapMaxValue");
    switch (flag) {
        case NET_STATS_NO_LIMIT_ENABLE:
            break;
        case NET_STATS_MONTHLY_LIMIT:
            trafficPlanService_->ResetNotifyState(simId);
            break;
        case NET_STATS_BEGIN_DATE:
            ProcessUpdateBeginDate(simId, value);
            break;
        case NET_STATS_NOTIFY_TYPE:
            break;
        case NET_STATS_MONTHLY_MARK:
            trafficPlanService_->UpdateTrafficLimitDate(simId);
            break;
        case NET_STATS_DAILY_MARK:
            break;
        default:
            break;
    }
    NETMGR_LOG_I("before  ProcessSettingsDataUpdate");
    ProcessSettingsDataUpdate(simId);
    return NETMANAGER_SUCCESS;
}

void NetStatsService::ProcessSettingsDataUpdate(int32_t simId)
{
    NETMGR_LOG_I("ProcessSettingsDataUpdate start");
    if (!IsSimIdExist(simId)) {
        NETMGR_LOG_I("IsSimIdExist error");
        return;
    }
    NETMGR_LOG_I("UpdateBpfMap start");
    UpdateBpfMap(simId);
}

void NetStatsService::ProcessUpdateBeginDate(int32_t simId, uint32_t beginDate)
{
    CalibrateInfo info;
    netStatsCalibrate_->ReadCalibrationTrafficInfo(simId, info);
    uint64_t startTime = static_cast<uint64_t>(NetStatsUtils::GetStartTimestamp(beginDate));
    if (info.startTime == startTime && info.startTime != 0) {
        NETMGR_LOG_I("don't need delete calibration info");
        return;
    }
    netStatsCalibrate_->DeleteCalibrationInfo(simId);
    UpdateHistoryData(simId);
}

void NetStatsService::UpdateHistoryData(int32_t simId)
{
    if (simId <= 0) {
        NETMGR_LOG_E("UpdateHistoryData simId invalid");
        return;
    }
#ifdef SUPPORT_TRAFFIC_STATISTIC
    std::function<void()> UpdateHistoryData = [this, simId]() {
        auto infoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
        if (!infoPtr) {
            NETMGR_LOG_E("infoPtr nullptr");
            return;
        }
        int32_t beginDate = infoPtr->startDate;

        uint64_t allUsedTraffic = 0;
        sptr<NetStatsNetwork> network = std::make_unique<NetStatsNetwork>().release();
        network->startTime_ = static_cast<uint64_t>(NetStatsUtils::GetStartTimestamp(beginDate));
        bool existCali = netStatsCalibrate_->IsExistCalibrationInfo(simId);
        if (existCali) {
            network->startTime_ = static_cast<uint64_t>(NetStatsUtils::GetStartTimestamp(1));
        }
        network->endTime_ = static_cast<uint64_t>(NetStatsUtils::GetEndTimestamp(beginDate));
        network->type_ = 0;  // 0: cellular
        network->simId_ = static_cast<uint32_t>(simId);

        std::vector<NetStatsInfo> netStatsHistoryInfos;
        GetHistoryTrafficInfo(network, netStatsHistoryInfos, true); // true: 计算校准流量
        std::vector<NetStatsInfo> netStatsCachedInfos;
        netStatsCached_->GetIfaceStatsCached(netStatsCachedInfos);
        CalibrateInfo caliInfo;
        netStatsCalibrate_->GetCalibrationInfo(simId, caliInfo);
        NETMGR_LOG_I("GetCalibrationInfo simId:%{public}d, start:%{public}d, end:%{public}d, data:%{public}" PRIu64,
            simId, caliInfo.startTime, caliInfo.endTime, caliInfo.usedTraffic);

        for (auto it = netStatsHistoryInfos.begin(); it != netStatsHistoryInfos.end(); ++it) {
            allUsedTraffic += it->rxBytes_;
            allUsedTraffic += it->txBytes_;
        }

        for (auto it = netStatsCachedInfos.begin(); it != netStatsCachedInfos.end(); ++it) {
            if (it->date_ <= caliInfo.endTime || it->ident_ != std::to_string(simId)) {
                continue;
            }
            allUsedTraffic += it->rxBytes_;
            allUsedTraffic += it->txBytes_;
        }

        netStatsCached_->ForceUpdateHistoryData(simId, beginDate, allUsedTraffic);
    };
    ffrt::submit(std::move(UpdateHistoryData), {}, {}, ffrt::task_attr().name("UpdateHistoryData"));
#endif
}

void NetStatsService::DeleteHistoryData(int32_t simId)
{
#ifdef SUPPORT_TRAFFIC_STATISTIC
    netStatsCached_->DeleteHistoryData(simId);
#endif
}

void NetStatsService::UpdateAllHistoryDateInfo()
{
    int32_t simId0 = Telephony::CoreServiceClient::GetInstance().GetSimId(SLOT_0);
    int32_t simId1 = Telephony::CoreServiceClient::GetInstance().GetSimId(SLOT_1);
    NETMGR_LOG_I("UpdateAllHistoryDateInfo simId0: %{public}d ,simId1: %{public}d", simId0, simId1);
    UpdateHistoryData(simId0);
    UpdateHistoryData(simId1);
}

TrafficObserver::TrafficObserver() {}
TrafficObserver::~TrafficObserver() {}

int32_t TrafficObserver::OnExceedTrafficLimits(int8_t &flag)
{
    NETMGR_LOG_I("OnExceedTrafficLimits flag: %{public}d", flag);
    if (flag < NET_STATS_MONTHLY_LIMIT || flag > NET_STATS_DAILY_MARK + TRAFFIC_NOTIFY_TYPE * 1) {
        NETMGR_LOG_E("OnExceedTrafficLimits flag error. value: %{public}d", flag);
        return -1;
    }

    int8_t slotId = -1;
    if (flag == 0) {
        slotId = 0;
    } else {
        slotId = flag / TRAFFIC_NOTIFY_TYPE;
    }
    int8_t trafficFlag = flag - TRAFFIC_NOTIFY_TYPE * slotId;
    int32_t simId = Telephony::CoreServiceClient::GetInstance().GetSimId(slotId);
    if (simId < 0) {
        NETMGR_LOG_E("get simId error");
        return -1;
    }

    NetStatsService::GetInstance()->NotifyTrafficAlertFfrt(simId, trafficFlag);
    return 0;
}

int32_t NetStatsService::NotifyTrafficAlert(int32_t simId, uint8_t flag)
{
    if (!IsSimIdExist(simId)) {
        NETMGR_LOG_E("simIdToIfIndexMap not find simId: %{public}d", simId);
        return -1;
    }

    if (NetStatsUtils::IsMobileDataEnabled() && GetNotifyStats(simId, flag)) {
        DealNotificaiton(simId, flag);
    } else {
        NETMGR_LOG_I("There is no need to pop up trafficLimit notification.");
    }
    return NETMANAGER_SUCCESS;
}
// LCOV_EXCL_STOP

int32_t NetStatsService::NotifyTrafficAlertFfrt(int32_t simId, uint8_t flag)
{
    if (!trafficPlanFfrtQueue_) {
        return NETMANAGER_ERR_INTERNAL;
    }
#ifndef UNITTEST_FORBID_FFRT
    trafficPlanFfrtQueue_->submit([this, simId, flag]() {
#endif
        NotifyTrafficAlert(simId, flag);
#ifndef UNITTEST_FORBID_FFRT
    });
#endif
    return NETMANAGER_SUCCESS;
}

// LCOV_EXCL_START
bool NetStatsService::GetNotifyStats(int32_t simId, uint8_t flag)
{
    auto trafficPlaninfoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return false;
    }
    if (trafficPlaninfoPtr->unlimitTrafficSwitch == 1) {
        NETMGR_LOG_I("simId: %{public}d, setting unlimitTrafficSwitch: true.", simId);
        return false;
    }
 
    switch (flag) {
        case NET_STATS_MONTHLY_LIMIT:
            return GetMonAlertStatus(simId);
        case NET_STATS_MONTHLY_MARK:
            return GetMonNotifyStatus(simId);
        case NET_STATS_DAILY_MARK:
            return GetDayNotifyStatus(simId);
        default:
            NETMGR_LOG_E("unknown notification type");
            return false;
    }
    return false;
}

bool NetStatsService::GetMonNotifyStatus(int32_t simId)
{
    NETMGR_LOG_I("Enter GetMonNotifyStatus.");
    auto trafficPlaninfoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return false;
    }
    if (trafficPlaninfoPtr->isCanNotifyMonthlyMark) {
        trafficPlaninfoPtr->isCanNotifyMonthlyMark = false;
        return true;
    }

    int32_t currentTime = NetStatsUtils::GetNowTimestamp();
    int32_t currentStartTime =
        NetStatsUtils::GetStartTimestamp(trafficPlaninfoPtr->startDate);
    NETMGR_LOG_I("Enter currentTime:%{public}d, currentDayStartTime:%{public}d, lastMonNotifyTime: %{public}d",
        currentTime, currentStartTime, trafficPlaninfoPtr->lastMonNotifyTime);
    if (trafficPlaninfoPtr->lastMonNotifyTime < currentStartTime) {
        return true;
    }
    return false;
}
 
bool NetStatsService::GetDayNotifyStatus(int32_t simId)
{
    NETMGR_LOG_I("Enter GetDayNotifyStatus.");

    auto trafficPlaninfoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return false;
    }
    if (trafficPlaninfoPtr->isCanNotifyDailyMark) {
        trafficPlaninfoPtr->isCanNotifyDailyMark = false;
        return true;
    }

    int32_t currentDayStartTime = NetStatsUtils::GetTodayStartTimestamp();
    NETMGR_LOG_I("Enter currentDayStartTime:%{public}d, lastDayNotifyTime: %{public}d",
        currentDayStartTime, trafficPlaninfoPtr->lastDayNotifyTime);
    if (trafficPlaninfoPtr->lastDayNotifyTime < currentDayStartTime) {
        return true;
    }
    return false;
}
 
bool NetStatsService::GetMonAlertStatus(int32_t simId)
{
    NETMGR_LOG_I("Enter GetMonAlertStatus.");

    auto trafficPlaninfoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return false;
    }
    if (trafficPlaninfoPtr->isCanNotifyMonthlyLimit) {
        NETMGR_LOG_I("isCanNotify true : states changed caused.");
        trafficPlaninfoPtr->isCanNotifyMonthlyLimit = false;
        trafficPlaninfoPtr->isCanNotifyMonthlyMark = false;
        trafficPlaninfoPtr->isCanNotifyDailyMark = false;
        return true;
    }
 
    int currentTime = NetStatsUtils::GetNowTimestamp();
    int currentStartTime = NetStatsUtils::GetStartTimestamp(trafficPlaninfoPtr->startDate);
    NETMGR_LOG_I("Enter currentTime:%{public}d, currentDayStartTime:%{public}d, lastMonAlertTime: %{public}d",
        currentTime, currentStartTime, trafficPlaninfoPtr->lastMonAlertTime);
    if (trafficPlaninfoPtr->lastMonAlertTime < currentStartTime) {
        return true;
    }
    return false;
}

void NetStatsService::DealNotificaiton(int32_t simId, uint8_t flag)
{
    NETMGR_LOG_I("Enter DealDayNotification.");
    int simNum = NetStatsUtils::IsDualCardEnabled();
    bool isDualCard = false;
    if (simNum == 0) {
        return;
    } else if (simNum == DUAL_CARD) {
        isDualCard = true;
    }
 
    switch (flag) {
        case NET_STATS_MONTHLY_LIMIT:
            return DealMonAlert(simId, isDualCard);
        case NET_STATS_MONTHLY_MARK:
            return DealMonNotification(simId, isDualCard);
        case NET_STATS_DAILY_MARK:
            return DealDayNotification(simId, isDualCard);
        default:
            NETMGR_LOG_I("unknown notificationdeal type");
    }
}

void NetStatsService::DealDayNotification(int32_t simId, bool isDualCard)
{
    NETMGR_LOG_I("Enter DealDayNotification.");

    auto trafficPlaninfoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return;
    }
    if (g_registerNotificationOps(gNetmgrStatsLmtNtf_)) {
        gNetmgrStatsLmtNtf_->PublishNetStatsLimitNotification(NETMGR_STATS_LIMIT_DAY, simId, isDualCard);
    }
    trafficPlaninfoPtr->lastDayNotifyTime = NetStatsUtils::GetNowTimestamp();
    trafficPlanService_->UpdateTrafficLimitDate(simId);
    NETMGR_LOG_I("update DayNotification time:%{public}d", trafficPlaninfoPtr->lastDayNotifyTime);
}
 
void NetStatsService::DealMonNotification(int32_t simId, bool isDualCard)
{
    NETMGR_LOG_I("Enter DealMonNotification.");
    auto trafficPlaninfoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return;
    }
    if (g_registerNotificationOps(gNetmgrStatsLmtNtf_)) {
        gNetmgrStatsLmtNtf_->PublishNetStatsLimitNotification(NETMGR_STATS_LIMIT_MONTH, simId, isDualCard);
    }
    trafficPlaninfoPtr->lastMonNotifyTime = NetStatsUtils::GetNowTimestamp();
    trafficPlanService_->UpdateTrafficLimitDate(simId);
    NETMGR_LOG_I("update MonNotification time:%{public}d", trafficPlaninfoPtr->lastMonNotifyTime);
}

void NetStatsService::DealMonAlert(int32_t simId, bool isDualCard)
{
    NETMGR_LOG_I("Enter DealMonAlert.");
    if (dialog_ == nullptr) {
        dialog_ = std::make_shared<TrafficLimitDialog>();
    }

    auto trafficPlaninfoPtr = trafficPlanService_->GetTrafficPlanInfoBySimId(simId);
    if (!trafficPlaninfoPtr) {
        return;
    }

    if (g_registerNotificationOps(gNetmgrStatsLmtNtf_)) {
        gNetmgrStatsLmtNtf_->PublishNetStatsLimitNotification(NETMGR_STATS_ALERT_MONTH, simId, isDualCard);
    }
    if (trafficPlaninfoPtr->overLimitBehavior) {
        dialog_->PopUpTrafficLimitDialog(simId);
    }
    trafficPlaninfoPtr->lastMonAlertTime = NetStatsUtils::GetNowTimestamp();
    trafficPlanService_->UpdateTrafficLimitDate(simId);
    NETMGR_LOG_I("update MonAlert time:%{public}d", trafficPlaninfoPtr->lastMonAlertTime);
}

bool NetStatsService::GetMonthlyLimitBySimId(int32_t simId, uint64_t &monthlyLimit)
{
    return trafficPlanService_->GetMonthlyLimitBySimId(simId, monthlyLimit);
}

bool NetStatsService::GetMonthlyMarkBySimId(int32_t simId, uint16_t &monthlyMark)
{
    return trafficPlanService_->GetMonthlyMarkBySimId(simId, monthlyMark);
}

bool NetStatsService::GetDailyMarkBySimId(int32_t simId, uint16_t &dailyMark)
{
    return trafficPlanService_->GetDailyMarkBySimId(simId, dailyMark);
}

void NetStatsService::SubscribeTelephonyInfo()
{
    if (telephonyInfoObserver_ == nullptr) {
        telephonyInfoObserver_ = sptr<TelephonyInfoObserver>::MakeSptr();
    }
    NETMGR_LOG_I("SubscribeTelephonyInfo start.");
    int32_t ret1 = Telephony::TelephonyObserverClient::GetInstance().AddStateObserver(telephonyInfoObserver_,
        SLOT_0, TELEPHONY_EVENT_MASK, true);
    int32_t ret2 = Telephony::TelephonyObserverClient::GetInstance().AddStateObserver(telephonyInfoObserver_,
        SLOT_1, TELEPHONY_EVENT_MASK, true);
    NETMGR_LOG_I("SubscribeTelephonyInfo result. ret1: %{public}d, ret2: %{public}d", ret1, ret2);
}

void TelephonyInfoObserver::OnSimStateUpdated(int32_t slotId, Telephony::CardType type,
    Telephony::SimState state, Telephony::LockReason reaso)
{
    NETMGR_LOG_I("OnSimStateUpdated start slot:%{public}d, state:%{public}d", slotId, state);
    int32_t simId = Telephony::CoreServiceClient::GetInstance().GetSimId(slotId);
    NETMGR_LOG_I("OnSimStateUpdated simId:%{public}d", simId);

    if (state == Telephony::SimState::SIM_STATE_NOT_PRESENT) {
        NetStatsService::GetInstance()->DeleteHistoryData(simId);
    } else if (state == Telephony::SimState::SIM_STATE_LOADED) {
        NetStatsService::GetInstance()->UpdateHistoryData(simId);
    }
    NetStatsService::GetInstance()->CommonEventSimStateChanged(slotId,
        static_cast<int32_t>(state));
}

void TelephonyInfoObserver::OnIccAccountUpdated()
{
    Telephony::SimState simState = Telephony::SimState::SIM_STATE_UNKNOWN;
    Telephony::CoreServiceClient::GetInstance().GetSimState(SLOT_0, simState);
    NETMGR_LOG_I("OnIccAccountUpdated slotId: 0, simState:%{public}d", static_cast<int32_t>(simState));
    if (simState == Telephony::SimState::SIM_STATE_LOADED) {
        NetStatsService::GetInstance()->CommonEventSimStateChanged(
            SLOT_0, static_cast<int32_t>(Telephony::SimState::SIM_STATE_LOADED));
    }
    Telephony::CoreServiceClient::GetInstance().GetSimState(SLOT_1, simState);
    NETMGR_LOG_I("OnIccAccountUpdated slotId: 1, simState:%{public}d", static_cast<int32_t>(simState));
    if (simState == Telephony::SimState::SIM_STATE_LOADED) {
        NetStatsService::GetInstance()->CommonEventSimStateChanged(
            SLOT_1, static_cast<int32_t>(Telephony::SimState::SIM_STATE_LOADED));
    }
}
#endif //SUPPORT_TRAFFIC_STATISTIC
// LCOV_EXCL_STOP
} // namespace NetManagerStandard
} // namespace OHOS

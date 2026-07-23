/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NET_STATS_SERVICE_H
#define NET_STATS_SERVICE_H

#include "singleton.h"
#include "system_ability.h"

#include "net_push_stats_info.h"
#include "net_stats_callback.h"
#include "net_stats_history.h"
#include "net_stats_info_sequence.h"
#include "net_stats_listener.h"
#include "net_stats_network.h"
#include "net_stats_service_stub.h"
#include "netlink_manager.h"
#include "net_bundle.h"
#include "net_stats_traffic_plan_service.h"
#include "traffic_plan_param.h"
#ifdef SUPPORT_TRAFFIC_STATISTIC
#include "ffrt_timer.h"
#include "netsys_traffic_callback_stub.h"
#include "netsys_controller_callback.h"
#include "net_stats_trafficLimit_dialog.h"
#include "telephony_observer.h"
#include "net_stats_notification_interface.h"
#endif // SUPPORT_TRAFFIC_STATISTIC
#include "network_sharing.h"
#include "net_stats_subscriber.h"
#include "safe_map.h"
#include "net_manager_constants.h"
#include "net_info_observer.h"
#include "net_stats_calibrate.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsCached;
#ifdef SUPPORT_TRAFFIC_STATISTIC
class TrafficObserver;
class NetsysControllerObserver;
class TelephonyInfoObserver;
#endif // SUPPORT_TRAFFIC_STATISTIC

class NetStatsService : public SystemAbility,
                        public NetStatsServiceStub,
                        public std::enable_shared_from_this<NetStatsService> {
    DECLARE_SYSTEM_ABILITY(NetStatsService)

public:
    NetStatsService();
    ~NetStatsService();
    static std::shared_ptr<NetStatsService> GetInstance();
    void OnStart() override;
    void OnStop() override;
    void StartNetObserver();
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    int32_t GetIfaceRxBytes(uint64_t &stats, const std::string &interfaceName) override;
    int32_t GetIfaceTxBytes(uint64_t &stats, const std::string &interfaceName) override;
    int32_t GetCellularRxBytes(uint64_t &stats) override;
    int32_t GetCellularTxBytes(uint64_t &stats) override;
    int32_t GetAllRxBytes(uint64_t &stats) override;
    int32_t GetAllTxBytes(uint64_t &stats) override;
    int32_t GetUidRxBytes(uint64_t &stats, uint32_t uid) override;
    int32_t GetUidTxBytes(uint64_t &stats, uint32_t uid) override;
    int32_t GetAllStatsInfo(std::vector<NetStatsInfo> &infos) override;
    int32_t GetAllSimStatsInfo(std::vector<NetStatsInfo> &infos) override;
    int32_t GetTrafficStatsByNetwork(std::unordered_map<uint32_t, NetStatsInfo> &infos,
                                     const NetStatsNetwork &networkIpc) override;
    int32_t GetTrafficStatsByUidNetwork(std::vector<NetStatsInfoSequence> &infos, uint32_t uid,
                                        const NetStatsNetwork &networkIpc) override;
    int32_t GetMonthTrafficStatsByNetwork(uint32_t simId, uint64_t &monthDataIpc) override;
    int32_t SetCalibrationTraffic(uint32_t simId, int64_t remainingData, uint64_t totalMonthlyData) override;
    int32_t SetAppStats(const PushStatsInfo &info) override;
    int32_t SetDpaAppStats(const NetStatsInfo &info) override;
    int32_t RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback) override;
    int32_t UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback) override;
    int32_t GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                NetStatsInfo &statsInfo) override;
    int32_t GetUidStatsDetail(const std::string &iface, uint32_t uid, uint64_t start, uint64_t end,
                              NetStatsInfo &statsInfo) override;
    int32_t UpdateIfacesStats(const std::string &iface, uint64_t start, uint64_t end,
                              const NetStatsInfo &stats) override;
    int32_t UpdateStatsData() override;
    void StartAccountObserver();
    int32_t ProcessOsAccountChanged(int32_t toId, AccountSA::OsAccountState state);
    int32_t ResetFactory() override;
    int32_t GetCookieRxBytes(uint64_t &stats, uint64_t cookie) override;
    int32_t GetCookieTxBytes(uint64_t &stats, uint64_t cookie) override;
    int32_t SaveSharingTraffic(const NetStatsInfo &infos) override;
    void AddUidStatsFlag(uint64_t delay);
    void ProcessDefaultSimIdChanged(std::string simId);
    int32_t SetTrafficPlanInfo(int32_t simId, int32_t param, int64_t value) override;
    int32_t GetTrafficPlanInfo(int32_t simId, int32_t param, int64_t &value) override;
    int32_t OnExtension(const std::string& extension, MessageParcel& data, MessageParcel& reply) override;

#ifdef SUPPORT_TRAFFIC_STATISTIC
    int32_t UpdateSettingsdata(int32_t simId, uint8_t flag, uint64_t value);
    int32_t NotifyTrafficAlert(int32_t simId, uint8_t flag);
    int32_t NotifyTrafficAlertFfrt(int32_t simId, uint8_t flag);
    bool GetMonthlyLimitBySimId(int32_t simId, uint64_t &monthlyLimit);
    bool GetMonthlyMarkBySimId(int32_t simId, uint16_t &monthlyMark);
    bool GetDailyMarkBySimId(int32_t simId, uint16_t &dailyMark);

    void UpdateAllHistoryDateInfo();
    void UpdateHistoryData(int32_t simId);
    void DeleteHistoryData(int32_t simId);
    bool CommonEventSimStateChanged(int32_t slotId, int32_t simState);
#endif // SUPPORT_TRAFFIC_STATISTIC

private:
    bool Init();
    void GetDumpMessage(std::string &message);
    void MergeTrafficStats(std::vector<NetStatsInfoSequence> &statsInfoSequences, const NetStatsInfo &info,
                           uint32_t currentTime, bool isNeedMerge = false);
    bool GetIfaceNamesFromManager(std::list<std::string> &ifaceNames);
    std::unordered_map<uint32_t, SampleBundleInfo> GetSampleBundleInfosForActiveUser();
    SampleBundleInfo GetSampleBundleInfoForUid(uint32_t uid);
    void RefreshUidStatsFlag(uint64_t delay);
    void RegisterCommonEvent();
    bool CommonEventPackageAdded(uint32_t uid);
    bool CommonEventPackageRemoved(uint32_t uid);
    void FilterTrafficStatsByNetwork(std::vector<NetStatsInfo> &allInfo,
        std::unordered_map<uint32_t, NetStatsInfo> &infos,
        const std::string ident, uint32_t startTime, uint32_t endTime);
    void MergeTrafficStatsByAccount(std::vector<NetStatsInfo> &infos);
    void FilterTrafficStatsByUidNetwork(std::vector<NetStatsInfo> &allInfo, std::vector<NetStatsInfoSequence> &infos,
        const uint32_t uid, const std::string ident, uint32_t startTime, uint32_t endTime);
    int32_t CheckNetManagerAvailable();
#ifdef SUPPORT_NETWORK_SHARE
    bool IsSharingOn();
    void GetSharingStats(std::vector<NetStatsInfo> &sharingStats, uint32_t endtime);
#endif
#ifdef SUPPORT_TRAFFIC_STATISTIC
    void UpdateBpfMapTimer();
    void UpdateBpfMapTimerTask();
    bool CommonEventSimStateChangedFfrt(int32_t simId, int32_t simState);
    bool CellularDataStateChangedFfrt(int32_t slotId, int32_t dataState);
    bool CommonEventCellularDataStateChanged(int32_t slotId, int32_t dataState);
    void GetAllUsedCellularTraffic(const sptr<NetStatsNetwork> &network, uint64_t &allUsedTraffic);
    void GetDailyTrafficStatsByNetwork(const sptr<NetStatsNetwork> &network, std::vector<NetStatsInfoSequence> &infos);
    void GetHistoryTrafficInfo(const sptr<NetStatsNetwork> &network, std::vector<NetStatsInfo> &infos,
        bool isNeedCalibrate = false); // isNeedCalibrate - is calculate calibration traffic
    int32_t GetHistoryTrafficInUidTable(const NetStatsNetwork &network, std::vector<NetStatsInfo> &infos);
    int32_t GetHitstoryTrafficInIfaceTable(const NetStatsNetwork &network, std::vector<NetStatsInfo> &infos);
    void PrintSumNetStatsInfo(const std::vector<NetStatsInfo> &infos);
    void UpdateBpfMap(int32_t simId);
    void SetTrafficMapMaxValue();
    void SetTrafficMapMaxValue(int32_t slotId);
    void StartTrafficOvserver();
    bool GetNotifyStats(int32_t simId, uint8_t flag);
    bool GetMonAlertStatus(int32_t simId);
    bool GetMonNotifyStatus(int32_t simId);
    bool GetDayNotifyStatus(int32_t simId);
    void DealMonAlert(int32_t simId, bool isDaulCard);
    void DealMonNotification(int32_t simId, bool isDaulCard);
    void DealDayNotification(int32_t simId, bool isDaulCard);
    void DealNotificaiton(int32_t simId, uint8_t flag);
    bool IsMobileDataEnabled();
    void UpdateTrafficLimitDate(int32_t simId);
    bool CalculateTrafficAvailable(int32_t simId, uint64_t &monthlyAvailable,
                                   uint64_t &monthlyMarkAvailable, uint64_t &dailyMarkAvailable);
    int32_t UpdataSettingsdataFfrt(int32_t simId, uint8_t flag, uint64_t value);
    void ProcessSettingsDataUpdate(int32_t simId);
    void ClearTrafficMapBySlotId(int32_t slotId, uint64_t ifIndex);
    void AddSimIdInTwoMap(int32_t simId, uint64_t ifIndex);
    void PrintTrafficBpfMapInfo(int32_t slotId);
    void PrintTrafficSettingsMapInfo(int32_t simId);
    void UpdateCurActiviteSimChanged(int32_t simId, uint64_t ifIndex);
    void SubscribeTelephonyInfo();
    bool IsSimIdExist(int32_t simId);
    bool GetIfIndex(int32_t simId, uint64_t &ifIndex);
    void ProcessUpdateBeginDate(int32_t simId, uint32_t beginDate);
    std::string GetFieldNameByParam(TrafficPlanParam param);
#endif // SUPPORT_TRAFFIC_STATISTIC
    void StartSysTimer();
    void StopSysTimer();
    int32_t ModifySysTimer();
    void RegisterCommonTelephonyEvent();
    void RegisterCommonTimeEvent();
    void RegisterCommonNetStatusEvent();
    int32_t UpdateStatsDataInner();
    int32_t GetHistoryData(std::vector<NetStatsInfo> &infos, std::string ident,
                           uint32_t uid, uint32_t start, uint32_t end);
    void DeleteTrafficStatsByAccount(std::vector<NetStatsInfoSequence> &infos, uint32_t uid);
    void InitPrivateUserId();
    bool UpdateNetStatusMap(uint8_t type, uint8_t value);
    void UpdateNetStatusMapCellular(int32_t dataState);
    void InsertHistoryData(int32_t simId);
    void InitHistoryData();
    void EraseNetStatsInfoByUserId(std::vector<NetStatsInfoSequence> &infos, int32_t userId);
    void RecordCallingData(const std::string &callingFunction, uint32_t uid);
    void ReportCallingData();

private:
    enum ServiceRunningState {
        STATE_STOPPED = 0,
        STATE_RUNNING,
    };

    bool registerToService_;
    ServiceRunningState state_;
    std::shared_ptr<NetStatsCallback> netStatsCallback_ = nullptr;
    std::shared_ptr<NetStatsListener> subscriber_ = nullptr;
    std::shared_ptr<NetStatsCached> netStatsCached_ = nullptr;
    std::shared_ptr<NetStatsCalibrate> netStatsCalibrate_ = nullptr;
    uint64_t netStatsSysTimerId_ = 0;
    std::shared_ptr<NetStatsAccountSubscriber> accountSubscriber_ = nullptr;
    int32_t defaultUserId_ = 0;
    std::atomic_bool isUpdate_ = false;
    sptr<NetInfoObserver> netconnCallback_ = nullptr;
    ffrt::shared_mutex netconnCallbackMutex_;

#ifdef SUPPORT_TRAFFIC_STATISTIC
    uint64_t curIfIndex_ = UINT64_MAX;
    std::atomic_bool isWifiConnected_ = false;
    ffrt::shared_mutex simIdToIfIndexMapMutex_;
    std::map<int32_t, uint64_t> simIdToIfIndexMap_;
    std::unique_ptr<FfrtTimer> trafficTimer_ = nullptr;
    sptr<TrafficObserver> trafficObserver_ = nullptr;
    sptr<NetsysControllerCallback> netsysControllerObserver_ = nullptr;
    SafeMap<std::string, std::string> ifaceNameIdentMap_;
    std::shared_ptr<TrafficLimitDialog> dialog_ = nullptr;
    std::shared_ptr<ffrt::queue> trafficPlanFfrtQueue_ = nullptr;
    sptr<TelephonyInfoObserver> telephonyInfoObserver_ = nullptr;
    std::shared_ptr<INetMgrStatsLimitNotification> gNetmgrStatsLmtNtf_ = nullptr;
#endif // SUPPORT_TRAFFIC_STATISTIC
    std::mutex timerMutex_;
    std::shared_ptr<NetStatsTrafficPlanService> trafficPlanService_ = nullptr;
    std::set<std::string> callingRecordSet_;
    ffrt::mutex recordCallingDataMutex_;
    std::shared_ptr<ffrt::queue> recordReportFfrtQueue_;
    bool isPostDelayReport_ = false;
    static std::mutex instanceLock_;
    static std::shared_ptr<NetStatsService> instance_;
};

#ifdef SUPPORT_TRAFFIC_STATISTIC
class TrafficObserver : public NetsysNative::NetsysTrafficCallbackStub {
public:
    TrafficObserver();
    ~TrafficObserver() override;
    int32_t OnExceedTrafficLimits(int8_t &flag) override;
};

class TelephonyInfoObserver : public Telephony::TelephonyObserver {
public:
    TelephonyInfoObserver() = default;
    ~TelephonyInfoObserver() = default;
    void OnSimStateUpdated(
         int32_t slotId, Telephony::CardType type, Telephony::SimState state, Telephony::LockReason reason) override;
    void OnIccAccountUpdated() override;
};

#endif // SUPPORT_TRAFFIC_STATISTIC
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_SERVICE_H

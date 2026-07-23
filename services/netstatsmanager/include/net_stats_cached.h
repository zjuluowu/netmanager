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

#ifndef NET_STATS_CACHED_H
#define NET_STATS_CACHED_H

#include <algorithm>
#include <map>
#include <mutex>
#include <vector>
#include <shared_mutex>

#include "ffrt.h"
#include "net_bundle.h"
#include "net_push_stats_info.h"
#include "net_stats_callback.h"
#include "net_stats_constants.h"
#include "net_stats_info.h"
#include "netmanager_base_common_utils.h"
#include "safe_map.h"
#ifdef SUPPORT_NETWORK_SHARE
#include "network_sharing.h"
#endif // SUPPORT_NETWORK_SHARE

#include "ffrt_timer.h"

namespace OHOS {
namespace NetManagerStandard {
typedef  struct {
    int32_t beginDate;
    uint64_t startTime;
    uint64_t endTime;
    uint64_t trafficData;
} HistoryData;

class NetStatsCached {
public:
    NetStatsCached();
    ~NetStatsCached() = default;
    void ForceUpdateStats();
    void ForceUpdateStatsAndBackupDB(const std::string &sourceDb, const std::string &backupDb);

    ffrt::task_handle ForceArchiveStats(uint32_t uid);

    void ForceCachedStats();

    int32_t StartCached();

    void CacheStatsSim();
    
    int32_t CreatNetStatsTables(const std::string &tableName);

    void SetCycleThreshold(uint32_t threshold);

    void GetUidStatsCached(std::vector<NetStatsInfo> &uidStatsInfo);

    void GetUidSimStatsCached(std::vector<NetStatsInfo> &uidSimStatsInfo);

    void GetUidPushStatsCached(std::vector<NetStatsInfo> &uidPushStatsInfo);

    void GetAllPushStatsCached(std::vector<NetStatsInfo> &uidPushStatsInfo);

    void GetIfaceStatsCached(std::vector<NetStatsInfo> &ifaceStatsInfo);

    void SetAppStats(const PushStatsInfo &info);

    void SetDpaAppStats(const NetStatsInfo &info);

    void GetKernelStats(std::vector<NetStatsInfo> &statsInfo);

    void UpdateDefaultSimId(const std::string &simId);
 
    std::string GetDefaultSimId();
 
    void CacheUidSimStats();

    void CacheIfaceStats();

    void UpdateIdent(const std::string &ifName, NetStatsInfo &statsInfo);

    void GetKernelRmnetIfaceStats(std::vector<NetStatsInfo> &statsInfo);

    NetStatsInfo GetIncreasedIfaceStats(const NetStatsInfo &info);

#ifdef SUPPORT_NETWORK_SHARE
    void GetIptablesStatsCached(std::vector<NetStatsInfo> &iptablesStatsInfo);

    void GetIptablesStatsIncrease(std::vector<NetStatsInfo> &InfosVec);
#endif
    void SaveSharingTraffic(const NetStatsInfo &infos);

    uint64_t GetMonthTrafficData(int32_t simId);
    void UpdateHistoryData(int32_t simId, int32_t beginDate);
    void ForceUpdateHistoryData(int32_t simId, int32_t beginDate, uint64_t historyData);
    void DeleteHistoryData(int32_t simId);

    inline void SetTrafficThreshold(uint64_t threshold)
    {
        trafficThreshold_ = threshold;
    }

    inline void SetDateThreshold(uint64_t threshold)
    {
        cycleThreshold_ = threshold;
    }

    inline void SetCallbackManager(const std::shared_ptr<NetStatsCallback> &callbackManager)
    {
        stats_.SetNotifier(callbackManager);
    }

    int32_t GetCurPrivateUserId();

    void SetCurPrivateUserId(int32_t userId);

    int32_t GetCurDefaultUserId();

    void SetCurDefaultUserId(int32_t userId);

    void Reset();

    void SetUidSimSampleBundle(uint32_t uid, const SampleBundleInfo &info);

    void DeleteUidSimSampleBundle(uint32_t uid);

    std::optional<SampleBundleInfo> GetUidSimSampleBundle(uint32_t uid);

    uint32_t GetUidSimSampleBundlesSize();

    void SetUidStatsFlag(std::unordered_map<uint32_t, SampleBundleInfo> &sampleBundleMap);

    void DeleteUidStatsFlag(uint32_t uid);

    void DeleteUidSimStatsWithFlag(uint32_t uid, uint32_t flag);

    void ClearUidStatsFlag();

    void SetPrivateStatus(bool status);

#ifdef SUPPORT_NETWORK_SHARE
    void DeleteIptablesStats();

    uint64_t GetWriteDateTime();
#endif

private:
    class CachedInfo {
    public:
        void PushUidStats(NetStatsInfo &info)
        {
            if (info.HasNoData()) {
                return;
            }
            uidStatsInfo_.push_back(info);
            currentUidStats_ += info.GetStats();
            if (netStatsCallbackManager_ != nullptr) {
                netStatsCallbackManager_->NotifyNetUidStatsChanged(info.iface_, info.uid_);
            }
        }

        void PushUidSimStats(NetStatsInfo &info)
        {
            if (info.HasNoData()) {
                return;
            }
            uidSimStatsInfo_.push_back(info);
            currentUidSimStats_ += info.GetStats();
            if (netStatsCallbackManager_ != nullptr) {
                netStatsCallbackManager_->NotifyNetUidStatsChanged(info.iface_, info.uid_);
            }
        }

        void PushIfaceStats(NetStatsInfo &info)
        {
            if (info.HasNoData()) {
                return;
            }
            ifaceStatsInfo_.push_back(info);
            currentIfaceStats_ += info.GetStats();
            if (netStatsCallbackManager_ != nullptr) {
                netStatsCallbackManager_->NotifyNetIfaceStatsChanged(info.iface_);
            }
        }

#ifdef SUPPORT_NETWORK_SHARE
        void PushIptablesStats(NetStatsInfo &info)
        {
            if (info.HasNoData()) {
                return;
            }
            iptablesStatsInfo_.push_back(info);
            currentIptablesStats_ += info.GetStats();
            if (netStatsCallbackManager_ != nullptr) {
                netStatsCallbackManager_->NotifyNetUidStatsChanged(info.iface_, info.uid_);
            }
        }
#endif

        inline std::vector<NetStatsInfo> &GetUidStatsInfo()
        {
            return uidStatsInfo_;
        }

        inline std::vector<NetStatsInfo> &GetUidSimStatsInfo()
        {
            return uidSimStatsInfo_;
        }

        inline std::vector<NetStatsInfo> &GetIfaceStatsInfo()
        {
            return ifaceStatsInfo_;
        }

#ifdef SUPPORT_NETWORK_SHARE
        inline std::vector<NetStatsInfo> &GetIptablesStatsInfo()
        {
            return iptablesStatsInfo_;
        }
#endif

        inline uint64_t GetCurrentUidStats() const
        {
            return currentUidStats_;
        }

        inline uint64_t GetCurrentUidSimStats() const
        {
            return currentUidSimStats_;
        }

        inline uint64_t GetCurrentIfaceStats() const
        {
            return currentIfaceStats_;
        }

#ifdef SUPPORT_NETWORK_SHARE
        inline uint64_t GetCurrentIptablesStats() const
        {
            return currentIptablesStats_;
        }
#endif

        void ResetUidStats()
        {
            uidStatsInfo_.clear();
            currentUidStats_ = 0;
        }

        void ResetUidStats(uint32_t uid)
        {
            for (const auto &item : uidStatsInfo_) {
                if (item.uid_ == uid) {
                    currentUidStats_ -= item.GetStats();
                }
            }
            uidStatsInfo_.erase(std::remove_if(uidStatsInfo_.begin(), uidStatsInfo_.end(),
                                               [uid](const auto &item) { return item.uid_ == uid; }),
                                uidStatsInfo_.end());
        }

        void ResetUidSimStats()
        {
            uidSimStatsInfo_.clear();
            currentUidSimStats_ = 0;
        }

        void ResetUidSimStats(uint32_t uid)
        {
            for (const auto &item : uidSimStatsInfo_) {
                if (item.uid_ == uid) {
                    currentUidSimStats_ -= item.GetStats();
                }
            }
            uidSimStatsInfo_.erase(std::remove_if(uidSimStatsInfo_.begin(), uidSimStatsInfo_.end(),
                                                  [uid](const auto &item) { return item.uid_ == uid; }),
                                   uidSimStatsInfo_.end());
        }

        void ResetIfaceStats()
        {
            ifaceStatsInfo_.clear();
            currentIfaceStats_ = 0;
        }

#ifdef SUPPORT_NETWORK_SHARE
        void ResetIptablesStats()
        {
            iptablesStatsInfo_.clear();
            currentIptablesStats_ = 0;
        }
#endif

        inline void SetNotifier(const std::shared_ptr<NetStatsCallback> &callbackManager)
        {
            netStatsCallbackManager_ = callbackManager;
        }

    private:
        uint64_t currentUidStats_ = 0;
        uint64_t currentUidSimStats_ = 0;
        uint64_t currentIfaceStats_ = 0;
        std::vector<NetStatsInfo> uidStatsInfo_;
        std::vector<NetStatsInfo> uidSimStatsInfo_;
        std::vector<NetStatsInfo> ifaceStatsInfo_;
        std::shared_ptr<NetStatsCallback> netStatsCallbackManager_ = nullptr;
#ifdef SUPPORT_NETWORK_SHARE
        uint64_t currentIptablesStats_ = 0;
        std::vector<NetStatsInfo> iptablesStatsInfo_;
#endif
    };

    static constexpr uint32_t DEFAULT_CACHE_CYCLE_MS = 30 * 60 * 1000;
    static constexpr uint64_t DEFAULT_TRAFFIC_STATISTICS_THRESHOLD_BYTES = 2 * 1024 * 1024;
    static constexpr uint64_t DEFAULT_DATA_CYCLE_S = 90 * 24 * 60 * 60;
    static constexpr uint64_t CACHE_DATE_TIME_S = 1 * 24 * 60 * 60;
    static constexpr uint64_t STATS_PACKET_CYCLE_MS = 1 * 60 * 60 * 1000;

    CachedInfo stats_;
    ffrt::mutex lock_;
    std::mutex mutex_ {};
    std::atomic<bool> isForce_ = false;
    std::atomic<bool> isExec_ = false;
    std::atomic<bool> isExecBackUp_ = false;
    std::unique_ptr<FfrtTimer> cacheTimer_ = nullptr;
    std::unique_ptr<FfrtTimer> writeTimer_ = nullptr;
    uint32_t cycleThreshold_ = DEFAULT_CACHE_CYCLE_MS;
    uint64_t trafficThreshold_ = DEFAULT_TRAFFIC_STATISTICS_THRESHOLD_BYTES;
    uint64_t dateCycle_ = DEFAULT_DATA_CYCLE_S;
    std::vector<NetStatsInfo> uidPushStatsInfo_;
    std::vector<NetStatsInfo> allPushStatsInfo_;
    std::vector<NetStatsInfo> lastUidStatsInfo_;
    std::vector<NetStatsInfo> lastUidSimStatsInfo_;
    std::vector<NetStatsInfo> lastIfaceStatsMap_;
    std::atomic<int64_t> uninstalledUid_ = -1;
    SafeMap<std::string, std::string> ifaceNameIdentMap_;
    SafeMap<uint32_t, NetStatsDataFlag> uidStatsFlagMap_;
    SafeMap<uint32_t, SampleBundleInfo> uidSimSampleBundleMap_;
    bool isDisplayTrafficAncoList = false;
    int32_t curPrivateUserId_ = -1;
    int32_t curDefaultUserId_ = -1;
    std::map<uint32_t, HistoryData> cellularHistoryData_;
    std::shared_mutex cellularHistoryDataMutex_;
    bool isPrivateSpaceExist_ = false;
    std::mutex simIdMutex_;
    std::mutex calibrationLock_;
    std::string curDefaultSimId_;
#ifdef SUPPORT_NETWORK_SHARE
    std::vector<NetStatsInfo> lastIptablesStatsInfo_;
    uint64_t writeDate_ = 0;
#endif

    void LoadIfaceNameIdentMaps();
    NetStatsDataFlag GetUidStatsFlag(uint32_t uid);
    void IsExistInUidSimSampleBundleMap(bool &isExistSim, bool &isExistSim2);
    std::optional<SampleBundleInfo> GetEarlySampleBundleInfo();

    void CacheStats();
    void CacheUidStats();
    void CacheAppStats();
    void GetKernelUidStats(std::vector<NetStatsInfo> &statsInfo);
    void GetKernelUidSimStats(std::vector<NetStatsInfo> &statsInfo);
    void DeleteUidStats(uint32_t uid);
    void DeleteUidSimStats(uint32_t uid);

    void WriteStats();
    void WriteUidStats();
    void WriteUidSimStats();
    void WriteIfaceStats();
#ifdef SUPPORT_NETWORK_SHARE
    void CacheIptablesStats();
    void CacheIptablesStatsService(nmd::NetworkSharingTraffic &traffic, std::string &ifaceName);
    void WriteIptablesStats();
#endif
    int32_t GetTotalHistoryStatsByIdent(int32_t simId, uint64_t start,
        uint64_t end, uint64_t &historyData);
    void UpdateHistoryData(const std::map<std::string, uint64_t> data);
    void JudgeAndUpdateHistoryData(uint64_t curSecond);

    void GetUpIfaceName(std::string &downIface, std::string &upIface);

    NetStatsInfo GetIncreasedStats(const NetStatsInfo &info);

    NetStatsInfo GetIncreasedSimStats(const NetStatsInfo &info);

    void UpdateNetStatsFlag(NetStatsInfo &info);

    void UpdateNetStatsUserIdSim(NetStatsInfo &info);

    inline bool CheckUidStor()
    {
        return stats_.GetCurrentUidStats() >= trafficThreshold_;
    }

    inline bool CheckUidSimStor()
    {
        return stats_.GetCurrentUidSimStats() >= trafficThreshold_;
    }

    inline bool CheckIfaceStor()
    {
        return stats_.GetCurrentIfaceStats() >= trafficThreshold_;
    }

#ifdef SUPPORT_NETWORK_SHARE
    inline bool CheckIptablesStor()
    {
        return stats_.GetCurrentIptablesStats() >= trafficThreshold_;
    }
#endif
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_STATS_CACHED_H

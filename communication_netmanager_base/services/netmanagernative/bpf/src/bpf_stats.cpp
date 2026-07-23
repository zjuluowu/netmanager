/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <net/if.h>
#include <vector>
#include <set>
#include <cinttypes>

#include "bpf_path.h"
#include "bpf_def.h"
#include "bpf_stats.h"
#include "securec.h"
#include "netnative_log_wrapper.h"
#include "net_stats_constants.h"

namespace OHOS::NetManagerStandard {
namespace {
constexpr const char *CELLULAR_IFACE = "rmnet0";
constexpr const char *CELLULAR_IFACE_1 = "rmnet1";
constexpr const char *CELLULAR_IFACE_2 = "rmnet2";
constexpr const char *CELLULAR_IFACE_3 = "rmnet3";
constexpr const char *VIRNIC_IFACE = "virnic";
constexpr const char *WIFI_IFACE = "wlan0";
constexpr const char *WIFI_IFACE_1 = "wlan1";
constexpr const char *ETH_IFACE_0 = "eth0";
constexpr const char *ETH_IFACE_1 = "eth1";
std::set<std::string> IFACE_NAME_SET { CELLULAR_IFACE, CELLULAR_IFACE_1, CELLULAR_IFACE_2, CELLULAR_IFACE_3,
    VIRNIC_IFACE, WIFI_IFACE, WIFI_IFACE_1, ETH_IFACE_0, ETH_IFACE_1 };
}
int32_t NetsysBpfStats::GetNumberFromStatsValue(uint64_t &stats, StatsType statsType, const stats_value &value)
{
    switch (statsType) {
        case StatsType::STATS_TYPE_RX_BYTES:
            stats = value.rxBytes;
            break;
        case StatsType::STATS_TYPE_RX_PACKETS:
            stats = value.rxPackets;
            break;
        case StatsType::STATS_TYPE_TX_BYTES:
            stats = value.txBytes;
            break;
        case StatsType::STATS_TYPE_TX_PACKETS:
            stats = value.txPackets;
            break;
        default:
            NETNATIVE_LOGE("invalid StatsType type %{public}d", statsType);
            return STATS_ERR_READ_BPF_FAIL;
    }
    return NETSYS_SUCCESS;
}

int32_t NetsysBpfStats::GetTotalStats(uint64_t &stats, StatsType statsType)
{
    stats = 0;
    std::lock_guard<std::mutex> lock(ifaceStatsMapMutext_);
    BpfMapper<iface_stats_key, iface_stats_value> ifaceStatsMap(IFACE_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!ifaceStatsMap.IsValid()) {
        NETNATIVE_LOGE("ifaceStatsMap IsValid");
        return STATS_ERR_INVALID_GET_BPF_MAP;
    }

    iface_stats_value totalStats = {0};
    auto keys = ifaceStatsMap.GetAllKeys();
    std::set<uint64_t> ifIndexSet;
    std::set<uint64_t> needFilterIfIndex;
    for (auto key : keys) {
        ifIndexSet.insert(key);
    }

    for (auto value : ifIndexSet) {
        char if_name[IFNAME_SIZE] = {0};
        if (memset_s(if_name, sizeof(if_name), 0, sizeof(if_name)) != EOK) {
            return STATS_ERR_READ_BPF_FAIL;
        }

        char *pName = if_indextoname(value, if_name);
        if (pName != nullptr && IFACE_NAME_SET.find(pName) == IFACE_NAME_SET.end()) {
            needFilterIfIndex.insert(value);
        }
    }
    for (const auto &k : keys) {
        if (needFilterIfIndex.find(k) != needFilterIfIndex.end()) {
            continue;
        }
        iface_stats_value v = {0};
        // LCOV_EXCL_START
        if (ifaceStatsMap.ReadAndLog(k, v) < NETSYS_SUCCESS) {
            NETNATIVE_LOGE("Read ifaceStatsMap err");
            return STATS_ERR_READ_BPF_FAIL;
        }
        // LCOV_EXCL_STOP
        totalStats.rxPackets += v.rxPackets;
        totalStats.rxBytes += v.rxBytes;
        totalStats.txPackets += v.txPackets;
        totalStats.txBytes += v.txBytes;
    }

    return GetNumberFromStatsValue(stats, statsType, totalStats);
}

int32_t NetsysBpfStats::GetUidStats(uint64_t &stats, StatsType statsType, uint32_t uid)
{
    stats = 0;
    BpfMapper<app_uid_stats_key, app_uid_stats_value> appUidStatsMap(APP_UID_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!appUidStatsMap.IsValid()) {
        return STATS_ERR_INVALID_GET_BPF_MAP;
    }

    app_uid_stats_value uidStats = {0};
    if (appUidStatsMap.Read(uid, uidStats) < 0) {
        return STATS_ERR_READ_BPF_FAIL;
    }
    return GetNumberFromStatsValue(stats, statsType, uidStats);
}

int32_t NetsysBpfStats::GetAllSimStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats)
{
    BpfMapper<stats_key, stats_value> uidSimStatsMap(APP_UID_SIM_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!uidSimStatsMap.IsValid()) {
        return STATS_ERR_INVALID_GET_BPF_MAP;
    }

    stats.clear();
    char if_name[IFNAME_SIZE] = {0};
    auto keys = uidSimStatsMap.GetAllKeys();
    for (const auto &k : keys) {
        stats_value v = {};
        if (uidSimStatsMap.Read(k, v) < 0) {
            NETNATIVE_LOGE("Read uid_sim_map err");
            return STATS_ERR_READ_BPF_FAIL;
        }

        NetStatsInfo tempStats;
        tempStats.uid_ = k.uId;
        if (memset_s(if_name, sizeof(if_name), 0, sizeof(if_name)) != EOK) {
            return STATS_ERR_READ_BPF_FAIL;
        }

        char *pName = if_indextoname(k.ifIndex, if_name);
        if (pName != nullptr) {
            tempStats.iface_ = pName;
        }
        if (k.ifType == IFACE_TYPE_WIFI) {
            tempStats.iface_ = WIFI_IFACE;
        } else if (k.ifType == IFACE_TYPE_CELLULAR) {
            tempStats.iface_ = CELLULAR_IFACE;
        }
        tempStats.rxBytes_ = v.rxBytes;
        tempStats.txBytes_ = v.txBytes;
        tempStats.rxPackets_ = v.rxPackets;
        tempStats.txPackets_ = v.txPackets;
        auto findRet = std::find_if(stats.begin(), stats.end(),
                                    [&tempStats](const NetStatsInfo &info) { return info.Equals(tempStats); });
        if (findRet == stats.end()) {
            stats.push_back(std::move(tempStats));
        } else {
            *findRet += tempStats;
        }
    }

    return NETSYS_SUCCESS;
}

int32_t NetsysBpfStats::GetAllStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats)
{
    BpfMapper<stats_key, stats_value> uidIfaceStatsMap(APP_UID_IF_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!uidIfaceStatsMap.IsValid()) {
        return STATS_ERR_INVALID_GET_BPF_MAP;
    }

    stats.clear();
    char if_name[IFNAME_SIZE] = {0};
    auto keys = uidIfaceStatsMap.GetAllKeys();
    for (const auto &k : keys) {
        stats_value v = {};
        if (uidIfaceStatsMap.Read(k, v) < 0) {
            NETNATIVE_LOGE("Read ifaceStatsMap err");
            return STATS_ERR_READ_BPF_FAIL;
        }

        NetStatsInfo tempStats;
        tempStats.uid_ = k.uId;
        if (memset_s(if_name, sizeof(if_name), 0, sizeof(if_name)) != EOK) {
            return STATS_ERR_READ_BPF_FAIL;
        }

        char *pName = if_indextoname(k.ifIndex, if_name);
        if (pName != nullptr) {
            tempStats.iface_ = pName;
        }
#ifndef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
        if (IFACE_NAME_SET.find(tempStats.iface_) == IFACE_NAME_SET.end()) {
            continue;
        }
#endif
        tempStats.rxBytes_ = v.rxBytes;
        tempStats.txBytes_ = v.txBytes;
        tempStats.rxPackets_ = v.rxPackets;
        tempStats.txPackets_ = v.txPackets;
        stats.emplace_back(std::move(tempStats));
    }

    return NETSYS_SUCCESS;
}

int32_t NetsysBpfStats::DeleteStatsInfo(const std::string &path, uint32_t uid)
{
    if (path != APP_UID_IF_STATS_MAP_PATH && path != APP_UID_SIM_STATS_MAP_PATH) {
        NETNATIVE_LOGI("DeleteStatsInfo invalid path");
        return NETSYS_SUCCESS;
    }
    BpfMapper<stats_key, stats_value> uidStatsMap(path, BPF_ANY);
    if (!uidStatsMap.IsValid()) {
        return STATS_ERR_INVALID_GET_BPF_MAP;
    }
    auto keys = uidStatsMap.GetAllKeys();
    for (const auto &k : keys) {
        if (k.uId == uid) {
            if (uidStatsMap.Delete(k) < 0) {
                NETNATIVE_LOGE("Delete uidStatsMap err");
                return STATS_ERR_WRITE_BPF_FAIL;
            }
        }
    }
    return NETSYS_SUCCESS;
}

int32_t NetsysBpfStats::GetIfaceStats(uint64_t &stats, const StatsType statsType, const std::string &interfaceName)
{
    stats = 0;
    std::lock_guard<std::mutex> lock(ifaceStatsMapMutext_);
    BpfMapper<iface_stats_key, iface_stats_value> ifaceStatsMap(IFACE_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!ifaceStatsMap.IsValid()) {
        return STATS_ERR_INVALID_GET_BPF_MAP;
    }

    auto ifIndex = if_nametoindex(interfaceName.c_str());
    if (ifIndex <= 0) {
        return STATS_ERR_GET_IFACE_NAME_FAILED;
    }

    iface_stats_value ifaceStats = {0};
    if (ifaceStatsMap.Read(ifIndex, ifaceStats) < 0) {
        return STATS_ERR_READ_BPF_FAIL;
    }
    return GetNumberFromStatsValue(stats, statsType, ifaceStats);
}

int32_t NetsysBpfStats::GetCookieStats(uint64_t &stats, StatsType statsType, uint64_t cookie)
{
    NETNATIVE_LOGI("GetCookieStats start");
    stats = 0;
    BpfMapper<socket_cookie_stats_key, app_cookie_stats_value> appUidCookieStatsMap(APP_COOKIE_STATS_MAP_PATH,
                                                                                    BPF_F_RDONLY);
    if (!appUidCookieStatsMap.IsValid()) {
        NETNATIVE_LOGE("GetCookieStats appUidCookieStatsMap is valid");
        return NETMANAGER_ERR_INTERNAL;
    }

    app_cookie_stats_value cookieStats = {0};
    if (appUidCookieStatsMap.Read(cookie, cookieStats) < 0) {
        NETNATIVE_LOGE("GetCookieStats appUidCookieStatsMap read error");
        return NETMANAGER_ERR_INTERNAL;
    }

    int32_t res = GetNumberFromStatsValue(stats, statsType, cookieStats);
    if (res == STATS_ERR_READ_BPF_FAIL) {
        NETNATIVE_LOGE("GetCookieStats GetNumberFromStatsValue error");
        return NETMANAGER_ERR_INTERNAL;
    }
    return NETSYS_SUCCESS;
}

// write taffic available value map.  update by timer/settings modify/network changed
int32_t NetsysBpfStats::SetNetStateTrafficMap(uint8_t flag, uint64_t availableTraffic)
{
    NETNATIVE_LOGI("NetsysBpfStats::SetNetStateTrafficMap start. flag:%{public}u, availableTraffic:%{public}" PRIu64,
        flag, availableTraffic);

    BpfMapper<traffic_notify_flag, traffic_value> netStatsTrafficMap(LIMITS_STATS_MAP_PATH, BPF_F_WRONLY);
    if (!netStatsTrafficMap.IsValid()) {
        NETNATIVE_LOGE("SetNetStateTrafficMap netStatsTrafficMap not exist.");
        return NETMANAGER_ERROR;
    }

    if (netStatsTrafficMap.Write(flag, availableTraffic, 0) != 0) {
        NETNATIVE_LOGE("SetNetStateTrafficMap Write netStatsTrafficMap err");
        return NETMANAGER_ERROR;
    }

    NETNATIVE_LOGI("NetsysBpfStats::SetNetStateTrafficMap flag:%{public}u, availableTraffic:%{public}" PRIu64,
        flag, availableTraffic);
    return NETMANAGER_SUCCESS;
}

// LCOV_EXCL_START
int32_t NetsysBpfStats::GetNetStateTrafficMap(uint8_t flag, uint64_t &availableTraffic)
{
    BpfMapper<traffic_notify_flag, traffic_value> netStatsTrafficMap(LIMITS_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!netStatsTrafficMap.IsValid()) {
        NETNATIVE_LOGE("GetNetStateTrafficMap netStatsTrafficMap not exist. errno: %{public}d", errno);
        return NETMANAGER_ERROR;
    }
    traffic_value value = 0;
    if (netStatsTrafficMap.Read(flag, value) != 0) {
        NETNATIVE_LOGE("GetNetStateTrafficMap read netStatsTrafficMap err");
        return NETMANAGER_ERROR;
    }
    availableTraffic = value;
    NETNATIVE_LOGI("NetsysBpfStats::GetNetStateTrafficMap flag:%{public}u, availableTraffic:%{public}" PRIu64,
        flag, availableTraffic);
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::GetNetStateIncreTrafficMap(std::vector<uint64_t> &keys)
{
    BpfMapper<uint64_t, traffic_value> netStatsIncreTrafficMap(INCREMENT_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!netStatsIncreTrafficMap.IsValid()) {
        NETNATIVE_LOGE("GetNetStateIncreTrafficMap netStatsTrafficMap not exist. errno: %{public}d", errno);
        return NETMANAGER_ERROR;
    }
    keys = netStatsIncreTrafficMap.GetAllKeys();
    NETNATIVE_LOGI("NetsysBpfStats::GetNetStateIncreTrafficMap keys.size: %{public}zu", keys.size());
    for (auto key : keys) {
        traffic_value value = { 0 };
        if (netStatsIncreTrafficMap.Read(key, value) != 0) {
            NETNATIVE_LOGE("GetNetStateIncreTrafficMap read netStatsTrafficMap err");
            return NETMANAGER_ERROR;
        }
        char ifName[IFNAME_SIZE] = { 0 };
        auto pName = if_indextoname(key, ifName);
        NETNATIVE_LOGI("NetsysBpfStats::GetNetStateIncreTrafficMap keys: %{public}" PRIu64 ", \
value: %{public}" PRIu64 ", name: %{public}s",
            key, static_cast<uint64_t>(value), ifName);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::ClearIncreaseTrafficMap()
{
    NETNATIVE_LOGI("NetsysBpfStats::ClearIncreaseTrafficMap start");
    std::vector<uint64_t> keys;
    if (GetNetStateIncreTrafficMap(keys) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }
    BpfMapper<uint64_t, traffic_value> increaseTrafficMap(INCREMENT_STATS_MAP_PATH, BPF_F_WRONLY);
    if (!increaseTrafficMap.IsValid()) {
        NETNATIVE_LOGE("ClearIncreaseTrafficMap increamentTrafficMap not exist.");
        return NETMANAGER_ERROR;
    }

    if (increaseTrafficMap.Clear(keys) != 0) {
        NETNATIVE_LOGE("ClearIncreaseTrafficMap Write increamentTrafficMap err");
        return NETMANAGER_ERROR;
    }
    keys = {};
    if (GetNetStateIncreTrafficMap(keys) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    NETNATIVE_LOGI("NetsysBpfStats::ClearIncreaseTrafficMap end");
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::DeleteIncreaseTrafficMap(uint64_t ifIndex)
{
    NETNATIVE_LOGI("NetsysBpfStats::DeleteIncreaseTrafficMap start, ifIndex: %{public}" PRIu64, ifIndex);
    std::vector<uint64_t> keys;
    if (GetNetStateIncreTrafficMap(keys) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }
    BpfMapper<uint64_t, traffic_value> increaseTrafficMap(INCREMENT_STATS_MAP_PATH, BPF_F_WRONLY);
    if (!increaseTrafficMap.IsValid()) {
        NETNATIVE_LOGE("DeleteIncreaseTrafficMap increamentTrafficMap not exist.");
        return NETMANAGER_ERROR;
    }

    if (increaseTrafficMap.Delete(static_cast<uint64_t>(ifIndex)) != 0) {
        NETNATIVE_LOGE("DeleteIncreaseTrafficMap Delete increamentTrafficMap err");
        return NETMANAGER_ERROR;
    }

    keys = {};
    if (GetNetStateIncreTrafficMap(keys) != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::ClearSimStatsBpfMap()
{
    NETNATIVE_LOGI("NetsysBpfStats::ClearSimStatsBpfMap start");
    BpfMapper<uint64_t, traffic_value> simStatsMap(APP_UID_SIM_STATS_MAP_PATH, BPF_F_WRONLY);
    if (!simStatsMap.IsValid()) {
        NETNATIVE_LOGE("ClearSimStatsBpfMap simStatsMap not exist.");
        return NETMANAGER_ERROR;
    }
    std::vector<uint64_t> keys = simStatsMap.GetAllKeys();
    if (simStatsMap.Clear(keys) != 0) {
        NETNATIVE_LOGE("ClearSimStatsBpfMap Write simStatsMap err");
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::UpdateIfIndexMap(int8_t key, uint64_t index)
{
    NETNATIVE_LOGI("UpdateIfIndexMap start. key:%{public}d, index:%{public}" PRIu64, key, index);

    BpfMapper<uint8_t, uint64_t> netStatsIfIndexMap(IFINDEX_MAP_PATH, BPF_F_WRONLY);
    if (!netStatsIfIndexMap.IsValid()) {
        NETNATIVE_LOGE("UpdateIfIndexMap netStatsTrafficMap not exist.");
        return NETMANAGER_ERROR;
    }

    if (netStatsIfIndexMap.Write(key, index, 0) != 0) {
        NETNATIVE_LOGE("UpdateIfIndexMap Write netStatsTrafficMap err");
        return NETMANAGER_ERROR;
    }
    GetIfIndexMap();
    return 0;
}

int32_t NetsysBpfStats::GetIfIndexMap()
{
    NETNATIVE_LOGI("GetIfIndexMap start");
    BpfMapper<uint8_t, uint64_t> netStatsIfIndexMap(IFINDEX_MAP_PATH, BPF_F_RDONLY);
    if (!netStatsIfIndexMap.IsValid()) {
        NETNATIVE_LOGE("GetIfIndexMap netStatsTrafficMap not exist.");
        return NETMANAGER_ERROR;
    }

    std::vector<uint8_t> keys = netStatsIfIndexMap.GetAllKeys();
    NETNATIVE_LOGI("GetIfIndexMap keys.size: %{public}zu", keys.size());
    for (auto key : keys) {
        uint64_t value = 0;
        if (netStatsIfIndexMap.Read(key, value) != 0) {
            NETNATIVE_LOGE("GetIfIndexMap read err");
            return NETMANAGER_ERROR;
        }
        NETNATIVE_LOGI("GetIfIndexMap keys: %{public}u, value: %{public}" PRIu64, key, value);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::SetNetStatusMap(uint8_t type, uint8_t value)
{
    NETNATIVE_LOGI("SetNetStatusMap: %{public}u, %{public}u", type, value);
    if (type != 0 && type != 1) {
        return NETMANAGER_ERROR;
    }
    {
        std::lock_guard<std::mutex> lock(netStatusMapMutex_);
        BpfMapper<uint8_t, uint8_t> netStatusMap(NET_STATUS_MAP_PATH, BPF_F_WRONLY);
        if (!netStatusMap.IsValid()) {
            NETNATIVE_LOGE("netStatusMap not exist.");
            return NETMANAGER_ERROR;
        }
        if (netStatusMap.Write(type, value, 0) != 0) {
            NETNATIVE_LOGE("UpdatenetStatusMap Write netStatusMap err.");
            return NETMANAGER_ERROR;
        }
    }

    std::string str = WIFI_IFACE_1;
    uint64_t ifIndex = if_nametoindex(str.c_str());
    NETNATIVE_LOGI("UpdatenetStatusMap ifIndex:%{public}" PRIu64, ifIndex);
    if (ifIndex <= 0) {
        ifIndex = 0;
    }
    SetNetWlan1Map(ifIndex);
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::SetNetWlan1Map(uint64_t ifIndex)
{
    std::lock_guard<std::mutex> lock(netWlan1MapMutex_);
    BpfMapper<uint8_t, uint64_t> netWlan1Map(NET_WLAN1_MAP_PATH, BPF_F_WRONLY);
    if (!netWlan1Map.IsValid()) {
        NETNATIVE_LOGE("SetNetWlan1Map not exist.");
        return NETMANAGER_ERROR;
    }
    if (netWlan1Map.Write(0, ifIndex, 0) != 0) {
        NETNATIVE_LOGE("SetNetWlan1Map Write err");
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}
// LCOV_EXCL_STOP
} // namespace OHOS::NetManagerStandard

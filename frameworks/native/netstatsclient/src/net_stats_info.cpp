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

#include "net_stats_info.h"
#include "net_mgr_log_wrapper.h"
#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t STATS_INFO_MAX_SIZE = 5000;

bool NetStatsInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(uid_)) {
        return false;
    }
    if (!parcel.WriteString(iface_)) {
        return false;
    }
    if (!parcel.WriteString(ident_)) {
        return false;
    }
    if (!parcel.WriteUint64(date_)) {
        return false;
    }
    if (!parcel.WriteUint64(rxBytes_)) {
        return false;
    }
    if (!parcel.WriteUint64(txBytes_)) {
        return false;
    }
    if (!parcel.WriteUint64(rxPackets_)) {
        return false;
    }
    if (!parcel.WriteUint64(txPackets_)) {
        return false;
    }
    return true;
}

bool NetStatsInfo::Marshalling(Parcel &parcel, const NetStatsInfo &stats)
{
    if (!parcel.WriteUint32(stats.uid_)) {
        return false;
    }
    if (!parcel.WriteString(stats.iface_)) {
        return false;
    }
    if (!parcel.WriteString(stats.ident_)) {
        return false;
    }
    if (!parcel.WriteUint64(stats.date_)) {
        return false;
    }
    if (!parcel.WriteUint64(stats.rxBytes_)) {
        return false;
    }
    if (!parcel.WriteUint64(stats.txBytes_)) {
        return false;
    }
    if (!parcel.WriteUint64(stats.rxPackets_)) {
        return false;
    }
    if (!parcel.WriteUint64(stats.txPackets_)) {
        return false;
    }
    return true;
}

bool NetStatsInfo::Marshalling(Parcel &parcel, const std::vector<NetStatsInfo> &statsInfos)
{
    uint32_t vSize = statsInfos.size();
    if (vSize > STATS_INFO_MAX_SIZE) {
        NETMGR_LOG_E("Size of the statsInfos exceeds maximum.");
        return false;
    }
    if (!parcel.WriteUint32(vSize)) {
        NETMGR_LOG_E("Write size of the statsInfos fail.");
        return false;
    }

    return std::all_of(statsInfos.begin(), statsInfos.end(),
                       [&parcel](const auto &info) { return info.Marshalling(parcel); });
}

bool NetStatsInfo::Marshalling(Parcel &parcel, const std::unordered_map<uint32_t, NetStatsInfo> &statsInfos)
{
    uint32_t vSize = statsInfos.size();
    if (vSize > STATS_INFO_MAX_SIZE) {
        NETMGR_LOG_E("Size of the statsInfos exceeds maximum.");
        return false;
    }
    if (!parcel.WriteUint32(vSize)) {
        NETMGR_LOG_E("Write size of the statsInfos fail.");
        return false;
    }
    return std::all_of(statsInfos.begin(), statsInfos.end(),
                       [&parcel](const auto &info) { return info.second.Marshalling(parcel); });
}

NetStatsInfo* NetStatsInfo::Unmarshalling(Parcel &parcel)
{
    std::unique_ptr<NetStatsInfo> stats = std::make_unique<NetStatsInfo>();
    if (stats == nullptr) {
        NETMGR_LOG_E("make ptr NetStatsInfo failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(stats->uid_)) {
        return nullptr;
    }
    if (!parcel.ReadString(stats->iface_)) {
        return nullptr;
    }
    if (!parcel.ReadString(stats->ident_)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(stats->date_)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(stats->rxBytes_)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(stats->txBytes_)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(stats->rxPackets_)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(stats->txPackets_)) {
        return nullptr;
    }
    return stats.release();
}

bool NetStatsInfo::Unmarshalling(Parcel &parcel, std::vector<NetStatsInfo> &statsInfos)
{
    uint32_t vSize = 0;
    if (!parcel.ReadUint32(vSize)) {
        NETMGR_LOG_E("Read size of the statsInfos fail.");
        return false;
    }

    if (vSize > STATS_INFO_MAX_SIZE) {
        NETMGR_LOG_E("Size of the statsInfos exceeds maximum.");
        return false;
    }
    statsInfos.reserve(vSize);
    for (uint32_t i = 0; i < vSize; i++) {
        NetStatsInfo tmpData;
        if (!NetStatsInfo::Unmarshalling(parcel, tmpData)) {
            NETMGR_LOG_E("Unmarshalling the statsInfo fail.");
            return false;
        }
        statsInfos.push_back(std::move(tmpData));
    }

    return true;
}

bool NetStatsInfo::Unmarshalling(Parcel &parcel, std::unordered_map<uint32_t, NetStatsInfo> &statsInfos)
{
    uint32_t vSize = 0;
    if (!parcel.ReadUint32(vSize)) {
        NETMGR_LOG_E("Read size of the statsInfos fail.");
        return false;
    }
    if (vSize > STATS_INFO_MAX_SIZE) {
        NETMGR_LOG_E("Size of the statsInfos exceeds maximum.");
        return false;
    }
    for (uint32_t i = 0; i < vSize; i++) {
        NetStatsInfo tmpData;
        if (!NetStatsInfo::Unmarshalling(parcel, tmpData)) {
            NETMGR_LOG_E("Unmarshalling the statsInfo fail.");
            return false;
        }
        statsInfos.emplace(tmpData.uid_, std::move(tmpData));
    }
    return true;
}

bool NetStatsInfo::Unmarshalling(Parcel &parcel, NetStatsInfo &stats)
{
    if (!parcel.ReadUint32(stats.uid_)) {
        return false;
    }
    if (!parcel.ReadString(stats.iface_)) {
        return false;
    }
    if (!parcel.ReadString(stats.ident_)) {
        return false;
    }
    if (!parcel.ReadUint64(stats.date_)) {
        return false;
    }
    if (!parcel.ReadUint64(stats.rxBytes_)) {
        return false;
    }
    if (!parcel.ReadUint64(stats.txBytes_)) {
        return false;
    }
    if (!parcel.ReadUint64(stats.rxPackets_)) {
        return false;
    }
    if (!parcel.ReadUint64(stats.txPackets_)) {
        return false;
    }
    return true;
}

bool StatsInfoUnmarshallingVector(Parcel &parcel, std::vector<NetStatsInfo> &statsInfos)
{
    return NetManagerStandard::NetStatsInfo::Unmarshalling(parcel, statsInfos);
}
} // namespace NetManagerStandard
} // namespace OHOS

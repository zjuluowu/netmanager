/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <cinttypes>
#include "net_stats_info_sequence.h"
#include "net_mgr_log_wrapper.h"
#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t STATS_INFO_MAX_SIZE = 5000;
bool NetStatsInfoSequence::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint64(startTime_)) {
        NETMGR_LOG_E("Write statsInfoSequence startTime failed. time=%{public}" PRIu64, startTime_);
        return false;
    }
    if (!parcel.WriteUint64(endTime_)) {
        NETMGR_LOG_E("Write statsInfoSequence endTime failed. time=%{public}" PRIu64, endTime_);
        return false;
    }
    return NetStatsInfo::Marshalling(parcel, info_);
}

bool NetStatsInfoSequence::Marshalling(Parcel &parcel, const NetStatsInfoSequence &statsSequence)
{
    if (!parcel.WriteUint64(statsSequence.startTime_)) {
        NETMGR_LOG_E("Write statsInfoSequence startTime failed. time=%{public}" PRIu64, statsSequence.startTime_);
        return false;
    }
    if (!parcel.WriteUint64(statsSequence.endTime_)) {
        NETMGR_LOG_E("Write statsInfoSequence endTime failed. time=%{public}" PRIu64, statsSequence.endTime_);
        return false;
    }
    return NetStatsInfo::Marshalling(parcel, statsSequence.info_);
}

bool NetStatsInfoSequence::Marshalling(Parcel &parcel, const std::vector<NetStatsInfoSequence> &statsSequence)
{
    uint32_t vSize = statsSequence.size();
    if (vSize > STATS_INFO_MAX_SIZE) {
        NETMGR_LOG_E("Size of the statsInfoSequence exceeds maximum.");
        return false;
    }
    if (!parcel.WriteUint32(vSize)) {
        NETMGR_LOG_E("Write statsInfoSequence size failed. size=%{public}u", vSize);
        return false;
    }
    return std::all_of(statsSequence.begin(), statsSequence.end(),
                       [&parcel](const NetStatsInfoSequence &info) { return info.Marshalling(parcel); });
}

NetStatsInfoSequence* NetStatsInfoSequence::Unmarshalling(Parcel &parcel)
{
    std::unique_ptr<NetStatsInfoSequence> stats = std::make_unique<NetStatsInfoSequence>();
    if (stats == nullptr) {
        NETMGR_LOG_E("make ptr NetStatsInfoSequence failed");
        return nullptr;
    }
    if (!parcel.ReadUint64(stats->startTime_)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(stats->endTime_)) {
        return nullptr;
    }
    NetStatsInfo* statsInfo = NetStatsInfo::Unmarshalling(parcel);
    if (statsInfo == nullptr) {
        return nullptr;
    }
    stats->info_ = *statsInfo;
    return stats.release();
}

bool NetStatsInfoSequence::Unmarshalling(Parcel &parcel, NetStatsInfoSequence &statsSequence)
{
    if (!parcel.ReadUint64(statsSequence.startTime_)) {
        NETMGR_LOG_E("Read statsInfoSequence startTime failed.");
        return false;
    }
    if (!parcel.ReadUint64(statsSequence.endTime_)) {
        NETMGR_LOG_E("Read statsInfoSequence endTime failed.");
        return false;
    }
    return NetStatsInfo::Unmarshalling(parcel, statsSequence.info_);
}

bool NetStatsInfoSequence::Unmarshalling(Parcel &parcel, std::vector<NetStatsInfoSequence> &statsSequence)
{
    uint32_t vSize = 0;
    if (!parcel.ReadUint32(vSize)) {
        NETMGR_LOG_E("Read statsInfoSequence size failed");
        return false;
    }
    if (vSize > STATS_INFO_MAX_SIZE) {
        NETMGR_LOG_E("Size of the statsInfoSequence exceeds maximum.");
        return false;
    }
    statsSequence.reserve(vSize);
    for (uint32_t i = 0; i < vSize; i++) {
        NetStatsInfoSequence tmp;
        if (!NetStatsInfoSequence::Unmarshalling(parcel, tmp)) {
            NETMGR_LOG_E("Unmarshalling the statsInfoSequence fail.");
            return false;
        }
        statsSequence.push_back(std::move(tmp));
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
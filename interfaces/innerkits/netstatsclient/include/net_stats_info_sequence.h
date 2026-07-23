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

#ifndef COMMUNICATION_NETMANAGER_BASE_NET_STATS_INFO_SEQUENCE_H
#define COMMUNICATION_NETMANAGER_BASE_NET_STATS_INFO_SEQUENCE_H

#include "net_stats_info.h"
#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetStatsInfoSequence final : public Parcelable {
    uint64_t startTime_ = 0;
    uint64_t endTime_ = 0;
    NetStatsInfo info_;

    inline bool Equals(const NetStatsInfoSequence &infoSequence) const
    {
        return startTime_ == infoSequence.startTime_ && endTime_ == infoSequence.endTime_ &&
               info_.Equals(infoSequence.info_);
    }

    bool Marshalling(Parcel &parcel) const override;
    static bool Marshalling(Parcel &parcel, const NetStatsInfoSequence &statsSequence);
    static bool Marshalling(Parcel &parcel, const std::vector<NetStatsInfoSequence> &statsSequence);
    static NetStatsInfoSequence* Unmarshalling(Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, NetStatsInfoSequence &statsSequence);
    static bool Unmarshalling(Parcel &parcel, std::vector<NetStatsInfoSequence> &statsSequence);
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_NETMANAGER_BASE_NET_STATS_INFO_SEQUENCE_H
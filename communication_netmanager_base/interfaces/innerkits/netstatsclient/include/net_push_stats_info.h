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
#ifndef COMMUNICATION_NETMANAGER_BASE_NET_PUSH_STATS_INFO_H
#define COMMUNICATION_NETMANAGER_BASE_NET_PUSH_STATS_INFO_H

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct PushStatsInfo final : public Parcelable {
    uint32_t uid_ = 0;
    std::string iface_ = "rmnet_push";
    uint32_t simId_ = 0;
    uint32_t netBearType_ = 0;
    uint32_t beginTime_ = 0;
    uint32_t endTime_ = 0;
    uint32_t rxBytes_ = 0;
    uint32_t txBytes_ = 0;
    int32_t userId_ = 0;

    ~PushStatsInfo() override = default;

    inline uint32_t GetStats() const
    {
        return rxBytes_ + txBytes_;
    }

    inline bool Equal(const PushStatsInfo &info) const
    {
        return info.uid_ == uid_ && info.iface_ == iface_ && info.simId_ == simId_;
    }

    bool Marshalling(Parcel &parcel) const override;
    static bool Marshalling(Parcel &parcel, const PushStatsInfo &info);
    static PushStatsInfo* Unmarshalling(Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, PushStatsInfo &info);
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_NETMANAGER_BASE_NET_PUSH_STATS_INFO_H
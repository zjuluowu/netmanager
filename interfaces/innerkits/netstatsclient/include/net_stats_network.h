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

#ifndef COMMUNICATION_NETMANAGER_BASE_NET_STATS_NETWORK_H
#define COMMUNICATION_NETMANAGER_BASE_NET_STATS_NETWORK_H

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetStatsNetwork final : public Parcelable {
    uint32_t type_ = 0;
    uint64_t startTime_ = 0;
    uint64_t endTime_ = 0;
    uint32_t simId_ = UINT32_MAX;

    bool Marshalling(Parcel &parcel) const override;
    static bool Marshalling(Parcel &parcel, const sptr<NetStatsNetwork> &object);
    static NetStatsNetwork* Unmarshalling(Parcel &parcel);
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_NETMANAGER_BASE_NET_STATS_NETWORK_H
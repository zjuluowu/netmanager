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

#include "net_push_stats_info.h"

namespace OHOS {
namespace NetManagerStandard {

bool PushStatsInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(uid_)) {
        return false;
    }
    if (!parcel.WriteString(iface_)) {
        return false;
    }
    if (!parcel.WriteUint32(simId_)) {
        return false;
    }
    if (!parcel.WriteUint32(netBearType_)) {
        return false;
    }
    if (!parcel.WriteUint32(beginTime_)) {
        return false;
    }
    if (!parcel.WriteUint32(endTime_)) {
        return false;
    }
    if (!parcel.WriteUint32(rxBytes_)) {
        return false;
    }
    if (!parcel.WriteUint32(txBytes_)) {
        return false;
    }
    return true;
}

bool PushStatsInfo::Marshalling(Parcel &parcel, const PushStatsInfo &info)
{
    if (!parcel.WriteUint32(info.uid_)) {
        return false;
    }
    if (!parcel.WriteString(info.iface_)) {
        return false;
    }
    if (!parcel.WriteUint32(info.simId_)) {
        return false;
    }
    if (!parcel.WriteUint32(info.netBearType_)) {
        return false;
    }
    if (!parcel.WriteUint32(info.beginTime_)) {
        return false;
    }
    if (!parcel.WriteUint32(info.endTime_)) {
        return false;
    }
    if (!parcel.WriteUint32(info.rxBytes_)) {
        return false;
    }
    if (!parcel.WriteUint32(info.txBytes_)) {
        return false;
    }
    return true;
}

PushStatsInfo* PushStatsInfo::Unmarshalling(Parcel &parcel)
{
    std::unique_ptr<PushStatsInfo> stats = std::make_unique<PushStatsInfo>();
    if (stats == nullptr) {
        return nullptr;
    }
    if (!parcel.ReadUint32(stats->uid_)) {
        return nullptr;
    }
    if (!parcel.ReadString(stats->iface_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(stats->simId_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(stats->netBearType_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(stats->beginTime_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(stats->endTime_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(stats->rxBytes_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(stats->txBytes_)) {
        return nullptr;
    }
    return stats.release();
}

bool PushStatsInfo::Unmarshalling(Parcel &parcel, PushStatsInfo &info)
{
    if (!parcel.ReadUint32(info.uid_)) {
        return false;
    }
    if (!parcel.ReadString(info.iface_)) {
        return false;
    }
    if (!parcel.ReadUint32(info.simId_)) {
        return false;
    }
    if (!parcel.ReadUint32(info.netBearType_)) {
        return false;
    }
    if (!parcel.ReadUint32(info.beginTime_)) {
        return false;
    }
    if (!parcel.ReadUint32(info.endTime_)) {
        return false;
    }
    if (!parcel.ReadUint32(info.rxBytes_)) {
        return false;
    }
    if (!parcel.ReadUint32(info.txBytes_)) {
        return false;
    }
    return true;
}

} // namespace NetManagerStandard
} // namespace OHOS
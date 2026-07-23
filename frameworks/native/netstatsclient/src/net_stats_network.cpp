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

#include "net_stats_network.h"
#include "net_mgr_log_wrapper.h"
#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {

bool NetStatsNetwork::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(type_)) {
        return false;
    }
    if (!parcel.WriteUint64(startTime_)) {
        return false;
    }
    if (!parcel.WriteUint64(endTime_)) {
        return false;
    }
    if (!parcel.WriteUint32(simId_)) {
        return false;
    }
    return true;
}

bool NetStatsNetwork::Marshalling(Parcel &parcel, const sptr<NetStatsNetwork> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("NetStatsNetwork object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteUint32(object->type_)) {
        return false;
    }
    if (!parcel.WriteUint64(object->startTime_)) {
        return false;
    }
    if (!parcel.WriteUint64(object->endTime_)) {
        return false;
    }
    if (!parcel.WriteUint32(object->simId_)) {
        return false;
    }
    return true;
}

NetStatsNetwork* NetStatsNetwork::Unmarshalling(Parcel &parcel)
{
    std::unique_ptr<NetStatsNetwork> ptr = std::make_unique<NetStatsNetwork>();
    if (ptr == nullptr) {
        NETMGR_LOG_E("make_unique<NetStatsNetwork>() failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(ptr->type_)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(ptr->startTime_)) {
        return nullptr;
    }
    if (!parcel.ReadUint64(ptr->endTime_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(ptr->simId_)) {
        return nullptr;
    }
    return ptr.release();
}
} // namespace NetManagerStandard
} // namespace OHOS
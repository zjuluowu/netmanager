/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "route_type.h"

#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
bool NetworkRouteInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(ifName)) {
        return false;
    }
    if (!parcel.WriteString(destination)) {
        return false;
    }
    if (!parcel.WriteString(nextHop)) {
        return false;
    }
    if (!parcel.WriteBool(isExcludedRoute)) {
        return false;
    }
    return true;
}

sptr<NetworkRouteInfo> NetworkRouteInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetworkRouteInfo> ptr = new (std::nothrow) NetworkRouteInfo();
    if (ptr == nullptr) {
        NETMGR_LOG_E("make_unique<NetworkRouteInfo>() failed");
        return nullptr;
    }
    if (!parcel.ReadString(ptr->ifName)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->destination)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->nextHop)) {
        return nullptr;
    }
    if (!parcel.ReadBool(ptr->isExcludedRoute)) {
        return nullptr;
    }
    return ptr;
}

bool NetworkRouteInfo::Marshalling(Parcel &parcel, const sptr<NetworkRouteInfo> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("NetworkRouteInfo object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteString(object->ifName)) {
        return false;
    }
    if (!parcel.WriteString(object->destination)) {
        return false;
    }
    if (!parcel.WriteString(object->nextHop)) {
        return false;
    }
    if (!parcel.WriteBool(object->isExcludedRoute)) {
        return false;
    }
    return true;
}
} // namespace nmd
} // namespace OHOS
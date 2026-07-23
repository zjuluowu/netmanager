/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "net_ip_mac_info.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
bool NetIpMacInfo::Marshalling(Parcel &parcel, const sptr<NetIpMacInfo> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("NetIpMacInfo object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteString(object->ipAddress_)) {
        return false;
    }

    if (!parcel.WriteString(object->iface_)) {
        return false;
    }

    if (!parcel.WriteString(object->macAddress_)) {
        return false;
    }

    if (!parcel.WriteUint32(object->family_)) {
        return false;
    }
    return true;
}

sptr<NetIpMacInfo> NetIpMacInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetIpMacInfo> ptr = sptr<NetIpMacInfo>::MakeSptr();
    if (ptr == nullptr) {
        NETMGR_LOG_E("create INetAddr failed");
        return nullptr;
    }
    if (!parcel.ReadString(ptr->ipAddress_)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->iface_)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->macAddress_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(ptr->family_)) {
        return nullptr;
    }
    return ptr;
}

bool NetIpMacInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(ipAddress_)) {
        return false;
    }
    if (!parcel.WriteString(iface_)) {
        return false;
    }
    if (!parcel.WriteString(macAddress_)) {
        return false;
    }
    if (!parcel.WriteUint32(family_)) {
        return false;
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
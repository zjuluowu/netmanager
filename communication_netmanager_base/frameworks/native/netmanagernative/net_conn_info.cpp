/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "net_conn_info.h"

namespace OHOS {
namespace NetManagerStandard {
bool NetConnInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(protocolType_)) {
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(family_))) {
        return false;
    }
    if (!parcel.WriteString(localAddress_)) {
        return false;
    }
    if (!parcel.WriteUint16(localPort_)) {
        return false;
    }
    if (!parcel.WriteString(remoteAddress_)) {
        return false;
    }
    if (!parcel.WriteUint16(remotePort_)) {
        return false;
    }
    return true;
}

sptr<NetConnInfo> NetConnInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetConnInfo> ptr = sptr<NetConnInfo>::MakeSptr();
    if (!parcel.ReadInt32(ptr->protocolType_)) {
        return nullptr;
    }
    uint32_t family = 0;
    if (!parcel.ReadUint32(family)) {
        return nullptr;
    }
    ptr->family_ = static_cast<Family>(family);
    if (!parcel.ReadString(ptr->localAddress_)) {
        return nullptr;
    }
    if (!parcel.ReadUint16(ptr->localPort_)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->remoteAddress_)) {
        return nullptr;
    }
    if (!parcel.ReadUint16(ptr->remotePort_)) {
        return nullptr;
    }
    return ptr;
}

bool NetConnInfo::CheckValid() const
{
    if ((protocolType_ != IPPROTO_TCP && protocolType_ != IPPROTO_UDP) ||
        (family_ != Family::IPv4 && family_ != Family::IPv6)) {
        return false;
    }

    in_addr ipv4Addr;
    in6_addr ipv6Addr;
    if (family_ == Family::IPv4) {
        if (inet_pton(AF_INET, localAddress_.c_str(), &ipv4Addr) != 1 ||
            inet_pton(AF_INET, remoteAddress_.c_str(), &ipv4Addr) != 1) {
            return false;
        }
    } else {
        if (inet_pton(AF_INET6, localAddress_.c_str(), &ipv6Addr) != 1 ||
            inet_pton(AF_INET6, remoteAddress_.c_str(), &ipv6Addr) != 1) {
            return false;
        }
    }

    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
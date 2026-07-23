/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "net_supplier_info.h"

#include "parcel.h"
#include "refbase.h"

#include "net_mgr_log_wrapper.h"
namespace OHOS {
namespace NetManagerStandard {
bool NetSupplierInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteBool(isAvailable_)) {
        return false;
    }
    if (!parcel.WriteBool(isRoaming_)) {
        return false;
    }
    if (!parcel.WriteInt8(strength_)) {
        return false;
    }
    if (!parcel.WriteUint32(frequency_)) {
        return false;
    }
    if (!parcel.WriteUint32(linkUpBandwidthKbps_)) {
        return false;
    }
    if (!parcel.WriteUint32(linkDownBandwidthKbps_)) {
        return false;
    }
    if (!parcel.WriteInt32(uid_)) {
        return false;
    }
    if (!parcel.WriteString(ident_)) {
        return false;
    }
    if (!parcel.WriteInt32(score_)) {
        return false;
    }
    return true;
}

sptr<NetSupplierInfo> NetSupplierInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetSupplierInfo> ptr = new (std::nothrow) NetSupplierInfo();
    if (ptr == nullptr) {
        NETMGR_LOG_E("make_unique<NetSupplierInfo>() failed");
        return nullptr;
    }
    if (!parcel.ReadBool(ptr->isAvailable_)) {
        return nullptr;
    }
    if (!parcel.ReadBool(ptr->isRoaming_)) {
        return nullptr;
    }
    if (!parcel.ReadInt8(ptr->strength_)) {
        NETMGR_LOG_E("read strength_ from parcel failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(ptr->frequency_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(ptr->linkUpBandwidthKbps_)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(ptr->linkDownBandwidthKbps_)) {
        return nullptr;
    }
    if (!parcel.ReadInt32(ptr->uid_)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->ident_)) {
        return nullptr;
    }
    if (!parcel.ReadInt32(ptr->score_)) {
        return nullptr;
    }
    return ptr;
}

bool NetSupplierInfo::Marshalling(Parcel &parcel, const sptr<NetSupplierInfo> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("NetSupplierInfo object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteBool(object->isAvailable_)) {
        return false;
    }
    if (!parcel.WriteBool(object->isRoaming_)) {
        return false;
    }
    if (!parcel.WriteInt8(object->strength_)) {
        return false;
    }
    if (!parcel.WriteUint32(object->frequency_)) {
        return false;
    }
    if (!parcel.WriteUint32(object->linkUpBandwidthKbps_)) {
        return false;
    }
    if (!parcel.WriteUint32(object->linkDownBandwidthKbps_)) {
        return false;
    }
    if (!parcel.WriteInt32(object->uid_)) {
        return false;
    }
    if (!parcel.WriteString(object->ident_)) {
        return false;
    }
    if (!parcel.WriteInt32(object->score_)) {
        return false;
    }
    return true;
}

std::string NetSupplierInfo::ToString(const std::string &tab) const
{
    std::string str = "{";
    str.append(tab).append("isAvailable_");
    str.append(tab).append(":").append(std::to_string(isAvailable_)).append(",");

    str.append(tab).append("isRoaming_");
    str.append(tab).append(":").append(std::to_string(isRoaming_)).append(",");

    str.append(tab).append("strength_");
    str.append(tab).append(":").append(std::to_string(strength_)).append(",");

    str.append(tab).append("frequency_");
    str.append(tab).append(":").append(std::to_string(frequency_)).append(",");

    str.append(tab).append("linkUpBandwidthKbps_");
    str.append(tab).append(":").append(std::to_string(linkUpBandwidthKbps_)).append(",");

    str.append(tab).append("linkDownBandwidthKbps_");
    str.append(tab).append(":").append(std::to_string(linkDownBandwidthKbps_)).append(",");

    str.append(tab).append("uid_");
    str.append(tab).append(":").append(std::to_string(uid_)).append(",");

    if (!ident_.empty()) {
        str.append(tab).append("ident_");
        str.append(tab).append(":").append(ident_).append(",");
    }

    str.append(tab).append("score_");
    str.append(tab).append(":").append(std::to_string(score_)).append("}");

    return str;
}
} // namespace NetManagerStandard
} // namespace OHOS
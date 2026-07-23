/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef NET_IP_MAC_INFO_H
#define NET_IP_MAC_INFO_H

#include <string>
#include <vector>
#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {

static constexpr const uint32_t FAMILY_INVALID = 0;
static constexpr const uint32_t FAMILY_V4 = 1;
static constexpr const uint32_t FAMILY_V6 = 2;

struct NetIpMacInfo : public Parcelable {
    std::string ipAddress_;
    std::string iface_;
    std::string macAddress_;
    uint32_t family_ = FAMILY_INVALID;
    
    NetIpMacInfo() = default;
    bool Marshalling(Parcel &parcel) const override;
    static sptr<NetIpMacInfo> Unmarshalling(Parcel &parcel);
    static bool Marshalling(Parcel &parcel, const sptr<NetIpMacInfo> &object);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_IP_MAC_INFO_H
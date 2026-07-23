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

#ifndef NET_ALL_CAPABILITIES_H
#define NET_ALL_CAPABILITIES_H

#include <set>
#include <shared_mutex>

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
enum NetCap {
    NET_CAPABILITY_MMS = 0,
    NET_CAPABILITY_SUPL = 1,
    NET_CAPABILITY_DUN = 2,
    NET_CAPABILITY_IA = 3,
    NET_CAPABILITY_XCAP = 4,
    NET_CAPABILITY_BIP = 5,
    NET_CAPABILITY_NOT_METERED = 11,
    NET_CAPABILITY_INTERNET = 12,
    NET_CAPABILITY_NOT_VPN = 15,
    NET_CAPABILITY_VALIDATED = 16,
    NET_CAPABILITY_PORTAL = 17,
    NET_CAPABILITY_INTERNAL_DEFAULT = 18, // for inner virtual interface if needed.
    NET_CAPABILITY_SNSSAI1 = 19,
    NET_CAPABILITY_SNSSAI2 = 20,
    NET_CAPABILITY_SNSSAI3 = 21,
    NET_CAPABILITY_SNSSAI4 = 22,
    NET_CAPABILITY_SNSSAI5 = 23,
    NET_CAPABILITY_SNSSAI6 = 24,
    NET_CAPABILITY_INTERNAL_CHANNEL = 25,
    NET_CAPABILITY_OEM_PAID = 26,
    NET_CAPABILITY_OEM_PRIVATE = 27,
    NET_CAPABILITY_CHECKING_CONNECTIVITY = 31,
    NET_CAPABILITY_END = 32 // The maximum value is 32. Do not exceed the limit.
};

enum ProxyModeType {
    PROXY_MODE_OFF = 0,
    PROXY_MODE_AUTO = 1
};

enum NetBearType {
    BEARER_CELLULAR = 0,
    BEARER_WIFI = 1,
    BEARER_BLUETOOTH = 2,
    BEARER_ETHERNET = 3,
    BEARER_VPN = 4,
    BEARER_WIFI_AWARE = 5,
    BEARER_DEFAULT // The maximum value of NetBearType. Do not exceed the limit. No actual meaning.
};

struct NetAllCapabilities final : public Parcelable {
    uint32_t linkUpBandwidthKbps_ = 0;
    uint32_t linkDownBandwidthKbps_ = 0;
    mutable std::shared_mutex netCapsMutex_;
    std::set<NetCap> netCaps_;
    std::set<NetBearType> bearerTypes_;

    NetAllCapabilities() = default;
    NetAllCapabilities(const NetAllCapabilities &cap);
    NetAllCapabilities &operator=(const NetAllCapabilities &cap);

    bool CapsIsValid() const;
    bool CapsIsNull() const;
    bool Marshalling(Parcel &parcel) const override;
    bool Unmarshalling(Parcel &parcel);
    std::string ToString(const std::string &tab) const;

private:
    void ConvertCapToStr(const NetCap &netCap, std::string &str) const;
    void ToStrNetCaps(const std::set<NetCap> &netCaps, std::string &str) const;
    void ToStrNetBearTypes(const std::set<NetBearType> &bearerTypes, std::string &str) const;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_ALL_CAPABILITIES_H

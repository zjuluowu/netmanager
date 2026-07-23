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

#ifndef NETMANAGER_BASE_NET_CONN_INFO_H
#define NETMANAGER_BASE_NET_CONN_INFO_H

#include <arpa/inet.h>
#include <string>

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetConnInfo final : public Parcelable {
    enum class Family : uint32_t {
        IPv4 = 1,
        IPv6 = 2,
    };

    int32_t protocolType_ = IPPROTO_TCP;
    Family family_ = Family::IPv4;
    std::string localAddress_;
    uint16_t localPort_ = 0;
    std::string remoteAddress_;
    uint16_t remotePort_ = 0;

    bool CheckValid() const;
    bool Marshalling(Parcel &parcel) const override;
    static sptr<NetConnInfo> Unmarshalling(Parcel &parcel);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_BASE_NET_CONN_INFO_H
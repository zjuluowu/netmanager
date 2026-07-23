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

#ifndef NET_LISTENING_PORTS_INFO_H
#define NET_LISTENING_PORTS_INFO_H

#include <string>
#include <vector>
#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {

struct TcpNetPortStatesInfo : public Parcelable {
    std::string tcpLocalIp_ = "";
    uint16_t tcpLocalPort_ = 0;
    std::string tcpRemoteIp_;
    uint16_t tcpRemotePort_ = 0;
    uint32_t tcpUid_ = 0;
    uint32_t tcpPid_ = 0;
    uint8_t tcpState_ = 0;

    TcpNetPortStatesInfo() = default;
    bool Marshalling(Parcel &parcel) const override;
    static sptr<TcpNetPortStatesInfo> Unmarshalling(Parcel &parcel);
};

struct UdpNetPortStatesInfo : public Parcelable {
    std::string udpLocalIp_ = "";
    uint16_t udpLocalPort_ = 0;
    uint32_t udpUid_ = 0;
    uint32_t udpPid_ = 0;

    UdpNetPortStatesInfo() = default;
    bool Marshalling(Parcel &parcel) const override;
    static sptr<UdpNetPortStatesInfo> Unmarshalling(Parcel &parcel);
};

struct NetPortStatesInfo : public Parcelable {
    std::vector<TcpNetPortStatesInfo> tcpNetPortStatesInfo_;
    std::vector<UdpNetPortStatesInfo> udpNetPortStatesInfo_;

    NetPortStatesInfo() = default;
    bool Marshalling(Parcel &parcel) const override;
    static sptr<NetPortStatesInfo> Unmarshalling(Parcel &parcel);
    static bool Marshalling(Parcel &parcel, const sptr<NetPortStatesInfo> &object);
    bool WriteTcpVector(Parcel &parcel, const std::vector<TcpNetPortStatesInfo> &tcpNetPortStatesInfo) const;
    bool ReadTcpVector(Parcel &parcel, std::vector<TcpNetPortStatesInfo> &tcpNetPortStatesInfo);
    bool WriteUdpVector(Parcel &parcel, const std::vector<UdpNetPortStatesInfo> &udpNetPortStatesInfo) const;
    bool ReadUdpVector(Parcel &parcel, std::vector<UdpNetPortStatesInfo> &udpNetPortStatesInfo);
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_LISTENING_PORTS_INFO_H
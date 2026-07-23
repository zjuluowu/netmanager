/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NETSYS_NET_DIAG_DATA_H
#define NETSYS_NET_DIAG_DATA_H

#include <iostream>
#include <list>

#include "parcel.h"

namespace OHOS {
namespace NetsysNative {
enum NetDiagForceType : uint8_t {
    FORCE_TYPE_IPV4 = 0,
    FORCE_TYPE_IPV6 = 1,
};

enum NetDiagProtocolType : uint8_t {
    PROTOCOL_TYPE_ALL,
    PROTOCOL_TYPE_TCP,
    PROTOCOL_TYPE_UDP,
    PROTOCOL_TYPE_UNIX,
    PROTOCOL_TYPE_RAW
};

struct NetDiagPingOption final : public Parcelable {
    NetDiagForceType forceType_ = FORCE_TYPE_IPV4; // Optional,default is FORCE_TYPE_IPV4
    std::string destination_;                      // Required
    std::string source_;                           // Optional
    uint32_t interval_ = 0;                        // Optional(millisecond),default is 1000ms
    uint16_t count_ = 0;                           // Optional,default is 25 counts
    uint16_t dataSize_ = 0;                        // Optional,default is 56 bytes
    uint16_t mark_ = 0;                            // Optional
    uint16_t ttl_ = 0;                             // Optional
    uint16_t timeOut_ = 0;                         // Optional(second),default is 3 seconds
    uint16_t duration_ = 0;                        // Optional(second),default and maximum duration is 30 seconds
    bool flood_ = false;                           // Optional

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, NetDiagPingOption &pingOption);
};

struct PingIcmpResponseInfo final : public Parcelable {
    uint16_t bytes_ = 0;
    uint16_t icmpSeq_ = 0;
    uint16_t ttl_ = 0;
    uint32_t costTime_ = 0;
    std::string from_;

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, PingIcmpResponseInfo &icmpSeq);
};

struct NetDiagPingResult final : public Parcelable {
    std::string host_;
    std::string ipAddr_;
    uint16_t dateSize_ = 0;
    uint16_t payloadSize_ = 0;
    uint16_t transCount_ = 0;
    uint16_t recvCount_ = 0;
    std::list<PingIcmpResponseInfo> icmpRespList_;

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, NetDiagPingResult &pingResult);
};

struct NetDiagRouteTable final : public Parcelable {
    std::string destination_;
    std::string gateway_;
    std::string mask_;
    std::string iface_;
    std::string flags_;
    uint32_t metric_ = 0;
    uint32_t ref_ = 0;
    uint32_t use_ = 0;

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, NetDiagRouteTable &routeTable);
};

struct NetDiagUnixSocketInfo final : public Parcelable {
    uint16_t refCnt_ = 0;
    uint32_t inode_ = 0;
    std::string protocol_;
    std::string flags_;
    std::string type_;
    std::string state_;
    std::string path_;

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, NetDiagUnixSocketInfo &socketInfo);
};

struct NeyDiagNetProtoSocketInfo final : public Parcelable {
    std::string protocol_;
    std::string localAddr_;
    std::string foreignAddr_;
    std::string state_;
    std::string user_;
    std::string programName_;
    uint16_t recvQueue_ = 0;
    uint16_t sendQueue_ = 0;
    uint32_t inode_;

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, NeyDiagNetProtoSocketInfo &socketInfo);
};

struct NetDiagSocketsInfo final : public Parcelable {
    std::list<NetDiagUnixSocketInfo> unixSocketsInfo_;
    std::list<NeyDiagNetProtoSocketInfo> netProtoSocketsInfo_;

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, NetDiagSocketsInfo &socketsInfo);
};

struct NetDiagIfaceConfig final : public Parcelable {
    std::string ifaceName_;
    std::string linkEncap_;
    std::string macAddr_;
    std::string ipv4Addr_;
    std::string ipv4Bcast_;
    std::string ipv4Mask_;
    std::list<std::pair<std::string, std::string>> ipv6Addrs_;
    uint32_t mtu_ = 0;
    uint32_t txQueueLen_ = 0;
    uint32_t rxBytes_ = 0;
    uint32_t txBytes_ = 0;
    bool isUp_ = false;

    void Initialize();
    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, NetDiagIfaceConfig &ifaceConfig);
};
} // namespace NetsysNative
} // namespace OHOS
#endif // NETSYS_NET_DIAG_DATA_H
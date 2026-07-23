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

#ifndef NET_TRACE_ROUTE_INFO_H
#define NET_TRACE_ROUTE_INFO_H

#define NETCONN_MAX_JUMP_NUM 30
#define NETCONN_MAX_STR_LEN 256

#include <string>

namespace OHOS {
namespace NetManagerStandard {

typedef enum NetConn_PacketsType {
    /** ICMP */
    NETCONN_PACKETS_ICMP = 0,
    /** UDP */
    NETCONN_PACKETS_UDP = 1,
} NetConn_PacketsType;

constexpr int8_t NETCONN_MAX_RTT_NUM = 4;

class TraceRouteOptions {
public:
    TraceRouteOptions() : maxJumpNumber_(NETCONN_MAX_JUMP_NUM),
        packetsType_(NetConn_PacketsType::NETCONN_PACKETS_ICMP) {}
    explicit TraceRouteOptions(int32_t maxJumpNumber, NetConn_PacketsType packetsType)
        : maxJumpNumber_(maxJumpNumber), packetsType_(packetsType) {}
    ~TraceRouteOptions() = default;

public:
    int32_t maxJumpNumber_;
    NetConn_PacketsType packetsType_;
};

class TraceRouteInfo {
public:
    TraceRouteInfo() : jumpNo_(NETCONN_MAX_JUMP_NUM), address_(""), rtt_(std::vector<uint32_t>{}) {}
    explicit TraceRouteInfo(uint8_t jumpNo, std::string address, std::vector<uint32_t> rtt)
        :jumpNo_(jumpNo), address_(address), rtt_(rtt) {}
    ~TraceRouteInfo() = default;

public:
    uint8_t jumpNo_;
    std::string address_;
    std::vector<uint32_t> rtt_;
};

typedef struct NetConn_TraceRouteOption {
    /** Maximum number of jumps */
    uint8_t maxJumpNumber; /** default NETCONN_MAX_JUMP_NUM */
    /** Packets Type */
    NetConn_PacketsType packetsType; /** default ICMP */
} NetConn_TraceRouteOption;

typedef struct NetConn_TraceRouteInfo {
    /** Number of jumps */
    uint8_t jumpNo;
    /** host name or address */
    char address[NETCONN_MAX_STR_LEN];
    /** RTT in millisecond */
    uint32_t rtt[NETCONN_MAX_RTT_NUM];
} NetConn_TraceRouteInfo;

typedef struct NetConn_ProbeResultInfo {
    uint8_t lossRate;
    uint32_t rtt[NETCONN_MAX_RTT_NUM];
} NetConn_ProbeResultInfo;

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_TRACE_ROUTE_INFO_H

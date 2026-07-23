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

#include "netnative_log_wrapper.h"
#include "netsys_net_diag_data.h"

namespace OHOS {
namespace NetsysNative {
namespace {
constexpr uint32_t SOCKET_INFO_LIST_MAX_SIZE = 1024;
constexpr uint32_t ICMP_SEQ_LIST_MAX_SIZE = 1024;
constexpr uint32_t IFCONFIG_MAX_IPV6_ADDR_NUM = 64;
} // namespace

bool NetDiagPingOption::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint8(forceType_)) {
        return false;
    }
    if (!parcel.WriteString(destination_)) {
        return false;
    }
    if (!parcel.WriteString(source_)) {
        return false;
    }
    if (!parcel.WriteUint32(interval_)) {
        return false;
    }
    if (!parcel.WriteUint16(count_)) {
        return false;
    }
    if (!parcel.WriteUint16(dataSize_)) {
        return false;
    }
    if (!parcel.WriteUint16(mark_)) {
        return false;
    }
    if (!parcel.WriteUint16(ttl_)) {
        return false;
    }
    if (!parcel.WriteUint16(timeOut_)) {
        return false;
    }
    if (!parcel.WriteUint16(duration_)) {
        return false;
    }
    if (!parcel.WriteBool(flood_)) {
        return false;
    }
    return true;
}

bool NetDiagPingOption::Unmarshalling(Parcel &parcel, NetDiagPingOption &pingOption)
{
    uint8_t forceType = FORCE_TYPE_IPV4;
    if (!parcel.ReadUint8(forceType)) {
        return false;
    }
    pingOption.forceType_ = static_cast<NetDiagForceType>(forceType);
    if (!parcel.ReadString(pingOption.destination_)) {
        return false;
    }
    if (!parcel.ReadString(pingOption.source_)) {
        return false;
    }
    if (!parcel.ReadUint32(pingOption.interval_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingOption.count_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingOption.dataSize_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingOption.mark_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingOption.ttl_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingOption.timeOut_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingOption.duration_)) {
        return false;
    }
    if (!parcel.ReadBool(pingOption.flood_)) {
        return false;
    }
    return true;
}

bool PingIcmpResponseInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint16(bytes_)) {
        return false;
    }
    if (!parcel.WriteUint16(icmpSeq_)) {
        return false;
    }
    if (!parcel.WriteUint16(ttl_)) {
        return false;
    }
    if (!parcel.WriteUint32(costTime_)) {
        return false;
    }
    if (!parcel.WriteString(from_)) {
        return false;
    }
    return true;
}

bool PingIcmpResponseInfo::Unmarshalling(Parcel &parcel, PingIcmpResponseInfo &icmpSeq)
{
    if (!parcel.ReadUint16(icmpSeq.bytes_)) {
        return false;
    }
    if (!parcel.ReadUint16(icmpSeq.icmpSeq_)) {
        return false;
    }
    if (!parcel.ReadUint16(icmpSeq.ttl_)) {
        return false;
    }
    if (!parcel.ReadUint32(icmpSeq.costTime_)) {
        return false;
    }
    if (!parcel.ReadString(icmpSeq.from_)) {
        return false;
    }
    return true;
}

bool NetDiagPingResult::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(host_)) {
        return false;
    }
    if (!parcel.WriteString(ipAddr_)) {
        return false;
    }
    if (!parcel.WriteUint16(dateSize_)) {
        return false;
    }
    if (!parcel.WriteUint16(payloadSize_)) {
        return false;
    }
    if (!parcel.WriteUint16(transCount_)) {
        return false;
    }
    if (!parcel.WriteUint16(recvCount_)) {
        return false;
    }
    if (!parcel.WriteUint32(
        static_cast<uint32_t>(std::min(ICMP_SEQ_LIST_MAX_SIZE, static_cast<uint32_t>(icmpRespList_.size()))))) {
        return false;
    }
    uint32_t count = 0;
    for (const auto &icmpSeq : icmpRespList_) {
        if (!icmpSeq.Marshalling(parcel)) {
            return false;
        }
        if (++count >= ICMP_SEQ_LIST_MAX_SIZE) {
            break;
        }
    }
    return true;
}

bool NetDiagPingResult::Unmarshalling(Parcel &parcel, NetDiagPingResult &pingResult)
{
    if (!parcel.ReadString(pingResult.host_)) {
        return false;
    }
    if (!parcel.ReadString(pingResult.ipAddr_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingResult.dateSize_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingResult.payloadSize_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingResult.transCount_)) {
        return false;
    }
    if (!parcel.ReadUint16(pingResult.recvCount_)) {
        return false;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    size = (size > ICMP_SEQ_LIST_MAX_SIZE) ? ICMP_SEQ_LIST_MAX_SIZE : size;
    for (uint32_t i = 0; i < size; ++i) {
        PingIcmpResponseInfo icmpResponse;
        if (!PingIcmpResponseInfo::Unmarshalling(parcel, icmpResponse)) {
            return false;
        }
        pingResult.icmpRespList_.push_back(icmpResponse);
    }
    return true;
}

bool NetDiagRouteTable::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(destination_)) {
        return false;
    }
    if (!parcel.WriteString(gateway_)) {
        return false;
    }
    if (!parcel.WriteString(mask_)) {
        return false;
    }
    if (!parcel.WriteString(iface_)) {
        return false;
    }
    if (!parcel.WriteString(flags_)) {
        return false;
    }
    if (!parcel.WriteUint32(metric_)) {
        return false;
    }
    if (!parcel.WriteUint32(ref_)) {
        return false;
    }
    if (!parcel.WriteUint32(use_)) {
        return false;
    }
    return true;
}

bool NetDiagRouteTable::Unmarshalling(Parcel &parcel, NetDiagRouteTable &routeTable)
{
    if (!parcel.ReadString(routeTable.destination_)) {
        return false;
    }
    if (!parcel.ReadString(routeTable.gateway_)) {
        return false;
    }
    if (!parcel.ReadString(routeTable.mask_)) {
        return false;
    }
    if (!parcel.ReadString(routeTable.iface_)) {
        return false;
    }
    if (!parcel.ReadString(routeTable.flags_)) {
        return false;
    }
    if (!parcel.ReadUint32(routeTable.metric_)) {
        return false;
    }
    if (!parcel.ReadUint32(routeTable.ref_)) {
        return false;
    }
    if (!parcel.ReadUint32(routeTable.use_)) {
        return false;
    }
    return true;
}

bool NetDiagUnixSocketInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint16(refCnt_)) {
        return false;
    }
    if (!parcel.WriteUint32(inode_)) {
        return false;
    }
    if (!parcel.WriteString(protocol_)) {
        return false;
    }
    if (!parcel.WriteString(flags_)) {
        return false;
    }
    if (!parcel.WriteString(type_)) {
        return false;
    }
    if (!parcel.WriteString(state_)) {
        return false;
    }
    if (!parcel.WriteString(path_)) {
        return false;
    }
    return true;
}

bool NetDiagUnixSocketInfo::Unmarshalling(Parcel &parcel, NetDiagUnixSocketInfo &socketInfo)
{
    if (!parcel.ReadUint16(socketInfo.refCnt_)) {
        return false;
    }
    if (!parcel.ReadUint32(socketInfo.inode_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.protocol_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.flags_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.type_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.state_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.path_)) {
        return false;
    }
    return true;
}

bool NeyDiagNetProtoSocketInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(protocol_)) {
        return false;
    }
    if (!parcel.WriteString(localAddr_)) {
        return false;
    }
    if (!parcel.WriteString(foreignAddr_)) {
        return false;
    }
    if (!parcel.WriteString(state_)) {
        return false;
    }
    if (!parcel.WriteString(user_)) {
        return false;
    }
    if (!parcel.WriteString(programName_)) {
        return false;
    }
    if (!parcel.WriteUint16(recvQueue_)) {
        return false;
    }
    if (!parcel.WriteUint16(sendQueue_)) {
        return false;
    }
    if (!parcel.WriteUint32(inode_)) {
        return false;
    }
    return true;
}

bool NeyDiagNetProtoSocketInfo::Unmarshalling(Parcel &parcel, NeyDiagNetProtoSocketInfo &socketInfo)
{
    if (!parcel.ReadString(socketInfo.protocol_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.localAddr_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.foreignAddr_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.state_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.user_)) {
        return false;
    }
    if (!parcel.ReadString(socketInfo.programName_)) {
        return false;
    }
    if (!parcel.ReadUint16(socketInfo.recvQueue_)) {
        return false;
    }
    if (!parcel.ReadUint16(socketInfo.sendQueue_)) {
        return false;
    }
    if (!parcel.ReadUint32(socketInfo.inode_)) {
        return false;
    }
    return true;
}

bool NetDiagSocketsInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(
        static_cast<uint32_t>(std::min(SOCKET_INFO_LIST_MAX_SIZE, static_cast<uint32_t>(unixSocketsInfo_.size()))))) {
        return false;
    }
    uint32_t count = 0;
    for (const auto &socketInfo : unixSocketsInfo_) {
        if (!socketInfo.Marshalling(parcel)) {
            return false;
        }
        if (++count >= SOCKET_INFO_LIST_MAX_SIZE) {
            break;
        }
    }

    if (!parcel.WriteUint32(static_cast<uint32_t>(
            std::min(SOCKET_INFO_LIST_MAX_SIZE, static_cast<uint32_t>(netProtoSocketsInfo_.size()))))) {
        return false;
    }
    count = 0;
    for (const auto &socketInfo : netProtoSocketsInfo_) {
        if (!socketInfo.Marshalling(parcel)) {
            return false;
        }
        if (++count >= SOCKET_INFO_LIST_MAX_SIZE) {
            break;
        }
    }
    return true;
}

bool NetDiagSocketsInfo::Unmarshalling(Parcel &parcel, NetDiagSocketsInfo &socketsInfo)
{
    std::list<NetDiagUnixSocketInfo>().swap(socketsInfo.unixSocketsInfo_);
    std::list<NeyDiagNetProtoSocketInfo>().swap(socketsInfo.netProtoSocketsInfo_);

    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    size = (size > SOCKET_INFO_LIST_MAX_SIZE) ? SOCKET_INFO_LIST_MAX_SIZE : size;
    for (uint32_t i = 0; i < size; ++i) {
        NetDiagUnixSocketInfo socketInfo;
        if (!NetDiagUnixSocketInfo::Unmarshalling(parcel, socketInfo)) {
            return false;
        }
        socketsInfo.unixSocketsInfo_.push_back(socketInfo);
    }

    size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    size = (size > SOCKET_INFO_LIST_MAX_SIZE) ? SOCKET_INFO_LIST_MAX_SIZE : size;
    for (uint32_t i = 0; i < size; ++i) {
        NeyDiagNetProtoSocketInfo socketInfo;
        if (!NeyDiagNetProtoSocketInfo::Unmarshalling(parcel, socketInfo)) {
            return false;
        }
        socketsInfo.netProtoSocketsInfo_.push_back(socketInfo);
    }
    return true;
}

void NetDiagIfaceConfig::Initialize()
{
    ifaceName_ = "";
    linkEncap_ = "";
    macAddr_ = "";
    macAddr_ = "";
    ipv4Addr_ = "";
    ipv4Bcast_ = "";
    ipv4Mask_ = "";
    ipv4Mask_ = "";
    ipv4Mask_ = "";
    mtu_ = 0;
    txQueueLen_ = 0;
    rxBytes_ = 0;
    txBytes_ = 0;
    isUp_ = false;
    std::list<std::pair<std::string, std::string>>().swap(ipv6Addrs_);
}

bool NetDiagIfaceConfig::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(ifaceName_) || !parcel.WriteString(linkEncap_) || !parcel.WriteString(macAddr_) ||
        !parcel.WriteString(ipv4Addr_) || !parcel.WriteString(ipv4Bcast_) || !parcel.WriteString(ipv4Mask_)) {
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(
        std::min(IFCONFIG_MAX_IPV6_ADDR_NUM, static_cast<uint32_t>(ipv6Addrs_.size()))))) {
        return false;
    }
    uint32_t count = 0;
    for (const auto &addr : ipv6Addrs_) {
        if (!parcel.WriteString(addr.first)) {
            return false;
        }
        if (!parcel.WriteString(addr.second)) {
            return false;
        }
        if (++count >= IFCONFIG_MAX_IPV6_ADDR_NUM) {
            break;
        }
    }
    if (!parcel.WriteUint32(mtu_) || !parcel.WriteUint32(txQueueLen_) || !parcel.WriteUint32(rxBytes_) ||
        !parcel.WriteUint32(txBytes_)) {
        return false;
    }
    if (!parcel.WriteBool(isUp_)) {
        return false;
    }
    return true;
}

bool NetDiagIfaceConfig::Unmarshalling(Parcel &parcel, NetDiagIfaceConfig &ifaceConfig)
{
    if (!parcel.ReadString(ifaceConfig.ifaceName_) || !parcel.ReadString(ifaceConfig.linkEncap_) ||
        !parcel.ReadString(ifaceConfig.macAddr_) || !parcel.ReadString(ifaceConfig.ipv4Addr_) ||
        !parcel.ReadString(ifaceConfig.ipv4Bcast_) || !parcel.ReadString(ifaceConfig.ipv4Mask_)) {
        return false;
    }

    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    size = (size > IFCONFIG_MAX_IPV6_ADDR_NUM) ? IFCONFIG_MAX_IPV6_ADDR_NUM : size;
    for (uint32_t i = 0; i < size; ++i) {
        std::string ipv6Addr;
        std::string scope;
        if (!parcel.ReadString(ipv6Addr)) {
            return false;
        }
        if (!parcel.ReadString(scope)) {
            return false;
        }
        ifaceConfig.ipv6Addrs_.push_back(std::make_pair(ipv6Addr, scope));
    }
    if (!parcel.ReadUint32(ifaceConfig.mtu_) || !parcel.ReadUint32(ifaceConfig.txQueueLen_) ||
        !parcel.ReadUint32(ifaceConfig.rxBytes_) || !parcel.ReadUint32(ifaceConfig.txBytes_)) {
        return false;
    }
    if (!parcel.ReadBool(ifaceConfig.isUp_)) {
        return false;
    }
    return true;
}
} // namespace NetsysNative
} // namespace OHOS

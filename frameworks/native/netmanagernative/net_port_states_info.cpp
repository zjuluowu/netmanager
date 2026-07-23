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

#include "net_port_states_info.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr uint32_t PORT_STATES_MAX_LIST_SIZE = 1000;
}
// LCOV_EXCL_START This will never happen.
sptr<TcpNetPortStatesInfo> TcpNetPortStatesInfo::Unmarshalling(Parcel &parcel)
{
    sptr<TcpNetPortStatesInfo> ptr = sptr<TcpNetPortStatesInfo>::MakeSptr();
    if (!parcel.ReadString(ptr->tcpLocalIp_) ||
        !parcel.ReadUint16(ptr->tcpLocalPort_) ||
        !parcel.ReadString(ptr->tcpRemoteIp_) ||
        !parcel.ReadUint16(ptr->tcpRemotePort_) ||
        !parcel.ReadUint32(ptr->tcpUid_) ||
        !parcel.ReadUint32(ptr->tcpPid_) ||
        !parcel.ReadUint8(ptr->tcpState_)) {
        return nullptr;
    }

    return ptr;
}

bool TcpNetPortStatesInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(tcpLocalIp_) ||
        !parcel.WriteUint16(tcpLocalPort_) ||
        !parcel.WriteString(tcpRemoteIp_) ||
        !parcel.WriteUint16(tcpRemotePort_) ||
        !parcel.WriteUint32(tcpUid_) ||
        !parcel.WriteUint32(tcpPid_) ||
        !parcel.WriteUint8(tcpState_)) {
        return false;
    }
    return true;
}

sptr<UdpNetPortStatesInfo> UdpNetPortStatesInfo::Unmarshalling(Parcel &parcel)
{
    sptr<UdpNetPortStatesInfo> ptr = sptr<UdpNetPortStatesInfo>::MakeSptr();
    if (!parcel.ReadString(ptr->udpLocalIp_) ||
        !parcel.ReadUint16(ptr->udpLocalPort_) ||
        !parcel.ReadUint32(ptr->udpUid_) ||
        !parcel.ReadUint32(ptr->udpPid_)) {
        return nullptr;
    }
    return ptr;
}

bool UdpNetPortStatesInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(udpLocalIp_) ||
        !parcel.WriteUint16(udpLocalPort_) ||
        !parcel.WriteUint32(udpUid_) ||
        !parcel.WriteUint32(udpPid_)) {
        return false;
    }
    return true;
}

bool NetPortStatesInfo::WriteTcpVector(Parcel &parcel,
    const std::vector<TcpNetPortStatesInfo> &tcpNetPortStatesInfo) const
{
    if (tcpNetPortStatesInfo.size() > PORT_STATES_MAX_LIST_SIZE) {
        NETMGR_LOG_E("Tcp net port states info size over 1000 when marshalling.");
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(tcpNetPortStatesInfo.size()))) {
        return false;
    }
    for (const auto &item : tcpNetPortStatesInfo) {
        if (!item.Marshalling(parcel)) {
            return false;
        }
    }
    return true;
}

bool NetPortStatesInfo::ReadTcpVector(Parcel &parcel, std::vector<TcpNetPortStatesInfo> &tcpNetPortStatesInfo)
{
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    if (size > PORT_STATES_MAX_LIST_SIZE) {
        NETMGR_LOG_E("Tcp net port states info size over 1000 when unmarshalling.");
        return false;
    }
    std::vector<TcpNetPortStatesInfo> tempTcpNetPortStatesInfo;
    tempTcpNetPortStatesInfo.resize(size);
    tcpNetPortStatesInfo.resize(size);
    for (uint32_t i = 0; i < size; ++i) {
        sptr<TcpNetPortStatesInfo> tcpInfo;
        tcpInfo = TcpNetPortStatesInfo::Unmarshalling(parcel);
        if (tcpInfo == nullptr) {
            return false;
        }
        tempTcpNetPortStatesInfo[i] = *tcpInfo;
    }
    tcpNetPortStatesInfo = std::move(tempTcpNetPortStatesInfo);
    return true;
}

bool NetPortStatesInfo::WriteUdpVector(Parcel &parcel,
    const std::vector<UdpNetPortStatesInfo> &udpNetPortStatesInfo) const
{
    if (udpNetPortStatesInfo.size() > PORT_STATES_MAX_LIST_SIZE) {
        NETMGR_LOG_E("Udp net port states info size over 1000 when marshalling.");
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(udpNetPortStatesInfo.size()))) {
        return false;
    }
    for (const auto &item : udpNetPortStatesInfo) {
        if (!item.Marshalling(parcel)) {
            return false;
        }
    }
    return true;
}

bool NetPortStatesInfo::ReadUdpVector(Parcel &parcel, std::vector<UdpNetPortStatesInfo> &udpNetPortStatesInfo)
{
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    if (size > PORT_STATES_MAX_LIST_SIZE) {
        NETMGR_LOG_E("Udp net port states info size over 1000 when unmarshalling.");
        return false;
    }
    std::vector<UdpNetPortStatesInfo> tempUdpNetPortStatesInfo;
    tempUdpNetPortStatesInfo.resize(size);
    for (uint32_t i = 0; i < size; ++i) {
        sptr<UdpNetPortStatesInfo> udpInfo;
        udpInfo = UdpNetPortStatesInfo::Unmarshalling(parcel);
        if (udpInfo == nullptr) {
            return false;
        }
        tempUdpNetPortStatesInfo[i] = *udpInfo;
    }
    udpNetPortStatesInfo = std::move(tempUdpNetPortStatesInfo);
    return true;
}

bool NetPortStatesInfo::Marshalling(Parcel &parcel, const sptr<NetPortStatesInfo> &object)
{
    if (object == nullptr) {
        return false;
    }
    if (!object->WriteTcpVector(parcel, object->tcpNetPortStatesInfo_)) {
        return false;
    }
    if (!object->WriteUdpVector(parcel, object->udpNetPortStatesInfo_)) {
        return false;
    }
    return true;
}

sptr<NetPortStatesInfo> NetPortStatesInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetPortStatesInfo> ptr = sptr<NetPortStatesInfo>::MakeSptr();
    if (ptr == nullptr) {
        return nullptr;
    }
    if (!ptr->ReadTcpVector(parcel, ptr->tcpNetPortStatesInfo_)) {
        return nullptr;
    }
    if (!ptr->ReadUdpVector(parcel, ptr->udpNetPortStatesInfo_)) {
        return nullptr;
    }
    return ptr;
}

bool NetPortStatesInfo::Marshalling(Parcel &parcel) const
{
    if (!WriteTcpVector(parcel, tcpNetPortStatesInfo_)) {
        return false;
    }
    if (!WriteUdpVector(parcel, udpNetPortStatesInfo_)) {
        return false;
    }
    return true;
}
// LCOV_EXCL_STOP
} // namespace NetManagerStandard
} // namespace OHOS
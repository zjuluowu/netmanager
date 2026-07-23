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
#include "netsys_net_dns_result_data.h"

namespace OHOS {
namespace NetsysNative {

bool NetDnsResultAddrInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(type_)) {
        return false;
    }
    if (!parcel.WriteString(addr_)) {
        return false;
    }
    return true;
}


bool NetDnsResultAddrInfo::Unmarshalling(Parcel &parcel, NetDnsResultAddrInfo &addrInfo)
{
    if (!parcel.ReadUint32(addrInfo.type_)) {
        return false;
    }

    if (!parcel.ReadString(addrInfo.addr_)) {
        return false;
    }

    return true;
}

bool NetDnsResultServerInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint8(type_)) {
        return false;
    }
    if (!parcel.WriteUint8(protocol_)) {
        return false;
    }
    if (!parcel.WriteString(addr_)) {
        return false;
    }
    return true;
}
 
 
bool NetDnsResultServerInfo::Unmarshalling(Parcel &parcel, NetDnsResultServerInfo &serverInfo)
{
    if (!parcel.ReadUint8(serverInfo.type_)) {
        return false;
    }
    if (!parcel.ReadUint8(serverInfo.protocol_)) {
        return false;
    }
    if (!parcel.ReadString(serverInfo.addr_)) {
        return false;
    }
    return true;
}

bool NetDnsResultReport::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(netid_)) {
        return false;
    }
    if (!parcel.WriteUint32(uid_)) {
        return false;
    }
    if (!parcel.WriteUint32(pid_)) {
        return false;
    }
    if (!parcel.WriteUint32(timeused_)) {
        return false;
    }
    if (!parcel.WriteUint32(queryresult_)) {
        return false;
    }
    if (!parcel.WriteString(host_)) {
        return false;
    }
    if (!parcel.WriteUint64(querytime_)) {
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(
        std::min(DNS_RESULT_MAX_SIZE, static_cast<uint32_t>(addrlist_.size()))))) {
        return false;
    }
    // LCOV_EXCL_START
    if (!parcel.WriteUint32(static_cast<uint32_t>(
        std::min(DNS_RESULT_MAX_SIZE, static_cast<uint32_t>(serverlist_.size()))))) {
        return false;
    }
    // LCOV_EXCL_STOP
    uint32_t count = 0;
    for (const auto &addr : addrlist_) {
        if (!addr.Marshalling(parcel)) {
            return false;
        }
        if (++count >= DNS_RESULT_MAX_SIZE) {
            break;
        }
    }
    uint32_t serverCount = 0;
    for (const auto &addr : serverlist_) {
        if (!addr.Marshalling(parcel)) {
            return false;
        }
        // LCOV_EXCL_START
        if (++serverCount >= DNS_RESULT_MAX_SIZE) {
            break;
        }
        // LCOV_EXCL_STOP
    }
    return true;
}


bool NetDnsResultReport::Unmarshalling(Parcel &parcel, NetDnsResultReport &resultReport)
{
    std::list<NetDnsResultAddrInfo>().swap(resultReport.addrlist_);
    std::vector<NetDnsResultServerInfo>().swap(resultReport.serverlist_);

    if (!parcel.ReadUint32(resultReport.netid_) || !parcel.ReadUint32(resultReport.uid_) ||
        !parcel.ReadUint32(resultReport.pid_) || !parcel.ReadUint32(resultReport.timeused_)) {
        return false;
    }

    if (!parcel.ReadUint32(resultReport.queryresult_)) {
        return false;
    }

    if (!parcel.ReadString(resultReport.host_)) {
        return false;
    }

    if (!parcel.ReadUint64(resultReport.querytime_)) {
        return false;
    }
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    uint32_t serverSize = 0;
    // LCOV_EXCL_START
    if (!parcel.ReadUint32(serverSize)) {
        return false;
    }
    // LCOV_EXCL_STOP
    size = (size > DNS_RESULT_MAX_SIZE) ? DNS_RESULT_MAX_SIZE : size;
    for (uint32_t i = 0; i < size; ++i) {
        NetDnsResultAddrInfo addrInfo;
        if (!NetDnsResultAddrInfo::Unmarshalling(parcel, addrInfo)) {
            return false;
        }
        resultReport.addrlist_.push_back(addrInfo);
    }
    // LCOV_EXCL_START
    for (uint32_t i = 0; i < serverSize; ++i) {
        NetDnsResultServerInfo serverInfo;
        if (!NetDnsResultServerInfo::Unmarshalling(parcel, serverInfo)) {
            return false;
        }
        resultReport.serverlist_.push_back(serverInfo);
    }
    // LCOV_EXCL_STOP
    return true;
}

bool NetDnsQueryResultAddrInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint16(type_)) {
        return false;
    }
    if (!parcel.WriteString(addr_)) {
        return false;
    }
    return true;
}

bool NetDnsQueryResultAddrInfo::Unmarshalling(Parcel &parcel, NetDnsQueryResultAddrInfo &addrInfo)
{
    if (!parcel.ReadUint16(addrInfo.type_)) {
        return false;
    }
    if (!parcel.ReadString(addrInfo.addr_)) {
        return false;
    }
    return true;
}

bool NetDnsQueryResultReport::MarshallingExt(Parcel &parcel) const
{
    return (!parcel.WriteUint32(uid_)
        ||!parcel.WriteUint32(pid_)
        ||!parcel.WriteString(srcAddr_)
        ||!parcel.WriteUint8(addrSize_)
        ||!parcel.WriteUint64(queryTime_)
        ||!parcel.WriteString(host_)
        ||!parcel.WriteInt32(retCode_)
        ||!parcel.WriteUint32(firstQueryEndDuration_)
        ||!parcel.WriteUint32(firstQueryEnd2AppDuration_)
        ||!parcel.WriteInt32(ipv4RetCode_)
        ||!parcel.WriteString(ipv4ServerName_)
        ||!parcel.WriteInt32(ipv6RetCode_)
        ||!parcel.WriteString(ipv6ServerName_)
        ||!parcel.WriteUint8(sourceFrom_)
        ||!parcel.WriteUint8(dnsServerSize_)
        ||!parcel.WriteUint16(resBitInfo_)
    );
}

bool NetDnsQueryResultReport::Marshalling(Parcel &parcel) const
{
    if (MarshallingExt(parcel)) {
        return false;
    }
    for (std::string server : dnsServerList_) {
        if (!parcel.WriteString(server)) {
            return false;
        }
    }
    for (const auto &addr : addrlist_) {
        if (!addr.Marshalling(parcel)) {
            return false;
        }
    }
    return true;
}

bool NetDnsQueryResultReport::UnmarshallingExt(Parcel &parcel, NetDnsQueryResultReport &resultReport)
{
    return (
        !parcel.ReadUint32(resultReport.uid_)
        ||!parcel.ReadUint32(resultReport.pid_)
        ||!parcel.ReadString(resultReport.srcAddr_)
        ||!parcel.ReadUint8(resultReport.addrSize_)
        ||!parcel.ReadUint64(resultReport.queryTime_)
        ||!parcel.ReadString(resultReport.host_)
        ||!parcel.ReadInt32(resultReport.retCode_)
        ||!parcel.ReadUint32(resultReport.firstQueryEndDuration_)
        ||!parcel.ReadUint32(resultReport.firstQueryEnd2AppDuration_)
        ||!parcel.ReadInt32(resultReport.ipv4RetCode_)
        ||!parcel.ReadString(resultReport.ipv4ServerName_)
        ||!parcel.ReadInt32(resultReport.ipv6RetCode_)
        ||!parcel.ReadString(resultReport.ipv6ServerName_)
        ||!parcel.ReadUint8(resultReport.sourceFrom_)
        ||!parcel.ReadUint8(resultReport.dnsServerSize_)
        ||!parcel.ReadUint16(resultReport.resBitInfo_)
    );
}

bool NetDnsQueryResultReport::Unmarshalling(Parcel &parcel, NetDnsQueryResultReport &resultReport)
{
    if (NetDnsQueryResultReport::UnmarshallingExt(parcel, resultReport)) {
        return false;
    }
    for (uint32_t i = 0; i < resultReport.dnsServerSize_; ++i) {
        std::string dnsServer;
        if (!parcel.ReadString(dnsServer)) {
            return false;
        }
        resultReport.dnsServerList_.push_back(dnsServer);
    }
    
    uint8_t size = resultReport.addrSize_;
    for (uint8_t j = 0; j < size; ++j) {
        NetDnsQueryResultAddrInfo addrInfo;
        if (!NetDnsQueryResultAddrInfo::Unmarshalling(parcel, addrInfo)) {
            return false;
        }
        resultReport.addrlist_.push_back(addrInfo);
    }
    return true;
}
} // namespace NetsysNative
} // namespace OHOS

/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "net_link_info.h"

#include "parcel.h"
#include "refbase.h"
#include "route.h"

#include "inet_addr.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_ADDR_SIZE = 16;
static constexpr uint32_t MAX_ROUTE_SIZE = 1024;

NetLinkInfo::NetLinkInfo(const NetLinkInfo &linkInfo)
{
    ifaceName_ = linkInfo.ifaceName_;
    domain_ = linkInfo.domain_;
    netAddrList_.assign(linkInfo.netAddrList_.begin(), linkInfo.netAddrList_.end());
    dnsList_.assign(linkInfo.dnsList_.begin(), linkInfo.dnsList_.end());
    routeList_.assign(linkInfo.routeList_.begin(), linkInfo.routeList_.end());
    mtu_ = linkInfo.mtu_;
    tcpBufferSizes_ = linkInfo.tcpBufferSizes_;
    ident_ = linkInfo.ident_;
    httpProxy_ = linkInfo.httpProxy_;
    isUserDefinedDnsServer_ = linkInfo.isUserDefinedDnsServer_;
    isIpv4LinkValid_ = linkInfo.isIpv4LinkValid_;
    isIpv6LinkValid_ = linkInfo.isIpv6LinkValid_;
}

NetLinkInfo &NetLinkInfo::operator=(const NetLinkInfo &linkInfo)
{
    ifaceName_ = linkInfo.ifaceName_;
    domain_ = linkInfo.domain_;
    netAddrList_.assign(linkInfo.netAddrList_.begin(), linkInfo.netAddrList_.end());
    dnsList_.assign(linkInfo.dnsList_.begin(), linkInfo.dnsList_.end());
    routeList_.assign(linkInfo.routeList_.begin(), linkInfo.routeList_.end());
    mtu_ = linkInfo.mtu_;
    tcpBufferSizes_ = linkInfo.tcpBufferSizes_;
    ident_ = linkInfo.ident_;
    httpProxy_ = linkInfo.httpProxy_;
    isUserDefinedDnsServer_ = linkInfo.isUserDefinedDnsServer_;
    isIpv4LinkValid_ = linkInfo.isIpv4LinkValid_;
    isIpv6LinkValid_ = linkInfo.isIpv6LinkValid_;
    return *this;
}

bool NetLinkInfo::AddrMarshalling(Parcel &parcel, const std::list<INetAddr> &addrList) const
{
    uint32_t size = addrList.size();
    size = size > MAX_ADDR_SIZE ? MAX_ADDR_SIZE : size;
    if (!parcel.WriteUint32(size)) {
        return false;
    }
    uint32_t i;
    auto it = addrList.begin();
    for (i = 0; i < size && it != addrList.end(); i++, it++) {
        if (!it->Marshalling(parcel)) {
            NETMGR_LOG_E("write net address to parcel failed");
            return false;
        }
    }
    return true;
}

bool NetLinkInfo::RouteMarshalling(Parcel &parcel, const std::list<Route> &routeList) const
{
    uint32_t size = routeList.size();
    size = size > MAX_ROUTE_SIZE ? MAX_ROUTE_SIZE : size;
    if (!parcel.WriteUint32(size)) {
        return false;
    }
    uint32_t i;
    auto it = routeList.begin();
    for (i = 0; i < size && it != routeList.end(); i++, it++) {
        if (!it->Marshalling(parcel)) {
            NETMGR_LOG_E("write route to parcel failed");
            return false;
        }
    }
    return true;
}

bool NetLinkInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(ifaceName_)) {
        return false;
    }
    if (!parcel.WriteString(domain_)) {
        return false;
    }
    if (!AddrMarshalling(parcel, netAddrList_)) {
        return false;
    }
    if (!AddrMarshalling(parcel, dnsList_)) {
        return false;
    }
    if (!RouteMarshalling(parcel, routeList_)) {
        return false;
    }
    if (!parcel.WriteUint16(mtu_)) {
        return false;
    }
    if (!parcel.WriteString(tcpBufferSizes_) || !parcel.WriteString(ident_)) {
        return false;
    }
    if (!parcel.WriteBool(isUserDefinedDnsServer_)) {
        return false;
    }
    if (!parcel.WriteBool(isIpv4LinkValid_)) {
        return false;
    }
    if (!parcel.WriteBool(isIpv6LinkValid_)) {
        return false;
    }
    if (!httpProxy_.Marshalling(parcel)) {
        NETMGR_LOG_E("Write http proxy to parcel failed");
        return false;
    }
    return true;
}

sptr<NetLinkInfo> NetLinkInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetLinkInfo> ptr = new (std::nothrow) NetLinkInfo();
    if (ptr == nullptr) {
        return nullptr;
    }
    uint32_t size = 0;
    if (!parcel.ReadString(ptr->ifaceName_) || !parcel.ReadString(ptr->domain_) || !parcel.ReadUint32(size)) {
        return nullptr;
    }
    size = size > MAX_ADDR_SIZE ? MAX_ADDR_SIZE : size;
    sptr<INetAddr> netAddr;
    for (uint32_t i = 0; i < size; i++) {
        netAddr = INetAddr::Unmarshalling(parcel);
        if (netAddr == nullptr) {
            NETMGR_LOG_E("INetAddr::Unmarshalling(parcel) is null");
            return nullptr;
        }
        ptr->netAddrList_.push_back(*netAddr);
    }
    if (!parcel.ReadUint32(size)) {
        return nullptr;
    }
    size = size > MAX_ADDR_SIZE ? MAX_ADDR_SIZE : size;
    for (uint32_t i = 0; i < size; i++) {
        netAddr = INetAddr::Unmarshalling(parcel);
        if (netAddr == nullptr) {
            NETMGR_LOG_E("INetAddr::Unmarshalling(parcel) is null");
            return nullptr;
        }
        ptr->dnsList_.push_back(*netAddr);
    }
    if (!parcel.ReadUint32(size)) {
        return nullptr;
    }
    size = size > MAX_ROUTE_SIZE ? MAX_ROUTE_SIZE : size;
    sptr<Route> route;
    for (uint32_t i = 0; i < size; i++) {
        route = Route::Unmarshalling(parcel);
        if (route == nullptr) {
            NETMGR_LOG_E("Route::Unmarshalling(parcel) is null");
            return nullptr;
        }
        ptr->routeList_.push_back(*route);
    }
    if (!ReadInfoFromParcel(parcel, ptr)) {
        return nullptr;
    }
    return ptr;
}

bool NetLinkInfo::ReadInfoFromParcel(Parcel &parcel, sptr<NetLinkInfo> &ptr)
{
    if (!parcel.ReadUint16(ptr->mtu_) || !parcel.ReadString(ptr->tcpBufferSizes_) || !parcel.ReadString(ptr->ident_)) {
        return false;
    }
    if (!parcel.ReadBool(ptr->isUserDefinedDnsServer_)) {
        return false;
    }
    if (!parcel.ReadBool(ptr->isIpv4LinkValid_)) {
        return false;
    }
    if (!parcel.ReadBool(ptr->isIpv6LinkValid_)) {
        return false;
    }
    if (!HttpProxy::Unmarshalling(parcel, ptr->httpProxy_)) {
        return false;
    }
    return true;
}

bool NetLinkInfo::Marshalling(Parcel &parcel, const sptr<NetLinkInfo> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("NetLinkInfo object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteString(object->ifaceName_)) {
        return false;
    }
    if (!parcel.WriteString(object->domain_)) {
        return false;
    }
    if (!object->AddrMarshalling(parcel, object->netAddrList_)) {
        return false;
    }
    if (!object->AddrMarshalling(parcel, object->dnsList_)) {
        return false;
    }
    if (!object->RouteMarshalling(parcel, object->routeList_)) {
        return false;
    }
    if (!parcel.WriteUint16(object->mtu_) || !parcel.WriteString(object->tcpBufferSizes_) ||
        !parcel.WriteString(object->ident_)|| !parcel.WriteBool(object->isUserDefinedDnsServer_) ||
        !parcel.WriteBool(object->isIpv4LinkValid_) || !parcel.WriteBool(object->isIpv6LinkValid_)) {
        return false;
    }
    if (!object->httpProxy_.Marshalling(parcel)) {
        NETMGR_LOG_E("Write http proxy to parcel failed");
        return false;
    }
    return true;
}

void NetLinkInfo::Initialize()
{
    ifaceName_ = "";
    domain_ = "";
    std::list<INetAddr>().swap(netAddrList_);
    std::list<INetAddr>().swap(dnsList_);
    std::list<Route>().swap(routeList_);
    mtu_ = 0;
    tcpBufferSizes_ = "";
    ident_ = "";
}

bool NetLinkInfo::HasNetAddr(const INetAddr &netAddr) const
{
    return std::find(netAddrList_.begin(), netAddrList_.end(), netAddr) != netAddrList_.end();
}

bool NetLinkInfo::operator==(const NetLinkInfo &info) const
{
    bool out = true;
    out = out && (ifaceName_ == info.ifaceName_);
    out = out && (domain_ == info.domain_);
    out = out && (netAddrList_ == info.netAddrList_);
    out = out && (dnsList_ == info.dnsList_);
    out = out && (routeList_ == info.routeList_);
    out = out && (mtu_ == info.mtu_);
    out = out && (tcpBufferSizes_ == info.tcpBufferSizes_);
    out = out && (ident_ == info.ident_);
    out = out && (httpProxy_ == info.httpProxy_);
    return out;
}

bool NetLinkInfo::operator!=(const NetLinkInfo &info) const
{
    return !(*this == info);
}

bool NetLinkInfo::HasRoute(const Route &route) const
{
    return std::find(routeList_.begin(), routeList_.end(), route) != routeList_.end();
}

bool NetLinkInfo::HasIpv6DefaultRoute() const
{
    for (const auto &route: routeList_) {
        if (route.IsIpv6Default()) {
            return true;
        }
    }
    return false;
}

bool NetLinkInfo::HasIpv4DefaultRoute() const
{
    for (const auto &route: routeList_) {
        if (route.IsIpv4Default()) {
            return true;
        }
    }
    return false;
}

bool NetLinkInfo::IsIpv4Provisioned() const
{
    return HasIpv4DefaultRoute() && HasIpv4Address() && HasIpv4DnsServer();
}

bool NetLinkInfo::HasIpv4Address() const
{
    for (const auto &addr: netAddrList_) {
        if (addr.type_ == INetAddr::IPV4) {
            return true;
        }
    }
    return false;
}

bool NetLinkInfo::HasIpv4DnsServer() const
{
    for (const auto &addr: dnsList_) {
        if (addr.type_ == INetAddr::IPV4) {
            return true;
        }
    }
    return false;
}

std::string NetLinkInfo::ToString(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("[NetLinkInfo]");

    str.append(tab);
    str.append("ifaceName_ = ");
    str.append(ifaceName_);

    str.append(tab);
    str.append("domain_ = ");
    str.append(domain_);

    str.append(tab);
    str.append(ToStringAddr(tab));

    str.append(tab);
    str.append(ToStringDns(tab));

    str.append(tab);
    str.append(ToStringRoute(tab));
    str.append("routeList_ = ");

    str.append(tab);
    str.append("mtu_ = ");
    str.append(std::to_string(mtu_));

    str.append(tab);
    str.append("tcpBufferSizes_ = ");
    str.append(tcpBufferSizes_);

    str.append(tab);
    str.append("ident_ = ");
    str.append(ident_);

    str.append(tab);
    str.append("isUserDefinedDnsServer_ = ");
    if (isUserDefinedDnsServer_) {
        str.append("true");
    } else {
        str.append("false");
    }

    str.append(tab);
    str.append(ToStringLinkValid(tab));

    str.append(tab);
    str.append("httpProxy = ");
    str.append(httpProxy_.ToString());
    return str;
}

std::string NetLinkInfo::ToStringAddr(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("netAddrList_ = ");
    if (netAddrList_.empty()) {
        str.append("null");
        str.append(tab);
    } else {
        for (const auto &it : netAddrList_) {
            str.append(it.ToString(tab));
        }
    }
    return str;
}

std::string NetLinkInfo::ToStringDns(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("dnsList_ = ");
    if (dnsList_.empty()) {
        str.append("null");
        str.append(tab);
    } else {
        for (const auto &it : dnsList_) {
            str.append(it.ToString(tab));
        }
    }
    return str;
}

std::string NetLinkInfo::ToStringRoute(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("routeList_ = ");
    if (routeList_.empty()) {
        str.append("null");
        str.append(tab);
    } else {
        for (const auto &it : routeList_) {
            str.append(it.ToString(tab));
        }
    }
    return str;
}

std::string NetLinkInfo::ToStringLinkValid(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("isIpv4LinkValid_ = ");
    if (isIpv4LinkValid_) {
        str.append("true");
    } else {
        str.append("false");
    }

    str.append(tab);
    str.append("isIpv6LinkValid_ = ");
    if (isIpv6LinkValid_) {
        str.append("true");
    } else {
        str.append("false");
    }
    return str;
}
} // namespace NetManagerStandard
} // namespace OHOS

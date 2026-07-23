/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "net_mgr_log_wrapper.h"
#include "http_proxy.h"

#include <cstdint>
#include <cstdlib>
#include <sstream>

namespace OHOS {
namespace NetManagerStandard {
static const size_t MAX_EXCLUSION_SIZE = 3000;
static const size_t MAX_URL_SIZE = 2048;
static const size_t BASE_DEC = 10;

HttpProxy::HttpProxy() : port_(0), userId_(-1) {}

HttpProxy::HttpProxy(std::string host, uint16_t port, const std::list<std::string> &exclusionList)
    : port_(0), userId_(-1)
{
    if (host.size() <= MAX_URL_SIZE) {
        host_ = std::move(host);
        port_ = port;
        for (const auto &s : exclusionList) {
            if (s.size() <= MAX_URL_SIZE) {
                exclusionList_.push_back(s);
            }
            if (exclusionList_.size() >= MAX_EXCLUSION_SIZE) {
                break;
            }
        }
    } else {
        NETMGR_LOG_E("HttpProxy: host length is invalid");
    }
}

std::string HttpProxy::GetHost() const
{
    return host_;
}

uint16_t HttpProxy::GetPort() const
{
    return port_;
}

SecureData HttpProxy::GetUsername() const
{
    return username_;
}

SecureData HttpProxy::GetPassword() const
{
    return password_;
}

std::list<std::string> HttpProxy::GetExclusionList() const
{
    return exclusionList_;
}

int32_t HttpProxy::GetUserId() const
{
    return userId_;
}

bool HttpProxy::operator==(const HttpProxy &httpProxy) const
{
    return (host_ == httpProxy.host_ && port_ == httpProxy.port_ && exclusionList_ == httpProxy.exclusionList_ &&
            username_ == httpProxy.username_ && password_ == httpProxy.password_);
}

bool HttpProxy::operator!=(const HttpProxy &httpProxy) const
{
    return !(httpProxy == *this);
}

bool HttpProxy::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(host_)) {
        return false;
    }

    if (!parcel.WriteUint16(port_)) {
        return false;
    }

    if (!parcel.WriteInt32(userId_)) {
        return false;
    }

    if (!parcel.WriteUint32(static_cast<uint32_t>(std::min(MAX_EXCLUSION_SIZE, exclusionList_.size())))) {
        return false;
    }

    uint32_t size = 0;
    for (const auto &s : exclusionList_) {
        if (!parcel.WriteString(s)) {
            return false;
        }
        ++size;
        if (size >= MAX_EXCLUSION_SIZE) {
            break;
        }
    }
    parcel.WriteString(username_);
    parcel.WriteString(password_);
    return true;
}

bool HttpProxy::Unmarshalling(Parcel &parcel, HttpProxy &httpProxy)
{
    std::string host;
    if (!parcel.ReadString(host)) {
        return false;
    }
    if (host.size() > MAX_URL_SIZE) {
        NETMGR_LOG_E("HttpProxy: Unmarshalling: host length is invalid");
        return false;
    }

    uint16_t port = 0;
    if (!parcel.ReadUint16(port)) {
        return false;
    }

    int32_t userId = -1;
    if (!parcel.ReadInt32(userId)) {
        return false;
    }

    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }

    if (size > static_cast<uint32_t>(MAX_EXCLUSION_SIZE)) {
        size = MAX_EXCLUSION_SIZE;
    }

    std::list<std::string> exclusionList;
    for (uint32_t i = 0; i < size; ++i) {
        std::string s;
        if (!parcel.ReadString(s)) {
            return false;
        }
        if (s.size() <= MAX_URL_SIZE) {
            exclusionList.push_back(s);
        }
    }

    httpProxy = {host, port, exclusionList};
    httpProxy.SetUserId(userId);
    parcel.ReadString(httpProxy.username_);
    parcel.ReadString(httpProxy.password_);
    return true;
}

std::string HttpProxy::ToString() const
{
    std::string s;
    std::string tab = "\t";
    s.append(host_);
    s.append(tab);
    s.append(std::to_string(port_));
    s.append(tab);
    for (const auto &e : exclusionList_) {
        s.append(e);
        s.append(",");
    }
    return s;
}

struct Parser {
    Parser(std::string::const_iterator begin, std::string::const_iterator end) : begin(begin), end(end) {}

    static std::optional<uint16_t> ParsePort(const std::string &portStr)
    {
        char *strEnd = nullptr;
        auto port = std::strtol(portStr.c_str(), &strEnd, BASE_DEC);
        if (strEnd == portStr.c_str() || port < 0 || port > std::numeric_limits<uint16_t>::max()) {
            return std::nullopt;
        }
        return static_cast<uint16_t>(port);
    }

    static std::list<std::string> ParseProxyExclusionList(const std::string &exclusionList)
    {
        std::list<std::string> exclusionItems;
        std::stringstream ss(exclusionList);
        std::string item;

        while (std::getline(ss, item, ',')) {
            size_t start = item.find_first_not_of(" \t");
            size_t end = item.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                item = item.substr(start, end - start + 1);
            }
            exclusionItems.push_back(item);
        }
        return exclusionItems;
    }

    std::optional<std::string> GetHost()
    {
        if (auto hostEnd = std::find(begin, end, '\t'); hostEnd != end) {
            auto host = std::string(begin, hostEnd);
            begin = hostEnd + 1;
            return host;
        }
        return std::nullopt;
    }

    std::optional<uint16_t> GetPort()
    {
        if (auto portEnd = std::find(begin, end, '\t'); portEnd != end) {
            auto host = std::string(begin, portEnd);
            auto port = ParsePort(std::string(begin, portEnd));
            begin = portEnd + 1;
            return port;
        }
        return std::nullopt;
    }

    std::list<std::string> GetExclusionList()
    {
        if (begin != end) {
            auto list = ParseProxyExclusionList(std::string(begin, end));
            begin = end;
            return list;
        }
        return {};
    }

    std::string::const_iterator begin;
    std::string::const_iterator end;
};

std::optional<HttpProxy> HttpProxy::FromString(const std::string &str)
{
    Parser parser(str.cbegin(), str.cend());
    auto host = parser.GetHost();
    auto port = parser.GetPort();
    if (!host || !port) {
        return std::nullopt;
    }
    return NetManagerStandard::HttpProxy(*host, *port, parser.GetExclusionList());
}
} // namespace NetManagerStandard
} // namespace OHOS

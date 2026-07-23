/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bitmap_manager.h"

#include <arpa/inet.h>
#include <cstdio>
#include <netdb.h>
#include <net/if.h>
#include <securec.h>
#include <string>
#include <sys/socket.h>
#include <vector>

#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "netsys_net_dns_result_data.h"

using namespace OHOS::NetsysNative;

namespace OHOS {
namespace NetManagerStandard {
const uint32_t BITMAP_BIT_COUNT = (BITMAP_LEN * sizeof(uint32_t) * BIT_PER_BYTE);
static constexpr const uint32_t IPV4_MAX_VAL = 0xFFFFFFFF;

Bitmap::Bitmap()
{
    Clear();
}

Bitmap::Bitmap(uint32_t n)
{
    Clear();
    Set(n);
}

Bitmap::Bitmap(const Bitmap &other)
{
    memcpy_s(bitmap_, sizeof(bitmap_), other.bitmap_, sizeof(bitmap_));
}

void Bitmap::Clear()
{
    memset_s(bitmap_, sizeof(bitmap_), 0, sizeof(bitmap_));
}

const int BIT_OFFSET = 5;       // n/32
const int BIT_REMAINDER = 0x1f; // n%32
void Bitmap::Set(uint32_t n)
{
    if (n < BITMAP_BIT_COUNT) {
        int32_t i = n >> BIT_OFFSET;
        int32_t j = n & BIT_REMAINDER;
        bitmap_[i] |= 1 << j;
    }
}

uint64_t Bitmap::SpecialHash() const
{
    uint64_t h = 0;
    for (int32_t i = 0; i < BITMAP_LEN; i++) {
        if (bitmap_[i]) {
            h = GetHash(bitmap_[i]);
            h = h << i;
            return h;
        }
    }
    return h;
}

void Bitmap::And(const Bitmap &other)
{
    for (int32_t i = 0; i < BITMAP_LEN; i++) {
        bitmap_[i] &= other.bitmap_[i];
    }
}

void Bitmap::Or(const Bitmap &other)
{
    for (int32_t i = 0; i < BITMAP_LEN; i++) {
        bitmap_[i] |= other.bitmap_[i];
    }
}

bool Bitmap::operator == (const Bitmap &other) const
{
    return (memcmp(bitmap_, other.bitmap_, sizeof(bitmap_)) == 0);
}

Bitmap &Bitmap::operator = (const Bitmap &other)
{
    memcpy_s(bitmap_, sizeof(bitmap_), other.bitmap_, sizeof(bitmap_));
    return *this;
}

// Thomas Wang's 32 bit Mix Function
uint32_t Bitmap::GetHash(uint32_t key) const
{
    const int THOMAS_INDEX_0 = 15;
    const int THOMAS_INDEX_1 = 10;
    const int THOMAS_INDEX_2 = 3;
    const int THOMAS_INDEX_3 = 6;
    const int THOMAS_INDEX_4 = 11;
    const int THOMAS_INDEX_5 = 16;
    key += ~(key << THOMAS_INDEX_0);
    key ^= (key >> THOMAS_INDEX_1);
    key += (key << THOMAS_INDEX_2);
    key ^= (key >> THOMAS_INDEX_3);
    key += ~(key << THOMAS_INDEX_4);
    key ^= (key >> THOMAS_INDEX_5);
    return key;
}

uint32_t *Bitmap::Get()
{
    return bitmap_;
}

uint16_t BitmapManager::Hltons(uint32_t n)
{
    return htons((uint16_t)(n & 0x0000ffff));
}

uint16_t BitmapManager::Nstohl(uint16_t n)
{
    uint16_t m = 0;
    return m | ntohs(n);
}

int32_t BitmapManager::BuildBitmapMap(const std::vector<sptr<NetFirewallIpRule>> &ruleList)
{
    Clear();
    int32_t ret = BuildMarkBitmap(ruleList);
    if (ret != NETFIREWALL_SUCCESS) {
        return ret;
    }

    Insert();
    BuildNoMarkBitmap(ruleList);
    return NETFIREWALL_SUCCESS;
}

void BitmapManager::Insert()
{
    std::lock_guard<std::mutex> guard(mutex_);
    Bitmap bitmap;
    srcIp4Map_.OrInsert(OTHER_IP4_KEY, IPV4_MAX_PREFIXLEN, bitmap);
    dstIp4Map_.OrInsert(OTHER_IP4_KEY, IPV4_MAX_PREFIXLEN, bitmap);
    in6_addr otherIp6Key;
    memset_s(&otherIp6Key, sizeof(in6_addr), 0xff, sizeof(in6_addr));
    srcIp6Map_.OrInsert(otherIp6Key, IPV6_MAX_PREFIXLEN, bitmap);
    dstIp6Map_.OrInsert(otherIp6Key, IPV6_MAX_PREFIXLEN, bitmap);
    srcPortMap_.OrInsert(OTHER_PORT_KEY, PORT_MAX_MASK, bitmap);
    dstPortMap_.OrInsert(OTHER_PORT_KEY, PORT_MAX_MASK, bitmap);
    protoMap_.OrInsert(OTHER_PROTO_KEY, Bitmap());
    appUidMap_.OrInsert(OTHER_APPUID_KEY, Bitmap());
    uidMap_.OrInsert(OTHER_UID_KEY, Bitmap());
    action_key Key = 1;
    actionMap_.OrInsert(Key, Bitmap());
    interfaceMap_.OrInsert(std::string(""), Bitmap());
}

void BitmapManager::Clear()
{
    std::lock_guard<std::mutex> guard(mutex_);
    srcIp4Map_.Clear();
    srcIp6Map_.Clear();
    dstIp4Map_.Clear();
    dstIp6Map_.Clear();
    srcPortMap_.Clear();
    dstPortMap_.Clear();
    protoMap_.Clear();
    appUidMap_.Clear();
    uidMap_.Clear();
    actionMap_.Clear();
    interfaceMap_.Clear();
}

int32_t BitmapManager::InsertIp4SegBitmap(const NetFirewallIpParam &item, Bitmap &bitmap, Ip4RuleMap *ip4Map)
{
    if (ip4Map == nullptr) {
        return NETFIREWALL_ERR;
    }
    if (item.type == SINGLE_IP) {
        uint32_t ipInt = item.ipv4.startIp.s_addr;
        ip4Map->OrInsert(ntohl(ipInt), static_cast<uint32_t>(item.mask), bitmap);
        NETNATIVE_LOG_D("InsertIpBitmap ipp[%{public}u] ipn[%{public}s] mask[%{public}u]", ipInt,
            item.GetStartIp().c_str(), item.mask);
    } else if (item.type == MULTIPLE_IP) {
        std::vector<Ip4Data> ips;
        int32_t ret = IpParamParser::GetIp4AndMask(item.ipv4.startIp, item.ipv4.endIp, ips);
        if (ret != NETFIREWALL_SUCCESS) {
            return ret;
        }
        for (auto &ipData : ips) {
            ip4Map->OrInsert(ipData.data, ipData.mask, bitmap);
            NETNATIVE_LOG_D("InsertIpBitmap ip[%{public}u], mask[%{public}u]", htonl(ipData.data), ipData.mask);
        }
    }
    return NETFIREWALL_SUCCESS;
}


int32_t BitmapManager::InsertIp6SegBitmap(const NetFirewallIpParam &item, Bitmap &bitmap, Ip6RuleMap *ip6Map)
{
    if (ip6Map == nullptr) {
        return NETFIREWALL_ERR;
    }
    if (item.type == SINGLE_IP) {
        ip6Map->OrInsert(item.ipv6.startIp, static_cast<uint32_t>(item.mask), bitmap);
        std::string addrStr = IpParamParser::Addr6ToStr(item.ipv6.startIp);
        NETNATIVE_LOG_D("InsertIp6SegBitmap ip[%{public}s], mask[%{public}u]",
            CommonUtils::ToAnonymousIp(addrStr).c_str(), item.mask);
    } else if (item.type == MULTIPLE_IP) {
        std::vector<Ip6Data> ips;
        int32_t ret = IpParamParser::GetIp6AndMask(item.ipv6.startIp, item.ipv6.endIp, ips);
        if (ret != NETFIREWALL_SUCCESS) {
            NETNATIVE_LOGW("InsertIp6SegBitmap GetIp6AndMask fail ret=%{public}d", ret);
            return ret;
        }
        for (auto &ipData : ips) {
            ip6Map->OrInsert(ipData.data, ipData.prefixlen, bitmap);
        }
    }
    return NETFIREWALL_SUCCESS;
}

int32_t BitmapManager::InsertIpBitmap(const std::vector<NetFirewallIpParam> &ipInfo, bool isSrc, Bitmap &bitmap)
{
    for (const NetFirewallIpParam &item : ipInfo) {
        Ip4RuleMap *ip4Map = nullptr;
        Ip6RuleMap *ip6Map = nullptr;
        if (isSrc) {
            if (item.family == FAMILY_IPV4) {
                ip4Map = &srcIp4Map_;
            } else if (item.family == FAMILY_IPV6) {
                ip6Map = &srcIp6Map_;
            }
        } else {
            if (item.family == FAMILY_IPV4) {
                ip4Map = &dstIp4Map_;
            } else if (item.family == FAMILY_IPV6) {
                ip6Map = &dstIp6Map_;
            }
        }

        int32_t ret = NETFIREWALL_SUCCESS;
        if (item.family == FAMILY_IPV4) {
            int32_t ret = InsertIp4SegBitmap(item, bitmap, ip4Map);
            if (ret != NETFIREWALL_SUCCESS) {
                return ret;
            }
        } else {
            ret = InsertIp6SegBitmap(item, bitmap, ip6Map);
            if (ret != NETFIREWALL_SUCCESS) {
                return ret;
            }
        }
    }
    return NETFIREWALL_SUCCESS;
}

void BitmapManager::OrInsertPortBitmap(SegmentBitmapMap &portSegMap, BpfPortMap &portMap)
{
    auto &segMap = portSegMap.GetMap();
    for (auto &item : segMap) {
        uint16_t keyStart = item.start;
        uint16_t keyEnd = item.end;
        NETNATIVE_LOG_D("Segment: start=%{public}u(0x%{public}04X), end=%{public}u(0x%{public}04X)",
            keyStart, keyStart, keyEnd, keyEnd);
        std::vector<portRule> portRules;
        int32_t ret = GetPortRangeAndMask(keyStart, keyEnd, portRules);
        if (ret != 0) {
            continue;
        }

        for (auto &rule : portRules) {
            if (rule.prefixlen > PORT_MAX_MASK) {
                continue;
            }
            uint16_t netOrderPort = htons(rule.data);
            NETNATIVE_LOG_D("OrInsert: port=0x%{public}04X(%{public}u)/%{public}u",
                            netOrderPort, ntohs(netOrderPort), rule.prefixlen);
            portMap.OrInsert(netOrderPort, static_cast<uint32_t>(rule.prefixlen), item.bitmap);
        }
    }
}

int32_t BitmapManager::GetPortRangeAndMask(uint16_t startPort, uint16_t endPort, std::vector<portRule> &list)
{
    if (startPort > endPort) {
        return -1;
    }
    uint32_t start = static_cast<uint32_t>(startPort);
    uint32_t end = static_cast<uint32_t>(endPort);
    while (start <= end) {
        uint32_t suffixZeros = 0;
        uint32_t temp = start;
        while (temp > 0 && (temp & 1) == 0) {
            suffixZeros++;
            temp >>= 1;
        }

        uint32_t maxBlock = 1U << suffixZeros;
        uint32_t remaining = end - start + 1;
        uint32_t blockSize = maxBlock;
        while (blockSize > remaining) {
            blockSize >>= 1;
        }

        uint32_t mask = PORT_MAX_MASK;
        uint32_t tempSize = blockSize;
        while (tempSize > 1) {
            mask--;
            tempSize >>= 1;
        }

        portRule rule;
        rule.data = static_cast<uint16_t>(start);
        rule.prefixlen = static_cast<uint16_t>(mask);
        list.push_back(rule);

        uint16_t blockEnd = rule.data + blockSize - 1;

        start += blockSize;
    }
    return 0;
}

void BitmapManager::AddPortBitmap(const std::vector<NetFirewallPortParam> &port, Bitmap &bitmap,
    SegmentBitmapMap &portMap)
{
    for (const NetFirewallPortParam &item : port) {
        uint16_t startPort = item.startPort;
        if (startPort == 0) {
            continue;
        }
        if (item.startPort <= item.endPort) {
            portMap.AddMap(item.startPort, item.endPort, bitmap);
        }
    }
}

int32_t BitmapManager::BuildMarkBitmap(const std::vector<sptr<NetFirewallIpRule>> &ruleList)
{
    SegmentBitmapMap srcPortMap;
    SegmentBitmapMap dstPortMap;
    uint32_t index = 0;
    int32_t ret;
    for (const auto &rule : ruleList) {
        Bitmap bitmap(index);
        ret = InsertIpBitmap(rule->remoteIps, true, bitmap);
        if (ret) {
            NETNATIVE_LOGW("BuildMarkBitmap InsertIpBitmap remoteIps fail ret=%{public}d", ret);
            return NETFIREWALL_ERR;
        }
        ret = InsertIpBitmap(rule->localIps, false, bitmap);
        if (ret) {
            NETNATIVE_LOGW("BuildMarkBitmap InsertIpBitmap localIps fail ret=%{public}d", ret);
            return NETFIREWALL_ERR;
        }
        if (!IsNotNeedPort(rule->protocol)) {
            AddPortBitmap(rule->remotePorts, bitmap, srcPortMap);
            AddPortBitmap(rule->localPorts, bitmap, dstPortMap);
        }

        if (rule->protocol != (NetworkProtocol)0) {
            ProtoKey proto = (ProtoKey)rule->protocol;
            protoMap_.OrInsert(proto, bitmap);
        }

        if (rule->appUid > 0) {
            appUidMap_.OrInsert((AppUidKey)rule->appUid, bitmap);
        }

        if (rule->userId > 0) {
            uidMap_.OrInsert((UidKey)rule->userId, bitmap);
        }

        if (rule->ruleAction != FirewallRuleAction::RULE_ALLOW) {
            action_key Key = 1;
            actionMap_.OrInsert(Key, bitmap);
        }

        if (!rule->interface.empty()) {
            interfaceMap_.OrInsert(rule->interface, bitmap);
        }
        index++;
    }

    OrInsertPortBitmap(srcPortMap, srcPortMap_);
    OrInsertPortBitmap(dstPortMap, dstPortMap_);
    return NETFIREWALL_SUCCESS;
}

void BitmapManager::BuildNoMarkBitmap(const std::vector<sptr<NetFirewallIpRule>> &ruleList)
{
    uint32_t index = 0;
    for (const auto &rule : ruleList) {
        Bitmap bitmap(index);
        if (rule->remoteIps.empty()) {
            srcIp4Map_.OrForEach(bitmap);
            srcIp6Map_.OrForEach(bitmap);
        }
        if (rule->localIps.empty()) {
            dstIp4Map_.OrForEach(bitmap);
            dstIp6Map_.OrForEach(bitmap);
        }
        if (rule->remotePorts.empty() || IsNotNeedPort(rule->protocol)) {
            srcPortMap_.OrForEach(bitmap);
        }
        if (rule->localPorts.empty() || IsNotNeedPort(rule->protocol)) {
            dstPortMap_.OrForEach(bitmap);
        }
        if (rule->protocol == (NetworkProtocol)0) {
            protoMap_.OrForEach(bitmap);
        }
        if (rule->appUid < 1) {
            appUidMap_.OrForEach(bitmap);
        }
        if (rule->userId < 1) {
            uidMap_.OrForEach(bitmap);
        }
        if (rule->interface.empty()) {
            interfaceMap_.OrForEach(bitmap);
        }
        index++;
    }
}

bool BitmapManager::IsNotNeedPort(NetworkProtocol protocol)
{
    if (protocol == NetworkProtocol::ICMPV6 || protocol == NetworkProtocol::ICMP) {
        return true;
    }
    return false;
}

int32_t IpParamParser::GetIpUint32(const std::string &address, uint32_t &ipInt)
{
    in_addr out = { 0 };
    memset_s(&out, sizeof(out), 0, sizeof(out));
    if (inet_pton(AF_INET, address.c_str(), &out) != 1) {
        return NETFIREWALL_IP_STR_ERR;
    }

    ipInt = out.s_addr;
    return NETFIREWALL_SUCCESS;
}

void IpParamParser::AddIp(uint32_t ip, uint32_t mask, std::vector<Ip4Data> &ip4Vec)
{
    Ip4Data info;
    info.data = ip;
    info.mask = mask;
    ip4Vec.emplace_back(info);
}

std::string IpParamParser::Ip4ToStr(uint32_t ip)
{
    char str[INET_ADDRSTRLEN] = {0};
    in_addr ipv4;
    memset_s(&ipv4, sizeof(ipv4), 0, sizeof(ipv4));
    ipv4.s_addr = ntohl(ip);

    if (inet_ntop(AF_INET, &ipv4, str, INET_ADDRSTRLEN) == NULL) {
        return "error ip";
    }
    return std::string(str);
}

uint32_t IpParamParser::GetCidrBlockBits(uint32_t cidrSize)
{
    uint32_t count = 0;
    while (cidrSize >>= 1) {
        ++count;
    }
    return count;
}
 
uint32_t IpParamParser::GetSuffixZeroLength(uint32_t ip)
{
    if (ip == 0) {
        return IPV4_BIT_COUNT;
    }
    uint32_t count = 0;
    while ((ip & 1) == 0) {
        ++count;
        ip >>= 1;
    }
    return count;
}

int32_t IpParamParser::GetIp4AndMask(const in_addr &startAddr, const in_addr &endAddr, std::vector<Ip4Data> &list)
{
    uint32_t startIpInt = ntohl(startAddr.s_addr);
    uint32_t endIpInt = ntohl(endAddr.s_addr);
    if (startIpInt > endIpInt) {
        return NETFIREWALL_ERR;
    }
    uint32_t suffixZeros = 0;
    uint32_t maxCidrSize = 0;
    uint32_t remainingCidrSize = 0;
    uint32_t cidrBits = 0;
    while (startIpInt <= endIpInt) {
        suffixZeros = GetSuffixZeroLength(startIpInt);
        maxCidrSize = (1 << suffixZeros);
        remainingCidrSize = endIpInt - startIpInt + 1;
        cidrBits = GetCidrBlockBits(std::min(maxCidrSize, remainingCidrSize));
        AddIp(startIpInt, IPV4_BIT_COUNT - cidrBits, list);
        startIpInt += (1 << cidrBits);
    }
    return NETFIREWALL_SUCCESS;
}

int32_t IpParamParser::GetInAddr6(const std::string &ipStr, in6_addr &addr)
{
    int32_t ret = inet_pton(AF_INET6, ipStr.c_str(), &addr);
    if (ret <= 0) {
        return NETFIREWALL_IP_STR_ERR;
    }
    return NETFIREWALL_SUCCESS;
}

std::string IpParamParser::Addr6ToStr(const in6_addr &v6Addr)
{
    char buf[INET6_ADDRSTRLEN] = { 0 };
    if (inet_ntop(AF_INET6, &v6Addr, buf, INET6_ADDRSTRLEN) == NULL) {
        return "error in6_addr";
    }
    return std::string(buf);
}

uint32_t IpParamParser::GetIp6Prefixlen(const in6_addr &start, const in6_addr &end)
{
    uint32_t prefixlen = IPV6_MAX_PREFIXLEN;
    for (uint32_t i = 0; i < IPV6_BYTE_COUNT; i++) {
        if (start.s6_addr[i] == end.s6_addr[i]) {
            continue;
        }
        for (int32_t j = static_cast<int32_t>(BIT_PER_BYTE - 1); j >= 0; --j) {
            uint8_t byte = (1 << j);
            if ((start.s6_addr[i] & byte) == (end.s6_addr[i] & byte)) {
                continue;
            } else {
                prefixlen = i * BIT_PER_BYTE + BIT_PER_BYTE - static_cast<uint32_t>(j) - 1;
                return prefixlen;
            }
        }
    }
    return prefixlen;
}

uint32_t IpParamParser::RfindIp6(const in6_addr &addr, uint32_t startBit, uint32_t endBit, uint8_t value)
{
    if (startBit > endBit) {
        return IPV6_BIT_COUNT;
    }
    uint32_t startBits = startBit % BIT_PER_BYTE;
    uint32_t startBytes = startBit / BIT_PER_BYTE;
    uint32_t endBits = endBit % BIT_PER_BYTE;
    uint32_t endBytes = endBit / BIT_PER_BYTE;
    for (uint32_t i = endBytes; i >= startBytes; --i) {
        uint32_t j = (i == endBytes) ? (BIT_PER_BYTE - endBits - 1) : 0;
        uint32_t k = (i == startBytes) ? (BIT_PER_BYTE - startBits - 1) : (BIT_PER_BYTE - 1);
        for (; j <= k; ++j) {
            uint8_t byte = (1 << j);
            uint8_t tmp = (addr.s6_addr[i] & byte) >> j;
            if (tmp == value) {
                return i * BIT_PER_BYTE + BIT_PER_BYTE - j - 1;
            }
        }
    }
    return IPV6_BIT_COUNT;
}

uint32_t IpParamParser::FindIp6(const in6_addr &addr, uint32_t startBit, uint8_t value)
{
    if (startBit >= IPV6_BIT_COUNT) {
        return IPV6_BIT_COUNT;
    }
    uint32_t startBits = startBit % BIT_PER_BYTE;
    uint32_t startBytes = startBit / BIT_PER_BYTE;
    for (uint32_t i = startBytes; i < IPV6_BYTE_COUNT; ++i) {
        int32_t j = static_cast<int32_t>((i == startBytes) ? (BIT_PER_BYTE - startBits - 1) : (BIT_PER_BYTE - 1));
        for (; j >= 0; --j) {
            uint8_t tmp = ((addr.s6_addr[i] >> j) & VALUE_ONE);
            if (tmp == value) {
                return i * BIT_PER_BYTE + BIT_PER_BYTE - static_cast<uint32_t>(j) - 1;
            }
        }
    }
    return IPV6_BIT_COUNT;
}

void IpParamParser::AddIp6(const in6_addr &addr, uint32_t prefixlen, std::vector<Ip6Data> &list)
{
    Ip6Data info;
    info.prefixlen = prefixlen;
    info.data = addr;
    list.emplace_back(info);

    std::string startIpStr = IpParamParser::Addr6ToStr(info.data);
    NETNATIVE_LOG_D("AddIp6 ip[%{public}s], mask[%{public}u]",
        CommonUtils::ToAnonymousIp(startIpStr).c_str(), info.prefixlen);
}

void IpParamParser::ChangeIp6Start(uint32_t startBit, in6_addr &addr)
{
    uint32_t bits = startBit % BIT_PER_BYTE;
    uint32_t bytes = startBit / BIT_PER_BYTE;
    if (bytes < IPV6_BYTE_COUNT - 1) {
        memset_s(addr.s6_addr + bytes + 1, IPV6_BYTE_COUNT - bytes - 1, 0, IPV6_BYTE_COUNT - bytes - 1);
    }

    bool needSetZero = true;
    if (bytes >= IPV6_BYTE_COUNT) {
        bytes = IPV6_BYTE_COUNT - 1;
        bits = BIT_PER_BYTE - 1;
        needSetZero = false;
    }

    uint32_t off = BIT_PER_BYTE - bits - 1;
    for (int32_t i = static_cast<int32_t>(bytes); i >= 0; --i) {
        for (uint32_t j = 0; j < BIT_PER_BYTE; ++j) {
            uint8_t byte = (1 << j);
            if (needSetZero && (i == static_cast<int32_t>(bytes) && j <= off)) {
                addr.s6_addr[i] &= (~byte);
                continue;
            }
            if (addr.s6_addr[i] & byte) {
                addr.s6_addr[i] &= (~byte);
                continue;
            } else {
                addr.s6_addr[i] |= byte;
                return;
            }
        }
    }
}

int32_t IpParamParser::GetIp6AndMask(const in6_addr &addr6Start, const in6_addr &addr6End, std::vector<Ip6Data> &list)
{
    int32_t ret = memcmp(&addr6Start, &addr6End, sizeof(addr6Start));
    if (ret > 0) {
        NETNATIVE_LOGW("GetIp6AndMask fail ret=%{public}d", ret);
        return NETFIREWALL_ERR;
    } else if (ret == 0) {
        AddIp6(addr6Start, IPV6_MAX_PREFIXLEN, list);
        return NETFIREWALL_SUCCESS;
    }
    uint32_t prefixlen = GetIp6Prefixlen(addr6Start, addr6End);
    in6_addr tmpAddr;
    memcpy_s(&tmpAddr, sizeof(tmpAddr), &addr6Start, sizeof(addr6Start));
    uint32_t off = RfindIp6(addr6Start, prefixlen, IPV6_BIT_COUNT - 1, 1);
    if (off != IPV6_BIT_COUNT) {
        AddIp6(tmpAddr, off + 1, list);
        ChangeIp6Start(off + 1, tmpAddr);
        off = RfindIp6(addr6Start, prefixlen, off, 0);
        while (off != IPV6_BIT_COUNT && off != prefixlen) {
            AddIp6(tmpAddr, off + 1, list);
            ChangeIp6Start(off + 1, tmpAddr);
            off = RfindIp6(addr6Start, prefixlen, off - 1, 0);
        }
    } else if (off == IPV6_BIT_COUNT) {
        if (RfindIp6(addr6End, prefixlen, IPV6_BIT_COUNT - 1, 0) == IPV6_BIT_COUNT) {
            AddIp6(addr6Start, prefixlen, list);
            return NETFIREWALL_SUCCESS;
        }
    }
    off = FindIp6(addr6End, prefixlen, 1);
    if (off == IPV6_BIT_COUNT) {
        NETNATIVE_LOGW("GetIp6AndMask off equal 128");
        return NETFIREWALL_ERR;
    }
    off = FindIp6(addr6End, off + 1, 1);
    while (off != IPV6_BIT_COUNT) {
        AddIp6(tmpAddr, off + 1, list);
        ChangeIp6Start(off + 1, tmpAddr);
        off = FindIp6(addr6End, off + 1, 1);
    }
    ret = memcmp(&tmpAddr, &addr6End, sizeof(addr6Start));
    if (ret == 0) {
        AddIp6(tmpAddr, IPV6_MAX_PREFIXLEN, list);
    }
    return NETFIREWALL_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
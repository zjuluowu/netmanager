/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "dns_resolv_config.h"

#include "netnative_log_wrapper.h"

namespace OHOS::nmd {
DnsResolvConfig::DelayedTaskWrapper::DelayedTaskWrapper(std::string hostName,
                                                        NetManagerStandard::LRUCache<AddrInfoWithTtl> &cache)
    : hostName_(std::move(hostName)), cache_(cache)
{
}

void DnsResolvConfig::DelayedTaskWrapper::Execute() const
{
    std::vector<AddrInfoWithTtl> infos = cache_.Get(hostName_);
    cache_.Delete(hostName_);
    if (infos.size() == 0) {
        return;
    }

    for (auto info : infos) {
        if (info.ttl > remainTime_) {
            info.ttl -= remainTime_;
            cache_.Put(hostName_, info);
        }
    }
}

uint64_t DnsResolvConfig::GetNowMs()
{
    struct timespec ts;
    return (clock_gettime(CLOCK_REALTIME, &ts) == 0)
               ? static_cast<uint64_t>(ts.tv_sec) * MILLIS_PER_SEC + static_cast<uint64_t>(ts.tv_nsec) / NANOS_PER_MILLI
               : static_cast<uint64_t>(time(nullptr)) * MILLIS_PER_SEC;
}

uint32_t DnsResolvConfig::DelayedTaskWrapper::GetUpdateTime()
{
    std::vector<AddrInfoWithTtl> infos = cache_.Get(hostName_);
    if (infos.size() == 0) {
        return 0;
    }
    remainTime_ = infos[0].ttl;
    for (auto info : infos) {
        remainTime_ = remainTime_ < info.ttl ? remainTime_ : info.ttl;
    }
    return remainTime_;
}

bool DnsResolvConfig::DelayedTaskWrapper::operator<(const DelayedTaskWrapper &other) const
{
    return hostName_ < other.hostName_;
}

DnsResolvConfig::DnsResolvConfig()
    : netId_(0), netIdIsSet_(false), revisionId_(0), timeoutMsec_(0), retryCount_(0), isIpv6Enable_(false),
    isIpv4Enable_(false), isUserDefinedDnsServer_(false), isClatIpv4Enable_(false)
{
}

void DnsResolvConfig::EnableIpv6(bool enable)
{
    isIpv6Enable_ = enable;
}

bool DnsResolvConfig::IsIpv6Enable()
{
    return isIpv6Enable_;
}

void DnsResolvConfig::EnableIpv4()
{
    isIpv4Enable_ = true;
}

void DnsResolvConfig::SetClatDnsEnableIpv4(bool enable)
{
    isClatIpv4Enable_ = enable;
}

bool DnsResolvConfig::IsClatIpv4Enable()
{
    return isClatIpv4Enable_;
}

bool DnsResolvConfig::IsIpv4Enable()
{
    return isIpv4Enable_;
}

void DnsResolvConfig::SetNetId(uint16_t netId)
{
    if (netIdIsSet_) {
        return;
    }
    netIdIsSet_ = true;
    netId_ = netId;
}

uint16_t DnsResolvConfig::GetNetId() const
{
    return netId_;
}

void DnsResolvConfig::SetTimeoutMsec(int32_t baseTimeoutMsec)
{
    timeoutMsec_ = baseTimeoutMsec;
}

uint16_t DnsResolvConfig::GetTimeoutMsec() const
{
    return timeoutMsec_;
}

void DnsResolvConfig::SetRetryCount(uint8_t retryCount)
{
    retryCount_ = retryCount;
}

uint8_t DnsResolvConfig::GetRetryCount() const
{
    return retryCount_;
}

void DnsResolvConfig::SetServers(const std::vector<std::string> &servers)
{
    nameServers_ = servers;
    revisionId_++;
}

std::vector<std::string> DnsResolvConfig::GetServers() const
{
    return nameServers_;
}

void DnsResolvConfig::SetDomains(const std::vector<std::string> &domains)
{
    searchDomains_ = domains;
}

std::vector<std::string> DnsResolvConfig::GetDomains() const
{
    return searchDomains_;
}

NetManagerStandard::LRUCache<AddrInfoWithTtl> &DnsResolvConfig::GetCache()
{
    return cache_;
}

void DnsResolvConfig::SetCacheDelayed(const std::string &hostName)
{
    auto wrapper = std::make_shared<DelayedTaskWrapper>(hostName, cache_);
    uint32_t time = wrapper->GetUpdateTime();
    if (time == 0) {
        return;
    }
    delayedQueue_.Put(wrapper, time);
}

void DnsResolvConfig::SetUserDefinedServerFlag(bool flag)
{
    isUserDefinedDnsServer_ = flag;
}

bool DnsResolvConfig::IsUserDefinedServer()
{
    return isUserDefinedDnsServer_;
}

uint64_t DnsResolvConfig::HashHostName(const std::string &hostName) const
{
    // Simple hash function for hostName to reduce memory usage
    // Using std::hash which is efficient and provides good distribution
    return std::hash<std::string>{}(hostName);
}

void DnsResolvConfig::SetNodataCache(const std::string &hostName)
{
    if (!IsIpv4Enable()) {
        return;
    }
    uint64_t now = GetNowMs();
    uint64_t hashKey = HashHostName(hostName);

    // Check if entry already exists and update it
    for (auto &entry : nodataCache_) {
        if (entry.first == hashKey) {
            entry.second = now;
            return;
        }
    }

    // Limit nodata cache to MAX_NODATA_CACHE_SIZE entries
    if (nodataCache_.size() >= MAX_NODATA_CACHE_SIZE) {
        // Find and remove the oldest entry
        uint64_t oldestTime = UINT64_MAX;
        size_t oldestIdx = 0;
        for (size_t i = 0; i < nodataCache_.size(); ++i) {
            if (nodataCache_[i].second < oldestTime) {
                oldestTime = nodataCache_[i].second;
                oldestIdx = i;
            }
        }
        nodataCache_.erase(nodataCache_.begin() + oldestIdx);
    }
    nodataCache_.emplace_back(hashKey, now);
}

bool DnsResolvConfig::IsInNodataCache(const std::string &hostName)
{
    uint64_t hashKey = HashHostName(hostName);
    uint64_t now = GetNowMs();

    for (auto it = nodataCache_.begin(); it != nodataCache_.end(); ++it) {
        if (it->first == hashKey) {
            uint64_t diff = now - it->second;
            if (diff > DEFAULT_AAAA_BLACK) {
                nodataCache_.erase(it);
                return false;
            }
            return true;
        }
    }
    return false;
}

void DnsResolvConfig::ClearNodataCache()
{
    nodataCache_.clear();
    nodataCache_.shrink_to_fit();  // Release unused memory
}

void DnsResolvConfig::SetIpv6UidBlackList(const uint32_t &uid)
{
    if (!IsIpv4Enable()) {
        return;
    }
    uint64_t now = GetNowMs();
 
    // Check if uid already exists and update it
    auto it = ipv6UidBlackList_.find(uid);
    if (it != ipv6UidBlackList_.end()) {
        ipv6UidBlackList_[uid] = now;
        NETNATIVE_LOGI("SetIpv6UidBlackList uid:%{public}u is exists", uid);
        return;
    }
 
    // Limit ipv6 uid black list to MAX_IPV6_UID_BLACK_LIST_SIZE
    if (ipv6UidBlackList_.size() >= MAX_IPV6_UID_BLACK_LIST_SIZE) {
        // Find and remove the oldest uid
        uint64_t oldestTime = UINT64_MAX;
        uint32_t oldestUid = 0;
        for (const auto& it : ipv6UidBlackList_) {
            if (it.second < oldestTime) {
                oldestTime = it.second;
                oldestUid = it.first;
            }
        }
        ipv6UidBlackList_.erase(oldestUid);
        NETNATIVE_LOGI("SetIpv6UidBlackList erase ipv6 black list of the oldest uid:%{public}u", oldestUid);
    }
    ipv6UidBlackList_.insert({uid, now});
    NETNATIVE_LOGI("SetIpv6UidBlackList uid:%{public}u", uid);
}
 
bool DnsResolvConfig::IsInIpv6UidBlackList(const uint32_t &uid)
{
    uint64_t now = GetNowMs();
 
    for (const auto& it : ipv6UidBlackList_) {
        if (it.first == uid) {
            uint64_t diff = now - it.second;
            if (diff > AAAA_SKIP_DURATION) {
                ipv6UidBlackList_.erase(it.first);
                NETNATIVE_LOG_D("skip AAAA time out, remove uid:%{public}u from ipv6UidBlackList", uid);
                return false;
            }
            return true;
        }
    }
    return false;
}
 
void DnsResolvConfig::ClearIpv6UidBlackList()
{
    ipv6UidBlackList_.clear();
}
} // namespace OHOS::nmd

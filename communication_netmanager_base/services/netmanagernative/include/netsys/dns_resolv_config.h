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

#ifndef INCLUDE_DNSRESOLV_CONFIG_H
#define INCLUDE_DNSRESOLV_CONFIG_H

#include <atomic>
#include <list>
#include <memory>
#include <vector>
#include <map>

#include "delayed_queue.h"
#include "dns_config_client.h"
#include "lru_cache.h"

namespace OHOS::nmd {

static constexpr size_t MAX_NODATA_CACHE_SIZE = 100;
static constexpr size_t MAX_IPV6_UID_BLACK_LIST_SIZE = 32;
static constexpr uint64_t MILLIS_PER_SEC = 1000ULL;
static constexpr uint64_t NANOS_PER_MILLI = 1000000ULL;

class DnsResolvConfig {
public:
    DnsResolvConfig();

    void SetNetId(uint16_t netId);
    void SetTimeoutMsec(int32_t baseTimeoutMsec);
    void SetRetryCount(uint8_t retryCount);
    void SetServers(const std::vector<std::string> &servers);
    void SetDomains(const std::vector<std::string> &domains);

    uint16_t GetNetId() const;
    uint16_t GetTimeoutMsec() const;
    std::vector<std::string> GetServers() const;
    std::vector<std::string> GetDomains() const;
    uint8_t GetRetryCount() const;

    NetManagerStandard::LRUCache<AddrInfoWithTtl> &GetCache();

    void SetCacheDelayed(const std::string &hostName);

    bool IsIpv6Enable();

    void EnableIpv6(bool enable = true);

    bool IsIpv4Enable();

    void EnableIpv4();

    void SetUserDefinedServerFlag(bool flag);

    bool IsUserDefinedServer();
    void SetClatDnsEnableIpv4(bool enable);
    bool IsClatIpv4Enable();

    // NODATA cache methods
    void SetNodataCache(const std::string &hostName);
    bool IsInNodataCache(const std::string &hostName);
    void ClearNodataCache();
    uint64_t GetNowMs();

    // ipv6 uid black list methods
    void SetIpv6UidBlackList(const uint32_t &uid);
    bool IsInIpv6UidBlackList(const uint32_t &uid);
    void ClearIpv6UidBlackList();

private:
    uint64_t HashHostName(const std::string &hostName) const;

private:
    class DelayedTaskWrapper {
    public:
        DelayedTaskWrapper(std::string hostName, NetManagerStandard::LRUCache<AddrInfoWithTtl> &cache);

        void Execute() const;

        uint32_t GetUpdateTime();

        bool operator<(const DelayedTaskWrapper &other) const;

    private:
        std::string hostName_;

        uint32_t remainTime_ = 0;

        NetManagerStandard::LRUCache<AddrInfoWithTtl> &cache_;
    };

    uint16_t netId_;
    std::atomic_bool netIdIsSet_;
    int32_t revisionId_;
    int32_t timeoutMsec_;
    uint8_t retryCount_;
    std::list<std::shared_ptr<DelayedTaskWrapper>> delayedTaskWrapperList_;
    std::vector<std::string> nameServers_;
    std::vector<std::string> searchDomains_;
    NetManagerStandard::LRUCache<AddrInfoWithTtl> cache_;
    NetManagerStandard::DelayedQueue<DelayedTaskWrapper, NetManagerStandard::DEFAULT_CAPABILITY, DEFAULT_DELAYED_COUNT>
        delayedQueue_;
    bool isIpv6Enable_;
    bool isIpv4Enable_;
    bool isUserDefinedDnsServer_;
    bool isClatIpv4Enable_;

    // NODATA cache: using vector for memory efficiency (100 elements max)
    // hash(hostName) -> timestamp in milliseconds (uint64_t)
    std::vector<std::pair<uint64_t, uint64_t>> nodataCache_;

    // ipv6 uid black list: using map for memory efficiency (32 elements max)
    // <uint32_t uid, uint64_t timestamp>
    std::map<uint32_t, uint64_t> ipv6UidBlackList_;
};
} // namespace OHOS::nmd
#endif // INCLUDE_DNSRESOLV_CONFIG_H

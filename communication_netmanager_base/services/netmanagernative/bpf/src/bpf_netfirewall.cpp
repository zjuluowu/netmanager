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

#include <arpa/inet.h>
#include <cstdio>
#include <ctime>
#include <libbpf.h>
#include <linux/bpf.h>
#include <net/if.h>
#include <netinet/in.h>
#include <regex>
#include <securec.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <vector>

#include "bpf_loader.h"
#include "bpf_netfirewall.h"
#include "bpf_ring_buffer.h"
#include "ffrt_inner.h"
#include "iservice_registry.h"
#include "netnative_log_wrapper.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::NetsysNative;

namespace OHOS {
namespace NetManagerStandard {
std::shared_ptr<NetsysBpfNetFirewall> NetsysBpfNetFirewall::instance_ = nullptr;
bool NetsysBpfNetFirewall::keepListen_ = false;
bool NetsysBpfNetFirewall::keepGc_ = false;
bool NetsysBpfNetFirewall::isBpfLoaded_ = false;
std::unique_ptr<BpfMapper<CtKey, CtVaule>> NetsysBpfNetFirewall::ctRdMap_ = nullptr;
std::unique_ptr<BpfMapper<CtKey, CtVaule>> NetsysBpfNetFirewall::ctWrMap_ = nullptr;

NetsysBpfNetFirewall::NetsysBpfNetFirewall()
{
    NETNATIVE_LOG_D("NetsysBpfNetFirewall construct");
    isBpfLoaded_ = false;
}

std::shared_ptr<NetsysBpfNetFirewall> NetsysBpfNetFirewall::GetInstance()
{
    static std::mutex instanceMutex;
    std::lock_guard<std::mutex> guard(instanceMutex);
    if (instance_ == nullptr) {
        instance_.reset(new NetsysBpfNetFirewall());
        return instance_;
    }
    return instance_;
}

void NetsysBpfNetFirewall::ConntrackGcTask()
{
    NETNATIVE_LOG_D("ConntrackGcTask: running");
    std::vector<CtKey> keys = ctRdMap_->GetAllKeys();
    if (keys.empty()) {
        NETNATIVE_LOG_D("GcConntrackCb: key is empty");
        return;
    }

    timespec now = { 0 };
    // bpf_ktime_get_ns: CLOCK_MONOTONIC
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        NETNATIVE_LOGE("ConntrackGcTask: clock_gettime failed");
        return;
    }
    for (const CtKey &k : keys) {
        CtVaule v = {};
        if (ctRdMap_->Read(k, v) < 0) {
            NETNATIVE_LOGE("GcConntrackCb: read failed");
            continue;
        }

        if (v.lifetime < now.tv_sec) {
            if (ctWrMap_->Delete(k) != 0) {
                NETNATIVE_LOGE("GcConntrackCb: delete failed");
                continue;
            }
        }
    }
}

void NetsysBpfNetFirewall::RingBufferListenThread(void)
{
    if (keepListen_) {
        NETNATIVE_LOG_D("under listening");
        return;
    }

    int mapFd = NetsysBpfRingBuffer::GetRingbufFd(MAP_PATH(EVENT_MAP), 0);
    if (mapFd < 0) {
        NETNATIVE_LOGE("failed to get ring buffer fd: errno=%{public}d", errno);
        return;
    }
    ring_buffer *rb = ring_buffer__new(mapFd, NetsysBpfNetFirewall::HandleEvent, NULL, NULL);
    if (!rb) {
        NETNATIVE_LOGE("failed to create ring buffer: errno=%{public}d", errno);
        return;
    }

    keepListen_ = true;
    while (keepListen_) {
        if (ffrt::this_task::get_id() != 0) {
            ffrt::sync_io(mapFd);
        }
        int err = ring_buffer__poll(rb, RING_BUFFER_POLL_TIME_OUT_MS);
        if (err < 0) {
            NETNATIVE_LOGE("Error polling ring buffer: errno=%{public}d", errno);
            keepListen_ = false;
            break;
        }
    }

    NETNATIVE_LOGE("Could not get bpf event ring buffer map");
    ring_buffer__free(rb);
}

int32_t NetsysBpfNetFirewall::StartListener()
{
    if (!isBpfLoaded_) {
        NETNATIVE_LOG_D("bfp is not loaded");
        return -1;
    }
    ctRdMap_ = std::make_unique<BpfMapper<CtKey, CtVaule>>(MAP_PATH(CT_MAP), BPF_F_RDONLY);
    ctWrMap_ = std::make_unique<BpfMapper<CtKey, CtVaule>>(MAP_PATH(CT_MAP), BPF_F_WRONLY);

    ffrt::submit(RingBufferListenThread, {}, {}, ffrt::task_attr().name("RingBufferListen"));
    ffrt::submit(StartConntrackGcThread, { &ctRdMap_ }, { &ctWrMap_ });
    return 0;
}

int32_t NetsysBpfNetFirewall::StopListener()
{
    keepListen_ = false;
    StopConntrackGc();
    return 0;
}

void NetsysBpfNetFirewall::StartConntrackGcThread(void)
{
    if (keepGc_) {
        NETNATIVE_LOG_D("under keepGc");
        return;
    }
    if (!ctRdMap_->IsValid()) {
        NETNATIVE_LOGE("GcConntrackCb: ctRdMap is invalid");
        return;
    }

    if (!ctWrMap_->IsValid()) {
        NETNATIVE_LOGE("GcConntrackCb: ctWrMap is invalid");
        return;
    }

    keepGc_ = true;

    int rdMapFd = NetsysBpfRingBuffer::GetRingbufFd(MAP_PATH(CT_MAP), BPF_F_RDONLY);
    int wrMapFd = NetsysBpfRingBuffer::GetRingbufFd(MAP_PATH(CT_MAP), BPF_F_WRONLY);
    if (rdMapFd < 0 || wrMapFd < 0) {
        NETNATIVE_LOGE("failed to get rdMapFd or wrMapFd: errno=%{public}d", errno);
        return;
    }

    while (keepGc_) {
        ffrt::this_task::sleep_for(std::chrono::milliseconds(CONNTRACK_GC_INTTERVAL_MS));
        if (ffrt::this_task::get_id() != 0) {
            ffrt::sync_io(rdMapFd);
            ffrt::sync_io(wrMapFd);
        }
        ConntrackGcTask();
    }
}

void NetsysBpfNetFirewall::StopConntrackGc()
{
    keepGc_ = false;
}

void NetsysBpfNetFirewall::SetBpfLoaded(bool load)
{
    isBpfLoaded_ = load;
    if (isBpfLoaded_) {
        WriteLoopBackBpfMap();
    }
}

int32_t NetsysBpfNetFirewall::WriteLoopBackBpfMap()
{
    Ipv4LpmKey ip4Key = {};
    LoopbackValue loopbackVal = 1;
    ip4Key.prefixlen = LOOP_BACK_IPV4_PREFIXLEN;
    inet_pton(AF_INET, LOOP_BACK_IPV4, &ip4Key.data);
    WriteBpfMap(MAP_PATH(LOOP_BACK_IPV4_MAP), ip4Key, loopbackVal);
    Ipv6LpmKey ip6Key = {};
    ip6Key.prefixlen = LOOP_BACK_IPV6_PREFIXLEN;
    inet_pton(AF_INET6, LOOP_BACK_IPV6, &ip6Key.data);
    WriteBpfMap(MAP_PATH(LOOP_BACK_IPV6_MAP), ip6Key, loopbackVal);
    return NETFIREWALL_SUCCESS;
}

void NetsysBpfNetFirewall::ClearBpfFirewallRules(NetFirewallRuleDirection direction)
{
    Ipv4LpmKey ip4Key = {};
    Ipv6LpmKey ip6Key = {};
    PortKey portKey = {};
    ProtoKey protoKey = 0;
    interface_key ifaceKey = {};
    AppUidKey appIdKey = 0;
    UidKey uidKey = 0;
    ActionKey actKey = 1;
    ActionValue actVal;
    RuleCode ruleCode;
    CtKey ctKey;
    CtVaule ctVal;

    bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
    int res = 0;
    res += ClearBpfMap(GET_MAP_PATH(ingress, saddr), ip4Key, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, saddr6), ip6Key, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, daddr), ip4Key, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, daddr6), ip6Key, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, sport), portKey, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, dport), portKey, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, proto), protoKey, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, appuid), appIdKey, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, uid), uidKey, ruleCode);
    res += ClearBpfMap(GET_MAP_PATH(ingress, action), actKey, actVal);
    res += ClearBpfMap(GET_MAP_PATH(ingress, iface), ifaceKey, ruleCode);
    res += ClearBpfMap(MAP_PATH(CT_MAP), ctKey, ctVal);
    if (res) {
        NETNATIVE_LOGE("ClearBpfFirewallRules: dir=%{public}d, res=%{public}d", direction, res);
    }
}

int32_t NetsysBpfNetFirewall::ClearFirewallRules(NetFirewallRuleType type)
{
    switch (type) {
        case NetFirewallRuleType::RULE_IP: {
            firewallIpRules_.clear();
            ClearBpfFirewallRules(NetFirewallRuleDirection::RULE_IN);
            ClearBpfFirewallRules(NetFirewallRuleDirection::RULE_OUT);
            break;
        }
        case NetFirewallRuleType::RULE_DOMAIN: {
            firewallDomainRules_.clear();
            ClearDomainRules();
            break;
        }
        case NetFirewallRuleType::RULE_DEFAULT_ACTION: {
            ClearFirewallDefaultAction();
            break;
        }
        case NetFirewallRuleType::RULE_ALL: {
            firewallIpRules_.clear();
            ClearBpfFirewallRules(NetFirewallRuleDirection::RULE_IN);
            ClearBpfFirewallRules(NetFirewallRuleDirection::RULE_OUT);
            firewallDomainRules_.clear();
            ClearDomainRules();
            ClearFirewallDefaultAction();
            break;
        }
        default:
            break;
    }
    return NETFIREWALL_SUCCESS;
}

int32_t NetsysBpfNetFirewall::SetBpfFirewallRules(const std::vector<sptr<NetFirewallIpRule>> &ruleList,
    NetFirewallRuleDirection direction)
{
    BitmapManager manager;
    int32_t ret = manager.BuildBitmapMap(ruleList);
    if (ret) {
        NETNATIVE_LOGE("SetBpfFirewallRules: BuildBitmapMap failed: %{public}d", ret);
        return ret;
    }
    int res = 0;
    ClearBpfFirewallRules(direction);
    res += WriteSrcIpv4BpfMap(manager, direction);
    res += WriteSrcIpv6BpfMap(manager, direction);
    res += WriteDstIpv4BpfMap(manager, direction);
    res += WriteDstIpv6BpfMap(manager, direction);
    res += WriteSrcPortBpfMap(manager, direction);
    res += WriteDstPortBpfMap(manager, direction);
    res += WriteProtoBpfMap(manager, direction);
    res += WriteAppUidBpfMap(manager, direction);
    res += WriteUidBpfMap(manager, direction);
    res += WriteActionBpfMap(manager, direction);
    res += WriteInterfaceBpfMap(manager, direction);
    if (res) {
        NETNATIVE_LOGE("SetBpfFirewallRules: dir=%{public}d, res=%{public}d", direction, res);
    }
    return NETFIREWALL_SUCCESS;
}

int32_t NetsysBpfNetFirewall::SetFirewallRules(NetFirewallRuleType type,
    const std::vector<sptr<NetFirewallBaseRule>> &ruleList, bool isFinish)
{
    if (!isBpfLoaded_) {
        NETNATIVE_LOGE("SetFirewallRules: bpf not loaded");
        return NETFIREWALL_ERR;
    }
    if (ruleList.empty()) {
        NETNATIVE_LOGE("SetFirewallRules: rules is empty");
        return NETFIREWALL_ERR;
    }
    int32_t ret = NETFIREWALL_SUCCESS;
    switch (type) {
        case NetFirewallRuleType::RULE_IP: {
            for (const auto &rule : ruleList) {
                firewallIpRules_.emplace_back(firewall_rule_cast<NetFirewallIpRule>(rule));
            }
            if (isFinish) {
                ret = SetFirewallIpRules(firewallIpRules_);
                firewallIpRules_.clear();
            }
            break;
        }
        case NetFirewallRuleType::RULE_DOMAIN: {
            for (const auto &rule : ruleList) {
                firewallDomainRules_.emplace_back(firewall_rule_cast<NetFirewallDomainRule>(rule));
            }
            if (isFinish) {
                ret = SetFirewallDomainRules(firewallDomainRules_);
                firewallDomainRules_.clear();
            }
            break;
        }
        default:
            break;
    }
    NETNATIVE_LOGI("SetFirewallRules: ret=%{public}d size=%{public}zu isFinish=%{public}" PRId32, ret,
        ruleList.size(), isFinish);
    return ret;
}

int32_t NetsysBpfNetFirewall::SetFirewallDomainRules(const std::vector<sptr<NetFirewallDomainRule>> &ruleList)
{
    if (ruleList.empty()) {
        NETNATIVE_LOGE("SetFirewallDomainRules: rules is empty");
        return NETFIREWALL_ERR;
    }
    DomainValue domainVaule = { 0 };
    bool isWildcard = false;
    ClearDomainRules();
    int ret = 0;
    for (const auto &rule : ruleList) {
        domainVaule.uid = static_cast<uint32_t>(rule->userId);
        domainVaule.appuid = static_cast<uint32_t>(rule->appUid);
        for (const auto &param : rule->domains) {
            if (param.isWildcard) {
                isWildcard = true;
            } else {
                isWildcard = false;
            }
            DomainHashKey key = { 0 };
            GetDomainHashKey(param.domain, key);
            ret = SetBpfFirewallDomainRules(rule->ruleAction, key, domainVaule, isWildcard);
        }
    }
    return ret;
}

void NetsysBpfNetFirewall::GetDomainHashKey(const std::string &domain, DomainHashKey &out)
{
    if (domain.empty()) {
        NETNATIVE_LOGE("GetDomainHashKey: domain is empty");
        return;
    }
    std::string text(domain);
    std::reverse(text.begin(), text.end());
    text.erase(std::remove(text.begin(), text.end(), '*'), text.end());

    std::regex delimit("\\.");
    std::vector<std::string> v(std::sregex_token_iterator(text.begin(), text.end(), delimit, -1),
        std::sregex_token_iterator());

    int i = 0;
    for (auto &s : v) {
        int strLen = static_cast<int>(s.length());
        if (i + strLen + 1 > DNS_DOMAIN_LEN) {
            NETNATIVE_LOGE("GetDomainHashKey: domain length exceeds DNS_DOMAIN_LEN");
            return;
        }
        if (memcpy_s(out.data + i, DNS_DOMAIN_LEN - i, (uint8_t *)s.c_str(), strLen) != EOK) {
            NETNATIVE_LOGE("GetDomainHashKey: memcpy_s failed");
            return;
        }
        i += strLen;
        out.data[i++] = static_cast<uint8_t>(strLen);
    }
    out.prefixlen = static_cast<uint32_t>((sizeof(out.uid) + sizeof(out.appuid) + i) * BIT_PER_BYTE);
}

int32_t NetsysBpfNetFirewall::SetBpfFirewallDomainRules(FirewallRuleAction action, DomainHashKey &key,
    DomainValue value, bool isWildcard)
{
    NETNATIVE_LOG_D("SetBpfFirewallDomainRules: action=%{public}d, userid=%{public}d appuid=%{public}d",
        (action == FirewallRuleAction::RULE_ALLOW), value.uid, value.appuid);
    int32_t ret = 0;
    key.uid = value.uid;
    key.appuid = value.appuid;
    __u8 v = 1;
    if (action == FirewallRuleAction::RULE_ALLOW) {
        ret = WriteBpfMap(MAP_PATH(DOMAIN_PASS_MAP), key, v);
    } else if (action == FirewallRuleAction::RULE_DENY) {
        ret = WriteBpfMap(MAP_PATH(DOMAIN_DENY_MAP), key, v);
    }
    return ret;
}

void NetsysBpfNetFirewall::ClearDomainRules()
{
    NETNATIVE_LOG_D("ClearDomainRules");
    ClearDomainCache();
    DomainHashKey key = { 0 };
    DomainValue value = { 0 };
    __u8 v = 1;
    ClearBpfMap(MAP_PATH(DOMAIN_PASS_MAP), key, v);
    ClearBpfMap(MAP_PATH(DOMAIN_DENY_MAP), key, v);
}

int32_t NetsysBpfNetFirewall::SetFirewallIpRules(const std::vector<sptr<NetFirewallIpRule>> &ruleList)
{
    std::vector<sptr<NetFirewallIpRule>> inRules;
    std::vector<sptr<NetFirewallIpRule>> outRules;

    for (const auto &rule : ruleList) {
        if (rule->ruleDirection == NetFirewallRuleDirection::RULE_IN) {
            if (rule->protocol == NetworkProtocol::ICMP || rule->protocol == NetworkProtocol::ICMPV6) {
                outRules.emplace_back(rule);
            } else {
                inRules.emplace_back(rule);
            }
        }
        if (rule->ruleDirection == NetFirewallRuleDirection::RULE_OUT) {
            outRules.emplace_back(rule);
        }
    }

    int32_t ret = NETFIREWALL_SUCCESS;
    if (!inRules.empty()) {
        ret = SetBpfFirewallRules(inRules, NetFirewallRuleDirection::RULE_IN);
    }
    if (!outRules.empty()) {
        ret += SetBpfFirewallRules(outRules, NetFirewallRuleDirection::RULE_OUT);
    }
    return ret;
}

void NetsysBpfNetFirewall::ClearFirewallDefaultAction()
{
    defalut_action_value val = { SK_PASS };
    int32_t userId = -1;
    ClearBpfMap(MAP_PATH(DEFAULT_ACTION_MAP), (uid_key)userId, val);
}

int32_t NetsysBpfNetFirewall::SetFirewallDefaultAction(int32_t userId, FirewallRuleAction inDefault,
    FirewallRuleAction outDefault)
{
    if (!isBpfLoaded_) {
        NETNATIVE_LOGE("SetFirewallDefaultAction: bpf not loaded");
        return NETFIREWALL_ERR;
    }
    defalut_action_value val = { SK_PASS };
    val.inaction = (inDefault == FirewallRuleAction::RULE_ALLOW) ? SK_PASS : SK_DROP;
    val.outaction = (outDefault == FirewallRuleAction::RULE_ALLOW) ? SK_PASS : SK_DROP;
    WriteBpfMap(MAP_PATH(DEFAULT_ACTION_MAP), (uid_key)userId, val);
    CtKey ctKey;
    CtVaule ctVal;
    ClearBpfMap(MAP_PATH(CT_MAP), ctKey, ctVal);
    return NETFIREWALL_SUCCESS;
}

int32_t NetsysBpfNetFirewall::SetFirewallCurrentUserId(int32_t userId)
{
    if (!isBpfLoaded_) {
        NETNATIVE_LOGE("SetFirewallCurrentUserId: bpf not loaded");
        return NETFIREWALL_ERR;
    }

    CurrentUserIdKey key = CURRENT_USER_ID_KEY;
    UidKey val = (UidKey)userId;
    WriteBpfMap(MAP_PATH(CURRENT_UID_MAP), key, val);
    return NETFIREWALL_SUCCESS;
}

int32_t NetsysBpfNetFirewall::WriteSrcIpv4BpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    std::vector<Ip4RuleBitmap> &srcIp4Map = manager.GetSrcIp4Map();
    if (srcIp4Map.empty()) {
        NETNATIVE_LOGE("WriteSrcIpv4BpfMap: srcIp4Map is empty");
        return -1;
    }
    bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
    int32_t res = 0;
    for (const auto &node : srcIp4Map) {
        Bitmap val = node.bitmap;
        RuleCode rule;
        memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));

        Ipv4LpmKey key = { 0 };
        key.prefixlen = node.mask;
        key.data = static_cast<Ip4Key>(node.data);
        res += WriteBpfMap(GET_MAP_PATH(ingress, saddr), key, rule);
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WriteSrcIpv6BpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    std::vector<Ip6RuleBitmap> &srcIp6Map = manager.GetSrcIp6Map();
    if (srcIp6Map.empty()) {
        NETNATIVE_LOGE("WriteSrcIpv6BpfMap: srcIp6Map is empty");
        return -1;
    }
    bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
    int32_t res = 0;
    for (const auto &node : srcIp6Map) {
        Bitmap val = node.bitmap;
        RuleCode rule;
        memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));

        Ipv6LpmKey key = { 0 };
        key.prefixlen = node.prefixlen;
        key.data = static_cast<Ip6Key>(node.data);
        res += WriteBpfMap(GET_MAP_PATH(ingress, saddr6), key, rule);
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WriteDstIpv4BpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    std::vector<Ip4RuleBitmap> &dstIp4Map = manager.GetDstIp4Map();
    int32_t res = 0;
    if (dstIp4Map.empty()) {
        NETNATIVE_LOGE("WriteDstIp4BpfMap: dstIp4Map is empty");
        return -1;
    } else {
        bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
        for (const auto &node : dstIp4Map) {
            Bitmap val = node.bitmap;
            RuleCode rule;
            memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));

            Ipv4LpmKey key = { 0 };
            key.prefixlen = node.mask;
            key.data = static_cast<Ip4Key>(node.data);
            res += WriteBpfMap(GET_MAP_PATH(ingress, daddr), key, rule);
        }
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WriteDstIpv6BpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    std::vector<Ip6RuleBitmap> &dstIp6Map = manager.GetDstIp6Map();
    int32_t res = 0;
    if (dstIp6Map.empty()) {
        NETNATIVE_LOGE("WriteDstIp6BpfMap: dstIp6Map is empty");
        return -1;
    } else {
        bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
        for (const auto &node : dstIp6Map) {
            Bitmap val = node.bitmap;
            RuleCode rule;
            memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));

            Ipv6LpmKey key = { 0 };
            key.prefixlen = node.prefixlen;
            key.data = static_cast<Ip6Key>(node.data);
            res += WriteBpfMap(GET_MAP_PATH(ingress, daddr6), key, rule);
        }
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WritePortBpfMap(BpfPortMap &portMap, const char *path)
{
    if (path == nullptr || *path == '\0') {
        return -1;
    }
    int32_t res = 0;
    if (portMap.Get().empty()) {
        NETNATIVE_LOGE("WriteSrcPortBpfMap: portMap is empty");
        return -1;
    } else {
        BpfMapper<PortKey, RuleCode> map(path, BPF_F_WRONLY);
        if (!map.IsValid()) {
            NETNATIVE_LOGE("WriteBpfMap: map invalid: %{public}s", path);
            return -1;
        }
        for (const auto &pair : portMap.Get()) {
            PortKey key;
            key.prefixlen = pair.prefixlen;
            key.data = pair.data;
            Bitmap val = pair.bitmap;
            RuleCode rule;
            memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));
            NETNATIVE_LOG_D("sport_map=%{public}u", key.data);
            res += map.Write(key, rule, BPF_ANY) == 0 ? 0 : -1;
        }
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WriteSrcPortBpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
    return WritePortBpfMap(manager.GetSrcPortMap(), GET_MAP_PATH(ingress, sport));
}

int32_t NetsysBpfNetFirewall::WriteDstPortBpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
    return WritePortBpfMap(manager.GetDstPortMap(), GET_MAP_PATH(ingress, dport));
}

int32_t NetsysBpfNetFirewall::WriteProtoBpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    BpfProtoMap &protoMap = manager.GetProtoMap();
    int32_t res = 0;
    if (protoMap.Empty()) {
        NETNATIVE_LOGE("WriteProtoBpfMap: protoMap is empty");
        return -1;
    } else {
        bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
        for (const auto &pair : protoMap.Get()) {
            ProtoKey key = pair.first;
            Bitmap val = pair.second;
            RuleCode rule;
            memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));
            NETNATIVE_LOG_D("proto_map=%{public}u", key);
            res += WriteBpfMap(GET_MAP_PATH(ingress, proto), key, rule);
        }
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WriteAppUidBpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    BpfAppUidMap &appIdMap = manager.GetAppIdMap();
    int32_t res = 0;
    if (appIdMap.Empty()) {
        NETNATIVE_LOGE("WriteAppUidBpfMap: appIdMap is empty");
        return -1;
    } else {
        bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
        for (const auto &pair : appIdMap.Get()) {
            AppUidKey key = pair.first;
            Bitmap val = pair.second;
            RuleCode rule;
            memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));
            NETNATIVE_LOG_D("appuid_map=%{public}u", key);
            res += WriteBpfMap(GET_MAP_PATH(ingress, appuid), key, rule);
        }
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WriteUidBpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    BpfUidMap &uidMap = manager.GetUidMap();
    int32_t res = 0;
    if (uidMap.Empty()) {
        NETNATIVE_LOGE("WriteUidBpfMap: uidMap is empty");
        return -1;
    } else {
        bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
        for (const auto &pair : uidMap.Get()) {
            UidKey key = pair.first;
            Bitmap val = pair.second;
            RuleCode rule;
            memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));
            NETNATIVE_LOG_D("uidMap=%{public}u", key);
            res += WriteBpfMap(GET_MAP_PATH(ingress, uid), key, rule);
        }
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WriteActionBpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    BpfActionMap &actionMap = manager.GetActionMap();
    int32_t res = 0;
    if (actionMap.Empty()) {
        NETNATIVE_LOGE("WriteActionBpfMap: actionMap is empty");
        return -1;
    } else {
        bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
        for (const auto &pair : actionMap.Get()) {
            ActionKey key = pair.first;
            Bitmap val = pair.second;
            RuleCode rule;
            memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));
            NETNATIVE_LOG_D("action_map=%{public}u", val.Get()[0]);
            res += WriteBpfMap(GET_MAP_PATH(ingress, action), key, rule);
        }
    }
    return res;
}

int32_t NetsysBpfNetFirewall::WriteInterfaceBpfMap(BitmapManager &manager, NetFirewallRuleDirection direction)
{
    BpfInterfaceMap &interfaceMap = manager.GetInterfaceMap();
    int32_t res = 0;
    if (interfaceMap.Empty()) {
        NETNATIVE_LOGE("WriteInterfaceBpfMap: interfaceMap is empty");
        return -1;
    }

    bool ingress = (direction == NetFirewallRuleDirection::RULE_IN);
    for (const auto &pair : interfaceMap.Get()) {
        const std::string &devName = pair.first;
        Bitmap val = pair.second;
        if (devName.length() > INTERFACE_NAME_MAX_LEN) {
            NETNATIVE_LOGE("WriteInterfaceBpfMap: interface name too long: %{public}s, bitmap=%{public}u",
                devName.c_str(), val.Get()[0]);
            continue;
        }
        interface_key ifKey = {};
        if (strncpy_s(ifKey.name, INTERFACE_NAME_MAX_LEN, devName.c_str(), devName.length()) != EOK) {
            NETNATIVE_LOGE("WriteInterfaceBpfMap: devName: %{public}s, bitmap=%{public}u", devName.c_str(),
                val.Get()[0]);
            continue;
        }

        RuleCode rule;
        memcpy_s(rule.val, sizeof(RuleCode), val.Get(), sizeof(RuleCode));
        res += WriteBpfMap(GET_MAP_PATH(ingress, iface), ifKey, rule);
    }
    return res;
}

int32_t NetsysBpfNetFirewall::RegisterCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    if (!callback) {
        return -1;
    }

    callbacks_.emplace_back(callback);

    return 0;
}
int32_t NetsysBpfNetFirewall::UnregisterCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    if (!callback) {
        return -1;
    }

    for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it) {
        if (*it == callback) {
            callbacks_.erase(it);
            return 0;
        }
    }
    return -1;
}

void NetsysBpfNetFirewall::NotifyInterceptEvent(InterceptEvent *info)
{
    if (!info) {
        return;
    }
    sptr<InterceptRecord> record = sptr<InterceptRecord>::MakeSptr();
    record->time = static_cast<decltype(record->time)>(GetNowMs());
    record->localPort = BitmapManager::Nstohl(info->sport);
    record->remotePort = BitmapManager::Nstohl(info->dport);
    record->protocol = static_cast<uint16_t>(info->protocol);
    record->appUid = (int32_t)info->appuid;
    std::string srcIp;
    std::string dstIp;
    if (info->family == AF_INET) {
        char ip4[INET_ADDRSTRLEN] = {};
        inet_ntop(AF_INET, &(info->ipv4.saddr), ip4, INET_ADDRSTRLEN);
        srcIp = ip4;
        memset_s(ip4, INET_ADDRSTRLEN, 0, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(info->ipv4.daddr), ip4, INET_ADDRSTRLEN);
        dstIp = ip4;
    } else {
        char ip6[INET6_ADDRSTRLEN] = {};
        inet_ntop(AF_INET6, &(info->ipv6.saddr), ip6, INET6_ADDRSTRLEN);
        srcIp = ip6;
        memset_s(ip6, INET6_ADDRSTRLEN, 0, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(info->ipv6.daddr), ip6, INET6_ADDRSTRLEN);
        dstIp = ip6;
    }
    if (info->dir == INGRESS) {
        record->localIp = srcIp;
        record->remoteIp = dstIp;
    } else {
        record->localIp = dstIp;
        record->remoteIp = srcIp;
    }
    record->domain = DecodeDomainFromKey(info->domainData);
    for (auto callback : callbacks_) {
        callback->OnIntercept(record);
    }
}

std::string NetsysBpfNetFirewall::DecodeDomainFromKey(const DomainHashKey &key)
{
    if (key.prefixlen == 0) {
        return "";
    }
    if (key.prefixlen < (sizeof(key.uid) + sizeof(key.appuid)) * BIT_PER_BYTE) {
        return "";
    }
    unsigned int domainPrefixlen = key.prefixlen - (sizeof(key.uid) + sizeof(key.appuid)) * BIT_PER_BYTE;
    if (domainPrefixlen <= 0) {
        return "";
    }
    size_t domainLenBytes = static_cast<size_t>(domainPrefixlen / BIT_PER_BYTE);
    if (domainLenBytes < DNS_DOMAIN_LEN_MIN) {
        return "";
    }
    if (domainLenBytes > DNS_DOMAIN_LEN) {
        domainLenBytes = DNS_DOMAIN_LEN;
    }
    std::vector<uint8_t> qnameReconstructed(domainLenBytes);
    for (size_t i = 0; i < domainLenBytes; i++) {
        qnameReconstructed[i] = key.data[domainLenBytes - 1 - i];
    }
    std::string domain;
    size_t i = 0;
    bool isFirstLable = true;
    while (i < qnameReconstructed.size()) {
        uint8_t labelLen = qnameReconstructed[i++];
        if (labelLen == 0) {
            break;
        }
        if (i + labelLen > qnameReconstructed.size()) {
            break;
        }
        if (!isFirstLable) {
            domain.push_back('.');
        }
        domain.append(reinterpret_cast<const char*>(&qnameReconstructed[i]), labelLen);
        isFirstLable = false;
        i += labelLen;
    }
    return domain;
}

uint64_t NetsysBpfNetFirewall::GetNowMs()
{
    struct timespec ts;
    return (clock_gettime(CLOCK_REALTIME, &ts) == 0)
               ? static_cast<uint64_t>(ts.tv_sec) * MILLIS_PER_SEC + static_cast<uint64_t>(ts.tv_nsec) / NANOS_PER_MILLI
               : static_cast<uint64_t>(time(nullptr)) * MILLIS_PER_SEC;
}

void NetsysBpfNetFirewall::HandleTupleEvent(TupleEvent *ev)
{
    NETNATIVE_LOG_D(
        "%{public}s tuple: sport=%{public}u dport=%{public}u protocol=%{public}u appuid=%{public}u uid=%{public}u",
        (ev->dir == INGRESS) ? "> ingress" : "< egress", ntohs(ev->sport), ntohs(ev->dport), ev->protocol, ev->appuid,
        ev->uid);
    NETNATIVE_LOG_D("\trstpacket=%{public}u", ev->rst);
}

void NetsysBpfNetFirewall::HandleInterceptEvent(InterceptEvent *ev)
{
    GetInstance()->NotifyInterceptEvent(ev);

    NETNATIVE_LOGI("%{public}s intercept: sport=%{public}u dport=%{public}u protocol=%{public}u appuid=%{public}u",
        (ev->dir == INGRESS) ? "ingress" : "egress", ntohs(ev->sport), ntohs(ev->dport), ev->protocol, ev->appuid);
}

void NetsysBpfNetFirewall::HandleDebugEvent(DebugEvent *ev)
{
    const char *direction = ev->dir == INGRESS ? ">" : "<";
    switch (ev->type) {
        case DBG_MATCH_SPORT:
            NETNATIVE_LOG_D("%{public}s sport: %{public}u bitmap: %{public}x", direction, ntohs(ev->arg1), ev->arg2);
            break;
        case DBG_MATCH_DPORT:
            NETNATIVE_LOG_D("%{public}s dport: %{public}u bitmap: %{public}x", direction, ntohs(ev->arg1), ev->arg2);
            break;
        case DBG_MATCH_PROTO:
            NETNATIVE_LOG_D("%{public}s protocol: %{public}u bitmap: %{public}x", direction, ev->arg1, ev->arg2);
            break;
        case DBG_MATCH_APPUID:
            NETNATIVE_LOG_D("%{public}s appuid: %{public}u bitmap: %{public}x", direction, ev->arg1, ev->arg2);
            break;
        case DBG_MATCH_UID:
            NETNATIVE_LOG_D("%{public}s uid: %{public}u bitmap: %{public}x", direction, ev->arg1, ev->arg2);
            break;
        case DBG_ACTION_KEY:
            NETNATIVE_LOG_D("%{public}s actionkey: %{public}x", direction, ev->arg1);
            break;
        case DBG_MATCH_ACTION:
            NETNATIVE_LOG_D("%{public}s    action: %{public}s", direction, (ev->arg1 == SK_PASS ? "PASS" : "DROP"));
            break;
        case DBG_CT_LOOKUP:
            NETNATIVE_LOG_D("%{public}s ct lookup status: %{public}u", direction, ev->arg1);
            break;
        case DBG_MATCH_DOMAIN:
            NETNATIVE_LOG_D("egress match domain, action PASS");
            break;
        case DBG_MATCH_DOMAIN_ACTION:
            NETNATIVE_LOG_D("%{public}s match domain action: %{public}s", direction,
                (ev->arg1 == SK_PASS ? "PASS" : "DROP"));
            break;
        case DBG_MATCH_INTERFACE:
            NETNATIVE_LOG_D("%{public}s interface: %{public}u bitmap: %{public}x", direction, ev->arg1, ev->arg2);
            break;
        default:
            break;
    }
}

int NetsysBpfNetFirewall::HandleEvent(void *ctx, void *data, size_t len)
{
    if (data != NULL && len >= sizeof(Event)) {
        Event *ev = (Event *)data;

        switch (ev->type) {
            case EVENT_DEBUG: {
                HandleDebugEvent(&(ev->debug));
                break;
            }
            case EVENT_INTERCEPT: {
                HandleInterceptEvent(&(ev->intercept));
                break;
            }
            case EVENT_TUPLE_DEBUG: {
                HandleTupleEvent(&(ev->tuple));
                break;
            }
            default:
                break;
        }
    }
    return 0;
}

int32_t NetsysBpfNetFirewall::LoadSystemAbility(int32_t systemAbilityId)
{
    NETNATIVE_LOG_D("GetSystemAbility: [%{public}d]", systemAbilityId);
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        NETNATIVE_LOGE("GetCmProxy registry is null");
        return -1;
    }

    auto object = saManager->CheckSystemAbility(systemAbilityId);
    if (object != nullptr) {
        return 0;
    }
    sptr<OHOS::IRemoteObject> ret = saManager->GetSystemAbility(systemAbilityId);
    if (ret == nullptr || !ret->IsProxyObject()) {
        NETNATIVE_LOGE("ret is nullptr");
        return -1;
    }

    return 0;
}

void NetsysBpfNetFirewall::AddDomainCache(const NetAddrInfo &addrInfo)
{
    NETNATIVE_LOGI("AddDomainCache");
    domain_value value = { 0 };
    if (addrInfo.aiFamily == AF_INET) {
        Ipv4LpmKey key = { 0 };
        key.prefixlen = IPV4_MAX_PREFIXLEN;
        key.data = addrInfo.aiAddr.sin.s_addr;
        WriteBpfMap(MAP_PATH(DOMAIN_IPV4_MAP), key, value);
    } else {
        Ipv6LpmKey key = { 0 };
        key.prefixlen = IPV6_MAX_PREFIXLEN;
        key.data = addrInfo.aiAddr.sin6;
        WriteBpfMap(MAP_PATH(DOMAIN_IPV6_MAP), key, value);
    }
}

void NetsysBpfNetFirewall::ClearDomainCache()
{
    NETNATIVE_LOG_D("ClearDomainCache");
    Ipv4LpmKey ip4Key = {};
    Ipv6LpmKey ip6Key = {};
    domain_value value { 0 };
    ClearBpfMap(MAP_PATH(DOMAIN_IPV4_MAP), ip4Key, value);
    ClearBpfMap(MAP_PATH(DOMAIN_IPV6_MAP), ip6Key, value);
}
} // namespace NetManagerStandard
} // namespace OHOS
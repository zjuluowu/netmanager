/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "netlink_msg.h"

#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
NetlinkMsg::NetlinkMsg(uint16_t flags, size_t maxBufLen, int32_t pid)
{
    maxBufLen_ = maxBufLen;
    msghdrBuf_ = std::make_unique<char[]>(NLMSG_SPACE(maxBufLen));
    netlinkMessage_ = reinterpret_cast<struct nlmsghdr *>(msghdrBuf_.get());
    errno_t result = memset_s(netlinkMessage_, NLMSG_SPACE(maxBufLen), 0, NLMSG_SPACE(maxBufLen));
    if (result != 0) {
        NETNATIVE_LOGE("[NetlinkMessage]: memset result %{public}d", result);
        return;
    }
    netlinkMessage_->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | flags;
    netlinkMessage_->nlmsg_pid = static_cast<uint32_t>(pid);
    netlinkMessage_->nlmsg_seq = 1;
}

NetlinkMsg::~NetlinkMsg() = default;

void NetlinkMsg::AddRoute(uint16_t action, struct rtmsg msg)
{
    netlinkMessage_->nlmsg_type = action;
    int32_t result = memcpy_s(NLMSG_DATA(netlinkMessage_), maxBufLen_, &msg, sizeof(struct rtmsg));
    if (result != 0) {
        NETNATIVE_LOGE("[AddRoute]: string copy failed result %{public}d", result);
    }
    netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
}

void NetlinkMsg::AddRule(uint16_t action, struct fib_rule_hdr msg)
{
    netlinkMessage_->nlmsg_type = action;
    int32_t result =
        memcpy_s(NLMSG_DATA(netlinkMessage_), maxBufLen_, &msg, sizeof(struct fib_rule_hdr));
    if (result != 0) {
        NETNATIVE_LOGE("[AddRule]: string copy failed result %{public}d", result);
    }
    netlinkMessage_->nlmsg_len = static_cast<uint32_t>(NLMSG_LENGTH(sizeof(struct fib_rule_hdr)));
}

void NetlinkMsg::AddAddress(uint16_t action, struct ifaddrmsg msg)
{
    netlinkMessage_->nlmsg_type = action;
    int32_t result = memcpy_s(NLMSG_DATA(netlinkMessage_), maxBufLen_, &msg, sizeof(struct ifaddrmsg));
    if (result != 0) {
        NETNATIVE_LOGE("[AddAddress]: string copy failed result %{public}d", result);
        return;
    }
    netlinkMessage_->nlmsg_len = static_cast<uint32_t>(NLMSG_LENGTH(sizeof(struct ifaddrmsg)));
}

int32_t NetlinkMsg::AddAttr(uint16_t type, void *data, size_t alen)
{
    if (alen == 0 || data == nullptr) {
        NETNATIVE_LOGE("[NetlinkMessage]: length data can not be 0 or attr data can not be null");
        return -1;
    }

    int32_t len = RTA_LENGTH(alen);
    if (NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_ALIGN(len) > maxBufLen_) {
        NETNATIVE_LOGE("[NetlinkMessage]: attr length than max len: %{public}d", (int32_t)maxBufLen_);
        return -1;
    }

    struct rtattr *rta = (struct rtattr *)(((char *)netlinkMessage_) + NLMSG_ALIGN(netlinkMessage_->nlmsg_len));
    if (rta == nullptr) {
        NETNATIVE_LOGE("Pointer rta is nullptr");
        return -1;
    }
    rta->rta_type = type;
    rta->rta_len = static_cast<uint16_t>(len);

    size_t remainSize = maxBufLen_ > (NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_LENGTH(0)) ?
        (maxBufLen_ - (NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_LENGTH(0))) : 0;
    if (data != nullptr) {
        int32_t result = memcpy_s(RTA_DATA(rta), remainSize, data, alen);
        if (result != 0) {
            NETNATIVE_LOGE("[get_addr_info]: string copy failed result %{public}d", result);
            return -1;
        }
    }

    netlinkMessage_->nlmsg_len = NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_ALIGN(len);
    return 0;
}

int32_t NetlinkMsg::AddAttr16(uint16_t type, uint16_t data)
{
    return AddAttr(type, &data, sizeof(uint16_t));
}

int32_t NetlinkMsg::AddAttr32(uint16_t type, uint32_t data)
{
    return AddAttr(type, &data, sizeof(uint32_t));
}

nlmsghdr *NetlinkMsg::GetNetLinkMessage()
{
    return netlinkMessage_;
}

void NetlinkMsg::AddNeighbor(uint16_t action, const struct ndmsg& msg)
{
    netlinkMessage_->nlmsg_type = action;
    int32_t result = memcpy_s(NLMSG_DATA(netlinkMessage_), sizeof(struct ndmsg), &msg, sizeof(struct ndmsg));
    if (result != 0) {
        NETNATIVE_LOGE("[AddNeighbor]: string copy failed result %{public}d", result);
        return;
    }
    netlinkMessage_->nlmsg_len = static_cast<uint32_t>(NLMSG_LENGTH(sizeof(struct ndmsg)));
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
bool NetlinkMsg::InitNflogConfig(uint16_t groupId)
{
    if (netlinkMessage_ == nullptr || maxBufLen_ < static_cast<size_t>(NLMSG_SPACE(sizeof(nfgenmsg)))) {
        return false;
    }
    netlinkMessage_->nlmsg_len = static_cast<uint32_t>(NLMSG_LENGTH(sizeof(nfgenmsg)));
    netlinkMessage_->nlmsg_type = LOCAL_NFLOG_CONFIG;
    netlinkMessage_->nlmsg_flags = NLM_F_REQUEST;
    netlinkMessage_->nlmsg_seq = static_cast<uint32_t>(time(nullptr));
    netlinkMessage_->nlmsg_pid = 0;

    auto *nfHeader = reinterpret_cast<nfgenmsg *>(NLMSG_DATA(netlinkMessage_));
    nfHeader->nfgen_family = AF_UNSPEC;
    nfHeader->version = NFNETLINK_V0;
    nfHeader->res_id = htons(groupId);
    return true;
}

bool NetlinkMsg::AddNlattr(uint16_t type, const void *data, size_t dataSize)
{
    if (netlinkMessage_ == nullptr || data == nullptr || dataSize == 0) {
        return false;
    }
    if (dataSize > SIZE_MAX - static_cast<size_t>(NLA_HDRLEN)) {
        return false;
    }
    size_t need = static_cast<size_t>(NLA_HDRLEN) + dataSize;
    size_t end = NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + NLA_ALIGN(need);
    if (end > maxBufLen_) {
        return false;
    }

    auto *attr =
        reinterpret_cast<nlattr *>(reinterpret_cast<char *>(netlinkMessage_) + NLMSG_ALIGN(netlinkMessage_->nlmsg_len));
    attr->nla_type = type;
    attr->nla_len = static_cast<uint16_t>(need);

    void *dest = reinterpret_cast<char *>(attr) + NLA_HDRLEN;
    size_t dstLen = need - static_cast<size_t>(NLA_HDRLEN);
    if (memcpy_s(dest, dstLen, data, dataSize) != EOK) {
        return false;
    }
    netlinkMessage_->nlmsg_len = static_cast<uint32_t>(end);
    return true;
}

bool NetlinkMsg::AddCmdAttr(uint16_t type, const nfulnl_msg_config_cmd &cmd)
{
    return AddNlattr(type, &cmd, sizeof(cmd));
}

bool NetlinkMsg::AddModeAttr(uint16_t type, const nfulnl_msg_config_mode &mode)
{
    return AddNlattr(type, &mode, sizeof(mode));
}
#endif

void NetlinkMsg::AddLink(uint16_t action, const struct ifinfomsg& msg)
{
    netlinkMessage_->nlmsg_type = action;
    size_t remainSize = maxBufLen_ > (NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_LENGTH(0)) ?
        (maxBufLen_ - (NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_LENGTH(0))) : 0;
    int32_t result = memcpy_s(NLMSG_DATA(netlinkMessage_), remainSize, &msg, sizeof(struct ifinfomsg));
    if (result != 0) {
        NETNATIVE_LOGE("[AddLink]: string copy failed result %{public}d", result);
        return;
    }
    netlinkMessage_->nlmsg_len = static_cast<uint32_t>(NLMSG_LENGTH(sizeof(struct ifinfomsg)));
}

struct nlattr *NetlinkMsg::AddNestedStart(int type)
{
    if (NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_ALIGN(sizeof(struct nlattr)) > nmd::NETLINK_MAX_LEN) {
        return nullptr;
    }
    struct nlattr *nested = reinterpret_cast<struct nlattr*>(
        reinterpret_cast<char*>(netlinkMessage_) + NLMSG_ALIGN(netlinkMessage_->nlmsg_len));
    nested->nla_type = type;
    nested->nla_len = RTA_LENGTH(0);
    netlinkMessage_->nlmsg_len = NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_ALIGN(nested->nla_len);
    return nested;
}

void NetlinkMsg::AddNestedEnd(struct nlattr *nested)
{
    if (nested == nullptr) {
        return;
    }
    nested->nla_len = reinterpret_cast<char*>(netlinkMessage_) + NLMSG_ALIGN(netlinkMessage_->nlmsg_len) -
                       reinterpret_cast<char*>(nested);
}
} // namespace nmd
} // namespace OHOS

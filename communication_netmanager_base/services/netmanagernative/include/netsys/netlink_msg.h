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

#ifndef INCLUDE_NETLINK_MSG_H
#define INCLUDE_NETLINK_MSG_H

#include <arpa/inet.h>
#include <asm/types.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <linux/fib_rules.h>
#include <linux/inet_diag.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef FEATURE_NET_FIREWALL_ENABLE
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nfnetlink_log.h>
#endif

namespace OHOS {
namespace nmd {
constexpr uint32_t NETLINK_MAX_LEN = 1024;
#ifdef FEATURE_NET_FIREWALL_ENABLE
constexpr uint32_t PACKET_COPY_LENGTH = 256;
constexpr int32_t LOCAL_NFLOG_CONFIG = NFNL_SUBSYS_ULOG << 8 | NFULNL_MSG_CONFIG;
constexpr int16_t MSG_BUFFER_SIZE = 512;
#endif
class NetlinkMsg {
public:
    NetlinkMsg(uint16_t flags, size_t maxBufLen, int32_t pid);
    ~NetlinkMsg();
    NetlinkMsg(NetlinkMsg&&) = default;
    NetlinkMsg& operator=(NetlinkMsg&&) = default;
    NetlinkMsg(const NetlinkMsg&) = delete;
    NetlinkMsg& operator=(const NetlinkMsg&) = delete;

    /**
     * Add route message to nlmsghdr
     *
     * @param action Action name
     * @param msg Added message
     */
    void AddRoute(uint16_t action, struct rtmsg msg);

    /**
     * Add rule message to nlmsghdr
     *
     * @param action Action name
     * @param msg Added message
     */
    void AddRule(uint16_t action, struct fib_rule_hdr msg);

    /**
     * Add address message to nlmsghdr
     *
     * @param action Action name
     * @param msg Added message
     */
    void AddAddress(uint16_t action, struct ifaddrmsg msg);

    /**
     * Add rtattr to nlmsghdr
     *
     * @param rtaType Rta type
     * @param buf Rta data
     * @param bufLen Rta data length
     * @return Returns 0, add rtattr to nlmsghdr successfully, otherwise it will fail
     */
    int32_t AddAttr(uint16_t rtaType, void *data, size_t dataLen);

    /**
     * Add 16 bit rtattr to nlmsghdr
     *
     * @param rtaType Rta type
     * @param data Rta data
     * @return Returns 0, add 16 bit rtattr to nlmsghdr successfully, otherwise it will fail
     */
    int32_t AddAttr16(uint16_t rtaType, uint16_t data);

    /**
     * Add 32 bit rtattr to nlmsghdr for
     *
     * @param rtaType Rta type
     * @param data Rta data
     * @return Returns 0, add 32 bit rtattr to nlmsghdr successfully, otherwise it will fail
     */
    int32_t AddAttr32(uint16_t rtaType, uint32_t data);

    /**
     * Get the netlink message
     *
     * @return Netlink message struct
     */
    struct nlmsghdr *GetNetLinkMessage();

    /**
     * Add ndmsg message to nlmsghdr
     *
     * @param action Action name
     * @param msg Added message
     */
    void AddNeighbor(uint16_t action, const struct ndmsg& msg);

#ifdef FEATURE_NET_FIREWALL_ENABLE
    /**
     * Init NFLOG config message
     *
     * @param msg NFLOG config message buffer
     * @param groupId NFLOG group id
     * @return Returns true if init successfully, otherwise false
     */
    bool InitNflogConfig(uint16_t groupId);

    /**
     * Add nlattr to NFLOG config message
     *
     * @param type Attr type
     * @param msg NFLOG config message buffer
     * @param data Attr data
     * @param dataSize Attr data length
     * @return Returns true if add successfully, otherwise false
     */
    bool AddNlattr(uint16_t type, const void *data, size_t dataSize);

    /**
     * Add command attr to NFLOG config message
     *
     * @param type Attr type
     * @param msg NFLOG config message buffer
     * @param cmd Command payload
     * @return Returns true if add successfully, otherwise false
     */
    bool AddCmdAttr(uint16_t type, const nfulnl_msg_config_cmd &cmd);

    /**
     * Add mode attr to NFLOG config message
     *
     * @param type Attr type
     * @param msg NFLOG config message buffer
     * @param mode Mode payload
     * @return Returns true if add successfully, otherwise false
     */
    bool AddModeAttr(uint16_t type, const nfulnl_msg_config_mode &mode);
#endif

    /**
     * Add ifinfomsg message to nlmsghdr
     *
     * @param action Action name
     * @param ifm Added ifinfomsg
     */
    void AddLink(uint16_t action, const struct ifinfomsg& ifm);

    /**
     * Begin adding nested attribute to nlmsghdr
     *
     * @param type Added the type of nested attribute
     */
    struct nlattr *AddNestedStart(int type);

    /**
     * End adding nested attribute to nlmsghdr
     *
     * @param nested Add the nested attribute to be ended
     */
    void AddNestedEnd(struct nlattr *nested);

private:
    std::unique_ptr<char[]> msghdrBuf_;
    struct nlmsghdr *netlinkMessage_;
    size_t maxBufLen_;
};
} // namespace nmd
} // namespace OHOS
#endif // !INCLUDE_NETLINK_MSG_H

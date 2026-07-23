/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_INTERFACE_MANAGER_H
#define INCLUDE_INTERFACE_MANAGER_H

#include "interface_type.h"
#include <ostream>
#include <string>
#include <vector>
#include "net/if_arp.h"
#include "netlink_msg.h"
#include "net_ip_mac_info.h"

namespace OHOS {
namespace nmd {
static const uint32_t INTERFACE_ERR_MAX_LEN = 256;
constexpr int32_t MAC_ADDRESS_INT_LEN = 6;

class InterfaceManager {
public:
    InterfaceManager() = default;
    ~InterfaceManager() = default;
    /**
     * Set network device mtu
     *
     * @param interfaceName Network device name
     * @param mtuValue Value of mtu
     * @return Returns 0, set network device mtu successfully, otherwise it will fail
     */
    static int SetMtu(const char *interfaceName, const char *mtuValue);

    /**
     * Get network device mtu
     *
     * @param interfaceName Network device name
     * @return Returns value of mtu
     */
    static int GetMtu(const char *interfaceName);

    /**
     * Add local IP address to network
     *
     * @param interfaceName Network device name
     * @param addr Network IP address
     * @param prefixLen Length of the network number of the subnet mask
     * @return Returns 0, add local IP address to network successfully, otherwise it will fail
     */
    static int AddAddress(const char *interfaceName, const char *addr, int prefixLen);

    /**
     * Delete local IP address to network
     *
     * @param interfaceName Network device name
     * @param addr Network IP address
     * @param prefixLen Length of the network number of the subnet mask
     * @return Returns 0, delete local IP address to network successfully, otherwise it will fail
     */
    static int DelAddress(const char *interfaceName, const char *addr, int prefixLen);

    /**
     * Delete local IP address to network
     *
     * @param interfaceName Network device name
     * @param addr Network IP address
     * @param prefixLen Length of the network number of the subnet mask
     * @param netCapabilities Net capabilities in string format
     * @return Returns 0, delete local IP address to network successfully, otherwise it will fail
     */
    static int DelAddress(const char *interfaceName, const char *addr, int prefixLen,
                          int socketType);

    /**
     * Get the network interface names
     *
     * @return Network interface names
     */
    static std::vector<std::string> GetInterfaceNames();

    /**
     * Get the network interface config
     *
     * @param ifName Network device name
     * @return Interface configuration parcel
     */
    static InterfaceConfigurationParcel GetIfaceConfig(const std::string &ifName);

    /**
     * Set network interface config
     *
     * @param ifaceConfig Interface configuration parcel
     * @return Returns 1, set network interface config successfully, otherwise it will fail
     */
    static int SetIfaceConfig(const nmd::InterfaceConfigurationParcel &ifaceConfig);

    /**
     * Set network interface ip address
     *
     * @param ifaceName Network port device name
     * @param ipAddress Ip address
     * @return Returns 0, set IP address to network successfully, otherwise it will fail
     */
    static int SetIpAddress(const std::string &ifaceName, const std::string &ipAddress);

    /**
     * Set iface up
     *
     * @param ifaceName Network port device name
     * @return Returns 0, set up to network successfully, otherwise it will fail
     */
    static int SetIffUp(const std::string &ifaceName);
    static int32_t AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                const std::string &ifName);
    static int32_t DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                const std::string &ifName);
    static int32_t AddStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName);
    static int32_t DelStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName);
    static int32_t SetIpv6AutoConf(const std::string &ipAddr, const uint32_t on);
    static int32_t GetIpNeighTable(std::vector<NetManagerStandard::NetIpMacInfo> &ipMacInfo);

    static int32_t CreateVlan(const std::string &ifName, uint32_t vlanId);
    static int32_t DestroyVlan(const std::string &ifName, uint32_t vlanId);
    static int32_t AddVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask);

private:
    static int ModifyAddress(uint32_t action, const char *interfaceName, const char *addr, int prefixLen);
    static int32_t AssembleArp(const std::string &ipAddr, const std::string &macAddr,
                               const std::string &ifName, arpreq &req);
    static int32_t AssembleIPv6Neighbor(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName, nmd::NetlinkMsg &nlmsg, uint16_t action);
    static int32_t MacStringToArray(const std::string &macAddr, sockaddr &macSock);
    static int32_t MacStringToBinary(const std::string &macAddr, uint8_t (&macBin)[MAC_ADDRESS_INT_LEN]);
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_INTERFACE_MANAGER_H

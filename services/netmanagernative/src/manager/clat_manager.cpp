/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <map>
#include <net/if.h>
#include <sys/socket.h>
#include <tuple>
#include <utility>

#include "clat_constants.h"
#include "clat_manager.h"
#include "clat_utils.h"
#include "clatd.h"
#include "fwmark.h"
#include "net_manager_constants.h"
#include "net_manager_native.h"
#include "netnative_log_wrapper.h"
#include "network_permission.h"
#include "iptables_wrapper.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;
ClatManager::ClatManager() = default;

// commands of create tables
constexpr const char *CREATE_CLAT_OUTPUT = "-t raw -N ";

// commands of set nat
constexpr const char *APPEND_CLAT_OUTPUT = "-I OUTPUT -j ";
constexpr const char *CLEAR_CLAT_OUTPUT_RULE = "-F "; //清除链中所有规则
constexpr const char *DELETE_CLAT_OUTPUT_CHAIN = "-D OUTPUT -j "; // 拆除指定链挂载
constexpr const char *DELETE_CLAT_CHAIN = "-X "; // 删除指定链

constexpr const char *RAW_TABLE = "*raw";
constexpr const char *CMD_COMMIT = "COMMIT";

const std::string ClatManager::GetClatNetChains(const std::string &v6Iface)
{
    return "clat_raw_" + v6Iface + "_OUTPUT";
}

const std::string ClatManager::EnableByPassNatCmd(const std::string &v6Iface, const std::string &v6Ip) // 新链上执行规则
{
    return "-A " + GetClatNetChains(v6Iface) + " -p all -s " + v6Ip + " -j NOTRACK";
}

void ClatManager::CombineRestoreRules(const std::string &cmds, std::string &cmdSet)
{
    cmdSet.append(cmds + "\n");
}

int32_t ClatManager::AddNatBypassRules(const std::string &v6Iface, const std::string &v6Ip)
{
    NETNATIVE_LOGI("AddNatBypassRules");
    DeleteNatBypassRules(v6Iface);
    auto ret = IptablesWrapper::GetInstance()->RunCommand(
        IPTYPE_IPV6, CREATE_CLAT_OUTPUT + GetClatNetChains(v6Iface)); // 创建子链
    // LCOV_EXCL_START
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("AddNatBypassRules create run command failed");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    // LCOV_EXCL_STOP
    std::string cmdSet = "";
    CombineRestoreRules(RAW_TABLE, cmdSet);
    CombineRestoreRules(APPEND_CLAT_OUTPUT + GetClatNetChains(v6Iface), cmdSet);
    CombineRestoreRules(EnableByPassNatCmd(v6Iface, v6Ip), cmdSet);
    CombineRestoreRules(CMD_COMMIT, cmdSet);
    // LCOV_EXCL_START
    ret = IptablesWrapper::GetInstance()->RunRestoreCommands(IPTYPE_IPV6, cmdSet);
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        DeleteNatBypassRules(v6Iface);
        NETNATIVE_LOGE("AddNatBypassRules run command failed");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t ClatManager::DeleteNatBypassRules(const std::string &v6Iface)
{
    NETNATIVE_LOGI("DeleteNatBypassRules %{public}s", v6Iface.c_str());
    // LCOV_EXCL_STOP
    std::string cmdSet = "";
    CombineRestoreRules(RAW_TABLE, cmdSet);
    CombineRestoreRules(CLEAR_CLAT_OUTPUT_RULE + GetClatNetChains(v6Iface), cmdSet); // 删除链上所有规则
    CombineRestoreRules(DELETE_CLAT_OUTPUT_CHAIN + GetClatNetChains(v6Iface), cmdSet); // 拆除挂载
    CombineRestoreRules(DELETE_CLAT_CHAIN + GetClatNetChains(v6Iface), cmdSet); // 删除链
    CombineRestoreRules(CMD_COMMIT, cmdSet);
    auto ret = IptablesWrapper::GetInstance()->RunRestoreCommands(IPTYPE_IPV6, cmdSet);
    // LCOV_EXCL_START
    if (ret != NetManagerStandard::NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("DeleteNatBypassRules run command failed");
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t ClatManager::AddClatRoute(int32_t netId, const std::string &tunIface, const std::string &v4Addr,
                                  NetManagerNative *netsysService)
{
    NETNATIVE_LOGI("AddClatRoute for %{public}s", tunIface.c_str());
    // LCOV_EXCL_START
    std::string tunIfaceName = tunIface;
    auto ret = netsysService->NetworkAddInterface(netId, tunIfaceName, BEARER_DEFAULT);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("NetworkAddInterface failed for %{public}s", tunIface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    ret = netsysService->NetworkAddRoute(netId, tunIface, DEFAULT_V4_ADDR, v4Addr, false);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("AddClatRoute failed for %{public}s", tunIface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    // LCOV_EXCL_STOP
    return NETMANAGER_SUCCESS;
}

int32_t ClatManager::DeleteClatRoute(int32_t netId, const std::string &tunIface, const std::string &v4Addr,
                                     NetManagerNative *netsysService)
{
    NETNATIVE_LOGI("DeleteClatRoute for %{public}s", tunIface.c_str());
    netsysService->NetworkRemoveRoute(netId, tunIface, DEFAULT_V4_ADDR, v4Addr, false);

    std::string tunIfaceName = tunIface;
    netsysService->NetworkRemoveInterface(netId, tunIfaceName);
    return NETMANAGER_SUCCESS;
}

int32_t ClatManager::ClatStart(const std::string &v6Iface, int32_t netId, const std::string &nat64PrefixStr,
                               NetManagerNative *netsysService)
{
    NETNATIVE_LOGI("Start Clatd on %{public}s", v6Iface.c_str());
    if (clatdTrackers_.find(v6Iface) != clatdTrackers_.end()) {
        NETNATIVE_LOGW("Clatd is already running on %{public}s", v6Iface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    if (netsysService == nullptr) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }

    uint32_t fwmark = GetFwmark(netId);
    INetAddr v4Addr;
    INetAddr v6Addr;
    int32_t ret = GenerateClatSrcAddr(v6Iface, fwmark, nat64PrefixStr, v4Addr, v6Addr);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("Fail to get source addresses for clat");
        return ret;
    }

    int tunFd = -1;
    std::string tunIface = std::string(CLAT_PREFIX) + v6Iface;
    ret = CreateAndConfigureTunIface(v6Iface, tunIface, v4Addr, netsysService, tunFd);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("Fail to create and configure tun interface for clat");
        return ret;
    }

    int readSock6 = -1;
    int writeSock6 = -1;
    ret = CreateAndConfigureClatSocket(v6Iface, v6Addr, fwmark, readSock6, writeSock6);
    if (ret != NETMANAGER_SUCCESS) {
        close(tunFd);
        NETNATIVE_LOGW("Fail to create and configure read/write sockets for clat");
        return ret;
    }

    clatds_.emplace(
        std::piecewise_construct, std::forward_as_tuple(v6Iface),
        std::forward_as_tuple(tunFd, readSock6, writeSock6, v6Iface, nat64PrefixStr, v4Addr.address_, v6Addr.address_));
    clatds_[v6Iface].Start();

    // LCOV_EXCL_START
    ret = AddClatRoute(netId, tunIface, v4Addr.address_, netsysService);
    auto netRet = AddNatBypassRules(v6Iface, v6Addr.address_);
    if (ret != NETMANAGER_SUCCESS || netRet != NETMANAGER_SUCCESS) {
        close(tunFd);
        close(readSock6);
        close(writeSock6);
        clatds_.erase(v6Iface);
        NETNATIVE_LOGW("Add route on %{public}s failed", tunIface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    // LCOV_EXCL_STOP
    netsysService->SetClatDnsEnableIpv4(netId, true);
    clatdTrackers_[v6Iface] = {v6Iface, tunIface, v4Addr, v6Addr, nat64PrefixStr, tunFd, readSock6, writeSock6, netId};

    return NETMANAGER_SUCCESS;
}

int32_t ClatManager::ClatStop(const std::string &v6Iface, NetManagerNative *netsysService)
{
    NETNATIVE_LOGI("Stop Clatd on %{public}s", v6Iface.c_str());
    if (clatdTrackers_.find(v6Iface) == clatdTrackers_.end()) {
        NETNATIVE_LOGW("Clatd has not started on %{public}s", v6Iface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    if (netsysService == nullptr) {
        NETNATIVE_LOGW("NetManagerNative pointer is null");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    // LCOV_EXCL_START
    int32_t netId = clatdTrackers_[v6Iface].netId;
    std::string tunIface = clatdTrackers_[v6Iface].tunIface;
    std::string v4Addr = clatdTrackers_[v6Iface].v4Addr.address_;

    DeleteNatBypassRules(v6Iface);
    NETNATIVE_LOGI("Stopping clatd on %{public}s", v6Iface.c_str());
    netsysService->SetClatDnsEnableIpv4(netId, false);
    DeleteClatRoute(netId, tunIface, v4Addr, netsysService);
    // LCOV_EXCL_STOP

    clatds_[v6Iface].Stop();
    clatds_.erase(v6Iface);

    FreeTunV4Addr(clatdTrackers_[v6Iface].v4Addr.address_);

    close(clatdTrackers_[v6Iface].tunFd);
    close(clatdTrackers_[v6Iface].readSock6);
    close(clatdTrackers_[v6Iface].writeSock6);
    clatdTrackers_.erase(v6Iface);

    NETNATIVE_LOGI("clatd on %{public}s stopped", v6Iface.c_str());
    return NETMANAGER_SUCCESS;
}

uint32_t ClatManager::GetFwmark(int32_t netId)
{
    Fwmark mark;
    mark.netId = static_cast<uint16_t>(netId);
    mark.explicitlySelected = true;
    mark.protectedFromVpn = true;
    NetworkPermission permission = NetworkPermission::PERMISSION_SYSTEM;
    mark.permission = permission;
    mark.uidBillingDone = false;
    return mark.intValue;
}

int32_t ClatManager::GenerateClatSrcAddr(const std::string &v6Iface, uint32_t fwmark, const std::string &nat64PrefixStr,
                                         INetAddr &v4Addr, INetAddr &v6Addr)
{
    std::string v4AddrStr;
    int32_t ret = SelectIpv4Address(std::string(INIT_V4ADDR_STRING), INIT_V4ADDR_PREFIX_BIT_LEN, v4AddrStr);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("no IPv4 addresses were available for clat");
        return ret;
    }
    v4Addr.type_ = INetAddr::IPV4;
    v4Addr.family_ = AF_INET;
    v4Addr.address_ = v4AddrStr;

    std::string v6AddrStr;
    ret = GenerateIpv6Address(v6Iface, v4AddrStr, nat64PrefixStr, fwmark, v6AddrStr);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("no IPv6 addresses were available for clat");
        return ret;
    }
    v6Addr.type_ = INetAddr::IPV6;
    v6Addr.family_ = AF_INET6;
    v6Addr.address_ = v6AddrStr;

    return NETMANAGER_SUCCESS;
}

int32_t ClatManager::CreateAndConfigureTunIface(const std::string &v6Iface, const std::string &tunIface,
                                                const INetAddr &v4Addr, NetManagerNative *netsysService, int &tunFd)
{
    int32_t ret = CreateTunInterface(tunIface, tunFd);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("Create tun interface %{public}s failed", tunIface.c_str());
        return ret;
    }

    uint32_t tunIfIndex = if_nametoindex(tunIface.c_str());
    if (tunIfIndex == INVALID_IFINDEX) {
        close(tunFd);
        NETNATIVE_LOGW("Fail to get interface index for interface %{public}s", tunIface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    ret = netsysService->SetEnableIpv6(tunIface, 0, false);
    if (ret != NETMANAGER_SUCCESS) {
        close(tunFd);
        NETNATIVE_LOGW("SetEnableIpv6 on %{public}s failed", tunIface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int mtu = CLAT_IPV6_MIN_MTU - MTU_DELTA;
    ret = netsysService->SetInterfaceMtu(tunIface, mtu);
    if (ret != NETMANAGER_SUCCESS) {
        close(tunFd);
        NETNATIVE_LOGW("Set MTU on %{public}s failed", tunIface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int v4AddrPrefixLen = V4ADDR_BIT_LEN;

    OHOS::nmd::InterfaceConfigurationParcel ifConfig;
    ifConfig.ifName = tunIface;
    ifConfig.hwAddr = "";
    ifConfig.ipv4Addr = v4Addr.address_;
    ifConfig.prefixLength = v4AddrPrefixLen;

    ifConfig.flags.emplace_back(IFACE_LINK_UP);
    netsysService->SetInterfaceConfig(ifConfig);

    ret = SetTunInterfaceAddress(tunIface, v4Addr.address_, v4AddrPrefixLen);
    if (ret != NETMANAGER_SUCCESS) {
        close(tunFd);
        NETNATIVE_LOGW("Set tun interface address on %{public}s failed", tunIface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return NETMANAGER_SUCCESS;
}

int32_t ClatManager::CreateAndConfigureClatSocket(const std::string &v6Iface, const INetAddr &v6Addr, uint32_t fwmark,
                                                  int &readSock6, int &writeSock6)
{
    int32_t ret = OpenPacketSocket(readSock6);
    if (ret != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGW("Open packet socket failed");
        return ret;
    }

    ret = OpenRawSocket6(fwmark, writeSock6);
    if (ret != NETMANAGER_SUCCESS) {
        close(readSock6);
        NETNATIVE_LOGW("Open raw socket failed");
        return ret;
    }

    uint32_t v6IfIndex = if_nametoindex(v6Iface.c_str());
    if (v6IfIndex == INVALID_IFINDEX) {
        close(readSock6);
        close(writeSock6);
        NETNATIVE_LOGW("Fail to get interface index for interface %{public}s", v6Iface.c_str());
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    ret = ConfigureWriteSocket(writeSock6, v6Iface);
    if (ret != NETMANAGER_SUCCESS) {
        close(readSock6);
        close(writeSock6);
        NETNATIVE_LOGW("Configure write sockopt failed");
        return ret;
    }

    ret = ConfigureReadSocket(readSock6, v6Addr.address_, v6IfIndex);
    if (ret != NETMANAGER_SUCCESS) {
        close(readSock6);
        close(writeSock6);
        NETNATIVE_LOGW("Configure read socket failed");
        return ret;
    }
    return NETMANAGER_SUCCESS;
}
} // namespace nmd
} // namespace OHOS
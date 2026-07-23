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

#include "distributed_manager.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/ipv6.h>

#include "init_socket.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {
constexpr const char *IP_CMD_PATH = "/system/bin/ip";
} // namespace

void DistributedManager::SetServerNicInfo(const std::string &iif, const std::string &devIface)
{
    serverIif_ = iif;
    serverDevIface_ = devIface;
}

std::string DistributedManager::GetServerIifNic()
{
    return serverIif_;
}

std::string DistributedManager::GetServerDevIfaceNic()
{
    return serverDevIface_;
}

int32_t DistributedManager::ConfigVirnicAndVeth(const std::string &virNicAddr, const std::string &virnicName,
    const std::string &virnicVethName)
{
    if (virnicName.empty() || virnicVethName.empty()) {
        NETNATIVE_LOGE("NicName is nullptr");
        return NETMANAGER_ERROR;
    }

    if (!CommonUtils::IsValidIPV4(virNicAddr)) {
        NETNATIVE_LOGE("the virNicAddr is not valid");
        return NETMANAGER_ERROR;
    }
    
    // Step1: ip link add virnic type veth peer name virnic1
    std::string out;
    std::string createVirnic = std::string(IP_CMD_PATH) + " link add " + virnicName +
        " type veth peer name " + virnicVethName;
    NETNATIVE_LOGI("setup virnic and veth : %{public}s", createVirnic.c_str());
    if (CommonUtils::ForkExec(createVirnic.c_str(), &out) != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("setup virnic and veth failed, output %{public}s", out.c_str());
        return NETMANAGER_ERROR;
    }
 
    // Step2-1: ip link set virnic up
    std::string virnicUp = std::string(IP_CMD_PATH) + " link set " + virnicName + " up";
    NETNATIVE_LOGI("set virnic up: %{public}s", virnicUp.c_str());
    if (CommonUtils::ForkExec(virnicUp.c_str(), &out) != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("set virnic up, output: %{public}s.", out.c_str());
        return NETMANAGER_ERROR;
    }
    // Step2-2: ip link set virnic-veth up
    std::string virnicVethUp = std::string(IP_CMD_PATH) + " link set " + virnicVethName + " up";
    NETNATIVE_LOGI("set virnicVeth up: %{public}s", virnicVethUp.c_str());
    if (CommonUtils::ForkExec(virnicVethUp.c_str(), &out) != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("set virnicVeth up failed, output: %{public}s.", out.c_str());
        return NETMANAGER_ERROR;
    }
 
    // Step3-1: ip addr add xx.xx.xx.xx/24 dev virnic
    std::string cfgVirnic = std::string(IP_CMD_PATH) + " addr add " + virNicAddr + "/24 dev " + virnicName;
    NETNATIVE_LOGI("add virnic ip: %{public}s", CommonUtils::AnonymousIpInStr(cfgVirnic).c_str());
    if (CommonUtils::ForkExec(cfgVirnic.c_str(), &out) != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("add virnic ip failed, output %{public}s.", out.c_str());
        return NETMANAGER_ERROR;
    }
    std::string maskAddr = CommonUtils::GetMaskByLength(DEFAULT_GATEWAY_MASK_MAX_LENGTH);
    std::string virNicVethAddr = CommonUtils::GetGatewayAddr(virNicAddr, maskAddr);
    if (virNicVethAddr.empty()) {
        NETNATIVE_LOGE("get gateway addr is empty");
        return NETMANAGER_ERROR;
    }

    // Step3-1: ip addr add xx.xx.xx.1/24 dev virnic
    std::string cfgVirnicVeth = std::string(IP_CMD_PATH) + " addr add " + virNicVethAddr + "/24 dev " + virnicVethName;
    NETNATIVE_LOGI("add virNic-veth ip: %{public}s", CommonUtils::AnonymousIpInStr(cfgVirnicVeth).c_str());
    if (CommonUtils::ForkExec(cfgVirnicVeth.c_str(), &out) != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("add virNic-veth ip failed, output %{public}s.", out.c_str());
        return NETMANAGER_ERROR;
    }
 
    return NETMANAGER_SUCCESS;
}
 
void DistributedManager::DisableVirnic(const std::string &virnicName)
{
    std::string out;
    std::string delVirnic = std::string(IP_CMD_PATH) + " link del " + virnicName;
    NETNATIVE_LOGI("del virnic: %{public}s", delVirnic.c_str());
    if (CommonUtils::ForkExec(delVirnic.c_str(), &out) != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("DisableVirnic del Virnic failed, output %{public}s", out.c_str());
    }
}
} // namespace NetManagerStandard
} // namespace OHOS

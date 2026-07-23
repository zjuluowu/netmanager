/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include "netsys_controller.h"

#include "net_conn_constants.h"
#include "net_conn_types.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"
#include "netsys_controller_service_impl.h"
#include "i_net_dns_result_callback.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t IPV4_MAX_LENGTH = 32;
static constexpr int32_t MAX_ROUTES_PER_SLICE_COUNT = 1024;

NetsysController::NetsysController()
{
    NETMGR_LOG_I("netsys Init");
#ifndef NETMANAGER_TEST
    netsysService_ = std::make_unique<NetsysControllerServiceImpl>().release();
    netsysService_->Init();
#endif
}

NetsysController &NetsysController::GetInstance()
{
    static NetsysController singleInstance_;
    return singleInstance_;
}

int32_t NetsysController::SetInternetPermission(uint32_t uid, uint8_t allow)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetInternetPermission(uid, allow);
}

int32_t NetsysController::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    NETMGR_LOG_I("Create Physical network: netId[%{public}d], permission[%{public}d]", netId, permission);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetworkCreatePhysical(netId, permission);
}

int32_t NetsysController::NetworkCreateVirtual(int32_t netId, bool hasDns)
{
    NETMGR_LOG_I("Create Virtual network: netId[%{public}d], hasDns[%{public}d]", netId, hasDns);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetworkCreateVirtual(netId, hasDns);
}

int32_t NetsysController::NetworkDestroy(int32_t netId, bool isVpnNet)
{
    NETMGR_LOG_I("Destroy network: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetworkDestroy(netId, isVpnNet);
}

int32_t NetsysController::CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix,
                                     const std::set<int32_t> &uids)
{
    NETMGR_LOG_I("Create Vnic network");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->CreateVnic(mtu, tunAddr, prefix, uids);
}

int32_t NetsysController::DestroyVnic()
{
    NETMGR_LOG_I("Destroy Vnic network");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DestroyVnic();
}

int32_t NetsysController::EnableDistributedClientNet(const std::string &virnicAddr,
    const std::string &virnicName, const std::string &iif)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
}

int32_t NetsysController::EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                                     const std::string &dstAddr, const std::string &gw)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
}

int32_t NetsysController::DisableDistributedNet(
    bool isServer, const std::string &virnicName, const std::string &dstAddr)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->DisableDistributedNet(isServer, virnicName, dstAddr);
}

int32_t NetsysController::NetworkAddUids(int32_t netId, const std::vector<int32_t> &beginUids,
                                         const std::vector<int32_t> &endUids, uint32_t priorityId)
{
    NETMGR_LOG_I("Destroy network: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    if (beginUids.size() != endUids.size()) {
        NETMGR_LOG_E("beginUids and endUids size is mismatch");
        return NETMANAGER_ERR_INTERNAL;
    }
    std::vector<UidRange> uidRanges;
    for (size_t i = 0; i < beginUids.size(); i++) {
        uidRanges.emplace_back(UidRange(beginUids[i], endUids[i], priorityId, netId));
    }
    return netsysService_->NetworkAddUids(netId, uidRanges);
}

int32_t NetsysController::NetworkDelUids(int32_t netId, const std::vector<int32_t> &beginUids,
                                         const std::vector<int32_t> &endUids, uint32_t priorityId)
{
    NETMGR_LOG_I("Destroy network: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    if (beginUids.size() != endUids.size()) {
        NETMGR_LOG_E("beginUids and endUids size is mismatch");
        return NETMANAGER_ERR_INTERNAL;
    }
    std::vector<UidRange> uidRanges;
    for (size_t i = 0; i < beginUids.size(); i++) {
        uidRanges.emplace_back(UidRange(beginUids[i], endUids[i], priorityId, netId));
    }
    return netsysService_->NetworkDelUids(netId, uidRanges);
}

int32_t NetsysController::NetworkAddInterface(int32_t netId, const std::string &iface, NetBearType netBearerType)
{
    NETMGR_LOG_I("Add network interface: netId[%{public}d], iface[%{public}s, bearerType[%{public}u]]", netId,
                 iface.c_str(), netBearerType);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetworkAddInterface(netId, iface, netBearerType);
}

int32_t NetsysController::NetworkRemoveInterface(int32_t netId, const std::string &iface)
{
    NETMGR_LOG_I("Remove network interface: netId[%{public}d], iface[%{public}s]", netId, iface.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetworkRemoveInterface(netId, iface);
}

int32_t NetsysController::NetworkAddRoute(int32_t netId, const std::string &ifName, const std::string &destination,
                                          const std::string &nextHop, bool isExcludedRoute)
{
    NETMGR_LOG_D("Add Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s] \
        isExcludedRoute[%{public}d]", netId, ifName.c_str(), ToAnonymousIp(destination).c_str(),
        ToAnonymousIp(nextHop).c_str(), isExcludedRoute);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetworkAddRoute(netId, ifName, destination, nextHop, isExcludedRoute);
}

int32_t NetsysController::NetworkAddRoutes(int32_t netId, const std::vector<nmd::NetworkRouteInfo> &infos)
{
    NETMGR_LOG_D("Add Routes: netId[%{public}d], routeCount[%{public}zu]", netId, infos.size());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    if (infos.empty()) {
        return NETMANAGER_SUCCESS;
    }
    size_t totalRoutes = infos.size();
    int32_t finalResult = NETMANAGER_SUCCESS;

    for (size_t i = 0; i < totalRoutes; i += MAX_ROUTES_PER_SLICE_COUNT) {
        size_t end = std::min(i + MAX_ROUTES_PER_SLICE_COUNT, totalRoutes);
        std::vector<nmd::NetworkRouteInfo> currentSlice(infos.begin() + i, infos.begin() + end);
        int32_t ret = netsysService_->NetworkAddRoutes(netId, currentSlice);
        if (ret != NETMANAGER_SUCCESS) {
            finalResult = ret;
            break;
        }
    }
    return finalResult;
}
int32_t NetsysController::NetworkRemoveRoute(int32_t netId, const std::string &ifName, const std::string &destination,
    const std::string &nextHop, bool isExcludedRoute)
{
    NETMGR_LOG_D("Remove Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s] \
        isExcludedRoute[%{public}d]", netId, ifName.c_str(), ToAnonymousIp(destination).c_str(),
        ToAnonymousIp(nextHop).c_str(), isExcludedRoute);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetworkRemoveRoute(netId, ifName, destination, nextHop, isExcludedRoute);
}

int32_t NetsysController::GetInterfaceConfig(OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    NETMGR_LOG_D("get interface config");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetInterfaceConfig(cfg);
}

int32_t NetsysController::SetInterfaceConfig(const OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    NETMGR_LOG_I("set interface config");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetInterfaceConfig(cfg);
}

int32_t NetsysController::SetInterfaceDown(const std::string &iface)
{
    NETMGR_LOG_I("Set interface down: iface[%{public}s]", iface.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetInterfaceDown(iface);
}

int32_t NetsysController::SetInterfaceUp(const std::string &iface)
{
    NETMGR_LOG_I("Set interface up: iface[%{public}s]", iface.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetInterfaceUp(iface);
}

void NetsysController::ClearInterfaceAddrs(const std::string &ifName)
{
    NETMGR_LOG_I("Clear addrs: ifName[%{public}s]", ifName.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return;
    }
    // LCOV_EXCL_STOP
    return netsysService_->ClearInterfaceAddrs(ifName);
}

int32_t NetsysController::GetInterfaceMtu(const std::string &ifName)
{
    NETMGR_LOG_I("Get mtu: ifName[%{public}s]", ifName.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetInterfaceMtu(ifName);
}

int32_t NetsysController::SetInterfaceMtu(const std::string &ifName, int32_t mtu)
{
    NETMGR_LOG_I("Set mtu: ifName[%{public}s], mtu[%{public}d]", ifName.c_str(), mtu);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetInterfaceMtu(ifName, mtu);
}

int32_t NetsysController::SetTcpBufferSizes(const std::string &tcpBufferSizes)
{
    NETMGR_LOG_I("Set tcp buffer sizes: tcpBufferSizes[%{public}s]", tcpBufferSizes.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetTcpBufferSizes(tcpBufferSizes);
}

int32_t NetsysController::AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                              int32_t prefixLength)
{
    NETMGR_LOG_I("Add address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]",
        ifName.c_str(), ToAnonymousIp(ipAddr).c_str(), prefixLength);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->AddInterfaceAddress(ifName, ipAddr, prefixLength);
}

int32_t NetsysController::DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                              int32_t prefixLength)
{
    NETMGR_LOG_I("Delete address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]",
        ifName.c_str(), ToAnonymousIp(ipAddr).c_str(), prefixLength);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DelInterfaceAddress(ifName, ipAddr, prefixLength);
}

int32_t NetsysController::DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                              int32_t prefixLength, int socketType)
{
    NETMGR_LOG_I("Delete address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]",
        ifName.c_str(), ToAnonymousIp(ipAddr).c_str(), prefixLength);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DelInterfaceAddress(ifName, ipAddr, prefixLength, socketType);
}

int32_t NetsysController::InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    NETMGR_LOG_D("Set Ip Address: ifName[%{public}s]", ifaceName.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->InterfaceSetIpAddress(ifaceName, ipAddress);
}

int32_t NetsysController::InterfaceSetIffUp(const std::string &ifaceName)
{
    NETMGR_LOG_D("Set Iff Up: ifName[%{public}s]", ifaceName.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->InterfaceSetIffUp(ifaceName);
}

int32_t NetsysController::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                            const std::vector<std::string> &servers,
                                            const std::vector<std::string> &domains)
{
    NETMGR_LOG_I("Set resolver config: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
}

int32_t NetsysController::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                            std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                            uint8_t &retryCount)
{
    NETMGR_LOG_I("Get resolver config: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
}

int32_t NetsysController::CreateNetworkCache(uint16_t netId, bool isVpnNet)
{
    NETMGR_LOG_I("create dns cache: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->CreateNetworkCache(netId, isVpnNet);
}

int32_t NetsysController::DestroyNetworkCache(uint16_t netId, bool isVpnNet)
{
    NETMGR_LOG_I("Destroy dns cache: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DestroyNetworkCache(netId, isVpnNet);
}

int32_t NetsysController::GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                                      uint16_t netId, std::vector<AddrInfo> &res)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NET_CONN_ERR_SERVICE_UPDATE_NET_LINK_INFO_FAIL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetAddrInfo(hostName, serverName, hints, netId, res);
}

int32_t NetsysController::GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                                   nmd::NetworkSharingTraffic &traffic)
{
    NETMGR_LOG_I("NetsysController GetNetworkSharingTraffic");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetNetworkSharingTraffic(downIface, upIface, traffic);
}

int32_t NetsysController::GetNetworkCellularSharingTraffic(nmd::NetworkSharingTraffic &traffic, std::string &ifaceName)
{
    NETMGR_LOG_D("NetsysController GetNetworkCellularSharingTraffic");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetNetworkCellularSharingTraffic(traffic, ifaceName);
}

int32_t NetsysController::SetDpaCellularSharingTraffic(nmd::NetworkDpaTrafficReport &traffic)
{
    // LCOV_EXCL_START
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetDpaCellularSharingTraffic(traffic);
}

int64_t NetsysController::GetCellularRxBytes()
{
    NETMGR_LOG_D("NetsysController GetCellularRxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetCellularRxBytes();
}

int64_t NetsysController::GetCellularTxBytes()
{
    NETMGR_LOG_D("NetsysController GetCellularTxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetCellularTxBytes();
}

int64_t NetsysController::GetAllRxBytes()
{
    NETMGR_LOG_D("NetsysController GetAllRxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetAllRxBytes();
}

int64_t NetsysController::GetAllTxBytes()
{
    NETMGR_LOG_D("NetsysController GetAllTxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetAllTxBytes();
}

int64_t NetsysController::GetUidRxBytes(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController GetUidRxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetUidRxBytes(uid);
}

int64_t NetsysController::GetUidTxBytes(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController GetUidTxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetUidTxBytes(uid);
}

int64_t NetsysController::GetUidOnIfaceRxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetUidOnIfaceRxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetUidOnIfaceRxBytes(uid, interfaceName);
}

int64_t NetsysController::GetUidOnIfaceTxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetUidOnIfaceTxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetUidOnIfaceTxBytes(uid, interfaceName);
}

int64_t NetsysController::GetIfaceRxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetIfaceRxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetIfaceRxBytes(interfaceName);
}

int64_t NetsysController::GetIfaceTxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetIfaceTxBytes");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetIfaceTxBytes(interfaceName);
}

std::vector<std::string> NetsysController::InterfaceGetList()
{
    NETMGR_LOG_I("InterfaceGetList");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return {};
    }
    // LCOV_EXCL_STOP
    return netsysService_->InterfaceGetList();
}

std::vector<std::string> NetsysController::UidGetList()
{
    NETMGR_LOG_I("UidGetList");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return {};
    }
    // LCOV_EXCL_STOP
    return netsysService_->UidGetList();
}

int64_t NetsysController::GetIfaceRxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetIfaceRxPackets");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetIfaceRxPackets(interfaceName);
}

int64_t NetsysController::GetIfaceTxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetIfaceTxPackets");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetIfaceTxPackets(interfaceName);
}

int32_t NetsysController::SetDefaultNetWork(int32_t netId)
{
    NETMGR_LOG_D("Set DefaultNetWork: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetDefaultNetWork(netId);
}

int32_t NetsysController::ClearDefaultNetWorkNetId()
{
    NETMGR_LOG_D("ClearDefaultNetWorkNetId");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->ClearDefaultNetWorkNetId();
}

int32_t NetsysController::IpEnableForwarding(const std::string &requestor)
{
    NETMGR_LOG_I("IpEnableForwarding: requestor[%{public}s]", requestor.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->IpEnableForwarding(requestor);
}

int32_t NetsysController::IpDisableForwarding(const std::string &requestor)
{
    NETMGR_LOG_I("IpDisableForwarding: requestor[%{public}s]", requestor.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->IpDisableForwarding(requestor);
}

int32_t NetsysController::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETMGR_LOG_I("EnableNat: intIface[%{public}s] intIface[%{public}s]", downstreamIface.c_str(),
                 upstreamIface.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->EnableNat(downstreamIface, upstreamIface);
}

int32_t NetsysController::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETMGR_LOG_I("DisableNat: intIface[%{public}s] intIface[%{public}s]",
                 downstreamIface.c_str(), upstreamIface.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DisableNat(downstreamIface, upstreamIface);
}

int32_t NetsysController::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    NETMGR_LOG_I("IpfwdAddInterfaceForward: fromIface[%{public}s], toIface[%{public}s]", fromIface.c_str(),
                 toIface.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->IpfwdAddInterfaceForward(fromIface, toIface);
}

int32_t NetsysController::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    NETMGR_LOG_I("IpfwdRemoveInterfaceForward: fromIface[%{public}s], toIface[%{public}s]", fromIface.c_str(),
                 toIface.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->IpfwdRemoveInterfaceForward(fromIface, toIface);
}

int32_t NetsysController::ShareDnsSet(uint16_t netId)
{
    NETMGR_LOG_I("ShareDnsSet: netId[%{public}d]", netId);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->ShareDnsSet(netId);
}

int32_t NetsysController::StartDnsProxyListen()
{
    NETMGR_LOG_I("StartDnsProxyListen");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->StartDnsProxyListen();
}

int32_t NetsysController::StopDnsProxyListen()
{
    NETMGR_LOG_I("StopDnsProxyListen");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->StopDnsProxyListen();
}

int32_t NetsysController::RegisterNetsysNotifyCallback(const NetsysNotifyCallback &callback)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->RegisterNetsysNotifyCallback(callback);
}

int32_t NetsysController::BindNetworkServiceVpn(int32_t socketFd)
{
    NETMGR_LOG_I("BindNetworkServiceVpn: socketFd[%{public}d]", socketFd);
    if (socketFd <= 0) {
        NETMGR_LOG_E("socketFd is null");
        return NETSYS_ERR_VPN;
    }
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->BindNetworkServiceVpn(socketFd);
}

int32_t NetsysController::EnableVirtualNetIfaceCard(int32_t socketFd, struct ifreq &ifRequest, int32_t &ifaceFd)
{
    NETMGR_LOG_I("EnableVirtualNetIfaceCard: socketFd[%{public}d]", socketFd);
    if (socketFd <= 0) {
        NETMGR_LOG_E("socketFd is null");
        return NETSYS_ERR_VPN;
    }
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->EnableVirtualNetIfaceCard(socketFd, ifRequest, ifaceFd);
}

int32_t NetsysController::SetIpAddress(int32_t socketFd, const std::string &ipAddress, int32_t prefixLen,
                                       struct ifreq &ifRequest)
{
    NETMGR_LOG_D("NetsysController::set addr");
    if ((socketFd <= 0) || (ipAddress.length() == 0) || (static_cast<uint32_t>(ipAddress.length()) > IPV4_MAX_LENGTH) ||
	    (prefixLen <= 0) || (static_cast<uint32_t>(prefixLen) > IPV4_MAX_LENGTH)) {
        NETMGR_LOG_E(
            "The paramemters of SetIpAddress is failed, socketFd[%{public}d], "
            "ipAddress[%{public}s], prefixLen[%{public}d].",
            socketFd, ToAnonymousIp(ipAddress).c_str(), prefixLen);
        return NETSYS_ERR_VPN;
    }
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetIpAddress(socketFd, ipAddress, prefixLen, ifRequest);
}

int32_t NetsysController::SetBlocking(int32_t ifaceFd, bool isBlock)
{
    NETMGR_LOG_D("NetsysController::SetBlocking: ifaceFd[%{public}d], isBlock[%{public}d]", ifaceFd, isBlock);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetBlocking(ifaceFd, isBlock);
}

int32_t NetsysController::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_I("StartDhcpClient: iface[%{public}s], bIpv6[%{public}d]", iface.c_str(), bIpv6);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->StartDhcpClient(iface, bIpv6);
}

int32_t NetsysController::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_I("StopDhcpClient: iface[%{public}s], bIpv6[%{public}d]", iface.c_str(), bIpv6);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->StopDhcpClient(iface, bIpv6);
}

int32_t NetsysController::RegisterCallback(sptr<NetsysControllerCallback> callback)
{
    NETMGR_LOG_D("NetsysController::RegisterCallback");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->RegisterCallback(callback);
}

int32_t NetsysController::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    NETMGR_LOG_I("StartDhcpService: iface[%{public}s], ipv4addr[%{public}s]",
        iface.c_str(), ToAnonymousIp(ipv4addr).c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->StartDhcpService(iface, ipv4addr);
}

int32_t NetsysController::StopDhcpService(const std::string &iface)
{
    NETMGR_LOG_I("StopDhcpService: ifaceFd[%{public}s]", iface.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->StopDhcpService(iface);
}

int32_t NetsysController::BandwidthEnableDataSaver(bool enable)
{
    NETMGR_LOG_D("NetsysController::BandwidthEnableDataSaver: enable=%{public}d", enable);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->BandwidthEnableDataSaver(enable);
}

int32_t NetsysController::BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    NETMGR_LOG_D("NetsysController::BandwidthSetIfaceQuota: ifName=%{public}s", ifName.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->BandwidthSetIfaceQuota(ifName, bytes);
}

int32_t NetsysController::BandwidthRemoveIfaceQuota(const std::string &ifName)
{
    NETMGR_LOG_D("NetsysController::BandwidthRemoveIfaceQuota: ifName=%{public}s", ifName.c_str());
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->BandwidthRemoveIfaceQuota(ifName);
}

int32_t NetsysController::BandwidthAddDeniedList(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController::BandwidthAddDeniedList: uid=%{public}d", uid);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->BandwidthAddDeniedList(uid);
}

int32_t NetsysController::BandwidthRemoveDeniedList(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController::BandwidthRemoveDeniedList: uid=%{public}d", uid);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->BandwidthRemoveDeniedList(uid);
}

int32_t NetsysController::BandwidthAddAllowedList(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController::BandwidthAddAllowedList: uid=%{public}d", uid);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->BandwidthAddAllowedList(uid);
}

int32_t NetsysController::BandwidthRemoveAllowedList(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController::BandwidthRemoveAllowedList: uid=%{public}d", uid);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->BandwidthRemoveAllowedList(uid);
}

int32_t NetsysController::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    NETMGR_LOG_I("NetsysController::FirewallSetUidsAllowedListChain: chain=%{public}d", chain);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->FirewallSetUidsAllowedListChain(chain, uids);
}

int32_t NetsysController::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    NETMGR_LOG_I("NetsysController::FirewallSetUidsDeniedListChain: chain=%{public}d", chain);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->FirewallSetUidsDeniedListChain(chain, uids);
}

int32_t NetsysController::FirewallEnableChain(uint32_t chain, bool enable)
{
    NETMGR_LOG_I("NetsysController::FirewallEnableChain: chain=%{public}d, enable=%{public}d", chain, enable);
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->FirewallEnableChain(chain, enable);
}

int32_t NetsysController::FirewallSetUidRule(uint32_t chain, const std::vector<uint32_t> &uids, uint32_t firewallRule)
{
    NETMGR_LOG_I("NetsysController::FirewallSetUidRule Start");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->FirewallSetUidRule(chain, uids, firewallRule);
}

void NetsysController::FreeAddrInfo(addrinfo *aihead)
{
    addrinfo *tmpNext = nullptr;
    for (addrinfo *tmp = aihead; tmp != nullptr;) {
        if (tmp->ai_addr != nullptr) {
            free(tmp->ai_addr);
        }
        if (tmp->ai_canonname != nullptr) {
            free(tmp->ai_canonname);
        }
        tmpNext = tmp->ai_next;
        free(tmp);
        tmp = tmpNext;
    }
}

int32_t NetsysController::GetTotalStats(uint64_t &stats, uint32_t type)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetTotalStats(stats, static_cast<uint32_t>(type));
}

int32_t NetsysController::GetUidStats(uint64_t &stats, uint32_t type, uint32_t uid)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetUidStats(stats, static_cast<uint32_t>(type), uid);
}

int32_t NetsysController::GetIfaceStats(uint64_t &stats, uint32_t type, const std::string &interfaceName)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetIfaceStats(stats, static_cast<uint32_t>(type), interfaceName);
}

int32_t NetsysController::GetAllSimStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetAllSimStatsInfo(stats);
}

int32_t NetsysController::DeleteSimStatsInfo(uint32_t uid)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DeleteSimStatsInfo(uid);
}

int32_t NetsysController::GetAllStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetAllStatsInfo(stats);
}

int32_t NetsysController::DeleteStatsInfo(uint32_t uid)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DeleteStatsInfo(uid);
}

int32_t NetsysController::SetNetStateTrafficMap(uint8_t flag, uint64_t availableTraffic)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetNetStateTrafficMap(flag, availableTraffic);
}

int32_t NetsysController::SetNetStatusMap(uint8_t type, uint8_t value)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetNetStatusMap(type, value);
}

int32_t NetsysController::GetNetStateTrafficMap(uint8_t flag, uint64_t &availableTraffic)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetNetStateTrafficMap(flag, availableTraffic);
}

int32_t NetsysController::ClearIncreaseTrafficMap()
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->ClearIncreaseTrafficMap();
}

int32_t NetsysController::ClearSimStatsBpfMap()
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->ClearSimStatsBpfMap();
}

int32_t NetsysController::DeleteIncreaseTrafficMap(uint64_t ifIndex)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }

    return netsysService_->DeleteIncreaseTrafficMap(ifIndex);
}

int32_t NetsysController::UpdateIfIndexMap(int8_t key, uint64_t index)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->UpdateIfIndexMap(key, index);
}

int32_t NetsysController::SetIptablesCommandForRes(const std::string &cmd, std::string &respond,
    NetsysNative::IptablesType ipType)
{
    if (cmd.empty()) {
        NETMGR_LOG_E("SetIptablesCommandForRes cmd is empty");
        return ERR_INVALID_DATA;
    }
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetIptablesCommandForRes netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    NETMGR_LOG_I("SetIptablesCommandForRes, iptables is %{public}d.", ipType);
    return netsysService_->SetIptablesCommandForRes(cmd, respond, ipType);
}

int32_t NetsysController::SetIpCommandForRes(const std::string &cmd, std::string &respond)
{
    if (cmd.empty()) {
        NETMGR_LOG_E("SetIpCommandForRes cmd is empty");
        return ERR_INVALID_DATA;
    }
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetIpCommandForRes netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    NETMGR_LOG_I("SetIpCommandForRes");
    return netsysService_->SetIpCommandForRes(cmd, respond);
}

int32_t NetsysController::NetDiagPingHost(const OHOS::NetsysNative::NetDiagPingOption &pingOption,
                                          const sptr<OHOS::NetsysNative::INetDiagCallback> &callback)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetDiagPingHost(pingOption, callback);
}

int32_t NetsysController::NetDiagGetRouteTable(std::list<OHOS::NetsysNative::NetDiagRouteTable> &routeTables)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetDiagGetRouteTable(routeTables);
}

int32_t NetsysController::NetDiagGetSocketsInfo(OHOS::NetsysNative::NetDiagProtocolType socketType,
                                                OHOS::NetsysNative::NetDiagSocketsInfo &socketsInfo)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetDiagGetSocketsInfo(socketType, socketsInfo);
}

int32_t NetsysController::NetDiagGetInterfaceConfig(std::list<OHOS::NetsysNative::NetDiagIfaceConfig> &configs,
                                                    const std::string &ifaceName)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetDiagGetInterfaceConfig(configs, ifaceName);
}

int32_t NetsysController::NetDiagUpdateInterfaceConfig(const OHOS::NetsysNative::NetDiagIfaceConfig &config,
                                                       const std::string &ifaceName, bool add)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetDiagUpdateInterfaceConfig(config, ifaceName, add);
}

int32_t NetsysController::NetDiagSetInterfaceActiveState(const std::string &ifaceName, bool up)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NetDiagSetInterfaceActiveState(ifaceName, up);
}

int32_t NetsysController::AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                       const std::string &ifName)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("AddStaticArp netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->AddStaticArp(ipAddr, macAddr, ifName);
}

int32_t NetsysController::DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                                       const std::string &ifName)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("DelStaticArp netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DelStaticArp(ipAddr, macAddr, ifName);
}

int32_t NetsysController::AddStaticIpv6Addr(const std::string &ipAddr, const std::string &macAddr,
    const std::string &ifName)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("AddStaticIpv6Addr netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
}

int32_t NetsysController::DelStaticIpv6Addr(const std::string &ipAddr, const std::string &macAddr,
    const std::string &ifName)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("DelStaticIpv6Addr netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
}

int32_t NetsysController::RegisterDnsResultCallback(
    const sptr<OHOS::NetManagerStandard::NetsysDnsReportCallback> &callback, uint32_t timeStep)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->RegisterDnsResultCallback(callback, timeStep);
}

int32_t NetsysController::UnregisterDnsResultCallback(
    const sptr<OHOS::NetManagerStandard::NetsysDnsReportCallback> &callback)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->UnregisterDnsResultCallback(callback);
}

int32_t NetsysController::RegisterDnsQueryResultCallback(
    const sptr<OHOS::NetManagerStandard::NetsysDnsQueryReportCallback> &callback)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->RegisterDnsQueryResultCallback(callback);
}

int32_t NetsysController::UnregisterDnsQueryResultCallback(
    const sptr<OHOS::NetManagerStandard::NetsysDnsQueryReportCallback> &callback)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->UnregisterDnsQueryResultCallback(callback);
}

int32_t NetsysController::GetCookieStats(uint64_t &stats, uint32_t type, uint64_t cookie)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("GetCookieStats netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetCookieStats(stats, type, cookie);
}

int32_t NetsysController::GetNetworkSharingType(std::set<uint32_t>& sharingTypeIsOn)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("GetNetworkSharingType netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetNetworkSharingType(sharingTypeIsOn);
}

int32_t NetsysController::UpdateNetworkSharingType(uint32_t type, bool isOpen)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("UpdateNetworkSharingType netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->UpdateNetworkSharingType(type, isOpen);
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
int32_t NetsysController::SetFirewallRules(NetFirewallRuleType type,
                                           const std::vector<sptr<NetFirewallBaseRule>> &ruleList, bool isFinish)
{
    NETMGR_LOG_I("NetsysController::SetFirewallRules");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetFirewallRules netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetFirewallRules(type, ruleList, isFinish);
}

int32_t NetsysController::SetFirewallDefaultAction(int32_t userId, FirewallRuleAction inDefault,
    FirewallRuleAction outDefault)
{
    NETMGR_LOG_I("NetsysController::SetFirewallDefaultAction");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetFirewallDefaultAction netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetFirewallDefaultAction(userId, inDefault, outDefault);
}

int32_t NetsysController::SetFirewallCurrentUserId(int32_t userId)
{
    NETMGR_LOG_I("NetsysController::SetFirewallCurrentUserId");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetFirewallCurrentUserId netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetFirewallCurrentUserId(userId);
}

int32_t NetsysController::ClearFirewallRules(NetFirewallRuleType type)
{
    NETMGR_LOG_I("NetsysController::ClearFirewallRules");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("ClearFirewallRules netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->ClearFirewallRules(type);
}

int32_t NetsysController::RegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->RegisterNetFirewallCallback(callback);
}

int32_t NetsysController::UnRegisterNetFirewallCallback(const sptr<NetsysNative::INetFirewallCallback> &callback)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->UnRegisterNetFirewallCallback(callback);
}
#endif

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
int32_t NetsysController::EnableWearableDistributedNetForward(const int32_t tcpPortId, const int32_t udpPortId)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("NetsysService is null in EnableWearableDistributedNetForward");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->EnableWearableDistributedNetForward(tcpPortId, udpPortId);
}

int32_t NetsysController::DisableWearableDistributedNetForward()
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("NetsysService is null in DisableWearableDistributedNetForward");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->DisableWearableDistributedNetForward();
}
#endif

int32_t NetsysController::RegisterNetsysTrafficCallback(const sptr<NetsysNative::INetsysTrafficCallback> &callback)
{
    NETMGR_LOG_E("RegisterNetsysTrafficCallback start");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->RegisterNetsysTrafficCallback(callback);
}

int32_t NetsysController::UnRegisterNetsysTrafficCallback(const sptr<NetsysNative::INetsysTrafficCallback> &callback)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->UnRegisterNetsysTrafficCallback(callback);
}

int32_t NetsysController::SetIpv6PrivacyExtensions(const std::string &interfaceName, const uint32_t on)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetIpv6PrivacyExtensions netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetIpv6PrivacyExtensions(interfaceName, on);
}

int32_t NetsysController::SetEnableIpv6(const std::string &interfaceName, const uint32_t on, bool needRestart)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetEnableIpv6 netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetEnableIpv6(interfaceName, on, needRestart);
}

int32_t NetsysController::SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetIpv6UidBlackList netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetIpv6UidBlackList(netIds, uid);
}

int32_t NetsysController::SetIpv6AutoConf(const std::string &interfaceName, const uint32_t on)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetIpv6AutoConf netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetIpv6AutoConf(interfaceName, on);
}

int32_t NetsysController::SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetNetworkAccessPolicy(uid, policy, reconfirmFlag);
}

int32_t NetsysController::NotifyNetBearerTypeChange(std::set<NetBearType> bearerTypes)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->NotifyNetBearerTypeChange(bearerTypes);
}

int32_t NetsysController::DeleteNetworkAccessPolicy(uint32_t uid)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->DeleteNetworkAccessPolicy(uid);
}

int32_t NetsysController::ClearFirewallAllRules()
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->ClearFirewallAllRules();
}

int32_t NetsysController::StartClat(const std::string &interfaceName, int32_t netId, const std::string &nat64PrefixStr)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("StartClat netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->StartClat(interfaceName, netId, nat64PrefixStr);
}

int32_t NetsysController::StopClat(const std::string &interfaceName)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("StopClat netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->StopClat(interfaceName);
}

int32_t NetsysController::SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetNicTrafficAllowed netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->SetNicTrafficAllowed(ifaceNames, status);
}

int32_t NetsysController::SetUserDefinedServerFlag(uint16_t netId, bool isUserDefinedServer)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("SetUserDefinedServerFlag netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetUserDefinedServerFlag(netId, isUserDefinedServer);
}

#ifdef SUPPORT_SYSVPN
int32_t NetsysController::ProcessVpnStage(NetsysNative::SysVpnStageCode stage, const std::string &message)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("ProcessVpnStage netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->ProcessVpnStage(stage, message);
}

int32_t NetsysController::UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add)
{
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("UpdateVpnRules netsysService is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->UpdateVpnRules(netId, extMessages, add);
}
#endif // SUPPORT_SYSVPN

int32_t NetsysController::CloseSocketsUid(const std::string &ipAddr, uint32_t uid)
{
    NETMGR_LOG_D("Set CloseSocketsUid: uid[%{public}d]", uid);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->CloseSocketsUid(ipAddr, uid);
}

int32_t NetsysController::SetBrokerUidAccessPolicyMap(const std::unordered_map<uint32_t, uint32_t> &uidMaps)
{
    NETMGR_LOG_D("SetBrokerUidAccessPolicyMap Enter");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetBrokerUidAccessPolicyMap(uidMaps);
}

int32_t NetsysController::DelBrokerUidAccessPolicyMap(uint32_t uid)
{
    NETMGR_LOG_D("DelBrokerUidAccessPolicyMap Enter");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->DelBrokerUidAccessPolicyMap(uid);
}

int32_t NetsysController::FlushDnsCache(uint16_t netId)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->FlushDnsCache(netId);
}

int32_t NetsysController::SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetDnsCache(netId, hostName, addrInfo);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
int32_t NetsysController::UpdateEnterpriseRoute(const std::string &interfaceName, uint32_t uid, bool add)
{
    NETMGR_LOG_I("UpdateEnterpriseRoute");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->UpdateEnterpriseRoute(interfaceName, uid, add);
}
#endif

int32_t NetsysController::SetInternetAccessByIpForWifiShare(
    const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName)
{
    if (family != AF_INET && family != AF_INET6) {
        NETMGR_LOG_E("family not support %{public}d", family);
        return NETSYS_PARAM_ERROR;
    }
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
}

int32_t NetsysController::CreateVlan(const std::string &ifName, uint32_t vlanId)
{
    NETMGR_LOG_I("CreateVlan");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->CreateVlan(ifName, vlanId);
}

int32_t NetsysController::DestroyVlan(const std::string &ifName, uint32_t vlanId)
{
    NETMGR_LOG_I("DestroyVlan");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->DestroyVlan(ifName, vlanId);
}

int32_t NetsysController::AddVlanIp(const std::string &ifName, uint32_t vlanId,
                                    const std::string &ip, uint32_t mask)
{
    NETMGR_LOG_I("AddVlanIp");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->AddVlanIp(ifName, vlanId, ip, mask);
}

int32_t NetsysController::GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo)
{
    NETMGR_LOG_D("GetIpNeighTable");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetIpNeighTable(ipMacInfo);
}

int32_t NetsysController::GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo,
                                             int32_t &uid)
{
    NETMGR_LOG_D("GetConnectOwnerUid");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetConnectOwnerUid(netConnInfo, uid);
}

int32_t NetsysController::GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo)
{
    NETMGR_LOG_D("GetSystemNetPortStates");
    // LCOV_EXCL_START This will never happen.
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    // LCOV_EXCL_STOP
    return netsysService_->GetSystemNetPortStates(netPortStatesInfo);
}
} // namespace NetManagerStandard
} // namespace OHOS

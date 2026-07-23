/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "net_conn_service_iface.h"
#include "net_conn_service.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
int32_t NetConnServiceIface::GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames)
{
    return NetConnService::GetInstance()->GetIfaceNames(bearerType, ifaceNames);
}

int32_t NetConnServiceIface::GetIfaceNameByType(NetBearType bearerType, const std::string &ident,
                                                std::string &ifaceName)
{
    return NetConnService::GetInstance()->GetIfaceNameByType(bearerType, ident, ifaceName);
}

int32_t NetConnServiceIface::EnableVnicNetwork(const sptr<NetLinkInfo> &netLinkInfo, const std::set<int32_t> &uids)
{
    return NetConnService::GetInstance()->EnableVnicNetwork(netLinkInfo, uids);
}

int32_t NetConnServiceIface::DisableVnicNetwork()
{
    return NetConnService::GetInstance()->DisableVnicNetwork();
}

int32_t NetConnServiceIface::EnableDistributedClientNet(const std::string &virnicAddr,
    const std::string &virnicName, const std::string &iif)
{
    return NetConnService::GetInstance()->EnableDistributedClientNet(virnicAddr, virnicName, iif);
}

int32_t NetConnServiceIface::EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                                        const std::string &dstAddr, const std::string &gw)
{
    return NetConnService::GetInstance()->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
}

int32_t NetConnServiceIface::DisableDistributedNet(
    bool isServer, const std::string &virnicName, const std::string &dstAddr)
{
    return NetConnService::GetInstance()->DisableDistributedNet(isServer, virnicName, dstAddr);
}

int32_t NetConnServiceIface::RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                                 const std::set<NetCap> &netCaps, uint32_t &supplierId)
{
    return NetConnService::GetInstance()->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
}

int32_t NetConnServiceIface::UnregisterNetSupplier(uint32_t supplierId)
{
    return NetConnService::GetInstance()->UnregisterNetSupplier(supplierId);
}

int32_t NetConnServiceIface::UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo)
{
    return NetConnService::GetInstance()->UpdateNetLinkInfo(supplierId, netLinkInfo);
}

int32_t NetConnServiceIface::UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo)
{
    return NetConnService::GetInstance()->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
}

int32_t NetConnServiceIface::RestrictBackgroundChanged(bool isRestrictBackground)
{
    return NetConnService::GetInstance()->RestrictBackgroundChanged(isRestrictBackground);
}

int32_t NetConnServiceIface::RegisterNetConnCallback(const sptr<INetConnCallback> &callback)
{
    auto netSpecifier = sptr<NetSpecifier>::MakeSptr();
    netSpecifier->SetCapabilities({NET_CAPABILITY_INTERNET, NET_CAPABILITY_NOT_VPN});
    return NetConnService::GetInstance()->RegisterNetConnCallback(netSpecifier, callback, 0);
}

int32_t NetConnServiceIface::RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback)
{
    return NetConnService::GetInstance()->RegisterNetFactoryResetCallback(callback);
}

bool NetConnServiceIface::IsIfaceNameInUse(const std::string &ifaceName, int32_t netId)
{
    return NetConnService::GetInstance()->IsIfaceNameInUse(ifaceName, netId);
}

int32_t NetConnServiceIface::SetReuseSupplierId(uint32_t supplierId, uint32_t reuseSupplierId, bool isReused)
{
    return NetConnService::GetInstance()->SetReuseSupplierId(supplierId, reuseSupplierId, isReused);
}

int32_t NetConnServiceIface::UpdateUidLostDelay(const std::set<uint32_t> &uidLostDelaySet)
{
    return NetConnService::GetInstance()->UpdateUidLostDelay(uidLostDelaySet);
}

int32_t NetConnServiceIface::UpdateUidDeadFlowReset(const std::vector<std::string> &bundleNameVec)
{
    return NetConnService::GetInstance()->UpdateUidDeadFlowReset(bundleNameVec);
}

int32_t NetConnServiceIface::GetConnectionProperties(int32_t netId, NetLinkInfo &info)
{
    return NetConnService::GetInstance()->GetConnectionProperties(netId, info);
}

int32_t NetConnServiceIface::RegisterDualStackProbeCallback(
    int32_t netId, std::shared_ptr<IDualStackProbeCallback>& callback)
{
    return NetConnService::GetInstance()->RegUnRegisterNetProbeCallback(netId, callback, true);
}

int32_t NetConnServiceIface::UnRegisterDualStackProbeCallback(
    int32_t netId, std::shared_ptr<IDualStackProbeCallback>& callback)
{
    return NetConnService::GetInstance()->RegUnRegisterNetProbeCallback(netId, callback, false);
}

int32_t NetConnServiceIface::DualStackProbe(int32_t netId)
{
    return NetConnService::GetInstance()->DualStackProbe(netId);
}

int32_t NetConnServiceIface::UpdateDualStackProbeTime(int32_t dualStackProbeTime)
{
    return NetConnService::GetInstance()->UpdateDualStackProbeTime(dualStackProbeTime);
}

ProbeUrls NetConnServiceIface::GetDataShareUrl()
{
    return NetConnService::GetInstance()->GetDataShareUrl();
}

bool NetConnServiceIface::RegisterNetRequestControlFunc(std::function<bool(const NetRequest &)> func)
{
    return NetConnService::GetInstance()->RegisterNetRequestControlFunc(func);
}

bool NetConnServiceIface::GetAllNetRequest(std::vector<NetRequest> &netRequests)
{
    return NetConnService::GetInstance()->GetAllNetRequest(netRequests);
}

bool NetConnServiceIface::UpdateNetRequestControlState(const std::vector<NetRequest> &netRequests)
{
    return NetConnService::GetInstance()->UpdateNetRequestControlState(netRequests);
}
} // namespace NetManagerStandard
} // namespace OHOS
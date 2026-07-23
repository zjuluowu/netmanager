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

#include "net_manager_center.h"

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_supplier_callback_base.h"

namespace OHOS {
namespace NetManagerStandard {
NetManagerCenter &NetManagerCenter::GetInstance()
{
    static NetManagerCenter gInstance;
    return gInstance;
}

int32_t NetManagerCenter::GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->GetIfaceNames(bearerType, ifaceNames);
}

int32_t NetManagerCenter::GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->GetIfaceNameByType(bearerType, ident, ifaceName);
}

int32_t NetManagerCenter::RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                              const std::set<NetCap> &netCaps, uint32_t &supplierId)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
}

int32_t NetManagerCenter::UnregisterNetSupplier(uint32_t supplierId)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UnregisterNetSupplier(supplierId);
}

int32_t NetManagerCenter::UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateNetLinkInfo(supplierId, netLinkInfo);
}

int32_t NetManagerCenter::UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
}

int32_t NetManagerCenter::RegisterNetConnCallback(const sptr<INetConnCallback> &callback)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->RegisterNetConnCallback(callback);
}

void NetManagerCenter::RegisterConnService(const sptr<NetConnBaseService> &service)
{
    connService_ = service;
}

int32_t NetManagerCenter::GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                              NetStatsInfo &info)
{
    if (statsService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return statsService_->GetIfaceStatsDetail(iface, start, end, info);
}

int32_t NetManagerCenter::ResetStatsFactory()
{
    if (statsService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return statsService_->ResetStatsFactory();
}

void NetManagerCenter::RegisterStatsService(const sptr<NetStatsBaseService> &service)
{
    statsService_ = service;
}

int32_t NetManagerCenter::ResetPolicyFactory()
{
    if (policyService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return ResetPolicies();
}

int32_t NetManagerCenter::ResetPolicies()
{
    if (policyService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return policyService_->ResetPolicies();
}

void NetManagerCenter::RegisterPolicyService(const sptr<NetPolicyBaseService> &service)
{
    policyService_ = service;
}

int32_t NetManagerCenter::ResetEthernetFactory()
{
    if (ethernetService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return ethernetService_->ResetEthernetFactory();
}

void NetManagerCenter::RegisterEthernetService(const sptr<NetEthernetBaseService> &service)
{
    ethernetService_ = service;
}

int32_t NetManagerCenter::RestrictBackgroundChanged(bool isRestrictBackground)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->RestrictBackgroundChanged(isRestrictBackground);
}

bool NetManagerCenter::IsUidNetAccess(uint32_t uid, bool metered)
{
    if (policyService_ == nullptr) {
        return false;
    }
    return IsUidNetAllowed(uid, metered);
}

bool NetManagerCenter::IsUidNetAllowed(uint32_t uid, bool metered)
{
    if (policyService_ == nullptr) {
        return false;
    }
    return policyService_->IsUidNetAllowed(uid, metered);
}

int32_t NetManagerCenter::RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    NETMGR_LOG_I("NetManagerCenter RegisterNetFactoryResetCallback");
    return connService_->RegisterNetFactoryResetCallback(callback);
}

int32_t NetManagerCenter::UpdateUidLostDelay(const std::set<uint32_t> &uidLostDelaySet)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateUidLostDelay(uidLostDelaySet);
}

int32_t NetManagerCenter::UpdateUidDeadFlowReset(const std::vector<std::string> &bundleNameVec)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateUidDeadFlowReset(bundleNameVec);
}

int32_t NetManagerCenter::GetConnectionProperties(int32_t netId, NetLinkInfo &info)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->GetConnectionProperties(netId, info);
}

int32_t NetManagerCenter::RegisterDualStackProbeCallback(
    int32_t netId, std::shared_ptr<IDualStackProbeCallback>& callback)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->RegisterDualStackProbeCallback(netId, callback);
}

int32_t NetManagerCenter::UnRegisterDualStackProbeCallback(
    int32_t netId, std::shared_ptr<IDualStackProbeCallback>& callback)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UnRegisterDualStackProbeCallback(netId, callback);
}

int32_t NetManagerCenter::DualStackProbe(int32_t netId)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->DualStackProbe(netId);
}

int32_t NetManagerCenter::UpdateDualStackProbeTime(int32_t dualStackProbeTimeOut)
{
    if (connService_ == nullptr) {
        return NETMANAGER_ERROR;
    }
    return connService_->UpdateDualStackProbeTime(dualStackProbeTimeOut);
}

void NetManagerCenter::RegisterVpnService(const std::shared_ptr<NetVpnBaseService> &service)
{
    vpnService_ = service;
}

bool NetManagerCenter::IsVpnApplication(int32_t uid)
{
    if (vpnService_ == nullptr) {
        return false;
    }

    return vpnService_->IsVpnApplication(uid);
}

bool NetManagerCenter::IsAppUidInWhiteList(int32_t callingUid, int32_t appUid)
{
    if (vpnService_ == nullptr) {
        return false;
    }

    return vpnService_->IsAppUidInWhiteList(callingUid, appUid);
}

bool NetManagerCenter::NotifyAllowConnectVpnBundleNameChanged(
    std::set<std::string> &&allowConnectVpnBundleName,
    std::set<std::string> &&allowVpnStartWithoutCheckPermissions)
{
    if (vpnService_ == nullptr) {
        return false;
    }
    vpnService_->NotifyAllowConnectVpnBundleNameChanged(
        std::move(allowConnectVpnBundleName), std::move(allowVpnStartWithoutCheckPermissions));
    return true;
}

bool NetManagerCenter::RegisterNetRequestControlFunc(std::function<bool(const NetRequest &)> func)
{
    if (connService_ == nullptr) {
        return false;
    }
    return connService_->RegisterNetRequestControlFunc(func);
}

bool NetManagerCenter::GetAllNetRequest(std::vector<NetRequest> &netRequests)
{
    if (connService_ == nullptr) {
        return false;
    }
    return connService_->GetAllNetRequest(netRequests);
}

bool NetManagerCenter::UpdateNetRequestControlState(const std::vector<NetRequest> &netRequests)
{
    if (connService_ == nullptr) {
        return false;
    }
    return connService_->UpdateNetRequestControlState(netRequests);
}
#ifdef SUPPORT_TRAFFIC_STATISTIC
bool NetManagerCenter::GetDailyMarkBySimId(int32_t simId, uint16_t &dailyMark)
{
    if (statsService_ == nullptr) {
        return false;
    }
    return statsService_->GetDailyMarkBySimId(simId, dailyMark);
}

bool NetManagerCenter::GetMonthlyLimitBySimId(int32_t simId, uint64_t &monthlyLimit)
{
    if (statsService_ == nullptr) {
        return false;
    }
    return statsService_->GetMonthlyLimitBySimId(simId, monthlyLimit);
}

bool NetManagerCenter::GetMonthlyMarkBySimId(int32_t simId, uint16_t &monthlyMark)
{
    if (statsService_ == nullptr) {
        return false;
    }
    return statsService_->GetMonthlyMarkBySimId(simId, monthlyMark);
}
#endif
} // namespace NetManagerStandard
} // namespace OHOS

/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef NET_MANAGER_CENTER_H
#define NET_MANAGER_CENTER_H

#include <set>
#include <functional>

#include "net_all_capabilities.h"
#include "net_conn_base_service.h"
#include "net_ethernet_base_service.h"
#include "net_policy_base_service.h"
#include "net_stats_base_service.h"
#include "net_vpn_base_service.h"
#include "dual_stack_probe_callback.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetRequest;
class NetManagerCenter {
public:
    static NetManagerCenter &GetInstance();
    int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames);
    int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName);
    int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
                                uint32_t &supplierId);
    int32_t UnregisterNetSupplier(uint32_t supplierId);
    int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo);
    int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo);
    int32_t RegisterNetConnCallback(const sptr<INetConnCallback> &callback);
    void RegisterConnService(const sptr<NetConnBaseService> &service);

    int32_t GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end, NetStatsInfo &info);
    int32_t ResetStatsFactory();
    void RegisterStatsService(const sptr<NetStatsBaseService> &service);

    int32_t ResetPolicyFactory();
    int32_t ResetPolicies();
    void RegisterPolicyService(const sptr<NetPolicyBaseService> &service);

    int32_t ResetEthernetFactory();
    void RegisterEthernetService(const sptr<NetEthernetBaseService> &service);

    int32_t RestrictBackgroundChanged(bool isRestrictBackground);
    bool IsUidNetAccess(uint32_t uid, bool metered);
    bool IsUidNetAllowed(uint32_t uid, bool metered);

    int32_t RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback);
    int32_t UpdateUidLostDelay(const std::set<uint32_t> &uidLostDelaySet);
    int32_t UpdateUidDeadFlowReset(const std::vector<std::string> &bundleNameVec);
    int32_t GetConnectionProperties(int32_t netId, NetLinkInfo &info);
    int32_t RegisterDualStackProbeCallback(int32_t netId, std::shared_ptr<IDualStackProbeCallback>& callback);
    int32_t UnRegisterDualStackProbeCallback(int32_t netId, std::shared_ptr<IDualStackProbeCallback>& callback);
    int32_t DualStackProbe(int32_t netId);
    int32_t UpdateDualStackProbeTime(int32_t dualStackProbeTimeOut);

    bool IsVpnApplication(int32_t uid);
    bool IsAppUidInWhiteList(int32_t callingUid, int32_t appUid);
    void RegisterVpnService(const std::shared_ptr<NetVpnBaseService> &service);
    bool NotifyAllowConnectVpnBundleNameChanged(
        std::set<std::string> &&allowConnectVpnBundleName,
        std::set<std::string> &&allowVpnStartWithoutCheckPermissions);

    bool RegisterNetRequestControlFunc(std::function<bool(const NetRequest&)> func);
    bool GetAllNetRequest(std::vector<NetRequest>& netRequests);
    bool UpdateNetRequestControlState(const std::vector<NetRequest>& netRequests);
#ifdef SUPPORT_TRAFFIC_STATISTIC
    bool GetDailyMarkBySimId(int32_t simId, uint16_t &dailyMark);
    bool GetMonthlyLimitBySimId(int32_t simId, uint64_t &monthlyLimit);
    bool GetMonthlyMarkBySimId(int32_t simId, uint16_t &monthlyMark);
#endif
private:
    sptr<NetConnBaseService> connService_ = nullptr;
    sptr<NetStatsBaseService> statsService_ = nullptr;
    sptr<NetPolicyBaseService> policyService_ = nullptr;
    sptr<NetEthernetBaseService> ethernetService_ = nullptr;
    std::shared_ptr<NetVpnBaseService> vpnService_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_MANAGER_CENTER_H

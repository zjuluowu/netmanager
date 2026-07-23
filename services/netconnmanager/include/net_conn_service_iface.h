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
#ifndef NET_CONN_SERVICE_IFACE_H
#define NET_CONN_SERVICE_IFACE_H

#include <set>

#include "net_all_capabilities.h"
#include "net_conn_base_service.h"

namespace OHOS {
namespace NetManagerStandard {
/**
 *  @deprecated use net_conn_client.h to instead
 */
class NetConnServiceIface : public NetConnBaseService {
public:
    int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames) override;
    int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName) override;
    int32_t EnableVnicNetwork(const sptr<NetLinkInfo> &netLinkInfo, const std::set<int32_t> &uids);
    int32_t DisableVnicNetwork();
    int32_t EnableDistributedClientNet(const std::string &virnicAddr,
        const std::string &virnicName, const std::string &iif);
    int32_t EnableDistributedServerNet(const std::string &iif, const std::string &devIface, const std::string &dstAddr,
                                       const std::string &gw);
    int32_t DisableDistributedNet(bool isServer, const std::string &virnicName, const std::string &dstAddr);
    int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
        uint32_t &supplierId) override;
    int32_t UnregisterNetSupplier(uint32_t supplierId) override;
    int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo) override;
    int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo) override;
    int32_t RestrictBackgroundChanged(bool isRestrictBackground) override;
    int32_t RegisterNetConnCallback(const sptr<INetConnCallback> &callback) override;
    int32_t RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback) override;
    bool IsIfaceNameInUse(const std::string &ifaceName, int32_t netId);
    int32_t SetReuseSupplierId(uint32_t supplierId, uint32_t reuseSupplierId, bool isReused) override;
    int32_t UpdateUidLostDelay(const std::set<uint32_t> &uidLostDelaySet) override;
    int32_t UpdateUidDeadFlowReset(const std::vector<std::string> &bundleNameVec) override;
    int32_t GetConnectionProperties(int32_t netId, NetLinkInfo &info) override;
    int32_t RegisterDualStackProbeCallback(int32_t netId, std::shared_ptr<IDualStackProbeCallback>& callback) override;
    int32_t UnRegisterDualStackProbeCallback(int32_t netId,
        std::shared_ptr<IDualStackProbeCallback>& callback) override;
    int32_t DualStackProbe(int32_t netId) override;
    int32_t UpdateDualStackProbeTime(int32_t dualStackProbeTime) override;
    ProbeUrls GetDataShareUrl() override;
    bool RegisterNetRequestControlFunc(std::function<bool(const NetRequest &)> func) override;
    bool GetAllNetRequest(std::vector<NetRequest> &netRequests) override;
    bool UpdateNetRequestControlState(const std::vector<NetRequest> &netRequests) override;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CONN_SERVICE_IFACE_H
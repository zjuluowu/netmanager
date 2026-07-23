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
#ifndef NET_CONN_BASE_SERVICE_H
#define NET_CONN_BASE_SERVICE_H

#include <set>
#include <functional>
#include <vector>
#include "net_all_capabilities.h"
#include "net_link_info.h"
#include "net_supplier_callback_base.h"
#include "net_supplier_info.h"
#include "i_net_conn_callback.h"
#include "refbase.h"
#include "i_net_factoryreset_callback.h"
#include "dual_stack_probe_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class NetConnBaseService : public virtual RefBase {
public:
    virtual int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames) = 0;
    virtual int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName) = 0;
    virtual int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                        const std::set<NetCap> &netCaps, uint32_t &supplierId) = 0;
    virtual int32_t UnregisterNetSupplier(uint32_t supplierId) = 0;
    virtual int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo) = 0;
    virtual int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo) = 0;
    virtual int32_t RestrictBackgroundChanged(bool isRestrictBackground) = 0;
    virtual int32_t RegisterNetConnCallback(const sptr<INetConnCallback> &callback) = 0;
    virtual int32_t RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback) = 0;
    virtual int32_t SetReuseSupplierId(uint32_t supplierId, uint32_t reuseSupplierId, bool isReused) = 0;
    virtual int32_t UpdateUidLostDelay(const std::set<uint32_t> &uidLostDelaySet) = 0;
    virtual int32_t UpdateUidDeadFlowReset(const std::vector<std::string> &bundleNameVec) = 0;
    virtual int32_t GetConnectionProperties(int32_t netId, NetLinkInfo &info) = 0;
    virtual int32_t RegisterDualStackProbeCallback(int32_t netId,
        std::shared_ptr<IDualStackProbeCallback>& callback) = 0;
    virtual int32_t UnRegisterDualStackProbeCallback(int32_t netId,
        std::shared_ptr<IDualStackProbeCallback>& callback) = 0;
    virtual int32_t DualStackProbe(int32_t netId) = 0;
    virtual int32_t UpdateDualStackProbeTime(int32_t dualStackProbeTimeOut) = 0;
    virtual ProbeUrls GetDataShareUrl() = 0;
    virtual bool RegisterNetRequestControlFunc(std::function<bool(const NetRequest &)> func) = 0;
    virtual bool GetAllNetRequest(std::vector<NetRequest> &netRequests) = 0;
    virtual bool UpdateNetRequestControlState(const std::vector<NetRequest> &netRequests) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CONN_BASE_SERVICE_H

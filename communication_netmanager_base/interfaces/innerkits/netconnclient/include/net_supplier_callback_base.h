/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef NET_SUPPLIER_CALLBACK_BASE_H
#define NET_SUPPLIER_CALLBACK_BASE_H

#include <string>
#include <set>

#include "refbase.h"

#include "net_all_capabilities.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetRequest {
    uint32_t uid = 0;
    uint32_t requestId = 0;
    uint32_t registerType = REGISTER;
    bool isControlled = false;
    std::string ident;
    std::set<NetBearType> bearTypes;
    std::set<NetCap> netCaps;
    NetRequest(const uint32_t uid, const uint32_t reqId = 0, const uint32_t &registerType = UNKOWN,
        const std::string ident = "", const std::set<NetBearType> &netBearTypes = {},
        const std::set<NetCap> &netCaps = {})
        : uid(uid), requestId(reqId), registerType(registerType), ident(ident), bearTypes(netBearTypes),
          netCaps(netCaps)
    {}
    NetRequest() = default;
};
class NetSupplierCallbackBase : public virtual RefBase {
public:
    virtual ~NetSupplierCallbackBase() = default;

    virtual int32_t RequestNetwork(const std::string &ident,
                                   const std::set<NetCap> &netCaps,
                                   const NetRequest &netrequest = {});
    virtual int32_t ReleaseNetwork(const NetRequest &netrequest);
};
} // NetManagerStandard
} // OHOS
#endif // NET_SUPPLIER_CALLBACK_BASE_H
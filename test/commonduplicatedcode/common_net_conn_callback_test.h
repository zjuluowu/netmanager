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

#ifndef COMMON_NET_CONN_CALLBACK_TEST_H
#define COMMON_NET_CONN_CALLBACK_TEST_H

#include "i_net_conn_callback.h"
#include "iremote_object.h"
#include "net_conn_callback_stub.h"
#include "net_manager_constants.h"
#include "net_supplier_callback_base.h"
#include "net_supplier_callback_stub.h"
#include "net_manager_constants.h"
#include <iostream>

namespace OHOS {
namespace NetManagerStandard {
class INetConnCallbackTest : public IRemoteStub<INetConnCallback> {
public:
    int32_t NetAvailable(sptr<NetHandle> &netHandle)
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetCapabilitiesChange(sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap)
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info)
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetLost(sptr<NetHandle> &netHandle)
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetUnavailable()
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
    {
        return NETMANAGER_SUCCESS;
    }
};

class PreAirplaneCallbackTest : public PreAirplaneCallbackStub {
public:
    int32_t PreAirplaneStart() override
    {
        std::cout << "test PreAirplaneStart" << std::endl;
        return NETMANAGER_SUCCESS;
    }
};

class NetSupplierCallbackBaseTestCb : public NetSupplierCallbackBase {
public:
    virtual ~NetSupplierCallbackBaseTestCb() = default;

    int32_t RequestNetwork(const std::string &ident, const std::set<NetCap> &netCaps,
        const NetRequest &netrequest = {}) override
    {
        return NETMANAGER_SUCCESS;
    };

    int32_t ReleaseNetwork(const NetRequest &netrequest) override
    {
        return NETMANAGER_SUCCESS;
    };
};

class NetSupplierCallbackStubTestCb : public NetSupplierCallbackStub {
public:
    NetSupplierCallbackStubTestCb() = default;
    ~NetSupplierCallbackStubTestCb() {}

    int32_t RequestNetwork(const std::string &ident, const std::set<NetCap> &netCaps,
                           const NetRequest &netrequest = {}) override
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t ReleaseNetwork(const NetRequest &netrequest) override
    {
        return NETMANAGER_SUCCESS;
    }
};

class IPreAirplaneCallbackStubTestCb : public PreAirplaneCallbackStub {
public:
    IPreAirplaneCallbackStubTestCb() = default;
    ~IPreAirplaneCallbackStubTestCb() = default;

    int32_t PreAirplaneStart() override
    {
        std::cout << "fuzz test PreAirplaneStart" << std::endl;
        return NETMANAGER_SUCCESS;
    }
};

class NetConnCallbackStubCb : public NetConnCallbackStub {
    int32_t NetAvailable(sptr<NetHandle> &netHandle) override
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetCapabilitiesChange(sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap) override
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info) override
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetLost(sptr<NetHandle> &netHandle) override
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetUnavailable() override
    {
        return NETMANAGER_SUCCESS;
    }

    int32_t NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked) override
    {
        return NETMANAGER_SUCCESS;
    }
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMON_NET_CONN_CALLBACK_TEST_H

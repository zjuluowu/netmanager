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

#ifndef NET_CONNECTION_IMPL_H
#define NET_CONNECTION_IMPL_H

#include "common.h"
#include "ffi_remote_data.h"
#include "net_connection_callback.h"
#include "net_manager_constants.h"
#include "net_specifier.h"
#include <map>
#include <vector>
namespace OHOS::NetManagerStandard {

class NetConnectionImpl final {
public:
    bool hasNetSpecifier_;

    bool hasTimeout_;

    NetManagerStandard::NetSpecifier netSpecifier_;

    uint32_t timeout_;

    std::vector<std::function<void(int32_t)>> netAvailible;

    std::vector<std::function<void(int32_t, bool)>> netBlockStatusChange;

    std::vector<std::function<void(CNetCapabilityInfo)>> netCapabilitiesChange;

    std::vector<std::function<void(int32_t, CConnectionProperties)>> netConnectionPropertiesChange;

    std::vector<std::function<void(int32_t)>> netLost;

    std::vector<std::function<void()>> netUnavailable;

public:
    [[nodiscard]] sptr<OHOS::NetManagerStandard::ConnectionCallbackObserver> GetObserver() const;

    static NetConnectionImpl *MakeNetConnection();

    static void DeleteNetConnection(OHOS::NetManagerStandard::NetConnectionImpl *netConnection);

private:
    sptr<ConnectionCallbackObserver> observer_;

    explicit NetConnectionImpl();

    ~NetConnectionImpl() = default;
};

class NetConnectionProxy : public OHOS::FFI::FFIData {
    DECL_TYPE(NetConnectionProxy, OHOS::FFI::FFIData);
public:
    NetConnectionProxy(CNetSpecifier specifier, uint32_t timeout);

    int32_t RegisterCallback();

    int32_t UnregisterCallback();

    void OnNetAvailible(void (*callback)(int32_t));

    void OnNetBlockStatusChange(void (*callback)(int32_t, bool));

    void OnNetCapabilitiesChange(void (*callback)(CNetCapabilityInfo));

    void OnNetConnectionPropertiesChange(void (*callback)(int32_t, CConnectionProperties));

    void OnNetLost(void (*callback)(int32_t));

    void OnNetUnavailable(void (*callback)());

    void Release();

private:
    NetConnectionImpl *netConn_;
};

extern std::map<OHOS::NetManagerStandard::ConnectionCallbackObserver *, OHOS::NetManagerStandard::NetConnectionImpl *>
    NET_CONNECTIONS_FFI;
extern std::shared_mutex g_netConnectionsMutex;
} // namespace OHOS::NetManagerStandard

#endif
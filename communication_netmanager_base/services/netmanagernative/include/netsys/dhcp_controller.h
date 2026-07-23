/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_DHCP_CONTROLLER_H
#define INCLUDE_DHCP_CONTROLLER_H

#include <shared_mutex>
#include "i_notify_callback.h"
#include "dhcp_c_api.h"

namespace OHOS {
namespace nmd {
class DhcpController {
public:
    class DhcpControllerResultNotify {
    public:
        explicit DhcpControllerResultNotify();
        ~DhcpControllerResultNotify();
        static void OnSuccess(int status, const char *ifname, DhcpResult *result);
        static void OnFailed(int status, const char *ifname, const char *reason);
        static void SetDhcpController(DhcpController *dhcpController);
    private:
        static DhcpController *dhcpController_;
    };

public:
    DhcpController();
    ~DhcpController();

    int32_t RegisterNotifyCallback(sptr<OHOS::NetsysNative::INotifyCallback> &callback);
    int32_t UnregisterNotifyCallback(sptr<OHOS::NetsysNative::INotifyCallback> &callback);
    void StartClient(const std::string &iface, bool bIpv6);
    void StopClient(const std::string &iface, bool bIpv6);
    bool StartDhcpService(const std::string &iface, const std::string &ipv4addr);
    bool StopDhcpService(const std::string &iface);

    void Process(const std::string &iface, DhcpResult *result);

private:
    ClientCallBack clientEvent;
    std::unique_ptr<DhcpControllerResultNotify> dhcpResultNotify_ = nullptr;
    std::vector<sptr<OHOS::NetsysNative::INotifyCallback>> callback_;
    std::shared_mutex callbackMutex_;
};
} // namespace nmd
} // namespace OHOS
#endif // !INCLUDE_DHCP_CONTROLLER_H

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

#include "dhcp_controller.h"

#include "dhcp_result_parcel.h"
#include "netnative_log_wrapper.h"
#include "netmanager_base_common_utils.h"
#include <securec.h>
#include <cstring>

namespace OHOS {
namespace nmd {
static constexpr const char *DEFAULT_STR_SUBNET = "255.255.255.0";
static constexpr const char *DEFAULT_STR_STARTIP = ".3";
static constexpr const char *DEFAULT_STR_ENDIP = ".254";
DhcpController *DhcpController::DhcpControllerResultNotify::dhcpController_ = nullptr;

DhcpController::DhcpControllerResultNotify::DhcpControllerResultNotify() {}

DhcpController::DhcpControllerResultNotify::~DhcpControllerResultNotify() {}

void DhcpController::DhcpControllerResultNotify::SetDhcpController(DhcpController *dhcpController)
{
    dhcpController_ = dhcpController;
}

void DhcpController::DhcpControllerResultNotify::OnSuccess(int status, const char *ifname,
                                                           DhcpResult *result)
{
    if (ifname == nullptr || result == nullptr) {
        NETNATIVE_LOGE("ifname or result is nullptr!");
        return;
    }
    NETNATIVE_LOGI(
        "Enter DhcpController::DhcpControllerResultNotify::OnSuccess "
        "ifname=[%{public}s], iptype=[%{public}d], strYourCli=[%{public}s], "
        "strServer=[%{public}s], strSubnet=[%{public}s], strDns1=[%{public}s], "
        "strDns2=[%{public}s] strRouter1=[%{public}s] strRouter2=[%{public}s]",
        ifname, result->iptype, result->strOptClientId,
        NetManagerStandard::CommonUtils::ToAnonymousIp(result->strOptServerId).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result->strOptSubnet).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result->strOptDns1).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result->strOptDns2).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result->strOptRouter1).c_str(),
        NetManagerStandard::CommonUtils::ToAnonymousIp(result->strOptRouter2).c_str());
    dhcpController_->Process(ifname, result);
}

void DhcpController::DhcpControllerResultNotify::OnFailed(int status, const char *ifname,
                                                          const char *reason)
{
    NETNATIVE_LOGE("Enter DhcpController::DhcpControllerResultNotify::OnFailed");
}

DhcpController::DhcpController()
{
    dhcpResultNotify_ = std::make_unique<DhcpControllerResultNotify>();
}

DhcpController::~DhcpController() {}

int32_t DhcpController::RegisterNotifyCallback(sptr<OHOS::NetsysNative::INotifyCallback> &callback)
{
    NETNATIVE_LOGI("DhcpController RegisterNotifyCallback");
    if (callback == nullptr) {
        NETNATIVE_LOGE("callback is nullptr");
        return 0;
    }
    std::unique_lock<std::shared_mutex> locker(callbackMutex_);
    for (const auto &cb : callback_) {
        if (cb->AsObject().GetRefPtr() == callback->AsObject().GetRefPtr()) {
            NETNATIVE_LOGI("callback is already registered");
            return 0;
        }
    }
    callback_.push_back(callback);
    return 0;
}

int32_t DhcpController::UnregisterNotifyCallback(sptr<OHOS::NetsysNative::INotifyCallback> &callback)
{
    NETNATIVE_LOGI("DhcpController UnregisterNotifyCallback");
    if (callback == nullptr) {
        NETNATIVE_LOGE("callback is nullptr");
        return 0;
    }
    std::unique_lock<std::shared_mutex> locker(callbackMutex_);
    for (auto it = callback_.begin(); it != callback_.end(); ++it) {
        if ((*it)->AsObject().GetRefPtr() == callback->AsObject().GetRefPtr()) {
            callback_.erase(it);
            NETNATIVE_LOGI("callback is unregistered successfully");
            return 0;
        }
    }
    NETNATIVE_LOGI("callback is not registered");
    return 0;
}

void DhcpController::StartClient(const std::string &iface, bool bIpv6)
{
    clientEvent.OnIpSuccessChanged = DhcpControllerResultNotify::OnSuccess;
    clientEvent.OnIpFailChanged = DhcpControllerResultNotify::OnFailed;
    dhcpResultNotify_->SetDhcpController(this);
    if (RegisterDhcpClientCallBack(iface.c_str(), &clientEvent) != DHCP_SUCCESS) {
        NETNATIVE_LOGE("RegisterDhcpClientCallBack failed.");
        return;
    }

    NETNATIVE_LOGI("DhcpController StartDhcpClient iface[%{public}s] ipv6[%{public}d]", iface.c_str(), bIpv6);
    RouterConfig config;
    config.bIpv6 = bIpv6;
    if (strncpy_s(config.ifname, sizeof(config.ifname), iface.c_str(), iface.length()) != DHCP_SUCCESS) {
        NETNATIVE_LOGE("strncpy_s config.ifname failed.");
        return;
    }
    if (StartDhcpClient(config) != DHCP_SUCCESS) {
        NETNATIVE_LOGE("Start dhcp client failed");
    }
}

void DhcpController::StopClient(const std::string &iface, bool bIpv6)
{
    NETNATIVE_LOGI("DhcpController StopDhcpClient iface[%{public}s] ipv6[%{public}d]", iface.c_str(), bIpv6);
    if (StopDhcpClient(iface.c_str(), bIpv6) != DHCP_SUCCESS) {
        NETNATIVE_LOGE("Stop dhcp client failed");
    }
}

void DhcpController::Process(const std::string &iface, DhcpResult *result)
{
    NETNATIVE_LOGI("DhcpController Process");
    sptr<OHOS::NetsysNative::DhcpResultParcel> ptr = new (std::nothrow) OHOS::NetsysNative::DhcpResultParcel();
    if (ptr == nullptr) {
        return;
    }
    ptr->iface_ = iface;
    ptr->ipAddr_ = result->strOptClientId;
    ptr->gateWay_ = result->strOptServerId;
    ptr->subNet_ = result->strOptSubnet;
    ptr->route1_ = result->strOptRouter1;
    ptr->route2_ = result->strOptRouter2;
    ptr->dns1_ = result->strOptDns1;
    ptr->dns2_ = result->strOptDns2;
    NETNATIVE_LOGI("DhcpController Process iface[%{public}s]", iface.c_str());
    std::shared_lock<std::shared_mutex> locker(callbackMutex_);
    for (auto cb : callback_) {
        cb->OnDhcpSuccess(ptr);
    }
}

bool DhcpController::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    constexpr int32_t IP_V4 = 0;
    std::string ipAddr = ipv4addr;
    std::string::size_type pos = ipAddr.rfind(".");
    if (pos == std::string::npos) {
        return false;
    }

    std::string ipHead = ipAddr.substr(0, pos);
    std::string strStartip = ipHead + DEFAULT_STR_STARTIP;
    std::string strEndip = ipHead + DEFAULT_STR_ENDIP;
    std::string strSubnet = DEFAULT_STR_SUBNET;

    DhcpRange range;
    range.iptype = IP_V4;
    if (strcpy_s(range.strTagName, DHCP_MAX_FILE_BYTES, iface.c_str()) != 0) {
        NETNATIVE_LOGE("strcpy_s strTagName failed!");
        return false;
    }

    if (strcpy_s(range.strStartip, INET_ADDRSTRLEN, strStartip.c_str()) != 0) {
        NETNATIVE_LOGE("strcpy_s strStartip failed!");
        return false;
    }

    if (strcpy_s(range.strEndip, INET_ADDRSTRLEN, strEndip.c_str()) != 0) {
        NETNATIVE_LOGE("strcpy_s strEndip failed!");
        return false;
    }

    if (strcpy_s(range.strSubnet, INET_ADDRSTRLEN, strSubnet.c_str()) != 0) {
        NETNATIVE_LOGE("strcpy_s strSubnet failed!");
        return false;
    }

    if (SetDhcpRange(iface.c_str(), &range) != DHCP_SUCCESS) {
        NETNATIVE_LOGE("SetDhcpRange failed!");
        return false;
    }
    NETNATIVE_LOGI(
        "Set dhcp range : ifaceName[%{public}s] TagName[%{public}s] start ip[%{public}s] end ip[%{public}s]",
        iface.c_str(), range.strTagName, range.strStartip, range.strEndip);
    if (StartDhcpServer(iface.c_str()) != DHCP_SUCCESS) {
        NETNATIVE_LOGE("Start dhcp server failed!, iface:[%{public}s]", iface.c_str());
        return false;
    }
    return true;
}

bool DhcpController::StopDhcpService(const std::string &iface)
{
    if (RemoveAllDhcpRange(iface.c_str()) != DHCP_SUCCESS) {
        NETNATIVE_LOGE("failed to remove [%{public}s] dhcp range.", iface.c_str());
    }

    if (StopDhcpServer(iface.c_str()) != DHCP_SUCCESS) {
        NETNATIVE_LOGE("Stop dhcp server failed!");
        return false;
    }
    NETNATIVE_LOGI("StopDhcpService ifaceName[%{public}s]", iface.c_str());
    return true;
}
} // namespace nmd
} // namespace OHOS

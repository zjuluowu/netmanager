/*
 * Copyright (C) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef NETMANAGER_PERMISSION_H
#define NETMANAGER_PERMISSION_H

#include <cstdint>
#include <string>
#include <vector>

namespace OHOS {
namespace NetManagerStandard {
namespace Permission {
static constexpr const char *GET_NETWORK_INFO = "ohos.permission.GET_NETWORK_INFO";
static constexpr const char *GET_ETHERNET_LOCAL_MAC = "ohos.permission.GET_ETHERNET_LOCAL_MAC";
static constexpr const char *INTERNET = "ohos.permission.INTERNET";
static constexpr const char *CONNECTIVITY_INTERNAL = "ohos.permission.CONNECTIVITY_INTERNAL";
static constexpr const char *GET_NETSTATS_SUMMARY = "ohos.permission.GET_NETSTATS_SUMMARY";
static constexpr const char *MANAGE_NET_STRATEGY = "ohos.permission.MANAGE_NET_STRATEGY";
static constexpr const char *MANAGE_VPN = "ohos.permission.MANAGE_VPN";
static constexpr const char *GET_NETWORK_STATS = "ohos.permission.GET_NETWORK_STATS";
static constexpr const char *NETSYS_INTERNAL = "ohos.permission.NETSYS_INTERNAL";
static constexpr const char *MANAGE_ENTERPRISE_WIFI_CONNECTION = "ohos.permission.MANAGE_ENTERPRISE_WIFI_CONNECTION";
static constexpr const char *SET_PAC_URL = "ohos.permission.SET_PAC_URL";
static constexpr const char *SET_NET_EXT_ATTRIBUTE = "ohos.permission.SET_NET_EXT_ATTRIBUTE";
static constexpr const char *GET_NETWORK_LOCATION = "ohos.permission.LOCATION";
static constexpr const char *GET_NETWORK_APPROXIMATE_LOCATION = "ohos.permission.APPROXIMATELY_LOCATION";
static constexpr const char *ACCESS_NET_TRACE_INFO = "ohos.permission.ACCESS_NET_TRACE_INFO";
static constexpr const char *GET_IP_MAC_INFO = "ohos.permission.GET_IP_MAC_INFO";
static constexpr const char *SEND_MESSAGE = "ohos.permission.SEND_MESSAGES";
} // namespace Permission

class NetManagerPermission {
public:
    static bool CheckPermission(const std::string &permissionName);
    static bool CheckPermissionWithCache(const std::string &permissionName);
    static bool IsSystemCaller();
    static bool CheckNetSysInternalPermission(const std::string &permissionName);
    static bool CheckUidPermission(const std::vector<uint32_t> &allowedUids);
    static int32_t GetApiVersion();
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_PERMISSION_H

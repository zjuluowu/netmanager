/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef NET_DATASHARE_UTILS_IFACE_H
#define NET_DATASHARE_UTILS_IFACE_H

#include <functional>
#include <memory>
#include <string>

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *SHARING_WIFI_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=sharing_wifi";
constexpr const char *SHARING_USB_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=sharing_usb";
constexpr const char *SHARING_BLUETOOTH_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=sharing_bluetooth";

constexpr const char *KEY_SHARING_WIFI = "settings.netmanager.sharing_wifi";
constexpr const char *KEY_SHARING_USB = "settings.netmanager.sharing_usb";
constexpr const char *KEY_SHARING_BLUETOOTH = "settings.netmanager.sharing_bluetooth";
} // namespace

class NetDataShareHelperUtils;

class NetDataShareHelperUtilsIface final {
public:
    static int32_t Query(const std::string &strUri, const std::string &key, std::string &value);
    static int32_t Insert(const std::string &strUri, const std::string &key, const std::string &value);
    static int32_t Update(const std::string &strUri, const std::string &key, const std::string &value);
    static int32_t Delete(const std::string &strUri, const std::string &key);
    static int32_t RegisterObserver(const std::string &strUri, const std::function<void()> &onChange);
    static int32_t UnregisterObserver(const std::string &strUri, int32_t callbackId);
    static int32_t RegisterSettingsObserver(const std::string &strUri, const std::function<void()> &onChange);
    static int32_t UnRegisterSettingsObserver(const std::string &strUri, const std::function<void()> &onChange);
    static std::unique_ptr<NetDataShareHelperUtils> dataShareHelperUtils_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_DATASHARE_UTILS_H
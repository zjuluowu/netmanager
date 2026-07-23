/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "net_interface_config.h"

#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr size_t MAX_INTERFACE_CONFIG_SIZE = 16;
constexpr const char *IFACE_LINK_UP = "up";
constexpr const char *IFACE_RUNNING = "running";
} // namespace

bool NetInterfaceConfiguration::IsInterfaceUp()
{
    return (std::find(flags_.begin(), flags_.end(), IFACE_LINK_UP) != flags_.end());
}

bool NetInterfaceConfiguration::IsInterfaceRunning()
{
    return (std::find(flags_.begin(), flags_.end(), IFACE_RUNNING) != flags_.end());
}

bool NetInterfaceConfiguration::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(ifName_)) {
        return false;
    }

    if (!parcel.WriteString(hwAddr_)) {
        return false;
    }

    if (!parcel.WriteString(ipv4Addr_)) {
        return false;
    }

    if (!parcel.WriteInt32(prefixLength_)) {
        return false;
    }

    if (!parcel.WriteInt32(static_cast<int32_t>(std::min(MAX_INTERFACE_CONFIG_SIZE, flags_.size())))) {
        return false;
    }

    size_t size = 0;
    for (const auto &flag : flags_) {
        if (!parcel.WriteString(flag)) {
            return false;
        }
        ++size;
        if (size >= MAX_INTERFACE_CONFIG_SIZE) {
            return true;
        }
    }
    return true;
}

bool NetInterfaceConfiguration::Unmarshalling(Parcel &parcel, NetInterfaceConfiguration &config)
{
    if (!parcel.ReadString(config.ifName_)) {
        return false;
    }
    if (!parcel.ReadString(config.hwAddr_)) {
        return false;
    }
    if (!parcel.ReadString(config.ipv4Addr_)) {
        return false;
    }
    if (!parcel.ReadInt32(config.prefixLength_)) {
        return false;
    }
    int32_t tmpSize = 0;
    if (!parcel.ReadInt32(tmpSize)) {
        return false;
    }
    size_t size = static_cast<size_t>(tmpSize);
    size = (size > MAX_INTERFACE_CONFIG_SIZE) ? MAX_INTERFACE_CONFIG_SIZE : size;
    for (size_t i = 0; i < size; i++) {
        std::string flag;
        if (!parcel.ReadString(flag)) {
            return false;
        }
        config.flags_.push_back(flag);
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS

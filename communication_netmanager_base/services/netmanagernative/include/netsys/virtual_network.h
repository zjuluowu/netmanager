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

#ifndef INCLUDE_VIRTUAL_NETWORK_H
#define INCLUDE_VIRTUAL_NETWORK_H

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <sys/types.h>

#include "netsys_network.h"
#include "network_permission.h"
#include "uid_range.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
class VirtualNetwork : public NetsysNetwork {
public:
    VirtualNetwork(uint16_t netId, bool hasDns);
    virtual ~VirtualNetwork() = default;

    /**
     * Judge network type whether or not physical
     *
     * @return Returns true physical
     */
    bool IsPhysical() override
    {
        return false;
    }

    bool GetHasDns() const;

    int32_t AddUids(const std::vector<UidRange> &uidRanges);
    int32_t RemoveUids(const std::vector<UidRange> &uidRanges);
#ifdef SUPPORT_SYSVPN
    int32_t UpdateVpnRules(const std::vector<std::string> &extMessages, bool add);
#endif // SUPPORT_SYSVPN

private:
    std::string GetNetworkType() const override
    {
        return "VIRTUAL";
    };

    int32_t AddInterface(std::string &interfaceName) override;
    int32_t RemoveInterface(std::string &interfaceName) override;

private:
    const bool hasDns_ = false;
    std::vector<UidRange> uidRanges_;
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_VIRTUAL_NETWORK_H

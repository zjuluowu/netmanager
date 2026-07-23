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

#ifndef SYSTEM_VPN_WRAPPER_H
#define SYSTEM_VPN_WRAPPER_H

#include <cstring>
#include "ffrt.h"
#include "i_netsys_service.h"

#define IPSEC_PIDDIR "/data/service/el1/public/vpn"

namespace OHOS {
namespace nmd {
using namespace NetsysNative;
class SystemVpnWrapper : public std::enable_shared_from_this<SystemVpnWrapper> {
public:
    SystemVpnWrapper();
    ~SystemVpnWrapper();
    static std::shared_ptr<SystemVpnWrapper> &GetInstance()
    {
        static std::shared_ptr<SystemVpnWrapper> instance = std::make_shared<SystemVpnWrapper>();
        return instance;
    }

    /**
     * update system vpn next stage by SysVpnStageCode
     *
     * @param stage one of the SysVpnStageCode
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t Update(SysVpnStageCode stage, const std::string &message = "");

private:
    void ExecuteUpdate(SysVpnStageCode stage, const std::string &message = "");
    bool PrepareUpdate(SysVpnStageCode stage, const std::string &message = "");

private:
    static constexpr const char *IPSEC_CMD_PATH = "/system/bin/ipsec";
    const std::string VPN_STAGE_RESTART = "restart";
    const std::string VPN_STAGE_SWANCTL_LOAD = "swanctl --load-all --file ";
    const std::string VPN_STAGE_UP_HOME = "up ";
    const std::string VPN_STAGE_DOWN_HOME = "down ";
    const std::string VPN_STAGE_STOP = "stop";
    const std::string VPN_STAGE_L2TP_LOAD = "xl2tpd -c ";
    const std::string VPN_STAGE_L2TP_CTL = "l2tpctl ";
    const std::string IPSEC_L2TP_CTL = " -C " IPSEC_PIDDIR "/l2tp-control";
    const std::string SWAN_CTL_FILE = IPSEC_PIDDIR "/swanctl.conf";
    const std::string L2TP_CFG = IPSEC_PIDDIR "/xl2tpd.conf";
    bool isIpSecAccess_ = false;
    std::shared_ptr<ffrt::queue> vpnFfrtQueue_ = nullptr;
    const std::string OPENVPN_CONFIG_FILE = IPSEC_PIDDIR "/config.ovpn";
    const std::string VPN_STAGE_OPENVPN_RESTART = "restartopenvpn --config ";
    const std::string VPN_STAGE_OPENVPN_STOP = "stopopenvpn";
    const std::string VPN_STAGE_L2TP_STOP = "stopl2tp ";
    const std::string VPN_STAGE_SET_L2TP_CONF = "setl2tp ";
};
} // namespace nmd
} // namespace OHOS
#endif /* SYSTEM_VPN_WRAPPER_H */

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
#ifndef CLAT_MANAGER_H
#define CLAT_MANAGER_H

#include <linux/bpf.h>
#include <map>
#include <string>

#include "clat_constants.h"
#include "clat_utils.h"
#include "clatd.h"
#include "inet_addr.h"
#include "net_manager_native.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;
class ClatManager {
public:
    ClatManager();

    int32_t ClatStart(const std::string &v6Iface, int32_t netId, const std::string &nat64PrefixStr,
                      NetManagerNative *netsysService);

    int32_t ClatStop(const std::string &v6Iface, NetManagerNative *netsysService);

    int32_t AddClatRoute(int32_t netId, const std::string &tunIface, const std::string &v4Addr,
                         NetManagerNative *netsysService);

    int32_t DeleteClatRoute(int32_t netId, const std::string &tunIface, const std::string &v4Addr,
                            NetManagerNative *netsysService);

private:
    uint32_t GetFwmark(int32_t netId);

    int32_t GenerateClatSrcAddr(const std::string &v6Iface, uint32_t fwmark, const std::string &nat64PrefixStr,
                                INetAddr &v4Addr, INetAddr &v6Addr);

    int32_t CreateAndConfigureTunIface(const std::string &v6Iface, const std::string &tunIface, const INetAddr &v4Addr,
                                       NetManagerNative *netsysService, int &tunFd);

    int32_t CreateAndConfigureClatSocket(const std::string &v6Iface, const INetAddr &v6Addr, uint32_t fwmark,
                                         int &readSock6, int &writeSock6);
    int32_t AddNatBypassRules(const std::string &v6Iface, const std::string &v6Ip);
    int32_t DeleteNatBypassRules(const std::string &v6Iface);
    void CombineRestoreRules(const std::string &cmds, std::string &cmdSet);
    const std::string EnableByPassNatCmd(const std::string &v6Iface, const std::string &v6Ip);
    const std::string GetClatNetChains(const std::string &v6Iface);
    std::map<std::string, Clatd> clatds_;

    std::map<std::string, ClatdTracker> clatdTrackers_;
};
} // namespace nmd
} // namespace OHOS
#endif
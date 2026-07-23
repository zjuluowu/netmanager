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

#ifndef NETSYSNATIVE_NET_DIAG_WRAPPER_H
#define NETSYSNATIVE_NET_DIAG_WRAPPER_H

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <regex>

#include "i_net_diag_callback.h"
#include "netsys_net_diag_data.h"
#include "singleton.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace NetsysNative;
} // namespace
class NetDiagWrapper : public std::enable_shared_from_this<NetDiagWrapper> {
public:
    NetDiagWrapper();
    ~NetDiagWrapper();
    static std::shared_ptr<NetDiagWrapper> &GetInstance()
    {
        static std::shared_ptr<NetDiagWrapper> instance = std::make_shared<NetDiagWrapper>();
        return instance;
    }

    int32_t PingHost(const NetDiagPingOption &pingOption, const sptr<INetDiagCallback> &callback);
    int32_t GetRouteTable(std::list<NetDiagRouteTable> &routeTables);
    int32_t GetSocketsInfo(NetDiagProtocolType socketType, NetDiagSocketsInfo &socketsInfo);
    int32_t GetInterfaceConfig(std::list<NetDiagIfaceConfig> &configs, const std::string &ifaceName);
    int32_t UpdateInterfaceConfig(const NetDiagIfaceConfig &config, const std::string &ifaceName, bool add);
    int32_t SetInterfaceActiveState(const std::string &ifaceName, bool up);
    int32_t ExecuteCommandForResult(const std::string &command, std::string &result);

private:
    int32_t GeneratePingCommand(const NetDiagPingOption &pingOption, std::string &command);
    bool IsBlankLine(const std::string &line);
    void ExtractPingResult(const std::string &result, const sptr<INetDiagCallback> &callback);
    void ExtractPingHeader(const std::smatch &match, NetDiagPingResult &pingResult);
    void ExtractIcmpSeqInfo(const std::smatch &match, NetDiagPingResult &pingResult);
    void ExtractPingStatistics(const std::smatch &match, NetDiagPingResult &pingResult);
    void ExtractRouteTableInfo(const std::smatch &match, std::list<NetDiagRouteTable> &routeTables);
    void ExtractNetProtoSocketsInfo(const std::smatch &match, NetDiagSocketsInfo &socketsInfo);
    void ExtractUnixSocketsInfo(const std::smatch &match, NetDiagSocketsInfo &socketsInfo);
    void ExtractIfaceName(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo);
    void ExtractIfaceInet(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo);
    void ExtractIfaceInet6(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo);
    void ExtractIfaceMtu(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo);
    void ExtractIfaceTxQueueLen(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo);
    void ExtractIfaceTransDataBytes(const std::smatch &match, NetDiagIfaceConfig &ifaceInfo);
};
} // namespace nmd
} // namespace OHOS
#endif // NETSYSNATIVE_NET_DIAG_WRAPPER_H

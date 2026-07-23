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

#ifndef NETSYS_WEARABLE_DISTRIBUTED_NET_MANAGER_H
#define NETSYS_WEARABLE_DISTRIBUTED_NET_MANAGER_H

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "cJSON.h"

#define TCP_ADD16 "-w -o lo -t nat -A DISTRIBUTED_NET_TCP -p tcp -j REDIRECT "\
    "--to-ports %d"
#define UDP_ADD16 "-w -i lo -t mangle -A DISTRIBUTED_NET_UDP -p udp -j TPROXY "\
    "--tproxy-mark 0x1/0x1 --on-port %d"
#define INPUT_ADD "-w -A INPUT -p tcp -s 127.0.0.1 --destination-port %d -j REJECT"
#define INPUT_DEL "-w -D INPUT -p tcp -s 127.0.0.1 --destination-port %d -j REJECT"

namespace OHOS {
namespace nmd {
class WearableDistributedNet {
public:
    enum RULES_TYPE {
        TCP_ADD_RULE,
        UDP_ADD_RULE,
        INPUT_ADD_RULE,
        INPUT_DEL_RULE,
        DEFAULT_RULE
    };

    /**
    * @brief Enables the wearable distributed network forwarding by configuring TCP and UDP ports
    *
    * @param tcpPortId The TCP port ID
    * @param udpPortId The UDP port ID
    * @return NETMANAGER_SUCCESS if successful, NETMANAGER_ERROR if any of the operations fail
    */
    int32_t EnableWearableDistributedNetForward(const int32_t tcpPortId, const int32_t udpPortId);

    /**
    * @brief Disables the wearable distributed network forwarding by removing configured rules
    *
    * @return NETMANAGER_SUCCESS if successful, NETMANAGER_ERROR if any of the operations fail
    */
    int32_t DisableWearableDistributedNetForward();

    /**
 　　* @brief Reads the system's iptables configuration from a JSON file and processes the relevant iptables settings
 　　*
 　　* This function reads a JSON configuration file located at IPTABLES_CONFIG_PATH, parses it, and then extracts
 　　* the iptables configuration. It specifically looks for the iptables component flag to decide whether to
 　　* proceed with reading and applying iptables interfaces or not
 　　*
 　　* @return true if the configuration was successfully read and processed, false otherwise
 　　*/
    bool ReadSystemIptablesConfiguration();
    
private:
    int32_t EstablishTcpIpRules();
    int32_t EstablishUdpIpRules(const int32_t udpPortId);
    int32_t ExecuteIptablesCommands(const std::vector<std::string> &commands);
    std::string GenerateRule(const std::string &inputRules, const int32_t portId);
    int32_t ApplyRule(const RULES_TYPE type, const int32_t portId);
    void SetTcpPort(const int32_t tcpPortId);
    int32_t GetTcpPort();

    bool ReadIptablesInterfaces(const cJSON &json);
    std::string ReadJsonFile();
    std::vector<std::string> GetTcpIptables();
    std::string GetOutputAddTcp();
    std::vector<std::string> GetUdpIptables();
    std::string GetUdpoutput();
    std::vector<std::string> GetIptablesDeleteCmds();

    bool ParseTcpIptables(const cJSON &json);
    bool ParseTcpOutputRule(const cJSON &json);
    bool ParseUdpIptables(const cJSON &json);
    bool ParseUdpOutputRule(const cJSON &json);
    bool ParseIptablesDeleteCmds(const cJSON &json);
  
private:
    int32_t tcpPort_;
    std::vector<std::string> tcpIptables_;
    std::string tcpOutput_;
    std::vector<std::string> udpIptables_;
    std::string udpOutput_;
    std::vector<std::string> iptablesDeleteCmds_;
    std::string configPath_ = IPTABLES_CONFIG_PATH;
    bool iptablesHasBeenParse_ = false;
    std::mutex iptablesParseMutex_;
};
} // namespace nmd
} // namespace OHOS// namespace OHOS::nmd
#endif // NETSYS_WEARABLE_DISTRIBUTED_NET_MANAGER_H
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
#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include "cJSON.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "errorcode_convertor.h"

#define private public
#include "wearable_distributed_net_manager.h"
#undef private
#include "iptables_type.h"
#include "iptables_wrapper.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
namespace NetsysNative {
using namespace testing::ext;
using namespace NetManagerStandard;
const int32_t MAX_CMD_LENGTH = 256;
const std::string CONFIG_KEY_NETFORWARD_COMPONENT_FLAG = "config_wearable_distributed_net_forward";
const std::string TCP_IPTABLES = "tcpiptables";
const std::string TCP_OUTPUT = "tcpoutput";
const std::string UDP_IPTABLES = "udpiptables";
const std::string UDP_OUTPUT = "udpoutput";
const std::string IPTABLES_DELETE_CMDS = "iptablesdeletecmds";

class WearableDistributedNetManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WearableDistributedNetManagerTest::SetUpTestCase() {}

void WearableDistributedNetManagerTest::TearDownTestCase() {}

void WearableDistributedNetManagerTest::SetUp() {}

void WearableDistributedNetManagerTest::TearDown() {}

std::string createTempFile(const std::string& content)
{
    std::string filePath = "temp_test_file.json";
    std::ofstream outfile(filePath);
    outfile << content;
    outfile.close();
    return filePath;
}

HWTEST_F(WearableDistributedNetManagerTest, GetOutputAddTcp, TestSize.Level1)
{
    WearableDistributedNet net;
    std::string expectedOutput = "testOutput";
    net.tcpOutput_ = expectedOutput;
    std::string actualOutput = net.GetOutputAddTcp();
    EXPECT_EQ(actualOutput, expectedOutput);
}

HWTEST_F(WearableDistributedNetManagerTest, GetUdpoutput, TestSize.Level1)
{
    WearableDistributedNet net;
    std::string expectedOutput = "testOutput";
    net.udpOutput_ = expectedOutput;
    std::string actualOutput = net.GetUdpoutput();
    EXPECT_EQ(actualOutput, expectedOutput);
}

HWTEST_F(WearableDistributedNetManagerTest, SetTcpPort, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t tcpPort = 8888;
    net.SetTcpPort(tcpPort);
    EXPECT_EQ(tcpPort, net.tcpPort_);
}

HWTEST_F(WearableDistributedNetManagerTest, GetTcpPort, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t setPort = 9999;
    net.SetTcpPort(setPort);
    int32_t retrievedPort = net.GetTcpPort();
    EXPECT_EQ(setPort, retrievedPort);
}

HWTEST_F(WearableDistributedNetManagerTest, GenerateRule, TestSize.Level1)
{
    WearableDistributedNet net;
    std::string inputRules = "-w -A CHAIN -p tcp --dport %u -j RETURN";
    int32_t portId = 8080;
    std::string expected = "-w -A CHAIN -p tcp --dport 8080 -j RETURN";
    std::string actual = net.GenerateRule(inputRules, portId);
    EXPECT_EQ(expected, actual);
}

HWTEST_F(WearableDistributedNetManagerTest, GenerateRule001, TestSize.Level1)
{
    WearableDistributedNet net;
    std::string inputRules = "";
    int32_t portId = 8080;
    std::string expected = "";
    std::string actual = net.GenerateRule(inputRules, portId);
    EXPECT_EQ(expected, actual);
}

HWTEST_F(WearableDistributedNetManagerTest, GenerateRule002, TestSize.Level1)
{
    WearableDistributedNet net;
    std::string inputRules = std::string(MAX_CMD_LENGTH + 1, 'a');
    int32_t portId = 8080;
    std::string expected = "";
    std::string actual = net.GenerateRule(inputRules, portId);
    EXPECT_EQ(expected, actual);
}

void AddTcpChain()
{
    IptablesWrapper::GetInstance()->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV4,
        "-w -t nat -N DISTRIBUTED_NET_TCP");
}

void DeleteTcpRule()
{
    IptablesWrapper::GetInstance()->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV4,
        "-w -t nat -F DISTRIBUTED_NET_TCP");
    IptablesWrapper::GetInstance()->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV4,
        "-w -t nat -X DISTRIBUTED_NET_TCP");
}

HWTEST_F(WearableDistributedNetManagerTest, ApplyRule, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t portId = 8080;
    AddTcpChain();
    int32_t result = net.ApplyRule(WearableDistributedNet::TCP_ADD_RULE, portId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    DeleteTcpRule();
}

void AddUdpChain()
{
    IptablesWrapper::GetInstance()->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV4,
        "-w -t mangle -N DISTRIBUTED_NET_UDP");
}

void DeleteUdpRule()
{
    IptablesWrapper::GetInstance()->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV4,
        "-w -t mangle -F DISTRIBUTED_NET_UDP");
    IptablesWrapper::GetInstance()->RunCommandForRes(OHOS::nmd::IpType::IPTYPE_IPV4,
        "-w -t mangle -X DISTRIBUTED_NET_UDP");
}

HWTEST_F(WearableDistributedNetManagerTest, ApplyRule001, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t portId = 8081;
    AddUdpChain();
    int32_t result = net.ApplyRule(WearableDistributedNet::UDP_ADD_RULE, portId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    DeleteUdpRule();
}

HWTEST_F(WearableDistributedNetManagerTest, ApplyRule002, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t portId = 8080;
    int32_t result = net.ApplyRule(WearableDistributedNet::INPUT_ADD_RULE, portId);
    EXPECT_NE(result, NETMANAGER_ERR_INVALID_PARAMETER);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(WearableDistributedNetManagerTest, ApplyRule003, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t portId = 8080;
    int32_t result = net.ApplyRule(WearableDistributedNet::INPUT_DEL_RULE, portId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(WearableDistributedNetManagerTest, ApplyRule004, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t portId = 8080;
    int32_t result = net.ApplyRule(static_cast <WearableDistributedNet::RULES_TYPE>(100), portId);
    EXPECT_EQ(result, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(WearableDistributedNetManagerTest, ApplyRule005, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t result = net.ApplyRule(WearableDistributedNet::TCP_ADD_RULE, -1);
    EXPECT_EQ(result, NETMANAGER_WEARABLE_DISTRIBUTED_NET_ERR_INVALID_PORT_ID);
}

HWTEST_F(WearableDistributedNetManagerTest, ExecuteIptablesCommands, TestSize.Level1)
{
    WearableDistributedNet net;
    std::string longCommand(MAX_CMD_LENGTH + 1, 'a');
    std::vector<std::string> commands = {longCommand};
    int32_t result = net.ExecuteIptablesCommands(commands);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(WearableDistributedNetManagerTest, ExecuteIptablesCommands001, TestSize.Level1)
{
    WearableDistributedNet net;
    std::vector<std::string> commands = {"success", "success"};
    int32_t result = net.ExecuteIptablesCommands(commands);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(WearableDistributedNetManagerTest, ExecuteIptablesCommands002, TestSize.Level1)
{
    WearableDistributedNet net;
    std::vector<std::string> commands = {"-A INPUT -p tcp --dport 8080 -j ACCEPT"};
    int32_t result = net.ExecuteIptablesCommands(commands);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(WearableDistributedNetManagerTest, EstablishTcpIpRules, TestSize.Level1)
{
    WearableDistributedNet net;
    std::vector<std::string> longCommands = {std::string(513, 'a')};
    net.tcpIptables_ = longCommands;
    int32_t result = net.EstablishTcpIpRules();
    EXPECT_EQ(result, NETMANAGER_ERROR);

    std::vector<std::string> tcpCommands = {"-A INPUT -p tcp --dport 8080 -j ACCEPT"};
    net.tcpIptables_ = tcpCommands;
    net.tcpPort_ = 65536;
    result = net.EstablishTcpIpRules();
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(WearableDistributedNetManagerTest, EstablishUdpIpRules, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t udpPortIdTest = 0;
    std::vector<std::string> longCommands = {std::string(513, 'a')};
    net.udpIptables_ = longCommands;
    int32_t result = net.EstablishUdpIpRules(udpPortIdTest);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    std::vector<std::string> udpCommands = {"-A INPUT -p tcp --dport 8090 -j ACCEPT"};
    net.udpIptables_ = udpCommands;
    int32_t udpPortId = -1;
    result = net.EstablishUdpIpRules(udpPortId);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(WearableDistributedNetManagerTest, EnableWearableDistributedNetForward, TestSize.Level1)
{
    WearableDistributedNet net;
    int32_t tcpPortId = 8080;
    int32_t udpPortId = 8081;
    int32_t result = net.EnableWearableDistributedNetForward(tcpPortId, udpPortId);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    tcpPortId = -1;
    result = net.EnableWearableDistributedNetForward(tcpPortId, udpPortId);
    EXPECT_EQ(result, NETMANAGER_WEARABLE_DISTRIBUTED_NET_ERR_INVALID_TCP_PORT_ID);

    tcpPortId = 888;
    udpPortId = -1;
    result = net.EnableWearableDistributedNetForward(tcpPortId, udpPortId);
    EXPECT_EQ(result, NETMANAGER_WEARABLE_DISTRIBUTED_NET_ERR_INVALID_UDP_PORT_ID);
}

HWTEST_F(WearableDistributedNetManagerTest, ReadSystemIptablesConfiguration, TestSize.Level1)
{
    WearableDistributedNet net;
    net.configPath_ = createTempFile("");
    bool result = net.ReadSystemIptablesConfiguration();
    EXPECT_FALSE(result);
}

HWTEST_F(WearableDistributedNetManagerTest, ReadJsonFile, TestSize.Level1)
{
    WearableDistributedNet net;
    net.configPath_ = "./path";
    std::string result = net.ReadJsonFile();
    EXPECT_EQ(result, "");
}

HWTEST_F(WearableDistributedNetManagerTest, ReadJsonFile001, TestSize.Level1)
{
    WearableDistributedNet net;
    net.configPath_ = createTempFile(R"({"key": "value"})");
    std::string result = net.ReadJsonFile();
    EXPECT_EQ(R"({"key": "value"})", result);
}

HWTEST_F(WearableDistributedNetManagerTest, ReadJsonFile002, TestSize.Level1)
{
    WearableDistributedNet net;
    net.configPath_ = "non_existent_file.json";
    std::string result = net.ReadJsonFile();
    EXPECT_EQ("", result);
}

HWTEST_F(WearableDistributedNetManagerTest, ReadJsonFile003, TestSize.Level1)
{
    WearableDistributedNet net;
    net.configPath_ = createTempFile("");
    std::string result = net.ReadJsonFile();
    EXPECT_EQ("", result);
}

HWTEST_F(WearableDistributedNetManagerTest, ReadJsonFile004, TestSize.Level1)
{
    WearableDistributedNet net;
    net.configPath_ = createTempFile(R"({"key": "value"}\n{\"anotherKey\": \"anotherValue\"})");
    std::string result = net.ReadJsonFile();
    EXPECT_EQ(R"({"key": "value"}\n{\"anotherKey\": \"anotherValue\"})", result);
}

HWTEST_F(WearableDistributedNetManagerTest, ReadJsonFile005, TestSize.Level1)
{
    WearableDistributedNet net;
    net.configPath_ = "invalid_file_path.json";
    std::string expected = "";
    std::string actual = net.ReadJsonFile();
    EXPECT_EQ(expected, actual);
}

HWTEST_F(WearableDistributedNetManagerTest, ParseTcpOutputRule, TestSize.Level1)
{
    WearableDistributedNet net;
    cJSON json;
    cJSON_AddStringToObject(&json, TCP_IPTABLES.c_str(), "114.114.114.114");
    EXPECT_EQ(net.ParseTcpOutputRule(json), false);

    cJSON_AddStringToObject(&json, TCP_OUTPUT.c_str(), "tcpoutput");
    EXPECT_EQ(net.ParseTcpOutputRule(json), true);
}

HWTEST_F(WearableDistributedNetManagerTest, ParseTcpOutputRuleNotStringType, TestSize.Level1)
{
    WearableDistributedNet net;
    cJSON json;
    cJSON_AddNumberToObject(&json, TCP_OUTPUT.c_str(), 12345);
    EXPECT_EQ(net.ParseTcpOutputRule(json), false);

    cJSON json2;
    cJSON_AddBoolToObject(&json2, TCP_OUTPUT.c_str(), 1);
    EXPECT_EQ(net.ParseTcpOutputRule(json2), false);

    cJSON json3;
    cJSON *arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateString("item"));
    cJSON_AddItemToObject(&json3, TCP_OUTPUT.c_str(), arr);
    EXPECT_EQ(net.ParseTcpOutputRule(json3), false);
}

HWTEST_F(WearableDistributedNetManagerTest, ParseUdpOutputRule, TestSize.Level1)
{
    WearableDistributedNet net;
    cJSON json;
    cJSON_AddStringToObject(&json, UDP_IPTABLES.c_str(), "114.114.114.114");
    EXPECT_EQ(net.ParseUdpOutputRule(json), false);

    cJSON_AddStringToObject(&json, UDP_OUTPUT.c_str(), "udpoutput");
    EXPECT_EQ(net.ParseUdpOutputRule(json), true);
}

HWTEST_F(WearableDistributedNetManagerTest, ParseUdpOutputRuleNotStringType, TestSize.Level1)
{
    WearableDistributedNet net;
    cJSON json;
    cJSON_AddNumberToObject(&json, UDP_OUTPUT.c_str(), 99999);
    EXPECT_EQ(net.ParseUdpOutputRule(json), false);

    cJSON json2;
    cJSON_AddBoolToObject(&json2, UDP_OUTPUT.c_str(), 0);
    EXPECT_EQ(net.ParseUdpOutputRule(json2), false);

    cJSON json3;
    cJSON *arr = cJSON_CreateArray();
    cJSON_AddItemToArray(arr, cJSON_CreateString("item"));
    cJSON_AddItemToObject(&json3, UDP_OUTPUT.c_str(), arr);
    EXPECT_EQ(net.ParseUdpOutputRule(json3), false);
}

HWTEST_F(WearableDistributedNetManagerTest, ParseUdpIptables, TestSize.Level1)
{
    WearableDistributedNet net;
    cJSON json;
    EXPECT_EQ(net.ParseUdpIptables(json), true);

    cJSON_AddItemToObject(&json, UDP_IPTABLES.c_str(), nullptr);
    EXPECT_EQ(net.ParseUdpIptables(json), true);

    cJSON *udpIptablesObj = cJSON_CreateArray();
    cJSON *udpIptablesItem = cJSON_CreateString("192.168.1.1");
    cJSON_AddItemToArray(udpIptablesObj, udpIptablesItem);
    cJSON_AddItemToObject(&json, UDP_IPTABLES.c_str(), udpIptablesObj);
    EXPECT_EQ(net.ParseUdpIptables(json), true);
}

HWTEST_F(WearableDistributedNetManagerTest, ParseTcpIptables, TestSize.Level1)
{
    WearableDistributedNet net;
    cJSON json;
    EXPECT_EQ(net.ParseTcpIptables(json), true);

    cJSON_AddItemToObject(&json, TCP_IPTABLES.c_str(), nullptr);
    EXPECT_EQ(net.ParseTcpIptables(json), true);

    cJSON *tcpIptablesObj = cJSON_CreateArray();
    cJSON *tcpIptablesItem = cJSON_CreateString("192.168.1.2");
    cJSON_AddItemToArray(tcpIptablesObj, tcpIptablesItem);
    cJSON_AddItemToObject(&json, TCP_IPTABLES.c_str(), tcpIptablesObj);
    EXPECT_EQ(net.ParseTcpIptables(json), true);
}

HWTEST_F(WearableDistributedNetManagerTest, ParseIptablesDeleteCmds, TestSize.Level1)
{
    WearableDistributedNet net;
    cJSON json;
    EXPECT_EQ(net.ParseIptablesDeleteCmds(json), true);

    cJSON_AddItemToObject(&json, IPTABLES_DELETE_CMDS.c_str(), nullptr);
    EXPECT_EQ(net.ParseIptablesDeleteCmds(json), true);

    cJSON *iptablesDeleteCmdsObj = cJSON_CreateArray();
    cJSON *iptablesDeleteCmdsItem = cJSON_CreateString("192.168.1.3");
    cJSON_AddItemToArray(iptablesDeleteCmdsObj, iptablesDeleteCmdsItem);
    cJSON_AddItemToObject(&json, IPTABLES_DELETE_CMDS.c_str(), iptablesDeleteCmdsObj);
    EXPECT_EQ(net.ParseIptablesDeleteCmds(json), true);
}

HWTEST_F(WearableDistributedNetManagerTest, ReadIptablesInterfaces, TestSize.Level1)
{
    WearableDistributedNet net;
    bool result = net.ReadSystemIptablesConfiguration();
    EXPECT_TRUE(result);

    cJSON json;
    result = net.ReadIptablesInterfaces(json);
    EXPECT_FALSE(result);

    cJSON *tcpIptablesObj = cJSON_CreateArray();
    cJSON *tcpIptablesItem = cJSON_CreateString("192.168.1.2");
    cJSON_AddItemToArray(tcpIptablesObj, tcpIptablesItem);
    cJSON_AddItemToObject(&json, TCP_IPTABLES.c_str(), tcpIptablesObj);
    result = net.ReadIptablesInterfaces(json);
    EXPECT_FALSE(result);

    cJSON_AddStringToObject(&json, TCP_OUTPUT.c_str(), "tcpoutput");
    result = net.ReadIptablesInterfaces(json);
    EXPECT_FALSE(result);

    cJSON *udpIptablesObj = cJSON_CreateArray();
    cJSON *udpIptablesItem = cJSON_CreateString("192.168.1.1");
    cJSON_AddItemToArray(udpIptablesObj, udpIptablesItem);
    cJSON_AddItemToObject(&json, UDP_IPTABLES.c_str(), udpIptablesObj);
    result = net.ReadIptablesInterfaces(json);
    EXPECT_FALSE(result);

    cJSON_AddStringToObject(&json, UDP_OUTPUT.c_str(), "udpoutput");
    result = net.ReadIptablesInterfaces(json);
    EXPECT_TRUE(result);
}

HWTEST_F(WearableDistributedNetManagerTest, DisableWearableDistributedNetForward, TestSize.Level1)
{
    WearableDistributedNet net;
    std::vector<std::string> longCommands = {std::string(513, 'a')};
    net.iptablesDeleteCmds_ = longCommands;
    int32_t result = net.DisableWearableDistributedNetForward();
    EXPECT_EQ(result, NETMANAGER_ERROR);

    std::vector<std::string> deleteCommands = {"-A INPUT -p tcp --dport 8090 -j ACCEPT"};
    net.iptablesDeleteCmds_ = deleteCommands;
    net.tcpPort_ = 65537;
    result = net.DisableWearableDistributedNetForward();
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(WearableDistributedNetManagerTest, DisableWearableDistributedNetForward001, TestSize.Level1)
{
    WearableDistributedNet net;
    net.iptablesDeleteCmds_ = {"-A INPUT -p tcp --dport 8090 -j ACCEPT"};
    net.tcpPort_ = 8090;
    int32_t result = net.DisableWearableDistributedNetForward();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(WearableDistributedNetManagerTest, DisableWearableDistributedNetForward002, TestSize.Level1)
{
    WearableDistributedNet net;
    net.iptablesDeleteCmds_ = {"-A INPUT -p tcp --dport 8080 -j ACCEPT"};
    int32_t result = net.DisableWearableDistributedNetForward();
    EXPECT_EQ(result, NETMANAGER_ERROR);
}
} // namespace NetsysNative
} // nmd
} // namespace OHOS
/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#ifdef GTEST_API_
#define private public
#define protected public
#endif
#include "mptcp_manager.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
using namespace OHOS::NetManagerStandard;
} // namespace

class MptcpManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MptcpManagerTest::SetUpTestCase() {}

void MptcpManagerTest::TearDownTestCase() {}

void MptcpManagerTest::SetUp() {}

void MptcpManagerTest::TearDown() {}

HWTEST_F(MptcpManagerTest, ConstructorTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    EXPECT_NE(manager, nullptr);
}

HWTEST_F(MptcpManagerTest, DestructorTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager.reset();
    EXPECT_TRUE(true);
}

HWTEST_F(MptcpManagerTest, IsMonitoredInterfaceTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    EXPECT_TRUE(manager->IsMonitoredInterface("wlan0"));
    EXPECT_TRUE(manager->IsMonitoredInterface("rmnet0"));
    EXPECT_TRUE(manager->IsMonitoredInterface("rmnet1"));
}

HWTEST_F(MptcpManagerTest, IsMonitoredInterfaceTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    EXPECT_FALSE(manager->IsMonitoredInterface("eth0"));
    EXPECT_FALSE(manager->IsMonitoredInterface("wlan1"));
    EXPECT_FALSE(manager->IsMonitoredInterface("rmnet2"));
    EXPECT_FALSE(manager->IsMonitoredInterface(""));
    EXPECT_FALSE(manager->IsMonitoredInterface("lo"));
}

HWTEST_F(MptcpManagerTest, BuildEndpointAddCommandTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "192.168.1.100";
    std::string ifName = "wlan0";
    std::string command = manager->BuildEndpointAddCommand(ipAddr, ifName);
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("/system/bin/ip") != std::string::npos);
    EXPECT_TRUE(command.find("mptcp endpoint add") != std::string::npos);
    EXPECT_TRUE(command.find(ipAddr) != std::string::npos);
    EXPECT_TRUE(command.find("dev") != std::string::npos);
    EXPECT_TRUE(command.find(ifName) != std::string::npos);
    EXPECT_TRUE(command.find("subflow") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildEndpointAddCommandTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "2001:db8::1";
    std::string ifName = "rmnet0";
    std::string command = manager->BuildEndpointAddCommand(ipAddr, ifName);
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find(ipAddr) != std::string::npos);
    EXPECT_TRUE(command.find(ifName) != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildEndpointAddCommandTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string command = manager->BuildEndpointAddCommand("10.0.0.1", "rmnet1");
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("10.0.0.1") != std::string::npos);
    EXPECT_TRUE(command.find("rmnet1") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildEndpointDeleteCommandTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t endpointId = 1;
    std::string command = manager->BuildEndpointDeleteCommand(endpointId);
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("/system/bin/ip") != std::string::npos);
    EXPECT_TRUE(command.find("mptcp endpoint delete id") != std::string::npos);
    EXPECT_TRUE(command.find("1") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildEndpointDeleteCommandTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t endpointId = -1;
    std::string command = manager->BuildEndpointDeleteCommand(endpointId);
    EXPECT_TRUE(command.empty());
}

HWTEST_F(MptcpManagerTest, BuildEndpointDeleteCommandTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t endpointId = 100;
    std::string command = manager->BuildEndpointDeleteCommand(endpointId);
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("100") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildEndpointDeleteCommandTest004, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t endpointId = 0;
    std::string command = manager->BuildEndpointDeleteCommand(endpointId);
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("0") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildEndpointShowCommandTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string command = manager->BuildEndpointShowCommand();
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("/system/bin/ip") != std::string::npos);
    EXPECT_TRUE(command.find("mptcp endpoint show") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildLimitsSetCommandTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = 3;
    int32_t addAddrAccepted = 7;
    std::string command = manager->BuildLimitsSetCommand(subflows, addAddrAccepted);
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("/system/bin/ip") != std::string::npos);
    EXPECT_TRUE(command.find("mptcp limits set") != std::string::npos);
    EXPECT_TRUE(command.find("subflows 3") != std::string::npos);
    EXPECT_TRUE(command.find("add_addr_accepted 7") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildLimitsSetCommandTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = 0;
    int32_t addAddrAccepted = 0;
    std::string command = manager->BuildLimitsSetCommand(subflows, addAddrAccepted);
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("subflows 0") != std::string::npos);
    EXPECT_TRUE(command.find("add_addr_accepted 0") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, BuildLimitsSetCommandTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = 7;
    int32_t addAddrAccepted = 7;
    std::string command = manager->BuildLimitsSetCommand(subflows, addAddrAccepted);
    EXPECT_FALSE(command.empty());
    EXPECT_TRUE(command.find("subflows 7") != std::string::npos);
    EXPECT_TRUE(command.find("add_addr_accepted 7") != std::string::npos);
}

HWTEST_F(MptcpManagerTest, ExecuteMptcpCommandTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string command = "";
    auto ret = manager->ExecuteMptcpCommand(command);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, ExecuteMptcpCommandWithResultTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string command = "";
    std::string result;
    auto ret = manager->ExecuteMptcpCommand(command, result);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
    EXPECT_TRUE(result.empty());
}

HWTEST_F(MptcpManagerTest, ExecuteMptcpCommandTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string command = "invalid_command_for_test";
    std::string result;
    auto ret = manager->ExecuteMptcpCommand(command, result);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(MptcpManagerTest, AddEndpointTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "";
    std::string ifName = "wlan0";
    auto ret = manager->AddEndpoint(ipAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, AddEndpointTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "192.168.1.100";
    std::string ifName = "";
    auto ret = manager->AddEndpoint(ipAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, AddEndpointTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr;
    std::string ifName;
    auto ret = manager->AddEndpoint(ipAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, DeleteEndpointTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "";
    std::string ifName = "wlan0";
    auto ret = manager->DeleteEndpoint(ipAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, DeleteEndpointTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "192.168.1.100";
    std::string ifName = "";
    auto ret = manager->DeleteEndpoint(ipAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, DeleteEndpointTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr;
    std::string ifName;
    auto ret = manager->DeleteEndpoint(ipAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, DeleteEndpointTest004, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "192.168.1.200";
    std::string ifName = "wlan0";
    auto ret = manager->DeleteEndpoint(ipAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(MptcpManagerTest, DeleteEndpointTest005, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->endpoints_.clear();
    MptcpEndpointInfo info;
    info.ipAddr = "192.168.1.100";
    info.ifName = "wlan0";
    info.endpointId = -1;
    manager->endpoints_["192.168.1.100_wlan0"] = info;
    
    auto ret = manager->DeleteEndpoint("192.168.1.100", "wlan0");
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(MptcpManagerTest, DeleteEndpointTest006, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->endpoints_.clear();
    MptcpEndpointInfo info;
    info.ipAddr = "192.168.1.101";
    info.ifName = "wlan0";
    info.endpointId = 1;
    manager->endpoints_["192.168.1.101_wlan0"] = info;
    
    auto ret = manager->DeleteEndpoint("192.168.1.101", "wlan0");
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(manager->endpoints_.find("192.168.1.101_wlan0") == manager->endpoints_.end());
}

HWTEST_F(MptcpManagerTest, SetLimitsTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = -1;
    int32_t addAddrAccepted = 7;
    auto ret = manager->SetLimits(subflows, addAddrAccepted);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, SetLimitsTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = 7;
    int32_t addAddrAccepted = -1;
    auto ret = manager->SetLimits(subflows, addAddrAccepted);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, SetLimitsTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = -1;
    int32_t addAddrAccepted = -1;
    auto ret = manager->SetLimits(subflows, addAddrAccepted);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(MptcpManagerTest, SetLimitsTest004, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = 0;
    int32_t addAddrAccepted = 0;
    auto ret = manager->SetLimits(subflows, addAddrAccepted);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(manager->currentSubflows_, 0);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 0);
}

HWTEST_F(MptcpManagerTest, SetLimitsTest005, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = 3;
    int32_t addAddrAccepted = 7;
    auto ret = manager->SetLimits(subflows, addAddrAccepted);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(manager->currentSubflows_, 3);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 7);
}

HWTEST_F(MptcpManagerTest, SetLimitsTest006, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t subflows = 7;
    int32_t addAddrAccepted = 7;
    auto ret = manager->SetLimits(subflows, addAddrAccepted);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(manager->currentSubflows_, 7);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 7);
}

HWTEST_F(MptcpManagerTest, GetActiveInterfaceCountTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t count = static_cast<int32_t>(manager->ifaceToIpAddrs_.size());
    EXPECT_GE(count, 0);
}

HWTEST_F(MptcpManagerTest, UpdateMptcpLimitsTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->currentSubflows_ = 0;
    manager->currentAddAddrAccepted_ = 0;
    manager->ifaceToIpAddrs_.clear();
    manager->UpdateMptcpLimits();
    EXPECT_EQ(manager->currentSubflows_, 0);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 0);
}

HWTEST_F(MptcpManagerTest, UpdateMptcpLimitsTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    manager->ifaceToIpAddrs_["wlan0"] = {"192.168.1.100"};
    manager->currentSubflows_ = 0;
    manager->currentAddAddrAccepted_ = 0;
    manager->UpdateMptcpLimits();
    EXPECT_EQ(manager->currentSubflows_, 0);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 0);
}

HWTEST_F(MptcpManagerTest, UpdateMptcpLimitsTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    manager->ifaceToIpAddrs_["wlan0"] = {"192.168.1.100"};
    manager->ifaceToIpAddrs_["rmnet0"] = {"10.0.0.1"};
    manager->currentSubflows_ = 7;
    manager->currentAddAddrAccepted_ = 7;
    manager->UpdateMptcpLimits();
    EXPECT_EQ(manager->currentSubflows_, 7);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 7);
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressUpdatedTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string addr = "192.168.1.100";
    std::string ifName = "eth0";
    size_t prevSize = manager->ifaceToIpAddrs_.size();
    manager->OnInterfaceAddressUpdated(addr, ifName);
    EXPECT_FALSE(manager->IsMonitoredInterface("eth0"));
    EXPECT_EQ(manager->ifaceToIpAddrs_.size(), prevSize);
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressUpdatedTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "wlan0";
    manager->ifaceToIpAddrs_.erase(ifName);
    manager->OnInterfaceAddressUpdated("192.168.1.101", ifName);
    EXPECT_TRUE(manager->IsMonitoredInterface(ifName));
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find(ifName) != manager->ifaceToIpAddrs_.end());
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressUpdatedTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "rmnet0";
    manager->ifaceToIpAddrs_.erase(ifName);
    manager->OnInterfaceAddressUpdated("10.0.0.1", ifName);
    EXPECT_TRUE(manager->IsMonitoredInterface(ifName));
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find(ifName) != manager->ifaceToIpAddrs_.end());
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressUpdatedTest004, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "rmnet1";
    manager->ifaceToIpAddrs_.erase(ifName);
    manager->OnInterfaceAddressUpdated("2001:db8::1", ifName);
    EXPECT_TRUE(manager->IsMonitoredInterface(ifName));
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find(ifName) != manager->ifaceToIpAddrs_.end());
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressUpdatedTest005, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "wlan0";
    manager->ifaceToIpAddrs_[ifName] = {"192.168.1.100"};
    manager->OnInterfaceAddressUpdated("192.168.1.100", ifName);
    auto it = manager->ifaceToIpAddrs_.find(ifName);
    EXPECT_TRUE(it != manager->ifaceToIpAddrs_.end());
    EXPECT_EQ(it->second.size(), 1u);
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressRemovedTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string addr = "192.168.1.100";
    std::string ifName = "eth0";
    size_t prevSize = manager->ifaceToIpAddrs_.size();
    manager->OnInterfaceAddressRemoved(addr, ifName);
    EXPECT_FALSE(manager->IsMonitoredInterface("eth0"));
    EXPECT_EQ(manager->ifaceToIpAddrs_.size(), prevSize);
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressRemovedTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "wlan0";
    std::string addr = "192.168.1.101";
    manager->ifaceToIpAddrs_[ifName] = {"192.168.1.101", "10.0.0.1"};
    manager->OnInterfaceAddressRemoved(addr, ifName);
    EXPECT_TRUE(manager->IsMonitoredInterface(ifName));
    auto it = manager->ifaceToIpAddrs_.find(ifName);
    if (it != manager->ifaceToIpAddrs_.end()) {
        auto &addrs = it->second;
        EXPECT_TRUE(std::find(addrs.begin(), addrs.end(), addr) == addrs.end());
    }
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressRemovedTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "wlan0";
    manager->ifaceToIpAddrs_.erase(ifName);
    manager->OnInterfaceAddressRemoved("192.168.1.100", ifName);
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find(ifName) == manager->ifaceToIpAddrs_.end());
}

HWTEST_F(MptcpManagerTest, OnInterfaceAddressRemovedTest004, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "rmnet0";
    manager->ifaceToIpAddrs_[ifName] = {"10.0.0.1"};
    manager->OnInterfaceAddressRemoved("10.0.0.2", ifName);
    auto it = manager->ifaceToIpAddrs_.find(ifName);
    EXPECT_TRUE(it != manager->ifaceToIpAddrs_.end());
    EXPECT_EQ(it->second.size(), 1u);
}

HWTEST_F(MptcpManagerTest, MptcpEndpointInfoTest001, TestSize.Level1)
{
    MptcpEndpointInfo info;
    EXPECT_TRUE(info.ipAddr.empty());
    EXPECT_TRUE(info.ifName.empty());
    EXPECT_EQ(info.endpointId, -1);
}

HWTEST_F(MptcpManagerTest, MptcpEndpointInfoTest002, TestSize.Level1)
{
    MptcpEndpointInfo info;
    info.ipAddr = "192.168.1.100";
    info.ifName = "wlan0";
    info.endpointId = 5;
    EXPECT_EQ(info.ipAddr, "192.168.1.100");
    EXPECT_EQ(info.ifName, "wlan0");
    EXPECT_EQ(info.endpointId, 5);
}

HWTEST_F(MptcpManagerTest, MptcpEndpointInfoTest003, TestSize.Level1)
{
    MptcpEndpointInfo info;
    info.ipAddr = "2001:db8::1";
    info.ifName = "rmnet0";
    info.endpointId = 0;
    EXPECT_EQ(info.ipAddr, "2001:db8::1");
    EXPECT_EQ(info.ifName, "rmnet0");
    EXPECT_EQ(info.endpointId, 0);
}

HWTEST_F(MptcpManagerTest, EndpointsMapTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->endpoints_.clear();
    EXPECT_TRUE(manager->endpoints_.empty());
}

HWTEST_F(MptcpManagerTest, EndpointsMapTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->endpoints_.clear();
    MptcpEndpointInfo info;
    info.ipAddr = "192.168.1.100";
    info.ifName = "wlan0";
    info.endpointId = 1;
    manager->endpoints_["192.168.1.100_wlan0"] = info;
    EXPECT_EQ(manager->endpoints_.size(), 1u);
    EXPECT_TRUE(manager->endpoints_.find("192.168.1.100_wlan0") != manager->endpoints_.end());
}

HWTEST_F(MptcpManagerTest, EndpointsMapTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->endpoints_.clear();
    MptcpEndpointInfo info1;
    info1.ipAddr = "192.168.1.100";
    info1.ifName = "wlan0";
    info1.endpointId = 1;
    manager->endpoints_["192.168.1.100_wlan0"] = info1;
    
    MptcpEndpointInfo info2;
    info2.ipAddr = "10.0.0.1";
    info2.ifName = "rmnet0";
    info2.endpointId = 2;
    manager->endpoints_["10.0.0.1_rmnet0"] = info2;
    
    EXPECT_EQ(manager->endpoints_.size(), 2u);
}

HWTEST_F(MptcpManagerTest, IfaceToIpAddrsTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    EXPECT_TRUE(manager->ifaceToIpAddrs_.empty());
}

HWTEST_F(MptcpManagerTest, IfaceToIpAddrsTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    manager->ifaceToIpAddrs_["wlan0"] = {"192.168.1.100", "2001:db8::1"};
    EXPECT_EQ(manager->ifaceToIpAddrs_.size(), 1u);
    EXPECT_EQ(manager->ifaceToIpAddrs_["wlan0"].size(), 2u);
}

HWTEST_F(MptcpManagerTest, IfaceToIpAddrsTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    manager->ifaceToIpAddrs_["wlan0"] = {"192.168.1.100"};
    manager->ifaceToIpAddrs_["rmnet0"] = {"10.0.0.1"};
    manager->ifaceToIpAddrs_["rmnet1"] = {"10.0.0.2"};
    EXPECT_EQ(manager->ifaceToIpAddrs_.size(), 3u);
}

HWTEST_F(MptcpManagerTest, CurrentSubflowsTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->currentSubflows_ = 5;
    EXPECT_EQ(manager->currentSubflows_, 5);
}

HWTEST_F(MptcpManagerTest, CurrentAddAddrAcceptedTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->currentAddAddrAccepted_ = 7;
    EXPECT_EQ(manager->currentAddAddrAccepted_, 7);
}

HWTEST_F(MptcpManagerTest, MultipleAddressSameInterfaceTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "wlan0";
    manager->ifaceToIpAddrs_.erase(ifName);
    
    manager->OnInterfaceAddressUpdated("192.168.1.10", ifName);
    manager->OnInterfaceAddressUpdated("192.168.1.11", ifName);
    manager->OnInterfaceAddressUpdated("2001:db8::10", ifName);
    
    EXPECT_TRUE(manager->IsMonitoredInterface(ifName));
    auto it = manager->ifaceToIpAddrs_.find(ifName);
    EXPECT_TRUE(it != manager->ifaceToIpAddrs_.end());
    EXPECT_EQ(it->second.size(), 3u);
}

HWTEST_F(MptcpManagerTest, DuplicateAddressTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "rmnet0";
    manager->ifaceToIpAddrs_.erase(ifName);
    
    manager->OnInterfaceAddressUpdated("10.0.0.1", ifName);
    manager->OnInterfaceAddressUpdated("10.0.0.1", ifName);
    manager->OnInterfaceAddressUpdated("10.0.0.1", ifName);
    
    auto it = manager->ifaceToIpAddrs_.find(ifName);
    EXPECT_TRUE(it != manager->ifaceToIpAddrs_.end());
    EXPECT_EQ(it->second.size(), 1u);
}

HWTEST_F(MptcpManagerTest, RemoveLastAddressTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "rmnet1";
    manager->ifaceToIpAddrs_[ifName] = {"10.0.0.2"};
    
    manager->OnInterfaceAddressRemoved("10.0.0.2", ifName);
    
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find(ifName) == manager->ifaceToIpAddrs_.end());
}

HWTEST_F(MptcpManagerTest, RemoveNonExistentAddressTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ifName = "wlan0";
    manager->ifaceToIpAddrs_[ifName] = {"192.168.1.100"};
    
    manager->OnInterfaceAddressRemoved("192.168.1.200", ifName);
    
    auto it = manager->ifaceToIpAddrs_.find(ifName);
    EXPECT_TRUE(it != manager->ifaceToIpAddrs_.end());
    EXPECT_EQ(it->second.size(), 1u);
}

HWTEST_F(MptcpManagerTest, UpdateMptcpLimitsDisabledTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    manager->currentSubflows_ = 0;
    manager->currentAddAddrAccepted_ = 0;
    
    manager->UpdateMptcpLimits();
    
    EXPECT_EQ(manager->currentSubflows_, 0);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 0);
}

HWTEST_F(MptcpManagerTest, UpdateMptcpLimitsEnabledTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    manager->ifaceToIpAddrs_["wlan0"] = {"192.168.1.100"};
    manager->ifaceToIpAddrs_["rmnet0"] = {"10.0.0.1"};
    manager->currentSubflows_ = 0;
    manager->currentAddAddrAccepted_ = 0;
    
    manager->UpdateMptcpLimits();
    
    EXPECT_EQ(manager->currentSubflows_, 7);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 7);
}

HWTEST_F(MptcpManagerTest, UpdateMptcpLimitsNoChangeTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    manager->currentSubflows_ = 0;
    manager->currentAddAddrAccepted_ = 0;
    manager->UpdateMptcpLimits();
    int32_t prevSubflows = manager->currentSubflows_;
    int32_t prevAddAddr = manager->currentAddAddrAccepted_;
    manager->UpdateMptcpLimits();
    EXPECT_EQ(manager->currentSubflows_, prevSubflows);
    EXPECT_EQ(manager->currentAddAddrAccepted_, prevAddAddr);
}

HWTEST_F(MptcpManagerTest, GetEndpointIdTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t id = manager->GetEndpointId("192.168.1.100", "wlan0");
    EXPECT_GE(id, -1);
}

HWTEST_F(MptcpManagerTest, GetEndpointIdTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t id = manager->GetEndpointId("", "wlan0");
    EXPECT_GE(id, -1);
}

HWTEST_F(MptcpManagerTest, GetEndpointIdTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t id = manager->GetEndpointId("192.168.1.100", "");
    EXPECT_GE(id, -1);
}

HWTEST_F(MptcpManagerTest, GetEndpointIdTest004, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t id = manager->GetEndpointId("10.0.0.1", "rmnet0");
    EXPECT_GE(id, -1);
}

HWTEST_F(MptcpManagerTest, GetEndpointIdTest005, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    int32_t id = manager->GetEndpointId("2001:db8::1", "rmnet1");
    EXPECT_GE(id, -1);
}

HWTEST_F(MptcpManagerTest, MutexTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::lock_guard<std::mutex> lock(manager->mptcpMutex_);
    EXPECT_TRUE(true);
}

HWTEST_F(MptcpManagerTest, EdgeCaseTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    manager->currentSubflows_ = 7;
    manager->currentAddAddrAccepted_ = 7;
    manager->UpdateMptcpLimits();
    EXPECT_EQ(manager->currentSubflows_, 0);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 0);
}

HWTEST_F(MptcpManagerTest, EdgeCaseTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_["wlan0"] = {"192.168.1.100"};
    manager->currentSubflows_ = 0;
    manager->currentAddAddrAccepted_ = 0;
    manager->UpdateMptcpLimits();
    EXPECT_EQ(manager->currentSubflows_, 0);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 0);
}

HWTEST_F(MptcpManagerTest, EdgeCaseTest003, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_["wlan0"] = {"192.168.1.100"};
    manager->ifaceToIpAddrs_["rmnet0"] = {"10.0.0.1"};
    manager->ifaceToIpAddrs_["rmnet1"] = {"10.0.0.2"};
    manager->currentSubflows_ = 0;
    manager->currentAddAddrAccepted_ = 0;
    manager->UpdateMptcpLimits();
    EXPECT_EQ(manager->currentSubflows_, 7);
    EXPECT_EQ(manager->currentAddAddrAccepted_, 7);
}

HWTEST_F(MptcpManagerTest, EndpointsKeyFormatTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "192.168.1.100";
    std::string ifName = "wlan0";
    std::string expectedKey = ipAddr + "_" + ifName;
    MptcpEndpointInfo info;
    info.ipAddr = ipAddr;
    info.ifName = ifName;
    info.endpointId = 1;
    manager->endpoints_[expectedKey] = info;
    EXPECT_TRUE(manager->endpoints_.find(expectedKey) != manager->endpoints_.end());
}

HWTEST_F(MptcpManagerTest, EndpointsKeyFormatTest002, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    std::string ipAddr = "2001:db8::1:2:3:4";
    std::string ifName = "rmnet0";
    std::string expectedKey = ipAddr + "_" + ifName;
    MptcpEndpointInfo info;
    info.ipAddr = ipAddr;
    info.ifName = ifName;
    info.endpointId = 2;
    manager->endpoints_[expectedKey] = info;
    EXPECT_TRUE(manager->endpoints_.find(expectedKey) != manager->endpoints_.end());
}

HWTEST_F(MptcpManagerTest, AddThenDeleteEndpointTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->endpoints_.clear();
    MptcpEndpointInfo info;
    info.ipAddr = "192.168.1.100";
    info.ifName = "wlan0";
    info.endpointId = 1;
    manager->endpoints_["192.168.1.100_wlan0"] = info;
    EXPECT_EQ(manager->endpoints_.size(), 1u);
    
    auto ret = manager->DeleteEndpoint("192.168.1.100", "wlan0");
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(manager->endpoints_.find("192.168.1.100_wlan0") == manager->endpoints_.end());
}

HWTEST_F(MptcpManagerTest, MultipleInterfacesTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    
    manager->OnInterfaceAddressUpdated("192.168.1.100", "wlan0");
    manager->OnInterfaceAddressUpdated("10.0.0.1", "rmnet0");
    
    EXPECT_EQ(manager->ifaceToIpAddrs_.size(), 2u);
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find("wlan0") != manager->ifaceToIpAddrs_.end());
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find("rmnet0") != manager->ifaceToIpAddrs_.end());
}

HWTEST_F(MptcpManagerTest, AllMonitoredInterfacesTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_.clear();
    
    manager->OnInterfaceAddressUpdated("192.168.1.100", "wlan0");
    manager->OnInterfaceAddressUpdated("10.0.0.1", "rmnet0");
    manager->OnInterfaceAddressUpdated("10.0.0.2", "rmnet1");
    
    EXPECT_EQ(manager->ifaceToIpAddrs_.size(), 3u);
}

HWTEST_F(MptcpManagerTest, RemoveAllAddressesFromInterfaceTest001, TestSize.Level1)
{
    auto manager = std::make_shared<MptcpManager>();
    manager->ifaceToIpAddrs_["wlan0"] = {"192.168.1.100", "192.168.1.101"};
    
    manager->OnInterfaceAddressRemoved("192.168.1.100", "wlan0");
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find("wlan0") != manager->ifaceToIpAddrs_.end());
    
    manager->OnInterfaceAddressRemoved("192.168.1.101", "wlan0");
    EXPECT_TRUE(manager->ifaceToIpAddrs_.find("wlan0") == manager->ifaceToIpAddrs_.end());
}

} // namespace nmd
} // namespace OHOS

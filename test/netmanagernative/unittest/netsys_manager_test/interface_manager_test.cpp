/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <gtest/gtest.h>
#ifdef GTEST_API_
#define private public
#define protected public
#endif
#include "interface_manager.h"
#include "netsys_controller.h"
#include "net_manager_constants.h"
namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
using namespace OHOS::NetManagerStandard;
} // namespace

class InterfaceManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void InterfaceManagerTest::SetUpTestCase() {}

void InterfaceManagerTest::TearDownTestCase() {}

void InterfaceManagerTest::SetUp() {}

void InterfaceManagerTest::TearDown() {}

HWTEST_F(InterfaceManagerTest, GetMtuTest001, TestSize.Level1)
{
    auto ret = InterfaceManager::GetMtu(nullptr);
    EXPECT_EQ(ret, -1);

    std::string interfaceName = "IfaceNameIsExtMax16";
    ret = InterfaceManager::GetMtu(interfaceName.data());
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, GetMtuTest002, TestSize.Level1)
{
    std::string interfaceName = "eth0";
    auto ret = InterfaceManager::GetMtu(interfaceName.c_str());
    EXPECT_TRUE(ret == -1 || ret == 1500);
}

HWTEST_F(InterfaceManagerTest, SetMtuTest001, TestSize.Level1)
{
    std::string mtuValue = "10";
    std::string interfaceName;
    auto ret = InterfaceManager::SetMtu(interfaceName.data(), mtuValue.data());
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, SetMtuTest002, TestSize.Level1)
{
    std::string mtuValue = "10";
    std::string interfaceName = "eth0";
    auto ret = InterfaceManager::SetMtu(interfaceName.data(), mtuValue.data());
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, SetMtuTest003, TestSize.Level1)
{
    std::string interfaceName = "eth0";
    auto ifaceList = InterfaceManager::GetInterfaceNames();
    bool eth0NotExist = std::find(ifaceList.begin(), ifaceList.end(), interfaceName) == ifaceList.end();
    ASSERT_NE(eth0NotExist, true);

    char *mtuValue = nullptr;
    auto ret = InterfaceManager::SetMtu(interfaceName.data(), mtuValue);
    EXPECT_EQ(ret, -1);

    const char *cmtu = "";
    ret = InterfaceManager::SetMtu(interfaceName.data(), cmtu);
    EXPECT_EQ(ret, -1);

    std::string mtu = "1500";
    ret = InterfaceManager::SetMtu(interfaceName.data(), mtu.data());
    EXPECT_EQ(ret, 0);

    mtu = "1500000000000000";
    ret = InterfaceManager::SetMtu(interfaceName.data(), mtu.data());
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, SetMtuTest004, TestSize.Level1)
{
    char *interfaceName = nullptr;
    char *mtuValue = nullptr;
    auto ret = InterfaceManager::SetMtu(interfaceName, mtuValue);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, ModifyAddressTest001, TestSize.Level1)
{
    std::string interfaceName = "lo";
    std::string addr = "127.0.0.1";
    auto ret = InterfaceManager::ModifyAddress(0, interfaceName.c_str(), addr.c_str(), 0);
    EXPECT_GE(ret, -1);

    addr = "fe80::af71:b0c7:e3f7:3c0f%5";
    ret = InterfaceManager::ModifyAddress(0, interfaceName.c_str(), addr.c_str(), 0);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, DelAddressTest004, TestSize.Level1)
{
    std::string interfaceName = "eth0";
    std::string addr = "127.0.0.1";
    auto ret = InterfaceManager::DelAddress(interfaceName.c_str(), addr.c_str(), 0, 2);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterfaceManagerTest, GetIfaceConfigTest003, TestSize.Level1)
{
    std::string ifName;
    nmd::InterfaceConfigurationParcel ifaceConfig = InterfaceManager::GetIfaceConfig(ifName);
    EXPECT_EQ(true, ifaceConfig.ifName.empty());
}

HWTEST_F(InterfaceManagerTest, SetIfaceConfigTest002, TestSize.Level1)
{
    nmd::InterfaceConfigurationParcel ifaceConfig;
    ifaceConfig.ifName = "";
    
    ifaceConfig.flags.push_back("flag");
    auto ret = InterfaceManager::SetIfaceConfig(ifaceConfig);
    EXPECT_EQ(ret, -1);

    ifaceConfig.ifName = "lo";
    ret = InterfaceManager::SetIfaceConfig(ifaceConfig);
    EXPECT_EQ(ret, 1);
}

HWTEST_F(InterfaceManagerTest, SetIfaceConfigTest003, TestSize.Level1)
{
    nmd::InterfaceConfigurationParcel ifaceConfig;
    ifaceConfig.ifName = "lo";
    ifaceConfig.flags.push_back("down");
    auto ret = InterfaceManager::SetIfaceConfig(ifaceConfig);
    EXPECT_EQ(ret, 1);
}

HWTEST_F(InterfaceManagerTest, SetIfaceConfigTest004, TestSize.Level1)
{
    nmd::InterfaceConfigurationParcel ifaceConfig;
    ifaceConfig.ifName = "lo";
    ifaceConfig.flags.push_back("up");
    auto ret = InterfaceManager::SetIfaceConfig(ifaceConfig);
    EXPECT_EQ(ret, 1);
}

HWTEST_F(InterfaceManagerTest, SetIpAddressTest003, TestSize.Level1)
{
    std::string ifaceName;
    std::string ipAddress = "127.0.0.1";
    auto ret = InterfaceManager::SetIpAddress(ifaceName, ipAddress);
    EXPECT_EQ(ret, -1);

    ipAddress.clear();
    ret = InterfaceManager::SetIpAddress(ifaceName, ipAddress);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, SetIpAddressTest004, TestSize.Level1)
{
    std::string ifaceName = "lo";
    std::string ipAddress = "127.0.0.1";
    auto ret = InterfaceManager::SetIpAddress(ifaceName, ipAddress);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterfaceManagerTest, SetIffUpTest001, TestSize.Level1)
{
    std::string ifaceName;
    auto ret = InterfaceManager::SetIffUp(ifaceName);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, SetIffUpTest003, TestSize.Level1)
{
    std::string ifaceName = "lo";
    auto ret = InterfaceManager::SetIffUp(ifaceName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterfaceManagerTest, AddStaticArpTest002, TestSize.Level1)
{
    std::string ipAddr;
    std::string macAddr;
    std::string ifName;
    auto ret = InterfaceManager::AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, DelStaticArpTest002, TestSize.Level1)
{
    std::string ipAddr;
    std::string macAddr;
    std::string ifName;
    auto ret = InterfaceManager::DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, AddStaticIpv6AddrTest001, TestSize.Level1)
{
    std::string ipAddr;
    std::string macAddr;
    std::string ifName;
    auto ret = InterfaceManager::AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, DelStaticIpv6AddrTest001, TestSize.Level1)
{
    std::string ipAddr;
    std::string macAddr;
    std::string ifName;
    auto ret = InterfaceManager::DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, AssembleArpTest001, TestSize.Level1)
{
    std::string ipAddr;
    std::string macAddr;
    std::string ifName;
    arpreq req;
    auto ret = InterfaceManager::AssembleArp(ipAddr, macAddr, ifName, req);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ipAddr = "127.0.0.1";
    ret = InterfaceManager::AssembleArp(ipAddr, macAddr, ifName, req);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, AssembleArpTest002, TestSize.Level1)
{
    std::string ipAddr = "127.0.0.1";
    std::string macAddr = "08:00:20:0A:8C:6D";
    std::string ifName;
    arpreq req;
    std::string addr = "08:00:20:0A:8C:6D";
    memcpy_s(req.arp_ha.sa_data, 14, addr.data(), addr.size());
    auto ret = InterfaceManager::AssembleArp(ipAddr, macAddr, ifName, req);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(InterfaceManagerTest, AssembleIPv6NeighborTest001, TestSize.Level1)///这个有问题写的
{
    std::string ipAddr;
    std::string macAddr;
    std::string ifName;
    nmd::NetlinkMsg nlmsg(NLM_F_CREATE | NLM_F_EXCL, nmd::NETLINK_MAX_LEN, 0);
    auto ret = InterfaceManager::AssembleIPv6Neighbor(ipAddr, macAddr, ifName, nlmsg, RTM_NEWNEIGH);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    ret = InterfaceManager::AssembleIPv6Neighbor(ipAddr, macAddr, ifName, nlmsg, RTM_NEWNEIGH);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, AssembleIPv6NeighborTest002, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "08:00:20:0A:8C:6D";
    std::string ifName;
    nmd::NetlinkMsg nlmsg(NLM_F_CREATE | NLM_F_EXCL, nmd::NETLINK_MAX_LEN, 0);
    auto ret = InterfaceManager::AssembleIPv6Neighbor(ipAddr, macAddr, ifName, nlmsg, RTM_NEWNEIGH);

    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(InterfaceManagerTest, MacStringToArrayTest001, TestSize.Level1)
{
    std::string macAddr;
    sockaddr macSock;
    auto ret = InterfaceManager::MacStringToArray(macAddr, macSock);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);

    macAddr = "08:00:20:0A:8C:6D";
    ret = InterfaceManager::MacStringToArray(macAddr, macSock);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(InterfaceManagerTest, MacStringToBinaryTest001, TestSize.Level1)
{
    std::string macAddr;
    uint8_t macBin[MAC_ADDRESS_INT_LEN];
    auto ret = InterfaceManager::MacStringToBinary(macAddr, macBin);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);

    macAddr = "08:00:20:0A:8C:6D";
    ret = InterfaceManager::MacStringToBinary(macAddr, macBin);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    std::string invalidMacStr = "08:00:20:0A:8C";
    ret = InterfaceManager::MacStringToBinary(macAddr, macBin);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(InterfaceManagerTest, AddAddressTest001, TestSize.Level1)
{
    std::string interfaceName = "eth0";
    std::string addr = "";
    int32_t prefixLength = 0;
    auto ifaceList = InterfaceManager::GetInterfaceNames();
    bool eth0NotExist = std::find(ifaceList.begin(), ifaceList.end(), interfaceName) == ifaceList.end();
    ASSERT_NE(eth0NotExist, true);
    auto ret = InterfaceManager::AddAddress(interfaceName.data(), addr.data(), prefixLength);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, AddAddressTest002, TestSize.Level1)
{
    std::string addr = "14.4.1.4";
    int32_t prefixLength = 45;
    auto ret = InterfaceManager::AddAddress(nullptr, addr.data(), prefixLength);
    EXPECT_LE(ret, 0);
}

HWTEST_F(InterfaceManagerTest, AddAddressTest003, TestSize.Level1)
{
    std::string interfaceName = "eth0";
    std::string addr;
    int32_t prefixLength = 45;
    auto ifaceList = InterfaceManager::GetInterfaceNames();
    bool eth0NotExist = std::find(ifaceList.begin(), ifaceList.end(), interfaceName) == ifaceList.end();
    ASSERT_NE(eth0NotExist, true);
    auto ret = InterfaceManager::AddAddress(interfaceName.c_str(), addr.data(), prefixLength);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, AddAddressTest004, TestSize.Level1)
{
    std::string interfaceName = "eth";
    std::string addr;
    int32_t prefixLength = 45;
    auto ret = InterfaceManager::AddAddress(interfaceName.c_str(), addr.data(), prefixLength);
    EXPECT_EQ(ret, -errno);
}

HWTEST_F(InterfaceManagerTest, AddAddressTest005, TestSize.Level1)
{
    std::string interfaceName = "eth";
    int32_t prefixLength = 45;
    auto ret = InterfaceManager::AddAddress(interfaceName.c_str(), nullptr, prefixLength);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, DelAddressTest001, TestSize.Level1)
{
    std::string interfaceName = "eth0";
    std::string addr = "";
    int32_t prefixLength = 0;
    auto ifaceList = InterfaceManager::GetInterfaceNames();
    bool eth0NotExist = std::find(ifaceList.begin(), ifaceList.end(), interfaceName) == ifaceList.end();
    ASSERT_NE(eth0NotExist, true);
    auto ret = InterfaceManager::DelAddress(interfaceName.data(), addr.data(), prefixLength);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, DelAddressTest002, TestSize.Level1)
{
    std::string addr = "14.4.1.4";
    int32_t prefixLength = 45;
    auto ret = InterfaceManager::DelAddress(nullptr, addr.data(), prefixLength);
    EXPECT_LE(ret, 0);
}

HWTEST_F(InterfaceManagerTest, DelAddressTest003, TestSize.Level1)
{
    std::string interfaceName = "eth0";
    int32_t prefixLength = 45;
    std::string addr;
    auto ifaceList = InterfaceManager::GetInterfaceNames();
    bool eth0NotExist = std::find(ifaceList.begin(), ifaceList.end(), interfaceName) == ifaceList.end();
    ASSERT_NE(eth0NotExist, true);
    auto ret = InterfaceManager::DelAddress(interfaceName.data(), addr.data(), prefixLength);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, GetInterfaceNamesTest001, TestSize.Level1)
{
    auto ret = InterfaceManager::GetInterfaceNames();
    EXPECT_FALSE(ret.empty());
}

HWTEST_F(InterfaceManagerTest, GetIfaceConfigTest001, TestSize.Level1)
{
    std::string ifaceName = "";
    auto ret = InterfaceManager::GetIfaceConfig(ifaceName);
    EXPECT_TRUE(ret.ifName.empty());
}

HWTEST_F(InterfaceManagerTest, GetIfaceConfigTest002, TestSize.Level1)
{
    std::string ifaceName = "eth0";
    auto ret = InterfaceManager::GetIfaceConfig(ifaceName);
    EXPECT_FALSE(ret.ifName.empty());
}

HWTEST_F(InterfaceManagerTest, SetIfaceConfigTest001, TestSize.Level1)
{
    nmd::InterfaceConfigurationParcel ifaceConfig;
    ifaceConfig.ifName = "test0";
    int32_t ret = InterfaceManager::SetIfaceConfig(ifaceConfig);
    EXPECT_LE(ret, 0);

    ifaceConfig.flags.push_back("up");
    ret = InterfaceManager::SetIfaceConfig(ifaceConfig);
    EXPECT_LE(ret, 0);

    std::string ifaceName = "eth0";
    auto config = InterfaceManager::GetIfaceConfig(ifaceName);
    EXPECT_FALSE(config.ifName.empty());
    ret = InterfaceManager::SetIfaceConfig(config);
    EXPECT_LE(ret, 1);
}

HWTEST_F(InterfaceManagerTest, SetIpAddressTest002, TestSize.Level1)
{
    std::string errName = "test0";
    std::string ipAddr = "172.17.5.245";
    auto ret = InterfaceManager::SetIpAddress(errName, ipAddr);
    EXPECT_LE(ret, 0);
}

HWTEST_F(InterfaceManagerTest, SetIffUpTest002, TestSize.Level1)
{
    std::string errName = "test0";
    auto ret = InterfaceManager::SetIffUp(errName);
    EXPECT_LE(ret, 0);

    std::string ifaceName = "eth1";
    ret = InterfaceManager::SetIffUp(ifaceName);
    EXPECT_LE(ret, 0);
}

HWTEST_F(InterfaceManagerTest, AssembleArp001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    arpreq req = {};
    auto ret = InterfaceManager::AssembleArp(ipAddr, macAddr, ifName, req);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterfaceManagerTest, AssembleIPv6Neighbor001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    nmd::NetlinkMsg nlmsg(NLM_F_CREATE | NLM_F_EXCL, nmd::NETLINK_MAX_LEN, 0);
    auto ret = InterfaceManager::AssembleIPv6Neighbor(ipAddr, macAddr, ifName, nlmsg, RTM_NEWNEIGH);
    EXPECT_TRUE(ret == 0 || ret == NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(InterfaceManagerTest, AssembleIPv6Neighbor002, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334:1111";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    nmd::NetlinkMsg nlmsg(NLM_F_CREATE | NLM_F_EXCL, nmd::NETLINK_MAX_LEN, 0);
    auto ret = InterfaceManager::AssembleIPv6Neighbor(ipAddr, macAddr, ifName, nlmsg, RTM_NEWNEIGH);
    EXPECT_TRUE(ret == 0 || ret == NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(InterfaceManagerTest, AddStaticArpTest001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    auto ret = InterfaceManager::AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, 0);

    ipAddr = "192.168.1.101";
    macAddr = "aa:bb:cc:dd:00:ff";
    ifName = "wlan0";
    ret = InterfaceManager::AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterfaceManagerTest, DelStaticArpTest001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    auto ret = InterfaceManager::DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, 0);

    ipAddr = "192.168.1.101";
    macAddr = "aa:bb:cc:dd:00:ff";
    ifName = "wlan0";
    ret = InterfaceManager::DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(InterfaceManagerTest, AddStaticIpv6AddrTest002, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    auto ret = InterfaceManager::AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_TRUE(ret == 0 || ret == NETMANAGER_ERR_OPERATION_FAILED);

    ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7335";
    macAddr = "aa:bb:cc:dd:00:ff";
    ifName = "chba0";
    ret = InterfaceManager::AddStaticIpv6Addr(ipAddr, macAddr, ifName);
}

HWTEST_F(InterfaceManagerTest, DelStaticIpv6AddrTest002, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    auto ret = InterfaceManager::DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_TRUE(ret == 0 || ret == NETMANAGER_ERR_OPERATION_FAILED);

    ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7335";
    macAddr = "aa:bb:cc:dd:00:ff";
    ifName = "chba0";
    ret = InterfaceManager::DelStaticIpv6Addr(ipAddr, macAddr, ifName);
}

HWTEST_F(InterfaceManagerTest, GetIfaceConfigTest004, TestSize.Level1)
{
    std::string ifaceName = "12345678901234567890";
    auto ret = InterfaceManager::GetIfaceConfig(ifaceName);
    EXPECT_FALSE(ret.ifName.empty());
}

HWTEST_F(InterfaceManagerTest, SetIfaceConfigTest005, TestSize.Level1)
{
    nmd::InterfaceConfigurationParcel ifaceConfig;
    ifaceConfig.ifName = "";
    auto ret = InterfaceManager::SetIfaceConfig(ifaceConfig);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, SetIpAddressTest005, TestSize.Level1)
{
    std::string ifaceName = "12345678901234567890";
    std::string ipAddress = "127.0.0.1";
    auto ret = InterfaceManager::SetIpAddress(ifaceName, ipAddress);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, SetIffUpTest004, TestSize.Level1)
{
    std::string ifaceName = "12345678901234567890";
    auto ret = InterfaceManager::SetIffUp(ifaceName);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(InterfaceManagerTest, SetIpv6AutoConfTest004, TestSize.Level1)
{
    std::string ifaceName = "wlan";
    uint32_t on = 0;
    auto ret = InterfaceManager::SetIpv6AutoConf(ifaceName, on);
    EXPECT_TRUE(ret == 0 || ret == -1);
}

HWTEST_F(InterfaceManagerTest, GetIpNeighTableTest003, TestSize.Level1)
{
    std::vector<NetIpMacInfo> ipMacInfo;
    int32_t ret = InterfaceManager::GetIpNeighTable(ipMacInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(InterfaceManagerTest, CreateVlan001, TestSize.Level1)
{
    std::string ifName = "rmnet0";
    uint32_t vlanId = 1;
    int32_t ret = InterfaceManager::CreateVlan(ifName, vlanId);
    ifName = "wlan0";
    ret = InterfaceManager::CreateVlan(ifName, vlanId);
    EXPECT_NE(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(InterfaceManagerTest, DestroyVlan001, TestSize.Level1)
{
    std::string ifName = "rmnet0";
    uint32_t vlanId = 1;
    int32_t ret = InterfaceManager::DestroyVlan(ifName, vlanId);
    ifName = "wlan0";
    ret = InterfaceManager::DestroyVlan(ifName, vlanId);
    EXPECT_TRUE(ret == NETMANAGER_ERR_OPERATION_FAILED || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(InterfaceManagerTest, AddVlanIp001, TestSize.Level1)
{
    std::string ifName = "rmnet0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    int32_t ret = InterfaceManager::AddVlanIp(ifName, vlanId, ip, mask);

    ifName = "wlan0";
    ret = InterfaceManager::AddVlanIp(ifName, vlanId, ip, mask);
    ifName = "rmnet0";
    ip = "2001:0db8:0000:0000:0000:0000:0000:0001";
    ret = InterfaceManager::AddVlanIp(ifName, vlanId, ip, mask);
    ip = "192.168.1.1.1";
    ret = InterfaceManager::AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_TRUE(ret == NETMANAGER_ERR_OPERATION_FAILED || ret == NETMANAGER_SUCCESS);
}

} // namespace nmd
} // namespace OHOS

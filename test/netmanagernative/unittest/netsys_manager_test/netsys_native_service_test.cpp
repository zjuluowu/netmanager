/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <string>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "interface_manager.h"
#include "netlink_socket_diag.cpp"
#include "netlink_socket_diag.h"
#include "netsys_controller.h"
#include "system_ability_definition.h"

#include "common_notify_callback_test.h"
#include "dns_config_client.h"
#include "net_stats_constants.h"
#include "netsys_native_service.h"

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace NetManagerStandard;
using namespace testing::ext;
static constexpr uint32_t TEST_UID = 1;
static constexpr uint64_t TEST_COOKIE = 1;
static constexpr uint32_t TEST_STATS_TYPE1 = 0;
#define DTEST_LOG std::cout << __func__ << ":" << __LINE__ << ":"
} // namespace

class NetsysNativeServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline auto instance_ = std::make_shared<NetsysNativeService>(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
};

void NetsysNativeServiceTest::SetUpTestCase() {}

void NetsysNativeServiceTest::TearDownTestCase() {}

void NetsysNativeServiceTest::SetUp() {}

void NetsysNativeServiceTest::TearDown() {}

HWTEST_F(NetsysNativeServiceTest, DumpTest001, TestSize.Level1)
{
    NetsysNativeService service;
    service.state_ = NetsysNativeService::ServiceRunningState::STATE_RUNNING;
    service.OnStart();
    instance_->Init();
    int32_t testFd = 11;
    int32_t ret = instance_->Dump(testFd, {});
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetResolverConfigTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    uint16_t baseTimeoutMsec = 200;
    uint8_t retryCount = 3;
    int32_t ret = instance_->SetResolverConfig(testNetId, baseTimeoutMsec, retryCount, {}, {});
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, GetResolverConfigTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    uint16_t baseTimeoutMsec = 200;
    uint8_t retryCount = 3;
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    int32_t ret = instance_->GetResolverConfig(testNetId, servers, domains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, CreateNetworkCacheTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    int32_t ret = instance_->CreateNetworkCache(testNetId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, DestroyNetworkCacheTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    int32_t ret = instance_->DestroyNetworkCache(testNetId);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetworkAddRouteTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    std::string interfaceName = "eth1";
    std::string destination = "";
    std::string nextHop = "";
    int32_t ret = instance_->NetworkAddRoute(testNetId, interfaceName, destination, nextHop);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetworkAddRouteParcelTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    RouteInfoParcel routeInfo;
    int32_t ret = instance_->NetworkAddRouteParcel(testNetId, routeInfo);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetworkRemoveRouteParcelTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    RouteInfoParcel routeInfo;
    int32_t ret = instance_->NetworkRemoveRouteParcel(testNetId, routeInfo);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetworkSetDefaultTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    int32_t ret = instance_->NetworkSetDefault(testNetId);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetworkGetDefaultTest001, TestSize.Level1)
{
    int32_t ret = instance_->NetworkGetDefault();
    EXPECT_LE(ret, 154);
}

HWTEST_F(NetsysNativeServiceTest, NetworkClearDefaultTest001, TestSize.Level1)
{
    int32_t ret = instance_->NetworkClearDefault();
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, GetProcSysNetTest001, TestSize.Level1)
{
    int32_t ipversion = 45;
    int32_t which = 14;
    std::string ifname = "testifname";
    std::string paramete = "testparamete";
    std::string value = "testvalue";
    int32_t ret = instance_->GetProcSysNet(ipversion, which, ifname, paramete, value);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetProcSysNetTest001, TestSize.Level1)
{
    int32_t ipversion = 45;
    int32_t which = 14;
    std::string ifname = "testifname";
    std::string paramete = "testparamete";
    std::string value = "testvalue";
    int32_t ret = instance_->SetProcSysNet(ipversion, which, ifname, paramete, value);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetInterfaceMtu001, TestSize.Level1)
{
    std::string testName = "test0";
    int32_t mtu = 1500;
    int32_t ret = instance_->SetInterfaceMtu(testName, mtu);
    EXPECT_NE(ret, 0);
    std::string eth0Name = "eth0";
    std::vector<std::string> ifaceList;
    instance_->InterfaceGetList(ifaceList);
    bool eth0NotExist = std::find(ifaceList.begin(), ifaceList.end(), eth0Name) == ifaceList.end();
    ASSERT_NE(eth0NotExist, true);
    ret = instance_->SetInterfaceMtu(eth0Name, mtu);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, SetTcpBufferSizes001, TestSize.Level1)
{
    std::string tcpBufferSizes = "524288,1048576,2097152,262144,524288,1048576";
    int32_t ret = instance_->SetTcpBufferSizes(tcpBufferSizes);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, GetInterfaceMtu001, TestSize.Level1)
{
    std::string testName = "test0";
    int32_t ret = instance_->GetInterfaceMtu(testName);
    EXPECT_NE(ret, 0);
    std::string eth0Name = "eth0";
    ret = instance_->GetInterfaceMtu(eth0Name);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, RegisterNotifyCallback001, TestSize.Level1)
{
    sptr<INotifyCallback> callback = new (std::nothrow) NotifyCallbackTest();
    int32_t ret = instance_->RegisterNotifyCallback(callback);
    EXPECT_EQ(ret, 0);

    ret = instance_->UnRegisterNotifyCallback(callback);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, NetworkRemoveRouteTest001, TestSize.Level1)
{
    uint16_t testNetId = 154;
    std::string interfaceName = "eth1";
    std::string destination = "";
    std::string nextHop = "";
    int32_t ret = instance_->NetworkRemoveRoute(testNetId, interfaceName, destination, nextHop);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, NetworkCreatePhysicalTest001, TestSize.Level1)
{
    int32_t netId = 1000;
    int32_t permission = 0;
    int32_t ret = instance_->NetworkCreatePhysical(netId, permission);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, AddInterfaceAddressTest001, TestSize.Level1)
{
    std::string iFName = "test0";
    std::string addrStr = "192.168.22.33";
    int32_t prefixLength = 24;
    int32_t ret = instance_->AddInterfaceAddress(iFName, addrStr, prefixLength);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, DelInterfaceAddressTest001, TestSize.Level1)
{
    std::string iFName = "test0";
    std::string addrStr = "192.168.22.33";
    int32_t prefixLength = 24;
    int32_t ret = instance_->DelInterfaceAddress(iFName, addrStr, prefixLength);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, InterfaceSetIpAddressTest001, TestSize.Level1)
{
    std::string iFName = "test0";
    std::string addrStr = "192.168.22.33";
    int32_t ret = instance_->InterfaceSetIpAddress(iFName, addrStr);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, InterfaceSetIffUpTest001, TestSize.Level1)
{
    std::string iFName = "test0";
    int32_t ret = instance_->InterfaceSetIffUp(iFName);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetworkAddInterfaceTest001, TestSize.Level1)
{
    int32_t netId = 1000;
    std::string iFName = "test0";
    int32_t ret = instance_->NetworkAddInterface(netId, iFName, BEARER_DEFAULT);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetworkRemoveInterfaceTest001, TestSize.Level1)
{
    int32_t netId = 1000;
    std::string iFName = "test0";
    int32_t ret = instance_->NetworkRemoveInterface(netId, iFName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetworkDestroyTest001, TestSize.Level1)
{
    int32_t netId = 1000;
    int32_t ret = instance_->NetworkDestroy(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, GetFwmarkForNetworkTest001, TestSize.Level1)
{
    int32_t netId = 1000;
    MarkMaskParcel markMaskParcel;
    int32_t ret = instance_->GetFwmarkForNetwork(netId, markMaskParcel);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceTest, SetInterfaceConfigTest001, TestSize.Level1)
{
    InterfaceConfigurationParcel cfg;
    cfg.ifName = "test0";
    cfg.hwAddr = "0b:2c:43:d7:22:1s";
    cfg.ipv4Addr = "192.168.133.12";
    cfg.prefixLength = 24;
    cfg.flags.push_back("up");
    int32_t ret = instance_->SetInterfaceConfig(cfg);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceTest, GetInterfaceConfigTest001, TestSize.Level1)
{
    InterfaceConfigurationParcel cfg;
    cfg.ifName = "eth0";
    int32_t ret = instance_->GetInterfaceConfig(cfg);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceTest, InterfaceGetListTest001, TestSize.Level1)
{
    std::vector<std::string> ifaces;
    int32_t ret = instance_->InterfaceGetList(ifaces);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceTest, StartDhcpClientTest001, TestSize.Level1)
{
    std::string iface = "test0";
    bool bIpv6 = false;
    int32_t ret = instance_->StartDhcpClient(iface, bIpv6);
    EXPECT_EQ(ret, ERR_NONE);

    ret = instance_->StopDhcpClient(iface, bIpv6);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceTest, StartDhcpServiceTest001, TestSize.Level1)
{
    std::string iface = "test0";
    std::string ipv4addr = "192.168.133.12";
    int32_t ret = instance_->StartDhcpService(iface, ipv4addr);
    EXPECT_EQ(ret, ERR_NONE);

    ret = instance_->StopDhcpService(iface);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceTest, IpEnableForwardingTest001, TestSize.Level1)
{
    std::string iface = "";
    std::string eth0 = "eth0";
    int32_t ret = instance_->IpEnableForwarding(iface);
    EXPECT_EQ(ret, 0);
    ret = instance_->IpDisableForwarding(iface);
    EXPECT_EQ(ret, 0);

    ret = instance_->EnableNat(eth0, eth0);
    EXPECT_NE(ret, 0);
    ret = instance_->DisableNat(eth0, eth0);
    EXPECT_NE(ret, 0);

    ret = instance_->IpfwdAddInterfaceForward(eth0, eth0);
    EXPECT_NE(ret, 0);
    ret = instance_->IpfwdRemoveInterfaceForward(eth0, eth0);
    EXPECT_NE(ret, 0);
    instance_->OnNetManagerRestart();
}

HWTEST_F(NetsysNativeServiceTest, NetsysNativeServiceTest001, TestSize.Level1)
{
    std::string fromIface = "";
    std::string toIface = "";

    int32_t ret = instance_->IpfwdRemoveInterfaceForward(fromIface, toIface);
    EXPECT_NE(ret, 0);

    ret = instance_->BandwidthEnableDataSaver(true);
    EXPECT_EQ(ret, 0);

    ret = instance_->BandwidthSetIfaceQuota("testifname", 32);
    EXPECT_EQ(ret, 0);

    ret = instance_->BandwidthRemoveIfaceQuota("testifname");
    EXPECT_EQ(ret, 0);

    uint32_t uid = 1001;
    ret = instance_->BandwidthAddDeniedList(uid);
    EXPECT_EQ(ret, 0);

    ret = instance_->BandwidthRemoveDeniedList(uid);
    EXPECT_EQ(ret, 0);

    ret = instance_->BandwidthAddAllowedList(uid);
    EXPECT_EQ(ret, 0);

    ret = instance_->BandwidthRemoveAllowedList(uid);
    EXPECT_EQ(ret, 0);

    uint32_t chain = 0;
    std::vector<uint32_t> uids = {1001};
    ret = instance_->FirewallSetUidsAllowedListChain(chain, uids);
    EXPECT_NE(ret, 0);

    ret = instance_->FirewallSetUidsDeniedListChain(chain, uids);
    EXPECT_NE(ret, 0);

    ret = instance_->FirewallEnableChain(chain, false);
    EXPECT_NE(ret, 0);

    uint32_t firewallRule = 0;
    ret = instance_->FirewallSetUidRule(chain, {uid}, firewallRule);
    EXPECT_EQ(ret, 0);

    uint16_t netid = 1000;
    ret = instance_->ShareDnsSet(netid);
    EXPECT_EQ(ret, 0);

    ret = instance_->StartDnsProxyListen();
    EXPECT_EQ(ret, 0);

    ret = instance_->StopDnsProxyListen();
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, NetsysNativeServiceTest002, TestSize.Level1)
{
    const std::string downIface = "testdownIface";
    const std::string upIface = "testupIface";
    NetworkSharingTraffic traffic;
    int ret = instance_->GetNetworkSharingTraffic(downIface, upIface, traffic);
    EXPECT_NE(ret, 0);
    NetworkDpaTrafficReport dpaTraffic;
    ret = instance_->SetDpaCellularSharingTraffic(dpaTraffic);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, NetsysNativeServiceState001, TestSize.Level1)
{
    const std::string iface = "wlan0";
    const uint32_t appID = 303030;

    uint64_t stats = 0;
    int ret = instance_->GetTotalStats(stats, 0);
    EXPECT_EQ(ret, 0);

    ret = instance_->GetUidStats(stats, 0, appID);
    EXPECT_NE(ret, 0);

    ret = instance_->GetIfaceStats(stats, 5, iface);
    EXPECT_EQ(ret, NetStatsResultCode::STATS_ERR_READ_BPF_FAIL);

    std::vector<OHOS::NetManagerStandard::NetStatsInfo> statsInfo;
    ret = instance_->GetAllStatsInfo(statsInfo);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeServiceTest, GetAddrInfoTest001, TestSize.Level1)
{
    std::string hostName;
    std::string serverName;
    AddrInfo hints;
    uint16_t netId = 1031;
    std::vector<AddrInfo> res;
    int32_t ret = instance_->GetAddrInfo(hostName, serverName, hints, netId, res);
    DTEST_LOG << ret << std::endl;
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, SetInternetPermissionTest001, TestSize.Level1)
{
    uint32_t uid = 0;
    uint8_t allow = 1;
    uint8_t isBroker = 0;
    int32_t ret = instance_->SetInternetPermission(uid, allow, isBroker);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, ShareDnsSetTest001, TestSize.Level1)
{
    uint16_t netid = 10034;
    auto backup = std::move(instance_->netsysService_);
    instance_->netsysService_ = nullptr;
    auto ret = instance_->ShareDnsSet(netid);
    instance_->netsysService_ = std::move(backup);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, StartDnsProxyListenTest001, TestSize.Level1)
{
    auto backup = std::move(instance_->netsysService_);
    instance_->netsysService_ = nullptr;
    auto ret = instance_->StartDnsProxyListen();
    instance_->netsysService_ = std::move(backup);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, StopDnsProxyListenTest001, TestSize.Level1)
{
    auto backup = std::move(instance_->netsysService_);
    instance_->netsysService_ = nullptr;
    auto ret = instance_->StopDnsProxyListen();
    instance_->netsysService_ = std::move(backup);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, GetNetworkSharingTrafficTest001, TestSize.Level1)
{
    std::string downIface = "dface";
    std::string upIface = "uface";
    NetworkSharingTraffic traffic;
    auto backup = std::move(instance_->sharingManager_);
    instance_->sharingManager_ = nullptr;
    auto ret = instance_->GetNetworkSharingTraffic(downIface, upIface, traffic);
    instance_->sharingManager_ = std::move(backup);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, SetDpaCellularSharingTrafficTest001, TestSize.Level1)
{
    NetworkDpaTrafficReport dpaTraffic;
    auto backup = std::move(instance_->sharingManager_);
    instance_->sharingManager_ = nullptr;
    auto ret = instance_->SetDpaCellularSharingTraffic(dpaTraffic);
    instance_->sharingManager_ = std::move(backup);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, OnAddRemoveSystemAbilityTest001, TestSize.Level1)
{
    instance_->hasSARemoved_ = false;
    instance_->OnAddSystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID, {});
    ASSERT_TRUE(instance_->hasSARemoved_);
    instance_->OnAddSystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID, {});
    ASSERT_TRUE(instance_->hasSARemoved_);
    instance_->hasSARemoved_ = false;
    instance_->OnAddSystemAbility(-1, {});
    ASSERT_FALSE(instance_->hasSARemoved_);
}

HWTEST_F(NetsysNativeServiceTest, GetTotalStatsTest001, TestSize.Level1)
{
    uint64_t stats = 0;
    uint32_t type = 1;
    auto ret = instance_->GetTotalStats(stats, type);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_SUCCESS);
    auto backup = std::move(instance_->bpfStats_);
    ret = instance_->GetTotalStats(stats, type);
    instance_->bpfStats_ = std::move(backup);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, GetUidStatsTest001, TestSize.Level1)
{
    uint64_t stats = 5;
    uint32_t uid = 99;
    uint32_t type = 1;
    auto ret = instance_->GetUidStats(stats, uid, type);
    EXPECT_EQ(stats, 0);
    auto backup = std::move(instance_->bpfStats_);
    ret = instance_->GetUidStats(stats, uid, type);
    instance_->bpfStats_ = std::move(backup);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, GetIfaceStatsTest002, TestSize.Level1)
{
    uint64_t stats = 0;
    uint32_t type = 1;
    const std::string &iface = "eth0";
    auto ret = instance_->GetIfaceStats(stats, type, iface);
    EXPECT_EQ(stats, 0);
    auto backup = std::move(instance_->bpfStats_);
    ret = instance_->GetIfaceStats(stats, type, iface);
    instance_->bpfStats_ = std::move(backup);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, GetAllStatsInfoTest001, TestSize.Level1)
{
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> stats;
    auto ret = instance_->GetAllStatsInfo(stats);
    EXPECT_GE(stats.size(), 0);
    auto backup = std::move(instance_->bpfStats_);
    ret = instance_->GetAllStatsInfo(stats);
    instance_->bpfStats_ = std::move(backup);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, DeleteStatsInfo001, TestSize.Level1)
{
    int32_t ret = instance_->DeleteStatsInfo(TEST_UID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, GetAllSimStatsInfo001, TestSize.Level1)
{
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> stats;
    int32_t ret = instance_->GetAllSimStatsInfo(stats);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, DeleteSimStatsInfo001, TestSize.Level1)
{
    int32_t ret = instance_->DeleteSimStatsInfo(TEST_UID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetIptablesCommandForResTest001, TestSize.Level1)
{
    std::string iptableCmd = "-Sabbbb";
    std::string iptableOutput = "";
    IptablesType ipType = IptablesType::IPTYPE_IPV4;
    auto ret = instance_->SetIptablesCommandForRes(iptableCmd, iptableOutput, ipType);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetIptablesCommandForResTest002, TestSize.Level1)
{
    std::string iptableCmd = "Sabbbb";
    std::string iptableOutput = "";
    IptablesType ipType = IptablesType::IPTYPE_IPV4;
    auto ret = instance_->SetIptablesCommandForRes(iptableCmd, iptableOutput, ipType);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetsysNativeServiceTest, SetIptablesCommandForResTest003, TestSize.Level1)
{
    instance_->manager_ = nullptr;
    instance_->OnNetManagerRestart();
    instance_->netsysService_ = nullptr;
    instance_->OnNetManagerRestart();
    std::string iptableCmd = "-Sabbbb";
    std::string iptableOutput = "";
    IptablesType ipType = IptablesType::IPTYPE_IPV4;
    auto backup = std::move(instance_->iptablesWrapper_);
    auto ret = instance_->SetIptablesCommandForRes(iptableCmd, iptableOutput, ipType);
    instance_->iptablesWrapper_ = std::move(backup);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, SetIpCommandForResTest001, TestSize.Level1)
{
    std::string ipCmd = "/system/bin/ip rule list";
    std::string ipOutput = "";
    auto ret = instance_->SetIpCommandForRes(ipCmd, ipOutput);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, StaticArpTest001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    if (instance_->netsysService_ == nullptr) {
        instance_->Init();
        return;
    }
    int32_t ret = instance_->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, StaticIpv6AddrTest001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    int32_t ret = instance_->AddStaticIpv6Addr(ipAddr, macAddr, ifName);

    ret = instance_->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetsysNativeServiceTest, GetCookieStatsTest001, TestSize.Level1)
{
    uint64_t stats = 0;
    int32_t ret = instance_->GetCookieStats(stats, TEST_STATS_TYPE1, TEST_COOKIE);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, GetCookieStatsTest002, TestSize.Level1)
{
    uint64_t stats = 0;
    auto ret = instance_->GetCookieStats(stats, TEST_STATS_TYPE1, TEST_COOKIE);
    EXPECT_EQ(stats, 0);
    auto backup = std::move(instance_->bpfStats_);
    ret = instance_->GetCookieStats(stats, TEST_STATS_TYPE1, TEST_COOKIE);
    instance_->bpfStats_ = std::move(backup);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, NetsysNativeServiceBranchTest001, TestSize.Level1)
{
    int32_t netId = 0;
    int32_t ret = instance_->NetworkCreateVirtual(netId, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t systemAbilityId = 0;
    std::string deviceId = "";
    instance_->OnRemoveSystemAbility(systemAbilityId, deviceId);
    systemAbilityId = COMM_NET_CONN_MANAGER_SYS_ABILITY_ID;
    instance_->OnRemoveSystemAbility(systemAbilityId, deviceId);

    std::vector<UidRange> uidRanges;
    ret = instance_->NetworkAddUids(netId, uidRanges);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkDelUids(netId, uidRanges);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetDiagPingOption pingOption;
    sptr<INetDiagCallback> callback = nullptr;
    ret = instance_->NetDiagPingHost(pingOption, callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER);

    pingOption.destination_ = "test";
    ret = instance_->NetDiagPingHost(pingOption, callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::list<NetDiagRouteTable> routeTables;
    ret = instance_->NetDiagGetRouteTable(routeTables);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetDiagProtocolType socketType = NetDiagProtocolType::PROTOCOL_TYPE_ALL;
    NetDiagSocketsInfo socketsInfo;
    ret = instance_->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::list<NetDiagIfaceConfig> configs;
    std::string ifaceName = "test";
    ret = instance_->NetDiagGetInterfaceConfig(configs, ifaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetDiagIfaceConfig ifConfig;
    ret = instance_->NetDiagUpdateInterfaceConfig(ifConfig, ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetDiagSetInterfaceActiveState(ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NetsysNativeServiceBranchTest002, TestSize.Level1)
{
    NetDiagPingOption pingOption;
    sptr<INetDiagCallback> callback = nullptr;
    instance_->netDiagWrapper = nullptr;
    int32_t ret = instance_->NetDiagPingHost(pingOption, callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    std::list<NetDiagRouteTable> routeTables;
    ret = instance_->NetDiagGetRouteTable(routeTables);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    NetDiagProtocolType socketType = NetDiagProtocolType::PROTOCOL_TYPE_ALL;
    NetDiagSocketsInfo socketsInfo;
    ret = instance_->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    std::list<NetDiagIfaceConfig> configs;
    std::string ifaceName = "test";
    ret = instance_->NetDiagGetInterfaceConfig(configs, ifaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    NetDiagIfaceConfig ifConfig;
    ret = instance_->NetDiagUpdateInterfaceConfig(ifConfig, ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = instance_->NetDiagSetInterfaceActiveState(ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, NetsysNativeServiceBranchTest003, TestSize.Level1)
{
    sptr<INetDnsResultCallback> dnsResultCallback = nullptr;
    uint32_t timeStep = 0;
    int32_t ret = instance_->RegisterDnsResultCallback(dnsResultCallback, timeStep);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->UnregisterDnsResultCallback(dnsResultCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, GetNetworkSharingTypeTest001, TestSize.Level1)
{
    uint32_t type = 0;
    int32_t ret = instance_->UpdateNetworkSharingType(type, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    std::set<uint32_t> sharingTypeIsOn;
    ret = instance_->GetNetworkSharingType(sharingTypeIsOn);
    EXPECT_EQ(sharingTypeIsOn.size(), 1);

    ret = instance_->UpdateNetworkSharingType(type, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    sharingTypeIsOn.clear();
    ret = instance_->GetNetworkSharingType(sharingTypeIsOn);
    EXPECT_EQ(sharingTypeIsOn.size(), 0);
}

HWTEST_F(NetsysNativeServiceTest, UpdateNetworkSharingTypeTest001, TestSize.Level1)
{
    uint32_t type = 0;
    int32_t ret = instance_->UpdateNetworkSharingType(type, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->UpdateNetworkSharingType(type, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetIpv6PrivacyExtensionsTest001, TestSize.Level1)
{
    uint32_t on = 0;
    std::string interface = "wlan0";
    int32_t ret = instance_->SetIpv6PrivacyExtensions(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = instance_->SetEnableIpv6(interface, on, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetEnableIpv6Test001, TestSize.Level1)
{
    uint32_t on = 0;
    std::string interface = "wlan0";
    int32_t ret = instance_->SetEnableIpv6(interface, on, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = instance_->SetEnableIpv6(interface, on, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetIpv6UidBlackListTest001, TestSize.Level1)
{
    int32_t uid = 0;
    std::vector<int32_t> netIds = {};
    int32_t ret = instance_->SetIpv6UidBlackList(netIds, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetIpv6AutoConfTest001, TestSize.Level1)
{
    auto instance = std::make_shared<NetsysNativeService>(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    uint32_t on = 0;
    std::string interface = "wlan0";
    int32_t ret = instance->SetIpv6AutoConf(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetNetworkAccessPolicyTest001, TestSize.Level1)
{
    uint32_t uid = 0;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    int32_t ret = instance_->SetNetworkAccessPolicy(uid, netAccessPolicy, reconfirmFlag);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, DeleteNetworkAccessPolicyTest001, TestSize.Level1)
{
    uint32_t uid = 0;
    int32_t ret = instance_->DeleteNetworkAccessPolicy(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, NotifyNetBearerTypeChangeTest001, TestSize.Level1)
{
    std::set<NetManagerStandard::NetBearType> bearerTypes;
    bearerTypes.insert(NetManagerStandard::NetBearType::BEARER_CELLULAR);
    int32_t ret = instance_->NotifyNetBearerTypeChange(bearerTypes);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, CreateVnic001, TestSize.Level1)
{
    uint16_t mtu = 1500;
    std::string tunAddr = "192.168.1.100";
    int32_t prefix = 24;
    std::set<int32_t> uids;
    int32_t ret = instance_->CreateVnic(mtu, tunAddr, prefix, uids);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, DestroyVnic001, TestSize.Level1)
{
    int32_t ret = instance_->DestroyVnic();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, CloseSocketsUid001, TestSize.Level1)
{
    std::string ipAddr = "";
    uint32_t uid = 1000;
    int32_t ret = instance_->CloseSocketsUid(ipAddr, uid);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceTest, SetBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    std::unordered_map<uint32_t, uint32_t> params;
    int32_t ret = instance_->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetBrokerUidAccessPolicyMapTest002, TestSize.Level1)
{
    std::unordered_map<uint32_t, uint32_t> params;
    params.emplace(TEST_UID, TEST_UID);
    int32_t ret = instance_->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
HWTEST_F(NetsysNativeServiceTest, DisableWearableDistributedNetForward, TestSize.Level1)
{
    int32_t ret = instance_->EnableWearableDistributedNetForward(8001, 8002);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = instance_->DisableWearableDistributedNetForward();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
#endif

HWTEST_F(NetsysNativeServiceTest, SetNetStatusMapTest001, TestSize.Level1)
{
    int32_t ret = instance_->SetNetStatusMap(0, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetIptablesCommandForResTest004, TestSize.Level1)
{
    std::string iptableCmd = "-Sabbbb";
    std::string iptableOutput = "";
    IptablesType ipType = IptablesType::IPTYPE_IPV6;
    auto ret = instance_->SetIptablesCommandForRes(iptableCmd, iptableOutput, ipType);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetIptablesCommandForResTest005, TestSize.Level1)
{
    std::string iptableCmd = "-Sabbbb";
    std::string iptableOutput = "";
    IptablesType ipType = IptablesType::IPTYPE_IPV4V6;
    auto ret = instance_->SetIptablesCommandForRes(iptableCmd, iptableOutput, ipType);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetIptablesCommandForResTest006, TestSize.Level1)
{
    std::string iptableCmd = "-Sabbbb";
    std::string iptableOutput = "";
    IptablesType ipType = IptablesType::IPTYPE_NONE;
    auto ret = instance_->SetIptablesCommandForRes(iptableCmd, iptableOutput, ipType);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INVALID_PARAMETER);
}

HWTEST_F(NetsysNativeServiceTest, SetNicTrafficAllowedTest001, TestSize.Level1)
{
    const std::vector<std::string> ifaceNames;
    bool allowed = true;
    auto ret = instance_->SetNicTrafficAllowed(ifaceNames, allowed);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetNicTrafficAllowedTest002, TestSize.Level1)
{
    NetsysNativeService temp;
    const std::vector<std::string> ifaceNames;
    bool allowed = true;
    temp.iptablesWrapper_ = nullptr;
    auto ret = temp.SetNicTrafficAllowed(ifaceNames, allowed);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, SetNicTrafficAllowedTest003, TestSize.Level1)
{
    const std::vector<std::string> ifaceNames;
    bool allowed = false;
    auto ret = instance_->SetNicTrafficAllowed(ifaceNames, allowed);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetBrokerUidAccessPolicyMapTest003, TestSize.Level1)
{
    NetsysNativeService temp;
    std::unordered_map<uint32_t, uint32_t> uidMaps = {};
    auto ret = temp.SetBrokerUidAccessPolicyMap(uidMaps);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetDnsCacheTest001, TestSize.Level1)
{
    uint16_t netId = 101;
    std::string testHost = "test";
    AddrInfo info;
    auto ret = instance_->SetDnsCache(netId, testHost, info);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetInternetAccessByIpForWifiShare001, TestSize.Level1)
{
    std::string iptest = "1.1.1.1";
    uint8_t family = 2;
    bool access = true;
    std::string ifname = "test";

    auto ret = instance_->SetInternetAccessByIpForWifiShare(iptest, family, access, ifname);
    EXPECT_LE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, SetDnsCacheTest002, TestSize.Level1)
{
    uint16_t netId = 101;
    std::string testHost = "test";
    AddrInfo info;
    auto backup = std::move(instance_->netsysService_);
    instance_->netsysService_ = nullptr;
    auto ret = instance_->SetDnsCache(netId, testHost, info);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    instance_->netsysService_ = std::move(backup);
}

HWTEST_F(NetsysNativeServiceTest, SetInternetAccessByIpForWifiShare002, TestSize.Level1)
{
    std::string iptest = "1.1.1.1";
    uint8_t family = 2;
    bool access = true;
    std::string ifname = "test";

    auto backup = std::move(instance_->netsysService_);
    instance_->netsysService_ = nullptr;
    auto ret = instance_->SetInternetAccessByIpForWifiShare(iptest, family, access, ifname);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    instance_->netsysService_ = std::move(backup);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
HWTEST_F(NetsysNativeServiceTest, UpdateEnterpriseRouteTest001, TestSize.Level1)
{
    uint32_t uid = 20000138;
    std::string ifname = "wlan0";
    bool add = true;
    auto ret = instance_->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysNativeServiceTest, UpdateEnterpriseRouteTest002, TestSize.Level1)
{
    uint32_t uid = 0;
    std::string ifname = "wlan0";
    bool add = true;
    auto ret = instance_->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysNativeServiceTest, UpdateEnterpriseRouteTest003, TestSize.Level1)
{
    uint32_t uid = 20000138;
    std::string ifname = "notexist";
    bool add = true;
    auto ret = instance_->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_PARAMETER_ERROR);
}
#endif

HWTEST_F(NetsysNativeServiceTest, GetIpNeighTableTest001, TestSize.Level1)
{
    std::vector<NetIpMacInfo> ipMacInfo;
    int32_t ret = instance_->GetIpNeighTable(ipMacInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, CreateVlanTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = instance_->CreateVlan(ifName, vlanId);
    EXPECT_NE(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetsysNativeServiceTest, DestroyVlanTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = instance_->DestroyVlan(ifName, vlanId);
    EXPECT_TRUE(ret == NETMANAGER_ERR_OPERATION_FAILED || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, AddVlanIpTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    int32_t ret = instance_->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_TRUE(ret == NETMANAGER_ERR_OPERATION_FAILED || ret == NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, GetConnectOwnerUidTest001, TestSize.Level1)
{
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    int32_t ret = instance_->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, GetSystemNetPortStatesTest001, TestSize.Level1)
{
    NetPortStatesInfo netPortStatesInfo;
    int32_t ret = instance_->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    instance_->netsysService_  = nullptr;
    ret = instance_->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, GetUdpNetPortStatesInfo001, TestSize.Level1)
{
    NetPortStatesInfo netPortStatesInfo;
    inet_diag_msg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.idiag_family = AF_INET;
    NetLinkSocketDiag diag;
    bool result = diag.GetUdpNetPortStatesInfo(&msg, netPortStatesInfo, 0);
    
    EXPECT_EQ(result, true);
}

HWTEST_F(NetsysNativeServiceTest, GetTcpNetPortStatesInfo001, TestSize.Level1)
{
    NetPortStatesInfo netPortStatesInfo;
    inet_diag_msg msg;
    memset_s(&msg, sizeof(msg), 0, sizeof(msg));
    msg.idiag_family = AF_INET;
    NetLinkSocketDiag diag;
    bool result = diag.GetTcpNetPortStatesInfo(&msg, netPortStatesInfo, 0);
    
    EXPECT_EQ(result, true);
}

HWTEST_F(NetsysNativeServiceTest, IsMatchNetwork001, TestSize.Level1)
{
    NetLinkSocketDiag netLinkSocketDiag;
    inet_diag_msg msg;
    msg.idiag_family = AF_INET6;
    msg.id.idiag_src[0] = 0;
    msg.id.idiag_src[1] = 0;
    msg.id.idiag_src[2] = htonl(0xffff);
    msg.id.idiag_src[3] = inet_addr("192.168.1.10");
    EXPECT_TRUE(netLinkSocketDiag.IsMatchNetwork(&msg, "192.168.1.10"));
}

HWTEST_F(NetsysNativeServiceTest, StopClat001, TestSize.Level1)
{
    std::string ifname = "testifname";
    auto ret = instance_->StopClat(ifname);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
} // namespace NetsysNative
} // namespace OHOS

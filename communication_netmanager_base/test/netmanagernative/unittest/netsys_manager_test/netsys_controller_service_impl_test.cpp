/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#include <cstring>
#include <gtest/gtest.h>
#include <iostream>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_conn_client.h"
#include "net_conn_constants.h"
#include "net_manager_constants.h"
#include "net_stats_constants.h"
#include "netsys_controller.h"
#include "netsys_controller_service_impl.h"
#include "common_mock_netsys_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing;
using namespace testing::ext;
} // namespace
static constexpr uint32_t TEST_UID = 1;
static constexpr uint64_t TEST_COOKIE = 1;
static constexpr uint32_t TEST_STATS_TYPE1 = 0;
static constexpr uint32_t TEST_STATS_TYPE2 = 2;

class NetsysControllerServiceImplTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();

    static inline std::shared_ptr<NetsysControllerServiceImpl> instance_ = nullptr;
    sptr<NetsysNative::MockINetsysService> mockNetsysService_ = sptr<NetsysNative::MockINetsysService>::MakeSptr();

private:
    void AddExtMockApi();
};

void NetsysControllerServiceImplTest::SetUpTestCase()
{
    instance_ = std::make_shared<NetsysControllerServiceImpl>();
    if (instance_) {
        instance_->mockNetsysClient_.mockApi_.clear();
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_NETWORKCREATEPHYSICAL_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_NETWORKDESTROY_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_NETWORKADDINTERFACE_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_NETWORKREMOVEINTERFACE_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_NETWORKADDROUTE_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_NETWORKREMOVEROUTE_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_SETINTERFACEDOWN_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_SETINTERFACEUP_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_INTERFACEGETMTU_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_INTERFACESETMTU_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_INTERFACEADDADDRESS_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_INTERFACEDELADDRESS_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_SETRESOLVERCONFIG_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETRESOLVERICONFIG_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_CREATENETWORKCACHE_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_SETDEFAULTNETWORK_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_CLEARDEFAULTNETWORK_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_STARTDHCPCLIENT_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_STOPDHCPCLIENT_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_REGISTERNOTIFYCALLBACK_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_STARTDHCPSERVICE_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_STOPDHCPSERVICE_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_CLOSESOCKETSUID_API);
    }
}

void NetsysControllerServiceImplTest::AddExtMockApi()
{
    if (instance_) {
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETCELLULARRXBYTES_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETCELLULARTXBYTES_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETALLRXBYTES_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETALLTXBYTES_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETUIDRXBYTES_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETUIDTXBYTES_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETIFACERXBYTES_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETIFACETXBYTES_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_INTERFACEGETLIST_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_UIDGETLIST_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETIFACERXPACKETS_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_GETIFACETXPACKETS_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_REGISTERNETSYSNOTIFYCALLBACK_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_BINDNETWORKSERVICEVPN_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_ENABLEVIRTUALNETIFACECARD_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_SETIPADDRESS_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_SETBLOCKING_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_SHAREDNSSET_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_INTERFACECLEARADDRS_API);
        instance_->mockNetsysClient_.mockApi_.insert(MOCK_FLUSHDNSCACHE_API);
    }
}

void NetsysControllerServiceImplTest::TearDownTestCase() {}

void NetsysControllerServiceImplTest::SetUp() {}

void NetsysControllerServiceImplTest::TearDown() {}

HWTEST_F(NetsysControllerServiceImplTest, NoRegisterMockApi, TestSize.Level1)
{
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    std::string testName = "eth0";
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    sptr<NetsysControllerCallback> callback = nullptr;

    auto ret = instance_->NetworkCreatePhysical(0, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkDestroy(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkAddInterface(0, testName, BEARER_DEFAULT);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkRemoveInterface(0, testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkAddRoute(0, testName, testName, testName);
    EXPECT_EQ(ret, -1);

    ret = instance_->NetworkRemoveRoute(0, testName, testName, testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetInterfaceDown(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetInterfaceUp(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetInterfaceMtu(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetInterfaceMtu(testName, 1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->AddInterfaceAddress(testName, testName, 1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->DelInterfaceAddress(testName, testName, 1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetResolverConfig(0, 0, 0, servers, domains);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetResolverConfig(0, servers, domains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->RegisterCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, RunRegisterMockApi, TestSize.Level1)
{
    std::string testName = "wlan0";

    auto ret = instance_->GetCellularRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    instance_->ClearInterfaceAddrs(testName);
    ret = instance_->GetCellularTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetAllRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetAllTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetUidRxBytes(20010038);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetUidTxBytes(20010038);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetIfaceRxBytes(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetIfaceTxBytes(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetIfaceRxPackets(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetIfaceTxPackets(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->CreateNetworkCache(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetDefaultNetWork(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->ClearDefaultNetWorkNetId();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->StartDhcpClient(testName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->StopDhcpClient(testName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->StartDhcpService(testName, testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->StopDhcpService(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    auto list = instance_->InterfaceGetList();
    EXPECT_GT(list.size(), static_cast<uint32_t>(0));

    list = instance_->UidGetList();
    EXPECT_EQ(list.size(), static_cast<uint32_t>(0));
}

HWTEST_F(NetsysControllerServiceImplTest, ServiceImplTest, TestSize.Level1)
{
    std::vector<UidRange> uidRanges;
    UidRange uidRang(1, 2, 0, 0);
    uidRanges.emplace_back(uidRang);
    int32_t ifaceFd = 5;
    std::string ipAddr = "172.17.5.245";
    NetsysNotifyCallback Callback;
    Callback.NetsysResponseInterfaceAdd = nullptr;
    Callback.NetsysResponseInterfaceRemoved = nullptr;

    auto ret = instance_->NetworkCreateVirtual(5, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkAddUids(5, uidRanges);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkDelUids(5, uidRanges);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    auto ret32 = instance_->RegisterNetsysNotifyCallback(Callback);
    EXPECT_EQ(ret32, NetManagerStandard::NETMANAGER_SUCCESS);

    ret32 = instance_->BindNetworkServiceVpn(5);
    EXPECT_TRUE(ret32 == NetsysContrlResultCode::NETSYS_ERR_VPN || ret == NetManagerStandard::NETMANAGER_SUCCESS);

    ifreq ifRequest;
    ret32 = instance_->EnableVirtualNetIfaceCard(5, ifRequest, ifaceFd);
    EXPECT_EQ(ret32, NetsysContrlResultCode::NETSYS_ERR_VPN);

    ret32 = instance_->SetIpAddress(5, ipAddr, 23, ifRequest);
    EXPECT_EQ(ret32, NetsysContrlResultCode::NETSYS_ERR_VPN);

    ret32 = instance_->SetBlocking(5, false);
    EXPECT_EQ(ret32, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, SetInternetPermission, TestSize.Level1)
{
    uint32_t uid = 0;
    uint8_t allow = 0;

    auto ret = instance_->SetInternetPermission(uid, allow);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);

    std::string tcpBufferSizes = "524288,1048576,2097152,262144,524288,1048576";
    ret = instance_->SetTcpBufferSizes(tcpBufferSizes);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, ServiceImplTest002, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    auto ret = instance_->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, AddStaticIpv6AddrTest001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    auto ret = instance_->AddStaticIpv6Addr(ipAddr, macAddr, ifName);

    ret = instance_->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, DelStaticIpv6AddrTest001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    auto ret = instance_->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    ret = instance_->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest001, TestSize.Level1)
{
    instance_->mockNetsysClient_.mockApi_.clear();
    int32_t netId = 0;
    int32_t permission = 0;
    auto ret = instance_->NetworkCreatePhysical(netId, permission);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkDestroy(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string testName = "eth0";
    ret = instance_->NetworkRemoveInterface(netId, testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    OHOS::nmd::InterfaceConfigurationParcel cfg = {};
    ret = instance_->GetInterfaceConfig(cfg);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetInterfaceConfig(cfg);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetInterfaceDown(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    std::vector<std::string> servers = {};
    std::vector<std::string> domains = {};
    ret = instance_->SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->CreateNetworkCache(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->DestroyNetworkCache(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetCellularRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetInterfaceUp(testName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string ifName = "";
    instance_->ClearInterfaceAddrs(ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetCellularTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetAllRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetAllTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest002, TestSize.Level1)
{
    instance_->mockNetsysClient_.mockApi_.clear();
    int32_t netId = 0;
    std::string interfaceName = "";
    auto ret = instance_->GetIfaceRxBytes(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetIfaceTxBytes(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<std::string> ifList = {};
    EXPECT_TRUE(instance_->InterfaceGetList() != ifList);
    EXPECT_FALSE(instance_->UidGetList() != ifList);

    ret = instance_->GetIfaceRxPackets(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetIfaceTxPackets(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetDefaultNetWork(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->ClearDefaultNetWorkNetId();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string testString = "";
    ret = instance_->IpEnableForwarding(testString);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->IpDisableForwarding(testString);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->ShareDnsSet(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->StartDnsProxyListen();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->StopDnsProxyListen();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetsysNotifyCallback callback = {};
    ret = instance_->RegisterNetsysNotifyCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    uint32_t uid = 0;
    ret = instance_->GetUidRxBytes(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetUidTxBytes(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetUidOnIfaceRxBytes(uid, interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetUidOnIfaceTxBytes(uid, interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest003, TestSize.Level1)
{
    instance_->mockNetsysClient_.mockApi_.clear();

    int32_t mtu = 0;
    std::string testName = "eth0";
    auto ret = instance_->SetInterfaceMtu(testName, mtu);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t prefixLength = 0;
    ret = instance_->AddInterfaceAddress(testName, "", prefixLength);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetInterfaceMtu(testName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->DelInterfaceAddress(testName, "", prefixLength);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->InterfaceSetIpAddress(testName, "");
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    uint16_t netId = 0;
    std::string hostName = "";
    std::string serverName = "";
    AddrInfo hints = {};
    std::vector<AddrInfo> res = {};
    ret = instance_->GetAddrInfo(hostName, serverName, hints, netId, res);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    nmd::NetworkSharingTraffic traffic = {};
    ret = instance_->GetNetworkSharingTraffic(hostName, serverName, traffic);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    nmd::NetworkDpaTrafficReport dpaTraffic = {};
    ret = instance_->SetDpaCellularSharingTraffic(dpaTraffic);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkAddInterface(netId, testName, BEARER_DEFAULT);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkAddRoute(netId, testName, "", "");
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetworkRemoveRoute(netId, testName, "", "");
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string testString = "";
    ret = instance_->EnableNat(testString, testString);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->DisableNat(testString, testString);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->IpfwdAddInterfaceForward(testString, testString);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->IpfwdRemoveInterfaceForward(testString, testString);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t socketFd = 0;
    ret = instance_->BindNetworkServiceVpn(socketFd);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest004, TestSize.Level1)
{
    instance_->mockNetsysClient_.mockApi_.clear();
    struct ifreq ifRequest = {};
    int32_t ifaceFd = 0;
    int32_t socketFd = 0;
    auto ret = instance_->EnableVirtualNetIfaceCard(socketFd, ifRequest, ifaceFd);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t prefixLen = 0;
    ret = instance_->SetIpAddress(socketFd, "", prefixLen, ifRequest);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->BandwidthEnableDataSaver(false);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int64_t bytes = 0;
    std::string ifName = "";
    ret = instance_->BandwidthSetIfaceQuota(ifName, bytes);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->BandwidthRemoveIfaceQuota(ifName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->RegisterCallback(nullptr);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<uint32_t> uidsParam = {};
    uint32_t chain = 0;
    ret = instance_->FirewallSetUidsAllowedListChain(chain, uidsParam);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->FirewallSetUidsDeniedListChain(chain, uidsParam);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->FirewallEnableChain(chain, false);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string cmd = "";
    std::string respond = "";
    NetsysNative::IptablesType ipType = NetsysNative::IptablesType::IPTYPE_IPV4;
    ret = instance_->SetIptablesCommandForRes(cmd, respond, ipType);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->SetIpCommandForRes(cmd, respond);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    OHOS::NetsysNative::NetDiagPingOption pingOption;
    ret = instance_->NetDiagPingHost(pingOption, nullptr);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string interfaceName = "";
    uint64_t stats = 0;
    uint32_t type = 0;
    ret = instance_->GetIfaceStats(stats, type, interfaceName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest005, TestSize.Level1)
{
    instance_->mockNetsysClient_.mockApi_.clear();
    uint32_t firewallRule = 0;
    uint32_t chain = 0;
    std::vector<uint32_t> uidsParam = {};
    int32_t ret = instance_->FirewallSetUidRule(chain, uidsParam, firewallRule);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->ClearFirewallAllRules();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t socketFd = 0;
    ret = instance_->SetBlocking(socketFd, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string iface = "";
    ret = instance_->StartDhcpClient(iface, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->StopDhcpClient(iface, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    uint32_t uid = 0;
    ret = instance_->BandwidthAddDeniedList(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->BandwidthRemoveDeniedList(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->BandwidthAddAllowedList(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->BandwidthRemoveAllowedList(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string ipv4addr = "";
    ret = instance_->StartDhcpService(iface, ipv4addr);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->StopDhcpService(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    uint64_t stats = 0;
    uint32_t type = 0;
    ret = instance_->GetTotalStats(stats, type);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->GetUidStats(stats, type, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->DeleteStatsInfo(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->DeleteSimStatsInfo(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<OHOS::NetManagerStandard::NetStatsInfo> statsInfo = {};
    ret = instance_->GetAllStatsInfo(statsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::list<OHOS::NetsysNative::NetDiagRouteTable> routeTables;
    ret = instance_->NetDiagGetRouteTable(routeTables);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, GetAllSimStatsInfo001, TestSize.Level1)
{
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> statsInfo = {};
    auto ret = instance_->GetAllSimStatsInfo(statsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest006, TestSize.Level1)
{
    instance_->mockNetsysClient_.mockApi_.clear();
    OHOS::NetsysNative::NetDiagProtocolType socketType = OHOS::NetsysNative::NetDiagProtocolType::PROTOCOL_TYPE_ALL;
    OHOS::NetsysNative::NetDiagSocketsInfo socketsInfo;
    int32_t ret = instance_->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::list<OHOS::NetsysNative::NetDiagIfaceConfig> configsList;
    std::string ifaceName = "";
    ret = instance_->NetDiagGetInterfaceConfig(configsList, ifaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    OHOS::NetsysNative::NetDiagIfaceConfig config;
    ret = instance_->NetDiagUpdateInterfaceConfig(config, ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->NetDiagSetInterfaceActiveState(ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest007, TestSize.Level1)
{
    AddExtMockApi();
    auto result = instance_->GetCellularRxBytes();
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string testName = "wlan0";
    instance_->ClearInterfaceAddrs(testName);
    result = instance_->GetCellularTxBytes();
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string interfaceName = "";
    result = instance_->GetIfaceTxPackets(interfaceName);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t netId = 0;
    result = instance_->ShareDnsSet(netId);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    result = instance_->GetIfaceRxBytes(interfaceName);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    result = instance_->GetIfaceTxBytes(interfaceName);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    result = instance_->GetIfaceRxPackets(interfaceName);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    uint32_t uid = 0;
    result = instance_->GetUidOnIfaceRxBytes(uid, interfaceName);
    EXPECT_NE(result, NetManagerStandard::NETMANAGER_SUCCESS);

    result = instance_->GetUidOnIfaceTxBytes(uid, interfaceName);
    EXPECT_NE(result, NetManagerStandard::NETMANAGER_SUCCESS);

    result = instance_->GetUidRxBytes(uid);
    EXPECT_NE(result, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<std::string> ifList = {};
    EXPECT_FALSE(instance_->UidGetList() != ifList);

    result = instance_->GetUidTxBytes(uid);
    EXPECT_NE(result, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest008, TestSize.Level1)
{
    AddExtMockApi();
    NetsysNotifyCallback callback = {};
    int32_t result = instance_->RegisterNetsysNotifyCallback(callback);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t socketFd = 0;
    result = instance_->BindNetworkServiceVpn(socketFd);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t ifaceFd = 0;
    struct ifreq ifRequest = {};
    result = instance_->EnableVirtualNetIfaceCard(socketFd, ifRequest, ifaceFd);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t prefixLen = 0;
    result = instance_->SetIpAddress(socketFd, "", prefixLen, ifRequest);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    result = instance_->SetBlocking(socketFd, false);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);

    uint16_t netId = 0;
    result = instance_->FlushDnsCache(netId);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, GetCookieStatsTest001, TestSize.Level1)
{
    EXPECT_CALL(*mockNetsysService_, GetCookieStats(_, _, _)).WillRepeatedly(Return(0));
    auto netsysControllerServiceImpl = std::make_shared<NetsysControllerServiceImpl>();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;

    uint64_t stats = 0;
    auto ret = netsysControllerServiceImpl->GetCookieStats(stats, TEST_STATS_TYPE1, TEST_COOKIE);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysControllerServiceImpl->GetCookieStats(stats, TEST_STATS_TYPE2, TEST_COOKIE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, GetNetworkSharingTypeTest001, TestSize.Level1)
{
    std::set<uint32_t> sharingTypeIsOn;
    auto ret = instance_->GetNetworkSharingType(sharingTypeIsOn);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, UpdateNetworkSharingTypeTest001, TestSize.Level1)
{
    uint64_t type = 0;
    bool isOpen = true;
    auto ret = instance_->UpdateNetworkSharingType(type, isOpen);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NetsysControllerServiceImplBranchTest009, TestSize.Level1)
{
    uint32_t timeStep = 0;
    sptr<OHOS::NetManagerStandard::NetsysDnsReportCallback> reportCallback = nullptr;
    auto ret = instance_->RegisterDnsResultCallback(reportCallback, timeStep);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = instance_->UnregisterDnsResultCallback(reportCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<OHOS::NetManagerStandard::NetsysDnsQueryReportCallback> queryCallback = nullptr;
    ret = instance_->RegisterDnsQueryResultCallback(queryCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = instance_->UnregisterDnsQueryResultCallback(queryCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysControllerServiceImplTest, SetIpv6PrivacyExtensionsTest001, TestSize.Level1)
{
    EXPECT_CALL(*mockNetsysService_, SetIpv6PrivacyExtensions(_, _)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mockNetsysService_, SetEnableIpv6(_, _, _)).WillRepeatedly(Return(0));
    auto netsysControllerServiceImpl = std::make_shared<NetsysControllerServiceImpl>();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;

    std::string interface = "wlan0";
    uint32_t on = 1;
    int32_t ret = netsysControllerServiceImpl->SetIpv6PrivacyExtensions(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysControllerServiceImpl->SetEnableIpv6(interface, on, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    netsysControllerServiceImpl->SetEnableIpv6(interface, 0, false);
    ret = netsysControllerServiceImpl->SetEnableIpv6(interface, 0, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, SetIpv6UidBlackListTest001, TestSize.Level1)
{
    EXPECT_CALL(*mockNetsysService_, SetIpv6UidBlackList(_, _)).WillRepeatedly(Return(0));
    auto netsysControllerServiceImpl = std::make_shared<NetsysControllerServiceImpl>();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
 
    std::vector<int32_t> netIds = {100};
    int32_t uid = 0;
    int32_t ret = netsysControllerServiceImpl->SetIpv6UidBlackList(netIds, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, SetIpv6AutoConfTest001, TestSize.Level1)
{
    EXPECT_CALL(*mockNetsysService_, SetIpv6AutoConf(_, _)).WillRepeatedly(Return(0));
    auto netsysControllerServiceImpl = std::make_shared<NetsysControllerServiceImpl>();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;

    std::string interface = "wlan0";
    uint32_t on = 1;
    int32_t ret = netsysControllerServiceImpl->SetIpv6AutoConf(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, SetNetworkAccessPolicy001, TestSize.Level1)
{
    uint32_t uid = 0;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    auto ret = instance_->SetNetworkAccessPolicy(uid, netAccessPolicy, reconfirmFlag);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, NotifyNetBearerTypeChange001, TestSize.Level1)
{
    std::set<NetManagerStandard::NetBearType> bearTypes;
    bearTypes.insert(NetManagerStandard::NetBearType::BEARER_CELLULAR);
    auto ret = instance_->NotifyNetBearerTypeChange(bearTypes);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, DeleteNetworkAccessPolicy001, TestSize.Level1)
{
    uint32_t uid = 0;
    auto ret = instance_->DeleteNetworkAccessPolicy(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, CloseSocketsUid001, TestSize.Level1)
{
    std::string ipAddr = "";
    uint32_t uid = 1000;
    int32_t result = instance_->CloseSocketsUid(ipAddr, uid);
    EXPECT_EQ(result, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, SetBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    std::unordered_map<uint32_t, uint32_t> params;
    int32_t ret = instance_->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, SetBrokerUidAccessPolicyMapTest002, TestSize.Level1)
{
    std::unordered_map<uint32_t, uint32_t> params;
    params.emplace(TEST_UID, TEST_UID);
    int32_t ret = instance_->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, DelBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    int32_t ret = instance_->DelBrokerUidAccessPolicyMap(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, DeleteIncreaseTrafficMapTest002, TestSize.Level1)
{
    EXPECT_CALL(*mockNetsysService_, DeleteIncreaseTrafficMap(_)).WillRepeatedly(Return(0));
    auto netsysControllerServiceImpl = std::make_shared<NetsysControllerServiceImpl>();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;

    SafeMap<std::string, std::string> ifaceNameIdentMap_;
    NetConnClient::GetInstance().GetIfaceNameIdentMaps(NetBearType::BEARER_CELLULAR, ifaceNameIdentMap_);
    int32_t ret = netsysControllerServiceImpl->DeleteIncreaseTrafficMap(12);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, ClearSimStatsBpfMap001, TestSize.Level1)
{
    auto netsysControllerServiceImpl = std::make_shared<NetsysControllerServiceImpl>();
    int32_t ret = netsysControllerServiceImpl->ClearSimStatsBpfMap();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
HWTEST_F(NetsysControllerServiceImplTest, DisableWearableDistributedNetForward, TestSize.Level1)
{
    auto ret = instance_->EnableWearableDistributedNetForward(8001, 8002);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = instance_->DisableWearableDistributedNetForward();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
#endif

HWTEST_F(NetsysControllerServiceImplTest, DelInterfaceAddressTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    std::string ipAddr = "";
    int32_t prefixLength = 123;
    int32_t ret = instance_->DelInterfaceAddress(ifName, ipAddr, prefixLength, 2);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, SetNetStateTrafficMapTest001, TestSize.Level1)
{
    uint8_t flag = 1;
    uint64_t availableTraffic = 1;
    int32_t ret = instance_->SetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_TRUE(ret == 300 || ret == 0);
}

HWTEST_F(NetsysControllerServiceImplTest, GetNetStateTrafficMapTest001, TestSize.Level1)
{
    uint8_t flag = 1;
    uint64_t availableTraffic = 1;
    int32_t ret = instance_->GetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_TRUE(ret == 3 || ret == 0);
}

HWTEST_F(NetsysControllerServiceImplTest, ClearIncreaseTrafficMapTest001, TestSize.Level1)
{
    int32_t ret = instance_->ClearIncreaseTrafficMap();
    EXPECT_TRUE(ret == 3 || ret == 0);
}

HWTEST_F(NetsysControllerServiceImplTest, UpdateIfIndexMapTest001, TestSize.Level1)
{
    int8_t key = 1;
    uint64_t index = 1;
    int32_t ret = instance_->UpdateIfIndexMap(key, index);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, RegisterNetsysTrafficCallbackTest001, TestSize.Level1)
{
    sptr<NetsysNative::INetsysTrafficCallback> callback;
    int32_t ret = instance_->RegisterNetsysTrafficCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysControllerServiceImplTest, UnRegisterNetsysTrafficCallbackTest001, TestSize.Level1)
{
    sptr<NetsysNative::INetsysTrafficCallback> callback;
    int32_t ret = instance_->UnRegisterNetsysTrafficCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysControllerServiceImplTest, SetDnsCacheTest001, TestSize.Level1)
{
    uint16_t netId = 101;
    std::string testHost = "test";
    AddrInfo info;
    int32_t ret = instance_->SetDnsCache(netId, testHost, info);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

HWTEST_F(NetsysControllerServiceImplTest, FlushDnsCache001, TestSize.Level1)
{
    uint16_t netId = 101;
    int32_t ret = instance_->FlushDnsCache(netId);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

HWTEST_F(NetsysControllerServiceImplTest, CreateVnic001, TestSize.Level1)
{
    EXPECT_CALL(*mockNetsysService_, CreateVnic(_, _, _, _)).WillRepeatedly(Return(0));
    auto netsysControllerServiceImpl = std::make_shared<NetsysControllerServiceImpl>();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    std::string tunAddr;
    std::set<int32_t> uids;
    int32_t ret = netsysControllerServiceImpl->CreateVnic(1400, tunAddr, 0, uids);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, GetIpNeighTableTest001, TestSize.Level1)
{
    std::vector<NetIpMacInfo> ipMacInfo;
    int32_t ret = instance_->GetIpNeighTable(ipMacInfo);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

HWTEST_F(NetsysControllerServiceImplTest, CreateVlanTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = instance_->CreateVlan(ifName, vlanId);
    EXPECT_FALSE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

HWTEST_F(NetsysControllerServiceImplTest, DestroyVlanTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = instance_->DestroyVlan(ifName, vlanId);
    EXPECT_FALSE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

HWTEST_F(NetsysControllerServiceImplTest, AddVlanIpTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    int32_t ret = instance_->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_FALSE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

HWTEST_F(NetsysControllerServiceImplTest, GetConnectOwnerUidTest001, TestSize.Level1)
{
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    auto ret = instance_->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerServiceImplTest, GetSystemNetPortStatesTest001, TestSize.Level1)
{
    NetPortStatesInfo netPortStatesInfo;
    int32_t ret = instance_->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}
} // namespace NetManagerStandard
} // namespace OHOS

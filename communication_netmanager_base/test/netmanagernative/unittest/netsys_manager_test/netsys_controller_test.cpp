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
#include <cstring>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "netmanager_base_test_security.h"

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "bpf_def.h"
#include "bpf_mapper.h"
#include "bpf_path.h"
#include "common_net_diag_callback_test.h"
#include "common_netsys_controller_callback_test.h"
#include "netmanager_base_common_utils.h"
#include "net_conn_constants.h"
#include "net_diag_callback_stub.h"
#include "netnative_log_wrapper.h"
#include "netsys_controller.h"
#include "netsys_ipc_interface_code.h"
#include "netsys_net_diag_data.h"
#include "netsys_controller_service_impl.h"
#include "common_mock_netsys_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace CommonUtils {
std::string ToAnonymousIp(const std::string &input, bool maskMiddle)
{
    return input;
}
}
namespace {
using namespace testing::ext;
static constexpr const char *IFACE = "test0";
static constexpr const char *WLAN = "wlan0";
static constexpr const char *ETH0 = "eth0";
static constexpr const char *DESTINATION = "192.168.1.3/24";
static constexpr const char *NEXT_HOP = "192.168.1.1";
static constexpr const char *PARCEL_IPV4_ADDR = "192.168.55.121";
static constexpr const char *IP_ADDR = "172.17.5.245";
static constexpr const char *INTERFACE_NAME = "";
static constexpr const char *IF_NAME = "iface0";
static constexpr const char *TCP_BUFFER_SIZES = "524288,1048576,2097152,262144,524288,1048576";
static constexpr uint64_t TEST_COOKIE = 1;
static constexpr uint32_t TEST_STATS_TYPE1 = 0;
static constexpr uint32_t TEST_STATS_TYPE2 = 2;
static constexpr uint32_t IPC_ERR_FLATTEN_OBJECT = 3;
const int NET_ID = 2;
const int PERMISSION = 5;
const int PREFIX_LENGTH = 23;
const int TEST_MTU = 111;
uint16_t g_baseTimeoutMsec = 200;
uint8_t g_retryCount = 3;
const int32_t TEST_UID_32 = 1;
const int64_t TEST_UID = 1010;
const int32_t SOCKET_FD = 5;
const int32_t TEST_STATS_UID = 11111;
int g_ifaceFd = 5;
const int64_t BYTES = 2097152;
const uint32_t FIREWALL_RULE = 1;
bool g_isWaitAsync = false;
const int32_t ERR_INVALID_DATA = 5;
} // namespace

class NetsysControllerTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();

    sptr<NetsysNative::NetDiagCallbackStubTest> netDiagCallback = new NetsysNative::NetDiagCallbackStubTest();
    sptr<NetsysNative::MockINetsysService> mockNetsysService_ = sptr<NetsysNative::MockINetsysService>::MakeSptr();
};

void NetsysControllerTest::SetUpTestCase() {}

void NetsysControllerTest::TearDownTestCase() {}

void NetsysControllerTest::SetUp() {}

void NetsysControllerTest::TearDown() {}

HWTEST_F(NetsysControllerTest, NetsysControllerTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->NetworkCreatePhysical(NET_ID, PERMISSION);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkDestroy(NET_ID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->NetworkAddInterface(NET_ID, WLAN, BEARER_DEFAULT);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkRemoveInterface(NET_ID, WLAN);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest003, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->NetworkAddRoute(NET_ID, ETH0, DESTINATION, NEXT_HOP);
    EXPECT_LE(ret, 0);

    ret = netsysController->NetworkRemoveRoute(NET_ID, ETH0, DESTINATION, NEXT_HOP);
    EXPECT_LE(ret, 0);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest004, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    OHOS::nmd::InterfaceConfigurationParcel parcel;
    parcel.ifName = ETH0;
    parcel.ipv4Addr = PARCEL_IPV4_ADDR;
    int32_t ret = netsysController->SetInterfaceConfig(parcel);
    EXPECT_EQ(ret, 0);

    ret = netsysController->GetInterfaceConfig(parcel);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest005, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->SetInterfaceDown(ETH0);
    EXPECT_EQ(ret, 0);

    ret = netsysController->SetInterfaceUp(ETH0);
    EXPECT_EQ(ret, 0);

    netsysController->ClearInterfaceAddrs(ETH0);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest006, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->SetInterfaceMtu(ETH0, TEST_MTU);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetInterfaceMtu(ETH0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetTcpBufferSizes(TCP_BUFFER_SIZES);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest007, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    auto ifaceList = netsysController->InterfaceGetList();
    bool eth0NotExist = std::find(ifaceList.begin(), ifaceList.end(), std::string(ETH0)) == ifaceList.end();
    ASSERT_NE(eth0NotExist, true);

    int32_t ret = netsysController->AddInterfaceAddress(ETH0, IP_ADDR, PREFIX_LENGTH);
    EXPECT_EQ(ret, 0);

    ret = netsysController->DelInterfaceAddress(ETH0, IP_ADDR, PREFIX_LENGTH);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest008, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->SetResolverConfig(NET_ID, g_baseTimeoutMsec, g_retryCount, {}, {});
    EXPECT_EQ(ret, 0);

    std::vector<std::string> servers;
    std::vector<std::string> domains;
    ret = netsysController->GetResolverConfig(NET_ID, servers, domains, g_baseTimeoutMsec, g_retryCount);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest009, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->CreateNetworkCache(NET_ID);
    EXPECT_EQ(ret, 0);

    ret = netsysController->DestroyNetworkCache(NET_ID);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest010, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    nmd::NetworkSharingTraffic traffic;
    int32_t ret = netsysController->GetNetworkSharingTraffic(ETH0, ETH0, traffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    nmd::NetworkDpaTrafficReport dpaTraffic = {};
    ret = netsysController->SetDpaCellularSharingTraffic(dpaTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest011, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->GetCellularRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetCellularTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetAllRxBytes();
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetAllTxBytes();
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidRxBytes(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidTxBytes(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidOnIfaceRxBytes(TEST_UID, INTERFACE_NAME);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidOnIfaceTxBytes(TEST_UID, INTERFACE_NAME);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetIfaceRxBytes(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetIfaceTxBytes(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest012, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::vector<std::string> getList = netsysController->InterfaceGetList();

    getList.clear();
    getList = netsysController->UidGetList();
    EXPECT_EQ(getList.size(), 0);

    int64_t ret = netsysController->GetIfaceRxPackets(INTERFACE_NAME);
    EXPECT_EQ(ret, 0);

    ret = netsysController->GetIfaceTxPackets(INTERFACE_NAME);
    EXPECT_EQ(ret, 0);

    ret = netsysController->SetDefaultNetWork(NET_ID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->ClearDefaultNetWorkNetId();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest013, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->IpEnableForwarding(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->IpDisableForwarding(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->EnableNat(ETH0, ETH0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DisableNat(ETH0, ETH0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->IpfwdAddInterfaceForward(ETH0, ETH0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->IpfwdRemoveInterfaceForward(ETH0, ETH0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest014, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->ShareDnsSet(NET_ID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StartDnsProxyListen();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StopDnsProxyListen();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BindNetworkServiceVpn(SOCKET_FD);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ifreq ifRequest;
    ret = netsysController->EnableVirtualNetIfaceCard(SOCKET_FD, ifRequest, g_ifaceFd);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->SetIpAddress(SOCKET_FD, IP_ADDR, PREFIX_LENGTH, ifRequest);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->SetBlocking(g_ifaceFd, true);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->SetBlocking(g_ifaceFd, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StartDhcpClient(INTERFACE_NAME, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StartDhcpClient(INTERFACE_NAME, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StopDhcpClient(INTERFACE_NAME, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StopDhcpClient(INTERFACE_NAME, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StartDhcpService(INTERFACE_NAME, IP_ADDR);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StopDhcpService(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest015, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    netsysController->BandwidthEnableDataSaver(false);
    int32_t ret = netsysController->BandwidthEnableDataSaver(true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthSetIfaceQuota(IF_NAME, BYTES);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthSetIfaceQuota(WLAN, BYTES);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthRemoveIfaceQuota(IF_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthRemoveIfaceQuota(WLAN);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthAddDeniedList(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthAddAllowedList(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthRemoveDeniedList(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthRemoveAllowedList(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<uint32_t> uids;
    uids.push_back(TEST_UID);
    ret = netsysController->FirewallSetUidsAllowedListChain(TEST_UID, uids);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysController->FirewallSetUidsDeniedListChain(TEST_UID, uids);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->FirewallEnableChain(TEST_UID, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->FirewallSetUidRule(TEST_UID, {TEST_UID}, FIREWALL_RULE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest016, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->InterfaceSetIpAddress("ifaceName", "192.168.x.x");
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->InterfaceSetIpAddress("ifaceName", "192.168.2.0");
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->InterfaceSetIffUp("");
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->InterfaceSetIffUp("ifaceName");
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string hostName = "";
    std::string serverName = "";
    AddrInfo hints;
    uint16_t netId = 0;
    std::vector<AddrInfo> res;

    ret = netsysController->GetAddrInfo(hostName, serverName, hints, netId, res);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    auto callback = new NetsysControllerCallbackTestCb();
    ret = netsysController->RegisterCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest017, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint64_t stats = 0;
    int32_t ret = netsysController->GetTotalStats(stats, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    stats = 0;
    ret = netsysController->GetUidStats(stats, 0, TEST_STATS_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    stats = 0;
    ret = netsysController->GetIfaceStats(stats, 0, IFACE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DeleteStatsInfo(TEST_UID_32);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DeleteSimStatsInfo(TEST_UID_32);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    stats = 0;
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> statsInfo;
    ret = netsysController->GetAllStatsInfo(statsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetAllSimStatsInfo(statsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest018, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string respond;
    int32_t ret = netsysController->SetIptablesCommandForRes("-L", respond);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetManagerBaseAccessToken token;
    ret = netsysController->SetIptablesCommandForRes("abc", respond);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetIptablesCommandForRes("-L", respond);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerTest019, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string respond;
    int32_t ret = netsysController->SetIpCommandForRes("-L", respond);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetManagerBaseAccessToken token;
    ret = netsysController->SetIpCommandForRes("abc", respond);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, SetNetStatusMap002, TestSize.Level1)
{
    int32_t ret = NetsysController::GetInstance().SetNetStatusMap(0, 1);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, NetsysControllerErr001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::vector<int32_t> beginUids;
    std::vector<int32_t> endUids;
    std::string iface = "test";
    OHOS::nmd::InterfaceConfigurationParcel Parcel;

    int32_t ret = netsysController->SetInternetPermission(0, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkCreateVirtual(0, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkDestroy(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkAddUids(0, beginUids, endUids, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkDelUids(0, beginUids, endUids, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkAddInterface(0, iface, BEARER_DEFAULT);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkRemoveInterface(0, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkAddRoute(0, iface, iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkRemoveRoute(0, iface, iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetInterfaceConfig(Parcel);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetInterfaceConfig(Parcel);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetInterfaceDown(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetInterfaceUp(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    netsysController->ClearInterfaceAddrs(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetInterfaceMtu(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetInterfaceMtu(iface, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerErr002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string iface = "test";
    std::vector<std::string> servers;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    AddrInfo hints = {0};
    std::vector<AddrInfo> res;
    nmd::NetworkSharingTraffic traffic;
    nmd::NetworkDpaTrafficReport dpaTraffic;
    addrinfo *aihead = static_cast<addrinfo *>(malloc(sizeof(addrinfo)));
    if (aihead != nullptr) {
        aihead->ai_next = nullptr;
        aihead->ai_addr = static_cast<sockaddr *>(malloc(sizeof(sockaddr)));
    }
    if (aihead != nullptr) {
        aihead->ai_canonname = static_cast<char *>(malloc(10));
    }

    int32_t ret = netsysController->AddInterfaceAddress(iface, iface, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DelInterfaceAddress(iface, iface, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->InterfaceSetIpAddress(iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->InterfaceSetIffUp(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetResolverConfig(0, 0, 0, servers, servers);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetResolverConfig(0, servers, servers, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->CreateNetworkCache(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DestroyNetworkCache(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    netsysController->FreeAddrInfo(aihead);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetAddrInfo(iface, iface, hints, 0, res);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetNetworkSharingTraffic(iface, iface, traffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetDpaCellularSharingTraffic(dpaTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerErr003, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string iface = "test";

    auto ret = netsysController->GetCellularRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetCellularTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetAllRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetAllTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidRxBytes(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidTxBytes(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidOnIfaceRxBytes(0, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidOnIfaceTxBytes(0, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetIfaceRxBytes(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetIfaceTxBytes(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetIfaceRxPackets(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetIfaceTxPackets(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerErr004, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string iface = "test";
    NetsysNotifyCallback callback;

    auto faceList = netsysController->InterfaceGetList();


    auto uidList = netsysController->UidGetList();
    EXPECT_EQ(uidList.size(), 0);

    auto ret = netsysController->SetDefaultNetWork(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->ClearDefaultNetWorkNetId();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->IpEnableForwarding(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->IpDisableForwarding(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->EnableNat(iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DisableNat(iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->IpfwdAddInterfaceForward(iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->IpfwdRemoveInterfaceForward(iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->ShareDnsSet(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StartDnsProxyListen();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StopDnsProxyListen();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->RegisterNetsysNotifyCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerErr005, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string iface = "test";
    struct ifreq ifRequest;
    int32_t ifaceFd = 0;
    sptr<NetsysControllerCallback> callback;
    auto ret = netsysController->BindNetworkServiceVpn(0);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->BindNetworkServiceVpn(1);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->EnableVirtualNetIfaceCard(0, ifRequest, ifaceFd);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->EnableVirtualNetIfaceCard(1, ifRequest, ifaceFd);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->SetIpAddress(0, iface, 0, ifRequest);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->SetIpAddress(1, iface, 1, ifRequest);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_ERR_VPN);

    ret = netsysController->SetBlocking(0, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StartDhcpClient(iface, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StopDhcpClient(iface, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StartDhcpService(iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->StopDhcpService(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthEnableDataSaver(false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthSetIfaceQuota(iface, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthRemoveIfaceQuota(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthAddDeniedList(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthRemoveDeniedList(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthAddAllowedList(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->BandwidthRemoveAllowedList(0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerErr006, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string iface = "test";
    std::vector<uint32_t> uids;
    uint64_t stats = 0;
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> statsInfo;

    auto ret = netsysController->FirewallSetUidsAllowedListChain(0, uids);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->FirewallSetUidsDeniedListChain(0, uids);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->FirewallEnableChain(0, false);
    ret = netsysController->FirewallSetUidRule(0, uids, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->ClearFirewallAllRules();
    ret = netsysController->GetTotalStats(stats, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetUidStats(stats, 0, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetIfaceStats(stats, 0, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetAllStatsInfo(statsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetIptablesCommandForRes(iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetIpCommandForRes(iface, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetTcpBufferSizes("");
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetDiagGetRouteTable001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::list<OHOS::NetsysNative::NetDiagRouteTable> diagrouteTable;
    auto ret = netsysController->NetDiagGetRouteTable(diagrouteTable);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    for (const auto &lt : diagrouteTable) {
        NETNATIVE_LOGI(
            "show NetDiagRouteTable destination_:%{public}s gateway_:%{public}s"
            "mask_:%{public}s iface_:%{public}s flags_:%{public}s metric_:%{public}d"
            "ref_:%{public}d use_:%{public}d",
            lt.destination_.c_str(), lt.gateway_.c_str(), lt.mask_.c_str(), lt.iface_.c_str(), lt.flags_.c_str(),
            lt.metric_, lt.ref_, lt.use_);
    }
}

void ShowSocketInfo(NetsysNative::NetDiagSocketsInfo &info)
{
    for (const auto &lt : info.netProtoSocketsInfo_) {
        NETNATIVE_LOGI(
            "ShowSocketInfo NeyDiagNetProtoSocketInfo protocol_:%{public}s localAddr_:%{public}s"
            "foreignAddr_:%{public}s state_:%{public}s user_:%{public}s programName_:%{public}s recvQueue_:%{public}d"
            "sendQueue_:%{public}d inode_:%{public}d ",
            lt.protocol_.c_str(), lt.localAddr_.c_str(), lt.foreignAddr_.c_str(), lt.state_.c_str(), lt.user_.c_str(),
            lt.programName_.c_str(), lt.recvQueue_, lt.sendQueue_, lt.inode_);
    }

    for (const auto &lt : info.unixSocketsInfo_) {
        NETNATIVE_LOGI(
            "ShowSocketInfo  unixSocketsInfo_ refCnt_:%{public}d inode_:%{public}d protocol_:%{public}s"
            "flags_:%{public}s type_:%{public}s state_:%{public}s path_:%{public}s",
            lt.refCnt_, lt.inode_, lt.protocol_.c_str(), lt.flags_.c_str(), lt.type_.c_str(), lt.state_.c_str(),
            lt.path_.c_str());
    }
}

HWTEST_F(NetsysControllerTest, NetDiagGetSocketsInfo001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    OHOS::NetsysNative::NetDiagProtocolType socketType = OHOS::NetsysNative::NetDiagProtocolType::PROTOCOL_TYPE_ALL;
    OHOS::NetsysNative::NetDiagSocketsInfo socketsInfo;
    auto ret = netsysController->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ShowSocketInfo(socketsInfo);

    socketsInfo.unixSocketsInfo_.clear();
    socketsInfo.netProtoSocketsInfo_.clear();
    socketType = OHOS::NetsysNative::NetDiagProtocolType::PROTOCOL_TYPE_RAW;
    ret = netsysController->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ShowSocketInfo(socketsInfo);

    socketsInfo.unixSocketsInfo_.clear();
    socketsInfo.netProtoSocketsInfo_.clear();
    socketType = OHOS::NetsysNative::NetDiagProtocolType::PROTOCOL_TYPE_TCP;
    ret = netsysController->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ShowSocketInfo(socketsInfo);

    socketsInfo.unixSocketsInfo_.clear();
    socketsInfo.netProtoSocketsInfo_.clear();
    socketType = OHOS::NetsysNative::NetDiagProtocolType::PROTOCOL_TYPE_UDP;
    ret = netsysController->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ShowSocketInfo(socketsInfo);

    socketsInfo.unixSocketsInfo_.clear();
    socketsInfo.netProtoSocketsInfo_.clear();
    socketType = OHOS::NetsysNative::NetDiagProtocolType::PROTOCOL_TYPE_UNIX;
    ret = netsysController->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ShowSocketInfo(socketsInfo);
}

HWTEST_F(NetsysControllerTest, NetDiagGetInterfaceConfig001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::list<OHOS::NetsysNative::NetDiagIfaceConfig> configs;
    std::string ifaceName = "eth0";

    auto ret = netsysController->NetDiagGetInterfaceConfig(configs, ifaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    for (const OHOS::NetsysNative::NetDiagIfaceConfig &lt : configs) {
        NETNATIVE_LOGI(
            "ShowSocketInfo  DiagGetInterfaceConfig  ifaceName_:%{public}s linkEncap_:%{public}s"
            "ipv4Addr_:%{public}s ipv4Bcast_:%{public}s ipv4Mask_:%{public}s mtu_:%{public}d txQueueLen_:%{public}d"
            "rxBytes_:%{public}d txBytes_:%{public}d isUp_:%{public}d",
            lt.ifaceName_.c_str(), lt.linkEncap_.c_str(), CommonUtils::ToAnonymousIp(lt.ipv4Addr_).c_str(),
            lt.ipv4Bcast_.c_str(), lt.ipv4Mask_.c_str(), lt.mtu_, lt.txQueueLen_, lt.rxBytes_, lt.txBytes_, lt.isUp_);
    }

    configs.clear();
    ifaceName = "eth1";
    ret = netsysController->NetDiagGetInterfaceConfig(configs, ifaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    for (const OHOS::NetsysNative::NetDiagIfaceConfig &lt : configs) {
        NETNATIVE_LOGI(
            "ShowSocketInfo  DiagGetInterfaceConfig ifaceName_:%{public}s linkEncap_:%{public}s"
            "ipv4Addr_:%{public}s ipv4Bcast_:%{public}s ipv4Mask_:%{public}s mtu_:%{public}d txQueueLen_:%{public}d"
            "rxBytes_:%{public}d txBytes_:%{public}d isUp_:%{public}d ",
            lt.ifaceName_.c_str(), lt.linkEncap_.c_str(), CommonUtils::ToAnonymousIp(lt.ipv4Addr_).c_str(),
            lt.ipv4Bcast_.c_str(), lt.ipv4Mask_.c_str(), lt.mtu_, lt.txQueueLen_, lt.rxBytes_, lt.txBytes_, lt.isUp_);
    }
}

HWTEST_F(NetsysControllerTest, NetDiagSetInterfaceActiveState001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::list<OHOS::NetsysNative::NetDiagIfaceConfig> configs;
    std::string ifaceName = "eth0";

    auto ret = netsysController->NetDiagSetInterfaceActiveState(ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    configs.clear();
    ifaceName = "eth1";
    ret = netsysController->NetDiagSetInterfaceActiveState(ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetDiagUpdateInterfaceConfig001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string ifaceName = "eth0";
    OHOS::NetsysNative::NetDiagIfaceConfig config;
    config.ifaceName_ = ifaceName;
    config.ipv4Addr_ = "192.168.222.234";
    config.ipv4Mask_ = "255.255.255.0";
    config.ipv4Bcast_ = "255.255.255.0";
    bool add = true;
    auto ret = netsysController->NetDiagUpdateInterfaceConfig(config, ifaceName, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ifaceName = "eth1";
    add = false;
    ret = netsysController->NetDiagUpdateInterfaceConfig(config, ifaceName, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetDiagPing001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    OHOS::NetsysNative::NetDiagPingOption pingOption;
    pingOption.destination_ = "127.0.0.1";
    const int maxWaitSecond = 10;
    g_isWaitAsync = true;
    auto ret = netsysController->NetDiagPingHost(pingOption, netDiagCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    std::chrono::steady_clock::time_point tp1 = std::chrono::steady_clock::now();
    while (g_isWaitAsync) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::chrono::steady_clock::time_point tp2 = std::chrono::steady_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(tp2 - tp1).count() > maxWaitSecond) {
            break;
        }
    }
}

HWTEST_F(NetsysControllerTest, NetsysControllerErr007, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";

    std::string ipAddr1 = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr1 = "aa:bb:cc:dd:ee:ff";
    std::string ifName1 = "chba0";

    auto ret = netsysController->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    auto ret1 = netsysController->AddStaticIpv6Addr(ipAddr1, macAddr1, ifName1);
    EXPECT_EQ(ret1, NetManagerStandard::NETMANAGER_SUCCESS);

    ret1 = netsysController->DelStaticIpv6Addr(ipAddr1, macAddr1, ifName1);
    EXPECT_EQ(ret1, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkCreatePhysical(NET_ID, PERMISSION);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string cmd = "";
    std::string respond = "";
    ret = netsysController->SetIptablesCommandForRes(cmd, respond);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->SetIpCommandForRes(cmd, respond);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    OHOS::NetsysNative::NetDiagPingOption pingOption = {};
    ret = netsysController->NetDiagPingHost(pingOption, netDiagCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::list<OHOS::NetsysNative::NetDiagRouteTable> diagrouteTable;
    ret = netsysController->NetDiagGetRouteTable(diagrouteTable);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    OHOS::NetsysNative::NetDiagProtocolType socketType = OHOS::NetsysNative::NetDiagProtocolType::PROTOCOL_TYPE_ALL;
    OHOS::NetsysNative::NetDiagSocketsInfo socketsInfo = {};
    ret = netsysController->NetDiagGetSocketsInfo(socketType, socketsInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::list<OHOS::NetsysNative::NetDiagIfaceConfig> configs;
    std::string ifaceName = "eth0";
    ret = netsysController->NetDiagGetInterfaceConfig(configs, ifaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    OHOS::NetsysNative::NetDiagIfaceConfig config;
    ret = netsysController->NetDiagUpdateInterfaceConfig(config, ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetDiagSetInterfaceActiveState(ifaceName, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerBranchTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::vector<int32_t> beginUids = {1};
    std::vector<int32_t> endUids = {1};
    int32_t netId = 0;

    netsysController->NetworkCreateVirtual(netId, false);

    auto ret = netsysController->NetworkAddUids(netId, beginUids, endUids, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkDelUids(netId, beginUids, endUids, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    endUids = {1, 2};
    ret = netsysController->NetworkAddUids(netId, beginUids, endUids, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);

    ret = netsysController->NetworkDelUids(netId, beginUids, endUids, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetsysControllerTest, NetsysControllerBranchTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t uid = 0;
    uint8_t allow = 0;
    auto ret = netsysController->SetInternetPermission(uid, allow);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";

    std::string ipAddr1 = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr1 = "aa:bb:cc:dd:ee:ff";
    std::string ifName1 = "chba0";
    ret = netsysController->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->AddStaticIpv6Addr(ipAddr1, macAddr1, ifName1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DelStaticIpv6Addr(ipAddr1, macAddr1, ifName1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetsysNotifyCallback callback;
    ret = netsysController->RegisterNetsysNotifyCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t netId = 0;
    int32_t permission = 0;
    ret = netsysController->NetworkCreatePhysical(netId, permission);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->NetworkCreateVirtual(netId, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, GetCookieStatsTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint64_t stats = 0;
    BpfMapper<socket_cookie_stats_key, app_cookie_stats_value> appCookieStatsMap(APP_COOKIE_STATS_MAP_PATH, BPF_ANY);
    int32_t ret = netsysController->GetCookieStats(stats, TEST_STATS_TYPE1, TEST_COOKIE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->GetCookieStats(stats, TEST_STATS_TYPE2, TEST_COOKIE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, GetNetworkSharingTypeTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::set<uint32_t> sharingTypeIsOn;
    int32_t ret = netsysController->GetNetworkSharingType(sharingTypeIsOn);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, UpdateNetworkSharingTypeTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint64_t type = 0;
    bool isOpen = true;
    int32_t ret = netsysController->UpdateNetworkSharingType(type, isOpen);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerBranchTest003, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t timeStep = 0;
    sptr<OHOS::NetManagerStandard::NetsysDnsReportCallback> callback = nullptr;
    int32_t ret = netsysController->RegisterDnsResultCallback(callback, timeStep);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = netsysController->UnregisterDnsResultCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<OHOS::NetManagerStandard::NetsysDnsQueryReportCallback> queryCallback = nullptr;
    ret = netsysController->RegisterDnsQueryResultCallback(queryCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = netsysController->UnregisterDnsQueryResultCallback(queryCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysControllerTest, SetEnableIpv6Test001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t on = 0;
    std::string interface = "wlan0";
    int32_t ret = netsysController->SetIpv6PrivacyExtensions(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysController->SetEnableIpv6(interface, on, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysController->SetEnableIpv6(interface, on, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, SetIpv6UidBlackListTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;
 
    int32_t uid = 0;
    std::vector<int32_t> netIds = {};
    int32_t ret = netsysController->SetIpv6UidBlackList(netIds, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysControllerTest, SetIpv6UidBlackListTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = nullptr;
 
    int32_t uid = 0;
    std::vector<int32_t> netIds = {};
    int32_t ret = netsysController->SetIpv6UidBlackList(netIds, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, SetIpv6AutoConfTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t on = 0;
    std::string interface = "wlan0";
    int32_t ret = netsysController->SetIpv6AutoConf(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, SetDnsCacheTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint16_t netId = 101;
    std::string testHost = "test";
    AddrInfo info;
    int32_t ret = netsysController->SetDnsCache(netId, testHost, info);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NetsysControllerBranchTest004, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    uint32_t timeStep = 0;
    sptr<OHOS::NetManagerStandard::NetsysDnsReportCallback> callback = nullptr;
    int32_t ret = netsysController->RegisterDnsResultCallback(callback, timeStep);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);

    ret = netsysController->UnregisterDnsResultCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);

    sptr<OHOS::NetManagerStandard::NetsysDnsQueryReportCallback> queryCallback = nullptr;
    ret = netsysController->RegisterDnsQueryResultCallback(queryCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);

    ret = netsysController->UnregisterDnsQueryResultCallback(queryCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);

    uint64_t stats = 0;
    ret = netsysController->GetCookieStats(stats, TEST_STATS_TYPE1, TEST_COOKIE);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, SetIpv6PrivacyExtensionsTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t on = 0;
    std::string interface = "wlan0";
    int32_t ret = netsysController->SetIpv6PrivacyExtensions(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysController->SetEnableIpv6(interface, on, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, SetNetworkAccessPolicy001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t uid = 0;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    int32_t ret = netsysController->SetNetworkAccessPolicy(uid, netAccessPolicy, reconfirmFlag);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, NotifyNetBearerTypeChange001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::set<NetManagerStandard::NetBearType> bearTypes;
    bearTypes.insert(NetManagerStandard::NetBearType::BEARER_CELLULAR);
    int32_t ret = netsysController->NotifyNetBearerTypeChange(bearTypes);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, DeleteNetworkAccessPolicy001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t uid = 0;
    int32_t ret = netsysController->DeleteNetworkAccessPolicy(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, CreateVnic001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint16_t mtu = 1500;
    std::string tunAddr = "192.168.1.100";
    int32_t prefix = 24;
    std::set<int32_t> uids;
    int32_t ret = netsysController->CreateVnic(mtu, tunAddr, prefix, uids);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, DestroyVnic001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->DestroyVnic();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, EnableDistributedClientNetTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    int32_t ret = netsysController->EnableDistributedClientNet("192.168.1.100", "virnic", ETH0);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, EnableDistributedClientNetTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->EnableDistributedClientNet("192.168.1.100", "virnic", ETH0);
    EXPECT_NE(ret, NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, EnableDistributedServerNetTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    int32_t ret = netsysController->EnableDistributedServerNet(ETH0, WLAN, "192.168.1.100", "0.0.0.0");
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, EnableDistributedServerNetTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->EnableDistributedServerNet(ETH0, WLAN, "192.168.1.100", "0.0.0.0");
    EXPECT_NE(ret, NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, DisableDistributedNetTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    std::string virnicName = "virnic";
    std::string dstAddr = "1.1.1.1";
    int32_t ret = netsysController->DisableDistributedNet(true, virnicName, dstAddr);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, DisableDistributedNetTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string virnicName = "virnic";
    std::string dstAddr = "1.1.1.1";
    int32_t ret = netsysController->DisableDistributedNet(true, virnicName, dstAddr);
    EXPECT_NE(ret, NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, GetNetworkCellularSharingTrafficTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    nmd::NetworkSharingTraffic traffic;
    std::string ifaceName;
    int32_t ret = netsysController->GetNetworkCellularSharingTraffic(traffic, ifaceName);
    EXPECT_EQ(ret, NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, GetNetworkCellularSharingTrafficTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    nmd::NetworkSharingTraffic traffic;
    std::string ifaceName;
    int32_t ret = netsysController->GetNetworkCellularSharingTraffic(traffic, ifaceName);
    EXPECT_NE(ret, NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, CloseSocketsUid002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string ipAddr = "";
    uint32_t uid = 1000;
    int32_t result = netsysController->CloseSocketsUid(ipAddr, uid);
    EXPECT_NE(result, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, CloseSocketsUid001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    std::string ipAddr = "";
    uint32_t uid = 1000;
    int32_t result = netsysController->CloseSocketsUid(ipAddr, uid);
    EXPECT_EQ(result, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, SetBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::unordered_map<uint32_t, uint32_t> params;
    int32_t ret = netsysController->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, SetBrokerUidAccessPolicyMapTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    std::unordered_map<uint32_t, uint32_t> params;
    params.emplace(TEST_UID_32, TEST_UID_32);
    int32_t ret = netsysController->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, DelBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->DelBrokerUidAccessPolicyMap(TEST_UID_32);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, SetBrokerUidAccessPolicyMapTest003, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::unordered_map<uint32_t, uint32_t> params;
    int32_t ret = netsysController->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, DelBrokerUidAccessPolicyMapTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    int32_t ret = netsysController->DelBrokerUidAccessPolicyMap(TEST_UID_32);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
HWTEST_F(NetsysControllerTest, EnableWearableDistributedNetForward, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->EnableWearableDistributedNetForward(8001, 8002);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);

    ret = netsysController->DisableWearableDistributedNetForward();
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, DisableWearableDistributedNetForward, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    netsysController->initFlag_ = false;
    netsysController->Init();
    int32_t ret = netsysController->EnableWearableDistributedNetForward(8001, 8002);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netsysController->DisableWearableDistributedNetForward();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
#endif

HWTEST_F(NetsysControllerTest, EnableDistributedClientNet001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string virnicAddr = "1.189.55.61";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    int32_t ret = netsysController->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = netsysController->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, EnableDistributedServerNet001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";
    int32_t ret = netsysController->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    bool isServer = true;
    std::string virnicName = "virnic";
    ret = netsysController->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, GetNetworkCellularSharingTraffic001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    nmd::NetworkSharingTraffic traffic;
    std::string ifaceName = "virnic";

    int32_t ret = netsysController->GetNetworkCellularSharingTraffic(traffic, ifaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, SetGetClearNetStateTrafficMap001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint8_t flag = 1;
    uint64_t availableTraffic = 1000000;

    int32_t ret = netsysController->SetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysController->GetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysController->ClearIncreaseTrafficMap();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = netsysController->DeleteIncreaseTrafficMap(12);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, ClearSimStatsBpfMap001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    int32_t ret = netsysController->ClearSimStatsBpfMap();
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, SetGetClearNetStateTrafficMap002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    uint8_t flag = 1;
    uint64_t availableTraffic = 1000000;

    int32_t ret = netsysController->SetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
    ret = netsysController->GetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
    ret = netsysController->ClearIncreaseTrafficMap();
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
    ret = netsysController->DeleteIncreaseTrafficMap(12);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, UpdateIfIndexMap001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    uint8_t key = 1;
    uint64_t index = 10;
    int32_t ret = netsysController->UpdateIfIndexMap(key, index);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, RegisterNetsysTrafficCallback002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = netsysController->RegisterNetsysTrafficCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, RegisterNetsysTrafficCallback001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = netsysController->RegisterNetsysTrafficCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, StartStopClat001, TestSize.Level1)
{
    std::string interfaceName = "eth0";
    int32_t netId = 1;
    std::string nat64PrefixStr = "2001:db8::/64";
    NetsysController::GetInstance().netsysService_ = nullptr;

    int32_t ret = NetsysController::GetInstance().StartClat(interfaceName, netId, nat64PrefixStr);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
    ret = NetsysController::GetInstance().StopClat(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, SetNicTrafficAllowed001, TestSize.Level1)
{
    std::vector<std::string> ifaceNames = {"eth0", "wlan0"};
    bool status = true;
    NetsysController::GetInstance().netsysService_ = nullptr;

    int32_t ret = NetsysController::GetInstance().SetNicTrafficAllowed(ifaceNames, status);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, SetNetStatusMap001, TestSize.Level1)
{
    NetsysController::GetInstance().netsysService_ = nullptr;

    int32_t ret = NetsysController::GetInstance().SetNetStatusMap(0, 1);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, DeleteIncreaseTrafficMap001, TestSize.Level1)
{
    NetsysController::GetInstance().netsysService_ = nullptr;
    int32_t ret = NetsysController::GetInstance().DeleteIncreaseTrafficMap(12); // 12:ifindex
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, UnRegisterNetsysTrafficCallback001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = netsysController->UnRegisterNetsysTrafficCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, UnRegisterNetsysTrafficCallback002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = netsysController->UnRegisterNetsysTrafficCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, SetUserDefinedServerFlag001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    std::string interfaceName = "eth0";
    int32_t netId = 1;
    std::string nat64PrefixStr = "2001:db8::/64";
    int32_t ret = netsysController->StartClat(interfaceName, netId, nat64PrefixStr);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
    ret = netsysController->StopClat(interfaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, SetDnsCacheTest02, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();

    std::vector<std::string> ifaceNames = {"eth0", "wlan0"};
    bool status = true;
    int32_t ret = netsysController->SetNicTrafficAllowed(ifaceNames, status);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
HWTEST_F(NetsysControllerTest, UpdateEnterpriseRouteTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t uid = 20000138;
    std::string ifname = "wlan0";
    bool add = true;
    auto ret = netsysController->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysControllerTest, UpdateEnterpriseRouteTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t uid = 0;
    std::string ifname = "wlan0";
    bool add = true;
    auto ret = netsysController->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysControllerTest, UpdateEnterpriseRouteTest003, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint32_t uid = 20000138;
    std::string ifname = "notexist";
    bool add = true;
    auto ret = netsysController->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
#endif

HWTEST_F(NetsysControllerTest, FlushDnsCache001, TestSize.Level1)
{
    uint16_t netId = 101;
    auto netsysController = std::make_shared<NetsysController>();
    netsysController->netsysService_  = nullptr;
    int32_t ret = netsysController->FlushDnsCache(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, FlushDnsCache002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    uint16_t netId = 101;
    int32_t ret = netsysController->FlushDnsCache(netId);
    EXPECT_NE(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, SetInternetAccessByIpForWifiShare001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    std::string ipAddr = "1.1.1.1";
    uint8_t family = 0;
    bool accessInternet = true;
    std::string clientNetIfName = "test";

    int32_t res = netsysController->SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
    EXPECT_EQ(res, NetManagerStandard::NETSYS_PARAM_ERROR);

    family = 2;
    netsysController->netsysService_ = nullptr;
    res = netsysController->SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
    EXPECT_EQ(res, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);

    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;
    res = netsysController->SetInternetAccessByIpForWifiShare(ipAddr, family, accessInternet, clientNetIfName);
    EXPECT_EQ(res, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, GetIpNeighTable001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::vector<NetIpMacInfo> ipMacInfo;
    int32_t ret = netsysController->GetIpNeighTable(ipMacInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, GetIpNeighTable002, TestSize.Level1)
{
    std::vector<NetIpMacInfo> ipMacInfo;
    auto netsysController = std::make_shared<NetsysController>();
    netsysController->netsysService_  = nullptr;
    int32_t ret = netsysController->GetIpNeighTable(ipMacInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, CreateVlan001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = netsysController->CreateVlan(ifName, vlanId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, CreateVlan002, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    auto netsysController = std::make_shared<NetsysController>();
    netsysController->netsysService_  = nullptr;
    int32_t ret = netsysController->CreateVlan(ifName, vlanId);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, DestroyVlan001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = netsysController->DestroyVlan(ifName, vlanId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, DestroyVlan002, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    auto netsysController = std::make_shared<NetsysController>();
    netsysController->netsysService_  = nullptr;
    int32_t ret = netsysController->DestroyVlan(ifName, vlanId);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, AddVlanIp001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    int32_t ret = netsysController->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, AddVlanIp002, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    auto netsysController = std::make_shared<NetsysController>();
    netsysController->netsysService_  = nullptr;
    int32_t ret = netsysController->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, GetConnectOwnerUidTest001, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    int32_t ret = netsysController->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysControllerTest, GetConnectOwnerUidTest002, TestSize.Level1)
{
    auto netsysController = std::make_shared<NetsysController>();
    netsysController->netsysService_  = nullptr;

    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    int32_t ret = netsysController->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}

HWTEST_F(NetsysControllerTest, GetSystemNetPortStatesTest001, TestSize.Level1)
{
    NetPortStatesInfo netPortStatesInfo;
    auto netsysController = std::make_shared<NetsysController>();
    auto netsysControllerServiceImpl = sptr<NetsysControllerServiceImpl>::MakeSptr();
    netsysControllerServiceImpl->netsysClient_->netsysNativeService_ = mockNetsysService_;
    netsysController->netsysService_ = netsysControllerServiceImpl;

    int32_t ret = netsysController->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    netsysController->netsysService_  = nullptr;
    ret = netsysController->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_NETSYSSERVICE_NULL);
}
} // namespace NetManagerStandard
} // namespace OHOS

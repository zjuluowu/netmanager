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

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#endif
#include "bpf_def.h"
#include "bpf_mapper.h"
#include "bpf_path.h"
#include "net_manager_constants.h"
#include "net_stats_constants.h"
#include "netsys_native_client.h"
#ifdef FEATURE_NET_FIREWALL_ENABLE
#include "netfirewall_callback_stub.h"
#endif

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
static constexpr const char *DESTINATION = "192.168.1.3/24";
static constexpr const char *NEXT_HOP = "192.168.1.1";
static constexpr const char *LOCALIP = "127.0.0.1";
static constexpr const char *IF_NAME = "iface0";
static constexpr const char *ETH0 = "eth0";
static constexpr const char *IFACE = "test0";
static constexpr const char *IP_ADDR = "172.17.5.245";
static constexpr const char *INTERFACE_NAME = "interface_name";
static constexpr const char *REQUESTOR = "requestor";
static constexpr const char *TCP_BUFFER_SIZES = "524288,1048576,2097152,262144,524288,1048576";
static constexpr uint64_t TEST_COOKIE = 1;
static constexpr uint32_t TEST_STATS_TYPE1 = 0;
static constexpr uint32_t TEST_STATS_TYPE2 = 2;
static constexpr uint32_t TEST_UID = 1;
const int32_t MTU = 111;
const int32_t NET_ID = 2;
const int32_t IFACEFD = 5;
const int64_t UID = 1010;
const int32_t APP_ID = 101010;
const int32_t SOCKET_FD = 5;
const int32_t PERMISSION = 5;
const int32_t STATRUID = 1000;
const int32_t ENDUID = 1100;
const int32_t PREFIX_LENGTH = 23;
const int32_t INVALID_ARGUMENTS = -22;
uint16_t BASE_TIMEOUT_MSEC = 200;
const int64_t CHAIN = 1010;
uint8_t RETRY_COUNT = 3;
const uint32_t FIREWALL_RULE = 1;
} // namespace

class NetsysNativeClientTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void NetsysNativeClientTest::SetUpTestCase() {}

void NetsysNativeClientTest::TearDownTestCase() {}

void NetsysNativeClientTest::SetUp() {}

void NetsysNativeClientTest::TearDown() {}

#ifdef FEATURE_NET_FIREWALL_ENABLE
class TestNetFirewallCallbackStub : public OHOS::NetsysNative::NetFirewallCallbackStub {
public:
    int32_t OnIntercept(OHOS::sptr<InterceptRecord> &info) override
    {
        return 0;
    }
};
#endif

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t ret = nativeClient->NetworkCreatePhysical(NET_ID, PERMISSION);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->NetworkCreatePhysical(NET_ID, PERMISSION);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->NetworkDestroy(NET_ID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->NetworkAddInterface(NET_ID, IF_NAME, BEARER_DEFAULT);
    EXPECT_EQ(ret, -1);

    ret = nativeClient->NetworkRemoveInterface(NET_ID, IF_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->NetworkAddRoute(NET_ID, IF_NAME, DESTINATION, NEXT_HOP, false);
    EXPECT_EQ(ret, INVALID_ARGUMENTS);

    ret = nativeClient->NetworkRemoveRoute(NET_ID, IF_NAME, DESTINATION, NEXT_HOP, false);
    EXPECT_EQ(ret, INVALID_ARGUMENTS);

    OHOS::nmd::InterfaceConfigurationParcel parcel;
    ret = nativeClient->GetInterfaceConfig(parcel);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->SetInterfaceDown(IF_NAME);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->SetInterfaceUp(IF_NAME);
    EXPECT_EQ(ret, 0);

    nativeClient->ClearInterfaceAddrs(IF_NAME);

    ret = nativeClient->GetInterfaceMtu(IF_NAME);
    EXPECT_EQ(ret, -1);

    ret = nativeClient->SetInterfaceMtu(IF_NAME, MTU);
    EXPECT_EQ(ret, -1);

    ret = nativeClient->SetTcpBufferSizes(TCP_BUFFER_SIZES);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest002, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t ret = nativeClient->AddInterfaceAddress(IF_NAME, IP_ADDR, PREFIX_LENGTH);
    EXPECT_EQ(ret, -19);

    ret = nativeClient->DelInterfaceAddress(IF_NAME, IP_ADDR, PREFIX_LENGTH);
    EXPECT_EQ(ret, -19);

    ret = nativeClient->SetResolverConfig(NET_ID, BASE_TIMEOUT_MSEC, RETRY_COUNT, {}, {});
    EXPECT_EQ(ret, 0);

    std::vector<std::string> servers;
    std::vector<std::string> domains;
    ret = nativeClient->GetResolverConfig(NET_ID, servers, domains, BASE_TIMEOUT_MSEC, RETRY_COUNT);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->CreateNetworkCache(NET_ID);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->DestroyNetworkCache(NET_ID);
    EXPECT_EQ(ret, 0);

    nmd::NetworkSharingTraffic traffic;
    ret = nativeClient->GetNetworkSharingTraffic(ETH0, ETH0, traffic);
    EXPECT_NE(ret, 0);

    nmd::NetworkDpaTrafficReport dpaTraffic = {};
    ret = nativeClient->SetDpaCellularSharingTraffic(dpaTraffic);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetCellularRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetCellularTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetAllRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetAllTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest003, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t ret = nativeClient->GetUidRxBytes(NET_ID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetUidTxBytes(NET_ID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetUidOnIfaceRxBytes(NET_ID, INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetUidOnIfaceTxBytes(NET_ID, INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetIfaceRxBytes(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetIfaceTxBytes(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<std::string> interFaceGetList = nativeClient->InterfaceGetList();
    EXPECT_NE(interFaceGetList.size(), 0U);

    std::vector<std::string> uidGetList = nativeClient->UidGetList();
    EXPECT_EQ(uidGetList.size(), 0U);

    ret = nativeClient->GetIfaceRxPackets(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->GetIfaceTxPackets(INTERFACE_NAME);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<uint32_t> uids;
    uids.push_back(UID);
    ret = nativeClient->FirewallSetUidsAllowedListChain(CHAIN, uids);
    EXPECT_EQ(ret, -1);
    ret = nativeClient->FirewallSetUidsDeniedListChain(CHAIN, uids);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest004, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t ret = nativeClient->SetDefaultNetWork(NET_ID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->ClearDefaultNetWorkNetId();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient->IpEnableForwarding(REQUESTOR);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->IpDisableForwarding(REQUESTOR);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->EnableNat(ETH0, ETH0);
    EXPECT_EQ(ret, -1);

    ret = nativeClient->DisableNat(ETH0, ETH0);
    EXPECT_EQ(ret, -1);

    ret = nativeClient->IpfwdAddInterfaceForward(ETH0, ETH0);
    EXPECT_EQ(ret, -1);

    ret = nativeClient->IpfwdRemoveInterfaceForward(ETH0, ETH0);
    EXPECT_EQ(ret, -1);

    ret = nativeClient->ShareDnsSet(NET_ID);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->StartDnsProxyListen();
    EXPECT_EQ(ret, 0);

    ret = nativeClient->StopDnsProxyListen();
    EXPECT_EQ(ret, 0);

    ret = nativeClient->FirewallEnableChain(CHAIN, true);
    EXPECT_EQ(ret, -1);
    ret = nativeClient->FirewallSetUidRule(CHAIN, {NET_ID}, FIREWALL_RULE);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest005, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint64_t stats = 0;
    int32_t ret = nativeClient->GetTotalStats(stats, 0);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->GetUidStats(stats, 0, APP_ID);
    EXPECT_EQ(ret, NetStatsResultCode::STATS_ERR_READ_BPF_FAIL);

    ret = nativeClient->GetIfaceStats(stats, 0, IFACE);
    EXPECT_EQ(ret, NetStatsResultCode::STATS_ERR_GET_IFACE_NAME_FAILED);

    std::vector<OHOS::NetManagerStandard::NetStatsInfo> statsInfo;
    ret = nativeClient->GetAllStatsInfo(statsInfo);
    EXPECT_EQ(ret, 0);

    ret = nativeClient->GetAllSimStatsInfo(statsInfo);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest006, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::vector<UidRange> uidRanges;
    std::vector<int32_t> beginUids;
    std::vector<int32_t> endUids;
    beginUids.push_back(STATRUID);
    endUids.push_back(ENDUID);
    for (size_t i = 0; i < beginUids.size(); i++) {
        uidRanges.emplace_back(UidRange(beginUids[i], endUids[i], 0, 0));
    }
    nativeClient->NetworkAddUids(NET_ID, uidRanges);
    nativeClient->NetworkDelUids(NET_ID, uidRanges);
    int32_t ret = nativeClient->NetworkCreatePhysical(NET_ID, PERMISSION);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest007, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetsysNotifyCallback callback;
    struct ifreq ifreq = {};
    int32_t ret = nativeClient->RegisterNetsysNotifyCallback(callback);
    EXPECT_EQ(ret, 0);
    ret = nativeClient->BindNetworkServiceVpn(SOCKET_FD);
    int32_t ifacefd1 = 0;
    nativeClient->EnableVirtualNetIfaceCard(SOCKET_FD, ifreq, ifacefd1);
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ret = nativeClient->BindNetworkServiceVpn(sockfd);
    int32_t ifacefd2 = 0;
    nativeClient->EnableVirtualNetIfaceCard(sockfd, ifreq, ifacefd2);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest008, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t ret = nativeClient->SetBlocking(IFACEFD, true);
    EXPECT_EQ(ret, NETSYS_ERR_VPN);
    struct ifreq ifreq = {};
    int32_t sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ret = nativeClient->SetIpAddress(sockfd, LOCALIP, PREFIX_LENGTH, ifreq);
    EXPECT_EQ(ret, NETSYS_ERR_VPN);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest009, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetsysNativeClient::NativeNotifyCallback notifyCallback(nativeClient);
    std::string ifName = "wlan";
    bool up = true;
    int32_t ret = notifyCallback.OnInterfaceChanged(ifName, up);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest010, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetsysNativeClient::NativeNotifyCallback notifyCallback(nativeClient);
    sptr<OHOS::NetsysNative::DhcpResultParcel> dhcpResult = new (std::nothrow) OHOS::NetsysNative::DhcpResultParcel();
    int32_t ret = notifyCallback.OnDhcpSuccess(dhcpResult);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest011, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetsysNativeClient::NativeNotifyCallback notifyCallback(nativeClient);
    std::string limitName = "wlan";
    std::string iface = "vpncard";
    int32_t ret = notifyCallback.OnBandwidthReachedLimit(limitName, iface);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest013, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    wptr<IRemoteObject> remote = nullptr;
    nativeClient->OnRemoteDied(remote);
    int handle = 1;
    sptr<IRemoteObject> result = nullptr;
    std::u16string descriptor = std::u16string();
    result = new (std::nothrow) IPCObjectProxy(handle, descriptor);
    IRemoteObject *object = result.GetRefPtr();
    remote = object;
    nativeClient->OnRemoteDied(remote);
    uint32_t uid = 0;
    uint8_t allow = 0;
    auto ret = nativeClient->SetInternetPermission(uid, allow);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest014, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";
    auto ret = nativeClient->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = nativeClient->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest016, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";
    auto ret = nativeClient->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = nativeClient->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

#ifdef FEATURE_NET_FIREWALL_ENABLE
HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest017, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetsysNativeClient::NativeNotifyCallback notifyCallback(nativeClient);
    sptr<InterceptRecord> record = new (std::nothrow) InterceptRecord();
    sptr<NetsysNative::INetFirewallCallback> netFierwallCallback = nullptr;
    nativeClient->RegisterFirewallCallback(netFierwallCallback);
    nativeClient->UnregisterFirewallCallback(netFierwallCallback);
    netFierwallCallback = new (std::nothrow) TestNetFirewallCallbackStub;
    ASSERT_NE(netFierwallCallback, nullptr);
    nativeClient->RegisterFirewallCallback(netFierwallCallback);
    int32_t ret = notifyCallback.OnInterceptRecord(record);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    nativeClient->UnregisterFirewallCallback(netFierwallCallback);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest018, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetsysNativeClient::NativeNotifyCallback notifyCallback(nativeClient);
    sptr<InterceptRecord> record = new (std::nothrow) InterceptRecord();
    int32_t ret = notifyCallback.OnInterceptRecord(record);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}
#endif

HWTEST_F(NetsysNativeClientTest, GetCookieStatsTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint64_t stats = 0;
    BpfMapper<socket_cookie_stats_key, app_cookie_stats_value> appCookieStatsMap(APP_COOKIE_STATS_MAP_PATH, BPF_ANY);
    int32_t ret = nativeClient->GetCookieStats(stats, TEST_STATS_TYPE1, TEST_COOKIE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);
    ret = nativeClient->GetCookieStats(stats, TEST_STATS_TYPE2, TEST_COOKIE);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetsysNativeClientTest, GetNetworkSharingTypeTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::set<uint32_t> sharingTypeIsOn;
    int32_t ret = nativeClient->GetNetworkSharingType(sharingTypeIsOn);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, UpdateNetworkSharingTypeTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint64_t type = 0;
    bool isOpen = true;
    int32_t ret = nativeClient->UpdateNetworkSharingType(type, isOpen);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientBranchTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint32_t timeStep = 0;
    sptr<OHOS::NetManagerStandard::NetsysDnsReportCallback> callback = nullptr;
    int32_t ret = nativeClient->RegisterDnsResultCallback(callback, timeStep);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
    ret = nativeClient->UnregisterDnsResultCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<OHOS::NetManagerStandard::NetsysDnsQueryReportCallback> queryCallback = nullptr;
    ret = nativeClient->RegisterDnsQueryResultCallback(queryCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = nativeClient->UnregisterDnsQueryResultCallback(queryCallback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeClientTest, SetIpv6PrivacyExtensionsTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint32_t on = 0;
    std::string interface = "wlan0";
    int32_t ret = nativeClient->SetIpv6PrivacyExtensions(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = nativeClient->SetEnableIpv6(interface, on, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, SetIpv6UidBlackListTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t uid = 0;
    std::vector<int32_t> netIds = {};
    int32_t ret = nativeClient->SetIpv6UidBlackList(netIds, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, SetIpv6AutoConfTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint32_t on = 0;
    std::string interface = "wlan0";
    int32_t ret = nativeClient->SetIpv6AutoConf(interface, on);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, SetNetworkAccessPolicy001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint32_t uid = 0;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    int32_t ret = nativeClient->SetNetworkAccessPolicy(uid, netAccessPolicy, reconfirmFlag);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, NotifyNetBearerTypeChange001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::set<NetManagerStandard::NetBearType> bearerTypes;
    bearerTypes.insert(NetManagerStandard::NetBearType::BEARER_CELLULAR);
    int32_t ret = nativeClient->NotifyNetBearerTypeChange(bearerTypes);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, DeleteNetworkAccessPolicy001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint32_t uid = 0;
    int32_t ret = nativeClient->DeleteNetworkAccessPolicy(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, CloseSocketsUid001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string ipAddr = "";
    uint32_t uid = 1000;
    int32_t ret = nativeClient->CloseSocketsUid(ipAddr, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, SetBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::unordered_map<uint32_t, uint32_t> params;
    int32_t ret = nativeClient->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, SetBrokerUidAccessPolicyMapTest002, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::unordered_map<uint32_t, uint32_t> params;
    params.emplace(TEST_UID, TEST_UID);
    int32_t ret = nativeClient->SetBrokerUidAccessPolicyMap(params);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, DelBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t ret = nativeClient->DelBrokerUidAccessPolicyMap(TEST_UID);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
HWTEST_F(NetsysNativeClientTest, DisableWearableDistributedNetForward, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t ret = nativeClient->EnableWearableDistributedNetForward(8001, 8002);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = nativeClient->DisableWearableDistributedNetForward();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
#endif

HWTEST_F(NetsysNativeClientTest, EnableDistributedClientNet001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string virnicAddr = "1.189.55.61";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    int32_t ret = nativeClient->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = nativeClient->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, EnableDistributedServerNet001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";
    int32_t ret = nativeClient->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    bool isServer = true;
    std::string virnicName = "virnic";
    ret = nativeClient->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, GetNetworkCellularSharingTraffic001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    nmd::NetworkSharingTraffic traffic;
    std::string ifaceName = "virnic";

    int32_t ret = nativeClient->GetNetworkCellularSharingTraffic(traffic, ifaceName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, SetGetClearNetStateTrafficMap001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint8_t flag = 1;
    uint64_t availableTraffic = 1000000;

    int32_t ret = nativeClient->SetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = nativeClient->GetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = nativeClient->ClearIncreaseTrafficMap();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = nativeClient->DeleteIncreaseTrafficMap(12);  // 12:ifIndex
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeClientTest, ClearSimStatsBpfMap001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t ret = nativeClient->ClearSimStatsBpfMap();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, UpdateIfIndexMap001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint8_t key = 1;
    uint64_t index = 10;
    int32_t ret = nativeClient->UpdateIfIndexMap(key, index);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, RegisterNetsysTrafficCallback001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = nativeClient->RegisterNetsysTrafficCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeClientTest, StartStopClat001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string interfaceName = "eth0";
    int32_t netId = 1;
    std::string nat64PrefixStr = "2001:db8::/64";

    int32_t ret = nativeClient->StartClat(interfaceName, netId, nat64PrefixStr);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    ret = nativeClient->StopClat(interfaceName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, SetNicTrafficAllowed001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::vector<std::string> ifaceNames = {"eth0", "wlan0"};
    bool status = true;

    int32_t ret = nativeClient->SetNicTrafficAllowed(ifaceNames, status);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, NetsysNativeClientTest015, TestSize.Level1)
{
    std::unique_ptr<NetsysNativeClient> client = std::make_unique<NetsysNativeClient>();
    EXPECT_EQ(client->netsysNativeService_, nullptr);
    EXPECT_EQ(client->deathRecipient_, nullptr);
    client.reset();
}

HWTEST_F(NetsysNativeClientTest, DelInterfaceAddressTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    auto ret = nativeClient->DelInterfaceAddress(IF_NAME, IP_ADDR, PREFIX_LENGTH, 2);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNativeClientTest, OnRemoteDiedTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    wptr<IRemoteObject> remote = nullptr;
    nativeClient->OnRemoteDied(remote);
    int handle = 1;
    sptr<IRemoteObject> result = nullptr;
    std::u16string descriptor = std::u16string();
    result = new (std::nothrow) IPCObjectProxy(handle, descriptor);
    IRemoteObject *object = result.GetRefPtr();
    remote = object;
    EXPECT_EQ(nativeClient->netsysNativeService_, nullptr);
    nativeClient->netsysNativeService_ = nullptr;
    nativeClient->OnRemoteDied(remote);
}

HWTEST_F(NetsysNativeClientTest, UnRegisterNetsysTrafficCallback001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    sptr<NetsysNative::INetsysTrafficCallback> callback = nullptr;
    int32_t ret = nativeClient->UnRegisterNetsysTrafficCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeClientTest, FlushDnsCache002, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint16_t netId = 0;
    int32_t ret = nativeClient->FlushDnsCache(netId);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, SetDnsCacheTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint16_t netId = 101;
    std::string testHost = "test";
    AddrInfo info;
    int32_t ret = nativeClient->SetDnsCache(netId, testHost, info);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == 400);
}

HWTEST_F(NetsysNativeClientTest, OnDnsQueryResultReportTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetsysNativeClient::NativeNetDnsResultCallback notifyCallback(nativeClient);
    uint32_t size = 1;
    OHOS::NetsysNative::NetDnsQueryResultReport netDnsResultReport{};
    std::list<OHOS::NetsysNative::NetDnsQueryResultReport> res = {netDnsResultReport};
    int32_t ret = notifyCallback.OnDnsQueryResultReport(size, res);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, OnDnsQueryAbnormalReportTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetsysNativeClient::NativeNetDnsResultCallback notifyCallback(nativeClient);
    uint32_t eventfailcause = 1;
    OHOS::NetsysNative::NetDnsQueryResultReport netDnsResultReport{};
    int32_t ret = notifyCallback.OnDnsQueryAbnormalReport(eventfailcause, netDnsResultReport);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
HWTEST_F(NetsysNativeClientTest, UpdateEnterpriseRouteTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint32_t uid = 20000138;
    std::string ifname = "wlan0";
    bool add = true;
    auto ret = nativeClient->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysNativeClientTest, UpdateEnterpriseRouteTest002, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint32_t uid = 0;
    std::string ifname = "wlan0";
    bool add = true;
    auto ret = nativeClient->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysNativeClientTest, UpdateEnterpriseRouteTest003, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint32_t uid = 20000138;
    std::string ifname = "notexist";
    bool add = true;
    auto ret = nativeClient->UpdateEnterpriseRoute(ifname, uid, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_PARAMETER_ERROR);
}
#endif

HWTEST_F(NetsysNativeClientTest, FlushDnsCache001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    uint16_t netId = 101;
    int32_t ret = nativeClient->FlushDnsCache(netId);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, GetIpNeighTable001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::vector<NetIpMacInfo> ipMacInfo;
    int32_t ret = nativeClient->GetIpNeighTable(ipMacInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, CreateVlan001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = nativeClient->CreateVlan(ifName, vlanId);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, DestroyVlan001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = nativeClient->DestroyVlan(ifName, vlanId);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, AddVlanIp001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    int32_t ret = nativeClient->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, GetConnectOwnerUidTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    int32_t ret = nativeClient->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeClientTest, GetSystemNetPortStatesTest001, TestSize.Level1)
{
    auto nativeClient = std::make_shared<NetsysNativeClient>();
    NetPortStatesInfo netPortStatesInfo;
    int32_t ret = nativeClient->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

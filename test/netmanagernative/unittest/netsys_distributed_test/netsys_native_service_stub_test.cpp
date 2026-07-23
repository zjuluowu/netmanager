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

#ifdef GTEST_API_
#define private public
#define protected public
#endif
#include "common_net_diag_callback_test.h"
#include "common_notify_callback_test.h"
#include "i_netsys_service.h"
#include "net_dns_result_callback_stub.h"
#include "netnative_log_wrapper.h"
#include "netsys_native_service_stub.h"

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace testing::ext;
#define DTEST_LOG std::cout << __func__ << ":" << __LINE__ << ":"
} // namespace
static constexpr uint64_t TEST_COOKIE = 1;
static constexpr uint64_t TEST_UID = 1;

class TestNetsysNativeServiceStub : public NetsysNativeServiceStub {
public:
    TestNetsysNativeServiceStub() = default;
    ~TestNetsysNativeServiceStub() override{};

    int32_t SetInternetPermission(uint32_t uid, uint8_t allow, uint8_t isBroker) override
    {
        return 0;
    }

    int32_t SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                              const std::vector<std::string> &servers, const std::vector<std::string> &domains) override
    {
        return 0;
    }

    int32_t GetResolverConfig(uint16_t netId, std::vector<std::string> &servers, std::vector<std::string> &domains,
                              uint16_t &baseTimeoutMsec, uint8_t &retryCount) override
    {
        return 0;
    }

    int32_t CreateNetworkCache(uint16_t netId, bool isVpnNet) override
    {
        return 0;
    }

    int32_t DestroyNetworkCache(uint16_t netId, bool isVpnNet) override
    {
        return 0;
    }

    int32_t GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                        uint16_t netId, std::vector<AddrInfo> &res) override
    {
        return 0;
    }

    int32_t SetInterfaceMtu(const std::string &interfaceName, int mtu) override
    {
        return 0;
    }

    int32_t SetTcpBufferSizes(const std::string &tcpBufferSizes) override
    {
        return 0;
    }

    int32_t GetInterfaceMtu(const std::string &interfaceName) override
    {
        return 0;
    }

    int32_t RegisterNotifyCallback(sptr<INotifyCallback> &callback) override
    {
        return 0;
    }

    int32_t UnRegisterNotifyCallback(sptr<INotifyCallback> &callback) override
    {
        return 0;
    }

    int32_t NetworkAddRoute(int32_t netId, const std::string &interfaceName, const std::string &destination,
                            const std::string &nextHop, bool isExcludedRoute) override
    {
        return 0;
    }

    int32_t NetworkAddRoutes(int32_t netId, const std::vector<nmd::NetworkRouteInfo> &infos) override
    {
        return 0;
    }

    int32_t NetworkRemoveRoute(int32_t netId, const std::string &interfaceName, const std::string &destination,
                            const std::string &nextHop, bool isExcludedRoute) override
    {
        return 0;
    }

    int32_t NetworkAddRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo) override
    {
        return 0;
    }

    int32_t NetworkRemoveRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo) override
    {
        return 0;
    }

    int32_t NetworkSetDefault(int32_t netId) override
    {
        return 0;
    }

    int32_t NetworkGetDefault() override
    {
        return 0;
    }

    int32_t NetworkClearDefault() override
    {
        return 0;
    }

    int32_t GetProcSysNet(int32_t family, int32_t which, const std::string &ifname, const std::string &parameter,
                          std::string &value) override
    {
        return 0;
    }

    int32_t SetProcSysNet(int32_t family, int32_t which, const std::string &ifname, const std::string &parameter,
                          std::string &value) override
    {
        return 0;
    }

    int32_t NetworkCreatePhysical(int32_t netId, int32_t permission) override
    {
        return 0;
    }

    int32_t NetworkCreateVirtual(int32_t netId, bool hasDns) override
    {
        return 0;
    }

    int32_t NetworkAddUids(int32_t netId, const std::vector<UidRange> &uidRanges) override
    {
        return 0;
    }

    int32_t NetworkDelUids(int32_t netId, const std::vector<UidRange> &uidRanges) override
    {
        return 0;
    }

    int32_t AddInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                int32_t prefixLength) override
    {
        return 0;
    }

    int32_t DelInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                int32_t prefixLength) override
    {
        return 0;
    }

    int32_t DelInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                int32_t prefixLength, int socketType) override
    {
        return 0;
    }

    int32_t InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress) override
    {
        return 0;
    }

    int32_t InterfaceSetIffUp(const std::string &ifaceName) override
    {
        return 0;
    }

    int32_t NetworkAddInterface(int32_t netId, const std::string &iface, NetBearType netBearerType) override
    {
        return 0;
    }

    int32_t NetworkRemoveInterface(int32_t netId, const std::string &iface) override
    {
        return 0;
    }

    int32_t NetworkDestroy(int32_t netId, bool isVpnNet) override
    {
        return 0;
    }

    int32_t CreateVnic(uint16_t mtu, const std::string &tunAddr, int32_t prefix,
                       const std::set<int32_t> &uids) override
    {
        return 0;
    }

    int32_t DestroyVnic() override
    {
        return 0;
    }

    int32_t EnableDistributedClientNet(
        const std::string &virnicAddr, const std::string &virnicName, const std::string &iif) override
    {
        return 0;
    }

    int32_t EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                       const std::string &dstAddr, const std::string &gw) override
    {
        return 0;
    }

    int32_t DisableDistributedNet(bool isServer, const std::string &virnicName, const std::string &dstAddr) override
    {
        return 0;
    }

    int32_t GetFwmarkForNetwork(int32_t netId, MarkMaskParcel &markMaskParcel) override
    {
        return 0;
    }

    int32_t SetInterfaceConfig(const InterfaceConfigurationParcel &cfg) override
    {
        return 0;
    }

    int32_t GetInterfaceConfig(InterfaceConfigurationParcel &cfg) override
    {
        return 0;
    }

    int32_t InterfaceGetList(std::vector<std::string> &ifaces) override
    {
        return 0;
    }

    int32_t StartDhcpClient(const std::string &iface, bool bIpv6) override
    {
        return 0;
    }

    int32_t StopDhcpClient(const std::string &iface, bool bIpv6) override
    {
        return 0;
    }

    int32_t StartDhcpService(const std::string &iface, const std::string &ipv4addr) override
    {
        return 0;
    }

    int32_t StopDhcpService(const std::string &iface) override
    {
        return 0;
    }

    int32_t IpEnableForwarding(const std::string &requestor) override
    {
        return 0;
    }

    int32_t IpDisableForwarding(const std::string &requestor) override
    {
        return 0;
    }

    int32_t EnableNat(const std::string &downstreamIface, const std::string &upstreamIface) override
    {
        return 0;
    }

    int32_t DisableNat(const std::string &downstreamIface, const std::string &upstreamIface) override
    {
        return 0;
    }

    int32_t IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface) override
    {
        return 0;
    }

    int32_t IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface) override
    {
        return 0;
    }

    int32_t BandwidthAddAllowedList(uint32_t uid) override
    {
        return 0;
    }

    int32_t BandwidthRemoveAllowedList(uint32_t uid) override
    {
        return 0;
    }

    int32_t BandwidthEnableDataSaver(bool enable) override
    {
        return 0;
    }

    int32_t BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes) override
    {
        return 0;
    }

    int32_t BandwidthAddDeniedList(uint32_t uid) override
    {
        return 0;
    }

    int32_t BandwidthRemoveDeniedList(uint32_t uid) override
    {
        return 0;
    }

    int32_t BandwidthRemoveIfaceQuota(const std::string &ifName) override
    {
        return 0;
    }

    int32_t FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids) override
    {
        return 0;
    }

    int32_t FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids) override
    {
        return 0;
    }

    int32_t FirewallEnableChain(uint32_t chain, bool enable) override
    {
        return 0;
    }

    int32_t FirewallSetUidRule(uint32_t chain, const std::vector<uint32_t> &uids, uint32_t firewallRule) override
    {
        return 0;
    }

    int32_t ShareDnsSet(uint16_t netId) override
    {
        return 0;
    }

    int32_t StartDnsProxyListen() override
    {
        return 0;
    }

    int32_t StopDnsProxyListen() override
    {
        return 0;
    }

    int32_t GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                     NetworkSharingTraffic &traffic) override
    {
        return 0;
    }

    int32_t SetDpaCellularSharingTraffic(NetworkDpaTrafficReport &traffic) override
    {
        return 0;
    }

    int32_t GetNetworkCellularSharingTraffic(NetworkSharingTraffic &traffic, std::string &ifaceName) override
    {
        return 0;
    }

    int32_t GetTotalStats(uint64_t &stats, uint32_t type) override
    {
        return 0;
    }

    int32_t GetUidStats(uint64_t &stats, uint32_t type, uint32_t uid) override
    {
        return 0;
    }

    int32_t GetIfaceStats(uint64_t &stats, uint32_t type, const std::string &interfaceName) override
    {
        return 0;
    }

    int32_t GetAllSimStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats) override
    {
        return 0;
    }

    int32_t DeleteSimStatsInfo(uint32_t uid) override
    {
        return 0;
    }

    int32_t GetAllStatsInfo(std::vector<OHOS::NetManagerStandard::NetStatsInfo> &stats) override
    {
        return 0;
    }

    int32_t DeleteStatsInfo(uint32_t uid) override
    {
        return 0;
    }

    int32_t SetNetStateTrafficMap(uint8_t flag, uint64_t availableTraffic) override
    {
        return 0;
    }

    int32_t GetNetStateTrafficMap(uint8_t flag, uint64_t &availableTraffic) override
    {
        return 0;
    }

    int32_t ClearIncreaseTrafficMap() override
    {
        return 0;
    }

    int32_t DeleteIncreaseTrafficMap(uint64_t ifIndex) override
    {
        return 0;
    }

    int32_t ClearSimStatsBpfMap() override
    {
        return 0;
    }

    int32_t UpdateIfIndexMap(int8_t key, uint64_t index) override
    {
        return 0;
    }

    int32_t SetNetStatusMap(uint8_t type, uint8_t value) override
    {
        return 0;
    }

    int32_t SetIptablesCommandForRes(const std::string &cmd, std::string &respond, IptablesType ipType) override
    {
        return 0;
    }

    int32_t SetIpCommandForRes(const std::string &cmd, std::string &respond) override
    {
        return 0;
    }

    int32_t NetDiagPingHost(const NetDiagPingOption &pingOption, const sptr<INetDiagCallback> &callback) override
    {
        return 0;
    }

    int32_t NetDiagGetRouteTable(std::list<NetDiagRouteTable> &routeTables) override
    {
        return 0;
    }

    int32_t NetDiagGetSocketsInfo(NetDiagProtocolType socketType, NetDiagSocketsInfo &socketsInfo) override
    {
        return 0;
    }

    int32_t NetDiagGetInterfaceConfig(std::list<NetDiagIfaceConfig> &configs, const std::string &ifaceName) override
    {
        return 0;
    }

    int32_t NetDiagUpdateInterfaceConfig(const NetDiagIfaceConfig &config, const std::string &ifaceName,
                                         bool add) override
    {
        return 0;
    }

    int32_t NetDiagSetInterfaceActiveState(const std::string &ifaceName, bool up) override
    {
        return 0;
    }

    int32_t AddStaticArp(const std::string &ipAddr, const std::string &macAddr, const std::string &ifName) override
    {
        return 0;
    }

    int32_t DelStaticArp(const std::string &ipAddr, const std::string &macAddr, const std::string &ifName) override
    {
        return 0;
    }

    int32_t AddStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName) override
    {
        return 0;
    }

    int32_t DelStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName) override
    {
        return 0;
    }

    int32_t RegisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback, uint32_t delay) override
    {
        return 0;
    }

    int32_t UnregisterDnsResultCallback(const sptr<INetDnsResultCallback> &callback) override
    {
        return 0;
    }

    int32_t RegisterNetsysTrafficCallback(const sptr<INetsysTrafficCallback> &callback) override
    {
        return 0;
    }

    int32_t UnRegisterNetsysTrafficCallback(const sptr<INetsysTrafficCallback> &callback) override
    {
        return 0;
    }

    int32_t GetCookieStats(uint64_t &stats, uint32_t type, uint64_t cookie) override
    {
        return 0;
    }

    int32_t GetNetworkSharingType(std::set<uint32_t>& sharingTypeIsOn) override
    {
        return 0;
    }

    int32_t UpdateNetworkSharingType(uint32_t type, bool isOpen) override
    {
        return 0;
    }

#ifdef FEATURE_NET_FIREWALL_ENABLE
    int32_t SetFirewallRules(NetFirewallRuleType type, const std::vector<sptr<NetFirewallBaseRule>> &ruleList,
        bool isFinish) override
    {
        return 0;
    }

    int32_t SetFirewallDefaultAction(int32_t userId, FirewallRuleAction inDefault,
        FirewallRuleAction outDefault) override
    {
        return 0;
    }
    int32_t SetFirewallCurrentUserId(int32_t userId) override
    {
        return 0;
    }

    int32_t ClearFirewallRules(NetFirewallRuleType type) override
    {
        return 0;
    }

    int32_t RegisterNetFirewallCallback(const sptr<INetFirewallCallback> &callback) override
    {
        return 0;
    }

    int32_t UnRegisterNetFirewallCallback(const sptr<INetFirewallCallback> &callback) override
    {
        return 0;
    }
#endif

    int32_t SetIpv6PrivacyExtensions(const std::string &interfaceName, const uint32_t on) override
    {
        return 0;
    }

    int32_t SetEnableIpv6(const std::string &interfaceName, const uint32_t on, bool needRestart) override
    {
        return 0;
    }

    int32_t SetIpv6AutoConf(const std::string &interfaceName, const uint32_t on) override
    {
        return 0;
    }

    int32_t SetIpv6UidBlackList(std::vector<int32_t> &netIds, int32_t uid) override
    {
        return 0;
    }

    int32_t SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag) override
    {
        return 0;
    }

    int32_t DeleteNetworkAccessPolicy(uint32_t uid) override
    {
        return 0;
    }

    int32_t NotifyNetBearerTypeChange(std::set<NetBearType> bearerTypes) override
    {
        return 0;
    }

    int32_t StartClat(const std::string &interfaceName, int32_t netId, const std::string &nat64PrefixStr) override
    {
        return 0;
    }

    int32_t StopClat(const std::string &interfaceName) override
    {
        return 0;
    }

    int32_t ClearFirewallAllRules() override
    {
        return 0;
    }

    int32_t SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status) override
    {
        return 0;
    }

    int32_t SetBrokerUidAccessPolicyMap(const std::unordered_map<uint32_t, uint32_t> &uidMaps) override
    {
        return 0;
    }

    int32_t DelBrokerUidAccessPolicyMap(uint32_t uid) override
    {
        return 0;
    }

#ifdef SUPPORT_SYSVPN
    int32_t ProcessVpnStage(NetsysNative::SysVpnStageCode stage, const std::string &message) override
    {
        return 0;
    }

    int32_t UpdateVpnRules(uint16_t netId, const std::vector<std::string> &extMessages, bool add) override
    {
        return 0;
    }
#endif // SUPPORT_SYSVPN

    int32_t CloseSocketsUid(const std::string &ipAddr, uint32_t uid) override
    {
        return 0;
    }

    int32_t SetUserDefinedServerFlag(uint16_t netId, bool flag) override
    {
        return 0;
    }

    int32_t FlushDnsCache(uint16_t netId) override
    {
        return 0;
    }
    
    int32_t SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo) override
    {
        return 0;
    }

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
    int32_t UpdateEnterpriseRoute(const std::string &interfaceName, uint32_t uid, bool add) override
    {
        return 0;
    }
#endif
    int32_t SetInternetAccessByIpForWifiShare(
        const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName) override
    {
        return 0;
    }

    int32_t GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo) override
    {
        return 0;
    }
    
    int32_t CreateVlan(const std::string &ifName, uint32_t vlanId) override
    {
        return 0;
    }
    
    int32_t DestroyVlan(const std::string &ifName, uint32_t vlanId) override
    {
        return 0;
    }
    
    int32_t AddVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask) override
    {
        return 0;
    }

    int32_t GetConnectOwnerUid(const OHOS::NetManagerStandard::NetConnInfo &netConnInfo, int32_t &ownerUid) override
    {
        return 0;
    }

    int32_t GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo) override
    {
        return 0;
    }
};

class NetsysNativeServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetsysNativeServiceStub> notifyStub_ = nullptr;
    sptr<NetDiagCallbackStubTest> ptrCallback = new NetDiagCallbackStubTest();
};

void NetsysNativeServiceStubTest::SetUpTestCase()
{
    notifyStub_ = std::make_shared<TestNetsysNativeServiceStub>();
}

void NetsysNativeServiceStubTest::TearDownTestCase() {}

void NetsysNativeServiceStubTest::SetUp() {}

void NetsysNativeServiceStubTest::TearDown() {}


HWTEST_F(NetsysNativeServiceStubTest, CmdEnableDistributedClientNet001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    std::string virnicAddr = "1.189.55.60";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    ASSERT_NE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()), false);
    if (!data.WriteString(virnicAddr)) {
        return;
    }
    if (!data.WriteString(virnicName)) {
        return;
    }
    if (!data.WriteString(iif)) {
        return;
    }
    int32_t ret = notifyStub_->CmdEnableDistributedClientNet(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdEnableDistributedServerNet001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.60";
    std::string gw = "0.0.0.0";
    ASSERT_NE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()), false);
    if (!data.WriteString(iif)) {
        return;
    }
    if (!data.WriteString(devIface)) {
        return;
    }
    if (!data.WriteString(dstAddr)) {
        return;
    }
    if (!data.WriteString(gw)) {
        return;
    }

    int32_t ret = notifyStub_->CmdEnableDistributedServerNet(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDisableDistributedNet001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    bool isServer = true;
    std::string virnicName = "virnic";
    std::string dstAddr = "1.1.1.1";
    ASSERT_NE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()), false);

    if (!data.WriteBool(isServer)) {
        return;
    }
    if (!data.WriteString(virnicName)) {
        return;
    }
    if (!data.WriteString(dstAddr)) {
        return;
    }
    int32_t ret = notifyStub_->CmdDisableDistributedNet(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

#ifdef FEATURE_ENTERPRISE_ROUTE_CUSTOM
HWTEST_F(NetsysNativeServiceStubTest, CmdUpdateEnterpriseRoute001, TestSize.Level1)
{
    uint32_t uid = 20000138;
    std::string ifname = "wlan0";
    bool add = true;
 
    MessageParcel data;
    ASSERT_NE(data.WriteString(ifname), false);
 
    if (!data.WriteUint32(uid)) {
        return;
    }
 
    if (!data.WriteBool(add)) {
        return;
    }
 
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdUpdateEnterpriseRoute(data, reply);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysNativeServiceStubTest, CmdUpdateEnterpriseRoute002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdUpdateEnterpriseRoute(data, reply);
    EXPECT_EQ(ret, ERR_FLATTEN_OBJECT);
}
 
HWTEST_F(NetsysNativeServiceStubTest, CmdUpdateEnterpriseRoute003, TestSize.Level1)
{
    std::string ifname = "wlan0";
 
    MessageParcel data;
    ASSERT_NE(data.WriteString(ifname), false);
 
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdUpdateEnterpriseRoute(data, reply);
    EXPECT_EQ(ret, ERR_FLATTEN_OBJECT);
}
 
HWTEST_F(NetsysNativeServiceStubTest, CmdUpdateEnterpriseRoute004, TestSize.Level1)
{
    uint32_t uid = 20000138;
    std::string ifname = "wlan0";
 
    MessageParcel data;
    ASSERT_NE(data.WriteString(ifname), false);
 
    if (!data.WriteUint32(uid)) {
        return;
    }
 
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdUpdateEnterpriseRoute(data, reply);
    EXPECT_EQ(ret, ERR_FLATTEN_OBJECT);
}
#endif

HWTEST_F(NetsysNativeServiceStubTest, CmdSetIpv6UidBlackList001, TestSize.Level1)
{
    uint32_t uid = 20000138;
    std::vector<int32_t> netIds = {1, 2, 3};
 
    MessageParcel data;
    ASSERT_NE(data.WriteInt32(netIds.size()), false);
 
    for (const int32_t& iter : netIds) {
        if (!data.WriteInt32(iter)) {
            return;
        }
    }
 
    if (!data.WriteInt32(uid)) {
        return;
    }
 
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetIpv6UidBlackList(data, reply);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
 
HWTEST_F(NetsysNativeServiceStubTest, CmdSetIpv6UidBlackList002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetIpv6UidBlackList(data, reply);
    EXPECT_EQ(ret, ERR_FLATTEN_OBJECT);
}
 
HWTEST_F(NetsysNativeServiceStubTest, CmdSetIpv6UidBlackList003, TestSize.Level1)
{
    std::vector<int32_t> netIds = {1, 2, 3};
 
    MessageParcel data;
    ASSERT_NE(data.WriteInt32(netIds.size()), false);
 
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetIpv6UidBlackList(data, reply);
    EXPECT_EQ(ret, ERR_FLATTEN_OBJECT);
}
 
HWTEST_F(NetsysNativeServiceStubTest, CmdSetIpv6UidBlackList004, TestSize.Level1)
{
    uint32_t uid = 20000138;
    std::vector<int32_t> netIds = {0, 1, 2, 3};
 
    MessageParcel data;
    ASSERT_NE(data.WriteInt32(netIds.size()), false);
 
    for (const int32_t& iter : netIds) {
        if (!data.WriteInt32(iter)) {
            return;
        }
    }
 
    if (!data.WriteInt32(uid)) {
        return;
    }
 
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetIpv6UidBlackList(data, reply);
    EXPECT_EQ(ret, ERR_FLATTEN_OBJECT);
}
} // namespace NetsysNative
} // namespace OHOS

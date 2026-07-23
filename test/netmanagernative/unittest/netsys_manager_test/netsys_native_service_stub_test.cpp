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
#include "mock_netsys_native_service_stub.h"

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace testing::ext;
#define DTEST_LOG std::cout << __func__ << ":" << __LINE__ << ":"
} // namespace
static constexpr uint64_t TEST_COOKIE = 1;
static constexpr uint64_t TEST_UID = 1;
static constexpr uint32_t TEST_UID_U32 = 1;

class TestNetDnsResultCallback : public NetDnsResultCallbackStub {
public:
    TestNetDnsResultCallback() = default;
    ~TestNetDnsResultCallback() override{};

    int32_t OnDnsResultReport(uint32_t size, const std::list<NetDnsResultReport>) override
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

HWTEST_F(NetsysNativeServiceStubTest, CmdSetResolverConfig001, TestSize.Level1)
{
    uint16_t netId = 1001;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    int32_t vServerSize = 2;
    std::string strServer = "TestServer";

    int32_t vDomainSize = 1;
    std::string strDomain = "TestDomain";

    MessageParcel data;
    EXPECT_TRUE(data.WriteUint16(netId));
    EXPECT_TRUE(data.WriteUint16(baseTimeoutMsec));
    EXPECT_TRUE(data.WriteUint16(retryCount));
    EXPECT_TRUE(data.WriteUint32(vServerSize));
    EXPECT_TRUE(data.WriteString(strServer));
    EXPECT_TRUE(data.WriteUint32(vDomainSize));
    EXPECT_TRUE(data.WriteString(strDomain));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetResolverConfig(data, reply);
    EXPECT_EQ(ret, ERR_FLATTEN_OBJECT);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdSetResolverConfig002, TestSize.Level1)
{
    uint16_t netId = 1001;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    int32_t vServerSize = 0;
    int32_t vDomainSize = 0;
    MessageParcel data;
    EXPECT_TRUE(data.WriteUint16(netId));
    EXPECT_TRUE(data.WriteUint16(baseTimeoutMsec));
    EXPECT_TRUE(data.WriteUint8(retryCount));
    EXPECT_TRUE(data.WriteUint32(vServerSize));
    EXPECT_TRUE(data.WriteUint32(vDomainSize));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetResolverConfig(data, reply);
    DTEST_LOG << "CmdSetResolverConfig002" << ret << std::endl;
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdSetResolverConfig003, TestSize.Level1)
{
    uint16_t netId = 1001;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    int32_t vServerSize = 2;
    std::string server = "testserver";
    int32_t vDomainSize = 2;
    std::string domain = "testdomain";
    MessageParcel data;
    EXPECT_TRUE(data.WriteUint16(netId));
    EXPECT_TRUE(data.WriteUint16(baseTimeoutMsec));
    EXPECT_TRUE(data.WriteUint8(retryCount));
    EXPECT_TRUE(data.WriteUint32(vServerSize));
    for (int32_t i = 0; i < vServerSize; i++) {
        EXPECT_TRUE(data.WriteString(server));
    }

    EXPECT_TRUE(data.WriteUint32(vDomainSize));
    for (int32_t i = 0; i < vDomainSize; i++) {
        EXPECT_TRUE(data.WriteString(domain));
    }
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetResolverConfig(data, reply);
    DTEST_LOG << "CmdSetResolverConfig003" << ret << std::endl;
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetResolverConfig001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    uint16_t netId = 1001;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint16(netId));
    int32_t ret = notifyStub_->CmdGetResolverConfig(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdCreateNetworkCache001, TestSize.Level1)
{
    MessageParcel data;
    uint16_t netId = 1001;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint16(netId));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdCreateNetworkCache(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDestroyNetworkCache001, TestSize.Level1)
{
    MessageParcel data;
    uint16_t netId = 1001;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint16(netId));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDestroyNetworkCache(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetAddrInfo001, TestSize.Level1)
{
    std::string hostName = "TestHostName";
    std::string serverName = "TestServerName";

    struct AddrInfo addrInfo;
    addrInfo.aiFlags = 0;
    addrInfo.aiFamily = AF_INET;

    uint16_t netId = 1001;

    MessageParcel data;
    EXPECT_TRUE(data.WriteString(hostName));
    EXPECT_TRUE(data.WriteString(serverName));
    EXPECT_TRUE(data.WriteRawData(&addrInfo, sizeof(AddrInfo)));
    EXPECT_TRUE(data.WriteUint16(netId));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetAddrInfo(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetAddrInfo002, TestSize.Level1)
{
    std::string hostName = "TestHostName";
    std::string serverName = "TestServerName";

    struct AddrInfo addrInfo;
    addrInfo.aiFlags = 0;
    addrInfo.aiFamily = 9999;
    uint16_t netId = 1001;

    MessageParcel data;
    EXPECT_TRUE(data.WriteString(hostName));
    EXPECT_TRUE(data.WriteString(serverName));
    EXPECT_TRUE(data.WriteRawData(&addrInfo, sizeof(AddrInfo)));
    EXPECT_TRUE(data.WriteUint16(netId));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetAddrInfo(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdSetInterfaceMtu001, TestSize.Level1)
{
    std::string ifName = "ifName";
    int32_t mtu = 0;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(ifName));
    EXPECT_TRUE(data.WriteUint32(mtu));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetInterfaceMtu(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetInterfaceMtu001, TestSize.Level1)
{
    std::string ifName = "ifName";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(ifName));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetInterfaceMtu(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdRegisterNotifyCallback001, TestSize.Level1)
{
    std::string ifName = "ifName";
    sptr<INotifyCallback> callback = new (std::nothrow) NotifyCallbackTest();
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteRemoteObject(callback->AsObject().GetRefPtr()));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdRegisterNotifyCallback(data, reply);
    EXPECT_EQ(ret, -1);
    MessageParcel data2;
    EXPECT_TRUE(data2.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data2.WriteRemoteObject(callback->AsObject().GetRefPtr()));
    ret = notifyStub_->CmdUnRegisterNotifyCallback(data2, reply);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdRegisterNotifyCallback002, TestSize.Level1)
{
    std::string ifName = "ifName";
    sptr<INotifyCallback> callback = new (std::nothrow) NotifyCallbackTest();
    MessageParcel data;
    EXPECT_TRUE(data.WriteRemoteObject(callback->AsObject().GetRefPtr()));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdRegisterNotifyCallback(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
    MessageParcel data2;
    EXPECT_TRUE(data2.WriteRemoteObject(callback->AsObject().GetRefPtr()));
    ret = notifyStub_->CmdUnRegisterNotifyCallback(data2, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetworkRouteParcel001, TestSize.Level1)
{
    uint16_t netId = 1001;
    std::string ifName = "ifName";
    std::string destination = "destination";
    std::string nextHop = "nextHop";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint16(netId));
    EXPECT_TRUE(data.WriteString(ifName));
    EXPECT_TRUE(data.WriteString(destination));
    EXPECT_TRUE(data.WriteString(nextHop));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdNetworkAddRouteParcel(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdNetworkRemoveRouteParcel(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetworkDefault001, TestSize.Level1)
{
    uint16_t netId = 1001;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint16(netId));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdNetworkSetDefault(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdNetworkGetDefault(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdNetworkClearDefault(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdProcSysNet001, TestSize.Level1)
{
    int32_t family = 0;
    int32_t which = 0;
    std::string ifName = "TestIfName";
    std::string parameter = "TestParameter";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(family));
    EXPECT_TRUE(data.WriteUint32(which));
    EXPECT_TRUE(data.WriteString(ifName));
    EXPECT_TRUE(data.WriteString(parameter));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetProcSysNet(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    std::string value = "TestValue";
    EXPECT_TRUE(data.WriteString(value));

    ret = notifyStub_->CmdSetProcSysNet(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetworkCreatePhysical001, TestSize.Level1)
{
    int32_t netId = 1001;
    int32_t permission = 0;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(netId));
    EXPECT_TRUE(data.WriteUint32(permission));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdNetworkCreatePhysical(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdInterfaceAddress001, TestSize.Level1)
{
    std::string interfaceName = "testInterfaceName";
    std::string ipAddr = "testIpAddr";
    int32_t prefixLength = 10;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(interfaceName));
    EXPECT_TRUE(data.WriteString(ipAddr));
    EXPECT_TRUE(data.WriteUint32(prefixLength));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdAddInterfaceAddress(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdDelInterfaceAddress(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdInterfaceSetIpAddress001, TestSize.Level1)
{
    std::string interfaceName = "testInterfaceName";
    std::string ipAddress = "testIpAddr";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(interfaceName));
    EXPECT_TRUE(data.WriteString(ipAddress));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdInterfaceSetIpAddress(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdInterfaceSetIffUp001, TestSize.Level1)
{
    std::string interfaceName = "testInterfaceName";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(interfaceName));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdInterfaceSetIffUp(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetworkInterface001, TestSize.Level1)
{
    int32_t netId = 1001;
    std::string interfaceName = "testInterfaceName";
    NetBearType bearerType = BEARER_DEFAULT;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(interfaceName));
    EXPECT_TRUE(data.WriteUint32(netId));
    EXPECT_TRUE(data.WriteUint8(bearerType));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdNetworkAddInterface(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    notifyStub_->CmdNetworkRemoveInterface(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetworkDestroy001, TestSize.Level1)
{
    int32_t netId = 1001;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(netId));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdNetworkDestroy(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetFwmarkForNetwork001, TestSize.Level1)
{
    int32_t netId = 1001;
    int32_t mark = 0;
    int32_t mask = 0;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(netId));
    EXPECT_TRUE(data.WriteUint32(mark));
    EXPECT_TRUE(data.WriteUint32(mask));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetFwmarkForNetwork(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdInterfaceConfig001, TestSize.Level1)
{
    std::string ifName = "testIfName";
    std::string hwAddr = "testHwAddr";
    std::string ipv4Addr = "testIpv4Addr";
    uint32_t prefixLength = 0;
    uint32_t vServerSize = 1;
    std::string flag = "testFlag";

    MessageParcel data;
    EXPECT_TRUE(data.WriteString(ifName));
    EXPECT_TRUE(data.WriteString(hwAddr));
    EXPECT_TRUE(data.WriteString(ipv4Addr));
    EXPECT_TRUE(data.WriteUint32(prefixLength));
    EXPECT_TRUE(data.WriteUint32(vServerSize));
    EXPECT_TRUE(data.WriteString(flag));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetInterfaceConfig(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdGetInterfaceConfig(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdInterfaceGetList001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdInterfaceGetList(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDhcpClient001, TestSize.Level1)
{
    std::string iface = "testIface";
    bool bIpv6 = true;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(iface));
    EXPECT_TRUE(data.WriteBool(bIpv6));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdStartDhcpClient(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdStopDhcpClient(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDhcpService001, TestSize.Level1)
{
    std::string iface = "testIface";
    std::string ipv4addr = "testIpv4addr";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(iface));
    EXPECT_TRUE(data.WriteString(ipv4addr));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdStartDhcpService(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdStopDhcpService(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdIpForwarding001, TestSize.Level1)
{
    std::string requester = "testRequester";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(requester));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdIpEnableForwarding(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdIpDisableForwarding(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNat001, TestSize.Level1)
{
    std::string downstreamIface = "testDownstreamIface";
    std::string upstreamIface = "testUpstreamIface";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(downstreamIface));
    EXPECT_TRUE(data.WriteString(upstreamIface));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdEnableNat(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdDisableNat(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdIpfwdInterfaceForward001, TestSize.Level1)
{
    std::string fromIface = "testFromIface";
    std::string toIface = "testToIface";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(fromIface));
    EXPECT_TRUE(data.WriteString(toIface));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdIpfwdAddInterfaceForward(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdIpfwdRemoveInterfaceForward(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdBandwidthEnableDataSaver001, TestSize.Level1)
{
    bool enable = true;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteBool(enable));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdBandwidthEnableDataSaver(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdBandwidthIfaceQuota001, TestSize.Level1)
{
    std::string ifName = "testIfName";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(ifName));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdBandwidthSetIfaceQuota(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdBandwidthRemoveIfaceQuota(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdBandwidthList001, TestSize.Level1)
{
    uint32_t uid = 1001;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(uid));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdBandwidthAddDeniedList(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdBandwidthRemoveDeniedList(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdBandwidthAddAllowedList(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdBandwidthRemoveAllowedList(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdFirewallSetUidsListChain001, TestSize.Level1)
{
    uint32_t chain = 0;
    uint32_t uidSize = 1;
    uint32_t uid = 1001;

    MessageParcel data;
    EXPECT_TRUE(data.WriteUint32(chain));
    EXPECT_TRUE(data.WriteUint32(uidSize));
    EXPECT_TRUE(data.WriteUint32(uid));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdFirewallSetUidsAllowedListChain(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdFirewallSetUidsDeniedListChain(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdFirewallEnableChain001, TestSize.Level1)
{
    uint32_t chain = 0;
    bool enable = true;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(chain));
    EXPECT_TRUE(data.WriteBool(enable));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdFirewallEnableChain(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdFirewallSetUidRule001, TestSize.Level1)
{
    uint32_t chain = 0;
    uint32_t uid = 1001;
    uint32_t firewallRule = 1;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(chain));
    EXPECT_TRUE(data.WriteUint32(uid));
    EXPECT_TRUE(data.WriteUint32(firewallRule));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdFirewallSetUidRule(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdClearFirewallAllRules001, TestSize.Level1)
{
    uint32_t chain = 0;
    bool enable = true;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdClearFirewallAllRules(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdShareDnsSet001, TestSize.Level1)
{
    uint16_t netId = 0;

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint16(netId));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdShareDnsSet(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDnsProxyListen001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdStartDnsProxyListen(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdStopDnsProxyListen(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetNetworkSharingTraffic001, TestSize.Level1)
{
    std::string downIface = "testDownIface";
    std::string upIface = "testUpIface ";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(downIface));
    EXPECT_TRUE(data.WriteString(upIface));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetNetworkSharingTraffic(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetTotalStats001, TestSize.Level1)
{
    uint32_t type = 0;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteInt32(type));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetTotalStats(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetUidStats001, TestSize.Level1)
{
    uint32_t type = 0;
    uint32_t uId = 2020;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteInt32(type));
    EXPECT_TRUE(data.WriteInt32(uId));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetUidStats(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetIfaceStats001, TestSize.Level1)
{
    uint32_t type = 0;
    std::string Iface = "wlan0";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteInt32(type));
    EXPECT_TRUE(data.WriteString(Iface));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetIfaceStats(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetAllStatsInfo001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetAllStatsInfo(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDeleteStatsInfoTest001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteUint32(TEST_UID));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDeleteStatsInfo(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetAllSimStatsInfoTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetAllSimStatsInfo(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDeleteSimStatsInfoTest001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteUint32(TEST_UID));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDeleteSimStatsInfo(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, NetsysFreeAddrinfoTest001, TestSize.Level1)
{
    addrinfo *ai = nullptr;
    int32_t ret = notifyStub_->NetsysFreeAddrinfo(ai);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdSetInternetPermissionTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    uint32_t uid = 0;
    uint8_t allow = 1;
    ASSERT_TRUE(data.WriteUint32(uid));
    ASSERT_TRUE(data.WriteUint8(allow));
    int32_t ret = notifyStub_->CmdSetInternetPermission(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetworkCreateVirtualTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(0);
    data.WriteBool(false);
    int32_t ret = notifyStub_->CmdNetworkCreateVirtual(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetworkAddUidsTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(0);
    data.WriteInt32(0);
    int32_t ret = notifyStub_->CmdNetworkAddUids(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetworkDelUidsTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(0);
    data.WriteInt32(0);
    int32_t ret = notifyStub_->CmdNetworkDelUids(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdSetIptablesCommandForResTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetIptablesCommandForRes(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdSetIpCommandForResTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetIpCommandForRes(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, OnRemoteRequestTest001, TestSize.Level1)
{
    uint32_t errcode = 9999;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto result = notifyStub_->OnRemoteRequest(errcode, data, reply, option);
    EXPECT_EQ(result, IPC_STUB_UNKNOW_TRANS_ERR);
    uint32_t code = 10;
    result = notifyStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(result, IPC_STUB_INVALID_DATA_ERR);
    auto descriptor = NetsysNativeServiceStub::GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    code = 8;
    result = notifyStub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(result, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetDiagPingHostCommandForResTest001, TestSize.Level1)
{
    NetDiagPingOption pingOption;
    MessageParcel data;

    pingOption.Marshalling(data);
    MessageParcel reply;

    int32_t ret = notifyStub_->CmdNetDiagPingHost(data, reply);
    EXPECT_EQ(ret, IPC_STUB_ERR);

    pingOption.Marshalling(data);
    data.WriteRemoteObject(ptrCallback->AsObject().GetRefPtr());
    ret = notifyStub_->CmdNetDiagPingHost(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetDiagGetRouteTableCommandForResTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    int32_t ret = notifyStub_->CmdNetDiagGetRouteTable(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetDiagGetSocketsInfoCommandForResTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteUint8(NetDiagProtocolType::PROTOCOL_TYPE_ALL);
    int32_t ret = notifyStub_->CmdNetDiagGetSocketsInfo(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    data.WriteUint8(NetDiagProtocolType::PROTOCOL_TYPE_TCP);
    ret = notifyStub_->CmdNetDiagGetSocketsInfo(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    data.WriteUint8(NetDiagProtocolType::PROTOCOL_TYPE_UDP);
    ret = notifyStub_->CmdNetDiagGetSocketsInfo(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    data.WriteUint8(NetDiagProtocolType::PROTOCOL_TYPE_UNIX);
    ret = notifyStub_->CmdNetDiagGetSocketsInfo(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    data.WriteUint8(NetDiagProtocolType::PROTOCOL_TYPE_RAW);
    ret = notifyStub_->CmdNetDiagGetSocketsInfo(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetDiagSetInterfaceActiveStateCommandForResTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("eth0");
    data.WriteBool(true);
    int32_t ret = notifyStub_->CmdNetDiagSetInterfaceActiveState(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    data.WriteString("eth0");
    data.WriteBool(false);
    ret = notifyStub_->CmdNetDiagSetInterfaceActiveState(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetDiagUpdateInterfaceConfigCommandForResTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    NetDiagIfaceConfig config;
    config.ifaceName_ = "eth0";
    const std::string ifaceName = "eth0";
    config.ipv4Addr_ = "192.168.222.234";
    config.mtu_ = 1000;
    config.ipv4Mask_ = "255.255.255.0";
    config.ipv4Bcast_ = "255.255.255.0";
    config.txQueueLen_ = 1000;
    config.Marshalling(data);
    data.WriteString("eth0");
    data.WriteBool(true);
    int32_t ret = notifyStub_->CmdNetDiagUpdateInterfaceConfig(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNetDiagGetInterfaceConfigCommandForResTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("eth0");
    int32_t ret = notifyStub_->CmdNetDiagGetInterfaceConfig(data, reply);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    MessageParcel data1;
    MessageParcel reply1;
    data1.WriteString("eth1");
    int32_t ret1 = notifyStub_->CmdNetDiagGetInterfaceConfig(data1, reply1);
    EXPECT_EQ(ret1, NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdAddStaticArp001, TestSize.Level1)
{
    std::string ipAddr = "192.168.1.100";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "wlan0";

    MessageParcel data;
    EXPECT_TRUE(data.WriteString(ipAddr));
    EXPECT_TRUE(data.WriteString(macAddr));
    EXPECT_TRUE(data.WriteString(ifName));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdAddStaticArp(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDelStaticArp001, TestSize.Level1)
{
    std::string ipAddrTest = "192.168.1.100";
    std::string macAddrTest = "aa:bb:cc:dd:ee:ff";
    std::string ifNameTest = "wlan0";

    MessageParcel data;
    EXPECT_TRUE(data.WriteString(ipAddrTest));
    EXPECT_TRUE(data.WriteString(ifNameTest));
    EXPECT_TRUE(data.WriteString(ifNameTest));

    MessageParcel reply;
    auto ret = notifyStub_->CmdDelStaticArp(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdAddStaticIpv6Addr001, TestSize.Level1)
{
    std::string ipAddr = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddr = "aa:bb:cc:dd:ee:ff";
    std::string ifName = "chba0";

    MessageParcel data;
    EXPECT_TRUE(data.WriteString(ipAddr));
    EXPECT_TRUE(data.WriteString(macAddr));
    EXPECT_TRUE(data.WriteString(ifName));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdAddStaticIpv6Addr(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDelStaticIpv6Addr001, TestSize.Level1)
{
    std::string ipAddrTest = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    std::string macAddrTest = "aa:bb:cc:dd:ee:ff";
    std::string ifNameTest = "chba0";

    MessageParcel data;
    EXPECT_TRUE(data.WriteString(ipAddrTest));
    EXPECT_TRUE(data.WriteString(ifNameTest));
    EXPECT_TRUE(data.WriteString(ifNameTest));

    MessageParcel reply;
    auto ret = notifyStub_->CmdDelStaticIpv6Addr(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetCookieStats001, TestSize.Level1)
{
    uint32_t type = 0;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(type));
    EXPECT_TRUE(data.WriteUint64(TEST_COOKIE));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetCookieStats(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdRegisterDnsResultListener001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    sptr<INetDnsResultCallback> callback = new (std::nothrow) TestNetDnsResultCallback();
    EXPECT_TRUE(data.WriteRemoteObject(callback->AsObject().GetRefPtr()));
    uint32_t timeStep = 1;
    EXPECT_TRUE(data.WriteUint32(timeStep));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdRegisterDnsResultListener(data, reply);
    EXPECT_EQ(ret, IPC_STUB_ERR);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdUnregisterDnsResultListener001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    sptr<INetDnsResultCallback> callback = new (std::nothrow) TestNetDnsResultCallback();
    EXPECT_TRUE(data.WriteRemoteObject(callback->AsObject().GetRefPtr()));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdUnregisterDnsResultListener(data, reply);
    EXPECT_EQ(ret, IPC_STUB_ERR);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetNetworkSharingType001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetNetworkSharingType(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdUpdateNetworkSharingType001, TestSize.Level1)
{
    uint32_t type = 0;
    bool isOpen = true;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(type));
    EXPECT_TRUE(data.WriteBool(isOpen));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdUpdateNetworkSharingType(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdSetIpv6PrivacyExtensions001, TestSize.Level1)
{
    std::string interface = "wlan0";

    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteString(interface));
    EXPECT_TRUE(data.WriteUint32(0));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetIpv6PrivacyExtensions(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdSetIpv6Enable(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    ret = notifyStub_->CmdSetIpv6AutoConf(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdSetNetworkAccessPolicy001, TestSize.Level1)
{
    uint32_t uid = 0;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(uid));
    EXPECT_TRUE(data.WriteUint8(netAccessPolicy.wifiAllow));
    EXPECT_TRUE(data.WriteUint8(netAccessPolicy.cellularAllow));
    EXPECT_TRUE(data.WriteBool(reconfirmFlag));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetNetworkAccessPolicy(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDeleteNetworkAccessPolicy001, TestSize.Level1)
{
    uint32_t uid = 0;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint32(uid));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDelNetworkAccessPolicy(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdNotifyNetBearerTypeChange001, TestSize.Level1)
{
    std::set<NetManagerStandard::NetBearType> bearerTypes;
    bearerTypes.clear();
    bearerTypes.insert(NetManagerStandard::NetBearType::BEARER_CELLULAR);
    MessageParcel data;

    uint32_t size = static_cast<uint32_t>(bearerTypes.size());
    EXPECT_TRUE(data.WriteUint32(size));

    for (auto bearerType : bearerTypes) {
        EXPECT_TRUE(data.WriteUint32(static_cast<uint32_t>(bearerType)));
    }

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdNotifyNetBearerTypeChange(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdCreateVnic001, TestSize.Level1)
{
    MessageParcel data;
    uint16_t mtu = 1500;
    std::string tunAddr = "192.168.1.100";
    int32_t prefix = 24;
    std::set<int32_t> uids;

    EXPECT_TRUE(data.WriteUint16(mtu));
    EXPECT_TRUE(data.WriteString(tunAddr));
    EXPECT_TRUE(data.WriteInt32(prefix));
    EXPECT_TRUE(data.WriteInt32(uids.size()));
 
    for (const auto &uid: uids) {
        EXPECT_TRUE(data.WriteInt32(uid));
    }

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdCreateVnic(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDestroyVnic001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDestroyVnic(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdCloseSocketsUid001, TestSize.Level1)
{
    MessageParcel data;
    std::string ipAddr = "192.168.1.100";
    uint32_t netId = 24;

    EXPECT_TRUE(data.WriteString(ipAddr));
    EXPECT_TRUE(data.WriteUint32(netId));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdCloseSocketsUid(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, SetBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    MessageParcel data;
    std::unordered_map<uint32_t, uint32_t> params;
    params.emplace(TEST_UID_U32, TEST_UID_U32);

    uint32_t count = static_cast<uint32_t>(params.size());
    EXPECT_TRUE(data.WriteUint32(count));
    for (auto iter = params.begin(); iter != params.end(); iter++) {
        EXPECT_TRUE(data.WriteUint32(iter->first));
        EXPECT_TRUE(data.WriteUint32(iter->second));
    }

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetBrokerUidAccessPolicyMap(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, DelBrokerUidAccessPolicyMapTest001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteUint32(TEST_UID_U32));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDelBrokerUidAccessPolicyMap(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

#ifdef FEATURE_WEARABLE_DISTRIBUTED_NET_ENABLE
HWTEST_F(NetsysNativeServiceStubTest, CmdEnableWearableDistributedNetForward, TestSize.Level1)
{
    uint32_t type = 0;
    uint32_t uId = 2020;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteInt32(type));
    EXPECT_TRUE(data.WriteInt32(uId));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdEnableWearableDistributedNetForward(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDisableWearableDistributedNetForward, TestSize.Level1)
{
    uint32_t type = 0;
    uint32_t uId = 2020;
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteInt32(type));
    EXPECT_TRUE(data.WriteInt32(uId));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDisableWearableDistributedNetForward(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}
#endif

HWTEST_F(NetsysNativeServiceStubTest, CmdSetNetStatusMapTest001, TestSize.Level1)
{
    uint8_t type = 0;
    uint8_t value = 0;
    MessageParcel data1;
    if (!data1.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor())) {
        return;
    }
    if (!data1.WriteUint8(type)) {
        return;
    }
    if (!data1.WriteUint8(value)) {
        return;
    }

    MessageParcel reply1;
    int32_t ret1 = notifyStub_->CmdSetNetStatusMap(data1, reply1);
    EXPECT_EQ(ret1, ERR_NONE);

    MessageParcel data2;
    uint32_t type2 = 1028;
    if (!data2.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor())) {
        return;
    }
    if (!data2.WriteUint32(type2)) {
        return;
    }
    if (!data2.WriteUint8(value)) {
        return;
    }

    MessageParcel reply2;
    int32_t ret2 = notifyStub_->CmdSetNetStatusMap(data2, reply2);
    EXPECT_NE(ret2, ERR_FLATTEN_OBJECT);

    MessageParcel data3;
    uint32_t value2 = 1028;
    if (!data3.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor())) {
        return;
    }
    if (!data3.WriteUint32(type)) {
        return;
    }
    if (!data3.WriteUint8(value2)) {
        return;
    }

    MessageParcel reply3;
    int32_t ret3 = notifyStub_->CmdSetNetStatusMap(data3, reply3);
}

HWTEST_F(NetsysNativeServiceStubTest, FlushDnsCache001, TestSize.Level1)
{
    uint16_t netId = 1001;
    MessageParcel data;
    EXPECT_TRUE(data.WriteUint16(netId));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdFlushDnsCache(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDeleteIncreaseTrafficMap001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint64(0));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDeleteIncreaseTrafficMap(data, reply);
    EXPECT_EQ(ret, ERR_NONE);

    MessageParcel data2;
    EXPECT_TRUE(data2.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data2.WriteUint8(0));
    MessageParcel reply2;
    int32_t ret2 = notifyStub_->CmdDeleteIncreaseTrafficMap(data2, reply2);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdClearSimStatsBpfMap001, TestSize.Level1)
{
    MessageParcel data;
    EXPECT_TRUE(data.WriteInterfaceToken(NetsysNativeServiceStub::GetDescriptor()));
    EXPECT_TRUE(data.WriteUint64(0));
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdClearSimStatsBpfMap(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, SetDnsCache001, TestSize.Level1)
{
    uint16_t netId = 101;
    std::string testHost = "test";
    AddrInfo info;
    MessageParcel data;
    EXPECT_TRUE(data.WriteUint32(netId));
    EXPECT_TRUE(data.WriteString(testHost));
    EXPECT_TRUE(data.WriteRawData(&info, sizeof(AddrInfo)));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetDnsCache(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, SetInternetAccessByIpForWifiShare001, TestSize.Level1)
{
    std::string iptest = "1.1.1.1";
    uint8_t family = 2;
    bool access = true;
    std::string ifname = "test";

    MessageParcel data;
    EXPECT_TRUE(data.WriteString(iptest));
    EXPECT_TRUE(data.WriteUint8(family));
    EXPECT_TRUE(data.WriteBool(access));
    EXPECT_TRUE(data.WriteString(ifname));

    MessageParcel reply;
    int32_t ret = notifyStub_->CmdSetInternetAccessByIpForWifiShare(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
    
    MessageParcel errData;
    ret = notifyStub_->CmdSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_EQ(ret, 3);

    errData.WriteString(iptest);
    ret = notifyStub_->CmdSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_EQ(ret, 3);

    errData.WriteUint8(family);
    ret = notifyStub_->CmdSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_EQ(ret, 3);

    errData.WriteBool(access);
    ret = notifyStub_->CmdSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_EQ(ret, 3);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetIpNeighTable001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetIpNeighTable(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdCreateVlan001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdCreateVlan(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdDestroyVlan001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdDestroyVlan(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdAddVlanIp001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdAddVlanIp(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetConnectOwnerUid001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetConnectOwnerUid(data, reply);
    EXPECT_EQ(ret, IPC_STUB_ERR);

    data.WriteInt32(IPPROTO_TCP);
    data.WriteUint32(static_cast<uint32_t>(NetConnInfo::Family::IPv4));
    data.WriteString("192.168.1.100");
    data.WriteUint16(1111);
    data.WriteString("192.168.1.200");
    data.WriteUint16(2222);
    ret = notifyStub_->CmdGetConnectOwnerUid(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(NetsysNativeServiceStubTest, CmdGetSystemNetPortStatesTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = notifyStub_->CmdGetSystemNetPortStates(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

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
    EXPECT_EQ(ret, ERR_NONE);
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

HWTEST_F(NetsysNativeServiceStubTest, CmdSetDpaCellularSharingTraffic001, TestSize.Level1)
{
    uint32_t srcIndex = 1;
    uint32_t dstIndex = 2;
    uint32_t pkts = 1234;
    uint32_t bytes = 5678;

    MessageParcel data;
    MessageParcel reply;

    if (!data.WriteUint32(srcIndex)) {
        return;
    }
    if (!data.WriteUint32(dstIndex)) {
        return;
    }
    if (!data.WriteUint32(pkts)) {
        return;
    }
    if (!data.WriteUint32(bytes)) {
        return;
    }

    int32_t ret = notifyStub_->CmdSetDpaCellularSharingTraffic(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}
} // namespace NetsysNative
} // namespace OHOS

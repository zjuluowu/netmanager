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
#endif
#include "mock_netsys_native_client.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class MockNetsysNativeClientTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
    static inline MockNetsysNativeClient nativeClient_;
};

void MockNetsysNativeClientTest::SetUpTestCase() {}

void MockNetsysNativeClientTest::TearDownTestCase() {}

void MockNetsysNativeClientTest::SetUp() {}

void MockNetsysNativeClientTest::TearDown() {}

HWTEST_F(MockNetsysNativeClientTest, MockNetsysNativeClientBranchTest001, TestSize.Level1)
{
    nativeClient_.Init();
    nativeClient_.RegisterMockApi();
    EXPECT_TRUE(nativeClient_.CheckMockApi(MOCK_INTERFACECLEARADDRS_API));

    int32_t netId = 0;
    int32_t ret = nativeClient_.NetworkDestroy(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string iface = "";
    ret = nativeClient_.NetworkAddInterface(netId, iface, BEARER_DEFAULT);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.NetworkRemoveInterface(netId, iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.NetworkAddRoute(netId, iface, "", "");
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<nmd::NetworkRouteInfo> infos;
    infos.resize(1);
    ret = nativeClient_.NetworkAddRoutes(netId, infos);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.SetIpv6PrivacyExtensions(iface, 1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.SetEnableIpv6(iface, 1, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.NetworkRemoveRoute(netId, iface, "", "");
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.SetInterfaceDown(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.SetInterfaceUp(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    nativeClient_.ClearInterfaceAddrs(iface);

    ret = nativeClient_.GetInterfaceMtu(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.SetInterfaceMtu(iface, 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.AddInterfaceAddress(iface, "", 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.DelInterfaceAddress(iface, "", 0);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(MockNetsysNativeClientTest, MockNetsysNativeClientBranchTest002, TestSize.Level1)
{
    nativeClient_.Init();
    nativeClient_.RegisterMockApi();

    int32_t netId = 0;
    std::string iface = "";
    std::vector<std::string> testInput = {};
    int32_t ret = nativeClient_.SetResolverConfig(netId, 0, 0, testInput, testInput);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    ret = nativeClient_.GetResolverConfig(netId, testInput, testInput, baseTimeoutMsec, retryCount);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.CreateNetworkCache(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.DestroyNetworkCache(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetCellularRxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetCellularTxBytes();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetAllBytes(iface);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetAllRxBytes();
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetAllTxBytes();
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(MockNetsysNativeClientTest, MockNetsysNativeClientBranchTest003, TestSize.Level1)
{
    nativeClient_.Init();
    nativeClient_.RegisterMockApi();

    uint32_t uid = 0;
    int32_t ret = nativeClient_.GetUidTxBytes(uid);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetUidRxBytes(uid);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string iface = "";
    ret = nativeClient_.GetUidOnIfaceTxBytes(uid, iface);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetUidOnIfaceRxBytes(uid, iface);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string filename = "";
    ret = nativeClient_.GetIfaceBytes(iface, filename);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetIfaceRxBytes(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetIfaceTxBytes(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.GetIfaceRxPackets(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(MockNetsysNativeClientTest, MockNetsysNativeClientBranchTest004, TestSize.Level1)
{
    nativeClient_.Init();
    nativeClient_.RegisterMockApi();

    std::string iface = "";
    std::string filename = "";
    int32_t ret = nativeClient_.GetIfaceTxPackets(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<std::string> ifList = {};
    EXPECT_TRUE(nativeClient_.InterfaceGetList() != ifList);

    EXPECT_FALSE(nativeClient_.UidGetList() != ifList);

    ret = nativeClient_.AddRoute("", "", "", "");
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t netId = 0;
    ret = nativeClient_.SetDefaultNetWork(netId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.ClearDefaultNetWorkNetId();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t socketFd = 0;
    uint16_t id = 0;
    ret = nativeClient_.ShareDnsSet(id);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    NetsysNotifyCallback callback;
    ret = nativeClient_.RegisterNetsysNotifyCallback(callback);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.BindNetworkServiceVpn(socketFd);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    struct ifreq ifRequest = {};
    int32_t ifaceFd = 0;
    ret = nativeClient_.EnableVirtualNetIfaceCard(socketFd, ifRequest, ifaceFd);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(MockNetsysNativeClientTest, MockNetsysNativeClientBranchTest005, TestSize.Level1)
{
    nativeClient_.Init();
    nativeClient_.RegisterMockApi();

    int32_t ifaceFd = 0;
    std::string iface = "";
    int32_t socketFd = 0;
    struct ifreq ifRequest = {};
    int32_t ret = nativeClient_.SetIpAddress(socketFd, "", ifaceFd, ifRequest);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.SetBlocking(ifaceFd, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.StartDhcpClient(iface, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.StopDhcpClient(iface, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.RegisterCallback(nullptr);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.StartDhcpService(iface, "");
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = nativeClient_.StopDhcpService(iface);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::string ipAddr = "";
    uint32_t uid = 1000;
    ret = nativeClient_.CloseSocketsUid(ipAddr, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::vector<int32_t> netIds = {};
    ret = nativeClient_.SetIpv6UidBlackList(netIds, uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

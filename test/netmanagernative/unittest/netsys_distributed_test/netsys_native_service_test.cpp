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

#include <algorithm>
#include <gtest/gtest.h>
#include <string>

#include "interface_manager.h"
#include "netsys_controller.h"
#include "system_ability_definition.h"

#ifdef GTEST_API_
#define private public
#define protected public
#endif

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

HWTEST_F(NetsysNativeServiceTest, EnableDistributedClientNet001, TestSize.Level1)
{
    std::string virnicAddr = "1.189.55.60";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    int32_t ret = instance_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = instance_->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_TRUE(ret == NetManagerStandard::NETMANAGER_SUCCESS || ret == NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetsysNativeServiceTest, EnableDistributedClientNet002, TestSize.Level1)
{
    std::string virnicAddr = "";
    std::string virnicName = "virnic";
    std::string iif = "";
    int32_t ret = instance_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    bool isServer = false;
    std::string dstAddr = "1.1.1.1";
    ret = instance_->DisableDistributedNet(isServer, virnicName, dstAddr);
}

HWTEST_F(NetsysNativeServiceTest, EnableDistributedServerNet001, TestSize.Level1)
{
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";
    int32_t ret = instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    bool isServer = true;
    std::string virnicName = "virnic";
    ret = instance_->DisableDistributedNet(isServer, virnicName, dstAddr);
}
HWTEST_F(NetsysNativeServiceTest, EnableDistributedServerNet002, TestSize.Level1)
{
    std::string iif = "";
    std::string devIface = "";
    std::string dstAddr = "";
    std::string gw = "";
    int32_t ret = instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    bool isServer = true;
    std::string virnicName = "virnic";
    ret = instance_->DisableDistributedNet(isServer, virnicName, dstAddr);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, EnableDistributedClientNet003, TestSize.Level1)
{
    std::string virnicAddr = "1.189.55.60";
    std::string virnicName = "virnic";
    std::string iif = "lo";
    int32_t ret = instance_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    virnicAddr = "";
    instance_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    virnicAddr = "1.189.55.60";
    iif = "";
    instance_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    virnicAddr = "";
    ret = instance_->EnableDistributedClientNet(virnicAddr, virnicName, iif);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, EnableDistributedServerNet003, TestSize.Level1)
{
    std::string iif = "lo";
    std::string devIface = "lo";
    std::string dstAddr = "1.189.55.61";
    std::string gw = "0.0.0.0";
    int32_t ret = instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
    dstAddr = "";
    instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    devIface = "";
    instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    dstAddr = "1.189.55.61";
    instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    iif = "";
    instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    devIface = "lo";
    instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    dstAddr = "";
    instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    devIface = "";
    ret = instance_->EnableDistributedServerNet(iif, devIface, dstAddr, gw);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, GetNetworkCellularSharingTraffic001, TestSize.Level1)
{
    NetworkSharingTraffic traffic;
    std::string ifaceName = "123";
    instance_->sharingManager_ = nullptr;
    int32_t ret = instance_->GetNetworkCellularSharingTraffic(traffic, ifaceName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
    instance_->sharingManager_ = std::make_unique<OHOS::nmd::SharingManager>();
    instance_->GetNetworkCellularSharingTraffic(traffic, ifaceName);
}

HWTEST_F(NetsysNativeServiceTest, SetNetStateTrafficMap001, TestSize.Level1)
{
    uint8_t flag = 123;
    uint64_t availableTraffic = 123;
    instance_->bpfStats_ = nullptr;
    int32_t ret = instance_->SetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
    instance_->bpfStats_ = std::make_unique<OHOS::NetManagerStandard::NetsysBpfStats>();
    instance_->SetNetStateTrafficMap(flag, availableTraffic);
}

HWTEST_F(NetsysNativeServiceTest, GetNetStateTrafficMap001, TestSize.Level1)
{
    uint8_t flag = 123;
    uint64_t availableTraffic = 123;
    instance_->bpfStats_ = nullptr;
    int32_t ret = instance_->GetNetStateTrafficMap(flag, availableTraffic);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
    instance_->bpfStats_ = std::make_unique<OHOS::NetManagerStandard::NetsysBpfStats>();
    instance_->GetNetStateTrafficMap(flag, availableTraffic);
}

HWTEST_F(NetsysNativeServiceTest, UpdateIfIndexMap001, TestSize.Level1)
{
    uint8_t flag = 123;
    uint64_t index = 123;
    instance_->bpfStats_ = nullptr;
    int32_t ret = instance_->UpdateIfIndexMap(flag, index);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
    instance_->bpfStats_ = std::make_unique<OHOS::NetManagerStandard::NetsysBpfStats>();
    instance_->UpdateIfIndexMap(flag, index);
}

HWTEST_F(NetsysNativeServiceTest, ClearIncreaseTrafficMap001, TestSize.Level1)
{
    instance_->bpfStats_ = nullptr;
    int32_t ret = instance_->ClearIncreaseTrafficMap();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
    instance_->bpfStats_ = std::make_unique<OHOS::NetManagerStandard::NetsysBpfStats>();
    instance_->ClearIncreaseTrafficMap();
}

HWTEST_F(NetsysNativeServiceTest, DeleteIncreaseTrafficMap001, TestSize.Level1)
{
    instance_->bpfStats_ = nullptr;
    uint64_t ifIndex = 12;
    int32_t ret = instance_->DeleteIncreaseTrafficMap(ifIndex);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
    instance_->bpfStats_ = std::make_unique<OHOS::NetManagerStandard::NetsysBpfStats>();
    instance_->DeleteIncreaseTrafficMap(ifIndex);
}

HWTEST_F(NetsysNativeServiceTest, ClearSimStatsBpfMap001, TestSize.Level1)
{
    instance_->bpfStats_ = nullptr;
    int32_t ret = instance_->ClearSimStatsBpfMap();
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
    instance_->bpfStats_ = std::make_unique<OHOS::NetManagerStandard::NetsysBpfStats>();
    instance_->ClearSimStatsBpfMap();
}

HWTEST_F(NetsysNativeServiceTest, GetAllSimStatsInfo001, TestSize.Level1)
{
    std::vector<OHOS::NetManagerStandard::NetStatsInfo> stats;
    instance_->bpfStats_ = nullptr;
    int32_t ret = instance_->GetAllSimStatsInfo(stats);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, DeleteSimStatsInfo001, TestSize.Level1)
{
    uint32_t uid = 123;
    instance_->bpfStats_ = nullptr;
    int32_t ret = instance_->DeleteSimStatsInfo(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, DeleteStatsInfo001, TestSize.Level1)
{
    uint32_t uid = 123;
    instance_->bpfStats_ = nullptr;
    int32_t ret = instance_->DeleteStatsInfo(uid);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, SetIpCommandForRes001, TestSize.Level1)
{
    std::string cmd = "123";
    std::string respond = "123";
    instance_->netDiagWrapper = nullptr;
    int32_t ret = instance_->SetIpCommandForRes(cmd, respond);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

HWTEST_F(NetsysNativeServiceTest, AddStaticArp001, TestSize.Level1)
{
    std::string ipAddr = "123";
    std::string macAddr = "123";
    std::string ifName = "123";
    instance_->netsysService_ = nullptr;
    int32_t ret = instance_->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, AddStaticArp002, TestSize.Level1)
{
    std::string ipAddr = "123";
    std::string macAddr = "123";
    std::string ifName = "123";
    instance_->netsysService_ = std::make_unique<OHOS::nmd::NetManagerNative>();
    int32_t ret = instance_->AddStaticArp(ipAddr, macAddr, ifName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, DelStaticArp001, TestSize.Level1)
{
    std::string ipAddr = "123";
    std::string macAddr = "123";
    std::string ifName = "123";
    instance_->netsysService_ = nullptr;
    int32_t ret = instance_->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, DelStaticArp002, TestSize.Level1)
{
    std::string ipAddr = "123";
    std::string macAddr = "123";
    std::string ifName = "123";
    instance_->netsysService_ = std::make_unique<OHOS::nmd::NetManagerNative>();
    int32_t ret = instance_->DelStaticArp(ipAddr, macAddr, ifName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, AddStaticIpv6Addr001, TestSize.Level1)
{
    std::string ipAddr = "123";
    std::string macAddr = "123";
    std::string ifName = "123";
    instance_->netsysService_ = nullptr;
    int32_t ret = instance_->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, AddStaticIpv6Addr002, TestSize.Level1)
{
    std::string ipAddr = "123";
    std::string macAddr = "123";
    std::string ifName = "123";
    instance_->netsysService_ = std::make_unique<OHOS::nmd::NetManagerNative>();
    int32_t ret = instance_->AddStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, DelStaticIpv6Addr001, TestSize.Level1)
{
    std::string ipAddr = "123";
    std::string macAddr = "123";
    std::string ifName = "123";
    instance_->netsysService_ = nullptr;
    int32_t ret = instance_->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, DelStaticIpv6Addr002, TestSize.Level1)
{
    std::string ipAddr = "123";
    std::string macAddr = "123";
    std::string ifName = "123";
    instance_->netsysService_ = std::make_unique<OHOS::nmd::NetManagerNative>();
    int32_t ret = instance_->DelStaticIpv6Addr(ipAddr, macAddr, ifName);
    EXPECT_NE(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, SetBrokerUidAccessPolicyMap001, TestSize.Level1)
{
    std::unordered_map<uint32_t, uint32_t> uidMaps;
    int32_t ret = instance_->SetBrokerUidAccessPolicyMap(uidMaps);
    EXPECT_EQ(ret, NetManagerStandard::NETSYS_SUCCESS);
}

HWTEST_F(NetsysNativeServiceTest, CreateVlan001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    auto instance = std::make_shared<NetsysNativeService>(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    instance->netsysService_ = nullptr;
    int32_t ret = instance->CreateVlan(ifName, vlanId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, DestroyVlan001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    auto instance = std::make_shared<NetsysNativeService>(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    instance->netsysService_ = nullptr;
    int32_t ret = instance->DestroyVlan(ifName, vlanId);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, AddVlanIp001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    auto instance = std::make_shared<NetsysNativeService>(COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    instance->netsysService_ = nullptr;
    int32_t ret = instance->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, GetConnectOwnerUid001, TestSize.Level1)
{
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    instance_->netsysService_ = std::make_unique<OHOS::nmd::NetManagerNative>();
    int32_t ret = instance_->GetConnectOwnerUid(info, uid);
    EXPECT_NE(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetsysNativeServiceTest, GetConnectOwnerUid002, TestSize.Level1)
{
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    instance_->netsysService_ = nullptr;
    int32_t ret = instance_->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}
} // namespace NetsysNative
} // namespace OHOS

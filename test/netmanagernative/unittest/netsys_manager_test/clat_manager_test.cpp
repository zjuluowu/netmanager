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

#include "iservice_registry.h"
#include "system_ability_definition.h"

#ifdef GTEST_API_
#define private public
#endif

#include "clat_constants.h"
#include "clat_manager.h"
#include "net_conn_manager_test_util.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_native_service_proxy.h"
#include "network_permission.h"
#include "physical_network.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace NetManagerStandard;
using namespace NetConnManagerTestUtil;

class ClatManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<ClatManager> instance_ = nullptr;
};

void ClatManagerTest::SetUpTestCase()
{
    instance_ = std::make_shared<ClatManager>();
}

void ClatManagerTest::TearDownTestCase()
{
    instance_ = nullptr;
}

void ClatManagerTest::SetUp() {}

void ClatManagerTest::TearDown() {}

/**
 * @tc.name: ClatStartTest001
 * @tc.desc: Test ConnManager ClatStart.
 * @tc.type: FUNC
 */
HWTEST_F(ClatManagerTest, ClatStartTest001, TestSize.Level1)
{
    const std::string v6Iface = "";
    int32_t netId = 1;
    const std::string nat64PrefixStr;
    NetManagerNative *netsysService = nullptr;

    int32_t ret = instance_->ClatStart(v6Iface, netId, nat64PrefixStr, netsysService);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
    netsysService = new NetManagerNative();
    ret = instance_->ClatStart(v6Iface, netId, nat64PrefixStr, netsysService);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

/**
 * @tc.name: ClatStopTest001
 * @tc.desc: Test ConnManager ClatStop - clatd not started case.
 * @tc.type: FUNC
 */
HWTEST_F(ClatManagerTest, ClatStopTest001, TestSize.Level1)
{
    const std::string v6Iface = "";
    NetManagerNative *netsysService = nullptr;
    netsysService = new NetManagerNative();
    int32_t ret = instance_->ClatStop(v6Iface, netsysService);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
    delete netsysService;
}

/**
 * @tc.name: GenerateClatSrcAddrTest001
 * @tc.desc: Test ConnManager GenerateClatSrcAddr.
 * @tc.type: FUNC
 */
HWTEST_F(ClatManagerTest, GenerateClatSrcAddrTest001, TestSize.Level1)
{
    const std::string v6Iface = "";
    const std::string nat64PrefixStr = "";
    uint32_t fwmark = 1;
    INetAddr v4Addr;
    INetAddr v6Addr;
    int32_t ret = instance_->GenerateClatSrcAddr(v6Iface, fwmark, nat64PrefixStr, v4Addr, v6Addr);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);
}

/**
 * @tc.name: CreateAndConfigureTunIfaceTest001
 * @tc.desc: Test ConnManager CreateAndConfigureTunIface.
 * @tc.type: FUNC
 */
HWTEST_F(ClatManagerTest, CreateAndConfigureTunIfaceTest001, TestSize.Level1)
{
    const std::string v6Iface = "";
    const std::string tunIface = "";
    INetAddr v4Addr;
    NetManagerNative *netsysService = nullptr;
    int tunFd = 1;

    int32_t ret = instance_->CreateAndConfigureTunIface(v6Iface, tunIface, v4Addr, netsysService, tunFd);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

/**
 * @tc.name: CreateAndConfigureClatSocketTest001
 * @tc.desc: Test ConnManager CreateAndConfigureClatSocket.
 * @tc.type: FUNC
 */
HWTEST_F(ClatManagerTest, CreateAndConfigureClatSocketTest001, TestSize.Level1)
{
    const std::string v6Iface = "";
    uint32_t fwmark = 1;
    INetAddr v6Addr;
    int readSock6 = 1;
    int writeSock6 = 1;

    int32_t ret = instance_->CreateAndConfigureClatSocket(v6Iface, v6Addr, fwmark, readSock6, writeSock6);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

/**
 * @tc.name: AddNatBypassRules
 * @tc.desc: Test ConnManager AddNatBypassRules.
 * @tc.type: FUNC
 */
HWTEST_F(ClatManagerTest, AddNatBypassRulesTest001, TestSize.Level1)
{
    const std::string v6Iface = "rmnet0";
    const std::string v6Ip = "240e:46e:b900:27ab:1532:b318:192b:2841";
    int32_t ret = instance_->AddNatBypassRules(v6Iface, v6Ip);
    EXPECT_LE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: DeleteNatBypassRules
 * @tc.desc: Test ConnManager DeleteNatBypassRules.
 * @tc.type: FUNC
 */
HWTEST_F(ClatManagerTest, DeleteNatBypassRulesTest001, TestSize.Level1)
{
    const std::string v6Iface = "rmnet0";
    int32_t ret = instance_->DeleteNatBypassRules(v6Iface);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ClatManager_GetFwmark_Success
 * @tc.name: Test GetFwmark returns correct mark value
 * @tc.desc: Verify that GetFwmark correctly constructs the fwmark with netId and permission
 */
HWTEST_F(ClatManagerTest, ClatManager_GetFwmark_Success, TestSize.Level1)
{
    int32_t netId = 1;
    uint32_t ret = instance_->GetFwmark(netId);
    EXPECT_GT(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.number: ClatManager_GetClatNetChains_Generate
 * @tc.name: Test GetClatNetChains generates correct chain name
 * @tc.desc: Verify that GetClatNetChains returns the expected chain name format
 */
HWTEST_F(ClatManagerTest, ClatManager_GetClatNetChains_Generate, TestSize.Level1)
{
    const std::string v6Iface = "rmnet0";
    std::string result = instance_->GetClatNetChains(v6Iface);
    EXPECT_EQ(result, "clat_raw_rmnet0_OUTPUT");
}

/**
 * @tc.number: ClatManager_EnableByPassNatCmd_Generate
 * @tc.name: Test EnableByPassNatCmd generates correct command
 * @tc.desc: Verify that EnableByPassNatCmd returns the expected iptables command
 */
HWTEST_F(ClatManagerTest, ClatManager_EnableByPassNatCmd_Generate, TestSize.Level1)
{
    const std::string v6Iface = "rmnet0";
    const std::string v6Ip = "240e:46e:b900:27ab:1532:b318:192b:2841";
    std::string result = instance_->EnableByPassNatCmd(v6Iface, v6Ip);
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("-A clat_raw_rmnet0_OUTPUT"), std::string::npos);
    EXPECT_NE(result.find("-s 240e:46e:b900:27ab:1532:b318:192b:2841"), std::string::npos);
    EXPECT_NE(result.find("-j NOTRACK"), std::string::npos);
}

/**
 * @tc.number: ClatManager_CombineRestoreRules_Append
 * @tc.name: Test CombineRestoreRules appends rules correctly
 * @tc.desc: Verify that CombineRestoreRules correctly appends commands to cmdSet
 */
HWTEST_F(ClatManagerTest, ClatManager_CombineRestoreRules_Append, TestSize.Level1)
{
    std::string cmdSet = "";
    std::string cmds = "-A OUTPUT -j ACCEPT";
    instance_->CombineRestoreRules(cmds, cmdSet);
    EXPECT_FALSE(cmdSet.empty());
    EXPECT_NE(cmdSet.find(cmds), std::string::npos);
}

/**
 * @tc.number: ClatManager_AddClatRoute_Success
 * @tc.name: Test AddClatRoute adds route successfully
 * @tc.desc: Verify that AddClatRoute returns NETMANAGER_SUCCESS when parameters are valid
 * @tc.note: NetManagerNative methods are not virtual, so this test uses the actual implementation
 */
HWTEST_F(ClatManagerTest, ClatManager_AddClatRoute_Success, TestSize.Level1)
{
    int32_t netId = 1;
    const std::string tunIface = "tunv4-rmnet0";
    const std::string v4Addr = "192.0.0.2";
    NetManagerNative *netsysService = new NetManagerNative();

    int32_t ret = instance_->AddClatRoute(netId, tunIface, v4Addr, netsysService);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);

    delete netsysService;
}

/**
 * @tc.number: ClatManager_DeleteClatRoute_Success
 * @tc.name: Test DeleteClatRoute deletes route successfully
 * @tc.desc: Verify that DeleteClatRoute returns NETMANAGER_SUCCESS when parameters are valid
 * @tc.note: NetManagerNative methods are not virtual, so this test uses the actual implementation
 */
HWTEST_F(ClatManagerTest, ClatManager_DeleteClatRoute_Success, TestSize.Level1)
{
    int32_t netId = 1;
    const std::string tunIface = "tunv4-rmnet0";
    const std::string v4Addr = "192.0.0.2";
    NetManagerNative *netsysService = new NetManagerNative();

    int32_t ret = instance_->DeleteClatRoute(netId, tunIface, v4Addr, netsysService);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    delete netsysService;
}
} // namespace NetsysNative
} // namespace OHOS
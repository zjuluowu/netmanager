/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "net_conn_service_iface.h"
#include "net_conn_constants.h"
#include "net_manager_constants.h"
#include "net_factoryreset_callback_stub.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr int32_t TEST_NETID = 999;
constexpr const char *TEST_IDENT = "test_ident";
uint32_t g_supplierId = 0;
} // namespace

class NetConnServiceIfaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline NetConnServiceIface instance_;
};

void NetConnServiceIfaceTest::SetUpTestCase() {}

void NetConnServiceIfaceTest::TearDownTestCase() {}

void NetConnServiceIfaceTest::SetUp() {}

void NetConnServiceIfaceTest::TearDown() {}

HWTEST_F(NetConnServiceIfaceTest, GetIfaceNamesTest001, TestSize.Level1)
{
    std::list<std::string> ifaceNames;
    int32_t ret = instance_.GetIfaceNames(NetBearType::BEARER_ETHERNET, ifaceNames);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceIfaceTest, GetIfaceNameByTypeTest001, TestSize.Level1)
{
    std::string ifaceName;
    int32_t ret = instance_.GetIfaceNameByType(NetBearType::BEARER_ETHERNET, TEST_IDENT, ifaceName);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceIfaceTest, RegisterNetSupplierTest001, TestSize.Level1)
{
    uint32_t supplierId = 0;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET};
    int32_t ret = instance_.RegisterNetSupplier(NetBearType::BEARER_CELLULAR, "", netCaps, supplierId);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    ret = instance_.UnregisterNetSupplier(supplierId);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceIfaceTest, UpdateNetLinkInfoTest001, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    ASSERT_NE(netLinkInfo, nullptr);
    auto ret = instance_.UpdateNetLinkInfo(g_supplierId, netLinkInfo);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceIfaceTest, UpdateNetSupplierInfoTest001, TestSize.Level1)
{
    sptr<NetSupplierInfo> netSupplierInfo = new (std::nothrow) NetSupplierInfo();
    ASSERT_NE(netSupplierInfo, nullptr);
    auto ret = instance_.UpdateNetSupplierInfo(g_supplierId, netSupplierInfo);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceIfaceTest, RestrictBackgroundChangedTest001, TestSize.Level1)
{
    int32_t ret = instance_.RestrictBackgroundChanged(false);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetConnServiceIfaceTest, RegisterNetConnCallbackTest001, TestSize.Level1)
{
    int32_t ret = instance_.RegisterNetConnCallback(nullptr);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceIfaceTest, RegisterNetFactoryResetCallbackTest001, TestSize.Level1)
{
    sptr<INetFactoryResetCallback> callback = nullptr;
    auto ret = instance_.RegisterNetFactoryResetCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    callback = new (std::nothrow) NetFactoryResetCallbackStub();
    ret = instance_.RegisterNetFactoryResetCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceIfaceTest, UpdateUidLostDelay, TestSize.Level1)
{
    std::set<uint32_t> uidLostDelaySet;
    uidLostDelaySet.insert(1000);
    int32_t ret = instance_.UpdateUidLostDelay(uidLostDelaySet);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetConnServiceIfaceTest, GetConnectionPropertiesTest001, TestSize.Level1)
{
    NetLinkInfo info;
    int32_t ret = instance_.GetConnectionProperties(TEST_NETID, info);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetConnServiceIfaceTest, RegisterDualStackProbeCallbackTest001, TestSize.Level1)
{
    std::shared_ptr<IDualStackProbeCallback> cb = nullptr;
    int32_t ret = instance_.RegisterDualStackProbeCallback(TEST_NETID, cb);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetConnServiceIfaceTest, UnRegisterDualStackProbeCallbackTest001, TestSize.Level1)
{
    std::shared_ptr<IDualStackProbeCallback> cb = nullptr;
    int32_t ret = instance_.UnRegisterDualStackProbeCallback(TEST_NETID, cb);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetConnServiceIfaceTest, DualStackProbeTest001, TestSize.Level1)
{
    int32_t ret = instance_.DualStackProbe(TEST_NETID);
    EXPECT_NE(ret, 0);
}

HWTEST_F(NetConnServiceIfaceTest, UpdateDualStackProbeTimeTest001, TestSize.Level1)
{
    int32_t testProbeTime = 5 * 1000;
    int32_t ret = instance_.UpdateDualStackProbeTime(testProbeTime);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetConnServiceIfaceTest, UpdateUidDeadFlowResetTest001, TestSize.Level1)
{
    std::vector<std::string> bundleNameVec;
    bundleNameVec.push_back("com.test.bundle");
    int32_t ret = instance_.UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetConnServiceIfaceTest, UpdateUidDeadFlowResetTest002, TestSize.Level1)
{
    std::vector<std::string> bundleNameVec;
    int32_t ret = instance_.UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_EQ(ret, 0);
}
} // namespace NetManagerStandard
} // namespace OHOS

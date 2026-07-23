/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#define private public
#include "net_conn_base_service.h"
#include "net_conn_service_iface.h"
#include "net_conn_types.h"
#include "net_ethernet_base_service.h"
#include "net_manager_center.h"
#include "net_manager_constants.h"
#include "net_policy_base_service.h"
#include "net_stats_base_service.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *TEST_IDENT = "testIdent";
constexpr std::initializer_list<NetBearType> BEAR_TYPE_LIST = {
    NetBearType::BEARER_CELLULAR, NetBearType::BEARER_WIFI, NetBearType::BEARER_BLUETOOTH,
    NetBearType::BEARER_ETHERNET, NetBearType::BEARER_VPN,  NetBearType::BEARER_WIFI_AWARE,
};

class TestConnService : public NetConnBaseService {
public:
    inline int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                       const std::set<NetCap> &netCaps, uint32_t &supplierId) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t UnregisterNetSupplier(uint32_t supplierId) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t SetReuseSupplierId(uint32_t supplierId, uint32_t reuseSupplierId, bool isReused) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t RestrictBackgroundChanged(bool isRestrictBackground) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t RegisterNetConnCallback(const sptr<INetConnCallback> &callback) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t UpdateUidLostDelay(const std::set<uint32_t> &uidLostDelaySet) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t UpdateUidDeadFlowReset(const std::vector<std::string> &bundleNameVec) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t GetConnectionProperties(int32_t netId, NetLinkInfo &info) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t RegisterDualStackProbeCallback(int32_t netId,
        std::shared_ptr<IDualStackProbeCallback>& callback) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t UnRegisterDualStackProbeCallback(int32_t netId,
        std::shared_ptr<IDualStackProbeCallback>& callback) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t DualStackProbe(int32_t netId) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t UpdateDualStackProbeTime(int32_t dualStackProbeTimeOut) override
    {
        return NETMANAGER_SUCCESS;
    }

    inline ProbeUrls GetDataShareUrl() override
    {
        ProbeUrls urls;
        return urls;
    }

    bool RegisterNetRequestControlFunc(std::function<bool(const NetRequest &)> func) override
    {
        return true;
    }

    bool GetAllNetRequest(std::vector<NetRequest> &netRequests) override
    {
        return true;
    }

    bool UpdateNetRequestControlState(const std::vector<NetRequest> &netRequests) override
    {
        return true;
    }
};

class TestNetEthernetService : public NetEthernetBaseService {
public:
    inline int32_t ResetEthernetFactory() override
    {
        return NETMANAGER_SUCCESS;
    }
};

class TestNetPolicyService : public NetPolicyBaseService {
public:
    inline int32_t ResetPolicies() override
    {
        return NETMANAGER_SUCCESS;
    }
    inline bool IsUidNetAllowed(uint32_t uid, bool metered) override
    {
        return NETMANAGER_SUCCESS;
    }
};

class TestNetStatsService : public NetStatsBaseService {
public:
    inline int32_t GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                       NetStatsInfo &info) override
    {
        return NETMANAGER_SUCCESS;
    }
    inline int32_t ResetStatsFactory() override
    {
        return NETMANAGER_SUCCESS;
    }
};

class TestNetVpnService : public NetVpnBaseService {
public:
    TestNetVpnService() = default;
    ~TestNetVpnService() override = default;
    bool IsVpnApplication(int32_t uid) override
    {
        return true;
    }

    bool IsAppUidInWhiteList(int32_t callingUid, int32_t appUid) override
    {
        return true;
    }

    void NotifyAllowConnectVpnBundleNameChanged(
        std::set<std::string> &&allowConnectVpnBundleName,
        std::set<std::string> &&allowVpnStartWithoutCheckPermissions) override
    {
        allowConnectVpnBundleName_ = std::move(allowConnectVpnBundleName);
        allowVpnStartWithoutCheckPermissions_ = std::move(allowVpnStartWithoutCheckPermissions);
    }

    std::set<std::string> allowConnectVpnBundleName_;
    std::set<std::string> allowVpnStartWithoutCheckPermissions_;
};
} // namespace

class NetManagerCenterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline NetManagerCenter &instance_ = NetManagerCenter::GetInstance();
    static inline uint32_t supplierId_ = 0;
};

void NetManagerCenterTest::SetUpTestCase() {}

void NetManagerCenterTest::TearDownTestCase() {}

void NetManagerCenterTest::SetUp()
{
    instance_.RegisterConnService(nullptr);
    instance_.RegisterStatsService(nullptr);
    instance_.RegisterPolicyService(nullptr);
    instance_.RegisterEthernetService(nullptr);
}

void NetManagerCenterTest::TearDown() {}

HWTEST_F(NetManagerCenterTest, GetIfaceNamesTest001, TestSize.Level1)
{
    std::list<std::string> list;
    std::for_each(BEAR_TYPE_LIST.begin(), BEAR_TYPE_LIST.end(), [this, &list](const auto &type) {
        int32_t ret = instance_.GetIfaceNames(type, list);
        std::cout << "TYPE:" << type << "LIST_SIZE:" << list.size() << std::endl;
        EXPECT_EQ(ret, NETMANAGER_ERROR);
        EXPECT_TRUE(list.empty());
        list.clear();
    });
}

HWTEST_F(NetManagerCenterTest, GetIfaceNamesTest002, TestSize.Level1)
{
    std::list<std::string> list;
    int32_t ret = instance_.GetIfaceNames(NetBearType::BEARER_DEFAULT, list);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    EXPECT_TRUE(list.empty());
}

HWTEST_F(NetManagerCenterTest, GetIfaceNamesTest003, TestSize.Level1)
{
    sptr<NetConnBaseService> service = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(service);
    std::list<std::string> list;
    std::for_each(BEAR_TYPE_LIST.begin(), BEAR_TYPE_LIST.end(), [this, &list](const auto &type) {
        int32_t ret = instance_.GetIfaceNames(type, list);
        std::cout << "TYPE:" << type << "LIST_SIZE:" << list.size() << std::endl;
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
        EXPECT_TRUE(list.empty());
        list.clear();
    });
}

HWTEST_F(NetManagerCenterTest, GetIfaceNamesTest004, TestSize.Level1)
{
    sptr<NetConnBaseService> service = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(service);
    std::list<std::string> list;
    int32_t ret = instance_.GetIfaceNames(NetBearType::BEARER_DEFAULT, list);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(list.empty());
}

HWTEST_F(NetManagerCenterTest, GetIfaceNameByTypeTest001, TestSize.Level1)
{
    std::string ifaceName;
    std::for_each(BEAR_TYPE_LIST.begin(), BEAR_TYPE_LIST.end(), [this, &ifaceName](const auto &type) {
        int32_t ret = instance_.GetIfaceNameByType(type, TEST_IDENT, ifaceName);
        std::cout << "TYPE:" << type << "LIST_SIZE:" << ifaceName.size() << std::endl;
        EXPECT_EQ(ret, NETMANAGER_ERROR);
        EXPECT_TRUE(ifaceName.empty());
        ifaceName.clear();
    });
}

HWTEST_F(NetManagerCenterTest, GetIfaceNameByTypeTest002, TestSize.Level1)
{
    std::string ifaceName;
    int32_t ret = instance_.GetIfaceNameByType(NetBearType::BEARER_DEFAULT, TEST_IDENT, ifaceName);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
    EXPECT_TRUE(ifaceName.empty());
}

HWTEST_F(NetManagerCenterTest, GetIfaceNameByTypeTest003, TestSize.Level1)
{
    sptr<NetConnBaseService> service = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(service);
    std::string ifaceName;
    std::for_each(BEAR_TYPE_LIST.begin(), BEAR_TYPE_LIST.end(), [this, &ifaceName](const auto &type) {
        int32_t ret = instance_.GetIfaceNameByType(type, TEST_IDENT, ifaceName);
        std::cout << "TYPE:" << type << "LIST_SIZE:" << ifaceName.size() << std::endl;
        EXPECT_EQ(ret, NETMANAGER_SUCCESS);
        EXPECT_TRUE(ifaceName.empty());
        ifaceName.clear();
    });
}

HWTEST_F(NetManagerCenterTest, GetIfaceNameByTypeTest004, TestSize.Level1)
{
    sptr<NetConnBaseService> service = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(service);
    std::string ifaceName;
    int32_t ret = instance_.GetIfaceNameByType(NetBearType::BEARER_DEFAULT, TEST_IDENT, ifaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(ifaceName.empty());
}

HWTEST_F(NetManagerCenterTest, RegisterNetSupplierTest001, TestSize.Level1)
{
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET};
    std::string ident = "ident";
    int32_t result = instance_.RegisterNetSupplier(bearerType, ident, netCaps, supplierId_);
    ASSERT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, RegisterNetSupplierTest002, TestSize.Level1)
{
    sptr<NetConnBaseService> service = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(service);
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET};
    std::string ident = "ident";
    int32_t result = instance_.RegisterNetSupplier(bearerType, ident, netCaps, supplierId_);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, UnegisterNetSupplierTest001, TestSize.Level1)
{
    int32_t result = instance_.UnregisterNetSupplier(supplierId_);
    ASSERT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, UnegisterNetSupplierTest002, TestSize.Level1)
{
    sptr<NetConnBaseService> service = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(service);
    int32_t result = instance_.UnregisterNetSupplier(supplierId_);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, UpdateNetLinkInfoTest001, TestSize.Level1)
{
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps = {NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};

    std::string ident = "ident04";
    uint32_t supplierId = 0;
    int32_t result = instance_.RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    ASSERT_EQ(result, NETMANAGER_ERROR);

    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    result = instance_.UpdateNetLinkInfo(supplierId, netLinkInfo);
    ASSERT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, UpdateNetLinkInfoTest002, TestSize.Level1)
{
    sptr<NetConnBaseService> service = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(service);
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps = {NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};

    std::string ident = "ident04";
    uint32_t supplierId = 0;
    int32_t result = instance_.RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);

    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    result = instance_.UpdateNetLinkInfo(supplierId, netLinkInfo);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, UpdateNetSupplierInfoTest001, TestSize.Level1)
{
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};
    std::string ident = "ident03";
    uint32_t supplierId = 0;
    int32_t result = instance_.RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    ASSERT_EQ(result, NETMANAGER_ERROR);

    sptr<NetSupplierInfo> netSupplierInfo = new NetSupplierInfo();
    netSupplierInfo->isAvailable_ = true;
    netSupplierInfo->isRoaming_ = true;
    netSupplierInfo->strength_ = 0x64;
    netSupplierInfo->frequency_ = 0x10;
    result = instance_.UpdateNetSupplierInfo(supplierId, netSupplierInfo);
    ASSERT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, UpdateNetSupplierInfoTest002, TestSize.Level1)
{
    sptr<NetConnBaseService> service = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(service);
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET, NET_CAPABILITY_MMS};
    std::string ident = "ident03";
    uint32_t supplierId = 0;
    int32_t result = instance_.RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);

    sptr<NetSupplierInfo> netSupplierInfo = new NetSupplierInfo();
    netSupplierInfo->isAvailable_ = true;
    netSupplierInfo->isRoaming_ = true;
    netSupplierInfo->strength_ = 0x64;
    netSupplierInfo->frequency_ = 0x10;
    result = instance_.UpdateNetSupplierInfo(supplierId, netSupplierInfo);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, GetIfaceStatsDetailTest001, TestSize.Level1)
{
    std::string iface = "test_iface";
    uint32_t startTime = 0;
    uint32_t endTime = 9999999;
    NetStatsInfo info;
    int32_t ret = instance_.GetIfaceStatsDetail(iface, startTime, endTime, info);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, GetIfaceStatsDetailTest002, TestSize.Level1)
{
    sptr<NetStatsBaseService> service = new (std::nothrow) TestNetStatsService();
    instance_.RegisterStatsService(service);
    std::string iface = "test_iface";
    uint32_t startTime = 0;
    uint32_t endTime = 9999999;
    NetStatsInfo info;
    int32_t ret = instance_.GetIfaceStatsDetail(iface, startTime, endTime, info);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, ResetStatsFactoryTest001, TestSize.Level1)
{
    int32_t ret = instance_.ResetStatsFactory();
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, ResetStatsFactoryTest002, TestSize.Level1)
{
    sptr<NetStatsBaseService> service = new (std::nothrow) TestNetStatsService();
    instance_.RegisterStatsService(service);
    int32_t ret = instance_.ResetStatsFactory();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, ResetPolicyFactoryTest001, TestSize.Level1)
{
    int32_t ret = instance_.ResetPolicyFactory();
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, ResetPolicyFactoryTest002, TestSize.Level1)
{
    sptr<NetPolicyBaseService> service = new (std::nothrow) TestNetPolicyService();
    instance_.RegisterPolicyService(service);
    int32_t ret = instance_.ResetPolicyFactory();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, ResetPoliciesTest001, TestSize.Level1)
{
    int32_t ret = instance_.ResetPolicies();
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, ResetPoliciesTest002, TestSize.Level1)
{
    sptr<NetPolicyBaseService> service = new (std::nothrow) TestNetPolicyService();
    instance_.RegisterPolicyService(service);
    int32_t ret = instance_.ResetPolicies();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, ResetEthernetFactoryTest001, TestSize.Level1)
{
    int32_t ret = instance_.ResetEthernetFactory();
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, ResetEthernetFactoryTest002, TestSize.Level1)
{
    sptr<NetEthernetBaseService> service = new (std::nothrow) TestNetEthernetService();
    instance_.RegisterEthernetService(service);
    int32_t ret = instance_.ResetEthernetFactory();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, RestrictBackgroundChangedTest001, TestSize.Level1)
{
    int32_t ret = instance_.RestrictBackgroundChanged(true);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, RestrictBackgroundChangedTest002, TestSize.Level1)
{
    int32_t ret = instance_.RestrictBackgroundChanged(false);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, IsUidNetAccessTest001, TestSize.Level1)
{
    bool ret = instance_.IsUidNetAccess(0, false);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerCenterTest, IsUidNetAccessTest002, TestSize.Level1)
{
    bool ret = instance_.IsUidNetAccess(0, true);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerCenterTest, IsUidNetAllowedTest001, TestSize.Level1)
{
    bool ret = instance_.IsUidNetAllowed(0, true);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerCenterTest, IsUidNetAllowedTest002, TestSize.Level1)
{
    bool ret = instance_.IsUidNetAllowed(0, false);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerCenterTest, NetManagerCenterBranchTest001, TestSize.Level1)
{
    instance_.RegisterPolicyService(nullptr);
    std::list<std::string> list;
    int32_t ret = instance_.GetIfaceNames(NetBearType::BEARER_DEFAULT, list);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    std::string ifaceName;
    ret = instance_.GetIfaceNameByType(NetBearType::BEARER_DEFAULT, TEST_IDENT, ifaceName);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    uint32_t supplierId = 0;
    ret = instance_.UpdateNetLinkInfo(supplierId, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = instance_.UpdateNetSupplierInfo(supplierId, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = instance_.RegisterNetConnCallback(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    instance_.RegisterStatsService(nullptr);
    ret = instance_.ResetStatsFactory();
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = instance_.ResetPolicyFactory();
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = instance_.ResetPolicies();
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    instance_.RegisterEthernetService(nullptr);
    ret = instance_.ResetEthernetFactory();
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = instance_.RestrictBackgroundChanged(false);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    ret = instance_.RegisterNetFactoryResetCallback(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, NetManagerCenterBranchTest002, TestSize.Level1)
{
    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(connService);
    NetBearType bearerType = BEARER_CELLULAR;
    std::set<NetCap> netCaps{NET_CAPABILITY_INTERNET};
    std::string ident = "ident";
    int32_t result = instance_.RegisterNetSupplier(bearerType, ident, netCaps, supplierId_);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = instance_.UnregisterNetSupplier(supplierId_);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    uint32_t supplierId = 0;
    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    result = instance_.UpdateNetLinkInfo(supplierId, netLinkInfo);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = instance_.UpdateNetSupplierInfo(supplierId, nullptr);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = instance_.RegisterNetConnCallback(nullptr);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = instance_.RestrictBackgroundChanged(false);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    sptr<NetStatsBaseService> netStatsService = new (std::nothrow) TestNetStatsService();
    instance_.RegisterStatsService(netStatsService);
    int32_t ret = instance_.ResetStatsFactory();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    sptr<NetPolicyBaseService> netPolicyService = new (std::nothrow) TestNetPolicyService();
    instance_.RegisterPolicyService(netPolicyService);
    EXPECT_EQ(instance_.ResetPolicyFactory(), NETMANAGER_SUCCESS);
}

HWTEST_F(NetManagerCenterTest, IsUidNetAccessTest003, TestSize.Level1)
{
    sptr<NetPolicyBaseService> netPolicyService = new (std::nothrow) TestNetPolicyService();
    instance_.RegisterPolicyService(netPolicyService);
    bool ret = instance_.IsUidNetAccess(0, true);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerCenterTest, IsUidNetAllowedTest003, TestSize.Level1)
{
    sptr<NetPolicyBaseService> netPolicyService = new (std::nothrow) TestNetPolicyService();
    instance_.RegisterPolicyService(netPolicyService);
    bool ret = instance_.IsUidNetAllowed(0, false);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerCenterTest, RegisterNetFactoryResetCallbackTest001, TestSize.Level1)
{
    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(connService);
    auto ret = instance_.RegisterNetFactoryResetCallback(nullptr);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, UpdateUidLostDelay, TestSize.Level1)
{
    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(connService);
    std::set<uint32_t> uidLostDelaySet;
    uidLostDelaySet.insert(1000);
    auto ret = instance_.UpdateUidLostDelay(uidLostDelaySet);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, NetManagerCenterBranchTest003, TestSize.Level1)
{
    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(connService);
 
    int32_t netId = 100;
    NetLinkInfo info;
    int32_t result = instance_.GetConnectionProperties(netId, info);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
 
    instance_.RegisterConnService(nullptr);
    result = instance_.GetConnectionProperties(netId, info);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, RegisterDualStackProbeCallback, TestSize.Level1)
{
    std::shared_ptr<IDualStackProbeCallback> cb = nullptr;
    instance_.RegisterConnService(nullptr);
    int32_t testNetId = 111;
    auto ret = instance_.RegisterDualStackProbeCallback(testNetId, cb);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(connService);
    ret = instance_.RegisterDualStackProbeCallback(testNetId, cb);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, UnRegisterDualStackProbeCallback, TestSize.Level1)
{
    std::shared_ptr<IDualStackProbeCallback> cb = nullptr;
    instance_.RegisterConnService(nullptr);
    int32_t testNetId = 111;
    auto ret = instance_.UnRegisterDualStackProbeCallback(testNetId, cb);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(connService);
    ret = instance_.UnRegisterDualStackProbeCallback(testNetId, cb);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, DualStackProbe, TestSize.Level1)
{
    instance_.RegisterConnService(nullptr);
    int32_t testNetId = 111;
    auto ret = instance_.DualStackProbe(testNetId);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(connService);
    ret = instance_.DualStackProbe(testNetId);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, UpdateDualStackProbeTime, TestSize.Level1)
{
    instance_.RegisterConnService(nullptr);
    int32_t testProbeTime = 5 * 1000;
    auto ret = instance_.UpdateDualStackProbeTime(testProbeTime);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    instance_.RegisterConnService(connService);
    ret = instance_.UpdateDualStackProbeTime(testProbeTime);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, NotifyAllowConnectVpnBundleNameChanged0001, TestSize.Level1)
{
    NetManagerCenter netManagerCenter;
    std::set<std::string> allowConnectVpnBundleName;
    allowConnectVpnBundleName.insert("test");
    std::set<std::string> allowVpnStartWithoutCheckPermissions;
    netManagerCenter.NotifyAllowConnectVpnBundleNameChanged(std::move(allowConnectVpnBundleName),
        std::move(allowVpnStartWithoutCheckPermissions));
    EXPECT_EQ(netManagerCenter.vpnService_, nullptr);
    EXPECT_FALSE(allowConnectVpnBundleName.empty());
}

HWTEST_F(NetManagerCenterTest, NotifyAllowConnectVpnBundleNameChanged0002, TestSize.Level1)
{
    auto vpnService = std::make_shared<TestNetVpnService>();
    NetManagerCenter netManagerCenter;
    netManagerCenter.RegisterVpnService(vpnService);
    std::set<std::string> allowConnectVpnBundleName;
    allowConnectVpnBundleName.insert("test");
    std::set<std::string> allowVpnStartWithoutCheckPermissions;
    netManagerCenter.NotifyAllowConnectVpnBundleNameChanged(std::move(allowConnectVpnBundleName),
        std::move(allowVpnStartWithoutCheckPermissions));
    EXPECT_NE(netManagerCenter.vpnService_, nullptr);
    EXPECT_TRUE(allowConnectVpnBundleName.empty());
    EXPECT_FALSE(vpnService->allowConnectVpnBundleName_.empty());
    EXPECT_TRUE(allowConnectVpnBundleName.empty());
}

HWTEST_F(NetManagerCenterTest, UpdateUidDeadFlowResetTest001, TestSize.Level1)
{
    instance_.RegisterConnService(nullptr);
    std::vector<std::string> bundleNameVec;
    bundleNameVec.push_back("com.test.bundle");
    auto ret = instance_.UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, UpdateUidDeadFlowResetTest002, TestSize.Level1)
{
    NetManagerCenter netManagerCenter;
    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    netManagerCenter.RegisterConnService(connService);
    std::vector<std::string> bundleNameVec;
    bundleNameVec.push_back("com.test.bundle1");
    bundleNameVec.push_back("com.test.bundle2");
    auto ret = netManagerCenter.UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, UpdateUidDeadFlowResetTest003, TestSize.Level1)
{
    NetManagerCenter netManagerCenter;
    sptr<NetConnBaseService> connService = new (std::nothrow) TestConnService();
    netManagerCenter.RegisterConnService(connService);
    std::vector<std::string> bundleNameVec;
    auto ret = netManagerCenter.UpdateUidDeadFlowReset(bundleNameVec);
    EXPECT_NE(ret, NETMANAGER_ERROR);
}

HWTEST_F(NetManagerCenterTest, RegisterNetRequestControlFunc001, TestSize.Level1)
{
    NetManagerCenter netManagerCenter;
    auto func = [](const NetRequest & netRequest) -> bool { return true; };
    bool ret = netManagerCenter.RegisterNetRequestControlFunc(func);
    EXPECT_FALSE(ret);
    sptr<NetConnBaseService> connService = sptr<TestConnService>::MakeSptr();
    netManagerCenter.RegisterConnService(connService);
    ret = netManagerCenter.RegisterNetRequestControlFunc(func);
    EXPECT_TRUE(true);
}

HWTEST_F(NetManagerCenterTest, GetAllNetRequest001, TestSize.Level1)
{
    NetManagerCenter netManagerCenter;
    std::vector<NetRequest> netRequests;
    bool ret = netManagerCenter.GetAllNetRequest(netRequests);
    EXPECT_FALSE(ret);
    sptr<NetConnBaseService> connService = sptr<TestConnService>::MakeSptr();
    netManagerCenter.RegisterConnService(connService);
    ret = netManagerCenter.GetAllNetRequest(netRequests);
    EXPECT_TRUE(true);
}

HWTEST_F(NetManagerCenterTest, UpdateNetRequestControlState001, TestSize.Level1)
{
    NetManagerCenter netManagerCenter;
    std::vector<NetRequest> netRequests;
    bool ret = netManagerCenter.UpdateNetRequestControlState(netRequests);
    EXPECT_FALSE(ret);
    sptr<NetConnBaseService> connService = sptr<TestConnService>::MakeSptr();
    netManagerCenter.RegisterConnService(connService);
    ret = netManagerCenter.UpdateNetRequestControlState(netRequests);
    EXPECT_TRUE(true);
}

HWTEST_F(NetManagerCenterTest, IsAppUidInWhiteListTest001, TestSize.Level1)
{
    NetManagerCenter netManagerCenter;
    int32_t callingUid = 1000;
    int32_t appUid = 2000;
    bool ret = netManagerCenter.IsAppUidInWhiteList(callingUid, appUid);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetManagerCenterTest, IsAppUidInWhiteListTest002, TestSize.Level1)
{
    auto vpnService = std::make_shared<TestNetVpnService>();
    NetManagerCenter netManagerCenter;
    netManagerCenter.RegisterVpnService(vpnService);
    int32_t callingUid = 1000;
    int32_t appUid = 2000;
    bool ret = netManagerCenter.IsAppUidInWhiteList(callingUid, appUid);
    EXPECT_TRUE(ret);
}
} // namespace NetManagerStandard
} // namespace OHOS
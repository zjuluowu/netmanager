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

#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "vnic_manager.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class VnicManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void VnicManagerTest::SetUpTestCase() {}

void VnicManagerTest::TearDownTestCase() {}

void VnicManagerTest::SetUp() {}

void VnicManagerTest::TearDown() {}

HWTEST_F(VnicManagerTest, CreateVnicInterface001, TestSize.Level1)
{
    auto result = VnicManager::GetInstance().CreateVnicInterface();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    VnicManager::GetInstance().DestroyVnicInterface();
}

HWTEST_F(VnicManagerTest, SetVnicMtu001, TestSize.Level1)
{
    std::string ifName = "";
    int32_t testNumber = 0;
    auto result = VnicManager::GetInstance().SetVnicMtu(ifName, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    testNumber = 1;
    result = VnicManager::GetInstance().SetVnicMtu(ifName, testNumber);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VnicManagerTest, SetVnicAddress001, TestSize.Level1)
{
    std::string ifName = "";
    std::string tunAddr = "";
    int32_t testNumber = 0;
    auto result = VnicManager::GetInstance().SetVnicAddress(ifName, tunAddr, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VnicManagerTest, SetVnicUp001, TestSize.Level1)
{
    auto result = VnicManager::GetInstance().SetVnicUp();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VnicManagerTest, SetVnicDown001, TestSize.Level1)
{
    auto result = VnicManager::GetInstance().SetVnicDown();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VnicManagerTest, InitIfreq001, TestSize.Level1)
{
    ifreq ifr;
    std::string cardName = "";
    auto result = VnicManager::GetInstance().InitIfreq(ifr, cardName);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VnicManagerTest, CreateWithDestroyVnic001, TestSize.Level1)
{
    uint16_t mtu = 1500;
    const std::string tunAddr = "192.168.1.100";
    int32_t prefix = 24;
    std::set<int32_t> uids = {5527};
    auto result = VnicManager::GetInstance().CreateVnic(mtu, tunAddr, prefix, uids);
    EXPECT_EQ(result, NETMANAGER_SUCCESS);

    result = VnicManager::GetInstance().DelDefaultRoute();

    result = VnicManager::GetInstance().AddDefaultRoute();

    result = VnicManager::GetInstance().DestroyVnic();
    EXPECT_EQ(result, NETMANAGER_SUCCESS);
}

HWTEST_F(VnicManagerTest, CreateWithDestroyVnic002, TestSize.Level1)
{
    uint16_t mtu = 1500;
    const std::string tunAddr = "192.168.1.100";
    int32_t prefix = 24;
    std::set<int32_t> uids;
    for (int32_t i = 1; i <= 30; ++i) {
        uids.insert(i);
    }

    auto result = VnicManager::GetInstance().CreateVnic(mtu, tunAddr, prefix, uids);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VnicManagerTest, CreateWithDestroyVnic003, TestSize.Level1)
{
    uint16_t mtu = 0;
    const std::string tunAddr = "192.168.1.100";
    int32_t prefix = 24;
    const std::set<int32_t> uids = {5527};
    auto result = VnicManager::GetInstance().CreateVnic(mtu, tunAddr, prefix, uids);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VnicManagerTest, DestroyVnicInterface001, TestSize.Level1)
{
    VnicManager vnicmanager;
    vnicmanager.net4Sock_ = 0;
    vnicmanager.net6Sock_ = 0;
    vnicmanager.tunFd_ = 0;
    vnicmanager.DestroyVnicInterface();
    EXPECT_EQ(vnicmanager.net4Sock_, 0);
    EXPECT_EQ(vnicmanager.net6Sock_, 0);
    EXPECT_EQ(vnicmanager.tunFd_, 0);
}

HWTEST_F(VnicManagerTest, SetVnicMtu002, TestSize.Level1)
{
    std::string ifName = "12345678901234567890";
    int32_t mtu = 1;
    auto result = VnicManager::GetInstance().SetVnicMtu(ifName, mtu);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VnicManagerTest, SetVnicAddress002, TestSize.Level1)
{
    std::string ifName = "12345678901234567890";
    std::string tunAddr = "2001:db8:85a3::8a2e:370:7334";
    int32_t testNumber = 0;
    auto result = VnicManager::GetInstance().SetVnicAddress(ifName, tunAddr, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    ifName = "";
    result = VnicManager::GetInstance().SetVnicAddress(ifName, tunAddr, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    tunAddr = "192.168.1.1";
    result = VnicManager::GetInstance().SetVnicAddress(ifName, tunAddr, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);
    testNumber = 32;
    result = VnicManager::GetInstance().SetVnicAddress(ifName, tunAddr, testNumber);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VnicManagerTest, InitIfreq002, TestSize.Level1)
{
    ifreq ifr;
    std::string cardName = "12345678901234567890";
    auto result = VnicManager::GetInstance().InitIfreq(ifr, cardName);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}

HWTEST_F(VnicManagerTest, SetVnicAddressIpv6PrefixBoundary, TestSize.Level1)
{
    VnicManager vnicmanager;
    std::string ifName = "vnic-tun";
    std::string tunAddr = "2001:db8:85a3::8a2e:370:7334";

    auto result = vnicmanager.SetVnicAddress(ifName, tunAddr, 0);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    result = vnicmanager.SetVnicAddress(ifName, tunAddr, -1);
    EXPECT_EQ(result, NETMANAGER_ERROR);

    result = vnicmanager.SetVnicAddress(ifName, tunAddr, 128);
    EXPECT_EQ(result, NETMANAGER_ERROR);
}
} // namespace NetManagerStandard
} // namespace OHOS

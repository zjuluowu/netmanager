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

#include "message_parcel.h"
#ifdef GTEST_API_
#define private public
#endif
#include "common_net_conn_callback_test.h"
#include "i_net_conn_callback.h"
#include "net_conn_client.h"
#include "net_conn_constants.h"
#include "net_factoryreset_callback_stub.h"
#include "net_interface_callback_stub.h"
#include "net_interface_config.h"
#include "net_manager_constants.h"
#include "netmanager_base_test_security.h"
#include "network.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
} // namespace

class NetConnClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetConnClientTest::SetUpTestCase() {}

void NetConnClientTest::TearDownTestCase() {}

void NetConnClientTest::SetUp() {}

void NetConnClientTest::TearDown() {}

class INetFactoryResetCallbackTest : public IRemoteStub<INetFactoryResetCallback> {
public:
    INetFactoryResetCallbackTest() = default;

    int32_t OnNetFactoryReset()
    {
        return 0;
    }
};

HWTEST_F(NetConnClientTest, EnableVnicNetwork001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    sptr<NetManagerStandard::NetLinkInfo> linkInfo = nullptr;
    std::set<int32_t> uids;

    int32_t ret = NetConnClient::GetInstance().EnableVnicNetwork(linkInfo, uids);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnClientTest, EnableVnicNetwork002, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    sptr<NetManagerStandard::NetLinkInfo> linkInfo = nullptr;
    std::set<int32_t> uids;

    linkInfo = new (std::nothrow) NetManagerStandard::NetLinkInfo();
    ASSERT_NE(linkInfo, nullptr);

    int32_t ret = NetConnClient::GetInstance().EnableVnicNetwork(linkInfo, uids);
    EXPECT_EQ(ret, NET_CONN_ERR_INVALID_NETWORK);
}

HWTEST_F(NetConnClientTest, EnableVnicNetwork003, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    sptr<NetManagerStandard::NetLinkInfo> linkInfo = nullptr;
    std::set<int32_t> uids;

    linkInfo = new (std::nothrow) NetManagerStandard::NetLinkInfo();
    ASSERT_NE(linkInfo, nullptr);

    NetManagerStandard::INetAddr inetAddr;
    inetAddr.type_ = NetManagerStandard::INetAddr::IpType::IPV4;
    inetAddr.family_ = 0x01;
    inetAddr.address_ = "10.0.0.2";
    inetAddr.netMask_ = "255.255.255.0";
    inetAddr.hostName_ = "localhost";
    inetAddr.port_ = 80;
    inetAddr.prefixlen_ = 24;

    linkInfo->ifaceName_ = "vnic-tun";
    linkInfo->netAddrList_.push_back(inetAddr);
    linkInfo->mtu_ = 1500;

    int32_t ret = NetConnClient::GetInstance().EnableVnicNetwork(linkInfo, uids);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnClientTest, DisableVnicNetwork001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret = NetConnClient::GetInstance().DisableVnicNetwork();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

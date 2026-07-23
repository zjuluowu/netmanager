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

#include <arpa/inet.h>
#include <cstdio>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <sys/types.h>

#define private public
#include "fwmark_client.h"
#include "fwmark_network.cpp"
#include "netsys_sock_client.h"
#undef private
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "singleton.h"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace nmd;
namespace {
constexpr int32_t NETID_FIRST = 101;
constexpr int32_t NETID_SECOND = 102;
static constexpr const int32_t ERROR_CODE_SOCKETFD_INVALID = -1;
static constexpr const int32_t ERROR_CODE_CONNECT_FAILED = -2;
static constexpr const int32_t ERROR_CODE_SENDMSG_FAILED = -3;
static constexpr const int32_t ERROR_CODE_READ_FAILED = -4;
class ManagerNative : public std::enable_shared_from_this<ManagerNative> {
    DECLARE_DELAYED_SINGLETON(ManagerNative);

public:
    std::shared_ptr<FwmarkClient> GetFwmarkClient();

private:
    std::shared_ptr<FwmarkClient> fwmarkClient_ = nullptr;
};

ManagerNative::ManagerNative()
{
    fwmarkClient_ = std::make_shared<FwmarkClient>();
}

std::shared_ptr<FwmarkClient> ManagerNative::GetFwmarkClient()
{
    return fwmarkClient_;
}

ManagerNative::~ManagerNative() {}
} // namespace
class UnitTestFwmarkClient : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<FwmarkClient> fwmarkClient = DelayedSingleton<ManagerNative>::GetInstance()->GetFwmarkClient();
};

void UnitTestFwmarkClient::SetUpTestCase() {}

void UnitTestFwmarkClient::TearDownTestCase() {}

void UnitTestFwmarkClient::SetUp() {}

void UnitTestFwmarkClient::TearDown() {}

/**
 * @tc.name: BindSocketTest001
 * @tc.desc: Test FwmarkClient BindSocket.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, BindSocketTest001, TestSize.Level1)
{
    int32_t udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int32_t ret = fwmarkClient->BindSocket(udpSocket, NETID_FIRST);
    NETNATIVE_LOGI("UnitTestFwmarkClient BindSocketTest001 ret=%{public}d", ret);
    close(udpSocket);
    udpSocket = -1;
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

/**
 * @tc.name: BindSocketTest002
 * @tc.desc: Test FwmarkClient BindSocket.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, BindSocketTest002, TestSize.Level1)
{
    int32_t tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int32_t ret = fwmarkClient->BindSocket(tcpSocket, NETID_SECOND);
    NETNATIVE_LOGI("UnitTestFwmarkClient BindSocketTest002 ret=%{public}d", ret);
    close(tcpSocket);
    tcpSocket = -1;
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

/**
 * @tc.name: BindSocketTest003
 * @tc.desc: Test FwmarkClient BindSocket.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, BindSocketTest003, TestSize.Level1)
{
    int32_t tcpSocket = -1;
    int32_t ret = fwmarkClient->BindSocket(tcpSocket, NETID_SECOND);
    NETNATIVE_LOGI("UnitTestFwmarkClient BindSocketTest002 ret=%{public}d", ret);
    close(tcpSocket);
    tcpSocket = -1;
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: HandleErrorTest
 * @tc.desc: Test FwmarkClient BindSocket.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, HandleErrorTest, TestSize.Level1)
{
    int32_t ret = -1;
    int32_t sock = -1;
    int32_t errorCode = ERROR_CODE_SOCKETFD_INVALID;
    ret = fwmarkClient->HandleError(ret, errorCode, sock);
    EXPECT_EQ(ret, -1);

    errorCode = ERROR_CODE_CONNECT_FAILED;
    ret = fwmarkClient->HandleError(ret, errorCode, sock);
    EXPECT_EQ(ret, -1);

    errorCode = ERROR_CODE_SENDMSG_FAILED;
    ret = fwmarkClient->HandleError(ret, errorCode, sock);
    EXPECT_EQ(ret, -1);

    errorCode = ERROR_CODE_READ_FAILED;
    ret = fwmarkClient->HandleError(ret, errorCode, sock);
    EXPECT_EQ(ret, -1);

    errorCode = 100;
    ret = fwmarkClient->HandleError(ret, errorCode, sock);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: CloseSocketTest001
 * @tc.desc: Test FwmarkNetwork CloseSocket.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, CloseSocketTest001, TestSize.Level1)
{
    int32_t socket = 32;
    int32_t ret = -1;
    CloseSocket(nullptr, ret, NO_ERROR_CODE);
    CloseSocket(&socket, ret, ERROR_CODE_RECVMSG_FAILED);
    CloseSocket(&socket, ret, ERROR_CODE_SOCKETFD_INVALID);
    CloseSocket(&socket, ret, ERROR_CODE_WRITE_FAILED);
    CloseSocket(&socket, ret, ERROR_CODE_GETSOCKOPT_FAILED);
    CloseSocket(&socket, ret, ERROR_CODE_SETSOCKOPT_FAILED);
    CloseSocket(&socket, ret, ERROR_CODE_SETSOCKOPT_FAILED - 1);
    EXPECT_EQ(socket, -1);
}

/**
 * @tc.name: SetMarkTest001
 * @tc.desc: Test FwmarkNetwork SetMark.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, SetMarkTest001, TestSize.Level1)
{
    FwmarkCommand cmd;
    auto ret = SetMark(nullptr, &cmd);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: SetMarkTest002
 * @tc.desc: Test FwmarkNetwork SetMark.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, SetMarkTest002, TestSize.Level1)
{
    int32_t socketFd = 0;
    auto ret = SetMark(&socketFd, nullptr);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: SetMarkTest003
 * @tc.desc: Test FwmarkNetwork SetMark.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, SetMarkTest003, TestSize.Level1)
{
    int32_t socketFd = 1111;
    FwmarkCommand cmd;
    auto ret = SetMark(&socketFd, &cmd);
    EXPECT_NE(ret, 0);
    EXPECT_EQ(socketFd, -1);
}

/**
 * @tc.name: SetMarkTest004
 * @tc.desc: Test FwmarkNetwork SetMark.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, SetMarkTest004, TestSize.Level1)
{
    FwmarkCommand cmd;
    int32_t tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int32_t ret = fwmarkClient->BindSocket(tcpSocket, NETID_SECOND);
    ASSERT_EQ(ret, 0);
    cmd.cmdId = FwmarkCommand::SELECT_NETWORK;
    cmd.netId = NETID_UNSET;
    ret = SetMark(&tcpSocket, &cmd);
    close(tcpSocket);
    tcpSocket = -1;
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: SetMarkTest005
 * @tc.desc: Test FwmarkNetwork SetMark.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, SetMarkTest005, TestSize.Level1)
{
    FwmarkCommand cmd;
    int32_t tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int32_t ret = fwmarkClient->BindSocket(tcpSocket, NETID_SECOND);
    ASSERT_EQ(ret, 0);
    cmd.cmdId = FwmarkCommand::SELECT_NETWORK;
    cmd.netId = 100;
    ret = SetMark(&tcpSocket, &cmd);
    close(tcpSocket);
    tcpSocket = -1;
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: SetMarkTest006
 * @tc.desc: Test FwmarkNetwork SetMark.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, SetMarkTest006, TestSize.Level1)
{
    FwmarkCommand cmd;
    int32_t tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int32_t ret = fwmarkClient->BindSocket(tcpSocket, NETID_SECOND);
    ASSERT_EQ(ret, 0);
    cmd.cmdId = FwmarkCommand::PROTECT_FROM_VPN;
    cmd.netId = 100;
    ret = SetMark(&tcpSocket, &cmd);
    close(tcpSocket);
    tcpSocket = -1;
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: SetMarkTest008
 * @tc.desc: Test FwmarkNetwork ProtectFromVpn.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, SetMarkTest008, TestSize.Level1)
{
    int32_t socketFd = 1111;
    auto ret = fwmarkClient->ProtectFromVpn(socketFd);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

/**
 * @tc.name: SetMarkTest009
 * @tc.desc: Test FwmarkNetwork ProtectFromVpn.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, SetMarkTest009, TestSize.Level1)
{
    int32_t socketFd = -1;
    auto ret = fwmarkClient->ProtectFromVpn(socketFd);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERROR);
}

/**
 * @tc.name: CloseSocketTest002
 * @tc.desc: Test FwmarkNetwork CloseSocket.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, CloseSocketTest002, TestSize.Level1)
{
    int32_t socket = 32;
    int32_t ret = -1;
    CloseSocket(&socket, ret, ERROR_CODE_SOCKETFD_INVALID);
    EXPECT_EQ(socket, -1);
}

/**
 * @tc.name: RunForClientFdTest001
 * @tc.desc: Test FwmarkNetwork RunForClientFd.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, RunForClientFdTest001, TestSize.Level1)
{
    int32_t socket = 32;
    RunForClientFd(socket);
    EXPECT_TRUE(socket == -1 || socket == 32);
}

/**
 * @tc.name: HookSocketTest001
 * @tc.desc: Test FwmarkNetwork HookSocketTest.
 * @tc.type: FUNC
 */
HWTEST_F(UnitTestFwmarkClient, HookSocketTest001, TestSize.Level1)
{
    SetProtectFromVpn();
    int32_t tcpSocket1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int32_t tcpSocket2 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    EXPECT_TRUE(tcpSocket1 != -1 && tcpSocket2 != -1);
}
} // namespace NetsysNative
} // namespace OHOS

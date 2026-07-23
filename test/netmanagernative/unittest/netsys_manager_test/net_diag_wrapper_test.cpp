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
#include <thread>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "common_net_diag_callback_test.h"
#include "net_diag_callback_stub.h"
#include "net_diag_wrapper.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_ipc_interface_code.h"
#include "netsys_net_diag_data.h"
#include "thread"

namespace OHOS {
namespace NetsysNative {
using namespace testing::ext;
using namespace OHOS::nmd;

const std::string PING_DESTINATION_IP1 = "127.0.0.1";
const std::string PING_SOURCE_IP = "127.0.0.1";
const uint32_t PING_INTERVAL = 3;
const uint16_t PING_COUNT = 10;
const uint16_t PING_DATASIZE = 256;
const uint16_t PING_MARK = 1;
const uint16_t PING_TTL = 240;
const uint16_t PING_TIMEOUT = 1;
const uint16_t PING_DURATION = 10;
const bool PING_FLOOD = false;
const uint32_t PING_WAIT_MS = 1;
const std::string IFACENAME1 = "eth0";
const std::string IFACENAME2 = "eth1";
const std::string IFACENAME3 = "wlan0";
const std::string TEST_STRING_VALUE = "test";
const uint16_t TEST_UINT16_VALUE = 1;
const uint32_t TEST_UINT32_VALUE = 2;
const uint32_t PING_TIMEOUT_EXIT = 10;

class NetDiagWrapperTest : public testing::Test {
public:
    NetDiagWrapperTest();
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    sptr<NetDiagCallbackStubTest> ptrCallback = new NetDiagCallbackStubTest();
};

NetDiagWrapperTest::NetDiagWrapperTest()
{
    NETNATIVE_LOGI("NetDiagWrapperTest create");
}

void NetDiagWrapperTest::SetUpTestCase() {}

void NetDiagWrapperTest::TearDownTestCase() {}

void NetDiagWrapperTest::SetUp() {}

void NetDiagWrapperTest::TearDown() {}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;

    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest001 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest002 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest002 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest003 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    pingOption.dataSize_ = PING_DATASIZE;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest003 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest004, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest004 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest004 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest005, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest005 enter");
    std::cout << "NetDiagWrapperTest  RunPingCommandTest005 enter" << std::endl;
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    pingOption.duration_ = PING_DURATION;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest005 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest006, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest006 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    pingOption.duration_ = PING_DURATION;
    pingOption.flood_ = PING_FLOOD;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest006 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest007, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest007 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    pingOption.duration_ = PING_DURATION;
    pingOption.flood_ = PING_FLOOD;
    pingOption.interval_ = PING_INTERVAL;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest007 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest008, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest008 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    pingOption.duration_ = PING_DURATION;
    pingOption.flood_ = PING_FLOOD;
    pingOption.interval_ = PING_INTERVAL;
    pingOption.mark_ = PING_MARK;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest008 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest009, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest009 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    pingOption.duration_ = PING_DURATION;
    pingOption.flood_ = PING_FLOOD;
    pingOption.interval_ = PING_INTERVAL;
    pingOption.mark_ = PING_MARK;
    pingOption.source_ = PING_SOURCE_IP;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest009 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t times = PING_TIMEOUT_EXIT;
    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
        times--;
        if (times == 0) {
            break;
        }
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest010, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest010 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;
    pingOption.count_ = PING_COUNT;
    pingOption.duration_ = PING_DURATION;
    pingOption.flood_ = PING_FLOOD;
    pingOption.interval_ = PING_INTERVAL;
    pingOption.mark_ = PING_MARK;
    pingOption.source_ = PING_SOURCE_IP;
    pingOption.timeOut_ = PING_TIMEOUT;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest010 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t times = PING_TIMEOUT_EXIT;
    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
        times--;
        if (times == 0) {
            break;
        }
    }
}

HWTEST_F(NetDiagWrapperTest, RunPingCommandTest011, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest011 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();
    NetDiagPingOption pingOption;
    pingOption.destination_ = PING_DESTINATION_IP1;

    pingOption.count_ = PING_COUNT;
    pingOption.duration_ = PING_DURATION;
    pingOption.flood_ = PING_FLOOD;
    pingOption.interval_ = PING_INTERVAL;
    pingOption.mark_ = PING_MARK;
    pingOption.source_ = PING_SOURCE_IP;
    pingOption.timeOut_ = PING_TIMEOUT;
    pingOption.ttl_ = PING_TTL;
    g_waitPingSync = true;
    auto ret = netDiagWrapper->PingHost(pingOption, ptrCallback);
    NETNATIVE_LOGI("NetDiagWrapperTest  RunPingCommandTest011 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    int32_t times = PING_TIMEOUT_EXIT;
    while (g_waitPingSync) {
        std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
        times--;
        if (times == 0) {
            break;
        }
    }
}

HWTEST_F(NetDiagWrapperTest, RunGetRouteTableCommandTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunGetRouteTableCommandTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::list<NetDiagRouteTable> routeTables;
    auto ret = netDiagWrapper->GetRouteTable(routeTables);

    for (auto const &rtable : routeTables) {
        NETNATIVE_LOGI(
            "NetDiagWrapperTest RunGetRouteTableCommandTest001 enter destination_:%{public}s gateway_:%{public}s "
            "mask_:%{public}s iface_:%{public}s flags_: %{public}s metric_:%{public}d ref_:%{public}d use_:%{public}d ",
            rtable.destination_.c_str(), rtable.gateway_.c_str(), rtable.mask_.c_str(), rtable.iface_.c_str(),
            rtable.flags_.c_str(), rtable.metric_, rtable.ref_, rtable.use_);
    }
    NETNATIVE_LOGI("NetDiagWrapperTest RunGetRouteTableCommandTest001 enter ret = %{public}d  %{public}d", ret,
                   NetManagerStandard::NETMANAGER_SUCCESS);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

void ShowSocketInfo(NetDiagSocketsInfo &info)
{
    for (const auto &lt : info.netProtoSocketsInfo_) {
        NETNATIVE_LOGI(
            "ShowSocketInfo NeyDiagNetProtoSocketInfo protocol_:%{public}s"
            "state_:%{public}s user_:%{public}s programName_:%{public}s recvQueue_:%{public}d "
            "sendQueue_:%{public}d inode_:%{public}d",
            lt.protocol_.c_str(), lt.state_.c_str(), lt.user_.c_str(),
            lt.programName_.c_str(), lt.recvQueue_, lt.sendQueue_, lt.inode_);
    }

    for (const auto &lt : info.unixSocketsInfo_) {
        NETNATIVE_LOGI(
            "ShowSocketInfo  unixSocketsInfo_ refCnt_:%{public}d inode_:%{public}d protocol_:%{public}s \
             flags_:%{public}s  type_:%{public}s  state_:%{public}s path_:%{public}s",
            lt.refCnt_, lt.inode_, lt.protocol_.c_str(), lt.flags_.c_str(), lt.type_.c_str(), lt.state_.c_str(),
            lt.path_.c_str());
    }
}

HWTEST_F(NetDiagWrapperTest, RunGetRouteTableCommandTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunGetRouteTableCommandTest002 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    NetDiagSocketsInfo socketInfo;
    NetDiagProtocolType socketType = NetDiagProtocolType::PROTOCOL_TYPE_ALL;
    auto ret = netDiagWrapper->GetSocketsInfo(socketType, socketInfo);
    ShowSocketInfo(socketInfo);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    socketType = NetDiagProtocolType::PROTOCOL_TYPE_TCP;
    ret = netDiagWrapper->GetSocketsInfo(socketType, socketInfo);
    ShowSocketInfo(socketInfo);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    socketType = NetDiagProtocolType::PROTOCOL_TYPE_UDP;
    ret = netDiagWrapper->GetSocketsInfo(socketType, socketInfo);
    ShowSocketInfo(socketInfo);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    socketType = NetDiagProtocolType::PROTOCOL_TYPE_UNIX;
    ret = netDiagWrapper->GetSocketsInfo(socketType, socketInfo);
    ShowSocketInfo(socketInfo);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    socketType = NetDiagProtocolType::PROTOCOL_TYPE_RAW;
    ret = netDiagWrapper->GetSocketsInfo(socketType, socketInfo);
    ShowSocketInfo(socketInfo);
    EXPECT_GE(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(PING_WAIT_MS));
}

HWTEST_F(NetDiagWrapperTest, RunInterFaceConfigCommandTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunInterFaceConfigCommandTest001 ");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::list<NetDiagIfaceConfig> configs;

    auto ret = netDiagWrapper->GetInterfaceConfig(configs, IFACENAME1);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netDiagWrapper->GetInterfaceConfig(configs, IFACENAME2);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netDiagWrapper->GetInterfaceConfig(configs, IFACENAME3);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetDiagWrapperTest, RunInterFaceConfigCommandTest002, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunInterFaceConfigCommandTest002 ");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::list<NetDiagIfaceConfig> configs;

    NetDiagIfaceConfig config;
    config.ifaceName_ = IFACENAME1;
    const std::string ifaceName = IFACENAME1;
    config.ipv4Addr_ = "192.168.222.234";
    config.mtu_ = 1000;
    config.ipv4Mask_ = "255.255.255.0";
    config.ipv4Bcast_ = "255.255.255.0";
    config.txQueueLen_ = 1000;
    bool add = true;

    auto ret = netDiagWrapper->UpdateInterfaceConfig(config, IFACENAME1, add);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netDiagWrapper->UpdateInterfaceConfig(config, IFACENAME1, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetDiagWrapperTest, RunInterFaceConfigCommandTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunInterFaceConfigCommandTest003 ");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    auto ret = netDiagWrapper->SetInterfaceActiveState(IFACENAME1, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netDiagWrapper->SetInterfaceActiveState(IFACENAME1, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netDiagWrapper->SetInterfaceActiveState(IFACENAME2, false);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);

    ret = netDiagWrapper->SetInterfaceActiveState(IFACENAME2, true);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetDiagWrapperTest, NetDiagWrapperBranchTest001, TestSize.Level1)
{
    PingIcmpResponseInfo responseInfo;
    responseInfo.bytes_ = TEST_UINT16_VALUE;
    responseInfo.icmpSeq_ = TEST_UINT16_VALUE;
    responseInfo.ttl_ = TEST_UINT16_VALUE;
    responseInfo.costTime_ = TEST_UINT32_VALUE;
    responseInfo.from_ = TEST_STRING_VALUE;
    Parcel parcel;
    auto ret = responseInfo.Marshalling(parcel);
    EXPECT_TRUE(ret);
    ret = responseInfo.Unmarshalling(parcel, responseInfo);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetDiagWrapperTest, NetDiagWrapperBranchTest002, TestSize.Level1)
{
    NetDiagPingResult pingResult;
    pingResult.host_ = TEST_STRING_VALUE;
    pingResult.ipAddr_ = TEST_STRING_VALUE;
    pingResult.dateSize_ = TEST_UINT16_VALUE;
    pingResult.payloadSize_ = TEST_UINT16_VALUE;
    pingResult.transCount_ = TEST_UINT16_VALUE;
    pingResult.recvCount_ = TEST_UINT16_VALUE;
    Parcel parcel;
    auto ret = pingResult.Marshalling(parcel);
    EXPECT_TRUE(ret);
    ret = pingResult.Unmarshalling(parcel, pingResult);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetDiagWrapperTest, NetDiagWrapperBranchTest003, TestSize.Level1)
{
    NetDiagRouteTable routeTable;
    routeTable.destination_ = TEST_STRING_VALUE;
    routeTable.gateway_ = TEST_STRING_VALUE;
    routeTable.mask_ = TEST_STRING_VALUE;
    routeTable.iface_ = TEST_STRING_VALUE;
    routeTable.flags_ = TEST_STRING_VALUE;
    routeTable.metric_ = TEST_UINT32_VALUE;
    routeTable.ref_ = TEST_UINT32_VALUE;
    routeTable.use_ = TEST_UINT32_VALUE;
    Parcel parcel;
    auto ret = routeTable.Marshalling(parcel);
    EXPECT_TRUE(ret);
    ret = routeTable.Unmarshalling(parcel, routeTable);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetDiagWrapperTest, NetDiagWrapperBranchTest004, TestSize.Level1)
{
    NetDiagUnixSocketInfo socketInfo;
    socketInfo.refCnt_ = TEST_UINT16_VALUE;
    socketInfo.inode_ = TEST_UINT32_VALUE;
    socketInfo.protocol_ = TEST_STRING_VALUE;
    socketInfo.flags_ = TEST_STRING_VALUE;
    socketInfo.type_ = TEST_STRING_VALUE;
    socketInfo.state_ = TEST_STRING_VALUE;
    socketInfo.path_ = TEST_STRING_VALUE;
    Parcel parcel;
    auto ret = socketInfo.Marshalling(parcel);
    EXPECT_TRUE(ret);
    ret = socketInfo.Unmarshalling(parcel, socketInfo);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetDiagWrapperTest, NetDiagWrapperBranchTest005, TestSize.Level1)
{
    NeyDiagNetProtoSocketInfo protoSocketInfo;
    protoSocketInfo.protocol_ = TEST_STRING_VALUE;
    protoSocketInfo.localAddr_ = TEST_STRING_VALUE;
    protoSocketInfo.foreignAddr_ = TEST_STRING_VALUE;
    protoSocketInfo.state_ = TEST_STRING_VALUE;
    protoSocketInfo.user_ = TEST_STRING_VALUE;
    protoSocketInfo.programName_ = TEST_STRING_VALUE;
    protoSocketInfo.recvQueue_ = TEST_UINT16_VALUE;
    protoSocketInfo.sendQueue_ = TEST_UINT16_VALUE;
    protoSocketInfo.inode_ = TEST_UINT32_VALUE;
    Parcel parcel;
    auto ret = protoSocketInfo.Marshalling(parcel);
    EXPECT_TRUE(ret);
    ret = protoSocketInfo.Unmarshalling(parcel, protoSocketInfo);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetDiagWrapperTest, RunGetRouteTableCommandTest003, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  RunGetRouteTableCommandTest003 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    NetDiagSocketsInfo socketInfo;
    NetDiagProtocolType socketType = static_cast<NetDiagProtocolType>(5);
    auto ret = netDiagWrapper->GetSocketsInfo(socketType, socketInfo);
    ShowSocketInfo(socketInfo);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetDiagWrapperTest, ExecuteCommandForResultTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExecuteCommandForResultTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::string command = "";
    std::string result = "";
    auto ret = netDiagWrapper->ExecuteCommandForResult(command, result);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetDiagWrapperTest, GeneratePingCommandTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  GeneratePingCommandTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    NetDiagPingOption pingOption;
    pingOption.destination_ = "123";
    pingOption.flood_ = true;
    std::string command = "";
    auto ret = netDiagWrapper->GeneratePingCommand(pingOption, command);
    EXPECT_EQ(ret, NetManagerStandard::NETMANAGER_SUCCESS);
}

HWTEST_F(NetDiagWrapperTest, ExtractPingResultTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractPingResultTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::string result = "";
    sptr<INetDiagCallback> callback = nullptr;
    netDiagWrapper->ExtractPingResult(result, callback);
    EXPECT_EQ(callback, nullptr);
}

HWTEST_F(NetDiagWrapperTest, ExtractPingHeaderTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractPingHeaderTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagPingResult pingResult;
    netDiagWrapper->ExtractPingHeader(match, pingResult);
    EXPECT_EQ(pingResult.host_, "");
}

HWTEST_F(NetDiagWrapperTest, ExtractIcmpSeqInfoTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractIcmpSeqInfoTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagPingResult pingResult;
    netDiagWrapper->ExtractIcmpSeqInfo(match, pingResult);
    EXPECT_EQ(pingResult.host_, "");
}

HWTEST_F(NetDiagWrapperTest, ExtractPingStatisticsTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractPingStatisticsTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagPingResult pingResult;
    netDiagWrapper->ExtractPingStatistics(match, pingResult);
    EXPECT_EQ(pingResult.host_, "");
}

HWTEST_F(NetDiagWrapperTest, ExtractRouteTableInfoTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractRouteTableInfoTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    std::list<NetDiagRouteTable> routeTables;
    netDiagWrapper->ExtractRouteTableInfo(match, routeTables);
    EXPECT_EQ(match.size(), 0);
}

HWTEST_F(NetDiagWrapperTest, ExtractNetProtoSocketsInfoTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractNetProtoSocketsInfoTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagSocketsInfo pingResult;
    netDiagWrapper->ExtractNetProtoSocketsInfo(match, pingResult);
    EXPECT_EQ(match.size(), 0);
}

HWTEST_F(NetDiagWrapperTest, ExtractUnixSocketsInfoTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractUnixSocketsInfoTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagSocketsInfo pingResult;
    netDiagWrapper->ExtractUnixSocketsInfo(match, pingResult);
    EXPECT_EQ(match.size(), 0);
}

HWTEST_F(NetDiagWrapperTest, ExtractIfaceNameTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractIfaceNameTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagIfaceConfig pingResult;
    netDiagWrapper->ExtractIfaceName(match, pingResult);
    EXPECT_EQ(match.size(), 0);
}

HWTEST_F(NetDiagWrapperTest, ExtractIfaceInetTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractIfaceInetTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagIfaceConfig pingResult;
    netDiagWrapper->ExtractIfaceInet(match, pingResult);
    EXPECT_EQ(match.size(), 0);
}

HWTEST_F(NetDiagWrapperTest, ExtractIfaceInetTest6001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractIfaceInet6Test001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagIfaceConfig pingResult;
    netDiagWrapper->ExtractIfaceInet6(match, pingResult);
    EXPECT_EQ(match.size(), 0);
}

HWTEST_F(NetDiagWrapperTest, ExtractIfaceMtuTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractIfaceMtuTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagIfaceConfig pingResult;
    netDiagWrapper->ExtractIfaceMtu(match, pingResult);
    EXPECT_EQ(match.size(), 0);
}

HWTEST_F(NetDiagWrapperTest, ExtractIfaceTxQueueLenTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractIfaceTxQueueLenTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagIfaceConfig pingResult;
    netDiagWrapper->ExtractIfaceTxQueueLen(match, pingResult);
    EXPECT_EQ(match.size(), 0);
}

HWTEST_F(NetDiagWrapperTest, ExtractIfaceTransDataBytesTest001, TestSize.Level1)
{
    NETNATIVE_LOGI("NetDiagWrapperTest  ExtractIfaceTransDataBytesTest001 enter");
    auto netDiagWrapper = NetDiagWrapper::GetInstance();

    std::smatch match;
    NetDiagIfaceConfig pingResult;
    netDiagWrapper->ExtractIfaceTransDataBytes(match, pingResult);
    EXPECT_EQ(match.size(), 0);
}
} // namespace NetsysNative
} // namespace OHOS

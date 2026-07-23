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
#include <iostream>
#include <memory>

#include "common_net_conn_callback_test.h"
#include "i_net_conn_service.h"
#include "i_net_detection_callback.h"
#include "i_net_factoryreset_callback.h"
#include "refresh_http_proxy_callback_stub.h"
#include "net_all_capabilities.h"
#include "net_conn_service_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
namespace {
constexpr int32_t TEST_UID = 1010;
constexpr const char *TEST_IDENT = "testIdent";
constexpr uint32_t TEST_TIMEOUTMS = 1000;
constexpr const char *TEST_HOST = "testHost";
constexpr int32_t TEST_NETID = 3;
constexpr int32_t TEST_SOCKETFD = 2;
constexpr int32_t TEST_SUPPLIERID = 1021;

uint32_t g_supplierId = 0;
enum DeadFlowReplyMode {
    DEAD_FLOW_REPLY_NORMAL = 0,
    DEAD_FLOW_REPLY_SKIP_RESULT,
    DEAD_FLOW_REPLY_SKIP_BOOL,
    DEAD_FLOW_REPLY_RESULT_ERROR,
};

class MockNetIRemoteObject : public IRemoteObject {
public:
    MockNetIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}
    ~MockNetIRemoteObject() {}

    int32_t GetObjectRefCount() override
    {
        return 0;
    }

    void HandleGetIfaceNamesReply(MessageParcel &reply)
    {
        reply.WriteUint32(NETMANAGER_SUCCESS);
    }

    void HandleGetIfaceNameByTypeReply(MessageParcel &reply)
    {
        reply.WriteString(TEST_HOST);
    }

    void HandleGetDefaultNetReply(MessageParcel &reply)
    {
        reply.WriteInt32(TEST_NETID);
    }

    void HandleHasDefaultNetReply(MessageParcel &reply)
    {
        reply.WriteBool(true);
    }

    void HandleGetConnectionPropertiesReply(MessageParcel &reply)
    {
        NetLinkInfo linkInfo;
        linkInfo.ifaceName_ = "ifacename_test";
        linkInfo.Marshalling(reply);
    }

    void HandleGetNetCapabilitiesReply(MessageParcel &reply)
    {
        NetAllCapabilities netCap;
        netCap.Marshalling(reply);
    }

    void HandleGetHttpProxyReply(MessageParcel &reply)
    {
        HttpProxy httpProxy;
        httpProxy.Marshalling(reply);
    }

    void HandleGetConnectOwnerUidReply(MessageParcel &reply)
    {
        reply.WriteInt32(TEST_UID);
    }

    void HandleDeadFlowResetTargetBundleReply(MessageParcel &reply)
    {
        if (deadFlowReplyMode_ == DEAD_FLOW_REPLY_SKIP_RESULT) {
            return;
        }
        if (deadFlowReplyMode_ == DEAD_FLOW_REPLY_RESULT_ERROR) {
            reply.WriteInt32(NETMANAGER_ERR_INTERNAL);
            return;
        }
        reply.WriteInt32(NETMANAGER_SUCCESS);
        if (deadFlowReplyMode_ == DEAD_FLOW_REPLY_SKIP_BOOL) {
            return;
        }
        reply.WriteBool(true);
    }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        switch (code) {
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_IFACE_NAMES):
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_SPECIFIC_NET):
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_ALL_NETS):
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_NET_ID_BY_IDENTIFIER):
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_GET_IFACENAME_IDENT_MAPS):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleGetIfaceNamesReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_IFACENAME_BY_TYPE):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleGetIfaceNameByTypeReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GETDEFAULTNETWORK):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleGetDefaultNetReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_HASDEFAULTNET):
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_IS_DEFAULT_NET_METERED):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleHasDefaultNetReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_CONNECTION_PROPERTIES):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleGetConnectionPropertiesReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_NET_CAPABILITIES):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleGetNetCapabilitiesReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_GLOBAL_HTTP_PROXY):
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_DEFAULT_HTTP_PROXY):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleGetHttpProxyReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_REFRESH_GLOBAL_HTTP_PROXY):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleGetHttpProxyReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_CONNECT_OWNER_UID):
                reply.WriteInt32(NETMANAGER_SUCCESS);
                HandleGetConnectOwnerUidReply(reply);
                break;
            case static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_DEAD_FLOW_RESET_TARGET_BUNDLE):
                HandleDeadFlowResetTargetBundleReply(reply);
                break;
            default:
                reply.WriteInt32(NETMANAGER_SUCCESS);
                reply.WriteUint32(TEST_SUPPLIERID);
                break;
        }

        return eCode;
    }

    bool IsProxyObject() const override
    {
        return true;
    }

    bool CheckObjectLegality() const override
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface() override
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return 0;
    }

    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }

    void SetErrorCode(int errorCode)
    {
        eCode = errorCode;
    }

    void SetDeadFlowReplyMode(DeadFlowReplyMode mode)
    {
        deadFlowReplyMode_ = mode;
    }

private:
    int eCode = NETMANAGER_SUCCESS;
    DeadFlowReplyMode deadFlowReplyMode_ = DEAD_FLOW_REPLY_NORMAL;
};

class NetDetectionTestCallback : public IRemoteStub<INetDetectionCallback> {
public:
    int32_t OnNetDetectionResultChanged(NetDetectionResultCode resultCode, const std::string &urlRedirect) override
    {
        return 0;
    }
};

class NetFactoryResetTestCallback : public IRemoteStub<INetFactoryResetCallback> {
public:
    int32_t OnNetFactoryReset() override
    {
        return 0;
    }
};

class NetConnServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline sptr<MockNetIRemoteObject> remoteObj_ = std::make_unique<MockNetIRemoteObject>().release();
    static inline std::shared_ptr<NetConnServiceProxy> instance_ = std::make_shared<NetConnServiceProxy>(remoteObj_);
    static inline sptr<INetSupplierCallback> supplierCallback_ = new (std::nothrow) NetSupplierCallbackStub();
    static inline sptr<INetConnCallback> netConnCallback_ = new (std::nothrow) NetConnCallbackStubCb();
    static inline sptr<INetDetectionCallback> detectionCallback_ = new (std::nothrow) NetDetectionTestCallback();
    static inline sptr<INetFactoryResetCallback> netFactoryResetCallback_ =
        new (std::nothrow) NetFactoryResetTestCallback();
};

void NetConnServiceProxyTest::SetUpTestCase() {}

void NetConnServiceProxyTest::TearDownTestCase() {}

void NetConnServiceProxyTest::SetUp() {}

void NetConnServiceProxyTest::TearDown()
{
    remoteObj_->SetErrorCode(NETMANAGER_SUCCESS);
    remoteObj_->SetDeadFlowReplyMode(DEAD_FLOW_REPLY_NORMAL);
}

/**
 * @tc.name: SystemReadyTest001
 * @tc.desc: Test NetConnServiceProxy SystemReady.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, SystemReadyTest001, TestSize.Level1)
{
    int32_t ret = instance_->SystemReady();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetInternetPermissionTest001
 * @tc.desc: Test NetConnServiceProxy SetInternetPermission.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, SetInternetPermissionTest001, TestSize.Level1)
{
    int32_t ret = instance_->SetInternetPermission(TEST_UID, true);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RegisterNetSupplierTest001
 * @tc.desc: Test NetConnServiceProxy RegisterNetSupplier.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, RegisterNetSupplierTest001, TestSize.Level1)
{
    std::set<NetCap> netCaps;
    int32_t ret = instance_->RegisterNetSupplier(NetBearType::BEARER_ETHERNET, TEST_IDENT, netCaps, g_supplierId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UnregisterNetSupplierTest001
 * @tc.desc: Test NetConnServiceProxy UnregisterNetSupplier.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, UnregisterNetSupplierTest001, TestSize.Level1)
{
    int32_t ret = instance_->UnregisterNetSupplier(g_supplierId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RegisterNetSupplierCallbackTest001
 * @tc.desc: Test NetConnServiceProxy RegisterNetSupplierCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, RegisterNetSupplierCallbackTest001, TestSize.Level1)
{
    int32_t ret = instance_->RegisterNetSupplierCallback(g_supplierId, supplierCallback_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RegisterNetConnCallbackTest001
 * @tc.desc: Test NetConnServiceProxy RegisterNetConnCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, RegisterNetConnCallbackTest001, TestSize.Level1)
{
    int32_t ret = instance_->RegisterNetConnCallback(netConnCallback_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RegisterNetConnCallbackTest002
 * @tc.desc: Test NetConnServiceProxy RegisterNetConnCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, RegisterNetConnCallbackTest002, TestSize.Level1)
{
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    int32_t ret = instance_->RegisterNetConnCallback(netSpecifier, netConnCallback_, TEST_TIMEOUTMS);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RequestNetConnectionTest001
 * @tc.desc: Test NetConnServiceProxy RequestNetConnection.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, RequestNetConnectionTest001, TestSize.Level1)
{
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    int32_t ret = instance_->RequestNetConnection(netSpecifier, netConnCallback_, TEST_TIMEOUTMS);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UnregisterNetConnCallbackTest001
 * @tc.desc: Test NetConnServiceProxy UnregisterNetConnCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, UnregisterNetConnCallbackTest001, TestSize.Level1)
{
    int32_t ret = instance_->UnregisterNetConnCallback(netConnCallback_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UpdateNetStateForTest001
 * @tc.desc: Test NetConnServiceProxy UpdateNetStateForTest.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, UpdateNetStateForTest001, TestSize.Level1)
{
    int32_t netState = 0;
    sptr<NetSpecifier> netSpecifier = new (std::nothrow) NetSpecifier();
    int32_t ret = instance_->UpdateNetStateForTest(netSpecifier, netState);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UpdateNetSupplierInfoTest001
 * @tc.desc: Test NetConnServiceProxy UpdateNetSupplierInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, UpdateNetSupplierInfoTest001, TestSize.Level1)
{
    sptr<NetSupplierInfo> netSupplierInfo = new (std::nothrow) NetSupplierInfo();
    int32_t ret = instance_->UpdateNetSupplierInfo(g_supplierId, netSupplierInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UpdateNetLinkInfoTest001
 * @tc.desc: Test NetConnServiceProxy UpdateNetLinkInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, UpdateNetLinkInfoTest001, TestSize.Level1)
{
    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    int32_t ret = instance_->UpdateNetLinkInfo(g_supplierId, netLinkInfo);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RegisterNetDetectionCallbackTest001
 * @tc.desc: Test NetConnServiceProxy RegisterNetDetectionCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, RegisterNetDetectionCallbackTest001, TestSize.Level1)
{
    int32_t ret = instance_->RegisterNetDetectionCallback(TEST_NETID, detectionCallback_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UnRegisterNetDetectionCallbackTest001
 * @tc.desc: Test NetConnServiceProxy UnRegisterNetDetectionCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, UnRegisterNetDetectionCallbackTest001, TestSize.Level1)
{
    int32_t ret = instance_->UnRegisterNetDetectionCallback(TEST_NETID, detectionCallback_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetDetectionTest001
 * @tc.desc: Test NetConnServiceProxy NetDetection.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, NetDetectionTest001, TestSize.Level1)
{
    int32_t ret = instance_->NetDetection(TEST_NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetDetectionTest002
 * @tc.desc: Test NetConnServiceProxy NetDetection.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, NetDetectionTest002, TestSize.Level1)
{
    std::string rawUrl = "";
    PortalResponse resp;
    int32_t ret = instance_->NetDetection(rawUrl, resp);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_REPLY_FAIL);
}

/**
 * @tc.name: GetIfaceNamesTest001
 * @tc.desc: Test NetConnServiceProxy GetIfaceNames.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetIfaceNamesTest001, TestSize.Level1)
{
    std::list<std::string> ifaceNames;
    int32_t ret = instance_->GetIfaceNames(NetBearType::BEARER_ETHERNET, ifaceNames);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetIfaceNameByTypeTest001
 * @tc.desc: Test NetConnServiceProxy GetIfaceNameByType.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetIfaceNameByTypeTest001, TestSize.Level1)
{
    std::string ifaceName;
    int32_t ret = instance_->GetIfaceNameByType(NetBearType::BEARER_ETHERNET, TEST_IDENT, ifaceName);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetDefaultNetTest001
 * @tc.desc: Test NetConnServiceProxy GetDefaultNet.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetDefaultNetTest001, TestSize.Level1)
{
    int32_t netId = 0;
    int32_t ret = instance_->GetDefaultNet(netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: HasDefaultNetTest001
 * @tc.desc: Test NetConnServiceProxy HasDefaultNet.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, HasDefaultNetTest001, TestSize.Level1)
{
    bool hasDefaultNet = false;
    int32_t ret = instance_->HasDefaultNet(hasDefaultNet);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(hasDefaultNet, true);
}

/**
 * @tc.name: GetSpecificNetTest001
 * @tc.desc: Test NetConnServiceProxy GetSpecificNet.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetSpecificNetTest001, TestSize.Level1)
{
    std::list<int32_t> netIdList;
    int32_t ret = instance_->GetSpecificNet(NetBearType::BEARER_ETHERNET, netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetSpecificNetByIdentTest001
 * @tc.desc: Test NetConnServiceProxy GetSpecificNetByIdent.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetSpecificNetByIdentTest001, TestSize.Level1)
{
    std::list<int32_t> netIdList;
    int32_t ret = instance_->GetSpecificNetByIdent(NetBearType::BEARER_ETHERNET, "test", netIdList);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERR_READ_REPLY_FAIL);
}

/**
 * @tc.name: GetAllNetsTest001
 * @tc.desc: Test NetConnServiceProxy GetAllNets.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetAllNetsTest001, TestSize.Level1)
{
    std::list<int32_t> netIdList;
    int32_t ret = instance_->GetAllNets(netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetSpecificUidNetTest001
 * @tc.desc: Test NetConnServiceProxy GetSpecificUidNet.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetSpecificUidNetTest001, TestSize.Level1)
{
    int32_t netId = 0;
    int32_t ret = instance_->GetSpecificUidNet(TEST_UID, netId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetConnectionPropertiesTest001
 * @tc.desc: Test NetConnServiceProxy GetConnectionProperties.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetConnectionPropertiesTest001, TestSize.Level1)
{
    NetLinkInfo info;
    int32_t ret = instance_->GetConnectionProperties(TEST_NETID, info);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetNetExtAttributeTest001
 * @tc.desc: Test NetConnServiceProxy SetNetExtAttribute.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, SetNetExtAttributeTest001, TestSize.Level1)
{
    instance_->SetNetExtAttribute(TEST_NETID, "test");
    std::string str;
    int32_t ret = instance_->GetNetExtAttribute(TEST_NETID, str);
    EXPECT_TRUE(ret == NETMANAGER_ERR_INTERNAL || ret == NETMANAGER_ERR_READ_REPLY_FAIL);
}

/**
 * @tc.name: GetNetExtAttributeTest001
 * @tc.desc: Test NetConnServiceProxy GetNetExtAttribute.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetNetExtAttributeTest001, TestSize.Level1)
{
    int32_t defaultNetId = 0;
    instance_->GetDefaultNet(defaultNetId);
    instance_->SetNetExtAttribute(defaultNetId, "test");
    std::string str;
    instance_->GetNetExtAttribute(defaultNetId, str);
    EXPECT_TRUE(str == "test" || str == "");
}

/**
 * @tc.name: GetNetCapabilitiesTest001
 * @tc.desc: Test NetConnServiceProxy GetNetCapabilities.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetNetCapabilitiesTest001, TestSize.Level1)
{
    NetAllCapabilities netAllCap;
    int32_t ret = instance_->GetNetCapabilities(TEST_NETID, netAllCap);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetIfaceNameIdentMapsTest001
 * @tc.desc: Test NetConnServiceProxy GetIfaceNameIdentMaps.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetIfaceNameIdentMapsTest001, TestSize.Level1)
{
    SafeMap<std::string, std::string> data;
    int32_t ret = instance_->GetIfaceNameIdentMaps(NetBearType::BEARER_CELLULAR, data);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetAirplaneModeTest001
 * @tc.desc: Test NetConnServiceProxy SetAirplaneMode.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, SetAirplaneModeTest001, TestSize.Level1)
{
    bool airplaneMode = true;
    int32_t ret = instance_->SetAirplaneMode(airplaneMode);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: IsDefaultNetMeteredTest001
 * @tc.desc: Test NetConnServiceProxy IsDefaultNetMetered.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, IsDefaultNetMeteredTest001, TestSize.Level1)
{
    bool isMetered;
    int32_t ret = instance_->IsDefaultNetMetered(isMetered);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetGlobalHttpProxyTest001
 * @tc.desc: Test NetConnServiceProxy SetGlobalHttpProxy.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, SetGlobalHttpProxyTest001, TestSize.Level1)
{
    HttpProxy proxy;
    proxy.SetHost(TEST_HOST);
    int32_t ret = instance_->SetGlobalHttpProxy(proxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetGlobalHttpProxyTest001
 * @tc.desc: Test NetConnServiceProxy GetGlobalHttpProxy.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetGlobalHttpProxyTest001, TestSize.Level1)
{
    HttpProxy proxy;
    int32_t ret = instance_->GetGlobalHttpProxy(proxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RefreshGlobalHttpProxyTest001
 * @tc.desc: Test NetConnServiceProxy RefreshGlobalHttpProxy.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, RefreshGlobalHttpProxyTest001, TestSize.Level1)
{
    sptr<IRefreshHttpProxyCallback> callback = new RefreshHttpProxyCallbackStub();
    int32_t ret = instance_->RefreshGlobalHttpProxy(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, RefreshGlobalHttpProxyTest002, TestSize.Level1)
{
    sptr<IRefreshHttpProxyCallback> callback = nullptr;
    int32_t ret = instance_->RefreshGlobalHttpProxy(callback);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetNetIdByIdentifierTest001
 * @tc.desc: Test NetConnServiceProxy GetNetIdByIdentifier.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, GetNetIdByIdentifierTest001, TestSize.Level1)
{
    std::list<int32_t> netIdList;
    int32_t ret = instance_->GetNetIdByIdentifier(TEST_IDENT, netIdList);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetAppNetTest001
 * @tc.desc: Test NetConnServiceProxy SetAppNet.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, SetAppNetTest001, TestSize.Level1)
{
    int32_t ret = instance_->SetAppNet(TEST_NETID);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: FactoryResetNetworkTest001
 * @tc.desc: Test NetConnServiceProxy FactoryResetNetwork.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, FactoryResetNetworkTest001, TestSize.Level1)
{
    int32_t ret = instance_->FactoryResetNetwork();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RegisterNetConnCallbackTest001
 * @tc.desc: Test NetConnServiceProxy RegisterNetFactoryResetCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, RegisterNetFactoryResetCallbackTest001, TestSize.Level1)
{
    int32_t ret = instance_->RegisterNetFactoryResetCallback(netFactoryResetCallback_);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NetConnServiceProxyBranchTest001
 * @tc.desc: Test NetConnServiceProxy Branch.
 * @tc.type: FUNC
 */
HWTEST_F(NetConnServiceProxyTest, NetConnServiceProxyBranchTest001, TestSize.Level1)
{
    sptr<INetSupplierCallback> supplierCallback = nullptr;
    int32_t ret = instance_->RegisterNetSupplierCallback(g_supplierId, supplierCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<INetConnCallback> connCallback = nullptr;
    ret = instance_->RegisterNetConnCallback(connCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<NetSpecifier> netSpecifier = nullptr;
    uint32_t timeoutMS = 0;
    ret = instance_->RegisterNetConnCallback(netSpecifier, connCallback, timeoutMS);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = instance_->RequestNetConnection(netSpecifier, connCallback, timeoutMS);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = instance_->UnregisterNetConnCallback(connCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    int32_t netState = 0;
    ret = instance_->UpdateNetStateForTest(netSpecifier, netState);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<INetInterfaceStateCallback> stateCallback = nullptr;
    ret = instance_->RegisterNetInterfaceCallback(stateCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = instance_->UnregisterNetInterfaceCallback(stateCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<INetFactoryResetCallback> netFactoryResetCallback = nullptr;
    ret = instance_->RegisterNetFactoryResetCallback(netFactoryResetCallback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceProxyTest, EnableVnicNetwork001, TestSize.Level1)
{
    sptr<NetManagerStandard::NetLinkInfo> linkInfo = nullptr;
    std::set<int32_t> uids;
    int32_t ret = instance_->EnableVnicNetwork(linkInfo, uids);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(NetConnServiceProxyTest, EnableVnicNetwork002, TestSize.Level1)
{
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

    int32_t ret = instance_->EnableVnicNetwork(linkInfo, uids);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, DisableVnicNetwork001, TestSize.Level1)
{
    int32_t ret = instance_->DisableVnicNetwork();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, CloseSocketsUid001, TestSize.Level1)
{
    int32_t netId = 100;
    uint32_t uid = 20020157;
    int32_t ret = instance_->CloseSocketsUid(netId, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, SetInterfaceTest001, TestSize.Level1)
{
    std::string iface = "wlan0";
    std::string ipAddr = "0.0.0.1";
    
    int32_t ret = instance_->SetInterfaceUp(iface);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->SetNetInterfaceIpAddress(iface, ipAddr);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    ret = instance_->SetInterfaceDown(iface);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, SetReuseSupplierId001, TestSize.Level1)
{
    uint32_t supplierId = 1004;
    uint32_t reuseSupplierId = 1008;
    bool add = false;
    int32_t ret = instance_->SetReuseSupplierId(supplierId, reuseSupplierId, add);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, UpdateSupplierScore001, TestSize.Level1)
{
    uint32_t supplierId = 1004;
    int32_t ret = instance_->UpdateSupplierScore(supplierId, 4);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, GetDefaultSupplierId001, TestSize.Level1)
{
    uint32_t supplierId = 1004;
    int32_t ret = instance_->GetDefaultSupplierId(NetBearType::BEARER_WIFI, TEST_IDENT,
        supplierId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, GetIpNeighTableTest001, TestSize.Level1)
{
    std::vector<NetIpMacInfo> ipMacInfo;
    int32_t ret = instance_->GetIpNeighTable(ipMacInfo);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, CreateVlanTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = instance_->CreateVlan(ifName, vlanId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, DestroyVlanTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    int32_t ret = instance_->DestroyVlan(ifName, vlanId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, AddVlanIpTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    int32_t ret = instance_->AddVlanIp(ifName, vlanId, ip, mask);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, DeleteVlanIpTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    uint32_t vlanId = 1;
    std::string ip = "192.148.1.1";
    uint32_t mask = 24;
    int32_t ret = instance_->DeleteVlanIp(ifName, vlanId, ip, mask);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, GetConnectOwnerUidTest001, TestSize.Level1)
{
    int32_t uid = 0;
    NetConnInfo info;
    info.protocolType_ = IPPROTO_TCP;
    info.family_ = NetConnInfo::Family::IPv4;
    info.localAddress_ = "192.168.1.100";
    info.localPort_ = 1111;
    info.remoteAddress_ = "192.168.1.200";
    info.remotePort_ = 2222;
    int32_t ret = instance_->GetConnectOwnerUid(info, uid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, GetSystemNetPortStatesTest001, TestSize.Level1)
{
    NetPortStatesInfo netPortStatesInfo;
    int32_t ret = instance_->GetSystemNetPortStates(netPortStatesInfo);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERR_READ_REPLY_FAIL);
}

HWTEST_F(NetConnServiceProxyTest, IsDeadFlowResetTargetBundleTest001, TestSize.Level1)
{
    std::string bundleName = "com.test.bundle";
    bool flag = false;
    int32_t ret = instance_->IsDeadFlowResetTargetBundle(bundleName, flag);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(flag);
}

HWTEST_F(NetConnServiceProxyTest, IsDeadFlowResetTargetBundleTest002, TestSize.Level1)
{
    std::string bundleName = "";
    bool flag = false;
    int32_t ret = instance_->IsDeadFlowResetTargetBundle(bundleName, flag);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetConnServiceProxyTest, IsDeadFlowResetTargetBundleTest003, TestSize.Level1)
{
    remoteObj_->SetErrorCode(NETMANAGER_ERR_INTERNAL);
    std::string bundleName = "com.test.bundle";
    bool flag = false;
    int32_t ret = instance_->IsDeadFlowResetTargetBundle(bundleName, flag);
    EXPECT_LE(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceProxyTest, IsDeadFlowResetTargetBundleTest004, TestSize.Level1)
{
    remoteObj_->SetDeadFlowReplyMode(DEAD_FLOW_REPLY_SKIP_RESULT);
    std::string bundleName = "com.test.bundle";
    bool flag = false;
    int32_t ret = instance_->IsDeadFlowResetTargetBundle(bundleName, flag);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_REPLY_FAIL);
}

HWTEST_F(NetConnServiceProxyTest, IsDeadFlowResetTargetBundleTest005, TestSize.Level1)
{
    remoteObj_->SetDeadFlowReplyMode(DEAD_FLOW_REPLY_RESULT_ERROR);
    std::string bundleName = "com.test.bundle";
    bool flag = false;
    int32_t ret = instance_->IsDeadFlowResetTargetBundle(bundleName, flag);
    EXPECT_EQ(ret, NETMANAGER_ERR_INTERNAL);
}

HWTEST_F(NetConnServiceProxyTest, IsDeadFlowResetTargetBundleTest006, TestSize.Level1)
{
    remoteObj_->SetDeadFlowReplyMode(DEAD_FLOW_REPLY_SKIP_BOOL);
    std::string bundleName = "com.test.bundle";
    bool flag = false;
    int32_t ret = instance_->IsDeadFlowResetTargetBundle(bundleName, flag);
    EXPECT_EQ(ret, NETMANAGER_ERR_READ_REPLY_FAIL);
}
}
} // namespace NetManagerStandard
} // namespace OHOS

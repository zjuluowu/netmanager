/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <vector>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif
#include "net_policy_service_stub.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
#define DTEST_LOG std::cout << __func__ << ":" << __LINE__ << ":"
constexpr const char *ETH_IFACE_NAME = "lo";
constexpr int32_t TEST_UID = 1010;
class MockNetPolicyServiceStubTest : public NetPolicyServiceStub {
public:
    MockNetPolicyServiceStubTest() = default;
    ~MockNetPolicyServiceStubTest() = default;
    int32_t SetPolicyByUid(uint32_t uid, uint32_t policy) override
    {
        return 0;
    }

    int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy) override
    {
        return 0;
    }

    int32_t GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids) override
    {
        return 0;
    }

    int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed) override
    {
        return 0;
    }

    int32_t IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed) override
    {
        return 0;
    }

    int32_t RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) override
    {
        return 0;
    }

    int32_t UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) override
    {
        return 0;
    }

    int32_t SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies) override
    {
        return 0;
    }

    int32_t GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies) override
    {
        return 0;
    }

    int32_t UpdateRemindPolicy(int32_t netType, const std::string &simId, uint32_t remindType) override
    {
        return 0;
    }

    int32_t SetDeviceIdleTrustlist(const std::vector<uint32_t> &uids, bool isAllowed) override
    {
        return 0;
    }

    int32_t GetDeviceIdleTrustlist(std::vector<uint32_t> &uids) override
    {
        return 0;
    }

    int32_t SetDeviceIdlePolicy(bool enable) override
    {
        return 0;
    }

    int32_t ResetPolicies(const std::string &simId) override
    {
        return 0;
    }

    int32_t SetBackgroundPolicy(bool isAllowed) override
    {
        return 0;
    }

    int32_t GetBackgroundPolicy(bool &backgroundPolicy) override
    {
        return 0;
    }

    int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid) override
    {
        return 0;
    }

    int32_t GetPowerSaveTrustlist(std::vector<uint32_t> &uids) override
    {
        return 0;
    }

    int32_t SetPowerSaveTrustlist(const std::vector<uint32_t> &uids, bool isAllowed) override
    {
        return 0;
    }

    int32_t SetPowerSavePolicy(bool enable) override
    {
        return 0;
    }

    int32_t CheckPermission() override
    {
        return 0;
    }

    int32_t FactoryResetPolicies() override
    {
        return 0;
    }

    int32_t SetNetworkAccessPolicy(uint32_t uid, NetworkAccessPolicy policy, bool reconfirmFlag) override
    {
        return 0;
    }

    int32_t GetNetworkAccessPolicy(AccessPolicyParameter parameter, AccessPolicySave& policy) override
    {
        return 0;
    }

    int32_t AddNetworkAccessPolicy(const std::vector<std::string> &bundleNames) override
    {
        return 0;
    }

    int32_t RemoveNetworkAccessPolicy(const std::vector<std::string> &bundleNames) override
    {
        return 0;
    }

    int32_t NotifyNetAccessPolicyDiag(uint32_t uid) override
    {
        return 0;
    }

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        bool byPassPolicyPermission = false;
        if (!data.ReadBool(byPassPolicyPermission)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }

        if (!byPassPolicyPermission) {
            return NetPolicyServiceStub::OnRemoteRequest(code, data, reply, option);
        }

        auto itFunc = memberFuncMap_.find(code);
        int32_t result = NETMANAGER_SUCCESS;
        if (itFunc != memberFuncMap_.end()) {
            auto requestFunc = itFunc->second;
            if (requestFunc != nullptr) {
                result = (this->*requestFunc)(data, reply);
                return result;
            }
        }

        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    int32_t SetNicTrafficAllowed(const std::vector<std::string> &ifaceNames, bool status) override
    {
        return 0;
    }

    int32_t SetInternetAccessByIpForWifiShare(
        const std::string &ipAddr, uint8_t family, bool accessInternet, const std::string &clientNetIfName) override
    {
        return 0;
    }

    int32_t SetIdleDenyPolicy(bool enable) override
    {
        return 0;
    }

    int32_t SetUidsDeniedListChain(const std::vector<uint32_t> &uids, bool isAdd) override
    {
        return 0;
    }

    int32_t GetSelfNetworkAccessPolicy(NetAccessPolicy& policy) override
    {
        policy.allowWiFi = true;
        policy.allowCellular = true;
        return 0;
    }
};

} // namespace

using namespace testing::ext;
class NetPolicyServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline sptr<NetPolicyServiceStub> instance_ = new (std::nothrow) MockNetPolicyServiceStubTest();
};

void NetPolicyServiceStubTest::SetUpTestCase() {}

void NetPolicyServiceStubTest::TearDownTestCase()
{
    instance_ = nullptr;
}

void NetPolicyServiceStubTest::SetUp() {}

void NetPolicyServiceStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequestTest001
 * @tc.desc: Test NetPolicyServiceStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnRemoteRequestTest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteBool(false);
    int32_t ret =
        instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_END), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_ERR_DESCRIPTOR_MISMATCH);
}

/**
 * @tc.name: OnRemoteRequestTest002
 * @tc.desc: Test NetPolicyServiceStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnRemoteRequestTest002, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(false);
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyServiceStub::GetDescriptor()), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret =
        instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_END), data, reply, option);
    EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: OnRemoteRequestTest003
 * @tc.desc: Test NetPolicyServiceStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnRemoteRequestTest003, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(false);
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyServiceStub::GetDescriptor()), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POLICY_BY_UID),
                                             data, reply, option);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);

    int32_t result = NETMANAGER_ERROR;
    EXPECT_NE(reply.ReadInt32(result), true);
    EXPECT_LE(result, NETMANAGER_ERR_PERMISSION_DENIED);
}

/**
 * @tc.name: OnSetPolicyByUidTest001
 * @tc.desc: Test NetPolicyServiceStub OnSetPolicyByUid.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetPolicyByUidTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteUint32(TEST_UID);
    data.WriteUint32(0);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POLICY_BY_UID),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetPolicyByUidTest001
 * @tc.desc: Test NetPolicyServiceStub OnGetPolicyByUid.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetPolicyByUidTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteUint32(TEST_UID);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_POLICY_BY_UID),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetUidsByPolicyTest001
 * @tc.desc: Test NetPolicyServiceStub OnGetUidsByPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetUidsByPolicyTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteUint32(TEST_UID);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_UIDS_BY_POLICY),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnIsUidNetAllowedMeteredTest001
 * @tc.desc: Test NetPolicyServiceStub OnIsUidNetAllowedMetered.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnIsUidNetAllowedMeteredTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteUint32(TEST_UID);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_IS_NET_ALLOWED_BY_METERED), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnIsUidNetAllowedIfaceNameTest001
 * @tc.desc: Test NetPolicyServiceStub OnIsUidNetAllowedIfaceName.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnIsUidNetAllowedIfaceNameTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteUint32(TEST_UID);
    data.WriteString(ETH_IFACE_NAME);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_IS_NET_ALLOWED_BY_IFACE), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetNetQuotaPoliciesTest001
 * @tc.desc: Test NetPolicyServiceStub OnSetNetQuotaPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetNetQuotaPoliciesTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    NetQuotaPolicy quotaPolicy;
    quotaPolicy.quotapolicy.title = "test";
    std::vector<NetQuotaPolicy> quotaPolicies;
    quotaPolicies.emplace_back(quotaPolicy);
    NetQuotaPolicy::Marshalling(data, quotaPolicies);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NET_QUOTA_POLICIES),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetNetQuotaPoliciesTest001
 * @tc.desc: Test NetPolicyServiceStub OnGetNetQuotaPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetNetQuotaPoliciesTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_NET_QUOTA_POLICIES),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnResetPoliciesTest001
 * @tc.desc: Test NetPolicyServiceStub OnResetPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnResetPoliciesTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteString("subscriberId");
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_RESET_POLICIES), data,
                                             reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetBackgroundPolicyTest001
 * @tc.desc: Test NetPolicyServiceStub OnSetBackgroundPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetBackgroundPolicyTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_BACKGROUND_POLICY),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetBackgroundPolicyTest001
 * @tc.desc: Test NetPolicyServiceStub OnGetBackgroundPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetBackgroundPolicyTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_BACKGROUND_POLICY),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetBackgroundPolicyByUidTest001
 * @tc.desc: Test NetPolicyServiceStub OnGetBackgroundPolicyByUid.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetBackgroundPolicyByUidTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_BACKGROUND_POLICY_BY_UID), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSnoozePolicyTest001
 * @tc.desc: Test NetPolicyServiceStub OnSnoozePolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSnoozePolicyTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteInt32(0);
    data.WriteString("simId");
    data.WriteInt32(0);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_UPDATE_REMIND_POLICY),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetDeviceIdleTrustlistTest003
 * @tc.desc: Test NetPolicyServiceStub OnSetDeviceIdleTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetDeviceIdleTrustlistTest003, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    std::vector<uint32_t> uids;
    uids.emplace_back(TEST_UID);
    data.WriteUInt32Vector(uids);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_TRUSTLIST),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetDeviceIdleTrustlistTest001
 * @tc.desc: Test NetPolicyServiceStub OnGetDeviceIdleTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetDeviceIdleTrustlistTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_IDLE_TRUSTLIST),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetDeviceIdlePolicyTest001
 * @tc.desc: Test NetPolicyServiceStub OnSetDeviceIdlePolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetDeviceIdlePolicyTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_DEVICE_IDLE_POLICY),
                                             data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetPowerSaveTrustlistTest001
 * @tc.desc: Test NetPolicyServiceStub OnGetPowerSaveTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetPowerSaveTrustlistTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_POWER_SAVE_TRUSTLIST), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetPowerSaveTrustlistTest001
 * @tc.desc: Test NetPolicyServiceStub OnSetPowerSaveTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetPowerSaveTrustlistTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    std::vector<uint32_t> uids;
    uids.emplace_back(TEST_UID);
    data.WriteUInt32Vector(uids);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_POWER_SAVE_TRUSTLIST), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnFactoryResetPoliciesTest001
 * @tc.desc: Test NetPolicyServiceStub OnFactoryResetPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnFactoryResetPoliciesTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_FACTORYRESET_POLICIES), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetNetworkAccessPolicyTest001
 * @tc.desc: Test NetPolicyServiceStub OnSetNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetNetworkAccessPolicyTest001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(false);
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyServiceStub::GetDescriptor()), false);
    bool wifiBool = true;
    bool cellularBool = true;
    data.WriteUint32(TEST_UID);
    data.WriteUint8(wifiBool);
    data.WriteUint8(cellularBool);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    std::cout << TEST_UID << std::endl;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_NETWORK_ACCESS_POLICY), data, reply, option);
    uint32_t callingUid = 1099;
    uint32_t setUID = 20020024;
    NetworkAccessPolicy policy;
    policy.wifiAllow = 0;
    policy.cellularAllow = 1;
    instance_->HandleStoreNetworkPolicy(setUID, policy, callingUid);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyServiceStub OnGetNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetNetworkAccessPolicy001, TestSize.Level1)
{
    MessageParcel data;
    uint32_t userId = 1;
    data.WriteBool(false);
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyServiceStub::GetDescriptor()), false);
    data.WriteBool(true);
    data.WriteInt32(TEST_UID);
    data.WriteUint32(userId);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_NETWORK_ACCESS_POLICY), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnAddNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyServiceStub OnAddNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnAddNetworkAccessPolicy001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    std::vector<std::string> bundleNames = {"com.example.test"};
    data.WriteStringVector(bundleNames);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_ADD_NETWORK_ACCESS_POLICY), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnRemoveNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyServiceStub OnRemoveNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnRemoveNetworkAccessPolicy001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    std::vector<std::string> bundleNames = {"com.example.test"};
    data.WriteStringVector(bundleNames);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_REMOVE_NETWORK_ACCESS_POLICY), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetInternetAccessByIpForWifiShare001
 * @tc.desc: Test NetPolicyServiceStub OnSetInternetAccessByIpForWifiShare.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetInternetAccessByIpForWifiShare001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(false);
    data.WriteInterfaceToken(NetPolicyServiceStub::GetDescriptor());
    std::string ip = "1.1.1.1";
    uint8_t family = 2;
    std::string ifname = "test";
    MessageParcel reply;
    MessageOption option;
    data.WriteString(ip);
    data.WriteUint8(family);
    data.WriteBool(true);
    data.WriteString(ifname);
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_INTERNET_ACCESS_BY_IP_FOR_WIFI_SHARE),
        data, reply, option);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
    
    MessageParcel errData;
    instance_->OnSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_NE(ret, 3);

    errData.WriteString(ip);
    instance_->OnSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_NE(ret, 3);

    errData.WriteString(ip);
    errData.WriteUint8(family);
    instance_->OnSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_NE(ret, 3);

    errData.WriteString(ip);
    errData.WriteUint8(family);
    errData.WriteBool(true);
    instance_->OnSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_NE(ret, 3);

    errData.WriteString(ip);
    errData.WriteUint8(family);
    errData.WriteBool(true);
    errData.WriteString(ifname);
    instance_->OnSetInternetAccessByIpForWifiShare(errData, reply);
    EXPECT_GE(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetIdleDenyPolicy
 * @tc.desc: Test NetPolicyServiceStub OnSetIdleDenyPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetIdleDenyPolicy001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENY_POLICY), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnSetUidsDeniedListChain
 * @tc.desc: Test NetPolicyServiceStub OnSetUidsDeniedListChain.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnSetUidsDeniedListChain001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    data.WriteInt32(1);
    data.WriteInt32(TEST_UID);
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_SET_IDLE_DENYLIST), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetSelfNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyServiceStub OnGetSelfNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetSelfNetworkAccessPolicy001, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(true);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_SELF_NETWORK_ACCESS_POLICY), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnGetSelfNetworkAccessPolicy002
 * @tc.desc: Test NetPolicyServiceStub OnGetSelfNetworkAccessPolicy with interface token.
 * @tc.type: FUNC
 */
HWTEST_F(NetPolicyServiceStubTest, OnGetSelfNetworkAccessPolicy002, TestSize.Level1)
{
    MessageParcel data;
    data.WriteBool(false);
    ASSERT_NE(data.WriteInterfaceToken(NetPolicyServiceStub::GetDescriptor()), false);
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(
        static_cast<uint32_t>(PolicyInterfaceCode::CMD_NPS_GET_SELF_NETWORK_ACCESS_POLICY), data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

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

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <gtest/gtest.h>

#include "iservice_registry.h"

#ifdef GTEST_API_
#define private public
#endif
#include "net_mgr_log_wrapper.h"
#include "net_policy_callback_test.h"
#include "net_policy_client.h"
#include "net_policy_constants.h"
#include "net_policy_inner_define.h"
#include "net_policy_service.h"
#include "netmanager_base_test_security.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
std::shared_ptr<NetPolicyClient> g_netPolicyClient = nullptr;
constexpr int32_t TRIGER_DELAY_US = 100000;
constexpr int32_t WAIT_TIME_SECOND_LONG = 10;
constexpr uint32_t TEST_UID = 10000;
const std::string TEST_STRING_PERIODDURATION = "M1";

NetQuotaPolicy GetQuota()
{
    NetQuotaPolicy quotaPolicy;
    quotaPolicy.networkmatchrule.netType = 0;
    quotaPolicy.networkmatchrule.simId = std::to_string(TRIGER_DELAY_US);
    quotaPolicy.quotapolicy.periodStartTime = TRIGER_DELAY_US;
    quotaPolicy.quotapolicy.periodDuration = TEST_STRING_PERIODDURATION;
    quotaPolicy.quotapolicy.warningBytes = TRIGER_DELAY_US;
    quotaPolicy.quotapolicy.limitBytes = TRIGER_DELAY_US;
    quotaPolicy.quotapolicy.lastLimitRemind = -1;
    quotaPolicy.quotapolicy.metered = true;
    quotaPolicy.quotapolicy.source = 0;
    return quotaPolicy;
}
} // namespace

class UtNetPolicyClient : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<NetPolicyCallbackTest> GetINetPolicyCallbackSample() const;
};

void UtNetPolicyClient::SetUpTestCase()
{
    g_netPolicyClient = DelayedSingleton<NetPolicyClient>::GetInstance();
}

void UtNetPolicyClient::TearDownTestCase() {}

void UtNetPolicyClient::SetUp() {}

void UtNetPolicyClient::TearDown() {}

sptr<NetPolicyCallbackTest> UtNetPolicyClient::GetINetPolicyCallbackSample() const
{
    sptr<NetPolicyCallbackTest> callback = new (std::nothrow) NetPolicyCallbackTest();
    return callback;
}

/**
 * @tc.name: SetPolicyByUid001
 * @tc.desc: Test NetPolicyClient SetPolicyByUid.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetPolicyByUid001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    NetPolicyClient::NetPolicyDeathRecipient deathRecipient(*g_netPolicyClient);
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remote = sam->CheckSystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID);
    deathRecipient.OnRemoteDied(remote);
    int32_t ret = g_netPolicyClient->SetPolicyByUid(TEST_UID, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
    std::cout << "NetPolicyClient001 SetPolicyByUid ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetPolicyByUid001
 * @tc.desc: Test NetPolicyClient GetPolicyByUid.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetPolicyByUid001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    uint32_t policy = 0;
    int32_t ret = g_netPolicyClient->GetPolicyByUid(TEST_UID, policy);
    std::cout << "NetPolicyClient002 GetPolicyByUid policy:" << policy << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ASSERT_EQ(policy, NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND);
}

/**
 * @tc.name: GetUidsByPolicy001
 * @tc.desc: Test NetPolicyClient GetUidsByPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetUidsByPolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<uint32_t> uids;
    int32_t ret = g_netPolicyClient->GetUidsByPolicy(NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND, uids);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ASSERT_TRUE(uids.size() > 0);
}

/**
 * @tc.name: IsUidNetAllowed001
 * @tc.desc: Test NetPolicyClient IsUidNetAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, IsUidNetAllowed001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    bool isAllowed = false;
    int32_t ret = g_netPolicyClient->IsUidNetAllowed(TEST_UID, false, isAllowed);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    std::cout << "NetPolicyClient004 IsUidNetAllowed ret:" << ret << std::endl;
    ASSERT_TRUE(isAllowed == true);
}

/**
 * @tc.name: IsUidNetAllowed002
 * @tc.desc: Test NetPolicyClient IsUidNetAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, IsUidNetAllowed002, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    bool isAllowed = false;
    const std::string ifaceName = "iface";
    int32_t ret = g_netPolicyClient->IsUidNetAllowed(TEST_UID, ifaceName, isAllowed);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    std::cout << "NetPolicyClient005 IsUidNetAllowed isAllowed:" << isAllowed << std::endl;
    ASSERT_TRUE(isAllowed == true);
}

/**
 * @tc.name: IsUidNetAccess001
 * @tc.desc: Test NetPolicyClient IsUidNetAccess.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, IsUidNetAccess001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    bool isAllowed = false;
    int32_t ret = g_netPolicyClient->IsUidNetAccess(TEST_UID, false, isAllowed);
    std::cout << "NetPolicyClient006 IsUidNetAccess isAllowed:" << isAllowed << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ASSERT_TRUE(isAllowed == true);
}

/**
 * @tc.name: IsUidNetAccess002
 * @tc.desc: Test NetPolicyClient IsUidNetAccess.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, IsUidNetAccess002, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    bool isAllowed = false;
    const std::string ifaceName = "iface";
    int32_t ret = g_netPolicyClient->IsUidNetAccess(TEST_UID, ifaceName, isAllowed);
    std::cout << "NetPolicyClient007 IsUidNetAccess isAllowed:" << isAllowed << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ASSERT_TRUE(isAllowed == true);
}

/**
 * @tc.name: SetNetQuotaPolicies001
 * @tc.desc: Test NetPolicyClient SetNetQuotaPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNetQuotaPolicies001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<NetQuotaPolicy> quotaPolicies;
    quotaPolicies.push_back(GetQuota());
    int32_t ret = g_netPolicyClient->SetNetQuotaPolicies(quotaPolicies);
    std::cout << "NetPolicyClient008 SetNetQuotaPolicies ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetNetQuotaPolicies002
 * @tc.desc: Test NetPolicyClient SetNetQuotaPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNetQuotaPolicies002, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<NetQuotaPolicy> quotaPolicies;
    int32_t ret = g_netPolicyClient->SetNetQuotaPolicies(quotaPolicies);
    std::cout << "NetPolicyClient008 SetNetQuotaPolicies ret:" << ret << std::endl;
    ASSERT_EQ(ret, POLICY_ERR_INVALID_QUOTA_POLICY);
}

/**
 * @tc.name: SetNetQuotaPolicies003
 * @tc.desc: Test NetPolicyClient SetNetQuotaPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNetQuotaPolicies003, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<NetQuotaPolicy> quotaPolicies;
    for (int32_t i = 0; i < QUOTA_POLICY_MAX_SIZE; i++) {
        quotaPolicies.push_back(GetQuota());
    }
    quotaPolicies.push_back(GetQuota());
    int32_t ret = g_netPolicyClient->SetNetQuotaPolicies(quotaPolicies);
    std::cout << "NetPolicyClient008 SetNetQuotaPolicies ret:" << ret << std::endl;
    ASSERT_EQ(ret, POLICY_ERR_INVALID_QUOTA_POLICY);
}

/**
 * @tc.name: GetNetQuotaPolicies001
 * @tc.desc: Test NetPolicyClient GetNetQuotaPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetNetQuotaPolicies001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<NetQuotaPolicy> quotaPolicies;
    int32_t ret = g_netPolicyClient->GetNetQuotaPolicies(quotaPolicies);
    std::cout << "NetPolicyClient009 GetNetQuotaPolicies ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetFactoryPolicy001
 * @tc.desc: Test NetPolicyClient SetFactoryPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetFactoryPolicy001, TestSize.Level1)
{
    std::string simId = "0";
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->SetFactoryPolicy(simId);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: ResetPolicies001
 * @tc.desc: Test NetPolicyClient ResetPolicies.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, ResetPolicies001, TestSize.Level1)
{
    std::string simId = "0";
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->ResetPolicies(simId);
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetBackgroundPolicy001
 * @tc.desc: Test NetPolicyClient SetBackgroundPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetBackgroundPolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->SetBackgroundPolicy(true);
    std::cout << "NetPolicyClient012 SetBackgroundPolicy ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

/**
 * @tc.name: GetBackgroundPolicy001
 * @tc.desc: Test NetPolicyClient GetBackgroundPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetBackgroundPolicy001, TestSize.Level1)
{
    bool backgroundPolicy;
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->GetBackgroundPolicy(backgroundPolicy);
    std::cout << "NetPolicyClient013 GetBackgroundPolicy ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
    ASSERT_TRUE(backgroundPolicy == true);
}

/**
 * @tc.name: GetBackgroundPolicyByUid001
 * @tc.desc: Test NetPolicyClient GetBackgroundPolicyByUid.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetBackgroundPolicyByUid001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret1 = g_netPolicyClient->SetBackgroundPolicy(false);
    ASSERT_EQ(ret1, NETMANAGER_SUCCESS);
    uint32_t backgroundPolicyOfUid = 0;

    int32_t ret2 = g_netPolicyClient->GetBackgroundPolicyByUid(TEST_UID, backgroundPolicyOfUid);
    ASSERT_EQ(ret2, NETMANAGER_SUCCESS);
    ASSERT_EQ(backgroundPolicyOfUid, NET_BACKGROUND_POLICY_DISABLE);
}

/**
 * @tc.name: SetSnoozePolicy001
 * @tc.desc: Test NetPolicyClient SetSnoozePolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetSnoozePolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->SetSnoozePolicy(0, std::to_string(TRIGER_DELAY_US));
    std::cout << "NetPolicyClient015 SetSnoozePolicy ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: UpdateRemindPolicy001
 * @tc.desc: Test NetPolicyClient UpdateRemindPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, UpdateRemindPolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret =
        g_netPolicyClient->UpdateRemindPolicy(0, std::to_string(TRIGER_DELAY_US), RemindType::REMIND_TYPE_LIMIT);
    std::cout << "NetPolicyClient016 UpdateRemindPolicy ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetIdleTrustlist001
 * @tc.desc: Test NetPolicyClient SetIdleTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetIdleTrustlist001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->SetIdleTrustlist(TEST_UID, true);
    std::cout << "NetPolicyClient017 SetIdleTrustlist ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetDeviceIdleTrustlist001
 * @tc.desc: Test NetPolicyClient SetDeviceIdleTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetDeviceIdleTrustlist001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->SetDeviceIdleTrustlist({TEST_UID}, true);
    std::cout << "NetPolicyClient018 SetDeviceIdleTrustlist ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetIdleTrustlist001
 * @tc.desc: Test NetPolicyClient GetIdleTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetIdleTrustlist001, TestSize.Level1)
{
    std::vector<uint32_t> uids;
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->GetIdleTrustlist(uids);
    std::cout << "NetPolicyClient019 GetIdleTrustlist ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetDeviceIdleTrustlist001
 * @tc.desc: Test NetPolicyClient GetDeviceIdleTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetDeviceIdleTrustlist001, TestSize.Level1)
{
    std::vector<uint32_t> uids;
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->GetDeviceIdleTrustlist(uids);
    std::cout << "NetPolicyClient020 GetDeviceIdleTrustlist ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetDeviceIdlePolicy001
 * @tc.desc: Test NetPolicyClient SetDeviceIdlePolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetDeviceIdlePolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->SetDeviceIdlePolicy(true);
    std::cout << "NetPolicyClient021 SetDeviceIdlePolicy ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

void PolicyServiceCallback()
{
    NetManagerBaseAccessToken token;
    usleep(TRIGER_DELAY_US);
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetPolicyByUid(
        TEST_UID, NetUidPolicy::NET_POLICY_REJECT_METERED_BACKGROUND);
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}
/**
 * @tc.name: RegisterNetPolicyCallback001
 * @tc.desc: Test NetPolicyClient RegisterNetPolicyCallback UnregisterNetPolicyCallback.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, RegisterNetPolicyCallback001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    sptr<NetPolicyCallbackTest> callback = GetINetPolicyCallbackSample();
    int32_t ret1 = g_netPolicyClient->RegisterNetPolicyCallback(callback);
    if (ret1 == NETMANAGER_SUCCESS && callback != nullptr) {
        std::thread trigerCallback(PolicyServiceCallback);
        callback->WaitFor(WAIT_TIME_SECOND_LONG);
        trigerCallback.join();
        uint32_t uid = callback->GetUid();
        uint32_t netPolicy = callback->GetPolicy();
        std::cout << "NetPolicyClient022 RegisterNetPolicyCallback uid:" << uid
                  << " netPolicy:" << static_cast<uint32_t>(netPolicy) << std::endl;
        ASSERT_EQ(uid, TEST_UID);
        ASSERT_EQ(netPolicy, NetUidPolicy::NET_POLICY_REJECT_METERED_BACKGROUND);
        ASSERT_EQ(ret1, NETMANAGER_SUCCESS);
    } else {
        std::cout << "NetPolicyClient022 RegisterNetPolicyCallback return fail" << std::endl;
    }
    NetManagerBaseAccessToken token1;
    int32_t ret2 = g_netPolicyClient->UnregisterNetPolicyCallback(callback);
    ASSERT_EQ(ret2, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetPowerSaveTrustlist001
 * @tc.desc: Test NetPolicyClient GetPowerSaveTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetPowerSaveTrustlist001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<uint32_t> uids;
    int32_t ret = g_netPolicyClient->GetPowerSaveTrustlist(uids);
    std::cout << "NetPolicyClient023 GetPowerSaveTrustlist ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetPowerSaveTrustlist001
 * @tc.desc: Test NetPolicyClient SetPowerSaveTrustlist.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetPowerSaveTrustlist001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<uint32_t> uids;
    bool isAllowed = true;
    int32_t ret = g_netPolicyClient->SetPowerSaveTrustlist(uids, isAllowed);
    std::cout << "NetPolicyClient024 SetPowerSaveTrustlist ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetPowerSavePolicy001
 * @tc.desc: Test NetPolicyClient SetPowerSavePolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetPowerSavePolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    bool enable = true;
    int32_t ret = g_netPolicyClient->SetPowerSavePolicy(enable);
    std::cout << "NetPolicyClient025 SetPowerSavePolicy ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: CheckPermission001
 * @tc.desc: Test NetPolicyClient CheckPermission.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, CheckPermission001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    int32_t ret = g_netPolicyClient->CheckPermission();
    std::cout << "NetPolicyClient026 CheckPermission ret:" << ret << std::endl;
    ASSERT_EQ(ret, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyClient SetNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNetworkAccessPolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    NetworkAccessPolicy netAccessPolicy;
    netAccessPolicy.wifiAllow = false;
    netAccessPolicy.cellularAllow = false;
    bool reconfirmFlag = true;

    int32_t result1 = g_netPolicyClient->SetNetworkAccessPolicy(
        TEST_UID, netAccessPolicy, reconfirmFlag);
    std::cout << "NetPolicyClient025 SetNetworkAccessPolicy ret:" << result1 << std::endl;
    ASSERT_EQ(result1, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyClient GetNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetNetworkAccessPolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    AccessPolicyParameter parameter;
    parameter.flag = 1;
    parameter.uid = TEST_UID;
    AccessPolicySave resultSave;

    int32_t result1 = g_netPolicyClient->GetNetworkAccessPolicy(parameter, resultSave);
    std::cout << "NetPolicyClient026 GetNetworkAccessPolicy ret:" << result1 << std::endl;
    ASSERT_EQ(result1, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: AddNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyClient AddNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, AddNetworkAccessPolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::string bundleName = "com.example.test";
    int32_t result = g_netPolicyClient->AddNetworkAccessPolicy({bundleName});
    std::cout << "NetPolicyClient026 AddNetworkAccessPolicy ret:" << result << std::endl;
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: RemoveNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyClient RemoveNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, RemoveNetworkAccessPolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::string bundleName = "com.example.test";
    int32_t result = g_netPolicyClient->RemoveNetworkAccessPolicy({bundleName});
    std::cout << "NetPolicyClient026 RemoveNetworkAccessPolicy ret:" << result << std::endl;
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: NotifyNetAccessPolicyDiag001
 * @tc.desc: Test NetPolicyClient NotifyNetAccessPolicyDiag.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, NotifyNetAccessPolicyDiag001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    uint32_t uid = 0;
    int32_t result = g_netPolicyClient->NotifyNetAccessPolicyDiag(uid);
    std::cout << "NetPolicyClient027 NotifyNetAccessPolicyDiag ret:" << result << std::endl;
    ASSERT_TRUE(result == NETMANAGER_SUCCESS || result == NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

/**
 * @tc.name: SetNicTrafficAllowed001
 * @tc.desc: Test NetPolicyClient SetNicTrafficAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNicTrafficAllowed001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<std::string> ifaceName = {};
    int32_t result = g_netPolicyClient->SetNicTrafficAllowed(ifaceName, false);
    std::cout << "NetPolicyClient028 NotifyNetAccessPolicyDiag ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetNicTrafficAllowed002
 * @tc.desc: Test NetPolicyClient SetNicTrafficAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNicTrafficAllowed002, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<std::string> ifaceName = {};
    int32_t result = g_netPolicyClient->SetNicTrafficAllowed(ifaceName, true);
    std::cout << "NetPolicyClient029 NotifyNetAccessPolicyDiag ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetNicTrafficAllowed003
 * @tc.desc: Test NetPolicyClient SetNicTrafficAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNicTrafficAllowed003, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<std::string> ifaceName = {"wlan0"};
    int32_t result = g_netPolicyClient->SetNicTrafficAllowed(ifaceName, false);
    std::cout << "NetPolicyClient030 NotifyNetAccessPolicyDiag ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetNicTrafficAllowed004
 * @tc.desc: Test NetPolicyClient SetNicTrafficAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNicTrafficAllowed004, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<std::string> ifaceName = {"wlan0"};
    int32_t result = g_netPolicyClient->SetNicTrafficAllowed(ifaceName, true);
    std::cout << "NetPolicyClient031 NotifyNetAccessPolicyDiag ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetNicTrafficAllowed005
 * @tc.desc: Test NetPolicyClient SetNicTrafficAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNicTrafficAllowed005, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<std::string> ifaceName = {"wlan0", "aaa"};
    int32_t result = g_netPolicyClient->SetNicTrafficAllowed(ifaceName, false);
    std::cout << "NetPolicyClient032 NotifyNetAccessPolicyDiag ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetNicTrafficAllowed006
 * @tc.desc: Test NetPolicyClient SetNicTrafficAllowed.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetNicTrafficAllowed006, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::vector<std::string> ifaceName = {"wlan0", "aaa"};
    int32_t result = g_netPolicyClient->SetNicTrafficAllowed(ifaceName, true);
    std::cout << "NetPolicyClient027 NotifyNetAccessPolicyDiag ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetInternetAccessByIpForWifiShare001
 * @tc.desc: Test NetPolicyClient SetInternetAccessByIpForWifiShare.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetInternetAccessByIpForWifiShare001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    std::string ip = "1.1.1.1";
    uint8_t family = 2;
    int32_t result = g_netPolicyClient->SetInternetAccessByIpForWifiShare(ip, family, false, "");
    std::cout << "NetPolicyClient034 SetInternetAccessByIpForWifiShare ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetIdleDenyPolicy001
 * @tc.desc: Test NetPolicyClient SetIdleDenyPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetIdleDenyPolicy001, TestSize.Level1)
{
    int32_t result = g_netPolicyClient->SetIdleDenyPolicy(true);
    std::cout << "NetPolicyClient035 SetIdleDenyPolicy ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetIdleDenyPolicy002
 * @tc.desc: Test NetPolicyClient SetIdleDenyPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetIdleDenyPolicy002, TestSize.Level1)
{
    int32_t result = g_netPolicyClient->SetIdleDenyPolicy(false);
    std::cout << "NetPolicyClient036 SetIdleDenyPolicy ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetUidsDeniedListChain001
 * @tc.desc: Test NetPolicyClient SetUidsDeniedListChain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetUidsDeniedListChain001, TestSize.Level1)
{
    std::vector<uint32_t> uids {1000};
    int32_t result = g_netPolicyClient->SetUidsDeniedListChain(uids, true);
    std::cout << "NetPolicyClient037 SetIdleDenyPolicy ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: SetUidsDeniedListChain002
 * @tc.desc: Test NetPolicyClient SetUidsDeniedListChain.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, SetUidsDeniedListChain002, TestSize.Level1)
{
    std::vector<uint32_t> uids {1000};
    int32_t result = g_netPolicyClient->SetUidsDeniedListChain(uids, false);
    std::cout << "NetPolicyClient037 SetIdleDenyPolicy ret:" << result << std::endl;
    ASSERT_GE(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: OnRemoteDied001
 * @tc.desc: Test NetPolicyClient OnRemoteDied.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, OnRemoteDied001, TestSize.Level1)
{
    const wptr<IRemoteObject> remote = nullptr;
    g_netPolicyClient->OnRemoteDied(remote);
    EXPECT_EQ(remote, nullptr);
}

/**
 * @tc.name: GetSelfNetworkAccessPolicy001
 * @tc.desc: Test NetPolicyClient GetSelfNetworkAccessPolicy.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetSelfNetworkAccessPolicy001, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    NetAccessPolicy policy;
    policy.allowWiFi = false;
    policy.allowCellular = false;
    int32_t result = g_netPolicyClient->GetSelfNetworkAccessPolicy(policy);
    std::cout << "NetPolicyClient GetSelfNetworkAccessPolicy ret:" << result << std::endl;
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}

/**
 * @tc.name: GetSelfNetworkAccessPolicy002
 * @tc.desc: Test NetPolicyClient GetSelfNetworkAccessPolicy with policy check.
 * @tc.type: FUNC
 */
HWTEST_F(UtNetPolicyClient, GetSelfNetworkAccessPolicy002, TestSize.Level1)
{
    NetManagerBaseAccessToken token;
    NetAccessPolicy policy;
    policy.allowWiFi = false;
    policy.allowCellular = false;
    int32_t result = g_netPolicyClient->GetSelfNetworkAccessPolicy(policy);
    std::cout << "NetPolicyClient GetSelfNetworkAccessPolicy allowWiFi:" << policy.allowWiFi
              << " allowCellular:" << policy.allowCellular << std::endl;
    ASSERT_EQ(result, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

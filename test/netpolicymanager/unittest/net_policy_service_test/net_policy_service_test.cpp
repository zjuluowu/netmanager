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

#include <thread>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "net_policy_firewall.h"
#include "net_policy_rule.h"
#include "net_policy_service.h"
#include "net_policy_traffic.h"
#include "system_ability_definition.h"
#include "netmanager_base_test_security.h"
#include "net_policy_callback_proxy.h"
#include "net_policy_listener.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "want.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
namespace {
constexpr uint32_t TEST_UID = 1;
}

class UtNetPolicyService : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    static inline std::shared_ptr<NetPolicyService> instance_ = nullptr;
};

void UtNetPolicyService::SetUpTestCase()
{
    instance_ = DelayedSingleton<NetPolicyService>::GetInstance();
    instance_->netPolicyRule_ = std::make_shared<NetPolicyRule>();
    instance_->netPolicyFirewall_ = std::make_shared<NetPolicyFirewall>();
    instance_->netPolicyTraffic_ = std::make_shared<NetPolicyTraffic>();
}

void UtNetPolicyService::TearDownTestCase() {}

void UtNetPolicyService::SetUp() {}

void UtNetPolicyService::TearDown() {}

HWTEST_F(UtNetPolicyService, OnStart001, TestSize.Level1)
{
    instance_->OnStart();
    EXPECT_EQ(instance_->state_, instance_->ServiceRunningState::STATE_STOPPED);
}

HWTEST_F(UtNetPolicyService, FactoryResetPolicies001, TestSize.Level1)
{
    auto ret = instance_->FactoryResetPolicies();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyService, RegisterFactoryResetCallback001, TestSize.Level1)
{
    instance_->RegisterFactoryResetCallback();
    instance_->UpdateNetAccessPolicyToMapFromDB();
    EXPECT_NE(instance_->netFactoryResetCallback_, nullptr);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag001, TestSize.Level1)
{
    uint32_t uid = 10000;
    auto ret = instance_->NotifyNetAccessPolicyDiag(uid);
    EXPECT_TRUE(ret == NETMANAGER_SUCCESS || ret == NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag002, TestSize.Level1)
{
    instance_->netPolicyRule_ = nullptr;
    auto ret = instance_->SetPolicyByUid(0, 0);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    uint32_t policy = 0;
    ret = instance_->GetPolicyByUid(0, policy);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    std::vector<uint32_t> uids;
    ret = instance_->GetUidsByPolicy(0, uids);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag003, TestSize.Level1)
{
    instance_->netPolicyRule_ = std::make_shared<NetPolicyRule>();
    std::string ifaceName = "faces";
    bool isAllowed = true;
    std::vector<std::string> newMeteredIfaces;
    newMeteredIfaces.push_back("faces");
    instance_->netPolicyTraffic_->UpdateMeteredIfaces(newMeteredIfaces);
    auto ret = instance_->IsUidNetAllowed(0, ifaceName, isAllowed);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag004, TestSize.Level1)
{
    auto ret = instance_->RegisterNetPolicyCallback(NULL);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    sptr<IRemoteObject> impl = new (std::nothrow) IPCObjectStub();
    sptr<NetPolicyCallbackProxy> callback = new (std::nothrow) NetPolicyCallbackProxy(impl);
    ret = instance_->RegisterNetPolicyCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    instance_->netPolicyCallback_ = std::make_shared<NetPolicyCallback>();
    ret = instance_->RegisterNetPolicyCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag005, TestSize.Level1)
{
    sptr<IRemoteObject> impl = new (std::nothrow) IPCObjectStub();
    sptr<NetPolicyCallbackProxy> callback = new (std::nothrow) NetPolicyCallbackProxy(impl);
    auto ret = instance_->UnregisterNetPolicyCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    instance_->netPolicyCallback_ = nullptr;
    ret = instance_->UnregisterNetPolicyCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    instance_->netPolicyCallback_ = std::make_shared<NetPolicyCallback>();
    ret = instance_->UnregisterNetPolicyCallback(callback);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag006, TestSize.Level1)
{
    instance_->netPolicyTraffic_ = nullptr;
    std::vector<NetQuotaPolicy> quotaPolicies;
    auto ret = instance_->SetNetQuotaPolicies(quotaPolicies);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    ret = instance_->GetNetQuotaPolicies(quotaPolicies);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    instance_->netPolicyFirewall_ = nullptr;
    std::string simId;
    ret = instance_->ResetPolicies(simId);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag007, TestSize.Level1)
{
    instance_->netPolicyRule_ = nullptr;
    auto ret = instance_->SetBackgroundPolicy(true);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    bool backgroundPolicy = true;
    ret = instance_->GetBackgroundPolicy(backgroundPolicy);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    uint32_t backgroundPolicyOfUid = 0;
    ret = instance_->GetBackgroundPolicyByUid(0, backgroundPolicyOfUid);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    std::vector<uint32_t> uids;
    instance_->netPolicyFirewall_ = nullptr;
    ret = instance_->SetDeviceIdleTrustlist(uids, true);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    instance_->netPolicyFirewall_ = std::make_shared<NetPolicyFirewall>();
    for (int i = 0; i <= 1001; i++) {
        instance_->netPolicyFirewall_->powerSaveAllowedList_.insert(i);
    }
    
    ret = instance_->SetDeviceIdleTrustlist(uids, true);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    instance_->netPolicyTraffic_ = nullptr;
    std::string message;
    ret = instance_->GetDumpMessage(message);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag008, TestSize.Level1)
{
    instance_->netPolicyRule_ = nullptr;
    instance_->netPolicyFirewall_ = nullptr;
    instance_->netPolicyTraffic_ = nullptr;
    auto ret = instance_->FactoryResetPolicies();
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    instance_->netPolicyRule_ = nullptr;
    std::vector<std::string> ifaceNames;
    ret = instance_->SetNicTrafficAllowed(ifaceNames, true);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);

    instance_->netPolicyRule_ = nullptr;
    ret = instance_->DeleteNetworkAccessPolicy(0);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
    instance_->netPolicyRule_ = nullptr;
    NetworkAccessPolicy policy;
    ret = instance_->SetNetworkAccessPolicy(0, policy, true);
    EXPECT_EQ(ret, NETMANAGER_ERR_LOCAL_PTR_NULL);
}

HWTEST_F(UtNetPolicyService, NotifyNetAccessPolicyDiag009, TestSize.Level1)
{
    std::shared_ptr<NetPolicyService> netPolicy = std::make_shared<NetPolicyService>();
    instance_->netFactoryResetCallback_ = new NetPolicyService::FactoryResetCallBack(netPolicy);
    instance_->RegisterFactoryResetCallback();
    EXPECT_EQ(NetManagerCenter::GetInstance().connService_, nullptr);
    
    instance_->netPolicyRule_ = nullptr;
    AccessPolicyParameter parameter = {true, 0, 0};
    AccessPolicySave policys;
    auto ret = instance_->GetNetworkAccessPolicy(parameter, policys);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyService, OnStart002, TestSize.Level1)
{
    instance_->state_ = NetPolicyService::ServiceRunningState::STATE_RUNNING;
    instance_->OnStart();
    EXPECT_NE(instance_->state_, instance_->ServiceRunningState::STATE_STOPPED);
}

HWTEST_F(UtNetPolicyService, OnAddSystemAbility002, TestSize.Level1)
{
    int32_t systemAbilityId = COMM_NET_CONN_MANAGER_SYS_ABILITY_ID;
    std::string deviceId = "";
    instance_->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(instance_->hasSARemoved_, false);
}

HWTEST_F(UtNetPolicyService, OnNetSysRestart001, TestSize.Level1)
{
    instance_->netPolicyRule_ = std::make_shared<NetPolicyRule>();
    instance_->OnNetSysRestart();
    EXPECT_NE(instance_->netPolicyRule_, nullptr);
}

HWTEST_F(UtNetPolicyService, OverwriteNetAccessPolicyToDBFromConfig001, TestSize.Level1)
{
    instance_->OverwriteNetAccessPolicyToDBFromConfig();
    EXPECT_EQ(instance_->hasSARemoved_, false);
}

HWTEST_F(UtNetPolicyService, GetActivatedOsAccountId001, TestSize.Level1)
{
    int32_t userId = 0;
    instance_->GetActivatedOsAccountId(userId);
    EXPECT_NE(userId, 0);
}

HWTEST_F(UtNetPolicyService, UpdateNetAccessPolicyToMapFromDB001, TestSize.Level1)
{
    instance_->netPolicyRule_ = nullptr;
    instance_->UpdateNetAccessPolicyToMapFromDB();
    EXPECT_EQ(instance_->netPolicyRule_, nullptr);
}

HWTEST_F(UtNetPolicyService, ResetNetAccessPolicy001, TestSize.Level1)
{
    instance_->ResetNetAccessPolicy();
    EXPECT_EQ(instance_->netPolicyRule_, nullptr);
}

HWTEST_F(UtNetPolicyService, SetBrokerUidAccessPolicyMap001, TestSize.Level1)
{
    std::optional<uint32_t> uid = 123;
    instance_->SetBrokerUidAccessPolicyMap(uid);
    EXPECT_EQ(instance_->hasSARemoved_, false);
}

HWTEST_F(UtNetPolicyService, DelBrokerUidAccessPolicyMap001, TestSize.Level1)
{
    uint32_t uid = 123;
    instance_->DelBrokerUidAccessPolicyMap(uid);
    EXPECT_EQ(instance_->hasSARemoved_, false);
}

HWTEST_F(UtNetPolicyService, GetSampleBundleInfosForActiveUser001, TestSize.Level1)
{
    instance_->GetSampleBundleInfosForActiveUser();
    EXPECT_EQ(instance_->hasSARemoved_, false);
}

HWTEST_F(UtNetPolicyService, IsUidNetAllowed001, TestSize.Level1)
{
    instance_->netPolicyTraffic_ = nullptr;
    std::string ifaceName = "faces";
    bool isAllowed = true;
    auto ret = instance_->IsUidNetAllowed(0, ifaceName, isAllowed);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(UtNetPolicyService, OnAddSystemAbility003, TestSize.Level1)
{
    int32_t systemAbilityId = BUNDLE_MGR_SERVICE_SYS_ABILITY_ID;
    std::string deviceId = "";
    instance_->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(instance_->hasSARemoved_, false);
}

HWTEST_F(UtNetPolicyService, OnAddSystemAbility004, TestSize.Level1)
{
    int32_t systemAbilityId = COMMON_EVENT_SERVICE_ID;
    std::string deviceId = "";
    instance_->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(instance_->hasSARemoved_, false);
}

HWTEST_F(UtNetPolicyService, OnNetSysRestart002, TestSize.Level1)
{
    instance_->netPolicyRule_ = nullptr;
    instance_->OnNetSysRestart();
    EXPECT_EQ(instance_->netPolicyRule_, nullptr);
}

HWTEST_F(UtNetPolicyService, OnReceiveEvent001, TestSize.Level1)
{
    std::shared_ptr<NetPolicyService> policyPtr = std::make_shared<NetPolicyService>();
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    matchingSkills.AddEvent(COMMON_EVENT_STATUS_CHANGED);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    std::shared_ptr<NetPolicyListener> subscriber = std::make_shared<NetPolicyListener>(
        subscribeInfo, std::static_pointer_cast<NetPolicyService>(policyPtr));
    EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber);
 
    EventFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    subscriber->OnReceiveEvent(data);
 
    EventFwk::Want want2;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    EventFwk::CommonEventData data2;
    data2.SetWant(want2);
    subscriber->OnReceiveEvent(data2);
 
    EventFwk::Want want3;
    want.SetAction(COMMON_EVENT_STATUS_CHANGED);
    EventFwk::CommonEventData data3;
    data3.SetWant(want3);
    subscriber->OnReceiveEvent(data3);
 
    EXPECT_NE(subscriber, nullptr);
}

HWTEST_F(UtNetPolicyService, OnReceiveEvent002, TestSize.Level1)
{
    std::shared_ptr<NetPolicyService> policyPtr = std::make_shared<NetPolicyService>();
    EventFwk::MatchingSkills matchingSkills;
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    std::shared_ptr<NetPolicyListener> subscriber = std::make_shared<NetPolicyListener>(
        subscribeInfo, std::static_pointer_cast<NetPolicyService>(policyPtr));
    EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber);
    EventFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_RESTORE_START);
    std::string bundleName = "test";
    want.SetBundle(bundleName);
    want.SetParam("bundleName", bundleName);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    subscriber->OnReceiveEvent(data);
    EXPECT_NE(subscriber, nullptr);
}

HWTEST_F(UtNetPolicyService, GetSelfNetworkAccessPolicy001, TestSize.Level1)
{
    NetAccessPolicy policy;
    auto ret = instance_->GetSelfNetworkAccessPolicy(policy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_TRUE(policy.allowWiFi);
    EXPECT_TRUE(policy.allowCellular);
}

HWTEST_F(UtNetPolicyService, GetSelfNetworkAccessPolicy002, TestSize.Level1)
{
    instance_->netPolicyRule_ = nullptr;
    NetAccessPolicy policy;
    auto ret = instance_->GetSelfNetworkAccessPolicy(policy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    instance_->netPolicyRule_ = std::make_shared<NetPolicyRule>();
}

HWTEST_F(UtNetPolicyService, GetSelfNetworkAccessPolicy003, TestSize.Level1)
{
    NetAccessPolicy policy;
    policy.allowWiFi = false;
    policy.allowCellular = false;
    auto ret = instance_->GetSelfNetworkAccessPolicy(policy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(UtNetPolicyService, ReportFirewallPolicyChange001, TestSize.Level1)
{
    NetPolicyService policyService;
    policyService.netPolicyRule_ = std::make_shared<NetPolicyRule>();
    policyService.netPolicyFirewall_ = std::make_shared<NetPolicyFirewall>();
    policyService.netPolicyTraffic_ = std::make_shared<NetPolicyTraffic>();
    std::string testFnc = "testFnc";
    std::shared_ptr<FirewallRule> testFireWall;
    policyService.netPolicyFirewall_->ReportFirewallPolicyChange(testFnc, 2, true, testFireWall);
    NetAccessPolicy policy;
    policy.allowWiFi = false;
    policy.allowCellular = false;
    auto ret = policyService.GetSelfNetworkAccessPolicy(policy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "gtest/gtest-message.h"
#include "gtest/gtest-test-part.h"
#include "gtest/hwext/gtest-ext.h"
#include "gtest/hwext/gtest-tag.h"
#include "http_proxy.h"
#include "inet_addr.h"
#include "net_manager_constants.h"
#include "netmanager_ext_test_security.h"
#include "netmgr_ext_log_wrapper.h"
#include "refbase.h"
#include "singleton.h"

#include <string>

#include "i_netfirewall_service.h"
#include "netfirewall_service.h"
#include "netfirewall_client.h"
#include "netfirewall_common.h"
#include "netfirewall_proxy.h"
#include "netsys_controller.h"
#include "system_ability_definition.h"
#include "bundle_constants.h"
#include "netfirewall_database.h"
#include "netfirewall_default_rule_parser.h"
#include "netfirewall_db_helper.h"
#include "netfirewall_hisysevent.h"
#include "netfirewall_intercept_recorder.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
int32_t g_rowId = 0;

constexpr uint32_t APPID_TEST01 = 2034;
constexpr int32_t USER_ID1 = 100;
constexpr int32_t USER_ID2 = 101;
constexpr int32_t OLD_VERSION = 1;
constexpr int32_t NEW_VERSION = 2;
constexpr int32_t MAX_USER_RULE = 1;
constexpr int32_t MAX_IPS = 1;
constexpr int32_t MAX_PORTS = 1;
constexpr int32_t MAX_DOMAINS = 1;
constexpr uint16_t LOCAL_START_PORT = 10020;
constexpr uint16_t LOCAL_END_PORT = 1003;
constexpr uint16_t REMOTE_START_PORT = 1002;
constexpr uint16_t REMOTE_END_PORT = 10030;

std::vector<NetFirewallIpParam> GetIpList(const std::string &addressStart)
{
    const uint8_t mask = 24;
    std::vector<NetFirewallIpParam> localParamList;
    NetFirewallIpParam localParam;
    localParam.family = 1;
    localParam.type = 1;
    localParam.mask = mask;
    for (int i = 0; i < MAX_IPS; i++) {
        inet_pton(AF_INET, (addressStart + std::to_string(i)).c_str(), &localParam.ipv4.startIp);
        localParamList.push_back(localParam);
    }
    return localParamList;
}

sptr<NetFirewallRule> GetNetFirewallRuleSptr()
{
    sptr<NetFirewallRule> rule = (std::make_unique<NetFirewallRule>()).release();
    if (!rule) {
        return rule;
    }
    rule->ruleId = 1;
    rule->userId = USER_ID1;
    rule->ruleName = "rule test";
    rule->ruleDescription = "AddNetFirewallRule 001";
    rule->ruleDirection = NetFirewallRuleDirection::RULE_OUT;
    rule->ruleAction = FirewallRuleAction::RULE_ALLOW;
    rule->isEnabled = true;
    rule->appUid = APPID_TEST01;

    rule->localIps = GetIpList("192.168.10.");
    rule->remoteIps = GetIpList("192.168.2.");
    std::vector<NetFirewallPortParam> localPortParamList;
    NetFirewallPortParam localPortParam;
    localPortParam.startPort = LOCAL_START_PORT;
    localPortParam.endPort = LOCAL_END_PORT;
    for (int i = 0; i < MAX_PORTS; i++) {
        localPortParamList.push_back(localPortParam);
    }
    rule->localPorts = localPortParamList;
    std::vector<NetFirewallPortParam> remotePortParamList;
    NetFirewallPortParam remotePortParam;
    remotePortParam.startPort = REMOTE_START_PORT;
    remotePortParam.endPort = REMOTE_END_PORT;
    for (int i = 0; i < MAX_PORTS; i++) {
        remotePortParamList.push_back(remotePortParam);
    }
    rule->remotePorts = remotePortParamList;
    std::vector<NetFirewallDomainParam> domainList;
    NetFirewallDomainParam domain;
    domain.isWildcard = 1;
    domain.domain = "www.openharmony.cn";
    for (int i = 0; i < MAX_DOMAINS; i++) {
        domainList.push_back(domain);
    }
    rule->domains = domainList;
    rule->dns.primaryDns = "192.168.1.245";
    rule->dns.standbyDns = "192.168.1.1";

    return rule;
}

sptr<NetFirewallRule> GetNetFirewallRuleSptrTypeDns()
{
    sptr<NetFirewallRule> rule = (std::make_unique<NetFirewallRule>()).release();
    if (!rule) {
        return rule;
    }
    int ruleId = 2;
    rule->ruleId = ruleId;
    rule->userId = USER_ID1;
    rule->ruleName = "rule test";
    rule->ruleDescription = "AddNetFirewallRule 001";
    rule->ruleDirection = NetFirewallRuleDirection::RULE_OUT;
    rule->ruleAction = FirewallRuleAction::RULE_ALLOW;
    rule->isEnabled = true;
    rule->appUid = APPID_TEST01;

    rule->localIps = GetIpList("192.168.10.");
    rule->remoteIps = GetIpList("192.168.2.");
    rule->ruleType = NetFirewallRuleType::RULE_IP;
    std::vector<NetFirewallPortParam> localPortParamList;
    NetFirewallPortParam localPortParam;
    localPortParam.startPort = LOCAL_START_PORT;
    localPortParam.endPort = LOCAL_END_PORT;
    for (int i = 0; i < MAX_PORTS; i++) {
        localPortParamList.push_back(localPortParam);
    }
    rule->localPorts = localPortParamList;
    std::vector<NetFirewallPortParam> remotePortParamList;
    NetFirewallPortParam remotePortParam;
    remotePortParam.startPort = REMOTE_START_PORT;
    remotePortParam.endPort = REMOTE_END_PORT;
    for (int i = 0; i < MAX_PORTS; i++) {
        remotePortParamList.push_back(remotePortParam);
    }
    rule->remotePorts = remotePortParamList;
    std::vector<NetFirewallDomainParam> domainList;
    NetFirewallDomainParam domain;
    domain.isWildcard = 1;
    domain.domain = "www.openharmony.cn";
    for (int i = 0; i < MAX_DOMAINS; i++) {
        domainList.push_back(domain);
    }
    rule->domains = domainList;
    rule->dns.primaryDns = "192.168.1.245";
    rule->dns.standbyDns = "192.168.1.1";

    return rule;
}
}

class NetFirewallServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp();
    void TearDown();
    bool PublishChangedEvent(const std::string &action, int32_t code) const;
    static inline auto instance_ = DelayedSingleton<NetFirewallService>::GetInstance();
};

void NetFirewallServiceTest::SetUpTestCase() {}

void NetFirewallServiceTest::TearDownTestCase() {}

void NetFirewallServiceTest::SetUp() {}

void NetFirewallServiceTest::TearDown() {}

/**
 * @tc.name: OnStart
 * @tc.desc: Test NetFirewallServiceTest OnStart.
 * @tc.type: FUNC
 */
HWTEST_F(NetFirewallServiceTest, OnStart, TestSize.Level1)
{
    instance_->state_ = NetFirewallService::ServiceRunningState::STATE_RUNNING;
    instance_->OnStart();
    instance_->state_ = NetFirewallService::ServiceRunningState::STATE_NOT_START;
    EXPECT_EQ(instance_->state_, NetFirewallService::ServiceRunningState::STATE_NOT_START);
}

/**
 * @tc.name: OnInit001
 * @tc.desc: Test NetFirewallServiceTest OnInit.
 * @tc.type: FUNC
 */
HWTEST_F(NetFirewallServiceTest, OnInit001, TestSize.Level1)
{
    int32_t ret = instance_->OnInit();
    EXPECT_EQ(ret, FIREWALL_SUCCESS);
}

/**
 * @tc.name: OnStop
 * @tc.desc: Test NetFirewallServiceTest OnStop.
 * @tc.type: FUNC
 */
HWTEST_F(NetFirewallServiceTest, OnStop, TestSize.Level1)
{
    instance_->OnStop();
    EXPECT_EQ(instance_->state_, NetFirewallService::ServiceRunningState::STATE_NOT_START);
}

HWTEST_F(NetFirewallServiceTest, Dump, TestSize.Level1)
{
    int32_t fd = 1;
    std::vector<std::u16string> args = {};
    EXPECT_EQ(instance_->Dump(fd, args), FIREWALL_SUCCESS);
}

HWTEST_F(NetFirewallServiceTest, GetDumpMessage, TestSize.Level1)
{
    std::string message;
    instance_->GetDumpMessage(message);
    EXPECT_EQ(message.empty(), false);
}
} // namespace NetManagerStandard
} // namespace OHOS

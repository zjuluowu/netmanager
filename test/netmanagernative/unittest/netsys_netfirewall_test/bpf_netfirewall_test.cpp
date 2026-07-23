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

#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "bpf_netfirewall.h"

namespace {
using namespace testing::ext;
using namespace OHOS::NetManagerStandard;
constexpr int32_t USER_ID1 = 100;
constexpr uint8_t LABEL_LEN = 3;
constexpr uint8_t OVERFLOW_TOTAL_LEN = 4;
constexpr uint8_t OVERFLOW_LABEL_LEN = static_cast<uint8_t>(OVERFLOW_TOTAL_LEN + 1);
constexpr uint8_t TWO_LABEL_TOTAL = static_cast<uint8_t>(LABEL_LEN + 1 + LABEL_LEN + 1);
}

class NetsysBpfNetFirewallTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetsysBpfNetFirewallTest::SetUpTestCase() {}

void NetsysBpfNetFirewallTest::TearDownTestCase() {}

void NetsysBpfNetFirewallTest::SetUp() {}

void NetsysBpfNetFirewallTest::TearDown() {}

HWTEST_F(NetsysBpfNetFirewallTest, AddDomainCache001, TestSize.Level0)
{
    std::shared_ptr<NetsysBpfNetFirewall> bpfNet = NetsysBpfNetFirewall::GetInstance();
    NetAddrInfo netInfo;
    netInfo.aiFamily = AF_INET;
    inet_pton(AF_INET, "192.168.8.116", &netInfo.aiAddr.sin);
    bpfNet->AddDomainCache(netInfo);
    EXPECT_EQ(netInfo.aiFamily, AF_INET);
    netInfo.aiFamily = AF_INET6;
    inet_pton(AF_INET6, "fe80::6bec:e9b9:a1df:f69d", &netInfo.aiAddr.sin6);
    bpfNet->AddDomainCache(netInfo);
    bpfNet->ClearDomainCache();
    EXPECT_EQ(netInfo.aiFamily, AF_INET6);
}

HWTEST_F(NetsysBpfNetFirewallTest, ClearFirewallDefaultAction001, TestSize.Level0)
{
    std::shared_ptr<NetsysBpfNetFirewall> bpfNet = NetsysBpfNetFirewall::GetInstance();
    bpfNet->SetBpfLoaded(true);
    int ret = bpfNet->SetFirewallDefaultAction(USER_ID1, FirewallRuleAction::RULE_ALLOW,
        FirewallRuleAction::RULE_ALLOW);
    bpfNet->ClearFirewallDefaultAction();
    EXPECT_EQ(ret, FIREWALL_SUCCESS);
    bpfNet->SetBpfLoaded(false);
    ret = bpfNet->SetFirewallDefaultAction(USER_ID1, FirewallRuleAction::RULE_ALLOW,
        FirewallRuleAction::RULE_ALLOW);
    EXPECT_EQ(ret, NETFIREWALL_ERR);
}

HWTEST_F(NetsysBpfNetFirewallTest, ClearFirewallRules001, TestSize.Level0)
{
    std::shared_ptr<NetsysBpfNetFirewall> bpfNet = NetsysBpfNetFirewall::GetInstance();
    int ret = bpfNet->ClearFirewallRules(NetFirewallRuleType::RULE_ALL);
    EXPECT_EQ(ret, FIREWALL_SUCCESS);
    ret = FIREWALL_ERR_INTERNAL;
    ret = bpfNet->ClearFirewallRules(NetFirewallRuleType::RULE_IP);
    EXPECT_EQ(ret, FIREWALL_SUCCESS);
    ret = FIREWALL_ERR_INTERNAL;
    ret = bpfNet->ClearFirewallRules(NetFirewallRuleType::RULE_DOMAIN);
    EXPECT_EQ(ret, FIREWALL_SUCCESS);
    ret = FIREWALL_ERR_INTERNAL;
    ret = bpfNet->ClearFirewallRules(NetFirewallRuleType::RULE_DEFAULT_ACTION);
    EXPECT_EQ(ret, FIREWALL_SUCCESS);
    ret = bpfNet->ClearFirewallRules(NetFirewallRuleType::RULE_DOMAIN);
    EXPECT_EQ(ret, FIREWALL_SUCCESS);
}

HWTEST_F(NetsysBpfNetFirewallTest, WriteSrcPortBpfMap001, TestSize.Level0)
{
    std::shared_ptr<NetsysBpfNetFirewall> bpfNet = NetsysBpfNetFirewall::GetInstance();
    
    BitmapManager manager;

    int ret = bpfNet->WriteSrcPortBpfMap(manager, NetFirewallRuleDirection::RULE_IN);
    EXPECT_EQ(ret, -1);
    ret = bpfNet->WriteSrcPortBpfMap(manager, NetFirewallRuleDirection::RULE_OUT);
    EXPECT_EQ(ret, -1);

    Bitmap bitmap(1);
    uint32_t mask = 16;
    uint16_t port = 6000;
    portRuleBitmap tmp;
    tmp.prefixlen = mask;
    tmp.data = port;
    tmp.bitmap = bitmap;
    manager.srcPortMap_.ruleBitmapVec_.emplace_back(tmp);
    manager.dstPortMap_.ruleBitmapVec_.emplace_back(tmp);
    ret = bpfNet->WriteSrcPortBpfMap(manager, NetFirewallRuleDirection::RULE_IN);
    EXPECT_EQ(ret, 0);
    ret = bpfNet->WriteSrcPortBpfMap(manager, NetFirewallRuleDirection::RULE_OUT);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysBpfNetFirewallTest, WriteDstPortBpfMap001, TestSize.Level0)
{
    std::shared_ptr<NetsysBpfNetFirewall> bpfNet = NetsysBpfNetFirewall::GetInstance();
    
    BitmapManager manager;

    int ret = bpfNet->WriteDstPortBpfMap(manager, NetFirewallRuleDirection::RULE_IN);
    EXPECT_EQ(ret, -1);
    ret = bpfNet->WriteDstPortBpfMap(manager, NetFirewallRuleDirection::RULE_OUT);
    EXPECT_EQ(ret, -1);

    Bitmap bitmap(1);
    uint32_t mask = 16;
    uint16_t port = 6000;
    portRuleBitmap tmp;
    tmp.prefixlen = mask;
    tmp.data = port;
    tmp.bitmap = bitmap;
    manager.srcPortMap_.ruleBitmapVec_.emplace_back(tmp);
    manager.dstPortMap_.ruleBitmapVec_.emplace_back(tmp);
    ret = bpfNet->WriteDstPortBpfMap(manager, NetFirewallRuleDirection::RULE_IN);
    EXPECT_EQ(ret, 0);
    ret = bpfNet->WriteDstPortBpfMap(manager, NetFirewallRuleDirection::RULE_OUT);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysBpfNetFirewallTest, WritePortBpfMap001, TestSize.Level0)
{
    std::shared_ptr<NetsysBpfNetFirewall> bpfNet = NetsysBpfNetFirewall::GetInstance();
    
    BpfPortMap portMap;
    const char *path = "test";
    int ret = bpfNet->WritePortBpfMap(portMap, path);
    EXPECT_EQ(ret, -1);

    uint16_t start = 1;
    uint32_t mask = 16;
    Bitmap bitmap(1);
    portMap.OrInsert(start, mask, bitmap);
    ret = bpfNet->WritePortBpfMap(portMap, path);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetsysBpfNetFirewallTest, DecodeDomainFromKey001, TestSize.Level0)
{
    std::shared_ptr<NetsysBpfNetFirewall> bpfNet = std::make_shared<NetsysBpfNetFirewall>();
    DomainHashKey nullKey = {};
    nullKey.prefixlen = 0;
    auto result = bpfNet->DecodeDomainFromKey(nullKey);
    EXPECT_TRUE(result.empty());

    DomainHashKey maxKey = {};
    maxKey.prefixlen = static_cast<uint32_t>((DNS_DOMAIN_LEN + LABEL_LEN) * BIT_PER_BYTE);
    EXPECT_EQ(memset_s(maxKey.data, sizeof(maxKey.data), 0, sizeof(maxKey.data)), EOK);
    result = bpfNet->DecodeDomainFromKey(maxKey);
    EXPECT_TRUE(result.empty());

    DomainHashKey overflowKey = {};
    overflowKey.prefixlen = static_cast<uint32_t>(OVERFLOW_TOTAL_LEN * BIT_PER_BYTE);
    EXPECT_EQ(memset_s(overflowKey.data, sizeof(overflowKey.data), 0, sizeof(overflowKey.data)), EOK);
    overflowKey.data[OVERFLOW_TOTAL_LEN - 1] = OVERFLOW_LABEL_LEN;
    result = bpfNet->DecodeDomainFromKey(overflowKey);
    EXPECT_TRUE(result.empty());

    DomainHashKey normalKey = {};
    normalKey.prefixlen = static_cast<uint32_t>((TWO_LABEL_TOTAL + 8) * BIT_PER_BYTE);
    normalKey.uid = 100;
    normalKey.appuid = 0;
    EXPECT_EQ(memset_s(normalKey.data, sizeof(normalKey.data), 0, sizeof(normalKey.data)), EOK);
    {
        const char label1[] = "moc";
        EXPECT_EQ(memcpy_s(normalKey.data, sizeof(label1) - 1, label1, sizeof(label1) - 1), EOK);
        normalKey.data[LABEL_LEN] = LABEL_LEN;
    }
    {
        const char label2[] = "www";
        const size_t off = static_cast<size_t>(LABEL_LEN + 1);
        EXPECT_EQ(memcpy_s(normalKey.data + off, sizeof(label2) - 1, label2, sizeof(label2) - 1), EOK);
        normalKey.data[off + LABEL_LEN] = LABEL_LEN;
    }
    result = bpfNet->DecodeDomainFromKey(normalKey);
    EXPECT_FALSE(result.empty());
}
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

#include "bpf_loader.h"
#include "i_netfirewall_callback.h"
#include "netfirewall_callback_stub.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/if.h>

#define private public
#define protected public

#include "bitmap_manager.h"
#include "bpf_netfirewall.h"

#define FILE_NAME (strrchr(__FILE__, '/') + 1)
#define MANUAL_TEST 0

using namespace std;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NetManagerStandard;

static sptr<NetFirewallIpRule> GeIpFirewallRule(NetFirewallRuleDirection dir, string addr,
    int32_t start = 80, int32_t end = 80)
{
    sptr<NetFirewallIpRule> rule = (std::make_unique<NetFirewallIpRule>()).release();
    if (!rule) {
        return rule;
    }
    rule->ruleAction = FirewallRuleAction::RULE_DENY;
    rule->appUid = 0;

    std::vector<NetFirewallIpParam> remoteIp;
    NetFirewallIpParam remoteIpParam;
    remoteIpParam.family = 1;
    remoteIpParam.type = 1;
    inet_pton(AF_INET, addr.c_str(), &remoteIpParam.ipv4.startIp);
    remoteIpParam.mask = IPV4_MAX_PREFIXLEN;
    remoteIp.push_back(remoteIpParam);
    rule->remoteIps = remoteIp;

    rule->protocol = NetworkProtocol::TCP;

    std::vector<NetFirewallPortParam> ports;
    NetFirewallPortParam remotePort;
    remotePort.startPort = start;
    remotePort.endPort = end;
    ports.emplace_back(remotePort);
    rule->localPorts = ports;
    rule->remotePorts = ports;

    return rule;
}

class NetsysNetFirewallTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetsysNetFirewallTest::SetUpTestCase()
{
    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();
    if (!bpfNetFirewall->IsBpfLoaded()) {
        auto ret = OHOS::NetManagerStandard::LoadElf(FIREWALL_BPF_PATH);
        printf("LoadElf is %d\n", ret);

        if (ret == ElfLoadError::ELF_LOAD_ERR_NONE) {
            bpfNetFirewall->SetBpfLoaded(true);
        }
        bpfNetFirewall->StartListener();
    }
}

void NetsysNetFirewallTest::TearDownTestCase() {}

void NetsysNetFirewallTest::SetUp() {}

void NetsysNetFirewallTest::TearDown() {}

class TestNetFirewallCallbackStub : public OHOS::NetsysNative::NetFirewallCallbackStub {
public:
    int32_t OnIntercept(OHOS::sptr<InterceptRecord> &info)
    {
        if (!info) {
            return -1;
        }

        std::thread t = std::thread([&]() {
            printf("\ttransProtocol=%u\n", info->protocol);
            printf("\tsourcePort=%u\n", info->localPort);
            printf("\tdestPort=%u\n", info->remotePort);
            printf("\tsourceIp=%s\n", (info->localIp).c_str());
            printf("\tdestIp=%s\n", (info->remoteIp).c_str());
            printf("\tappUid=%d\n", info->appUid);
        });
        t.join();

        return 0;
    }
};

HWTEST_F(NetsysNetFirewallTest, NetsysNetFirewallTest001, TestSize.Level0)
{
    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();

    OHOS::sptr<OHOS::NetsysNative::INetFirewallCallback> callback = new (nothrow)TestNetFirewallCallbackStub;
    int32_t ret = bpfNetFirewall->RegisterCallback(callback);
    EXPECT_EQ(ret, 0);

    ret = bpfNetFirewall->UnregisterCallback(callback);
    EXPECT_EQ(ret, 0);

    Ip4Key srcIp = 0;
    string src = "192.168.8.116";
    inet_pton(AF_INET, src.c_str(), &srcIp);

    Ip4Key dstIp = 0;
    string dst = "192.168.1.5";
    inet_pton(AF_INET, dst.c_str(), &dstIp);
    InterceptEvent interceptEv = {
        .dir = INGRESS,
        .family = AF_INET,
        .protocol = IPPROTO_TCP,
        .sport = 80,
        .dport = 5684,
        .appuid = 0,
    };
    interceptEv.ipv4.saddr = srcIp;
    interceptEv.ipv4.daddr = dstIp;

    bpfNetFirewall->NotifyInterceptEvent(&interceptEv);

    Event ev = {
        .intercept = interceptEv,
        .type = EVENT_INTERCEPT,
        .len = sizeof(InterceptEvent),
    };
    ret = NetsysBpfNetFirewall::HandleEvent(NULL, &ev, sizeof(ev));
    EXPECT_EQ(ret, 0);

    DebugEvent debugEv = {
        .type = DBG_GENERIC,
        .dir = INGRESS,
        .arg1 = 0,
    };
    NetsysBpfNetFirewall::HandleDebugEvent(&debugEv);
}

HWTEST_F(NetsysNetFirewallTest, NetsysNetFirewallTest002, TestSize.Level0)
{
    Bitmap a(1);
    Bitmap b(a);

    b.Clear();
    b.Set(3);
    EXPECT_TRUE(a.SpecialHash() != 0);
    a.Or(b);
    a.And(b);

    EXPECT_TRUE(a == b);
}

HWTEST_F(NetsysNetFirewallTest, NetsysNetFirewallTest003, TestSize.Level0)
{
    BpfUnorderedMap<int> map;
    int key = 1;
    Bitmap val(10);
    Bitmap other(20);
    map.OrInsert(key, val);
    map.OrInsert(key + 1, val);
    map.OrForEach(other);

    map.Delete(key);
    EXPECT_FALSE(map.Empty());
    map.Clear();
    EXPECT_TRUE(map.Empty());

    EXPECT_TRUE(map.Get().empty());
}

HWTEST_F(NetsysNetFirewallTest, NetsysNetFirewallTest004, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    ruleList.push_back(rule);
    sptr<NetFirewallIpRule> rule2 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.102");
    ruleList.push_back(rule2);
    sptr<NetFirewallIpRule> rule3 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.102", 80, 443);
    ruleList.push_back(rule3);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    EXPECT_FALSE(manager.GetSrcIp4Map().empty());
    EXPECT_FALSE(manager.GetSrcIp6Map().empty());
    EXPECT_FALSE(manager.GetDstIp4Map().empty());
    EXPECT_FALSE(manager.GetDstIp6Map().empty());
    EXPECT_FALSE(manager.GetSrcPortMap().Get().empty());
    EXPECT_FALSE(manager.GetDstPortMap().Get().empty());
    EXPECT_FALSE(manager.GetProtoMap().Empty());
    EXPECT_FALSE(manager.GetActionMap().Empty());
    EXPECT_FALSE(manager.GetAppIdMap().Empty());
}

HWTEST_F(NetsysNetFirewallTest, NetsysNetFirewallTest005, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    ruleList.push_back(rule);
    sptr<NetFirewallIpRule> rule2 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.102");
    ruleList.push_back(rule2);

    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();
    int32_t ret = bpfNetFirewall->SetFirewallIpRules(ruleList);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNetFirewallTest, NetsysNetFirewallTest006, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.102", 1, 65535);
    ruleList.push_back(rule);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    EXPECT_FALSE(manager.GetSrcIp4Map().empty());
    EXPECT_FALSE(manager.GetSrcIp6Map().empty());
    EXPECT_FALSE(manager.GetDstIp4Map().empty());
    EXPECT_FALSE(manager.GetDstIp6Map().empty());
    EXPECT_FALSE(manager.GetSrcPortMap().Get().empty());
    EXPECT_FALSE(manager.GetDstPortMap().Get().empty());
    EXPECT_FALSE(manager.GetProtoMap().Empty());
    EXPECT_FALSE(manager.GetActionMap().Empty());
    EXPECT_FALSE(manager.GetAppIdMap().Empty());
}

HWTEST_F(NetsysNetFirewallTest, IpParamParserTest001, TestSize.Level0)
{
    std::vector<Ip4Data> list;
    in_addr startAddr;
    startAddr.s_addr = 2887802737;
    in_addr endAddr;
    endAddr.s_addr = 2887802770;
    EXPECT_NE(IpParamParser::GetIp4AndMask(endAddr, startAddr, list), 0);
    EXPECT_EQ(IpParamParser::GetIp4AndMask(startAddr, endAddr, list), 0);
    EXPECT_EQ(IpParamParser::GetSuffixZeroLength(0), 32);
    EXPECT_GE(IpParamParser::GetSuffixZeroLength(1000), 0);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap001, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    segBitMap.AddMap(1, 65535, bitmap0);
    ASSERT_EQ(segBitMap.segments_.size(), 1);
    EXPECT_EQ(segBitMap.segments_[0].start, 1);
    EXPECT_EQ(segBitMap.segments_[0].end, 65535);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap0);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap002, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(500, 1000, bitmap0);
    segBitMap.AddMap(65535, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    EXPECT_EQ(segBitMap.segments_[0].start, 500);
    EXPECT_EQ(segBitMap.segments_[0].end, 1000);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap0);
    EXPECT_EQ(segBitMap.segments_[1].start, 65535);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, bitmap1);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap003, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(65535, 65535, bitmap0);
    segBitMap.AddMap(500, 1000, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    EXPECT_EQ(segBitMap.segments_[0].start, 500);
    EXPECT_EQ(segBitMap.segments_[0].end, 1000);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 65535);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, bitmap0);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap004, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(500, 65535, bitmap0);
    segBitMap.AddMap(65535, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    EXPECT_EQ(segBitMap.segments_[0].start, 500);
    EXPECT_EQ(segBitMap.segments_[0].end, 65534);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap0);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 65535);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap005, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(65535, 65535, bitmap0);
    segBitMap.AddMap(500, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    EXPECT_EQ(segBitMap.segments_[0].start, 500);
    EXPECT_EQ(segBitMap.segments_[0].end, 65534);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap1);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 65535);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap006, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(500, 65535, bitmap0);
    segBitMap.AddMap(500, 500, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[0].start, 500);
    EXPECT_EQ(segBitMap.segments_[0].end, 500);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, result);
    EXPECT_EQ(segBitMap.segments_[1].start, 501);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, bitmap0);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap007, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(500, 500, bitmap0);
    segBitMap.AddMap(500, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[0].start, 500);
    EXPECT_EQ(segBitMap.segments_[0].end, 500);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, result);
    EXPECT_EQ(segBitMap.segments_[1].start, 501);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, bitmap1);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap008, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(500, 65535, bitmap0);
    segBitMap.AddMap(400, 400, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    EXPECT_EQ(segBitMap.segments_[0].start, 400);
    EXPECT_EQ(segBitMap.segments_[0].end, 400);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, bitmap0);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap009, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(1, 65535, bitmap0);
    segBitMap.AddMap(1, 65535, bitmap1);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 1);
    EXPECT_EQ(segBitMap.segments_[0].start, 1);
    EXPECT_EQ(segBitMap.segments_[0].end, 65535);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, result);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap010, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(100, 65535, bitmap0);
    segBitMap.AddMap(500, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    EXPECT_EQ(segBitMap.segments_[0].start, 100);
    EXPECT_EQ(segBitMap.segments_[0].end, 499);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap0);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap011, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(500, 65535, bitmap0);
    segBitMap.AddMap(100, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    EXPECT_EQ(segBitMap.segments_[0].start, 100);
    EXPECT_EQ(segBitMap.segments_[0].end, 499);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap1);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap012, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(100, 65535, bitmap0);
    segBitMap.AddMap(100, 1000, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[0].start, 100);
    EXPECT_EQ(segBitMap.segments_[0].end, 1000);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, result);
    EXPECT_EQ(segBitMap.segments_[1].start, 1001);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, bitmap0);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap013, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(100, 1000, bitmap0);
    segBitMap.AddMap(100, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 2);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[0].start, 100);
    EXPECT_EQ(segBitMap.segments_[0].end, 1000);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, result);
    EXPECT_EQ(segBitMap.segments_[1].start, 1001);
    EXPECT_EQ(segBitMap.segments_[1].end, 65535);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, bitmap1);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap014, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(100, 65535, bitmap0);
    segBitMap.AddMap(500, 1000, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 3);
    EXPECT_EQ(segBitMap.segments_[0].start, 100);
    EXPECT_EQ(segBitMap.segments_[0].end, 499);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap0);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 1000);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result);
    EXPECT_EQ(segBitMap.segments_[2].start, 1001);
    EXPECT_EQ(segBitMap.segments_[2].end, 65535);
    EXPECT_EQ(segBitMap.segments_[2].bitmap, bitmap0);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap015, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(500, 65535, bitmap0);
    segBitMap.AddMap(100, 1000, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 3);
    EXPECT_EQ(segBitMap.segments_[0].start, 100);
    EXPECT_EQ(segBitMap.segments_[0].end, 499);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap1);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 1000);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result);
    EXPECT_EQ(segBitMap.segments_[2].start, 1001);
    EXPECT_EQ(segBitMap.segments_[2].end, 65535);
    EXPECT_EQ(segBitMap.segments_[2].bitmap, bitmap0);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap016, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(100, 1000, bitmap0);
    segBitMap.AddMap(500, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 3);
    EXPECT_EQ(segBitMap.segments_[0].start, 100);
    EXPECT_EQ(segBitMap.segments_[0].end, 499);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap0);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 1000);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result);
    EXPECT_EQ(segBitMap.segments_[2].start, 1001);
    EXPECT_EQ(segBitMap.segments_[2].end, 65535);
    EXPECT_EQ(segBitMap.segments_[2].bitmap, bitmap1);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap017, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    segBitMap.AddMap(500, 1000, bitmap0);
    segBitMap.AddMap(100, 65535, bitmap1);
    ASSERT_EQ(segBitMap.segments_.size(), 3);
    EXPECT_EQ(segBitMap.segments_[0].start, 100);
    EXPECT_EQ(segBitMap.segments_[0].end, 499);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap1);
    Bitmap result;
    result.Or(bitmap0);
    result.Or(bitmap1);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 1000);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result);
    EXPECT_EQ(segBitMap.segments_[2].start, 1001);
    EXPECT_EQ(segBitMap.segments_[2].end, 65535);
    EXPECT_EQ(segBitMap.segments_[2].bitmap, bitmap1);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap018, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    Bitmap bitmap2(2);
    segBitMap.AddMap(40, 800, bitmap0);
    segBitMap.AddMap(1000, 65535, bitmap1);
    segBitMap.AddMap(500, 5000, bitmap2);
    ASSERT_EQ(segBitMap.segments_.size(), 5);
    EXPECT_EQ(segBitMap.segments_[0].start, 40);
    EXPECT_EQ(segBitMap.segments_[0].end, 499);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap0);
    Bitmap result02;
    result02.Or(bitmap0);
    result02.Or(bitmap2);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 800);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result02);
    EXPECT_EQ(segBitMap.segments_[2].start, 801);
    EXPECT_EQ(segBitMap.segments_[2].end, 999);
    EXPECT_EQ(segBitMap.segments_[2].bitmap, bitmap2);
    Bitmap result12;
    result12.Or(bitmap1);
    result12.Or(bitmap2);
    EXPECT_EQ(segBitMap.segments_[3].start, 1000);
    EXPECT_EQ(segBitMap.segments_[3].end, 5000);
    EXPECT_EQ(segBitMap.segments_[3].bitmap, result12);
    EXPECT_EQ(segBitMap.segments_[4].start, 5001);
    EXPECT_EQ(segBitMap.segments_[4].end, 65535);
    EXPECT_EQ(segBitMap.segments_[4].bitmap, bitmap1);
}

HWTEST_F(NetsysNetFirewallTest, SegmentBitmapMap019, TestSize.Level0)
{
    SegmentBitmapMap segBitMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    Bitmap bitmap2(2);
    segBitMap.AddMap(40, 1000, bitmap0);
    segBitMap.AddMap(800, 65535, bitmap1);
    segBitMap.AddMap(500, 5000, bitmap2);
    ASSERT_EQ(segBitMap.segments_.size(), 5);
    EXPECT_EQ(segBitMap.segments_[0].start, 40);
    EXPECT_EQ(segBitMap.segments_[0].end, 499);
    EXPECT_EQ(segBitMap.segments_[0].bitmap, bitmap0);
    Bitmap result02;
    result02.Or(bitmap0);
    result02.Or(bitmap2);
    EXPECT_EQ(segBitMap.segments_[1].start, 500);
    EXPECT_EQ(segBitMap.segments_[1].end, 799);
    EXPECT_EQ(segBitMap.segments_[1].bitmap, result02);
    Bitmap result012;
    result012.Or(bitmap0);
    result012.Or(bitmap1);
    result012.Or(bitmap2);
    EXPECT_EQ(segBitMap.segments_[2].start, 800);
    EXPECT_EQ(segBitMap.segments_[2].end, 1000);
    EXPECT_EQ(segBitMap.segments_[2].bitmap, result012);
    Bitmap result12;
    result12.Or(bitmap1);
    result12.Or(bitmap2);
    EXPECT_EQ(segBitMap.segments_[3].start, 1001);
    EXPECT_EQ(segBitMap.segments_[3].end, 5000);
    EXPECT_EQ(segBitMap.segments_[3].bitmap, result12);
    EXPECT_EQ(segBitMap.segments_[4].start, 5001);
    EXPECT_EQ(segBitMap.segments_[4].end, 65535);
    EXPECT_EQ(segBitMap.segments_[4].bitmap, bitmap1);
}

HWTEST_F(NetsysNetFirewallTest, InterfaceBitmapBuild001, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    rule->interface = "lo";
    ruleList.push_back(rule);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    BpfInterfaceMap &interfaceMap = manager.GetInterfaceMap();
    EXPECT_FALSE(interfaceMap.Empty());
}

HWTEST_F(NetsysNetFirewallTest, InterfaceBitmapBuild002, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    ruleList.push_back(rule);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    BpfInterfaceMap &interfaceMap = manager.GetInterfaceMap();
    EXPECT_FALSE(interfaceMap.Empty());
}

HWTEST_F(NetsysNetFirewallTest, InterfaceBitmapBuild003, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule1 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    rule1->interface = "lo";
    ruleList.push_back(rule1);
    sptr<NetFirewallIpRule> rule2 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.102");
    rule2->interface = "lo";
    ruleList.push_back(rule2);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    BpfInterfaceMap &interfaceMap = manager.GetInterfaceMap();
    EXPECT_FALSE(interfaceMap.Empty());
    auto &map = interfaceMap.Get();
    EXPECT_EQ(map.size(), 2u);
    auto it = map.find("lo");
    if (it != map.end()) {
        EXPECT_NE(it->second.SpecialHash(), 0u);
    }
}

HWTEST_F(NetsysNetFirewallTest, InterfaceBitmapBuild004, TestSize.Level0)
{
    BpfInterfaceMap interfaceMap;
    Bitmap bitmap0(0);
    Bitmap bitmap1(1);
    interfaceMap.OrInsert("wlan0", bitmap0);
    interfaceMap.OrInsert("eth0", bitmap1);

    EXPECT_FALSE(interfaceMap.Empty());
    auto &map = interfaceMap.Get();
    EXPECT_EQ(map.size(), 2u);

    interfaceMap.Clear();
    EXPECT_TRUE(interfaceMap.Empty());
}

HWTEST_F(NetsysNetFirewallTest, InterfaceBitmapBuild005, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule1 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    rule1->interface = "wlan0";
    ruleList.push_back(rule1);
    sptr<NetFirewallIpRule> rule2 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.102");
    rule2->interface = "eth0";
    ruleList.push_back(rule2);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    BpfInterfaceMap &interfaceMap = manager.GetInterfaceMap();
    EXPECT_FALSE(interfaceMap.Empty());
    auto &map = interfaceMap.Get();
    EXPECT_EQ(map.size(), 3u);
    EXPECT_NE(map.find("wlan0"), map.end());
    EXPECT_NE(map.find("eth0"), map.end());
}

HWTEST_F(NetsysNetFirewallTest, InterfaceBitmapBuild006, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule1 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_OUT, "153.3.238.110");
    rule1->interface = "wlan0";
    ruleList.push_back(rule1);
    sptr<NetFirewallIpRule> rule2 = GeIpFirewallRule(NetFirewallRuleDirection::RULE_OUT, "153.3.238.102");
    ruleList.push_back(rule2);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    BpfInterfaceMap &interfaceMap = manager.GetInterfaceMap();
    EXPECT_FALSE(interfaceMap.Empty());
    auto &map = interfaceMap.Get();
    EXPECT_EQ(map.size(), 2u);
    EXPECT_NE(map.find("wlan0"), map.end());
    EXPECT_NE(map.find(std::string("")), map.end());
}

HWTEST_F(NetsysNetFirewallTest, InterfaceBitmapBuild007, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    rule->interface = "lo";
    ruleList.push_back(rule);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    BpfInterfaceMap &interfaceMap = manager.GetInterfaceMap();
    EXPECT_FALSE(interfaceMap.Empty());

    manager.Clear();
    EXPECT_TRUE(interfaceMap.Empty());
}

HWTEST_F(NetsysNetFirewallTest, NetsysNetFirewallTest007, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    rule->interface = "lo";
    ruleList.push_back(rule);

    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();
    int32_t ret = bpfNetFirewall->SetFirewallIpRules(ruleList);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNetFirewallTest, WriteInterfaceBpfMapEmptyTest, TestSize.Level0)
{
    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();
    BitmapManager manager;
    manager.Clear();
    int32_t ret = bpfNetFirewall->WriteInterfaceBpfMap(manager, NetFirewallRuleDirection::RULE_IN);
    EXPECT_EQ(ret, -1);

    ret = bpfNetFirewall->WriteInterfaceBpfMap(manager, NetFirewallRuleDirection::RULE_OUT);
    EXPECT_EQ(ret, -1);
}

HWTEST_F(NetsysNetFirewallTest, WriteInterfaceBpfMapLongNameTest, TestSize.Level0)
{
    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    rule->interface = "abcdefghijklmnopq";
    ruleList.push_back(rule);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    int32_t writeRet = bpfNetFirewall->WriteInterfaceBpfMap(manager, NetFirewallRuleDirection::RULE_IN);
    EXPECT_EQ(writeRet, 0);
}

HWTEST_F(NetsysNetFirewallTest, WriteInterfaceBpfMapEgressTest, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_OUT, "153.3.238.110");
    rule->interface = "eth0";
    ruleList.push_back(rule);

    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();
    int32_t ret = bpfNetFirewall->SetFirewallIpRules(ruleList);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetsysNetFirewallTest, InterfaceRebuildTest, TestSize.Level0)
{
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    ruleList.push_back(rule);

    BitmapManager manager;
    manager.BuildBitmapMap(ruleList);
    manager.BuildBitmapMap(ruleList);

    BpfInterfaceMap &interfaceMap = manager.GetInterfaceMap();
    EXPECT_FALSE(interfaceMap.Empty());
}

HWTEST_F(NetsysNetFirewallTest, WriteInterfaceBpfMapSuccessTest, TestSize.Level0)
{
    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();
    std::vector<sptr<NetFirewallIpRule>> ruleList;
    sptr<NetFirewallIpRule> rule = GeIpFirewallRule(NetFirewallRuleDirection::RULE_IN, "153.3.238.110");
    rule->interface = "lo";
    ruleList.push_back(rule);

    BitmapManager manager;
    int ret = manager.BuildBitmapMap(ruleList);
    EXPECT_EQ(ret, 0);

    int32_t writeRet = bpfNetFirewall->WriteInterfaceBpfMap(manager, NetFirewallRuleDirection::RULE_IN);
    EXPECT_EQ(writeRet, 0);
}

HWTEST_F(NetsysNetFirewallTest, ClearBpfFirewallRulesInterfaceTest, TestSize.Level0)
{
    shared_ptr<NetsysBpfNetFirewall> bpfNetFirewall = NetsysBpfNetFirewall::GetInstance();
    bpfNetFirewall->ClearBpfFirewallRules(NetFirewallRuleDirection::RULE_IN);
    bpfNetFirewall->ClearBpfFirewallRules(NetFirewallRuleDirection::RULE_OUT);
    EXPECT_NE(bpfNetFirewall, nullptr);
}

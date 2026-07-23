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

#include <atomic>
#include <chrono>
#include <thread>

#include <gtest/gtest.h>

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include "network.h"
#include "nat464_service.h"
#include "net_http_probe_result.h"
#include "probe_thread.h"
#include "net_connection.h"
#include "net_connection_adapter.h"
#include "net_probe_callback_test.h"
#include "net_conn_service.h"
#include "net_proxy_userinfo.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr const char *LOCAL_ROUTE_NEXT_HOP = "0.0.0.0";
constexpr const char *LOCAL_ROUTE_IPV6_DESTINATION = "::";
constexpr int32_t SUCCESS_CODE = 204;
constexpr int32_t PORTAL_CODE_MIN = 200;
constexpr int32_t PORTAL_CODE_MAX = 399;
constexpr size_t CURL_MAX_SIZE = 1024;
constexpr size_t CURL_MAX_NITEMS = 100;
constexpr int32_t HTTP_OK_CODE = 200;
constexpr int32_t DEFAULT_CONTENT_LENGTH_VALUE = -1;
constexpr int32_t MIN_VALID_CONTENT_LENGTH_VALUE = 5;
constexpr int32_t FAIL_CODE = 599;
constexpr int32_t PORTAL_CODE = 302;
constexpr int32_t HTTP_RES_CODE_BAD_REQUEST = 400;
constexpr int32_t HTTP_RES_CODE_CLIENT_ERRORS_MAX = 499;
const std::string CONNECTION_CLOSE_VALUE = "close";
const std::string CONNECTION_KEY = "Connection:";
const std::string CONTENT_LENGTH_KEY = "Content-Length:";
const std::string KEY_WORDS_REDIRECTION = "location.replace";
const std::string HTML_TITLE_HTTP_EN = "http://";
const std::string HTML_TITLE_HTTPS_EN = "https://";
constexpr int32_t VALID_NETID_START = 100;
constexpr int32_t PAC_URL_MAX_LEN = 1024;
constexpr int32_t BATCH_ROUTE_THRESHOLD = 1024;
constexpr int32_t DNS_NUM_TEST = 5;
constexpr uint32_t CALLBACK_WAIT_TIMEOUT_S = 1;
} // namespace

class NetworkTest : public testing::Test {
public:
    static void SetUpTestCase() {}

    static void TearDownTestCase() {}

    void SetUp() {}

    void TearDown() {}
};

HWTEST_F(NetworkTest, UpdateBasicNetworkTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    network->nat464Service_ = std::make_shared<Nat464Service>(netId, "ifaceName");
    auto ret = network->UpdateBasicNetwork(false);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, ReleaseVirtualNetworkTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    EXPECT_FALSE(network->isVirtualCreated_);
    auto ret = network->ReleaseVirtualNetwork();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, ReleaseVirtualNetworkTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    network->isVirtualCreated_ = true;
    INetAddr addr;
    network->netLinkInfo_.netAddrList_.push_back(addr);
    auto ret = network->ReleaseVirtualNetwork();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, ReleaseVirtualNetworkTest003, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    network->isVirtualCreated_ = true;
    INetAddr addr;
    addr.prefixlen_ = 24;
    network->netLinkInfo_.netAddrList_.push_back(addr);
    auto ret = network->ReleaseVirtualNetwork();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, GetNetLinkInfoTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_EQ(network->netSupplierType_, NetBearType::BEARER_VPN);
    network->GetNetLinkInfo();
}

HWTEST_F(NetworkTest, GetNetLinkInfoTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    Route route1;
    route1.destination_.address_ = LOCAL_ROUTE_NEXT_HOP;
    Route route2;
    route2.destination_.address_ = LOCAL_ROUTE_IPV6_DESTINATION;
    Route route3;
    route3.destination_.address_ = "192.168.1.1";
    network->netLinkInfo_.routeList_.push_back(route1);
    network->netLinkInfo_.routeList_.push_back(route2);
    network->netLinkInfo_.routeList_.push_back(route3);
    auto ret = network->GetNetLinkInfo();
    EXPECT_EQ(ret.routeList_.size(), 2);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoLinkTypeTest, TestSize.Level1)
{
    int32_t netId = 1;
    uint32_t supplierId = 1;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_CELLULAR, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    
    INetAddr addr1;
    addr1.address_ = "10.0.0.2";
    addr1.family_ = AF_INET;
    
    Route ipv4Route;
    ipv4Route.destination_.address_ = "10.0.0.0";
    ipv4Route.destination_.prefixlen_ = 24;
    ipv4Route.iface_ = "test";
    ipv4Route.gateway_.address_ = "10.0.0.1";
    
    network->netLinkInfo_.netAddrList_.push_back(addr1);
    network->netLinkInfo_.routeList_.push_back(ipv4Route);
    network->UpdateNetLinkInfoLinkType(network->netLinkInfo_);
    auto res1 = network->GetNetLinkInfo();
    EXPECT_TRUE(res1.isIpv4LinkValid_);
    EXPECT_FALSE(res1.isIpv6LinkValid_);

    INetAddr addr2;
    addr2.address_ = "2001:db8::1";
    addr2.family_ = AF_INET6;
    
    Route ipv6Route;
    ipv6Route.destination_.address_ = "2001:db8::";
    ipv6Route.destination_.prefixlen_ = 32;
    ipv6Route.iface_ = "test";
    ipv6Route.gateway_.address_ = "fe80::1";
    
    network->netLinkInfo_.netAddrList_.push_back(addr2);
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    network->UpdateNetLinkInfoLinkType(network->netLinkInfo_);
    auto res2 = network->GetNetLinkInfo();
    EXPECT_TRUE(res2.isIpv4LinkValid_);
    EXPECT_FALSE(res2.isIpv6LinkValid_);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoLinkTypeTest002, TestSize.Level1)
{
    int32_t netId = 1;
    uint32_t supplierId = 1;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_CELLULAR, nullptr);
    
    network->netLinkInfo_.ifaceName_ = "test";
    
    INetAddr addr2;
    addr2.address_ = "2001:4860::1";
    addr2.family_ = AF_INET6;
    
    Route ipv6Route;
    ipv6Route.destination_.address_ = "2001:4860::";
    ipv6Route.destination_.prefixlen_ = 32;
    ipv6Route.iface_ = "test";
    ipv6Route.gateway_.address_ = "fe80::1";
    
    network->netLinkInfo_.netAddrList_.push_back(addr2);
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    network->UpdateNetLinkInfoLinkType(network->netLinkInfo_);
    auto res2 = network->GetNetLinkInfo();
    EXPECT_FALSE(res2.isIpv4LinkValid_);
    EXPECT_TRUE(res2.isIpv6LinkValid_);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoLinkTypeTest003, TestSize.Level1)
{
    int32_t netId = 1;
    uint32_t supplierId = 1;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_CELLULAR, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";

    INetAddr addr3;
    addr3.address_ = "fe80::1";
    addr3.family_ = AF_INET6;
    network->netLinkInfo_.netAddrList_.push_back(addr3);

    Route ipv6Route;
    ipv6Route.destination_.address_ = "2001:4860::";
    ipv6Route.destination_.prefixlen_ = 32;
    ipv6Route.iface_ = "test";
    ipv6Route.gateway_.address_ = "fe80::1";
    
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    network->UpdateNetLinkInfoLinkType(network->netLinkInfo_);
    auto res3 = network->GetNetLinkInfo();
    EXPECT_FALSE(res3.isIpv4LinkValid_);
    EXPECT_FALSE(res3.isIpv6LinkValid_);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoLinkTypeTest004, TestSize.Level1)
{
    int32_t netId = 1;
    uint32_t supplierId = 1;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_CELLULAR, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    
    INetAddr addr4;
    addr4.address_ = "192.168.1.100";
    addr4.family_ = AF_INET;
    
    Route invalidIpv4Route;
    invalidIpv4Route.destination_.address_ = "invalid.address";
    invalidIpv4Route.destination_.prefixlen_ = 24;
    invalidIpv4Route.iface_ = "test";
    invalidIpv4Route.gateway_.address_ = "192.168.1.1";
    
    network->netLinkInfo_.netAddrList_.push_back(addr4);
    network->netLinkInfo_.routeList_.push_back(invalidIpv4Route);
    network->UpdateNetLinkInfoLinkType(network->netLinkInfo_);
    auto res4 = network->GetNetLinkInfo();
    EXPECT_FALSE(res4.isIpv4LinkValid_);
    EXPECT_FALSE(res4.isIpv6LinkValid_);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoLinkTypeTest005, TestSize.Level1)
{
    int32_t netId = 1;
    uint32_t supplierId = 1;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_CELLULAR, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    
    INetAddr addr5_ipv4;
    addr5_ipv4.address_ = "192.168.1.100";
    addr5_ipv4.family_ = AF_INET;
    
    INetAddr addr5_ipv6;
    addr5_ipv6.address_ = "2001:4860::1";
    addr5_ipv6.family_ = AF_INET6;
    
    Route ipv4Route, ipv6Route;
    ipv4Route.destination_.address_ = "192.168.1.0";
    ipv4Route.destination_.prefixlen_ = 24;
    ipv4Route.iface_ = "test";
    ipv4Route.gateway_.address_ = "192.168.1.1";
    
    ipv6Route.destination_.address_ = "2001:4860::";
    ipv6Route.destination_.prefixlen_ = 32;
    ipv6Route.iface_ = "test";
    ipv6Route.gateway_.address_ = "fe80::1";
    
    network->netLinkInfo_.netAddrList_.push_back(addr5_ipv4);
    network->netLinkInfo_.netAddrList_.push_back(addr5_ipv6);
    network->netLinkInfo_.routeList_.push_back(ipv4Route);
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    network->UpdateNetLinkInfoLinkType(network->netLinkInfo_);
    auto res5 = network->GetNetLinkInfo();
    EXPECT_TRUE(res5.isIpv4LinkValid_);
    EXPECT_TRUE(res5.isIpv6LinkValid_);
}

HWTEST_F(NetworkTest, IsValidIpRouteIpv4Test001, TestSize.Level1)
{
    int32_t netId = 1;
    uint32_t supplierId = 1;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_CELLULAR, nullptr);
    
    INetAddr ipv4Dest1;
    ipv4Dest1.address_ = "192.168.1.0/24";
    ipv4Dest1.family_ = AF_INET;
    EXPECT_TRUE(network->IsValidIpRoute(ipv4Dest1, "192.168.1.1"));
    
    INetAddr ipv4Dest2;
    ipv4Dest2.address_ = "192.168.1.0/24";
    ipv4Dest2.family_ = AF_INET;
    EXPECT_TRUE(network->IsValidIpRoute(ipv4Dest2, ""));
    
    INetAddr ipv4Dest3;
    ipv4Dest3.address_ = "192.168.1.0";
    ipv4Dest3.family_ = AF_INET;
    EXPECT_TRUE(network->IsValidIpRoute(ipv4Dest3, ""));
    
    INetAddr ipv4Dest4;
    ipv4Dest4.address_ = "192.168.1.0/24";
    ipv4Dest4.family_ = AF_INET;
    EXPECT_FALSE(network->IsValidIpRoute(ipv4Dest4, "invalid.gateway"));
    
    INetAddr ipv4Dest5;
    ipv4Dest5.address_ = "192.168.1.0/24";
    ipv4Dest5.family_ = AF_INET;
    EXPECT_FALSE(network->IsValidIpRoute(ipv4Dest5, "2001:4860::1"));
    
    INetAddr invalidDest1;
    invalidDest1.address_ = "invalid.ip.address";
    invalidDest1.family_ = AF_INET;
    EXPECT_FALSE(network->IsValidIpRoute(invalidDest1, ""));
}

HWTEST_F(NetworkTest, IsValidIpRouteIpv6Test001, TestSize.Level1)
{
    int32_t netId = 1;
    uint32_t supplierId = 1;
    auto network = std::make_shared<Network>(netId, supplierId, NetBearType::BEARER_CELLULAR, nullptr);
    
    INetAddr ipv6Dest1;
    ipv6Dest1.address_ = "2001:4860::/32";
    ipv6Dest1.family_ = AF_INET6;
    EXPECT_TRUE(network->IsValidIpRoute(ipv6Dest1, "2001:4860::1"));
    
    INetAddr ipv6Dest2;
    ipv6Dest2.address_ = "2001:4860::/32";
    ipv6Dest2.family_ = AF_INET6;
    EXPECT_TRUE(network->IsValidIpRoute(ipv6Dest2, ""));
    
    INetAddr ipv6Dest3;
    ipv6Dest3.address_ = "2001:4860::/1";
    ipv6Dest3.family_ = AF_INET6;
    EXPECT_TRUE(network->IsValidIpRoute(ipv6Dest3, "2001:4860::1"));
    
    INetAddr ipv6Dest4;
    ipv6Dest4.address_ = "2001:4860::/0";
    ipv6Dest4.family_ = AF_INET6;
    EXPECT_FALSE(network->IsValidIpRoute(ipv6Dest4, ""));
    
    INetAddr ipv6Dest5;
    ipv6Dest5.address_ = "2001:4860::/32";
    ipv6Dest5.family_ = AF_INET6;
    EXPECT_FALSE(network->IsValidIpRoute(ipv6Dest5, "invalid.ipv6.gateway"));
    
    INetAddr ipv6Dest6;
    ipv6Dest6.address_ = "2001:4860::/32";
    ipv6Dest6.family_ = AF_INET6;
    EXPECT_FALSE(network->IsValidIpRoute(ipv6Dest6, "192.168.1.1"));
    
    INetAddr invalidDest2;
    invalidDest2.address_ = "invalid.ipv6.format";
    invalidDest2.family_ = AF_INET6;
    EXPECT_FALSE(network->IsValidIpRoute(invalidDest2, ""));
    
    INetAddr invalidDest3;
    invalidDest3.address_ = "unknown.format";
    invalidDest3.family_ = AF_UNSPEC;
    EXPECT_FALSE(network->IsValidIpRoute(invalidDest3, ""));
}

HWTEST_F(NetworkTest, UpdateInterfacesTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    NetLinkInfo newNetLinkInfo;
    EXPECT_TRUE(newNetLinkInfo.ifaceName_.empty());
    network->UpdateInterfaces(newNetLinkInfo);
    EXPECT_TRUE(network->netLinkInfo_.ifaceName_.empty());
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    INetAddr addr1;
    INetAddr addr2;
    addr2.prefixlen_ = 24;
    network->netLinkInfo_.netAddrList_.push_back(addr1);
    network->netLinkInfo_.netAddrList_.push_back(addr2);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.netAddrList_.push_back(addr1);
    network->UpdateIpAddrs(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv4Addr;
    oldIpv4Addr.address_ = "192.168.1.100";
    oldIpv4Addr.prefixlen_ = 24;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv4Addr);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.netAddrList_.push_back(oldIpv4Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest003, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv6Addr;
    oldIpv6Addr.address_ = "2001:db8::1";
    oldIpv6Addr.prefixlen_ = 64;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv6Addr);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.netAddrList_.push_back(oldIpv6Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest004, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv4Addr;
    oldIpv4Addr.address_ = "192.168.1.100";
    oldIpv4Addr.prefixlen_ = 24;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv4Addr);
    Route ipv4Route;
    ipv4Route.destination_.type_ = INetAddr::IpType::IPV4;
    ipv4Route.iface_ = "rmnet0";
    ipv4Route.destination_.address_ = "192.168.1.0";
    ipv4Route.destination_.prefixlen_ = 24;
    ipv4Route.gateway_.address_ = "192.168.1.1";
    network->netLinkInfo_.routeList_.push_back(ipv4Route);
    NetLinkInfo newNetLinkInfo;
    INetAddr newIpv4Addr;
    newIpv4Addr.address_ = "10.0.0.2";
    newIpv4Addr.prefixlen_ = 24;
    newNetLinkInfo.netAddrList_.push_back(newIpv4Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_FALSE(ret);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 0);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest005, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv6Addr;
    oldIpv6Addr.address_ = "2001:db8::1";
    oldIpv6Addr.prefixlen_ = 64;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv6Addr);
    Route ipv6Route;
    ipv6Route.destination_.type_ = INetAddr::IpType::IPV6;
    ipv6Route.iface_ = "rmnet0";
    ipv6Route.destination_.address_ = "2001:db8::";
    ipv6Route.destination_.prefixlen_ = 64;
    ipv6Route.gateway_.address_ = "fe80::1";
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    NetLinkInfo newNetLinkInfo;
    INetAddr newIpv6Addr;
    newIpv6Addr.address_ = "2001:4860::1";
    newIpv6Addr.prefixlen_ = 64;
    newNetLinkInfo.netAddrList_.push_back(newIpv6Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_FALSE(ret);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 0);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest006, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv4Addr;
    oldIpv4Addr.address_ = "192.168.1.100";
    oldIpv4Addr.prefixlen_ = 24;
    INetAddr oldIpv6Addr;
    oldIpv6Addr.address_ = "2001:db8::1";
    oldIpv6Addr.prefixlen_ = 64;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv4Addr);
    network->netLinkInfo_.netAddrList_.push_back(oldIpv6Addr);
    Route ipv4Route;
    ipv4Route.destination_.type_ = INetAddr::IpType::IPV4;
    ipv4Route.iface_ = "rmnet0";
    ipv4Route.destination_.address_ = "192.168.1.0";
    ipv4Route.destination_.prefixlen_ = 24;
    ipv4Route.gateway_.address_ = "192.168.1.1";
    Route ipv6Route;
    ipv6Route.destination_.type_ = INetAddr::IpType::IPV6;
    ipv6Route.iface_ = "rmnet0";
    ipv6Route.destination_.address_ = "2001:db8::";
    ipv6Route.destination_.prefixlen_ = 64;
    ipv6Route.gateway_.address_ = "fe80::1";
    network->netLinkInfo_.routeList_.push_back(ipv4Route);
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.netAddrList_.push_back(oldIpv4Addr);
    newNetLinkInfo.netAddrList_.push_back(oldIpv6Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 2);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest007, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    EXPECT_TRUE(network->netLinkInfo_.netAddrList_.empty());
    NetLinkInfo newNetLinkInfo;
    INetAddr newAddr;
    newAddr.address_ = "10.0.0.2";
    newAddr.prefixlen_ = 24;
    newNetLinkInfo.netAddrList_.push_back(newAddr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest008, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv4Addr;
    oldIpv4Addr.address_ = "192.168.1.100";
    oldIpv4Addr.prefixlen_ = 24;
    INetAddr oldIpv6Addr;
    oldIpv6Addr.address_ = "2001:db8::1";
    oldIpv6Addr.prefixlen_ = 64;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv4Addr);
    network->netLinkInfo_.netAddrList_.push_back(oldIpv6Addr);
    Route ipv4Route;
    ipv4Route.destination_.type_ = INetAddr::IpType::IPV4;
    ipv4Route.iface_ = "rmnet0";
    ipv4Route.destination_.address_ = "192.168.1.0";
    ipv4Route.destination_.prefixlen_ = 24;
    ipv4Route.gateway_.address_ = "192.168.1.1";
    Route ipv6Route;
    ipv6Route.destination_.type_ = INetAddr::IpType::IPV6;
    ipv6Route.iface_ = "rmnet0";
    ipv6Route.destination_.address_ = "2001:db8::";
    ipv6Route.destination_.prefixlen_ = 64;
    ipv6Route.gateway_.address_ = "fe80::1";
    network->netLinkInfo_.routeList_.push_back(ipv4Route);
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    NetLinkInfo newNetLinkInfo;
    EXPECT_TRUE(newNetLinkInfo.netAddrList_.empty());
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_FALSE(ret);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 0);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest009, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv4Addr;
    oldIpv4Addr.address_ = "192.168.1.100";
    oldIpv4Addr.netMask_ = "255.255.255.0";
    oldIpv4Addr.prefixlen_ = 0;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv4Addr);
    NetLinkInfo newNetLinkInfo;
    INetAddr newIpv4Addr;
    newIpv4Addr.address_ = "10.0.0.2";
    newIpv4Addr.prefixlen_ = 24;
    newNetLinkInfo.netAddrList_.push_back(newIpv4Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest010, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv6Addr;
    oldIpv6Addr.address_ = "2001:db8::1";
    oldIpv6Addr.netMask_ = "ffff:ffff:ffff:ffff::";
    oldIpv6Addr.prefixlen_ = 0;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv6Addr);
    NetLinkInfo newNetLinkInfo;
    INetAddr newIpv6Addr;
    newIpv6Addr.address_ = "2001:4860::1";
    newIpv6Addr.prefixlen_ = 64;
    newNetLinkInfo.netAddrList_.push_back(newIpv6Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest011, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv4Addr;
    oldIpv4Addr.address_ = "192.168.1.100";
    oldIpv4Addr.prefixlen_ = 24;
    INetAddr oldIpv6Addr;
    oldIpv6Addr.address_ = "2001:db8::1";
    oldIpv6Addr.prefixlen_ = 64;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv4Addr);
    network->netLinkInfo_.netAddrList_.push_back(oldIpv6Addr);
    Route ipv4Route;
    ipv4Route.destination_.type_ = INetAddr::IpType::IPV4;
    ipv4Route.iface_ = "rmnet0";
    ipv4Route.destination_.address_ = "192.168.1.0";
    ipv4Route.destination_.prefixlen_ = 24;
    ipv4Route.gateway_.address_ = "192.168.1.1";
    Route ipv6Route;
    ipv6Route.destination_.type_ = INetAddr::IpType::IPV6;
    ipv6Route.iface_ = "rmnet0";
    ipv6Route.destination_.address_ = "2001:db8::";
    ipv6Route.destination_.prefixlen_ = 64;
    ipv6Route.gateway_.address_ = "fe80::1";
    network->netLinkInfo_.routeList_.push_back(ipv4Route);
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.netAddrList_.push_back(oldIpv4Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 1);
    EXPECT_EQ(network->netLinkInfo_.routeList_.front().destination_.type_, INetAddr::IpType::IPV4);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest012, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldIpv4Addr;
    oldIpv4Addr.address_ = "192.168.1.100";
    oldIpv4Addr.prefixlen_ = 24;
    INetAddr oldIpv6Addr;
    oldIpv6Addr.address_ = "2001:db8::1";
    oldIpv6Addr.prefixlen_ = 64;
    network->netLinkInfo_.netAddrList_.push_back(oldIpv4Addr);
    network->netLinkInfo_.netAddrList_.push_back(oldIpv6Addr);
    Route ipv4Route;
    ipv4Route.destination_.type_ = INetAddr::IpType::IPV4;
    ipv4Route.iface_ = "rmnet0";
    ipv4Route.destination_.address_ = "192.168.1.0";
    ipv4Route.destination_.prefixlen_ = 24;
    ipv4Route.gateway_.address_ = "192.168.1.1";
    Route ipv6Route;
    ipv6Route.destination_.type_ = INetAddr::IpType::IPV6;
    ipv6Route.iface_ = "rmnet0";
    ipv6Route.destination_.address_ = "2001:db8::";
    ipv6Route.destination_.prefixlen_ = 64;
    ipv6Route.gateway_.address_ = "fe80::1";
    network->netLinkInfo_.routeList_.push_back(ipv4Route);
    network->netLinkInfo_.routeList_.push_back(ipv6Route);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.netAddrList_.push_back(oldIpv6Addr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_TRUE(ret);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 1);
    EXPECT_EQ(network->netLinkInfo_.routeList_.front().destination_.type_, INetAddr::IpType::IPV6);
}

HWTEST_F(NetworkTest, UpdateIpAddrsTest013, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    ASSERT_NE(network, nullptr);
    INetAddr oldAddr1;
    oldAddr1.address_ = "192.168.1.100";
    oldAddr1.prefixlen_ = 24;
    INetAddr oldAddr2;
    oldAddr2.address_ = "192.168.2.100";
    oldAddr2.prefixlen_ = 24;
    network->netLinkInfo_.netAddrList_.push_back(oldAddr1);
    network->netLinkInfo_.netAddrList_.push_back(oldAddr2);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.netAddrList_.push_back(oldAddr1);
    INetAddr newAddr;
    newAddr.address_ = "10.0.0.2";
    newAddr.prefixlen_ = 24;
    newNetLinkInfo.netAddrList_.push_back(newAddr);
    auto ret = network->UpdateIpAddrs(newNetLinkInfo);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, RemoveRouteByFamily001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    Route route1;
    Route route2;
    route1.destination_.type_ = INetAddr::IpType::IPV4;
    route1.iface_ = "rmnet0";
    route1.destination_.address_ = "0.0.0.0";
    route1.destination_.prefixlen_ = 24;
    route1.gateway_.address_ = "0.0.0.0";
    route2.destination_.type_ = INetAddr::IpType::IPV6;
    route2.iface_ = "rmnet0";
    route2.destination_.address_ = "fe80::1";
    route2.destination_.prefixlen_ = 64;
    route2.gateway_.address_ = "fe80::1";
    network->netLinkInfo_.routeList_.push_back(route1);
    network->netLinkInfo_.routeList_.push_back(route2);

    network->RemoveRouteByFamily(INetAddr::IpType::IPV6);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 1);
}

HWTEST_F(NetworkTest, HandleUpdateIpAddrsTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    INetAddr addr1;
    INetAddr addr2;
    addr2.prefixlen_ = 24;
    network->netLinkInfo_.netAddrList_.push_back(addr1);
    network->netLinkInfo_.netAddrList_.push_back(addr2);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.netAddrList_.push_back(addr1);
    network->HandleUpdateIpAddrs(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    Route route;
    route.destination_.address_ = LOCAL_ROUTE_NEXT_HOP;
    network->netLinkInfo_.routeList_.push_back(route);
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.routeList_.push_back(route);
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    Route route;
    route.destination_.address_ = LOCAL_ROUTE_NEXT_HOP;
    network->netLinkInfo_.routeList_.push_back(route);
    NetLinkInfo newNetLinkInfo;
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest003, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    Route route;
    route.destination_.address_ = LOCAL_ROUTE_NEXT_HOP;
    network->netLinkInfo_.routeList_.push_back(route);
    NetLinkInfo newNetLinkInfo;
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest004, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    Route route;
    route.destination_.address_ = LOCAL_ROUTE_IPV6_DESTINATION;
    network->netLinkInfo_.routeList_.push_back(route);
    NetLinkInfo newNetLinkInfo;
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest005, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    Route route;
    route.destination_.address_ = "192.168.1.1";
    network->netLinkInfo_.routeList_.push_back(route);
    NetLinkInfo newNetLinkInfo;
    EXPECT_TRUE(newNetLinkInfo.routeList_.empty());
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest006, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    EXPECT_TRUE(network->netLinkInfo_.routeList_.empty());
    Route route;
    route.destination_.address_ = LOCAL_ROUTE_NEXT_HOP;
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.routeList_.push_back(route);
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest007, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    EXPECT_TRUE(network->netLinkInfo_.routeList_.empty());
    Route route;
    route.destination_.address_ = LOCAL_ROUTE_NEXT_HOP;
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.routeList_.push_back(route);
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest008, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    EXPECT_TRUE(network->netLinkInfo_.routeList_.empty());
    Route route;
    route.destination_.address_ = LOCAL_ROUTE_IPV6_DESTINATION;
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.routeList_.push_back(route);
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest009, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    EXPECT_TRUE(network->netLinkInfo_.routeList_.empty());
    Route route;
    route.destination_.address_ = "192.168.1.1";
    NetLinkInfo newNetLinkInfo;
    newNetLinkInfo.routeList_.push_back(route);
    network->UpdateRoutes(newNetLinkInfo);
}

HWTEST_F(NetworkTest, UpdateRoutesTest010, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    ASSERT_NE(network, nullptr);
    EXPECT_TRUE(network->netLinkInfo_.routeList_.empty());
    Route route;
    route.destination_.address_ = LOCAL_ROUTE_IPV6_DESTINATION;
    route.destination_.type_ = INetAddr::IPV6;
    network->netLinkInfo_.routeList_.push_back(route);

    NetLinkInfo newNetLinkInfo;
    route.destination_.address_ = "0.0.0.0";
    route.destination_.type_ = INetAddr::IPV4;
    newNetLinkInfo.routeList_.push_back(route);
    INetAddr addr;
    addr.address_ = "192.168.0.1";
    addr.type_ = INetAddr::IPV4;
    newNetLinkInfo.netAddrList_.push_back(addr);
    newNetLinkInfo.dnsList_.push_back(addr);
    network->UpdateRoutes(newNetLinkInfo);
    EXPECT_FALSE(network->netLinkInfo_.routeList_.empty());
}

HWTEST_F(NetworkTest, UpdateRoutesTest011, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    ASSERT_NE(network, nullptr);
    EXPECT_TRUE(network->netLinkInfo_.routeList_.empty());
    Route route1;
    route1.destination_.address_ = LOCAL_ROUTE_IPV6_DESTINATION;
    route1.destination_.type_ = INetAddr::IPV6;
    Route route2;
    std::string mockAddress = "";
    int32_t mockLength = 5000;
    for (uint32_t i = 0; i < mockLength; i++) {
        mockAddress += "1";
    }
    route2.iface_ = "rmnet0";
    route2.destination_.address_ = mockAddress;
    route2.destination_.type_ = INetAddr::IPV6;
    network->netLinkInfo_.routeList_.push_back(route1);
    network->netLinkInfo_.routeList_.push_back(route2);
    Route route3;
    Route route4;
    NetLinkInfo newNetLinkInfo;
    route3.destination_.address_ = "0.0.0.0";
    route3.destination_.type_ = INetAddr::IPV4;
    route4.destination_.address_ = mockAddress;
    route4.destination_.type_ = INetAddr::IPV6;
    route4.iface_ = "rmnet1";
    newNetLinkInfo.routeList_.push_back(route3);
    newNetLinkInfo.routeList_.push_back(route4);
    INetAddr addr;
    addr.address_ = "192.168.0.1";
    addr.type_ = INetAddr::IPV4;
    newNetLinkInfo.netAddrList_.push_back(addr);
    newNetLinkInfo.dnsList_.push_back(addr);
    network->UpdateRoutes(newNetLinkInfo);
    EXPECT_FALSE(network->netLinkInfo_.routeList_.empty());
}

HWTEST_F(NetworkTest, UpdateDnsTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetLinkInfo netLinkInfo;
    EXPECT_TRUE(netLinkInfo.dnsList_.empty());
    network->UpdateDns(netLinkInfo);
    NetManagerStandard::INetAddr dns;
    dns.type_ = NetManagerStandard::INetAddr::IPV4;
    NetManagerStandard::INetAddr ipv6Dns;
    ipv6Dns.type_ = NetManagerStandard::INetAddr::IPV6;
    for (int32_t i = 0 ; i < DNS_NUM_TEST; i++) {
        dns.address_ = "99.99.99.99";
        ipv6Dns.address_ = "fe80::99:99:99:99";
        netLinkInfo.dnsList_.push_back(dns);
        netLinkInfo.dnsList_.push_back(ipv6Dns);
    }
    network->UpdateDns(netLinkInfo);
    dns.type_ = NetManagerStandard::INetAddr::UNKNOWN;
    netLinkInfo.dnsList_.push_back(dns);
    network->UpdateDns(netLinkInfo);
    dns.address_ = "0.0.0.0";
    ipv6Dns.address_ = "::";
    netLinkInfo.dnsList_.push_back(dns);
    netLinkInfo.dnsList_.push_back(ipv6Dns);
    network->UpdateDns(netLinkInfo);
}

HWTEST_F(NetworkTest, UpdateTcpBufferSizeTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetLinkInfo netLinkInfo;
    netLinkInfo.tcpBufferSizes_ = "4096";
    EXPECT_NE(netLinkInfo.tcpBufferSizes_, network->netLinkInfo_.tcpBufferSizes_);
    network->UpdateTcpBufferSize(netLinkInfo);
}

HWTEST_F(NetworkTest, NetDetectionForDnsHealthTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    network->NetDetectionForDnsHealth(false);
    network->InitNetMonitor();
    EXPECT_NE(network->netMonitor_, nullptr);
    network->detectResult_ = INVALID_DETECTION_STATE;
    network->NetDetectionForDnsHealth(false);
}

HWTEST_F(NetworkTest, HandleNetMonitorResultTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetDetectionStatus state = UNKNOWN_STATE;
    std::string urlRedirect = "test";
    network->netCallback_ = nullptr;
    network->HandleNetMonitorResult(state, urlRedirect);
}

HWTEST_F(NetworkTest, HandleNetMonitorResultTest003, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetDetectionStatus state = UNKNOWN_STATE;
    std::string urlRedirect = "test";
    network->netCallback_ = [](uint32_t supplierId, NetDetectionStatus netState) {};
    network->detectResult_ = state;
    network->HandleNetMonitorResult(state, urlRedirect);
}

HWTEST_F(NetworkTest, HandleNetMonitorResultTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetDetectionStatus state = UNKNOWN_STATE;
    std::string urlRedirect = "test";
    network->netCallback_ = [](uint32_t supplierId, NetDetectionStatus netState) {};
    network->detectResult_ = INVALID_DETECTION_STATE;
    network->HandleNetMonitorResult(state, urlRedirect);
}

HWTEST_F(NetworkTest, NotifyNetDetectionResultTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetDetectionResultCode detectionResult = NET_DETECTION_FAIL;
    std::string urlRedirect = "test";
    network->netDetectionRetCallback_.push_back(nullptr);
    network->NotifyNetDetectionResult(detectionResult, urlRedirect);
}

HWTEST_F(NetworkTest, NetDetectionResultConvertTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    int32_t internalRet = static_cast<int32_t>(INVALID_DETECTION_STATE);
    auto ret = network->NetDetectionResultConvert(internalRet);
    EXPECT_EQ(ret, NET_DETECTION_FAIL);
}

HWTEST_F(NetworkTest, UpdateNetConnStateTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    network->state_ = NET_CONN_STATE_CONNECTED;
    network->nat464Service_ = nullptr;
    EXPECT_TRUE(network->netLinkInfo_.netAddrList_.empty());
    NetConnState netConnState = NET_CONN_STATE_IDLE;
    network->UpdateNetConnState(netConnState);
}

HWTEST_F(NetworkTest, UpdateNetConnStateTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    network->state_ = NET_CONN_STATE_CONNECTED;
    network->nat464Service_ = std::make_shared<Nat464Service>(netId, "test");
    EXPECT_TRUE(network->netLinkInfo_.netAddrList_.empty());
    NetConnState netConnState = NET_CONN_STATE_IDLE;
    network->UpdateNetConnState(netConnState);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    auto ret = network->UpdateNetLinkInfo(network->netLinkInfo_);
    EXPECT_TRUE(ret);
    network->netSupplierType_ = NetBearType::BEARER_CELLULAR;
    network->isSupportInternet_ = true;
    network->netMonitor_ = nullptr;
    ret = network->UpdateNetLinkInfo(network->netLinkInfo_);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_CELLULAR, nullptr);
    network->InitNetMonitor();
    EXPECT_NE(network->netMonitor_, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    INetAddr addr1;
    INetAddr addr2;
    addr1.address_ = "10.0.0.2";
    addr2.address_ = "fe80::";
    network->netLinkInfo_.netAddrList_.push_back(addr1);
    network->isSupportInternet_ = true;
    auto ret = network->UpdateNetLinkInfo(network->netLinkInfo_);
    EXPECT_TRUE(ret);
    network->netLinkInfo_.netAddrList_.push_back(addr2);
    ret = network->UpdateNetLinkInfo(network->netLinkInfo_);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoTest003, TestSize.Level1)
{
    auto network = std::make_shared<Network>(1, 2000, NetBearType::BEARER_CELLULAR, nullptr);
    network->UpdateNetLinkInfo(network->netLinkInfo_);
    EXPECT_FALSE(network->isSupportInternet_);
    EXPECT_FALSE(network->IsIfaceNameInUse());
    network->isSupportInternet_ = true;
    network->UpdateNetLinkInfo(network->netLinkInfo_);
}

HWTEST_F(NetworkTest, UpdateNetLinkInfoTest004, TestSize.Level1)
{
    auto network1 = std::make_shared<Network>(1, 2000, NetBearType::BEARER_VPN, nullptr);
    network1->netLinkInfo_.ifaceName_ = "test";
    auto network2 = std::make_shared<Network>(2, 2000, NetBearType::BEARER_VPN, nullptr);
    network2->netLinkInfo_.ifaceName_ = "test";
    std::set<NetCap> netCaps;
    NetConnService::GetInstance()->netSuppliers_[2000] =
        sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_VPN, "", netCaps);
    NetConnService::GetInstance()->netSuppliers_[2000]->SetNetwork(network2);
    NetConnService::GetInstance()->netSuppliers_[2000]->netSupplierInfo_.isAvailable_ = true;
    network1->UpdateNetLinkInfo(network1->netLinkInfo_);
    EXPECT_FALSE(network1->isSupportInternet_);
    EXPECT_TRUE(network1->IsIfaceNameInUse());
    network1->isSupportInternet_ = true;
    network1->UpdateNetLinkInfo(network1->netLinkInfo_);
    NetConnService::GetInstance()->netSuppliers_.erase(2000);
}

HWTEST_F(NetworkTest, DelayStartDetectionTest001, TestSize.Level1)
{
    bool hasSameIpAddr = false;
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    network->InitNetMonitor();
    EXPECT_NE(network->netMonitor_, nullptr);
    auto ret = network->DelayStartDetectionForIpUpdate(hasSameIpAddr);
    EXPECT_FALSE(ret);
    hasSameIpAddr = true;
    ret = network->DelayStartDetectionForIpUpdate(hasSameIpAddr);
    EXPECT_FALSE(ret);
    network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_CELLULAR, nullptr);
    network->InitNetMonitor();
    ret = network->DelayStartDetectionForIpUpdate(hasSameIpAddr);
    EXPECT_TRUE(network->netMonitor_->IsDetecting());
    network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, nullptr);
    network->InitNetMonitor();
    network->netMonitor_->Stop();
    ret = network->DelayStartDetectionForIpUpdate(hasSameIpAddr);
    EXPECT_FALSE(network->netMonitor_->IsDetecting());
}

HWTEST_F(NetworkTest, DelayStartDetectionTest002, TestSize.Level1)
{
    bool hasSameIpAddr = true;
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, nullptr);
    network->InitNetMonitor();
    uint64_t nowTime = CommonUtils::GetCurrentMilliSecond();
    network->netMonitor_->lastDetectTimestamp_ = nowTime - 30;
    std::cout << "last_lapse_ms:" << nowTime - network->netMonitor_->GetLastDetectTime();

    auto ret = network->DelayStartDetectionForIpUpdate(hasSameIpAddr);
    EXPECT_TRUE(ret);
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("RUNNER_CREATE");
    if (runner) {
        network->eventHandler_ = std::make_shared<NetConnEventHandler>(runner);
        ret = network->DelayStartDetectionForIpUpdate(hasSameIpAddr);
        EXPECT_TRUE(ret);
    }
    if (network->netMonitor_) {
        nowTime = CommonUtils::GetCurrentMilliSecond();
        network->netMonitor_->lastDetectTimestamp_ = nowTime - 300;
        std::cout << "last_lapse_ms:" << nowTime - network->netMonitor_->GetLastDetectTime();
        nowTime = CommonUtils::GetCurrentMilliSecond();
        ret = network->DelayStartDetectionForIpUpdate(hasSameIpAddr);
    }
}

HWTEST_F(NetworkTest, DelayStartDetectionTest003, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_CELLULAR, nullptr);
    network->netMonitor_ = nullptr;
    EXPECT_FALSE(network->DelayStartDetectionForIpUpdate(true));
    network->InitNetMonitor();
    network->InitNetMonitor();
    network->StartNetDetection(true);
    network->StartNetDetection(false);
    network->StopNetDetection();
    EXPECT_EQ(network->netMonitor_, nullptr);
}

HWTEST_F(NetworkTest, IsNat464PreferedTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    EXPECT_TRUE(network->netLinkInfo_.netAddrList_.empty());
    auto ret = network->IsNat464Prefered();
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, IsNat464PreferedTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    network->netLinkInfo_.ifaceName_ = "test";
    network->state_ = NET_CONN_STATE_CONNECTED;
    EXPECT_TRUE(network->netLinkInfo_.netAddrList_.empty());
    auto ret = network->IsNat464Prefered();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, SetScreenStateTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_EQ(network->netMonitor_, nullptr);
    network->SetScreenState(false);
}

HWTEST_F(NetworkTest, SetScreenStateTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    network->InitNetMonitor();
    EXPECT_NE(network->netMonitor_, nullptr);
    network->InitNetMonitor();
    EXPECT_NE(network->netMonitor_, nullptr);
    network->SetScreenState(false);
}

HWTEST_F(NetworkTest, MaybeUpdateV6IfaceTest001, TestSize.Level1)
{
    int32_t netId = 1;
    std::string v6Iface;
    auto service = std::make_shared<Nat464Service>(netId, v6Iface);
    EXPECT_NE(service, nullptr);
    service->MaybeUpdateV6Iface(v6Iface);

    service->serviceState_ = NAT464_SERVICE_STATE_DISCOVERING;
    service->MaybeUpdateV6Iface(v6Iface);
}

HWTEST_F(NetworkTest, UpdateServiceStateTest001, TestSize.Level1)
{
    int32_t netId = 1;
    std::string v6Iface;
    auto service = std::make_shared<Nat464Service>(netId, v6Iface);
    EXPECT_NE(service, nullptr);
    Nat464UpdateFlag updateFlag = NAT464_SERVICE_STOP;
    service->UpdateServiceState(updateFlag);
    EXPECT_EQ(service->serviceState_, NAT464_SERVICE_STATE_IDLE);

    updateFlag = NAT464_SERVICE_CONTINUE;
    service->UpdateServiceState(updateFlag);
    EXPECT_EQ(service->serviceState_, NAT464_SERVICE_STATE_DISCOVERING);
}

HWTEST_F(NetworkTest, UpdateServiceStateTest002, TestSize.Level1)
{
    int32_t netId = 1;
    std::string v6Iface;
    auto service = std::make_shared<Nat464Service>(netId, v6Iface);
    EXPECT_NE(service, nullptr);
    service->serviceState_ = NAT464_SERVICE_STATE_DISCOVERING;
    Nat464UpdateFlag updateFlag = NAT464_SERVICE_STOP;
    service->UpdateServiceState(updateFlag);
    EXPECT_EQ(service->serviceState_, NAT464_SERVICE_STATE_IDLE);

    service->serviceState_ = NAT464_SERVICE_STATE_DISCOVERING;
    updateFlag = NAT464_SERVICE_CONTINUE;
    EXPECT_TRUE(service->nat64PrefixFromDns_.address_.empty());
    service->UpdateServiceState(updateFlag);
    EXPECT_EQ(service->serviceState_, NAT464_SERVICE_STATE_DISCOVERING);

    service->nat64PrefixFromDns_.address_ = "test";
    service->UpdateServiceState(updateFlag);
    EXPECT_EQ(service->serviceState_, NAT464_SERVICE_STATE_RUNNING);
}

HWTEST_F(NetworkTest, UpdateServiceStateTest003, TestSize.Level1)
{
    int32_t netId = 1;
    std::string v6Iface;
    auto service = std::make_shared<Nat464Service>(netId, v6Iface);
    EXPECT_NE(service, nullptr);
    service->serviceState_ = NAT464_SERVICE_STATE_RUNNING;
    Nat464UpdateFlag updateFlag = NAT464_SERVICE_CONTINUE;
    service->UpdateServiceState(updateFlag);

    updateFlag = NAT464_SERVICE_STOP;
    service->UpdateServiceState(updateFlag);
    EXPECT_EQ(service->serviceState_, NAT464_SERVICE_STATE_IDLE);

    service->serviceState_ = static_cast<Nat464ServiceState>(-1);
    service->UpdateServiceState(updateFlag);
}

HWTEST_F(NetworkTest, DiscoverPrefixTest001, TestSize.Level1)
{
    int32_t netId = 1;
    std::string v6Iface;
    auto service = std::make_shared<Nat464Service>(netId, v6Iface);
    EXPECT_NE(service, nullptr);
    service->tryStopDiscovery_ = true;
    service->DiscoverPrefix();
    EXPECT_FALSE(service->tryStopDiscovery_);
}

HWTEST_F(NetworkTest, DiscoverPrefixTest002, TestSize.Level1)
{
    int32_t netId = 1;
    std::string v6Iface;
    auto service = std::make_shared<Nat464Service>(netId, v6Iface);
    EXPECT_NE(service, nullptr);
    EXPECT_FALSE(service->tryStopDiscovery_);
    EXPECT_FALSE(service->GetPrefixFromDns64());
    service->DiscoverPrefix();
}

HWTEST_F(NetworkTest, GetPrefixFromDns64Test001, TestSize.Level1)
{
    int32_t netId = 1;
    std::string v6Iface;
    auto service = std::make_shared<Nat464Service>(netId, v6Iface);
    EXPECT_NE(service, nullptr);
    auto ret = service->GetPrefixFromDns64();
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, StartServiceTest001, TestSize.Level1)
{
    int32_t netId = 1;
    std::string v6Iface;
    auto service = std::make_shared<Nat464Service>(netId, v6Iface);
    EXPECT_NE(service, nullptr);
    service->serviceState_ = NAT464_SERVICE_STATE_RUNNING;
    service->StartService();

    service->serviceState_ = NAT464_SERVICE_STATE_DISCOVERING;
    service->StartService();
}

HWTEST_F(NetworkTest, NetHttpProbeResultTest001, TestSize.Level1)
{
    NetHttpProbeResult result1;
    NetHttpProbeResult result2;
    result1.responseCode_ = SUCCESS_CODE;
    auto ret = result1 == result2;
    EXPECT_FALSE(ret);

    result2.responseCode_ = SUCCESS_CODE;
    ret = result1 == result2;
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, NetHttpProbeResultTest002, TestSize.Level1)
{
    NetHttpProbeResult result1;
    NetHttpProbeResult result2;
    result1.responseCode_ = PORTAL_CODE_MIN;
    auto ret = result1 == result2;
    EXPECT_FALSE(ret);

    result2.responseCode_ = PORTAL_CODE_MIN;
    EXPECT_EQ(result1.redirectUrl_, result2.redirectUrl_);
    ret = result1 == result2;
    EXPECT_TRUE(ret);

    result1.redirectUrl_ = "test";
    ret = result1 == result2;
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, NetHttpProbeResultTest003, TestSize.Level1)
{
    NetHttpProbeResult result1;
    NetHttpProbeResult result2;
    auto ret = result1 == result2;
    EXPECT_TRUE(ret);

    result2.responseCode_ = SUCCESS_CODE;
    ret = result1 == result2;
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, SendHttpProbeTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    std::string httpUrl = "test";
    std::string httpsUrl = "test";
    ProbeType probeType = PROBE_HTTP;
    auto latch = std::make_shared<TinyCountDownLatch>(0);
    auto probeThread = std::make_shared<ProbeThread>(netId, BEARER_CELLULAR, netLinkInfo,
        latch, latch, probeType, httpUrl, httpsUrl);
    probeThread->httpProbe_ = nullptr;
    probeThread->SendHttpProbe(probeType);
    EXPECT_FALSE(probeThread->isDetecting_);
}

HWTEST_F(NetworkTest, IsConclusiveResultTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    std::string httpUrl = "test";
    std::string httpsUrl = "test";
    ProbeType probeType = PROBE_HTTP_HTTPS;
    auto probeThread = std::make_shared<ProbeThread>(netId, BEARER_CELLULAR, netLinkInfo,
        nullptr, nullptr, probeType, httpUrl, httpsUrl);
    auto ret = probeThread->IsConclusiveResult();
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, IsConclusiveResultTest002, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    std::string httpUrl = "test";
    std::string httpsUrl = "test";
    ProbeType probeType = PROBE_HTTP;
    auto probeThread = std::make_shared<ProbeThread>(netId, BEARER_CELLULAR, netLinkInfo,
        nullptr, nullptr, probeType, httpUrl, httpsUrl);
    probeThread->httpProbe_->httpProbeResult_.responseCode_ = SUCCESS_CODE;
    auto ret = probeThread->IsConclusiveResult();
    EXPECT_FALSE(ret);

    probeThread->probeType_ = PROBE_HTTP_FALLBACK;
    ret = probeThread->IsConclusiveResult();
    EXPECT_FALSE(ret);

    probeThread->httpProbe_->httpProbeResult_.responseCode_ = PORTAL_CODE_MIN;
    ret = probeThread->IsConclusiveResult();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, IsConclusiveResultTest003, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    std::string httpUrl = "test";
    std::string httpsUrl = "test";
    ProbeType probeType = PROBE_HTTPS;
    auto probeThread = std::make_shared<ProbeThread>(netId, BEARER_CELLULAR, netLinkInfo,
        nullptr, nullptr, probeType, httpUrl, httpsUrl);
    probeThread->httpProbe_->httpsProbeResult_.responseCode_ = PORTAL_CODE_MIN;
    auto ret = probeThread->IsConclusiveResult();
    EXPECT_FALSE(ret);

    probeThread->probeType_ = PROBE_HTTPS_FALLBACK;
    ret = probeThread->IsConclusiveResult();
    EXPECT_FALSE(ret);

    probeThread->httpProbe_->httpsProbeResult_.responseCode_ = SUCCESS_CODE;
    ret = probeThread->IsConclusiveResult();
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, UpdateGlobalHttpProxyTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    std::string httpUrl = "test";
    std::string httpsUrl = "test";
    ProbeType probeType = PROBE_HTTP;
    auto probeThread = std::make_shared<ProbeThread>(netId, BEARER_CELLULAR, netLinkInfo,
        nullptr, nullptr, probeType, httpUrl, httpsUrl);
    EXPECT_NE(probeThread->httpProbe_, nullptr);
    probeThread->httpProbe_ = nullptr;
    HttpProxy httpProxy;
    probeThread->UpdateGlobalHttpProxy(httpProxy);
}

HWTEST_F(NetworkTest, UpdateGlobalHttpProxyTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_CELLULAR, nullptr);
    network->netMonitor_ = nullptr;
    HttpProxy httpProxy;
    network->UpdateGlobalHttpProxy(httpProxy);
    network->InitNetMonitor();
    network->UpdateGlobalHttpProxy(httpProxy);
    EXPECT_NE(network->netMonitor_, nullptr);
}

HWTEST_F(NetworkTest, CurlGlobalCleanupTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_CELLULAR, netLinkInfo, probeType);
    EXPECT_NE(probe, nullptr);
    probe->useCurlCount_ = 0;
    probe->CurlGlobalCleanup();
}

HWTEST_F(NetworkTest, CleanHttpCurlTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_CELLULAR, netLinkInfo, probeType);
    probe->httpCurl_ = curl_easy_init();
    EXPECT_NE(probe->httpCurl_, nullptr);
    probe->curlMulti_ = nullptr;
    probe->CleanHttpCurl();
}

HWTEST_F(NetworkTest, CleanHttpCurlTest002, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_CELLULAR, netLinkInfo, probeType);
    probe->httpsCurl_ = curl_easy_init();
    EXPECT_NE(probe->httpsCurl_, nullptr);
    probe->curlMulti_ = nullptr;
    probe->CleanHttpCurl();
}

HWTEST_F(NetworkTest, ExtractDomainFormUrlTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_CELLULAR, netLinkInfo, probeType);
    std::string url = "http://example.com";
    auto ret = probe->ExtractDomainFormUrl(url);
    EXPECT_EQ(ret, "example.com");
}

HWTEST_F(NetworkTest, HeaderCallbackTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_CELLULAR, netLinkInfo, probeType);
    char* buffer = nullptr;
    size_t size = CURL_MAX_SIZE;
    size_t nitems = CURL_MAX_NITEMS + 1;
    void* userdata = nullptr;
    auto ret = probe->HeaderCallback(buffer, size, nitems, userdata);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetworkTest, HeaderCallbackTest002, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_CELLULAR, netLinkInfo, probeType);
    char* buffer = nullptr;
    size_t size = 1;
    size_t nitems = 1;
    void* userdata = nullptr;
    auto ret = probe->HeaderCallback(buffer, size, nitems, userdata);
    EXPECT_EQ(ret, size * nitems);
}

HWTEST_F(NetworkTest, HeaderCallbackTest003, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_CELLULAR, netLinkInfo, probeType);
    char* buffer = nullptr;
    size_t size = 1;
    size_t nitems = 1;
    std::string data;
    void* userdata = static_cast<void*>(&data);
    auto ret = probe->HeaderCallback(buffer, size, nitems, userdata);
    EXPECT_EQ(ret, size * nitems);

    char buf[10] = "1";
    ret = probe->HeaderCallback(buf, size, nitems, userdata);
    EXPECT_EQ(ret, size * nitems);
    EXPECT_EQ(data, "1");
}

HWTEST_F(NetworkTest, SetHttpOptionsTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTPS;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_CELLULAR, netLinkInfo, probeType);
    std::string url;
    CURL *curl = curl_easy_init();
    auto ret = probe->SetHttpOptions(probeType, curl, url);
    EXPECT_FALSE(ret);

    url = "http://example.com";
    ret = probe->SetHttpOptions(probeType, curl, url);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, SetProxyOptionTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTPS;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    std::string url = "http://";
    bool useHttpProxy = true;
    probe->globalHttpProxy_.host_ = url;
    probe->globalHttpProxy_.port_ = 1;
    EXPECT_TRUE(probe->defaultUseGlobalHttpProxy_);
    auto ret = probe->SetProxyOption(probeType, useHttpProxy);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, SetProxyOptionTest002, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    std::string url = "http://192.168.1.1";
    bool useHttpProxy = true;
    probe->globalHttpProxy_.host_ = url;
    probe->globalHttpProxy_.port_ = 1;
    EXPECT_EQ(probe->httpCurl_, nullptr);
    EXPECT_TRUE(probe->defaultUseGlobalHttpProxy_);
    auto ret = probe->SetProxyOption(probeType, useHttpProxy);
    EXPECT_FALSE(ret);

    probe->httpCurl_ = curl_easy_init();
    EXPECT_NE(probe->httpCurl_, nullptr);
    ret = probe->SetProxyOption(probeType, useHttpProxy);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, SetProxyOptionTest003, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTPS;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    std::string url = "http://192.168.1.1";
    bool useHttpProxy = true;
    probe->globalHttpProxy_.host_ = url;
    probe->globalHttpProxy_.port_ = 1;
    EXPECT_EQ(probe->httpCurl_, nullptr);
    EXPECT_TRUE(probe->defaultUseGlobalHttpProxy_);
    auto ret = probe->SetProxyOption(probeType, useHttpProxy);
    EXPECT_FALSE(ret);

    probe->httpCurl_ = curl_easy_init();
    EXPECT_NE(probe->httpCurl_, nullptr);
    ret = probe->SetProxyOption(probeType, useHttpProxy);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, SetProxyInfoTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    CURL *curlHandler = nullptr;
    std::string proxyHost = "http://192.168.1.1";
    int32_t proxyPort = 1;
    auto ret = probe->SetProxyInfo(curlHandler, proxyHost, proxyPort);
    EXPECT_FALSE(ret);

    probe->httpCurl_ = curl_easy_init();
    EXPECT_NE(probe->httpCurl_, nullptr);
    ret = probe->SetProxyInfo(curlHandler, proxyHost, proxyPort);
    EXPECT_FALSE(ret);
}

HWTEST_F(NetworkTest, SetUserInfoTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    CURL *curlHandler = curl_easy_init();
    ASSERT_NE(curlHandler, nullptr);
    HttpProxy httpProxy = {"127.0.0.1", 8080, {}};
    SecureData username;
    username.append("testuser", strlen("testuser"));
    SecureData password;
    password.append("testpass", strlen("testpass"));
    httpProxy.SetUserName(username);
    httpProxy.SetPassword(password);
    NetProxyUserinfo::GetInstance().SaveHttpProxyHostPass(httpProxy);
    auto ret = probe->SetUserInfo(curlHandler);
    EXPECT_TRUE(ret);
    curl_easy_cleanup(curlHandler);
}

HWTEST_F(NetworkTest, SetUserInfoTest002, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    CURL *curlHandler = curl_easy_init();
    ASSERT_NE(curlHandler, nullptr);
    HttpProxy httpProxy = {"127.0.0.1", 8080, {}};
    SecureData username;
    username.append("testuser", strlen("testuser"));
    httpProxy.SetUserName(username);
    NetProxyUserinfo::GetInstance().SaveHttpProxyHostPass(httpProxy);
    auto ret = probe->SetUserInfo(curlHandler);
    EXPECT_TRUE(ret);
    curl_easy_cleanup(curlHandler);
}

HWTEST_F(NetworkTest, SetResolveOptionTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP_HTTPS;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    std::string domain = "test";
    std::string ipAddress = "test";
    int32_t port = 1;
    auto ret = probe->SetResolveOption(probeType, domain, ipAddress, port);
    EXPECT_TRUE(ret);
}

HWTEST_F(NetworkTest, GetHeaderFieldTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    std::string key = "test";
    auto ret = probe->GetHeaderField(key);
    EXPECT_EQ(ret, "");
}

HWTEST_F(NetworkTest, GetHeaderFieldTest002, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    probe->respHeader_ = "test";
    std::string key = "1";
    auto ret = probe->GetHeaderField(key);
    EXPECT_EQ(ret, "");

    key = "t";
    ret = probe->GetHeaderField(key);
    EXPECT_NE(ret, "");
}

HWTEST_F(NetworkTest, CheckRespCodeTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    int32_t respCode = HTTP_RES_CODE_CLIENT_ERRORS_MAX + 1;
    auto ret = probe->CheckRespCode(respCode);
    EXPECT_EQ(ret, respCode);
}

HWTEST_F(NetworkTest, CheckRespCodeTest002, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTPS;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    int32_t respCode = HTTP_OK_CODE;
    auto ret = probe->CheckRespCode(respCode);
    EXPECT_EQ(ret, FAIL_CODE);

    probe->respHeader_ = "test";
    ret = probe->CheckRespCode(respCode);
    EXPECT_EQ(ret, HTTP_OK_CODE);

    probe->respHeader_ = CONTENT_LENGTH_KEY + "1\r\n";
    ret = probe->CheckRespCode(respCode);
    EXPECT_EQ(ret, FAIL_CODE);
}

HWTEST_F(NetworkTest, CheckRespCodeTest003, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTPS;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    int32_t respCode = HTTP_OK_CODE;
    probe->respHeader_ =  CONTENT_LENGTH_KEY + "123\r\n";
    probe->respHeader_ += CONNECTION_KEY + CONNECTION_CLOSE_VALUE + "\r\n";
    auto ret = probe->CheckRespCode(respCode);
    EXPECT_EQ(ret, HTTP_OK_CODE);

    probe->probeType_ = PROBE_HTTP;
    ret = probe->CheckRespCode(respCode);
    EXPECT_EQ(ret, FAIL_CODE);
}

HWTEST_F(NetworkTest, CheckClientErrorRespCodeTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    int32_t respCode = HTTP_RES_CODE_BAD_REQUEST;
    auto ret = probe->CheckClientErrorRespCode(respCode);
    EXPECT_EQ(ret, HTTP_RES_CODE_BAD_REQUEST);

    strcpy_s(probe->errBuffer, CURL_ERROR_SIZE, HTML_TITLE_HTTP_EN.c_str());
    ret = probe->CheckClientErrorRespCode(respCode);
    EXPECT_EQ(ret, HTTP_RES_CODE_BAD_REQUEST);

    std::string errMsg = HTML_TITLE_HTTPS_EN + KEY_WORDS_REDIRECTION;
    strcpy_s(probe->errBuffer, CURL_ERROR_SIZE, errMsg.c_str());
    ret = probe->CheckClientErrorRespCode(respCode);
    EXPECT_EQ(ret, PORTAL_CODE);
}

HWTEST_F(NetworkTest, CheckSuccessRespCodeTest001, TestSize.Level1)
{
    uint32_t netId = 1;
    NetLinkInfo netLinkInfo;
    ProbeType probeType = PROBE_HTTP;
    auto probe = std::make_shared<NetHttpProbe>(netId, BEARER_WIFI, netLinkInfo, probeType);
    int32_t respCode = HTTP_RES_CODE_BAD_REQUEST;
    auto ret = probe->CheckSuccessRespCode(respCode);
    EXPECT_EQ(ret, HTTP_RES_CODE_BAD_REQUEST);
 
    probe->respHeader_ = "";
    respCode = SUCCESS_CODE;
    ret = probe->CheckSuccessRespCode(respCode);
    EXPECT_GE(ret, PORTAL_CODE);
 
    probe->respHeader_ = "X-Hwcloud-ReqId:12345678910";
    respCode = SUCCESS_CODE;
    probe->CheckSuccessRespCode(respCode);
 
    probe->respHeader_ = "X-Hwcloud-ReqId:40483ead9aeb8af136beb74071f1365f";
    probe->CheckSuccessRespCode(respCode);
}

HWTEST_F(NetworkTest, OH_NetConn_GetAddrInfoTest001, TestSize.Level1)
{
    char *host = nullptr;
    char *serv = nullptr;
    struct addrinfo *hint = nullptr;
    struct addrinfo **res = nullptr;
    int32_t netId = 1;
    auto ret = OH_NetConn_GetAddrInfo(host, serv, hint, res, netId);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    char host1[] = "192.168.1.1";
    ret = OH_NetConn_GetAddrInfo(host1, serv, hint, res, netId);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_GetAddrInfoTest002, TestSize.Level1)
{
    char host[] = "";
    char *serv = nullptr;
    struct addrinfo info;
    struct addrinfo *hint = &info;
    struct addrinfo **res = &hint;
    int32_t netId = 1;
    auto ret = OH_NetConn_GetAddrInfo(host, serv, hint, res, netId);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    char host1[] = "192.168.1.1";
    ret = OH_NetConn_GetAddrInfo(host1, serv, hint, res, netId);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    netId = -1;
    OH_NetConn_GetAddrInfo(host, serv, hint, res, netId);

    netId = VALID_NETID_START;
    OH_NetConn_GetAddrInfo(host, serv, hint, res, netId);
}

HWTEST_F(NetworkTest, OH_NetConn_FreeDnsResultTest001, TestSize.Level1)
{
    struct addrinfo *res = nullptr;
    auto ret = OH_NetConn_FreeDnsResult(res);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_GetAllNetsTest001, TestSize.Level1)
{
    auto ret = OH_NetConn_GetAllNets(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    NetConn_NetHandleList netHandleList;
    OH_NetConn_GetAllNets(&netHandleList);
}

HWTEST_F(NetworkTest, OH_NetConn_HasDefaultNetTest001, TestSize.Level1)
{
    auto ret = OH_NetConn_HasDefaultNet(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    int32_t hasDefaultNet;
    OH_NetConn_HasDefaultNet(&hasDefaultNet);
}

HWTEST_F(NetworkTest, OH_NetConn_GetDefaultNetTest001, TestSize.Level1)
{
    auto ret = OH_NetConn_GetDefaultNet(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    NetConn_NetHandle netHandle;
    OH_NetConn_GetDefaultNet(&netHandle);
}

HWTEST_F(NetworkTest, OH_NetConn_IsDefaultNetMeteredTest001, TestSize.Level1)
{
    auto ret = OH_NetConn_IsDefaultNetMetered(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    int32_t isMetered;
    OH_NetConn_IsDefaultNetMetered(&isMetered);
}

HWTEST_F(NetworkTest, OH_NetConn_GetConnectionPropertiesTest001, TestSize.Level1)
{
    auto ret = OH_NetConn_GetConnectionProperties(nullptr, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    NetConn_NetHandle netHandle; NetConn_ConnectionProperties prop;
    ret = OH_NetConn_GetConnectionProperties(&netHandle, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    OH_NetConn_GetConnectionProperties(&netHandle, &prop);
}

HWTEST_F(NetworkTest, OH_NetConn_GetNetCapabilitiesTest001, TestSize.Level1)
{
    auto ret = OH_NetConn_GetNetCapabilities(nullptr, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    NetConn_NetHandle netHandle;
    NetConn_NetCapabilities netAllCapabilities;
    ret = OH_NetConn_GetNetCapabilities(&netHandle, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    OH_NetConn_GetNetCapabilities(&netHandle, &netAllCapabilities);
}

HWTEST_F(NetworkTest, OH_NetConn_GetDefaultHttpProxyTest001, TestSize.Level1)
{
    auto ret = OH_NetConn_GetDefaultHttpProxy(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    NetConn_HttpProxy httpProxy;
    OH_NetConn_GetDefaultHttpProxy(&httpProxy);
}

HWTEST_F(NetworkTest, OHOS_NetConn_RegisterDnsResolverTest001, TestSize.Level1)
{
    OH_NetConn_CustomDnsResolver resolver = nullptr;
    auto ret = OHOS_NetConn_RegisterDnsResolver(resolver);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    resolver = [](const char *host, const char *serv,
        const struct addrinfo *hint, struct addrinfo **res) -> int {
            return NETMANAGER_ERR_PARAMETER_ERROR;
    };
    ret = OHOS_NetConn_RegisterDnsResolver(resolver);
    EXPECT_NE(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_RegisterDnsResolverTest001, TestSize.Level1)
{
    OH_NetConn_CustomDnsResolver resolver = nullptr;
    auto ret = OH_NetConn_RegisterDnsResolver(resolver);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    resolver = [](const char *host, const char *serv,
        const struct addrinfo *hint, struct addrinfo **res) -> int {
            return NETMANAGER_ERR_PARAMETER_ERROR;
    };
    ret = OH_NetConn_RegisterDnsResolver(resolver);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_BindSocketTest001, TestSize.Level1)
{
    int32_t socketFd = -1;
    NetConn_NetHandle netHandle;
    auto ret = OH_NetConn_BindSocket(socketFd, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_BindSocket(socketFd, &netHandle);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_BindSocketTest002, TestSize.Level1)
{
    int32_t socketFd = 1;
    NetConn_NetHandle netHandle = {1};
    auto ret = OH_NetConn_BindSocket(socketFd, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    netHandle.netId = VALID_NETID_START;
    ret = OH_NetConn_BindSocket(socketFd, &netHandle);
    EXPECT_NE(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_RegisterNetConnCallbackTest001, TestSize.Level1)
{
    NetConn_NetSpecifier specifier;
    NetConn_NetConnCallback netConnCallback;
    uint32_t timeout = 1;
    uint32_t callbackId = 1;
    auto ret = OH_NetConn_RegisterNetConnCallback(nullptr, nullptr, timeout, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_RegisterNetConnCallback(&specifier, nullptr, timeout, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_RegisterNetConnCallbackTest002, TestSize.Level1)
{
    NetConn_NetSpecifier specifier;
    NetConn_NetConnCallback netConnCallback;
    uint32_t timeout = 1;
    uint32_t callbackId = 1;
    auto ret = OH_NetConn_RegisterNetConnCallback(&specifier, &netConnCallback, timeout, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_RegisterNetConnCallback(&specifier, &netConnCallback, timeout, &callbackId);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_RegisterDefaultNetConnCallbackTest001, TestSize.Level1)
{
    NetConn_NetConnCallback netConnCallback;
    uint32_t callbackId = 1;
    auto ret = OH_NetConn_RegisterDefaultNetConnCallback(nullptr, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_RegisterDefaultNetConnCallback(&netConnCallback, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_RegisterDefaultNetConnCallback(&netConnCallback, &callbackId);
    EXPECT_NE(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_SetAppHttpProxyTest001, TestSize.Level1)
{
    NetConn_HttpProxy httpProxy;
    auto ret = OH_NetConn_SetAppHttpProxy(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_SetAppHttpProxy(&httpProxy);
    EXPECT_NE(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_RegisterAppHttpProxyCallbackTest001, TestSize.Level1)
{
    OH_NetConn_AppHttpProxyChange appHttpProxyChange = nullptr;
    uint32_t callbackId = 1;
    auto ret = OH_NetConn_RegisterAppHttpProxyCallback(appHttpProxyChange, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    appHttpProxyChange = [](NetConn_HttpProxy *proxy) {};
    ret = OH_NetConn_RegisterAppHttpProxyCallback(appHttpProxyChange, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_RegisterAppHttpProxyCallback(appHttpProxyChange, &callbackId);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkTest, OH_NetConn_SetPacUrlTest001, TestSize.Level1)
{
    const char *pacUrl = "test";
    auto ret = OH_NetConn_SetPacUrl(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_SetPacUrl(pacUrl);
    EXPECT_NE(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_GetPacUrlTest001, TestSize.Level1)
{
    char pacUrl[PAC_URL_MAX_LEN] = {0};
    auto ret = OH_NetConn_GetPacUrl(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    ret = OH_NetConn_GetPacUrl(pacUrl);
    EXPECT_NE(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryProbeResult001, TestSize.Level1)
{
    struct NetConn_ProbeResultInfo result;
    auto ret = OH_NetConn_QueryProbeResult("www.baidu.com", 10, &result);
    EXPECT_EQ(ret, 0);

    ret = OH_NetConn_QueryProbeResult(nullptr, 10, &result);
    EXPECT_NE(ret, 0);

    ret = OH_NetConn_QueryProbeResult("www.baidu.com", 0, &result);
    EXPECT_NE(ret, 0);

    ret = OH_NetConn_QueryProbeResult("www.baidu.com", -1, &result);
    EXPECT_NE(ret, 0);

    ret = OH_NetConn_QueryProbeResult("www.baidu.com", 1001, &result);
    EXPECT_NE(ret, 0);

    ret = OH_NetConn_QueryProbeResult("8.8.8.8", 10, &result);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetworkTest, OH_NetConn_GetAddrInfoTest003, TestSize.Level1)
{
    char host[] = "192.168.1.1";
    char *serv = nullptr;
    struct addrinfo info;
    struct addrinfo *hint = &info;
    struct addrinfo **res = &hint;
    int32_t netId = -1;
    auto ret = OH_NetConn_GetAddrInfo(host, serv, hint, res, netId);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest001, TestSize.Level1)
{
    const char *destination = "www.example.com";
    NetConn_TraceRouteInfo traceRouteInfo[1] = {};
    auto ret = OH_NetConn_QueryTraceRoute(destination, nullptr, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    ret = OH_NetConn_QueryTraceRoute(nullptr, nullptr, traceRouteInfo);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest002, TestSize.Level1)
{
    std::string traceRouteInfoStr = "1 192.168.1.1 50 2 192.168.1.2 100 3 192.168.1.3 150";
    NetConn_TraceRouteInfo traceRouteInfo[3];
    int32_t maxJumpNumber = 3;

    EXPECT_EQ(Conv2TraceRouteInfo(traceRouteInfoStr, traceRouteInfo, maxJumpNumber), NETMANAGER_SUCCESS);
    EXPECT_EQ(traceRouteInfo[0].jumpNo, 1);
    EXPECT_STREQ(traceRouteInfo[0].address, "192.168.1.1");
    EXPECT_EQ(traceRouteInfo[0].rtt[0], 50);
    EXPECT_EQ(traceRouteInfo[1].jumpNo, 2);
    EXPECT_STREQ(traceRouteInfo[1].address, "192.168.1.2");
    EXPECT_EQ(traceRouteInfo[1].rtt[0], 100);
    EXPECT_EQ(traceRouteInfo[2].jumpNo, 3);
    EXPECT_STREQ(traceRouteInfo[2].address, "192.168.1.3");
    EXPECT_EQ(traceRouteInfo[2].rtt[0], 150);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest003, TestSize.Level1)
{
    std::string traceRouteInfoStr = "1 192.168.1.1 50 2 192.168.1.2 100 3 192.168.1.3 150";
    NetConn_TraceRouteInfo traceRouteInfo[2];
    int32_t maxJumpNumber = 2;

    EXPECT_EQ(Conv2TraceRouteInfo(traceRouteInfoStr, traceRouteInfo, maxJumpNumber), NETMANAGER_SUCCESS);
    EXPECT_EQ(traceRouteInfo[0].jumpNo, 1);
    EXPECT_STREQ(traceRouteInfo[0].address, "192.168.1.1");
    EXPECT_EQ(traceRouteInfo[0].rtt[0], 50);
    EXPECT_EQ(traceRouteInfo[1].jumpNo, 2);
    EXPECT_STREQ(traceRouteInfo[1].address, "192.168.1.2");
    EXPECT_EQ(traceRouteInfo[1].rtt[0], 100);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest004, TestSize.Level1)
{
    std::string traceRouteInfoStr = "1 192.168.1.1 50 2 192.168.1.2 100 3 192.168.1.3 150";
    int32_t maxJumpNumber = 3;

    EXPECT_EQ(Conv2TraceRouteInfo(traceRouteInfoStr, nullptr, maxJumpNumber), NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest005, TestSize.Level1)
{
    std::string traceRouteInfoStr = "1 192.168.1.1 50 2 192.168.1.2 invalid 3 192.168.1.3 150";
    NetConn_TraceRouteInfo traceRouteInfo[3];
    int32_t maxJumpNumber = 3;

    EXPECT_EQ(Conv2TraceRouteInfo(traceRouteInfoStr, traceRouteInfo, maxJumpNumber), NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest006, TestSize.Level1)
{
    std::string rttStr = "100;200;300";
    uint32_t rtt[NETCONN_MAX_RTT_NUM] = {0};

    int32_t result = Conv2TraceRouteInfoRtt(rttStr, &rtt);

    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    EXPECT_EQ(rtt[0], 100);
    EXPECT_EQ(rtt[1], 200);
    EXPECT_EQ(rtt[2], 300);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest007, TestSize.Level1)
{
    std::string rttStr;
    for (int i = 0; i < NETCONN_MAX_RTT_NUM; ++i) {
        rttStr += std::to_string(i) + ";";
    }
    uint32_t rtt[NETCONN_MAX_RTT_NUM] = {0};

    int32_t result = Conv2TraceRouteInfoRtt(rttStr, &rtt);

    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    EXPECT_EQ(rtt[0], 0);
    EXPECT_EQ(rtt[1], 1);
    EXPECT_EQ(rtt[2], 2);
    EXPECT_EQ(rtt[3], 3);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest008, TestSize.Level1)
{
    std::string rttStr = "100;abc;300";
    uint32_t rtt[NETCONN_MAX_RTT_NUM] = {0};

    int32_t result = Conv2TraceRouteInfoRtt(rttStr, &rtt);

    EXPECT_EQ(result, NETMANAGER_SUCCESS);
    EXPECT_EQ(rtt[0], 100);
    EXPECT_EQ(rtt[1], 0);
    EXPECT_EQ(rtt[2], 300);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest09, TestSize.Level1)
{
    const std::string traceRouteInfoStr = "1 192.168.2.1 788;889;998;110 2 10.111.120.189 1334;1445;1667;1678";
    NetConn_TraceRouteInfo traceRouteInfo[2] = {};
    auto ret = Conv2TraceRouteInfo(traceRouteInfoStr, traceRouteInfo, 2);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(1678, traceRouteInfo[1].rtt[3]);
}

HWTEST_F(NetworkTest, OH_NetConn_QueryTraceRouteTest10, TestSize.Level1)
{
    const char *destination = "www.text.com";
    OHOS::NetManagerStandard::NetConn_TraceRouteInfo traceRouteInfo[30] = {};
    OHOS::NetManagerStandard::NetConn_TraceRouteOption Option = {30, NETCONN_PACKETS_ICMP};
    OH_NetConn_QueryTraceRoute(destination, &Option, traceRouteInfo);
    Option = {31, NETCONN_PACKETS_ICMP};
    auto ret = OH_NetConn_QueryTraceRoute(destination, &Option, traceRouteInfo);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_BindSocketTest003, TestSize.Level1)
{
    int32_t socketFd = 1;
    NetConn_NetHandle netHandle = {1};
    auto ret = OH_NetConn_BindSocket(socketFd, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);

    netHandle.netId = 100;
    ret = OH_NetConn_BindSocket(socketFd, &netHandle);
    EXPECT_NE(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, PAC_OH_NetConn_SetPacFileUrl_001, TestSize.Level1)
{
    int32_t ret = OH_NetConn_SetPacFileUrl(nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    char url[204];
    ret = OH_NetConn_GetPacFileUrl(url);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(std::string(url), "");
    ret = OH_NetConn_SetPacFileUrl("testurl");
    EXPECT_EQ(ret, NETMANAGER_ERR_PERMISSION_DENIED);
    ret = OH_NetConn_GetPacFileUrl(url);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkTest, PAC_OH_NetConn_FindProxyForURL_001, TestSize.Level1)
{
    int32_t ret = OH_NetConn_FindProxyForURL(nullptr, nullptr, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    char empty[] = "";
    ret = OH_NetConn_FindProxyForURL(nullptr, nullptr, empty);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    ret = OH_NetConn_FindProxyForURL(nullptr, empty, empty);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
    ret = OH_NetConn_FindProxyForURL(empty, empty, empty);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    char url[] = "example.com";
    char host[] = "example.com";
    char proxy[1024];
    ret = OH_NetConn_FindProxyForURL(url, host, proxy);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkTest, PAC_OH_NetConn_SetProxyMode_001, TestSize.Level1)
{
    int32_t ret = OH_NetConn_SetProxyMode(PROXY_MODE_OFF);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ProxyModeType mode;
    ret  = OH_NetConn_GetProxyMode(&mode);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(mode, PROXY_MODE_OFF);
    ret = OH_NetConn_SetProxyMode(PROXY_MODE_AUTO);
#ifdef NETMANAGER_ENABLE_PAC_PROXY
    EXPECT_EQ(ret != NETMANAGER_SUCCESS, true);
#else
    EXPECT_EQ(ret == NETMANAGER_SUCCESS, true);
#endif
    ret  = OH_NetConn_GetProxyMode(&mode);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    EXPECT_EQ(mode, PROXY_MODE_AUTO);
}

HWTEST_F(NetworkTest, OnHandleNetProbeResultTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, nullptr);
    network->OnHandleDualStackProbeResult(DualStackProbeResultCode::PROBE_FAIL);
    EXPECT_EQ(network->eventHandler_, nullptr);
}

HWTEST_F(NetworkTest, OnHandleNetProbeResultTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto eventRunner = AppExecFwk::EventRunner::Create("TEST_THREAD");
    auto eventHandle = std::make_shared<NetConnEventHandler>(eventRunner);
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, eventHandle);
    network->OnHandleDualStackProbeResult(DualStackProbeResultCode::PROBE_FAIL);
    EXPECT_NE(network->eventHandler_, nullptr);
}

HWTEST_F(NetworkTest, StartDualStackProbeThreadTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, nullptr);
    network->StartDualStackProbeThread();
    EXPECT_EQ(network->netMonitor_, nullptr);
    network->InitNetMonitor();
    network->StartDualStackProbeThread();
    EXPECT_NE(network->netMonitor_, nullptr);
}

HWTEST_F(NetworkTest, RegisterNetProbeCallbackTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, nullptr);
    std::shared_ptr<IDualStackProbeCallback> probeCb = nullptr;
    auto ret = network->RegisterDualStackProbeCallback(probeCb);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
    probeCb = std::make_shared<NetProbeCallbackTest>();
    ret = network->RegisterDualStackProbeCallback(probeCb);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = network->RegisterDualStackProbeCallback(probeCb);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkTest, UnRegisterNetProbeCallbackTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, nullptr);
    std::shared_ptr<IDualStackProbeCallback> probeCb = nullptr;
    auto ret = network->UnRegisterDualStackProbeCallback(probeCb);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
    probeCb = std::make_shared<NetProbeCallbackTest>();
    ret = network->UnRegisterDualStackProbeCallback(probeCb);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
    ret = network->RegisterDualStackProbeCallback(probeCb);
    ret = network->UnRegisterDualStackProbeCallback(probeCb);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkTest, HandleNetProbeResultTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, nullptr);
    network->HandleNetProbeResult(DualStackProbeResultCode::PROBE_FAIL);
    EXPECT_EQ(network->dualStackProbeCallback_.size(), 0);
    std::shared_ptr<IDualStackProbeCallback> probeCb = std::make_shared<NetProbeCallbackTest>();
    network->RegisterDualStackProbeCallback(probeCb);
    network->HandleNetProbeResult(DualStackProbeResultCode::PROBE_FAIL);
    EXPECT_NE(network->dualStackProbeCallback_.size(), 0);
}

HWTEST_F(NetworkTest, UpdateNetProbeTimeTest001, TestSize.Level1)
{
    int32_t netId = 1;
    int32_t probeTime = 5 * 1000;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_WIFI, nullptr);
    network->UpdateDualStackProbeTime(probeTime);
    EXPECT_EQ(network->netMonitor_, nullptr);
    
    network->InitNetMonitor();
    network->UpdateDualStackProbeTime(probeTime);
    EXPECT_NE(network->netMonitor_, nullptr);
}

HWTEST_F(NetworkTest, UpdateDnsTest003, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetLinkInfo netLinkInfo;
    EXPECT_TRUE(netLinkInfo.dnsList_.empty());
    network->UpdateDns(netLinkInfo);
    NetManagerStandard::INetAddr dns;
    dns.address_ = "";
    dns.type_ = NetManagerStandard::INetAddr::IPV4;
    netLinkInfo.dnsList_.push_back(dns);
    EXPECT_TRUE(!netLinkInfo.dnsList_.empty());
    network->UpdateDns(netLinkInfo);
}

HWTEST_F(NetworkTest, ReleaseBasicNetwork001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    Route route1;
    Route route2;
    route1.destination_.type_ = INetAddr::IpType::IPV4;
    route1.iface_ = "rmnet0";
    route1.destination_.address_ = "0.0.0.0";
    route1.destination_.prefixlen_ = 24;
    route1.gateway_.address_ = "0.0.0.0";
    route2.destination_.type_ = INetAddr::IpType::IPV6;
    route2.iface_ = "rmnet0";
    std::string mockAddress = "";
    int32_t mockLength = 5000;
    for (uint32_t i = 0; i < mockLength; i++) {
        mockAddress += "1";
    }
    route2.destination_.address_ = mockAddress;
    route2.destination_.prefixlen_ = 64;
    route2.gateway_.address_ = "fe80::1";
    network->netLinkInfo_.routeList_.push_back(route1);
    network->netLinkInfo_.routeList_.push_back(route2);
    network->isPhyNetCreated_ = true;
    network->isNeedResume_ = true;
    network->ReleaseBasicNetwork();
    network->RemoveRouteByFamily(INetAddr::IpType::IPV6);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 0);
}

HWTEST_F(NetworkTest, ReleaseBasicNetwork002, TestSize.Level1)
{
    auto network1 = std::make_shared<Network>(1, 2000, NetBearType::BEARER_VPN, nullptr);
    network1->isPhyNetCreated_ = true;
    network1->isNeedResume_ = false;
    network1->netLinkInfo_.ifaceName_ = "test";
    auto network2 = std::make_shared<Network>(2, 2000, NetBearType::BEARER_VPN, nullptr);
    network2->netLinkInfo_.ifaceName_ = "test";
    std::set<NetCap> netCaps;
    NetConnService::GetInstance()->netSuppliers_[2000] =
        sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_VPN, "", netCaps);
    NetConnService::GetInstance()->netSuppliers_[2000]->SetNetwork(network2);
    NetConnService::GetInstance()->netSuppliers_[2000]->netSupplierInfo_.isAvailable_ = true;
    network1->isInternalDefault_ = true;
    EXPECT_TRUE(network1->IsIfaceNameInUse());
    network1->ReleaseBasicNetwork();
    NetConnService::GetInstance()->netSuppliers_.erase(2000);
}

HWTEST_F(NetworkTest, ReleaseBasicNetwork003, TestSize.Level1)
{
    auto network1 = std::make_shared<Network>(1, 2000, NetBearType::BEARER_VPN, nullptr);
    network1->isPhyNetCreated_ = true;
    network1->isNeedResume_ = false;
    network1->netLinkInfo_.ifaceName_ = "test";
    auto network2 = std::make_shared<Network>(2, 2000, NetBearType::BEARER_VPN, nullptr);
    network2->netLinkInfo_.ifaceName_ = "test";
    std::set<NetCap> netCaps;
    NetConnService::GetInstance()->netSuppliers_[2000] =
        sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_VPN, "", netCaps);
    NetConnService::GetInstance()->netSuppliers_[2000]->SetNetwork(network2);
    NetConnService::GetInstance()->netSuppliers_[2000]->netSupplierInfo_.isAvailable_ = true;
    network1->isInternalDefault_ = false;
    EXPECT_TRUE(network1->IsIfaceNameInUse());
    network1->ReleaseBasicNetwork();
    NetConnService::GetInstance()->netSuppliers_.erase(2000);
}

HWTEST_F(NetworkTest, ReleaseBasicNetwork004, TestSize.Level1)
{
    auto network1 = std::make_shared<Network>(1, 2000, NetBearType::BEARER_CELLULAR, nullptr);
    network1->isPhyNetCreated_ = true;
    network1->isNeedResume_ = false;
    network1->netLinkInfo_.ifaceName_ = "test";
    auto network2 = std::make_shared<Network>(2, 2000, NetBearType::BEARER_CELLULAR, nullptr);
    network2->netLinkInfo_.ifaceName_ = "test";
    std::set<NetCap> netCaps;
    NetConnService::GetInstance()->netSuppliers_[2000] =
        sptr<NetSupplier>::MakeSptr(NetBearType::BEARER_CELLULAR, "", netCaps);
    NetConnService::GetInstance()->netSuppliers_[2000]->SetNetwork(network2);
    NetConnService::GetInstance()->netSuppliers_[2000]->netSupplierInfo_.isAvailable_ = true;
    network1->isInternalDefault_ = false;
    EXPECT_TRUE(network1->IsIfaceNameInUse());
    network1->ReleaseBasicNetwork();
    NetConnService::GetInstance()->netSuppliers_.erase(2000);
}

HWTEST_F(NetworkTest, ResumeNetworkInfo001, TestSize.Level1)
{
    auto network = std::make_shared<Network>(1, 2000, NetBearType::BEARER_VPN, nullptr);
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_INTERNET);
    network->SetNetCaps(netCaps);
    network->ResumeNetworkInfo();
    EXPECT_TRUE(network->isSupportInternet_);
    EXPECT_FALSE(network->isInternalDefault_);
    EXPECT_TRUE(network->isNeedResume_);
}
 
HWTEST_F(NetworkTest, UpdateRoutesTest012, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetLinkInfo newNetLinkInfo;
    for (int i = 0; i < BATCH_ROUTE_THRESHOLD + 1; i++) {
        Route route;
        route.iface_ = "test" + std::to_string(i);
        route.destination_.address_ = "192.168." + std::to_string(i/256) + "." + std::to_string(i%256);
        route.destination_.prefixlen_ = 24;
        route.gateway_.address_ = "192.168." + std::to_string(i/256) + ".1";
        newNetLinkInfo.routeList_.push_back(route);
    }
    network->UpdateRoutes(newNetLinkInfo);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 0);
}

HWTEST_F(NetworkTest, BatchUpdateRoutesTest001, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetLinkInfo netLinkInfoBck;
    NetLinkInfo newNetLinkInfo;
    for (int i = 0; i < 5; i++) {
        Route route;
        route.iface_ = "eth" + std::to_string(i);
        route.destination_.address_ = "192.168." + std::to_string(i) + ".0";
        route.destination_.prefixlen_ = 24;
        route.gateway_.address_ = "192.168." + std::to_string(i) + ".1";
        route.isExcludedRoute_ = (i % 2 == 0);
        newNetLinkInfo.routeList_.push_back(route);
    }
    network->BatchUpdateRoutes(netLinkInfoBck, newNetLinkInfo);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 0);
}

HWTEST_F(NetworkTest, BatchUpdateRoutesTest002, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_VPN, nullptr);
    EXPECT_NE(network, nullptr);
    NetLinkInfo netLinkInfoBck;
    NetLinkInfo newNetLinkInfo;
    for (int i = 0; i < 3; i++) {
        Route route;
        route.iface_ = "tun0";
        route.destination_.address_ = "10.0." + std::to_string(i) + ".0";
        route.destination_.prefixlen_ = 24;
        route.gateway_.address_ = "10.0." + std::to_string(i) + ".1";
        newNetLinkInfo.routeList_.push_back(route);
    }
    network->BatchUpdateRoutes(netLinkInfoBck, newNetLinkInfo);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 0);
}

HWTEST_F(NetworkTest, BatchUpdateRoutesTest003, TestSize.Level1)
{
    int32_t netId = 1;
    auto network = std::make_shared<Network>(netId, netId, NetBearType::BEARER_ETHERNET, nullptr);
    EXPECT_NE(network, nullptr);
    NetLinkInfo netLinkInfoBck;
    NetLinkInfo newNetLinkInfo;
    Route route;
    route.iface_ = "eth0";
    route.destination_.address_ = "192.168.1.0";
    route.destination_.prefixlen_ = 24;
    route.gateway_.address_ = "192.168.1.1";
    netLinkInfoBck.routeList_.push_back(route);
    newNetLinkInfo.routeList_.push_back(route);
    network->BatchUpdateRoutes(netLinkInfoBck, newNetLinkInfo);
    EXPECT_EQ(network->netLinkInfo_.routeList_.size(), 0);
}

HWTEST_F(NetworkTest, OH_NetConn_RefreshGlobalHttpProxyWithCallbackTest001, TestSize.Level1)
{
    OH_NetConn_GlobalHttpProxyRefreshCallback callback = nullptr;
    auto ret = OH_NetConn_RefreshGlobalHttpProxyWithCallback(callback, nullptr);
    EXPECT_EQ(ret, NETMANAGER_ERR_PARAMETER_ERROR);
}

HWTEST_F(NetworkTest, OH_NetConn_RefreshGlobalHttpProxyWithCallbackTest002, TestSize.Level1)
{
    OH_NetConn_GlobalHttpProxyRefreshCallback callback =
        [](int32_t result, const NetConn_HttpProxy *proxy, void *userContext) {};
    auto ret = OH_NetConn_RefreshGlobalHttpProxyWithCallback(callback, nullptr);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetworkTest, InvokeRefreshCallbackNullCallback, TestSize.Level1)
{
    HttpProxy httpProxy;
    InvokeRefreshCallback(nullptr, NETMANAGER_SUCCESS, httpProxy, nullptr);
    EXPECT_TRUE(httpProxy.GetHost().empty());
}

HWTEST_F(NetworkTest, InvokeRefreshCallbackAuthFailed, TestSize.Level1)
{
    static std::atomic<bool> receivedNullptr{false};
    receivedNullptr = false;
    OH_NetConn_GlobalHttpProxyRefreshCallback callback =
        [](int32_t result, const NetConn_HttpProxy *proxy, void *userContext) {
            if (proxy == nullptr) {
                receivedNullptr = true;
            }
        };
    HttpProxy httpProxy;
    InvokeRefreshCallback(callback, NETMANAGER_ERR_INTERNAL, httpProxy, nullptr);
    EXPECT_TRUE(receivedNullptr.load());
}

HWTEST_F(NetworkTest, InvokeRefreshCallbackEmptyHost, TestSize.Level1)
{
    static std::atomic<bool> receivedNullptr{false};
    receivedNullptr = false;
    OH_NetConn_GlobalHttpProxyRefreshCallback callback =
        [](int32_t result, const NetConn_HttpProxy *proxy, void *userContext) {
            if (proxy == nullptr) {
                receivedNullptr = true;
            }
        };
    HttpProxy httpProxy;
    InvokeRefreshCallback(callback, NETMANAGER_SUCCESS, httpProxy, nullptr);
    EXPECT_TRUE(receivedNullptr.load());
}

HWTEST_F(NetworkTest, InvokeRefreshCallbackSuccess, TestSize.Level1)
{
    static std::atomic<bool> receivedProxy{false};
    receivedProxy = false;
    OH_NetConn_GlobalHttpProxyRefreshCallback callback =
        [](int32_t result, const NetConn_HttpProxy *proxy, void *userContext) {
            if (proxy != nullptr) {
                receivedProxy = true;
            }
        };
    HttpProxy httpProxy;
    httpProxy.SetHost("127.0.0.1");
    httpProxy.SetPort(8080);
    InvokeRefreshCallback(callback, NETMANAGER_SUCCESS, httpProxy, nullptr);
    EXPECT_TRUE(receivedProxy.load());
}
} // namespace NetManagerStandard
} // namespace OHOS
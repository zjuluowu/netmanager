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

#ifdef GTEST_API_
#define private public
#define protected public
#endif

#include <arpa/inet.h>
#include <list>
#include "clatd.h"
#include "clat_utils.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace nmd {
namespace {
using namespace testing::ext;
using namespace OHOS::NetManagerStandard;
constexpr int V4ADDR_BIT_LEN = 32;
}

bool IsIpv4AddressFree(const in_addr_t v4Addr);
in_addr_t GetAvailableIpv4Address(const in_addr initV4Addr, const int16_t prefixLen);
int32_t GetSuitableIpv6Address(const std::string &v6IfaceStr, const in_addr v4Addr,
    const in6_addr &nat64Prefix, in6_addr &v6Addr, const uint32_t mark);

class ClatdTest : public testing::Test {
public:
    static void SetUpTestCase() {}

    static void TearDownTestCase() {}

    void SetUp() {}

    void TearDown() {}
};

HWTEST_F(ClatdTest, IsIpv4AddressFreeTest001, TestSize.Level1)
{
    std::string v4AddrStr;
    FreeTunV4Addr(v4AddrStr);

    v4AddrStr = "192.168.1.1";
    in_addr v4Addr;
    inet_pton(AF_INET, v4AddrStr.c_str(), &v4Addr);
    auto ret = IsIpv4AddressFree(v4Addr.s_addr);

    ret = IsIpv4AddressFree(v4Addr.s_addr);
    EXPECT_FALSE(ret);
    FreeTunV4Addr(v4AddrStr);
}

HWTEST_F(ClatdTest, IsIpv4AddressFreeTest002, TestSize.Level1)
{
    std::string v4AddrStr;
    FreeTunV4Addr(v4AddrStr);

    v4AddrStr = "192.168.1.2";
    in_addr v4Addr;
    inet_pton(AF_INET, v4AddrStr.c_str(), &v4Addr);
    auto ret = IsIpv4AddressFree(v4Addr.s_addr);
    EXPECT_TRUE(ret);

    ret = IsIpv4AddressFree(v4Addr.s_addr);
    EXPECT_FALSE(ret);
    FreeTunV4Addr(v4AddrStr);
}

HWTEST_F(ClatdTest, GetAvailableIpv4AddressTest001, TestSize.Level1)
{
    in_addr initV4Addr;
    int16_t prefixLen = -1;
    auto ret = GetAvailableIpv4Address(initV4Addr, prefixLen);
    EXPECT_EQ(ret, INADDR_NONE);

    prefixLen = V4ADDR_BIT_LEN + 1;
    ret = GetAvailableIpv4Address(initV4Addr, prefixLen);
    EXPECT_EQ(ret, INADDR_NONE);

    initV4Addr.s_addr = 0;
    prefixLen = 0;
    ret = GetAvailableIpv4Address(initV4Addr, prefixLen);
    EXPECT_EQ(ret, 0);
}

HWTEST_F(ClatdTest, SelectIpv4AddressTest001, TestSize.Level1)
{
    std::string initV4AddrStr;
    int prefixLen = -1;
    std::string v4AddrStr;
    auto ret = SelectIpv4Address(initV4AddrStr, prefixLen, v4AddrStr);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    initV4AddrStr = "192.168.1.1";
    ret = SelectIpv4Address(initV4AddrStr, prefixLen, v4AddrStr);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(ClatdTest, GetSuitableIpv6AddressTest001, TestSize.Level1)
{
    std::string v6IfaceStr = "eth0";
    in_addr v4Addr;
    in6_addr nat64Prefix, v6Addr;
    uint32_t mark = 100;
    inet_pton(AF_INET, "192.0.2.1", &v4Addr);
    inet_pton(AF_INET6, "64:ff9b::", &nat64Prefix);
    auto ret = GetSuitableIpv6Address(v6IfaceStr, v4Addr, nat64Prefix, v6Addr, mark);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(ClatdTest, GetSuitableIpv6AddressTest002, TestSize.Level1)
{
    std::string v6IfaceStr = "lo";
    in_addr v4Addr;
    in6_addr nat64Prefix, v6Addr;
    uint32_t mark = 0;
    v4Addr.s_addr = 0;
    inet_pton(AF_INET6, "64:ff9b::", &nat64Prefix);
    auto ret = GetSuitableIpv6Address(v6IfaceStr, v4Addr, nat64Prefix, v6Addr, mark);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(ClatdTest, GenerateIpv6AddressTest001, TestSize.Level1)
{
    std::string v6IfaceStr;
    std::string v4AddrStr;
    std::string prefix64Str;
    uint32_t mark = 1;
    std::string v6AddrStr;
    auto ret = GenerateIpv6Address(v6IfaceStr, v4AddrStr, prefix64Str, mark, v6AddrStr);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    v6IfaceStr = "eth0";
    ret = GenerateIpv6Address(v6IfaceStr, v4AddrStr, prefix64Str, mark, v6AddrStr);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    v4AddrStr = "192.168.1.1";
    ret = GenerateIpv6Address(v6IfaceStr, v4AddrStr, prefix64Str, mark, v6AddrStr);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    prefix64Str = "2001:db8::";
    ret = GenerateIpv6Address(v6IfaceStr, v4AddrStr, prefix64Str, mark, v6AddrStr);
    EXPECT_NE(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdTest, CreateTunInterfaceTest001, TestSize.Level1)
{
    std::string tunIface = "eth0";
    int fd = -1;
    auto ret = CreateTunInterface(tunIface, fd);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);

    tunIface = "9999999999999999999";
    ret = CreateTunInterface(tunIface, fd);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);
}

HWTEST_F(ClatdTest, OpenPacketSocketTest001, TestSize.Level1)
{
    int readSock6 = -1;
    auto ret = OpenPacketSocket(readSock6);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdTest, OpenRawSocket6Test001, TestSize.Level1)
{
    int writeSock6 = -1;
    auto ret = OpenRawSocket6(0, writeSock6);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdTest, ConfigureWriteSocketTest001, TestSize.Level1)
{
    int sockFd = -1;
    std::string v6Iface;
    auto ret = ConfigureWriteSocket(sockFd, v6Iface);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    sockFd = 0;
    ret = ConfigureWriteSocket(sockFd, v6Iface);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);

    OpenRawSocket6(0, sockFd);
    ret = ConfigureWriteSocket(sockFd, v6Iface);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdTest, ConfigureReadSocketTest001, TestSize.Level1)
{
    int sockFd = -1;
    std::string addrStr;
    int ifIndex = 0;
    auto ret = ConfigureReadSocket(sockFd, addrStr, ifIndex);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    sockFd = 0;
    ret = ConfigureReadSocket(sockFd, addrStr, ifIndex);
    EXPECT_EQ(ret, NETMANAGER_ERR_INVALID_PARAMETER);

    addrStr = "2001:db8:85a3::8a2e:370:7334";
    ret = ConfigureReadSocket(sockFd, addrStr, ifIndex);
    EXPECT_EQ(ret, NETMANAGER_ERR_OPERATION_FAILED);

    OpenPacketSocket(sockFd);
    ret = ConfigureReadSocket(sockFd, addrStr, ifIndex);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdTest, SetTunInterfaceAddressTest001, TestSize.Level1)
{
    std::string ifName = "eth0";
    std::string tunAddr;
    int32_t prefix = 0;
    auto ret = SetTunInterfaceAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    tunAddr = "9999999999999999999";
    ret = SetTunInterfaceAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    tunAddr = "192.168.1.1";
    ret = SetTunInterfaceAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

HWTEST_F(ClatdTest, SetTunInterfaceAddressTest002, TestSize.Level1)
{
    std::string ifName = "eth0";
    std::string tunAddr = "192.168.1.1";
    int32_t prefix = V4ADDR_BIT_LEN + 1;
    auto ret = SetTunInterfaceAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(ret, NETMANAGER_ERROR);

    prefix = V4ADDR_BIT_LEN;
    ret = SetTunInterfaceAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(ClatdTest, SetTunInterfaceAddressTest003, TestSize.Level1)
{
    std::string ifName = "";
    std::string tunAddr = "192.168.1.1";
    int32_t prefix = V4ADDR_BIT_LEN;
    auto ret = SetTunInterfaceAddress(ifName, tunAddr, prefix);
    EXPECT_EQ(ret, NETMANAGER_ERROR);
}

} // namespace nmd
} // namespace OHOS
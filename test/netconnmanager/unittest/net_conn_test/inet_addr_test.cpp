/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "inet_addr.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr uint8_t FAMILY = 0x00;
constexpr uint8_t PRE_FIX_LEN = 0;
const std::string ADDRESS = "127.0.0.1";
const std::string NET_MASK = "0x80000000";
const std::string HOST_NAME = "127.0.0.1";
constexpr uint8_t PORT = 0;
INetAddr GetINetAddrData()
{
    typedef enum {
        UNKNOWN = 0x00,
        IPV4 = 0x01,
        IPV6 = 0x02,
    } IpType;

    INetAddr info;
    info.type_ = IpType::UNKNOWN;
    info.family_ = FAMILY;
    info.prefixlen_ = PRE_FIX_LEN;
    info.address_ = ADDRESS;
    info.netMask_ = NET_MASK;
    info.hostName_ = HOST_NAME;
    info.port_ = PORT;
    return info;
}
} // namespace

class INetAddrTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    void SetUp();

    void TearDown();
};

void INetAddrTest::SetUpTestCase() {}

void INetAddrTest::TearDownTestCase() {}

void INetAddrTest::SetUp() {}

void INetAddrTest::TearDown() {}

HWTEST_F(INetAddrTest, INetAddrTest001, TestSize.Level1)
{
    Parcel parcel;
    INetAddr addr = GetINetAddrData();
    EXPECT_TRUE(addr.Marshalling(parcel));

    INetAddr result;
    sptr<INetAddr> iNetAddr = INetAddr::Unmarshalling(parcel);
    EXPECT_NE(iNetAddr, nullptr);
}
} // namespace NetManagerStandard
} // namespace OHOS
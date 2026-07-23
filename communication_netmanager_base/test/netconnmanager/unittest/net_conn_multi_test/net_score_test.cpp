/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#endif

#include "net_conn_types.h"
#include "net_mgr_log_wrapper.h"
#include "net_supplier.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class NetScoreTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NetScoreTest::SetUpTestCase() {}

void NetScoreTest::TearDownTestCase() {}

void NetScoreTest::SetUp() {}

void NetScoreTest::TearDown() {}

HWTEST_F(NetScoreTest, GetServiceScore, TestSize.Level1)
{
    std::set<NetCap> netCaps {NET_CAPABILITY_MMS, NET_CAPABILITY_INTERNET};
    std::string ident = "ident";
    NetBearType bearerType = BEARER_CELLULAR;
    sptr<NetSupplier> supplier = (std::make_unique<NetSupplier>(bearerType, ident, netCaps)).release();
    supplier->SetDetectionDone();

    // mock Failed to detect network
    supplier->SetNetValid(INVALID_DETECTION_STATE);

    ASSERT_TRUE(supplier->GetNetScore() == static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE));
    ASSERT_TRUE(supplier->GetRealScore() ==
        (static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE) - NET_VALID_SCORE));

    supplier->SetNetValid(CAPTIVE_PORTAL_STATE);

    ASSERT_TRUE(supplier->GetNetScore() == static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE));
    ASSERT_TRUE(supplier->GetRealScore() ==
        (static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE) - NET_VALID_SCORE));

    // mock successed to detect network
    supplier->SetNetValid(VERIFICATION_STATE);

    ASSERT_TRUE(supplier->GetNetScore() == static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE));
    ASSERT_TRUE(supplier->GetRealScore() == static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE));

    // quality_poor
    supplier->SetNetValid(QUALITY_POOR_STATE);
    ASSERT_TRUE(supplier->GetNetScore() == static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE));
    ASSERT_TRUE(supplier->GetRealScore() ==
        static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE) - DIFF_SCORE_BETWEEN_GOOD_POOR);
    // quality_good
    supplier->SetNetValid(QUALITY_GOOD_STATE);
    ASSERT_TRUE(supplier->GetNetScore() == static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE));
    ASSERT_TRUE(supplier->GetRealScore() ==
        static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE));
}

HWTEST_F(NetScoreTest, NetSupplierBranchTest, TestSize.Level1)
{
    std::set<NetCap> netCaps{NET_CAPABILITY_MMS, NET_CAPABILITY_INTERNET};
    std::string ident = "ident";
    NetBearType bearerType = BEARER_CELLULAR;
    sptr<NetSupplier> supplier = (std::make_unique<NetSupplier>(bearerType, ident, netCaps)).release();

    HttpProxy httpProxy;
    supplier->ClearDefault();
    supplier->UpdateGlobalHttpProxy(httpProxy);
    uint32_t reqId = 0;
    supplier->RemoveBestRequest(reqId);
    supplier->IsConnecting();

    NetLinkInfo netLinkInfo = {};
    supplier->UpdateNetLinkInfo(netLinkInfo);
    supplier->GetHttpProxy(httpProxy);
    supplier->ClearDefault();
    supplier->UpdateGlobalHttpProxy(httpProxy);

    std::set<NetCap> caps;
    uint32_t uid = 0;
    bool ret = supplier->CompareNetCaps(caps);
    ASSERT_TRUE(ret);

    ret = supplier->HasNetCaps(caps);
    ASSERT_TRUE(ret);

    ret = supplier->IsConnecting();
    ASSERT_FALSE(ret);

    supplier->netController_ = nullptr;
    supplier->netSupplierInfo_.isAvailable_ = true;
    ret = supplier->SupplierDisconnection(caps, uid);
    ASSERT_FALSE(ret);

    int32_t result = supplier->GetNetId();
    EXPECT_EQ(result, INVALID_NET_ID);
}
} // namespace NetManagerStandard
} // namespace OHOS
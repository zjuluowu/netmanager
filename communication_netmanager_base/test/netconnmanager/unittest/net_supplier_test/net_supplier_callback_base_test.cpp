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

#include <gtest/gtest.h>
#define private public
#include "net_manager_constants.h"
#include "net_supplier_callback_base.h"
#include "net_supplier_callback_stub.h"
namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class NetSupplierCallbackBaseTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetSupplierCallbackBase> supplierCbStub_ =
        std::make_shared<NetSupplierCallbackBase>();
};

void NetSupplierCallbackBaseTest::SetUpTestCase() {}

void NetSupplierCallbackBaseTest::TearDownTestCase() {}

void NetSupplierCallbackBaseTest::SetUp() {}

void NetSupplierCallbackBaseTest::TearDown() {}

HWTEST_F(NetSupplierCallbackBaseTest, RequestNetwork001, TestSize.Level1)
{
    std::string ident = "testsupid";
    std::set<NetCap> netCaps;
    netCaps.insert(NetCap::NET_CAPABILITY_NOT_METERED);
    int32_t ret = supplierCbStub_->RequestNetwork(ident, netCaps);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}

HWTEST_F(NetSupplierCallbackBaseTest, ReleaseNetwork001, TestSize.Level1)
{
    NetRequest netrequest;
    netrequest.ident = "testIdent";
    netrequest.netCaps.insert(NetCap::NET_CAPABILITY_NOT_METERED);
    int32_t ret = supplierCbStub_->ReleaseNetwork(netrequest);
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS
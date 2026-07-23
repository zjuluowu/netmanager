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

#include "dns_result_call_back.h"
#include "net_manager_constants.h"

#include <fcntl.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
namespace {
constexpr const int32_t TEST_NETID = 0;
constexpr const int32_t TEST_UID = 10000;
constexpr const int32_t TEST_PID = 1000;
constexpr const int32_t TEST_TIME = 500;
constexpr const int32_t TEST_PASS = 0;
constexpr const int32_t TEST_FAIL = -1;
constexpr const char *TEST_HOST = "www.test.com";

class TestDnsResultCallback : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetDnsResultCallback> instance_ = std::make_shared<NetDnsResultCallback>();
};

void TestDnsResultCallback::SetUpTestCase() {}

void TestDnsResultCallback::TearDownTestCase() {}

void TestDnsResultCallback::SetUp() {}

void TestDnsResultCallback::TearDown() {}

HWTEST_F(TestDnsResultCallback, OnDnsResultReportTest001, TestSize.Level1)
{
    NetsysNative::NetDnsResultReport netDnsPassReport;
    NetsysNative::NetDnsResultReport netDnsFailReport;
    netDnsPassReport.netid_ = TEST_NETID;
    netDnsPassReport.uid_ = TEST_UID;
    netDnsPassReport.pid_ = TEST_PID;
    netDnsPassReport.timeused_ = TEST_TIME;
    netDnsPassReport.queryresult_ = TEST_PASS;
    netDnsPassReport.host_ = TEST_HOST;

    netDnsFailReport.netid_ = TEST_NETID;
    netDnsFailReport.uid_ = TEST_UID;
    netDnsFailReport.pid_ = TEST_PID;
    netDnsFailReport.timeused_ = TEST_TIME;
    netDnsFailReport.queryresult_ = TEST_FAIL;
    netDnsFailReport.host_ = TEST_HOST;

    std::list<NetsysNative::NetDnsResultReport> netDnsResultReport;
    netDnsResultReport.push_back(netDnsPassReport);
    netDnsResultReport.push_back(netDnsFailReport);
    int32_t ret = instance_->OnDnsResultReport(netDnsResultReport.size(), netDnsResultReport);
    EXPECT_EQ(ret, 0);
}
} // namespace
} // namespace NetManagerStandard
} // namespace OHOS

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
#include <map>
#include <thread>

#include "errorcode_convertor.h"
#include "event_report.h"
#include "net_manager_constants.h"
#include "netmanager_hitrace.h"
#include "ipc_types.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
using namespace testing::ext;
constexpr uint32_t TEST_DELAY_TIME = 1;
constexpr const char *TEST_VALUE = "TEST_VALUE";
constexpr int32_t TEST_TASK_ID = 10054;
} // namespace

class ErrorCodeConvertorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void AllTest001();
};

void ErrorCodeConvertorTest::SetUpTestCase() {}

void ErrorCodeConvertorTest::TearDownTestCase() {}

void ErrorCodeConvertorTest::SetUp() {}

void ErrorCodeConvertorTest::TearDown() {}

void ErrorCodeConvertorTest::AllTest001()
{
    NetmanagerHiTrace::NetmanagerStartSyncTrace(TEST_VALUE);
    std::this_thread::sleep_for(std::chrono::seconds(TEST_DELAY_TIME));
    NetmanagerHiTrace::NetmanagerFinishSyncTrace(TEST_VALUE);
    std::thread thread([this]() {
        NetmanagerHiTrace::NetmanagerStartAsyncTrace(TEST_VALUE, TEST_TASK_ID);
        std::this_thread::sleep_for(std::chrono::seconds(TEST_DELAY_TIME));
        NetmanagerHiTrace::NetmanagerFinishAsyncTrace(TEST_VALUE, TEST_TASK_ID);
    });
    thread.join();
}

HWTEST_F(ErrorCodeConvertorTest, ConvertErrorCodeTest001, TestSize.Level1)
{
    EventInfo eventInfo;
    EventReport::SendSupplierFaultEvent(eventInfo);
    EventReport::SendSupplierBehaviorEvent(eventInfo);
    EventReport::SendRequestFaultEvent(eventInfo);
    EventReport::SendRequestBehaviorEvent(eventInfo);
    EventReport::SendMonitorFaultEvent(eventInfo);
    EventReport::SendMonitorBehaviorEvent(eventInfo);

    AllTest001();

    int32_t testErrorCode = 5445645;
    auto instance = std::make_unique<NetBaseErrorCodeConvertor>();
    int32_t errorCode = NETMANAGER_ERR_INTERNAL;
    auto ret = instance->ConvertErrorCode(errorCode);
    ASSERT_FALSE(ret.empty());
    ret = instance->ConvertErrorCode(testErrorCode);
    ASSERT_TRUE(ret.empty());
    testErrorCode = IPC_PROXY_ERR;
    ret = instance->ConvertErrorCode(testErrorCode);
    ASSERT_FALSE(ret.empty());
    testErrorCode = 1000; // 1000:ERROR_DIVISOR
    ret = instance->ConvertErrorCode(testErrorCode);
    ASSERT_TRUE(ret.empty());
}
} // namespace NetManagerStandard
} // namespace OHOS
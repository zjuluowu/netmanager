/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>

#include "dns_quality_event_handler.h"
#include "dns_quality_diag.h"

namespace OHOS::nmd {
namespace {
using namespace testing::ext;
const char *DNS_DIAG_WORK_THREAD = "DNS_DIAG_WORK_THREAD";
}  // namespace

class DnsQualityEventHandlerMock : public DnsQualityEventHandler {
public:
    explicit DnsQualityEventHandlerMock(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
        : DnsQualityEventHandler(runner)
    {
    }
    MOCK_METHOD1(ProcessEvent, void(const AppExecFwk::InnerEvent::Pointer &event));
};

class DnsQualityEventHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DnsQualityEventHandlerTest::SetUpTestCase() {}

void DnsQualityEventHandlerTest::TearDownTestCase() {}

void DnsQualityEventHandlerTest::SetUp() {}

void DnsQualityEventHandlerTest::TearDown() {}

HWTEST_F(DnsQualityEventHandlerTest, ProcessEvent_ShouldCallHandleEvent_WhenEventIsNotNullptr, TestSize.Level0)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(DNS_DIAG_WORK_THREAD);
    if (runner) {
        std::shared_ptr<DnsQualityEventHandlerMock> pMockHandler = std::make_shared<DnsQualityEventHandlerMock>(runner);
        AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get();
        EXPECT_CALL(*pMockHandler, ProcessEvent(testing::_)).Times(1);
        pMockHandler->ProcessEvent(event);
    }
}

HWTEST_F(DnsQualityEventHandlerTest, ProcessEvent_ShouldHandleEvent_WhenEventIsNotNull, TestSize.Level0)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(DNS_DIAG_WORK_THREAD);
    DnsQualityEventHandler *dnsQualityEventHandler = new DnsQualityEventHandler(runner);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get();
    dnsQualityEventHandler->ProcessEvent(event);
    EXPECT_NE(event.get(), nullptr);
}

HWTEST_F(DnsQualityEventHandlerTest, ProcessEvent_ShouldNotHandleEvent_WhenEventIsNull, TestSize.Level0)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(DNS_DIAG_WORK_THREAD);
    DnsQualityEventHandler *dnsQualityEventHandler = new DnsQualityEventHandler(runner);
    AppExecFwk::InnerEvent::Pointer event = AppExecFwk::InnerEvent::Get();
    event.reset();
    dnsQualityEventHandler->ProcessEvent(event);
    EXPECT_EQ(event.get(), nullptr);
}

}  // namespace OHOS::nmd

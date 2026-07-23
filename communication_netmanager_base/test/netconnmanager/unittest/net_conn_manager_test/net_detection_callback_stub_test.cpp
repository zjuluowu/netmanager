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
#include <iostream>
#include <memory>
#define private public
#include "net_detection_callback_stub.h"
#undef private

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class MockNetDetectionCallbackStubTest : public NetDetectionCallbackStub {
public:
    MockNetDetectionCallbackStubTest() = default;
    ~MockNetDetectionCallbackStubTest() override {}
    int32_t OnNetDetectionResultChanged(NetDetectionResultCode detectionResult, const std::string &urlRedirect) override
    {
        std::cout << std::endl;
        std::cout << "OnNetDetectionResultChanged" << std::endl;
        return 0;
    }
};

class NetDetectionCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetDetectionCallbackStub> instance_ =
        std::make_shared<MockNetDetectionCallbackStubTest>();
};

void NetDetectionCallbackStubTest::SetUpTestCase() {}

void NetDetectionCallbackStubTest::TearDownTestCase() {}

void NetDetectionCallbackStubTest::SetUp() {}

void NetDetectionCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test NetDetectionCallbackStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NetDetectionCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(100, data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_ERR_DESCRIPTOR_MISMATCH);

    data.WriteInterfaceToken(NetDetectionCallbackStub::GetDescriptor());
    ret = instance_->OnRemoteRequest(100, data, reply, option);
    EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: OnNetDetectionResult001
 * @tc.desc: Test NetDetectionCallbackStub OnNetDetectionResult.
 * @tc.type: FUNC
 */
HWTEST_F(NetDetectionCallbackStubTest, OnNetDetectionResult001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(NetDetectionCallbackStub::GetDescriptor());
    data.WriteString("test");
    data.WriteInt32(NET_DETECTION_SUCCESS);
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(DetectionCallback::NET_DETECTION_RESULT),
                                             data, reply, option);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS
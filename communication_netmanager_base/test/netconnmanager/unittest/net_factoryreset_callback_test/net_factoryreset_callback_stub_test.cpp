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
#ifdef GTEST_API_
#define private public
#endif
#include "net_factoryreset_callback_stub.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace testing::ext;
class MockNetFactoryResetCallbackStubTest : public NetFactoryResetCallbackStub {
public:
    MockNetFactoryResetCallbackStubTest() = default;
    ~MockNetFactoryResetCallbackStubTest() override {}
    int32_t OnNetFactoryReset() override
    {
        std::cout << std::endl;
        std::cout << "OnNetFactoryReset" << std::endl;
        return 0;
    }
};

class NetFactoryResetCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline std::shared_ptr<NetFactoryResetCallbackStub> instance_ =
        std::make_shared<MockNetFactoryResetCallbackStubTest>();
};

void NetFactoryResetCallbackStubTest::SetUpTestCase() {}

void NetFactoryResetCallbackStubTest::TearDownTestCase() {}

void NetFactoryResetCallbackStubTest::SetUp() {}

void NetFactoryResetCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test NetDetectionCallbackStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NetFactoryResetCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t ret = instance_->OnRemoteRequest(100, data, reply, option);
    EXPECT_EQ(ret, NETMANAGER_ERR_DESCRIPTOR_MISMATCH);

    data.WriteInterfaceToken(NetFactoryResetCallbackStub::GetDescriptor());
    ret = instance_->OnRemoteRequest(100, data, reply, option);
    EXPECT_EQ(ret, IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: OnNetDetectionResult001
 * @tc.desc: Test NetDetectionCallbackStub OnNetDetectionResult.
 * @tc.type: FUNC
 */
HWTEST_F(NetFactoryResetCallbackStubTest, OnNetFactoryReset001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(NetFactoryResetCallbackStub::GetDescriptor());
    int32_t ret = instance_->OnRemoteRequest(static_cast<uint32_t>(FactoryResetCallbackInterfaceCode::NET_FACTORYRESET),
                                             data, reply, option);
    EXPECT_EQ(ret, NETSYS_SUCCESS);
}

/**
 * @tc.name: OnNetFactoryReset002
 * @tc.desc: Test NetDetectionCallbackStub OnNetFactoryReset.
 * @tc.type: FUNC
 */
HWTEST_F(NetFactoryResetCallbackStubTest, OnNetFactoryReset002, TestSize.Level1)
{
    int32_t ret = instance_->OnNetFactoryReset();
    EXPECT_EQ(ret, NETMANAGER_SUCCESS);
}
} // namespace NetManagerStandard
} // namespace OHOS

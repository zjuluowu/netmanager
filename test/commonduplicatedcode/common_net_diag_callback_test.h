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

#ifndef COMMON_NET_DIAG_CALLBACK_TEST_H
#define COMMON_NET_DIAG_CALLBACK_TEST_H

#include "net_diag_callback_stub.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_ipc_interface_code.h"
#include "netsys_net_diag_data.h"
#include "notify_callback_stub.h"

namespace OHOS {
namespace NetsysNative {
namespace {
bool g_waitPingSync = false;
} // namespace

class NetDiagCallbackStubTest : public IRemoteStub<NetsysNative::INetDiagCallback> {
public:
    NetDiagCallbackStubTest()
    {
        memberFuncMap_[static_cast<uint32_t>(NetsysNative::NetDiagInterfaceCode::ON_NOTIFY_PING_RESULT)] =
            &NetDiagCallbackStubTest::CmdNotifyPingResult;
    }
    virtual ~NetDiagCallbackStubTest() = default;

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        NETNATIVE_LOGI("Stub call start, code:[%{public}d]", code);
        std::u16string myDescriptor = NetsysNative::NetDiagCallbackStub::GetDescriptor();
        std::u16string remoteDescriptor = data.ReadInterfaceToken();
        if (myDescriptor != remoteDescriptor) {
            NETNATIVE_LOGE("Descriptor checked failed");
            return NetManagerStandard::NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
        }

        auto itFunc = memberFuncMap_.find(code);
        if (itFunc != memberFuncMap_.end()) {
            auto requestFunc = itFunc->second;
            if (requestFunc != nullptr) {
                return (this->*requestFunc)(data, reply);
            }
        }

        NETNATIVE_LOGI("Stub default case, need check");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    int32_t OnNotifyPingResult(const NetsysNative::NetDiagPingResult &pingResult) override
    {
        g_waitPingSync = false;
        NETNATIVE_LOGI(
            "OnNotifyPingResult received dateSize_:%{public}d payloadSize_:%{public}d transCount_:%{public}d "
            "recvCount_:%{public}d",
            pingResult.dateSize_, pingResult.payloadSize_, pingResult.transCount_, pingResult.recvCount_);
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

private:
    using NetDiagCallbackFunc = int32_t (NetDiagCallbackStubTest::*)(MessageParcel &, MessageParcel &);

private:
    int32_t CmdNotifyPingResult(MessageParcel &data, MessageParcel &reply)
    {
        NETNATIVE_LOGI("CmdNotifyPingResult received CmdNotifyPingResult");

        NetsysNative::NetDiagPingResult pingResult;
        if (!NetsysNative::NetDiagPingResult::Unmarshalling(data, pingResult)) {
            return NetManagerStandard::NETMANAGER_ERR_READ_DATA_FAIL;
        }

        int32_t result = OnNotifyPingResult(pingResult);
        if (!reply.WriteInt32(result)) {
            return NetManagerStandard::NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        return NetManagerStandard::NETMANAGER_SUCCESS;
    }

private:
    std::map<uint32_t, NetDiagCallbackFunc> memberFuncMap_;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // COMMON_NET_DIAG_CALLBACK_TEST_H

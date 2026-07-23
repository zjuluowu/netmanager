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

#include "net_diag_callback_stub.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_ipc_interface_code.h"

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace OHOS::NetManagerStandard;
} // namespace

NetDiagCallbackStub::NetDiagCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(NetDiagInterfaceCode::ON_NOTIFY_PING_RESULT)] =
        &NetDiagCallbackStub::CmdNotifyPingResult;
}

int32_t NetDiagCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                             MessageOption &option)
{
    NETNATIVE_LOGI("Stub call start, code:[%{public}d]", code);
    std::u16string myDescriptor = NetDiagCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        NETNATIVE_LOGE("Descriptor checked failed");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    NETNATIVE_LOGE("Stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetDiagCallbackStub::CmdNotifyPingResult(MessageParcel &data, MessageParcel &reply)
{
    NetDiagPingResult pingResult;
    if (!NetDiagPingResult::Unmarshalling(data, pingResult)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = OnNotifyPingResult(pingResult);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetDiagCallbackStub::OnNotifyPingResult(const NetDiagPingResult &pingResult)
{
    NETNATIVE_LOGI("OnNotifyPingResult received.");
    return NETMANAGER_SUCCESS;
}
} // namespace NetsysNative
} // namespace OHOS

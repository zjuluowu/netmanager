/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "netsys_traffic_callback_stub.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_ipc_interface_code.h"

namespace OHOS {
namespace NetsysNative {
NetsysTrafficCallbackStub::NetsysTrafficCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(NetsysTrafficfaceCode::NETSYS_TRAFFIC_STATUS_CHANGED)] =
        &NetsysTrafficCallbackStub::CmdOnExceedTrafficLimits;
}

int32_t NetsysTrafficCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    NETNATIVE_LOGI("Stub call start, code:[%{public}d]", code);
    std::u16string myDescriptor = NetsysTrafficCallbackStub::GetDescriptor();
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

int32_t NetsysTrafficCallbackStub::CmdOnExceedTrafficLimits(MessageParcel &data, MessageParcel &reply)
{
    int8_t flag = 0;
    if (!data.ReadInt8(flag)) {
        NETNATIVE_LOGE("CmdOnExceedTrafficLimits read flag failed");
        return NetManagerStandard::NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t result = OnExceedTrafficLimits(flag);
    if (!reply.WriteInt32(result)) {
        return NetManagerStandard::NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

int32_t NetsysTrafficCallbackStub::OnExceedTrafficLimits(int8_t &flag)
{
    return NetManagerStandard::NETMANAGER_SUCCESS;
}
} // namespace NetsysNative
} // namespace OHOS

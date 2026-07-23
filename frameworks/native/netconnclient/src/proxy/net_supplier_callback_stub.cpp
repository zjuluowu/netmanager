/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "net_supplier_callback_stub.h"

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"

static constexpr uint32_t MAX_NET_CAP_NUM = 32;
static constexpr uint32_t MAX_NET_BEARTYPE_NUM = 7;

namespace OHOS {
namespace NetManagerStandard {
NetSupplierCallbackStub::NetSupplierCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(SupplierInterfaceCode::NET_SUPPLIER_REQUEST_NETWORK)] =
        &NetSupplierCallbackStub::OnRequestNetwork;
    memberFuncMap_[static_cast<uint32_t>(SupplierInterfaceCode::NET_SUPPLIER_RELEASE_NETWORK)] =
        &NetSupplierCallbackStub::OnReleaseNetwork;
}

NetSupplierCallbackStub::~NetSupplierCallbackStub() {}

void NetSupplierCallbackStub::RegisterSupplierCallbackImpl(const sptr<NetSupplierCallbackBase> &callback)
{
    callback_ = callback;
}

int32_t NetSupplierCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                 MessageOption &option)
{
    NETMGR_LOG_D("Net supplier callback stub call start, code:[%{public}d]", code);
    std::u16string myDescripter = NetSupplierCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (myDescripter != remoteDescripter) {
        NETMGR_LOG_I("Descriptor checked failed");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    NETMGR_LOG_I("Stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetSupplierCallbackStub::OnRequestNetwork(MessageParcel &data, MessageParcel &reply)
{
    std::string ident;
    std::set<NetCap> netCaps;

    data.ReadString(ident);
    uint32_t size = 0;
    uint32_t value = 0;
    data.ReadUint32(size);
    if (size > MAX_NET_CAP_NUM) {
        NETMGR_LOG_E("Net cap size is too large");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    for (uint32_t i = 0; i < size; i++) {
        data.ReadUint32(value);
        if (value < NET_CAPABILITY_END) {
            netCaps.insert(static_cast<NetCap>(value));
        }
    }
    int32_t registerType = 0;
    data.ReadInt32(registerType);
    std::set<NetBearType> netBearTypes;
    uint32_t bearTypeSize = 0;
    data.ReadUint32(bearTypeSize);
    if (bearTypeSize > MAX_NET_BEARTYPE_NUM) {
        NETMGR_LOG_E("Net beartype size is too large");
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    for (uint32_t i = 0; i < bearTypeSize; i++) {
        data.ReadUint32(value);
        if (value <= BEARER_DEFAULT) {
            netBearTypes.insert(static_cast<NetBearType>(value));
        }
    }
    uint32_t uid = 0;
    data.ReadUint32(uid);
    uint32_t requestId = 0;
    data.ReadUint32(requestId);
    std::string requestIdent;
    data.ReadString(requestIdent);
    NetRequest netRequest(uid, requestId, registerType, requestIdent, netBearTypes, netCaps);
    RequestNetwork(ident, netCaps, netRequest);

    reply.WriteInt32(0);
    return NETMANAGER_SUCCESS;
}

int32_t NetSupplierCallbackStub::OnReleaseNetwork(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    uint32_t requestId = 0;
    uint32_t registerType = 0;
    std::string ident;
    uint32_t size = 0;
    int32_t result = data.ReadUint32(uid) && data.ReadUint32(requestId) && data.ReadUint32(registerType) &&
                     data.ReadString(ident) && data.ReadUint32(size);
    if (!result) {
        NETMGR_LOG_E("Read uid, requestid, registerType, ident or size failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    std::set<NetBearType> netBearTypes;
    int32_t value = 0;
    if (size > MAX_NET_BEARTYPE_NUM) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    for (uint32_t i = 0; i < size; i++) {
        if (!data.ReadInt32(value)) {
            NETMGR_LOG_E("Read bearType failed");
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        if (value <= BEARER_DEFAULT) {
            netBearTypes.insert(static_cast<NetBearType>(value));
        }
    }
    std::set<NetCap> netCaps;
    if (!data.ReadUint32(size)) {
        NETMGR_LOG_E("Read size failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    if (size > MAX_NET_CAP_NUM) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    for (uint32_t i = 0; i < size; i++) {
        if (!data.ReadInt32(value)) {
            NETMGR_LOG_E("Read Netcap failed");
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        if (value < NET_CAPABILITY_END) {
            netCaps.insert(static_cast<NetCap>(value));
        }
    }
    NetRequest netrequest(uid, requestId, registerType, ident, netBearTypes, netCaps);
    ReleaseNetwork(netrequest);
    reply.WriteInt32(0);
    return NETMANAGER_SUCCESS;
}

int32_t NetSupplierCallbackStub::RequestNetwork(const std::string &ident, const std::set<NetCap> &netCaps,
    const NetRequest &netrequest)
{
    if (callback_ != nullptr) {
        auto startTime = std::chrono::steady_clock::now();
        callback_->RequestNetwork(ident, netCaps, netrequest);
        auto endTime = std::chrono::steady_clock::now();
        auto durationNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        NETMGR_LOG_I("RequestNetwork[%{public}s], cost=%{public}lld", ident.c_str(), durationNs.count());
    }
    return 0;
}

int32_t NetSupplierCallbackStub::ReleaseNetwork(const NetRequest &netrequest)
{
    if (callback_ != nullptr) {
        auto startTime = std::chrono::steady_clock::now();
        callback_->ReleaseNetwork(netrequest);
        auto endTime = std::chrono::steady_clock::now();
        auto durationNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        NETMGR_LOG_I("ReleaseNetwork[%{public}s], cost=%{public}lld", netrequest.ident.c_str(), durationNs.count());
    }
    return 0;
}
} // namespace NetManagerStandard
} // namespace OHOS

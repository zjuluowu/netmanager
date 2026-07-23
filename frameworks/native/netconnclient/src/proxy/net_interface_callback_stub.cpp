/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "net_interface_callback_stub.h"

#include "net_conn_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {

NetInterfaceStateCallbackStub::NetInterfaceStateCallbackStub()
{
    memberFuncMap_[static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDR_UPDATED)] =
        &NetInterfaceStateCallbackStub::CmdInterfaceAddressUpdated;
    memberFuncMap_[static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDR_REMOVED)] =
        &NetInterfaceStateCallbackStub::CmdInterfaceAddressRemoved;
    memberFuncMap_[static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_ADDED)] =
        &NetInterfaceStateCallbackStub::CmdInterfaceAdded;
    memberFuncMap_[static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_REMOVED)] =
        &NetInterfaceStateCallbackStub::CmdInterfaceRemoved;
    memberFuncMap_[static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_CHANGED)] =
        &NetInterfaceStateCallbackStub::CmdInterfaceChanged;
    memberFuncMap_[static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_IFACE_LINK_STATE_CHANGED)] =
        &NetInterfaceStateCallbackStub::CmdInterfaceLinkStateChanged;
    memberFuncMap_[static_cast<uint32_t>(InterfaceCallbackInterfaceCode::CMD_ON_ROUTE_CHANGED)] =
        &NetInterfaceStateCallbackStub::CmdRouteChanged;
}

int32_t NetInterfaceStateCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                       MessageOption &option)
{
    NETMGR_LOG_D("Stub call start, code:[%{public}d]", code);
    std::u16string myDescriptor = NetInterfaceStateCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        NETMGR_LOG_E("Descriptor checked failed");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }

    NETMGR_LOG_D("Stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetInterfaceStateCallbackStub::CmdInterfaceAddressUpdated(MessageParcel &data, MessageParcel &reply)
{
    std::string addr = data.ReadString();
    std::string ifName = data.ReadString();
    int32_t flags = data.ReadInt32();
    int32_t scope = data.ReadInt32();

    int32_t result = OnInterfaceAddressUpdated(addr, ifName, flags, scope);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::CmdInterfaceAddressRemoved(MessageParcel &data, MessageParcel &reply)
{
    std::string addr;
    if (!data.ReadString(addr)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifName;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t flags = 0;
    if (!data.ReadInt32(flags)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t scope = 0;
    if (!data.ReadInt32(scope)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = OnInterfaceAddressRemoved(addr, ifName, flags, scope);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::CmdInterfaceAdded(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = OnInterfaceAdded(ifName);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::CmdInterfaceRemoved(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = OnInterfaceRemoved(ifName);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::CmdInterfaceChanged(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    bool isUp = false;
    if (!data.ReadBool(isUp)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = OnInterfaceChanged(ifName, isUp);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::CmdInterfaceLinkStateChanged(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    bool isUp = false;
    if (!data.ReadBool(isUp)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = OnInterfaceLinkStateChanged(ifName, isUp);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::CmdRouteChanged(MessageParcel &data, MessageParcel &reply)
{
    bool updated = false;
    if (!data.ReadBool(updated)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string route;
    if (!data.ReadString(route)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string gateway;
    if (!data.ReadString(gateway)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifName;
    if (!data.ReadString(ifName)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = OnRouteChanged(updated, route, gateway, ifName);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName,
                                                                 int32_t flags, int32_t scope)
{
    NETMGR_LOG_D("OnInterfaceAddressUpdated, addr, iface:[%{public}s], scope:[%{public}d]", ifName.c_str(), scope);
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName,
                                                                 int32_t flags, int32_t scope)
{
    NETMGR_LOG_D("OnInterfaceAddressRemoved, addr, iface:[%{public}s], scope:[%{public}d]", ifName.c_str(), scope);
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::OnInterfaceAdded(const std::string &ifName)
{
    NETMGR_LOG_D("OnInterfaceAdded, iface:[%{public}s]", ifName.c_str());
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::OnInterfaceRemoved(const std::string &ifName)
{
    NETMGR_LOG_D("OnInterfaceRemoved, iface:[%{public}s]", ifName.c_str());
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::OnInterfaceChanged(const std::string &ifName, bool up)
{
    NETMGR_LOG_D("OnInterfaceChanged, iface:[%{public}s] -> Up:[%{public}d]", ifName.c_str(), up);
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::OnInterfaceLinkStateChanged(const std::string &ifName, bool up)
{
    NETMGR_LOG_D("OnInterfaceLinkStateChanged, iface:[%{public}s] -> Up:[%{public}d]", ifName.c_str(), up);
    return NETMANAGER_SUCCESS;
}

int32_t NetInterfaceStateCallbackStub::OnRouteChanged(bool updated, const std::string &route,
                                                      const std::string &gateway, const std::string &ifName)
{
    NETMGR_LOG_D("OnRouteChanged, updated:[%{public}d], route:[%{public}s], gateway:[%{public}s], iface:[%{public}s]",
                 updated, route.c_str(), gateway.c_str(), ifName.c_str());
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS

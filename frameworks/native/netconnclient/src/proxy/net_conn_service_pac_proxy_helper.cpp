/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "net_conn_service_pac_proxy_helper.h"
#include "net_conn_service_proxy.h"

#include "net_conn_constants.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace NetManagerStandard {
std::shared_ptr<NetConnServicePacProxyHelper> NetConnServicePacProxyHelper::GetInstance(
    RequestFunction fun)
{
    static std::shared_ptr<NetConnServicePacProxyHelper> instance;
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
        instance = std::make_shared<NetConnServicePacProxyHelper>();
        instance->requestFunction_ = fun;
    }
    return instance;
}

bool NetConnServicePacProxyHelper::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetConnServiceProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t NetConnServicePacProxyHelper::SetPacUrl(const std::string &pacUrl)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(pacUrl)) {
        NETMGR_LOG_E("Write pacUrl string data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = requestFunction_(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_PAC_URL),
                                     data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServicePacProxyHelper::GetPacFileUrl(std::string &pacUrl)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t error = requestFunction_(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_PAC_FILE_URL), data,
                                     reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = reply.ReadInt32();
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadString(pacUrl)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServicePacProxyHelper::SetPacFileUrl(const std::string &pacUrl)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(pacUrl)) {
        NETMGR_LOG_E("Write pacFileUrl string data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = requestFunction_(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_PAC_FILE_URL), data,
                                     reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServicePacProxyHelper::GetProxyMode(OHOS::NetManagerStandard::ProxyModeType &mode)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    uint32_t tempCode = static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_PROXY_MODE);
    int32_t error = requestFunction_(tempCode, data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    int32_t temp;
    if (!reply.ReadInt32(temp)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    switch (temp) {
        case PROXY_MODE_OFF:
            mode = OHOS::NetManagerStandard::ProxyModeType::PROXY_MODE_OFF;
            break;
        case PROXY_MODE_AUTO:
            mode = OHOS::NetManagerStandard::ProxyModeType::PROXY_MODE_AUTO;
            break;
        default:
            return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServicePacProxyHelper::SetProxyMode(const OHOS::NetManagerStandard::ProxyModeType mode)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(mode)) {
        NETMGR_LOG_E("Write proxy policy string data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t error = requestFunction_(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_SET_PROXY_MODE),
                                     data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServicePacProxyHelper::FindProxyForURL(const std::string &url, const std::string &host,
                                                      std::string &proxy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(url)) {
        NETMGR_LOG_E("Write url string data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(host)) {
        NETMGR_LOG_E("Write host string data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    uint32_t codeTemp = static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_FIND_PAC_PROXY_FOR_URL);
    int32_t error = requestFunction_(codeTemp, data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = reply.ReadInt32();
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadString(proxy)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServicePacProxyHelper::GetPacUrl(std::string &pacUrl)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t error = requestFunction_(static_cast<uint32_t>(ConnInterfaceCode::CMD_NM_GET_PAC_URL),
                                     data, reply);
    if (error != NETMANAGER_SUCCESS) {
        return error;
    }

    int32_t ret = reply.ReadInt32();
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadString(pacUrl)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}
}
}

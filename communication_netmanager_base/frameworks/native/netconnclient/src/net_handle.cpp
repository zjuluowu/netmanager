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

#include "net_handle.h"

#include "iservice_registry.h"
#include "net_conn_constants.h"
#include "net_manager_constants.h"
#include "system_ability_definition.h"

#include "net_conn_client.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
int32_t NetHandle::BindSocket(int32_t socketFd)
{
    if (socketFd < 0) {
        NETMGR_LOG_E("socketFd is invalid");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    return NetConnClient::GetInstance().BindSocket(socketFd, netId_);
}
} // namespace NetManagerStandard
} // namespace OHOS
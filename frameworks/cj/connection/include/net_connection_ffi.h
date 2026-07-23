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

#ifndef NET_CONNECTION_FFI_H
#define NET_CONNECTION_FFI_H

#include "cj_ffi/cj_common_ffi.h"
#include "common.h"
#include "ffi_remote_data.h"
#include "napi/native_api.h"
#include "netmanager_base_log.h"
#include <cstdint>
#include <list>
#include <set>
#include <vector>

EXTERN_C_START
FFI_EXPORT int64_t CJ_CreateNetConnection(CNetSpecifier netSpecifier, uint32_t timeout);
FFI_EXPORT void CJ_ReleaseNetConnection(int64_t connId);
FFI_EXPORT int32_t CJ_GetDefaultNet(int32_t &netId);
FFI_EXPORT RetNetAddressArr CJ_GetAddressesByName(int32_t netId, const char *host);
FFI_EXPORT int32_t CJ_IsDefaultNetMetered(bool &ret);
FFI_EXPORT int32_t CJ_HasDefaultNet(bool &ret);
FFI_EXPORT int32_t CJ_GetNetCapabilities(int32_t netId, CNetCapabilities &ret);
FFI_EXPORT int32_t CJ_GetConnectionProperties(int32_t netId, CConnectionProperties &ret);
FFI_EXPORT int32_t CJ_GetGlobalHttpProxy(CHttpProxy &chttpProxy);
FFI_EXPORT int32_t CJ_GetDefaultHttpProxy(CHttpProxy &chttpProxy);
FFI_EXPORT int32_t CJ_SetGlobalHttpProxy(CHttpProxy cHttpProxy);
FFI_EXPORT RetDataCArrI32 CJ_GetAllNets();
FFI_EXPORT int32_t CJ_EnableAirplaneMode();
FFI_EXPORT int32_t CJ_DisableAirplaneMode();
FFI_EXPORT int32_t CJ_ReportNetConnected(int32_t netId);
FFI_EXPORT int32_t CJ_ReportNetDisconnected(int32_t netId);
FFI_EXPORT int32_t CJ_NetConnectionRegister(int64_t id);
FFI_EXPORT int32_t CJ_NetConnectionUnRegister(int64_t id);
FFI_EXPORT int32_t CJ_NetHandleBindSocket(int32_t netId, int socketFd);
FFI_EXPORT void CJ_OnNetAvailable(int64_t connId, void (*callback)(int32_t));
FFI_EXPORT void CJ_OnNetBlockStatusChange(int64_t connId, void (*callback)(int32_t, bool));
FFI_EXPORT void CJ_OnNetCapabilitiesChange(int64_t connId, void (*callback)(CNetCapabilityInfo));
FFI_EXPORT void CJ_OnNetConnectionPropertiesChange(int64_t connId, void (*callback)(int32_t, CConnectionProperties));
FFI_EXPORT void CJ_OnNetLost(int64_t connId, void (*callback)(int32_t));
FFI_EXPORT void CJ_OnNetUnavailable(int64_t connId, void (*callback)());
FFI_EXPORT int32_t CJ_GetAppNet(int32_t &netId);
FFI_EXPORT int32_t CJ_SetAppNet(int32_t netId);
FFI_EXPORT napi_value FfiConvertNetHandle2Napi(napi_env env, uint32_t netId);

char *MallocCString(const std::string &origin);
char **MallocCStringList(std::list<std::string> &list);
EXTERN_C_END

#endif
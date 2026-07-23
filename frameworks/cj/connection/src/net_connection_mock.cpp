/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "cj_ffi/cj_common_ffi.h"

EXTERN_C_START
FFI_EXPORT int CJ_CreateNetConnection = 0;
FFI_EXPORT int CJ_GetDefaultNet = 0;
FFI_EXPORT int CJ_GetAddressesByName = 0;
FFI_EXPORT int CJ_GetAddressByName = 0;
FFI_EXPORT int CJ_IsDefaultNetMetered = 0;
FFI_EXPORT int CJ_HasDefaultNet = 0;
FFI_EXPORT int CJ_GetNetCapabilities = 0;
FFI_EXPORT int CJ_GetConnectionProperties = 0;
FFI_EXPORT int CJ_GetGlobalHttpProxy = 0;
FFI_EXPORT int CJ_GetDefaultHttpProxy = 0;
FFI_EXPORT int CJ_SetGlobalHttpProxy = 0;
FFI_EXPORT int CJ_GetAllNets = 0;
FFI_EXPORT int CJ_EnableAirplaneMode = 0;
FFI_EXPORT int CJ_DisableAirplaneMode = 0;
FFI_EXPORT int CJ_ReportNetConnected = 0;
FFI_EXPORT int CJ_ReportNetDisconnected = 0;
FFI_EXPORT int CJ_NetConnectionRegister = 0;
FFI_EXPORT int CJ_NetConnectionUnRegister = 0;
FFI_EXPORT int CJ_NetHandleBindSocket = 0;
FFI_EXPORT int CJ_OnNetAvailable = 0;
FFI_EXPORT int CJ_OnNetBlockStatusChange = 0;
FFI_EXPORT int CJ_OnNetCapabilitiesChange = 0;
FFI_EXPORT int CJ_OnNetConnectionPropertiesChange = 0;
FFI_EXPORT int CJ_OnNetLost = 0;
FFI_EXPORT int CJ_OnNetUnavailable = 0;
FFI_EXPORT int CJ_GetAppNet = 0;
FFI_EXPORT int CJ_SetAppNet = 0;
EXTERN_C_END
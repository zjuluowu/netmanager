/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef NETNATIVE_LOG_WRAPPER_H
#define NETNATIVE_LOG_WRAPPER_H

#include "netmanager_base_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#ifndef NETMGR_LOG_TAG
#define LOG_TAG "NetsysNativeService"
#else
#define LOG_TAG NETMGR_LOG_TAG
#endif

#define NETNATIVE_LOG_D(fmt, ...) NETMANAGER_LOG(DEBUG, fmt, ##__VA_ARGS__)
#define NETNATIVE_LOGE(fmt, ...) NETMANAGER_LOG(ERROR, fmt, ##__VA_ARGS__)
#define NETNATIVE_LOGW(fmt, ...) NETMANAGER_LOG(WARN, fmt, ##__VA_ARGS__)
#define NETNATIVE_LOGI(fmt, ...) NETMANAGER_LOG(INFO, fmt, ##__VA_ARGS__)
#define NETNATIVE_LOGF(fmt, ...) NETMANAGER_LOG(FATAL, fmt, ##__VA_ARGS__)

#if DNS_CONFIG_DEBUG
#define DNS_CONFIG_PRINT(fmt, ...) NETMANAGER_LOG(INFO, fmt, ##__VA_ARGS__)
#else
#define DNS_CONFIG_PRINT(fmt, ...) ((void)(0, ##__VA_ARGS__))
#endif

#ifdef __cplusplus
}
#endif

#endif // NETNATIVE_LOG_WRAPPER_H

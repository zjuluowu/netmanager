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

#ifndef COMMUNICATIONNETMANAGER_BASE_NETMANAGER_BASE_LOG
#define COMMUNICATIONNETMANAGER_BASE_NETMANAGER_BASE_LOG
#include "hilog/log.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#ifndef NETMGR_LOG_TAG
#define LOG_TAG "NetMgrSubSystem"
#else
#define LOG_TAG NETMGR_LOG_TAG
#endif
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD0015B0

// 日志宏主体,不额外输出源代码位置和函数名
#define NETMANAGER_LOG(Level, fmt, ...)                                                  \
    (void)HILOG_IMPL(LOG_CORE, LOG_##Level, LOG_DOMAIN, LOG_TAG, fmt, ##__VA_ARGS__)

#define MAKE_FILE_NAME (strrchr(__FILE__, '/') + 1)

// 日志宏函数，添加日志所在的源代码文件名和行号
#define NETMANAGER_LOG_CODELINE(Level, fmt, ...)                                                                    \
    do {                                                                                                            \
        (void)HILOG_IMPL(LOG_CORE, LOG_##Level, LOG_DOMAIN, LOG_TAG, "[%{public}s:%{public}d]" fmt, MAKE_FILE_NAME, \
                         __LINE__, ##__VA_ARGS__);                                                                  \
    } while (0)

// 日志宏函数，添加日志所在的函数名
#define NETMANAGER_LOG_FUNC(Level, fmt, ...)                                                                           \
    do {                                                                                                               \
        (void)HILOG_IMPL(LOG_CORE, LOG_##Level, LOG_DOMAIN, LOG_TAG, "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__); \
    } while (0)

// 简化接口（DEBUG级别对应空宏）
#define NETMANAGER_BASE_LOGE(fmt, ...) NETMANAGER_LOG(ERROR, fmt, ##__VA_ARGS__)
#define NETMANAGER_BASE_LOGI(fmt, ...) NETMANAGER_LOG(INFO, fmt, ##__VA_ARGS__)
#define NETMANAGER_BASE_LOGW(fmt, ...) NETMANAGER_LOG(WARN, fmt, ##__VA_ARGS__)
#define NETMANAGER_BASE_LOGD(fmt, ...) ((void)(0, ##__VA_ARGS__))

#define NETMGR_LOGD(fmt, ...) NETMANAGER_LOG(DEBUG, fmt, ##__VA_ARGS__)
#define NETMGR_LOG_D(fmt, ...) ((void)(0, ##__VA_ARGS__))
#define NETMGR_LOG_E(fmt, ...) NETMANAGER_LOG(ERROR, fmt, ##__VA_ARGS__)
#define NETMGR_LOG_W(fmt, ...) NETMANAGER_LOG(WARN, fmt, ##__VA_ARGS__)
#define NETMGR_LOG_I(fmt, ...) NETMANAGER_LOG(INFO, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* COMMUNICATIONNETMANAGER_BASE_NETMANAGER_BASE_LOG */

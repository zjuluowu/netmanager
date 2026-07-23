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

#include "mock_timer.h"
#include "cstdio"
#include "unistd.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dlfcn.h>

static struct tm g_defaultTmLocalTime;
static struct tm g_defaultGmtime;
bool g_enableMock = false;

tm *GetDefaultTmLocalTime()
{
    return &g_defaultTmLocalTime;
}

tm *GetDefaultGmtime()
{
    return &g_defaultGmtime;
}

void SetEnableMock(bool mock)
{
    g_enableMock = mock;
}

void InitDefaultTime()
{
    static struct tm *(*realLocaltime)(const time_t *) = nullptr;
    if (!realLocaltime) {
        realLocaltime = reinterpret_cast<struct tm *(*)(const time_t *)>(dlsym(RTLD_NEXT, "localtime"));
    }
    time_t rawtime;
    struct tm *timeinfo;
    if (time(&rawtime) == static_cast<time_t>(-1)) {
        perror("time function failed \n");
    }
    timeinfo = realLocaltime(&rawtime);
    if (timeinfo == nullptr) {
        perror("localtime function failed");
    }
    g_defaultTmLocalTime = *timeinfo;
    static struct tm *(*realGmtime)(const time_t *) = nullptr;
    if (!realGmtime) {
        realGmtime = reinterpret_cast<struct tm *(*)(const time_t *)>(dlsym(RTLD_NEXT, "gmtime"));
    }
    time_t rawtimeGmt;
    struct tm *timeinfoGmt;
    if (time(&rawtimeGmt) == static_cast<time_t>(-1)) {
        perror("time function failed \n");
    }
    timeinfoGmt = realGmtime(&rawtimeGmt);
    g_defaultGmtime = *timeinfoGmt;
}

struct tm *localtime(const time_t *timep)
{
    static struct tm *(*realLocaltime)(const time_t *) = nullptr;
    if (!realLocaltime) {
        realLocaltime = reinterpret_cast<struct tm *(*)(const time_t *)>(dlsym(RTLD_NEXT, "localtime"));
    }
    if (!g_enableMock) {
        return realLocaltime(timep);
    } else {
        return &g_defaultTmLocalTime;
    }
}

struct tm *gmtime(const time_t *timep)
{
    static struct tm *(*realGmtime)(const time_t *) = nullptr;
    if (!realGmtime) {
        realGmtime = reinterpret_cast<struct tm *(*)(const time_t *)>(dlsym(RTLD_NEXT, "gmtime"));
    }
    if (!g_enableMock) {
        return realGmtime(timep);
    } else {
        return &g_defaultGmtime;
    }
}

void EnableTimeMock()
{
    InitDefaultTime();
    g_enableMock = true;
}

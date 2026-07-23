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

#ifndef PAC_FUNCTIONS_H
#define PAC_FUNCTIONS_H
#include <cstdint>
#include "jerryscript.h"
namespace OHOS {
namespace NetManagerStandard {
class PacFunctions {
public:
    PacFunctions() = default;
    ~PacFunctions() = default;
    static void RegisterPacFunctions(void);

private:
    static jerry_value_t JsIsPlainHostname(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsDnsDomainIs(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsLocalHostOrDomainIs(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsConsoleInfo(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsIsResolvable(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsMyIpAddress(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsMyIpAddressEx(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsIsInNet(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsIsInNetEx(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsWeekdayRange(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsTimeRange(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsDateRange(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsShExpMatch(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsDnsDomainLevels(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsDnsResolve(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);
    static jerry_value_t JsSortIpAddressList(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
        const jerry_value_t args[], const jerry_length_t argsCnt);

    static void RegisterGlobalFunction(jerry_value_t globalObj, const char *funcName,
        jerry_external_handler_t handler);
    static void RegisterHostDomainFunctions(jerry_value_t globalObj);
    static void RegisterDnsResolveFunctions(jerry_value_t globalObj);
    static void RegisterIpAddressFunctions(jerry_value_t globalObj);
    static void RegisterTimeAndDateFunctions(jerry_value_t globalObj);
    static void RegisterPatternMatchingFunctions(jerry_value_t globalObj);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif

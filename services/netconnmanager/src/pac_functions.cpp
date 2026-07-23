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

#include <map>
#include <arpa/inet.h>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <ifaddrs.h>
#include <malloc.h>
#include <netdb.h>
#include <string>
#include <unistd.h>

#include "pac_functions.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr size_t ARG_COUNT_1 = 1;
constexpr size_t ARG_COUNT_2 = 2;
constexpr size_t ARG_COUNT_3 = 3;
constexpr size_t ARG_COUNT_4 = 4;
constexpr size_t ARG_COUNT_5 = 5;
constexpr size_t ARG_COUNT_6 = 6;
constexpr size_t ARG_COUNT_7 = 7;
constexpr size_t ARG_COUNT_31 = 31;
constexpr size_t ARG_INDEX_0 = 0;
constexpr size_t ARG_INDEX_1 = 1;
constexpr size_t ARG_INDEX_2 = 2;
constexpr size_t ARG_INDEX_3 = 3;
constexpr size_t ARG_INDEX_4 = 4;
constexpr size_t ARG_INDEX_5 = 5;
constexpr size_t ARG_INDEX_6 = 6;
constexpr int YEAR_BASE = 1900;
constexpr int SECONDS_PER_HOUR = 3600;
constexpr int SECONDS_PER_MINUTE = 60;
constexpr int TEN_THOUSAND = 10000;
constexpr int HUNDRED = 100;
constexpr char SPACE_CHAR = ' ';
constexpr char COLON_CHAR = ':';
constexpr char PATH_CHAR = '/';
constexpr char NULL_CHAR = '\0';
constexpr char SEMICOLON_CHAR = ';';
constexpr char DOT_CHAR = '.';
constexpr char ASTERISK_CHAR = '*';
constexpr char QUESTION_CHAR = '?';
constexpr const char COMMA_SPACE[] = ", ";
constexpr const char GMT[] = "GMT";
constexpr const char DEFAULT_URL[] = "127.0.0.1";
constexpr const char DEFAULT_IPV6_ADDR[] = "::1";
constexpr const char IPV6_LINK_LOCAL_PREFIX[] = "fe80:";
constexpr const char INVALID_ARGUMENT[] = "Invalid argument";
constexpr const char MEMORY_ALLOCATION_FAILED[] = "Memory allocation failed";
constexpr const char *MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
constexpr size_t MONTH_COUNT = std::size(MONTH_NAMES);
constexpr const char *DAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
constexpr size_t DAY_COUNT = std::size(DAY_NAMES);
using Checker = std::function<bool(const jerry_length_t argsCnt, const jerry_value_t args[])>;
using Handler = std::function<jerry_value_t(const jerry_value_t args[], struct tm *timeinfo)>;
} // namespace

static const jerry_char_t *JERRY_CONCHAR(const char *str)
{
    return reinterpret_cast<const jerry_char_t *>(str);
}

static jerry_char_t *JERRY_CHAR(char *str)
{
    return reinterpret_cast<jerry_char_t *>(str);
}

static char *JerryStringToChar(jerry_value_t strVal)
{
    jerry_size_t strSize = jerry_get_string_size(strVal);
    if (strSize > SIZE_MAX - 1) {
        return nullptr;
    }
    char *str = (char *)malloc(strSize + 1);
    if (str) {
        jerry_string_to_char_buffer(strVal, JERRY_CHAR(str), strSize);
        str[strSize] = NULL_CHAR;
    }
    return str;
}

static jerry_value_t CreateJerryString(const char *str)
{
    return jerry_create_string(reinterpret_cast<const jerry_char_t *>(str));
}

jerry_value_t PacFunctions::JsIsPlainHostname(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt < ARG_COUNT_1 || !jerry_value_is_string(args[ARG_INDEX_0])) {
        return jerry_create_boolean(false);
    }
    char *host = JerryStringToChar(args[ARG_INDEX_0]);
    if (host) {
        if (host[0] == NULL_CHAR) {
            free(host);
            return jerry_create_boolean(true);
        }
        bool result = (strchr(host, DOT_CHAR) == nullptr);
        free(host);
        return jerry_create_boolean(result);
    }
    return jerry_create_boolean(false);
}

jerry_value_t PacFunctions::JsDnsDomainIs(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt < ARG_COUNT_2 || !jerry_value_is_string(args[ARG_INDEX_0]) ||
        !jerry_value_is_string(args[ARG_INDEX_1])) {
        return jerry_create_boolean(false);
    }
    char *host = JerryStringToChar(args[ARG_INDEX_0]);
    char *domain = JerryStringToChar(args[ARG_INDEX_1]);
    if (host && domain) {
        if (strlen(domain) == 0) {
            free(host);
            free(domain);
            return jerry_create_boolean(false);
        }
        bool result = false;
        if (strcasecmp(host, domain) == 0) {
            result = true;
        } else if (domain[0] == DOT_CHAR && strcasecmp(host, domain + 1) == 0) {
            result = true;
        } else {
            int hostLen = strlen(host);
            int domainLen = strlen(domain);
            if (hostLen >= domainLen) {
                result = (strcasecmp(host + (hostLen - domainLen), domain) == 0);
            }
        }
        free(host);
        free(domain);
        return jerry_create_boolean(result);
    }
    return jerry_create_boolean(false);
}

jerry_value_t PacFunctions::JsLocalHostOrDomainIs(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt < ARG_COUNT_2 || !jerry_value_is_string(args[ARG_INDEX_0]) ||
        !jerry_value_is_string(args[ARG_INDEX_1])) {
        return jerry_create_boolean(false);
    }
    char *host = JerryStringToChar(args[ARG_INDEX_0]);
    char *hostdom = JerryStringToChar(args[ARG_INDEX_1]);
    if (!host || !hostdom) {
        free(host);
        free(hostdom);
        return jerry_create_boolean(false);
    }
    if (strcasecmp(host, hostdom) == 0) {
        free(host);
        free(hostdom);
        return jerry_create_boolean(true);
    }
    if (strchr(host, DOT_CHAR) == nullptr) {
        char *dot = strchr(hostdom, DOT_CHAR);
        if (dot != nullptr) {
            size_t hostLen = strlen(host);
            size_t prefixLen = dot - hostdom;
            if (hostLen == prefixLen && strncasecmp(host, hostdom, hostLen) == 0) {
                free(host);
                free(hostdom);
                return jerry_create_boolean(true);
            }
        }
    }
    free(host);
    free(hostdom);
    return jerry_create_boolean(false);
}

jerry_value_t PacFunctions::JsIsResolvable(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt < ARG_COUNT_1 || !jerry_value_is_string(args[ARG_INDEX_0])) {
        return jerry_create_boolean(false);
    }
    char *host = JerryStringToChar(args[ARG_INDEX_0]);
    if (host) {
        struct hostent *he = gethostbyname(host);
        bool result = (he != nullptr);
        free(host);
        return jerry_create_boolean(result);
    } else {
        return jerry_create_boolean(false);
    }
}

jerry_value_t PacFunctions::JsMyIpAddress(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        return CreateJerryString(DEFAULT_URL);
    }
    struct hostent *he = gethostbyname(hostname);
    if (!he || he->h_addr_list[0] == nullptr) {
        return CreateJerryString(DEFAULT_URL);
    }
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, he->h_addr_list[0], ip, sizeof(ip));
    return CreateJerryString(ip);
}

jerry_value_t PacFunctions::JsMyIpAddressEx(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    struct ifaddrs *ifaddr;
    struct ifaddrs *ifa;
    std::string ipList;
    int first = 1;
    if (getifaddrs(&ifaddr) == -1) {
        return CreateJerryString(DEFAULT_IPV6_ADDR);
    }
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }
        char ip[INET6_ADDRSTRLEN];
        void *addr;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in *>(ifa->ifa_addr);
            addr = &(ipv4->sin_addr);
            inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN);
            if (strcmp(ip, DEFAULT_URL) == 0) {
                continue;
            }
            if (!first) {
                ipList.append(COMMA_SPACE);
            }
            ipList.append(ip);
            first = 0;
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = reinterpret_cast<sockaddr_in6 *>(ifa->ifa_addr);
            addr = &(ipv6->sin6_addr);
            inet_ntop(AF_INET6, addr, ip, INET6_ADDRSTRLEN);
            if (strcmp(ip, DEFAULT_IPV6_ADDR) == 0 || strncmp(ip, IPV6_LINK_LOCAL_PREFIX, ARG_COUNT_5) == 0) {
                continue;
            }
            if (!first) {
                ipList.append(COMMA_SPACE);
            }
            ipList.append(ip);
            first = 0;
        }
    }
    freeifaddrs(ifaddr);
    if (first) {
        return CreateJerryString(DEFAULT_IPV6_ADDR);
    }
    return CreateJerryString(ipList.c_str());
}

jerry_value_t PacFunctions::JsIsInNet(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt < ARG_COUNT_3) {
        return jerry_create_boolean(false);
    }
    char *ip = JerryStringToChar(args[ARG_INDEX_0]);
    char *net = JerryStringToChar(args[ARG_INDEX_1]);
    char *mask = JerryStringToChar(args[ARG_INDEX_2]);
    if (ip && net && mask) {
        struct in_addr ipAddr;
        struct in_addr netAddr;
        struct in_addr maskAddr;
        bool result = false;
        if (inet_pton(AF_INET, ip, &ipAddr) == 1 && inet_pton(AF_INET, net, &netAddr) == 1 &&
            inet_pton(AF_INET, mask, &maskAddr) == 1) {
            result = ((ipAddr.s_addr & maskAddr.s_addr) == (netAddr.s_addr & maskAddr.s_addr));
        }
        free(ip);
        free(net);
        free(mask);
        return jerry_create_boolean(result);
    } else {
        return jerry_create_boolean(false);
    }
}

jerry_value_t PacFunctions::JsSortIpAddressList(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    return jerry_create_boolean(false);
}

jerry_value_t PacFunctions::JsDnsResolve(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt != ARG_COUNT_1) {
        return jerry_create_error(JERRY_ERROR_TYPE, JERRY_CONCHAR(INVALID_ARGUMENT));
    }
    if (!jerry_value_is_string(args[ARG_INDEX_0])) {
        return jerry_create_error(JERRY_ERROR_TYPE, JERRY_CONCHAR(INVALID_ARGUMENT));
    }
    jerry_size_t host_size = jerry_get_string_size(args[ARG_INDEX_0]);
    if (host_size < 0) {
        return jerry_create_boolean(false);
    }
    char *host = (char *)malloc(host_size + 1);
    if (host == nullptr) {
        return jerry_create_error(JERRY_ERROR_COMMON, JERRY_CONCHAR(MEMORY_ALLOCATION_FAILED));
    }
    jerry_string_to_char_buffer(args[ARG_INDEX_0], JERRY_CHAR(host), host_size);
    host[host_size] = NULL_CHAR;
    struct addrinfo hints;
    explicit_bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *res = nullptr;
    int status = getaddrinfo(host, nullptr, &hints, &res);
    free(host);
    if (status != 0 || res == nullptr) {
        char empty[] = "";
        return jerry_create_string_sz(JERRY_CHAR(empty), 0);
    }
    struct sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in *>(res->ai_addr);
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipv4->sin_addr), ipStr, INET_ADDRSTRLEN);
    freeaddrinfo(res);
    return jerry_create_string_sz(JERRY_CHAR(ipStr), strlen(ipStr));
}

static int MatchPattern(const char *str, const char *pattern)
{
    if (pattern[0] == NULL_CHAR) {
        return str[0] == NULL_CHAR;
    }
    if (*pattern == ASTERISK_CHAR) {
        pattern++;
        for (const char *s = str;; s++) {
            if (MatchPattern(s, pattern)) {
                return true;
            }
            if (*s == NULL_CHAR) {
                return false;
            }
        }
    }
    if (*str == NULL_CHAR) {
        return false;
    }
    if (*pattern == QUESTION_CHAR || *pattern == *str) {
        return MatchPattern(str + 1, pattern + 1);
    }
    return false;
}

jerry_value_t PacFunctions::JsDnsDomainLevels(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt != ARG_COUNT_1) {
        return jerry_create_error(JERRY_ERROR_TYPE, JERRY_CONCHAR(INVALID_ARGUMENT));
    }
    if (!jerry_value_is_string(args[ARG_INDEX_0])) {
        return jerry_create_error(JERRY_ERROR_TYPE, JERRY_CONCHAR(INVALID_ARGUMENT));
    }
    jerry_size_t host_size = jerry_get_string_size(args[ARG_INDEX_0]);
    if (host_size < 0) {
        return jerry_create_boolean(false);
    }
    char *host = (char *)malloc(host_size + 1);
    if (host == nullptr) {
        return jerry_create_error(JERRY_ERROR_COMMON, JERRY_CONCHAR(MEMORY_ALLOCATION_FAILED));
    }
    jerry_string_to_char_buffer(args[ARG_INDEX_0], JERRY_CHAR(host), host_size);
    host[host_size] = NULL_CHAR;
    int levels = 0;
    char *p = host;
    while ((p = strchr(p, DOT_CHAR)) != nullptr) {
        levels++;
        p++;
    }
    free(host);
    return jerry_create_number(levels);
}

jerry_value_t PacFunctions::JsShExpMatch(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt != ARG_COUNT_2) {
        return jerry_create_error(JERRY_ERROR_TYPE, JERRY_CONCHAR(INVALID_ARGUMENT));
    }
    if (!jerry_value_is_string(args[ARG_INDEX_0]) || !jerry_value_is_string(args[ARG_INDEX_1])) {
        return jerry_create_error(JERRY_ERROR_TYPE, JERRY_CONCHAR(INVALID_ARGUMENT));
    }
    jerry_size_t str_size = jerry_get_string_size(args[ARG_INDEX_0]);
    jerry_size_t pattern_size = jerry_get_string_size(args[ARG_INDEX_1]);
    if (str_size < 0) {
        return jerry_create_error(JERRY_ERROR_COMMON, JERRY_CONCHAR(INVALID_ARGUMENT));
    }
    char *str = (char *)malloc(str_size + 1);
    if (str == nullptr) {
        return jerry_create_error(JERRY_ERROR_COMMON, JERRY_CONCHAR(MEMORY_ALLOCATION_FAILED));
    }
    if (pattern_size < 0) {
        free(str);
        return jerry_create_error(JERRY_ERROR_COMMON, JERRY_CONCHAR(INVALID_ARGUMENT));
    }
    char *pattern = (char *)malloc(pattern_size + 1);
    if (pattern == nullptr) {
        free(str);
        return jerry_create_error(JERRY_ERROR_COMMON, JERRY_CONCHAR(MEMORY_ALLOCATION_FAILED));
    }
    jerry_string_to_char_buffer(args[ARG_INDEX_0], JERRY_CHAR(str), str_size);
    jerry_string_to_char_buffer(args[ARG_INDEX_1], JERRY_CHAR(pattern), pattern_size);
    str[str_size] = NULL_CHAR;
    pattern[pattern_size] = NULL_CHAR;
    int result = MatchPattern(str, pattern);
    free(str);
    free(pattern);
    return jerry_create_boolean(result);
}

static int MonthAbbrToNumber(const char *abbr)
{
    if (abbr == nullptr || strlen(abbr) < ARG_COUNT_3) {
        return -1;
    }
    char upperAbbr[ARG_INDEX_4];
    for (int i = 0; i < ARG_COUNT_3; i++) {
        upperAbbr[i] = toupper(abbr[i]);
    }
    upperAbbr[ARG_INDEX_3] = NULL_CHAR;
    for (int i = 0; i < MONTH_COUNT; i++) {
        if (strcmp(upperAbbr, MONTH_NAMES[i]) == 0) {
            return i;
        }
    }
    return -1;
}

static jerry_value_t JsDateRangeArg1Num(const jerry_value_t args[], struct tm *timeinfo)
{
    int num = jerry_get_number_value(args[ARG_INDEX_0]);
    if (num >= ARG_COUNT_1 && num <= ARG_COUNT_31) {
        return jerry_create_boolean(timeinfo->tm_mday == num);
    } else {
        return jerry_create_boolean(timeinfo->tm_year + YEAR_BASE == num);
    }
}

static jerry_value_t JsDateRangeArg61(const jerry_value_t args[], struct tm *timeinfo)
{
    int day0 = jerry_get_number_value(args[ARG_INDEX_0]);
    jerry_size_t mon0_size = jerry_get_string_size(args[ARG_INDEX_1]);
    jerry_char_t mon0_buff[mon0_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_1], mon0_buff, mon0_size);
    mon0_buff[mon0_size] = NULL_CHAR;
    int m0 = MonthAbbrToNumber((const char *)mon0_buff);
    int year0 = jerry_get_number_value(args[ARG_INDEX_2]);
    int day1 = jerry_get_number_value(args[ARG_INDEX_3]);
    jerry_size_t mon1_size = jerry_get_string_size(args[ARG_INDEX_4]);
    jerry_char_t mon1_buff[mon1_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_4], mon1_buff, mon1_size);
    mon1_buff[mon1_size] = NULL_CHAR;
    int m1 = MonthAbbrToNumber((const char *)mon1_buff);
    int year1 = jerry_get_number_value(args[ARG_INDEX_5]);
    int value0 = (year0) * TEN_THOUSAND + (m0 + 1) * HUNDRED + day0;
    int value1 = (year1) * TEN_THOUSAND + (m1 + 1) * HUNDRED + day1;
    int currentValue = (timeinfo->tm_year + YEAR_BASE) * TEN_THOUSAND +
        (timeinfo->tm_mon + 1) * HUNDRED + timeinfo->tm_mday;
    return jerry_create_boolean(currentValue >= value0 && currentValue <= value1);
}

static jerry_value_t JsDateRangeArg42(const jerry_value_t args[], struct tm *timeinfo)
{
    int day0 = 1;
    jerry_size_t mon0_size = jerry_get_string_size(args[ARG_INDEX_0]);
    jerry_char_t mon0_buff[mon0_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_0], mon0_buff, mon0_size);
    mon0_buff[mon0_size] = NULL_CHAR;
    int m0 = MonthAbbrToNumber((const char *)mon0_buff);
    int year0 = jerry_get_number_value(args[ARG_INDEX_1]);
    int day1 = ARG_COUNT_31;
    jerry_size_t mon1_size = jerry_get_string_size(args[ARG_INDEX_2]);
    jerry_char_t mon1_buff[mon1_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_2], mon1_buff, mon1_size);
    mon1_buff[mon1_size] = NULL_CHAR;
    int m1 = MonthAbbrToNumber((const char *)mon1_buff);
    int year1 = jerry_get_number_value(args[ARG_INDEX_3]);
    int value0 = (year0) * TEN_THOUSAND + (m0 + 1) * HUNDRED + day0;
    int value1 = (year1) * TEN_THOUSAND + (m1 + 1) * HUNDRED + day1;
    int currentValue = (timeinfo->tm_year + YEAR_BASE) * TEN_THOUSAND +
        (timeinfo->tm_mon + 1) * HUNDRED + timeinfo->tm_mday;
    return jerry_create_boolean(currentValue >= value0 && currentValue <= value1);
}

static jerry_value_t JsDateRangeArg41(const jerry_value_t args[], struct tm *timeinfo)
{
    int day0 = jerry_get_number_value(args[ARG_INDEX_0]);
    jerry_size_t mon0_size = jerry_get_string_size(args[ARG_INDEX_1]);
    jerry_char_t mon0_buff[mon0_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_1], mon0_buff, mon0_size);
    mon0_buff[mon0_size] = NULL_CHAR;
    int m0 = MonthAbbrToNumber((const char *)mon0_buff);
    int day1 = jerry_get_number_value(args[ARG_INDEX_2]);
    jerry_size_t mon1_size = jerry_get_string_size(args[ARG_INDEX_3]);
    jerry_char_t mon1_buff[mon1_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_3], mon1_buff, mon1_size);
    mon1_buff[mon1_size] = NULL_CHAR;
    int m1 = MonthAbbrToNumber((const char *)mon1_buff);
    int currentValue = (timeinfo->tm_mon + 1) * HUNDRED + timeinfo->tm_mday;
    int value0 = (m0 + 1) * HUNDRED + day0;
    int value1 = (m1 + 1) * HUNDRED + day1;
    return jerry_create_boolean(currentValue >= value0 && currentValue <= value1);
}

static jerry_value_t JsDateRangeArg2StrStr(const jerry_value_t args[], struct tm *timeinfo)
{
    jerry_size_t mon0_size = jerry_get_string_size(args[ARG_INDEX_0]);
    jerry_char_t mon0_buff[mon0_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_0], mon0_buff, mon0_size);
    mon0_buff[mon0_size] = NULL_CHAR;
    jerry_size_t mon1_size = jerry_get_string_size(args[ARG_INDEX_1]);
    jerry_char_t mon1_buff[mon1_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_1], mon1_buff, mon1_size);
    mon1_buff[mon1_size] = NULL_CHAR;
    int m0 = MonthAbbrToNumber((const char *)mon0_buff);
    int m1 = MonthAbbrToNumber((const char *)mon1_buff);
    return jerry_create_boolean(timeinfo->tm_mon >= m0 && timeinfo->tm_mon <= m1);
}

static jerry_value_t JsDateRangeArg2NumNum(const jerry_value_t args[], struct tm *timeinfo)
{
    int num1 = jerry_get_number_value(args[ARG_INDEX_0]);
    int num2 = jerry_get_number_value(args[ARG_INDEX_1]);
    if (num1 > YEAR_BASE && num2 > YEAR_BASE) {
        return jerry_create_boolean(timeinfo->tm_year + YEAR_BASE >= num1 && timeinfo->tm_year + YEAR_BASE <= num2);
    } else {
        return jerry_create_boolean(timeinfo->tm_mday >= num1 && timeinfo->tm_mday <= num2);
    }
}

static jerry_value_t JsDateRangeArg2NumStr(const jerry_value_t args[], struct tm *timeinfo)
{
    int num = jerry_get_number_value(args[ARG_INDEX_0]);
    jerry_size_t str_size = jerry_get_string_size(args[ARG_INDEX_1]);
    jerry_char_t str_buff[str_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_1], str_buff, str_size);
    str_buff[str_size] = NULL_CHAR;
    if (strcmp((const char *)(str_buff), GMT) == 0) {
        time_t rawtimeGmt;
        struct tm *timeinfoGmt;
        time(&rawtimeGmt);
        timeinfoGmt = gmtime(&rawtimeGmt);
        if (timeinfoGmt == nullptr) {
            return jerry_create_boolean(false);
        } else {
            return jerry_create_boolean(timeinfoGmt->tm_mday == num);
        }
    } else {
        int mon = MonthAbbrToNumber((const char *)(str_buff));
        return jerry_create_boolean(mon == timeinfo->tm_mon && num == timeinfo->tm_mday);
    }
}

static std::vector<Checker> jsDateRangeCheckers = {
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt < ARG_COUNT_1 || argsCnt > ARG_COUNT_7;
    },
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt == ARG_COUNT_1 && jerry_value_is_number(args[ARG_INDEX_0]);
    },
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt == ARG_COUNT_2 && jerry_value_is_number(args[ARG_INDEX_0]) &&
               jerry_value_is_string(args[ARG_INDEX_1]);
    },
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt == ARG_COUNT_2 && jerry_value_is_number(args[ARG_INDEX_0]) &&
               jerry_value_is_number(args[ARG_INDEX_1]);
    },
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt == ARG_COUNT_2 && jerry_value_is_string(args[ARG_INDEX_0]) &&
               jerry_value_is_string(args[ARG_INDEX_1]);
    },
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt == ARG_COUNT_4 && jerry_value_is_number(args[ARG_INDEX_0]) &&
               jerry_value_is_string(args[ARG_INDEX_1]) && jerry_value_is_number(args[ARG_INDEX_2]) &&
               jerry_value_is_string(args[ARG_INDEX_3]);
    },
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt == ARG_COUNT_6 && jerry_value_is_number(args[ARG_INDEX_0]) &&
               jerry_value_is_string(args[ARG_INDEX_1]) && jerry_value_is_number(args[ARG_INDEX_2]) &&
               jerry_value_is_number(args[ARG_INDEX_3]) && jerry_value_is_string(args[ARG_INDEX_4]) &&
               jerry_value_is_number(args[ARG_INDEX_5]);
    },
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt == ARG_COUNT_4 && jerry_value_is_string(args[ARG_INDEX_0]) &&
               jerry_value_is_number(args[ARG_INDEX_1]) && jerry_value_is_string(args[ARG_INDEX_2]) &&
               jerry_value_is_number(args[ARG_INDEX_3]);
    },
    [](const jerry_length_t argsCnt, const jerry_value_t args[]) {
        return argsCnt == ARG_COUNT_3 && jerry_value_is_number(args[0]) && jerry_value_is_string(args[1]) &&
               jerry_value_is_number(args[2]);
    },
};

static jerry_value_t JsDateRangeArg3(const jerry_value_t args[], struct tm *timeinfo)
{
    int day = jerry_get_number_value(args[0]);
    jerry_size_t mon_size = jerry_get_string_size(args[1]);
    jerry_char_t mon_buff[mon_size+11];
    jerry_string_to_char_buffer(args[1], mon_buff, mon_size);
    mon_buff[mon_size] = NULL_CHAR;
    int month = MonthAbbrToNumber((const char *)mon_buff);
    int year = jerry_get_number_value(args[2]);
    if (month < 0) {
        return jerry_create_boolean(false);
    }
    return jerry_create_boolean(timeinfo->tm_mday == day && timeinfo->tm_mon == month &&
                                timeinfo->tm_year + YEAR_BASE == year);
}

static std::vector<Handler> jsDateRangeHandler = {
    [](const jerry_value_t args[], struct tm *timeinfo) { return jerry_create_boolean(false); },
    [](const jerry_value_t args[], struct tm *timeinfo) { return JsDateRangeArg1Num(args, timeinfo); },
    [](const jerry_value_t args[], struct tm *timeinfo) { return JsDateRangeArg2NumStr(args, timeinfo); },
    [](const jerry_value_t args[], struct tm *timeinfo) { return JsDateRangeArg2NumNum(args, timeinfo); },
    [](const jerry_value_t args[], struct tm *timeinfo) { return JsDateRangeArg2StrStr(args, timeinfo); },
    [](const jerry_value_t args[], struct tm *timeinfo) { return JsDateRangeArg41(args, timeinfo); },
    [](const jerry_value_t args[], struct tm *timeinfo) { return JsDateRangeArg61(args, timeinfo); },
    [](const jerry_value_t args[], struct tm *timeinfo) { return JsDateRangeArg42(args, timeinfo); },
    [](const jerry_value_t args[], struct tm *timeinfo) { return JsDateRangeArg3(args, timeinfo); },
};

jerry_value_t PacFunctions::JsDateRange(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    time_t rawtime;
    struct tm *timeinfo;
    if (time(&rawtime) == static_cast<time_t>(-1)) {
        return jerry_create_boolean(false);
    }
    int useGmt = 0;
    timeinfo = localtime(&rawtime);
    for (size_t i = 0; i < jsDateRangeCheckers.size(); i++) {
        if (jsDateRangeCheckers[i](argsCnt, args)) {
            return jsDateRangeHandler[i](args, timeinfo);
        }
    }
    return jerry_create_boolean(false);
}

struct TimeParam {
    int h0;
    int m0;
    int s0;
    int h1;
    int m1;
    int s1;
};

static bool IsBetweenTime(TimeParam param, struct tm *timeinfo)
{
    int h = timeinfo->tm_hour;
    int m = timeinfo->tm_min;
    int s = timeinfo->tm_sec;
    int timeSeconds = h * SECONDS_PER_HOUR + m * SECONDS_PER_MINUTE + s;
    int startSeconds = param.h0 * SECONDS_PER_HOUR + param.m0 * SECONDS_PER_MINUTE + param.s0;
    int endSeconds = param.h1 * SECONDS_PER_HOUR + param.m1 * SECONDS_PER_MINUTE + param.s1;
    if (startSeconds <= endSeconds) {
        return (timeSeconds >= startSeconds && timeSeconds <= endSeconds);
    } else {
        return (timeSeconds >= startSeconds || timeSeconds <= endSeconds);
    }
}

static bool ValidateBasicConditions(time_t rawtime, const jerry_length_t argsCnt)
{
    return rawtime != static_cast<time_t>(-1) && argsCnt >= ARG_COUNT_1 && argsCnt <= ARG_COUNT_7;
}

struct TimezoneResult {
    struct tm *timeinfo;
    bool useGMT;
    bool isValid;
};

static TimezoneResult ProcessTimezone(time_t rawtime, const jerry_value_t args[], const jerry_length_t argsCnt)
{
    struct tm *timeinfo = localtime(&rawtime);
    bool useGMT = false;
    if (argsCnt < ARG_COUNT_2 || !jerry_value_is_string(args[ARG_INDEX_1])) {
        return {timeinfo, useGMT, true};
    }
    jerry_size_t str_size = jerry_get_string_size(args[ARG_INDEX_1]);
    jerry_char_t str_buf[str_size+1];
    jerry_string_to_char_buffer(args[ARG_INDEX_1], str_buf, str_size);
    str_buf[str_size]  = NULL_CHAR;
    if (strcmp((char *)str_buf, GMT) == 0) {
        timeinfo = gmtime(&rawtime);
        useGMT = true;
        return {timeinfo, useGMT, true};
    }
    return {nullptr, false, false};
}

static bool ParseSingleHourArg(const jerry_value_t args[], TimeParam &range)
{
    if (!jerry_value_is_number(args[ARG_INDEX_0])) {
        return false;
    }
    int h = jerry_get_number_value(args[ARG_INDEX_0]);
    range = {h, 0, 0, h + 1, 0, 0};
    return true;
}

static bool ParseTwoArgs(const jerry_value_t args[], bool useGMT, TimeParam &range)
{
    if (!jerry_value_is_number(args[ARG_INDEX_0])) {
        return false;
    }
    int h0 = jerry_get_number_value(args[ARG_INDEX_0]);
    int h1;
    if (useGMT) {
        h1 = h0 + 1;
    } else {
        if (!jerry_value_is_number(args[ARG_INDEX_1])) {
            return false;
        }
        h1 = jerry_get_number_value(args[ARG_INDEX_1]);
    }
    range = {h0, 0, 0, h1, 0, 0};
    return true;
}

static bool ParseFourArgs(const jerry_value_t args[], TimeParam &range)
{
    for (int i = 0; i < ARG_COUNT_4; i++) {
        if (!jerry_value_is_number(args[i])) {
            return false;
        }
    }
    range = {jerry_get_number_value(args[ARG_INDEX_0]), jerry_get_number_value(args[ARG_INDEX_1]), 0,
             jerry_get_number_value(args[ARG_INDEX_2]), jerry_get_number_value(args[ARG_INDEX_3]), 0};
    return true;
}

static bool ParseSixArgs(const jerry_value_t args[], TimeParam &range)
{
    for (int i = 0; i < ARG_COUNT_6; i++) {
        if (!jerry_value_is_number(args[i])) {
            return false;
        }
    }
    range = {jerry_get_number_value(args[ARG_INDEX_0]), jerry_get_number_value(args[ARG_INDEX_1]),
             jerry_get_number_value(args[ARG_INDEX_2]), jerry_get_number_value(args[ARG_INDEX_3]),
             jerry_get_number_value(args[ARG_INDEX_4]), jerry_get_number_value(args[ARG_INDEX_5])};
    return true;
}

static bool ParseTimeRangeArgs(const jerry_value_t args[], const jerry_length_t argsCnt, bool useGMT, TimeParam &range)
{
    switch (argsCnt) {
        case ARG_COUNT_1:
            return ParseSingleHourArg(args, range);
        case ARG_COUNT_2:
            return ParseTwoArgs(args, useGMT, range);
        case ARG_COUNT_4:
            return ParseFourArgs(args, range);
        case ARG_COUNT_6:
            return ParseSixArgs(args, range);
        default:
            return false;
    }
}

jerry_value_t PacFunctions::JsTimeRange(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    time_t rawtime;
    if (time(&rawtime) == static_cast<time_t>(-1)) {
        return jerry_create_boolean(false);
    }
    if (!ValidateBasicConditions(rawtime, argsCnt)) {
        return jerry_create_boolean(false);
    }
    TimezoneResult tzResult = ProcessTimezone(rawtime, args, argsCnt);
    if (!tzResult.isValid) {
        return jerry_create_boolean(false);
    }
    TimeParam range = {0, 0, 0, 0, 0, 0};
    if (!ParseTimeRangeArgs(args, argsCnt, tzResult.useGMT, range)) {
        return jerry_create_boolean(false);
    }
    return jerry_create_boolean(IsBetweenTime(range, tzResult.timeinfo));
}

static int ParseDayName(jerry_value_t dayArg)
{
    if (!jerry_value_is_string(dayArg)) {
        return -1;
    }
    jerry_size_t strSize = jerry_get_string_size(dayArg);
    if (strSize < 0) {
        return -1;
    }
    char strBuf[strSize + 1];
    jerry_string_to_char_buffer(dayArg, reinterpret_cast<jerry_char_t *>(strBuf), strSize);
    strBuf[strSize] = NULL_CHAR;
    for (int i = 0; i < DAY_COUNT; i++) {
        if (strcmp(reinterpret_cast<char *>(strBuf), DAY_NAMES[i]) == 0) {
            return i;
        }
    }
    return -1;
}

struct TimeResult {
    struct tm *timeinfo;
    bool valid;
};

static TimeResult GetTimeInfo(bool useGmt)
{
    time_t rawtime;
    if (time(&rawtime) == static_cast<time_t>(-1)) {
        return {nullptr, false};
    }
    struct tm *timeinfo = useGmt ? gmtime(&rawtime) : localtime(&rawtime);
    return {timeinfo, timeinfo != nullptr};
}

static bool ShouldUseGmt(const jerry_value_t args[], jerry_length_t argsCnt)
{
    if (argsCnt <= ARG_COUNT_1) {
        return false;
    }
    jerry_value_t lastArg = args[argsCnt - 1];
    if (!jerry_value_is_string(lastArg)) {
        return false;
    }
    jerry_size_t strSize = jerry_get_string_size(lastArg);
    if (strSize == 0) {
        return false;
    }
    char str_buf[strSize + 1];
    jerry_string_to_char_buffer(lastArg, reinterpret_cast<jerry_char_t *>(str_buf), strSize);
    str_buf[strSize] = NULL_CHAR;
    return strcmp(str_buf, GMT) == 0;
}

struct DayRange {
    int startDay;
    int endDay;
    bool valid;
};

static DayRange ParseDayRange(const jerry_value_t args[], jerry_length_t argsCnt, bool useGmt)
{
    int startDay = ParseDayName(args[ARG_INDEX_0]);
    if (startDay == -1) {
        return {-1, -1, false};
    }
    int endDay = startDay;
    bool hasSecondDay = (argsCnt == ARG_COUNT_2 && !useGmt) || (argsCnt == ARG_COUNT_3 && useGmt);
    if (hasSecondDay) {
        int secondArgIndex = useGmt ? 1 : ARG_INDEX_1;
        endDay = ParseDayName(args[secondArgIndex]);
        if (endDay == -1) {
            return {-1, -1, false};
        }
    }
    return {startDay, endDay, true};
}

static bool IsWeekdayInRange(int currentWeekday, int startDay, int endDay)
{
    return (startDay <= endDay) ? (currentWeekday >= startDay && currentWeekday <= endDay)
                                : (currentWeekday >= startDay || currentWeekday <= endDay);
}

jerry_value_t PacFunctions::JsWeekdayRange(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt < ARG_COUNT_1 || argsCnt > ARG_COUNT_3) {
        return jerry_create_boolean(false);
    }
    bool useGmt = ShouldUseGmt(args, argsCnt);
    TimeResult timeResult = GetTimeInfo(useGmt);
    if (!timeResult.valid) {
        return jerry_create_boolean(false);
    }
    DayRange dayRange = ParseDayRange(args, argsCnt, useGmt);
    if (!dayRange.valid) {
        return jerry_create_boolean(false);
    }
    int currentWeekday = timeResult.timeinfo->tm_wday;
    bool inRange = IsWeekdayInRange(currentWeekday, dayRange.startDay, dayRange.endDay);
    return jerry_create_boolean(inRange);
}

static bool CheckIpv4InNet(const char *ip, const char *cidr, int prefixLen)
{
    const int ipv4Bits = 32;
    struct in_addr ip4;
    struct in_addr net4;
    if (inet_pton(AF_INET, ip, &ip4) != 1 || inet_pton(AF_INET, cidr, &net4) != 1) {
        return false;
    }
    if (prefixLen < 0 || prefixLen > ipv4Bits) {
        return false;
    }
    uint32_t mask;
    if (prefixLen == 0) {
        mask = 0x00000000;
    } else if (prefixLen == ipv4Bits) {
        mask = 0xffffffff;
    } else {
        mask = ~((1 << (ipv4Bits - prefixLen)) - 1);
    }
    return (ip4.s_addr & htonl(mask)) == (net4.s_addr & htonl(mask));
}

static bool CheckIpv6InNet(const char *ip, const char *cidr, int prefixLen)
{
    struct in6_addr ipAddr;
    struct in6_addr netAddr;
    if (inet_pton(AF_INET6, ip, &ipAddr) != 1 || inet_pton(AF_INET6, cidr, &netAddr) != 1) {
        return false;
    }
    int bytes = prefixLen / 8;
    int bits = prefixLen % 8;
    for (int i = 0; i < bytes; i++) {
        if (ipAddr.s6_addr[i] != netAddr.s6_addr[i]) {
            return false;
        }
    }
    if (bits > 0) {
        uint8_t mask = 0xff << (8 - bits);
        return (ipAddr.s6_addr[bytes] & mask) == (netAddr.s6_addr[bytes] & mask);
    }
    return true;
}

jerry_value_t PacFunctions::JsIsInNetEx(const jerry_value_t funcObjVal, const jerry_value_t thisVal,
    const jerry_value_t args[], const jerry_length_t argsCnt)
{
    if (argsCnt < ARG_COUNT_2) {
        return jerry_create_boolean(false);
    }
    char *ip = JerryStringToChar(args[ARG_INDEX_0]);
    char *cidr = JerryStringToChar(args[ARG_INDEX_1]);
    if (!ip || !cidr) {
        free(ip);
        free(cidr);
        return jerry_create_boolean(false);
    }
    char *slash = strchr(cidr, PATH_CHAR);
    if (!slash) {
        free(ip);
        free(cidr);
        return jerry_create_boolean(false);
    }
    *slash = NULL_CHAR;
    int prefixLen = atoi(slash + 1);
    bool isIpv4 = (strchr(ip, COLON_CHAR) == nullptr && strchr(cidr, COLON_CHAR) == nullptr);
    bool result = isIpv4 ? CheckIpv4InNet(ip, cidr, prefixLen) : CheckIpv6InNet(ip, cidr, prefixLen);
    free(ip);
    free(cidr);
    return jerry_create_boolean(result);
}

void PacFunctions::RegisterGlobalFunction(jerry_value_t globalObj,
    const char *funcName, jerry_external_handler_t handler)
{
    jerry_value_t funcNameVal = CreateJerryString(funcName);
    jerry_value_t funcObj = jerry_create_external_function(handler);
    jerry_release_value(jerry_set_property(globalObj, funcNameVal, funcObj));
    jerry_release_value(funcNameVal);
    jerry_release_value(funcObj);
}

void PacFunctions::RegisterHostDomainFunctions(jerry_value_t globalObj)
{
    RegisterGlobalFunction(globalObj, "isPlainHostName", JsIsPlainHostname);
    RegisterGlobalFunction(globalObj, "dnsDomainIs", JsDnsDomainIs);
    RegisterGlobalFunction(globalObj, "localHostOrDomainIs", JsLocalHostOrDomainIs);
    RegisterGlobalFunction(globalObj, "dnsDomainLevels", JsDnsDomainLevels);
}

void PacFunctions::RegisterDnsResolveFunctions(jerry_value_t globalObj)
{
    RegisterGlobalFunction(globalObj, "isResolvable", JsIsResolvable);
    RegisterGlobalFunction(globalObj, "isResolvableEx", JsIsResolvable);
    RegisterGlobalFunction(globalObj, "dnsResolve", JsDnsResolve);
    RegisterGlobalFunction(globalObj, "dnsResolveEx", JsDnsResolve);
    RegisterGlobalFunction(globalObj, "sortIpAddressList", JsSortIpAddressList);
}

void PacFunctions::RegisterIpAddressFunctions(jerry_value_t globalObj)
{
    RegisterGlobalFunction(globalObj, "myIpAddress", JsMyIpAddress);
    RegisterGlobalFunction(globalObj, "myIpAddressEx", JsMyIpAddressEx);
    RegisterGlobalFunction(globalObj, "isInNet", JsIsInNet);
    RegisterGlobalFunction(globalObj, "isInNetEx", JsIsInNetEx);
}

void PacFunctions::RegisterTimeAndDateFunctions(jerry_value_t globalObj)
{
    RegisterGlobalFunction(globalObj, "weekdayRange", JsWeekdayRange);
    RegisterGlobalFunction(globalObj, "timeRange", JsTimeRange);
    RegisterGlobalFunction(globalObj, "dateRange", JsDateRange);
}

void PacFunctions::RegisterPatternMatchingFunctions(jerry_value_t globalObj)
{
    RegisterGlobalFunction(globalObj, "shExpMatch", JsShExpMatch);
}

void PacFunctions::RegisterPacFunctions(void)
{
    jerry_value_t globalObj = jerry_get_global_object();
    RegisterHostDomainFunctions(globalObj);
    RegisterDnsResolveFunctions(globalObj);
    RegisterIpAddressFunctions(globalObj);
    RegisterTimeAndDateFunctions(globalObj);
    RegisterPatternMatchingFunctions(globalObj);
    jerry_release_value(globalObj);
}
} // namespace NetManagerStandard
} // namespace OHOS

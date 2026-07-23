/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATION_NETMANAGER_BASE_DNS_CONFIG_CLIENT_H
#define COMMUNICATION_NETMANAGER_BASE_DNS_CONFIG_CLIENT_H

#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>

#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SERVER_NUM 5
#define MAX_SERVER_NUM_EXT 8
#define MAX_SERVER_LENGTH 50
#define DNS_SOCKET_PATH "/dev/unix/socket/dnsproxyd"
#define DNS_SOCKET_NAME "dnsproxyd"
#define MAX_RESULTS 32
#define MAX_CANON_NAME 256
#define MAX_HOST_NAME_LEN 256
#define DEFAULT_TIMEOUT 5000
#define DEFAULT_RETRY 2
#define DEFAULT_SERVER_LENTH 16
#define DEFAULT_SERVER_NAME 114

#define MAX_DNS_CACHE_SIZE 50
#define MIN_DNS_REPORT_PERIOD 10 // 10s
#define LOOP_BACK_ADDR1 "127.0.0.1"
#define LOOP_BACK_ADDR2 "0.0.0.0"
#define MIN_REPORT_INTERVAL 1
#define MIN_QUERY_REPORT_INTERVAL 5 // MIN STATISTIC DNS REPORT INTERVAL
#define FIRST_RETURN_SLOW_THRESHOLD 500
#define QUERY_CALLBACK_RETURN_SLOW_THRESHOLD 1500
#define FAIL_CAUSE_REPORT_INTERVAL (15 * 60)
#define MIN_APP_UID 100000
#define UID_PUSH 7023
#define UID_ACCOUNT 7008
#define TOTAL_FAIL_CAUSE_COUNT 8
#define FAIL_CAUSE_NONE 0
#define FAIL_CAUSE_QUERY_FAIL 1
#define FAIL_CAUSE_FIRST_RETURN_SLOW 2
#define FAIL_CAUSE_CALLBACK_RETURN_SLOW 3
#define FAIL_CAUSE_USE_BACKUP_DNS_SERVER 4
#define FAIL_CAUSE_RETURN_LOOPBACK_ADDR 5
#define FAIL_CAUSE_RETURN_CNAME 6
#define FAIL_CAUSE_RETURN_NO_ANSWER 7
#define FAIL_CAUSE_INTERFACE_NOT_DEFAULT 8
#define DEFAULT_DELAYED_COUNT (60 * 10)
#define DEFAULT_AAAA_BLACK (30 * 60 * 1000)
#define AAAA_SKIP_DURATION (6 * 60 * 60 * 1000)
#define MAX_ALL_CACHE_SIZE (200 * 1024 * 1024)

enum CommandType {
    GET_CONFIG = 1,
    GET_CACHE = 2,
    SET_CACHE = 3,
    JUDGE_IPV6 = 4,
    POST_DNS_RESULT = 5,
    GET_DEFAULT_NETWORK = 6,
    BIND_SOCKET = 7,
    POST_DNS_QUERY_RESULT = 8,    // for musl and c-ares
    POST_DNS_ABNORMAL_RESULT = 9, // for musl and c-ares
    GET_CONFIG_EXT = 10, // for musl and c-ares
    JUDGE_IPV4 = 11,
    SET_NODATA_CACHE = 12, // for musl to set AAAA NODATA cache
    GET_NODATA_CACHE = 13, // for musl to get AAAA NODATA cache status
};

struct RequestInfo {
    uint32_t uid;
    uint32_t command;
    uint32_t netId;
};

struct ResolvConfig {
    int32_t error;
    int32_t timeoutMs;
    uint32_t retryCount;
    uint32_t nonPublicNum;
    char nameservers[MAX_SERVER_NUM][MAX_SERVER_LENGTH + 1];
};

struct ResolvConfigExt {
    int32_t error;
    int32_t timeoutMs;
    uint32_t retryCount;
    uint32_t nonPublicNum;
    char nameservers[MAX_SERVER_NUM_EXT][MAX_SERVER_LENGTH + 1];
};

typedef union {
    struct sockaddr sa;
    struct sockaddr_in6 sin6;
    struct sockaddr_in sin;
} AlignedSockAddr;

struct AddrInfo {
    uint32_t aiFlags;
    uint32_t aiFamily;
    uint32_t aiSockType;
    uint32_t aiProtocol;
    uint32_t aiAddrLen;
    AlignedSockAddr aiAddr;
    char aiCanonName[MAX_CANON_NAME + 1];
};

struct DnsServerInfo {
    int queryProtocol;
    uint32_t aiFamily;
    char addr[MAX_SERVER_LENGTH + 1];
};

struct DnsAns {
    struct addrinfo *ai;
    uint32_t ttl;
};

struct AddrInfoWithTtl {
    struct AddrInfo addrInfo;
    uint32_t ttl;
};

struct ParamWrapper {
    char *host;
    char *serv;
    struct addrinfo *hint;
};

typedef int32_t (*FuncNetDnsqueryHook)(int32_t, int32_t, int32_t);

struct QueryParam {
    int32_t type;
    int32_t netId;
    int32_t mark;
    int32_t flags;
    int64_t queryTime;
    FuncNetDnsqueryHook qHook;
};

struct DnsResultPollParam {
    int sockFd;
    int queryRet;
    int32_t resNum;
    int32_t dnsServerNum;
    struct QueryParam *param;
};

struct FamilyQueryInfo {
    int32_t retCode;
    char *serverAddr;
    uint8_t isNoAnswer;
    uint8_t cname;
};

struct FamilyQueryInfoExt {
    int32_t retCode;
    char serverAddr[MAX_SERVER_LENGTH + 1];
    uint8_t isNoAnswer;
    uint8_t cname;
};

struct DnsProcessInfo {
    long long queryTime;
    char *hostname;
    int32_t retCode;
    uint32_t firstQueryEndDuration;
    uint32_t firstQueryEnd2AppDuration;
    uint16_t firstReturnType; /* a or aaaa */
    uint8_t isFromCache;
    uint8_t sourceFrom;
    struct FamilyQueryInfo ipv4QueryInfo;
    struct FamilyQueryInfo ipv6QueryInfo;
};

struct DnsProcessInfoExt {
    long long queryTime;
    char hostname[MAX_HOST_NAME_LEN + 1];
    char srcAddr[MAX_SERVER_LENGTH + 1];
    int32_t retCode;
    uint32_t firstQueryEndDuration;
    uint32_t firstQueryEnd2AppDuration;
    uint16_t firstReturnType; /* a or aaaa */
    uint8_t isFromCache;
    uint8_t sourceFrom;
    struct FamilyQueryInfoExt ipv4QueryInfo;
    struct FamilyQueryInfoExt ipv6QueryInfo;
};

struct PostDnsQueryParam {
    uint32_t netId;
    uint32_t uid;
    uint32_t pid;
    uint8_t addrSize;
    struct DnsProcessInfoExt processInfo;
};

struct DnsCacheInfo {
    uint8_t addrSize;
    struct DnsProcessInfoExt dnsProcessInfo;
    struct AddrInfo addrInfo[MAX_RESULTS];
};
#ifdef __cplusplus
}
#endif
#endif // COMMUNICATION_NETMANAGER_BASE_1_DNS_CONFIG_CLIENT_H

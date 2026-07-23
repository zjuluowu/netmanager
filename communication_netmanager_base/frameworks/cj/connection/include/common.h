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

#ifndef NET_CONNECTION_COMMON_H
#define NET_CONNECTION_COMMON_H

#include <cstdint>

#ifdef __cplusplus
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_START
#define EXTERN_C_END
#endif

EXTERN_C_START
struct CNetCapabilities {
    int64_t bearedTypeSize;
    int64_t networkCapSize;
    uint32_t linkUpBandwidthKbps;
    uint32_t linkDownBandwidthKbps;
    int32_t *bearerTypes;
    int32_t *networkCap;
};

struct CNetAddress {
    char *address;
    uint32_t family;
    uint16_t port;
};

struct RetNetAddressArr {
    int32_t code;
    int64_t size;
    CNetAddress *data;
};

struct CLinkAddress {
    CNetAddress address;
    int32_t prefixLength;
};

struct CRouteInfo {
    char *interfaceName;
    CLinkAddress destination;
    CNetAddress gateway;
    bool hasGateway;
    bool isDefaultRoute;
};

struct CConnectionProperties {
    char *interfaceName;
    char *domains;
    int64_t linkAddressSize;
    int64_t dnsSize;
    int64_t routeSize;
    uint16_t mtu;
    CLinkAddress *linkAddresses;
    CNetAddress *dnses;
    CRouteInfo *routes;
};

struct CNetSpecifier {
    CNetCapabilities netCapabilities;
    char *bearerPrivateIdentifier;
    bool hasSpecifier;
};

struct CHttpProxy {
    char *host;
    uint16_t port;
    char **exclusionList;
    int64_t exclusionListSize;
};

struct CNetCapabilityInfo {
    int32_t netHandle;
    CNetCapabilities netCap;
};
EXTERN_C_END

#endif
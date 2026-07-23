/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#ifndef POLICY_ANI_H
#define POLICY_ANI_H

#include "cxx.h"
#include "net_policy_client.h"

namespace OHOS {
namespace NetManagerAni {

// Forward declarations for ANI runtime opaque types (provided by ability runtime)
typedef struct AniEnv AniEnv;
typedef struct AniObject AniObject;

struct NetAccessPolicyInner;
struct NetworkAccessPolicyAni;
struct UidNetworkPolicyAni;
struct NetQuotaPolicyAni;

NetAccessPolicyInner GetSelfNetworkAccessPolicy(int32_t &ret);
rust::String GetErrorCodeAndMessage(int32_t &errorCode);

// Register/unregister callbacks for net quota policy changes.
int32_t RegisterNetQuotaPolicyChangeCallback();
int32_t UnregisterNetQuotaPolicyChangeCallback();
int32_t ShowAppNetPolicySettings(int64_t context);
int32_t SetBackgroundAllowed(bool allowed);
int32_t RestoreAllPolicies(rust::String iccid);
int32_t SetPowerTrustlist(rust::Vec<uint32_t> ids, bool allowed);
int32_t UpdateRemindPolicy(int32_t netType, rust::String iccid, uint32_t remindType);
int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy);
int32_t GetBackgroundPolicy(bool &backgroundPolicy);
int32_t GetPowerSaveTrustlist(rust::Vec<uint32_t> &result);
int32_t GetDeviceIdleTrustlist(rust::Vec<uint32_t> &result);
int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed);
int32_t IsUidNetAllowedByIface(uint32_t uid, rust::String iface, bool &isAllowed);
int32_t SetPolicyByUid(uint32_t uid, uint32_t policy);
int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid);
int32_t SetDeviceIdleTrustlist(rust::Vec<uint32_t> uids, bool isAllowed);
int32_t GetUidsByPolicy(uint32_t policy, rust::Vec<uint32_t> &result);
int32_t GetNetworkAccessPolicyByUid(uint32_t uid, NetworkAccessPolicyAni &result);
int32_t GetAllNetworkAccessPolicies(rust::Vec<UidNetworkPolicyAni> &result);
int32_t GetNetQuotaPoliciesTyped(rust::Vec<NetQuotaPolicyAni> &result);
int32_t SetNetworkAccessPolicyTyped(uint32_t uid, NetworkAccessPolicyAni policy, bool reconfirmFlag);
int32_t SetNetQuotaPolicies(rust::Vec<NetQuotaPolicyAni> policies);

} // namespace NetManagerAni
} // namespace OHOS

#endif // POLICY_ANI_H

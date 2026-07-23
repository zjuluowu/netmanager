/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef NET_STATISTICS_ANI_H
#define NET_STATISTICS_ANI_H

#include "cxx.h"
#include "net_stats_callback_stub.h"
#include "net_stats_client.h"
#include "refbase.h"

namespace OHOS {
namespace NetManagerAni {
struct NetStatsChangeInfo;
struct StatisticsCallback;
struct NetStatsInfoInner;
struct AniNetworkInfo;
struct AniUidNetStatsInfoPair;
struct AniNetStatsInfoSequenceItem;
struct IfaceInfo;
struct UidInfo;

NetManagerStandard::NetStatsClient &GetNetStatsClient(int32_t &nouse);
rust::String GetErrorCodeAndMessage(int32_t &errorCode);

class StatisEventCallbackObserverAni : public NetManagerStandard::NetStatsCallbackStub {
public:
    int32_t NetIfaceStatsChanged(const std::string &iface);
    int32_t NetUidStatsChanged(const std::string &iface, uint32_t uid);
};

int32_t RegisterNetStatisObserver();
int32_t UnRegisterNetStatisObserver();

NetStatsInfoInner GetTrafficStatsByIface(IfaceInfo &info, int32_t &ret);
NetStatsInfoInner GetTrafficStatsByUid(UidInfo &info, int32_t &ret);
int32_t GetTrafficStatsByNetworkVec(AniNetworkInfo &networkInfo, rust::Vec<AniUidNetStatsInfoPair> &netStatsInfos);
int32_t GetTrafficStatsByUidNetworkVec(rust::Vec<AniNetStatsInfoSequenceItem> &netStatsInfosSequence, uint32_t uid,
                                       AniNetworkInfo &networkInfo);

uint32_t GetCallingUid();
int32_t UpdateIfacesStatsCxx(const std::string &iface, uint64_t start, uint64_t end, const NetStatsInfoInner &stats);
} // namespace NetManagerAni
} // namespace OHOS

#endif // NET_STATISTICS_ANI_H
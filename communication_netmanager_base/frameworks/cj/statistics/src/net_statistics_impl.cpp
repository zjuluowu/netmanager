/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "net_statistics_impl.h"
#include "net_stats_client.h"
#include "netmanager_base_log.h"

namespace OHOS::NetManagerStandard {

int32_t NetStatisticsImpl::GetUidRxBytes(uint64_t &stats, uint32_t uid)
{
    return NetStatsClient::GetInstance().GetUidRxBytes(stats, uid);
}

int32_t NetStatisticsImpl::GetUidTxBytes(uint64_t &stats, uint32_t uid)
{
    return NetStatsClient::GetInstance().GetUidTxBytes(stats, uid);
}

} // namespace OHOS::NetManagerStandard

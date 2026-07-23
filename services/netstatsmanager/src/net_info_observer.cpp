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
 
#include <net/if.h>
#include "net_info_observer.h"
#include <unistd.h>
#include "net_conn_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_cached.h"
#include "net_stats_utils.h"
#include "net_stats_service.h"
#include "ffrt_inner.h"
 
namespace OHOS {
namespace NetManagerStandard {
 
int32_t NetInfoObserver::NetCapabilitiesChange(sptr<NetManagerStandard::NetHandle> &netHandle,
    const sptr<NetManagerStandard::NetAllCapabilities> &netAllCap)
{
    return 0;
}
 
int32_t NetInfoObserver::NetAvailable(sptr<NetManagerStandard::NetHandle> &netHandle)
{
    return 0;
}
 
int32_t NetInfoObserver::NetLost(sptr<NetManagerStandard::NetHandle> &netHandle)
{
    return 0;
}
 
int32_t NetInfoObserver::NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info)
{
    if (info == nullptr) {
        return -1;
    }
    NETMGR_LOG_D("NetInfoObserver NetConnectionPropertiesChange ifName: %{public}s, idnet: %{public}s",
        info->ifaceName_.c_str(), info->ident_.c_str());
    
    if (info->ident_ == ident_) {
        NETMGR_LOG_D("ident no changed");
        return 0;
    }
    ident_ = info->ident_;
    auto simId = info->ident_;
    auto task = [simId]() {
        NetStatsService::GetInstance()->ProcessDefaultSimIdChanged(simId);
    };
    ffrt::submit(std::move(task), {}, {}, ffrt::task_attr().name("ProcessDefaultSimIdChanged"));
    return 0;
}
 
} // namespace NetManagerStandard
} // namespace OHOS
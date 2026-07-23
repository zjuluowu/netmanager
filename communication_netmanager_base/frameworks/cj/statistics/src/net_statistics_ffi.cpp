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

#include <cstdint>
#include "common.h"
#include "net_statistics_impl.h"
#include "net_manager_constants.h"
#include "netmanager_base_log.h"

namespace OHOS::NetManagerStandard {

EXTERN_C_START
FFI_EXPORT RetDataI64 FfiNetStatisticsGetUidRxBytes(uint32_t uid)
{
    RetDataI64 ret = {.code = NETMANAGER_ERROR, .data = 0};
    NetStatisticsImpl impl;
    uint64_t stats = 0;
    int32_t result = impl.GetUidRxBytes(stats, uid);
    if (result == NETMANAGER_SUCCESS) {
        ret.code = NETMANAGER_SUCCESS;
        ret.data = static_cast<int64_t>(stats);
    } else {
        ret.code = result;
        NETMANAGER_BASE_LOGE("FfiNetStatisticsGetUidRxBytes failed, uid=%{public}u, result=%{public}d", uid, result);
    }
    return ret;
}

FFI_EXPORT RetDataI64 FfiNetStatisticsGetUidTxBytes(uint32_t uid)
{
    RetDataI64 ret = {.code = NETMANAGER_ERROR, .data = 0};
    NetStatisticsImpl impl;
    uint64_t stats = 0;
    int32_t result = impl.GetUidTxBytes(stats, uid);
    if (result == NETMANAGER_SUCCESS) {
        ret.code = NETMANAGER_SUCCESS;
        ret.data = static_cast<int64_t>(stats);
    } else {
        ret.code = result;
        NETMANAGER_BASE_LOGE("FfiNetStatisticsGetUidTxBytes failed, uid=%{public}u, result=%{public}d", uid, result);
    }
    return ret;
}
EXTERN_C_END

} // namespace OHOS::NetManagerStandard

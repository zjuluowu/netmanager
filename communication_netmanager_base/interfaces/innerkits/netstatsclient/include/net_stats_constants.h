/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_CONSTANTS_H
#define NET_STATS_CONSTANTS_H

#include "net_manager_constants.h"
#include <limits>

namespace OHOS {
namespace NetManagerStandard {
constexpr int16_t LIMIT_STATS_CALLBACK_NUM = 200;
constexpr uint32_t Sim_UID = std::numeric_limits<uint32_t>::max();
constexpr uint32_t UNINSTALLED_UID = std::numeric_limits<uint32_t>::max() - 1;
constexpr uint32_t SIM2_UID = std::numeric_limits<uint32_t>::max() - 2;
constexpr uint32_t IPTABLES_UID = std::numeric_limits<uint32_t>::max() - 3;
constexpr uint32_t DEFAULT_ACCOUNT_UID = std::numeric_limits<uint32_t>::max() - 4;
constexpr uint32_t OTHER_ACCOUNT_UID = std::numeric_limits<uint32_t>::max() - 5;
constexpr uint32_t CALIBRATE_UID = std::numeric_limits<uint32_t>::max() - 6;
constexpr uint32_t USER_ID_DIVIDOR = 200000;
constexpr uint32_t USER_ID_DIVIDOR_SIM = 100000;
constexpr int32_t SYSTEM_DEFAULT_USERID = 0;
constexpr int32_t SIM_PRIVATE_USERID = 124;
enum NetStatsResultCode {
    STATS_DUMP_MESSAGE_FAIL = 2103002,
    STATS_REMOVE_FILE_FAIL,
    STATS_ERR_INVALID_TIME_PERIOD,
    STATS_ERR_READ_BPF_FAIL,
    STATS_ERR_INVALID_KEY,
    STATS_ERR_INVALID_IFACE_STATS_MAP,
    STATS_ERR_INVALID_STATS_VALUE,
    STATS_ERR_INVALID_STATS_TYPE,
    STATS_ERR_INVALID_UID_STATS_MAP,
    STATS_ERR_INVALID_GET_BPF_MAP,
    STATS_ERR_GET_IFACE_NAME_FAILED,
    STATS_ERR_CLEAR_STATS_DATA_FAIL,
    STATS_ERR_CREATE_TABLE_FAIL,
    STATS_ERR_DATABASE_RECV_NO_DATA,
    STATS_ERR_WRITE_DATA_FAIL,
    STATS_ERR_READ_DATA_FAIL,
    STATS_ERR_WRITE_BPF_FAIL,
    STATS_ERR_TIMESTAMP_ERROR,
};

enum NetStatsDataFlag {
    STATS_DATA_FLAG_DEFAULT,        // The minimum value of DataFlag, Do not less than the limit. No actual meaning.
    STATS_DATA_FLAG_UNINSTALLED,    // the stats of the uninstalled uid.
    STATS_DATA_FLAG_SIM2,           // the stats of the sim2 (hap)uid.
    STATS_DATA_FLAG_SIM,            // the stats of the sim (hap)uid.
    STATS_DATA_FLAG_SIM_BASIC,      // the stats of the sim (basic)uid.
    STATS_DATA_FLAG_SIM2_BASIC,     // the stats of the sim2 (basic)uid.
    STATS_DATA_FLAG_LIMIT,          // The maximum value of DataFlag, Do not exceed the limit. No actual meaning.
};

enum NetStatsNotificationFlag {
    NET_STATS_MONTHLY_LIMIT = 0,
    NET_STATS_MONTHLY_MARK,
    NET_STATS_DAILY_MARK,
    NET_STATS_NO_LIMIT_ENABLE,
    NET_STATS_NOTIFY_TYPE,
    NET_STATS_BEGIN_DATE,
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_CONSTANTS_H

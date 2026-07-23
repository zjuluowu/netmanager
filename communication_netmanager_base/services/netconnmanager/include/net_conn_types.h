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

#ifndef NET_CONN_TYPES_H
#define NET_CONN_TYPES_H

namespace OHOS {
namespace NetManagerStandard {
constexpr int32_t MAX_IPV4_ADDRESS_LEN = 32;

constexpr int32_t STATE_ENTER_FORCESLEEP = 0x30;
constexpr int32_t STATE_EXIT_FORCESLEEP = 0x31;
constexpr int32_t STATE_ENTER_SLEEP_NOT_FORCE = 0x40;
constexpr int32_t STATE_EXIT_SLEEP_NOT_FORCE = 0x41;

enum NetMonitorResponseCode {
    OK = 200,
    CREATED = 201,
    NO_CONTENT = 204,
    URL_REDIRECT_MAX = 399,
    BAD_REQUEST = 400,
    CLIENT_ERROR_MAX = 499
};

enum NetDetectionStatus {
    UNKNOWN_STATE,
    INVALID_DETECTION_STATE,
    VERIFICATION_STATE,
    CAPTIVE_PORTAL_STATE,
    QUALITY_POOR_STATE,
    QUALITY_NORMAL_STATE,
    QUALITY_GOOD_STATE,
    ACCEPT_UNVALIDATED
};
using NetDetectionStateHandler = std::function<void(NetDetectionStatus netDetectionState,
    const std::string &urlRedirect)>;
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CONN_TYPES_H

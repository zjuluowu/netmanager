/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "net_http_probe_result.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int32_t SUCCESS_CODE = 204;
constexpr int32_t PORTAL_CODE_MIN = 200;
constexpr int32_t PORTAL_CODE_MAX = 399;
} // namespace
NetHttpProbeResult::NetHttpProbeResult(int32_t code, const std::string &redirectUrl)
    : responseCode_(code), redirectUrl_(redirectUrl)
{
}

int32_t NetHttpProbeResult::GetCode() const
{
    return responseCode_;
}

std::string NetHttpProbeResult::GetRedirectUrl() const
{
    return redirectUrl_;
}

bool NetHttpProbeResult::IsSuccessful() const
{
    return (responseCode_ == SUCCESS_CODE);
}

bool NetHttpProbeResult::IsNeedPortal() const
{
    return (responseCode_ >= PORTAL_CODE_MIN && responseCode_ <= PORTAL_CODE_MAX) && (responseCode_ != SUCCESS_CODE);
}

bool NetHttpProbeResult::IsFailed() const
{
    return !IsSuccessful() && !IsNeedPortal();
}

bool NetHttpProbeResult::operator==(const NetHttpProbeResult &result) const
{
    if (IsSuccessful() && result.IsSuccessful()) {
        return true;
    }
    if (IsNeedPortal() && result.IsNeedPortal() && (redirectUrl_ == result.redirectUrl_)) {
        return true;
    }
    if (IsFailed() && result.IsFailed()) {
        return true;
    }
    return false;
}

bool NetHttpProbeResult::operator!=(const NetHttpProbeResult &result) const
{
    return !(*this == result);
}

NetHttpProbeResult &NetHttpProbeResult::operator=(const NetHttpProbeResult &result)
{
    responseCode_ = result.responseCode_;
    redirectUrl_ = result.redirectUrl_;
    return *this;
}
} // namespace NetManagerStandard
} // namespace OHOS

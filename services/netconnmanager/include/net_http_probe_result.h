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

#ifndef NET_HTTP_PROBE_RESULT_H
#define NET_HTTP_PROBE_RESULT_H

#include <string>

namespace OHOS {
namespace NetManagerStandard {
class NetHttpProbeResult {
public:
    NetHttpProbeResult() = default;
    ~NetHttpProbeResult() = default;
    NetHttpProbeResult(int32_t code, const std::string &redirectUrl);

    int32_t GetCode() const;
    std::string GetRedirectUrl() const;
    bool IsSuccessful() const;
    bool IsNeedPortal() const;
    bool IsFailed() const;

    bool operator==(const NetHttpProbeResult &result) const;
    bool operator!=(const NetHttpProbeResult &result) const;
    NetHttpProbeResult &operator=(const NetHttpProbeResult &result);

private:
    int32_t responseCode_ = 0;
    std::string redirectUrl_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_HTTP_PROBE_RESULT_H
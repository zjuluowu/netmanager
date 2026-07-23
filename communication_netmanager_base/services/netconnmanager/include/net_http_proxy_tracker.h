/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef NET_HTTP_PROXY_TRACKER_H
#define NET_HTTP_PROXY_TRACKER_H
#include "net_proxy_userinfo.h"
#include "http_proxy.h"
#include "uri.h"

namespace OHOS {
namespace NetManagerStandard {
class NetHttpProxyTracker {
public:
    NetHttpProxyTracker() = default;
    ~NetHttpProxyTracker() = default;

    void ReadFromSettingsData(HttpProxy &httpProxy);
    bool WriteToSettingsData(HttpProxy &httpProxy);

    void ReadFromSettingsDataUser(HttpProxy &httpProxy, int32_t userId);
    bool WriteToSettingsDataUser(HttpProxy &httpProxy, int32_t userId);

private:
    struct KeyUri {
        Uri hostUri_;
        Uri portUri_;
        Uri exclusionsUri_;
    };

private:
    std::list<std::string> ParseExclusionList(const std::string &exclusions) const;
    std::string GetExclusionsAsString(const std::list<std::string> &exclusionList) const;
    void ReadFromSettingsData(HttpProxy &httpProxy, KeyUri keyUri);
    bool WriteToSettingsData(HttpProxy &httpProxy, KeyUri keyUri);
    std::string ReplaceUserIdForUri(const char* uri, int32_t userId);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_HTTP_PROXY_TRACKER_H
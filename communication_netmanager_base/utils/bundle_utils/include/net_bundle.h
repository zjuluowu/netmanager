/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef NET_BUNDLE__H
#define NET_BUNDLE__H

#include <string>
#include <optional>

namespace OHOS {
namespace NetManagerStandard {
struct SampleBundleInfo {
public:
    inline bool Valid() const
    {
        return uid_ > 0 && !bundleName_.empty();
    }
    std::string ToString() const
    {
        std::string s;
        s.append(std::to_string(uid_));
        s.append(",");
        s.append(bundleName_);
        s.append(",");
        s.append(installSource_);
        s.append(",");
        s.append(std::to_string(installTime_));
        return s;
    }

public:
    uint32_t uid_ = 0;
    std::string bundleName_;
    std::string installSource_;
    int64_t installTime_ = -1;
};

class INetBundle {
public:
    virtual int32_t GetJsonFromBundle(std::string &jsonProfile) = 0;
    virtual bool IsAtomicService(std::string &bundleName) = 0;
    virtual std::optional<int32_t> ObtainTargetApiVersionForSelf() = 0;
    virtual std::optional<std::string> ObtainBundleNameForSelf() = 0;
    virtual std::optional<std::unordered_map<uint32_t, SampleBundleInfo>> ObtainBundleInfoForActive() = 0;
    virtual std::optional<SampleBundleInfo> ObtainBundleInfoForUid(uint32_t uid) = 0;
};
extern "C" INetBundle *GetNetBundle();
extern "C" bool IsAtomicService(std::string &bundleName);
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_BUNDLE__H
/*
 * Copyright (C) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef NET_DATASHARE_UTILS_H
#define NET_DATASHARE_UTILS_H

#include <functional>
#include <map>
#include <memory>
#include <utility>

#include "datashare_helper.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_values_bucket.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "uri.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *GLOBAL_PROXY_HOST_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=global_proxy_host";
} // namespace

class NetDataShareHelperUtils final {
public:
    NetDataShareHelperUtils();
    ~NetDataShareHelperUtils() = default;
    int32_t Query(Uri &uri, const std::string &key, std::string &value);
    int32_t Insert(Uri &uri, const std::string &key, const std::string &value);
    int32_t Update(Uri &uri, const std::string &key, const std::string &value);
    int32_t Delete(Uri &uri, const std::string &key);
    int32_t RegisterObserver(const Uri &uri, const std::function<void()> &onChange);
    int32_t UnregisterObserver(const Uri &uri, int32_t callbackId);
    int32_t UnRegisterSettingsObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);
    int32_t RegisterSettingsObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);

private:
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper();
    std::map<int32_t, sptr<AAFwk::IDataAbilityObserver>> callbacks_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_DATASHARE_UTILS_H
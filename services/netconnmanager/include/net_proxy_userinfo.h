/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef NETMANAGER_BASE_NET_PROXY_USERINFO_H
#define NETMANAGER_BASE_NET_PROXY_USERINFO_H

#include <string>

#include "http_proxy.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store.h"
#include "rdb_store_config.h"
#include "rdb_types.h"

namespace OHOS {
namespace NetManagerStandard {
class DataBaseRdbOpenCallBack : public NativeRdb::RdbOpenCallback {
public:
    int32_t OnCreate(NativeRdb::RdbStore &rdbStore) override;

    int32_t OnOpen(NativeRdb::RdbStore &rdbStore) override;

    int32_t OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t currentVersion, int32_t targetVersion) override;
};

class NetProxyUserinfo {
public:
    ~NetProxyUserinfo() = default;

    NetProxyUserinfo();

    void SaveHttpProxyHostPass(const HttpProxy &httpProxy);

    void GetHttpProxyHostPass(HttpProxy &httpProxy);

    static NetProxyUserinfo &GetInstance();

    NetProxyUserinfo(const NetProxyUserinfo &other) = default;

private:
    NetProxyUserinfo &operator=(const NetProxyUserinfo &) = delete;

    std::shared_ptr<NativeRdb::RdbStore> rdbStore_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_BASE_NET_PROXY_USERINFO_H
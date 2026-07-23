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

#include "net_proxy_userinfo.h"

#include <cinttypes>
#include <securec.h>
#include <unistd.h>

#include "net_mgr_log_wrapper.h"
#include "rdb_common.h"
#include "net_manager_constants.h"

using namespace OHOS::NativeRdb;
using namespace OHOS::NetManagerStandard;

namespace {
static const int32_t RDB_VERSION = 0;
static const std::string NET_CONN_PROXY_DATABASE_FILE = "net_conn_proxy.db";

static const std::string NETCONNPROXY_TABLE_NAME = "net_userinfo";
static const std::string NETCONNPROXY_PRIM_KEY_COL = "proxypasswd";
static const std::string NETCONNPROXY_HOST_COL = "host";
static const std::string NETCONNPROXY_PASS_COL = "pass";
static const std::string NETCONNPROXY_PRIMARY_KEY = "ProxyPasswd";
static const std::string NETCONNPROXY_BUNDLENAME = "net_conn_permission";

static const std::string CREATE_TABLE = CREATE_TABLE_IF_NOT_EXISTS + NETCONNPROXY_TABLE_NAME + " (" +
                                        NETCONNPROXY_PRIM_KEY_COL + " TEXT PRIMARY KEY, " + NETCONNPROXY_HOST_COL +
                                        " TEXT, " + NETCONNPROXY_PASS_COL + " TEXT);";

const std::string NETMGR_BASE_PATH = "/data/service/el1/public/netmanager/";
} // namespace

int32_t DataBaseRdbOpenCallBack::OnCreate(NativeRdb::RdbStore &store)
{
    NETMGR_LOG_D("net_conn permission database onCreate");
    return NativeRdb::E_OK;
}

int32_t DataBaseRdbOpenCallBack::OnOpen(NativeRdb::RdbStore &store)
{
    NETMGR_LOG_D("net_conn permission database onOpen, create table");
    return store.ExecuteSql(CREATE_TABLE);
}

int32_t DataBaseRdbOpenCallBack::OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t currentVersion, int32_t targetVersion)
{
    NETMGR_LOG_D("net_conn permission database upgrade");
    return OHOS::NativeRdb::E_OK;
}

NetProxyUserinfo &NetProxyUserinfo::GetInstance()
{
    static NetProxyUserinfo instance;
    return instance;
}

void NetProxyUserinfo::SaveHttpProxyHostPass(const HttpProxy &httpProxy)
{
    NETMGR_LOG_I("net_conn database save user and pass info");
    if (rdbStore_ == nullptr) {
        NETMGR_LOG_E("net_conn save rdbStore_ is empty");
        return;
    }

    if (httpProxy.GetUsername().size() == 0 || httpProxy.GetPassword().size() == 0) {
        NETMGR_LOG_E("net_conn userPass info is empty");
        return;
    }

    NativeRdb::ValuesBucket valuesBucket;
    std::string userTemp = httpProxy.GetUsername();
    std::string passTemp = httpProxy.GetPassword();
    valuesBucket.Clear();
    valuesBucket.PutString(NETCONNPROXY_PRIM_KEY_COL, NETCONNPROXY_PRIMARY_KEY);
    valuesBucket.PutString(NETCONNPROXY_HOST_COL, userTemp);
    valuesBucket.PutString(NETCONNPROXY_PASS_COL, passTemp);
    
    errno_t userErrCode = memset_s(userTemp.data(), userTemp.size(), 0, userTemp.size());
    if (userErrCode != 0) {
        NETMGR_LOG_E("net_conn userData memory clearing failed, errCode=%{public}d", userErrCode);
    }
    errno_t passErrCode = memset_s(passTemp.data(), passTemp.size(), 0, passTemp.size());
    if (passErrCode != 0) {
        NETMGR_LOG_E("net_conn passData memory clearing failed, errCode=%{public}d", passErrCode);
    }

    int64_t outRowId;
    int32_t errCode = rdbStore_->InsertWithConflictResolution(outRowId, NETCONNPROXY_TABLE_NAME, valuesBucket,
                                                              ConflictResolution::ON_CONFLICT_REPLACE);
    if (errCode != NativeRdb::E_OK) {
        NETMGR_LOG_E("net_conn database rdb store insert failed, errCode=%{public}d", errCode);
        return;
    }
    NETMGR_LOG_D("net_conn database save user and pass info end");
}

void NetProxyUserinfo::GetHttpProxyHostPass(HttpProxy &httpProxy)
{
    if (rdbStore_ == nullptr) {
        NETMGR_LOG_E("net_conn get rdbStore_ is empty");
        return;
    }

    std::vector<std::string> columns;
    NativeRdb::AbsRdbPredicates dirAbsPred(NETCONNPROXY_TABLE_NAME);
    dirAbsPred.EqualTo(NETCONNPROXY_PRIM_KEY_COL, NETCONNPROXY_PRIMARY_KEY);
    auto resultSet = rdbStore_->Query(dirAbsPred, columns);
    if (resultSet == nullptr) {
        NETMGR_LOG_E("net_conn database rdb store query failed - null resultSet");
        return;
    }
    if (resultSet->GoToFirstRow() != NativeRdb::E_OK) {
        NETMGR_LOG_E("net_conn database rdb store query failed");
        resultSet->Close();
        return;
    }

    int32_t columnIndex;
    std::string user;
    std::string pass;
    resultSet->GetColumnIndex(NETCONNPROXY_HOST_COL, columnIndex);
    resultSet->GetString(columnIndex, user);
    resultSet->GetColumnIndex(NETCONNPROXY_PASS_COL, columnIndex);
    resultSet->GetString(columnIndex, pass);

    SecureData userData;
    userData.append(user.c_str(), user.size());
    httpProxy.SetUserName(userData);
    SecureData passData;
    passData.append(pass.c_str(), pass.size());
    httpProxy.SetPassword(passData);

    errno_t userErrCode = memset_s(user.data(), user.size(), 0, user.size());
    if (userErrCode != 0) {
        NETMGR_LOG_E("net_conn userData memory clearing failed, errCode=%{public}d", userErrCode);
    }
    errno_t passErrCode = memset_s(pass.data(), pass.size(), 0, pass.size());
    if (passErrCode != 0) {
        NETMGR_LOG_E("net_conn passData memory clearing failed, errCode=%{public}d", passErrCode);
    }
    NETMGR_LOG_D("net_conn database get host and pass info end");
}

NetProxyUserinfo::NetProxyUserinfo()
{
    std::string databaseDir = NETMGR_BASE_PATH;
    std::string bundleName = NETCONNPROXY_BUNDLENAME;
    std::string name = NET_CONN_PROXY_DATABASE_FILE;
    std::string realPath = databaseDir + name;
    NativeRdb::RdbStoreConfig config(std::move(realPath));
    config.SetBundleName(bundleName);
    config.SetName(std::move(name));
    config.SetEncryptStatus(true);

    int32_t errCode = NativeRdb::E_OK;
    DataBaseRdbOpenCallBack callBack;
    rdbStore_ = NativeRdb::RdbHelper::GetRdbStore(config, RDB_VERSION, callBack, errCode);
    if (rdbStore_ == nullptr) {
        NETMGR_LOG_E("net_conn database get rdb store failed, errCode=%{public}d", errCode);
    }
}

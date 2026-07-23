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

#include "net_datashare_utils.h"

#include <atomic>
#include <vector>

#include "data_ability_observer_stub.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *SETTINGS_DATASHARE_URI =
    "datashare:///com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true";
constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
constexpr const char *SETTINGS_DATA_COLUMN_KEYWORD = "KEYWORD";
constexpr const char *SETTINGS_DATA_COLUMN_VALUE = "VALUE";

constexpr int INVALID_VALUE = -1;
} // namespace

class NetDataAbilityObserver : public AAFwk::DataAbilityObserverStub {
public:
    explicit NetDataAbilityObserver(std::function<void()> onChange) : onChange_(std::move(onChange)) {}
    void OnChange() override
    {
        if (onChange_) {
            onChange_();
        }
    }
    void OnChangeExt(const AAFwk::ChangeInfo &) override {}
    void OnChangePreferences(const std::string &) override {}

private:
    std::function<void()> onChange_;
};

NetDataShareHelperUtils::NetDataShareHelperUtils() {}

std::shared_ptr<DataShare::DataShareHelper> NetDataShareHelperUtils::CreateDataShareHelper()
{
    sptr<ISystemAbilityManager> saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        NETMGR_LOG_E("GetSystemAbilityManager failed.");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObj = saManager->GetSystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID);
    if (remoteObj == nullptr) {
        NETMGR_LOG_E("GetSystemAbility Service Failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, SETTINGS_DATASHARE_URI, SETTINGS_DATA_EXT_URI);
}

int32_t NetDataShareHelperUtils::Query(Uri &uri, const std::string &key, std::string &value)
{
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper = CreateDataShareHelper();
    if (dataShareHelper == nullptr) {
        NETMGR_LOG_E("dataShareHelper is nullptr");
        return NETMANAGER_ERROR;
    }
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> columns;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    auto result = dataShareHelper->Query(uri, predicates, columns);
    if (result == nullptr) {
        NETMGR_LOG_E("query error, result is null");
        dataShareHelper->Release();
        return NETMANAGER_ERROR;
    }

    if (result->GoToFirstRow() != DataShare::E_OK) {
        NETMGR_LOG_E("go to first row error");
        result->Close();
        dataShareHelper->Release();
        return NETMANAGER_ERROR;
    }

    int columnIndex;
    result->GetColumnIndex(SETTINGS_DATA_COLUMN_VALUE, columnIndex);
    result->GetString(columnIndex, value);
    result->Close();
    dataShareHelper->Release();
    NETMGR_LOG_D("query success,value[%{public}s]", value.c_str());
    return NETMANAGER_SUCCESS;
}

int32_t NetDataShareHelperUtils::Insert(Uri &uri, const std::string &key, const std::string &value)
{
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper = CreateDataShareHelper();
    if (dataShareHelper == nullptr) {
        NETMGR_LOG_E("dataShareHelper is nullptr");
        return NETMANAGER_ERROR;
    }
    DataShare::DataShareValuesBucket valuesBucket;
    DataShare::DataShareValueObject keyObj(key);
    DataShare::DataShareValueObject valueObj(value);
    valuesBucket.Put(SETTINGS_DATA_COLUMN_KEYWORD, keyObj);
    valuesBucket.Put(SETTINGS_DATA_COLUMN_VALUE, valueObj);
    int32_t result = dataShareHelper->Insert(uri, valuesBucket);
    if (result == INVALID_VALUE) {
        NETMGR_LOG_E("insert failed");
        dataShareHelper->Release();
        return NETMANAGER_ERROR;
    }
    dataShareHelper->NotifyChange(uri);
    dataShareHelper->Release();
    NETMGR_LOG_I("insert success");
    return NETMANAGER_SUCCESS;
}

int32_t NetDataShareHelperUtils::Update(Uri &uri, const std::string &key, const std::string &value)
{
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper = CreateDataShareHelper();
    if (dataShareHelper == nullptr) {
        NETMGR_LOG_E("dataShareHelper is nullptr");
        return NETMANAGER_ERROR;
    }
    std::string queryValue;
    int32_t ret = Query(uri, key, queryValue);
    if (ret == NETMANAGER_ERROR) {
        dataShareHelper->Release();
        return Insert(uri, key, value);
    }

    DataShare::DataShareValuesBucket valuesBucket;
    DataShare::DataShareValueObject valueObj(value);
    valuesBucket.Put(SETTINGS_DATA_COLUMN_VALUE, valueObj);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    int32_t result = dataShareHelper->Update(uri, predicates, valuesBucket);
    if (result == INVALID_VALUE) {
        dataShareHelper->Release();
        return NETMANAGER_ERROR;
    }
    dataShareHelper->NotifyChange(uri);
    dataShareHelper->Release();
    NETMGR_LOG_I("update success");
    return NETMANAGER_SUCCESS;
}

int32_t NetDataShareHelperUtils::Delete(Uri &uri, const std::string &key)
{
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper = CreateDataShareHelper();
    if (dataShareHelper == nullptr) {
        NETMGR_LOG_E("dataShareHelper is nullptr");
        return NETMANAGER_ERROR;
    }
    std::string queryValue;
    int32_t ret = Query(uri, key, queryValue);
    if (ret == NETMANAGER_ERROR) {
        dataShareHelper->Release();
        NETMGR_LOG_D("don't have record");
        return NETMANAGER_SUCCESS;
    }

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTINGS_DATA_COLUMN_KEYWORD, key);
    int32_t result = dataShareHelper->Delete(uri, predicates);
    if (result == INVALID_VALUE) {
        dataShareHelper->Release();
        NETMGR_LOG_D("Delete failed");
        return NETMANAGER_ERROR;
    }
    dataShareHelper->NotifyChange(uri);
    dataShareHelper->Release();
    NETMGR_LOG_I("Delete success");
    return NETMANAGER_SUCCESS;
}

int32_t NetDataShareHelperUtils::RegisterObserver(const Uri &uri, const std::function<void()> &onChange)
{
    static std::atomic<int32_t> callbackId;
    auto dataShareHelper = CreateDataShareHelper();
    if (dataShareHelper == nullptr) {
        NETMGR_LOG_E("dataShareHelper is nullptr");
        return NETMANAGER_ERROR;
    }
    sptr<AAFwk::IDataAbilityObserver> observer = new (std::nothrow) NetDataAbilityObserver(onChange);
    if (observer == nullptr) {
        return NETMANAGER_ERROR;
    }
    dataShareHelper->RegisterObserver(uri, observer);
    auto id = ++callbackId;
    callbacks_.emplace(id, observer);
    return id;
}

int32_t NetDataShareHelperUtils::UnregisterObserver(const Uri &uri, int32_t callbackId)
{
    auto dataShareHelper = CreateDataShareHelper();
    if (dataShareHelper == nullptr) {
        NETMGR_LOG_E("dataShareHelper is nullptr");
        return NETMANAGER_ERROR;
    }
    auto it = callbacks_.find(callbackId);
    if (it == callbacks_.end() || it->second == nullptr) {
        return NETMANAGER_ERROR;
    }
    dataShareHelper->UnregisterObserver(uri, it->second);
    callbacks_.erase(it);
    return NETMANAGER_SUCCESS;
}

int32_t NetDataShareHelperUtils::UnRegisterSettingsObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    NETMGR_LOG_I("NetDataShareHelperUtils::UnRegisterSettingsObserver");
    auto settingHelper = CreateDataShareHelper();
    if (settingHelper == nullptr) {
        NETMGR_LOG_E("settingHelper is nullptr");
        return NETMANAGER_ERROR;
    }
    settingHelper->UnregisterObserver(uri, dataObserver);
    settingHelper->Release();
    return NETMANAGER_SUCCESS;
}

int32_t NetDataShareHelperUtils::RegisterSettingsObserver(const Uri &uri,
    const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    NETMGR_LOG_E("NetDataShareHelperUtils::RegisterSettingsObserver");
    auto settingHelper = CreateDataShareHelper();
    if (settingHelper == nullptr) {
        NETMGR_LOG_E("settingHelper is nullptr");
        return NETMANAGER_ERROR;
    }
    settingHelper->RegisterObserver(uri, dataObserver);
    settingHelper->Release();
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS

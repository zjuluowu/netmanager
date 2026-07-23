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

#include "netmanager_base_test_security.h"

#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace Security::AccessToken;
using Security::AccessToken::AccessTokenID;
namespace {
HapInfoParams netManagerBaseParms = {
    .userID = 1,
    .bundleName = "netmanager_base_test",
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};

HapInfoParams netConnManagerNotSystemInfo = {
    .userID = 1,
    .bundleName = "netmanager_base_test",
    .instIndex = 0,
    .appIDDesc = "test",
};

HapInfoParams netDataShareInfo = {
    .userID = 100,
    .bundleName = "netmanager_base_test",
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};

PermissionDef testNetConnInfoPermDef = {
    .permissionName = "ohos.permission.GET_NETWORK_INFO",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test ethernet maneger network info",
    .descriptionId = 1,
};

PermissionStateFull testNetConnInfoState = {
    .permissionName = "ohos.permission.GET_NETWORK_INFO",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

PermissionDef testNetConnInternetPermDef = {
    .permissionName = "ohos.permission.INTERNET",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test net connect manager internet",
    .descriptionId = 1,
};

PermissionStateFull testNetConnInternetState = {
    .permissionName = "ohos.permission.INTERNET",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

PermissionDef testNetConnInternalPermDef = {
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test net connect manager internet",
    .descriptionId = 1,
};

PermissionStateFull testNetConnInternalState = {
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

PermissionDef testNetPolicyStrategyPermDef = {
    .permissionName = "ohos.permission.MANAGE_NET_STRATEGY",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test net policy manager",
    .descriptionId = 1,
};

PermissionStateFull testManageNetStrategyState = {
    .permissionName = "ohos.permission.MANAGE_NET_STRATEGY",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

PermissionDef testNetSysInternalDef = {
    .permissionName = "ohos.permission.NETSYS_INTERNAL",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test netsys_native_manager_test",
    .descriptionId = 1,
};

PermissionStateFull testNetSysInternalState = {
    .permissionName = "ohos.permission.NETSYS_INTERNAL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

PermissionDef testNetConnSettingsPermDef = {
    .permissionName = "ohos.permission.MANAGE_SECURE_SETTINGS",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "Test net data share",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionStateFull testNetConnSettingsState = {
    .grantFlags = { 2 },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .isGeneral = true,
    .permissionName = "ohos.permission.MANAGE_SECURE_SETTINGS",
    .resDeviceID = { "local" },
};

PermissionDef testNetStatsPermDef = {
    .permissionName = "ohos.permission.GET_NETWORK_STATS",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test net stats manager",
    .descriptionId = 1,
};

PermissionStateFull testNetStatsState = {
    .permissionName = "ohos.permission.GET_NETWORK_STATS",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

PermissionDef testNetManageSettingsDef = {
    .permissionName = "ohos.permission.MANAGE_SETTINGS",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test net stats manager",
    .descriptionId = 1,
};

PermissionStateFull testNetManageSettingsState = {
    .permissionName = "ohos.permission.MANAGE_SETTINGS",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

PermissionDef testPacUrlPermDef = {
    .permissionName = "ohos.permission.SET_PAC_URL",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test set pac url",
    .descriptionId = 1,
};
 
PermissionStateFull testPacUrlState = {
    .permissionName = "ohos.permission.SET_PAC_URL",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

HapPolicyParams netManagerBasePolicy = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testNetConnInfoPermDef, testNetConnInternetPermDef, testNetConnInternalPermDef,
        testNetPolicyStrategyPermDef, testNetSysInternalDef, testNetStatsPermDef, testNetManageSettingsDef,
        testPacUrlPermDef },
    .permStateList = { testNetConnInfoState, testNetConnInternetState, testNetConnInternalState,
        testManageNetStrategyState, testNetSysInternalState, testNetStatsState, testNetManageSettingsState,
        testPacUrlState },
};

PermissionDef testNoPermissionDef = {
    .permissionName = "",
    .bundleName = "netmanager_base_test",
    .grantMode = 1,
    .availableLevel = APL_SYSTEM_BASIC,
    .label = "label",
    .labelId = 1,
    .description = "Test no permission",
    .descriptionId = 1,
};

PermissionStateFull testNoPermissionState = {
    .permissionName = "",
    .isGeneral = true,
    .resDeviceID = { "local" },
    .grantStatus = { PermissionState::PERMISSION_GRANTED },
    .grantFlags = { 2 },
};

HapPolicyParams testNoPermission = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testNoPermissionDef },
    .permStateList = { testNoPermissionState },
};

HapPolicyParams netDataSharePolicy = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = { testNetConnSettingsPermDef },
    .permStateList = { testNetConnSettingsState },
};
} // namespace

NetManagerBaseAccessToken::NetManagerBaseAccessToken() : currentID_(GetSelfTokenID())
{
    AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(netManagerBaseParms, netManagerBasePolicy);
    accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
    SetSelfTokenID(tokenIdEx.tokenIDEx);
}

NetManagerBaseAccessToken::~NetManagerBaseAccessToken()
{
    AccessTokenKit::DeleteToken(accessID_);
    SetSelfTokenID(currentID_);
}

NetManagerBaseNotSystemToken::NetManagerBaseNotSystemToken() : currentID_(GetSelfTokenID())
{
    AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(netConnManagerNotSystemInfo, netManagerBasePolicy);
    accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
    SetSelfTokenID(accessID_);
}

NetManagerBaseNotSystemToken::~NetManagerBaseNotSystemToken()
{
    AccessTokenKit::DeleteToken(accessID_);
    SetSelfTokenID(currentID_);
}

NetManagerBaseNoPermissionToken::NetManagerBaseNoPermissionToken() : currentID_(GetSelfTokenID())
{
    AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(netManagerBaseParms, testNoPermission);
    accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
    SetSelfTokenID(tokenIdEx.tokenIDEx);
}

NetManagerBaseNoPermissionToken::~NetManagerBaseNoPermissionToken()
{
    AccessTokenKit::DeleteToken(accessID_);
    SetSelfTokenID(currentID_);
}

NetManagerBaseDataShareToken::NetManagerBaseDataShareToken() : currentID_(GetSelfTokenID())
{
    AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(netDataShareInfo, netDataSharePolicy);
    accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
    SetSelfTokenID(tokenIdEx.tokenIDEx);
}

NetManagerBaseDataShareToken::~NetManagerBaseDataShareToken()
{
    AccessTokenKit::DeleteToken(accessID_);
    SetSelfTokenID(currentID_);
}
} // namespace NetManagerStandard
} // namespace OHOS

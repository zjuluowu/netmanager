/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "accesstoken_kit.h"
#include "token_setproc.h"
#include <gtest/gtest.h>

using namespace OHOS::Security::AccessToken;
PermissionDef connectPerm = {
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .bundleName = "net_client_pac_file_url_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "Test web connect maneger",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionStateFull connectState = {
    .grantFlags = {2},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.CONNECTIVITY_INTERNAL",
    .resDeviceID = {"local"},
};

PermissionDef pacPerm = {
    .permissionName = "ohos.permission.SET_PAC_URL",
    .bundleName = "net_client_pac_file_url_test",
    .grantMode = 1,
    .label = "label",
    .labelId = 1,
    .description = "Test web connect maneger",
    .descriptionId = 1,
    .availableLevel = APL_SYSTEM_BASIC,
};

PermissionStateFull pacState = {
    .grantFlags = {2},
    .grantStatus = {PermissionState::PERMISSION_GRANTED},
    .isGeneral = true,
    .permissionName = "ohos.permission.SET_PAC_URL",
    .resDeviceID = {"local"},
};

HapPolicyParams testPolicyPrams = {
    .apl = APL_SYSTEM_BASIC,
    .domain = "test.domain",
    .permList = {pacPerm, connectPerm},
    .permStateList = {pacState, connectState},
};

HapInfoParams testInfoParms = {
    .userID = 1,
    .bundleName = "net_client_pac_file_url_test",
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};
static AccessTokenID accessID_;
static uint64_t currentID_;

void SetUpPermission()
{
    currentID_ = GetSelfTokenID();
    AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParms, testPolicyPrams);
    accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
    SetSelfTokenID(tokenIdEx.tokenIDEx);
}

void UnsetUpPermission()
{
    AccessTokenKit::DeleteToken(accessID_);
    SetSelfTokenID(currentID_);
}

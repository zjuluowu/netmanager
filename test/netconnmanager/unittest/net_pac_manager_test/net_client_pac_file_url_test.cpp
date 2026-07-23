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
#include "mock_timer.h"
#include "net_all_capabilities.h"
#include "net_connection.h"
#include "pac_server.h"
#include "token_setproc.h"
#include <gtest/gtest.h>

using namespace OHOS::Security::AccessToken;

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
    .permList = {pacPerm},
    .permStateList = {pacState},
};

HapInfoParams testInfoParms = {
    .userID = 1,
    .bundleName = "net_client_pac_file_url_test",
    .instIndex = 0,
    .appIDDesc = "test",
    .isSystemApp = true,
};
static void Test1()
{
    int32_t ret = -1;
    {
        std::string url = "http://127.0.0.1/index";
        std::string host = "127.0.0.1";
        char proxy[1024];
        ret = OH_NetConn_FindProxyForURL(url.c_str(), nullptr, proxy);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(std::string(proxy), "DIRECT");
    }
    {
        std::string url = "http://test.local/index";
        char proxy[1024];
        ret = OH_NetConn_FindProxyForURL(url.c_str(), nullptr, proxy);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(std::string(proxy), "DIRECT");
    }
    {
        std::string url = "http://192.168.0.111/index";
        char proxy[1024];
        ret = OH_NetConn_FindProxyForURL(url.c_str(), nullptr, proxy);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(std::string(proxy), "DIRECT");
    }
    {
        std::string url = "http://192.168.111.111/index";
        char proxy[1024];
        ret = OH_NetConn_FindProxyForURL(url.c_str(), nullptr, proxy);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(std::string(proxy), "DIRECT");
    }
    {
        std::string url = "http://hostname/index";
        char proxy[1024];
        ret = OH_NetConn_FindProxyForURL(url.c_str(), nullptr, proxy);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(std::string(proxy), "PROXY special-proxy.com:8001");
    }
}

TEST(MyTests, PacFileUrlClient)
{
    uint64_t currentID_ = GetSelfTokenID();
    AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(testInfoParms, testPolicyPrams);
    AccessTokenID accessID_ = tokenIdEx.tokenIdExStruct.tokenID;
    SetSelfTokenID(tokenIdEx.tokenIDEx);

    StartHttpServer();
    sleep(6);
    std::string url = "http://localhost:8888/";
    char pacFileUrl[1024];
    int32_t ret = OH_NetConn_SetPacFileUrl(url.c_str());
    EXPECT_EQ(ret, 0);
    ret = OH_NetConn_SetProxyMode(OHOS::NetManagerStandard::ProxyModeType::PROXY_MODE_AUTO);
    EXPECT_EQ(ret, 0);
    {
        std::string url = "http://127.0.0.1/index";
        std::string host = "127.0.0.1";
        char proxy[1024];
        ret = OH_NetConn_FindProxyForURL(url.c_str(), nullptr, proxy);
        EXPECT_EQ(ret, 0);
        EXPECT_EQ(std::string(proxy), "DIRECT");
    }
    ret = OH_NetConn_GetPacFileUrl(pacFileUrl);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(std::string(pacFileUrl), url);
    Test1();
    AccessTokenKit::DeleteToken(accessID_);
    SetSelfTokenID(currentID_);
}
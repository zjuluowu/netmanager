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

#ifndef NETMANAGER_BASE_TEST_SECURITY_H
#define NETMANAGER_BASE_TEST_SECURITY_H

#include "accesstoken_kit.h"

namespace OHOS {
namespace NetManagerStandard {
class NetManagerBaseAccessToken {
public:
    NetManagerBaseAccessToken();
    ~NetManagerBaseAccessToken();

private:
    Security::AccessToken::AccessTokenID currentID_ = 0;
    Security::AccessToken::AccessTokenID accessID_ = 0;
};

class NetManagerBaseNotSystemToken {
public:
    NetManagerBaseNotSystemToken();
    ~NetManagerBaseNotSystemToken();

private:
    Security::AccessToken::AccessTokenID currentID_ = 0;
    Security::AccessToken::AccessTokenID accessID_ = 0;
};

class NetManagerBaseNoPermissionToken {
public:
    NetManagerBaseNoPermissionToken();
    ~NetManagerBaseNoPermissionToken();

private:
    Security::AccessToken::AccessTokenID currentID_ = 0;
    Security::AccessToken::AccessTokenID accessID_ = 0;
};

class NetManagerBaseDataShareToken {
public:
    NetManagerBaseDataShareToken();
    ~NetManagerBaseDataShareToken();

private:
    Security::AccessToken::AccessTokenID currentID_ = 0;
    Security::AccessToken::AccessTokenID accessID_ = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_BASE_TEST_SECURITY_H

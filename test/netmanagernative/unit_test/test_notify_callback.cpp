/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include <iostream>

#include "netnative_log_wrapper.h"
#include "test_notify_callback.h"

namespace OHOS {
namespace NetsysNative {
TestNotifyCallback::TestNotifyCallback() {}

TestNotifyCallback::~TestNotifyCallback() {}

int32_t TestNotifyCallback::OnInterfaceAddressUpdated(const std::string &, const std::string &, int, int)
{
    NETNATIVE_LOGI("Begin to TestNotifyCallback::OnInterfaceAddressUpdated");

    return 0;
}

int32_t TestNotifyCallback::OnInterfaceAddressRemoved(const std::string &, const std::string &, int, int)
{
    NETNATIVE_LOGI("Begin to TestNotifyCallback::OnInterfaceAddressRemoved");

    return 0;
}

int32_t TestNotifyCallback::OnInterfaceAdded(const std::string &)
{
    NETNATIVE_LOGI("Begin to TestNotifyCallback::OnInterfaceAdded");

    return 0;
}

int32_t TestNotifyCallback::OnInterfaceRemoved(const std::string &)
{
    NETNATIVE_LOGI("Begin to TestNotifyCallback::OnInterfaceRemoved");

    return 0;
}

int32_t TestNotifyCallback::OnInterfaceChanged(const std::string &, bool)
{
    NETNATIVE_LOGI("Begin to TestNotifyCallback::OnInterfaceChanged");

    return 0;
}

int32_t TestNotifyCallback::OnInterfaceLinkStateChanged(const std::string &, bool)
{
    NETNATIVE_LOGI("Begin to TestNotifyCallback::OnInterfaceLinkStateChanged");

    return 0;
}

int32_t TestNotifyCallback::OnRouteChanged(bool, const std::string &, const std::string &, const std::string &)
{
    NETNATIVE_LOGI("Begin to TestNotifyCallback::OnRouteChanged");

    return 0;
}

int32_t TestNotifyCallback::OnDhcpSuccess(sptr<OHOS::NetsysNative::DhcpResultParcel> &dhcpResult)
{
    NETNATIVE_LOGI("Begin to TestNotifyCallback::OnDhcpSuccess");

    return 0;
}
} // namespace NetsysNative
} // namespace OHOS

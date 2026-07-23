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

#ifndef TEST_NOTIFY_CALLBACK_H
#define TEST_NOTIFY_CALLBACK_H

#include "notify_callback_stub.h"

namespace OHOS {
namespace NetsysNative {
class TestNotifyCallback : public NotifyCallbackStub {
public:
    TestNotifyCallback();
    ~TestNotifyCallback() override;
    int32_t OnInterfaceAddressUpdated(const std::string &, const std::string &, int, int) override;
    int32_t OnInterfaceAddressRemoved(const std::string &, const std::string &, int, int) override;
    int32_t OnInterfaceAdded(const std::string &) override;
    int32_t OnInterfaceRemoved(const std::string &) override;
    int32_t OnInterfaceChanged(const std::string &, bool) override;
    int32_t OnInterfaceLinkStateChanged(const std::string &, bool) override;
    int32_t OnRouteChanged(bool, const std::string &, const std::string &, const std::string &) override;
    int32_t OnDhcpSuccess(sptr<OHOS::NetsysNative::DhcpResultParcel> &dhcpResult) override;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // TEST_NOTIFY_CALLBACK_H

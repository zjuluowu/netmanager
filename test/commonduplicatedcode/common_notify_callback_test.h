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

#ifndef COMMON_NOTIFY_CALLBACK_TEST_H
#define COMMON_NOTIFY_CALLBACK_TEST_H

#include "notify_callback_stub.h"
#include "net_dns_result_callback_stub.h"

namespace OHOS {
namespace NetsysNative {
class NotifyCallbackTest : public NotifyCallbackStub {
public:
    NotifyCallbackTest() = default;
    ~NotifyCallbackTest() override {};
    int32_t OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName, int flags, int scope) override
    {
        return 0;
    }

    int32_t OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName, int flags, int scope) override
    {
        return 0;
    }

    int32_t OnInterfaceAdded(const std::string &ifName) override
    {
        return 0;
    }

    int32_t OnInterfaceRemoved(const std::string &ifName) override
    {
        return 0;
    }

    int32_t OnInterfaceChanged(const std::string &ifName, bool up) override
    {
        return 0;
    }

    int32_t OnInterfaceLinkStateChanged(const std::string &ifName, bool up) override
    {
        return 0;
    }

    int32_t OnRouteChanged(
        bool updated, const std::string &route, const std::string &gateway, const std::string &ifName) override
    {
        return 0;
    }

    int32_t OnDhcpSuccess(sptr<DhcpResultParcel> &dhcpResult) override
    {
        return 0;
    }

    int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface) override
    {
        return 0;
    }

#ifdef FEATURE_NET_FIREWALL_ENABLE
    int32_t OnInterceptRecord(sptr<NetManagerStandard::InterceptRecord> &record) override
    {
        return 0;
    }
#endif
};

class DnsResultCallbackTest : public NetDnsResultCallbackStub {
public:
    DnsResultCallbackTest() = default;
    ~DnsResultCallbackTest() override {};
    int32_t OnDnsResultReport(uint32_t size, std::list<OHOS::NetsysNative::NetDnsResultReport> res) override
    {
        return 0;
    }
};
} // namespace NetsysNative
} // namespace OHOS
#endif // COMMON_NOTIFY_CALLBACK_TEST_H

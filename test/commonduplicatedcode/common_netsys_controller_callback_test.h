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

#ifndef COMMON_NETSYS_CONTROLLER_CALLBACK_TEST_H
#define COMMON_NETSYS_CONTROLLER_CALLBACK_TEST_H

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_constants.h"
#include "netsys_controller_callback.h"

namespace OHOS {
namespace NetManagerStandard {
class NetsysControllerCallbackTestCb : public NetsysControllerCallback {
public:
    virtual int32_t OnInterfaceAddressUpdated(const std::string &, const std::string &, int, int)
    {
        return NETMANAGER_SUCCESS;
    }
    virtual int32_t OnInterfaceAddressRemoved(const std::string &, const std::string &, int, int)
    {
        return NETMANAGER_SUCCESS;
    }
    virtual int32_t OnInterfaceAdded(const std::string &)
    {
        return NETMANAGER_SUCCESS;
    }
    virtual int32_t OnInterfaceRemoved(const std::string &)
    {
        return NETMANAGER_SUCCESS;
    }
    virtual int32_t OnInterfaceChanged(const std::string &, bool)
    {
        return NETMANAGER_SUCCESS;
    }
    virtual int32_t OnInterfaceLinkStateChanged(const std::string &, bool)
    {
        return NETMANAGER_SUCCESS;
    }
    virtual int32_t OnRouteChanged(bool, const std::string &, const std::string &, const std::string &)
    {
        return NETMANAGER_SUCCESS;
    }
    virtual int32_t OnDhcpSuccess(NetsysControllerCallback::DhcpResult &dhcpResult)
    {
        return NETMANAGER_SUCCESS;
    }
    virtual int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface)
    {
        return NETMANAGER_SUCCESS;
    }
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMON_NETSYS_CONTROLLER_CALLBACK_TEST_H

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

#include "netinterface.h"
#include "netmanager_base_log.h"

#include <shared_mutex>

namespace OHOS::NetManagerStandard {
std::map<NetInterfaceCallbackObserver *, NetInterface *> NET_INTERFACES;
std::shared_mutex g_netInterfacesMutex;

NetInterface::NetInterface(std::shared_ptr<EventManager>& eventManager)
    : observer_(new NetInterfaceCallbackObserver),
      manager_(eventManager)
{
}

NetInterface *NetInterface::MakeNetInterface(std::shared_ptr<EventManager>& eventManager)
{
    std::unique_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    auto netInterface = new NetInterface(eventManager);
    NET_INTERFACES[netInterface->observer_.GetRefPtr()] = netInterface;
    return netInterface;
}

void NetInterface::DeleteNetInterface(NetInterface *netInterface)
{
    std::unique_lock<std::shared_mutex> lock(g_netInterfacesMutex);
    NET_INTERFACES.erase(netInterface->observer_.GetRefPtr());
    delete netInterface;
}

sptr<NetInterfaceCallbackObserver> NetInterface::GetObserver() const
{
    return observer_;
}

std::shared_ptr<EventManager> NetInterface::GetEventManager() const
{
    return manager_;
}
NetInterface::NetInterface() {}

} // namespace OHOS::NetManagerStandard

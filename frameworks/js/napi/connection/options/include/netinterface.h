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

#ifndef NETMANAGER_BASE_NETINTERFACE_H
#define NETMANAGER_BASE_NETINTERFACE_H

#include <map>

#include "net_interface_callback_observer.h"
#include "event_manager.h"

namespace OHOS::NetManagerStandard {
class NetInterface final {
public:
    uint64_t moduleId_ = 0;

public:
    NetInterface();
    ~NetInterface() = default;

    [[nodiscard]] sptr<NetInterfaceCallbackObserver> GetObserver() const;

    [[nodiscard]] std::shared_ptr<EventManager> GetEventManager() const;

    static NetInterface *MakeNetInterface(std::shared_ptr<EventManager>& eventManager);

    static void DeleteNetInterface(NetInterface *netInterface);

private:
    sptr<NetInterfaceCallbackObserver> observer_;

    std::shared_ptr<EventManager> manager_{nullptr};

    explicit NetInterface(std::shared_ptr<EventManager>& eventManager);
};

extern std::map<NetInterfaceCallbackObserver *, NetInterface *> NET_INTERFACES;
extern std::shared_mutex g_netInterfacesMutex;
} // namespace OHOS::NetManagerStandard

#endif /* NETMANAGER_BASE_NETINTERFACE_H */

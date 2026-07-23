/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef NETMANAGER_BASE_NETCONNECTION_H
#define NETMANAGER_BASE_NETCONNECTION_H

#include <map>

#include "net_conn_callback_observer.h"
#include "net_specifier.h"
#include "event_manager.h"

namespace OHOS::NetManagerStandard {
class NetConnection final {
public:
    bool hasNetSpecifier_ = false;
    bool hasTimeout_ = false;
    NetManagerStandard::NetSpecifier netSpecifier_;
    uint32_t timeout_ = 0;
    uint64_t moduleId_ = 0;

public:
    NetConnection();
    ~NetConnection() = default;

    [[nodiscard]] sptr<NetConnCallbackObserver> GetObserver() const;

    [[nodiscard]] std::shared_ptr<EventManager> GetEventManager() const;

    static NetConnection *MakeNetConnection(std::shared_ptr<EventManager>& eventManager);

    static void DeleteNetConnection(NetConnection *netConnection);

private:
    sptr<NetConnCallbackObserver> observer_;

    std::shared_ptr<EventManager> manager_{nullptr};

    explicit NetConnection(std::shared_ptr<EventManager>& eventManager);
};

extern std::map<NetConnCallbackObserver *, NetConnection *> NET_CONNECTIONS;
extern std::shared_mutex g_netConnectionsMutex;
} // namespace OHOS::NetManagerStandard

#endif /* NETMANAGER_BASE_NETCONNECTION_H */

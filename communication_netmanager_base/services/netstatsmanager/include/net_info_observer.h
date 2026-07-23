 
/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef NET_OBSERVER_H
#define NET_OBSERVER_H
 
#include "net_conn_callback_stub.h"
#include "net_handle.h"
#include "net_all_capabilities.h"
 
namespace OHOS {
namespace NetManagerStandard {
 
enum ErrorNum {
    ERR_NONE,
    ERR_FAIL,
};
 
class NetInfoObserver final : public NetManagerStandard::NetConnCallbackStub {
public:
    NetInfoObserver() {}
    int32_t NetCapabilitiesChange(sptr<NetManagerStandard::NetHandle> &netHandle,
        const sptr<NetManagerStandard::NetAllCapabilities> &netAllCap) override;
    int32_t NetAvailable(sptr<NetManagerStandard::NetHandle> &netHandle) override;
    int32_t NetLost(sptr<NetManagerStandard::NetHandle> &netHandle) override;
    int32_t NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info) override;
 
private:
    std::string ident_;
};
}
}
#endif
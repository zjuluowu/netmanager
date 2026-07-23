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

#ifndef OPENHARMONY_NET_CONN_SERVICE_PAC_PROXY_HELPER_H
#define OPENHARMONY_NET_CONN_SERVICE_PAC_PROXY_HELPER_H
#include "iremote_proxy.h"
#include "i_net_conn_service.h"

namespace OHOS {
namespace NetManagerStandard {
class NetConnServicePacProxyHelper {
typedef std::function<int32_t(uint32_t, MessageParcel&, MessageParcel&)> RequestFunction;
public:
    int32_t SetPacUrl(const std::string &pacUrl);
    int32_t GetPacUrl(std::string &pacUrl);
    int32_t SetPacFileUrl(const std::string &pacUrl);
    int32_t SetProxyMode(const OHOS::NetManagerStandard::ProxyModeType mode);
    int32_t GetProxyMode(OHOS::NetManagerStandard::ProxyModeType &mode);
    int32_t GetPacFileUrl(std::string &pacUrl);
    int32_t FindProxyForURL(const std::string &url, const std::string &host, std::string &proxy);
    static std::shared_ptr<NetConnServicePacProxyHelper> GetInstance(RequestFunction function);
private:
    bool WriteInterfaceToken(MessageParcel &data);
    RequestFunction requestFunction_;
};
}
}
#endif //OPENHARMONY_NET_CONN_SERVICE_PAC_PROXY_HELPER_H

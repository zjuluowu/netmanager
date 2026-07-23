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

#ifndef NETMANAGER_BASE_HTTP_PROXY_H
#define NETMANAGER_BASE_HTTP_PROXY_H

#include <string>
#include <list>
#include <optional>

#include "parcel.h"
#include "securec.h"
#include "netmanager_secure_data.h"

namespace OHOS {
namespace NetManagerStandard {
class NetConnService;
class HttpProxy final : public Parcelable {
public:
    friend class NetConnService;
    HttpProxy();
    HttpProxy(std::string host, uint16_t port, const std::list<std::string> &exclusionList);

    [[nodiscard]] std::string GetHost() const;
    [[nodiscard]] uint16_t GetPort() const;
    [[nodiscard]] std::list<std::string> GetExclusionList() const;
    [[nodiscard]] std::string ToString() const;
    [[nodiscard]] SecureData GetUsername() const;
    [[nodiscard]] SecureData GetPassword() const;
    [[nodiscard]] int32_t GetUserId() const;
    [[nodiscard]] static std::optional<HttpProxy> FromString(const std::string &str);
    void inline SetHost(std::string &&host)
    {
        host_ = host;
    }
    void inline SetPort(uint16_t port)
    {
        port_ = port;
    }
    void inline SetExclusionList(const std::list<std::string> &list)
    {
        exclusionList_ = list;
    }
    void inline SetUserName(const SecureData &username)
    {
        username_ = username;
    }
    void inline SetPassword(const SecureData &password)
    {
        password_ = password;
    }

    void inline SetUserId(int32_t userId)
    {
        userId_ = userId;
    }

    bool operator==(const HttpProxy &httpProxy) const;
    bool operator!=(const HttpProxy &httpProxy) const;
    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, HttpProxy &httpProxy);

private:
    std::string host_;
    uint16_t port_;
    SecureData username_;
    SecureData password_;
    std::list<std::string> exclusionList_;
    int32_t userId_ = -1;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif /* NETMANAGER_BASE_HTTP_PROXY_H */

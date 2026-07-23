/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef NET_INTERFACE_CONFIG_H
#define NET_INTERFACE_CONFIG_H

#include <string>
#include <vector>

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetInterfaceConfiguration : public Parcelable {
    std::string ifName_;
    std::string hwAddr_;
    std::string ipv4Addr_;
    int prefixLength_ = 0;
    std::vector<std::string> flags_;

    bool IsInterfaceUp();
    bool IsInterfaceRunning();

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, NetInterfaceConfiguration &config);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_INTERFACE_CONFIG_H

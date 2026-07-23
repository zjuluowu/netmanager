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

#include "parcel.h"

namespace OHOS::NetsysNative {
bool Parcel::WriteBool(bool value)
{
    return true;
}

bool Parcel::WriteString(const std::string &value)
{
    return true;
}

bool Parcel::WriteUint16(uint16_t value)
{
    return true;
}

bool Parcel::WriteUint32(uint32_t value)
{
    return true;
}

bool ReadUint32(uint32_t &value)
{
    return true;
}

bool ReadString(const std::string &value)
{
    return true;
}

bool ReadUint16(uint16_t value)
{
    return true;
}

bool ReadBool(bool value)
{
    return true;
}
}  // namespace OHOS::NetsysNative

namespace OHOS::NetManagerStandard {
bool Parcel::WriteBool(bool value)
{
    return true;
}

bool Parcel::WriteString(const std::string &value)
{
    return true;
}

bool Parcel::WriteUint16(uint16_t value)
{
    return true;
}

bool Parcel::WriteUint32(uint32_t value)
{
    return true;
}

bool ReadUint32(uint32_t value)
{
    return true;
}

bool ReadString(const std::string &value)
{
    return true;
}

bool ReadUint16(uint16_t value)
{
    return true;
}

bool ReadBool(bool value)
{
    return true;
}
}  // namespace OHOS::NetManagerStandard

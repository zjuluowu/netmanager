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

#ifndef MOCK_NETMANAGERNATIVE_PARCEL
#define MOCK_NETMANAGERNATIVE_PARCEL

#include <string>
#include "refbase.h"

namespace OHOS::NetsysNative {
class Parcelable;
class Parcel {
public:
    Parcel() {}
    virtual ~Parcel() = default;

    virtual bool WriteUint32(uint32_t) = 0;

    virtual bool WriteUint16(uint16_t) = 0;

    virtual bool WriteBool(bool) = 0;

    virtual bool WriteUint8(uint8_t) = 0;

    virtual bool WriteUint64(uint64_t) = 0;

    virtual bool WriteInt32(int32_t) = 0;

    virtual bool WriteString(const std::string &) = 0;

    virtual bool ReadUint32(uint32_t &) = 0;

    virtual bool ReadString(const std::string &) = 0;

    virtual bool ReadUint16(uint16_t) = 0;

    virtual bool ReadBool(bool) = 0;

    virtual bool ReadUint8(uint8_t) = 0;

    virtual bool ReadUint64(uint64_t) = 0;

    virtual bool ReadInt32(int32_t) = 0;
};

class Parcelable {
public:
    Parcelable() = default;
    virtual ~Parcelable() = default;
    virtual bool Marshalling(Parcel &parcel) const = 0;
};
}  // namespace OHOS::NetsysNative

namespace OHOS::NetManagerStandard {
class Parcelable;
class Parcel {
public:
    Parcel() {}
    virtual ~Parcel() = default;

    virtual bool WriteUint32(uint32_t) = 0;

    virtual bool WriteUint16(uint16_t) = 0;

    virtual bool WriteBool(bool) = 0;

    virtual bool WriteUint8(uint8_t) = 0;

    virtual bool WriteUint64(uint64_t) = 0;

    virtual bool WriteInt32(int32_t) = 0;

    virtual bool WriteString(const std::string &) = 0;

    virtual bool ReadUint32(uint32_t) = 0;

    virtual bool ReadString(const std::string &) = 0;

    virtual bool ReadUint16(uint16_t) = 0;

    virtual bool ReadBool(bool) = 0;

    virtual bool ReadUint8(uint8_t) = 0;

    virtual bool ReadUint64(uint64_t) = 0;

    virtual bool ReadInt32(int32_t) = 0;
};

class Parcelable {
public:
    Parcelable() = default;
    virtual ~Parcelable() = default;
    virtual bool Marshalling(Parcel &parcel) const = 0;
};
}  // namespace OHOS::NetManagerStandard
#endif

/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "uid_range.h"

#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {
constexpr int32_t INVLID_VALUE = -1;
}

UidRange::UidRange(int32_t begin, int32_t end, uint32_t priorityId, uint32_t netId)
    : begin_(begin), end_(end), priorityId_(priorityId), netId_(netId) {}

uint32_t UidRange::Size() const
{
    if (begin_ == INVLID_VALUE || end_ == INVLID_VALUE || end_ < begin_) {
        return 0;
    }
    return static_cast<uint32_t>(end_ - begin_ + 1);
}

bool UidRange::Marshalling(Parcel &parcel) const
{
    return parcel.WriteInt32(begin_) && parcel.WriteInt32(end_) &&
        parcel.WriteUint32(priorityId_) && parcel.WriteUint32(netId_);
}

sptr<UidRange> UidRange::Unmarshalling(Parcel &parcel)
{
    sptr<UidRange> ptr = new (std::nothrow) UidRange();
    if (ptr == nullptr) {
        NETNATIVE_LOGE("UidRange Unmarshalling new object is failed.");
        return nullptr;
    }

    bool allOK = parcel.ReadInt32(ptr->begin_) && parcel.ReadInt32(ptr->end_) &&
        parcel.ReadUint32(ptr->priorityId_) && parcel.ReadUint32(ptr->netId_);
    return allOK ? ptr : nullptr;
}
} // namespace NetManagerStandard
} // namespace OHOS
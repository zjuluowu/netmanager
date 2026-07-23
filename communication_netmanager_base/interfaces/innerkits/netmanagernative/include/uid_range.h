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

#ifndef APP_UID_RANGE_H
#define APP_UID_RANGE_H

#include <string>

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct UidRange : public Parcelable {
    UidRange(int32_t begin, int32_t end, uint32_t priorityId, uint32_t netId);
    UidRange() = default;
    virtual ~UidRange(){};

    uint32_t Size() const;

    friend bool operator<(const UidRange &lhs, const UidRange &rhs)
    {
        if (lhs.priorityId_ == rhs.priorityId_) {
            return lhs.begin_ != rhs.begin_ ? (lhs.begin_ < rhs.begin_) : (lhs.end_ < rhs.end_);
        } else {
            return lhs.priorityId_ > rhs.priorityId_;
        }
    }

    friend bool operator==(const UidRange &lhs, const UidRange &rhs)
    {
        return (lhs.begin_ == rhs.begin_ && lhs.end_ == rhs.end_ &&
            lhs.priorityId_ == rhs.priorityId_ && lhs.netId_ == rhs.netId_);
    }

    friend bool operator!=(const UidRange &lhs, const UidRange &rhs)
    {
        return !(lhs == rhs);
    }

    bool Marshalling(Parcel &parcel) const override;
    static sptr<UidRange> Unmarshalling(Parcel &parcel);

    int32_t begin_ = -1;
    int32_t end_ = -1;
    uint32_t priorityId_ = 0;
    uint32_t netId_ = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // APP_UID_RANGE_H
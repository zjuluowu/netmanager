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

#ifndef COMMUNICATIONNETMANAGERBASE_ICU_HELPER_H
#define COMMUNICATIONNETMANAGERBASE_ICU_HELPER_H

#include <string>

namespace OHOS::NetManagerStandard {
enum class ConversionProcess : uint8_t { NO_CONFIGURATION = 0, ALLOW_UNASSIGNED = 1, USE_STD3_ASCII_RULES = 2 };

class ICUHelper {
public:
    static int32_t GetDnsASCII(const std::string &host, ConversionProcess conversionProcess, std::string &ascii);
    static int32_t GetDnsUnicode(const std::string &host, ConversionProcess conversionProcess, std::string &unicode);

private:
    static int32_t ConvertIDN(const std::string &input, ConversionProcess conversionProcess, bool toASCII,
                              std::string &output);
};
} // namespace OHOS::NetManagerStandard

#endif // COMMUNICATIONNETMANAGERBASE_ICU_HELPER_H

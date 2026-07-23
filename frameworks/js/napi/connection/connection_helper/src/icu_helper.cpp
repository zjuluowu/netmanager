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

#include "icu_helper.h"
#include "net_manager_constants.h"
#include "unicode/uidna.h"
#include "unicode/unistr.h"

namespace OHOS::NetManagerStandard {
constexpr uint32_t MAX_HOST_LENGTH = 255;

int32_t ICUHelper::GetDnsASCII(const std::string &host, ConversionProcess conversionProcess, std::string &ascii)
{
    return ConvertIDN(host, conversionProcess, true, ascii);
}

int32_t ICUHelper::GetDnsUnicode(const std::string &host, ConversionProcess conversionProcess, std::string &unicode)
{
    return ConvertIDN(host, conversionProcess, false, unicode);
}

int32_t ICUHelper::ConvertIDN(const std::string &input, ConversionProcess conversionProcess, bool toASCII,
                              std::string &output)
{
    if (input.empty() || input.length() > MAX_HOST_LENGTH) {
        return NETMANAGER_ERR_INVALID_PARAMETER;
    }
    int32_t options = 0;
    switch (conversionProcess) {
        case ConversionProcess::NO_CONFIGURATION:
            options = UIDNA_DEFAULT;
            break;
        case ConversionProcess::ALLOW_UNASSIGNED:
            options = UIDNA_ALLOW_UNASSIGNED;
            break;
        case ConversionProcess::USE_STD3_ASCII_RULES:
            options = UIDNA_USE_STD3_RULES;
            break;
        // LCOV_EXCL_START
        default:
            return NETMANAGER_ERR_INVALID_PARAMETER;
        // LCOV_EXCL_STOP
    }
    // UTF-8 input → ICU UnicodeString (UTF-16)
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::UnicodeString ustrInput = icu::UnicodeString::fromUTF8(icu::StringPiece(input));

    UParseError parseError;
    int32_t requiredLen = 0;
    if (toASCII) {
        requiredLen =
            uidna_IDNToASCII(ustrInput.getBuffer(), ustrInput.length(), nullptr, 0, options, &parseError, &errorCode);
    } else {
        requiredLen =
            uidna_IDNToUnicode(ustrInput.getBuffer(), ustrInput.length(), nullptr, 0, options, &parseError, &errorCode);
    }
    if (errorCode != U_BUFFER_OVERFLOW_ERROR || requiredLen <= 0) {
        return NETMANAGER_ERR_INTERNAL;
    }

    auto destBuffer = std::make_unique<UChar[]>(requiredLen + 1);
    int32_t len = 0;
    errorCode = U_ZERO_ERROR;
    if (toASCII) {
        len = uidna_IDNToASCII(ustrInput.getBuffer(), ustrInput.length(), destBuffer.get(), requiredLen + 1, options,
                               &parseError, &errorCode);
    } else {
        len = uidna_IDNToUnicode(ustrInput.getBuffer(), ustrInput.length(), destBuffer.get(), requiredLen + 1, options,
                                 &parseError, &errorCode);
    }
    if (U_FAILURE(errorCode) || len <= 0) {
        return NETMANAGER_ERR_INTERNAL;
    }

    // Convert UTF-16 output back to UTF-8 std::string
    icu::UnicodeString ustrResult(destBuffer.get(), len);
    ustrResult.toUTF8String(output);
    return NETMANAGER_SUCCESS;
}
} // namespace OHOS::NetManagerStandard

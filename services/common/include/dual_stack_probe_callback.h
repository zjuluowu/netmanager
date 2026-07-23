/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef I_DUAL_STACK_PROBE_CALLBACK_H
#define I_DUAL_STACK_PROBE_CALLBACK_H

namespace OHOS {
namespace NetManagerStandard {

enum DualStackProbeResultCode {
    PROBE_FAIL,
    PROBE_SUCCESS,
    PROBE_SUCCESS_IPV4,
    PROBE_SUCCESS_IPV6,
    PROBE_PORTAL,
};

class IDualStackProbeCallback {
public:
    IDualStackProbeCallback() = default;
    virtual ~IDualStackProbeCallback() = default;
    virtual int32_t OnHandleDualStackProbeResult(DualStackProbeResultCode result) = 0;
};
}
}
#endif // I_DUAL_STACK_PROBE_CALLBACK_H
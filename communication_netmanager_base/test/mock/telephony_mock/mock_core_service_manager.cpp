/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "mock_core_service_manager.h"

#include "core_service_client.h"

namespace OHOS {
namespace NetManagerStandard {
MockCoreServiceManager &MockCoreServiceManager::GetInstance()
{
    static MockCoreServiceManager gMockCoreServiceManager;
    return gMockCoreServiceManager;
}
}

namespace Telephony {
int32_t CoreServiceClient::GetSlotId(int32_t simId)
{
    return NetManagerStandard::MockCoreServiceManager::GetInstance().GetSlotId(simId);
}

int32_t CoreServiceClient::GetSimId(int32_t slotId)
{
    return NetManagerStandard::MockCoreServiceManager::GetInstance().GetSimId(slotId);
}

int32_t CoreServiceClient::GetSimIccId(int32_t slotId, std::u16string &iccId)
{
    return NetManagerStandard::MockCoreServiceManager::GetInstance().GetSimIccId(slotId, iccId);
}

}
}

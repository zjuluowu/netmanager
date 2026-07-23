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

#include <thread>

#include <securec.h>

#include "singleton.h"

#include "i_net_policy_service.h"
#include "net_conn_service_iface.h"
#include "net_mgr_log_wrapper.h"
#include "net_policy_constants.h"
#include "net_policy_service_common.h"
#include "net_stats_info.h"
#define private public
#include "net_manager_center.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
const uint8_t *g_baseFuzzData = nullptr;
static constexpr uint32_t CREATE_NET_TYPE_VALUE = 7;
static constexpr uint32_t CONVERT_NUMBER_TO_BOOL = 2;
size_t g_baseFuzzSize = 0;
size_t g_baseFuzzPos;
constexpr size_t STR_LEN = 10;
} // namespace

template <class T> T NetCommonGetData()
{
    T object{};
    size_t commonSize = sizeof(object);
    if (g_baseFuzzData == nullptr || commonSize > g_baseFuzzSize - g_baseFuzzPos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, commonSize, g_baseFuzzData + g_baseFuzzPos, commonSize);
    if (ret != EOK) {
        return {};
    }
    g_baseFuzzPos += commonSize;
    return object;
}

std::string NetCommonGetString(int strlen)
{
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        cstr[i] = NetCommonGetData<char>();
    }
    std::string str(cstr);
    return str;
}

bool IsCommonFuzzValidData(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    g_baseFuzzData = data;
    g_baseFuzzSize = size;
    g_baseFuzzPos = 0;
    return true;
}

static auto g_netManagerCenter = DelayedSingleton<NetManagerCenter>::GetInstance();

void GetIfaceNamesFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }

    uint32_t netType = NetCommonGetData<uint32_t>() % CREATE_NET_TYPE_VALUE;
    std::list<std::string> ifaceNames;
    g_netManagerCenter->GetIfaceNames(static_cast<NetBearType>(netType), ifaceNames);
    return;
}

void GetIfaceNameByTypeFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }

    uint32_t bearerType = NetCommonGetData<uint32_t>() % CREATE_NET_TYPE_VALUE;
    std::string ident = NetCommonGetString(STR_LEN);
    std::string ifaceName = NetCommonGetString(STR_LEN);
    g_netManagerCenter->GetIfaceNameByType(static_cast<NetBearType>(bearerType), ident, ifaceName);
}

void UnregisterNetSupplierFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    uint32_t supplierId = NetCommonGetData<uint32_t>();
    g_netManagerCenter->UnregisterNetSupplier(supplierId);
}

void UpdateNetLinkInfoFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    uint32_t supplierId = NetCommonGetData<uint32_t>();
    sptr<NetLinkInfo> netLinkInfo = new (std::nothrow) NetLinkInfo();
    if (netLinkInfo == nullptr) {
        return;
    }
    
    g_netManagerCenter->UpdateNetLinkInfo(supplierId, netLinkInfo);
}

void UpdateNetSupplierInfoFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    uint32_t supplierId = NetCommonGetData<uint32_t>();
    sptr<NetSupplierInfo> netSupplierInfo = new (std::nothrow) NetSupplierInfo();
    if (netSupplierInfo == nullptr) {
        return;
    }
    g_netManagerCenter->UpdateNetSupplierInfo(supplierId, netSupplierInfo);
}

__attribute__((no_sanitize("cfi"))) void RegisterConnServiceFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    sptr<NetConnServiceIface> serviceIface = new (std::nothrow) NetConnServiceIface();
    if (serviceIface == nullptr) {
        return;
    }
    g_netManagerCenter->RegisterConnService(serviceIface);
}

void GetIfaceStatsDetailFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    std::string iface = NetCommonGetString(STR_LEN);
    uint32_t start = NetCommonGetData<uint32_t>();
    uint32_t end = NetCommonGetData<uint32_t>() + start;
    NetStatsInfo info;
    g_netManagerCenter->GetIfaceStatsDetail(iface, start, end, info);
}

void ResetStatsFactoryFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }

    g_netManagerCenter->ResetStatsFactory();
}

void RegisterStatsServiceFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    sptr<NetStatsBaseService> service = nullptr;

    g_netManagerCenter->RegisterStatsService(service);
}

void ResetPolicyFactoryFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }

    g_netManagerCenter->ResetPolicyFactory();
}

void ResetPoliciesFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    g_netManagerCenter->ResetPolicies();
}

void RegisterPolicyServiceFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    sptr<NetPolicyBaseService> service = new (std::nothrow) NetPolicyServiceCommon();
    if (service == nullptr) {
        return;
    }

    g_netManagerCenter->RegisterPolicyService(service);
}

void ResetEthernetFactoryFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }

    g_netManagerCenter->ResetEthernetFactory();
}

void RegisterEthernetServiceFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    sptr<NetEthernetBaseService> service = nullptr;
    g_netManagerCenter->RegisterEthernetService(service);
}

void RestrictBackgroundChangedFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    bool isRestrictBackground = NetCommonGetData<uint32_t>() % CONVERT_NUMBER_TO_BOOL == 0;
    g_netManagerCenter->RestrictBackgroundChanged(isRestrictBackground);
}

void IsUidNetAccessFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    uint32_t uid = NetCommonGetData<uint32_t>();
    bool metered = NetCommonGetData<uint32_t>() % CONVERT_NUMBER_TO_BOOL == 0;
    g_netManagerCenter->IsUidNetAccess(uid, metered);
}

void IsUidNetAllowedFuzzTest(const uint8_t *data, size_t size)
{
    if (!IsCommonFuzzValidData(data, size)) {
        return;
    }
    uint32_t uid = NetCommonGetData<uint32_t>();
    bool metered = NetCommonGetData<uint32_t>() % CONVERT_NUMBER_TO_BOOL == 0;
    g_netManagerCenter->IsUidNetAllowed(uid, metered);
}

} // namespace NetManagerStandard
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::NetManagerStandard::RegisterConnServiceFuzzTest(data, size);
    OHOS::NetManagerStandard::RegisterStatsServiceFuzzTest(data, size);
    OHOS::NetManagerStandard::RegisterEthernetServiceFuzzTest(data, size);
    OHOS::NetManagerStandard::RegisterPolicyServiceFuzzTest(data, size);
    OHOS::NetManagerStandard::GetIfaceNamesFuzzTest(data, size);
    OHOS::NetManagerStandard::GetIfaceNameByTypeFuzzTest(data, size);
    OHOS::NetManagerStandard::UpdateNetLinkInfoFuzzTest(data, size);
    OHOS::NetManagerStandard::UpdateNetSupplierInfoFuzzTest(data, size);
    OHOS::NetManagerStandard::GetIfaceStatsDetailFuzzTest(data, size);
    OHOS::NetManagerStandard::ResetStatsFactoryFuzzTest(data, size);
    OHOS::NetManagerStandard::ResetPolicyFactoryFuzzTest(data, size);
    OHOS::NetManagerStandard::ResetPoliciesFuzzTest(data, size);
    OHOS::NetManagerStandard::ResetEthernetFactoryFuzzTest(data, size);
    OHOS::NetManagerStandard::RestrictBackgroundChangedFuzzTest(data, size);
    OHOS::NetManagerStandard::IsUidNetAccessFuzzTest(data, size);
    OHOS::NetManagerStandard::IsUidNetAllowedFuzzTest(data, size);
    OHOS::NetManagerStandard::UnregisterNetSupplierFuzzTest(data, size);

    return 0;
}
/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "net_bundle_impl.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "bundle_mgr_proxy.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "os_account_manager.h"

namespace OHOS {
namespace NetManagerStandard {
sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy()
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        NETMGR_LOG_E("fail to get system ability mgr.");
        return nullptr;
    }

    auto remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        NETMGR_LOG_E("fail to get bundle manager proxy.");
        return nullptr;
    }
    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}

int32_t NetBundleImpl::GetJsonFromBundle(std::string &jsonProfile)
{
    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return NETMANAGER_ERR_INTERNAL;
    }
    AppExecFwk::BundleInfo bundleInfo;
    auto ret = bundleMgrProxy->GetBundleInfoForSelf(0, bundleInfo);
    if (ret != ERR_OK) {
        NETMGR_LOG_I("GetSelfBundleName: bundleName get fail.");
        return NETMANAGER_ERR_INTERNAL;
    }
    ret = bundleMgrProxy->GetJsonProfile(AppExecFwk::ProfileType::NETWORK_PROFILE,
        bundleInfo.name, bundleInfo.entryModuleName, jsonProfile);
    if (ret != ERR_OK) {
        NETMGR_LOG_D("No network_config profile configured in bundle manager.[%{public}d]", ret);
        return NETMANAGER_SUCCESS;
    }
    return NETMANAGER_SUCCESS;
}

bool NetBundleImpl::IsAtomicService(std::string &bundleName)
{
    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return false;
    }
    AppExecFwk::BundleInfo bundleInfo;
    auto flags = AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION;
    auto ret = bundleMgrProxy->GetBundleInfoForSelf(static_cast<int32_t>(flags), bundleInfo);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("GetSelfBundleName: bundleName get fail.");
        return false;
    }
    bundleName = bundleInfo.applicationInfo.bundleName;
    return bundleInfo.applicationInfo.bundleType == AppExecFwk::BundleType::ATOMIC_SERVICE;
}

std::optional<std::string> NetBundleImpl::ObtainBundleNameForSelf()
{
    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return std::nullopt;
    }
    AppExecFwk::BundleInfo bundleInfo;
    auto flags = AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION;
    auto ret = bundleMgrProxy->GetBundleInfoForSelf(static_cast<int32_t>(flags), bundleInfo);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("bundleName get failed %{public}d.", ret);
        return std::nullopt;
    }
    return bundleInfo.applicationInfo.bundleName;
}

std::optional<int32_t> NetBundleImpl::ObtainTargetApiVersionForSelf()
{
    static constexpr int32_t API_VERSION_MOD = 1000;
    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("Failed to get bundle manager proxy.");
        return std::nullopt;
    }
    AppExecFwk::BundleInfo bundleInfo;
    auto flags = AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION;
    auto ret = bundleMgrProxy->GetBundleInfoForSelf(static_cast<int32_t>(flags), bundleInfo);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("GetBundleInfoForSelf: bundleName get failed %{public}d.", ret);
        return std::nullopt;
    }
    auto targetApiVersion = bundleInfo.applicationInfo.apiTargetVersion % API_VERSION_MOD;
    NETMGR_LOG_I("Got target API version %{public}d.", targetApiVersion);
    return targetApiVersion;
}

std::optional<std::unordered_map<uint32_t, SampleBundleInfo>> NetBundleImpl::ObtainBundleInfoForActive()
{
    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("ObtainBundleInfoForActive Failed to get bundle manager proxy.");
        return std::nullopt;
    }
    int32_t userId;
    if (GetActivatedOsAccountId(userId) != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("ObtainBundleInfoForActive Failed to get userid.");
        return std::nullopt;
    }
    auto flags = AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION;
    std::vector<AppExecFwk::BundleInfo> bundleInfos;
    auto ret = bundleMgrProxy->GetBundleInfosV9(static_cast<int32_t>(flags), bundleInfos, userId);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("ObtainBundleInfoForUid Failed GetBundleInfo. ret[%{public}d] userId[%{public}d]", ret,
                     userId);
        return std::nullopt;
    }
    std::unordered_map<uint32_t, SampleBundleInfo> result;
    for (const auto &bundleInfo : bundleInfos) {
        result.insert(
            std::make_pair(static_cast<uint32_t>(bundleInfo.applicationInfo.uid),
                           SampleBundleInfo{static_cast<uint32_t>(bundleInfo.applicationInfo.uid),
                                            bundleInfo.applicationInfo.bundleName,
                                            bundleInfo.applicationInfo.installSource, bundleInfo.installTime}));
    }
    return result;
}

std::optional<SampleBundleInfo> NetBundleImpl::ObtainBundleInfoForUid(uint32_t uid)
{
    auto bundleMgrProxy = GetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        NETMGR_LOG_E("ObtainBundleInfoForUid Failed to get bundle manager proxy.");
        return std::nullopt;
    }
    std::string bundleName;
    if (bundleMgrProxy->GetNameForUid(uid, bundleName) != ERR_OK) {
        NETMGR_LOG_E("ObtainBundleInfoForUid Failed to GetBundleName. uid[%{public}u]", uid);
        return std::nullopt;
    }
    int32_t userId;
    if (GetActivatedOsAccountId(userId) != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("ObtainBundleInfoForUid Failed to GetUserId.");
        return std::nullopt;
    }
    auto flags = AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION;
    AppExecFwk::BundleInfo bundleInfo;
    auto ret = bundleMgrProxy->GetBundleInfoV9(bundleName, static_cast<int32_t>(flags), bundleInfo, userId);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("ObtainBundleInfoForUid Failed. ret[%{public}d] userId[%{public}d], bundleName[%{public}s]", ret,
                     userId, bundleName.c_str());
        return std::nullopt;
    }
    return SampleBundleInfo{bundleInfo.applicationInfo.uid, bundleInfo.applicationInfo.bundleName,
                            bundleInfo.applicationInfo.installSource, bundleInfo.installTime};
}

int32_t NetBundleImpl::GetActivatedOsAccountId(int32_t &userId)
{
    std::vector<int32_t> activatedOsAccountIds;
    int ret = AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("QueryActiveOsAccountIds failed. ret is %{public}d", ret);
        return NETMANAGER_ERR_INTERNAL;
    }
    if (activatedOsAccountIds.empty()) {
        NETMGR_LOG_E("QueryActiveOsAccountIds is empty");
        return NETMANAGER_ERR_INTERNAL;
    }
    userId = activatedOsAccountIds[0];
    NETMGR_LOG_I("QueryActiveOsAccountIds is %{public}d", userId);
    return NETMANAGER_SUCCESS;
}

INetBundle *GetNetBundle()
{
    static NetBundleImpl impl;
    return &impl;
}

bool IsAtomicService(std::string &bundleName)
{
    NetBundleImpl impl;
    return impl.IsAtomicService(bundleName);
}
} // namespace NetManagerStandard
} // namespace OHOS

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

#include <atomic>
#include <fstream>
#include <memory>
#include <set>
#include <string_ex.h>
#include <string>

#include "extension_manager_client.h"
#include <message_parcel.h>

#include "net_stats_rdb.h"
#include "net_stats_trafficLimit_dialog.h"
#include "net_stats_service.h"
#include "net_stats_utils.h"
#include "cellular_data_client.h"
#include "core_service_client.h"
#include "cJSON.h"
#include "net_mgr_log_wrapper.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace NetManagerStandard {
constexpr int32_t INVALID_USERID = -1;
constexpr int32_t MESSAGE_PARCEL_KEY_SIZE = 3;
int32_t g_simId = 0;

TrafficLimitDialog::TrafficLimitDialog() {}
// LCOV_EXCL_START
TrafficLimitDialog::~TrafficLimitDialog()
{
    if (isDialogOpen_) {
        (void)UnShowTrafficLimitDialog();
    }
}

bool TrafficLimitDialog::PopUpTrafficLimitDialog(int32_t simId)
{
    g_simId = simId;
    isDialogOpen_ = true;
    return ShowTrafficLimitDialog();
}

void TrafficLimitDialog::TrafficLimitAbilityConn::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    NETMGR_LOG_I("TrafficLimitDialog::OnAbilityConnectDone");
    int32_t slotId = Telephony::CoreServiceClient::GetInstance().GetSlotId(g_simId);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInt32(MESSAGE_PARCEL_KEY_SIZE);
    data.WriteString16(u"bundleName");
    data.WriteString16(u"com.xxxxxx.hmos.communicationsetting");
    data.WriteString16(u"abilityName");
    data.WriteString16(u"DisableMobileDataDialogAbility");

    cJSON* paramJson = cJSON_CreateObject();
    std::string uiExtensionTypeStr = "sysDialog/common";

    cJSON_AddStringToObject(paramJson, "ability.want.params.uiExtensionType", uiExtensionTypeStr.c_str());
    cJSON_AddNumberToObject(paramJson, "sysDialogZOrder", 1);
    cJSON_AddNumberToObject(paramJson, "slotId", slotId);

    char *pParamJson = cJSON_PrintUnformatted(paramJson);
    
    if (!pParamJson) {
        NETMGR_LOG_I("Print paramJson error");
        cJSON_Delete(paramJson);
        cJSON_free(pParamJson);
        return;
    }
    std::string paramStr(pParamJson);
    data.WriteString16(u"parameters");
    data.WriteString16(Str8ToStr16(paramStr));
    cJSON_Delete(paramJson);
    cJSON_free(pParamJson);

    NETMGR_LOG_I("OnAbilityConnectDone : tmpParameters = %{public}s", paramStr.c_str());

    const uint32_t cmdCode = 1;
    int32_t ret = remoteObject->SendRequest(cmdCode, data, reply, option);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("TrafficLimit Dialog failed: ret=%{public}u", ret);
        return;
    }
    std::unique_lock<std::shared_mutex> lock(remoteObjectMutex_);
    remoteObject_ = remoteObject;
    return;
}

void TrafficLimitDialog::TrafficLimitAbilityConn::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName& element, int resultCode)
{
    NETMGR_LOG_I("TrafficLimitAbilityConn::OnAbilityDisconnectDone");
    std::unique_lock<std::shared_mutex> lock(remoteObjectMutex_);
    remoteObject_ = nullptr;
    return;
}

void TrafficLimitDialog::TrafficLimitAbilityConn::CloseDialog()
{
    std::shared_lock<std::shared_mutex> lock(remoteObjectMutex_);
    if (remoteObject_ == nullptr) {
        NETMGR_LOG_I("CloseDialog: disconnected");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    const uint32_t cmdCode = 3;  // 3：表示关闭弹窗
    int32_t ret = remoteObject_->SendRequest(cmdCode, data, reply, option);
    int32_t replyCode = -1;
    bool success = false;
    if (ret == ERR_OK) {
        success = reply.ReadInt32(replyCode);
    }
    NETMGR_LOG_I("CloseDialog: ret=%{public}d, %{public}d, %{public}d", ret, success, replyCode);
}

bool TrafficLimitDialog::ShowTrafficLimitDialog()
{
    NETMGR_LOG_I("Show TrafficLimit Dialog");
    std::lock_guard<std::mutex> guard(opMutex_);
    if (trafficlimitAbilityConn_ == nullptr) {
        trafficlimitAbilityConn_ = new (std::nothrow) TrafficLimitAbilityConn();
    }
    if (trafficlimitAbilityConn_ == nullptr) {
        NETMGR_LOG_E("TrafficLimitAbilityConn create failed");
        return false;
    }
    DelayedRefSingleton<Telephony::CellularDataClient>::GetInstance().EnableCellularData(false);

    Want want;
    want.SetElementName("com.ohos.sceneboard", "com.ohos.sceneboard.systemdialog");
    NETMGR_LOG_I("ConnectAbility start");
    auto ret = ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, trafficlimitAbilityConn_->AsObject(), INVALID_USERID);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("ConnectServiceExtensionAbility systemui failed");
        trafficlimitAbilityConn_ = nullptr;
        return false;
    }

    return true;
}

bool TrafficLimitDialog::UnShowTrafficLimitDialog()
{
    std::lock_guard<std::mutex> guard(opMutex_);
    if (trafficlimitAbilityConn_ == nullptr) {
        return true;
    }

    NETMGR_LOG_I("Unshow TrafficLimit Dialog");
    trafficlimitAbilityConn_->CloseDialog();

    auto ret = ExtensionManagerClient::GetInstance().DisconnectAbility(trafficlimitAbilityConn_->AsObject());
    if (ret != 0) {
        NETMGR_LOG_E("DisconnectAbility failed %{public}d", ret);
        return false;
    }
    NETMGR_LOG_I("Unshow TrafficLimit Dialog success");
    return true;
}
// LCOV_EXCL_STOP
} // namespace NetManagerStandard
} // namespace OHOS

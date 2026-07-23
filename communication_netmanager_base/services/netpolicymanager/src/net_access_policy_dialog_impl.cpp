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

#include "net_access_policy_dialog_impl.h"

#include <atomic>
#include <fstream>
#include <memory>
#include <set>
#include <string_ex.h>
#include <string>

#include "extension_manager_client.h"
#include <message_parcel.h>
#include "net_mgr_log_wrapper.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace NetManagerStandard {
namespace {
static constexpr int32_t INVALID_USERID = -1;
constexpr int32_t SIGNAL_NUM = 3;
sptr<IRemoteObject> g_remoteObject = nullptr;
uint32_t g_uid = 0;
std::atomic_bool g_isDialogShow = false;
} // namespace

NetAccessPolicyDialogImpl::NetAccessPolicyDialogImpl() : dialogConnectionCallback_(new DialogAbilityConnection()) {}

NetAccessPolicyDialogImpl::~NetAccessPolicyDialogImpl()
{
    dialogConnectionCallback_ = nullptr;
}

bool NetAccessPolicyDialogImpl::ConnectSystemUi(uint32_t uid)
{
    NETMGR_LOG_I("OnAbilityConnectDone");

    g_uid = uid;
    if (g_isDialogShow) {
        AppExecFwk::ElementName element;
        dialogConnectionCallback_->OnAbilityConnectDone(element, g_remoteObject, INVALID_USERID);
        NETMGR_LOG_I("network access policy dialog has been show");
        return true;
    }

    Want want;
    want.SetElementName("com.ohos.sceneboard", "com.ohos.sceneboard.systemdialog");
    ErrCode result = ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, dialogConnectionCallback_->AsObject(), INVALID_USERID);
    if (result != ERR_OK) {
        NETMGR_LOG_E("ConnectAbility err");
        return false;
    }

    return true;
}

void NetAccessPolicyDialogImpl::DialogAbilityConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int resultCode)
{
    NETMGR_LOG_I("OnAbilityConnectDone");
    std::lock_guard lock(mutex_);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (g_remoteObject == nullptr) {
        g_remoteObject = remoteObject;
    }

    std::string parameters =
        "{\"ability.want.params.uiExtensionType\":\"sysDialog/common\",\"sysDialogZOrder\":1, \"appUid\":";
    std::string tmpParameters = parameters + std::to_string(g_uid) + "}";
    data.WriteInt32(SIGNAL_NUM);
    data.WriteString16(u"bundleName");
    data.WriteString16(u"com.example.myapplication");
    data.WriteString16(u"abilityName");
    data.WriteString16(u"UIExtensionProvider");
    data.WriteString16(u"parameters");
    data.WriteString16(Str8ToStr16(tmpParameters));
 
    int32_t ret = remoteObject->SendRequest(IAbilityConnection::ON_ABILITY_CONNECT_DONE, data, reply, option);
    if (ret != ERR_OK) {
        NETMGR_LOG_E("Show power dialog is failed:%{public}d", ret);
        return;
    }
    g_isDialogShow = true;
    NETMGR_LOG_I("Show network access policy dialog is success");
}

void NetAccessPolicyDialogImpl::DialogAbilityConnection::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName& element, int resultCode)
{
    NETMGR_LOG_I("OnAbilityDisconnectDone");
    std::lock_guard lock(mutex_);
    g_remoteObject = nullptr;
    g_isDialogShow = false;
}

INetAccessPolicyDialog *GetNetAccessPolicyDialog()
{
    static NetAccessPolicyDialogImpl impl;
    return &impl;
}
} // namespace NetManagerStandard
} // namespace OHOS

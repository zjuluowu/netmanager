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

#include "connection_exec.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <mutex>
#include <condition_variable>

#include "constant.h"
#include "errorcode_convertor.h"
#include "napi_utils.h"
#include "net_conn_callback_observer.h"
#include "net_interface_callback_observer.h"
#include "net_conn_client.h"
#include "net_handle_interface.h"
#include "net_manager_constants.h"
#include "netconnection.h"
#include "netinterface.h"
#include "netmanager_base_common_utils.h"
#include "netmanager_base_log.h"
#include "securec.h"
#include "hi_app_event_report.h"
#include "icu_helper.h"
#include "net_probe.h"

namespace OHOS::NetManagerStandard {
std::mutex g_predefinedHostMtx;

namespace {
constexpr int32_t NO_PERMISSION_CODE = 1;
constexpr int32_t PERMISSION_DENIED_CODE = 13;
constexpr int32_t NET_UNREACHABLE_CODE = 101;
constexpr int32_t INVALID_UID = -1;
constexpr int32_t REFRESH_HTTP_PROXY_MAX_TIME = 15;
static int64_t g_limitSdkReport = 0;
static int64_t g_limitSdkReports = 0;
} // namespace

napi_value ConnectionExec::CreateNetHandle(napi_env env, NetHandle *handle)
{
    napi_value netHandle = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, netHandle) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }

    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_WRITABLE_NAPI_FUNCTION(NetHandleInterface::FUNCTION_GET_ADDRESSES_BY_NAME,
            NetHandleInterface::GetAddressesByName),
        DECLARE_WRITABLE_NAPI_FUNCTION(NetHandleInterface::FUNCTION_GET_ADDRESS_BY_NAME,
            NetHandleInterface::GetAddressByName),
        DECLARE_WRITABLE_NAPI_FUNCTION(NetHandleInterface::FUNCTION_GET_ADDRESSES_BY_NAME_WITH_OPTION,
            NetHandleInterface::GetAddressesByNameWithOptions),
        DECLARE_NAPI_FUNCTION(NetHandleInterface::FUNCTION_BIND_SOCKET,
            NetHandleInterface::BindSocket),
    };
    NapiUtils::DefineProperties(env, netHandle, properties);
    NapiUtils::SetUint32Property(env, netHandle, NetHandleInterface::PROPERTY_NET_ID, handle->GetNetId());
    return netHandle;
}

napi_value ConnectionExec::CreateNetCapabilities(napi_env env, NetAllCapabilities *capabilities)
{
    napi_value netCapabilities = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, netCapabilities) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }

    NapiUtils::SetUint32Property(env, netCapabilities, KEY_LINK_UP_BAND_WIDTH_KPS, capabilities->linkUpBandwidthKbps_);
    NapiUtils::SetUint32Property(env, netCapabilities, KEY_LINK_DOWN_BAND_WIDTH_KPS,
                                 capabilities->linkDownBandwidthKbps_);
    if (!capabilities->netCaps_.empty() && capabilities->netCaps_.size() <= MAX_ARRAY_LENGTH) {
        napi_value networkCap = NapiUtils::CreateArray(env, std::min(capabilities->netCaps_.size(), MAX_ARRAY_LENGTH));
        auto it = capabilities->netCaps_.begin();
        for (uint32_t index = 0; index < MAX_ARRAY_LENGTH && it != capabilities->netCaps_.end(); ++index, ++it) {
            NapiUtils::SetArrayElement(env, networkCap, index, NapiUtils::CreateUint32(env, *it));
        }
        NapiUtils::SetNamedProperty(env, netCapabilities, KEY_NETWORK_CAP, networkCap);
    }
    if (!capabilities->bearerTypes_.empty() && capabilities->bearerTypes_.size() <= MAX_ARRAY_LENGTH) {
        napi_value bearerTypes =
            NapiUtils::CreateArray(env, std::min(capabilities->bearerTypes_.size(), MAX_ARRAY_LENGTH));
        auto it = capabilities->bearerTypes_.begin();
        for (uint32_t index = 0; index < MAX_ARRAY_LENGTH && it != capabilities->bearerTypes_.end(); ++index, ++it) {
            NapiUtils::SetArrayElement(env, bearerTypes, index, NapiUtils::CreateUint32(env, *it));
        }
        NapiUtils::SetNamedProperty(env, netCapabilities, KEY_BEARER_TYPE, bearerTypes);
    }
    return netCapabilities;
}

napi_value ConnectionExec::CreateConnectionProperties(napi_env env, NetLinkInfo *linkInfo)
{
    napi_value connectionProperties = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, connectionProperties) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    NapiUtils::SetStringPropertyUtf8(env, connectionProperties, KEY_INTERFACE_NAME, linkInfo->ifaceName_);
    NapiUtils::SetStringPropertyUtf8(env, connectionProperties, KEY_DOMAINS, linkInfo->domain_);
    NapiUtils::SetUint32Property(env, connectionProperties, KEY_MTU, linkInfo->mtu_);
    NapiUtils::SetBooleanProperty(env, connectionProperties, KEY_IS_IPV4_LINK_VALID, linkInfo->isIpv4LinkValid_);
    NapiUtils::SetBooleanProperty(env, connectionProperties, KEY_IS_IPV6_LINK_VALID, linkInfo->isIpv6LinkValid_);
    FillLinkAddress(env, connectionProperties, linkInfo);
    FillRouoteList(env, connectionProperties, linkInfo);
    FillDns(env, connectionProperties, linkInfo);
    return connectionProperties;
}

napi_value ConnectionExec::CreateIpNeighTable(napi_env env, const NetIpMacInfo &ipNeighTable)
{
    napi_value jsIpNeigh = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, jsIpNeigh) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }

    napi_value jsNetAddress = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, jsNetAddress) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }

    FillNetAddressInfo(env, jsNetAddress, ipNeighTable);
    NapiUtils::SetNamedProperty(env, jsIpNeigh, KEY_IP_ADDRESS, jsNetAddress);
    NapiUtils::SetStringPropertyUtf8(env, jsIpNeigh, KEY_IFACE_NAME, ipNeighTable.iface_);
    NapiUtils::SetStringPropertyUtf8(env, jsIpNeigh, KEY_MAC_ADDRESS, ipNeighTable.macAddress_);
    return jsIpNeigh;
}

napi_value ConnectionExec::CreateNetPortStatesInfo(napi_env env, const NetPortStatesInfo &netPortStatesInfo)
{
    napi_value netPortStatesInfoObject = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, netPortStatesInfoObject) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }

    napi_value tcpArray = NapiUtils::CreateArray(env, netPortStatesInfo.tcpNetPortStatesInfo_.size());
    uint32_t tcpArrayIndex = 0;
    for (const auto &tcpInfo : netPortStatesInfo.tcpNetPortStatesInfo_) {
        napi_value item = NapiUtils::CreateObject(env);
        NapiUtils::SetStringPropertyUtf8(env, item, KEY_TCP_LOCAL_IP, tcpInfo.tcpLocalIp_);
        NapiUtils::SetUint32Property(env, item, KEY_TCP_LOCAL_PORT, tcpInfo.tcpLocalPort_);
        NapiUtils::SetStringPropertyUtf8(env, item, KEY_TCP_REOMTE_IP, tcpInfo.tcpRemoteIp_);
        NapiUtils::SetUint32Property(env, item, KEY_TCP_REMOTE_PORT, tcpInfo.tcpRemotePort_);
        NapiUtils::SetUint32Property(env, item, KEY_TCP_UID, tcpInfo.tcpUid_);
        NapiUtils::SetUint32Property(env, item, KEY_TCP_PID, tcpInfo.tcpPid_);
        NapiUtils::SetUint32Property(env, item, KEY_TCP_STATE, tcpInfo.tcpState_);
        NapiUtils::SetArrayElement(env, tcpArray, tcpArrayIndex++, item);
    }
    NapiUtils::SetNamedProperty(env, netPortStatesInfoObject, KEY_TCP_PORT_STATES_INFO, tcpArray);

    napi_value udpArray = NapiUtils::CreateArray(env, netPortStatesInfo.udpNetPortStatesInfo_.size());
    uint32_t udpArrayIndex = 0;
    for (const auto &udpInfo : netPortStatesInfo.udpNetPortStatesInfo_) {
        napi_value item = NapiUtils::CreateObject(env);
        NapiUtils::SetStringPropertyUtf8(env, item, KEY_UDP_LOCAL_IP, udpInfo.udpLocalIp_);
        NapiUtils::SetUint32Property(env, item, KEY_UDP_LOCAL_PORT, udpInfo.udpLocalPort_);
        NapiUtils::SetUint32Property(env, item, KEY_UDP_UID, udpInfo.udpUid_);
        NapiUtils::SetUint32Property(env, item, KEY_UDP_PID, udpInfo.udpPid_);
        NapiUtils::SetArrayElement(env, udpArray, udpArrayIndex++, item);
    }
    NapiUtils::SetNamedProperty(env, netPortStatesInfoObject, KEY_UDP_PORT_STATES_INFO, udpArray);

    return netPortStatesInfoObject;
}

bool ConnectionExec::ExecGetAddressByName(GetAddressByNameContext *context)
{
    return NetHandleExec::ExecGetAddressesByName(context);
}

napi_value ConnectionExec::GetAddressByNameCallback(GetAddressByNameContext *context)
{
    return NetHandleExec::GetAddressesByNameCallback(context);
}

bool ConnectionExec::ExecGetAddressesByNameWithOptions(GetAddressByNameWithOptionsContext *context)
{
    return NetHandleExec::ExecGetAddressesByNameWithOptions(context);
}

napi_value ConnectionExec::GetAddressesByNameWithOptionsCallback(GetAddressByNameWithOptionsContext *context)
{
    return NetHandleExec::GetAddressesByNameWithOptionsCallback(context);
}

bool ConnectionExec::ExecGetDefaultNet(GetDefaultNetContext *context)
{
    auto ret = NetConnClient::GetInstance().GetDefaultNet(context->netHandle_);
    if (ret != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("get default net failed %{public}d", ret);
        context->SetErrorCode(ret);
    }
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::GetDefaultNetCallback(GetDefaultNetContext *context)
{
    return CreateNetHandle(context->GetEnv(), &context->netHandle_);
}

bool ConnectionExec::ExecHasDefaultNet(HasDefaultNetContext *context)
{
    auto ret = NetConnClient::GetInstance().HasDefaultNet(context->hasDefaultNet_);
    if (ret != NETMANAGER_SUCCESS && ret != NET_CONN_ERR_NO_DEFAULT_NET) {
        NETMANAGER_BASE_LOGE("ExecHasDefaultNet ret %{public}d", ret);
        context->SetErrorCode(ret);
        return false;
    }
    return true;
}

napi_value ConnectionExec::HasDefaultNetCallback(HasDefaultNetContext *context)
{
    return NapiUtils::GetBoolean(context->GetEnv(), context->hasDefaultNet_);
}

bool ConnectionExec::ExecIsDefaultNetMetered(IsDefaultNetMeteredContext *context)
{
    auto ret = NetConnClient::GetInstance().IsDefaultNetMetered(context->isMetered_);
    if (ret != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("get net metered status failed %{public}d", ret);
        context->SetErrorCode(ret);
    }
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::IsDefaultNetMeteredCallback(IsDefaultNetMeteredContext *context)
{
    return NapiUtils::GetBoolean(context->GetEnv(), context->isMetered_);
}

bool ConnectionExec::ExecGetNetCapabilities(GetNetCapabilitiesContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    auto ret = NetConnClient::GetInstance().GetNetCapabilities(context->netHandle_, context->capabilities_);
    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::GetNetCapabilitiesCallback(GetNetCapabilitiesContext *context)
{
    return CreateNetCapabilities(context->GetEnv(), &context->capabilities_);
}

bool ConnectionExec::ExecGetConnectionProperties(GetConnectionPropertiesContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    auto ret = NetConnClient::GetInstance().GetConnectionProperties(context->netHandle_, context->linkInfo_);
    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::GetConnectionPropertiesCallback(GetConnectionPropertiesContext *context)
{
    return CreateConnectionProperties(context->GetEnv(), &context->linkInfo_);
}

bool ConnectionExec::ExecGetAllNets(GetAllNetsContext *context)
{
    int32_t ret = NetConnClient::GetInstance().GetAllNets(context->netHandleList_);
    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::GetAllNetsCallback(GetAllNetsContext *context)
{
    napi_value array = NapiUtils::CreateArray(context->GetEnv(), context->netHandleList_.size());
    uint32_t index = 0;
    std::for_each(context->netHandleList_.begin(), context->netHandleList_.end(),
                  [array, &index, context](const sptr<NetHandle> &handle) {
                      NapiUtils::SetArrayElement(context->GetEnv(), array, index,
                                                 CreateNetHandle(context->GetEnv(), handle.GetRefPtr()));
                      ++index;
                  });
    return array;
}

bool ConnectionExec::ExecEnableAirplaneMode(EnableAirplaneModeContext *context)
{
    int32_t res = NetConnClient::GetInstance().SetAirplaneMode(true);
    if (res != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecEnableAirplaneMode failed %{public}d", res);
        context->SetErrorCode(res);
    }
    return res == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::EnableAirplaneModeCallback(EnableAirplaneModeContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecDisableAirplaneMode(DisableAirplaneModeContext *context)
{
    int32_t res = NetConnClient::GetInstance().SetAirplaneMode(false);
    if (res != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecDisableAirplaneMode failed %{public}d", res);
        context->SetErrorCode(res);
    }
    return res == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::DisableAirplaneModeCallback(DisableAirplaneModeContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecReportNetConnected(ReportNetConnectedContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    int32_t res = NetConnClient::GetInstance().NetDetection(context->netHandle_);
    if (res != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecReportNetConnected failed %{public}d", res);
        context->SetErrorCode(res);
    }
    return res == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::ReportNetConnectedCallback(ReportNetConnectedContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecReportNetDisconnected(ReportNetConnectedContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    int32_t res = NetConnClient::GetInstance().NetDetection(context->netHandle_);
    if (res != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecReportNetDisconnected failed %{public}d", res);
        context->SetErrorCode(res);
    }
    return res == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::ReportNetDisconnectedCallback(ReportNetConnectedContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecGetDefaultHttpProxy(GetHttpProxyContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().GetDefaultHttpProxy(context->httpProxy_);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetDefaultHttpProxyCallback(GetHttpProxyContext *context)
{
    napi_value host = NapiUtils::CreateStringUtf8(context->GetEnv(), context->httpProxy_.GetHost());
    napi_value port = NapiUtils::CreateInt32(context->GetEnv(), context->httpProxy_.GetPort());
    auto lists = context->httpProxy_.GetExclusionList();
    napi_value exclusionList = NapiUtils::CreateArray(context->GetEnv(), lists.size());
    size_t index = 0;
    for (auto list : lists) {
        napi_value jsList = NapiUtils::CreateStringUtf8(context->GetEnv(), list);
        NapiUtils::SetArrayElement(context->GetEnv(), exclusionList, index++, jsList);
    }
    napi_value httpProxy = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "host", host);
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "port", port);
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "exclusionList", exclusionList);
    return httpProxy;
}

bool ConnectionExec::ExecGetGlobalHttpProxy(GetHttpProxyContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().GetGlobalHttpProxy(context->httpProxy_);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetGlobalHttpProxyCallback(GetHttpProxyContext *context)
{
    napi_value host = NapiUtils::CreateStringUtf8(context->GetEnv(), context->httpProxy_.GetHost());
    napi_value port = NapiUtils::CreateInt32(context->GetEnv(), context->httpProxy_.GetPort());
    auto lists = context->httpProxy_.GetExclusionList();
    napi_value exclusionList = NapiUtils::CreateArray(context->GetEnv(), lists.size());
    size_t index = 0;
    for (auto list : lists) {
        napi_value jsList = NapiUtils::CreateStringUtf8(context->GetEnv(), list);
        NapiUtils::SetArrayElement(context->GetEnv(), exclusionList, index++, jsList);
    }
    napi_value httpProxy = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "host", host);
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "port", port);
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "exclusionList", exclusionList);
    return httpProxy;
}

bool ConnectionExec::ExecSetGlobalHttpProxy(SetGlobalHttpProxyContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().SetGlobalHttpProxy(context->httpProxy_);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::SetGlobalHttpProxyCallback(SetGlobalHttpProxyContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecRefreshGlobalHttpProxy(GetHttpProxyContext *context)
{
    auto mtx = std::make_shared<std::mutex>();
    auto cv = std::make_shared<std::condition_variable>();
    auto ready = std::make_shared<bool>(false);
    auto result = std::make_shared<int32_t>(NETMANAGER_SUCCESS);
    auto httpProxy = std::make_shared<HttpProxy>();

    auto callback = [mtx, cv, ready, result, httpProxy](int32_t ret, const HttpProxy &proxy) {
        std::lock_guard<std::mutex> lock(*mtx);
        *result = ret;
        *httpProxy = proxy;
        *ready = true;
        cv->notify_one();
    };

    int32_t ret = NetConnClient::GetInstance().RefreshGlobalHttpProxy(callback);
    if (ret != NETMANAGER_SUCCESS) {
        context->SetErrorCode(ret);
        return false;
    }

    std::unique_lock<std::mutex> lock(*mtx);
    cv->wait_for(lock, std::chrono::seconds(REFRESH_HTTP_PROXY_MAX_TIME), [&ready] { return *ready; });

    if (*result != NETMANAGER_SUCCESS) {
        context->SetErrorCode(*result);
        return false;
    }
    context->httpProxy_ = *httpProxy;
    return true;
}

napi_value ConnectionExec::RefreshGlobalHttpProxyCallback(GetHttpProxyContext *context)
{
    napi_value host = NapiUtils::CreateStringUtf8(context->GetEnv(), context->httpProxy_.GetHost());
    napi_value port = NapiUtils::CreateInt32(context->GetEnv(), context->httpProxy_.GetPort());
    auto lists = context->httpProxy_.GetExclusionList();
    napi_value exclusionList = NapiUtils::CreateArray(context->GetEnv(), lists.size());
    size_t index = 0;
    for (auto list : lists) {
        napi_value jsList = NapiUtils::CreateStringUtf8(context->GetEnv(), list);
        NapiUtils::SetArrayElement(context->GetEnv(), exclusionList, index++, jsList);
    }
    napi_value httpProxy = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "host", host);
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "port", port);
    NapiUtils::SetNamedProperty(context->GetEnv(), httpProxy, "exclusionList", exclusionList);
    return httpProxy;
}

bool ConnectionExec::ExecSetAppHttpProxy(SetAppHttpProxyContext *context)
{
    auto hiAppEventReport = std::make_shared<HiAppEventReport>("NetworkKit", "ConnectionSetAppHttpProxy");
    int32_t errorCode = NetConnClient::GetInstance().SetAppHttpProxy(context->httpProxy_);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        hiAppEventReport->ReportSdkEvent(RESULT_SUCCESS, errorCode);
        return false;
    }
    hiAppEventReport->ReportSdkEvent(RESULT_SUCCESS, NET_CONN_SUCCESS);
    return true;
}

napi_value ConnectionExec::SetAppHttpProxyCallback(SetAppHttpProxyContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecGetAppNet(GetAppNetContext *context)
{
    int32_t netId = 0;
    int32_t errorCode = NetConnClient::GetInstance().GetAppNet(netId);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec getAppNet failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    context->netHandle_.SetNetId(netId);
    return true;
}

napi_value ConnectionExec::GetAppNetCallback(GetAppNetContext *context)
{
    return CreateNetHandle(context->GetEnv(), &context->netHandle_);
}

bool ConnectionExec::ExecSetAppNet(SetAppNetContext *context)
{
    auto hiAppEventReport = std::make_shared<HiAppEventReport>("NetworkKit", "ConnectionSetAppNet");
    NETMANAGER_BASE_LOGI("into");
    int32_t errorCode = NetConnClient::GetInstance().SetAppNet(context->netHandle_.GetNetId());
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec setAppNet failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        hiAppEventReport->ReportSdkEvent(RESULT_SUCCESS, errorCode);
        return false;
    }
    hiAppEventReport->ReportSdkEvent(RESULT_SUCCESS, NET_CONN_SUCCESS);
    return true;
}

napi_value ConnectionExec::SetAppNetCallback(SetAppNetContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecGetPacUrl(GetPacUrlContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().GetPacUrl(context->pacUrl_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec GetPacUrl failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetPacUrlCallback(GetPacUrlContext *context)
{
    return NapiUtils::CreateStringUtf8(context->GetEnv(), context->pacUrl_);
}

bool ConnectionExec::ExecSetPacUrl(SetPacUrlContext *context)
{
    if (context->pacUrl_.empty()) {
        NETMANAGER_BASE_LOGE("pac Url is empty!");
        context->SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return false;
    }
    int32_t errorCode = NetConnClient::GetInstance().SetPacUrl(context->pacUrl_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec SetPacUrl failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::SetPacUrlCallback(SetPacUrlContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecSetNetExtAttribute(SetNetExtAttributeContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    int32_t errorCode = NetConnClient::GetInstance().SetNetExtAttribute(context->netHandle_, context->netExtAttribute_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec SetNetExtAttribute failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::SetNetExtAttributeCallback(SetNetExtAttributeContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecGetNetExtAttribute(GetNetExtAttributeContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    int32_t errorCode = NetConnClient::GetInstance().GetNetExtAttribute(context->netHandle_, context->netExtAttribute_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec GetNetExtAttribute failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetNetExtAttributeCallback(GetNetExtAttributeContext *context)
{
    return NapiUtils::CreateStringUtf8(context->GetEnv(), context->netExtAttribute_);
}

bool ConnectionExec::ExecSetCustomDNSRule(SetCustomDNSRuleContext *context)
{
    if (context == nullptr) {
        NETMANAGER_BASE_LOGE("context is nullptr");
        return false;
    }

    if (!CommonUtils::HasInternetPermission()) {
        context->SetErrorCode(NETMANAGER_ERR_PERMISSION_DENIED);
        return false;
    }

    if (context->host_.empty() || context->ip_.empty()) {
        context->SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return false;
    }

    std::vector<std::string> ip = context->ip_;
    for (size_t i = 0; i < ip.size(); i++) {
        if (!CommonUtils::IsValidIPV4(ip[i]) && !CommonUtils::IsValidIPV6(ip[i])) {
            context->SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
            return false;
        }
    }

    if (!context->IsParseOK()) {
        context->SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return false;
    }

    std::string host_ips = context->host_ + ",";
    for (size_t i = 0; i < ip.size(); i++) {
        host_ips.append(ip[i]);
        if (i < ip.size() - 1) {
            host_ips.append(",");
        }
    }

    std::lock_guard<std::mutex> lock(g_predefinedHostMtx);
    NETMANAGER_BASE_LOGI("set host with ip addr");
    int res = predefined_host_set_hosts(host_ips.c_str());
    if (res != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetCustomDNSRule failed %{public}d", res);
        context->SetErrorCode(res);
        return false;
    }

    return true;
}

napi_value ConnectionExec::SetCustomDNSRuleCallback(SetCustomDNSRuleContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecDeleteCustomDNSRule(DeleteCustomDNSRuleContext *context)
{
    if (context == nullptr) {
        NETMANAGER_BASE_LOGE("context is nullptr");
        return false;
    }
    if (!CommonUtils::HasInternetPermission()) {
        context->SetErrorCode(NETMANAGER_ERR_PERMISSION_DENIED);
        return false;
    }

    if (context->host_.empty()) {
        context->SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return false;
    }

    if (!context->IsParseOK()) {
        context->SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return false;
    }

    std::lock_guard<std::mutex> lock(g_predefinedHostMtx);
    NETMANAGER_BASE_LOGI("delete host with ip addr");
    int res = predefined_host_remove_host(context->host_.c_str());
    if (res != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecDeleteCustomDNSRule failed %{public}d", res);
        context->SetErrorCode(res);
        return false;
    }
    return true;
}

napi_value ConnectionExec::DeleteCustomDNSRuleCallback(DeleteCustomDNSRuleContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecDeleteCustomDNSRules(DeleteCustomDNSRulesContext *context)
{
    if (context == nullptr) {
        NETMANAGER_BASE_LOGE("context is nullptr");
        return false;
    }
    if (!CommonUtils::HasInternetPermission()) {
        context->SetErrorCode(NETMANAGER_ERR_PERMISSION_DENIED);
        return false;
    }

    if (!context->IsParseOK()) {
        context->SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return false;
    }

    std::lock_guard<std::mutex> lock(g_predefinedHostMtx);
    int res = predefined_host_clear_all_hosts();
    if (res != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecDeleteCustomDNSRules failed %{public}d", res);
        context->SetErrorCode(res);
        return false;
    }

    return true;
}

napi_value ConnectionExec::DeleteCustomDNSRulesCallback(DeleteCustomDNSRulesContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecSetInterfaceUp(SetInterfaceUpContext *context)
{
    NETMANAGER_BASE_LOGI("ExecSetInterfaceUp");
    int32_t errorCode = NetConnClient::GetInstance().SetInterfaceUp(context->interface_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec setInterfaceUp failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::SetInterfaceUpCallback(SetInterfaceUpContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecSetInterfaceIpAddr(SetInterfaceIpAddrContext *context)
{
    NETMANAGER_BASE_LOGI("ExecSetInterfaceIpAddr");
    int32_t errorCode = NetConnClient::GetInstance().SetNetInterfaceIpAddress(
        context->interface_, context->ipAddr_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec setInterfaceIpAddr failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::SetInterfaceIpAddrCallback(SetInterfaceIpAddrContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecGetIpNeighTable(GetIpNeighTableContext *context)
{
    NETMANAGER_BASE_LOGI("ExecGetIpNeighTable");
    int32_t errorCode = NetConnClient::GetInstance().GetIpNeighTable(context->ipMacInfo_);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetIpNeighTableCallback(GetIpNeighTableContext *context)
{
    napi_value array = NapiUtils::CreateArray(context->GetEnv(), context->ipMacInfo_.size());
    if (context->ipMacInfo_.empty()) {
        NETMANAGER_BASE_LOGE("ip neighbor table is empty!");
        return array;
    }
    uint32_t index = 0;
    std::for_each(context->ipMacInfo_.begin(), context->ipMacInfo_.end(),
        [array, &index, context](const NetIpMacInfo &ipMacInfo) {
        NapiUtils::SetArrayElement(context->GetEnv(), array, index, CreateIpNeighTable(context->GetEnv(), ipMacInfo));
        ++index;
    });
    return array;
}

bool ConnectionExec::ExecCreateVlan(CreateVlanContext *context)
{
    NETMANAGER_BASE_LOGI("ExecCreateVlan");
    if (context == nullptr) {
        return false;
    }
    int32_t errorCode = NetConnClient::GetInstance().CreateVlan(context->ifName_, context->vlanId_);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::CreateVlanCallback(CreateVlanContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecDestroyVlan(DestroyVlanContext *context)
{
    NETMANAGER_BASE_LOGI("ExecDestroyVlan");
    if (context == nullptr) {
        return false;
    }
    int32_t errorCode = NetConnClient::GetInstance().DestroyVlan(context->ifName_, context->vlanId_);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::DestroyVlanCallback(DestroyVlanContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecAddVlanIp(AddVlanIpContext *context)
{
    NETMANAGER_BASE_LOGI("ExecAddVlanIp");
    if (context == nullptr) {
        return false;
    }
    std::string ip = context->address_.address_;
    uint32_t mask = context->address_.prefixlen_;
    int32_t errorCode = NetConnClient::GetInstance().AddVlanIp(context->ifName_, context->vlanId_, ip, mask);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::AddVlanIpCallback(AddVlanIpContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecDeleteVlanIp(DeleteVlanIpContext *context)
{
    NETMANAGER_BASE_LOGI("ExecDeleteVlanIp");
    if (context == nullptr) {
        return false;
    }
    std::string ip = context->address_.address_;
    uint32_t mask = context->address_.prefixlen_;
    int32_t errorCode = NetConnClient::GetInstance().DeleteVlanIp(context->ifName_, context->vlanId_, ip, mask);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::DeleteVlanIpCallback(DeleteVlanIpContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecGetDnsASCII(GetDnsContext *context)
{
    NETMANAGER_BASE_LOGI("ExecGetDnsASCII");
    auto host = context->GetHost();
    if (host.empty()) {
        NETMANAGER_BASE_LOGE("host is empty");
        return true;
    }
    std::string ascii = "";
    int32_t errorCode = ICUHelper::GetDnsASCII(host, context->GetConversionProcess(), ascii);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec GetDnsASCII failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    context->SetHost(ascii);
    return true;
}

bool ConnectionExec::ExecGetConnectOwnerUid(GetConnectOwnerUidContext *context)
{
    NETMANAGER_BASE_LOGI("ExecGetConnectOwnerUid");
    if (context == nullptr) {
        NETMANAGER_BASE_LOGE("context is nullptr");
        return false;
    }

    if (!context->IsParseOK()) {
        context->SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return false;
    }
    NetConnInfo netConnInfo;
    netConnInfo.protocolType_ = context->protocolType_;
    netConnInfo.family_ = static_cast<NetConnInfo::Family>(context->localAddress_.GetJsValueFamily());
    netConnInfo.localAddress_ = context->localAddress_.GetAddress();
    netConnInfo.localPort_ = context->localAddress_.GetPort();
    netConnInfo.remoteAddress_ = context->remoteAddress_.GetAddress();
    netConnInfo.remotePort_ = context->remoteAddress_.GetPort();
    int32_t errorCode = NetConnClient::GetInstance().GetConnectOwnerUid(netConnInfo, context->ownerUid_);
    if (errorCode != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec getConnectOwnerUid failed errorCode: %{public}d", errorCode);
        context->ownerUid_ = INVALID_UID;
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetConnectOwnerUidCallback(GetConnectOwnerUidContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->ownerUid_);
}

bool ConnectionExec::ExecGetDnsUnicode(GetDnsContext *context)
{
    NETMANAGER_BASE_LOGI("ExecGetDnsUnicode");
    auto host = context->GetHost();
    if (host.empty()) {
        NETMANAGER_BASE_LOGE("host is empty");
        return true;
    }
    std::string unicode = "";
    int32_t errorCode = ICUHelper::GetDnsUnicode(host, context->GetConversionProcess(), unicode);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec GetDnsUnicode failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    context->SetHost(unicode);
    return true;
}

napi_value ConnectionExec::GetDnsCallback(GetDnsContext *context)
{
    return NapiUtils::CreateStringUtf8(context->GetEnv(), context->GetHost());
}

bool ConnectionExec::ExecGetSystemNetPortStates(GetSystemNetPortStatesContext *context)
{
    NETMANAGER_BASE_LOGI("ExecGetSystemNetPortStates");
    int32_t errorCode = NetConnClient::GetInstance().GetSystemNetPortStates(context->netPortStatesInfo_);
    if (errorCode != NET_CONN_SUCCESS) {
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetSystemNetPortStatesCallback(GetSystemNetPortStatesContext *context)
{
    return CreateNetPortStatesInfo(context->GetEnv(), context->netPortStatesInfo_);
}

bool ConnectionExec::ExecAddNetworkRoute(AddNetworkRouteContext *context)
{
    NETMANAGER_BASE_LOGI("ExecAddNetworkRoute");
    std::string destAddress =
        context->route_.destination_.address_ + "/" + std::to_string(context->route_.destination_.prefixlen_);
    int32_t errorCode = NetConnClient::GetInstance().AddNetworkRoute(
        context->netId_, context->route_.iface_, destAddress, context->route_.gateway_.address_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec addNetworkRoute failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::AddNetworkRouteCallback(AddNetworkRouteContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecGetNetInterfaceConfiguration(GetNetInterfaceConfigurationContext *context)
{
    NETMANAGER_BASE_LOGI("ExecAddNetworkRoute");
    int32_t errorCode = NetConnClient::GetInstance().GetNetInterfaceConfiguration(
        context->interface_, context->config_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec getNetInterfaceConfiguration failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetNetInterfaceConfigurationCallback(GetNetInterfaceConfigurationContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().GetNetInterfaceConfiguration(
        context->interface_, context->config_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("getNetInterfaceConfiguration callback failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return NapiUtils::GetUndefined(context->GetEnv());
    }
    napi_value ifName = NapiUtils::CreateStringUtf8(context->GetEnv(), context->config_.ifName_);
    napi_value hwAddr = NapiUtils::CreateStringUtf8(context->GetEnv(), context->config_.hwAddr_);
    napi_value ipv4Addr = NapiUtils::CreateStringUtf8(context->GetEnv(), context->config_.ipv4Addr_);
    napi_value prefixLength = NapiUtils::CreateInt32(context->GetEnv(), context->config_.prefixLength_);
    napi_value flagsList = NapiUtils::CreateArray(context->GetEnv(), context->config_.flags_.size());
    size_t index = 0;
    for (auto flag : context->config_.flags_) {
        napi_value jsList = NapiUtils::CreateStringUtf8(context->GetEnv(), flag);
        NapiUtils::SetArrayElement(context->GetEnv(), flagsList, index++, jsList);
    }
    napi_value interfaceConfig = NapiUtils::CreateObject(context->GetEnv());
    NapiUtils::SetNamedProperty(context->GetEnv(), interfaceConfig, "interfaceName", ifName);
    NapiUtils::SetNamedProperty(context->GetEnv(), interfaceConfig, "hwAddress", hwAddr);
    NapiUtils::SetNamedProperty(context->GetEnv(), interfaceConfig, "ipv4Address", ipv4Addr);
    NapiUtils::SetNamedProperty(context->GetEnv(), interfaceConfig, "prefixLength", prefixLength);
    NapiUtils::SetNamedProperty(context->GetEnv(), interfaceConfig, "flags", flagsList);
    return interfaceConfig;
}

bool ConnectionExec::ExecRegisterNetSupplier(RegisterNetSupplierContext *context)
{
    NETMANAGER_BASE_LOGI("ExecRegisterNetSupplier");
    int32_t errorCode = NetConnClient::GetInstance().RegisterNetSupplier(
        context->bearerType_, context->ident_, context->netCaps_, context->netSupplierId_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec registerNetSupplier failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::RegisterNetSupplierCallback(RegisterNetSupplierContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().RegisterNetSupplier(
        context->bearerType_, context->ident_, context->netCaps_, context->netSupplierId_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("registerNetSupplier callback failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return NapiUtils::GetUndefined(context->GetEnv());
    }
    napi_value netSupplierId = NapiUtils::CreateUint32(context->GetEnv(), context->netSupplierId_);
    return netSupplierId;
}

bool ConnectionExec::ExecUnregisterNetSupplier(UnregisterNetSupplierContext *context)
{
    NETMANAGER_BASE_LOGI("ExecUnregisterNetSupplier");
    int32_t errorCode = NetConnClient::GetInstance().UnregisterNetSupplier(context->netSupplierId_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec addNetworkRoute failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::UnregisterNetSupplierCallback(UnregisterNetSupplierContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecFactoryResetNetwork(FactoryResetNetworkContext *context)
{
    NETMANAGER_BASE_LOGI("ExecFactoryResetNetwork into");
    int32_t errorCode = NetConnClient::GetInstance().FactoryResetNetwork();
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec ResetNetwork failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::FactoryResetNetworkCallback(FactoryResetNetworkContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

int32_t TransErrorCode(int32_t error)
{
    switch (error) {
        case NO_PERMISSION_CODE:
            return NETMANAGER_ERR_PERMISSION_DENIED;
        case PERMISSION_DENIED_CODE:
            return NETMANAGER_ERR_PERMISSION_DENIED;
        case NET_UNREACHABLE_CODE:
            return NETMANAGER_ERR_INTERNAL;
        default:
            return NETMANAGER_ERR_OPERATION_FAILED;
    }
}

bool ConnectionExec::NetHandleExec::ExecGetAddressesByName(GetAddressByNameContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    uint32_t netid = static_cast<uint32_t>(context->netId_);
    addrinfo *res = nullptr;
    queryparam param;
    param.qp_type = QEURY_TYPE_NORMAL;
    param.qp_netid = netid;
    NETMANAGER_BASE_LOGD("getaddrinfo_ext %{public}d %{public}d", netid, param.qp_netid);
    if (context->host_.empty()) {
        NETMANAGER_BASE_LOGE("host is empty!");
        context->SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return false;
    }
    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int status = getaddrinfo_ext(context->host_.c_str(), nullptr, &hints, &res, &param);
    if (status < 0) {
        NETMANAGER_BASE_LOGE("getaddrinfo errno %{public}d %{public}s,  status: %{public}d", errno, strerror(errno),
                             status);
        int32_t temp = TransErrorCode(errno);
        context->SetErrorCode(temp);
        return false;
    }

    for (addrinfo *tmp = res; tmp != nullptr; tmp = tmp->ai_next) {
        std::string host;
        if (tmp->ai_family == AF_INET) {
            auto addr = reinterpret_cast<sockaddr_in *>(tmp->ai_addr);
            char ip[MAX_IPV4_STR_LEN] = {0};
            inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
            host = ip;
        } else if (tmp->ai_family == AF_INET6) {
            auto addr = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
            char ip[MAX_IPV6_STR_LEN] = {0};
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
            host = ip;
        }

        NetAddress address;
        SetAddressInfo(host.c_str(), tmp, address);

        context->addresses_.emplace_back(address);
    }
    freeaddrinfo(res);
    return true;
}

napi_value ConnectionExec::NetHandleExec::GetAddressesByNameCallback(GetAddressByNameContext *context)
{
    napi_value addresses = NapiUtils::CreateArray(context->GetEnv(), context->addresses_.size());
    for (uint32_t index = 0; index < context->addresses_.size(); ++index) {
        napi_value obj = MakeNetAddressJsValue(context->GetEnv(), context->addresses_[index]);
        NapiUtils::SetArrayElement(context->GetEnv(), addresses, index, obj);
    }
    return addresses;
}

bool ConnectionExec::NetHandleExec::ExecGetAddressByName(GetAddressByNameContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    uint32_t netid = static_cast<uint32_t>(context->netId_);
    addrinfo *res = nullptr;
    queryparam param;
    param.qp_type = QEURY_TYPE_NORMAL;
    param.qp_netid = netid;
    NETMANAGER_BASE_LOGD("getaddrinfo_ext %{public}d %{public}d", netid, param.qp_netid);
    if (context->host_.empty()) {
        NETMANAGER_BASE_LOGE("host is empty!");
        context->SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return false;
    }

    int status = getaddrinfo_ext(context->host_.c_str(), nullptr, nullptr, &res, &param);
    if (status < 0) {
        NETMANAGER_BASE_LOGE("getaddrinfo errno %{public}d %{public}s,  status: %{public}d", errno, strerror(errno),
                             status);
        int32_t temp = TransErrorCode(errno);
        context->SetErrorCode(temp);
        return false;
    }

    if (res != nullptr) {
        std::string host;
        if (res->ai_family == AF_INET) {
            auto addr = reinterpret_cast<sockaddr_in *>(res->ai_addr);
            char ip[MAX_IPV4_STR_LEN] = {0};
            inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
            host = ip;
        } else if (res->ai_family == AF_INET6) {
            auto addr = reinterpret_cast<sockaddr_in6 *>(res->ai_addr);
            char ip[MAX_IPV6_STR_LEN] = {0};
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
            host = ip;
        }

        NetAddress address;
        SetAddressInfo(host.c_str(), res, address);

        context->addresses_.emplace_back(address);
    }
    freeaddrinfo(res);
    return true;
}

napi_value ConnectionExec::NetHandleExec::GetAddressesByNameWithOptionsCallback(
    GetAddressByNameWithOptionsContext *context)
{
    napi_value addresses = NapiUtils::CreateArray(context->GetEnv(), context->addresses_.size());
    for (uint32_t index = 0; index < context->addresses_.size(); ++index) {
        napi_value obj = MakeNetAddressJsValue(context->GetEnv(), context->addresses_[index]);
        NapiUtils::SetArrayElement(context->GetEnv(), addresses, index, obj);
    }
    return addresses;
}

bool ConnectionExec::NetHandleExec::ExecGetAddressesByNameWithOptions(GetAddressByNameWithOptionsContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    addrinfo *res = nullptr;
    queryparam param;
    param.qp_type = QEURY_TYPE_NORMAL;
    param.qp_netid = context->netId_;
    NETMANAGER_BASE_LOGD("getaddrinfo_ext %{public}d", param.qp_netid);
    if (context->host_.empty()) {
        NETMANAGER_BASE_LOGE("host is empty!");
        context->SetErrorCode(NETMANAGER_ERR_INVALID_PARAMETER);
        return false;
    }
    struct addrinfo hints = {};
    hints.ai_family = ConvertToAiFamily(context->family_);
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int status = getaddrinfo_ext(context->host_.c_str(), nullptr, &hints, &res, &param);
    if (status < 0) {
        NETMANAGER_BASE_LOGE("getaddrinfo errno %{public}d %{public}s,  status: %{public}d", errno, strerror(errno),
                             status);
        int32_t temp = TransErrorCode(errno);
        context->SetErrorCode(temp);
        return false;
    }

    for (addrinfo *tmp = res; tmp != nullptr; tmp = tmp->ai_next) {
        std::string host;
        if (tmp->ai_family == AF_INET) {
            auto addr = reinterpret_cast<sockaddr_in *>(tmp->ai_addr);
            char ip[MAX_IPV4_STR_LEN] = {0};
            inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
            host = ip;
        } else if (tmp->ai_family == AF_INET6) {
            auto addr = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
            char ip[MAX_IPV6_STR_LEN] = {0};
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
            host = ip;
        }

        NetAddress address;
        SetAddressInfo(host.c_str(), tmp, address);

        context->addresses_.emplace_back(address);
    }
    freeaddrinfo(res);
    return true;
}

napi_value ConnectionExec::NetHandleExec::GetAddressByNameCallback(GetAddressByNameContext *context)
{
    if (context->addresses_.empty()) {
        return NapiUtils::GetUndefined(context->GetEnv());
    }
    return MakeNetAddressJsValue(context->GetEnv(), context->addresses_[0]);
}

napi_value ConnectionExec::NetHandleExec::MakeNetAddressJsValue(napi_env env, const NetAddress &address)
{
    napi_value obj = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, obj) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }

    NapiUtils::SetStringPropertyUtf8(env, obj, KEY_ADDRESS, address.GetAddress());
    NapiUtils::SetUint32Property(env, obj, KEY_FAMILY, address.GetJsValueFamily());
    NapiUtils::SetUint32Property(env, obj, KEY_PORT, address.GetPort());
    return obj;
}

bool ConnectionExec::NetHandleExec::ExecBindSocket(BindSocketContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    NetHandle handle(context->netId_);
    int32_t res = handle.BindSocket(context->socketFd_);
    if (res != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecBindSocket failed %{public}d", res);
        context->SetErrorCode(res);
    }
    return res == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::NetHandleExec::BindSocketCallback(BindSocketContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

void ConnectionExec::NetHandleExec::SetAddressInfo(const char *host, addrinfo *info, NetAddress &address)
{
    address.SetAddress(host);
    address.SetFamilyBySaFamily(info->ai_addr->sa_family);
    if (info->ai_addr->sa_family == AF_INET) {
        auto addr4 = reinterpret_cast<sockaddr_in *>(info->ai_addr);
        address.SetPort(addr4->sin_port);
    } else if (info->ai_addr->sa_family == AF_INET6) {
        auto addr6 = reinterpret_cast<sockaddr_in6 *>(info->ai_addr);
        address.SetPort(addr6->sin6_port);
    }
}

bool ConnectionExec::NetConnectionExec::ExecRegister(RegisterContext *context)
{
    auto hiAppEventReport = std::make_shared<HiAppEventReport>("NetworkKit", "ConnectionRegister");
    auto wCallback = context->GetNetConnCallback();
    sptr<INetConnCallback> callback = wCallback.promote();
    if (callback == nullptr) {
        NETMANAGER_BASE_LOGE("ExecRegister getNetConnCallback nullptr");
        return false;
    }

    auto conn = context->GetConn();
    if (conn.hasNetSpecifier_ && conn.hasTimeout_) {
        sptr<NetSpecifier> specifier = new NetSpecifier(conn.netSpecifier_);
        int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(specifier, callback, conn.timeout_);
        NETMANAGER_BASE_LOGI("Register result hasNetSpecifier_ and hasTimeout_ %{public}d", ret);
        context->SetErrorCode(ret);
        return ret == NETMANAGER_SUCCESS;
    }

    if (conn.hasNetSpecifier_) {
        sptr<NetSpecifier> specifier = new NetSpecifier(conn.netSpecifier_);
        int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(specifier, callback, 0);
        NETMANAGER_BASE_LOGD("Register result hasNetSpecifier_ %{public}d", ret);
        context->SetErrorCode(ret);
        return ret == NETMANAGER_SUCCESS;
    }

    int32_t ret = NetConnClient::GetInstance().RegisterNetConnCallback(callback);
    NETMANAGER_BASE_LOGI("Register result %{public}d", ret);
    context->SetErrorCode(ret);
    if (g_limitSdkReport == 0) {
        hiAppEventReport->ReportSdkEvent(RESULT_SUCCESS, ret);
        g_limitSdkReport = 1;
    }
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::NetConnectionExec::RegisterCallback(RegisterContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::NetConnectionExec::ExecUnregister(UnregisterContext *context)
{
    auto hiAppEventReport = std::make_shared<HiAppEventReport>("NetworkKit", "ConnectionUnregister");
    auto wCallback = context->GetNetConnCallback();
    auto callback = wCallback.promote();
    if (callback == nullptr) {
        NETMANAGER_BASE_LOGE("ExecUnregister getNetConnCallback nullptr");
        return false;
    }

    int32_t ret = NetConnClient::GetInstance().UnregisterNetConnCallback(callback);
    if (ret != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGD("Unregister result %{public}d", ret);
        context->SetErrorCode(ret);
    }
    if (g_limitSdkReports == 0) {
        hiAppEventReport->ReportSdkEvent(RESULT_SUCCESS, ret);
        g_limitSdkReports = 1;
    }
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::NetConnectionExec::UnregisterCallback(RegisterContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::NetInterfaceExec::ExecIfaceRegister(IfaceRegisterContext *context)
{
    auto wCallback = context->GetNetInterfaceCallback();
    sptr<INetInterfaceStateCallback> callback = wCallback.promote();
    if (callback == nullptr) {
        NETMANAGER_BASE_LOGE("ExecIfaceRegister getNetInterfaceCallback nullptr");
        return false;
    }

    int32_t ret = NetConnClient::GetInstance().RegisterNetInterfaceCallback(callback);
    NETMANAGER_BASE_LOGI("Register result %{public}d", ret);
    context->SetErrorCode(ret);
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::NetInterfaceExec::IfaceRegisterCallback(IfaceRegisterContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::NetInterfaceExec::ExecIfaceUnregister(IfaceUnregisterContext *context)
{
    auto wCallback = context->GetNetInterfaceCallback();
    auto callback = wCallback.promote();
    if (callback == nullptr) {
        NETMANAGER_BASE_LOGE("ExecIfaceUnregister getNetInterfaceCallback nullptr");
        return false;
    }

    int32_t ret = NetConnClient::GetInstance().UnregisterNetInterfaceCallback(callback);
    if (ret != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGD("Unregister result %{public}d", ret);
        context->SetErrorCode(ret);
    }
    return ret == NETMANAGER_SUCCESS;
}

napi_value ConnectionExec::NetInterfaceExec::IfaceUnregisterCallback(IfaceUnregisterContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

void ConnectionExec::FillLinkAddress(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo)
{
    if (!linkInfo->netAddrList_.empty() && linkInfo->netAddrList_.size() <= MAX_ARRAY_LENGTH) {
        napi_value linkAddresses =
            NapiUtils::CreateArray(env, std::min(linkInfo->netAddrList_.size(), MAX_ARRAY_LENGTH));
        auto it = linkInfo->netAddrList_.begin();
        for (uint32_t index = 0; index < MAX_ARRAY_LENGTH && it != linkInfo->netAddrList_.end(); ++index, ++it) {
            napi_value netAddr = NapiUtils::CreateObject(env);
            NapiUtils::SetStringPropertyUtf8(env, netAddr, KEY_ADDRESS, it->address_);
            NapiUtils::SetUint32Property(env, netAddr, KEY_FAMILY, it->family_);
            NapiUtils::SetUint32Property(env, netAddr, KEY_PORT, it->port_);

            napi_value linkAddr = NapiUtils::CreateObject(env);
            NapiUtils::SetNamedProperty(env, linkAddr, KEY_ADDRESS, netAddr);
            NapiUtils::SetUint32Property(env, linkAddr, KEY_PREFIX_LENGTH, it->prefixlen_);
            NapiUtils::SetArrayElement(env, linkAddresses, index, linkAddr);
        }
        NapiUtils::SetNamedProperty(env, connectionProperties, KEY_LINK_ADDRESSES, linkAddresses);
    }
}

void ConnectionExec::FillRouoteList(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo)
{
    if (!linkInfo->routeList_.empty() && linkInfo->routeList_.size() <= MAX_ROUTE_LENGTH) {
        napi_value routes = NapiUtils::CreateArray(env, std::min(linkInfo->routeList_.size(), MAX_ROUTE_LENGTH));
        auto it = linkInfo->routeList_.begin();
        for (uint32_t index = 0; index < MAX_ROUTE_LENGTH && it != linkInfo->routeList_.end(); ++index, ++it) {
            napi_value route = NapiUtils::CreateObject(env);
            NapiUtils::SetStringPropertyUtf8(env, route, KEY_INTERFACE, it->iface_);

            napi_value dest = NapiUtils::CreateObject(env);
            napi_value netAddr = NapiUtils::CreateObject(env);
            NapiUtils::SetStringPropertyUtf8(env, netAddr, KEY_ADDRESS, it->destination_.address_);
            NapiUtils::SetUint32Property(env, netAddr, KEY_FAMILY, it->destination_.family_);
            NapiUtils::SetUint32Property(env, netAddr, KEY_PORT, it->destination_.port_);
            NapiUtils::SetNamedProperty(env, dest, KEY_ADDRESS, netAddr);
            NapiUtils::SetUint32Property(env, dest, KEY_PREFIX_LENGTH, it->destination_.prefixlen_);
            NapiUtils::SetNamedProperty(env, route, KEY_DESTINATION, dest);

            napi_value gateway = NapiUtils::CreateObject(env);
            NapiUtils::SetStringPropertyUtf8(env, gateway, KEY_ADDRESS, it->gateway_.address_);
            NapiUtils::SetUint32Property(env, gateway, KEY_PREFIX_LENGTH, it->gateway_.prefixlen_);
            NapiUtils::SetNamedProperty(env, route, KEY_GATE_WAY, gateway);

            NapiUtils::SetBooleanProperty(env, route, KEY_HAS_GET_WAY, it->hasGateway_);
            NapiUtils::SetBooleanProperty(env, route, KEY_IS_DEFAULT_ROUE, it->isDefaultRoute_);

            NapiUtils::SetArrayElement(env, routes, index, route);
        }
        NapiUtils::SetNamedProperty(env, connectionProperties, KEY_ROUTES, routes);
    }
}

void ConnectionExec::FillDns(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo)
{
    if (!linkInfo->dnsList_.empty() && linkInfo->dnsList_.size() <= MAX_ARRAY_LENGTH) {
        napi_value dnsList = NapiUtils::CreateArray(env, std::min(linkInfo->dnsList_.size(), MAX_ARRAY_LENGTH));
        auto it = linkInfo->dnsList_.begin();
        for (uint32_t index = 0; index < MAX_ARRAY_LENGTH && it != linkInfo->dnsList_.end(); ++index, ++it) {
            napi_value netAddr = NapiUtils::CreateObject(env);
            NapiUtils::SetStringPropertyUtf8(env, netAddr, KEY_ADDRESS, it->address_);
            NapiUtils::SetUint32Property(env, netAddr, KEY_FAMILY, it->family_);
            NapiUtils::SetUint32Property(env, netAddr, KEY_PORT, it->port_);
            NapiUtils::SetArrayElement(env, dnsList, index, netAddr);
        }
        NapiUtils::SetNamedProperty(env, connectionProperties, KEY_DNSES, dnsList);
    }
}

void ConnectionExec::FillNetAddressInfo(napi_env env, napi_value jsNetAddress, const NetIpMacInfo &ipNeighTable)
{
    if (ipNeighTable.family_ == FAMILY_INVALID) {
        NETMANAGER_BASE_LOGE("Fill NetAddressInfo failed, family is invalid");
        return;
    }

    NapiUtils::SetStringPropertyUtf8(env, jsNetAddress, KEY_ADDRESS, ipNeighTable.ipAddress_);
    if (ipNeighTable.family_ == FAMILY_V4) {
        NapiUtils::SetUint32Property(env, jsNetAddress, KEY_FAMILY, static_cast<uint32_t>(NetAddress::Family::IPv4));
    } else if (ipNeighTable.family_ == FAMILY_V6) {
        NapiUtils::SetUint32Property(env, jsNetAddress, KEY_FAMILY, static_cast<uint32_t>(NetAddress::Family::IPv6));
    }
}


int ConnectionExec::ConvertToAiFamily(GetAddressByNameWithOptionsContext::Family family)
{
    switch (family) {
        case GetAddressByNameWithOptionsContext::Family::IPv4:
            return AF_INET;
        case GetAddressByNameWithOptionsContext::Family::IPv6:
            return AF_INET6;
        default:
            return AF_UNSPEC;
    }
}

bool ConnectionExec::ExecSetProxyMode(ProxyModeContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().SetProxyMode(context->mode_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec SetProxyMode failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

bool ConnectionExec::ExecGetProxyMode(ProxyModeContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().GetProxyMode(context->mode_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec GetProxyMode failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::SetProxyModeCallback(ProxyModeContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value ConnectionExec::GetProxyModeCallback(ProxyModeContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->mode_);
}

bool ConnectionExec::ExecSetPacFileUrl(SetPacFileUrlContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().SetPacFileUrl(context->pacUrl_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec SetPacFileUrl failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::SetPacFileUrlCallback(SetPacFileUrlContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

bool ConnectionExec::ExecGetPacFileUrl(GetPacFileUrlContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().GetPacFileUrl(context->pacUrl_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec GetPacFileUrl failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::GetPacFileUrlCallback(GetPacFileUrlContext *context)
{
    return NapiUtils::CreateStringUtf8(context->GetEnv(), context->pacUrl_);
}

bool ConnectionExec::ExecFindProxyForUrl(FindPacFileUrlContext *context)
{
    int32_t errorCode = NetConnClient::GetInstance().FindProxyForURL(context->url_, context->proxy_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec ExecFindProxyForUrl failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::FindProxyForUrlCallback(FindPacFileUrlContext *context)
{
    return NapiUtils::CreateStringUtf8(context->GetEnv(), context->proxy_);
}

bool ConnectionExec::ExecQueryTraceRoute(QueryTraceRouteContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    int32_t errorCode = NetConnClient::GetInstance().QueryTraceRoute(context->dest_, context->option_.maxJumpNumber_,
        static_cast<int32_t>(context->option_.packetsType_), context->traceRouteInfoStr_, false);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec ExecQueryTraceRoute failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::QueryTraceRouteCallback(QueryTraceRouteContext *context)
{
    context->Conv2TraceRouteInfo(context->traceRouteInfoStr_, context->traceRouteInfo_,
                                 context->option_.maxJumpNumber_);
    napi_value array = NapiUtils::CreateArray(context->GetEnv(), context->traceRouteInfo_.size());
    if (context->traceRouteInfo_.empty()) {
        NETMANAGER_BASE_LOGE("trace route info is empty!");
        return array;
    }
    uint32_t index = 0;
    std::for_each(context->traceRouteInfo_.begin(), context->traceRouteInfo_.end(),
        [array, &index, context](const TraceRouteInfo &traceRouteInfo) {
        NapiUtils::SetArrayElement(context->GetEnv(), array, index,
            CreateTraceRouteInfo(context->GetEnv(), traceRouteInfo));
        ++index;
    });
    return array;
}

napi_value ConnectionExec::CreateTraceRouteInfo(napi_env env, const TraceRouteInfo &traceRouteInfo)
{
    napi_value jsTraceRoute = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, jsTraceRoute) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }

    napi_value jsRttArray = NapiUtils::CreateArray(env, traceRouteInfo.rtt_.size());
    if (NapiUtils::GetValueType(env, jsRttArray) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    auto it = traceRouteInfo.rtt_.begin();
    for (uint32_t index = 0; it != traceRouteInfo.rtt_.end(); ++index, ++it) {
        NapiUtils::SetArrayElement(env, jsRttArray, index, NapiUtils::CreateUint32(env, *it));
    }

    NapiUtils::SetUint32Property(env, jsTraceRoute, KEY_JUMP_NO, traceRouteInfo.jumpNo_);
    NapiUtils::SetStringPropertyUtf8(env, jsTraceRoute, KEY_ADDRESS, traceRouteInfo.address_);
    NapiUtils::SetNamedProperty(env, jsTraceRoute, KEY_RTT, jsRttArray);

    return jsTraceRoute;
}

bool ConnectionExec::ExecQueryProbeResult(QueryProbeResultContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    if (!CommonUtils::HasInternetPermission()) {
        context->SetErrorCode(NETMANAGER_ERR_PERMISSION_DENIED);
        return false;
    }
    NetProbe np;
    int32_t errorCode = np.QueryProbeResult(context->dest_, context->duration_, context->probeResultInfo_);
    if (errorCode != NET_CONN_SUCCESS) {
        NETMANAGER_BASE_LOGE("exec ExecQueryProbeResult failed errorCode: %{public}d", errorCode);
        context->SetErrorCode(errorCode);
        return false;
    }
    return true;
}

napi_value ConnectionExec::QueryProbeResultCallback(QueryProbeResultContext *context)
{
    napi_env env = context->GetEnv();
    napi_value jsProbeResult = NapiUtils::CreateObject(env);
    if (NapiUtils::GetValueType(env, jsProbeResult) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    NapiUtils::SetUint32Property(env, jsProbeResult, KEY_LOSS_RATE, context->probeResultInfo_.lossRate);
    
    napi_value jsRttArray = NapiUtils::CreateArray(env, OHOS::NetManagerStandard::NETCONN_MAX_RTT_NUM);
    if (NapiUtils::GetValueType(env, jsRttArray) != napi_object) {
        return NapiUtils::GetUndefined(env);
    }
    std::swap(context->probeResultInfo_.rtt[NETCONN_RTT_MAX], context->probeResultInfo_.rtt[NETCONN_RTT_AVG]);
    for (uint32_t index = 0; index < OHOS::NetManagerStandard::NETCONN_MAX_RTT_NUM; ++index) {
        NapiUtils::SetArrayElement(env, jsRttArray, index,
            NapiUtils::CreateUint32(env, context->probeResultInfo_.rtt[index]));
    }
    NapiUtils::SetNamedProperty(env, jsProbeResult, KEY_RTT, jsRttArray);

    return jsProbeResult;
}

} // namespace OHOS::NetManagerStandard

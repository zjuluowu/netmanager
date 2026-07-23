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

#include "net_dns_result_callback_proxy.h"
#include "net_manager_constants.h"
#include "netnative_log_wrapper.h"
#include "netsys_ipc_interface_code.h"

namespace OHOS {
namespace NetsysNative {
namespace {
using namespace OHOS::NetManagerStandard;
} // namespace

NetDnsResultCallbackProxy::NetDnsResultCallbackProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<INetDnsResultCallback>(impl) {}

int32_t NetDnsResultCallbackProxy::OnDnsResultReport(uint32_t listsize,
    const std::list<NetDnsResultReport> dnsResultReport)
{
    NETNATIVE_LOG_D("Proxy OnDnsResultReport");
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetDnsResultCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t size = listsize;
    if (!data.WriteUint32(size)) {
        return false;
    }
   
    if (size > 0) {
        for (NetDnsResultReport report: dnsResultReport) {
            if (!report.Marshalling(data)) {
                NETNATIVE_LOGE("NetDnsResultReport Marshalling failed");
                return NETMANAGER_ERR_WRITE_DATA_FAIL;
            }
        }
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(NetDnsResultInterfaceCode::ON_DNS_RESULT_REPORT),
                            data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("proxy SendRequest failed, error: [%{public}d]", ret);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetDnsResultCallbackProxy::OnDnsQueryResultReport(uint32_t listsize,
    const std::list<NetDnsQueryResultReport> dnsResultReport)
{
    NETNATIVE_LOG_D("Proxy OnDnsQueryResultReport");
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetDnsResultCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t size = listsize;
    if (!data.WriteUint32(size)) {
        return false;
    }
   
    if (size > 0) {
        for (NetDnsQueryResultReport report: dnsResultReport) {
            if (!report.Marshalling(data)) {
                NETNATIVE_LOGE("NetDnsQueryResultReport Marshalling failed");
                return NETMANAGER_ERR_WRITE_DATA_FAIL;
            }
        }
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(NetDnsResultInterfaceCode::ON_DNS_QUERY_RESULT_REPORT),
                            data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("proxy SendRequest failed, error: [%{public}d]", ret);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetDnsResultCallbackProxy::OnDnsQueryAbnormalReport(uint32_t eventfailcause,
    const NetDnsQueryResultReport dnsResultReport)
{
    NETNATIVE_LOG_D("Proxy OnDnsQueryAbnormalReport");
    MessageParcel data;
    if (!data.WriteInterfaceToken(NetDnsResultCallbackProxy::GetDescriptor())) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteUint32(eventfailcause)) {
        return false;
    }
    if (!dnsResultReport.Marshalling(data)) {
        NETNATIVE_LOGE("OnDnsQueryAbnormalReport Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(NetDnsResultInterfaceCode::ON_DNS_QUERY_ABNORMAL_REPORT),
                            data, reply, option);
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("proxy SendRequest failed, error: [%{public}d]", ret);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}
} // namespace NetsysNative
} // namespace OHOS

/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "net_factoryreset_callback.h"

#include "net_mgr_log_wrapper.h"
#include "cpp/queue.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr int16_t LIMIT_CALLBACK_NUM = 200;

NetFactoryResetCallback::NetFactoryResetCallback()
{
    factoryResetCallFfrtQueue_ = std::make_shared<ffrt::queue>("NetFactoryResetCall");
}

// LCOV_EXCL_START
int32_t NetFactoryResetCallback::RegisterNetFactoryResetCallbackAsync(const sptr<INetFactoryResetCallback> &callback)
{
    if (callback == nullptr || callback->AsObject() == nullptr || callback->AsObject().GetRefPtr() == nullptr) {
        NETMGR_LOG_E("The parameter callback is null");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    int32_t ret = NETMANAGER_SUCCESS;
    if (!factoryResetCallFfrtQueue_) {
        NETMGR_LOG_E("FFRT Init Fail");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    ffrt::task_handle ResetCallbackAsyncTask = factoryResetCallFfrtQueue_->submit_h([this, &callback, &ret]() {
        ret = RegisterNetFactoryResetCallback(callback);
    }, ffrt::task_attr().name("FfrtRegisterNetFactoryResetCallbackAsync"));
    factoryResetCallFfrtQueue_->wait(ResetCallbackAsyncTask);
    return ret;
}

int32_t NetFactoryResetCallback::RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback)
{
    uint32_t callbackCounts = callbacks_.size();
    NETMGR_LOG_I("callback counts [%{public}u]", callbackCounts);
    if (callbackCounts >= LIMIT_CALLBACK_NUM) {
        NETMGR_LOG_E("callback counts cannot more than [%{public}u]", LIMIT_CALLBACK_NUM);
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    for (uint32_t i = 0; i < callbackCounts; i++) {
        if (callback->AsObject().GetRefPtr() == callbacks_[i]->AsObject().GetRefPtr()) {
            NETMGR_LOG_W("NetFactoryResetCallback_ had this callback");
            return NETMANAGER_ERR_PARAMETER_ERROR;
        }
    }

    callbacks_.emplace_back(callback);
    NETMGR_LOG_I("End RegisterNetFactoryResetCallback,callback counts [%{public}zu]", callbacks_.size());
    return NETMANAGER_SUCCESS;
}

int32_t NetFactoryResetCallback::UnregisterNetFactoryResetCallbackAsync(const sptr<INetFactoryResetCallback> &callback)
{
    if (callback == nullptr || callback->AsObject() == nullptr || callback->AsObject().GetRefPtr() == nullptr) {
        NETMGR_LOG_E("The parameter of callback is null");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    if (!factoryResetCallFfrtQueue_) {
        NETMGR_LOG_E("FFRT Init Fail");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    int32_t ret = NETMANAGER_SUCCESS;
    ffrt::task_handle UnregisterNetFactoryResetCallbackAsync =
        factoryResetCallFfrtQueue_->submit_h([this, &callback, &ret]() {
            ret = UnregisterNetFactoryResetCallback(callback);
        }, ffrt::task_attr().name("FfrtUnregisterNetFactoryResetCallbackAsync"));
    factoryResetCallFfrtQueue_->wait(UnregisterNetFactoryResetCallbackAsync);

    return ret;
}

int32_t NetFactoryResetCallback::UnregisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback)
{
    NETMGR_LOG_I("Enter UnregisterNetFactoryResetCallback");
    auto it = std::remove_if(callbacks_.begin(), callbacks_.end(),
        [callback](const sptr<INetFactoryResetCallback> &tempCallback) -> bool {
            if (tempCallback == nullptr || tempCallback->AsObject() == nullptr ||
                tempCallback->AsObject().GetRefPtr() == nullptr) {
                return true;
            }
            return callback->AsObject().GetRefPtr() == tempCallback->AsObject().GetRefPtr();
        });
    callbacks_.erase(it, callbacks_.end());
    NETMGR_LOG_I("End UnregisterNetFactoryResetCallback");
    return NETMANAGER_SUCCESS;
}


int32_t NetFactoryResetCallback::NotifyNetFactoryResetAsync()
{
    NETMGR_LOG_I("NotifyNetFactoryResetAsync enter");
    int32_t ret = NETMANAGER_SUCCESS;
    if (!factoryResetCallFfrtQueue_) {
        NETMGR_LOG_E("FFRT Init Fail");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    ffrt::task_handle  NotifyNetFactoryResetAsyncTask = factoryResetCallFfrtQueue_->submit_h([this, &ret]() {
           ret = NotifyNetFactoryReset();
    }, ffrt::task_attr().name("FfrtNotifyNetFactoryResetAsync"));
    factoryResetCallFfrtQueue_->wait(NotifyNetFactoryResetAsyncTask);
    return ret;
}

int32_t NetFactoryResetCallback::NotifyNetFactoryReset()
{
    NETMGR_LOG_I("NotifyNetFactoryReset enter, callback count = [%{public}zu]", callbacks_.size());
    for (const auto &callback : callbacks_) {
        if (callback != nullptr && callback->AsObject() != nullptr && callback->AsObject().GetRefPtr() != nullptr) {
            callback->OnNetFactoryReset();
        }
    }

    return NETMANAGER_SUCCESS;
}
// LCOV_EXCL_STOP
} // namespace NetManagerStandard
} // namespace OHOS

/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_ACTIVATE_H
#define NET_ACTIVATE_H

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <shared_mutex>

#include "i_net_conn_callback.h"
#include "net_specifier.h"
#include "net_supplier.h"
#include "net_supplier_callback_base.h"
class NetSupplier;

namespace OHOS {
namespace NetManagerStandard {
constexpr uint32_t DEFAULT_REQUEST_ID = 0;
constexpr uint32_t MIN_REQUEST_ID = DEFAULT_REQUEST_ID + 1;
constexpr uint32_t MAX_REQUEST_ID = 0x7FFFFFFF;
class NetActivate;
class INetActivateCallback {
public:
    virtual ~INetActivateCallback() = default;

public:
    virtual void OnNetActivateTimeOut(std::shared_ptr<NetActivate> activate) = 0;
};

class NetActivate : public std::enable_shared_from_this<NetActivate> {
public:
    using TimeOutHandler = std::function<int32_t(uint32_t &reqId)>;

public:
    NetActivate(const sptr<NetSpecifier> &specifier, const sptr<INetConnCallback> &callback,
                std::weak_ptr<INetActivateCallback> timeoutCallback, const uint32_t &timeoutMS,
                const std::shared_ptr<AppExecFwk::EventHandler> &netActEventHandler, uint32_t uid = 0,
                const int32_t registerType = REGISTER);
    ~NetActivate();
    bool MatchRequestAndNetwork(sptr<NetSupplier> supplier, bool skipCheckIdent = false);
    void SetRequestId(uint32_t reqId);
    uint32_t GetRequestId() const;
    sptr<NetSupplier> GetServiceSupply() const;
    void SetServiceSupply(sptr<NetSupplier> netServiceSupplied);
    sptr<INetConnCallback> GetNetCallback();
    sptr<NetSpecifier> GetNetSpecifier();
    int32_t GetRegisterType() const;
    std::set<NetBearType> GetBearType() const;
    void StartTimeOutNetAvailable();
    uint32_t GetUid() const;
    bool IsAppFrozened() const;
    void SetIsAppFrozened(bool isFrozened);
    CallbackType GetLastCallbackType() const;
    void SetLastCallbackType(CallbackType callbackType);
    bool IsAllowCallback(CallbackType callbackType);
    int32_t GetLastNetid();
    void SetLastNetid(const int32_t netid);
    void SetNotifyLostDelay(bool isNotifyLostDelay);
    void SetNotifyLostNetId(int32_t notifyLostNetId);
    int32_t GetNotifyLostNetId();
    bool GetNotifyLostDelay();
    NetRequest &GetNetRequest();
    bool CheckTypes(const std::set<NetBearType> &bearerTypes) const;
    void SetIsFrozenedSkip(bool isFrozenedSkip);
    bool IsFrozenedSkip() const;
    void SetNeedSkipLostDelay(bool NeedSkipLostDelay);
    bool NeedSkipLostDelay() const;
private:
    bool CompareByNetworkIdent(const std::string &ident, NetBearType bearerType, bool skipCheckIdent);
    bool CompareByNetworkCapabilities(const NetCaps &netCaps);
    bool CompareByNetworkNetType(NetBearType bearerType);
    bool CompareByNetworkBand(uint32_t netLinkUpBand, uint32_t netLinkDownBand);
    bool HaveCapability(NetCap netCap) const;
    bool HaveTypes(const std::set<NetBearType> &bearerTypes) const;
    void TimeOutNetAvailable();

private:
    sptr<NetSpecifier> netSpecifier_ = nullptr;
    sptr<INetConnCallback> netConnCallback_ = nullptr;
    mutable std::shared_mutex netServiceSuppliedMutex_;
    sptr<NetSupplier> netServiceSupplied_ = nullptr;
    uint32_t timeoutMS_ = 0;
    std::weak_ptr<INetActivateCallback> timeoutCallback_;
    std::shared_ptr<AppExecFwk::EventHandler> netActEventHandler_;
    std::string activateName_ = "";
    NetRequest netRequest_;
    std::atomic<bool> isAppFrozened_ = false;
    std::atomic<int32_t> lastCallbackType_ = 0;
    int32_t lastNetId_ = 0;
    std::recursive_mutex notifyLostMutex_;
    std::atomic<bool> isNotifyLostDelay_ = false;
    std::atomic<int32_t> notifyLostNetId_ = 0;
    std::atomic<bool> isFrozenedSkip_ = false;
    std::atomic<bool> NeedSkipLostDelay_ = false;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_ACTIVATE_H
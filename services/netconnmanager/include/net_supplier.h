/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef NET_SUPPLIER_H
#define NET_SUPPLIER_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "i_net_supplier_callback.h"
#include "i_net_conn_callback.h"
#include "http_proxy.h"
#include "network.h"
#include "net_caps.h"
#include "net_specifier.h"
#include "net_supplier_info.h"

namespace OHOS {
namespace NetManagerStandard {
enum CallbackType {
    CALL_TYPE_UNKNOWN = 0,
    CALL_TYPE_AVAILABLE = 1,
    CALL_TYPE_LOSTING = 2,
    CALL_TYPE_LOST = 3,
    CALL_TYPE_UPDATE_CAP = 4,
    CALL_TYPE_UPDATE_LINK = 5,
    CALL_TYPE_UNAVAILABLE = 6,
    CALL_TYPE_BLOCK_STATUS = 7,
};

using NetTypeScore = std::unordered_map<NetBearType, int32_t>;
constexpr int32_t NET_TYPE_SCORE_INTERVAL = 10;
constexpr int32_t NET_VALID_SCORE = 4 * NET_TYPE_SCORE_INTERVAL;
constexpr int32_t DIFF_SCORE_BETWEEN_GOOD_POOR = 2 * NET_TYPE_SCORE_INTERVAL;
enum class NetTypeScoreValue : int32_t {
    USB_VALUE = 4 * NET_TYPE_SCORE_INTERVAL,
    BLUETOOTH_VALUE = 5 * NET_TYPE_SCORE_INTERVAL,
    CELLULAR_VALUE = 6 * NET_TYPE_SCORE_INTERVAL,
    WIFI_VALUE = 7 * NET_TYPE_SCORE_INTERVAL,
    ETHERNET_VALUE = 8 * NET_TYPE_SCORE_INTERVAL,
    VPN_VALUE = 9 * NET_TYPE_SCORE_INTERVAL,
    WIFI_AWARE_VALUE = 10 * NET_TYPE_SCORE_INTERVAL,
    MAX_SCORE = 10 * NET_TYPE_SCORE_INTERVAL
};

static inline NetTypeScore netTypeScore_ = {
    {BEARER_CELLULAR, static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE)},
    {BEARER_WIFI, static_cast<int32_t>(NetTypeScoreValue::WIFI_VALUE)},
    {BEARER_BLUETOOTH, static_cast<int32_t>(NetTypeScoreValue::BLUETOOTH_VALUE)},
    {BEARER_ETHERNET, static_cast<int32_t>(NetTypeScoreValue::ETHERNET_VALUE)},
    {BEARER_VPN, static_cast<int32_t>(NetTypeScoreValue::VPN_VALUE)},
    {BEARER_WIFI_AWARE, static_cast<int32_t>(NetTypeScoreValue::WIFI_AWARE_VALUE)}};

class NetSupplier : public virtual RefBase {
public:
    NetSupplier(NetBearType bearerType, const std::string &netSupplierIdent, const std::set<NetCap> &netCaps);
    ~NetSupplier() = default;
    void InitNetScore();
    /**
     * Resets all attributes that may change in the supplier, such as detection progress and network quality.
     */
    void ResetNetSupplier();
    bool operator==(const NetSupplier &netSupplier) const;
    void SetNetwork(const std::shared_ptr<Network> &network);
    void UpdateNetSupplierInfo(const NetSupplierInfo &netSupplierInfo);
    int32_t UpdateNetLinkInfo(NetLinkInfo &netLinkInfo);
    uint32_t GetSupplierId() const;
    NetBearType GetNetSupplierType() const;
    std::string GetNetSupplierIdent() const;
    bool CompareNetCaps(const std::set<NetCap> caps) const;
    bool HasNetCap(NetCap cap) const;
    bool HasNetCaps(const std::set<NetCap> &caps) const;
    const NetCaps &GetNetCaps() const;
    NetAllCapabilities GetNetCapabilities() const;
    bool GetRoaming() const;
    int8_t GetStrength() const;
    uint16_t GetFrequency() const;
    int32_t GetSupplierUid() const;
    int32_t GetUid() const;
    void SetUid(int32_t uid);
    bool IsAvailable() const;
    std::shared_ptr<Network> GetNetwork() const;
    int32_t GetNetId() const;
    sptr<NetHandle> GetNetHandle() const;
    void GetHttpProxy(HttpProxy &httpProxy);
    void UpdateNetConnState(NetConnState netConnState);
    bool IsConnecting() const;
    bool IsConnected() const;
    void SetNetValid(NetDetectionStatus netState);
    bool IsNetValidated() const;
    /**
     * This method returns the score of the current network supplier.
     *
     * It is used to prioritize network suppliers so that higher priority producers can activate when lower
     * priority networks are available.
     *
     * @return the score of the current network supplier.
     */
    int32_t GetNetScore() const;

    /**
     * This method returns the real score of current network supplier.
     *
     * This method subtracts the score depending on different conditions, or returns netScore_ if the conditions are not
     * met.
     * It is used to compare the priorities of different networks.
     *
     * @return the real score of current network supplier.
     */
    int32_t GetRealScore();
    bool SupplierConnection(const std::set<NetCap> &netCaps, const NetRequest &netrequest = {});
    bool SupplierDisconnection(const std::set<NetCap> &netCaps, const NetRequest &netrequest);
    void SetRestrictBackground(bool restrictBackground);
    bool GetRestrictBackground() const;
    bool RequestToConnect(const NetRequest &netrequest = {});
    bool AddRequest(const NetRequest &netrequest);
    bool RemoveRequest(const NetRequest &netRequest);
    int32_t SelectAsBestNetwork(const NetRequest &netrequest);
    void ReceiveBestScore(int32_t bestScore, uint32_t supplierId, const NetRequest &netrequest);
    int32_t CancelRequest(const NetRequest &netrequest);
    void AddBestRequest(uint32_t reqId);
    void RemoveBestRequest(uint32_t reqId);
    bool HasBestRequest(uint32_t reqId);
    size_t GetBestRequestSize();
    void SetDefault();
    void ClearDefault();
    sptr<INetSupplierCallback> GetSupplierCallback();
    void RegisterSupplierCallback(const sptr<INetSupplierCallback> &callback);
    void UpdateGlobalHttpProxy(const HttpProxy &httpProxy);
    void SetSupplierType(int32_t type);
    std::string GetSupplierType();
    std::string TechToType(NetSlotTech type);
    void SetDetectionDone();
    void SetReuseCap(NetCap reuseCap, bool add);
    void UpdateNetCap(const std::set<NetCap> &netCaps);
    bool ResumeNetworkInfo();
    bool IsNetAcceptUnavalidate();
    bool IsInFirstTimeDetecting() const;
    std::string GetNetExtAttribute();
    void SetNetExtAttribute(const std::string &netExtAttribute);
    bool IsNetQualityPoor() const;
    bool IsOnceSuppress() const;

private:
    void SetOnceSuppress();

private:
    NetBearType netSupplierType_;
    std::string netSupplierIdent_;
    NetCaps netCaps_;
    NetSupplierInfo netSupplierInfo_;
    NetAllCapabilities netAllCapabilities_;
    uint32_t supplierId_ = 0;
    int32_t netScore_ = 0;
    std::shared_mutex requestListMutex_;
    std::set<uint32_t> requestList_;
    std::shared_mutex bestReqListMutex_;
    std::set<uint32_t> bestReqList_;
    sptr<INetSupplierCallback> netController_ = nullptr;
    std::shared_ptr<Network> network_ = nullptr;
    sptr<NetHandle> netHandle_ = nullptr;
    bool restrictBackground_ = true;
    int32_t type_ = -1;
    NetDetectionStatus netQuality_ = QUALITY_NORMAL_STATE;
    bool isFirstTimeDetectionDone = false;
    bool isAcceptUnvaliad = false;
    int32_t uid_ = 0;
    std::string netExtAttribute_ = "";
    bool isOnceSuppress_ = false;
    NetRequest netRequest_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_SUPPLIER_H

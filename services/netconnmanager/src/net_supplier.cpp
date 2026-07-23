/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include <cinttypes>

#include "broadcast_manager.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_supplier.h"
#include "netsys_controller.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int32_t REG_OK = 0;
constexpr const char *SIMID_IDENT_PREFIX = "simId";
constexpr uint32_t REMOVE_UID_ONLY = 255;
}
static std::atomic<uint32_t> g_nextNetSupplierId = 0x03EB;

NetSupplier::NetSupplier(NetBearType bearerType, const std::string &netSupplierIdent, const std::set<NetCap> &netCaps)
    : netSupplierType_(bearerType),
      netSupplierIdent_(netSupplierIdent),
      netCaps_(netCaps),
      supplierId_(g_nextNetSupplierId++)
{
    netAllCapabilities_.netCaps_ = netCaps;
    netAllCapabilities_.bearerTypes_.insert(bearerType);
    ResetNetSupplier();
    InitNetScore();
}

sptr<INetSupplierCallback> NetSupplier::GetSupplierCallback()
{
    return netController_;
}

void NetSupplier::RegisterSupplierCallback(const sptr<INetSupplierCallback> &callback)
{
    netController_ = callback;
    std::shared_lock<std::shared_mutex> rLock(requestListMutex_);
    if (netController_!= nullptr && !requestList_.empty()) {
        rLock.unlock();
        SupplierConnection(netCaps_.ToSet(), netRequest_);
    }
}

void NetSupplier::InitNetScore()
{
    int32_t netScore = 0;
    auto iter = netTypeScore_.find(netSupplierType_);
    if (iter == netTypeScore_.end()) {
        NETMGR_LOG_E("Can not find net bearer type[%{public}d] for this net service", netSupplierType_);
        return;
    }
    NETMGR_LOG_D("Net type[%{public}d],default score[%{public}d]",
                 static_cast<int32_t>(iter->first), static_cast<int32_t>(iter->second));
    netScore = static_cast<int32_t>(iter->second);
    netScore_ = netScore;
    NETMGR_LOG_D("netScore_ = %{public}d", netScore_);
}

/**
 * Reset all attributes that may change in the supplier, such as detection progress and network quality.
 */
void NetSupplier::ResetNetSupplier()
{
    // Reset network quality.
    netQuality_ = QUALITY_NORMAL_STATE;
    // Reset network detection progress.
    isFirstTimeDetectionDone = false;
    //Reset User Selection
    isAcceptUnvaliad = false;
    isOnceSuppress_ = false;
    // Reset network capabilities for checking connectivity finished flag.
    netCaps_.InsertNetCap(NET_CAPABILITY_CHECKING_CONNECTIVITY);
    // Reset network verification status to validated.
    SetNetValid(VERIFICATION_STATE);
    // Reset checking connectivity flag.
    std::unique_lock<std::shared_mutex> lock(netAllCapabilities_.netCapsMutex_);
    netAllCapabilities_.netCaps_.insert(NET_CAPABILITY_CHECKING_CONNECTIVITY);
    lock.unlock();
    // Reset network extAttribute.
    netExtAttribute_ = "";
    NETMGR_LOG_I("Reset net supplier %{public}u", supplierId_);
}

bool NetSupplier::operator==(const NetSupplier &netSupplier) const
{
    return supplierId_ == netSupplier.supplierId_ && netSupplierType_ == netSupplier.netSupplierType_ &&
           netSupplierIdent_ == netSupplier.netSupplierIdent_ && netCaps_ == netSupplier.netCaps_;
}

void NetSupplier::UpdateNetSupplierInfo(const NetSupplierInfo &netSupplierInfo)
{
    NETMGR_LOG_D("Update net supplier[%{public}d, %{public}s], netSupplierInfo[%{public}s]", supplierId_,
                 netSupplierIdent_.c_str(), netSupplierInfo_.ToString("").c_str());
    bool oldAvailable = netSupplierInfo_.isAvailable_;
    netSupplierInfo_ = netSupplierInfo;
    netAllCapabilities_.linkUpBandwidthKbps_ = netSupplierInfo_.linkUpBandwidthKbps_;
    netAllCapabilities_.linkDownBandwidthKbps_ = netSupplierInfo_.linkDownBandwidthKbps_;
    if (!netSupplierInfo_.ident_.empty()) {
        netSupplierIdent_ = netSupplierInfo_.ident_;
    }
    if (netSupplierInfo_.score_ != 0) {
        netScore_ = netSupplierInfo_.score_;
    }
    if (oldAvailable == netSupplierInfo_.isAvailable_) {
        NETMGR_LOG_W("Same supplier available status:[%{public}d]", oldAvailable);
        return;
    }
    if (network_ == nullptr) {
        NETMGR_LOG_E("network_ is nullptr!");
        return;
    }
    network_->UpdateBasicNetwork(netSupplierInfo_.isAvailable_);
    if (!netSupplierInfo_.isAvailable_) {
        UpdateNetConnState(NET_CONN_STATE_DISCONNECTED);
    }
}

int32_t NetSupplier::UpdateNetLinkInfo(NetLinkInfo &netLinkInfo)
{
    if (network_ == nullptr) {
        NETMGR_LOG_E("network_ is nullptr!");
        return NET_CONN_ERR_INVALID_NETWORK;
    }
    if (!netSupplierInfo_.isAvailable_) {
        NETMGR_LOG_E("supplier not ava!");
        return NET_CONN_ERR_INVALID_NETWORK;
    }

    if (GetNetSupplierIdent().substr(0, strlen(SIMID_IDENT_PREFIX)) == SIMID_IDENT_PREFIX) {
        netLinkInfo.ident_ = GetNetSupplierIdent().substr(strlen(SIMID_IDENT_PREFIX));
    }
    NETMGR_LOG_D("Update netlink info: netLinkInfo[%{public}s]", netLinkInfo.ToString(" ").c_str());
    std::shared_ptr<Network> network = network_;
    if (!network->UpdateNetLinkInfo(netLinkInfo)) {
        return NET_CONN_ERR_SERVICE_UPDATE_NET_LINK_INFO_FAIL;
    }
    UpdateNetConnState(NET_CONN_STATE_CONNECTED);
    return NETMANAGER_SUCCESS;
}

NetBearType NetSupplier::GetNetSupplierType() const
{
    return netSupplierType_;
}

std::string NetSupplier::GetNetSupplierIdent() const
{
    return netSupplierIdent_;
}

bool NetSupplier::CompareNetCaps(const std::set<NetCap> caps) const
{
    if (caps.empty()) {
        return true;
    }
    return netCaps_.HasNetCaps(caps);
}

bool NetSupplier::HasNetCap(NetCap cap) const
{
    return netCaps_.HasNetCap(cap);
}

bool NetSupplier::HasNetCaps(const std::set<NetCap> &caps) const
{
    return netCaps_.HasNetCaps(caps);
}

const NetCaps &NetSupplier::GetNetCaps() const
{
    return netCaps_;
}

NetAllCapabilities NetSupplier::GetNetCapabilities() const
{
    return netAllCapabilities_;
}

void NetSupplier::SetNetwork(const std::shared_ptr<Network> &network)
{
    network_ = network;
    if (network_ != nullptr) {
        netHandle_ = std::make_unique<NetHandle>(network_->GetNetId()).release();
    }
}

std::shared_ptr<Network> NetSupplier::GetNetwork() const
{
    return network_;
}

int32_t NetSupplier::GetNetId() const
{
    if (network_ == nullptr) {
        return INVALID_NET_ID;
    }
    return network_->GetNetId();
}

sptr<NetHandle> NetSupplier::GetNetHandle() const
{
    return netHandle_;
}

void NetSupplier::GetHttpProxy(HttpProxy &httpProxy)
{
    if (network_ == nullptr) {
        NETMGR_LOG_E("network_ is nullptr.");
        return;
    }
    httpProxy = network_->GetHttpProxy();
}

uint32_t NetSupplier::GetSupplierId() const
{
    return supplierId_;
}

bool NetSupplier::GetRoaming() const
{
    return netSupplierInfo_.isRoaming_;
}

int8_t NetSupplier::GetStrength() const
{
    return netSupplierInfo_.strength_;
}

uint16_t NetSupplier::GetFrequency() const
{
    return netSupplierInfo_.frequency_;
}

int32_t NetSupplier::GetSupplierUid() const
{
    return netSupplierInfo_.uid_;
}

int32_t NetSupplier::GetUid() const
{
    return uid_;
}

void NetSupplier::SetUid(int32_t uid)
{
    uid_ = uid;
}

bool NetSupplier::IsAvailable() const
{
    return netSupplierInfo_.isAvailable_;
}

bool NetSupplier::SupplierConnection(const std::set<NetCap> &netCaps, const NetRequest &netRequest)
{
    if (netSupplierType_ == BEARER_CELLULAR) {
        NETMGR_LOG_I("Supplier[%{public}d, %{public}s] request connect, available=%{public}d, "
            "isInternet:%{public}d", supplierId_, netSupplierIdent_.c_str(), netSupplierInfo_.isAvailable_,
            netCaps_.HasNetCap(NET_CAPABILITY_INTERNET));
    }

    if (netController_ == nullptr) {
        NETMGR_LOG_E("netController_ is nullptr");
        return false;
    }
    NETMGR_LOG_D("execute RequestNetwork");
    int32_t errCode = netController_->RequestNetwork(netSupplierIdent_, netCaps, netRequest);
    NETMGR_LOG_D("RequestNetwork errCode[%{public}d]", errCode);
    if (errCode != REG_OK) {
        NETMGR_LOG_E("RequestNetwork fail");
        return false;
    }
    return true;
}

void NetSupplier::SetRestrictBackground(bool restrictBackground)
{
    restrictBackground_ = restrictBackground;
}
bool NetSupplier::GetRestrictBackground() const
{
    return restrictBackground_;
}

bool NetSupplier::SupplierDisconnection(const std::set<NetCap> &netCaps, const NetRequest &netrequest)
{
    if (netSupplierType_ == BEARER_CELLULAR) {
        NETMGR_LOG_I("Supplier[%{public}d, %{public}s] request disconnect, available=%{public}d, "
            "isInternet:%{public}d", supplierId_, netSupplierIdent_.c_str(), netSupplierInfo_.isAvailable_,
            netCaps_.HasNetCap(NET_CAPABILITY_INTERNET));
    }
    if (netController_ == nullptr) {
        NETMGR_LOG_E("netController_ is nullptr");
        return false;
    }
    NETMGR_LOG_D("execute ReleaseNetwork, supplierId[%{public}d]", supplierId_);
    NetRequest request;
    request.requestId = netrequest.requestId;
    request.uid = netrequest.uid;
    request.ident = netSupplierIdent_;
    request.netCaps = netCaps;
    request.registerType = netrequest.registerType;
    request.bearTypes = netrequest.bearTypes;
    int32_t errCode = netController_->ReleaseNetwork(request);
    NETMGR_LOG_D("ReleaseNetwork retCode[%{public}d]", errCode);
    if (errCode != REG_OK) {
        NETMGR_LOG_E("ReleaseNetwork fail");
        return false;
    }
    return true;
}

void NetSupplier::UpdateNetConnState(NetConnState netConnState)
{
    if (network_) {
        network_->UpdateNetConnState(netConnState);
    }
}

bool NetSupplier::IsConnecting() const
{
    if (network_) {
        return network_->IsConnecting();
    }
    return false;
}

bool NetSupplier::IsConnected() const
{
    if (network_) {
        return network_->IsConnected();
    }
    return false;
}

bool NetSupplier::RequestToConnect(const NetRequest &netrequest)
{
    if (netSupplierType_ == BEARER_CELLULAR && netrequest.isControlled) {
        NETMGR_LOG_E("RequestToConnect is Controlled");
        return true;
    }
    return AddRequest(netrequest);
}

int32_t NetSupplier::SelectAsBestNetwork(const NetRequest &netrequest)
{
    HILOG_COMM_IMPL(LOG_INFO, LOG_DOMAIN, LOG_TAG,
        "Request[%{public}d] select [%{public}d,%{public}s] as best network, isControlled:%{public}d, size:%{public}zu",
        netrequest.requestId, supplierId_, netSupplierIdent_.c_str(), netrequest.isControlled, requestList_.size());
    if (netSupplierType_ != BEARER_CELLULAR || !netrequest.isControlled) {
        AddRequest(netrequest);
    } else {
        RemoveRequest(netrequest);
    }
    AddBestRequest(netrequest.requestId);
    return NETMANAGER_SUCCESS;
}

void NetSupplier::ReceiveBestScore(int32_t bestScore, uint32_t supplierId, const NetRequest &netrequest)
{
    NETMGR_LOG_D("Supplier[%{public}d, %{public}s] receive best score, bestSupplierId[%{public}d], "
        "isInternet:%{public}d", supplierId_, netSupplierIdent_.c_str(), supplierId,
        netCaps_.HasNetCap(NET_CAPABILITY_INTERNET));
    if (supplierId == supplierId_) {
        NETMGR_LOG_D("Same net supplier, no need to disconnect.");
        return;
    }
    std::shared_lock<std::shared_mutex> rLock(requestListMutex_);
    if (requestList_.find(netrequest.requestId) == requestList_.end()) {
        NETMGR_LOG_D("Can not find request[%{public}d]", netrequest.requestId);
        return;
    }
    rLock.unlock();
    if (netScore_ >= bestScore) {
        NETMGR_LOG_D("High priority network, no need to disconnect");
        return;
    }
    RemoveRequest(netrequest);
    NETMGR_LOG_D("Supplier[%{public}d, %{public}s] remaining request list size[%{public}zd]", supplierId_,
                 netSupplierIdent_.c_str(), requestList_.size());
}

int32_t NetSupplier::CancelRequest(const NetRequest &netrequest)
{
    RemoveBestRequest(netrequest.requestId);
    int32_t ret = RemoveRequest(netrequest);
    if (!ret) {
        return NET_CONN_ERR_SERVICE_NO_REQUEST;
    }
    NETMGR_LOG_I("CancelRequest requestId:%{public}u", netrequest.requestId);
    return NETMANAGER_SUCCESS;
}

bool NetSupplier::AddRequest(const NetRequest &netrequest)
{
    std::unique_lock<std::shared_mutex> lock(requestListMutex_);
    bool isEmpty = requestList_.empty();
    if (requestList_.find(netrequest.requestId) == requestList_.end()) {
        requestList_.insert(netrequest.requestId);
    }
    if (netController_ == nullptr) {
        netRequest_ = netrequest;
    }
    lock.unlock();
    NETMGR_LOG_D("AddRequest, requestList.size:%{public}d, supplierId:%{public}d, isInternet:%{public}d",
        requestList_.size(), supplierId_, netCaps_.HasNetCap(NET_CAPABILITY_INTERNET));
    if (isEmpty || netSupplierType_ == BEARER_WIFI || !IsAvailable()) {
        return SupplierConnection(netCaps_.ToSet(), netrequest);
    }
    return true;
}

bool NetSupplier::RemoveRequest(const NetRequest &netRequest)
{
    std::unique_lock<std::shared_mutex> lock(requestListMutex_);
    auto iter = requestList_.find(netRequest.requestId);
    if (iter == requestList_.end()) {
        return false;
    }
    requestList_.erase(netRequest.requestId);
    bool isEmpty = requestList_.empty();
    lock.unlock();
    NETMGR_LOG_D("RemoveRequest, requestList.size:%{public}d, supplierId:%{public}d, isInternet:%{public}d",
        requestList_.size(), supplierId_, netCaps_.HasNetCap(NET_CAPABILITY_INTERNET));
    if (isEmpty || netSupplierType_ == BEARER_WIFI) {
        SupplierDisconnection(netCaps_.ToSet(), netRequest);
    }
    return true;
}

void NetSupplier::AddBestRequest(uint32_t reqId)
{
    std::unique_lock<std::shared_mutex> lock(bestReqListMutex_);
    if (bestReqList_.find(reqId) == bestReqList_.end()) {
        bestReqList_.insert(reqId);
    }
}

void NetSupplier::RemoveBestRequest(uint32_t reqId)
{
    std::unique_lock<std::shared_mutex> lock(bestReqListMutex_);
    auto iter = bestReqList_.find(reqId);
    if (iter == bestReqList_.end()) {
        return;
    }
    bestReqList_.erase(reqId);
    NETMGR_LOG_I("RemoveBestRequest supplierId=[%{public}d], reqId=[%{public}u]", supplierId_, reqId);
}

bool NetSupplier::HasBestRequest(uint32_t reqId)
{
    std::shared_lock<std::shared_mutex> lock(bestReqListMutex_);
    return bestReqList_.find(reqId) != bestReqList_.end();
}

size_t NetSupplier::GetBestRequestSize()
{
    std::shared_lock<std::shared_mutex> lock(bestReqListMutex_);
    return bestReqList_.size();
}

static void RemoveNetCap(NetCaps& netCaps, NetAllCapabilities& netAllCapabilities, NetCap netCap)
{
    netCaps.RemoveNetCap(netCap);
    std::unique_lock<std::shared_mutex> lock(netAllCapabilities.netCapsMutex_);
    netAllCapabilities.netCaps_.erase(netCap);
}

void NetSupplier::SetNetValid(NetDetectionStatus netState)
{
    NETMGR_LOG_I("Enter SetNetValid. supplier[%{public}d, %{public}s], ifValid[%{public}d]", supplierId_,
                 netSupplierIdent_.c_str(), netState);
    if (netState == VERIFICATION_STATE) {
        if (!HasNetCap(NET_CAPABILITY_VALIDATED)) {
            NETMGR_LOG_I("NetSupplier inserted cap:NET_CAPABILITY_VALIDATED");
            netCaps_.InsertNetCap(NET_CAPABILITY_VALIDATED);
            std::unique_lock<std::shared_mutex> lock(netAllCapabilities_.netCapsMutex_);
            netAllCapabilities_.netCaps_.insert(NET_CAPABILITY_VALIDATED);
        }
        if (HasNetCap(NET_CAPABILITY_PORTAL)) {
            NETMGR_LOG_I("NetSupplier remove cap:NET_CAPABILITY_PORTAL, need to clear DNS cache");
            RemoveNetCap(netCaps_, netAllCapabilities_, NET_CAPABILITY_PORTAL);
            int32_t ret = NetsysController::GetInstance().FlushDnsCache(network_->GetNetId());
            if (ret != NETMANAGER_SUCCESS) {
                NETMGR_LOG_E("FlushDnsCache failed, ret = %{public}d", ret);
            }
        }
    } else if (netState == CAPTIVE_PORTAL_STATE) {
        if (!HasNetCap(NET_CAPABILITY_PORTAL)) {
            NETMGR_LOG_I("NetSupplier inserted cap:NET_CAPABILITY_PORTAL");
            netCaps_.InsertNetCap(NET_CAPABILITY_PORTAL);
            std::unique_lock<std::shared_mutex> lock(netAllCapabilities_.netCapsMutex_);
            netAllCapabilities_.netCaps_.insert(NET_CAPABILITY_PORTAL);
        }
        if (HasNetCap(NET_CAPABILITY_VALIDATED)) {
            NETMGR_LOG_I("NetSupplier remove cap:NET_CAPABILITY_VALIDATED");
            RemoveNetCap(netCaps_, netAllCapabilities_, NET_CAPABILITY_VALIDATED);
        }
    } else if (netState == QUALITY_POOR_STATE) {
        netQuality_ = QUALITY_POOR_STATE;
    } else if (netState == QUALITY_GOOD_STATE) {
        netQuality_ = QUALITY_GOOD_STATE;
        SetOnceSuppress();
    } else if (netState == ACCEPT_UNVALIDATED) {
        netQuality_ = ACCEPT_UNVALIDATED;
        isAcceptUnvaliad = true;
    } else {
        if (HasNetCap(NET_CAPABILITY_VALIDATED)) {
            NETMGR_LOG_I("NetSupplier remove cap:NET_CAPABILITY_VALIDATED");
            RemoveNetCap(netCaps_, netAllCapabilities_, NET_CAPABILITY_VALIDATED);
        }
        if (HasNetCap(NET_CAPABILITY_PORTAL)) {
            NETMGR_LOG_I("NetSupplier remove cap:NET_CAPABILITY_PORTAL");
            RemoveNetCap(netCaps_, netAllCapabilities_, NET_CAPABILITY_PORTAL);
        }
    }
}

void NetSupplier::SetOnceSuppress()
{
    isOnceSuppress_ = IsAvailable() ? false : true;
}

bool NetSupplier::IsOnceSuppress() const
{
    if (IsNetQualityPoor()) {
        return false;
    }
    return isOnceSuppress_;
}

bool NetSupplier::IsNetValidated() const
{
    return HasNetCap(NET_CAPABILITY_VALIDATED) && !HasNetCap(NET_CAPABILITY_CHECKING_CONNECTIVITY);
}

/**
 * This method returns the score of the current network supplier.
 *
 * It is used to prioritize network suppliers so that higher priority producers can activate when lower
 * priority networks are available.
 *
 * @return the score of the current network supplier.
 */
int32_t NetSupplier::GetNetScore() const
{
    return netScore_;
}

/**
 * This method returns the real score of current network supplier.
 *
 * This method subtracts the score depending on different conditions, or returns netScore_ if the conditions are not
 * met.
 * It is used to compare the priorities of different networks.
 *
 * @return the real score of current network supplier.
 */
int32_t NetSupplier::GetRealScore()
{
    // Notice: the order is important here:
    // 1.If the user chooses to use this network, return MAX_SCORE
    if (isAcceptUnvaliad) {
        return static_cast<int32_t>(NetManagerStandard::NetTypeScoreValue::MAX_SCORE);
    }

    // 2. If network detection is not complete in the first time, subtract NET_VALID_SCORE.
    if (IsInFirstTimeDetecting()) {
        return netScore_ - NET_VALID_SCORE;
    }

    // 3. If network is not validated, subtract NET_VALID_SCORE.
    if (!IsNetValidated()) {
        return netScore_ - NET_VALID_SCORE;
    }

    // 4. Deduct DIFF_SCORE_BETWEEN_GOOD_POOR for poor network quality (reported by the supplier).
    if (IsNetQualityPoor()) {
        return netScore_ - DIFF_SCORE_BETWEEN_GOOD_POOR;
    }
    return netScore_;
}

void NetSupplier::SetDefault()
{
    NETMGR_LOG_I("set default supplier[%{public}d].", supplierId_);
    if (network_) {
        network_->SetDefaultNetWork();
    }
}

void NetSupplier::ClearDefault()
{
    NETMGR_LOG_I("clear default supplier[%{public}d].", supplierId_);
    if (network_) {
        network_->ClearDefaultNetWorkNetId();
    }
}

void NetSupplier::UpdateGlobalHttpProxy(const HttpProxy &httpProxy)
{
    NETMGR_LOG_I("supplierId[%{public}d] update global httpProxy.", supplierId_);
    if (network_) {
        network_->UpdateGlobalHttpProxy(httpProxy);
    }
}

std::string NetSupplier::TechToType(NetSlotTech techType)
{
    switch (techType) {
        case NetSlotTech::SLOT_TYPE_GSM:
            return "2G";
        case NetSlotTech::SLOT_TYPE_LTE:
        case NetSlotTech::SLOT_TYPE_LTE_CA:
            return "4G";
        default:
            return "3G";
    }
}

void NetSupplier::SetSupplierType(int32_t type)
{
    NETMGR_LOG_I("supplierId[%{public}d] update type[%{public}d].", supplierId_, type);
    type_ = type;
}

std::string NetSupplier::GetSupplierType()
{
    return type_ == -1 ? "" : TechToType(static_cast<NetSlotTech>(type_));
}

bool NetSupplier::ResumeNetworkInfo()
{
    if (network_ == nullptr) {
        NETMGR_LOG_E("network_ is nullptr!");
        return false;
    }

    return network_->ResumeNetworkInfo();
}

bool NetSupplier::IsNetQualityPoor() const
{
    return netQuality_ == QUALITY_POOR_STATE;
}

bool NetSupplier::IsNetAcceptUnavalidate()
{
    return netQuality_ == ACCEPT_UNVALIDATED;
}

void NetSupplier::SetDetectionDone()
{
    if (!isFirstTimeDetectionDone) {
        isFirstTimeDetectionDone = true;
    }
    if (HasNetCap(NET_CAPABILITY_CHECKING_CONNECTIVITY)) {
        NETMGR_LOG_I("supplier %{public}u detection done, remove NET_CAPABILITY_CHECKING_CONNECTIVITY", supplierId_);
        netCaps_.RemoveNetCap(NET_CAPABILITY_CHECKING_CONNECTIVITY);
        std::unique_lock<std::shared_mutex> lock(netAllCapabilities_.netCapsMutex_);
        netAllCapabilities_.netCaps_.erase(NET_CAPABILITY_CHECKING_CONNECTIVITY);
    }
}

bool NetSupplier::IsInFirstTimeDetecting() const
{
    return !isFirstTimeDetectionDone;
}

void NetSupplier::SetReuseCap(NetCap reuseCap, bool add)
{
    if (add) {
        netCaps_.InsertNetCap(reuseCap);
        std::unique_lock<std::shared_mutex> lock(netAllCapabilities_.netCapsMutex_);
        netAllCapabilities_.netCaps_.insert(reuseCap);
    } else {
        netCaps_.RemoveNetCap(reuseCap);
        std::unique_lock<std::shared_mutex> lock(netAllCapabilities_.netCapsMutex_);
        netAllCapabilities_.netCaps_.erase(reuseCap);
    }
}

void NetSupplier::UpdateNetCap(const std::set<NetCap> &netCaps)
{
    NetCaps caps(netCaps);
    netCaps_ = caps;
    std::unique_lock<std::shared_mutex> lock(netAllCapabilities_.netCapsMutex_);
    netAllCapabilities_.netCaps_ = netCaps;
}

std::string NetSupplier::GetNetExtAttribute()
{
    if (netExtAttribute_.empty() && netHandle_ != nullptr) {
        NETMGR_LOG_E("supplier %{public}u, netId: %{public}d, get netExtAttribute is empty",
            supplierId_, netHandle_->GetNetId());
    }
    return netExtAttribute_;
}

void NetSupplier::SetNetExtAttribute(const std::string &netExtAttribute)
{
    if (netHandle_ != nullptr) {
        NETMGR_LOG_I("supplier %{public}u, netId: %{public}d, set netExtAtt: [length: %{public}d, value: %{private}s]",
            supplierId_, netHandle_->GetNetId(), (int)netExtAttribute.size(), netExtAttribute.c_str());
    }
    netExtAttribute_ = netExtAttribute;
}
} // namespace NetManagerStandard
} // namespace OHOS

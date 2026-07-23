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

#ifndef NET_CONN_MANAGER_H
#define NET_CONN_MANAGER_H

#include <map>
#include <string>

#include "parcel.h"
#include "singleton.h"

#include "http_proxy.h"
#include "i_net_conn_service.h"
#include "i_net_interface_callback.h"
#include "i_net_supplier_callback.h"
#include "net_conn_info.h"
#include "net_handle.h"
#include "net_link_info.h"
#include "net_specifier.h"
#include "net_supplier_callback_base.h"
#include "i_net_factoryreset_callback.h"
#include "safe_map.h"
#include <shared_mutex>
#include "net_conn_callback_stub.h"
#include "refresh_http_proxy_callback_stub.h"
#include "net_trace_route_info.h"

namespace ffrt {
class queue;
};
namespace OHOS {
class ISystemAbilityStatusChange;
namespace nmd {
class FwmarkClient;
}
namespace NetManagerStandard {
constexpr uint32_t RESERVED_BUFFER_SIZE = 512;

class NetConnClient : public std::enable_shared_from_this<NetConnClient> {
public:
    /**
     * Do not use constor directly to create instance, it just for std::make_shared in `GetInstance()`
     */
    NetConnClient();
    ~NetConnClient();
    static NetConnClient &GetInstance();

    /**
     * The interface in NetConnService can be called when the system is ready
     *
     * @return Returns 0, the system is ready, otherwise the system is not ready
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SystemReady();

    /**
     * The interface is set permission for network
     *
     * @param The specified UID of app
     * @param allow internet permission
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetInternetPermission(uint32_t uid, uint8_t allow);

    /**
     * The interface is register the network
     *
     * @param bearerType Bearer Network Type
     * @param ident Unique identification of mobile phone card
     * @param netCaps Network capabilities registered by the network supplier
     * @param supplierId out param, return supplier id
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
                                uint32_t &supplierId);

    /**
     * The interface is unregister the network
     *
     * @param supplierId The id of the network supplier
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UnregisterNetSupplier(uint32_t supplierId);

    /**
     * Register supplier callback
     *
     * @param supplierId The id of the network supplier
     * @param callback INetSupplierCallback callback interface
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t RegisterNetSupplierCallback(uint32_t supplierId, const sptr<NetSupplierCallbackBase> &callback);

    /**
     * update net capabilities
     *
     * @param netCaps netcap set
     * @param supplierId The id of the network supplier
     * @return Returns 0, update net caps of the network successfully, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UpdateNetCaps(const std::set<NetCap> &netCaps, const uint32_t supplierId);

    /**
     * The interface is update network connection status information
     *
     * @param supplierId The id of the network supplier
     * @param netSupplierInfo network connection status information
     * @return Returns 0, successfully update the network connection status information, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo);

    /**
     * The interface is update network link attribute information
     *
     * @param supplierId The id of the network supplier
     * @param netLinkInfo network link attribute information
     * @return Returns 0, successfully update the network link attribute information, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo);

    /**
     * @param supplierId The id of the network supplier
     * @param reuseSupplierId The id of the reused network supplier
     * @param isReused whether to reuse supplier id
     * @return Returns 0, successfully set reuse supplier id, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetReuseSupplierId(uint32_t supplierId, uint32_t reuseSupplierId, bool isReused);

    /**
     * Register net connection callback
     *
     * @param callback The callback of INetConnCallback interface
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t RegisterNetConnCallback(const sptr<INetConnCallback> callback);

    /**
     * Register net connection callback by NetSpecifier
     *
     * @param netSpecifier specifier information
     * @param callback The callback of INetConnCallback interface
     * @param timeoutMS net connection time out
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> callback,
                                    const uint32_t &timeoutMS);

    /**
     * Request net connection callback by NetSpecifier
     *
     * @param netSpecifier specifier information
     * @param callback The callback of INetConnCallback interface
     * @param timeoutMS net connection time out
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t RequestNetConnection(const sptr<NetSpecifier> netSpecifier, const sptr<INetConnCallback> callback,
                                    const uint32_t timeoutMS);
    /**
     * Unregister net connection callback
     *
     * @param callback The callback of INetConnCallback interface
     * @return Returns 0, successfully unregister net connection callback, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UnregisterNetConnCallback(const sptr<INetConnCallback> &callback);

    /**
     * Register net detection callback by netId
     *
     * @param netSpecifier specifier information
     * @param callback The callback of INetDetectionCallback interface
     * @param timeoutMS net connection time out
     * @return Returns 0, successfully register net detection callback, otherwise it will failed
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t RegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback);
    /**
     * Unregister net detection callback by netId
     *
     * @param callback The callback of INetDetectionCallback interface
     * @return Returns 0, successfully unregister net detection callback, otherwise it will fail
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UnRegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback);
    
    /**
     * The interface is to get default network
     *
     * @param netHandle network handle
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetDefaultNet(NetHandle &netHandle);

    /**
     * The interface is to check whether has default network
     *
     * @param flag has default network or not
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t HasDefaultNet(bool &flag);

    /**
     * The interface is to get all acvite network
     *
     * @param netList a list of network
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAllNets(std::list<sptr<NetHandle>> &netList);

    /**
     * get the network link information of the connection
     *
     * @param netHandle network handle
     * @param info network link infomation
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetConnectionProperties(const NetHandle &netHandle, NetLinkInfo &info);

    /**
     * get all capabilities from network
     *
     * @param netHandle network handle
     * @param netAllCap network all of capabilities
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetNetCapabilities(const NetHandle &netHandle, NetAllCapabilities &netAllCap);

    /**
     * The interface is to get all iface and ident maps
     *
     * @param bearerType the type of network
     * @param ifaceNameIdentMaps the map of ifaceName and ident
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetIfaceNameIdentMaps(NetBearType bearerType, SafeMap<std::string, std::string> &ifaceNameIdentMaps);

    /**
     * The interface is to bind socket
     *
     * @param socketFd socket file description
     * @param netId network id
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t BindSocket(int32_t socketFd, int32_t netId);

    /**
     * The interface of network detection called by the application
     *
     * @param netHandle network handle
     * @return int32_t Whether the network probe is successful
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t NetDetection(const NetHandle &netHandle);

    /**
     * The interface of network detection called by the application
     *
     * @param rawUrl detection url
     * @param resp detection result
     * @return int32_t Whether the network probe is successful
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t NetDetection(const std::string &rawUrl, PortalResponse &resp);

    /**
     * set air plane mode on or off
     *
     * @param state air plane mode on or not
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetAirplaneMode(bool state);

    /**
     * check whether the network meter is default
     *
     * @param isMetered the network meter is default or not
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t IsDefaultNetMetered(bool &isMetered);

    /**
     * set global http proxy in the network
     *
     * @param httpProxy http proxy
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetGlobalHttpProxy(const HttpProxy &httpProxy);

    /**
     * get global http proxy in the network
     *
     * @param httpProxy http proxy
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetGlobalHttpProxy(HttpProxy &httpProxy);

    /**
     * set network id of app binding network
     *
     * @param netId network id
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetDefaultHttpProxy(HttpProxy &httpProxy);

    int32_t RefreshGlobalHttpProxy(const std::function<void(int32_t, const HttpProxy &)> &callback);
    void ResetRefreshState();
    int32_t PrepareRefreshCallback(const std::function<void(int32_t, const HttpProxy &)> &callback,
        bool &needSendRequest);
    int32_t SendRefreshHttpProxyRequest(const sptr<IRefreshHttpProxyCallback> &stub);

    /**
     * set network id of app binding network
     *
     * @param netId network id
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetAppNet(int32_t netId);

    /**
     * get network id of app binding network
     *
     * @param netId network id
     * @return Returns 0 success. Otherwise fail.
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAppNet(int32_t &netId);

    /**
     * protect net by process for vpn
     *
     * @return Returns 0 success. Otherwise fail.
     * @systemapi Hide this for inner system use.
     */
    int32_t ProtectProcessNet();

    /**
     * Get network id by identifier
     *
     * @param ident identifier
     * @param netIdList  list of network id
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList);

    /**
     * Register network interface state change callback
     *
     * @param callback The callback of INetInterfaceStateCallback interface
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t RegisterNetInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback);

    /**
     * UnRegister network interface state change callback
     *
     * @param callback The callback of INetInterfaceStateCallback interface
     * @return Returns 0, successfully unregister net connection callback, otherwise it will failed
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UnregisterNetInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback);

    /**
     * Get network interface configuration
     *
     * @param ifaceName Network port device name
     * @param config Network interface configuration
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetNetInterfaceConfiguration(const std::string &iface, NetInterfaceConfiguration &config);
    
    int32_t SetNetInterfaceIpAddress(const std::string &iface, const std::string &ipAddress);
    int32_t SetInterfaceUp(const std::string &iface);
    int32_t SetInterfaceDown(const std::string &iface);

    int32_t AddNetworkRoute(int32_t netId, const std::string &ifName, const std::string &destination,
                            const std::string &nextHop);
    int32_t RemoveNetworkRoute(int32_t netId, const std::string &ifName, const std::string &destination,
                               const std::string &nextHop);
    int32_t AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                int32_t prefixLength);
    int32_t DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                int32_t prefixLength);
    int32_t AddStaticArp(const std::string &ipAddr, const std::string &macAddr, const std::string &ifName);
    int32_t DelStaticArp(const std::string &ipAddr, const std::string &macAddr, const std::string &ifName);

    int32_t RegisterSlotType(uint32_t supplierId, int32_t type);
    int32_t GetSlotType(std::string &type);
    int32_t FactoryResetNetwork();
    int32_t RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback);
    void RegisterAppHttpProxyCallback(std::function<void(const HttpProxy &httpProxy)> callback, uint32_t &callbackid);
    void UnregisterAppHttpProxyCallback(uint32_t callbackid);
    int32_t SetAppHttpProxy(const HttpProxy &httpProxy);
     /**
     * Whether this url prefer cellular
     *
     * @param url url input
     * @param preferCellular out param, whether prefer cellular
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t IsPreferCellularUrl(const std::string& url, PreferCellularType& preferCellular);

    int32_t RegisterPreAirplaneCallback(const sptr<IPreAirplaneCallback> callback);

    int32_t UnregisterPreAirplaneCallback(const sptr<IPreAirplaneCallback> callback);
    int32_t UpdateSupplierScore(uint32_t supplierId, uint32_t detectionStatus);
    int32_t GetDefaultSupplierId(NetBearType bearerType, const std::string &ident,
        uint32_t& supplierId);

    int32_t EnableVnicNetwork(const sptr<NetLinkInfo> &netLinkInfo, const std::set<int32_t> &uids);

    int32_t DisableVnicNetwork();

    /**
     * This function returns whether the caller process's API version is not earlier
     * than {@link targetApiVersion}, which meaning the caller process has same or later
     * target API version.
     *
     * @param targetApiVersion target API version.
     * @return true for supported and false for not, and true by default if cannot get
     * process bundle's information.
     */
    static bool IsAPIVersionSupported(int targetApiVersion);

    /**
     * This function returns the caller's bundle name.
     * This function is defined here because it is required in some Network Kit APIs.
     * Please do not use this function except Network Kit APIs.
     *
     * @return optional bundle name in string format, return empty if cannot get bundle
     * info from bundle manager.
     */
    static std::optional<std::string> ObtainBundleNameForSelf();

    int32_t EnableDistributedClientNet(const std::string &virnicAddr,
        const std::string &virnicName, const std::string &iif);

    int32_t EnableDistributedServerNet(const std::string &iif, const std::string &devIface, const std::string &dstAddr,
                                       const std::string &gw);

    int32_t DisableDistributedNet(bool isServer, const std::string &virnicName, const std::string &dstAddr);

    int32_t CloseSocketsUid(int32_t netId, uint32_t uid);

    void RecoverCallbackAndGlobalProxy();

    int32_t SetPacUrl(const std::string &pacUrl);

    int32_t GetPacUrl(std::string &pacUrl);

    int32_t SetPacFileUrl(const std::string &pacUrl);

    int32_t SetProxyMode(const OHOS::NetManagerStandard::ProxyModeType mode);

    int32_t GetProxyMode(OHOS::NetManagerStandard::ProxyModeType &mode);

    int32_t GetPacFileUrl(std::string &pacUrl);

    int32_t FindProxyForURL(const std::string &url, std::string &proxy, const std::string host = "");

    int32_t QueryTraceRoute(const std::string &destination, int32_t maxJumpNumber,
        int32_t packetsType, std::string &traceRouteInfo, bool isCallerNative = true);

    int32_t GetSpecificNet(NetBearType bearerType, std::list<int32_t> &netIdList);
    int32_t GetSpecificNetByIdent(NetBearType bearerType, const std::string &ident, std::list<int32_t> &netIdList);

    int32_t SetAppIsFrozened(uint32_t uid, bool isFrozened);
    int32_t IsDeadFlowResetTargetBundle(const std::string &bundleName, bool &flag);
    int32_t EnableAppFrozenedCallbackLimitation(bool flag);

    void UnsubscribeSystemAbility();

    int32_t GetNetExtAttribute(const NetHandle &netHandle, std::string &netExtAttribute);
    int32_t SetNetExtAttribute(const NetHandle &netHandle, const std::string &netExtAttribute);
    int32_t AddStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr, const std::string &ifName);
    int32_t DelStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr, const std::string &ifName);
    int32_t GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo);
    int32_t CreateVlan(const std::string &ifName, uint32_t vlanId);
    int32_t DestroyVlan(const std::string &ifName, uint32_t vlanId);
    int32_t AddVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask);
    int32_t DeleteVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask);
    int32_t GetConnectOwnerUid(const NetConnInfo &netConnInfo, int32_t &ownerUid);
    int32_t GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo);

private:
    class NetConnDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit NetConnDeathRecipient(NetConnClient &client) : client_(client) {}
        ~NetConnDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }

    private:
        NetConnClient &client_;
    };

    class NetConnCallbackManager : public NetConnCallbackStub {
        friend NetConnClient;
    public:
        NetConnCallbackManager(std::shared_ptr<ffrt::queue>& ffrtQueue);
        ~NetConnCallbackManager() override = default;
        int32_t NetAvailable(sptr<NetHandle> &netHandle) override;
        int32_t NetCapabilitiesChange(sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap) override;
        int32_t NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info) override;
        int32_t NetLost(sptr<NetHandle> &netHandle) override;
        int32_t NetUnavailable() override;
        int32_t NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked) override;
 
        int32_t AddNetConnCallback(const sptr<INetConnCallback>& callback);
        void RemoveNetConnCallback(const sptr<INetConnCallback>& callback);
        bool HasExistCallback(const sptr<INetConnCallback>& callback);
    private:
        std::mutex netHandlerMutex_;
        sptr<NetHandle> netHandle_ = nullptr;
        sptr<NetAllCapabilities> netAllCap_ = nullptr;
        sptr<NetLinkInfo> netLinkInfo_ = nullptr;
        std::shared_mutex netConnCallbackListMutex_;
        std::list<sptr<INetConnCallback>> netConnCallbackList_;
        bool isNetStateUpdated_ = false;
        std::shared_ptr<ffrt::queue> ffrtQueue_ = nullptr;
    };

private:
    NetConnClient& operator=(const NetConnClient&) = delete;
    NetConnClient(const NetConnClient&) = delete;

    sptr<INetConnService> GetProxy();
    void OnRemoteDied(const wptr<IRemoteObject> &remote);
    void DlCloseRemoveDeathRecipient();
    static std::optional<int32_t> ObtainTargetApiVersionForSelf();
    static std::optional<std::string> ObtainBundleNameFromBundleMgr();
    void SubscribeSystemAbility();
    using NetConnCallbackManagerMap = std::map<sptr<NetSpecifier>, sptr<NetConnCallbackManager>>;
    int32_t UnRegisterNetConnCallbackManager(const sptr<INetConnCallback>& callback,
        NetConnCallbackManagerMap& netConnCallbackManagerMap);
    void RecoverCallbackAndGlobalProxy(NetConnCallbackManagerMap& netConnCallbackManagerMap);
    sptr<NetConnClient::NetConnCallbackManager> FindConnCallbackManager(NetConnCallbackManagerMap &managerMap,
        const sptr<NetSpecifier> &netSpecifier);
    bool IsCallbackExist(const sptr<INetConnCallback> &callback);

private:
    std::mutex appHttpProxyCbMapMutex_;
    uint32_t currentCallbackId_ = 0;
    std::map<uint32_t, std::function<void(const HttpProxy &httpProxy)>> appHttpProxyCbMap_;
    HttpProxy appHttpProxy_;
    HttpProxy globalHttpProxy_;
    char buffer_[RESERVED_BUFFER_SIZE] = {0};
    std::mutex mutex_;
    sptr<INetConnService> NetConnService_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    std::map<uint32_t, sptr<INetSupplierCallback>> netSupplierCallback_;
    std::list<std::tuple<sptr<NetSpecifier>, sptr<INetConnCallback>, uint32_t>> registerConnTupleList_;
    sptr<NetSpecifier> defaultNetSpecifier_ = nullptr;
    std::shared_mutex netConnCallbackManagerMapMutex_;
    NetConnCallbackManagerMap netConnCallbackManagerMap_;
    NetConnCallbackManagerMap systemNetConnCallbackManagerMap_;
    SafeMap<uint32_t, uint8_t> netPermissionMap_;
    sptr<IPreAirplaneCallback> preAirplaneCallback_;
    std::mutex registerConnTupleListMutex_;
    std::mutex netSupplierCallbackMutex_;
    std::string pacUrl_;
    sptr<ISystemAbilityStatusChange> saStatusListener_;
    std::mutex refreshCbMutex_;
    sptr<RefreshHttpProxyCallbackStub> refreshCallbackStub_;
    bool refreshInProgress_ = false;
    std::vector<std::function<void(int32_t, const HttpProxy &)>> pendingRefreshCallbacks_;
    static inline std::mutex instanceMtx_;
    static inline std::shared_ptr<NetConnClient> instance_ = nullptr;
    std::shared_ptr<ffrt::queue> ffrtQueue_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_CONN_MANAGER_H

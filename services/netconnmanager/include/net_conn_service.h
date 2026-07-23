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

#ifndef NET_CONN_SERVICE_H
#define NET_CONN_SERVICE_H

#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <thread>
#include <condition_variable>
#include <tuple>

#include "singleton.h"
#include "system_ability.h"

#include "http_proxy.h"
#include "net_activate.h"
#include "net_conn_constants.h"
#include "net_conn_event_handler.h"
#include "net_conn_service_iface.h"
#include "net_conn_service_stub.h"
#include "i_refresh_http_proxy_callback.h"
#include "net_supplier.h"
#include "netsys_controller_callback.h"
#include "network.h"
#include "dns_result_call_back.h"
#include "net_factoryreset_callback.h"
#include "net_policy_callback_stub.h"
#include "net_policy_service.h"
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "os_account_manager.h"
#include "app_state_aware.h"

#include "net_trace_route_probe.h"
#ifdef NETMANAGER_ENABLE_PAC_PROXY
#include "net_pac_local_proxy_server.h"
#include "net_pac_manager.h"
#endif
namespace OHOS {
namespace NetManagerStandard {
using EventReceiver = std::function<void(const EventFwk::CommonEventData&)>;
class NetDataShareHelperUtilsIface;
class NetConnService : public SystemAbility,
                       public INetActivateCallback,
                       public NetConnServiceStub,
                       public std::enable_shared_from_this<NetConnService> {
    DECLARE_SYSTEM_ABILITY(NetConnService)

    NetConnService();
    virtual ~NetConnService();
    using NET_SUPPLIER_MAP = std::map<uint32_t, sptr<NetSupplier>>;
    using NET_UIDREQUEST_MAP = std::map<uint32_t, uint32_t>;
    using NET_UIDACTIVATE_MAP = std::map<uint32_t, std::vector<std::shared_ptr<NetActivate>>>;

public:
    class NetConnListener : public EventFwk::CommonEventSubscriber {
    public:
        NetConnListener(const EventFwk::CommonEventSubscribeInfo &subscribeInfo, EventReceiver receiver);
        void OnReceiveEvent(const EventFwk::CommonEventData &data) override;

    private:
        EventReceiver eventReceiver_;
    };
    static std::shared_ptr<NetConnService> &GetInstance()
    {
        static std::shared_ptr<NetConnService> instance = std::make_shared<NetConnService>();
        return instance;
    }
#ifdef NETMANAGER_ENABLE_PAC_PROXY
    std::shared_ptr<NetPACManager> GetNetPacManager();
    bool StartPacLocalProxyServer();
    int StopPacLocalProxyServer();
    uint32_t SetProxyOff();
    uint32_t SetProxyAuto();
#endif
    void OnStart() override;
    void OnStop() override;
    /**
     * The interface in NetConnService can be called when the system is ready
     *
     * @return Returns 0, the system is ready, otherwise the system is not ready
     */
    int32_t SystemReady() override;

    /**
     * Disallow or allow a app to create AF_INET or AF_INET6 socket
     *
     * @param uid App's uid which need to be disallowed ot allowed to create AF_INET or AF_INET6 socket
     * @param allow 0 means disallow, 1 means allow
     * @return return 0 if OK, return error number if not OK
     */
    int32_t SetInternetPermission(uint32_t uid, uint8_t allow) override;

    /**
     * The interface is register the network
     *
     * @param bearerType Bearer Network Type
     * @param ident Unique identification of mobile phone card
     * @param netCaps Network capabilities registered by the network supplier
     * @param supplierId out param, return supplier id
     *
     * @return function result
     */
    int32_t RegisterNetSupplier(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
                                uint32_t &supplierId) override;

    /**
     * The interface is unregister the network
     *
     * @param supplierId The id of the network supplier
     *
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t UnregisterNetSupplier(uint32_t supplierId) override;

    /**
     * Register supplier callback
     *
     * @param supplierId The id of the network supplier
     * @param callback INetSupplierCallback callback interface
     *
     * @return Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t RegisterNetSupplierCallback(uint32_t supplierId, const sptr<INetSupplierCallback> &callback) override;

    /**
     * Register net connection callback
     *
     * @param netSpecifier specifier information
     * @param callback The callback of INetConnCallback interface
     *
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     */
    int32_t RegisterNetConnCallback(const sptr<INetConnCallback> callback) override;

    /**
     * Register net connection callback by NetSpecifier
     *
     * @param netSpecifier specifier information
     * @param callback The callback of INetConnCallback interface
     * @param timeoutMS net connection time out
     *
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     */
    int32_t RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> callback,
                                    const uint32_t &timeoutMS) override;

    /**
     * Request net connection callback by NetSpecifier
     *
     * @param netSpecifier specifier information
     * @param callback The callback of INetConnCallback interface
     * @param timeoutMS net connection time out
     *
     * @return Returns 0, successfully register net connection callback, otherwise it will failed
     */
    int32_t RequestNetConnection(const sptr<NetSpecifier> netSpecifier, const sptr<INetConnCallback> callback,
                                    const uint32_t timeoutMS) override;
    /**
     * Unregister net connection callback
     *
     * @return Returns 0, successfully unregister net connection callback, otherwise it will fail
     */
    int32_t UnregisterNetConnCallback(const sptr<INetConnCallback> &callback) override;

    int32_t UpdateNetStateForTest(const sptr<NetSpecifier> &netSpecifier, int32_t netState) override;

    /**
     * update net capabilities
     *
     * @param netCaps netcap set
     * @param supplierId The id of the network supplier
     * @return Returns 0, update net caps of the network successfully, otherwise it will fail
     */
    int32_t UpdateNetCaps(const std::set<NetCap> &netCaps, const uint32_t supplierId) override;

    /**
     * The interface is update network connection status information
     *
     * @param supplierId The id of the network supplier
     * @param netSupplierInfo network connection status information
     *
     * @return Returns 0, successfully update the network connection status information, otherwise it will fail
     */
    int32_t UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo) override;

    /**
     * The interface is update network link attribute information
     *
     * @param supplierId The id of the network supplier
     * @param netLinkInfo network link attribute information
     *
     * @return Returns 0, successfully update the network link attribute information, otherwise it will fail
     */
    int32_t UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo) override;

    /**
     * The interface is set reuse supplier id
     *
     * @param supplierId The id of the network supplier
     * @param reuseSupplierId The id of the reuse network supplier
     * @param isReused whether to reuse supplier id
     *
     * @return Returns 0, successfully update the network link attribute information, otherwise it will fail
     */
    int32_t SetReuseSupplierId(uint32_t supplierId, uint32_t reuseSupplierId, bool isReused) override;

    /**
     * The interface names which NetBearType is equal than bearerType
     *
     * @param bearerType Network bearer type
     * @param ifaceNames save the obtained ifaceNames
     * @return Returns 0, successfully get the network link attribute iface name, otherwise it will fail
     */
    int32_t GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames) override;

    /**
     * The interface is get the iface name for network
     *
     * @param bearerType Network bearer type
     * @param ident Unique identification of mobile phone card
     * @param ifaceName save the obtained ifaceName
     * @return Returns 0, successfully get the network link attribute iface name, otherwise it will fail
     */
    int32_t GetIfaceNameByType(NetBearType bearerType, const std::string &ident, std::string &ifaceName) override;

    /**
     * The interface is to get all iface and ident maps
     *
     * @param bearerType the type of network
     * @param ifaceNameIdentMaps the map of ifaceName and ident
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetIfaceNameIdentMaps(NetBearType bearerType,
                                  SafeMap<std::string, std::string> &ifaceNameIdentMaps) override;

    /**
     * register network detection return result method
     *
     * @param netId  Network ID
     * @param callback The callback of INetDetectionCallback interface
     * @return int32_t  Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t RegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback) override;

    /**
     * unregister network detection return result method
     *
     * @param netId Network ID
     * @param callback  The callback of INetDetectionCallback interface
     * @return int32_t  Returns 0, unregister the network successfully, otherwise it will fail
     */
    int32_t UnRegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback) override;

    /**
     * The interface of network detection called by the application
     *
     * @param netId network ID
     * @return int32_t Whether the network probe is successful
     */
    int32_t NetDetection(int32_t netId) override;
    int32_t NetDetection(const std::string &rawUrl, PortalResponse &resp) override;
    int32_t GetDefaultNet(int32_t &netId) override;
    int32_t HasDefaultNet(bool &flag) override;
    int32_t GetSpecificNet(NetBearType bearerType, std::list<int32_t> &netIdList) override;
    int32_t GetSpecificNetByIdent(NetBearType bearerType, const std::string &ident,
        std::list<int32_t> &netIdList) override;
    int32_t GetAllNetsAsync(std::list<int32_t> &netIdList);
    int32_t GetAllNets(std::list<int32_t> &netIdList) override;
    int32_t GetSpecificUidNet(int32_t uid, int32_t &netId) override;
    int32_t GetConnectionProperties(int32_t netId, NetLinkInfo &info) override;
    int32_t GetNetCapabilities(int32_t netId, NetAllCapabilities &netAllCap) override;
    void HandleDetectionResult(uint32_t supplierId, NetDetectionStatus netState);
    int32_t RestrictBackgroundChanged(bool isRestrictBackground);
    /**
     * Set airplane mode
     *
     * @param state airplane state
     * @return Returns 0, successfully set airplane mode, otherwise it will fail
     */
    int32_t SetAirplaneMode(bool state) override;
    /**
     * Dump
     *
     * @param fd file description
     * @param args unused
     * @return Returns 0, successfully get dump info, otherwise it will fail
     */
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    /**
     * Is default network metered
     *
     * @param save the metered state
     * @return Returns 0, Successfully get whether the default network is metered, otherwise it will fail
     */
    int32_t IsDefaultNetMetered(bool &isMetered) override;

    /**
     * Set http proxy server
     *
     * @param httpProxy the http proxy server
     * @return NETMANAGER_SUCCESS if OK, NET_CONN_ERR_HTTP_PROXY_INVALID if httpProxy is null string
     */
    int32_t SetGlobalHttpProxy(const HttpProxy &httpProxy) override;

    /**
     * Get http proxy server
     *
     * @param httpProxy output param, the http proxy server
     * @return NETMANAGER_SUCCESS if OK, NET_CONN_ERR_NO_HTTP_PROXY if httpProxy is null string
     */
    int32_t GetGlobalHttpProxy(HttpProxy &httpProxy) override;

    /**
     * Obtains the default proxy settings.
     *
     * <p>If a global proxy is set, the global proxy parameters are returned.
     * If the process is bound to a network using {@link setAppNet},
     * the {@link Network} proxy settings are returned.
     * In other cases, the default proxy settings of network are returned.
     *
     * @param bindNetId App bound network ID
     * @param httpProxy output param, the http proxy server
     * @return Returns NETMANAGER_SUCCESS even if HttpProxy is empty
     */
    int32_t GetDefaultHttpProxy(int32_t bindNetId, HttpProxy &httpProxy) override;

    int32_t RefreshGlobalHttpProxy(const sptr<IRefreshHttpProxyCallback> &callback) override;

    /**
     * Get net id by identifier
     *
     * @param ident Net identifier
     * @param netIdList output param, the net id list
     * @return NETMANAGER_SUCCESS if OK, ERR_NO_NET_IDENT if ident is null string
     */
    int32_t GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList) override;

    /**
     * Activate network timeout
     *
     * @param activate NetActivate pointer
     */
    void OnNetActivateTimeOut(std::shared_ptr<NetActivate> activate) override;

    /**
     * The interface of network detection called when DNS health check failed
     *
     * @param netId network ID
     * @return int32_t Whether the network probe is successful
     */
    int32_t NetDetectionForDnsHealth(int32_t netId, bool dnsHealthSuccess);

    int32_t SetAppNet(int32_t netId) override;
    int32_t RegisterNetInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback) override;
    int32_t UnregisterNetInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback) override;
    int32_t GetNetInterfaceConfiguration(const std::string &iface, NetInterfaceConfiguration &config) override;
    int32_t SetNetInterfaceIpAddress(const std::string &iface, const std::string &ipAddress) override;
    int32_t SetInterfaceUp(const std::string &iface) override;
    int32_t SetInterfaceDown(const std::string &iface) override;
    int32_t AddNetworkRoute(int32_t netId, const std::string &ifName,
                            const std::string &destination, const std::string &nextHop) override;
    int32_t RemoveNetworkRoute(int32_t netId, const std::string &ifName,
                               const std::string &destination, const std::string &nextHop) override;
    int32_t AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                int32_t prefixLength) override;
    int32_t DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                int32_t prefixLength) override;
    int32_t AddStaticArp(const std::string &ipAddr, const std::string &macAddr,
                         const std::string &ifName) override;
    int32_t DelStaticArp(const std::string &ipAddr, const std::string &macAddr,
                         const std::string &ifName) override;
    int32_t RegisterSlotType(uint32_t supplierId, int32_t type) override;
    int32_t GetSlotType(std::string &type) override;
    int32_t FactoryResetNetwork() override;
    int32_t RegisterNetFactoryResetCallback(const sptr<INetFactoryResetCallback> &callback) override;
    int32_t IsPreferCellularUrl(const std::string& url, PreferCellularType& preferCellular) override;
    int32_t RegisterPreAirplaneCallback(const sptr<IPreAirplaneCallback> callback) override;
    int32_t UnregisterPreAirplaneCallback(const sptr<IPreAirplaneCallback> callback) override;
    bool IsIfaceNameInUse(const std::string &ifaceName, int32_t netId);
    int32_t UpdateSupplierScore(uint32_t supplierId, uint32_t detectionStatus) override;
    int32_t GetDefaultSupplierId(NetBearType bearerType, const std::string &ident,
        uint32_t& supplierId) override;
    int32_t EnableVnicNetwork(const sptr<NetLinkInfo> &netLinkInfo, const std::set<int32_t> &uids) override;
    int32_t DisableVnicNetwork() override;
    int32_t EnableDistributedClientNet(const std::string &virnicAddr, const std::string &virnicName,
                                       const std::string &iif) override;
    int32_t EnableDistributedServerNet(const std::string &iif, const std::string &devIface,
                                       const std::string &dstAddr, const std::string &gw) override;
    int32_t DisableDistributedNet(bool isServer, const std::string &virnicName, const std::string &dstAddr) override;
    int32_t CloseSocketsUid(int32_t netId, uint32_t uid) override;
    int32_t SetPacUrl(const std::string &pacUrl) override;
    int32_t GetPacUrl(std::string &pacUrl) override;
    int32_t SetPacFileUrl(const std::string &pacUrl) override;
    int32_t SetProxyMode(const OHOS::NetManagerStandard::ProxyModeType mode) override;
    int32_t GetProxyMode(OHOS::NetManagerStandard::ProxyModeType &mode) override;
    int32_t GetPacFileUrl(std::string &pacUrl) override;
    int32_t FindProxyForURL(const std::string &url, const std::string &host, std::string &proxy) override;
    int32_t QueryTraceRoute(const std::string &destination, int32_t maxJumpNumber, int32_t packetsType,
        std::string &traceRouteInfo, bool isCallerNative) override;
    int32_t SetAppIsFrozened(uint32_t uid, bool isFrozened) override;
    int32_t IsDeadFlowResetTargetBundle(const std::string &bundleName, bool &flag) override;
    int32_t EnableAppFrozenedCallbackLimitation(bool flag) override;
    bool IsAppFrozenedCallbackLimitation();
    int32_t SetNetExtAttribute(int32_t netId, const std::string &netExtAttribute) override;
    int32_t GetNetExtAttribute(int32_t netId, std::string &netExtAttribute) override;
    int32_t AddStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName) override;
    int32_t DelStaticIpv6Addr(const std::string &ipv6Addr, const std::string &macAddr,
        const std::string &ifName) override;
    int32_t UpdateUidLostDelay(const std::set<uint32_t> &uidLostDelaySet);
    int32_t UpdateUidDeadFlowReset(const std::vector<std::string> &bundleNameVec);
    int32_t RegUnRegisterNetProbeCallback(int32_t netId,
        std::shared_ptr<IDualStackProbeCallback>& callback, bool isReg);
    int32_t DualStackProbe(uint32_t netId);
    int32_t UpdateDualStackProbeTime(int32_t dualStackProbeTime);
    int32_t GetIpNeighTable(std::vector<NetIpMacInfo> &ipMacInfo) override;
    ProbeUrls GetDataShareUrl();
    void HandleDataShareMessage();
    int32_t CreateVlan(const std::string &ifName, uint32_t vlanId) override;
    int32_t DestroyVlan(const std::string &ifName, uint32_t vlanId) override;
    int32_t AddVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask) override;
    int32_t DeleteVlanIp(const std::string &ifName, uint32_t vlanId, const std::string &ip, uint32_t mask) override;
    void SendNetPolicyChange(uint32_t uid, uint32_t policy);
    int32_t GetConnectOwnerUid(const NetConnInfo &netConnInfo, int32_t &ownerUid) override;
    int32_t GetSystemNetPortStates(NetPortStatesInfo &netPortStatesInfo) override;
    bool RegisterNetRequestControlFunc(std::function<bool(const NetRequest &)> func);
    bool GetAllNetRequest(std::vector<NetRequest> &netRequests);
    bool UpdateNetRequestControlState(const std::vector<NetRequest> &netRequests);

private:
    class NetInterfaceStateCallback : public NetsysControllerCallback {
    public:
        NetInterfaceStateCallback() = default;
        ~NetInterfaceStateCallback() = default;
        int32_t OnInterfaceAddressUpdated(const std::string &addr, const std::string &ifName, int flags,
                                          int scope) override;
        int32_t OnInterfaceAddressRemoved(const std::string &addr, const std::string &ifName, int flags,
                                          int scope) override;
        int32_t OnInterfaceAdded(const std::string &iface) override;
        int32_t OnInterfaceRemoved(const std::string &iface) override;
        int32_t OnInterfaceChanged(const std::string &iface, bool up) override;
        int32_t OnInterfaceLinkStateChanged(const std::string &iface, bool up) override;
        int32_t OnRouteChanged(bool updated, const std::string &route, const std::string &gateway,
                               const std::string &ifName) override;
        int32_t OnDhcpSuccess(NetsysControllerCallback::DhcpResult &dhcpResult) override;
        int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface) override;

        int32_t RegisterInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback);
        int32_t UnregisterInterfaceCallback(const sptr<INetInterfaceStateCallback> &callback);

    private:
    class NetIfaceStateCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
        public:
            explicit NetIfaceStateCallbackDeathRecipient(NetInterfaceStateCallback &client) : client_(client) {}
            ~NetIfaceStateCallbackDeathRecipient() override = default;
            void OnRemoteDied(const wptr<IRemoteObject> &remote) override
            {
                client_.OnNetIfaceStateRemoteDied(remote);
            }

        private:
            NetInterfaceStateCallback &client_;
        };

        std::mutex mutex_;
        std::vector<sptr<INetInterfaceStateCallback>> ifaceStateCallbacks_;
        sptr<IRemoteObject::DeathRecipient> netIfaceStateDeathRecipient_ = nullptr;

        void OnNetIfaceStateRemoteDied(const wptr<IRemoteObject> &remoteObject);
        void AddIfaceDeathRecipient(const sptr<INetInterfaceStateCallback> &callback);
    };

    class NetPolicyCallback : public NetPolicyCallbackStub {
    public:
        NetPolicyCallback(std::weak_ptr<NetConnService> netConnService) : netConnService_(netConnService) {}
        int32_t NetUidPolicyChange(uint32_t uid, uint32_t policy) override;

    private:
        std::weak_ptr<NetConnService> netConnService_;
    };

protected:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

private:
    enum RegisterType {
        INVALIDTYPE,
        REGISTER,
        REQUEST,
    };
    enum UserIdType {
        ACTIVE,
        LOCAL,
        SPECIFY,
    };
    bool Init();
    void SetCurlOptions(CURL *curl, HttpProxy tempProxy);
    void GetHttpUrlFromConfig(std::string &httpUrl);
    std::list<sptr<NetSupplier>> GetNetSupplierFromList(NetBearType bearerType, const std::string &ident = "");
    sptr<NetSupplier> GetNetSupplierFromList(NetBearType bearerType, const std::string &ident,
                                             const std::set<NetCap> &netCaps);
    int32_t ActivateNetwork(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> &callback,
                            const uint32_t &timeoutMS, const int32_t registerType = REGISTER,
                            const uint32_t callingUid = 0);
    void CallbackForSupplier(sptr<NetSupplier> &supplier, CallbackType type);
    void CallbackForAvailable(sptr<NetSupplier> &supplier, const sptr<INetConnCallback> &callback);
    uint32_t FindBestNetworkForRequest(sptr<NetSupplier> &supplier, std::shared_ptr<NetActivate> &netActivateNetwork);
    uint32_t FindInternalNetworkForRequest(std::shared_ptr<NetActivate> &netActivateNetwork,
                                           sptr<NetSupplier> &supplier);
    void SendRequestToAllNetwork(std::shared_ptr<NetActivate> request);
    void SendBestScoreAllNetwork(uint32_t reqId, int32_t bestScore, uint32_t supplierId,
                                 NetBearType supplierType, uint32_t uid);
    void SendAllRequestToNetwork(sptr<NetSupplier> supplier);
    void FindBestNetworkForAllRequest();
    void MakeDefaultNetWork(sptr<NetSupplier> &oldService, sptr<NetSupplier> &newService);
    void NotFindBestSupplier(uint32_t reqId, const std::shared_ptr<NetActivate> &active,
                             const sptr<NetSupplier> &supplier, const sptr<INetConnCallback> &callback);
    void CreateDefaultRequest();
    int32_t RegUnRegNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback, bool isReg);
    int32_t GenerateNetId();
    int32_t GenerateInternalNetId();
    bool FindSameCallback(const sptr<INetConnCallback> &callback, uint32_t &reqId);
    bool FindSameCallback(const sptr<INetConnCallback> &callback, uint32_t &reqId,
                          RegisterType &registerType, uint32_t &uid);
    void GetDumpMessage(std::string &message);
    sptr<NetSupplier> FindNetSupplier(uint32_t supplierId);
    std::shared_ptr<Network> FindNetwork(int32_t netId);
    int32_t RegisterNetSupplierAsync(NetBearType bearerType, const std::string &ident, const std::set<NetCap> &netCaps,
                                     uint32_t &supplierId, int32_t callingUid);
    int32_t UnregisterNetSupplierAsync(uint32_t supplierId, bool ignoreUid, int32_t callingUid);
    int32_t RegisterNetSupplierCallbackAsync(uint32_t supplierId, const sptr<INetSupplierCallback> &callback);
    int32_t RegisterNetConnCallbackAsync(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> &callback,
                                         const uint32_t &timeoutMS, const uint32_t callingUid);
    int32_t RequestNetConnectionAsync(const sptr<NetSpecifier> &netSpecifier, const sptr<INetConnCallback> &callback,
                                         const uint32_t &timeoutMS, const uint32_t callingUid);
    int32_t UpdateNetCapsAsync(const std::set<NetCap> &netCaps, const uint32_t supplierId);
    int32_t UnregisterNetConnCallbackAsync(const sptr<INetConnCallback> &callback, const uint32_t callingUid);
    int32_t RegUnRegNetDetectionCallbackAsync(int32_t netId, const sptr<INetDetectionCallback> &callback, bool isReg);
    int32_t UpdateNetStateForTestAsync(const sptr<NetSpecifier> &netSpecifier, int32_t netState);
    int32_t UpdateNetSupplierInfoAsync(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo,
                                       int32_t callingUid);
    void UpdateNetSupplierInfoAsyncExpand(sptr<NetSupplier> &supplier,
                                            const HttpProxy &oldHttpProxy);
    int32_t UpdateNetLinkInfoAsync(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo, int32_t callingUid);
    int32_t NetDetectionAsync(int32_t netId);
    int32_t RestrictBackgroundChangedAsync(bool restrictBackground);
    int32_t UpdateSupplierScoreAsync(uint32_t supplierId, uint32_t detectionStatus);
    int32_t GetDefaultSupplierIdAsync(NetBearType bearerType, const std::string &ident,
        uint32_t& supplierId);
    void SendHttpProxyChangeBroadcast(const HttpProxy &httpProxy);
    void RequestAllNetworkExceptDefault();
    void LoadGlobalHttpProxy(UserIdType userIdType, HttpProxy &httpProxy);
    void UpdateGlobalHttpProxy(const HttpProxy &httpProxy);
    void ProcessHttpProxyCancel(const sptr<NetSupplier> &supplier);
    void ActiveHttpProxy();
    void CreateActiveHttpProxyThread();
    bool IsRefreshRateLimited(const HttpProxy &currentProxy);
    int32_t LoadCurrentProxyForRefresh(HttpProxy &currentProxy);
    int32_t PrepareRefreshGlobalHttpProxy(const HttpProxy &currentProxy,
        const sptr<IRefreshHttpProxyCallback> &callback);
    void ExecuteRefreshInFfrt(const HttpProxy &currentProxy);
    long PerformProxyCurlProbe(CURL *curl);
    void NotifyRefreshGlobalHttpProxyResult(long responseCode);
    void WaitForNextActiveCycle(uint32_t &retryTimes, long responseCode);
    void DecreaseNetConnCallbackCntForUid(const uint32_t callingUid,
        const RegisterType registerType = REGISTER);
    int32_t IncreaseNetConnCallbackCntForUid(const uint32_t callingUid,
        const RegisterType registerType = REGISTER);

    void RecoverNetSys();
    void OnNetSysRestart();

    bool IsSupplierMatchRequestAndNetwork(sptr<NetSupplier> ns);
    std::tuple<std::vector<std::string>, std::vector<std::string>> GetPreferredRegex();
    bool IsValidDecValue(const std::string &inputValue);
    int32_t GetDelayNotifyTime();
    int32_t NetDetectionForDnsHealthSync(int32_t netId, bool dnsHealthSuccess);
    std::vector<sptr<NetSupplier>> FindSupplierWithInternetByBearerType(
        NetBearType bearerType, const std::string &ident);
    uint32_t FindSupplierForConnected(std::vector<sptr<NetSupplier>> &suppliers);
    int32_t GetLocalUserId(int32_t &userId);
    int32_t GetActiveUserId(int32_t &userId);
    bool IsValidUserId(int32_t userId);
    int32_t GetValidUserIdFromProxy(const HttpProxy &httpProxy);
    inline bool IsPrimaryUserId(const int32_t userId)
    {
        return userId == PRIMARY_USER_ID;
    }
    int32_t EnableVnicNetworkAsync(const sptr<NetLinkInfo> &netLinkInfo, const std::set<int32_t> &uids);
    int32_t DisableVnicNetworkAsync();
    int32_t EnableDistributedClientNetAsync(const std::string &virnicAddr,
        const std::string &virnicName, const std::string &iif);
    int32_t EnableDistributedServerNetAsync(const std::string &iif, const std::string &devIface,
                                            const std::string &dstAddr, const std::string &gw);
    int32_t DisableDistributedNetAsync(bool isServer, const std::string &virnicName, const std::string &dstAddr);
    int32_t CloseSocketsUidAsync(int32_t netId, uint32_t uid);
    int32_t SetAppIsFrozenedAsync(uint32_t uid, bool isFrozened);
    void HandleUnfrozenCallback(uint32_t uid, std::shared_ptr<NetActivate> &curNetAct);
    void HandleSkipRequest(uint32_t uid, std::shared_ptr<NetActivate> &curNetAct);
    int32_t EnableAppFrozenedCallbackLimitationAsync(bool flag);
    void HandleCallback(sptr<NetSupplier> &supplier, sptr<NetHandle> &netHandle,
                        sptr<INetConnCallback> callback, CallbackType type);
    std::shared_ptr<NetActivate> CreateNetActivateRequest(const sptr<NetSpecifier> &netSpecifier,
                            const sptr<INetConnCallback> &callback,
                            const uint32_t &timeoutMS, const int32_t registerType,
                            const uint32_t callingUid);

    // for NET_CAPABILITY_INTERNAL_DEFAULT
    bool IsInRequestNetUids(int32_t uid);
    void NotifyNetBearerTypeChange();
    void ReportFaultEventIfUidNotMatch(sptr<NetSupplier> &supplier, int32_t callingUid);
#ifdef SUPPORT_SYSVPN
    int32_t realCallingUid_ = -1;
    bool IsCallingUserSupplier(uint32_t supplierId);
#endif // SUPPORT_SYSVPN
#ifdef FEATURE_SUPPORT_POWERMANAGER
    void StopAllNetDetection();
    void StartAllNetDetection();
#endif
    void DecreaseNetActivates(const uint32_t callingUid, const sptr<INetConnCallback> &callback);
    bool CheckNetCapPermission(const std::set<NetCap> &netCaps);
    sptr<NetSupplier> GetSupplierByNetId(int32_t netId);
    void RegisterNetDataShareObserver();
    void UnregisterNetDataShareObserver();
    NetBearType GetDefaultNetSupplierType();
    uint32_t GetDefaultNetSupplierId();
    void RegisterAppStateAware();
    bool IsInPreferredList(const std::string& hostName, const std::vector<std::string> &Regexlist);

private:
    enum ServiceRunningState {
        STATE_STOPPED = 0,
        STATE_RUNNING,
    };

    bool registerToService_;
    ServiceRunningState state_;
    sptr<NetSpecifier> defaultNetSpecifier_ = nullptr;
    std::shared_ptr<NetActivate> defaultNetActivate_ = nullptr;
    sptr<NetSupplier> defaultNetSupplier_ = nullptr;
    ffrt::shared_mutex netSuppliersMutex_;
    NET_SUPPLIER_MAP netSuppliers_;
    NET_UIDREQUEST_MAP netUidRequest_;
    NET_UIDREQUEST_MAP internalDefaultUidRequest_;
    NET_UIDACTIVATE_MAP netUidActivates_;
    std::shared_mutex uidActivateMutex_;
    std::atomic<bool> vnicCreated = false;
    sptr<NetConnServiceIface> serviceIface_ = nullptr;
    std::atomic<int32_t> netIdLastValue_ = MIN_NET_ID - 1;
    std::atomic<int32_t> internalNetIdLastValue_ = MIN_INTERNAL_NET_ID;
    std::atomic<bool> isDataShareReady_ = false;
    SafeMap<int32_t, HttpProxy> globalHttpProxyCache_;
    std::mutex netUidRequestMutex_;
    std::shared_ptr<AppExecFwk::EventRunner> netConnEventRunner_ = nullptr;
    std::shared_ptr<NetConnEventHandler> netConnEventHandler_ = nullptr;
    sptr<NetInterfaceStateCallback> interfaceStateCallback_ = nullptr;
    sptr<NetDnsResultCallback> dnsResultCallback_ = nullptr;
    sptr<NetFactoryResetCallback> netFactoryResetCallback_ = nullptr;
    sptr<NetPolicyCallback> policyCallback_ = nullptr;
    std::atomic_bool httpProxyThreadNeedRun_ = false;
    std::condition_variable httpProxyThreadCv_;
    std::mutex httpProxyThreadMutex_;
    static constexpr uint32_t HTTP_PROXY_ACTIVE_PERIOD_S = 120;
    static constexpr uint32_t HTTP_PROXY_ACTIVE_PERIOD_IN_SLEEP_S = 240;
    std::mutex refreshProxyMutex_;
    bool refreshInProgress_ = false;
    bool refreshAuthSuccess_ = false;
    bool refreshResultReady_ = false;
    std::condition_variable refreshResultCv_;
    std::vector<sptr<IRefreshHttpProxyCallback>> refreshCallbacks_;
    std::chrono::steady_clock::time_point lastRefreshTime_;
    HttpProxy lastRefreshProxy_;
    static constexpr uint32_t REFRESH_RATE_LIMIT_S = 10;
    static constexpr uint32_t REFRESH_WAIT_TIMEOUT_S = 15;
    std::map<int32_t, sptr<IPreAirplaneCallback>> preAirplaneCallbacks_;
    std::mutex preAirplaneCbsMutex_;
    std::mutex dataShareMutex_;
    ProbeUrls probeUrl_;
    std::function<void()> onChange_;

    std::atomic<bool> isObserverRegistered_ = false;
    bool hasSARemoved_ = false;
    std::atomic<bool> isInSleep_ = false;
    static constexpr int32_t INVALID_USER_ID = -1;
    static constexpr int32_t ROOT_USER_ID = 0;
    bool isFallbackProbeWithProxy_ = false;
    AppStateAwareCallback appStateAwareCallback_;
    std::atomic<bool> enableAppFrozenedCallbackLimitation_ = false;
    std::recursive_mutex delayFindBestNetMutex_;
    std::atomic<bool> isDelayHandleFindBestNetwork_ = false;
    uint32_t delaySupplierId_ = 0;
#ifdef NETMANAGER_ENABLE_PAC_PROXY
    std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> netPACManager_;
    std::mutex netPacManagerMutex_;
    std::shared_ptr<OHOS::NetManagerStandard::ProxyServer> netPACProxyServer_;
    std::mutex netPacProxyServerMutex_;
#endif
    std::shared_mutex uidLostDelayMutex_;
    std::set<uint32_t> uidLostDelaySet_;
    ffrt::shared_mutex deadFlowResetVecMutex_;
    std::vector<std::string> deadFlowResetBundleNameVec_;
    SafeMap<int32_t, bool> notifyLostDelayCache_;
    ffrt::shared_mutex defaultNetSupplierMutex_;

private:
    class ConnCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ConnCallbackDeathRecipient(NetConnService &client) : client_(client) {}
        ~ConnCallbackDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }

    private:
        NetConnService &client_;
    };
    class NetSupplierCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit NetSupplierCallbackDeathRecipient(NetConnService &client) : client_(client) {}
        ~NetSupplierCallbackDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnNetSupplierRemoteDied(remote);
        }
 
    private:
        NetConnService &client_;
    };
 
    void CheckProxyStatus();
    void OnRemoteDied(const wptr<IRemoteObject> &remoteObject);
    void OnNetSupplierRemoteDied(const wptr<IRemoteObject> &remoteObject);
    void AddClientDeathRecipient(const sptr<INetConnCallback> &callback);
    void AddNetSupplierDeathRecipient(const sptr<INetSupplierCallback> &callback);
    void RemoveNetSupplierDeathRecipient(const sptr<INetSupplierCallback> &callback);
    void RemoveClientDeathRecipient(const sptr<INetConnCallback> &callback);
    void RemoveALLClientDeathRecipient();
    void OnReceiveEvent(const EventFwk::CommonEventData &data);
    void SubscribeCommonEvent();
    void HandlePowerMgrEvent(int code);
    void HandleScreenEvent(bool isScreenOn);
    void HandleFindBestNetworkForDelay();
    void HandlePreFindBestNetworkForDelay(uint32_t supplierId, const sptr<NetSupplier> &supplier,
        bool isFirstTimeDetect);
    void RemoveDelayNetwork();
    void UpdateNetSupplierInfoAsyncInvalid(uint32_t supplierId);
    bool CheckNotifyLostDelay(const std::shared_ptr<NetActivate> &active, int32_t netId, CallbackType type);
    void HandleNotifyLostDelay(int32_t netId);
    bool FindNotifyLostUid(uint32_t uid);
    void StopNotifyLostDelay(int32_t netId);
    void StartNotifyLostDelay(int32_t netId);
    bool FindNotifyLostDelayCache(int32_t netId);
    void HandleSupplierNotAvailable(uint32_t supplierId, bool isOldAvailable, sptr<NetSupplier> &supplier);
    void CancelRequestForSupplier(std::shared_ptr<NetActivate> &netActivate, uint32_t reqId);
    std::mutex remoteMutex_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> netSuplierDeathRecipient_ = nullptr;
    std::vector<sptr<INetConnCallback>> remoteCallback_;
    NetBearType prevNetSupplierType_ = BEARER_DEFAULT;
    bool CheckIfSettingsDataReady();
    std::mutex dataShareMutexWait;
    std::condition_variable dataShareWait;
    std::shared_ptr<NetConnListener> subscriberPtr_ = nullptr;
    bool isScreenOn_ = true;
    int32_t dualStackProbeTime_ = 0;
    std::function<bool(const NetRequest &)> controlFunc_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CONN_SERVICE_H

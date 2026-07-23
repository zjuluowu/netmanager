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

#ifndef NET_STATS_CLIENT_H
#define NET_STATS_CLIENT_H

#include <string>
#include <shared_mutex>

#include "parcel.h"
#include "singleton.h"

#include "inet_stats_service.h"
#include "net_push_stats_info.h"
#include "net_stats_constants.h"
#include "net_stats_info.h"
#include "net_stats_info_sequence.h"
#include "net_stats_network.h"
#include "network_sharing.h"
#include "traffic_plan_param.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsClient : public std::enable_shared_from_this<NetStatsClient> {
public:
    NetStatsClient();
    ~NetStatsClient();
    static NetStatsClient& GetInstance()
    {
        static std::shared_ptr<NetStatsClient> instance = std::make_shared<NetStatsClient>();
        return *instance;
    }

    /**
     * Register network card traffic monitoring
     *
     * @param callback callback function
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback);

    /**
     * Unregister network card traffic monitoring
     *
     * @param callback callback function
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback);

    /**
     * Get the received traffic of the network card
     *
     * @param stats Traffic (bytes)
     * @param interfaceName network card name
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetIfaceRxBytes(uint64_t &stats, const std::string &interfaceName);

    /**
     * Get the send traffic of the network card
     *
     * @param stats Traffic (bytes)
     * @param interfaceName network card name
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetIfaceTxBytes(uint64_t &stats, const std::string &interfaceName);

    /**
     * Get received traffic from the cell
     *
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetCellularRxBytes(uint64_t &stats);

    /**
     * Get send traffic from the cell
     *
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetCellularTxBytes(uint64_t &stats);

    /**
     * Get all received traffic
     *
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAllRxBytes(uint64_t &stats);

    /**
     * Get all send traffic
     *
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAllTxBytes(uint64_t &stats);

    /**
     * Get the received traffic for the specified UID of application
     *
     * @param stats Traffic (bytes)
     * @param uid The specified UID of application.
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetUidRxBytes(uint64_t &stats, uint32_t uid);

    /**
     * Get the send traffic for the specified UID of application
     *
     * @param stats Traffic (bytes)
     * @param uid The specified UID of application.
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetUidTxBytes(uint64_t &stats, uint32_t uid);

    /**
     * Get traffic details for all network cards
     *
     * @param infos all network cards informations
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAllStatsInfo(std::vector<NetStatsInfo> &infos);

    /**
     * Get traffic details for all network cards with container application
     *
     * @param infos all network cards informations
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetAllContainerStatsInfo(std::vector<NetStatsInfo> &infos);

    /**
     * Get traffic of all application with the specified network cards
     *
     * @param infos traffic of all application
     * @param network the network of traffic stats
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetTrafficStatsByNetwork(std::unordered_map<uint32_t, NetStatsInfo> &infos,
                                     const sptr<NetStatsNetwork> &network);

    /**
     * Get traffic of the specified application with the specified network cards
     *
     * @param infos traffic of all application
     * @param uid the id of the specified application
     * @param network the network of traffic stats
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetTrafficStatsByUidNetwork(std::vector<NetStatsInfoSequence> &infos, uint32_t uid,
                                        const sptr<NetStatsNetwork> &network);

    /**
     * Get this month traffic data of the cellular network
     *
     * @param simId the id of the specified sim card
     * @param monthData the network of traffic stats
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.GET_NETWORK_STATS
     * @systemapi Hide this for inner system use.
     */
    int32_t GetMonthTrafficStatsByNetwork(uint32_t simId, uint64_t &monthData);

    /**
     * Set traffic stats of the specified application
     *
     * @param info traffic of the application
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetAppStats(const PushStatsInfo &info);

    /**
     * set sharing traffic before hotspot stop
     *
     * @param info traffic of the application
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetDpaAppStats(const NetStatsInfo &info);

    /**
     * set sharing traffic before hotspot stop
     *
     * @param info traffic of the cellular
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SaveSharingTraffic(const NetStatsInfo &infos);

    /**
     * Get the historical traffic details of the specified network card
     *
     * @param iface network cards name
     * @param start start time
     * @param end end time
     * @param statsInfo traffic information
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end, NetStatsInfo &statsInfo);

    /**
     * Get the historical traffic details from UID of application.
     *
     * @param iface network cards name
     * @param uid The specified UID of application.
     * @param start start time
     * @param end end time
     * @param statsInfo traffic information
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetUidStatsDetail(const std::string &iface, uint32_t uid, uint64_t start, uint64_t end,
                              NetStatsInfo &statsInfo);

    /**
     * Update the traffic of the specified network card
     *
     * @param iface network cards name
     * @param start start time
     * @param end end time
     * @param stats Traffic (bytes)
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UpdateIfacesStats(const std::string &iface, uint64_t start, uint64_t end, const NetStatsInfo &stats);

    /**
     * Update network card traffic data
     *
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t UpdateStatsData();

    /**
     * Clear network card traffic
     *
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t ResetFactory();

    /**
     * Get Sockfd RxBytes
     *
     * @param stats stats
     * @param sockfd sockfd
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetSockfdRxBytes(uint64_t &stats, int32_t sockfd);

    /**
     * Get Sockfd TxBytes
     *
     * @param stats stats
     * @param sockfd sockfd
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetSockfdTxBytes(uint64_t &stats, int32_t sockfd);

    /**
     * Set Calibration Traffic Data
     *
     * @param simId simId
     * @param remainingData remainingData
     * @param remaintotalMonthlyDataingData totalMonthlyData
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.GET_NETWORK_STATS
     * @systemapi Hide this for inner system use.
     */
    int32_t SetCalibrationTraffic(uint32_t simId, int64_t remainingData, uint64_t totalMonthlyData);
    int32_t SetTrafficPlanInfo(int32_t simId, int32_t param, int64_t value);
    int32_t GetTrafficPlanInfo(int32_t simId, int32_t param, int64_t &value);

private:
    class NetStatsDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit NetStatsDeathRecipient(NetStatsClient &client) : client_(client) {}
        ~NetStatsDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }

    private:
        NetStatsClient &client_;
    };

private:
    sptr<INetStatsService> GetProxy();
    void RecoverCallback();
    void OnRemoteDied(const wptr<IRemoteObject> &remote);

private:
    std::mutex mutex_;
    sptr<INetStatsService> netStatsService_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_;
    std::shared_mutex callbackMutex_;
    sptr<INetStatsCallback> callback_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_CLIENT_H

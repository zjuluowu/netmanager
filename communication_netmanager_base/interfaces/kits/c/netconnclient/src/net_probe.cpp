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

#include <unistd.h>

#include <cstdint>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctime>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>
#include <functional>
#include <netinet/ip_icmp.h>
#include <poll.h>

#include "netmanager_base_common_utils.h"
#include "net_manager_constants.h"
#include "net_probe.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {

constexpr int32_t PING_DATA_SIZE = 64;
constexpr int32_t MAX_PING_DURATION = 1000;

constexpr uint8_t ICMPV4_ECHO_REQUEST = 8u;
constexpr uint8_t ICMPV6_ECHO_REQUEST = 128u;

constexpr int32_t MSEC_PER_SECOND = 1000;
constexpr int32_t NSEC_PER_MSEC = 1000000;

constexpr int32_t PERCENTAGE = 100u;

constexpr uint16_t PING_CHECKSUM_MASK = 255;

constexpr uint32_t SQUARE = 2;

#if __BYTE_ORDER == __BIG_ENDIAN
#define IS_BIG_ENDIAN 1
#else
#define IS_BIG_ENDIAN 0
#endif

static uint16_t PingChecksum(uint16_t *data, int32_t len)
{
    uint16_t u = 0;
    uint16_t d;

    while (len > 0) {
        d = *data++;

        if (len == 1) {
            d &= PING_CHECKSUM_MASK << IS_BIG_ENDIAN;
        }

        u += d;
        if (d >= u) {
            u++;
        }

        len -= sizeof(uint16_t);
    }

    return u;
}

static int64_t Now(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * MSEC_PER_SECOND + ts.tv_nsec / NSEC_PER_MSEC;
}

static int32_t SendRequest(int32_t fd, iovec &iov, struct addrinfo *ai, int16_t seq, int64_t time)
{
    struct icmphdr *ih = reinterpret_cast<struct icmphdr*>(iov.iov_base);
    int64_t *timePtr = nullptr;

    ih->code = 0;
    ih->type = (ai->ai_family == AF_INET) ? ICMPV4_ECHO_REQUEST : ICMPV6_ECHO_REQUEST;
    ih->un.echo.sequence = seq;
    ih->un.echo.id = getpid();

    timePtr = reinterpret_cast<int64_t *>(ih + 1);
    *timePtr = time;

    ih->checksum = PingChecksum(reinterpret_cast<uint16_t *>(iov.iov_base),
                                static_cast<int32_t>(iov.iov_len));
    return sendto(fd, iov.iov_base, iov.iov_len, 0, ai->ai_addr, ai->ai_addrlen);
}

static int32_t WaitResponse(int32_t fd, iovec &iov, int64_t &respTime, int32_t timeout)
{
    int32_t rc;

    if (timeout >= 0) {
        struct pollfd pFd;

        pFd.fd = fd;
        pFd.events = POLLIN;
        if (poll(&pFd, 1, timeout) <= 0) {
            return 0;
        }
    }

    rc = recv(fd, iov.iov_base, iov.iov_len, 0);
    if (rc >= static_cast<int32_t>(sizeof(struct icmphdr) + sizeof(int64_t))) {
        struct icmphdr *ih = reinterpret_cast<struct icmphdr*>(iov.iov_base);
        int64_t *timePtr = reinterpret_cast<int64_t *>(ih + 1);

        respTime = *timePtr;
    }

    return rc;
}

/*
 * When error like no routes or sendto failed occurs during ping operations, function will return success
 * but with 100% packet loss.
 */
constexpr uint8_t LOSS_RATE_ALL = 100u;
static int32_t ResetToFullLoss(NetConn_ProbeResultInfo &result, uint32_t duration)
{
    result.lossRate = LOSS_RATE_ALL;
    result.rtt[NETCONN_RTT_MIN] = static_cast<uint32_t>(duration * MSEC_PER_SECOND);
    result.rtt[NETCONN_RTT_MAX] = static_cast<uint32_t>(duration * MSEC_PER_SECOND);
    result.rtt[NETCONN_RTT_AVG] = static_cast<uint32_t>(duration * MSEC_PER_SECOND);
    result.rtt[NETCONN_RTT_STD] = 0u;

    return 0;
}

static uint32_t CalcPingRttStd(std::vector<int64_t> &timesTake, uint32_t totalRecv, uint32_t rttAvg)
{
    int64_t sumDelay = 0;

    for (uint32_t i = 0u; i < totalRecv; ++i) {
        sumDelay += pow(rttAvg - timesTake[i], SQUARE);
    }

    return (totalRecv > 0) ? static_cast<uint32_t>(sqrt(static_cast<uint32_t>(sumDelay) / totalRecv)) : 0u;
}

const int64_t SEND_INTERVAL = 1000; /* ms */
const int64_t WAIT_INTERVAL = 1000; /* ms */

static int32_t DoPing(int32_t s, struct addrinfo *ai, uint32_t duration, NetConn_ProbeResultInfo &result)
{
    uint8_t buffer[sizeof(struct icmphdr) + PING_DATA_SIZE] = {0};    /* icmp header and data */
    iovec iov = {reinterpret_cast<void *>(buffer), sizeof(buffer)};
    std::vector<int64_t> timesTake(duration);
    uint16_t seq = 0;
    int64_t timeNextSend = 0; /* ms, initial to 0, send request immediately */
    int64_t timeWait; /* ms */
    uint32_t totalSend = 0;
    uint32_t totalRecv = 0;
    int64_t sumDelay = 0;
    int64_t respTime = 0;
    int32_t rc = 0;

    while (totalSend < duration) {
        int64_t timeNow = Now();
        if (timeNextSend < timeNow) {
            rc = SendRequest(s, iov, ai, seq++, timeNow);
            if (rc < 0) {
                NETMGR_LOG_I("sendRequest rc < 0, errno = %{public}d", errno);
                break;
            }

            totalSend += 1;

            timeNextSend = timeNow + SEND_INTERVAL;
            timeWait = WAIT_INTERVAL;
        } else {
            timeWait = timeNextSend - timeNow;
            if (timeWait <= 0) {
                timeWait = 1;
            }
        }

        rc = WaitResponse(s, iov, respTime, static_cast<int>(timeWait));
        if (rc <= 0) {
            continue;
        }

        int64_t currentDelay = Now() - respTime;
        timesTake[totalRecv++] = currentDelay;
        sumDelay += currentDelay;

        result.rtt[NETCONN_RTT_MAX] = std::max(result.rtt[NETCONN_RTT_MAX], static_cast<uint32_t>(currentDelay));
        result.rtt[NETCONN_RTT_MIN] = std::min(result.rtt[NETCONN_RTT_MIN], static_cast<uint32_t>(currentDelay));
    }

    if (totalRecv == 0) {
        NETMGR_LOG_I("totalRecv = 0, errno = %{public}d", errno);
        return ResetToFullLoss(result, duration);
    }

    result.rtt[NETCONN_RTT_AVG] = (totalRecv > 0) ? (static_cast<uint32_t>(sumDelay) / totalRecv) : 0u;
    result.rtt[NETCONN_RTT_STD] = CalcPingRttStd(timesTake, totalRecv, result.rtt[NETCONN_RTT_AVG]);
    result.lossRate = static_cast<uint8_t>(
        (totalSend > 0 && totalSend >= totalRecv) ? ((totalSend - totalRecv) * PERCENTAGE / totalSend) : 0u);

    return rc;
}

static void InitialProbeResult(NetConn_ProbeResultInfo &result)
{
    result.rtt[NETCONN_RTT_MIN] = UINT_MAX;
    result.rtt[NETCONN_RTT_MAX] = 0u;
    result.rtt[NETCONN_RTT_AVG] = 0u;
    result.rtt[NETCONN_RTT_STD] = 0u;
    result.lossRate = 0u;
}

int32_t NetProbe::QueryProbeResult(std::string &dest, int32_t duration, NetConn_ProbeResultInfo &result)
{
    struct addrinfo info = {0};
    struct addrinfo *ai = nullptr;
    int32_t family = AF_UNSPEC;
    int32_t rc;

    if ((duration <= 0) || (duration > MAX_PING_DURATION)) {
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }

    info.ai_family = family;
    rc = getaddrinfo(dest.c_str(), nullptr, &info, &ai);
    if (rc < 0 || ai == nullptr) {
        NETMGR_LOG_I("rc < 0 || ai == nullptr, errno = %{public}d", errno);
        (void)ResetToFullLoss(result, duration);
        return NETMANAGER_SUCCESS;
    }

    int32_t fd = socket(ai->ai_family, SOCK_DGRAM, (ai->ai_family == AF_INET) ? IPPROTO_ICMP : IPPROTO_ICMPV6);
    if (fd >= 0) {
        InitialProbeResult(result);

        rc = DoPing(fd, ai, static_cast<uint32_t>(duration), result);
        close(fd);
    } else {
        (void)ResetToFullLoss(result, duration);
    }

    freeaddrinfo(ai);

    return NETMANAGER_SUCCESS;
}

}
}

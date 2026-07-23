/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "dns_proxy_request_socket.h"
#include "netnative_log_wrapper.h"

namespace OHOS::nmd {
DnsProxyRequestSocket::DnsProxyRequestSocket(int32_t sock, std::unique_ptr<AlignedSockAddr> &&clientSock,
                                             std::unique_ptr<RecvBuff> &&recvBuff)
{
    NETNATIVE_LOG_D("dns_proxy_listen DnsProxyRequestSocket");
    this->sock = sock;
    event.data.fd = sock;
    event.events = EPOLLIN;
    this->clientSock = std::move(clientSock);
    this->recvBuff = std::move(recvBuff);
    endTime = std::chrono::system_clock::now() + std::chrono::milliseconds(EPOLL_TIMEOUT);
}

DnsProxyRequestSocket::~DnsProxyRequestSocket()
{
    NETNATIVE_LOG_D("dns_proxy_listen ~DnsProxyRequestSocket sock: %{public}d", sock);
    if (sock > 0) {
        close(sock);
    }
}

int32_t DnsProxyRequestSocket::GetSock() const
{
    return sock;
}

size_t DnsProxyRequestSocket::GetIdx() const
{
    return dnsServerIdx;
}

void DnsProxyRequestSocket::ResetIdx()
{
    dnsServerIdx = 0;
}

void DnsProxyRequestSocket::IncreaseIdx()
{
    dnsServerIdx++;
}

epoll_event *DnsProxyRequestSocket::GetEventPtr()
{
    return &event;
}

AlignedSockAddr &DnsProxyRequestSocket::GetAddr()
{
    return this->addrParse;
}
AlignedSockAddr &DnsProxyRequestSocket::GetClientSock()
{
    return *clientSock;
}
RecvBuff &DnsProxyRequestSocket::GetRecvBuff()
{
    return *recvBuff;
}

std::list<std::map<int32_t, DnsProxyRequestSocket>::iterator>::iterator DnsProxyRequestSocket::GetLruIterator() const
{
    return this->lruIterator;
}

void DnsProxyRequestSocket::SetLruIterator(
    std::list<std::map<int32_t, DnsProxyRequestSocket>::iterator>::iterator iterator)
{
    this->lruIterator = iterator;
}
} // namespace OHOS::nmd

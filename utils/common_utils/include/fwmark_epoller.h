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

#ifndef NETMANAGER_BASE_EPOLLER_RECVMSG_H
#define NETMANAGER_BASE_EPOLLER_RECVMSG_H

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

#include "securec.h"

namespace OHOS::NetManagerStandard::FwmarkTool {
typedef int FileDescriptor;
using RecvMsgRunner = std::function<void(FileDescriptor fd)>;
static constexpr size_t MAX_EPOLL_EVENTS = 32;

bool MakeNonBlock(int sock)
{
    static constexpr uint32_t maxRetry = 30;
    uint32_t retry = 0;
    int flags = fcntl(sock, F_GETFL, 0);
    while (flags == -1 && errno == EINTR && retry < maxRetry) {
        flags = fcntl(sock, F_GETFL, 0);
        ++retry;
    }

    if (flags == -1) {
        return false;
    }

    retry = 0;
    uint32_t tempFlags = static_cast<uint32_t>(flags) | O_NONBLOCK;
    int ret = fcntl(sock, F_SETFL, tempFlags);
    while (ret == -1 && errno == EINTR && retry < maxRetry) {
        ret = fcntl(sock, F_SETFL, tempFlags);
        ++retry;
    }
    if (ret == -1) {
        return false;
    }
    return true;
}

struct Epoller {
    Epoller()
    {
        underlying_ = epoll_create1(EPOLL_CLOEXEC);
    }

    ~Epoller()
    {
        close(underlying_);
    }

    Epoller(const Epoller &) = delete;
    Epoller(Epoller &&) = delete;
    Epoller &operator=(const Epoller &) = delete;
    Epoller &operator=(const Epoller &&) = delete;

    void RegisterMe(FileDescriptor descriptor) const
    {
        RegisterMe(descriptor, EPOLLIN);
    }

    void RegisterMe(FileDescriptor descriptor, uint32_t flags) const
    {
        epoll_event ev{};
        ev.events = flags;
        ev.data.fd = descriptor;
        epoll_ctl(underlying_, EPOLL_CTL_ADD, descriptor, &ev);
    }

    void UnregisterMe(FileDescriptor descriptor) const
    {
        epoll_ctl(underlying_, EPOLL_CTL_DEL, descriptor, nullptr);
    }

    int Wait(epoll_event *events, int maxEvents, int timeout) const
    {
        return epoll_wait(underlying_, events, maxEvents, timeout);
    }

private:
    FileDescriptor underlying_ = 0;
};

class FwmarkEpollServer {
public:
    FwmarkEpollServer(FileDescriptor serverFd, RecvMsgRunner runner) : serverFd_(serverFd), runner_(std::move(runner))
    {
        epoller_ = std::make_shared<Epoller>();
        epoller_->RegisterMe(serverFd);
    }

    void Run()
    {
        while (true) {
            static constexpr int waitTimeoutMs = 5000;
            if (!epoller_) {
                return;
            }

            epoll_event events[MAX_EPOLL_EVENTS]{};
            int eventsToHandle = epoller_->Wait(events, MAX_EPOLL_EVENTS, receivers_.empty() ? -1 : waitTimeoutMs);
            if (eventsToHandle == -1) {
                continue;
            }
            if (eventsToHandle == 0) {
                for (const auto fd : receivers_) {
                    epoller_->UnregisterMe(fd);
                    close(fd);
                }
                receivers_.clear();
                continue;
            }
            RunForReceivers(events, eventsToHandle);
        }
    }

private:
    void RunForReceivers(epoll_event events[MAX_EPOLL_EVENTS], int eventsToHandle)
    {
        for (int idx = 0; idx < eventsToHandle; ++idx) {
            if (serverFd_ == events[idx].data.fd) {
                sockaddr_un clientAddr{};
                socklen_t len = sizeof(clientAddr);
                auto clientFd = accept(serverFd_, reinterpret_cast<sockaddr *>(&clientAddr), &len);
                if (!MakeNonBlock(clientFd)) {
                    close(clientFd);
                    continue;
                }
                if (clientFd > 0) {
                    epoller_->RegisterMe(clientFd);
                    receivers_.insert(clientFd);
                }
            } else if (receivers_.count(events[idx].data.fd) > 0) {
                epoller_->UnregisterMe(events[idx].data.fd);
                receivers_.erase(events[idx].data.fd);
                if (runner_) {
                    runner_(events[idx].data.fd);
                } else {
                    close(events[idx].data.fd);
                }
            } else {
                // maybe not my fd, just UnregisterMe
                // this may not happen
                // not in receivers and not serverFd, just unregister
                epoller_->UnregisterMe(events[idx].data.fd);
            }
        }
    }

    std::shared_ptr<Epoller> epoller_;
    FileDescriptor serverFd_ = 0;
    RecvMsgRunner runner_;
    std::unordered_set<FileDescriptor> receivers_;
};
} // namespace OHOS::NetManagerStandard::FwmarkTool
#endif // NETMANAGER_BASE_EPOLLER_RECVMSG_H

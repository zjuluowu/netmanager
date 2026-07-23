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

#ifndef NETMANAGER_BASE_EPOLLER_H
#define NETMANAGER_BASE_EPOLLER_H

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
#include <unordered_map>
#include <utility>

#include "securec.h"

namespace OHOS::NetManagerStandard {
static constexpr size_t MAX_EPOLL_EVENTS = 32;
typedef int FileDescriptor;
enum class FixedLengthReceiverState {
    ONERROR,
    DATA_ENOUGH,
    CONTINUE,
};
using ReceiverRunner = std::function<FixedLengthReceiverState(FileDescriptor fd, const std::string &data)>;

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

class FixedLengthReceiver {
public:
    FixedLengthReceiver() = delete;
    FixedLengthReceiver(FileDescriptor clientFd, size_t neededLength, ReceiverRunner runner)
        : fd_(clientFd), neededLength_(neededLength), runner_(std::move(runner))
    {
    }

    FixedLengthReceiverState Run()
    {
        if (!runner_) {
            return FixedLengthReceiverState::ONERROR;
        }
        auto res = GetData();
        if (res == FixedLengthReceiverState::ONERROR) {
            return res;
        }
        if (res == FixedLengthReceiverState::DATA_ENOUGH) {
            return runner_(fd_, data_);
        }
        return FixedLengthReceiverState::CONTINUE;
    }

private:
    FixedLengthReceiverState GetData()
    {
        if (data_.size() >= neededLength_) {
            return FixedLengthReceiverState::DATA_ENOUGH;
        }
        auto size = neededLength_ - data_.size();
        auto buf = malloc(size);
        if (buf == nullptr) {
            return FixedLengthReceiverState::ONERROR;
        }
        if (memset_s(buf, size, 0, size) != EOK) {
            free(buf);
            return FixedLengthReceiverState::ONERROR;
        }
        auto recvSize = read(fd_, buf, size);
        if (recvSize < 0) {
            if (errno == EINTR) {
                free(buf);
                return FixedLengthReceiverState::CONTINUE;
            }
            free(buf);
            return FixedLengthReceiverState::ONERROR;
        }
        if (recvSize == 0) {
            free(buf);
            return FixedLengthReceiverState::ONERROR;
        }
        data_.append(reinterpret_cast<char *>(buf), recvSize);
        free(buf);
        return data_.size() >= neededLength_ ? FixedLengthReceiverState::DATA_ENOUGH
                                             : FixedLengthReceiverState::CONTINUE;
    }

    FileDescriptor fd_ = 0;
    size_t neededLength_ = 0;
    ReceiverRunner runner_;
    std::string data_;
};

class EpollServer {
public:
    EpollServer(FileDescriptor serverFd, size_t firstPackageSize, ReceiverRunner firstPackageRunner)
        : serverFd_(serverFd), firstPackageSize_(firstPackageSize), firstPackageRunner_(std::move(firstPackageRunner))
    {
        epoller_ = std::make_shared<Epoller>();
        epoller_->RegisterMe(serverFd);
    }

    void AddReceiver(FileDescriptor clientFd, size_t neededLength, const ReceiverRunner &runner)
    {
        auto receiver = std::make_shared<FixedLengthReceiver>(clientFd, neededLength, runner);
        receivers_[clientFd] = receiver;
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
                for (const auto &[fd, receiver] : receivers_) {
                    epoller_->UnregisterMe(fd);
                    close(fd);
                }
                receivers_.clear();
                continue;
            }
            RunForEvents(events, eventsToHandle);
        }
    }

private:
    void RunForFd(int fd)
    {
        auto receiver = receivers_[fd];
        if (receiver) {
            if (receiver->Run() != FixedLengthReceiverState::CONTINUE) {
                receivers_.erase(fd);
                epoller_->UnregisterMe(fd);
                close(fd);
            }
        } else {
            // my fd, UnregisterMe and close
            receivers_.erase(fd);
            epoller_->UnregisterMe(fd);
            close(fd);
        }
    }

    void RunForEvents(epoll_event events[MAX_EPOLL_EVENTS], int eventsToHandle)
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
                    AddReceiver(clientFd, firstPackageSize_, firstPackageRunner_);
                }
            } else if (receivers_.count(events[idx].data.fd) > 0) {
                RunForFd(events[idx].data.fd);
            } else {
                // maybe not my fd, just UnregisterMe
                // this may not happen
                // not in receivers and not serverFd, just unregister
                epoller_->UnregisterMe(events[idx].data.fd);
            }
        }
    }

    std::unordered_map<FileDescriptor, std::shared_ptr<FixedLengthReceiver>> receivers_;
    std::shared_ptr<Epoller> epoller_;
    FileDescriptor serverFd_ = 0;
    size_t firstPackageSize_ = 0;
    ReceiverRunner firstPackageRunner_;
};
} // namespace OHOS::NetManagerStandard
#endif // NETMANAGER_BASE_EPOLLER_H

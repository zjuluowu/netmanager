/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "fwmark_network.h"

#include <cerrno>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>

#include "fwmark.h"
#include "fwmark_command.h"
#include "init_socket.h"
#include "netnative_log_wrapper.h"
#ifdef USE_SELINUX
#include "selinux/selinux.h"
#endif
#include "fwmark_epoller.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
static constexpr const uint16_t NETID_UNSET = 0;
static constexpr const int32_t NO_ERROR_CODE = 0;
static constexpr const int32_t ERROR_CODE_RECVMSG_FAILED = -1;
static constexpr const int32_t ERROR_CODE_SOCKETFD_INVALID = -2;
static constexpr const int32_t ERROR_CODE_WRITE_FAILED = -3;
static constexpr const int32_t ERROR_CODE_GETSOCKOPT_FAILED = -4;
static constexpr const int32_t ERROR_CODE_SETSOCKOPT_FAILED = -5;
static constexpr const int32_t ERROR_CODE_SET_MARK = -6;
static constexpr const int32_t MAX_CONCURRENT_CONNECTION_REQUESTS = 10;
union Cmsgu {
    cmsghdr cmh;
    char cmsg[CMSG_SPACE(sizeof(0))];
};
void CloseSocket(int32_t *socket, int32_t ret, int32_t errorCode)
{
    if (socket == nullptr) {
        NETNATIVE_LOGE("CloseSocket failed, socket is nullptr");
        return;
    }
    switch (errorCode) {
        case ERROR_CODE_RECVMSG_FAILED:
            NETNATIVE_LOGE("recvmsg failed, clientSockfd:%{public}d, ret:%{public}d, errno: %{public}d", *socket, ret,
                           errno);
            break;
        case ERROR_CODE_SOCKETFD_INVALID:
            NETNATIVE_LOGE("socketFd invalid:%{public}d, ret:%{public}d, errno: %{public}d", *socket, ret, errno);
            break;
        case ERROR_CODE_WRITE_FAILED:
            NETNATIVE_LOGE("wirte failed, clientSockfd:%{public}d, ret:%{public}d, errno: %{public}d", *socket, ret,
                           errno);
            break;
        case ERROR_CODE_GETSOCKOPT_FAILED:
            NETNATIVE_LOGE("getsockopt failed, socketFd:%{public}d, ret:%{public}d, errno: %{public}d", *socket, ret,
                           errno);
            break;
        case ERROR_CODE_SETSOCKOPT_FAILED:
            NETNATIVE_LOGE("setsockopt failed socketFd:%{public}d, ret:%{public}d, errno: %{public}d", *socket, ret,
                           errno);
            break;
        case ERROR_CODE_SET_MARK:
            NETNATIVE_LOGE("SetMark failed, clientSockfd:%{public}d, ret:%{public}d, errno: %{public}d", *socket, ret,
                           errno);
            break;
        default:
            NETNATIVE_LOG_D("NO_ERROR_CODE CloseSocket socket:%{public}d, ret:%{public}d", *socket, ret);
            break;
    }
    close(*socket);
    *socket = -1;
}

int32_t SetMark(int32_t *socketFd, FwmarkCommand *command)
{
    if (command == nullptr || socketFd == nullptr) {
        NETNATIVE_LOGE("SetMark failed, command or socketFd is nullptr");
        return -1;
    }
    Fwmark fwmark;
    socklen_t fwmarkLen = sizeof(fwmark.intValue);
    int32_t ret = getsockopt(*socketFd, SOL_SOCKET, SO_MARK, &fwmark.intValue, &fwmarkLen);
    if (ret != 0) {
        CloseSocket(socketFd, ret, ERROR_CODE_GETSOCKOPT_FAILED);
        return ret;
    }
    NETNATIVE_LOG_D("FwmarkNetwork: SetMark netId: %{public}d, socketFd:%{public}d, cmd:%{public}d", command->netId,
        *socketFd, command->cmdId);
    switch (command->cmdId) {
        case FwmarkCommand::SELECT_NETWORK: {
            fwmark.netId = command->netId;
            if (command->netId == NETID_UNSET) {
                fwmark.explicitlySelected = false;
                fwmark.protectedFromVpn = false;
                fwmark.permission = PERMISSION_NONE;
            } else {
                fwmark.explicitlySelected = true;
                fwmark.protectedFromVpn = true;
            }
            break;
        }
        case FwmarkCommand::PROTECT_FROM_VPN: {
            fwmark.protectedFromVpn = true;
            break;
        }
        default:
            break;
    }
    ret = setsockopt(*socketFd, SOL_SOCKET, SO_MARK, &fwmark.intValue, sizeof(fwmark.intValue));
    if (ret != 0) {
        NETNATIVE_LOGE("FwmarkNetwork: SetMark failed, ret %{public}d.", ret);
        CloseSocket(socketFd, ret, ERROR_CODE_SETSOCKOPT_FAILED);
        return ret;
    }
    CloseSocket(socketFd, ret, NO_ERROR_CODE);
    return ret;
}

void RunForClientFd(int32_t clientSockfd)
{
    FwmarkCommand fwmCmd{};
    iovec iov = {.iov_base = &fwmCmd, .iov_len = sizeof(fwmCmd)};
    int32_t socketFd = -1;
    Cmsgu cmsgu;
    if (memset_s(cmsgu.cmsg, sizeof(cmsgu.cmsg), 0, sizeof(cmsgu.cmsg)) != EOK) {
        CloseSocket(&clientSockfd, -1, ERROR_CODE_RECVMSG_FAILED);
        return;
    }
    msghdr message;
    if (memset_s(&message, sizeof(message), 0, sizeof(message)) != EOK) {
        CloseSocket(&clientSockfd, -1, ERROR_CODE_RECVMSG_FAILED);
        return;
    }
    message = {.msg_iov = &iov, .msg_iovlen = 1, .msg_control = cmsgu.cmsg, .msg_controllen = sizeof(cmsgu.cmsg)};
    int32_t ret = recvmsg(clientSockfd, &message, 0);
    if (ret < 0) {
        CloseSocket(&clientSockfd, ret, ERROR_CODE_RECVMSG_FAILED);
        return;
    }
    cmsghdr *const cmsgh = CMSG_FIRSTHDR(&message);
    if (cmsgh && cmsgh->cmsg_level == SOL_SOCKET && cmsgh->cmsg_type == SCM_RIGHTS &&
        cmsgh->cmsg_len == CMSG_LEN(sizeof(socketFd))) {
        if (memcpy_s(&socketFd, sizeof(socketFd), CMSG_DATA(cmsgh), sizeof(socketFd)) != 0) {
            return;
        }
    }
    if (socketFd < 0) {
        CloseSocket(&clientSockfd, ret, ERROR_CODE_SOCKETFD_INVALID);
        return;
    }
    if ((ret = SetMark(&socketFd, &fwmCmd)) != 0) {
        CloseSocket(&clientSockfd, ret, ERROR_CODE_SET_MARK);
        return;
    }
    if ((ret = write(clientSockfd, &ret, sizeof(ret))) < 0) {
        CloseSocket(&clientSockfd, ret, ERROR_CODE_WRITE_FAILED);
        return;
    }
    CloseSocket(&clientSockfd, ret, NO_ERROR_CODE);
}

void StartListener()
{
    int32_t serverSockfd = GetControlSocket("fwmarkd");

    int32_t result = listen(serverSockfd, MAX_CONCURRENT_CONNECTION_REQUESTS);
    if (result < 0) {
        NETNATIVE_LOGE("FwmarkNetwork: listen failed result %{public}d, errno: %{public}d", result, errno);
        close(serverSockfd);
        serverSockfd = -1;
        return;
    }
    if (!FwmarkTool::MakeNonBlock(serverSockfd)) {
        close(serverSockfd);
        serverSockfd = -1;
        return;
    }
    FwmarkTool::FwmarkEpollServer server(serverSockfd, RunForClientFd);
    server.Run();
    close(serverSockfd);
    serverSockfd = -1;
}

FwmarkNetwork::FwmarkNetwork()
{
    ListenerClient();
}

FwmarkNetwork::~FwmarkNetwork() = default;

void FwmarkNetwork::ListenerClient()
{
    std::thread t(StartListener);
    pthread_setname_np(t.native_handle(), "FwmarkListen");
    t.detach();
    NETNATIVE_LOGI("FwmarkNetwork: StartListener");
}
} // namespace nmd
} // namespace OHOS

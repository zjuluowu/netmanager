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
#ifndef NETSYS_CLATD_H
#define NETSYS_CLATD_H

#include <netinet/in.h>
#include <string>

#include "clatd_packet_converter.h"
#include "ffrt.h"

namespace OHOS {
namespace nmd {
using namespace OHOS::NetManagerStandard;
class Clatd {
public:
    Clatd(){};
    Clatd(int tunFd, int readSock6, int writeSock6, const std::string &v6Iface, const std::string &prefixAddrStr,
          const std::string &v4AddrStr, const std::string &v6AddrStr);

    ~Clatd();
    Clatd(const Clatd &clatd) = delete;
    Clatd &operator=(const Clatd &clatd) = delete;

    void Start();

    void Stop();

private:
    void SendDadPacket();
    void RunLoop();
    int32_t MaybeCalculateL4Checksum(int packetLen, ClatdReadV6Buf &readBuf);
    void ProcessV6Packet();
    void ProcessV4Packet();
    int32_t ReadV6Packet(msghdr &msgHdr, ssize_t &readLen);
    int32_t ReadV4Packet(ClatdReadTunBuf &readBuf, ssize_t &readLen);
    void SendV6OnRawSocket(int fd, std::vector<iovec> &iovPackets, int effectivePos);

    int tunFd_;
    int readSock6_;
    int writeSock6_;
    int stopFd_;
    std::string v6Iface_;
    std::string tunIface_;
    in6_addr v6Addr_;
    in_addr v4Addr_;
    in6_addr prefixAddr_;
    bool isSocketClosed_;
    ffrt::mutex mutex_;
    ffrt::condition_variable cv_;
    std::atomic<bool> stopStatus_;
};
} // namespace nmd
} // namespace OHOS

#endif
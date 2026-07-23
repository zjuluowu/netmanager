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

#ifndef BPF_RING_BUFFER_H
#define BPF_RING_BUFFER_H
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <linux/bpf.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <time.h>
#include <linux/if_ether.h>
#include <linux/unistd.h>
#include <string>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "bpf_path.h"
#include "bpf_def.h"
#include "bpf_stats.h"
#include "bpf_mapper.h"
#include "securec.h"

#include "i_netsys_traffic_callback.h"

namespace OHOS::NetManagerStandard {

enum RingBufferError {
    RING_BUFFER_ERR_NONE = 0,
    RING_BUFFER_ERR_INTERNAL,
};

class NetsysBpfRingBuffer {
public:
    NetsysBpfRingBuffer() = default;
    ~NetsysBpfRingBuffer() = default;

    static uint64_t BpfMapPathNameToU64(const std::string &pathName);
    static int32_t BpfSyscall(int32_t cmd, const bpf_attr &attr);
    static int GetRingbufFd(const std::string &path, uint32_t fileFlags);

    static int HandleNetworkPolicyEventCallback(void *ctx, void *data, size_t data_sz);
    static int HandleNetStatsEventCallback(void *ctx, void *data, size_t dataSz);
    static void ListenRingBufferThread(void);
    static void ListenNetworkAccessPolicyEvent();
    static void ListenNetStatsRingBufferThread();
    static void ExistRingBufferPoll(void);
    static void ListenNetworkStatsEvent(void);
    static void ExistNetstatsRingBufferPoll();
    static int RegisterNetsysTrafficCallback(const sptr<NetsysNative::INetsysTrafficCallback> &callback);
    static int UnRegisterNetsysTrafficCallback(const sptr<NetsysNative::INetsysTrafficCallback> &callback);

private:
    static bool existThread_;
    static std::atomic_bool existNetStatsThread_;
    static std::vector<sptr<NetsysNative::INetsysTrafficCallback>> callbacks_;
    static std::mutex callbackMutex_;
};
} // namespace OHOS::NetManagerStandard
#endif // BPF_RING_BUFFER_H

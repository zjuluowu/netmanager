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

#ifndef NETMANAGER_BASE_NET_PROBE_H
#define NETMANAGER_BASE_NET_PROBE_H

#include <string>

#include "net_trace_route_info.h"

namespace OHOS {
namespace NetManagerStandard {

enum {
    NETCONN_RTT_MIN = 0,
    NETCONN_RTT_MAX = 1,
    NETCONN_RTT_AVG = 2,
    NETCONN_RTT_STD = 3
};

class NetProbe {
public:
	int32_t QueryProbeResult(std::string &dest, int32_t duration, NetConn_ProbeResultInfo &result);
};
}
}

#endif

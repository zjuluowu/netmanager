/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "hi_app_event_report.h"
#include <random>
#ifdef ENABLE_EMULATOR
#include "net_mgr_log_wrapper.h"
#include "time_service_client.h"
#endif
#include <mutex>
#include "ffrt.h"

namespace OHOS {
namespace NetManagerStandard {
#ifdef ENABLE_EMULATOR
const int64_t TIMEOUT = 90;
const int64_t ROW = 30;
const int64_t PROCESSOR_ID_NOT_CREATE = -1;
static int64_t g_processorID = PROCESSOR_ID_NOT_CREATE;
static ffrt::mutex g_netAppEventProcessorIdMutex;
#endif

HiAppEventReport::HiAppEventReport(std::string sdk, std::string api)
{
#ifdef ENABLE_EMULATOR
    apiName_ = api;
    sdkName_ = sdk;
    transId_ = std::string("transId_") + std::to_string(std::rand());

    beginTime_ = OHOS::MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
#endif
}

HiAppEventReport::~HiAppEventReport()
{
}

void HiAppEventReport::ReportSdkEvent(const int result, const int errCode)
{
#ifdef ENABLE_EMULATOR
    std::weak_ptr<HiAppEventReport> wp = shared_from_this();
    ffrt::submit([result, errCode, wp]() {
        auto selfShared = wp.lock();
        if (selfShared == nullptr) {
            return;
        }
        std::unique_lock<ffrt::mutex> lock(g_netAppEventProcessorIdMutex);
        if (g_processorID == PROCESSOR_ID_NOT_CREATE) {
            g_processorID = selfShared->AddProcessor();
        }
        lock.unlock();
        int64_t endTime = OHOS::MiscServices::TimeServiceClient::GetInstance()->GetBootTimeMs();
        OHOS::HiviewDFX::HiAppEvent::Event event("api_diagnostic", "api_exec_end",
            OHOS::HiviewDFX::HiAppEvent::BEHAVIOR);
        event.AddParam("trans_id", selfShared->transId_);
        event.AddParam("api_name", selfShared->apiName_);
        event.AddParam("sdk_name", selfShared->sdkName_);
        event.AddParam("begin_time", selfShared->beginTime_);
        event.AddParam("end_time", endTime);
        event.AddParam("result", result);
        event.AddParam("error_code", errCode);
        int ret = Write(event);
        NETMGR_LOG_D("transId:%{public}s, apiName:%{public}s, sdkName:%{public}s, "
            "startTime:%{public}ld, endTime:%{public}ld, result:%{public}d, errCode:%{public}d, ret:%{public}d",
            selfShared->transId_.c_str(), selfShared->apiName_.c_str(), selfShared->sdkName_.c_str(),
            selfShared->beginTime_, endTime, result, errCode, ret);
        }, {}, {}, ffrt::task_attr().name("reportSdkEvent"));
#endif
}

int64_t HiAppEventReport::AddProcessor()
{
#ifdef ENABLE_EMULATOR
    NETMGR_LOG_D("AddProcessor enter");
    OHOS::HiviewDFX::HiAppEvent::ReportConfig config;
    config.name = "ha_app_event";
    config.appId = "com_hua" "wei_hmos_sdk_ocg";
    config.routeInfo = "AUTO";
    config.triggerCond.timeout = TIMEOUT;
    config.triggerCond.row = ROW;
    config.eventConfigs.clear();
    {
        OHOS::HiviewDFX::HiAppEvent::EventConfig event;
        event.domain = "api_diagnostic";
        event.name = "api_exec_end";
        event.isRealTime = false;
        config.eventConfigs.push_back(event);
    }
    {
        OHOS::HiviewDFX::HiAppEvent::EventConfig event2;
        event2.domain = "api_diagnostic";
        event2.name = "api_called_stat";
        event2.isRealTime = true;
        config.eventConfigs.push_back(event2);
    }
    {
        OHOS::HiviewDFX::HiAppEvent::EventConfig event3;
        event3.domain = "api_diagnostic";
        event3.name = "api_called_stat_cnt";
        event3.isRealTime = true;
        config.eventConfigs.push_back(event3);
    }
    return OHOS::HiviewDFX::HiAppEvent::AppEventProcessorMgr::AddProcessor(config);
#endif
}
} // namespace NetManagerStandard
} // namespace OHOS

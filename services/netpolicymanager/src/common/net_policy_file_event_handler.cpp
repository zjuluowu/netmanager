/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "net_policy_file_event_handler.h"

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

#include "net_mgr_log_wrapper.h"
#include "net_policy_inner_define.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr uint32_t MAX_TIME_MS_DELTA = 5000;
constexpr uint32_t SEND_TIME_MS_INTERVAL = 2000;

int64_t GetNowMilliSeconds()
{
    auto nowSys = AppExecFwk::InnerEvent::Clock::now();
    auto epoch = nowSys.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
}
} // namespace

NetPolicyFileEventHandler::NetPolicyFileEventHandler(const char *queueName) : netPolicyFileEventQueue_(queueName) {}

void NetPolicyFileEventHandler::SendWriteEvent(AppExecFwk::InnerEvent::Pointer &event)
{
    SendEvent(event);
}

void NetPolicyFileEventHandler::SendEvent(const AppExecFwk::InnerEvent::Pointer &event, uint32_t delayTime)
{
    auto eventId = event->GetInnerEventId();
    auto eventData = event->GetSharedObject<PolicyFileEvent>();
    netPolicyFileEventQueue_.submit([this, eventId, eventData] { ProcessEvent(eventId, eventData); },
                                    ffrt::task_attr().delay(static_cast<uint64_t>(delayTime)).name("FfrtSendEvent"));
}

void NetPolicyFileEventHandler::ProcessEvent(uint32_t eventId, std::shared_ptr<PolicyFileEvent> eventData)
{
    if (eventId == MSG_POLICY_FILE_WRITE) {
        fileContent_ = eventData->json;
        if (commitWait_) {
            return;
        }
        int64_t timeDelta = GetNowMilliSeconds() - timeStamp_;
        uint32_t delay = timeDelta >= MAX_TIME_MS_DELTA ? 0 : static_cast<uint32_t>(MAX_TIME_MS_DELTA - timeDelta);
        commitWait_ = true;
        NETMGR_LOG_D("SendEvent MSG_POLICY_FILE_COMMIT[delay=%{public}d, now=%{public}s]", delay,
                     std::to_string(GetNowMilliSeconds()).c_str());
        SendEvent(AppExecFwk::InnerEvent::Get(MSG_POLICY_FILE_COMMIT, std::make_shared<PolicyFileEvent>()), delay);
        return;
    }

    if (eventId == MSG_POLICY_FILE_COMMIT) {
        commitWait_ = !Write();
        timeStamp_ = GetNowMilliSeconds();
        if (commitWait_) {
            SendEvent(AppExecFwk::InnerEvent::Get(MSG_POLICY_FILE_COMMIT, std::make_shared<PolicyFileEvent>()),
                      MAX_TIME_MS_DELTA);
        }
        SendEvent(AppExecFwk::InnerEvent::Get(MSG_POLICY_FILE_DELETE, std::make_shared<PolicyFileEvent>()),
                  SEND_TIME_MS_INTERVAL);
        return;
    }

    if (MSG_POLICY_FILE_DELETE == eventId) {
        DeleteBak();
    }
}

bool NetPolicyFileEventHandler::Write()
{
    NETMGR_LOG_D("write file to disk.");
    struct stat buffer;
    CommonUtils::RemoveDeleteControlFromPath(POLICY_FILE_BAK_PATH);
    if (stat(POLICY_FILE_NAME, &buffer) == 0) {
        std::ifstream oldFile(POLICY_FILE_NAME, std::ios::binary);
        std::ofstream newFile(POLICY_FILE_BAK_NAME, std::ios::binary);
        if (!oldFile.is_open() && !newFile.is_open()) {
            NETMGR_LOG_E("File backup failed.");
            return false;
        }
        newFile << oldFile.rdbuf();
        oldFile.close();
        newFile.close();
    }
    std::fstream file(POLICY_FILE_NAME, std::fstream::out | std::fstream::trunc);
    if (!file.is_open()) {
        NETMGR_LOG_E("open file error.");
        return false;
    }
    file << fileContent_;
    file.close();
    return true;
}

bool NetPolicyFileEventHandler::DeleteBak()
{
    struct stat buffer;
    if (stat(POLICY_FILE_BAK_NAME, &buffer) == 0) {
        int32_t err = remove(POLICY_FILE_BAK_NAME);
        if (err != 0) {
            NETMGR_LOG_E("remove file error.");
            return false;
        }
        int fd = open(POLICY_FILE_BAK_PATH, O_RDONLY);
        if (fd == -1) {
            NETMGR_LOG_E("open the file path failed.");
            return false;
        }
        if (fsync(fd) != 0) {
            NETMGR_LOG_E("fsync the file path failed.");
            close(fd);
            return false;
        }
        close(fd);
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS

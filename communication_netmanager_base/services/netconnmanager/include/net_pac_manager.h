/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef JERRY_NET_PAC_MANAGER_H
#define JERRY_NET_PAC_MANAGER_H
#include <string>
#include <memory>
#include "mutex"
namespace OHOS {
namespace NetManagerStandard {
enum PAC_STATUS {
    /**
     * 执行成功
     */
    PAC_OK,

    /**
     * pac脚本格式有误
     */
    PAC_SCRIPT_DOWNLOAD_ERROR,

    /**
     * pac加载执行错误
     */
    PAC_SCRIPT_RUN_ERROR,

    /**
     * pac FindProxyForURL函数没有找到
     */
    PAC_SCRIPT_FUNCTION_ERROR,

    /**
     *pac FindProxyForURL函数调用错误
     */
    PAC_SCRIPT_CALL_ERROR
};

class NetPACManager {
public:
    NetPACManager();

    ~NetPACManager();

    bool InitPACScriptWithURL(const std::string &scriptUrl);

    bool InitPACScript(const std::string &script);

    PAC_STATUS FindProxyForURL(const std::string &url, std::string &proxy);

    PAC_STATUS FindProxyForURL(const std::string &url, const std::string &host, std::string &proxy);

    void DownloadPACScript(const std::string &url);

    std::string ParseHost(const std::string &url);

private:
    void ReleasePACScript();

    uint32_t pacScriptVal_;
    std::mutex pacMutex_;
    std::string scriptFileUrl_;
    bool status_;
    bool engineInitialized_;
};
}  // namespace NetManagerStandard
}  // namespace OHOS
#endif

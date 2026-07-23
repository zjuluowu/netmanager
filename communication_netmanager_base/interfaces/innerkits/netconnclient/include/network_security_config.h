/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef NETMANAGER_BASE_NET_SECURITY_CONFIG_H
#define NETMANAGER_BASE_NET_SECURITY_CONFIG_H

#include <string>
#include <set>
#include <vector>

struct cJSON;
struct x509_st;
typedef struct x509_st X509;
using ComponentCfg = std::unordered_map<std::string, bool>;
namespace OHOS {
namespace NetManagerStandard {
struct Domain {
    std::string domainName_;
    bool includeSubDomains_;
};

struct TrustAnchors {
    std::vector<std::string> certs_;
};

struct Pin {
    std::string digestAlgorithm_;
    std::string digest_;
};

struct PinSet {
    bool isOpenMode = false;
    bool shouldVerifyRootCa_ = false;
    std::vector<Pin> pins_;
    std::string expiration_;
};

struct BaseConfig {
    bool cleartextTrafficPermitted_ = true;
    TrustAnchors trustAnchors_;
};

struct DomainConfig {
    bool cleartextTrafficPermitted_ = true;
    std::vector<Domain> domains_;
    TrustAnchors trustAnchors_;
    PinSet pinSet_;
};

class NetworkSecurityConfig final {
public:
    static NetworkSecurityConfig& GetInstance();
    int32_t GetPinSetForHostName(const std::string &hostname, std::string &pins);
    bool IsPinOpenMode(const std::string &hostname);
    bool IsPinOpenModeVerifyRootCa(const std::string &hostname);
    bool TrustUser0Ca();
    bool TrustUserCa();
    int32_t GetTrustAnchorsForHostName(const std::string &hostname, std::vector<std::string> &certs);
    bool IsUserDnsCache();
    int32_t IsCleartextPermitted(bool &baseCleartextPermitted);
    int32_t IsCleartextPermitted(const std::string &hostname, bool &cleartextPermitted);
    int32_t IsCleartextCfgByComponent(const std::string &component, bool &componentCfg);

private:
    int32_t GetConfig();
    bool IsCACertFileName(const char *fileName);
    void GetCAFilesFromPath(const std::string caPath, std::vector<std::string> &caFiles);
    void AddSurfixToCACertFileName(const std::string &caPath,
                                   std::set<std::string> &allFileNames, std::string &caFile);
    X509 *ReadCertFile(const std::string &fileName);
    std::string GetRehashedCADirName(const std::string &caPath);
    std::string BuildRehasedCAPath(const std::string &caPath);
    std::string GetRehasedCAPath(const std::string &caPath);
    std::string ReHashCAPathForX509(const std::string &caPath);
    int32_t CreateRehashedCertFiles();
    int32_t GetJsonFromBundle(std::string &jsonProfile);
    int32_t ParseJsonConfig(const std::string &content);
    void ParseJsonBaseConfig(const cJSON* const root, BaseConfig &baseConfig);
    void ParseJsonDomainConfigs(const cJSON* const root, std::vector<DomainConfig> &domainConfigs);
    void ParseJsonTrustAnchors(const cJSON* const root, TrustAnchors &trustAnchors);
    void ParseJsonDomains(const cJSON* const root, std::vector<Domain> &domains);
    void ParseJsonPinSet(const cJSON* const root, PinSet &pinSet);
    bool ValidateDate(const std::string &dateStr);
    void DumpConfigs();
    std::string GetJsonProfile();
    void ParseJsonCleartextPermitted(const cJSON* const root, bool &cleartextPermitted);
    void ParseJsonComponentCfg(const cJSON* const root, ComponentCfg &componentConfigs);
    void ParseJsonComponentCfg(const cJSON* const root, ComponentCfg &componentConfigs, const std::string &component);

private:
    NetworkSecurityConfig();
    ~NetworkSecurityConfig();
    BaseConfig baseConfig_;
    std::vector<DomainConfig> domainConfigs_;
    bool trustUser0Ca_ = true;
    bool trustUserCa_ = true;
    bool isUserDnsCache_ = true;
    bool hasBaseConfig_ = false;
    ComponentCfg componentConfig_ = {
        {"Network Kit", true},
        {"Request", true},
        {"Remote Communication Kit", false},
        {"Media Kit", false},
        {"ArkWeb", false}
    };
};

}
}
#endif /* NETMANAGER_BASE_NET_SECURITY_CONFIG_H */

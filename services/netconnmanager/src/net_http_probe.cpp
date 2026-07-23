/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "net_http_probe.h"

#include <cerrno>
#include <memory>
#include <numeric>
#include <unistd.h>

#include "fwmark_client.h"
#include "netsys_controller.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_proxy_userinfo.h"
#include "netmanager_base_common_utils.h"

#define NETPROBE_CURL_EASY_SET_OPTION(handle, opt, data)                                                     \
    do {                                                                                                     \
        CURLcode result = curl_easy_setopt(handle, opt, data);                                               \
        if (result != CURLE_OK) {                                                                            \
            const char *err = curl_easy_strerror(result);                                                    \
            NETMGR_LOG_E("Failed to set curl option: %{public}s, %{public}s %{public}d", #opt, err, result); \
            return false;                                                                                    \
        }                                                                                                    \
    } while (0)

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr int PERFORM_POLL_INTERVAL_MS = 50;
constexpr int32_t HTTP_OK_CODE = 200;
constexpr int32_t HTTP_SUCCESS_CODE = 204;
constexpr int32_t DEFAULT_CONTENT_LENGTH_VALUE = -1;
constexpr int32_t MIN_VALID_CONTENT_LENGTH_VALUE = 5;
constexpr int32_t FAIL_CODE = 599;
constexpr int32_t PORTAL_CODE = 302;
constexpr int32_t HTTP_RES_CODE_BAD_REQUEST = 400;
constexpr int32_t HTTP_RES_CODE_CLIENT_ERRORS_MAX = 499;
constexpr int CURL_CONNECT_TIME_OUT_MS = 10000;
constexpr int CURL_OPERATE_TIME_OUT_MS = 10000;
constexpr int32_t DOMAIN_IP_ADDR_LEN_MAX = 128;
constexpr int32_t DEFAULT_HTTP_PORT = 80;
constexpr int32_t DEFAULT_HTTPS_PORT = 443;
constexpr const char *ADDR_SEPARATOR = ",";
constexpr const char *SYMBOL_COLON = ":";
const std::string DEFAULT_USER_AGENT = std::string("User-Agent: Mozilla/5.0 (X11; Linux x86_64) ") +
    std::string("AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.32 Safari/537.36");
constexpr const char *CONNECTION_PROPERTY = "Connection: close";
constexpr const char *ACCEPT_ENCODING = "Accept-Encoding: gzip";
constexpr const char *ACCEPT = "Accept:";
const std::string CONNECTION_CLOSE_VALUE = "close";
const std::string CONNECTION_KEY = "Connection:";
const std::string CONTENT_LENGTH_KEY = "Content-Length:";
const std::string KEY_WORDS_REDIRECTION = "location.replace";
const std::string HTML_TITLE_HTTP_EN = "http://";
const std::string HTML_TITLE_HTTPS_EN = "https://";
constexpr const char NEW_LINE_STR = '\n';
constexpr const char *TLS12_SECURITY_CIPHER_SUITE =
        "DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:DHE-DSS-AES128-GCM-SHA256:DHE-DSS-AES256-GCM-SHA384:"
        "PSK-AES256-GCM-SHA384:DHE-PSK-AES128-GCM-SHA256:DHE-PSK-AES256-GCM-SHA384:DHE-PSK-CHACHA20-POLY1305:"
        "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:"
        "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-PSK-CHACHA20-POLY1305:DHE-RSA-AES128-CCM:"
        "DHE-RSA-AES256-CCM:DHE-RSA-CHACHA20-POLY1305:PSK-AES256-CCM:DHE-PSK-AES128-CCM:DHE-PSK-AES256-CCM:"
        "ECDHE-ECDSA-AES128-CCM:ECDHE-ECDSA-AES256-CCM:ECDHE-ECDSA-CHACHA20-POLY1305";
constexpr const char *TLS13_SECURITY_CIPHER_SUITE = "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:"
        "TLS_AES_128_GCM_SHA256:TLS_AES_128_CCM_SHA256";
} // namespace

std::mutex NetHttpProbe::initCurlMutex_;
int32_t NetHttpProbe::useCurlCount_ = 0;
bool NetHttpProbe::CurlGlobalInit()
{
    NETMGR_LOG_D("curl_global_init() in");
    std::lock_guard<std::mutex> lock(initCurlMutex_);
    if (useCurlCount_ == 0) {
        NETMGR_LOG_D("Call curl_global_init()");
        if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
            NETMGR_LOG_E("curl_global_init() failed");
            return false;
        }
    }
    useCurlCount_++;
    NETMGR_LOG_D("curl_global_init() count:[%{public}d]", useCurlCount_);
    return true;
}

void NetHttpProbe::SetXReqId(const std::string& xReqId, int8_t xReqIdLen)
{
    xReqId_ = xReqId;
    xReqIdLen_ = xReqIdLen;
}

void NetHttpProbe::CurlGlobalCleanup()
{
    NETMGR_LOG_D("CurlGlobalCleanup() in");
    std::lock_guard<std::mutex> lock(initCurlMutex_);
    useCurlCount_ = useCurlCount_ > 0 ? (useCurlCount_ - 1) : 0;
    NETMGR_LOG_D("Curl global used count remain:[%{public}d]", useCurlCount_);
    if (useCurlCount_ == 0) {
        NETMGR_LOG_D("Call curl_global_cleanup()");
        curl_global_cleanup();
    }
}

NetHttpProbe::NetHttpProbe(uint32_t netId, NetBearType bearType, const NetLinkInfo &netLinkInfo,
    ProbeType probeType, std::string ipAddrList)
    : netId_(netId), netBearType_(bearType), netLinkInfo_(netLinkInfo),
    probeType_(probeType), ipAddrList_(ipAddrList)
{
    isCurlInit_ = NetHttpProbe::CurlGlobalInit();
}

NetHttpProbe::NetHttpProbe()
    : netId_(0), netBearType_(BEARER_WIFI), probeType_(ProbeType::PROBE_HTTP)
{}

NetHttpProbe::~NetHttpProbe()
{
    NetHttpProbe::CurlGlobalCleanup();
    isCurlInit_ = false;
}

int32_t NetHttpProbe::SendProbe(ProbeType probeType, const std::string &httpUrl, const std::string &httpsUrl)
{
    NETMGR_LOG_I("Send net:[%{public}d] %{public}s probe in", netId_,
        ((IsHttpsDetect(probeType)) ? "https" : "http"));
    ClearProbeResult();
    if (!CheckCurlGlobalInitState()) {
        return NETMANAGER_ERR_INTERNAL;
    }

    if (!InitHttpCurl(probeType)) {
        CleanHttpCurl();
        return NETMANAGER_ERR_INTERNAL;
    }

    if (!SetCurlOptions(probeType, httpUrl, httpsUrl)) {
        NETMGR_LOG_E("Set http/https probe options failed");
        CleanHttpCurl();
        return NETMANAGER_ERR_INTERNAL;
    }

    SendHttpProbeRequest();
    RecvHttpProbeResponse();
    CleanHttpCurl();
    if (!defaultUseGlobalHttpProxy_) {
        defaultUseGlobalHttpProxy_ = true;
    }
    return NETMANAGER_SUCCESS;
}

NetHttpProbeResult NetHttpProbe::GetHttpProbeResult() const
{
    return httpProbeResult_;
}

NetHttpProbeResult NetHttpProbe::GetHttpsProbeResult() const
{
    return httpsProbeResult_;
}

void NetHttpProbe::UpdateNetLinkInfo(const NetLinkInfo &netLinkInfo)
{
    netLinkInfo_ = netLinkInfo;
}

void NetHttpProbe::UpdateGlobalHttpProxy(const HttpProxy &httpProxy)
{
    std::lock_guard<std::mutex> locker(proxyMtx_);
    globalHttpProxy_ = httpProxy;
}

bool NetHttpProbe::CheckCurlGlobalInitState()
{
    if (!isCurlInit_) {
        NETMGR_LOG_E("Curl global does not initialized, attempting to reinitialize.");
        isCurlInit_ = NetHttpProbe::CurlGlobalInit();
    }
    return isCurlInit_;
}

void NetHttpProbe::CleanHttpCurl()
{
    if (httpCurl_) {
        if (curlMulti_) {
            curl_multi_remove_handle(curlMulti_, httpCurl_);
        }
        curl_easy_cleanup(httpCurl_);
        std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
        httpCurl_ = nullptr;
    }

    if (httpsCurl_) {
        if (curlMulti_) {
            curl_multi_remove_handle(curlMulti_, httpsCurl_);
        }
        curl_easy_cleanup(httpsCurl_);
        std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
        httpsCurl_ = nullptr;
    }

    if (httpResolveList_) {
        curl_slist_free_all(httpResolveList_);
        std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
        httpResolveList_ = nullptr;
    }

    if (httpsResolveList_) {
        curl_slist_free_all(httpsResolveList_);
        std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
        httpsResolveList_ = nullptr;
    }

    if (curlMulti_) {
        curl_multi_cleanup(curlMulti_);
        std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
        curlMulti_ = nullptr;
    }
}

void NetHttpProbe::ClearProbeResult()
{
    httpProbeResult_ = {};
    httpsProbeResult_ = {};
}

std::string NetHttpProbe::ExtractDomainFormUrl(const std::string &url)
{
    if (url.empty()) {
        return std::string();
    }

    size_t doubleSlashPos = url.find("//");
    if (doubleSlashPos == std::string::npos) {
        return url;
    }

    std::string domain;
    size_t domainStartPos = doubleSlashPos + 2;
    size_t domainEndPos = url.find('/', domainStartPos);
    if (domainEndPos != std::string::npos) {
        domain = url.substr(domainStartPos, domainEndPos - domainStartPos);
    } else {
        domain = url.substr(domainStartPos);
    }
    return domain;
}

std::string NetHttpProbe::GetAddrInfo(const std::string &domain)
{
    if (domain.empty()) {
        NETMGR_LOG_E("domain is empty");
        return std::string();
    }

    struct addrinfo *result = nullptr;
    struct queryparam qparam = {};
    qparam.qp_netid = static_cast<int>(netId_);
    qparam.qp_type = QEURY_TYPE_NETSYS;

    int32_t ret = getaddrinfo_ext(domain.c_str(), nullptr, nullptr, &result, &qparam);
    if (ret < 0) {
        NETMGR_LOG_E("Get net[%{public}d] address info failed,errno[%{public}d]:%{public}s", netId_, errno,
                     strerror(errno));
        return std::string();
    }

    std::string ipAddress;
    char ip[DOMAIN_IP_ADDR_LEN_MAX] = {0};
    for (addrinfo *tmp = result; tmp != nullptr; tmp = tmp->ai_next) {
        errno_t err = memset_s(&ip, sizeof(ip), 0, sizeof(ip));
        if (err != EOK) {
            NETMGR_LOG_E("memset_s failed,err:%{public}d", err);
            freeaddrinfo(result);
            return std::string();
        }
        if (tmp->ai_family == AF_INET) {
            auto addr = reinterpret_cast<sockaddr_in *>(tmp->ai_addr);
            if (!inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip))) {
                continue;
            }
        } else if (tmp->ai_family == AF_INET6) {
            auto addr = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
            if (!inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip))) {
                continue;
            }
        }
        if (ipAddress.find(ip) != std::string::npos) {
            continue;
        }
        ipAddress = ipAddress.empty() ? (ipAddress + ip) : (ipAddress + ADDR_SEPARATOR + ip);
    }

    freeaddrinfo(result);

    if (ipAddress.empty()) {
        NETMGR_LOG_E("Get net[%{public}d] address info return nullptr result",  netId_);
        return std::string();
    }
    return ipAddress;
}

bool NetHttpProbe::InitHttpCurl(ProbeType probeType)
{
    curlMulti_ = curl_multi_init();
    if (curlMulti_ == nullptr) {
        NETMGR_LOG_E("curl_multi_init() failed.");
        return false;
    }

    if (IsHttpDetect(probeType)) {
        httpCurl_ = curl_easy_init();
        if (!httpCurl_) {
            NETMGR_LOG_E("httpCurl_ init failed");
            return false;
        }
    }

    if (IsHttpsDetect(probeType)) {
        httpsCurl_ = curl_easy_init();
        if (!httpsCurl_) {
            NETMGR_LOG_E("httpsCurl_ init failed");
            return false;
        }
    }
    return true;
}

bool NetHttpProbe::SetCurlOptions(ProbeType probeType, const std::string &httpUrl, const std::string &httpsUrl)
{
    bool useProxy = false;
    if (!SetProxyOption(probeType, useProxy)) {
        NETMGR_LOG_E("Set curl proxy option failed.");
        return false;
    }
    if (!SendDnsProbe(probeType, httpUrl, httpsUrl, useProxy)) {
        NETMGR_LOG_E("Set resolve option failed.");
        return false;
    }

    if (IsHttpDetect(probeType)) {
        if (!SetHttpOptions(ProbeType::PROBE_HTTP, httpCurl_, httpUrl)) {
            return false;
        }
    }

    if (IsHttpsDetect(probeType)) {
        if (!SetHttpOptions(ProbeType::PROBE_HTTPS, httpsCurl_, httpsUrl)) {
            return false;
        }
    }

    return true;
}

size_t NetHttpProbe::HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    std::string* data = static_cast<std::string*>(userdata);
    NETMGR_LOG_D("recv data size:[%{public}zu] nitems:[%{public}zu]", size, nitems);
    if (size * nitems > CURL_MAX_HTTP_HEADER) {
        NETMGR_LOG_E("recv data error, greater than 100K");
        return 0;
    }
    if (data != nullptr && buffer != nullptr) {
        data->append(buffer, size * nitems);
    }
    return size * nitems;
}

bool NetHttpProbe::SetHttpOptions(ProbeType probeType, CURL *curl, const std::string &url)
{
    if (!curl) {
        NETMGR_LOG_E("curl is nullptr");
        return false;
    }
    if (url.empty()) {
        NETMGR_LOG_E("Probe url is empty");
        return false;
    }
    struct curl_slist *list = nullptr;
    list = curl_slist_append(list, ACCEPT);
    list = curl_slist_append(list, CONNECTION_PROPERTY);
    list = curl_slist_append(list, DEFAULT_USER_AGENT.c_str());
    list = curl_slist_append(list, ACCEPT_ENCODING);
    if (!list) {
        NETMGR_LOG_E("add request header properties failed.");
        return false;
    }

    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_VERBOSE, 0L);
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_FORBID_REUSE, 1L);
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_URL, url.c_str());
    if (IsHttpsDetect(probeType)) {
        /* the connection succeeds regardless of the peer certificate validation */
        NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        /* the connection succeeds regardless of the names in the certificate. */
        NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_SSL_CIPHER_LIST, TLS12_SECURITY_CIPHER_SUITE);
        NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_TLS13_CIPHERS, TLS13_SECURITY_CIPHER_SUITE);
    }
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_NOSIGNAL, 1L);
    /* connection timeout time */
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_CONNECTTIMEOUT_MS, CURL_CONNECT_TIME_OUT_MS);
    /* transfer operation timeout time */
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_TIMEOUT_MS, CURL_OPERATE_TIME_OUT_MS);
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_INTERFACE, netLinkInfo_.ifaceName_.c_str());
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_HEADERFUNCTION, NetHttpProbe::HeaderCallback);
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_HEADERDATA, &respHeader_);
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_HEADER, 1L);
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_HTTPHEADER, list);
    NETPROBE_CURL_EASY_SET_OPTION(curl, CURLOPT_ERRORBUFFER, errBuffer);

    CURLMcode code = curl_multi_add_handle(curlMulti_, curl);
    if (code != CURLM_OK) {
        NETMGR_LOG_E("curl multi add handle failed, code:[%{public}d]", code);
        return false;
    }
    return true;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response)
{
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
}

bool NetHttpProbe::NetDetection(const std::string& portalUrl, PortalResponse& resp)
{
    if (portalUrl.size() > MAX_URL_LEN || portalUrl.empty()) {
        NETMGR_LOG_E("rawUrl too long or empty");
        return false;
    }
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string htmlResponse;
        long http_code;
        curl_easy_setopt(curl, CURLOPT_URL, portalUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_INTERFACE, "wlan0");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlResponse);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            NETMGR_LOG_E("curl_easy_perform failed");
            curl_easy_cleanup(curl);
            return false;
        }
        // LCOV_EXCL_START
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        resp.respCode = static_cast<int>(http_code);
        std::string respUrl = CommonUtils::ExtractMetaRefreshUrl(htmlResponse);
        curl_easy_cleanup(curl);
        if (strcpy_s(resp.url, MAX_URL_LEN - 1, respUrl.c_str()) != 0) {
            NETMGR_LOG_E("url copy failed");
            return false;
        }
        return true;
        // LCOV_EXCL_STOP
    }
    return false;
}

bool NetHttpProbe::SetProxyOption(ProbeType probeType, bool &useHttpProxy)
{
    useHttpProxy = false;
    /* WIFI or Ethernet require the use of proxy for network detection */
    if (netBearType_ != BEARER_WIFI && netBearType_ != BEARER_ETHERNET) {
        NETMGR_LOG_W("Net:[%{public}d] bear type:[%{public}d], no proxy probe required.", netId_, probeType);
        return true;
    }

    std::string proxyHost;
    int32_t proxyPort = 0;
    /* Prioritize the use of global HTTP proxy, if there is no global proxy, use network http proxy */
    if (!LoadProxy(proxyHost, proxyPort)) {
        NETMGR_LOG_D("global http proxy or network proxy is empty.");
        return true;
    }

    std::string proxyDomain = ExtractDomainFormUrl(proxyHost);
    if (proxyDomain.empty()) {
        NETMGR_LOG_E("Extract proxy domain from host return empty.");
        return true;
    }
    std::string proxyIpAddress = GetAddrInfo(proxyDomain);

    NETMGR_LOG_I("Using proxy for http probe on netId:[%{public}d]", netId_);
    bool ret = true;
    if (IsHttpDetect(probeType)) {
        if (!SetProxyInfo(httpCurl_, proxyHost, proxyPort)) {
            NETMGR_LOG_E("Set proxy info failed.");
        }
        ret &= SetResolveOption(ProbeType::PROBE_HTTP, proxyDomain, proxyIpAddress, proxyPort);
    }

    if (IsHttpsDetect(probeType)) {
        if (!SetProxyInfo(httpsCurl_, proxyHost, proxyPort)) {
            NETMGR_LOG_E("Set proxy info failed.");
        }
        ret &= SetResolveOption(ProbeType::PROBE_HTTPS, proxyDomain, proxyIpAddress, proxyPort);
    }
    useHttpProxy = true;
    return ret;
}

bool NetHttpProbe::SetProxyInfo(CURL *curlHandler, const std::string &proxyHost, int32_t proxyPort)
{
    auto proxyType = (proxyHost.find("https://") != std::string::npos) ? CURLPROXY_HTTPS : CURLPROXY_HTTP;
    if (curlHandler == nullptr) {
        NETMGR_LOG_E("curlHandler is nullptr.");
        return false;
    }
    NETPROBE_CURL_EASY_SET_OPTION(curlHandler, CURLOPT_PROXY, proxyHost.c_str());
    NETPROBE_CURL_EASY_SET_OPTION(curlHandler, CURLOPT_PROXYPORT, proxyPort);
    NETPROBE_CURL_EASY_SET_OPTION(curlHandler, CURLOPT_PROXYTYPE, proxyType);
    NETPROBE_CURL_EASY_SET_OPTION(curlHandler, CURLOPT_HTTPPROXYTUNNEL, 1L);
    if (!SetUserInfo(curlHandler)) {
        NETMGR_LOG_E("Set user info failed.");
    }
    return true;
}

bool NetHttpProbe::SetUserInfo(CURL *curlHandler)
{
    HttpProxy tempProxy;
    auto userInfoHelp = NetProxyUserinfo::GetInstance();
    userInfoHelp.GetHttpProxyHostPass(tempProxy);
    auto username = tempProxy.GetUsername();
    auto passwd = tempProxy.GetPassword();
    if (!username.empty()) {
        NETPROBE_CURL_EASY_SET_OPTION(curlHandler, CURLOPT_PROXYUSERNAME, username.c_str());
        NETPROBE_CURL_EASY_SET_OPTION(curlHandler, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        NETPROBE_CURL_EASY_SET_OPTION(curlHandler, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
        if (!passwd.empty()) {
            NETPROBE_CURL_EASY_SET_OPTION(curlHandler, CURLOPT_PROXYPASSWORD, passwd.c_str());
        } else {
            NETMGR_LOG_I("passwd is empty.");
        }
    } else {
        NETMGR_LOG_I("username is empty.");
    }
    return true;
}

bool NetHttpProbe::SetResolveOption(ProbeType probeType, const std::string &domain, const std::string &ipAddress,
                                    int32_t port)
{
    if (domain.empty()) {
        NETMGR_LOG_E("domain is empty");
        return false;
    }

    if (ipAddress.empty()) {
        NETMGR_LOG_E("ipAddress is empty");
        return false;
    }

    std::string resolve = domain + SYMBOL_COLON + std::to_string(port) + SYMBOL_COLON + ipAddress;
    if (IsHttpDetect(probeType)) {
        httpResolveList_ = curl_slist_append(httpResolveList_, resolve.c_str());
        NETPROBE_CURL_EASY_SET_OPTION(httpCurl_, CURLOPT_RESOLVE, httpResolveList_);
    }

    if (IsHttpsDetect(probeType)) {
        httpsResolveList_ = curl_slist_append(httpsResolveList_, resolve.c_str());
        NETPROBE_CURL_EASY_SET_OPTION(httpsCurl_, CURLOPT_RESOLVE, httpsResolveList_);
    }
    return true;
}

bool NetHttpProbe::SendDnsProbe(ProbeType probeType, const std::string &httpUrl, const std::string &httpsUrl,
                                const bool useProxy)
{
    if (useProxy) {
        NETMGR_LOG_W("Net[%{public}d] probe use http proxy,no DNS detection required.", netId_);
        return true;
    }
    if (!ipAddrList_.empty()) {
        return IsHttpDetect(probeType) ?
            SetResolveOption(ProbeType::PROBE_HTTP, ExtractDomainFormUrl(httpUrl), ipAddrList_, DEFAULT_HTTP_PORT) :
            SetResolveOption(ProbeType::PROBE_HTTPS, ExtractDomainFormUrl(httpsUrl), ipAddrList_, DEFAULT_HTTPS_PORT);
    }

    std::string httpDomain;
    std::string httpsDomain;
    if (IsHttpDetect(probeType)) {
        httpDomain = ExtractDomainFormUrl(httpUrl);
        if (httpDomain.empty()) {
            NETMGR_LOG_E("The http domain extracted from [%{public}s] is empty", httpUrl.c_str());
            return false;
        }
    }

    if (IsHttpsDetect(probeType)) {
        httpsDomain = ExtractDomainFormUrl(httpsUrl);
        if (httpsDomain.empty()) {
            NETMGR_LOG_E("The https domain extracted from [%{public}s] is empty", httpsUrl.c_str());
            return false;
        }
    }

    std::string ipAddress;
    if (httpDomain == httpsDomain) {
        NETMGR_LOG_I("Get net[%{public}d] ip addr for HTTP&HTTPS probe url ", netId_);
        ipAddress = GetAddrInfo(httpDomain);
        return SetResolveOption(ProbeType::PROBE_HTTP, httpDomain, ipAddress, DEFAULT_HTTP_PORT) &&
               SetResolveOption(ProbeType::PROBE_HTTPS, httpsDomain, ipAddress, DEFAULT_HTTPS_PORT);
    }

    if (IsHttpDetect(probeType)) {
        if (netBearType_ == BEARER_CELLULAR) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        NETMGR_LOG_D("Get net[%{public}d] ip addr for HTTP probe url ", netId_);
        ipAddress = GetAddrInfo(httpDomain);
        return SetResolveOption(ProbeType::PROBE_HTTP, httpDomain, ipAddress, DEFAULT_HTTP_PORT);
    }

    if (IsHttpsDetect(probeType)) {
        NETMGR_LOG_D("Get net[%{public}d] ip addr for HTTPS probe url ", netId_);
        ipAddress = GetAddrInfo(httpsDomain);
        return SetResolveOption(ProbeType::PROBE_HTTPS, httpsDomain, ipAddress, DEFAULT_HTTPS_PORT);
    }
    return false;
}

void NetHttpProbe::SendHttpProbeRequest()
{
    if (!curlMulti_) {
        NETMGR_LOG_E("curlMulti_ is nullptr");
        return;
    }

    int running = 0;
    do {
        CURLMcode result = curl_multi_perform(curlMulti_, &running);
        if ((result == CURLM_OK) && running) {
            result = curl_multi_poll(curlMulti_, nullptr, 0, PERFORM_POLL_INTERVAL_MS, nullptr);
        }
        if (result != CURLM_OK) {
            NETMGR_LOG_E("curl_multi_perform() error, error code:[%{public}d]", result);
            break;
        }
    } while (running);
}

std::string NetHttpProbe::GetHeaderField(const std::string key)
{
    std::string result = "";
    if (respHeader_.empty()) {
        NETMGR_LOG_I("net[%{public}d], probeType[%{public}d] response header empty", netId_, probeType_);
        return result;
    }
    size_t start = respHeader_.find(key);
    if (start != std::string::npos) {
        start += key.length();
        size_t end = respHeader_.find(NEW_LINE_STR, start);
        result = respHeader_.substr(start, end - start);
        result = CommonUtils::Trim(result);
    }
    NETMGR_LOG_I("net[%{public}d], probeType[%{public}d], key:[%{public}s]", netId_, probeType_, key.c_str());
    return result;
}

int32_t NetHttpProbe::CheckRespCode(int32_t respCode)
{
    NETMGR_LOG_D("net[%{public}d], response code before check:%{public}d", netId_, respCode);
    if (respCode == HTTP_OK_CODE) {
        std::string contentLengthValue = GetHeaderField(CONTENT_LENGTH_KEY);
        int32_t lengthValue = contentLengthValue.empty() ? DEFAULT_CONTENT_LENGTH_VALUE :
            CommonUtils::StrToInt(contentLengthValue, DEFAULT_CONTENT_LENGTH_VALUE);
        if (lengthValue == DEFAULT_CONTENT_LENGTH_VALUE) {
            if (respHeader_.empty()) {
                NETMGR_LOG_I("net[%{public}d], response code 200 with content length -1, consider as fail", netId_);
                return FAIL_CODE;
            }
        } else if (lengthValue < MIN_VALID_CONTENT_LENGTH_VALUE) {
            NETMGR_LOG_I("net[%{public}d], response code 200, content length less 5, consider as fail", netId_);
            return FAIL_CODE;
        }

        std::string value = GetHeaderField(CONNECTION_KEY);
        value = CommonUtils::ToLower(value);
        if (CONNECTION_CLOSE_VALUE.compare(value) == 0 &&
            probeType_ == ProbeType::PROBE_HTTP) {
            NETMGR_LOG_I("net[%{public}d] http detection, response code 200 with connection close, consider as fail",
                netId_);
            return FAIL_CODE;
        }
    }
    int32_t result = respCode;
    if (IsHttpDetect(probeType_)) {
        result = CheckSuccessRespCode(result);
        result = CheckClientErrorRespCode(result);
    }
    return result;
}

int32_t NetHttpProbe::CheckSuccessRespCode(int32_t respCode)
{
    int32_t result = respCode;
    if (result != HTTP_SUCCESS_CODE) {
        return result;
    }
    std::string requestId = GetHeaderField(xReqId_);
    // LCOV_EXCL_START
    if (xReqIdLen_ < 0) {
        NETMGR_LOG_I("xReqIdLen: %{public}d", xReqIdLen_);
        return result;
    }
    if (requestId.empty() || xReqIdLen_ != static_cast<int8_t>(requestId.length())) {
        NETMGR_LOG_I("http return 204, but request id error and unreachable! xReqIdLen:%{public}d, rlen: %{public}lu",
            xReqIdLen_, requestId.length());
        result = FAIL_CODE;
    }
    // LCOV_EXCL_STOP
    return result;
}

int32_t NetHttpProbe::CheckClientErrorRespCode(int32_t respCode)
{
    int32_t result = respCode;
    if (respCode >= HTTP_RES_CODE_BAD_REQUEST && respCode <= HTTP_RES_CODE_CLIENT_ERRORS_MAX) {
        std::string errMsg(errBuffer);
        if ((errMsg.find(HTML_TITLE_HTTP_EN) != std::string::npos ||
            errMsg.find(HTML_TITLE_HTTPS_EN) != std::string::npos) &&
            errMsg.find(KEY_WORDS_REDIRECTION) != std::string::npos) {
            NETMGR_LOG_I("net[%{public}d] reset url in content, consider as portal when http return %{public}d",
                netId_, respCode);
            result = PORTAL_CODE;
        }
    }
    return result;
}

void NetHttpProbe::RecvHttpProbeResponse()
{
    if (!curlMulti_) {
        NETMGR_LOG_E("curlMulti_ is nullptr");
        return;
    }
    CURLMsg *curlMsg = nullptr;
    int32_t msgQueue = 0;
    while ((curlMsg = curl_multi_info_read(curlMulti_, &msgQueue)) != nullptr) {
        if (curlMsg->msg != CURLMSG_DONE) {
            NETMGR_LOG_W("curl multi read not done, msg:[%{public}d]", curlMsg->msg);
            continue;
        }

        if (!curlMsg->easy_handle) {
            NETMGR_LOG_E("Read nullptr curl easy handle");
            continue;
        }

        int64_t responseCode = 0;
        curl_easy_getinfo(curlMsg->easy_handle, CURLINFO_RESPONSE_CODE, &responseCode);
        responseCode = CheckRespCode(static_cast<int32_t>(responseCode));
        std::string redirectUrl;
        char* url = nullptr;
        curl_easy_getinfo(curlMsg->easy_handle, CURLINFO_REDIRECT_URL, &url);
        if (url != nullptr) {
            redirectUrl = url;
        } else {
            curl_easy_getinfo(curlMsg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);
            redirectUrl = url;
        }

        if (curlMsg->easy_handle == httpCurl_) {
            httpProbeResult_ = {responseCode, redirectUrl};
            NETMGR_LOG_I("Recv net[%{public}d] http probe response, code:[%{public}d]", netId_,
                         httpProbeResult_.GetCode());
        } else if (curlMsg->easy_handle == httpsCurl_) {
            httpsProbeResult_ = {responseCode, redirectUrl};
            NETMGR_LOGD("Recv net[%{public}d] https probe response, code:[%{public}d]", netId_,
                         httpsProbeResult_.GetCode());
        } else {
            NETMGR_LOG_E("Unknown curl handle.");
        }
    }
}

int32_t NetHttpProbe::LoadProxy(std::string &proxyHost, int32_t &proxyPort)
{
    std::lock_guard<std::mutex> locker(proxyMtx_);
    if (!globalHttpProxy_.GetHost().empty() && defaultUseGlobalHttpProxy_) {
        proxyHost = globalHttpProxy_.GetHost();
        proxyPort = static_cast<int32_t>(globalHttpProxy_.GetPort());
    } else if (!netLinkInfo_.httpProxy_.GetHost().empty()) {
        proxyHost = netLinkInfo_.httpProxy_.GetHost();
        proxyPort = static_cast<int32_t>(netLinkInfo_.httpProxy_.GetPort());
    } else {
        return false;
    }
    return true;
}

void NetHttpProbe::ProbeWithoutGlobalHttpProxy()
{
    defaultUseGlobalHttpProxy_ = false;
}

bool NetHttpProbe::IsHttpDetect(ProbeType probeType)
{
    return probeType == ProbeType::PROBE_HTTP || probeType == ProbeType::PROBE_HTTP_FALLBACK;
}

bool NetHttpProbe::IsHttpsDetect(ProbeType probeType)
{
    return probeType == ProbeType::PROBE_HTTPS || probeType == ProbeType::PROBE_HTTPS_FALLBACK;
}

} // namespace NetManagerStandard
} // namespace OHOS

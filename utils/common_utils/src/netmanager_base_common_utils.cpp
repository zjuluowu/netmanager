/*
 * Copyright (C) 2022-2024 Huawei Device Co., Ltd.
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

#include "netmanager_base_common_utils.h"

#include <algorithm>
#include <arpa/inet.h>
#include <cstddef>
#include <cstdlib>
#include <netinet/in.h>
#include <regex>
#include <sstream>
#include <set>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <thread>
#include <type_traits>
#include <unistd.h>
#include <vector>
#include <numeric>
#include <fstream>
#include <random>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#ifndef CROSS_PLATFORM
#include "parameters.h"
#include "dfx_kernel_stack.h"
#endif
#include "securec.h"
#include "datetime_ex.h"

namespace OHOS::NetManagerStandard::CommonUtils {
constexpr int32_t INET_OPTION_SUC = 1;
constexpr int32_t DECIMAL_SYSTEM = 10;
constexpr uint32_t CONST_MASK = 0x80000000;
constexpr size_t MAX_DISPLAY_NUM = 2;
constexpr uint32_t IPV4_DOT_NUM = 3;
constexpr int32_t MIN_BYTE = 0;
constexpr int32_t MAX_BYTE = 255;
constexpr int32_t BYTE_16 = 16;
constexpr uint32_t BIT_NUM_BYTE = 8;
constexpr int32_t BITS_32 = 32;
constexpr int32_t BITS_24 = 24;
constexpr int32_t BITS_16 = 16;
constexpr int32_t BITS_8 = 8;
constexpr uint32_t INTERFACE_NAME_MAX_SIZE = 16;
constexpr int32_t CHAR_ARRAY_SIZE_MAX = 1024;
constexpr int32_t PIPE_FD_NUM = 2;
constexpr int32_t PIPE_OUT = 0;
constexpr int32_t PIPE_IN = 1;
constexpr int32_t DOMAIN_VALID_MIN_PART_SIZE = 2;
constexpr int32_t DOMAIN_VALID_MAX_PART_SIZE = 5;
constexpr int32_t NET_MASK_MAX_LENGTH = 32;
constexpr int32_t NET_MASK_GROUP_COUNT = 4;
constexpr int32_t MAX_IPV6_PREFIX_LENGTH = 128;
constexpr int32_t WAIT_FOR_PID_TIME_MS = 20;
constexpr int32_t MAX_WAIT_PID_COUNT = 500;
constexpr int32_t IPTABLES_READ_TIME_OUT = 10000; // 10s
constexpr const char *IPADDR_DELIMITER = ".";
constexpr const char *IP6ADDR_DELIMITER = ":";
constexpr const char *CMD_SEP = " ";
constexpr const char *DOMAIN_DELIMITER = ".";
constexpr const char *TLDS_SPLIT_SYMBOL = "|";
constexpr const char *HOST_DOMAIN_PATTERN_HEADER = "^(https?://)?[a-zA-Z0-9-]+(\\.[a-zA-Z0-9-]+)*\\.(";
constexpr const char *HOST_DOMAIN_PATTERN_TAIL = ")$";
constexpr const char *DEFAULT_IPV4_ANY_INIT_ADDR = "0.0.0.0";
constexpr const char *DEFAULT_IPV6_ANY_INIT_ADDR = "::";
const std::string DISPLAY_TRAFFIC_ANCO_LIST = "const.netmanager.display_traffic_anco_list";
const std::regex IP_PATTERN{
    "((2([0-4]\\d|5[0-5])|1\\d\\d|[1-9]\\d|\\d)\\.){3}(2([0-4]\\d|5[0-5])|1\\d\\d|[1-9]\\d|\\d)"};

const std::regex IP_MASK_PATTERN{
    "((2([0-4]\\d|5[0-5])|1\\d\\d|[1-9]\\d|\\d)\\.){3}(2([0-4]\\d|5[0-5])|1\\d\\d|[1-9]\\d|\\d)/"
    "(3[0-2]|[1-2]\\d|\\d)"};

const std::regex IPV6_PATTERN{"([\\da-fA-F]{0,4}:){2,7}([\\da-fA-F]{0,4})"};

const std::regex IPV6_MASK_PATTERN{"([\\da-fA-F]{0,4}:){2,7}([\\da-fA-F]{0,4})/(1[0-2][0-8]|[1-9]\\d|[1-9])"};

std::vector<std::string> HOST_DOMAIN_TLDS{"com",  "net",     "org",    "edu",  "gov", "mil",  "cn",   "hk",  "tw",
                                          "jp",   "de",      "uk",     "fr",   "au",  "ca",   "br",   "ru",  "it",
                                          "es",   "in",      "online", "shop", "vip", "club", "xyz",  "top", "icu",
                                          "work", "website", "tech",   "asia", "xin", "co",   "mobi", "info"};
constexpr const char *BUNDLENAME_DELIMITER = ",";
constexpr const char *SIM_APP_BUNDLENAMES = "com.droi.iapps,com.droi.tong";
constexpr const char *INSTALL_SOURCE_FROM_SIM = "com.zhuoyi.appstore.lite";
constexpr const char *SIM2_BUNDLENAMES = "com.easy.abroadHarmony.temp,com.easy.hmos.abroad";
constexpr const char *INSTALL_SOURCE_FROM_SIM2 = "com.easy.abroad";
std::mutex g_commonUtilsMutex;

// IPv6 related constants to avoid magic numbers
constexpr uint8_t IPV6_LOOPBACK_BYTES[BYTE_16] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01
};

// ORCHID prefix (2001:0010::/28) components and mask/value for checking
constexpr uint8_t ORCHID_PREFIX_BYTE0 = 0x20;
constexpr uint8_t ORCHID_PREFIX_BYTE1 = 0x01;
constexpr uint8_t ORCHID_PREFIX_BYTE2 = 0x00;
constexpr uint8_t ORCHID_BYTE3_MASK = 0xF0; // high 4 bits
constexpr uint8_t ORCHID_BYTE3_VALUE = 0x10;
// Byte indices for ORCHID prefix check to avoid magic index literals
constexpr int ORCHID_BYTE_IDX0 = 0;
constexpr int ORCHID_BYTE_IDX1 = 1;
constexpr int ORCHID_BYTE_IDX2 = 2;
constexpr int ORCHID_BYTE_IDX3 = 3;

std::string Strip(const std::string &str, char ch)
{
    auto size = static_cast<int64_t>(str.size());
    int64_t i = 0;
    while (i < size && str[i] == ch) {
        ++i;
    }
    int64_t j = size - 1;
    while (j > 0 && str[j] == ch) {
        --j;
    }
    if (i >= 0 && i < size && j >= 0 && j < size && j - i + 1 > 0) {
        return str.substr(i, j - i + 1);
    }
    return "";
}

std::string ToLower(const std::string &s)
{
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(), tolower);
    return res;
}

bool IsValidIPV4(const std::string &ip)
{
    if (ip.empty()) {
        return false;
    }
    struct in_addr s;
    return inet_pton(AF_INET, ip.c_str(), reinterpret_cast<void *>(&s)) == INET_OPTION_SUC;
}

bool IsValidIPV6(const std::string &ip)
{
    if (ip.empty()) {
        return false;
    }
    struct in6_addr s;
    return inet_pton(AF_INET6, ip.c_str(), reinterpret_cast<void *>(&s)) == INET_OPTION_SUC;
}

int8_t GetAddrFamily(const std::string &ip)
{
    if (IsValidIPV4(ip)) {
        return AF_INET;
    }
    if (IsValidIPV6(ip)) {
        return AF_INET6;
    }
    return 0;
}

int GetMaskLength(const std::string &mask)
{
    int netMask = 0;
    unsigned int maskTmp = ntohl(static_cast<int>(inet_addr(mask.c_str())));
    while (maskTmp & CONST_MASK) {
        ++netMask;
        maskTmp = (maskTmp << 1);
    }
    return netMask;
}

std::string GetMaskByLength(uint32_t length)
{
    const uint32_t mask = length == 0 ? 0 : 0xFFFFFFFF << (NET_MASK_MAX_LENGTH - length);
    auto maskGroup = new int[NET_MASK_GROUP_COUNT];
    for (int i = 0; i < NET_MASK_GROUP_COUNT; i++) {
        int pos = NET_MASK_GROUP_COUNT - 1 - i;
        maskGroup[pos] = (static_cast<uint32_t>(mask) >> (i * BIT_NUM_BYTE)) & 0x000000ff;
    }
    std::string sMask = "" + std::to_string(maskGroup[0]);
    for (int i = 1; i < NET_MASK_GROUP_COUNT; i++) {
        sMask = sMask + "." + std::to_string(maskGroup[i]);
    }
    delete[] maskGroup;
    return sMask;
}

std::string GetIpv6Prefix(const std::string &ipv6Addr, uint8_t prefixLen)
{
    if (prefixLen >= MAX_IPV6_PREFIX_LENGTH) {
        return ipv6Addr;
    }

    in6_addr ipv6AddrBuf = IN6ADDR_ANY_INIT;
    inet_pton(AF_INET6, ipv6Addr.c_str(), &ipv6AddrBuf);

    char buf[INET6_ADDRSTRLEN] = {0};
    if (inet_ntop(AF_INET6, &ipv6AddrBuf, buf, INET6_ADDRSTRLEN) == nullptr) {
        return ipv6Addr;
    }

    in6_addr ipv6Prefix = IN6ADDR_ANY_INIT;
    uint32_t byteIndex = prefixLen / BIT_NUM_BYTE;
    if (memset_s(ipv6Prefix.s6_addr, sizeof(ipv6Prefix.s6_addr), 0, sizeof(ipv6Prefix.s6_addr)) != EOK ||
        memcpy_s(ipv6Prefix.s6_addr, sizeof(ipv6Prefix.s6_addr), &ipv6AddrBuf, byteIndex) != EOK) {
        return DEFAULT_IPV6_ANY_INIT_ADDR;
    }
    uint32_t bitOffset = prefixLen & 0x7;
    if ((bitOffset != 0) && (byteIndex < INET_ADDRSTRLEN)) {
        ipv6Prefix.s6_addr[byteIndex] = ipv6AddrBuf.s6_addr[byteIndex] & (0xff00 >> bitOffset);
    }
    char ipv6PrefixBuf[INET6_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET6, &ipv6Prefix, ipv6PrefixBuf, INET6_ADDRSTRLEN);
    return ipv6PrefixBuf;
}

std::string ConvertIpv4Address(uint32_t addressIpv4)
{
    if (addressIpv4 == 0) {
        return "";
    }

    std::ostringstream stream;
    stream << ((addressIpv4 >> BITS_24) & 0xFF) << IPADDR_DELIMITER << ((addressIpv4 >> BITS_16) & 0xFF)
           << IPADDR_DELIMITER << ((addressIpv4 >> BITS_8) & 0xFF) << IPADDR_DELIMITER << (addressIpv4 & 0xFF);
    return stream.str();
}

uint32_t ConvertIpv4Address(const std::string &address)
{
    std::string tmpAddress = address;
    uint32_t addrInt = 0;
    uint32_t i = 0;
    for (i = 0; i < IPV4_DOT_NUM; i++) {
        std::string::size_type npos = tmpAddress.find(IPADDR_DELIMITER);
        if (npos == std::string::npos) {
            break;
        }
        const auto &value = tmpAddress.substr(0, npos);
        int32_t itmp = std::atoi(value.c_str());
        if ((itmp < MIN_BYTE) || (itmp > MAX_BYTE)) {
            break;
        }
        uint32_t utmp = static_cast<uint32_t>(itmp);
        addrInt += utmp << ((IPV4_DOT_NUM - i) * BIT_NUM_BYTE);
        tmpAddress = tmpAddress.substr(npos + 1);
    }

    if (i != IPV4_DOT_NUM) {
        return 0;
    }
    int32_t itmp = std::atoi(tmpAddress.c_str());
    if ((itmp < MIN_BYTE) || (itmp > MAX_BYTE)) {
        return 0;
    }
    uint32_t utmp = static_cast<uint32_t>(itmp);
    addrInt += utmp;

    return addrInt;
}

int32_t Ipv4PrefixLen(const std::string &ip)
{
    if (ip.empty()) {
        return 0;
    }
    int32_t ret = 0;
    uint32_t ipNum = 0;
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    uint8_t c3 = 0;
    uint8_t c4 = 0;
    int32_t cnt = 0;
    ret = sscanf_s(ip.c_str(), "%hhu.%hhu.%hhu.%hhu", &c1, &c2, &c3, &c4);
    if (ret != sizeof(int32_t)) {
        return 0;
    }
    ipNum = (c1 << static_cast<uint32_t>(BITS_24)) | (c2 << static_cast<uint32_t>(BITS_16)) |
            (c3 << static_cast<uint32_t>(BITS_8)) | c4;
    if (ipNum == 0xFFFFFFFF) {
        return BITS_32;
    }
    if (ipNum == 0xFFFFFF00) {
        return BITS_24;
    }
    if (ipNum == 0xFFFF0000) {
        return BITS_16;
    }
    if (ipNum == 0xFF000000) {
        return BITS_8;
    }
    for (int32_t i = 0; i < BITS_32; i++) {
        if ((ipNum << i) & 0x80000000) {
            cnt++;
        } else {
            break;
        }
    }
    return cnt;
}

int32_t Ipv6PrefixLen(const std::string &ip)
{
    constexpr int32_t LENGTH_8 = 8;
    constexpr int32_t LENGTH_7 = 7;
    constexpr int32_t LENGTH_6 = 6;
    constexpr int32_t LENGTH_5 = 5;
    constexpr int32_t LENGTH_4 = 4;
    constexpr int32_t LENGTH_3 = 3;
    constexpr int32_t LENGTH_2 = 2;
    constexpr int32_t LENGTH_1 = 1;
    if (ip.empty()) {
        return 0;
    }
    in6_addr addr{};
    inet_pton(AF_INET6, ip.c_str(), &addr);
    int32_t prefixLen = 0;
    for (int32_t i = 0; i < BYTE_16; ++i) {
        if (addr.s6_addr[i] == 0xFF) {
            prefixLen += LENGTH_8;
        } else if (addr.s6_addr[i] == 0xFE) {
            prefixLen += LENGTH_7;
            break;
        } else if (addr.s6_addr[i] == 0xFC) {
            prefixLen += LENGTH_6;
            break;
        } else if (addr.s6_addr[i] == 0xF8) {
            prefixLen += LENGTH_5;
            break;
        } else if (addr.s6_addr[i] == 0xF0) {
            prefixLen += LENGTH_4;
            break;
        } else if (addr.s6_addr[i] == 0xE0) {
            prefixLen += LENGTH_3;
            break;
        } else if (addr.s6_addr[i] == 0xC0) {
            prefixLen += LENGTH_2;
            break;
        } else if (addr.s6_addr[i] == 0x80) {
            prefixLen += LENGTH_1;
            break;
        } else {
            break;
        }
    }
    return prefixLen;
}

bool ParseInt(const std::string &str, int32_t *value)
{
    char *end;
    long long v = strtoll(str.c_str(), &end, 10);
    if (std::string(end) == str || *end != '\0' || v < INT_MIN || v > INT_MAX) {
        return false;
    }
    *value = v;
    return true;
}

int64_t ConvertToInt64(const std::string &str)
{
    return strtoll(str.c_str(), nullptr, DECIMAL_SYSTEM);
}

std::string MaskIpMiddle(std::string &maskedResult, const std::string &delimiter)
{
    size_t start = maskedResult.find(delimiter);
    size_t end = maskedResult.rfind(delimiter);
    if (start == std::string::npos || end <= start + delimiter.size()) {
        start = 0;
        end = maskedResult.size();
    }
    for (size_t i = start; i < end; i++) {
        if (maskedResult[i] != delimiter[0] && maskedResult[i] != '/') {
            maskedResult[i] = '*';
        }
    }
    return maskedResult;
}

std::string MaskIpv4(std::string &maskedResult, bool maskMiddle)
{
    if (maskMiddle) {
        return MaskIpMiddle(maskedResult, IPADDR_DELIMITER);
    }
    int maxDisplayNum = MAX_DISPLAY_NUM;
    for (char &i : maskedResult) {
        if (i == '/') {
            break;
        }
        if (maxDisplayNum > 0) {
            if (i == '.') {
                maxDisplayNum--;
            }
        } else {
            if (i != '.') {
                i = '*';
            }
        }
    }
    return maskedResult;
}

std::string MaskIpv6(std::string &maskedResult, bool maskMiddle)
{
    if (maskMiddle) {
        return MaskIpMiddle(maskedResult, IP6ADDR_DELIMITER);
    }
    size_t colonCount = 0;
    for (char &i : maskedResult) {
        if (i == '/') {
            break;
        }
        if (i == ':') {
            colonCount++;
        }

        if (colonCount >= MAX_DISPLAY_NUM) { // An legal ipv6 address has at least 2 ':'.
            if (i != ':' && i != '/') {
                i = '*';
            }
        }
    }
    return maskedResult;
}

std::string ToAnonymousIp(const std::string &input, bool maskMiddle)
{
    std::lock_guard<std::mutex> lock(g_commonUtilsMutex);
    std::string maskedResult{input};
    // Mask ipv4 address.
    if (std::regex_match(maskedResult, IP_PATTERN) || std::regex_match(maskedResult, IP_MASK_PATTERN)) {
        return MaskIpv4(maskedResult, maskMiddle);
    }
    // Mask ipv6 address.
    if (std::regex_match(maskedResult, IPV6_PATTERN) || std::regex_match(maskedResult, IPV6_MASK_PATTERN)) {
        return MaskIpv6(maskedResult, maskMiddle);
    }
    return input;
}

std::string AnonymousIpInStr(const std::string &input)
{
    std::string result;
    // Mask ipv4 address.
    result = std::regex_replace(input, IP_PATTERN, "X.X.X.X");
    // Mask ipv6 address.
    result = std::regex_replace(result, IPV6_PATTERN, "X:X:X:X");
    return result;
}

std::string AnonymizeIptablesCommand(const std::string &command)
{
    std::string temp{command};
    std::transform(temp.cbegin(), temp.cend(), temp.begin(), [](char c) {
        return std::isdigit(c) ? 'x' : c;
    });
    return temp;
}

int32_t StrToInt(const std::string &value, int32_t defaultErr)
{
    errno = 0;
    char *pEnd = nullptr;
    int64_t result = std::strtol(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || (result < INT_MIN || result > INT_MAX) || errno == ERANGE) {
        return defaultErr;
    }
    return static_cast<int32_t>(result);
}

uint32_t StrToUint(const std::string &value, uint32_t defaultErr)
{
    errno = 0;
    char *pEnd = nullptr;
    uint64_t result = std::strtoul(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || result > UINT32_MAX || errno == ERANGE) {
        return defaultErr;
    }
    return result;
}

bool StrToBool(const std::string &value, bool defaultErr)
{
    errno = 0;
    char *pEnd = nullptr;
    uint64_t result = std::strtoul(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || result > UINT32_MAX || errno == ERANGE) {
        return defaultErr;
    }
    return static_cast<bool>(result);
}

int64_t StrToLong(const std::string &value, int64_t defaultErr)
{
    errno = 0;
    char *pEnd = nullptr;
    int64_t result = std::strtoll(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return defaultErr;
    }
    return result;
}

uint64_t StrToUint64(const std::string &value, uint64_t defaultErr)
{
    errno = 0;
    char *pEnd = nullptr;
    uint64_t result = std::strtoull(value.c_str(), &pEnd, 0);
    if (pEnd == value.c_str() || errno == ERANGE) {
        return defaultErr;
    }
    return result;
}

bool CheckIfaceName(const std::string &name)
{
    uint32_t index = 0;
    if (name.empty()) {
        return false;
    }
    size_t len = name.size();
    if (len > INTERFACE_NAME_MAX_SIZE) {
        return false;
    }
    while (index < len) {
        if ((index == 0) && !isalnum(name[index])) {
            return false;
        }
        if (!isalnum(name[index]) && (name[index] != '-') && (name[index] != '_') && (name[index] != '.') &&
            (name[index] != ':')) {
            return false;
        }
        index++;
    }
    return true;
}

std::string ExtractMetaRefreshUrl(const std::string& htmlContent)
{
    std::string lowerContent = htmlContent;
    std::transform(lowerContent.begin(), lowerContent.end(), lowerContent.begin(),
                   [](unsigned char c) { return std::tolower(c); });
 
    size_t metaPos = lowerContent.find("http-equiv=\"refresh\"");
    if (metaPos == std::string::npos) {
        metaPos = lowerContent.find("http-equiv='refresh'");
    }
    if (metaPos == std::string::npos) {
        return "";
    }
    size_t contentPos = lowerContent.find("content=", metaPos);
    if (contentPos == std::string::npos) {
        return "";
    }
    size_t urlPos = lowerContent.find("url=", contentPos);
    if (urlPos == std::string::npos) {
        return "";
    }
    size_t urlStart = urlPos + 4; // 跳过"url="
    size_t urlEnd = htmlContent.find_first_of("'\"", urlStart);
    if (urlEnd != std::string::npos) {
        return htmlContent.substr(urlStart, urlEnd - urlStart);
    }
    return "";
}

std::vector<const char *> FormatCmd(const std::vector<std::string> &cmd)
{
    std::vector<const char *> res;
    res.reserve(cmd.size() + 1);

    // string is converted to char * and the result is saved in res
    std::transform(cmd.begin(), cmd.end(), std::back_inserter(res), [](const std::string &str) { return str.c_str(); });
    res.emplace_back(nullptr);
    return res;
}

int32_t ForkExecChildProcess(const int32_t *pipeFd, int32_t count, const std::vector<const char *> &args)
{
    if (count != PIPE_FD_NUM) {
        _exit(-1);
    }
    if (close(pipeFd[PIPE_OUT]) != 0) {
        _exit(-1);
    }
    if (dup2(pipeFd[PIPE_IN], STDOUT_FILENO) == -1) {
        _exit(-1);
    }
    if (close(pipeFd[PIPE_IN]) != 0) {
        _exit(-1);
    }
    if (execv(args[0], const_cast<char *const *>(&args[0])) == -1) {
        NETMGR_LOG_E("execv command failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
    }
    NETMGR_LOG_D("execv done");
    _exit(-1);
}

int32_t ReadFromChildProcess(const int32_t *pipeFd, pid_t childPid, std::string *out)
{
    char buf[CHAR_ARRAY_SIZE_MAX] = {0};
    out->clear();
    NETMGR_LOG_D("ready for read");

    int fd = pipeFd[PIPE_OUT];
    int64_t start = GetTickCount();
    int ret;
    while (true) {
        struct pollfd fds[1];
        fds[0].fd = fd;
        fds[0].events = POLLIN;

        int64_t now = GetTickCount();
        int64_t elapsed = now - start;
        if (elapsed < 0 || elapsed > IPTABLES_READ_TIME_OUT) {
            ret = 0;
            break;
        }
        int64_t timeout = IPTABLES_READ_TIME_OUT - elapsed;
        ret = poll(fds, 1, timeout);
        if (ret == -1 && errno == EINTR) {
            continue;
        } else {
            break;
        }
    }

    int result = NETMANAGER_SUCCESS;
    if (ret <= 0) {
        NETMGR_LOG_E("iptables select fail, ret %{public}d, pid %{public}d", ret, childPid);
#ifndef CROSS_PLATFORM
        std::string childStack;
        HiviewDFX::DfxGetKernelStack(childPid, childStack);
        NETMGR_LOG_E("child process stack %{public}s", childStack.c_str());
#endif
        result = NETMANAGER_ERROR;
    } else {
        while (read(fd, buf, CHAR_ARRAY_SIZE_MAX - 1) > 0) {
            out->append(buf);
            (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
        }
    }
    return result;
}

int32_t ForkExecParentProcess(const int32_t *pipeFd, int32_t count, pid_t childPid, std::string *out)
{
    if (count != PIPE_FD_NUM) {
        NETMGR_LOG_E("fork exec parent process failed");
        return NETMANAGER_ERROR;
    }
    if (close(pipeFd[PIPE_IN]) != 0) {
        NETMGR_LOG_E("close failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
    }
    if (out != nullptr) {
        int32_t res = ReadFromChildProcess(pipeFd, childPid, out);
        if (res != NETMANAGER_SUCCESS) {
            close(pipeFd[PIPE_OUT]);
            return res;
        }
    }
    NETMGR_LOG_D("read done");
    if (close(pipeFd[PIPE_OUT]) != 0) {
        NETMGR_LOG_E("close failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
    }
    int status = 0;
    pid_t pidRet;
    int waitCount;
    for (waitCount = 0; waitCount < MAX_WAIT_PID_COUNT; waitCount++) {
        pidRet = waitpid(childPid, &status, WNOHANG);
        if (pidRet == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_FOR_PID_TIME_MS));
        } else {
            break;
        }
    }
    if (waitCount == MAX_WAIT_PID_COUNT) {
        NETMGR_LOG_E("waitpid[%{public}d] timeout", childPid);
#ifndef CROSS_PLATFORM
        std::string childStack;
        HiviewDFX::DfxGetKernelStack(childPid, childStack);
        NETMGR_LOG_E("child process stack %{public}s", childStack.c_str());
#endif
        return NETMANAGER_ERROR;
    }
    if (pidRet != childPid) {
        NETMGR_LOG_E("waitpid[%{public}d] failed, pidRet:%{public}d", childPid, pidRet);
        return NETMANAGER_ERROR;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        // child process abnormal exit
        NETMGR_LOG_E("child process abnormal exit, status:%{public}d", status);
    }
    NETMGR_LOG_I("waitpid %{public}d done", childPid);
    return NETMANAGER_SUCCESS;
}

int32_t ForkExec(const std::string &command, std::string *out)
{
    const std::vector<std::string> cmd = Split(command, CMD_SEP);
    std::vector<const char *> args = FormatCmd(cmd);
    int32_t pipeFd[PIPE_FD_NUM] = {0};
    if (pipe(pipeFd) < 0) {
        NETMGR_LOG_E("creat pipe failed, errorno:%{public}d, errormsg:%{public}s", errno, strerror(errno));
        return NETMANAGER_ERROR;
    }
    NETMGR_LOG_D("ForkExec");
    pid_t pid = fork();
    if (pid < 0) {
        return NETMANAGER_ERROR;
    }
    if (pid == 0) {
        ForkExecChildProcess(pipeFd, PIPE_FD_NUM, args);
        return NETMANAGER_SUCCESS;
    } else {
        NETMGR_LOG_I("ForkDone %{public}d", pid);
        return ForkExecParentProcess(pipeFd, PIPE_FD_NUM, pid, out);
    }
}

bool IsValidDomain(const std::string &domain)
{
    if (domain.empty()) {
        return false;
    }

    std::string pattern = HOST_DOMAIN_PATTERN_HEADER;
    pattern = std::accumulate(HOST_DOMAIN_TLDS.begin(), HOST_DOMAIN_TLDS.end(), pattern,
        [](const std::string &pattern, const std::string &tlds) { return pattern + tlds + TLDS_SPLIT_SYMBOL; });
    pattern = pattern.replace(pattern.size() - 1, 1, "") + HOST_DOMAIN_PATTERN_TAIL;
    std::regex reg(pattern);
    if (!std::regex_match(domain, reg)) {
        NETMGR_LOG_E("Domain regex match failed.");
        return false;
    }

    std::vector<std::string> parts = Split(domain, DOMAIN_DELIMITER);
    if (parts.size() < DOMAIN_VALID_MIN_PART_SIZE || parts.size() > DOMAIN_VALID_MAX_PART_SIZE) {
        NETMGR_LOG_E("The domain parts size:[%{public}d] is invalid", static_cast<int>(parts.size()));
        return false;
    }

    std::set<std::string> tldsList;
    for (const auto &item : parts) {
        if (std::find(HOST_DOMAIN_TLDS.begin(), HOST_DOMAIN_TLDS.end(), item) == HOST_DOMAIN_TLDS.end()) {
            continue;
        }
        if (tldsList.find(item) != tldsList.end()) {
            NETMGR_LOG_E("Domain has duplicate tlds:%{public}s", item.c_str());
            return false;
        }
        tldsList.insert(item);
    }
    return true;
}

bool WriteFile(const std::string &filePath, const std::string &fileContent)
{
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        NETMGR_LOG_E("write file=%{public}s fstream failed. err %{public}d %{public}s",
            filePath.c_str(), errno, strerror(errno));
        return false;
    }
    file << fileContent;
    file.close();
    return true;
}

bool HasInternetPermission()
{
    int testSock = socket(AF_INET, SOCK_STREAM, 0);
    if (testSock < 0 && errno == EPERM) {
        NETMGR_LOG_E("make tcp testSock failed errno is %{public}d %{public}s", errno, strerror(errno));
        return false;
    }
    if (testSock > 0) {
        close(testSock);
    }
    return true;
}

std::string Trim(const std::string &str)
{
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

bool IsUrlRegexValid(const std::string &regex)
{
    if (Trim(regex).empty()) {
        return false;
    }
    return regex_match(regex, std::regex("^[a-zA-Z0-9\\-_\\.*]+$"));
}

std::string InsertCharBefore(const std::string &input, const char from, const char preChar, const char nextChar)
{
    std::ostringstream output;
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == from && (i == input.size() - 1 || input[i + 1] != nextChar)) {
            output << preChar;
        }
        output << input[i];
    }
    return output.str();
}

std::string ReplaceCharacters(const std::string &input)
{
    std::string output = InsertCharBefore(input, '*', '.', '\0');
    output = InsertCharBefore(output, '.', '\\', '*');
    return output;
}

bool UrlRegexParse(const std::string &str, const std::string &patternStr)
{
    if (patternStr.empty()) {
        return false;
    }
    if (patternStr == "*") {
        return true;
    }
    if (!IsUrlRegexValid(patternStr)) {
        return patternStr == str;
    }
    std::regex pattern(ReplaceCharacters(patternStr));
    return !patternStr.empty() && std::regex_match(str, pattern);
}

uint64_t GenRandomNumber()
{
    static std::random_device rd;
    static std::uniform_int_distribution<uint64_t> dist(0ULL, UINT64_MAX);
    uint64_t num = dist(rd);
    return num;
}

bool IsSim(const std::string &bundleName)
{
    std::vector<std::string> list = Split(SIM_APP_BUNDLENAMES, BUNDLENAME_DELIMITER);
    auto findRet =
        std::find_if(list.begin(), list.end(), [&bundleName](const auto &item) { return item == bundleName; });
    return findRet != list.end();
}

bool IsInstallSourceFromSim(const std::string &installSource)
{
    return installSource == INSTALL_SOURCE_FROM_SIM;
}

std::string GetHostnameFromURL(const std::string &url)
{
    if (url.empty()) {
        return "";
    }
    std::string delimiter = "://";
    std::string tempUrl = url;
    std::replace(tempUrl.begin(), tempUrl.end(), '\\', '/');
    size_t posStart = tempUrl.find(delimiter);
    if (posStart != std::string::npos) {
        posStart += delimiter.length();
    } else {
        posStart = 0;
    }
    size_t notSlash = tempUrl.find_first_not_of('/', posStart);
    if (notSlash != std::string::npos) {
        posStart = notSlash;
    }
    size_t posEnd = std::min({ tempUrl.find(':', posStart),
                              tempUrl.find('/', posStart), tempUrl.find('?', posStart) });
    if (posEnd != std::string::npos) {
        return tempUrl.substr(posStart, posEnd - posStart);
    }
    return tempUrl.substr(posStart);
}

bool IsSim2(const std::string &bundleName)
{
    std::vector<std::string> list = Split(SIM2_BUNDLENAMES, BUNDLENAME_DELIMITER);
    auto findRet =
        std::find_if(list.begin(), list.end(), [&bundleName](const auto &item) { return item == bundleName; });
    return findRet != list.end();
}

bool IsInstallSourceFromSim2(const std::string &installSource)
{
    return installSource == INSTALL_SOURCE_FROM_SIM2;
}

bool IsNeedDisplayTrafficAncoList()
{
#ifndef CROSS_PLATFORM
    NETMGR_LOG_D("Is need display traffic anco list: %{public}s",
        system::GetParameter(DISPLAY_TRAFFIC_ANCO_LIST, "false").c_str());
    return system::GetParameter(DISPLAY_TRAFFIC_ANCO_LIST, "false") == "true";
#else
    return false;
#endif
}

bool IsSimAnco(const std::string &bundleName)
{
    return bundleName == INSTALL_SOURCE_FROM_SIM;
}

bool IsSim2Anco(const std::string &bundleName)
{
    return bundleName == INSTALL_SOURCE_FROM_SIM2;
}

std::string GetGatewayAddr(const std::string& ipAddr, const std::string& subnetMask)
{
    uint32_t ipIntAddr;
    if (!IpToInt(ipAddr, ipIntAddr)) {
        NETMGR_LOG_E("virNicAddr is not valid");
        return "";
    }

    uint32_t maskIntAddr;
    if (!IpToInt(subnetMask, maskIntAddr)) {
        NETMGR_LOG_E("subnetMask is not valid");
        return "";
    }

    uint32_t networkAddr = ipIntAddr & maskIntAddr;
    uint32_t gatewayAddr = networkAddr + 1;
    std::string gatewayStrAddr;
    if (!IpToString(gatewayAddr, gatewayStrAddr)) {
        return "";
    }
    return gatewayStrAddr;
}
 
bool IpToInt(const std::string& ipAddr, uint32_t &ipIntAddr)
{
    in_addr addr;
    if (inet_pton(AF_INET, ipAddr.c_str(), &addr) != INET_OPTION_SUC) {
        NETMGR_LOG_E("IpToInt failed for invalid IP address");
        return false;
    }

    ipIntAddr = ntohl(addr.s_addr);
    return true;
}

bool IpToString(uint32_t ipAddr, std::string &ipStrAddr)
{
    in_addr addr;
    addr.s_addr = htonl(ipAddr);
    char bufIp[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &addr, bufIp, sizeof(bufIp)) == nullptr) {
        NETMGR_LOG_E("IpToString conversion failed");
        return false;
    }

    ipStrAddr = std::string(bufIp);
    return true;
}

int32_t GetTodayMidnightTimestamp(int hour, int min, int sec)
{
    auto now = std::chrono::system_clock::now();
    std::time_t nowClock = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&nowClock);
    if (localTime == nullptr) {
        NETMGR_LOG_E("localTime is nullptr");
        return 0;
    }
    localTime->tm_hour = hour;
    localTime->tm_min = min;
    localTime->tm_sec = sec;

    std::time_t endOfDayTime = std::mktime(localTime);
    auto timeXth = std::chrono::system_clock::from_time_t(endOfDayTime);
    auto timestamp = std::chrono::system_clock::to_time_t(timeXth);

    return timestamp;
}

void DeleteFile(const std::string &filePath)
{
#ifndef CROSS_PLATFORM
    std::filesystem::path path = filePath;
    if (std::filesystem::exists(path)) {
        std::filesystem::remove(path);
        NETMGR_LOG_I("file deleted success.");
    } else {
        NETMGR_LOG_E("file does not exist.");
    }
#else
    NETMGR_LOG_E("CROSS_PLATFORM not support");
#endif
}

bool IsSameNaturalDay(uint32_t current, uint32_t another)
{
    std::time_t tt1 =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(std::chrono::seconds(current)));
    std::time_t tt2 =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(std::chrono::seconds(another)));
    struct std::tm tempTm1;
    struct std::tm tempTm2;
    std::tm *tm1 = std::localtime(&tt1);
    if (tm1 == nullptr) {
        return false;
    }
    tempTm1 = *tm1;
    std::tm *tm2 = std::localtime(&tt2);
    if (tm2 == nullptr) {
        return false;
    }
    tempTm2 = *tm2;
    return tempTm1.tm_year == tempTm2.tm_year &&
        tempTm1.tm_mon == tempTm2.tm_mon && tempTm1.tm_mday == tempTm2.tm_mday;
}

std::string ExtractDomainFormUrl(const std::string &url)
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

bool IsUsableGlobalIpv6Addr(const std::string &ipv6Addr)
{
    if (ipv6Addr.empty()) {
        return false;
    }

    struct in6_addr addr = IN6ADDR_ANY_INIT;
    if (inet_pton(AF_INET6, ipv6Addr.c_str(), &addr) != 1) {
        return false;
    }

    // Check if is unspecified address (::)
    if (memcmp(addr.s6_addr, in6addr_any.s6_addr, BYTE_16) == 0) {
        NETMGR_LOG_I("Reject unspecified address");
        return false;
    }

    if (memcmp(addr.s6_addr, IPV6_LOOPBACK_BYTES, BYTE_16) == 0) {
        NETMGR_LOG_I("Reject loopback address");
        return false;
    }

    // check if is document address (2001:db8::/32)
    const uint8_t docAddrPrefix[4] = {0x20, 0x01, 0x0d, 0xb8};
    if (memcmp(addr.s6_addr, docAddrPrefix, sizeof(docAddrPrefix)) == 0) {
        NETMGR_LOG_I("Reject documentation address");
        return false;
    }

    // check if is multicast address (ff00::/8)
    if (addr.s6_addr[0] == 0xFF) {
        NETMGR_LOG_I("Reject multicast address");
        return false;
    }

    // check if is link local address (fe80::/10)
    if (addr.s6_addr[0] == 0xFE && (addr.s6_addr[1] & 0xC0) == 0x80) {
        NETMGR_LOG_I("Reject link local address");
        return false;
    }

    // check if is unique local address/private address (fc00::/7)
    if ((addr.s6_addr[0] & 0xFE) == 0xFC) {
        NETMGR_LOG_I("Reject unique local address");
        return false;
    }

    // check if is site local address (fec0::/10) - deprecated but still filtered
    if (addr.s6_addr[0] == 0xFE && (addr.s6_addr[1] & 0xC0) == 0xC0) {
        NETMGR_LOG_I("Reject site local address");
        return false;
    }

    const uint8_t globalUnicastPrefix = 0x20;
    const uint8_t mask = 0xE0;
    
    if ((addr.s6_addr[0] & mask) == globalUnicastPrefix) {
        if (addr.s6_addr[ORCHID_BYTE_IDX0] == ORCHID_PREFIX_BYTE0 &&
            addr.s6_addr[ORCHID_BYTE_IDX1] == ORCHID_PREFIX_BYTE1 &&
            addr.s6_addr[ORCHID_BYTE_IDX2] == ORCHID_PREFIX_BYTE2 &&
            (addr.s6_addr[ORCHID_BYTE_IDX3] & ORCHID_BYTE3_MASK) == ORCHID_BYTE3_VALUE) {
            return false;
        }
        return true;
    }

    return false;
}

bool IsValidAddress(const std::string &ipStrAddr)
{
    if (IsValidIPV4(ipStrAddr)) {
        if (ipStrAddr != DEFAULT_IPV4_ANY_INIT_ADDR) {
            return true;
        }
    }
    if (IsValidIPV6(ipStrAddr)) {
        if (ipStrAddr != DEFAULT_IPV6_ANY_INIT_ADDR) {
            return true;
        }
    }
    return false;
}

bool IsIPv6LinkLocal(const std::string& ipv6Addr)
{
    if (ipv6Addr.empty()) {
        return false;
    }
    
    std::string pureAddr = ipv6Addr;
    size_t percentPos = pureAddr.find('%');
    if (percentPos != std::string::npos) {
        pureAddr = pureAddr.substr(0, percentPos);
    }
    
    struct in6_addr addr;
    if (inet_pton(AF_INET6, pureAddr.c_str(), &addr) != 1) {
        return false;
    }
    
    return (addr.s6_addr[0] == 0xfe) && ((addr.s6_addr[1] & 0xc0) == 0x80);
}

void RemoveDeleteControlFromPath(const char *path)
{
    char realPathBuffer[PATH_MAX];
    char* realPath = realpath(path, realPathBuffer);
    if (realPath == nullptr) {
        return;
    }
    int fd = open(realPath, O_RDONLY);
    if (fd > 0) {
        RemoveDeleteControlFlag(fd);
        close(fd);
    }
}

void RemoveDeleteControlFlag(int fd)
{
    if (fd <= 0) {
        return;
    }
    uint32_t flags = 0;
    int ret = ioctl(fd, HMFS_IOCTL_HW_GET_FLAGS, &flags);
    if (ret < 0) {
        return;
    }
    if (!(flags & HMFS_MONITOR_FL)) {
        return;
    }
    flags &= ~HMFS_MONITOR_FL;
    ioctl(fd, HMFS_IOCTL_HW_SET_FLAGS, &flags);
}

std::string SerializeStringVector(const std::vector<std::string>& vec)
{
    std::stringstream ss;
    ss << "\"";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i + 1 < vec.size()) ss << ",";
    }
    ss << "\"";
    return ss.str();
}
} // namespace OHOS::NetManagerStandard::CommonUtils

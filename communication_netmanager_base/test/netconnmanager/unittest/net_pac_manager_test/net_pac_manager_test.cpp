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

#include "mock_timer.h"
#include "net_pac_manager.h"
#include "pac_server.h"
#include <gtest/gtest.h>

constexpr int32_t TM_YEAR_BASE = 1900;
constexpr int32_t YEAR_1900 = 1900;
constexpr int32_t YEAR_1999 = 1999;
constexpr int32_t YEAR_2022 = 2022;
constexpr int32_t YEAR_2023 = 2023;
constexpr int32_t YEAR_2024 = 2024;
constexpr int32_t YEAR_2025 = 2025;
constexpr int32_t YEAR_2033 = 2033;

constexpr int32_t MONTH_JAN = 0;
constexpr int32_t MONTH_FEB = 1;
constexpr int32_t MONTH_MAY = 4;
constexpr int32_t MONTH_JUN = 5;
constexpr int32_t MONTH_JUL = 6;
constexpr int32_t MONTH_AUG = 7;
constexpr int32_t MONTH_NOV = 10;
constexpr int32_t MONTH_DEC = 11;

constexpr int32_t DAY_1 = 1;
constexpr int32_t DAY_2 = 2;
constexpr int32_t DAY_5 = 5;
constexpr int32_t DAY_11 = 11;
constexpr int32_t DAY_15 = 15;
constexpr int32_t DAY_16 = 16;
constexpr int32_t DAY_24 = 24;
constexpr int32_t DAY_25 = 25;
constexpr int32_t DAY_31 = 31;

constexpr int32_t HOUR_8 = 8;
constexpr int32_t HOUR_11 = 11;
constexpr int32_t HOUR_22 = 22;

constexpr int32_t WEEKDAY_SUN = 0;
constexpr int32_t WEEKDAY_MON = 1;
constexpr int32_t WEEKDAY_SAT = 6;

constexpr int32_t K_HOUR12 = 12;
constexpr int32_t K_HOUR9 = 9;
constexpr int32_t K_HOUR13 = 13;
constexpr int32_t K_HOUR8 = 8;
constexpr int32_t K_HOUR17 = 17;

constexpr int32_t K_MINUTE0 = 0;
constexpr int32_t K_MINUTE30 = 30;
constexpr int32_t K_MINUTE59 = 59;

constexpr int32_t K_SECOND0 = 0;
constexpr int32_t K_SECOND1 = 1;
constexpr int32_t K_SECOND59 = 59;

using namespace OHOS::NetManagerStandard;
std::string script =
    "// PAC (Proxy Auto-Configuration) 脚本示例\n"
    "// 包含所有主要PAC函数的使用示例，只演示基础函数使用。\n"
    "// 目前扩展辅助函数功能支持不全面，不建议使用。\n"
    "// isInNetEx和isInNet功能相同\n"
    "// myIpAddressEx返回所有本地ip\n"
    "// dnsResolveEx和dnsResolve相同，只返回一个地址\n"
    "// sortIpAddressList不支持\n"
    "\n"
    "function isIP(str) {\n"
    "    // 正则表达式检查是否为有效的 IP 地址\n"
    "    var ipPattern = "
    "/^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-"
    "9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;\n"
    "    return ipPattern.test(str);\n"
    "}\n"
    "\n"
    "function FindProxyForURL(url, host) {\n"
    "    // 本机地址直接访问\n"
    "    if (host === \"127.0.0.1\" || host === \"localhost\") {\n"
    "        return \"DIRECT\";\n"
    "    }\n"
    "    // 2. 检查本地域名\n"
    "    //本机域名直接访问\n"
    "    if (dnsDomainIs(host, \".local\") ||\n"
    "        dnsDomainIs(host, \".localhost\") ||\n"
    "        localHostOrDomainIs(host, \"localhost\")) {\n"
    "        return \"DIRECT\";\n"
    "    }\n"
    "    // 3. 检查内网地址\n"
    "    // 内网地址直接访问\n"
    "    var myIP = myIpAddress();\n"
    "    if (isInNet(host, \"192.168.0.0\", \"255.255.0.0\") ||\n"
    "        isInNet(host, \"10.0.0.0\", \"255.0.0.0\") ||\n"
    "        isInNet(host, \"172.16.0.0\", \"255.240.0.0\") ||\n"
    "        isInNet(myIP, \"192.168.0.0\", \"255.255.0.0\")) {\n"
    "        return \"DIRECT\";\n"
    "    }\n"
    "    // 5 ip地址使用代理\n"
    "    if (isIP(host)) {\n"
    "        return \"PROXY special-proxy.com:8000\";\n"
    "    }\n"
    "    var pattern=/baidu/g; \n"
    "    if(pattern.test(host)){\n"
    "        return \"PROXY special-proxy.com:7000\";\n"
    "    }\n"
    "    // 4. 检查纯主机使用代理\n"
    "    if (isPlainHostName(host)) {\n"
    "        return \"PROXY special-proxy.com:8001\";\n"
    "    }\n"
    "    // 4. 基于时间的规则\n"
    "    // 周一到周五的时间\n"
    "    if (weekdayRange(\"MON\", \"FRI\")) {\n"
    "        //九点到18点的时间\n"
    "        if (timeRange(9, 18)) {\n"
    "            if (dnsDomainIs(host, \".company.com\")) {\n"
    "                return \"PROXY special-proxy.com:8002\";\n"
    "            }\n"
    "        } else {\n"
    "            return \"PROXY special-proxy.com:8102\";\n"
    "        }\n"
    "    }\n"
    "    // 5. 基于日期的规则\n"
    "    // 1月到3月\n"
    "    if (dateRange(\"JAN\", \"MAR\")) {\n"
    "        // 域名判断\n"
    "        if (shExpMatch(host, \"*.special.com\")) {\n"
    "            return \"PROXY special-proxy.com:8003\";\n"
    "        }else{\n"
    "            return \"PROXY special-proxy.com:8103\";\n"
    "        }\n"
    "    }\n"
    "    // 6. 检查域名级别\n"
    "    if (dnsDomainLevels(host) >= 3) {\n"
    "        // 多级域名使用特定代理\n"
    "        return \"PROXY special-proxy.com:8004\";\n"
    "    }\n"
    "    // 7. 可解析性检查\n"
    "    if (isResolvable(host)) {\n"
    "        var resolvedIP = dnsResolve(host);\n"
    "        if (resolvedIP && isInNet(resolvedIP, \"116.205.4.0\", \"255.255.255.0\")) {\n"
    "            return \"PROXY special-proxy.com:8005\";\n"
    "        }\n"
    "    }\n"
    "    // 8. Shell表达式匹配\n"
    "    if (shExpMatch(url, \"http://download*.example.com/*\") ||\n"
    "        shExpMatch(url, \"https://*.cdn.com/*\")) {\n"
    "        return \"PROXY special-proxy.com:8006\";\n"
    "    }\n"
    "    // 扩展函数示例（部分浏览器支持）\n"
    "    try {\n"
    "        // 9. 扩展IP检查 返回所有的本地ip，myIpAddress只会返回127.0.0.1，\n"
    "        var allMyIPs = myIpAddressEx();\n"
    "        // isInNetEx和isInNet功能相同\n"
    "        if (allMyIPs && isInNetEx(host, allMyIPs)) {\n"
    "            return \"PROXY special-proxy.com:8007\";\n"
    "        }\n"
    "        // 10. 扩展DNS解析\n"
    "        //dnsResolveEx和dnsResolve相同，不支持返回多个域名的ip\n"
    "        var allIPs = dnsResolveEx(host);\n"
    "        if (allIPs) {\n"
    "            //sortIpAddressList函数不支持\n"
    "            var sortedIPs = sortIpAddressList(allIPs);\n"
    "            // 使用排序后的IP列表进行进一步处理\n"
    "        }\n"
    "        // 11. 扩展可解析性检查\n"
    "        //isResolvableEx和isResolvable相同\n"
    "        if (isResolvableEx(host)) {\n"
    "            // 进行额外的处理\n"
    "        }\n"
    "    } catch (e) {\n"
    "        // 扩展函数不支持时的降级处理\n"
    "    }\n"
    "    // 默认规则\n"
    "    return \"PROXY default-proxy.com:8080; DIRECT\";\n"
    "}";

#if 1
TEST(MyTests, weekdayRange)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    std::string script =
        "function FindProxyForURL(url, host) {\n"
        "    return weekdayRange(\"MON\", \"FRI\");\n"
        "}";
    bool status = manager->InitPACScript(script);
    EXPECT_EQ(status, true);
    std::string proxy;
    std::string url;
    std::string host;
    PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    int32_t dayOfWeek = timeinfo->tm_wday;
    if (dayOfWeek == 0 || dayOfWeek == 6) {
        EXPECT_EQ(proxy, "false");
    } else {
        EXPECT_EQ(proxy, "true");
    }
}

static void Test1()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_11;
        GetDefaultGmtime()->tm_mday = DAY_11;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
}

static void Test2()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2022 - TM_YEAR_BASE;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
}

static void Test3()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"GMT\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_2;
        GetDefaultGmtime()->tm_mday = DAY_2;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"GMT\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
}

static void Test4()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_24;
        GetDefaultTmLocalTime()->tm_mon = MONTH_DEC;
        GetDefaultGmtime()->tm_mday = DAY_24;
        GetDefaultGmtime()->tm_mon = MONTH_DEC;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(24, \"DEC\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_25;
        GetDefaultTmLocalTime()->tm_mon = MONTH_DEC;
        GetDefaultGmtime()->tm_mday = DAY_25;
        GetDefaultGmtime()->tm_mon = MONTH_DEC;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(24, \"DEC\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
}

static void Test5()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_25;
        GetDefaultGmtime()->tm_mday = DAY_25;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, 15);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_5;
        GetDefaultGmtime()->tm_mday = DAY_5;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, 15);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

static void Test6()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_1999 - TM_YEAR_BASE;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(2020, 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2023 - YEAR_1900;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(2020, 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

static void Test7()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2033 - YEAR_1900;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(2020, 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mon = MONTH_FEB;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(\"JAN\", \"MAR\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

static void Test8()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mon = MONTH_DEC;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(\"JAN\", \"MAR\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }

    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUN;
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_mon = MONTH_JUN;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 15, \"AUG\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

static void Test9()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mon = MONTH_MAY;
        GetDefaultTmLocalTime()->tm_mday = DAY_31;
        GetDefaultGmtime()->tm_mon = MONTH_MAY;
        GetDefaultGmtime()->tm_mday = DAY_31;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 15, \"AUG\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUL;
        GetDefaultTmLocalTime()->tm_mday = DAY_15;
        GetDefaultGmtime()->tm_mon = MONTH_JUL;
        GetDefaultGmtime()->tm_mday = DAY_15;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 15, \"AUG\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

static void Test10()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mon = MONTH_AUG;
        GetDefaultTmLocalTime()->tm_mday = DAY_15;
        GetDefaultGmtime()->tm_mon = MONTH_AUG;
        GetDefaultGmtime()->tm_mday = DAY_15;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 15, \"AUG\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mon = MONTH_AUG;
        GetDefaultTmLocalTime()->tm_mday = DAY_16;
        GetDefaultGmtime()->tm_mon = MONTH_AUG;
        GetDefaultGmtime()->tm_mday = DAY_16;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 15, \"AUG\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
}

static void Test11()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultTmLocalTime()->tm_mon = MONTH_MAY;
        GetDefaultTmLocalTime()->tm_mday = DAY_31;
        GetDefaultGmtime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultGmtime()->tm_mon = MONTH_MAY;
        GetDefaultGmtime()->tm_mday = DAY_31;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 2025, 15, \"AUG\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUN;
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultGmtime()->tm_mon = MONTH_JUN;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 2025, 15, \"AUG\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

static void Test12()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUL;
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultGmtime()->tm_mon = MONTH_JUL;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 2025, 15, \"AUG\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultTmLocalTime()->tm_mon = MONTH_AUG;
        GetDefaultTmLocalTime()->tm_mday = DAY_15;
        GetDefaultGmtime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultGmtime()->tm_mon = MONTH_AUG;
        GetDefaultGmtime()->tm_mday = DAY_15;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 2025, 15, \"AUG\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

static void Test13()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultTmLocalTime()->tm_mon = MONTH_AUG;
        GetDefaultTmLocalTime()->tm_mday = DAY_16;
        GetDefaultGmtime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultGmtime()->tm_mon = MONTH_AUG;
        GetDefaultGmtime()->tm_mday = DAY_16;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(1, \"JUN\", 2025, 15, \"AUG\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }

    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2024 - YEAR_1900;
        GetDefaultTmLocalTime()->tm_mon = MONTH_NOV;
        GetDefaultTmLocalTime()->tm_mday = DAY_31;
        GetDefaultGmtime()->tm_year = YEAR_2024 - YEAR_1900;
        GetDefaultGmtime()->tm_mon = MONTH_NOV;
        GetDefaultGmtime()->tm_mday = DAY_31;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(\"DEC\", 2024, \"JAN\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
}

static void Test14()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2024 - YEAR_1900;
        GetDefaultTmLocalTime()->tm_mon = MONTH_DEC;
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_year = YEAR_2024 - YEAR_1900;
        GetDefaultGmtime()->tm_mon = MONTH_DEC;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(\"DEC\", 2024, \"JAN\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }

    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultTmLocalTime()->tm_mon = MONTH_JAN;
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultGmtime()->tm_mon = MONTH_JAN;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(\"DEC\", 2024, \"JAN\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

static void Test15()
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultTmLocalTime()->tm_mon = MONTH_JAN;
        GetDefaultTmLocalTime()->tm_mday = DAY_31;
        GetDefaultGmtime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultGmtime()->tm_mon = MONTH_JAN;
        GetDefaultGmtime()->tm_mday = DAY_31;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(\"DEC\", 2024, \"JAN\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }

    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultTmLocalTime()->tm_mon = MONTH_FEB;
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_year = YEAR_2025 - TM_YEAR_BASE;
        GetDefaultGmtime()->tm_mon = MONTH_FEB;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    return dateRange(\"DEC\", 2024, \"JAN\", 2025);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
}

TEST(MyTests, dateRange)
{
    EnableTimeMock();
    Test1();
    Test2();
    Test3();
    Test4();
    Test5();
    Test6();
    Test7();
    Test8();
    Test9();
    Test10();
    Test11();
    Test12();
    Test13();
    Test14();
    Test15();
    SetEnableMock(false);
}

void SetupTestTime(int32_t hour, int32_t minute, int32_t second = 0)
{
    GetDefaultTmLocalTime()->tm_hour = hour;
    GetDefaultTmLocalTime()->tm_min = minute;
    GetDefaultTmLocalTime()->tm_sec = second;
    GetDefaultGmtime()->tm_hour = hour;
    GetDefaultGmtime()->tm_min = minute;
    GetDefaultGmtime()->tm_sec = second;
}

std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> CreatePACManager(const std::string &script)
{
    auto manager = std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    bool status = manager->InitPACScript(script);
    EXPECT_EQ(status, true);
    return manager;
}

void VerifyPACResult(const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> &manager,
                     const std::string &expectedResult)
{
    std::string proxy;
    std::string url;
    std::string host;
    PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
    EXPECT_EQ(ret, PAC_OK);
    EXPECT_EQ(proxy, expectedResult);
}

void TestSingleHourTimeRange()
{
    SetupTestTime(K_HOUR12 - 1, K_MINUTE59, K_SECOND0);
    auto manager = CreatePACManager(
        "function FindProxyForURL(url, host) {\n"
        "    return timeRange(12);\n"
        "}");
    VerifyPACResult(manager, "false");

    SetupTestTime(K_HOUR12, K_MINUTE0, K_SECOND0);
    VerifyPACResult(manager, "true");

    SetupTestTime(K_HOUR12 + 1, K_MINUTE0, K_SECOND0);
    VerifyPACResult(manager, "true");

    SetupTestTime(K_HOUR12 + 1, K_MINUTE0 + 1, K_SECOND0);
    VerifyPACResult(manager, "false");
}

void TestHourRangeTimeRange()
{
    SetupTestTime(K_HOUR9 - 1, K_MINUTE59, K_SECOND0);
    auto manager = CreatePACManager(
        "function FindProxyForURL(url, host) {\n"
        "    return timeRange(9, 13);\n"
        "}");
    VerifyPACResult(manager, "false");

    SetupTestTime(K_HOUR9, K_MINUTE0, K_SECOND0);
    VerifyPACResult(manager, "true");

    SetupTestTime(K_HOUR13, K_MINUTE0, K_SECOND0);
    VerifyPACResult(manager, "true");

    SetupTestTime(K_HOUR13, K_MINUTE0, K_SECOND0 + 1);
    VerifyPACResult(manager, "false");
}

void TestHourMinuteRangeTimeRange()
{
    SetupTestTime(K_HOUR8, K_MINUTE30 - 1, K_SECOND59);
    auto manager = CreatePACManager(
        "function FindProxyForURL(url, host) {\n"
        "   return timeRange(8, 30, 17, 0);\n"
        "}");
    VerifyPACResult(manager, "false");

    SetupTestTime(K_HOUR8, K_MINUTE30, K_SECOND0);
    VerifyPACResult(manager, "true");

    SetupTestTime(K_HOUR9, K_MINUTE30, K_SECOND0);
    VerifyPACResult(manager, "true");
}

void TestHourMinuteRangeTimeRangeContinued()
{
    SetupTestTime(K_HOUR17, K_MINUTE0, K_SECOND0);
    auto manager = CreatePACManager(
        "function FindProxyForURL(url, host) {\n"
        "   return timeRange(8, 30, 17, 0);\n"
        "}");
    VerifyPACResult(manager, "true");

    SetupTestTime(K_HOUR17, K_MINUTE0, K_SECOND0 + 1);
    VerifyPACResult(manager, "false");
}

TEST(MyTests, timerange)
{
    EnableTimeMock();
    TestSingleHourTimeRange();
    TestHourRangeTimeRange();
    TestHourMinuteRangeTimeRange();
    TestHourMinuteRangeTimeRangeContinued();
    SetEnableMock(false);
}

TEST(MyTests, test)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    {
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    var str=\"baidu.com\";\n"
            "    var patt=/baidu/g;\n"
            "    return patt.test(str);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
    {
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "    var str=\"huaw1ei.com\";\n"
            "    var patt=/baidu/g;\n"
            "    return patt.test(str);\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "false");
    }
}

TEST(MyTests, FindProxyForURL_InitAndBasic)
{
    std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    int32_t ret = manager->InitPACScriptWithURL("http://localhost:8888/");
    {
        std::string url = "http://127.0.0.1/index";
        std::string host = "127.0.0.1";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_SCRIPT_DOWNLOAD_ERROR);
        EXPECT_EQ(proxy, "");
    }

    StartHttpServer();
    sleep(6);
    {
        std::string url = "http://127.0.0.1/index";
        std::string host = "127.0.0.1";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }

    std::string url = "https://www.jacjsjsjsj.company.com/aaal";
    std::string host = manager->ParseHost(url);
    EXPECT_EQ(host, "www.jacjsjsjsj.company.com");
    bool initRet = manager->InitPACScriptWithURL("https://gitee.com/test.pac");
    EXPECT_EQ(initRet, false);
}

void TestLocalNetworks(const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> &manager)
{
    {
        std::string url = "http://127.0.0.1/index";
        std::string host = "127.0.0.1";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
    {
        std::string url = "http://test.local/index";
        std::string host = "test.local";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
    {
        std::string url = "http://test.localhost/index";
        std::string host = "test.localhost";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
}

void TestPrivateNetworks(const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> &manager)
{
    {
        std::string url = "http://192.168.0.111/index";
        std::string host = "192.168.0.111";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
    {
        std::string url = "http://192.168.111.111/index";
        std::string host = "192.168.111.111";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
}

TEST(MyTests, FindProxyForURL_LocalAndPrivateNetworks)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    bool ret = manager->InitPACScript(script);
    EXPECT_EQ(ret, true);
    TestLocalNetworks(manager);
    TestPrivateNetworks(manager);
}

void TestMorePrivateNetworks(const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> &manager)
{
    {
        std::string url = "http://10.0.0.1/index";
        std::string host = "10.0.0.1";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
    {
        std::string url = "http://10.1.1.1/index";
        std::string host = "10.1.1.1";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
    {
        std::string url = "http://172.16.0.1/index";
        std::string host = "172.16.0.1";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
    {
        std::string url = "http://192.168.0.11/index";
        std::string host = "192.168.0.11";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "DIRECT");
    }
}

TEST(MyTests, FindProxyForURL_MorePrivateNetworks)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    bool ret = manager->InitPACScript(script);
    EXPECT_EQ(ret, true);
    TestMorePrivateNetworks(manager);
}

TEST(MyTests, FindProxyForURL_NonPrivateNetworks)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    bool ret = manager->InitPACScript(script);
    EXPECT_EQ(ret, true);

    {
        std::string url = "http://191.168.0.11/index";
        std::string host = "191.168.0.11";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8000");
    }
    {
        std::string url = "http://hostname/index";
        std::string host = "hostname";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8001");
    }
    {
        std::string url = "https://app.baidu.com/";
        std::string host = "app.baidu.com";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:7000");
    }
}

void TestTimeBasedRules1(const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> &manager)
{
    std::string url = "https://www.jacjsjsjsj.company.com/aaal";
    std::string host = "www.jacjsjsjsj.company.com";
    std::string proxy;

    {
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_MON;
        GetDefaultTmLocalTime()->tm_hour = HOUR_11;
        GetDefaultGmtime()->tm_wday = WEEKDAY_MON;
        GetDefaultGmtime()->tm_hour = HOUR_11;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8002");
    }
    {
        proxy.clear();
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_MON;
        GetDefaultTmLocalTime()->tm_hour = HOUR_8;
        GetDefaultGmtime()->tm_wday = WEEKDAY_MON;
        GetDefaultGmtime()->tm_hour = HOUR_8;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8102");
    }
    {
        proxy.clear();
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_MON;
        GetDefaultTmLocalTime()->tm_hour = HOUR_22;
        GetDefaultGmtime()->tm_wday = WEEKDAY_MON;
        GetDefaultGmtime()->tm_hour = HOUR_22;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8102");
    }
}

TEST(MyTests, FindProxyForURL_TimeBasedRules1)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    bool ret = manager->InitPACScript(script);
    EXPECT_EQ(ret, true);
    EnableTimeMock();
    TestTimeBasedRules1(manager);
    SetEnableMock(false);
}

void TestTimeBasedRules2(const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> &manager)
{
    std::string proxy;
    std::string url;
    std::string host;

    {
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_SUN;
        GetDefaultTmLocalTime()->tm_mon = MONTH_FEB;
        GetDefaultTmLocalTime()->tm_hour = HOUR_22;
        GetDefaultGmtime()->tm_wday = WEEKDAY_SUN;
        GetDefaultTmLocalTime()->tm_mon = MONTH_FEB;
        GetDefaultGmtime()->tm_hour = HOUR_22;
        host = "example.com";
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8103");
    }
    {
        proxy.clear();
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_SUN;
        GetDefaultTmLocalTime()->tm_mon = MONTH_FEB;
        GetDefaultTmLocalTime()->tm_hour = HOUR_22;
        GetDefaultGmtime()->tm_wday = WEEKDAY_SUN;
        GetDefaultTmLocalTime()->tm_mon = MONTH_FEB;
        GetDefaultGmtime()->tm_hour = HOUR_22;
        url = "https://www.jacjsjsjsj.special.com/aaal";
        host = "www.jacjsjsjsj.special.com";
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8003");
    }
}

TEST(MyTests, FindProxyForURL_TimeBasedRules2)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    bool ret = manager->InitPACScript(script);
    EXPECT_EQ(ret, true);
    EnableTimeMock();
    TestTimeBasedRules2(manager);
    SetEnableMock(false);
}

TEST(MyTests, FindProxyForURL_DomainBasedRules)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    bool ret = manager->InitPACScript(script);
    EXPECT_EQ(ret, true);

    std::string proxy;
    EnableTimeMock();

    {
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUL;
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_SAT;
        GetDefaultGmtime()->tm_mon = MONTH_JUL;
        GetDefaultGmtime()->tm_wday = WEEKDAY_SAT;
        std::string url = "https://www.chinasofti.com/";
        std::string host = "www.chinasofti.com";
        std::string proxy;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ((proxy == "PROXY default-proxy.com:8080; DIRECT") || (proxy == "PROXY special-proxy.com:8005"), true);
    }
    {
        std::string proxy;
        std::string url = "http://www.a.b.c.d/index";
        std::string host = "www.a.b.c.d";
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUL;
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_SAT;
        GetDefaultTmLocalTime()->tm_hour = HOUR_11;
        GetDefaultGmtime()->tm_mon = MONTH_JUL;
        GetDefaultGmtime()->tm_wday = WEEKDAY_SAT;
        GetDefaultGmtime()->tm_hour = HOUR_11;
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8004");
    }
    SetEnableMock(false);
}

TEST(MyTests, FindProxyForURL_PatternMatching)
{
    const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
        std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
    bool ret = manager->InitPACScript(script);
    EXPECT_EQ(ret, true);

    std::string proxy;
    EnableTimeMock();

    {
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_SUN;
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUL;
        GetDefaultTmLocalTime()->tm_hour = HOUR_22;
        GetDefaultGmtime()->tm_wday = WEEKDAY_SUN;
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUL;
        GetDefaultGmtime()->tm_hour = HOUR_22;
        std::string url = "http://download1.example.com/test.xml";
        std::string host = "download1.example.com";
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY special-proxy.com:8006");
    }
    {
        proxy.clear();
        GetDefaultTmLocalTime()->tm_wday = WEEKDAY_SUN;
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUL;
        GetDefaultTmLocalTime()->tm_hour = HOUR_22;
        GetDefaultGmtime()->tm_wday = WEEKDAY_SUN;
        GetDefaultTmLocalTime()->tm_mon = MONTH_JUL;
        GetDefaultGmtime()->tm_hour = HOUR_22;
        std::string url = "http://a.b.com/test.xml";
        std::string host = "a.b.com";
        PAC_STATUS status = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(status, PAC_OK);
        EXPECT_EQ(proxy, "PROXY default-proxy.com:8080; DIRECT");
    }

    SetEnableMock(false);
}

TEST(MyTests, startsWith)
{
    {
        const std::shared_ptr<OHOS::NetManagerStandard::NetPACManager> manager =
            std::make_shared<OHOS::NetManagerStandard::NetPACManager>();
        GetDefaultTmLocalTime()->tm_mday = DAY_1;
        GetDefaultGmtime()->tm_mday = DAY_1;
        std::string script =
            "function FindProxyForURL(url, host) {\n"
            "   var str = \"Hello world, welcome to the Runoob.\"; \n"
            "    return str.startsWith(\"Hello\");\n"
            "}";
        bool status = manager->InitPACScript(script);
        EXPECT_EQ(status, true);
        std::string proxy;
        std::string url;
        std::string host;
        PAC_STATUS ret = manager->FindProxyForURL(url, host, proxy);
        EXPECT_EQ(ret, PAC_OK);
        EXPECT_EQ(proxy, "true");
    }
}

TEST(PacHelperTest, isPlainHostName)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return isPlainHostName(host) ? "PLAIN" : "NOT_PLAIN";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "localhost", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "PLAIN");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "www.example.com", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "NOT_PLAIN");
}

TEST(PacHelperTest, dnsDomainIs)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return dnsDomainIs(host, ".example.com") ? "MATCH" : "NO_MATCH";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "www.example.com", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "MATCH");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "example.org", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "NO_MATCH");
}

TEST(PacHelperTest, localHostOrDomainIs)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return localHostOrDomainIs(host, "internal.site") ? "LOCAL" : "EXTERNAL";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "internal.site", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "LOCAL");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "internal", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "LOCAL");

    std::string proxy3;
    EXPECT_EQ(manager->FindProxyForURL("", "external.site", proxy3), PAC_OK);
    EXPECT_EQ(proxy3, "EXTERNAL");
}

TEST(PacHelperTest, shExpMatch)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return shExpMatch(url, "*/download/*") ? "DOWNLOAD" : "REGULAR";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("http://example.com/download/file", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "DOWNLOAD");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("http://example.com/docs/index", "", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "REGULAR");
}

TEST(PacHelperTest, dnsResolve)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return dnsResolve(host) || "UNRESOLVED";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "localhost", proxy1), PAC_OK);
    EXPECT_TRUE(proxy1 != "UNRESOLVED");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "invalid.abcdefgh", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "UNRESOLVED");
}

TEST(PacHelperTest, myIpAddress)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return myIpAddress();
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy), PAC_OK);
    EXPECT_EQ(proxy, "127.0.0.1");
}

TEST(PacHelperTest, timeRange)
{
    EnableTimeMock();
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return timeRange(9, 18) ? "WORK_HOURS" : "AFTER_HOURS";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    GetDefaultTmLocalTime()->tm_hour = 10;
    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "WORK_HOURS");

    GetDefaultTmLocalTime()->tm_hour = 20;
    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "AFTER_HOURS");
}

TEST(PacHelperTest, dnsDomainLevels)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return dnsDomainLevels(host) >= 2 ? "MULTI_LEVEL" : "SIMPLE";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "sub.domain.com", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "MULTI_LEVEL");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "localhost", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "SIMPLE");
}

TEST(PacHelperTest, isResolvable)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return isResolvable(host) ? "RESOLVABLE" : "UNRESOLVABLE";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "localhost", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "RESOLVABLE");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "invalid.abcdefgh", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "UNRESOLVABLE");
}

TEST(PacHelperTest, isInNet)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return isInNet(host, "192.168.0.0", "255.255.255.0") ? "IN_NET" : "NOT_IN_NET";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "192.168.0.10", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "IN_NET");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "10.0.0.1", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "NOT_IN_NET");
}

TEST(PacHelperTest, isPlainHostName_Boundary)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return isPlainHostName(host) ? "PLAIN" : "NOT_PLAIN";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "PLAIN");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", ".", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "NOT_PLAIN");

    std::string proxy3;
    EXPECT_EQ(manager->FindProxyForURL("", "example.", proxy3), PAC_OK);
    EXPECT_EQ(proxy3, "NOT_PLAIN");
}

TEST(PacHelperTest, dnsDomainIs_Boundary)
{
    const auto manager = std::make_shared<NetPACManager>();

    std::string script1 = R"(
        function FindProxyForURL(url, host) {
            return dnsDomainIs(host, "") ? "MATCH" : "NO_MATCH";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script1));
    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "www.example.com", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "NO_MATCH");

    std::string script2 = R"(
        function FindProxyForURL(url, host) {
            return dnsDomainIs(host, ".com") ? "MATCH" : "NO_MATCH";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script2));
    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "com", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "MATCH");
}

TEST(PacHelperTest, localHostOrDomainIs_Boundary)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return localHostOrDomainIs(host, "example.com") ? "LOCAL" : "EXTERNAL";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "EXTERNAL");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "sub.example.com", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "EXTERNAL");

    std::string proxy3;
    EXPECT_EQ(manager->FindProxyForURL("", "example.co", proxy3), PAC_OK);
    EXPECT_EQ(proxy3, "EXTERNAL");
}

TEST(PacHelperTest, shExpMatch_Boundary)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script1 = R"(
        function FindProxyForURL(url, host) {
            return shExpMatch(url, "") ? "MATCH" : "NO_MATCH";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script1));
    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "MATCH");

    std::string script2 = R"(
        function FindProxyForURL(url, host) {
            return shExpMatch(url, "http://example.*/path") ? "MATCH" : "NO_MATCH";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script2));
    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("http://example.com/path", "", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "MATCH");

    std::string script3 = R"(
        function FindProxyForURL(url, host) {
            return shExpMatch(url, "*\?*") ? "MATCH" : "NO_MATCH";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script3));
    std::string proxy3;
    EXPECT_EQ(manager->FindProxyForURL("http://example.com/search?q=test", "", proxy3), PAC_OK);
    EXPECT_EQ(proxy3, "MATCH");
}

TEST(PacHelperTest, weekdayRange_Boundary)
{
    EnableTimeMock();
    const auto manager = std::make_shared<NetPACManager>();

    std::string script1 = R"(
        function FindProxyForURL(url, host) {
            return weekdayRange("XXX", "YYY") ? "INVALID" : "VALID";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script1));
    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "VALID");

    std::string script2 = R"(
        function FindProxyForURL(url, host) {
            return weekdayRange("FRI", "MON") ? "WEEKEND" : "WEEKDAY";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script2));

    GetDefaultTmLocalTime()->tm_wday = 5;
    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "WEEKEND");

    GetDefaultTmLocalTime()->tm_wday = 1;
    std::string proxy3;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy3), PAC_OK);
    EXPECT_EQ(proxy3, "WEEKEND");
}

TEST(PacHelperTest, dateRange_Boundary)
{
    EnableTimeMock();
    const auto manager = std::make_shared<NetPACManager>();

    std::string script1 = R"(
        function FindProxyForURL(url, host) {
            return dateRange(29, "FEB", 2024) ? "LEAP" : "NOT_LEAP";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script1));

    GetDefaultTmLocalTime()->tm_year = 124;
    GetDefaultTmLocalTime()->tm_mon = 1;
    GetDefaultTmLocalTime()->tm_mday = 29;
    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "LEAP");

    std::string script2 = R"(
        function FindProxyForURL(url, host) {
            return dateRange(1, "XXX") ? "INVALID" : "VALID";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script2));
    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "VALID");
}

TEST(PacHelperTest, timeRange_Boundary)
{
    EnableTimeMock();
    const auto manager = std::make_shared<NetPACManager>();

    std::string script1 = R"(
        function FindProxyForURL(url, host) {
            return timeRange(23, 0, 1, 0) ? "NIGHT" : "DAY";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script1));

    GetDefaultTmLocalTime()->tm_hour = 23;
    GetDefaultTmLocalTime()->tm_min = 30;
    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "NIGHT");

    GetDefaultTmLocalTime()->tm_hour = 0;
    GetDefaultTmLocalTime()->tm_min = 30;
    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "NIGHT");

    std::string script2 = R"(
        function FindProxyForURL(url, host) {
            return timeRange(25, 70) ? "INVALID" : "VALID";
        }
    )";
    ASSERT_TRUE(manager->InitPACScript(script2));
    std::string proxy3;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy3), PAC_OK);
    EXPECT_EQ(proxy3, "VALID");
}

TEST(PacHelperTest, dnsDomainLevels_Boundary)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return dnsDomainLevels(host) > 1 ? "HIGH" : "LOW";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "LOW");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "com", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "LOW");

    std::string proxy3;
    EXPECT_EQ(manager->FindProxyForURL("", "sub.domain.example.com", proxy3), PAC_OK);
    EXPECT_EQ(proxy3, "HIGH");
}

TEST(PacHelperTest, isInNet_Boundary)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return isInNet(host, "192.168.0.0", "255.255.0.0") ? "IN_NET" : "NOT_IN_NET";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "192.168.255.255", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "IN_NET");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "192.169.0.1", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "NOT_IN_NET");

    std::string proxy3;
    EXPECT_EQ(manager->FindProxyForURL("", "invalid-ip", proxy3), PAC_OK);
    EXPECT_EQ(proxy3, "NOT_IN_NET");
}

TEST(PacHelperTest, isResolvable_Boundary)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            return isResolvable(host) ? "RESOLVABLE" : "UNRESOLVABLE";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string longHost(1024, 'x');
    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", longHost, proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "UNRESOLVABLE");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "!@#$%^&*()", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "UNRESOLVABLE");
}

TEST(PacHelperTest, dnsResolve_Boundary)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            var ip = dnsResolve(host);
            return ip ? ip : "UNRESOLVED";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy1;
    EXPECT_EQ(manager->FindProxyForURL("", "", proxy1), PAC_OK);
    EXPECT_EQ(proxy1, "UNRESOLVED");

    std::string proxy2;
    EXPECT_EQ(manager->FindProxyForURL("", "localhost", proxy2), PAC_OK);
    EXPECT_EQ(proxy2, "127.0.0.1");
}

TEST(PacHelperTest, ExtendedFunctions)
{
    const auto manager = std::make_shared<NetPACManager>();
    std::string script = R"(
        function FindProxyForURL(url, host) {
            var allIPs = myIpAddressEx();
            var hasMultipleIPs = allIPs.split(';').length > 1;

            var resolvedIPs = dnsResolveEx(host);
            var hasMultipleResolved = resolvedIPs && resolvedIPs.split(';').length > 1;

            return (hasMultipleIPs && hasMultipleResolved) ? "EXTENDED" : "BASIC";
        }
    )";

    ASSERT_TRUE(manager->InitPACScript(script));

    std::string proxy;
    EXPECT_EQ(manager->FindProxyForURL("", "example.com", proxy), PAC_OK);
    EXPECT_TRUE(proxy == "EXTENDED" || proxy == "BASIC");
}
#endif

# OpenHarmony 简化版 Watch 网络管理架构深度分析

## 1. 范围与结论摘要

本报告分析仓库中的 `communication_netmanager_base`、`communication_netmanager_ext`
和 `communication_netstack`，目标是形成面向资源受限、以单一 Wi-Fi 接入为主的
OpenHarmony watch 产品裁剪依据。

核心结论如下：

1. 最小联网闭环由 `NetConnService`（SA 1151）、`NetsysNativeService`
   （SA 1158）、`netsys_controller`、DNS、DHCP、接口/路由管理和 fwmark 构成。
2. Ethernet、Sharing、mDNS、VPN、VPN Extension、防火墙、网络切片和穿戴分布式网络
   已具有顶层构建开关；其中 Ethernet、Sharing、mDNS、VPN 和 VPN Extension 默认开启，
   watch 产品必须显式关闭。
3. Policy、Stats 与 Netsys 内部管理器仍是粗粒度目标。连接服务静态依赖 policy/stats
   InnerKit，Netsys 无条件编入 sharing、VPN、MPTCP、CLAT 等实现，不能仅靠现有开关
   获得真正的最小闭包。
4. 第一阶段应只做产品配置和构建依赖裁剪；第二阶段再拆分 Policy/Stats/Netsys，
   以避免一次性改变 IPC ABI 和联网主链路。
5. `bundle.json` 中的 ROM/RAM 是声明预算而非实测结果，所有收益必须在目标硬件上通过
   二进制、PSS、线程、FD、时延和功耗数据验收。

## 2. 仓库结构、清单与源码入口

### 2.1 顶层目录

| 路径 | 角色 |
|---|---|
| `communication_netmanager_base/` | 连接、策略、统计和 Netsys 原生网络控制 |
| `communication_netmanager_ext/` | Ethernet、Sharing、mDNS、VPN、防火墙、网络切片、穿戴分布式网络 |
| `communication_netstack/` | HTTP、Socket、TLS、WebSocket 等应用侧协议能力 |
| `docs/` | 架构与裁剪研究文档 |

三个部件均通过 `bundle.json` 声明部件元数据、SysCap、feature、依赖和构建组。
`netmanager_base` 声明 ROM 4.5 MB、RAM 10 MB，`netmanager_ext` 声明 ROM 2 MB、
RAM 500 KB，`netstack` 声明 ROM 3 MB、RAM 5 MB；这些字段不能视为当前产品实测值
（`communication_netmanager_base/bundle.json:42-89`，
`communication_netmanager_ext/bundle.json:45-91`，
`communication_netstack/bundle.json:32-73`）。

### 2.2 netmanager_base

| 顶层目录/文件 | 代表入口与职责 |
|---|---|
| `README.md`, `README_zh.md` | 公开架构和 API 概览 |
| `bundle.json` | `NetManager.Core`、部件 features、依赖和打包目标 |
| `netmanager_base_config.gni` | 15 个核心 feature args、优化 flags 和源码路径变量 |
| `services/BUILD.gn` | `netmanager_base_services` 聚合入口 |
| `services/netconnmanager/` | `NetConnService`、`NetSupplier`、`Network`、探测和代理 |
| `services/netmanagernative/` | `NetsysNativeService`、DNS、路由、iptables、netlink、BPF |
| `services/netpolicymanager/` | UID 策略、配额、节电与 idle 防火墙 |
| `services/netstatsmanager/` | 流量采集、缓存、历史和 SQLite 持久化 |
| `services/netsyscontroller/` | 上层服务到 Netsys 的控制器/客户端适配层 |
| `interfaces/innerkits/` | Binder 接口、Proxy/Stub 和跨部件 SDK |
| `interfaces/kits/` | C/应用侧公开接口 |
| `frameworks/native/` | C++ 客户端实现 |
| `frameworks/js/` | connection、network、statistics、policy NAPI |
| `frameworks/ets/` | ANI/ETS 桥接；部分由 Rust 实现 |
| `common/ani_rs*` | ANI Rust 公共运行时与宏 |
| `bpf/` | `464xlat.c`、`net_permission.c`、`netstats.c`、`offload.c` |
| `sa_profile/` | SA 1151–1158 描述 |
| `services/etc/init/` | netmanager/netsysnative init、DNS/探测配置 |
| `test/` | 单测、fuzz、安全和 mock |

`services/BUILD.gn:19-28` 无条件将 `netsys_native_manager`、`net_conn_manager`、
`net_policy_manager` 和 `net_stats_manager` 加入 `netmanager_base_services`
（FPGA 模式除外），这是 base 无法按能力独立裁剪的第一层证据。

### 2.3 netmanager_ext

| 顶层目录/文件 | 代表入口与职责 |
|---|---|
| `BUILD.gn` | 按 Ethernet、Sharing、mDNS、VPN、Firewall、NetworkSlice 等分组 |
| `netmanager_ext_config.gni` | 扩展能力开关和 Wi-Fi/Bluetooth/USB/Battery 部件探测 |
| `services/ethernetmanager/` | `EthernetService`、DHCP、EAP、LAN 管理 |
| `services/networksharemanager/` | 共享状态机、上行监控、RA daemon |
| `services/mdnsmanager/` | mDNS 报文、协议、socket listener |
| `services/vpnmanager/` | `NetworkVpnService`、扩展 VPN、系统 VPN |
| `services/netfirewallmanager/` | 规则、数据库、拦截记录和流量重定向 |
| `services/networkslicemanager/` | 5G URSP、网络切片和蜂窝适配 |
| `services/wearabledistributednetmanager/` | 穿戴分布式网络链路和供应商适配 |
| `frameworks/` | C、native、JS、ETS 和 VPN dialog |
| `interfaces/innerkits/` | 各扩展服务客户端 IPC SDK |
| `tools/ohos-networkShare/` | 网络共享命令工具 |
| `sa_profile/` | SA 1161、8300、8301、8400 等 |

`communication_netmanager_ext/BUILD.gn:25-130` 对各扩展组已有独立 `if(feature)`
控制，能够可靠地缩小扩展构建闭包，裁剪基础明显优于 base。

### 2.4 netstack

`communication_netstack` 没有常驻服务组，主要提供进程内库：

- `frameworks/native/http/http_client`、`http_curl`、`http_rust`：HTTP 客户端后端；
- `frameworks/native/tls_socket`、`net_ssl`：TLS 与证书处理；
- `frameworks/native/websocket_native`：WebSocket；
- `frameworks/js/napi/`：fetch、HTTP、Socket、TLS、WebSocket、NetSSL；
- `frameworks/ets/ani/`、`frameworks/cj/`：ETS/ANI 与 CJ 前端；
- `interfaces/innerkits/rust/`：YLong HTTP 和 Rust SDK。

其 `bundle.json` 的 base group 默认包含多项 JS/NAPI、C NDK 和 SSL/WebSocket 目标
（`communication_netstack/bundle.json:75-90`），仅靠
`netstack_http_boringssl` 开关不足以形成 watch 最小协议栈。

## 3. 当前运行时架构

### 3.1 进程和系统能力

| 进程 | SA | 共享库 | 启动 |
|---|---:|---|---|
| `netmanager` | 1151 | `libnet_conn_manager.z.so` | run-on-create |
| `netmanager` | 1152 | `libnet_policy_manager.z.so` | run-on-create |
| `netmanager` | 1153 | `libnet_stats_manager.z.so` | run-on-create |
| `netmanager` | 1154 | `libnet_tether_manager.z.so` | run-on-create |
| `netmanager` | 1155 | `libnet_vpn_manager.z.so` | 按需 |
| `netmanager` | 1156 | `libdns_resolver_manager.z.so` | run-on-create |
| `netmanager` | 1157 | `libethernet_manager.z.so` | run-on-create |
| `netsysnative` | 1158 | `libnetsys_native_manager.z.so` | run-on-create |
| `mdnsmanager` | 1161 | `libmdns_manager.z.so` | run-on-create |
| `netmanager` | 8300 | `libnetfirewall_manager.z.so` | 按需 |
| `netmanager` | 8301 | `libnetworkslice_manager.z.so` | run-on-create |
| `netmanager` | 8400 | `libwearable_distributed_net_manager.z.so` | 按需 |

证据来自 `communication_netmanager_base/sa_profile/1151.json:2-12` 至
`1158.json:2-12`，以及 `communication_netmanager_ext/sa_profile/1161.json:2-12`、
`8300.json:2-12`、`8301.json:2-12`、`8400.json:2-12`。

### 3.2 启动与注册链路

1. `early-boot` 创建数据目录并启动 `netmanager`
   （`services/etc/init/netmanager_base.cfg:2-10`）。
2. `netmanager` 通过 `sa_main /system/profile/netmanager.json` 运行，获得 NET_ADMIN、
   NET_BIND_SERVICE、NET_RAW 等 capability
   （`services/etc/init/netmanager_base.cfg:23-38`）。
3. `netsysnative` 通过独立 `sa_main` 进程运行，挂载 BPF/cgroup2，并创建
   `dnsproxyd`、`fwmarkd`、`tunfd`、`multivpnfd` socket
   （`services/etc/init/netsysnative.cfg:2-67`）。
4. SA framework 按 profile 加载各 `.z.so`。`sa_profile/BUILD.gn:18-32` 始终注册
   1151、1152、1153、1158，并在 `netmanager_base_extended_features=true` 时追加
   1154–1157。
5. Wi-Fi 或蜂窝供应商通过 `INetConnService::RegisterNetSupplier()` 注册，再调用
   `UpdateNetSupplierInfo()` 和 `UpdateNetLinkInfo()` 上报状态
   （`interfaces/innerkits/netconnclient/include/proxy/i_net_conn_service.h:41-55`）。
6. `NetConnService` 选出默认网络，经 `NetsysController`/`NetsysNativeClient`
   调用 SA 1158，最终通过 netlink、iptables、BPF 和 Unix socket 配置内核。

### 3.3 IPC 与客户端 SDK

调用层次为：

```text
JS/ETS/CJ/C/C++ API
  → framework/native client
  → innerkits Proxy
  → Binder IPC
  → NetConn/Policy/Stats/Ext SA Stub
  → NetsysController
  → Binder IPC
  → NetsysNativeService
  → netlink / BPF / iptables / fwmarkd / dnsproxyd
```

连接 IPC 面由 `INetConnService` 定义，包含供应商注册、网络请求、默认网络、探测、
代理、接口配置、路由等多类方法
（`interfaces/innerkits/netconnclient/include/proxy/i_net_conn_service.h:35-100`）。
Netsys IPC 面由 `INetsysService` 定义，包含 resolver cache、路由、默认网络、接口、
共享、VPN、统计和诊断等底层方法
（`interfaces/innerkits/netmanagernative/include/i_netsys_service.h:57-100`）。

### 3.4 数据流和线程模型

- **网络选择**：供应商状态 → `NetConnService`/`NetSupplier` → `Network` →
  默认网络评分和切换 → Netsys 默认路由。
- **DNS**：应用解析请求 → `dnsproxyd` → `DnsProxyListen`/`DnsParamCache` →
  上游 DNS；每个 netId 维护 resolver 配置和缓存。
- **统计**：内核/BPF map → `BpfStats`/`TrafficManager` →
  `NetStatsCached` → SQLite/历史查询。
- **策略**：`NetPolicyCore`/`NetPolicyRule` → `NetsysPolicyWrapper` →
  bandwidth/firewall manager → iptables/BPF。
- **探测**：`NetMonitor`/`ProbeThread` 发起 HTTP/双栈探测，结果更新 capability，
  再触发默认网络和订阅回调。
- **异步执行**：服务广泛依赖 `eventhandler:libeventhandler` 和 `ffrt:libffrt`；
  网络共享还维护主/子状态机和定时器。当前 BUILD 目标没有 watch 专用线程数或队列容量配置。

## 4. 功能—组件映射

| 功能 | 主要实现/target |
|---|---|
| 连接、选择、默认网络、切换 | `services/netconnmanager:net_conn_manager`；`NetConnService`、`NetSupplier`、`Network` |
| 网络探测、质量 | `net_http_probe.cpp`、`net_dual_stack_probe.cpp`、`net_monitor.cpp`、`probe_thread.cpp` |
| DNS/解析 | `NetsysNativeService`、`DnsManager`、`dnsresolv/*`、`dnsproxyd` |
| DHCP | `dhcp_controller.cpp`；Ethernet 场景另有 `EthernetDhcpController` |
| 路由、接口、fwmark | `RouteManager`、`InterfaceManager`、`FwmarkNetwork`、`fwmark_client` |
| 统计 | `services/netstatsmanager:net_stats_manager`、`services/netmanagernative/bpf:netsys` |
| 流量策略/基础防火墙 | `services/netpolicymanager:net_policy_manager` |
| 高级网络防火墙 | `services/netfirewallmanager:netfirewall_manager`，SA 8300 |
| VPN | `services/vpnmanager:net_vpn_manager`，SA 1155 |
| Ethernet | `services/ethernetmanager:ethernet_manager`，SA 1157 |
| 网络共享/热点 | `services/networksharemanager:net_tether_manager`，SA 1154 |
| mDNS | `services/mdnsmanager:mdns_manager`，SA 1161 |
| Wi-Fi/蜂窝适配 | 本仓提供 supplier/NetLinkInfo 接口；驱动和供应商主体位于外部部件 |
| NAT64/CLAT | `nat464_service.cpp`、`ClatManager`、`clatd*`、`bpf/bpf_progs/464xlat.c` |
| 代理/PAC | `NetHttpProxyTracker`、`NetPacManager`、JerryScript |
| 网络切片 | `services/networkslicemanager:networkslice_manager`，SA 8301 |
| 穿戴分布式网络 | `services/wearabledistributednetmanager:wearable_distributed_net_manager`，SA 8400 |
| 事件订阅 | 各 `I*Callback` Proxy/Stub、`BroadcastManager`、CommonEvent |
| 权限/审计 | access_token、SA Stub 检查、HiSysEvent/HiTrace |
| JS/NAPI | base/ext/netstack 的 `frameworks/js/napi/*` |
| ETS/ANI/Rust | `frameworks/ets/ani/*`、`common/ani_rs*`、netstack Rust InnerKit |
| 测试/工具 | `test/`、`test/fuzztest/`、`tools/ohos-networkShare` |

## 5. 构建依赖与裁剪证据

### 5.1 可直接通过现有开关裁剪

`communication_netmanager_ext/netmanager_ext_config.gni:48-73` 的默认值中：

- Ethernet、Sharing、mDNS、VPN、VPN Extension 默认 `true`；
- Firewall、SysVPN、Wearable Distributed Net、NetworkSlice 默认 `false`；
- Wi-Fi、Bluetooth、USB、Battery 支持根据 `global_parts_info` 自动开启
  （`communication_netmanager_ext/netmanager_ext_config.gni:99-126`）。

顶层 `communication_netmanager_ext/BUILD.gn:25-130` 已按上述 feature 包围 service、
InnerKit、NAPI 和配置目标。因此关闭对应 feature 可可靠缩小扩展构建闭包。

base 已可关闭 PAC、SysVPN、高级防火墙、企业路由、公共 DNS、穿戴分布式网络和
高级流量统计；`netmanager_base_support_ebpf_memory_miniaturization` 可向 BPF target
传递 `SUPPORT_EBPF_MEM_MIN`
（`services/netmanagernative/bpf/BUILD.gn:37-50`）。

### 5.2 当前无法直接裁剪的静态耦合

1. **Base 服务聚合过粗**：`services/BUILD.gn:19-28` 同时构建 Conn、Policy、Stats、
   Netsys，缺少 `feature_policy`、`feature_stats`。
2. **Conn 静态依赖 Policy/Stats**：
   `services/netconnmanager/BUILD.gn:66-75` 的 `net_conn_manager` 无条件依赖
   `net_policy_manager_if` 和 `net_stats_manager_if`。直接删除 SA 1152/1153
   不能同步删除客户端库。
3. **Conn 无条件引入重依赖**：
   `services/netconnmanager/BUILD.gn:91-111` 无条件列出 DataShare、RDB、OS Account、
   curl 和 JerryScript；而 PAC sources 仅在 `:118-125` 条件追加。即使 PAC 关闭，
   JerryScript 仍进入链接闭包。
4. **Stats 基础目标始终持久化**：
   `services/netstatsmanager/BUILD.gn:42-58` 无条件编译 database/history/sqlite
   sources，`:87-105` 无条件依赖 DataShare、SQLite、OS Account。现有
   `SUPPORT_TRAFFIC_STATISTIC` 只裁掉扩展的流量套餐/通知功能。
5. **Policy 无条件包含复杂策略**：
   `services/netpolicymanager/BUILD.gn:35-61` 同时编译流量、UID、idle、power-save、
   firewall、RDB 和备份逻辑；`:83-104` 引入 Ability、Bundle、RDB、OS Account 等。
6. **Netsys 内部管理器无条件编译**：
   `services/netmanagernative/BUILD.gn:50-100` 同时包含 bandwidth、CLAT、distributed、
   firewall、sharing、traffic、VNIC、VPN、MPTCP；只有 SysVPN、Wearable 和高级
   Firewall 的附加代码受条件控制。
7. **Ext 与 Base 存在反向依赖风险**：
   ext 普遍依赖 base；base 的 bundle 又声明 ext。具体的 Stats→Tether 依赖仅在
   `netmanager_base_share_traffic_limit_enable` 开启时发生
   （`services/netstatsmanager/BUILD.gn:130-133`），watch 必须保持关闭。
8. **Netstack 前端未细分产品组**：
   `communication_netstack/bundle.json:75-90` 默认打包 JS/NAPI、C NDK、SSL 和
   WebSocket；HTTP3 feature 存在，但 Socket/WebSocket/NAPI 缺少统一的产品开关。

## 6. Watch 最小闭环与能力分层

### 6.1 必须保留：单 Wi-Fi 基础档

- SA 1151：供应商注册、链路状态、默认网络、事件回调；
- SA 1158：接口、路由、DNS、fwmark 和必要 netlink；
- `netsys_controller`、`net_conn_manager_if`、必要 Parcel/Proxy/Stub；
- DHCP 客户端及外部 Wi-Fi supplier；
- DNS resolver/cache；
- 最小 socket 绑网权限与 access-token 校验；
- 应用实际需要的一种 API 前端；优先 C/C++，若产品应用为 ETS 再保留 ANI；
- OTA/云同步所需的最小 HTTP+TLS，不默认保留 WebSocket/HTTP3。

### 6.2 可选保留

- SA 1152 的最小后台/idle 策略，用于功耗治理；
- 仅内存、低频采样的基础流量统计；
- 简化网络探测；仅在 captive portal/自动切网需求明确时保留；
- mDNS；仅在局域网发现场景保留；
- 蜂窝连接、CLAT/NAT64、网络切片；作为蜂窝 watch 独立产品档；
- Wearable Distributed Net；它是与手机协同联网的专用能力，而不是单 Wi-Fi watch
  的必需项。

### 6.3 默认移除

Ethernet、网络共享/热点、VPN/VPN Extension/SysVPN、高级防火墙、网络切片（Wi-Fi
档）、PAC/JerryScript、统计持久化与套餐通知、复杂多用户/企业策略、NAPI/JS（无 JS
应用时）、CJ、未使用的 Rust 前端、调试工具、单测、fuzz 和覆盖率目标。

测试源码仍应保留在仓库并在开发/CI 构建运行，只是不进入产品镜像。

## 7. RAM/ROM 优化手段

### 7.1 低风险构建裁剪

1. 新增 watch 产品参数集，显式关闭 ext 默认开启项和 base extended features。
2. 将 JerryScript 从 Conn 的无条件 `external_deps` 移到 PAC 条件分支。
3. 不把 JS/NAPI、CJ、测试、fuzz、共享工具加入 watch product group。
4. 关闭 `enable_netmgr_debug`；保留必要错误日志，降低 HiTrace/HiSysEvent 高频路径。
5. 开启 BPF memory miniaturization，并根据实际 UID/iface 数量调整 map 上限。
6. 沿用现有 `-fdata-sections`、`-ffunction-sections`、`-Oz/-Os`、
   `--gc-sections` 和 hidden visibility；在全量回归后评估跨目标 LTO。
7. 不生成关闭能力对应的 SA profile、init 配置、SysCap 和权限声明，避免“库已裁、
   注册仍在”的启动失败。

### 7.2 需要重构的深度裁剪

1. 把 `net_conn_manager` 对 Policy/Stats 的调用倒置为可选 provider；watch 配置注入
   无状态桩或最小实现。
2. 将 Stats 拆为 `stats_counter_core`、`stats_persistence`、`stats_plan_ui`，watch
   只保留固定容量内存计数器。
3. 将 Policy 拆为 `policy_core`、`quota`、`power_idle`、`enterprise_multiuser`，
   防止 RDB/Ability/Bundle 依赖污染最小闭包。
4. 将 Netsys 拆为 interface/route/dns core 与 sharing/vpn/clat/mptcp/traffic 插件；
   用编译期 provider table 替代散布的宏。
5. 收缩 watch IPC 面，但保持现有 descriptor/transaction ABI 或提供兼容 Stub；
   不应在同一阶段同时重写客户端和服务端协议。
6. 缩减 DNS cache、supplier/network 容器、callback 列表、事件队列、定时器和线程池；
   容量必须由测量到的 watch 并发上限驱动。
7. 若权限与故障隔离允许，可评估合并 netmanager/netsysnative；这是高风险方案，
   会改变 UID、SELinux、capability 和崩溃域，不应作为首轮优化。
8. 对 HTTP 后端做二选一，避免 curl/OpenSSL 与 YLong/Rust 双重闭包；选择必须依据
   目标镜像实际依赖和 TLS 合规要求。

## 8. 建议目标架构和启动流程

```text
Watch App (C/C++ 或 ETS，二选一为主)
  │
  ├─ Minimal Net Connection SDK
  │       │ Binder
  │       ▼
  │   Net Core SA (1151)
  │   ├─ Supplier registry
  │   ├─ Single-default selection
  │   ├─ Link/callback manager
  │   └─ Optional lightweight probe
  │       │
  │       ▼
  │   Minimal Netsys Controller
  │       │ Binder
  │       ▼
  └─ Netsys Core SA (1158)
      ├─ Interface + netlink
      ├─ Route/default network
      ├─ DNS resolver/cache
      ├─ DHCP adapter
      └─ fwmark/socket permission

可选 provider：Policy-lite、Stats-memory、Cellular/CLAT、mDNS、Wearable Distributed Net
```

启动顺序建议：

1. netsysnative 初始化接口、netlink、DNS/fwmark socket；
2. Net Core SA 注册并等待供应商，不初始化关闭能力；
3. Wi-Fi supplier 注册并上报 DHCP 获得的 `NetLinkInfo`；
4. Net Core 创建网络、下发路由/DNS、设置默认网；
5. 发布 `netAvailable`；
6. 探测、Stats、Policy 和协同联网按 feature 延迟初始化。

## 9. 建议 feature 配置

以下为建议产品参数示意，不代表仓库当前已有完整组合约束：

```gn
enable_netmgr_debug = false
netmanager_base_extended_features = false
netmanager_base_enable_pac_proxy = false
netmanager_base_enable_feature_sysvpn = false
netmanager_base_enable_feature_net_firewall = false
netmanager_base_enable_traffic_statistic = false
netmanager_base_enable_netsys_access_policy_diag_listen = false
netmanager_base_share_traffic_limit_enable = false
netmanager_base_feature_enterprise_route_custom = false
netmanager_base_support_ebpf_memory_miniaturization = true

netmanager_ext_feature_ethernet = false
netmanager_ext_feature_share = false
netmanager_ext_feature_mdns = false
netmanager_ext_feature_vpn = false
netmanager_ext_feature_vpnext = false
netmanager_ext_feature_sysvpn = false
netmanager_ext_feature_net_firewall = false
netmanager_ext_feature_networkslice = false
netmanager_ext_feature_wearable_distributed_net = false

netstack_feature_http3 = false
netstack_feature_communication_http3 = false
```

若启用协同联网，需要同时验证 base/ext 两侧 wearable feature、SA 8400 profile、
Netsys manager 附加实现和配置 JSON 的一致性，不能只开启一个布尔参数。

## 10. 测量与验收

### 10.1 基线

每个配置至少采集：

- 镜像中全部网络 `.so`、可执行文件、配置和 profile 的文件大小；
- `llvm-size/readelf/nm` 的 section、NEEDED、导出符号和最大符号；
- `netmanager`、`netsysnative`、`mdnsmanager` 的 PSS/RSS/heap peak；
- 线程、FD、Binder 对象、timer 和 socket 数；
- 启动至 SA ready、Wi-Fi supplier ready、DHCP 完成、default net、DNS 成功的时间戳；
- 空闲、周期同步、弱网重连的 CPU 唤醒和稳态功耗。

禁止把三个 `bundle.json` 的 RAM/ROM 字段当作实测基线。

### 10.2 建议目标预算

下列数值仅作为立项建议值，必须经硬件基线校准：

| 指标 | P0 建议目标 | P2 建议目标 |
|---|---:|---:|
| 网络管理相关 ROM | 相对基线减少 ≥25% | 相对基线减少 ≥45% |
| netmanager+netsysnative 稳态 PSS | 相对基线减少 ≥20% | 相对基线减少 ≥35% |
| 常驻线程 | 相对基线减少 ≥20% | 由最小事件模型进一步下降 |
| 首次 DNS/联网时延 | 不劣化超过 5% | 不劣化超过 5% |
| 稳态功耗 | 不高于基线 | 相对基线下降 |

### 10.3 回归矩阵

| 维度 | 必测场景 |
|---|---|
| Wi-Fi | 冷启动、DHCP、静态 IP、断开/重连、AP 切换、弱网 |
| IP | IPv4；如保留 IPv6，再测 SLAAC、IPv6 DNS 和双栈 |
| DNS | 正常、超时、缓存命中、网络切换后 cache 隔离 |
| 默认网 | 单供应商；蜂窝档再测 Wi-Fi/Cellular 切换 |
| Socket | 默认绑网、显式 netId、进程重启后的 fwmark |
| 事件 | available/lost/capability/link 回调顺序和去重 |
| 权限 | 普通应用、系统应用、越权 IPC、非法参数 |
| 稳定性 | SA 重启、netsys 重启、连续重连、低内存 |
| 功耗 | 屏灭待机、周期同步、无网络重试、弱网扫描 |

## 11. 分阶段路线图

1. **P0：可配置裁剪**  
   建立 watch 产品参数；关闭 ext 默认能力、PAC、SysVPN、复杂统计、debug 和无用 API
   前端；同步清理 profile/init/SysCap/权限打包项。
2. **P1：依赖闭包修正**  
   条件化 JerryScript、DataShare、RDB、Ability 等依赖；生成并比较 NEEDED 图和镜像
   文件清单。
3. **P2：服务内部拆分**  
   拆 Stats 持久化、Policy 高级模块和 Netsys optional managers；引入明确的
   `requires/conflicts` 校验。
4. **P3：运行时优化**  
   按需初始化可选 provider，缩减线程/队列/cache/timer；评估 Policy/Stats
   是否保留 SA 或并入 Net Core。
5. **P4：深度架构优化**  
   根据实测决定是否收缩 IPC ABI、合并进程、替换 HTTP 后端或移除 BPF/iptables
   子能力。

每一阶段均须保留前一阶段产品配置作为对照，并通过第 10 节回归矩阵后才能进入下一阶段。

## 12. 风险与待验证项

1. `netmanager_base_extended_features=false` 同时移除 1154–1157 profile，需验证产品
   DNS 调用是否依赖独立 SA 1156，而非仅依赖 SA 1158 resolver。
2. Wi-Fi 实现不在本仓，必须验证 supplier 注册 API、DHCP 归属和 SA 启动时序。
3. 穿戴分布式网络同时跨 base/ext/Netsys，开关不一致可能产生 IPC 方法存在但实现
   缺失的问题。
4. Policy 对省电的净收益需实测：移除线程/RDB 可省 RAM，但失去后台限流可能增加功耗。
5. Stats/BPF 是否被系统设置、计费、审计或安全策略依赖，需要做跨仓调用图。
6. 合并 netsysnative 会扩大 netmanager 权限和故障域，需 SELinux 与安全评审。
7. LTO、异常和 RTTI 调整可能影响插件加载、CFI 和 ABI，必须以逐 target 方式推进。
8. 建议预算不是当前仓库测量结果；最终阈值应由目标 SoC、内存档位和产品用例确定。

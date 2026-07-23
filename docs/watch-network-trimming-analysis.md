# OpenHarmony 手表网络组件裁剪架构分析

## 1. 目标与范围

本文分析当前仓库中的 `netmanager_base`、`netmanager_ext` 和 `netstack`，给出在满足手表基本网络功能前提下最大程度降低 RAM、ROM 占用的架构指导。

当前仓库不包含完整 Telephony 源码，因此本文仅分析 NetManager 与 Telephony 的现有依赖边界；Telephony 内部的具体裁剪项需要对相关仓库另行执行同样的能力闭包分析。

## 2. 当前架构

仓库包含三个 OpenHarmony 部件：

| 部件 | 主要职责 | ROM 预算（`bundle.json`） | RAM 预算（`bundle.json`） |
| --- | --- | --- | --- |
| `netmanager_base` | 网络连接、策略、统计、Netsys 原生服务 | 4.5 MB | 10 MB |
| `netmanager_ext` | 以太网、网络共享、mDNS、VPN、防火墙、网络切片、穿戴分布式网络 | 2 MB | 500 KB |
| `netstack` | HTTP、WebSocket、Socket、TLS 等应用侧协议能力 | 3 MB | 5 MB |

这些预算来自各部件 `bundle.json`，是声明值而非实测值，当前不能证明产品的实际峰值内存满足预算。

现有架构通过四种机制实现组件化：

1. `bundle.json`：声明部件、SysCap、Feature、依赖和预算；
2. `*_config.gni`：定义编译期 Feature；
3. `BUILD.gn`：选择源码、接口和打包目标；
4. SA Profile：定义运行时服务。

运行路径大致为：

```text
应用 API → InnerKit/IPC Proxy → SA Service → Netsys Controller → 内核网络能力
```

该结构实现了控制面与数据面分离，但 SysCap、Feature、构建目标和 SA 之间缺少统一、可计算的映射。

## 3. 当前裁剪障碍

### 3.1 核心服务粒度过粗

`communication_netmanager_base/services/BUILD.gn` 默认把 Netsys、连接管理、策略管理和统计管理四项服务一起加入 `netmanager_base_services`。手表即使只需要连接、路由和 DNS，也难以完整移除策略、统计及其数据库、通知等依赖。

相比之下，`communication_netmanager_ext/BUILD.gn` 已能按 Ethernet、Sharing、mDNS、VPN、Firewall、NetworkSlice 等能力选择构建目标，具备更好的能力级裁剪基础。

### 3.2 依赖闭包过大

`netmanager_base` 的部件依赖包含数据库、应用运行时、UI、通知、电源管理和 Telephony 相关部件。`netmanager_base` 依赖 `netmanager_ext`，而 `netmanager_ext` 又依赖 `netmanager_base`，形成回环，使可选能力可能反向污染核心闭包。

### 3.3 Feature 缺少组合约束

当前 Feature 主要是独立布尔变量，没有统一表达 `requires`、`conflicts` 和硬件条件。理论组合数量远大于有效组合数量，非法配置只能在构建或运行阶段被动发现。

### 3.4 构建裁剪与运行时裁剪脱节

关闭服务实现并不必然同时移除其 API、InnerKit、SA Profile、启动配置、资源文件和传递依赖。运行时不启动也不能减少已经进入镜像的 ROM，服务进程和 IPC 框架还会增加常驻 RAM。

## 4. 手表产品能力基线

应先定义产品能力，再求解构建目标，而不是直接选择 GN 开关。

### 4.1 必需能力

- Wi-Fi 和/或蜂窝数据链路接入；
- 网络接口、路由和默认网络管理；
- DNS 解析；
- Socket 和 TLS；
- 网络状态变化通知；
- 最小权限与流量隔离；
- 休眠、唤醒和弱网重连。

### 4.2 条件能力

- HTTP：只保留升级、时间同步、账户等系统业务需要的客户端能力；
- Cellular：仅蜂窝版手表启用；
- Wearable Distributed Network：仅依赖手机代理联网的产品启用；
- mDNS：仅有局域网发现需求时启用；
- 流量统计：优先保留轻量计数，移除历史数据库、复杂套餐策略和 UI 通知。

### 4.3 默认移除

- Ethernet；
- USB、Bluetooth、Wi-Fi 网络共享；
- VPN 和 VPN Extension；
- Network Slice；
- 企业防火墙和复杂网络策略；
- PAC Proxy、Public DNS Server；
- HTTP/3；
- 无常驻业务需求时的 WebSocket；
- 产品未使用的 JS、ETS、CJ 等语言绑定；
- 调试、覆盖率、诊断和 FPGA 功能。

`netmanager_ext_config.gni` 当前默认启用 Ethernet、Sharing、mDNS、VPN 和 VPN Extension，不适合直接作为手表产品基线，手表画像应改为扩展能力按需启用。

## 5. 裁剪方法

### 5.1 ROM 裁剪

按收益优先级执行：

1. **整部件移除**：产品不需要扩展服务时不引入 `netmanager_ext`；不需要完整应用协议栈时不引入完整 `netstack`。
2. **能力闭包移除**：同步移除 API、InnerKit、SA、启动配置、资源文件及传递依赖。
3. **语言前端裁剪**：只保留产品实际使用的 API 绑定。
4. **依赖解耦**：避免单项功能引入数据库、UI、完整应用运行时或完整 Telephony。
5. **实现变体**：为 DNS、HTTP、统计和策略提供 `full`/`minimal` 实现。
6. **链接优化**：继续使用 section GC、隐藏符号和尺寸优化，并在工具链支持时评估 LTO、ICF。
7. **资源裁剪**：删除未使用证书、配置、协议、错误文本和本地化资源。

### 5.2 RAM 裁剪

1. 将低风险 SA 折叠到少量进程域，减少 IPC、线程池和动态库重复映射；
2. 非必要服务按需拉起，并在空闲后退出；
3. 减少常驻线程、EventRunner、缓存和定时任务；
4. 使用固定大小环形缓冲或内核计数替代常驻 SQLite/DataShare；
5. 限制 TLS Buffer、DNS Cache 和 HTTP Connection Pool；
6. 保持数据面在内核或 eBPF 路径，避免逐包 IPC；
7. 使用 PSS、峰值 RSS、堆峰值、线程栈和 mmap 实测值核算，而不是仅依赖部件声明。

对于能力固定的手表，静态裁剪通常优于“全部编译、运行时关闭”；后者可能减少启动开销，但不能消除镜像占用。

## 6. 推荐架构模型

建议采用**能力原生架构（Capability-Native Architecture，CNA）**和**产品画像驱动架构（Product Profile Driven Architecture，PPDA）**。

将产品裁剪形式化为：

> 给定产品必需能力集合、硬件条件和 RAM/ROM 预算，求满足依赖约束的最小组件及运行时闭包。

建立以下可计算映射。箭头表示逐层推导：每一层以上一层的约束为输入，生成下一层的最小合法闭包。

```text
Product Profile
→ Capability Graph
→ Component Graph
→ Service Graph
→ Runtime Deployment
→ Binary/Memory Layout
```

每项能力的统一描述至少应包含：

- `provides`、`requires`、`conflicts`；
- 硬件条件和 SysCap；
- 对应源码、构建目标、API、SA 和配置文件；
- ROM、静态 RAM、峰值 RAM、启动时间和功耗；
- 独立测试闭包；
- 可选的 `full`/`minimal` 实现。

该模型应遵守三条原则：

1. **可减性**：任何非核心能力都能完整移除，剩余系统仍可构建、启动和通过基线测试；
2. **闭包性**：能力到源码、二进制、服务和依赖的关系可由工具计算；
3. **预算性**：ROM、RAM、启动时间、线程数和唤醒次数由 CI 自动测量并强制校验。

## 7. 推荐的手表运行时形态

逻辑服务与物理部署应解耦。手表可采用两个主要进程域：

1. **网络核心域**：NetConn、最小 Policy、Netsys 和 DNS；
2. **链路适配域**：Wi-Fi、Cellular 或 Companion Link，按产品选择。

HTTP/TLS 尽量作为按需库进入调用进程，统计、诊断和扩展服务按需拉起或完全移除。手机产品仍可保留多 SA 隔离，而手表通过部署画像折叠服务，避免维护独立分支。

## 8. Telephony 协同裁剪

NetManager 当前存在 `cellular_data`、`core_service` 等 Telephony 依赖。后续分析 Telephony 仓库时应重点检查：

- 蜂窝数据、语音、短信、IMS、SIM、RIL 是否可形成独立能力闭包；
- Wi-Fi-only 手表是否能完全移除 Telephony；
- 蜂窝手表是否只保留注册、数据连接、SIM 和最小 RIL；
- NetManager 是否通过稳定的小接口依赖 Cellular，而不是依赖完整 Telephony 部件；
- Network Slice、套餐统计等手机能力是否能独立关闭；
- Telephony 服务是否支持按产品折叠进程和减少常驻线程。

核心原则是：Telephony 可以依赖 NetManager 提供网络基础能力，但 NetManager 核心不应反向依赖 Telephony 的可选业务实现。

## 9. 工程演进路线

1. 定义 `Watch-WiFi`、`Watch-Cellular`、`Companion-Watch` 和现有 Phone Profile；
2. 自动生成 Feature—Target—SA—Dependency 图；
3. 打破 `netmanager_base` 与 `netmanager_ext` 的循环依赖；
4. 将 base 拆分为 Connectivity Core、Policy、Statistics 和 Native Data Plane；
5. 关闭 ext 默认能力和未使用语言前端；
6. 为 DNS、统计、策略和 HTTP 引入最小实现；
7. 支持手表服务进程折叠和按需拉起；
8. 在 CI 中对每个 Profile 测量镜像增量、PSS/RSS、线程数、启动时间和功耗；
9. 使用差分构建计算每项能力的边际资源成本；
10. 将有效 Profile 纳入持续构建，防止后续依赖和资源占用回涨。

## 10. 验收指标

每个手表 Profile 至少应持续检查：

- 必需能力测试通过率；
- 系统镜像及各能力的 ROM 增量；
- 冷启动和稳定态 PSS、峰值 RSS；
- 常驻进程数、线程数和 IPC 对象数；
- 启动时间、网络首次可用时间；
- 空闲及弱网场景唤醒次数；
- Feature 依赖闭包完整性；
- 被禁用能力对应的二进制、SA、配置和依赖不存在。

最终目标不是维护一套“手表专用删减代码”，而是让同一代码库能够根据产品画像稳定、可重复地生成满足资源预算的最小网络系统。

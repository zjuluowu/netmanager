# 《Theory of Resource-Constrained Product-Line Software Architecture》

## ——以 OpenHarmony 通信子系统为案例的资源受限产品线软件架构理论

**研究方法**：Observation（仓库实证）→ Abstraction（思想提炼）→ Theory（模型建立）→ Validation（多平台验证）→ Evolution（演进路线）

---

# Part I　Case Study：OpenHarmony 通信子系统

## 1. Current Architecture（当前架构）

### 1.1 宏观结构

仓库包含三个 OpenHarmony 部件（Component/Part）：

| 部件 | 职责 | SysCap | ROM/RAM 预算 |
|---|---|---|---|
| `netmanager_base` | 网络连接管理核心：连接、策略、统计、netsys 原生服务 | `NetManager.Core` | 4.5MB / 10MB |
| `netmanager_ext` | 扩展能力：以太网、共享、mDNS、VPN、防火墙、网络切片、穿戴分布式网络 | `Ethernet / NetSharing / MDNS / Vpn / NetFirewall / Eap` | 2MB / 500KB |
| `netstack` | 应用侧协议栈：HTTP(3)、WebSocket、Socket、TLS | `NetStack` | 3MB / 5MB |

> 注：表中 ROM/RAM 数值直接取自各部件 `bundle.json` 的 `rom`/`ram` 字段，属于部件自声明的资源预算上限（非实测值），当前构建系统并不强制校验。

每个部件的内部结构高度同构，形成“部件模板”：

```
component/
├── bundle.json          # 部件元数据：SysCap、features、deps、ROM/RAM 预算、构建组
├── *_config.gni         # 构建期变量与 Feature 声明（declare_args）
├── interfaces/
│   ├── innerkits/       # 系统内部 C++ SDK（IPC Proxy）
│   └── kits/            # 应用 API（NAPI/ANI/CJ）
├── frameworks/          # js/ets/cj/native 多语言前端框架
├── services/            # SA（System Ability）服务端实现
├── sa_profile/          # SA 注册描述（进程模型）
├── utils/ common/ test/
```

### 1.2 运行时模型

- **服务即 SA**：`netconnmanager`、`netpolicymanager`、`netstatsmanager`、`netsyscontroller`（base），`ethernetmanager`、`networksharemanager`、`mdnsmanager`、`vpnmanager`、`netfirewallmanager`、`networkslicemanager`、`wearabledistributednetmanager`（ext），均以 SA 形式由 `samgr` 注册、`safwk` 承载，通过 `init` 的 `.cfg/.rc` 文件（如 `vpnmanager.cfg`、`ethernet.cfg`）纳入启动编排——**启动即编排产物**。
- **三层运行时**：应用（NAPI/ETS-ANI/CJ 桥）→ innerkits Proxy →（IPC）→ SA Service → `netsyscontroller` →（IPC/直调）→ `netsys` 原生守护（含 eBPF/iptables/fwmark 内核操纵层）。
- **控制面与数据面分离**：控制面走 IPC 决策；数据面通过 eBPF map、fwmark、iptables 下沉到内核，避免每包 IPC。

### 1.3 依赖关系

`bundle.json` 的 `deps.components` 显式声明部件级依赖。观察到两个重要事实：

1. **依赖数量巨大**：base 依赖 50+ 个部件（ipc、samgr、safwk、ffrt、access_token、relational_store、power_manager、cellular_data、ace_engine……）。
2. **存在循环隐患**：`netmanager_base` 的 deps 中出现 `netmanager_ext`，而 ext 又依赖 base——base/ext 的“分层”边界在依赖图上已经被侵蚀。这是本研究的第一个关键观察：**声明式分层挡不住实现级回环**。

### 1.4 设计目标（还原）

- 用统一的部件模板覆盖多产品形态（standard system）；
- 用 Feature 开关 + GN 静态裁剪控制 ROM/RAM；
- 用 SysCap 向应用生态承诺能力；
- 用 SA + IPC 实现进程隔离与按需拉起。

## 2. Design Philosophy（设计哲学）

提炼出六条隐含哲学：

1. **Capability as Contract**：SysCap 是对外契约，`SystemCapability.Communication.NetManager.Eap = false` 这种写法表明能力可以被产品显式否定——能力是一等公民且可被减法定义。
2. **Feature as Compile-Time Variation Point**：`declare_args()` 中 16+（base）+17（ext）个 Feature 开关，默认多为 false，采用“opt-in 增量”策略。
3. **Base/Ext 二分**：核心不可裁，扩展可裁——一种粗粒度的产品线二元分区。
4. **Manager–Controller–Client 三角**：Manager（SA 业务编排）、Controller（`netsys_controller` + `i_netsys_controller_service` 接口 + `service_impl`/`native_client`/`mock_client`，典型 Strategy/Factory 组合，支持 mock 注入）、Client（innerkits Proxy）。
5. **预算即架构约束**：bundle.json 中 rom/ram 字段把资源预算写进元数据，是“架构经济学”的雏形。
6. **多语言前端、单一服务后端**：js/ets/cj/c 前端全部收敛到同一 SA。

## 3. Component Model（组件模型）

OpenHarmony 部件 = **元数据（bundle.json）+ 构建单元（GN group）+ 运行单元（SA/so）+ 契约（SysCap）+ 变体（features）** 的五元组。构建组分为 `base_group / fwk_group / service_group`，这实质上是**同一部件内部的再分层**：C API、框架桥、服务端可独立进出产品镜像。

## 4. Capability Model（能力模型）

从仓库归纳能力四象限：

| 类别 | 定义 | 实例 |
|---|---|---|
| **Always Required** | 任何联网产品必需 | NetManager.Core（连接管理、netsys、DNS） |
| **Optional** | 产品可选增强 | MDNS、NetSharing、Vpn、HTTP3 |
| **Conditional** | 依赖其它硬件/子系统存在 | Ethernet（依赖 hdf ethernet 驱动）、NetSharing（依赖 wifi/bluetooth/usb） |
| **Product Variant** | 仅特定产品形态 | wearable_distributed_net（手表）、networkslice（5G 手机）、net_firewall（企业/PC） |

关键发现：**能力分类与代码组织不完全对齐**——Product Variant 能力（wearable_distributed_net）同时散布在 base 和 ext 的 feature 列表中，说明当前架构缺少“能力→代码”的强映射机制。

## 5. Product Line Model（产品线模型）

当前产品线机制 = SysCap（对外承诺）× Feature（编译变体）× GN group（打包粒度）× deps（依赖闭包）。产品定义（product config）选择 feature 值，GN 据此裁剪源码与打包目标。**变体空间理论上是 2^N（N≈36 个 feature），实际有效组合远少于此，但架构没有任何机制约束非法组合**——这是产品线复杂度失控的根源。

## 6. Complexity Analysis（复杂度分析）

建立五维复杂度模型：

- **组件复杂度 C_comp**：部件内部构建目标数 × 接口面。base 的 build group 中目标 20+，内聚性尚可但目标粒度不均。
- **依赖复杂度 C_dep = |E| / |V| + 环数**：base 出度 50+，且 base↔ext 成环，C_dep 已越过“可推理阈值”（经验上出度 >15 即难以人工推理裁剪影响）。
- **Feature 复杂度 C_feat = 2^N 受约束后的有效变体数**：36 个开关无组合约束模型，测试矩阵不可穷举。
- **产品复杂度 C_prod = Σ(产品 × 有效 feature 差异)**：每新增一个产品形态，回归成本近似线性叠加而验证成本超线性增长。
- **生命周期复杂度 C_life**：feature 只有“存在/不存在”两态，没有 deprecate→remove 的生命周期治理，开关只增不减（fpga_mode、coverage 等调试性开关混入产品 feature 列表即为证据）。

**核心结论**：复杂度增长的主导项不是代码量，而是 **Feature × Product × Dependency 的组合爆炸**。

---

# Part II　Theory：理论体系

## 7. Architecture Taxonomy（架构分类学）

建立三轴分类空间：**组合时机**（编译期/链接期/启动期/运行期）×**变体机制**（配置/条件编译/插件/能力协商）×**隔离模型**（单内核/进程/微内核 IPC/虚拟化）。

| 系统 | 组合时机 | 变体机制 | 隔离模型 | 资源策略 | 产品适配策略 | 演化能力 |
|---|---|---|---|---|---|---|
| **Linux** | 编译期(Kconfig)+运行期(module) | 条件编译+可加载模块 | 单内核 | 极致可裁（KB级~服务器） | defconfig | 强（30年验证） |
| **Android** | 运行期为主 | HAL/Treble、feature flag | 进程+Binder | 弱裁剪（GMS 下限高） | Vendor overlay | 中（Treble 后改善） |
| **Fuchsia** | 启动期+运行期 | Component Framework + Capability Routing | 微内核 Zircon | 中 | Product Assembly | 理论最强，实践未证 |
| **AUTOSAR Adaptive** | 配置期(ARXML) | Service/Manifest | POSIX 进程+SOME/IP | 强（形式化配置） | Machine Manifest | 弱（标准演进慢） |
| **QNX** | 启动期 | Resource Manager | 微内核消息传递 | 强 | BSP+启动脚本 | 中 |
| **Zephyr** | 编译期(Kconfig+DeviceTree) | 全静态裁剪 | 单镜像 | 极强（KB 级） | board/soc overlay | 中 |
| **OpenHarmony** | 编译期(GN feature)+启动期(init/SA) | Feature+SysCap+部件 | 进程+IPC(Binder-like) | 中强 | 部件选配+feature | 中，受依赖复杂度拖累 |

**OpenHarmony 定位**：它试图同时占据 Zephyr 的静态裁剪象限与 Android 的服务化象限，是**唯一显式把“能力（SysCap）”作为生态契约的系统**，但其能力模型停在“声明”层，未达到 Fuchsia 的“能力路由即运行时”层。这正是其理论机会所在。

## 8. Architecture Economics（架构经济学）

定义成本模型（单位：工程师维护/验证工作量）：

- **Feature Cost** = 实现成本 + Σ(与其他 feature 的交互测试成本) + 生命周期治理成本。交互项使总成本随 feature 数**平方增长**：TC(F) ≈ aN + bN²。
- **Dependency Cost** = 每条依赖边的版本协同成本 × 变更传播概率。环形依赖使传播概率趋近 1（base↔ext 即案例）。
- **Capability Cost** = 生态承诺成本：一旦 SysCap 发布，撤销成本 ≈ ∞（应用兼容性），故能力发布应比 feature 添加**慎一个数量级**。
- **Product Cost** = 基线维护 + 差异维护；当差异靠散落的 `#ifdef`/GN 条件表达时，Product Cost 随产品数超线性增长；靠 Profile 集中表达时可回到线性。
- **Evolution Cost** = 架构重构成本 × 生态锁定系数。

**复杂度增长定律（本理论第一定律）**：*在缺乏能力—组件强映射的产品线中，维护成本增长率由 Feature 交互项主导，而非代码规模。* 推论：治理重点应是**约束 feature 组合空间**（合法性模型），而非减少代码。

## 9. Resource-Constrained Architecture Theory（资源受限架构理论）

**定义**：资源受限产品线架构是这样一种架构，其首要设计变量不是功能分解，而是 *“给定产品资源预算 B，求满足能力需求集 R 的最小闭包 S”* 的可计算性。

**三条公理**：

1. **可减性公理**：任何能力必须可以被完整移除，且移除后系统仍可构建、启动、通过基线测试。
2. **闭包公理**：能力 → 组件 → 依赖 → 二进制的映射必须机器可计算（当前 OpenHarmony 只能靠人工+GN 试错，违反此公理）。
3. **预算公理**：ROM/RAM/启动时间预算必须是构建系统的一等输入并可被自动校验（bundle.json 的 rom/ram 字段目前只是注释性质，未被强制）。

Phone→Watch/IoT 需要大量裁剪的根本原因据此可解释：Phone 架构隐含违反公理 1（大量能力默认在场、隐式互依）与公理 2（裁剪影响不可计算），故裁剪成为一次性人工考古工程而非可重复的求解过程。

## 10. Capability-Native Architecture（能力原生架构，CNA）

- **名称**：Capability-Native Architecture (CNA)。
- **定义**：以“能力”为架构第一公民的体系——每个能力具有：形式化声明（提供的接口、要求的资源、依赖的其他能力）、独立的代码闭包、独立的测试闭包、显式生命周期。组件、服务、二进制都是能力的投影产物。
- **原理**：把当前“SysCap（声明）/ Feature（编译）/ 部件（打包）/ SA（运行）”四套彼此松耦合的机制统一为单一能力元模型，使“声明的能力”与“构建出的字节”之间可追溯、可验证。
- **生命周期**：Proposed → Experimental（不承诺 ABI）→ Stable（进入 SysCap 生态契约）→ Deprecated（编译警告 + 迁移期）→ Removed。每一态迁移都需通过闭包完整性检查。
- **优点**：裁剪可计算；测试矩阵按能力组合约束收敛；生态承诺与实现同源。
- **缺点**：前期元模型建设成本高；对既有代码需要大规模“能力考古”；粒度过细会引入元数据爆炸。
- **适用范围**：多产品形态（≥3 种资源等级）的 OS/中间件产品线；不适用于单产品应用或纯云服务。

## 11. Product Profile Driven Architecture（产品画像驱动架构，PPDA）

- **定义**：产品不再通过散落的 feature 布尔值定义，而通过一份**Product Profile**（形式化文档）定义：能力需求集 + 资源预算 + 合规约束 + 硬件条件。
- **原理**：Profile 是求解器的输入：`Solver(Profile, Capability Graph) → 合法组件闭包 + 预算报告 + 冲突诊断`。产品团队从“翻 GN 找开关”变为“声明我要什么”。
- **生命周期**：Profile 随产品版本演进，可 diff、可继承（Watch Profile extends Wearable Base Profile）。
- **优缺点**：产品成本回归线性、非法组合在构建前被拒；代价是需要维护求解器与 Profile 语言，且团队心智模型转换成本高。
- **适用范围**：产品数 ≥3 且形态差异大（Phone/Watch/TV/Car/IoT）的产品线。

## 12. Architecture Meta Model（架构元模型）

统一元模型为六层映射链（本理论核心贡献）：

**Product Profile → Capability Graph → Component Graph → Service Graph → Runtime Graph → Binary/Memory Layout**

每层是上一层的求解产物：能力图确定“要什么”，组件图确定“哪些代码”，服务图确定“哪些进程/SA 及 IPC 拓扑”，运行时图确定“启动序与按需拉起策略”，二进制/内存布局给出可验证的预算账单。**架构正确性 = 六层映射的可计算性与一致性**。

---

# Part III　Validation：验证

## 13. OpenHarmony 验证（历史验证）

OpenHarmony 自身演化印证理论：base/ext 二分是对“可减性公理”的朴素实现；ext 的 7 个可裁 SA 服务证明能力级裁剪可行；但 base↔ext 依赖回环、feature 无组合约束、rom/ram 字段无强制校验，证明缺少闭包公理与预算公理会导致裁剪退化为人工工程——与理论预测一致。

## 14. Android 验证

Android 早期（Treble 前）Vendor 修改成本失控，正是 Dependency Cost 模型的实例；Project Treble/Mainline 本质是引入接口冻结与模块闭包，即向“闭包公理”靠拢，其成功（升级速度提升）反向验证理论。Android Go 的困境（GMS 下限）验证“能力不可减则资源下限不可破”。

## 15. Fuchsia 验证

Fuchsia 的 Component Framework + Capability Routing 是 CNA 在运行期维度的最完整实现（能力显式路由、默认拒绝）；其 Product Assembly 即 PPDA 的工程原型。Fuchsia 的存在证明理论可实现；其商业落地缓慢提示 CNA 的“前期成本高、生态迁移难”缺点真实存在。

## 16. AUTOSAR 验证

AUTOSAR（Classic/Adaptive）用 ARXML 形式化配置实现了“Profile→Binary”的可计算映射，在汽车行业运行二十年，验证了公理 2、3 的工程可行性；其代价（工具链沉重、演进僵化）验证了元模型过重的风险边界。

## 17. Zephyr 验证

Zephyr 的 Kconfig 依赖表达式（`depends on` / `select`）就是能力图的编译期形式，证明 feature 组合约束可以机器化；DeviceTree 即 Conditional Capability 的硬件条件建模。Zephyr 能覆盖 8KB RAM 设备，验证“可减性公理彻底执行则资源下限极低”。

## 18. Counter Example Analysis（反例验证）

- **Symbian**：能力过度形式化（Capability 安全模型繁琐）+ 构建变体失控 → 开发者逃离。教训：元模型复杂度本身是成本，验证 CNA 的“粒度过细”缺点。
- **Windows CE/Mobile 产品线**：组件化（Platform Builder）做到了裁剪，但能力与生态 API 脱节，应用兼容性碎裂。教训：能力必须同时是**生态契约**，只做构建裁剪不够。
- **单体大内核直接改小（早期 Android Wear）**：违反可减性公理的裁剪，长期维护成本证明“fork 式产品线”不可持续。

## 19. Trade-off Analysis（权衡分析）

- 静态裁剪（ROM 最优、变体验证成本高）vs 动态加载（验证收敛、RAM/启动开销）：**理论上应由 Profile 的资源预算决定组合时机，而非全局统一**。
- 能力粒度：粗（裁剪不净）vs 细（元数据爆炸）——建议以“可独立测试的最小契约”为粒度判据。
- IPC 隔离 vs 直调合并：资源受限产品应支持**同一服务图在小设备上折叠为单进程**（Service Graph 与 Runtime Graph 解耦的价值所在）。
- 理论鲁棒性：六层元模型对未来形态（XR：高实时渲染旁路；Robot：确定性调度域；AI Device：NPU 内存预算）均可通过在 Profile 增加约束维度扩展，无需修改核心公理——满足开放封闭性。**预测验证**：XR 设备将迫使“数据面内核下沉”模式（本案例中的 eBPF 先例）成为能力元模型的标准字段。
- **最终评价**：理论在 7 个系统、3 个反例上一致成立；主要风险是工程落地的元模型维护成本，需以增量方式引入。

---

# Part IV　Evolution：演进

## 20. Next Generation Architecture（2035 年通信架构重设计）

- **Capability Native**：每个通信能力（Connectivity.Core、Connectivity.Sharing、Connectivity.Vpn、Stack.Http3…）拥有独立 manifest（提供/依赖/资源/生命周期），取代分散的 syscap+feature+GN 三元。
- **Composable Runtime**：Service Graph 按 Profile 折叠——手机上 11 个 SA 独立进程；手表上折叠为 2 个进程域；IoT 上折叠为单镜像静态链接，IPC 自动降级为函数调用（接口生成器同时产出 Proxy 与 inline 绑定）。
- **Adaptive Component Architecture**：组件在部署时选择实现变体（全功能 DNS vs 微型解析器），由能力求解器按预算选型。
- **Architecture Meta Model**：Profile→Capability→Component→Service→Runtime→Binary→Memory 全链机器可验证，CI 中每个 Profile 自动出预算账单与合法性证明。

## 21. Ten-Year Roadmap（3~10 年路线）

| 阶段 | 时间 | 目标 | 收益 | 风险 | 迁移策略 |
|---|---|---|---|---|---|
| P1 治理期 | 0–2 年 | feature 台账化、组合约束建模（Kconfig 式）、打破 base↔ext 回环、rom/ram 预算 CI 强制化 | 测试矩阵收敛、裁剪可预测 | 团队惯性 | 只加约束不改代码，绞杀者模式 |
| P2 能力化 | 2–4 年 | 能力 manifest 落地，SysCap/feature/GN 收敛为单源；ext 按能力拆分为独立能力包 | 能力级独立发布与测试 | 元数据债务 | 新能力必须 CNA、存量按热度迁移 |
| P3 画像化 | 4–6 年 | Product Profile 语言 + 求解器；Watch/IoT Profile 一键出闭包 | 产品成本线性化 | 求解器正确性 | 与人工配置双轨对账一年 |
| P4 可组合运行时 | 6–8 年 | 服务图折叠、IPC↔直调自动切换、按需拉起全面化 | RAM/启动时间大幅下降 | ABI 兼容 | 接口生成器先行 |
| P5 自适应期 | 8–10 年 | 实现变体自动选型、跨形态（XR/Robot/AI）Profile 扩展 | 新形态接入周期从年降到月 | 过度自动化 | 保留人工否决权 |

## 22. Architecture Principles（架构原则，32 条）

**能力原则**

1. 能力是架构第一公民，组件是能力的投影。
2. 每个能力必须可被完整移除且系统仍可构建。
3. 能力对外承诺（SysCap）一经 Stable 不得静默撤销。
4. 能力必须有显式生命周期：实验→稳定→废弃→移除。
5. 能力粒度以“可独立测试的最小契约”为准。
6. 能力间依赖必须显式声明，禁止隐式运行时发现依赖。
7. Conditional 能力必须声明其硬件/子系统前置条件。

**依赖原则**

8. 部件依赖图必须无环（base↔ext 类回环为架构缺陷）。
9. 依赖出度超过阈值（建议 15）必须触发架构评审。
10. 依赖只能指向更稳定的方向（稳定依赖原则）。
11. 可选能力对核心的依赖允许，核心对可选能力的依赖禁止。
12. 每条跨部件依赖必须可回答“若被裁掉会怎样”。

**变体原则**

13. Feature 开关必须有 owner、到期日与组合约束声明。
14. 调试/覆盖率开关不得与产品 feature 混用同一命名空间。
15. 变体组合合法性必须机器可判定，非法组合构建前拒绝。
16. 默认值哲学统一：核心 opt-out、扩展 opt-in。
17. 每个 feature 必须能映射到至少一个能力，否则删除。

**资源原则**

18. ROM/RAM/启动时间预算是构建系统一等输入并在 CI 强制校验。
19. 每个能力 manifest 必须声明资源足迹并可实测对账。
20. 数据面操作禁止逐包 IPC，必须可下沉（eBPF/内核路径）。
21. 组合时机（静态链接/动态加载/进程隔离）由 Profile 决定而非全局统一。
22. 小设备优先折叠进程域，隔离等级是可配置属性。

**产品线原则**

23. 产品用 Profile 声明式定义，禁止散落条件编译定义产品。
24. Profile 可继承、可 diff、可版本化。
25. 新产品形态接入不得修改核心代码，只允许新增能力与 Profile。
26. 每个 Profile 必须有自己的 CI 流水线与预算账单。

**演化原则**

27. 接口先冻结，实现后重构（Treble 定律）。
28. 架构迁移采用绞杀者模式，禁止大爆炸重写。
29. 元模型本身要有复杂度预算，防止 Symbian 式过度形式化。
30. 一切架构规则必须可被工具检查，不可检查的规则视为不存在。
31. 服务图与运行时图解耦：逻辑拓扑不变，物理部署可变。
32. 强调 Why 的架构决策记录（ADR）与能力 manifest 同库存放。

## 23. Engineering Migration（工程迁移）

1. **考古**：自动扫描现有 36 个 feature 与 50+ 依赖，生成现状能力图（工具化，非人工）。
2. **止血**：CI 加三条门禁——新 feature 必须带约束声明、新依赖必须过出度检查、rom/ram 超预算即失败。
3. **试点**：选 `mdnsmanager`（依赖最少、边界最清晰）做第一个 CNA 能力包，验证 manifest→构建→测试闭包全链。
4. **双轨**：Profile 求解结果与人工 GN 配置对账运行两个版本周期后切换权威源。
5. **回收**：每季度删除过期 feature，依赖回环限期归零。

## 24. Future Research（未来研究）

- 能力图的形式化验证（SAT/SMT 求解合法变体空间的复杂度边界）；
- 资源足迹的可组合性度量（能力 A+B 的实际 RAM ≠ RAM(A)+RAM(B)，交互项建模）；
- IPC↔直调自动折叠的安全等价性证明；
- 架构经济学模型的实证标定（用 OpenHarmony 历史 commit 数据拟合 Feature 交互成本系数）；
- LLM 辅助的“能力考古”：从存量代码自动推导能力边界与 manifest；
- 跨 OS 的 Capability 互操作标准（OpenHarmony SysCap ↔ Fuchsia Capability ↔ AUTOSAR Service 的映射理论）。

---

## 结语

OpenHarmony 通信子系统展示了一个**处于“Feature 产品线”向“Capability 产品线”跃迁中点**的真实系统：它已经把能力写进契约（SysCap）、把预算写进元数据（rom/ram）、把变体写进构建（GN feature），但三者尚未统一为可计算的元模型，因而裁剪仍是工程考古而非求解过程。本报告提出的资源受限产品线架构理论——三公理、六层元模型、CNA/PPDA 两个架构风格、复杂度第一定律与 32 条原则——在七个工业系统与三个反例上得到一致验证，其普适性不依赖 OpenHarmony 本身：**任何需要用一套代码覆盖多个资源等级产品的软件产品线，其架构质量最终取决于“从产品画像到内存布局”这条映射链的可计算程度。**

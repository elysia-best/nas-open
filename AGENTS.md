以下是专为 **AI Agent（如 Cursor/Devin/Codex 等代码生成与编排模型）** 设计的 `Task Arrangement`。整体采用 **分阶段、强契约、可并行、可测试** 的工程结构，适配 Debian 底层、C++ 核心、vcpkg 依赖管理、前后端分离架构。

---
# 🧠 AI Agent Task Arrangement: C++ NAS Management System

## 📐 0. 全局架构与技术栈约定
| 模块 | 技术选型 | 通信/接口 | 权限模型 |
|------|----------|-----------|----------|
| 核心守护进程 | C++17/20 + `nasd` 单进程插件架构 | gRPC (内部/CLI) + REST (WebUI) | `polkit` + `sudo` 提权网关 |
| 依赖管理 | `vcpkg` (manifest mode) + CMake | `vcpkg.json` 集中声明 | 自定义 overlay ports 处理底层 C 库 |
| Web UI | Node.js + Yarn + React/Vue + Tailwind | REST/gRPC Gateway | JWT Session + RBAC |
| CLI | `CLI11` / `fmt` / `linenoise` | 直连本地 gRPC socket | 继承调用者权限 |
| OS 定制 | Debian 12 Bookworm + `live-build` + `calamares` | Preseed + Post-install script | 最小化 root 暴露 |

> ⚠️ **AI 执行约束**：所有直接调用内核/硬件的模块必须提供 **OSAL（OS Abstraction Layer）** 接口，默认实现对接 `sysfs/procfs/libudev/libparted` 等，测试时必须注入 Mock。

---

## 🗂️ 1. 阶段任务清单 (Phase 0 → Phase 6)

### 🔹 Phase 0: 项目骨架与架构契约
| 任务ID | 描述 | 输入 | 输出 | 依赖 |
|--------|------|------|------|------|
| P0.1 | 初始化 CMake + vcpkg manifest 项目结构 | 需求文档 | `CMakeLists.txt`, `vcpkg.json`, `src/` 目录树 | 无 |
| P0.2 | 定义核心 IPC 契约 (protobuf + OpenAPI) | 功能清单 | `proto/nas.proto`, `openapi/api.yaml` | P0.1 |
| P0.3 | 搭建基础服务框架 (日志/配置/事件总线/权限网关) | 架构设计 | `src/core/` 完整编译通过 | P0.2 |

**🤖 AI 执行提示**：
```text
使用 manifest mode 生成 vcpkg.json，声明基础依赖：fmt, spdlog, nlohmann-json, protobuf, grpc, cli11, libuv, sqlite3, boost。
生成 CMake 跨平台构建脚本，启用 FetchContent 或 vcpkg 工具链。
编写 gRPC service 定义，覆盖所有核心模块的 CRUD/Action 接口。
```

---

### 🔹 Phase 1: 存储核心模块 (Storage Core)
任务ID
描述
关键依赖
验收点
P1.1
磁盘分区管理器
libparted / gdisk
支持 GPT/MBR，LVM 基础操作，返回分区拓扑 JSON
P1.2
ZFS 存储池创建与管理
libzfs2-dev (Debian), libblkid
支持多磁盘 VDEV 拓扑构建 (stripe/mirror/raidz), ashraid/压缩/去重/加密参数配置, 健康状态检测, 返回池拓扑 JSON
P1.3
SCSI Target 核心
libtcmu 或 configfs 绑定
支持 iSCSI/FC 基础导出，LUN 映射，会话监控
P1.4
ZFS Boot Environment 管理器
libzfs2-dev
创建/回滚/列表/克隆 BE，与 zfsbootmenu 兼容
P1.5
NFSv4 ACL 工具
libnfsidmap, libacl
POSIX/NFSv4 ACL 转换，继承策略，权限校验
P1.6
NFS 服务核心组件集
nfs-utils 集成
动态 /etc/exports 生成，服务状态监控，ID 映射
P1.7
高性能定制版 Samba 管理核心
libsmbclient + VFS 配置器
动态生成 smb.conf，共享/权限/审计管理，热重载
P1.8
ZFS 本地/远程快照与复制引擎
zfs send/recv 包装器
增量/全量调度，带宽限制，断点续传，远程 SSH 隧道

**🤖 AI 执行提示**：
```text
每个存储模块独立编译为静态库 (.a)，通过 plugin 注册到 nasd。
所有底层命令调用必须封装为 `exec_with_timeout()` + `stdout/stderr` 解析器。
提供 `mock_os_calls.h` 用于 CI 测试，禁止在单元测试中直接调用 `zfs`/`mkfs`。
```

---

### 🔹 Phase 2: 网络、工具与监控 (Networking & Tools)
| 任务ID | 描述 | 关键依赖 | 验收点 |
|--------|------|----------|--------|
| P2.1 | 云存储同步引擎 | `libcurl`, `rclone` CLI/API | 支持 S3/OneDrive/Backblaze，增量同步，冲突策略，任务队列 |
| P2.2 | S.M.A.R.T. 健康监控 | `smartctl` in`smartmontools` | 周期性读取，阈值告警，预测失效，邮件/Webhook 推送 |
| P2.3 | 系统诊断报告生成 | `tar`, `jq`, `systemd` 日志 | 一键打包：dmesg, journalctl, zpool status, smart, config, 生成 HTML/PDF |
| P2.4 | 硬件信息管理 | `libudev`, `dmidecode`, `lshw` | 拓扑发现 (CPU/内存/NIC/DISK/PCIe)，温度/功耗采集，资产报表 |

**🤖 AI 执行提示**：
```text
监控模块采用 epoll/inotify/eventfd 实现低开销轮询。
云同步模块需实现幂等重试与本地状态缓存（SQLite）。
硬件检测需兼容虚拟化环境（跳过不存在的传感器，返回 null 而非 crash）。
```

---

### 🔹 Phase 3: CLI 控制台 (TrueNAS CLI 风格)
| 任务ID | 描述 | 关键依赖 | 验收点 |
|--------|------|----------|--------|
| P3.1 | 交互式命令行框架 | `CLI11`, `linenoise`/`readline` | 支持 `nas-cli >`, Tab 补全, `help`, `show`, `set`, `task` |
| P3.2 | 命令路由与 API 映射 | gRPC Client | 所有 Phase1/2 功能可通过 CLI 调用，支持 JSON/YAML 输出 |
| P3.3 | 权限与会话管理 | `polkit`, `sudo` | 非 root 用户提权确认，审计日志，命令历史加密存储 |

**🤖 AI 执行提示**：
```text
CLI 必须完全无状态，仅作为 gRPC 客户端。
实现 `--dry-run` 和 `--output=json|table|yaml` 选项。
命令结构遵循：`<namespace> <entity> <action> [params]` (例: `zfs snapshot create pool/dataset@snap1`)
```

---

### 🔹 Phase 4: Web UI 与 Node.js 前端
| 任务ID | 描述 | 关键依赖 | 验收点 |
|--------|------|----------|--------|
| P4.1 | 前端工程脚手架 | Node.js, Yarn, Vite/Next.js, Tailwind | 基础路由，组件库，暗黑/亮色主题，响应式布局 |
| P4.2 | API Gateway & Auth | `axios`, JWT, RBAC | 登录/登出，Token 刷新，权限拦截，WebSocket 状态推送 |
| P4.3 | 核心功能页集成 | React/Vue 组件 | 存储池/共享/网络/监控/诊断/用户 管理页面，与 C++ 后端联调通过 |

**🤖 AI 执行提示**：
```text
UI 不直接调用系统 API，必须通过 C++ 后端 REST/gRPC Gateway。
所有表单提交需包含 `csrf` 或 `jwt` 校验。
使用 `storybook` 管理组件，确保可复用性。
```

---

### 🔹 Phase 5: ISO 引导与系统安装向导
| 任务ID | 描述 | 关键依赖 | 验收点 |
|--------|------|----------|--------|
| P5.1 | Debian Live 定制 | `live-build`, `calamares` | 可启动 ISO，包含 `nasd` + 依赖 + 基础工具链 |
| P5.2 | 安装向导流程 | `calamares` 模块开发 | 磁盘选择/分区/ZFS池创建/网络配置/用户创建 |
| P5.3 | 安装后自动化 | `post-install` 脚本 | 启用服务，生成初始配置，清理缓存，首次启动引导 |

**🤖 AI 执行提示**：
```text
使用 `debian-cd` 或 `live-build` 构建脚本，确保 vcpkg 编译的二进制可跨架构运行。
安装器必须支持 UEFI/Legacy，自动检测 ZFS 兼容内核模块。
提供 `--headless` 模式供自动化部署（PXE/云镜像）。
```

---

### 🔹 Phase 6: 集成测试、CI/CD 与文档
| 任务ID | 描述 | 工具 | 验收点 |
|--------|------|------|--------|
| P6.1 | 单元/集成测试 | `GoogleTest`, `Catch2` | 覆盖率 >70%，Mock 硬件调用，CI 阻塞失败 |
| P6.2 | CI/CD 流水线 | GitHub Actions, Docker, vcpkg cache | 自动构建 Debian 容器/ISO，静态分析，二进制签名 |
| P6.3 | 开发者文档 | `Doxygen`, `MkDocs`, `OpenAPI` | 架构说明/接口文档/部署指南/故障排查 |

**🤖 AI 执行提示**：
```text
CI 需缓存 `vcpkg_installed` 目录加速构建。
提供 `make dev` / `make test` / `make iso` 快捷目标。
文档必须包含 API 示例、权限矩阵、ZFS 最佳实践。
```

---

## 🛠️ 2. AI Agent 执行规范 (Execution Protocol)

| 维度 | 要求 |
|------|------|
| **模块化** | 每个功能必须独立编译为 `.a` 或 `.so`，通过 `nasd_plugin_register()` 动态加载 |
| **依赖隔离** | 所有第三方库仅通过 `vcpkg` 引入，禁止 `apt install -dev` 混用 |
| **安全边界** | 所有特权操作必须走 `polkit`，CLI/Web 默认无权限，需显式提权 |
| **错误处理** | 统一使用 `std::expected<T, Error>` 或自定义 `Result<T>`，禁止裸抛异常 |
| **测试策略** | 单元测试 Mock `sysfs`/`zfs`/`smartctl`；集成测试使用 QEMU + Debian 容器 |
| **代码规范** | `clang-format` + `clang-tidy` + `cppcheck` 强制通过，禁止 C 风格字符串/裸指针 |

---

## 📦 3. vcpkg & CMake 配置指南 (AI 可直接复用)
```json
// vcpkg.json
{
  "name": "nas-manager",
  "version": "0.1.0",
  "dependencies": [
    "fmt", "spdlog", "nlohmann-json", "protobuf", "grpc",
    "cli11", "libuv", "sqlite3", "boost", "libcurl", "libatasmart",
    "libparted", "libudev"
  ],
  "builtin-baseline": "main"
}
```
```cmake
# CMakeLists.txt (片段)
cmake_minimum_required(VERSION 3.25)
project(nas-manager LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
add_subdirectory(src/core)
add_subdirectory(src/storage)
add_subdirectory(src/network)
add_subdirectory(src/cli)
add_subdirectory(src/ui-gateway)
enable_testing()
```

---

## ✅ 4. 验收标准与测试策略
| 层级 | 验证方式 | 通过条件 |
|------|----------|----------|
| 单元 | `ctest` + Mock | 覆盖率 ≥70%，无内存泄漏 (Valgrind) |
| 集成 | QEMU + Debian 虚拟机 | `nasd` 启动正常，gRPC/REST 响应 <50ms |
| 存储 | `zpool create` + `smbclient` + `mount -t nfs4` | 数据读写正常，ACL 继承正确，快照可恢复 |
| UI | Playwright 自动化 | 登录/创建共享/查看监控/下载诊断报告全流程通过 |
| 安装 | ISO 在 UEFI/Legacy VM 引导 | 自动分区/ZFS 初始化/服务自启/首次登录成功 |

注意！！！！ 每部分都需要进行编译测试和测试用例编写，同时更新cli调用

---

## ⚠️ 5. 关键风险与边界说明
1. **ZFS 许可协议**：Debian 默认不含 ZFS 内核模块，需通过 `zfs-dkms` 或定制内核解决。AI 需在安装脚本中自动处理 DKMS 构建。
2. **Samba 定制**：不建议直接修改 Samba 源码。推荐通过动态生成 `smb.conf` + `vfs` 插件 + 热重载 `systemctl reload smbd` 实现“定制版”效果。
3. **vcpkg C 库兼容**：部分底层库（如 `libzfs`）在 vcpkg 无官方 port，需编写 `portfile.cmake` overlay 或回退到 `apt` 仅用于 CI 构建。
4. **实时性要求**：NAS 管理非实时系统，所有 I/O 密集型操作必须异步化（`asio`/`libuv`），避免阻塞 `nasd` 主事件循环。


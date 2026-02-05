# Watch Project - 嵌入式学习与作业提交

![alt text](https://img.shields.io/badge/Focus-Embedded_System-blue)
![alt text](https://img.shields.io/badge/Language-C/C++-orange)
![alt text](https://img.shields.io/badge/Tool-Keil_MDK-lightgrey)

## 📌 项目简介

本仓库用于记录我的 **嵌入式开发学习历程** 以及 **作业代码提交**。
主要涵盖了基于 STM32/单片机 的各项实验、驱动开发以及逻辑功能的实现。

- **目标**：保持代码整洁、实现版本控制、方便在不同设备间同步。
- **内容**：包含 Keil 工程文件、源代码（.c/.h）、硬件电路图（如有）以及实验笔记。

---

## 🚀 交互式 Git 管理脚本 ([GitManager.ps1](GitManager.ps1))

为了适配 **Windows 终端 1.23** 的特性并简化 Gitee 远程仓库的管理，项目内置了深度定制的 PowerShell 交互脚本。

### ✨ 主要功能

1. **终端深度适配**：专为 Windows Terminal 1.23 + PowerShell 5.1 优化，完美解决中文路径乱码、颜色渲染失效及目录列表对齐问题。
2. **沉浸式文件管理**：采用类资源管理器交互，支持输入 `cd 数字` 进入文件夹、输入 `0` 返回上一级，彻底告别复杂的路径命令。
3. **精准提交控制**：支持通过 **数字索引** 多选文件（而非粗暴的 `git add .`），输入 `done` 即可一键完成 Add -> Commit -> Push 流程。
4. **分支安全管控**：绿色高亮当前分支，自动校验保护分支（master/main）删除权限，支持快速切换或创建新分支。
5. **智能安全退出**：输入 **`Q`** 即可触发安全退出机制，脚本自动执行 `git reset` 清理暂存区残留，防止误操作污染。

### 🛠️ 如何运行

1. **保存脚本**：
   - 确保脚本名为 `GitManager.ps1`。
   - 建议放置在 `E:\Git_Repository\Watch_project\` 或您的仓库根目录下。

2. **启动方式**：
   - 打开 **Windows Terminal**，进入仓库目录。
   - 执行命令：`.\GitManager.ps1`

3. **操作指令速查**：
   - **浏览与选择**：
     - `cd [数字]`：进入对应序号的文件夹（如 `cd 1`）。
     - `[数字]`：选中/取消选中该文件（标记为待提交）。
     - `0`：返回上一级目录。
   - **流程控制**：
     - `done`：提交已选文件并推送到 Gitee。
     - `B`：返回分支选择界面 (Back)。
     - `Q`：**安全退出脚本** (Quit)。

------



## 📦 如何在其他电脑上运行本项目

为了确保从本仓库下载的代码能够直接在 Keil 中编译运行，请遵循以下说明：

### 1. 过滤规则 (.gitignore)

本仓库已配置 .gitignore，**只上传源码和工程结构**，不会上传编译生成的临时文件（如 Objects/, Listings/）。

> **这样做的好处**：仓库体积小，且避免了因本地路径不一致导致的编译报错。

### 2. 运行步骤

1. 从 Gitee 下载 ZIP 包或使用 git clone 到本地。
2. 进入对应的实验文件夹，找到以 .uvprojx 结尾的项目文件。
3. **双击打开 Keil MDK**。
4. **关键步骤**：点击 Keil 工具栏的 **Rebuild All** (全部重建) 按钮 🛠️。
   - Keil 会根据你的本地环境重新生成编译文件。
5. 点击 **Download** 即可烧录到开发板。

------



## 📂 目录结构说明

```Text
ProjectTable (项目根目录)
├── 00_ProjectManagement (项目管理) [1, 2]
│   ├── 00_需求导入——QFD [1, 2]
│   ├── 01_需求约束——Pugh [1, 2]
│   ├── 02_需求转化_BasicStatics [1, 2]
│   ├── 03_功能图谱_FunctionMap (说明：项目所需实现的功能，可由系统需求分解成软硬件需求) [2]
│   ├── 04_法规认证_国内国外 [1, 2]
│   ├── 04_功能风险管控_DFMEA [1, 2]
│   ├── 04_知识产权_国内_国外 [1, 2]
│   ├── 05_敏捷开发_Scrum [1, 2]
│   ├── 06_持续集成与测试_DevOps [1, 2]
│   ├── 07_产品生产管理_SixSigma [1, 2]
│   └── 08_缺陷管理追踪_Jira [1, 2]
│
├── 00_Reference (参考文档) [1, 2]
│   ├── 01_DataSheet (说明：用于存放项目过程中所用到的芯片手册) [2]
│   └── 02_Doc (说明：用于存放项目过程中所用到的资料) [2]
│
├── 01_FunctionMap (功能需求清单) [1, 2]
│   ├── 01_HarwareFunction (说明：项目需要实现的硬件功能) [2]
│   └── 02_SoftwareFunction (说明：项目需要实现的软件功能) [2]
│
├── 02_Hardware (硬件相关) [1, 2]
│   ├── 01_project (说明：工程文档) [2]
│   ├── 02_Sch (说明：原理图) [2]
│   ├── 03_Pcb (说明：Pcb文件) [2]
│   └── 04_Hardware_Software_Interface (说明：软硬件接口文件) [2]
│
├── 03_Firmware (固件/版本文件) [1, 2]
│   ├── 01_Bootloader [2]
│   └── 02_APP [2]
│
├── 04_Software (软件代码) [1, 2]
│   ├── 01_PC_Software (上位机/电脑端软件) [2]
│   └── 02_Mobile_Software (移动端软件) [2]
│
├── 05_Mechanical (机械结构) [1, 2]
│   ├── 01_standard (说明：标准零部件) [2]
│   └── 02_customization (说明：定制化零部件) [2]
│
├── 06_Factory (工厂测试) [2]
│   └── (说明：用于工厂下线的测试版软件) [2]
│
├── 07_Tools (工具链) [1, 2]
│   └── (说明：放置项目开发过程中所用到的工具，例如：自动化构建脚本、持续集成工具等) [1, 2]
│
└── README.md (说明文档) [1, 2]
```

------



**Author:** [zhang___qi](https://gitee.com/zhang___qi/projects)
**Status:** In Progress... 🚀